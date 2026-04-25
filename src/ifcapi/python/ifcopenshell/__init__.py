# SPDX-License-Identifier: LGPL-3.0-or-later

"""
Minimal ifcopenshell package backed by the native ifcapi C library.

This package provides ``file`` and ``entity_instance`` classes compatible with
the standard ifcopenshell API, using ctypes to call libifcopenshell_capi directly (no SWIG).
"""

from __future__ import annotations

import ctypes
import ctypes.util
import builtins
import json
import os
import re
import tempfile
import weakref
import zipfile
from pathlib import Path
from typing import List, Optional, Set, Tuple

version = "0.8.1"


def get_log() -> str:
    """Return the accumulated parser/validator log (parity with SWIG)."""
    from ifcopenshell import ifcopenshell_wrapper as _W
    return _W.get_log()


def convert_path_to_rocksdb(ifcspf_path, rocksdb_path) -> None:
    """Convert an IFC-SPF file on disk to IfcOpenShell's RocksDB encoding."""
    from ifcopenshell.geom import serializers

    serializer = serializers.rocksdb_streaming(str(ifcspf_path), str(rocksdb_path), True)
    serializer.finalize()


class Error(Exception):
    """Error used when a generic problem occurs"""

    pass


class SchemaError(Error):
    """Error used when an IFC schema related problem occurs"""

    pass


class UndoSystemError(Exception):
    def __init__(self, message: str, transaction: "Transaction"):
        super().__init__(message)
        self.transaction = transaction


class Transaction:
    """Records create/edit/delete operations and replays them for undo/redo.

    Mirrors the upstream ``ifcopenshell.file.Transaction`` semantics so that
    file-level ``begin_transaction``/``end_transaction``/``undo``/``redo``
    behave identically to the SWIG-backed implementation.
    """

    def __init__(self, ifc_file: "file"):
        self.file = ifc_file
        self.operations: list = []
        self.is_batched = False
        self.batch_delete_index = 0
        self.batch_delete_ids: set = set()
        self.batch_inverses: list = []

    def serialise_entity_instance(self, element) -> dict:
        info = element.get_info()
        for key, value in info.items():
            info[key] = self.serialise_value(element, value)
        return info

    def serialise_value(self, element, value):
        return entity_instance.walk(
            lambda v: isinstance(v, entity_instance),
            lambda v: {"id": v.id()} if v.id() else {"type": v.is_a(), "value": v.wrappedValue},
            value,
        )

    def unserialise_value(self, element, value):
        return entity_instance.walk(
            lambda v: isinstance(v, dict),
            lambda v: self.file.by_id(v["id"]) if v.get("id") else self.file.create_entity(v["type"], v["value"]),
            value,
        )

    def batch(self) -> None:
        self.is_batched = True
        self.batch_delete_index = len(self.operations)
        self.batch_delete_ids = set()
        self.batch_inverses = []

    def unbatch(self) -> None:
        for inverses in self.batch_inverses:
            if inverses:
                self.operations.insert(self.batch_delete_index, {"action": "batch_delete", "inverses": inverses})
        self.is_batched = False
        self.batch_delete_index = 0
        self.batch_delete_ids = set()
        self.batch_inverses = []

    def store_create(self, element) -> None:
        if element.id():
            self.operations.append({"action": "create", "value": self.serialise_entity_instance(element)})

    def store_edit(self, element, index: int, value) -> None:
        if element.id():
            self.operations.append(
                {
                    "action": "edit",
                    "id": element.id(),
                    "index": index,
                    "old": self.serialise_value(element, element[index]),
                    "new": self.serialise_value(element, value),
                }
            )

    def store_delete(self, element) -> None:
        inverses: dict = {}
        if self.is_batched:
            if element.id() not in self.batch_delete_ids:
                self.batch_inverses.append(self.get_element_inverses(element))
            self.batch_delete_ids.add(element.id())
        else:
            inverses = self.get_element_inverses(element)
        self.operations.append(
            {"action": "delete", "inverses": inverses, "value": self.serialise_entity_instance(element)}
        )

    def get_element_inverses(self, element) -> dict:
        inverses: dict = {}
        for inverse in self.file.get_inverse(element):
            inverse_references: list = []
            for i, attribute in enumerate(inverse):
                if self.has_element_reference(attribute, element):
                    inverse_references.append((i, self.serialise_value(inverse, attribute)))
            inverses[inverse.id()] = inverse_references
        return inverses

    def has_element_reference(self, value, element) -> bool:
        if isinstance(value, (tuple, list)):
            for v in value:
                if self.has_element_reference(v, element):
                    return True
            return False
        return value == element

    def rollback(self) -> None:
        for operation in self.operations[::-1]:
            if operation["action"] == "create":
                element = self.file.by_id(operation["value"]["id"])
                if hasattr(element, "GlobalId") and element.GlobalId is None:
                    # Hack: the validator forbids removal when GlobalId is null,
                    # so set a placeholder before removing.
                    element.GlobalId = "x"
                self.file.remove(element)
            elif operation["action"] == "edit":
                element = self.file.by_id(operation["id"])
                try:
                    element[operation["index"]] = self.unserialise_value(element, operation["old"])
                except Exception:
                    # Tolerate values the schema accepts on parse but rejects on edit.
                    pass
            elif operation["action"] == "delete":
                e = self.file.create_entity(operation["value"]["type"], id=operation["value"]["id"])
                for k, v in operation["value"].items():
                    if k in ("id", "type"):
                        continue
                    try:
                        setattr(e, k, self.unserialise_value(e, v))
                    except Exception:
                        pass
                for inverse_id, data in operation["inverses"].items():
                    inverse = self.file.by_id(inverse_id)
                    for index, value in data:
                        inverse[index] = self.unserialise_value(inverse, value)
            elif operation["action"] == "batch_delete":
                for inverse_id, data in operation["inverses"].items():
                    inverse = self.file.by_id(inverse_id)
                    for index, value in data:
                        inverse[index] = self.unserialise_value(inverse, value)

    def commit(self) -> None:
        for operation in self.operations:
            if operation["action"] == "create":
                e = self.file.create_entity(operation["value"]["type"], id=operation["value"]["id"])
                for k, v in operation["value"].items():
                    if k in ("id", "type"):
                        continue
                    try:
                        setattr(e, k, self.unserialise_value(e, v))
                    except Exception:
                        pass
            elif operation["action"] == "edit":
                element = self.file.by_id(operation["id"])
                element[operation["index"]] = self.unserialise_value(element, operation["new"])
            elif operation["action"] == "delete":
                element = self.file.by_id(operation["value"]["id"])
                self.file.remove(element)
            elif operation["action"] == "batch_delete":
                pass


_lib = None


def _find_library():
    explicit = os.environ.get("IFCOPENSHELL_CAPI_LIB_PATH")
    if explicit and os.path.isfile(explicit):
        return explicit

    here = os.path.dirname(os.path.abspath(__file__))
    repo_root = os.path.abspath(os.path.join(here, "..", "..", "..", ".."))
    for build_dir in ("build-capi-stable", "build", "build-release", "build-debug"):
        for name in ("libifcopenshell_capi.dylib", "libifcopenshell_capi.so", "ifcopenshell_capi.dll"):
            path = os.path.join(repo_root, build_dir, "ifcwrap", name)
            if os.path.isfile(path):
                return path

    return ctypes.util.find_library("ifcopenshell_capi")


def _get_lib():
    global _lib
    if _lib is not None:
        return _lib

    path = _find_library()
    if not path:
        raise RuntimeError(
            "Cannot find libifcopenshell_capi shared library. "
            "Set IFCOPENSHELL_CAPI_LIB_PATH or build with -DBUILD_IFCCAPI=ON."
        )

    _lib = ctypes.CDLL(path)

    # -- Error handling -------------------------------------------------------
    _lib.ifcopenshell_last_error_message.restype = ctypes.c_char_p
    _lib.ifcopenshell_last_error_message.argtypes = []
    _lib.ifcopenshell_clear_error.restype = None
    _lib.ifcopenshell_clear_error.argtypes = []

    # -- GUID -----------------------------------------------------------------
    _lib.ifcopenshell_guid_new.restype = ctypes.c_char_p
    _lib.ifcopenshell_guid_new.argtypes = []
    _lib.ifcopenshell_free_string.restype = None
    _lib.ifcopenshell_free_string.argtypes = [ctypes.c_char_p]

    # -- File operations ------------------------------------------------------
    _lib.ifcopenshell_file_create.restype = ctypes.c_void_p
    _lib.ifcopenshell_file_create.argtypes = [ctypes.c_char_p]
    _lib.ifcopenshell_file_open.restype = ctypes.c_void_p
    _lib.ifcopenshell_file_open.argtypes = [ctypes.c_char_p]
    _lib.ifcopenshell_file_open_bypass.restype = ctypes.c_void_p
    _lib.ifcopenshell_file_open_bypass.argtypes = [
        ctypes.c_char_p,
        ctypes.POINTER(ctypes.c_char_p),
        ctypes.c_size_t,
    ]
    _lib.ifcopenshell_file_from_string.restype = ctypes.c_void_p
    _lib.ifcopenshell_file_from_string.argtypes = [ctypes.c_char_p, ctypes.c_int]
    _lib.ifcopenshell_file_free.restype = None
    _lib.ifcopenshell_file_free.argtypes = [ctypes.c_void_p]
    _lib.ifcopenshell_file_schema.restype = ctypes.c_char_p
    _lib.ifcopenshell_file_schema.argtypes = [ctypes.c_void_p]
    _lib.ifcopenshell_file_create_entity.restype = ctypes.c_void_p
    _lib.ifcopenshell_file_create_entity.argtypes = [ctypes.c_void_p, ctypes.c_char_p]
    _lib.ifcopenshell_file_create_entity_with_id.restype = ctypes.c_void_p
    _lib.ifcopenshell_file_create_entity_with_id.argtypes = [ctypes.c_void_p, ctypes.c_char_p, ctypes.c_uint32]
    _lib.ifcopenshell_file_add_entity.restype = ctypes.c_void_p
    _lib.ifcopenshell_file_add_entity.argtypes = [ctypes.c_void_p, ctypes.c_void_p, ctypes.c_uint32]
    _lib.ifcopenshell_file_get_max_id.restype = ctypes.c_uint32
    _lib.ifcopenshell_file_get_max_id.argtypes = [ctypes.c_void_p]
    _lib.ifcopenshell_file_by_type_count.restype = ctypes.c_int32
    _lib.ifcopenshell_file_by_type_count.argtypes = [ctypes.c_void_p, ctypes.c_char_p]
    _lib.ifcopenshell_file_by_type.restype = ctypes.c_int32
    _lib.ifcopenshell_file_by_type.argtypes = [ctypes.c_void_p, ctypes.c_char_p, ctypes.POINTER(ctypes.c_void_p)]
    _lib.ifcopenshell_file_remove.restype = None
    _lib.ifcopenshell_file_remove.argtypes = [ctypes.c_void_p]
    _lib.ifcopenshell_file_by_guid.restype = ctypes.c_void_p
    _lib.ifcopenshell_file_by_guid.argtypes = [ctypes.c_void_p, ctypes.c_char_p]
    _lib.ifcopenshell_file_entity_count.restype = ctypes.c_uint32
    _lib.ifcopenshell_file_entity_count.argtypes = [ctypes.c_void_p]
    _lib.ifcopenshell_file_entity_ids.restype = ctypes.POINTER(ctypes.c_void_p)
    _lib.ifcopenshell_file_entity_ids.argtypes = [ctypes.c_void_p, ctypes.POINTER(ctypes.c_uint32)]
    _lib.ifcopenshell_free_instance_array.restype = None
    _lib.ifcopenshell_free_instance_array.argtypes = [ctypes.POINTER(ctypes.c_void_p), ctypes.c_uint32]
    _lib.ifcopenshell_free_instance_array_only.restype = None
    _lib.ifcopenshell_free_instance_array_only.argtypes = [ctypes.POINTER(ctypes.c_void_p)]
    _lib.ifcopenshell_ifc_instance_destroy.restype = None
    _lib.ifcopenshell_ifc_instance_destroy.argtypes = [ctypes.c_void_p]
    _lib.ifcopenshell_ifc_file_by_id.restype = ctypes.c_bool
    _lib.ifcopenshell_ifc_file_by_id.argtypes = [ctypes.c_void_p, ctypes.c_int32, ctypes.POINTER(ctypes.c_void_p)]
    _lib.ifcopenshell_ifc_instance_id.restype = ctypes.c_bool
    _lib.ifcopenshell_ifc_instance_id.argtypes = [ctypes.c_void_p, ctypes.POINTER(ctypes.c_uint32)]
    _lib.ifcopenshell_ifc_instance_file.restype = ctypes.c_void_p
    _lib.ifcopenshell_ifc_instance_file.argtypes = [ctypes.c_void_p]
    _lib.ifcopenshell_file_get_inverse.restype = ctypes.POINTER(ctypes.c_void_p)
    _lib.ifcopenshell_file_get_inverse.argtypes = [ctypes.c_void_p, ctypes.POINTER(ctypes.c_uint32)]
    _lib.ifcopenshell_file_get_inverse_indices.restype = ctypes.POINTER(ctypes.c_int32)
    _lib.ifcopenshell_file_get_inverse_indices.argtypes = [ctypes.c_void_p, ctypes.POINTER(ctypes.c_uint32)]
    _lib.ifcopenshell_free_int32_array.restype = None
    _lib.ifcopenshell_free_int32_array.argtypes = [ctypes.POINTER(ctypes.c_int32)]
    _lib.ifcopenshell_file_traverse.restype = ctypes.POINTER(ctypes.c_void_p)
    _lib.ifcopenshell_file_traverse.argtypes = [ctypes.c_void_p, ctypes.c_int, ctypes.POINTER(ctypes.c_uint32)]
    _lib.ifcopenshell_file_write.restype = ctypes.c_bool
    _lib.ifcopenshell_file_write.argtypes = [ctypes.c_void_p, ctypes.c_char_p]
    _lib.ifcopenshell_file_to_string.restype = ctypes.c_char_p
    _lib.ifcopenshell_file_to_string.argtypes = [ctypes.c_void_p]
    _lib.ifcopenshell_file_header_file_description.restype = ctypes.c_void_p
    _lib.ifcopenshell_file_header_file_description.argtypes = [ctypes.c_void_p]
    _lib.ifcopenshell_file_header_file_name.restype = ctypes.c_void_p
    _lib.ifcopenshell_file_header_file_name.argtypes = [ctypes.c_void_p]
    _lib.ifcopenshell_file_header_file_schema.restype = ctypes.c_void_p
    _lib.ifcopenshell_file_header_file_schema.argtypes = [ctypes.c_void_p]
    _lib.ifcopenshell_ifc_file_storage_mode.restype = ctypes.c_bool
    _lib.ifcopenshell_ifc_file_storage_mode.argtypes = [ctypes.c_void_p, ctypes.POINTER(ctypes.c_int32)]
    _lib.ifcopenshell_ifc_file_key_value_store_query.restype = ctypes.c_bool
    _lib.ifcopenshell_ifc_file_key_value_store_query.argtypes = [
        ctypes.c_void_p,
        ctypes.c_char_p,
        ctypes.c_void_p,
    ]
    _lib.ifcopenshell_instance_streamer_create.restype = ctypes.c_void_p
    _lib.ifcopenshell_instance_streamer_create.argtypes = []
    _lib.ifcopenshell_instance_streamer_create_from_path.restype = ctypes.c_void_p
    _lib.ifcopenshell_instance_streamer_create_from_path.argtypes = [ctypes.c_char_p, ctypes.c_bool]
    _lib.ifcopenshell_ifcparse_stream_from_string.restype = ctypes.c_bool
    _lib.ifcopenshell_ifcparse_stream_from_string.argtypes = [ctypes.c_char_p, ctypes.POINTER(ctypes.c_void_p)]
    _lib.ifcopenshell_ifc_instance_streamer_destroy.restype = None
    _lib.ifcopenshell_ifc_instance_streamer_destroy.argtypes = [ctypes.c_void_p]
    _lib.ifcopenshell_ifc_instance_streamer_has_semicolon.restype = ctypes.c_bool
    _lib.ifcopenshell_ifc_instance_streamer_has_semicolon.argtypes = [ctypes.c_void_p]
    _lib.ifcopenshell_ifc_instance_streamer_push_page.restype = ctypes.c_bool
    _lib.ifcopenshell_ifc_instance_streamer_push_page.argtypes = [ctypes.c_void_p, ctypes.c_char_p]
    _lib.ifcopenshell_ifc_instance_streamer_read_instance_py.restype = ctypes.c_bool
    _lib.ifcopenshell_ifc_instance_streamer_read_instance_py.argtypes = [
        ctypes.c_void_p,
        ctypes.c_bool,
        ctypes.c_void_p,
    ]

    # -- Entity operations ----------------------------------------------------
    _lib.ifcopenshell_entity_type.restype = ctypes.c_char_p
    _lib.ifcopenshell_entity_type.argtypes = [ctypes.c_void_p]
    _lib.ifcopenshell_entity_is_a.restype = ctypes.c_bool
    _lib.ifcopenshell_entity_is_a.argtypes = [ctypes.c_void_p, ctypes.c_char_p]
    _lib.ifcopenshell_entity_has_attr.restype = ctypes.c_bool
    _lib.ifcopenshell_entity_has_attr.argtypes = [ctypes.c_void_p, ctypes.c_char_p]
    _lib.ifcopenshell_entity_attr_type.restype = ctypes.c_int
    _lib.ifcopenshell_entity_attr_type.argtypes = [ctypes.c_void_p, ctypes.c_char_p]
    _lib.ifcopenshell_entity_attr_is_null.restype = ctypes.c_bool
    _lib.ifcopenshell_entity_attr_is_null.argtypes = [ctypes.c_void_p, ctypes.c_char_p]
    _lib.ifcopenshell_entity_attr_count.restype = ctypes.c_uint32
    _lib.ifcopenshell_entity_attr_count.argtypes = [ctypes.c_void_p]
    _lib.ifcopenshell_entity_attr_name.restype = ctypes.c_char_p
    _lib.ifcopenshell_entity_attr_name.argtypes = [ctypes.c_void_p, ctypes.c_uint32]
    _lib.ifcopenshell_entity_to_string.restype = ctypes.c_char_p
    _lib.ifcopenshell_entity_to_string.argtypes = [ctypes.c_void_p]
    _lib.ifcopenshell_entity_to_string_valid_spf.restype = ctypes.c_char_p
    _lib.ifcopenshell_entity_to_string_valid_spf.argtypes = [ctypes.c_void_p]

    # -- Scalar getters -------------------------------------------------------
    _lib.ifcopenshell_entity_get_string.restype = ctypes.c_char_p
    _lib.ifcopenshell_entity_get_string.argtypes = [ctypes.c_void_p, ctypes.c_char_p]
    _lib.ifcopenshell_entity_get_int.restype = ctypes.c_int64
    _lib.ifcopenshell_entity_get_int.argtypes = [ctypes.c_void_p, ctypes.c_char_p]
    _lib.ifcopenshell_entity_get_double.restype = ctypes.c_double
    _lib.ifcopenshell_entity_get_double.argtypes = [ctypes.c_void_p, ctypes.c_char_p]
    _lib.ifcopenshell_entity_get_bool.restype = ctypes.c_int
    _lib.ifcopenshell_entity_get_bool.argtypes = [ctypes.c_void_p, ctypes.c_char_p]
    _lib.ifcopenshell_entity_get_enum.restype = ctypes.c_char_p
    _lib.ifcopenshell_entity_get_enum.argtypes = [ctypes.c_void_p, ctypes.c_char_p]
    _lib.ifcopenshell_entity_get_reference.restype = ctypes.c_void_p
    _lib.ifcopenshell_entity_get_reference.argtypes = [ctypes.c_void_p, ctypes.c_char_p]

    # -- Scalar setters -------------------------------------------------------
    _lib.ifcopenshell_entity_set_string.restype = None
    _lib.ifcopenshell_entity_set_string.argtypes = [ctypes.c_void_p, ctypes.c_char_p, ctypes.c_char_p]
    _lib.ifcopenshell_entity_set_int.restype = None
    _lib.ifcopenshell_entity_set_int.argtypes = [ctypes.c_void_p, ctypes.c_char_p, ctypes.c_int64]
    _lib.ifcopenshell_entity_set_double.restype = None
    _lib.ifcopenshell_entity_set_double.argtypes = [ctypes.c_void_p, ctypes.c_char_p, ctypes.c_double]
    _lib.ifcopenshell_entity_set_bool.restype = None
    _lib.ifcopenshell_entity_set_bool.argtypes = [ctypes.c_void_p, ctypes.c_char_p, ctypes.c_int]
    _lib.ifcopenshell_entity_set_enum.restype = ctypes.c_bool
    _lib.ifcopenshell_entity_set_enum.argtypes = [ctypes.c_void_p, ctypes.c_char_p, ctypes.c_char_p]
    _lib.ifcopenshell_entity_set_reference.restype = None
    _lib.ifcopenshell_entity_set_reference.argtypes = [ctypes.c_void_p, ctypes.c_char_p, ctypes.c_void_p]
    _lib.ifcopenshell_entity_set_null.restype = None
    _lib.ifcopenshell_entity_set_null.argtypes = [ctypes.c_void_p, ctypes.c_char_p]

    _lib.ifcopenshell_file_create_type_instance.restype = ctypes.c_void_p
    _lib.ifcopenshell_file_create_type_instance.argtypes = [ctypes.c_void_p, ctypes.c_char_p, ctypes.c_char_p]

    _lib.ifcopenshell_entity_set_typed_value.restype = ctypes.c_bool
    _lib.ifcopenshell_entity_set_typed_value.argtypes = [ctypes.c_void_p, ctypes.c_char_p, ctypes.c_char_p, ctypes.c_char_p]

    _lib.ifcopenshell_entity_get_typed_value.restype = ctypes.c_char_p
    _lib.ifcopenshell_entity_get_typed_value.argtypes = [ctypes.c_void_p, ctypes.c_char_p, ctypes.POINTER(ctypes.c_char_p)]

    # -- Aggregate getters ----------------------------------------------------
    _lib.ifcopenshell_entity_get_aggregate_size.restype = ctypes.c_int32
    _lib.ifcopenshell_entity_get_aggregate_size.argtypes = [ctypes.c_void_p, ctypes.c_char_p]
    _lib.ifcopenshell_entity_get_aggregate_int.restype = ctypes.POINTER(ctypes.c_int64)
    _lib.ifcopenshell_entity_get_aggregate_int.argtypes = [ctypes.c_void_p, ctypes.c_char_p, ctypes.POINTER(ctypes.c_uint32)]
    _lib.ifcopenshell_entity_get_aggregate_double.restype = ctypes.POINTER(ctypes.c_double)
    _lib.ifcopenshell_entity_get_aggregate_double.argtypes = [ctypes.c_void_p, ctypes.c_char_p, ctypes.POINTER(ctypes.c_uint32)]
    _lib.ifcopenshell_entity_get_aggregate_string.restype = ctypes.POINTER(ctypes.c_char_p)
    _lib.ifcopenshell_entity_get_aggregate_string.argtypes = [ctypes.c_void_p, ctypes.c_char_p, ctypes.POINTER(ctypes.c_uint32)]
    _lib.ifcopenshell_entity_get_aggregate_ref.restype = ctypes.POINTER(ctypes.c_void_p)
    _lib.ifcopenshell_entity_get_aggregate_ref.argtypes = [ctypes.c_void_p, ctypes.c_char_p, ctypes.POINTER(ctypes.c_uint32)]
    _lib.ifcopenshell_entity_get_aggregate_typed_value.restype = ctypes.c_int32
    _lib.ifcopenshell_entity_get_aggregate_typed_value.argtypes = [ctypes.c_void_p, ctypes.c_char_p, ctypes.POINTER(ctypes.POINTER(ctypes.c_char_p)), ctypes.POINTER(ctypes.POINTER(ctypes.c_char_p))]

    # -- Aggregate setters ----------------------------------------------------
    for name in ("int", "double", "string", "ref"):
        fn = getattr(_lib, f"ifcopenshell_entity_set_aggregate_{name}")
        fn.restype = ctypes.c_bool
    _lib.ifcopenshell_entity_set_aggregate_int.argtypes = [ctypes.c_void_p, ctypes.c_char_p, ctypes.POINTER(ctypes.c_int64), ctypes.c_uint32]
    _lib.ifcopenshell_entity_set_aggregate_double.argtypes = [ctypes.c_void_p, ctypes.c_char_p, ctypes.POINTER(ctypes.c_double), ctypes.c_uint32]
    _lib.ifcopenshell_entity_set_aggregate_string.argtypes = [ctypes.c_void_p, ctypes.c_char_p, ctypes.POINTER(ctypes.c_char_p), ctypes.c_uint32]
    _lib.ifcopenshell_entity_set_aggregate_ref.argtypes = [ctypes.c_void_p, ctypes.c_char_p, ctypes.POINTER(ctypes.c_void_p), ctypes.c_uint32]
    _lib.ifcopenshell_entity_set_aggregate_typed_value.restype = ctypes.c_bool
    _lib.ifcopenshell_entity_set_aggregate_typed_value.argtypes = [ctypes.c_void_p, ctypes.c_char_p, ctypes.POINTER(ctypes.c_char_p), ctypes.POINTER(ctypes.c_char_p), ctypes.c_uint32]

    # -- Aggregate memory management ------------------------------------------
    _lib.ifcopenshell_free_int_array.restype = None
    _lib.ifcopenshell_free_int_array.argtypes = [ctypes.POINTER(ctypes.c_int64)]
    _lib.ifcopenshell_free_double_array.restype = None
    _lib.ifcopenshell_free_double_array.argtypes = [ctypes.POINTER(ctypes.c_double)]
    _lib.ifcopenshell_free_string_array.restype = None
    _lib.ifcopenshell_free_string_array.argtypes = [ctypes.POINTER(ctypes.c_char_p), ctypes.c_uint32]

    # -- Inverse attribute access ---------------------------------------------
    _lib.ifcopenshell_entity_is_inverse_attr.restype = ctypes.c_bool
    _lib.ifcopenshell_entity_is_inverse_attr.argtypes = [ctypes.c_void_p, ctypes.c_char_p]
    _lib.ifcopenshell_entity_get_inverse_attr.restype = ctypes.POINTER(ctypes.c_void_p)
    _lib.ifcopenshell_entity_get_inverse_attr.argtypes = [ctypes.c_void_p, ctypes.c_char_p, ctypes.POINTER(ctypes.c_uint32)]

    # -- Utility: deep removal ------------------------------------------------
    _lib.ifcopenshell_util_remove_deep2.restype = None
    _lib.ifcopenshell_util_remove_deep2.argtypes = [ctypes.c_void_p]

    # -- High-level: aggregate module -----------------------------------------
    _lib.ifcopenshell_aggregate_assign_object.restype = ctypes.c_void_p
    _lib.ifcopenshell_aggregate_assign_object.argtypes = [ctypes.c_void_p, ctypes.POINTER(ctypes.c_void_p), ctypes.c_uint32, ctypes.c_void_p]

    _lib.ifcopenshell_aggregate_unassign_object.restype = None
    _lib.ifcopenshell_aggregate_unassign_object.argtypes = [ctypes.c_void_p, ctypes.POINTER(ctypes.c_void_p), ctypes.c_uint32]

    # -- High-level: spatial module -------------------------------------------
    _lib.ifcopenshell_spatial_assign_container.restype = ctypes.c_void_p
    _lib.ifcopenshell_spatial_assign_container.argtypes = [ctypes.c_void_p, ctypes.POINTER(ctypes.c_void_p), ctypes.c_uint32, ctypes.c_void_p]

    _lib.ifcopenshell_spatial_unassign_container.restype = None
    _lib.ifcopenshell_spatial_unassign_container.argtypes = [ctypes.c_void_p, ctypes.POINTER(ctypes.c_void_p), ctypes.c_uint32]

    # -- High-level: type module ----------------------------------------------
    _lib.ifcopenshell_type_assign_type.restype = ctypes.c_void_p
    _lib.ifcopenshell_type_assign_type.argtypes = [ctypes.c_void_p, ctypes.POINTER(ctypes.c_void_p), ctypes.c_uint32, ctypes.c_void_p]

    _lib.ifcopenshell_type_unassign_type.restype = None
    _lib.ifcopenshell_type_unassign_type.argtypes = [ctypes.c_void_p, ctypes.POINTER(ctypes.c_void_p), ctypes.c_uint32]

    # -- High-level: nest -------------------------------------------------------
    _lib.ifcopenshell_nest_assign_object.restype = ctypes.c_void_p
    _lib.ifcopenshell_nest_assign_object.argtypes = [ctypes.c_void_p, ctypes.POINTER(ctypes.c_void_p), ctypes.c_uint32, ctypes.c_void_p]

    _lib.ifcopenshell_nest_unassign_object.restype = None
    _lib.ifcopenshell_nest_unassign_object.argtypes = [ctypes.c_void_p, ctypes.POINTER(ctypes.c_void_p), ctypes.c_uint32]

    # -- High-level: group -------------------------------------------------------
    _lib.ifcopenshell_group_assign_group.restype = ctypes.c_void_p
    _lib.ifcopenshell_group_assign_group.argtypes = [ctypes.c_void_p, ctypes.POINTER(ctypes.c_void_p), ctypes.c_uint32, ctypes.c_void_p]

    _lib.ifcopenshell_group_unassign_group.restype = None
    _lib.ifcopenshell_group_unassign_group.argtypes = [ctypes.c_void_p, ctypes.POINTER(ctypes.c_void_p), ctypes.c_uint32, ctypes.c_void_p]

    # -- High-level: root.create_entity ---------------------------------------
    _lib.ifcopenshell_root_create_entity.restype = ctypes.c_void_p
    _lib.ifcopenshell_root_create_entity.argtypes = [ctypes.c_void_p, ctypes.c_char_p, ctypes.c_char_p, ctypes.c_char_p, ctypes.c_void_p]

    # -- High-level: util.pset templates --------------------------------------
    _lib.ifcopenshell_util_pset_set_template_dir.restype = None
    _lib.ifcopenshell_util_pset_set_template_dir.argtypes = [ctypes.c_char_p]
    _lib.ifcopenshell_util_pset_get_template.restype = ctypes.c_void_p
    _lib.ifcopenshell_util_pset_get_template.argtypes = [ctypes.c_char_p]
    _lib.ifcopenshell_util_pset_template_get_by_name.restype = ctypes.c_void_p
    _lib.ifcopenshell_util_pset_template_get_by_name.argtypes = [ctypes.c_void_p, ctypes.c_char_p]
    _lib.ifcopenshell_util_pset_template_is_templated.restype = ctypes.c_bool
    _lib.ifcopenshell_util_pset_template_is_templated.argtypes = [ctypes.c_void_p, ctypes.c_char_p]
    _lib.ifcopenshell_util_pset_template_get_applicable.restype = ctypes.POINTER(ctypes.c_void_p)
    _lib.ifcopenshell_util_pset_template_get_applicable.argtypes = [
        ctypes.c_void_p, ctypes.c_char_p, ctypes.c_char_p,
        ctypes.c_bool, ctypes.c_bool, ctypes.c_char_p, ctypes.POINTER(ctypes.c_uint32)
    ]
    _lib.ifcopenshell_util_pset_template_get_applicable_names.restype = ctypes.POINTER(ctypes.c_char_p)
    _lib.ifcopenshell_util_pset_template_get_applicable_names.argtypes = [
        ctypes.c_void_p, ctypes.c_char_p, ctypes.c_char_p,
        ctypes.c_bool, ctypes.c_bool, ctypes.c_char_p, ctypes.POINTER(ctypes.c_uint32)
    ]
    _lib.ifcopenshell_util_pset_template_pset_type.restype = ctypes.c_char_p
    _lib.ifcopenshell_util_pset_template_pset_type.argtypes = [ctypes.c_void_p]

    # -- High-level: util.schema.reassign_class -------------------------------
    _lib.ifcopenshell_util_schema_reassign_class.restype = ctypes.c_void_p
    _lib.ifcopenshell_util_schema_reassign_class.argtypes = [
        ctypes.c_void_p, ctypes.c_void_p, ctypes.c_char_p
    ]

    # Make the bundled buildingSMART pset templates discoverable to the
    # native pset edit / template code. Templates ship under
    # ``ifcopenshell/util/schema/`` and are loaded lazily on first lookup.
    try:
        _schema_dir = os.path.join(
            os.path.dirname(os.path.abspath(__file__)), "util", "schema"
        )
        if os.path.isdir(_schema_dir):
            _lib.ifcopenshell_util_pset_set_template_dir(_enc(_schema_dir))
    except Exception:
        pass

    return _lib


# ---------------------------------------------------------------------------
# Attribute type tags (must match ifcopenshell_attr_type enum in ifcapi.h)
# ---------------------------------------------------------------------------
ATTR_NULL = 0
ATTR_STRING = 1
ATTR_INT = 2
ATTR_DOUBLE = 3
ATTR_BOOL = 4
ATTR_ENUM = 5
ATTR_REFERENCE = 6
ATTR_AGGREGATE = 7
ATTR_DERIVED = 8
ATTR_UNKNOWN = 99


def _enc(s):
    """Encode a string to UTF-8 bytes for ctypes, or return None."""
    if s is None:
        return None
    return s.encode("utf-8") if isinstance(s, str) else s


# ---------------------------------------------------------------------------
# _typed_value — lightweight wrapper for inline IFC simple types
# ---------------------------------------------------------------------------

# Cache: (schema_name, type_name) -> resolver kind tuple
# Resolver kinds: ("scalar", primitive), ("aggregate", element_kind), ("unknown",)
_TYPED_VALUE_KIND_CACHE: dict = {}

_SIMPLE_KIND_TO_PRIMITIVE = {
    "string": "string",
    "real": "real",
    "number": "real",
    "integer": "integer",
    "boolean": "boolean",
    "logical": "logical",
    "binary": "string",
}


def _scalar_kind_for_pt(pt) -> str:
    """Walk a parameter_type fully through nested typedefs to its scalar kind."""
    seen = 0
    while pt is not None and seen < 16:
        seen += 1
        st = pt.as_simple_type()
        if st is not None:
            return _SIMPLE_KIND_TO_PRIMITIVE.get(st.declared_type(), "real")
        nt = pt.as_named_type()
        if nt is not None:
            inner = nt.declared_type()
            inner_td = inner.as_type_declaration() if inner else None
            if inner_td is not None:
                pt = inner_td.declared_type()
                continue
        return "real"
    return "real"


def _resolve_typed_value_kind(file_obj, type_name: str):
    """Use schema introspection to map an IFC type name to a python kind.

    Returns one of:
      ("scalar", "string"|"real"|"integer"|"boolean"|"logical")
      ("aggregate", <scalar_kind>)
      ("unknown",)
    """
    if file_obj is None or not type_name:
        return ("unknown",)
    schema_name = getattr(file_obj, "_schema_name", None)
    if schema_name is None:
        try:
            schema_name = file_obj.schema
        except Exception:
            schema_name = None
        if schema_name:
            try:
                file_obj._schema_name = schema_name
            except Exception:
                pass
    cache_key = (schema_name or "", type_name)
    cached = _TYPED_VALUE_KIND_CACHE.get(cache_key)
    if cached is not None:
        return cached

    kind = ("unknown",)
    try:
        from ifcopenshell import ifcopenshell_wrapper as _wrapper
        schema = _wrapper.schema_by_name(schema_name) if schema_name else None
        decl = schema.declaration_by_name(type_name) if schema else None
        td = decl.as_type_declaration() if decl else None
        pt = td.declared_type() if td else None
        # Walk through nested named types (typedefs of typedefs)
        seen = 0
        while pt is not None and seen < 16:
            seen += 1
            nt = pt.as_named_type()
            if nt is not None:
                inner = nt.declared_type()
                inner_td = inner.as_type_declaration() if inner else None
                if inner_td is not None:
                    pt = inner_td.declared_type()
                    continue
                pt = None
                break
            agg = pt.as_aggregation_type()
            if agg is not None:
                kind = ("aggregate", _scalar_kind_for_pt(agg.type_of_element()))
                break
            st = pt.as_simple_type()
            if st is not None:
                kind = ("scalar", _SIMPLE_KIND_TO_PRIMITIVE.get(st.declared_type(), "real"))
                break
            break
    except Exception:
        kind = ("unknown",)

    _TYPED_VALUE_KIND_CACHE[cache_key] = kind
    return kind


def _convert_scalar(prim: str, raw):
    if raw is None:
        return None
    if not isinstance(raw, str):
        return raw
    if prim == "integer":
        try:
            return int(raw)
        except (ValueError, TypeError):
            return raw
    if prim == "real":
        try:
            return float(raw)
        except (ValueError, TypeError):
            return raw
    if prim in ("boolean", "logical"):
        return raw.lower() in ("true", "1", ".t.")
    return raw


def _parse_aggregate_literal(raw, elem_prim: str):
    if raw is None:
        return None
    if isinstance(raw, (list, tuple)):
        return tuple(_convert_scalar(elem_prim, v) for v in raw)
    if not isinstance(raw, str):
        return raw
    s = raw.strip()
    if s.startswith("(") and s.endswith(")"):
        inner = s[1:-1].strip()
        if not inner:
            return ()
        parts = [p.strip() for p in inner.split(",")]
        return tuple(_convert_scalar(elem_prim, p) for p in parts)
    return raw


# ---------------------------------------------------------------------------
# entity_instance
# ---------------------------------------------------------------------------

from ifcopenshell.entity_instance import entity_instance  # noqa: E402


class _typed_value(entity_instance):
    """Represents an inline IFC type instance like ``IfcLabel('Hello')``.

    Inherits from :class:`entity_instance` so that ``isinstance`` checks and
    unbound-method calls (e.g. ``entity_instance.is_a(tv)``) behave the same
    as SWIG, where inline values are returned as ``entity_instance`` objects
    with ``id() == 0``. The handle is set to ``0`` and the typed-value
    payload is stored in ``_type_name`` / ``_wrapped``; base methods on
    ``entity_instance`` detect this inline mode and dispatch on the payload.
    """

    def __init__(self, file_obj, type_name: str, value):
        # Bypass entity_instance.__init__'s handle bookkeeping; inline values
        # never own a native handle. Setting attributes directly via
        # object.__setattr__ avoids tripping any future descriptor logic.
        object.__setattr__(self, "_file", file_obj)
        object.__setattr__(self, "_handle", 0)
        object.__setattr__(self, "_type_name", type_name)
        kind = _resolve_typed_value_kind(file_obj, type_name)
        if kind[0] == "aggregate":
            wrapped = _parse_aggregate_literal(value, kind[1])
        elif kind[0] == "scalar":
            wrapped = _convert_scalar(kind[1], value)
        elif value is None or not isinstance(value, str):
            wrapped = value
        else:
            try:
                wrapped = float(value)
            except (ValueError, TypeError):
                wrapped = value
        object.__setattr__(self, "_wrapped", wrapped)

    def is_a(self, ifc_class=None):
        if ifc_class is None:
            return self._type_name
        if isinstance(ifc_class, bool) and ifc_class:
            schema = getattr(self._file, "schema", "")
            return f"{schema}.{self._type_name}" if schema else self._type_name
        return self._type_name.lower() == ifc_class.lower()

    @property
    def wrappedValue(self):
        return self._wrapped

    @wrappedValue.setter
    def wrappedValue(self, value):
        object.__setattr__(self, "_wrapped", value)

    def id(self) -> int:
        return 0

    def __repr__(self):
        return f"{self._type_name}({self._wrapped!r})"

    def __eq__(self, other):
        if isinstance(other, _typed_value):
            return self._type_name == other._type_name and self._wrapped == other._wrapped
        return NotImplemented

    def __hash__(self):
        return hash((self._type_name, self._wrapped))

    def __getitem__(self, index):
        # Mirrors entity_instance: index 0 returns the only positional
        # attribute (the wrapped value); other indices raise IndexError.
        if index == 0:
            return self._wrapped
        raise IndexError(index)

    def __len__(self):
        # Mirrors entity_instance: number of positional attributes. Inline
        # typed values always have exactly one (the wrapped value).
        return 1

    def __iter__(self):
        # Inline typed values have a single positional slot (the wrapped
        # value). Iteration should always yield exactly one item — the full
        # wrapped payload — so ``enumerate(tv)`` matches SWIG's behaviour
        # (one attribute named ``wrappedValue``).
        return iter((self._wrapped,))

    def __setitem__(self, index, value):
        # Inline typed values have a single positional slot (the wrapped
        # value). copy_deep / similar utilities assign to ``[0]`` after
        # creating the value via ``create_entity``.
        if index == 0:
            object.__setattr__(self, "_wrapped", value)
            return
        raise IndexError(index)

    def __setattr__(self, name, value):
        # Bypass entity_instance.__setattr__ — inline values have no handle,
        # so the native attribute path would crash. Property setters (e.g.
        # ``wrappedValue``) are still honoured because ``object.__setattr__``
        # respects data descriptors defined on the class.
        object.__setattr__(self, name, value)

    def __del__(self):
        # Inline values never own a native handle; suppress entity_instance's
        # destroy call entirely.
        pass

# ---------------------------------------------------------------------------
# file
# ---------------------------------------------------------------------------

HEADER_FIELDS = {
    "file_description": ("description", "implementation_level"),
    "file_name": (
        "name",
        "time_stamp",
        "author",
        "organization",
        "preprocessor_version",
        "originating_system",
        "authorization",
    ),
}


def _schema_identifier_from_version(schema_version) -> str:
    major, minor, addendum, corrigendum = tuple(schema_version)
    schema = f"IFC{major}"
    if minor:
        schema += f"X{minor}"
    if addendum:
        schema += f"_ADD{addendum}"
    if corrigendum:
        schema += f"_TC{corrigendum}"
    return file._SCHEMA_ALIASES.get(schema, schema)


def _schema_version_from_identifier(schema_identifier: str) -> tuple[int, int, int, int]:
    match = re.fullmatch(r"IFC(?P<major>\d+)(?:X(?P<minor>\d+))?(?:_ADD(?P<addendum>\d+))?(?:_TC(?P<corrigendum>\d+))?", schema_identifier)
    if not match:
        return ()
    return tuple(int(match.group(name) or 0) for name in ("major", "minor", "addendum", "corrigendum"))


class file:
    """Wraps a native IFC file."""

    # Canonical schema aliasing — map user-facing schema names that don't
    # have a dedicated EXPRESS schema to the canonical release that backs
    # them. Mirrors the mapping in ifcopenshell.schema_by_name upstream so
    # DERIVE/WHERE rules registered against the canonical name resolve
    # correctly when the file is created with the bare-major name.
    _SCHEMA_ALIASES = {"IFC4X3": "IFC4X3_ADD2"}

    def __init__(self, schema="IFC4", schema_version=None):
        if schema_version is not None:
            schema = _schema_identifier_from_version(schema_version)
        else:
            schema = self._SCHEMA_ALIASES.get(schema, schema)
        lib = _get_lib()
        self._ptr = lib.ifcopenshell_file_create(_enc(schema))
        if not self._ptr:
            err = lib.ifcopenshell_last_error_message()
            msg = err.decode("utf-8") if err else "Unknown error"
            raise RuntimeError(f"Failed to create IFC file: {msg}")
        self._owns_ptr = True
        self.header = _file_header(self)
        self.transaction = None
        self.history = []
        self.future = []
        self.history_size = 64
        self.units = {}
        self.to_delete = None

    def batch(self) -> None:
        if self.transaction:
            self.transaction.batch()

    def unbatch(self) -> None:
        if self.transaction:
            self.transaction.unbatch()

    def set_history_size(self, size: int) -> None:
        self.history_size = size
        while len(self.history) > self.history_size:
            self.history.pop(0)

    def begin_transaction(self) -> None:
        if self.history_size:
            self.transaction = Transaction(self)

    def end_transaction(self) -> None:
        if self.transaction:
            self.history.append(self.transaction)
            if len(self.history) > self.history_size:
                self.history.pop(0)
            self.future = []
            self.transaction = None

    def discard_transaction(self) -> None:
        if self.transaction:
            self.transaction.rollback()
        self.transaction = None

    def undo(self) -> None:
        if not self.history:
            return
        transaction = self.history.pop()
        try:
            transaction.rollback()
        except Exception as e:
            raise UndoSystemError("Error during transaction undo.", transaction) from e
        self.future.append(transaction)

    def redo(self) -> None:
        if not self.future:
            return
        transaction = self.future.pop()
        try:
            transaction.commit()
        except Exception as e:
            raise UndoSystemError("Error during transaction redo.", transaction) from e
        self.history.append(transaction)

    def assign_header_from(self, other: "file") -> None:
        for section_name, attr_names in HEADER_FIELDS.items():
            try:
                target = getattr(self.header, section_name)
                source = getattr(other.header, section_name)
            except AttributeError:
                continue
            for attr_name in attr_names:
                try:
                    value = getattr(source, attr_name)
                except AttributeError:
                    continue
                setattr(target, attr_name, value)

    def __del__(self):
        if getattr(self, "_ptr", None) and getattr(self, "_owns_ptr", True):
            lib = _get_lib()
            lib.ifcopenshell_file_free(self._ptr)
        self._ptr = None

    @property
    def schema_identifier(self) -> str:
        """Full IFC schema version: IFC2X3_TC1, IFC4_ADD2, IFC4X3_ADD2, etc."""
        lib = _get_lib()
        val = lib.ifcopenshell_file_schema(self._ptr)
        return val.decode("utf-8") if val else ""

    @property
    def schema(self) -> str:
        """General IFC schema version: IFC2X3, IFC4, IFC4X3."""
        prefixes = ("IFC", "X", "_ADD", "_TC")
        reg = "".join(f"(?P<{s}>{s}\\d+)?" for s in prefixes)
        match = re.match(reg, self.schema_identifier)
        if not match:
            return self.schema_identifier
        version_tuple = tuple(
            (int(match.group(p)[len(p):]) if match.group(p) else None)
            for p in prefixes
        )
        return "".join(
            f"{p}{v}" if v is not None else ""
            for p, v in zip(prefixes, version_tuple[0:2])
        )

    @property
    def schema_version(self) -> tuple[int, int, int, int]:
        return _schema_version_from_identifier(self.schema_identifier)

    @property
    def mvd(self):
        from ifcopenshell.util.mvd_info import LARK_AVAILABLE, MvdInfo

        if not LARK_AVAILABLE:
            return None
        return MvdInfo(self.header)

    def create_entity(self, type_name=None, *args, **kwargs):
        if type_name is None:
            type_name = kwargs.pop("type", None)
        elif "type" in kwargs:
            kwargs.pop("type")
        if type_name is None:
            raise TypeError("create_entity() requires a type name")
        eid = kwargs.pop("id", -1)
        lib = _get_lib()
        # Probe the schema to see whether this name is a type declaration
        # (IfcLabel, IfcLineIndex, ...) rather than an entity. Type
        # declarations are not creatable via the C entity API and must be
        # returned as inline ``_typed_value`` instances (mirrors SWIG, where
        # ``create_entity('IfcLabel', 'foo')`` yields an ``entity_instance``
        # with ``id() == 0``).
        if _resolve_typed_value_kind(self, type_name)[0] != "unknown":
            val_arg = args[0] if args else kwargs.get("wrappedValue")
            return _typed_value(self, type_name, val_arg)
        if eid == -1 or eid is None:
            h = lib.ifcopenshell_file_create_entity(self._ptr, _enc(type_name))
        else:
            h = lib.ifcopenshell_file_create_entity_with_id(self._ptr, _enc(type_name), int(eid))
        if not h:
            err = lib.ifcopenshell_last_error_message()
            msg = err.decode("utf-8") if err else "Unknown error"
            raise RuntimeError(f"Failed to create entity '{type_name}': {msg}")
        entity = entity_instance(self, h)
        # Suspend transaction recording while populating attributes — the
        # creation itself already captures the full attribute payload.
        active_transaction = self.transaction
        if args or kwargs:
            self.transaction = None
        try:
            for i, arg in enumerate(args):
                if arg is not None:
                    entity[i] = arg
            for attr_name, value in kwargs.items():
                setattr(entity, attr_name, value)
        finally:
            self.transaction = active_transaction
        if active_transaction is not None:
            active_transaction.store_create(entity)
        return entity

    def by_type(self, type_name, include_subtypes=True) -> list:
        lib = _get_lib()
        count = lib.ifcopenshell_file_by_type_count(self._ptr, _enc(type_name))
        if count <= 0:
            return []
        handles = (ctypes.c_void_p * count)()
        lib.ifcopenshell_file_by_type(self._ptr, _enc(type_name), handles)
        result = [entity_instance(self, handles[i]) for i in range(count)]
        if not include_subtypes:
            result = [e for e in result if e.is_a().lower() == type_name.lower()]
        return result

    def by_id(self, id: int) -> entity_instance:
        if isinstance(id, (str, bytes)):
            return self.by_guid(id.decode("utf-8") if isinstance(id, bytes) else id)
        lib = _get_lib()
        out = ctypes.c_void_p(0)
        ok = lib.ifcopenshell_ifc_file_by_id(self._ptr, id, ctypes.byref(out))
        if not ok or not out.value:
            raise RuntimeError(f"Entity #{id} not found")
        return entity_instance(self, out.value)

    def __getattr__(self, attr):
        if attr.startswith("create"):
            import functools
            return functools.partial(self.create_entity, attr[6:])
        raise AttributeError(f"'{type(self).__name__}' object has no attribute '{attr}'")

    def by_guid(self, guid: str) -> entity_instance:
        if isinstance(guid, int):
            return self.by_id(guid)
        lib = _get_lib()
        h = lib.ifcopenshell_file_by_guid(self._ptr, _enc(guid))
        if not h:
            raise RuntimeError(f"Entity with GUID '{guid}' not found")
        return entity_instance(self, h)

    def storage_mode(self) -> int:
        lib = _get_lib()
        out = ctypes.c_int32(-1)
        if not lib.ifcopenshell_ifc_file_storage_mode(self._ptr, ctypes.byref(out)):
            return -1
        return int(out.value)

    def key_value_store_query(self, key: str) -> bytes:
        from ifcopenshell import ifcopenshell_wrapper as W

        lib = _get_lib()
        W._bind()
        out = W.ifcopenshell_string_t()
        if not lib.ifcopenshell_ifc_file_key_value_store_query(self._ptr, _enc(key), ctypes.byref(out)):
            return b""
        try:
            if out.data and out.size:
                return ctypes.string_at(out.data, out.size)
            return b""
        finally:
            lib.ifcopenshell_string_destroy(ctypes.byref(out))

    def remove(self, entity) -> None:
        lib = _get_lib()
        if isinstance(entity, int):
            entity = self.by_id(entity)
        if not isinstance(entity, entity_instance):
            raise TypeError(f"Expected entity_instance or int, got {type(entity)}")
        if self.transaction is not None:
            self.transaction.store_delete(entity)
        lib.ifcopenshell_file_remove(entity._handle)

    def get_max_id(self) -> int:
        lib = _get_lib()
        return int(lib.ifcopenshell_file_get_max_id(self._ptr))

    def add(self, inst, _id=None):
        """Adds an entity (and its forward references) to this file.

        Mirrors upstream's ``file.add()``: when the source instance belongs
        to a different file, it is deep-copied across; otherwise it is
        re-registered (idempotent). When a transaction is active, every
        newly assigned ID becomes a ``store_create`` operation so that undo
        removes the entire subtree.
        """
        if not isinstance(inst, entity_instance):
            raise TypeError(f"Expected entity_instance, got {type(inst)}")
        # Inline typed values (``_typed_value`` subclass with ``_handle == 0``)
        # have no STEP identity; SWIG's wrapped_data.add() returns an
        # equivalent inline value bound to the target file. Mirror that here.
        if isinstance(inst, _typed_value):
            return _typed_value(self, inst._type_name, inst._wrapped)
        lib = _get_lib()
        max_id = self.get_max_id() if self.transaction is not None else 0
        h = lib.ifcopenshell_file_add_entity(
            self._ptr, inst._handle, 0 if _id is None else int(_id))
        if not h:
            err = lib.ifcopenshell_last_error_message()
            msg = err.decode("utf-8") if err else "Unknown error"
            raise RuntimeError(f"Failed to add entity: {msg}")
        result = entity_instance(self, h)
        if self.transaction is not None:
            added = [e for e in self.traverse(result) if e.id() > max_id]
            for e in reversed(added):
                self.transaction.store_create(e)
        return result

    def get_inverse(self, entity, allow_duplicate=False, with_attribute_indices=False):
        if with_attribute_indices and not allow_duplicate:
            raise ValueError("with_attribute_indices requires allow_duplicate to be True")
        lib = _get_lib()
        if not isinstance(entity, entity_instance):
            entity = self.by_id(entity)
        count = ctypes.c_uint32(0)
        arr = lib.ifcopenshell_file_get_inverse(entity._handle, ctypes.byref(count))
        if not arr or count.value == 0:
            inverses = []
        else:
            inverses = [entity_instance(self, arr[i]) for i in range(count.value)]
            lib.ifcopenshell_free_instance_array_only(arr)
        if allow_duplicate:
            if with_attribute_indices:
                idx_count = ctypes.c_uint32(0)
                idx_arr = lib.ifcopenshell_file_get_inverse_indices(entity._handle, ctypes.byref(idx_count))
                if not idx_arr or idx_count.value == 0:
                    idxs = []
                else:
                    idxs = [int(idx_arr[i]) for i in range(idx_count.value)]
                    lib.ifcopenshell_free_int32_array(idx_arr)
                return list(zip(inverses, idxs))
            return inverses
        return set(inverses)

    def get_total_inverses(self, entity) -> int:
        lib = _get_lib()
        if not isinstance(entity, entity_instance):
            entity = self.by_id(entity)
        count = ctypes.c_uint32(0)
        arr = lib.ifcopenshell_file_get_inverse(entity._handle, ctypes.byref(count))
        if arr:
            lib.ifcopenshell_free_instance_array(arr, count.value)
        return count.value

    def traverse(self, entity, max_levels=None, breadth_first=False) -> list:
        lib = _get_lib()
        if not isinstance(entity, entity_instance):
            entity = self.by_id(entity)
        count = ctypes.c_uint32(0)
        ml = max_levels if max_levels is not None else 0
        arr = lib.ifcopenshell_file_traverse(entity._handle, ml, ctypes.byref(count))
        if not arr or count.value == 0:
            return []
        result = [entity_instance(self, arr[i]) for i in range(count.value)]
        lib.ifcopenshell_free_instance_array_only(arr)
        return result

    def write(self, path, format=None, zipped=False) -> None:
        path = Path(path)
        path.parent.mkdir(parents=True, exist_ok=True)
        if format is None:
            format = ".ifcZIP" if path.suffix.lower() in (".ifczip", ".zip") else ".ifc"
        if format == ".ifcXML":
            raise NotImplementedError("Writing .ifcXML files is not supported")
        if format == ".ifcZIP":
            return self.write(path, ".ifc", zipped=True)
        lib = _get_lib()
        if not lib.ifcopenshell_file_write(self._ptr, _enc(str(path))):
            err = lib.ifcopenshell_last_error_message()
            msg = err.decode("utf-8") if err else "Unknown error"
            raise RuntimeError(f"Failed to write file: {msg}")
        if zipped:
            unzipped_path = path.with_suffix(format)
            path.rename(unzipped_path)
            with zipfile.ZipFile(path, "w") as zip_file:
                zip_file.write(unzipped_path, unzipped_path.name, compress_type=zipfile.ZIP_DEFLATED)
            unzipped_path.unlink()

    def to_string(self) -> str:
        lib = _get_lib()
        val = lib.ifcopenshell_file_to_string(self._ptr)
        return val.decode("utf-8") if val else ""

    def __len__(self) -> int:
        lib = _get_lib()
        return lib.ifcopenshell_file_entity_count(self._ptr)

    def entity_names(self) -> list:
        return [inst.id() for inst in self]

    @classmethod
    def from_string(cls, data) -> "file":
        lib = _get_lib()
        if isinstance(data, str):
            buf = data.encode("utf-8")
        elif isinstance(data, (bytes, bytearray)):
            buf = bytes(data)
        else:
            raise TypeError("from_string expects str or bytes")
        ptr = lib.ifcopenshell_file_from_string(buf, len(buf))
        if not ptr:
            err = lib.ifcopenshell_last_error_message()
            msg = err.decode("utf-8") if err else "Unknown error"
            raise RuntimeError(f"Failed to parse IFC from string: {msg}")
        return _wrap_file_ptr(ptr)

    def __iter__(self):
        lib = _get_lib()
        count = ctypes.c_uint32(0)
        arr = lib.ifcopenshell_file_entity_ids(self._ptr, ctypes.byref(count))
        if not arr or count.value == 0:
            return
        try:
            for i in range(count.value):
                yield entity_instance(self, arr[i])
        finally:
            lib.ifcopenshell_free_instance_array_only(arr)

    def __getitem__(self, key):
        if isinstance(key, int):
            return self.by_id(key)
        if isinstance(key, (str, bytes)):
            return self.by_guid(key.decode("utf-8") if isinstance(key, bytes) else key)
        raise TypeError(f"Expected int or str key, got {type(key)}")

    def __contains__(self, entity):
        if isinstance(entity, entity_instance):
            lib = _get_lib()
            return bool(lib.ifcopenshell_entity_type(entity._handle))
        return False

    def __getattr__(self, name):
        if name == "wrapped_data":
            return self
        if name.startswith("create") and name != "create_entity":
            type_name = name[6:]  # strip 'create' prefix
            def creator(*args, **kwargs):
                return self.create_entity(type_name, *args, **kwargs)
            return creator
        raise AttributeError(f"'{type(self).__name__}' has no attribute '{name}'")


# ---------------------------------------------------------------------------
# Header stubs
# ---------------------------------------------------------------------------

class _file_header:
    """File header backed by real header-section entity instances from C++."""

    def __init__(self, file_obj=None):
        self._file_ref = weakref.ref(file_obj) if file_obj is not None else None

    @property
    def _file(self):
        return self._file_ref() if self._file_ref is not None else None

    @property
    def file_description(self):
        if self._file is None:
            return _header_file_description()
        lib = _get_lib()
        h = lib.ifcopenshell_file_header_file_description(self._file._ptr)
        if not h:
            return _header_file_description()
        return entity_instance(self._file, h)

    @property
    def file_name(self):
        if self._file is None:
            return _header_file_name()
        lib = _get_lib()
        h = lib.ifcopenshell_file_header_file_name(self._file._ptr)
        if not h:
            return _header_file_name()
        return entity_instance(self._file, h)

    @property
    def file_schema(self):
        if self._file is None:
            return None
        lib = _get_lib()
        h = lib.ifcopenshell_file_header_file_schema(self._file._ptr)
        if not h:
            return None
        return entity_instance(self._file, h)


class _header_file_name:
    def __init__(self):
        self.name = ""
        self.time_stamp = ""
        self.preprocessor_version = ""
        self.originating_system = ""
        self.authorization = ""


class _header_file_description:
    def __init__(self):
        self.description = ()


def schema_by_name(name):
    """Look up an IFC schema by its string identifier (e.g. 'IFC4')."""
    from ifcopenshell import ifcopenshell_wrapper as _W
    return _W.schema_by_name(name)


_SCRATCH_FILES: dict = {}
_BORROWED_FILES: dict[int, file] = {}
_STREAM_ATTR_TYPE_CACHE: dict = {}


def _scratch_file(schema: str) -> "file":
    f = _SCRATCH_FILES.get(schema)
    if f is None:
        f = file(schema=schema)
        _SCRATCH_FILES[schema] = f
    return f


def create_entity(type: str, schema: str = "IFC4", *args, **kwargs):
    """Create an IFC entity in a per-schema scratch file.

    Used by the auto-generated EXPRESS rules modules to construct intermediate
    geometric entities while computing DERIVE attributes."""
    return _scratch_file(schema).create_entity(type, *args, **kwargs)


def _wrap_file_ptr(ptr, *, owned=True) -> "file":
    if isinstance(ptr, ctypes.c_void_p):
        ptr = ptr.value
    f = file.__new__(file)
    f._ptr = ptr
    f._owns_ptr = owned
    f.header = _file_header(f)
    f.transaction = None
    f.history = []
    f.future = []
    f.history_size = 64
    f.units = {}
    f.to_delete = None
    return f


def _borrow_file_ptr(ptr, fallback=None) -> "file":
    if isinstance(ptr, ctypes.c_void_p):
        ptr = ptr.value
    if not ptr:
        return fallback
    if fallback is not None and getattr(fallback, "_ptr", None) == ptr:
        return fallback
    borrowed = _BORROWED_FILES.get(ptr)
    if borrowed is None or getattr(borrowed, "_ptr", None) != ptr:
        borrowed = _wrap_file_ptr(ptr, owned=False)
        _BORROWED_FILES[ptr] = borrowed
    return borrowed


class _InstanceStreamer:
    def __init__(self, ptr):
        self._ptr = ptr.value if isinstance(ptr, ctypes.c_void_p) else ptr

    @classmethod
    def create(cls):
        ptr = _get_lib().ifcopenshell_instance_streamer_create()
        if not ptr:
            raise RuntimeError("Failed to create InstanceStreamer")
        return cls(ptr)

    @classmethod
    def create_from_path(cls, path, mmap=False):
        ptr = _get_lib().ifcopenshell_instance_streamer_create_from_path(_enc(str(path)), bool(mmap))
        if not ptr:
            err = _get_lib().ifcopenshell_last_error_message()
            msg = err.decode("utf-8") if err else "Unknown error"
            raise RuntimeError(f"Failed to stream {path}: {msg}")
        return cls(ptr)

    @classmethod
    def from_string(cls, data):
        if isinstance(data, str):
            data = data.encode("utf-8")
        out = ctypes.c_void_p()
        if not _get_lib().ifcopenshell_ifcparse_stream_from_string(data, ctypes.byref(out)):
            err = _get_lib().ifcopenshell_last_error_message()
            msg = err.decode("utf-8") if err else "Unknown error"
            raise RuntimeError(f"Failed to stream IFC data: {msg}")
        return cls(out)

    def __del__(self):
        ptr = getattr(self, "_ptr", None)
        if ptr:
            try:
                _get_lib().ifcopenshell_ifc_instance_streamer_destroy(ptr)
            except Exception:
                pass
            self._ptr = None

    def pushPage(self, data):
        if isinstance(data, str):
            data = data.encode("utf-8")
        return _get_lib().ifcopenshell_ifc_instance_streamer_push_page(self._ptr, data)

    def hasSemicolon(self):
        return _get_lib().ifcopenshell_ifc_instance_streamer_has_semicolon(self._ptr)

    def readInstancePy(self, type_as_declaration_instance=False):
        from ifcopenshell import ifcopenshell_wrapper as _W

        out = _W.ifcopenshell_string_t()
        ok = _get_lib().ifcopenshell_ifc_instance_streamer_read_instance_py(
            self._ptr,
            bool(type_as_declaration_instance),
            ctypes.byref(out),
        )
        if not ok:
            err = _get_lib().ifcopenshell_last_error_message()
            msg = err.decode("utf-8") if err else "Unknown error"
            raise RuntimeError(f"Failed to read streamed IFC instance: {msg}")
        text = _W._take_string(out)
        value = json.loads(text)
        return None if value is None else value


def _schema_name_from_spf(data: str) -> Optional[str]:
    match = re.search(r"FILE_SCHEMA\s*\(\s*\(\s*'([^']+)'", data, re.IGNORECASE)
    return match.group(1) if match else None


def _schema_from_spf_path(path):
    try:
        with builtins.open(path, encoding="ascii", errors="ignore") as f:
            return _schema_name_from_spf(f.read(8192))
    except OSError:
        return None


def _schema_by_name_or_none(schema_name):
    if not schema_name:
        return None
    try:
        from ifcopenshell import ifcopenshell_wrapper as _W

        return _W.schema_by_name(schema_name)
    except Exception:
        return None


def _type_decl_wrap_candidate(parameter_type, value):
    named = parameter_type.as_named_type() if parameter_type is not None else None
    declaration = named.declared_type() if named is not None else None
    if declaration is None:
        return None

    type_decl = declaration.as_type_declaration()
    if type_decl is not None:
        declared_type = type_decl.declared_type()
        if declared_type is not None and declared_type.as_aggregation_type() is not None and isinstance(value, list):
            return type_decl.name()
        if not isinstance(value, (dict, list)):
            return type_decl.name()
        return None

    select = declaration.as_select_type()
    if select is None:
        return None
    for item in select.select_list():
        type_decl = item.as_type_declaration()
        if type_decl is None:
            continue
        declared_type = type_decl.declared_type()
        aggregate = declared_type.as_aggregation_type() if declared_type is not None else None
        if aggregate is not None and isinstance(value, list):
            return type_decl.name()
        if aggregate is None and not isinstance(value, (dict, list)):
            return type_decl.name()
    return None


def _stream_attr_type_name(schema, entity_name: str, attribute_name: str, value):
    if schema is None:
        return None
    key = (schema.name(), entity_name, attribute_name, isinstance(value, list), isinstance(value, dict))
    if key in _STREAM_ATTR_TYPE_CACHE:
        return _STREAM_ATTR_TYPE_CACHE[key]
    result = None
    declaration = schema.declaration_by_name(entity_name)
    entity = declaration.as_entity() if declaration is not None else None
    if entity is not None:
        index = entity.attribute_index(attribute_name)
        attributes = entity.all_attributes()
        if 0 <= index < len(attributes):
            result = _type_decl_wrap_candidate(attributes[index].type_of_attribute(), value)
    _STREAM_ATTR_TYPE_CACHE[key] = result
    return result


def _postprocess_stream_instance(instance, schema):
    if instance is None or schema is None:
        return instance
    entity_name = instance.get("type")
    if not entity_name:
        return instance
    for key, value in tuple(instance.items()):
        if key in ("id", "type"):
            continue
        type_name = _stream_attr_type_name(schema, entity_name, key, value)
        if type_name is not None:
            instance[key] = {"type": type_name, "value": tuple(value) if isinstance(value, list) else value}
    return instance


def stream2(path, mmap=False, page_size=0):
    """Yield parsed STEP instances without materialising a full file."""
    schema = _schema_by_name_or_none(_schema_from_spf_path(path))
    if page_size:
        with builtins.open(path, encoding="ascii") as f:
            yield from stream2_from_string(f.read())
        return

    streamer = _InstanceStreamer.create_from_path(path, mmap)
    while True:
        instance = streamer.readInstancePy()
        if instance is None:
            break
        yield _postprocess_stream_instance(instance, schema)


def stream2_from_string(data):
    """Yield parsed STEP instances from an in-memory IFC-SPF string."""
    streamer = _InstanceStreamer.from_string(data)
    schema_text = data if isinstance(data, str) else data.decode("ascii", errors="ignore")
    schema = _schema_by_name_or_none(_schema_name_from_spf(schema_text))
    while True:
        instance = streamer.readInstancePy()
        if instance is None:
            break
        yield _postprocess_stream_instance(instance, schema)


def _open_bypass(path, bypass_types):
    type_names = tuple(_enc(str(name)) for name in bypass_types)
    array_type = ctypes.c_char_p * len(type_names)
    return _get_lib().ifcopenshell_file_open_bypass(_enc(str(path)), array_type(*type_names), len(type_names))


from ifcopenshell.sql import sqlite  # noqa: E402


def open(path, format=None, should_stream=False, readonly=False, bypass_types=None):  # noqa: A001
    """Load an IFC file from disk.

    Minimal implementation backed by the native C API. ``readonly`` is
    accepted for upstream API parity but currently has no native effect.
    """
    path = Path(path)
    suffix = path.suffix.lower()
    if format is None and suffix in (".ifczip", ".zip"):
        format = ".ifcZIP"
    if format == ".ifcZIP":
        with tempfile.TemporaryDirectory() as unzipped_path:
            with zipfile.ZipFile(path) as zf:
                for name in zf.namelist():
                    if Path(name).suffix.lower() in (".ifc", ".ifcxml"):
                        return open(zf.extract(name, unzipped_path), should_stream=should_stream, readonly=readonly)
                raise LookupError(f"No .ifc or .ifcXML file found in {path}")
    if format == ".ifcSQLite" or suffix in (".ifcsqlite", ".sqlite", ".db"):
        return sqlite(str(path))
    if should_stream:
        from ifcopenshell.stream import stream

        return stream(str(path))

    lib = _get_lib()
    if not hasattr(lib, "ifcopenshell_file_open"):
        raise NotImplementedError("ifcopenshell_file_open is not exported by the native library")
    ptr = _open_bypass(path, bypass_types) if bypass_types else lib.ifcopenshell_file_open(_enc(str(path)))
    if not ptr:
        err = lib.ifcopenshell_last_error_message()
        msg = err.decode("utf-8") if err else "Unknown error"
        raise RuntimeError(f"Failed to open {path}: {msg}")
    return _wrap_file_ptr(ptr)
