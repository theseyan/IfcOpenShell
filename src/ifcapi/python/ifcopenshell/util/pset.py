# SPDX-License-Identifier: LGPL-3.0-or-later

"""Property set / quantity template lookup, backed by the native ifcapi C library."""

import ctypes
import pathlib
from functools import lru_cache
from typing import Literal, NamedTuple, Optional, Union

import ifcopenshell
import ifcopenshell.util.schema
from ifcopenshell.entity_instance import entity_instance


_lib_configured = False
_template_dir_initialized = False


def _configure_lib(lib) -> None:
    global _lib_configured
    if _lib_configured:
        return
    lib.ifcopenshell_free_string_array.restype = None
    lib.ifcopenshell_free_string_array.argtypes = [ctypes.POINTER(ctypes.c_char_p), ctypes.c_uint32]
    lib.ifcopenshell_free_instance_array.restype = None
    lib.ifcopenshell_free_instance_array.argtypes = [ctypes.POINTER(ctypes.c_void_p), ctypes.c_uint32]
    _lib_configured = True


def _resolve_template_dir() -> str:
    local = pathlib.Path(__file__).parent / "schema"
    if local.is_dir() and any(local.glob("Pset_IFC*.ifc")):
        return str(local.absolute())
    try:
        import ifcopenshell as _ios
        if hasattr(_ios, "util") and hasattr(_ios.util, "__file__"):
            upstream = pathlib.Path(_ios.util.__file__).parent / "schema"
            if upstream.is_dir():
                return str(upstream.absolute())
    except Exception:
        pass
    return str(local.absolute())


def _ensure_template_dir(lib) -> None:
    global _template_dir_initialized
    if _template_dir_initialized:
        return
    lib.ifcopenshell_util_pset_set_template_dir(_resolve_template_dir().encode("utf-8"))
    _template_dir_initialized = True


templates: dict[ifcopenshell.util.schema.IFC_SCHEMA, "PsetQto"] = {}


def get_template(schema_identiier: str) -> "PsetQto":
    """:param schema_identiier: As in ``file.schema_identifier``, not ``file.schema``."""
    global templates
    schema = ifcopenshell.util.schema.get_fallback_schema(schema_identiier)
    if schema not in templates:
        templates[schema] = PsetQto(schema)
    return templates[schema]


class PsetQto:
    """Thin wrapper around the native PsetQto template handle."""

    templates_path: dict[ifcopenshell.util.schema.IFC_SCHEMA, str] = {
        "IFC2X3": "Pset_IFC2X3.ifc",
        "IFC4": "Pset_IFC4_ADD2.ifc",
        "IFC4X3": "Pset_IFC4X3.ifc",
    }

    def __init__(
        self,
        schema: ifcopenshell.util.schema.IFC_SCHEMA,
        templates: Optional[list] = None,
    ) -> None:
        self._schema_id = schema
        self.schema = ifcopenshell.schema_by_name(schema)
        lib = ifcopenshell._get_lib()
        _configure_lib(lib)
        _ensure_template_dir(lib)
        self._handle = lib.ifcopenshell_util_pset_get_template(schema.encode("utf-8"))
        if not self._handle:
            err = lib.ifcopenshell_last_error_message()
            msg = err.decode("utf-8") if err else "Unknown error"
            raise RuntimeError(f"Failed to load PSD templates for {schema}: {msg}")

    @lru_cache
    def get_by_name(self, name: str) -> Optional[entity_instance]:
        if not name:
            return None
        lib = ifcopenshell._get_lib()
        h = lib.ifcopenshell_util_pset_template_get_by_name(self._handle, name.encode("utf-8"))
        if not h:
            return None
        return entity_instance(None, h)

    def is_templated(self, name: str) -> bool:
        if not name:
            return False
        lib = ifcopenshell._get_lib()
        return bool(lib.ifcopenshell_util_pset_template_is_templated(self._handle, name.encode("utf-8")))

    @lru_cache
    def get_applicable(
        self,
        ifc_class: str = "",
        predefined_type: str = "",
        pset_only: bool = False,
        qto_only: bool = False,
        schema: ifcopenshell.util.schema.IFC_SCHEMA = "IFC4",
    ) -> list[entity_instance]:
        """Get applicable property set templates."""
        lib = ifcopenshell._get_lib()
        _configure_lib(lib)
        count = ctypes.c_uint32(0)
        arr = lib.ifcopenshell_util_pset_template_get_applicable(
            self._handle,
            (ifc_class or "").encode("utf-8"),
            (predefined_type or "").encode("utf-8"),
            bool(pset_only), bool(qto_only),
            (schema or "IFC4").encode("utf-8"),
            ctypes.byref(count),
        )
        if not arr or count.value == 0:
            return []
        try:
            return [entity_instance(None, arr[i]) for i in range(count.value)]
        finally:
            lib.ifcopenshell_free_instance_array_only(arr)

    @lru_cache
    def get_applicable_names(
        self,
        ifc_class: str,
        predefined_type: str = "",
        pset_only: bool = False,
        qto_only: bool = False,
        schema: ifcopenshell.util.schema.IFC_SCHEMA = "IFC4",
    ) -> list[str]:
        """Return names instead of objects."""
        lib = ifcopenshell._get_lib()
        _configure_lib(lib)
        count = ctypes.c_uint32(0)
        arr = lib.ifcopenshell_util_pset_template_get_applicable_names(
            self._handle,
            (ifc_class or "").encode("utf-8"),
            (predefined_type or "").encode("utf-8"),
            bool(pset_only), bool(qto_only),
            (schema or "IFC4").encode("utf-8"),
            ctypes.byref(count),
        )
        if not arr or count.value == 0:
            return []
        try:
            return [arr[i].decode("utf-8") for i in range(count.value)]
        finally:
            lib.ifcopenshell_free_string_array(arr, count.value)


def get_pset_template_type(pset_template: entity_instance) -> Literal["PSET", "QTO", None]:
    """Get the type of the pset template.

    If type is mixed or not defined, return None.
    """
    lib = ifcopenshell._get_lib()
    res = lib.ifcopenshell_util_pset_template_pset_type(pset_template._handle)
    if not res:
        return None
    return res.decode("ascii")


class ApplicableEntity(NamedTuple):
    value: str
    ifc_class: str
    predefined_type: Union[str, None]
    performance_history: bool


def parse_applicable_entity(applicable_entity: str) -> list[ApplicableEntity]:
    """Parse ApplicableEntity string query to tuples."""
    items: list[ApplicableEntity] = []
    for item in applicable_entity.split(","):
        value = item
        item, predefined_type = parts if len(parts := item.split("/")) > 1 else (item, None)
        ifc_class, performance_history = (parts[0], True) if len(parts := item.split("[")) > 1 else (item, False)
        items.append(ApplicableEntity(value, ifc_class, predefined_type, performance_history))
    return items


def convert_applicable_entities_to_query(applicable_entities: list[ApplicableEntity]) -> str:
    """Get query supported by :func:`ifcopenshell.util.selector.filter_elements`."""
    parts: list[str] = []
    for entity in applicable_entities:
        part = entity.ifc_class
        if entity.predefined_type:
            part += f', PredefinedType="{entity.predefined_type}"'
        parts.append(part)
    return " + ".join(parts)
