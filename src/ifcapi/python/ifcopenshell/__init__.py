# SPDX-License-Identifier: LGPL-3.0-or-later

"""
Minimal ifcopenshell package backed by the native ifcapi C library.

This package provides ``file`` and ``entity_instance`` classes compatible with
the standard ifcopenshell API, using the native _ifcopenshell_capi extension.
"""

from __future__ import annotations

import builtins
import functools
import json
import re
import struct
import tempfile
import weakref
import zipfile
from pathlib import Path
from typing import Optional

from . import _ifcopenshell_capi as _capi
from . import guid, settings

version = "0.8.1"
version_core = version
guid.new = _capi.guid_new


def _take_instance_list(file_obj, instance_list):
    if not instance_list:
        return []
    try:
        size = _capi.instance_list_size(instance_list)
        return [entity_instance(file_obj, _capi.instance_list_get(instance_list, i)) for i in range(size)]
    finally:
        _capi.instance_list_destroy(instance_list)


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
    def __init__(self, message: str, transaction: Transaction):
        super().__init__(message)
        self.transaction = transaction


class Transaction:
    """Records create/edit/delete operations and replays them for undo/redo.

    Mirrors the upstream ``ifcopenshell.file.Transaction`` semantics so that
    file-level ``begin_transaction``/``end_transaction``/``undo``/``redo``
    behave identically to the SWIG-backed implementation.
    """

    def __init__(self, ifc_file: file):
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


def _instance_file_ptr(handle):
    if not handle:
        return None
    return _capi.instance_file_pointer(handle) or None


# ---------------------------------------------------------------------------
# Native typed-value helpers
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
        return "unknown"
    return "unknown"


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


# ---------------------------------------------------------------------------
# entity_instance
# ---------------------------------------------------------------------------

from ifcopenshell.entity_instance import (  # noqa: E402
    entity_instance,
    register_schema_attributes,
)

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
        self._handle = _capi.new_file(schema, 0, "")
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

    def assign_header_from(self, other: file) -> None:
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
        if getattr(self, "_handle", None) and getattr(self, "_owns_ptr", True):
            try:
                _capi.file_destroy(self._handle)
            except Exception:
                pass
        self._handle = None

    @property
    def schema_identifier(self) -> str:
        """Full IFC schema version: IFC2X3_TC1, IFC4_ADD2, IFC4X3_ADD2, etc."""
        return _capi.file_schema_name(self._handle) or ""

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
        if eid == -1 or eid is None:
            h = _capi.file_create_entity_by_name(self._handle, type_name)
        else:
            h = _capi.file_create_entity_by_name_with_id(self._handle, type_name, int(eid))
        entity = entity_instance(self, h)
        initializes_global_id = (
            (args and args[0] is not None and entity.attribute_name(0) == "GlobalId")
            or ("GlobalId" in kwargs and kwargs["GlobalId"] is not None)
        )
        # Suspend transaction recording while populating attributes — the
        # creation itself already captures the full attribute payload.
        active_transaction = self.transaction
        if args or kwargs:
            self.transaction = None
        try:
            if initializes_global_id:
                target_global_id = kwargs.get("GlobalId", args[0] if args else None)
                entity.GlobalId = ""
                get_log()
                if target_global_id:
                    try:
                        self.by_guid(target_global_id)
                        from ifcopenshell import ifcopenshell_wrapper as W

                        W._LOG_BUFFER.append(f"Overwriting existing entity for GlobalId {target_global_id}")
                    except RuntimeError:
                        pass
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
        fn = _capi.file_by_type if include_subtypes else _capi.file_by_type_excl_subtypes
        return [entity_instance(self, h) for h in fn(self._handle, type_name)]

    def by_id(self, id: int) -> entity_instance:
        if isinstance(id, (str, bytes)):
            return self.by_guid(id.decode("utf-8") if isinstance(id, bytes) else id)
        handle = _capi.file_by_id(self._handle, id)
        if not handle:
            raise RuntimeError(f"Entity #{id} not found")
        return entity_instance(self, handle)

    def __getattr__(self, attr):
        if attr.startswith("create"):
            import functools
            return functools.partial(self.create_entity, attr[6:])
        raise AttributeError(f"'{type(self).__name__}' object has no attribute '{attr}'")

    def by_guid(self, guid: str) -> entity_instance:
        if isinstance(guid, int):
            return self.by_id(guid)
        return entity_instance(self, _capi.file_by_guid(self._handle, guid))

    def storage_mode(self) -> int:
        return int(_capi.file_storage_mode(self._handle))

    def file_pointer(self) -> int:
        return int(_capi.file_file_pointer(self._handle)) if self._handle else 0

    def key_value_store_query(self, key: str) -> bytes:
        return _capi.file_key_value_store_query(self._handle, key) or b""

    def key_value_store_iter(self, prefix: str) -> tuple[bytes, ...]:
        return _capi.file_key_value_store_iter(self._handle, prefix) or ()

    @property
    def storage(self) -> Optional[rocksdb_file_storage]:
        return rocksdb_file_storage(self) if self.storage_mode() == 1 else None

    def remove(self, entity) -> None:
        if isinstance(entity, int):
            entity = self.by_id(entity)
        if not isinstance(entity, entity_instance):
            raise TypeError(f"Expected entity_instance or int, got {type(entity)}")
        if self.transaction is not None:
            self.transaction.store_delete(entity)
        _capi.file_remove(self._handle, entity._handle)

    def get_max_id(self) -> int:
        return int(_capi.file_get_max_id(self._handle))

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
        if inst.id() == 0 and inst._is_wrapped_value_instance():
            return self.create_entity(inst.is_a(), inst.wrappedValue)
        max_id = self.get_max_id() if self.transaction is not None else 0
        result = entity_instance(self, _capi.file_add(self._handle, inst._handle, -1 if _id is None else int(_id)))
        if self.transaction is not None:
            added = [e for e in self.traverse(result) if e.id() > max_id]
            for e in reversed(added):
                self.transaction.store_create(e)
        return result

    def get_inverse(self, entity, allow_duplicate=False, with_attribute_indices=False):
        if with_attribute_indices and not allow_duplicate:
            raise ValueError("with_attribute_indices requires allow_duplicate to be True")
        if not isinstance(entity, entity_instance):
            entity = self.by_id(entity)
        inverses = [entity_instance(self, h) for h in _capi.file_get_inverse(self._handle, entity._handle)]
        if allow_duplicate:
            if with_attribute_indices:
                idxs = list(_capi.file_get_inverse_indices(self._handle, entity._handle))
                return list(zip(inverses, idxs))
            return inverses
        return set(inverses)

    def get_total_inverses(self, entity) -> int:
        if not isinstance(entity, entity_instance):
            entity = self.by_id(entity)
        return int(_capi.file_get_total_inverses(self._handle, entity._handle))

    def traverse(self, entity, max_levels=None, breadth_first=False) -> list:
        if not isinstance(entity, entity_instance):
            entity = self.by_id(entity)
        ml = max_levels if max_levels is not None else -1
        fn = _capi.file_traverse_breadth_first if breadth_first else _capi.file_traverse
        return [entity_instance(self, h) for h in fn(self._handle, entity._handle, ml)]

    def write(self, path, format=None, zipped=False) -> None:
        path = Path(path)
        path.parent.mkdir(parents=True, exist_ok=True)
        if format is None:
            format = ".ifcZIP" if path.suffix.lower() in (".ifczip", ".zip") else ".ifc"
        if format == ".ifcXML":
            raise NotImplementedError("Writing .ifcXML files is not supported")
        if format == ".ifcZIP":
            return self.write(path, ".ifc", zipped=True)
        _capi.file_write(self._handle, str(path))
        if zipped:
            unzipped_path = path.with_suffix(format)
            path.rename(unzipped_path)
            with zipfile.ZipFile(path, "w") as zip_file:
                zip_file.write(unzipped_path, unzipped_path.name, compress_type=zipfile.ZIP_DEFLATED)
            unzipped_path.unlink()

    def to_string(self) -> str:
        return _capi.file_to_string(self._handle) or ""

    def __len__(self) -> int:
        return len(_capi.file_entity_names(self._handle))

    def entity_names(self) -> list:
        return [inst.id() for inst in self]

    @classmethod
    def from_string(cls, data) -> file:
        if isinstance(data, str):
            buf = data.encode("utf-8")
        elif isinstance(data, (bytes, bytearray)):
            buf = bytes(data)
        else:
            raise TypeError("from_string expects str or bytes")
        result = _wrap_file_ptr(_capi.read_memory(buf, len(buf)))
        result._buffer_keepalive = buf
        return result

    def __iter__(self):
        for entity_id in _capi.file_entity_names(self._handle):
            yield self.by_id(entity_id)

    def __getitem__(self, key):
        if isinstance(key, int):
            return self.by_id(key)
        if isinstance(key, (str, bytes)):
            return self.by_guid(key.decode("utf-8") if isinstance(key, bytes) else key)
        raise TypeError(f"Expected int or str key, got {type(key)}")

    def __contains__(self, entity):
        if isinstance(entity, entity_instance):
            try:
                entity.declaration()
            except RuntimeError:
                return False
            return True
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
        try:
            out = _capi.file_header_file_description(self._file._handle)
        except Exception:
            return _header_file_description()
        return entity_instance(self._file, out)

    @property
    def file_name(self):
        if self._file is None:
            return _header_file_name()
        try:
            out = _capi.file_header_file_name(self._file._handle)
        except Exception:
            return _header_file_name()
        return entity_instance(self._file, out)

    @property
    def file_schema(self):
        if self._file is None:
            return None
        try:
            out = _capi.file_header_file_schema(self._file._handle)
        except Exception:
            return None
        return entity_instance(self._file, out)


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


def register_schema(schema) -> None:
    """Register a custom schema object with the native schema registry."""
    from ifcopenshell import ifcopenshell_wrapper as _W

    wrapper_schema = getattr(schema, "schema", schema)
    if hasattr(wrapper_schema, "this"):
        raise TypeError("SWIG schema objects cannot be registered by the native ABI wrapper")
    if not isinstance(wrapper_schema, _W.schema_definition):
        raise TypeError("register_schema() expects a native schema_definition or an object with a .schema member")
    _W.register_schema(wrapper_schema)
    register_schema_attributes(wrapper_schema)


def _consume_binary_buffer(value: bytes, inner):
    while value:
        size = struct.unpack("@q", value[:8])[0]
        value = value[8:]
        yield inner(value[:size])
        value = value[size:]


@functools.cache
def _rocksdb_attribute_lookup(schema_name, entity_name):
    from ifcopenshell import ifcopenshell_wrapper as _W

    declaration = _W.schema_by_name(schema_name).declaration_by_name(entity_name)
    entity_decl = declaration.as_entity()
    attributes = entity_decl.all_attributes()
    lookup = {attribute.name(): index for index, attribute in enumerate(attributes)}
    for inverse in entity_decl.all_inverse_attributes():
        def visit(decl):
            yield decl.index_in_schema()
            for subtype in decl.subtypes():
                yield from visit(subtype)

        lookup[inverse.name()] = (
            tuple(visit(inverse.entity_reference())),
            inverse.entity_reference().attribute_index(inverse.attribute_reference().name()),
        )
    return lookup


@functools.cache
def _rocksdb_entity_name_lookup(schema_name, index):
    from ifcopenshell import ifcopenshell_wrapper as _W

    return _W.schema_by_name(schema_name).declarations()[index].name()


def _rocksdb_binary_deserializers():
    from ifcopenshell import ifcopenshell_wrapper as _W

    return (
        lambda __, _: None,
        lambda __, _: None,
        lambda __, value: struct.unpack("@i", value)[0],
        lambda __, value: value[0] == 1,
        lambda __, value: value[0] == 1,
        lambda __, value: struct.unpack("@d", value)[0],
        lambda __, value: value.decode("utf-8"),
        lambda __, value: value.decode("utf-8"),
        lambda storage, value: _W.schema_by_name(storage.schema_identifier)
        .declarations()[struct.unpack("@q", value[:8])[0]]
        .enumeration_items()[struct.unpack("@q", value[8:])[0]],
        lambda storage, value: storage.by_id((value[0] == 105, struct.unpack("@q", value[1:])[0])),
        lambda __, _: (),
        lambda __, value: struct.unpack("@" + "i" * (len(value) // 4), value),
        lambda __, value: struct.unpack("@" + "d" * (len(value) // 8), value),
        lambda __, value: tuple(_consume_binary_buffer(value, lambda inner: inner.decode("utf-8"))),
        lambda __, value: tuple(_consume_binary_buffer(value, lambda inner: inner.decode("utf-8"))),
        lambda storage, value: tuple(
            storage.by_id((value[i * 9] == 105, struct.unpack("@q", value[i * 9 + 1: i * 9 + 9])[0]))
            for i in range(len(value) // 9)
        ),
        lambda __, _: ((),),
        lambda __, value: tuple(
            _consume_binary_buffer(value, lambda inner: struct.unpack("@" + "i" * (len(inner) // 4), inner))
        ),
        lambda __, value: tuple(
            _consume_binary_buffer(value, lambda inner: struct.unpack("@" + "d" * (len(inner) // 8), inner))
        ),
        lambda storage, value: tuple(
            _consume_binary_buffer(
                value,
                lambda inner: tuple(
                    storage.by_id((inner[i * 9] == 105, struct.unpack("@q", inner[i * 9 + 1: i * 9 + 9])[0]))
                    for i in range(len(inner) // 9)
                ),
            )
        ),
    )


class rocksdb_lazy_instance:
    __slots__ = ("storage", "name")

    def __init__(self, storage, name):
        self.storage = storage
        self.name = name.decode("utf-8") if isinstance(name, bytes) else name

    def _transform_value(self, value: bytes):
        if not value:
            return None
        return _rocksdb_binary_deserializers()[value[0] - 65](self.storage, value[1:])

    @functools.cache
    def is_a(self):
        if self.name.startswith("h|"):
            return self.name[2:]
        index_data = self.storage.read(f"{self.name}|_")
        if not index_data:
            return None
        return _rocksdb_entity_name_lookup(self.storage.schema_identifier, struct.unpack("@q", index_data)[0])

    def __getattr__(self, name):
        attr = _rocksdb_attribute_lookup(self.storage.schema_identifier, self.is_a()).get(name)
        if isinstance(attr, int):
            return self[attr]
        if attr is None:
            raise AttributeError(name)

        entity_indices, attribute_index = attr

        def inverse_values():
            for index_in_schema in entity_indices:
                data = self.storage.read(f"v|{self.name[2:]}|{index_in_schema}|{attribute_index}") or b""
                yield from map(self.storage.by_id, struct.unpack("<" + "I" * (len(data) // 4), data))

        return list(inverse_values())

    def __getitem__(self, index):
        return self._transform_value(self.storage.read(f"{self.name}|{index}"))

    @functools.cache
    def __len__(self):
        indices = []
        for key, _ in self.storage.prefix(f"{self.name}|").items():
            if isinstance(key, bytes):
                parts = key.split(b"|")
            else:
                parts = key.split("|")
            if len(parts) > 2 and parts[2].isdigit():
                indices.append(int(parts[2]))
        return max(indices, default=-1) + 1

    def __iter__(self):
        for index in range(len(self)):
            yield self[index]

    def __repr__(self):
        prefix = f"#{self.name[2:]}=" if self.name.startswith("i|") else ""

        def value_repr(value):
            if isinstance(value, rocksdb_lazy_instance):
                return f"#{value.name[2:]}" if value.name.startswith("i|") else repr(value)
            if isinstance(value, (tuple, list)):
                return f"({','.join(map(value_repr, value))})"
            if value is None:
                return "$"
            return repr(value)

        return f"{prefix}{self.is_a()}({','.join(map(value_repr, self))})"

    def id(self):
        return int(self.name[2:]) if self.name.startswith("i|") else 0

    def __bool__(self):
        return len(self) > 0

    @property
    def _comparison_tuple(self):
        return self.storage.file.file_pointer(), self.name

    def __eq__(self, other):
        return isinstance(other, rocksdb_lazy_instance) and self._comparison_tuple == other._comparison_tuple

    def __hash__(self):
        return hash(self._comparison_tuple)


class rocksdb_file_storage:
    def __init__(self, file, prefix=""):
        self.file = file
        self._prefix = prefix

    @property
    def schema_identifier(self):
        return rocksdb_lazy_instance(self, "h|file_schema")[0][0]

    def read(self, key):
        return self.file.key_value_store_query(self._prefix + key)

    def items(self):
        for key in self.file.key_value_store_iter(self._prefix):
            yield key, self.file.key_value_store_query(key)

    def prefix(self, prefix):
        return rocksdb_file_storage(self.file, self._prefix + prefix)

    def by_id(self, name):
        if isinstance(name, tuple):
            instance = rocksdb_lazy_instance(self, f"{'i' if name[0] else 't'}|{name[1]}")
        else:
            instance = rocksdb_lazy_instance(self, f"i|{name}")
        return instance if instance else None

    def by_type(self, type_name):
        from ifcopenshell import ifcopenshell_wrapper as _W

        declaration = _W.schema_by_name(self.schema_identifier).declaration_by_name(type_name)
        if declaration is None:
            return
        data = self.read(f"t|{declaration.index_in_schema()}")
        for offset in range(0, len(data), 8):
            yield rocksdb_lazy_instance(self, f"i|{struct.unpack('<Q', data[offset: offset + 8])[0]}")


_SCRATCH_FILES: dict = {}
_BORROWED_FILES: dict[int, file] = {}
_STREAM_ATTR_TYPE_CACHE: dict = {}


def _scratch_file(schema: str) -> file:
    f = _SCRATCH_FILES.get(schema)
    if f is None:
        f = file(schema=schema)
        _SCRATCH_FILES[schema] = f
        # Register in _BORROWED_FILES so that _borrow_file_ptr can resolve
        # entities belonging to this scratch file.
        ptr = f.file_pointer()
        if ptr:
            _BORROWED_FILES[ptr] = f
        _register_scratch_with_native(schema, f)
    return f


def _register_scratch_with_native(schema: str, file_obj: file) -> None:
    """Register the Python scratch file with the C++ DERIVE/WHERE runtime
    so that entities produced by ``compute_derived`` belong to the same
    underlying ``ifcopenshell::file`` and remain addressable via
    ``by_id`` on the Python side."""
    raw_ptr = file_obj.file_pointer()
    if raw_ptr:
        try:
            _capi.register_scratch_file(schema, raw_ptr)
        except Exception:
            pass


def create_entity(type: str, schema: str = "IFC4", *args, **kwargs):
    """Create an IFC entity in a per-schema scratch file.

    Used by the auto-generated EXPRESS rules modules to construct intermediate
    geometric entities while computing DERIVE attributes."""
    return _scratch_file(schema).create_entity(type, *args, **kwargs)


def _wrap_file_ptr(ptr, *, owned=True, raw=False) -> file:
    f = file.__new__(file)
    f._handle = ptr
    f._owns_ptr = owned
    if not owned:
        ptr.owned = 0
    f.header = _file_header(f)
    f.transaction = None
    f.history = []
    f.future = []
    f.history_size = 64
    f.units = {}
    f.to_delete = None
    return f


def _borrow_file_ptr(ptr, fallback=None) -> file:
    if not ptr:
        return fallback
    ptr_value = ptr if isinstance(ptr, int) else int(getattr(ptr, "handle", 0) or 0)
    if fallback is not None:
        try:
            if getattr(fallback, "_handle", None) and fallback.file_pointer() == ptr_value:
                return fallback
        except Exception:
            pass
    borrowed = _BORROWED_FILES.get(ptr_value)
    if borrowed is None or borrowed.file_pointer() != ptr_value:
        try:
            borrowed = _wrap_file_ptr(ptr, owned=False)
        except Exception:
            return fallback
        _BORROWED_FILES[ptr_value] = borrowed
    return borrowed


class _InstanceStreamer:
    def __init__(self, ptr):
        self._handle = ptr

    @classmethod
    def create(cls):
        return cls(_capi.stream())

    @classmethod
    def create_from_path(cls, path, mmap=False):
        return cls(_capi.stream_from_path(str(path), bool(mmap)))

    @classmethod
    def from_string(cls, data):
        if isinstance(data, bytes):
            data = data.decode("utf-8")
        return cls(_capi.stream_from_string(data))

    def __del__(self):
        ptr = getattr(self, "_handle", None)
        if ptr:
            try:
                _capi.instance_streamer_destroy(ptr)
            except Exception:
                pass
            self._handle = None

    def pushPage(self, data):
        if isinstance(data, str):
            data = data.encode("utf-8")
        return _capi.instance_streamer_push_page(self._handle, data)

    def hasSemicolon(self):
        return bool(_capi.instance_streamer_has_semicolon(self._handle))

    def semicolonCount(self):
        return int(_capi.instance_streamer_semicolon_count(self._handle))

    def readInstancePy(self, type_as_declaration_instance=False):
        text = _capi.instance_streamer_read_instance_py(self._handle, bool(type_as_declaration_instance))
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
    try:
        declaration = schema.declaration_by_name(entity_name)
    except Exception:
        declaration = None
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
    return _capi.open_bypass(str(path), tuple(bypass_types))


from ifcopenshell.sql import sqlite, sqlite_entity  # noqa: E402


def guess_format(path):
    """Guess the canonical IFC format from a path, matching upstream."""
    path = Path(path)
    suffix = path.suffix.lower()
    if path.is_dir():
        return "rocksdb"
    if suffix == ".ifc":
        return ".ifc"
    if suffix in (".ifczip", ".zip"):
        return ".ifcZIP"
    if suffix in (".ifcxml", ".xml"):
        return ".ifcXML"
    if suffix in (".ifcjson", ".json"):
        return ".ifcJSON"
    if suffix in (".ifcsqlite", ".sqlite", ".db"):
        return ".ifcSQLite"
    return None


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

    if bypass_types:
        ptr = _open_bypass(path, bypass_types)
    else:
        ptr = _capi.open(str(path), readonly)
    return _wrap_file_ptr(ptr)


from ifcopenshell.stream import stream, stream_entity  # noqa: E402
