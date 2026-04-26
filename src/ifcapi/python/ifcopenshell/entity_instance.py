"""entity_instance class backed by the native ifcapi C library."""

from __future__ import annotations

import ctypes
from typing import List, Optional, Set, Tuple

from ifcopenshell import (
    _get_lib,
    _enc,
    ATTR_NULL,
    ATTR_STRING,
    ATTR_INT,
    ATTR_DOUBLE,
    ATTR_BOOL,
    ATTR_ENUM,
    ATTR_REFERENCE,
    ATTR_AGGREGATE,
    ATTR_DERIVED,
    ATTR_UNKNOWN,
)
from ifcopenshell._value_api import configure_value_lib, value_to_python


def _tv_class():
    """Lazily resolve ``ifcopenshell._typed_value`` to avoid the circular
    import at module load time (``_typed_value`` inherits from
    :class:`entity_instance` and is therefore defined *after* this module
    finishes loading)."""
    from ifcopenshell import _typed_value
    return _typed_value


def _typed_value_str(tv):
    """Serialise a ``_typed_value``'s wrapped payload into the canonical
    SPF representation expected by the C ``create_type_value`` helper.

    For aggregate payloads (``IfcLineIndex((1,2,3,4,1))`` etc.) we emit a
    parenthesised, comma-separated literal with no whitespace so that the
    C-side parser can dispatch unambiguously on the declared element type.
    Strings are kept bare; the C side strips a single layer of quotes.
    """
    w = tv._wrapped
    if w is None:
        return ""
    if isinstance(w, (list, tuple)):
        parts = []
        for item in w:
            if isinstance(item, bool):
                parts.append("true" if item else "false")
            elif isinstance(item, (int, float)):
                parts.append(repr(item))
            elif isinstance(item, str):
                parts.append("'" + item.replace("'", "''") + "'")
            else:
                parts.append(str(item))
        return "(" + ",".join(parts) + ")"
    if isinstance(w, bool):
        return "true" if w else "false"
    return str(w)


_derived_lib_configured = False
_attribute_value_lib_configured = False
_MISSING = object()

_ATTRIBUTE_VALUE_TYPES = {
    "NULL": ATTR_NULL,
    "DERIVED": ATTR_DERIVED,
    "INT": ATTR_INT,
    "BOOL": ATTR_BOOL,
    "LOGICAL": ATTR_BOOL,
    "DOUBLE": ATTR_DOUBLE,
    "STRING": ATTR_STRING,
    "ENUMERATION": ATTR_ENUM,
    "ENTITY INSTANCE": ATTR_REFERENCE,
    "AGGREGATE OF INT": ATTR_AGGREGATE,
    "AGGREGATE OF DOUBLE": ATTR_AGGREGATE,
    "AGGREGATE OF STRING": ATTR_AGGREGATE,
    "AGGREGATE OF BINARY": ATTR_AGGREGATE,
    "AGGREGATE OF ENTITY INSTANCE": ATTR_AGGREGATE,
    "AGGREGATE OF EMPTY AGGREGATE": ATTR_AGGREGATE,
    "AGGREGATE OF AGGREGATE OF INT": ATTR_AGGREGATE,
    "AGGREGATE OF AGGREGATE OF DOUBLE": ATTR_AGGREGATE,
    "AGGREGATE OF AGGREGATE OF ENTITY INSTANCE": ATTR_AGGREGATE,
}


def _instance_handle_ptr(handle):
    if not handle:
        return None
    from . import ifcopenshell_wrapper as W

    return ctypes.cast(ctypes.c_void_p(handle), W._HandleStructP)


def _generated_instance_handle_ptr(handle):
    if not handle:
        return None
    from ifcopenshell import _generated_capi

    return ctypes.cast(ctypes.c_void_p(handle), ctypes.POINTER(_generated_capi._HandleStruct))


def _instance_type_name(handle) -> str:
    from . import ifcopenshell_wrapper as W

    return W.instance_declaration(handle).name()


def _instance_to_string(handle, valid_spf: bool) -> str:
    from . import ifcopenshell_wrapper as W

    lib = _get_lib()
    W._bind()
    out = W.ifcopenshell_string_t()
    if not lib.ifcopenshell_ifc_instance_to_string(_instance_handle_ptr(handle), valid_spf, ctypes.byref(out)):
        return ""
    return W._take_string(out)


def _instance_attribute_names(handle) -> tuple[str, ...]:
    from . import ifcopenshell_wrapper as W

    lib = _get_lib()
    W._bind()
    out = W.ifcopenshell_string_list_t()
    if not lib.ifcopenshell_ifc_instance_get_attribute_names(_instance_handle_ptr(handle), ctypes.byref(out)):
        return ()
    return W._take_string_list(out)


def _instance_inverse_attribute_names(handle) -> tuple[str, ...]:
    from . import ifcopenshell_wrapper as W

    lib = _get_lib()
    W._bind()
    out = W.ifcopenshell_string_list_t()
    if not lib.ifcopenshell_ifc_instance_get_inverse_attribute_names(_instance_handle_ptr(handle), ctypes.byref(out)):
        return ()
    return W._take_string_list(out)


def _take_attribute_value_type(lib, attribute_value) -> str:
    from . import ifcopenshell_wrapper as W

    out = W.ifcopenshell_string_t()
    if not lib.ifcopenshell_ifcparse_attribute_value_type(attribute_value, ctypes.byref(out)):
        return ""
    return W._take_string(out)


def _attribute_type_from_value(lib, attribute_value) -> int:
    out = ctypes.c_bool(False)
    if lib.ifcopenshell_ifcparse_attribute_value_is_null(attribute_value, ctypes.byref(out)) and out.value:
        return ATTR_NULL
    return _ATTRIBUTE_VALUE_TYPES.get(_take_attribute_value_type(lib, attribute_value), ATTR_UNKNOWN)


def _make_string_list(values):
    from . import ifcopenshell_wrapper as W

    buffers = [ctypes.create_string_buffer(_enc(str(value))) for value in values]
    items = (W.ifcopenshell_string_t * len(buffers))()
    for i, buffer in enumerate(buffers):
        items[i].data = ctypes.cast(buffer, ctypes.c_void_p).value
        items[i].size = len(buffer.value)
        items[i].owned = False
    return W.ifcopenshell_string_list_t(items, len(buffers)), buffers


def register_schema_attributes(schema) -> None:
    """Validate a registered schema for upstream API parity.

    The native wrapper resolves setter/getter behaviour from schema declarations
    at the point of use, so it does not maintain SWIG's Python-side setter table.
    """
    if schema is None or not hasattr(schema, "declarations"):
        raise TypeError("register_schema_attributes() expects a schema_definition")
    tuple(schema.declarations())


class _Int32List(ctypes.Structure):
    _fields_ = [("items", ctypes.POINTER(ctypes.c_int32)), ("size", ctypes.c_size_t)]


class _Int32ListList(ctypes.Structure):
    _fields_ = [("items", ctypes.POINTER(_Int32List)), ("size", ctypes.c_size_t)]


class _DoubleList(ctypes.Structure):
    _fields_ = [("items", ctypes.POINTER(ctypes.c_double)), ("size", ctypes.c_size_t)]


def _configure_derived_lib(lib) -> None:
    """Bind ctypes signatures for the generated value/DERIVE facade."""
    global _derived_lib_configured
    if _derived_lib_configured:
        return
    configure_value_lib(lib)
    _derived_lib_configured = True


def _configure_attribute_value_lib(lib) -> None:
    """Bind ctypes signatures for the autogen AttributeValue ABI."""
    global _attribute_value_lib_configured
    if _attribute_value_lib_configured:
        return
    from . import ifcopenshell_wrapper as W

    W._bind()
    lib.ifcopenshell_ifc_instance_get_argument.restype = ctypes.c_bool
    lib.ifcopenshell_ifc_instance_get_argument.argtypes = [
        ctypes.c_void_p,
        ctypes.c_size_t,
        ctypes.POINTER(ctypes.c_void_p),
    ]
    lib.ifcopenshell_ifcparse_attribute_value_destroy.restype = None
    lib.ifcopenshell_ifcparse_attribute_value_destroy.argtypes = [ctypes.c_void_p]
    lib.ifcopenshell_ifcparse_attribute_value_is_null.restype = ctypes.c_bool
    lib.ifcopenshell_ifcparse_attribute_value_is_null.argtypes = [ctypes.c_void_p, ctypes.POINTER(ctypes.c_bool)]
    lib.ifcopenshell_ifcparse_attribute_value_type.restype = ctypes.c_bool
    lib.ifcopenshell_ifcparse_attribute_value_type.argtypes = [ctypes.c_void_p, ctypes.POINTER(W.ifcopenshell_string_t)]
    lib.ifcopenshell_ifcparse_attribute_value_as_int32.restype = ctypes.c_bool
    lib.ifcopenshell_ifcparse_attribute_value_as_int32.argtypes = [ctypes.c_void_p, ctypes.POINTER(ctypes.c_int32)]
    lib.ifcopenshell_ifcparse_attribute_value_as_bool.restype = ctypes.c_bool
    lib.ifcopenshell_ifcparse_attribute_value_as_bool.argtypes = [ctypes.c_void_p, ctypes.POINTER(ctypes.c_bool)]
    lib.ifcopenshell_ifcparse_attribute_value_as_double.restype = ctypes.c_bool
    lib.ifcopenshell_ifcparse_attribute_value_as_double.argtypes = [ctypes.c_void_p, ctypes.POINTER(ctypes.c_double)]
    lib.ifcopenshell_ifcparse_attribute_value_as_string.restype = ctypes.c_bool
    lib.ifcopenshell_ifcparse_attribute_value_as_string.argtypes = [
        ctypes.c_void_p,
        ctypes.POINTER(W.ifcopenshell_string_t),
    ]
    lib.ifcopenshell_ifcparse_attribute_value_as_enumeration_value.restype = ctypes.c_bool
    lib.ifcopenshell_ifcparse_attribute_value_as_enumeration_value.argtypes = [
        ctypes.c_void_p,
        ctypes.POINTER(W.ifcopenshell_string_t),
    ]
    lib.ifcopenshell_ifcparse_attribute_value_as_int32_list.restype = ctypes.c_bool
    lib.ifcopenshell_ifcparse_attribute_value_as_int32_list.argtypes = [
        ctypes.c_void_p,
        ctypes.POINTER(_Int32List),
    ]
    lib.ifcopenshell_ifcparse_attribute_value_as_int32_list_list.restype = ctypes.c_bool
    lib.ifcopenshell_ifcparse_attribute_value_as_int32_list_list.argtypes = [
        ctypes.c_void_p,
        ctypes.POINTER(_Int32ListList),
    ]
    lib.ifcopenshell_ifcparse_attribute_value_as_double_list.restype = ctypes.c_bool
    lib.ifcopenshell_ifcparse_attribute_value_as_double_list.argtypes = [
        ctypes.c_void_p,
        ctypes.POINTER(_DoubleList),
    ]
    lib.ifcopenshell_ifcparse_attribute_value_as_string_list.restype = ctypes.c_bool
    lib.ifcopenshell_ifcparse_attribute_value_as_string_list.argtypes = [
        ctypes.c_void_p,
        ctypes.POINTER(W.ifcopenshell_string_list_t),
    ]
    lib.ifcopenshell_ifcparse_attribute_value_as_instance_list.restype = ctypes.c_bool
    lib.ifcopenshell_ifcparse_attribute_value_as_instance_list.argtypes = [
        ctypes.c_void_p,
        ctypes.POINTER(W._HandleStructP),
    ]
    lib.ifcopenshell_ifcparse_attribute_value_as_instance.restype = ctypes.c_bool
    lib.ifcopenshell_ifcparse_attribute_value_as_instance.argtypes = [
        ctypes.c_void_p,
        ctypes.POINTER(W._HandleStructP),
    ]
    lib.ifcopenshell_ifcparse_instance_list_size.restype = ctypes.c_bool
    lib.ifcopenshell_ifcparse_instance_list_size.argtypes = [W._HandleStructP, ctypes.POINTER(ctypes.c_size_t)]
    lib.ifcopenshell_ifcparse_instance_list_get.restype = ctypes.c_bool
    lib.ifcopenshell_ifcparse_instance_list_get.argtypes = [
        W._HandleStructP,
        ctypes.c_size_t,
        ctypes.POINTER(W._HandleStructP),
    ]
    lib.ifcopenshell_ifcparse_instance_list_destroy.restype = None
    lib.ifcopenshell_ifcparse_instance_list_destroy.argtypes = [W._HandleStructP]
    lib.ifcopenshell_int32_list_destroy.restype = None
    lib.ifcopenshell_int32_list_destroy.argtypes = [ctypes.c_void_p]
    lib.ifcopenshell_int32_list_list_destroy.restype = None
    lib.ifcopenshell_int32_list_list_destroy.argtypes = [ctypes.POINTER(_Int32ListList)]
    lib.ifcopenshell_double_list_destroy.restype = None
    lib.ifcopenshell_double_list_destroy.argtypes = [ctypes.c_void_p]
    _attribute_value_lib_configured = True


def _should_unpack_inverse(file_obj: "file", type_name: str, attr_name: str) -> bool:
    try:
        import ifcopenshell.settings as settings
        if not settings.unpack_non_aggregate_inverses:
            return False
    except Exception:
        return False

    try:
        from . import ifcopenshell_wrapper as W

        schema = W.schema_by_name(file_obj.schema)
        ent = schema.declaration_by_name(type_name)
        inv = next(i for i in ent.all_inverse_attributes() if i.name() == attr_name)
        return (inv.bound1(), inv.bound2()) == (-1, -1)
    except Exception:
        return False


class entity_instance:
    """Wraps a native IFC entity identified by file pointer and STEP ID."""

    def __init__(self, file_obj: "file", handle):
        """Wrap a native ifcopenshell_ifc_instance_t* handle.

        ``handle`` is a c_void_p value (int) returned from a C API call.
        Ownership of the handle is transferred to this instance.
        """
        self._file = file_obj
        if isinstance(handle, ctypes.c_void_p):
            handle = handle.value
        self._handle = handle or 0

    def __del__(self):
        try:
            if self._handle:
                lib = _get_lib()
                lib.ifcopenshell_ifc_instance_destroy(_instance_handle_ptr(self._handle))
                self._handle = 0
        except Exception:
            pass

    @property
    def file(self) -> "file":
        return self._file

    @property
    def wrapped_data(self):
        """Compatibility shim for code expecting SWIG wrapped_data."""
        return getattr(self, "_wrapped_data", self)

    @wrapped_data.setter
    def wrapped_data(self, value):
        object.__setattr__(self, "_wrapped_data", value)

    def id(self) -> int:
        if not self._handle:
            return 0
        lib = _get_lib()
        out = ctypes.c_uint32(0)
        if lib.ifcopenshell_ifc_instance_id(_instance_handle_ptr(self._handle), ctypes.byref(out)):
            return int(out.value)
        return 0

    def identity(self) -> tuple:
        """Stable identity key for this instance (parity with SWIG wrapped_data.identity()).

        Returns a tuple unique per (file, instance) so it can be used as a
        dict key for caching, e.g. by ifcopenshell.api.project.append_asset.
        """
        return (id(self._file), self.id(), self.is_a())

    def is_a(self, type_name=None):
        # Inline typed values (``_typed_value`` subclass with ``_handle == 0``)
        # carry their declared type in ``_type_name``. ``entity_instance.is_a``
        # is sometimes called as an unbound function on these objects (mirrors
        # SWIG's ``entity_instance`` behaviour where inline values have
        # ``id() == 0``); Python 3 does not dispatch through the subclass for
        # such calls, so the fallback must live here.
        if not getattr(self, "_handle", 0) and hasattr(self, "_type_name"):
            tn = self._type_name
            if type_name is None:
                return tn
            if isinstance(type_name, bool) and type_name:
                schema = getattr(self._file, "schema", "")
                return f"{schema}.{tn}" if schema else tn
            return tn.lower() == type_name.lower()
        lib = _get_lib()
        if type_name is None:
            return _instance_type_name(self._handle)
        if isinstance(type_name, bool) and type_name:
            schema = self._file.schema
            name = _instance_type_name(self._handle)
            return f"{schema}.{name}" if name else ""
        from . import ifcopenshell_wrapper as W

        W._bind()
        out = ctypes.c_bool(False)
        if not lib.ifcopenshell_ifc_instance_is_a(
            _instance_handle_ptr(self._handle), _enc(type_name), ctypes.byref(out)
        ):
            return False
        return bool(out.value)

    def is_entity(self) -> bool:
        return True

    def declaration(self):
        """Return the schema declaration for this entity's type.

        Routes through the autogen ABI (`ifcopenshell_ifc_instance_declaration`)
        so header-section entities (which live in `Header_section_schema` rather
        than the file's IFC schema) are also resolvable.
        """
        from . import ifcopenshell_wrapper as W
        return W.instance_declaration(self._handle)

    def attribute_name(self, index: int) -> str:
        names = _instance_attribute_names(self._handle)
        return names[index] if 0 <= index < len(names) else ""

    def attribute_type(self, attr) -> str:
        lib = _get_lib()
        if isinstance(attr, int):
            attr = self.attribute_name(attr)
        _configure_attribute_value_lib(lib)
        av = ctypes.c_void_p()
        atype = ATTR_UNKNOWN
        idx = self._attr_index(attr)
        if idx >= 0 and lib.ifcopenshell_ifc_instance_get_argument(self._handle, idx, ctypes.byref(av)) and av.value:
            try:
                atype = _attribute_type_from_value(lib, av)
            finally:
                lib.ifcopenshell_ifcparse_attribute_value_destroy(av)
        _type_names = {
            ATTR_NULL: "NULL", ATTR_STRING: "STRING", ATTR_INT: "INT",
            ATTR_DOUBLE: "DOUBLE", ATTR_BOOL: "BOOL", ATTR_ENUM: "ENUMERATION",
            ATTR_REFERENCE: "ENTITY_INSTANCE", ATTR_AGGREGATE: "AGGREGATE",
            ATTR_DERIVED: "DERIVED",
        }
        return _type_names.get(atype, "UNKNOWN")

    def get_info(self, include_identifier=True, recursive=False, return_type=dict, ignore=(), scalar_only=False):
        lib = _get_lib()
        info = return_type()
        if include_identifier:
            info["id"] = self.id()
            info["type"] = self.is_a()
        for name in _instance_attribute_names(self._handle):
            if name in ignore:
                continue
            try:
                val = getattr(self, name)
            except (AttributeError, RuntimeError):
                val = None
            if scalar_only and isinstance(val, (entity_instance, list, tuple)):
                continue
            if recursive and isinstance(val, entity_instance):
                val = val.get_info(include_identifier, recursive, return_type, ignore, scalar_only)
            elif recursive and isinstance(val, (list, tuple)):
                val = entity_instance.walk(
                    lambda v: isinstance(v, entity_instance),
                    lambda v: v.get_info(include_identifier, recursive, return_type, ignore, scalar_only),
                    val,
                )
            info[name] = val
        return info

    def get_info_2(self, include_identifier=True, recursive=False, return_type=dict, ignore=()):
        assert recursive
        assert return_type is dict
        assert len(ignore) == 0
        result = self.get_info(include_identifier=True, recursive=True, return_type=dict)
        if include_identifier:
            return result

        def strip_id(value):
            if isinstance(value, dict):
                value.pop("id", None)
                for child in value.values():
                    strip_id(child)
            elif isinstance(value, (list, tuple)):
                for child in value:
                    strip_id(child)
            return value

        return strip_id(result)

    __dict__ = property(get_info)

    def to_string(self, valid_spf=True) -> str:
        return _instance_to_string(self._handle, valid_spf)

    def __len__(self) -> int:
        count = len(_instance_attribute_names(self._handle))
        if count == 0 and self._is_wrapped_value_instance():
            return 1
        return count

    def __getitem__(self, index: int):
        name = self.attribute_name(index)
        if not name:
            if index == 0 and self._is_wrapped_value_instance():
                return self.wrappedValue
            raise IndexError(f"Attribute index {index} out of range")
        return getattr(self, name)

    def __setitem__(self, index: int, value):
        name = self.attribute_name(index)
        if not name:
            raise IndexError(f"Attribute index {index} out of range")
        setattr(self, name, value)

    def _is_wrapped_value_instance(self) -> bool:
        if not self._handle:
            return False
        try:
            decl = self.declaration()
            return hasattr(decl, "declared_type") or hasattr(decl, "enumeration_items")
        except Exception:
            return False

    def _is_schema_derived(self, name: str) -> bool:
        try:
            entity = self.wrapped_data.declaration().as_entity()
            index = entity.attribute_index(name)
            derived = entity.derived()
            return 0 <= index < len(derived) and bool(derived[index])
        except Exception:
            return False

    def __getattr__(self, name):
        if name.startswith("_"):
            raise AttributeError(name)
        if name == "wrappedValue":
            wrapped = self._get_wrapped_value()
            if wrapped is not _MISSING:
                return wrapped
        lib = _get_lib()
        h = self._handle
        attr = _enc(name)

        # Try direct attribute first.
        if name in _instance_attribute_names(h):
            _configure_attribute_value_lib(lib)
            av = ctypes.c_void_p()
            atype = ATTR_UNKNOWN
            idx = self._attr_index(name)
            if lib.ifcopenshell_ifc_instance_get_argument(h, idx, ctypes.byref(av)) and av.value:
                try:
                    atype = _attribute_type_from_value(lib, av)
                    if atype == ATTR_NULL:
                        if self._is_schema_derived(name):
                            derived = self._get_derived(name)
                            if derived is not None:
                                return derived
                        return None
                    if atype == ATTR_AGGREGATE:
                        return self._get_aggregate(h, attr, name)
                    if atype == ATTR_REFERENCE:
                        typed_value = self._get_typed_value(name)
                        if typed_value is not None:
                            return typed_value
                        ref = self._attribute_value_to_python(lib, av, "entity")
                        if ref is not _MISSING:
                            return ref
                    elif atype != ATTR_AGGREGATE:
                        value = self._attribute_value_to_python(lib, av, "")
                        if value is not _MISSING:
                            return value
                finally:
                    lib.ifcopenshell_ifcparse_attribute_value_destroy(av)
            if atype == ATTR_DERIVED:
                from . import ifcopenshell_wrapper as W

                if W.get_feature("use_attribute_value_derived"):
                    return W.attribute_value_derived()
                return self._get_derived(name)
            return None

        # Try inverse attribute (e.g. IsDecomposedBy, Decomposes, ContainedInStructure).
        if name in _instance_inverse_attribute_names(h):
            from ifcopenshell import _take_instance_list
            from . import ifcopenshell_wrapper as W

            out = W._HandleStructP()
            if lib.ifcopenshell_ifc_instance_get_inverse_attribute_by_name(
                _instance_handle_ptr(h), _enc(attr), ctypes.byref(out)
            ):
                result = tuple(_take_instance_list(self._file, out))
                if result:
                    if _should_unpack_inverse(self._file, self.is_a(), name):
                        return result[0]
                    return result
            return tuple()

        # Try computed derived attributes before giving up
        derived = self._get_derived(name)
        if derived is not None:
            return derived

        raise AttributeError(f"Entity #{self.id()} has no attribute '{name}'")

    def _get_wrapped_value(self):
        """Return the sole payload of a native type-declaration instance."""
        if not self._handle:
            return _MISSING
        try:
            decl = self.declaration()
            if hasattr(decl, "enumeration_items"):
                primitive = "enum"
            else:
                td = decl if hasattr(decl, "declared_type") else decl.as_type_declaration()
                if td is None:
                    return _MISSING
                from ifcopenshell.util.attribute import get_primitive_type

                primitive = get_primitive_type(td.declared_type())
        except Exception:
            return _MISSING

        lib = _get_lib()
        _configure_attribute_value_lib(lib)
        av = ctypes.c_void_p()
        if not lib.ifcopenshell_ifc_instance_get_argument(self._handle, 0, ctypes.byref(av)) or not av.value:
            return _MISSING
        try:
            wrapped = self._attribute_value_to_python(lib, av, primitive)
            return None if wrapped is _MISSING else wrapped
        finally:
            lib.ifcopenshell_ifcparse_attribute_value_destroy(av)

    def _attribute_value_to_python(self, lib, av, primitive):
        from . import ifcopenshell_wrapper as W

        is_null = ctypes.c_bool(False)
        if lib.ifcopenshell_ifcparse_attribute_value_is_null(av, ctypes.byref(is_null)) and is_null.value:
            return None

        value_type = W.ifcopenshell_string_t()
        if lib.ifcopenshell_ifcparse_attribute_value_type(av, ctypes.byref(value_type)):
            value_type = W._take_string(value_type)
        else:
            value_type = ""

        def take_string(fn):
            s = W.ifcopenshell_string_t()
            if not fn(av, ctypes.byref(s)):
                return _MISSING
            return W._take_string(s)

        if value_type == "ENUMERATION":
            return take_string(lib.ifcopenshell_ifcparse_attribute_value_as_enumeration_value)
        if value_type in ("STRING", "BINARY"):
            val = take_string(lib.ifcopenshell_ifcparse_attribute_value_as_string)
            if val is not _MISSING:
                return val
        if primitive == "entity" or value_type == "ENTITY INSTANCE":
            from . import ifcopenshell_wrapper as W

            out = W._HandleStructP()
            if lib.ifcopenshell_ifcparse_attribute_value_as_instance(av, ctypes.byref(out)) and out:
                return entity_instance(self._file, ctypes.cast(out, ctypes.c_void_p).value)
        if primitive == "integer" or value_type == "INT":
            out = ctypes.c_int32(0)
            if lib.ifcopenshell_ifcparse_attribute_value_as_int32(av, ctypes.byref(out)):
                return int(out.value)
        if primitive == "boolean" or value_type in ("BOOL", "LOGICAL"):
            out = ctypes.c_bool(False)
            if lib.ifcopenshell_ifcparse_attribute_value_as_bool(av, ctypes.byref(out)):
                return bool(out.value)
        if primitive == "float" or value_type == "DOUBLE":
            out = ctypes.c_double(0.0)
            if lib.ifcopenshell_ifcparse_attribute_value_as_double(av, ctypes.byref(out)):
                return float(out.value)
        if primitive in ("string", "binary", "enum"):
            val = take_string(lib.ifcopenshell_ifcparse_attribute_value_as_string)
            if val is not _MISSING:
                return val
        if isinstance(primitive, tuple) and primitive[0] in ("list", "array", "set"):
            return self._attribute_value_list_to_python(lib, av, primitive[1], value_type)
        return _MISSING

    def _attribute_value_list_to_python(self, lib, av, primitive, value_type):
        from . import ifcopenshell_wrapper as W

        if primitive == "integer" or value_type == "AGGREGATE OF INT":
            out = _Int32List()
            if lib.ifcopenshell_ifcparse_attribute_value_as_int32_list(av, ctypes.byref(out)):
                try:
                    return tuple(int(out.items[i]) for i in range(out.size))
                finally:
                    lib.ifcopenshell_int32_list_destroy.argtypes = [ctypes.c_void_p]
                    lib.ifcopenshell_int32_list_destroy(ctypes.byref(out))
        if primitive == "float" or value_type == "AGGREGATE OF DOUBLE":
            out = _DoubleList()
            if lib.ifcopenshell_ifcparse_attribute_value_as_double_list(av, ctypes.byref(out)):
                try:
                    return tuple(float(out.items[i]) for i in range(out.size))
                finally:
                    lib.ifcopenshell_double_list_destroy.argtypes = [ctypes.c_void_p]
                    lib.ifcopenshell_double_list_destroy(ctypes.byref(out))
        if primitive == "entity" or value_type == "AGGREGATE OF ENTITY INSTANCE":
            out = W._HandleStructP()
            if lib.ifcopenshell_ifcparse_attribute_value_as_instance_list(av, ctypes.byref(out)) and out:
                try:
                    size = ctypes.c_size_t(0)
                    if not lib.ifcopenshell_ifcparse_instance_list_size(out, ctypes.byref(size)):
                        return _MISSING
                    result = []
                    for i in range(size.value):
                        item = W._HandleStructP()
                        if lib.ifcopenshell_ifcparse_instance_list_get(out, i, ctypes.byref(item)) and item:
                            result.append(entity_instance(self._file, ctypes.cast(item, ctypes.c_void_p).value))
                    return tuple(result)
                finally:
                    lib.ifcopenshell_ifcparse_instance_list_destroy(out)
        if primitive in ("string", "binary", "enum") or value_type in ("AGGREGATE OF STRING", "AGGREGATE OF BINARY"):
            out = W.ifcopenshell_string_list_t()
            if lib.ifcopenshell_ifcparse_attribute_value_as_string_list(av, ctypes.byref(out)):
                return W._take_string_list(out)
        return _MISSING

    def _get_derived(self, name):
        """Compute a DERIVE attribute via the native ``ifcopenshell_compute_derived``
        C ABI. Returns ``None`` when no rule is registered for this entity/attr
        or when the rule evaluates to INDETERMINATE."""
        if not self._handle:
            return None

        lib = _get_lib()
        _configure_derived_lib(lib)
        from ifcopenshell import _generated_capi

        ptr = ctypes.POINTER(_generated_capi._HandleStruct)()
        if not lib.ifcopenshell_ifcapi_compute_derived(
            _generated_instance_handle_ptr(self._handle), _enc(name), ctypes.byref(ptr)
        ):
            return None
        if not ptr:
            return None
        try:
            return value_to_python(lib, ptr, self)
        finally:
            lib.ifcopenshell_ifcapi_value_destroy(ptr)

    def _get_aggregate(self, h, attr, name):
        """Read an aggregate attribute, returning a Python tuple.

        Codec is selected from the schema-declared element type."""
        lib = _get_lib()

        pt = self._declared_attribute_primitive(name)
        elem_kind = None
        nested_double = False
        nested_int = False
        nested_entity = False
        if isinstance(pt, tuple) and len(pt) == 2:
            inner = pt[1]
            if isinstance(inner, tuple) and inner and inner[0] == "select":
                elem_kind = "select"
            elif isinstance(inner, tuple) and len(inner) == 2 and inner[1] == "float":
                nested_double = True
            elif isinstance(inner, tuple) and len(inner) == 2 and inner[1] == "integer":
                nested_int = True
            elif isinstance(inner, tuple) and len(inner) == 2 and inner[1] == "entity":
                nested_entity = True
            elif isinstance(inner, str):
                elem_kind = inner
        elif isinstance(pt, str):
            # Schema declares scalar but runtime is aggregate: caller stored
            # a list in a scalar attribute. Treat declared scalar as element type.
            elem_kind = pt
        elif isinstance(pt, tuple) and pt and pt[0] == "select":
            elem_kind = "select"

        def read_generated_list(primitive):
            _configure_attribute_value_lib(lib)
            av = ctypes.c_void_p()
            idx = self._attr_index(name)
            if idx < 0 or not lib.ifcopenshell_ifc_instance_get_argument(h, idx, ctypes.byref(av)) or not av.value:
                return _MISSING
            try:
                return self._attribute_value_list_to_python(lib, av, primitive, _take_attribute_value_type(lib, av))
            finally:
                lib.ifcopenshell_ifcparse_attribute_value_destroy(av)

        if elem_kind == "entity":
            result = read_generated_list("entity")
            return result if result is not _MISSING else tuple()

        if elem_kind == "float":
            result = read_generated_list("float")
            return result if result is not _MISSING else tuple()

        if elem_kind in ("integer", "boolean"):
            result = read_generated_list("integer")
            if result is _MISSING:
                return tuple()
            return tuple(bool(item) for item in result) if elem_kind == "boolean" else result

        if elem_kind in ("string", "enum", "binary"):
            result = read_generated_list(elem_kind)
            return result if result is not _MISSING else tuple()

        if nested_double:
            nested = self._get_aggregate_double_list_list(name)
            return nested if nested is not None else tuple()

        if nested_int:
            nested = self._get_aggregate_int_list_list(name)
            return nested if nested is not None else tuple()

        if nested_entity:
            nested = self._get_aggregate_ref_list_list(name)
            return nested if nested is not None else tuple()

        if elem_kind == "select":
            tv_result = self._get_aggregate_typed_value(h, attr)
            if tv_result is not None and len(tv_result) > 0:
                return tv_result
            entity_result = read_generated_list("entity")
            if entity_result is not _MISSING:
                return entity_result
            return tv_result if tv_result is not None else tuple()

        raise TypeError(f"Unsupported aggregate element type for '{name}': {pt!r}")

    def _get_aggregate_ref_list_list(self, name):
        lib = _get_lib()
        _configure_attribute_value_lib(lib)

        av = ctypes.c_void_p()
        idx = self._attr_index(name)
        if idx < 0 or not lib.ifcopenshell_ifc_instance_get_argument(self._handle, idx, ctypes.byref(av)) or not av.value:
            return None
        try:
            out = _Int32ListList()
            if not lib.ifcopenshell_ifcparse_attribute_value_as_instance_id_list_list(av, ctypes.byref(out)):
                return None
            try:
                return tuple(
                    tuple(self._file.by_id(int(out.items[i].items[j])) for j in range(out.items[i].size))
                    for i in range(out.size)
                )
            finally:
                lib.ifcopenshell_int32_list_list_destroy(ctypes.byref(out))
        finally:
            lib.ifcopenshell_ifcparse_attribute_value_destroy(av)

    def _get_typed_value(self, name):
        from . import ifcopenshell_wrapper as W

        lib = _get_lib()
        W._bind()
        out = W.ifcopenshell_string_list_t()
        if not lib.ifcopenshell_ifcapi_entity_get_typed_value(
            _instance_handle_ptr(self._handle), _enc(name), ctypes.byref(out)
        ):
            return None
        values = W._take_string_list(out)
        if len(values) < 2:
            return None
        return _tv_class()(self._file, values[0], values[1] if values[1] != "" else None)

    def _get_aggregate_int_list_list(self, name):
        """Read LIST OF LIST OF INTEGER via autogen attribute_value_as_int32_list_list."""
        lib = _get_lib()
        _configure_attribute_value_lib(lib)

        av = ctypes.c_void_p()
        idx = self._attr_index(name)
        if idx < 0 or not lib.ifcopenshell_ifc_instance_get_argument(self._handle, idx, ctypes.byref(av)) or not av.value:
            return None
        try:
            out = _Int32ListList()
            if not lib.ifcopenshell_ifcparse_attribute_value_as_int32_list_list(av, ctypes.byref(out)):
                return None
            try:
                return tuple(
                    tuple(int(out.items[i].items[j]) for j in range(out.items[i].size))
                    for i in range(out.size)
                )
            finally:
                lib.ifcopenshell_int32_list_list_destroy(ctypes.byref(out))
        finally:
            lib.ifcopenshell_ifcparse_attribute_value_destroy(av)

    def _get_aggregate_double_list_list(self, name):
        """Read LIST OF LIST OF REAL via autogen attribute_value_as_double_list_list."""
        lib = _get_lib()

        class _DL(ctypes.Structure):
            _fields_ = [("items", ctypes.POINTER(ctypes.c_double)), ("size", ctypes.c_size_t)]

        class _DLL(ctypes.Structure):
            _fields_ = [("items", ctypes.POINTER(_DL)), ("size", ctypes.c_size_t)]

        _configure_attribute_value_lib(lib)
        lib.ifcopenshell_ifcparse_attribute_value_as_double_list_list.restype = ctypes.c_bool
        lib.ifcopenshell_ifcparse_attribute_value_as_double_list_list.argtypes = [
            ctypes.c_void_p, ctypes.POINTER(_DLL),
        ]
        lib.ifcopenshell_double_list_list_destroy.restype = None
        lib.ifcopenshell_double_list_list_destroy.argtypes = [ctypes.POINTER(_DLL)]

        av = ctypes.c_void_p()
        idx = self._attr_index(name)
        if idx < 0 or not lib.ifcopenshell_ifc_instance_get_argument(self._handle, idx, ctypes.byref(av)) or not av.value:
            return None
        try:
            out = _DLL()
            if not lib.ifcopenshell_ifcparse_attribute_value_as_double_list_list(av, ctypes.byref(out)):
                return None
            try:
                result = tuple(
                    tuple(out.items[i].items[j] for j in range(out.items[i].size))
                    for i in range(out.size)
                )
                return result
            finally:
                lib.ifcopenshell_double_list_list_destroy(ctypes.byref(out))
        finally:
            lib.ifcopenshell_ifcparse_attribute_value_destroy(av)

    def _get_aggregate_typed_value(self, h, attr):
        """Try reading an aggregate of inline typed values."""
        from . import ifcopenshell_wrapper as W

        lib = _get_lib()
        W._bind()
        out = W.ifcopenshell_string_list_t()
        if not lib.ifcopenshell_ifcapi_entity_get_aggregate_typed_value(
            _instance_handle_ptr(h), attr, ctypes.byref(out)
        ):
            return None
        values = W._take_string_list(out)
        if len(values) < 2:
            return None
        result = [
            _tv_class()(self._file, values[i], values[i + 1] if values[i + 1] != "" else None)
            for i in range(0, len(values) - 1, 2)
        ]
        return tuple(result)

    def __setattr__(self, name, value):
        if name.startswith("_"):
            super().__setattr__(name, value)
            return
        lib = _get_lib()
        h = self._handle
        attr = _enc(name)

        if name not in _instance_attribute_names(h):
            super().__setattr__(name, value)
            return

        value = self._coerce_to_declared_type(name, value)
        _configure_attribute_value_lib(lib)
        idx = self._attr_index(name)
        handle = _instance_handle_ptr(h)

        # Record the edit before mutation so the transaction captures the
        # pre-image. Only entities that belong to a file (id != 0) are tracked.
        file_obj = getattr(self, "_file", None)
        transaction = getattr(file_obj, "transaction", None) if file_obj is not None else None
        if transaction is not None and self.id():
            if idx >= 0:
                try:
                    transaction.store_edit(self, idx, value)
                except Exception:
                    pass

        if value is None:
            lib.ifcopenshell_ifc_instance_unset_argument(handle, idx)
        elif isinstance(value, bool):
            lib.ifcopenshell_ifc_instance_set_argument_bool(handle, idx, value)
        elif isinstance(value, int):
            lib.ifcopenshell_ifc_instance_set_argument_int32(handle, idx, value)
        elif isinstance(value, float):
            lib.ifcopenshell_ifc_instance_set_argument_double(handle, idx, value)
        elif isinstance(value, _tv_class()):
            ok = ctypes.c_bool(False)
            if (
                not lib.ifcopenshell_ifcapi_entity_set_typed_value(
                handle,
                attr,
                _enc(value._type_name),
                _enc(_typed_value_str(value)) if value._wrapped is not None else None,
                    ctypes.byref(ok),
                )
                or not ok.value
            ):
                err = lib.ifcopenshell_last_error_message()
                raise RuntimeError(err.decode("utf-8", errors="replace") if err else f"Failed to set typed value '{name}'")
        elif isinstance(value, entity_instance):
            lib.ifcopenshell_ifc_instance_set_argument_instance(handle, idx, _instance_handle_ptr(value._handle))
        elif isinstance(value, str):
            lib.ifcopenshell_clear_error()
            was_enum = ctypes.c_bool(False)
            if not lib.ifcopenshell_ifc_instance_set_argument_enumeration_by_name(
                handle, idx, _enc(value), ctypes.byref(was_enum)
            ):
                err = lib.ifcopenshell_last_error_message()
                if err:
                    msg = err.decode("utf-8", errors="replace")
                    lib.ifcopenshell_clear_error()
                    raise RuntimeError(msg)
                raise RuntimeError(f"Failed to set attribute '{name}'")
            if not was_enum.value:
                lib.ifcopenshell_ifc_instance_set_argument_string(handle, idx, _enc(value))
        elif isinstance(value, (list, tuple)):
            self._set_aggregate(h, attr, name, value)
        else:
            raise TypeError(f"Cannot set attribute '{name}' to {type(value)}")

    def _coerce_to_declared_type(self, name, value):
        """Coerce Python values to the schema-declared attribute primitive.

        Mirrors SWIG's implicit coercion: e.g. passing int 123 for an
        IfcLabel attribute yields the string "123".
        """
        if value is None or isinstance(value, (list, tuple, entity_instance)):
            return value
        try:
            from ifcopenshell.util.attribute import get_primitive_type
            decl = self.declaration()
            for a in decl.all_attributes():
                if a.name() == name:
                    pt = get_primitive_type(a)
                    attr_type = str(a.type_of_attribute()).lower()
                    break
            else:
                return value
        except Exception:
            return value
        if isinstance(pt, tuple):  # aggregate
            return value
        if pt == "string":
            if isinstance(value, bool):
                return "TRUE" if value else "FALSE"
            if isinstance(value, (int, float)):
                return str(value)
            return value
        if pt == "integer":
            if isinstance(value, str):
                try:
                    return int(value)
                except ValueError:
                    return value
            if isinstance(value, float):
                if "<number>" in attr_type:
                    return value
                return int(value)
            return value
        if pt == "float":
            if isinstance(value, (int, str)):
                try:
                    return float(value)
                except (TypeError, ValueError):
                    return value
            return value
        if pt == "boolean":
            if isinstance(value, str):
                if value.upper() in ("TRUE", "T", ".T."):
                    return True
                if value.upper() in ("FALSE", "F", ".F."):
                    return False
            if isinstance(value, (int, float)):
                return bool(value)
            return value
        return value

    def _attr_index(self, name: str) -> int:
        for i, attribute_name in enumerate(_instance_attribute_names(self._handle)):
            if attribute_name == name:
                return i
        return -1

    def _declared_attribute_primitive(self, name):
        """Return the schema-declared primitive type for `name`.

        Returns either a leaf primitive ("string"/"float"/"integer"/"boolean"/
        "entity"/"enum"/"binary") or a tuple ("list"|"set"|"array"|"bag",
        <element-primitive>) for aggregates, or None when the schema lookup
        fails (caller should fall back to value-driven dispatch)."""
        try:
            from ifcopenshell.util.attribute import get_primitive_type
            decl = self.declaration()
            for a in decl.all_attributes():
                if a.name() == name:
                    return get_primitive_type(a)
        except Exception:
            return None
        return None

    def _set_aggregate(self, h, attr, name, items):
        """Write a Python list to an aggregate attribute.

        Codec is chosen from the schema-declared element type whenever
        possible; the value-driven path is only taken when no declaration
        is available (e.g. derived attributes)."""
        lib = _get_lib()

        pt = self._declared_attribute_primitive(name)
        elem_kind = None
        nested_double = False
        nested_int = False
        nested_entity = False
        if isinstance(pt, tuple) and len(pt) == 2:
            inner = pt[1]
            if isinstance(inner, tuple) and len(inner) == 2:
                if inner[1] == "float":
                    nested_double = True
                elif inner[1] == "integer":
                    nested_int = True
                elif inner[1] == "entity":
                    nested_entity = True
            elif isinstance(inner, str):
                elem_kind = inner
        elif isinstance(pt, str):
            elem_kind = pt

        def set_generated_instance_list(values):
            from ifcopenshell import _generated_capi

            idx = self._attr_index(name)
            if idx < 0:
                raise TypeError(f"Unknown attribute '{name}'")
            handle_ptrs = None
            if values:
                handle_ptrs = (ctypes.POINTER(_generated_capi._HandleStruct) * len(values))()
                for i, value in enumerate(values):
                    if not isinstance(value, entity_instance):
                        raise TypeError(f"Cannot set aggregate '{name}' with {type(value)}")
                    handle_ptrs[i] = _generated_instance_handle_ptr(value._handle)
            input_list = _generated_capi.ifcopenshell_ifc_instance_list_t(handle_ptrs, len(values))
            instance_list = ctypes.POINTER(_generated_capi._HandleStruct)()
            if not lib.ifcopenshell_ifcparse_instance_list_create_from_handles(
                ctypes.byref(input_list), ctypes.byref(instance_list)
            ):
                err = lib.ifcopenshell_last_error_message()
                raise RuntimeError(err.decode("utf-8", errors="replace") if err else f"Failed to create aggregate '{name}'")
            try:
                if not lib.ifcopenshell_ifc_instance_set_argument_instance_list(
                    _generated_instance_handle_ptr(h), idx, instance_list
                ):
                    err = lib.ifcopenshell_last_error_message()
                    raise RuntimeError(err.decode("utf-8", errors="replace") if err else f"Failed to set aggregate '{name}'")
            finally:
                lib.ifcopenshell_ifcparse_instance_list_destroy(instance_list)

        first = items[0] if items else None
        if nested_double or nested_int:
            idx = self._attr_index(name)
            if idx < 0:
                raise TypeError(f"Unknown attribute '{name}'")
            c_elem = ctypes.c_double if nested_double else ctypes.c_int32
            py_cast = float if nested_double else int
            class _L(ctypes.Structure):
                _fields_ = [("items", ctypes.POINTER(c_elem)), ("size", ctypes.c_size_t)]
            class _LL(ctypes.Structure):
                _fields_ = [("items", ctypes.POINTER(_L)), ("size", ctypes.c_size_t)]
            inner_arrays = []
            l_items = (_L * len(items))()
            for i, row in enumerate(items):
                row_vals = [py_cast(v) for v in row]
                buf = (c_elem * len(row_vals))(*row_vals)
                inner_arrays.append(buf)
                l_items[i].items = ctypes.cast(buf, ctypes.POINTER(c_elem))
                l_items[i].size = len(row_vals)
            ll = _LL()
            ll.items = l_items
            ll.size = len(items)
            sym = (
                lib.ifcopenshell_ifc_instance_set_argument_double_list_list
                if nested_double
                else lib.ifcopenshell_ifc_instance_set_argument_int32_list_list
            )
            sym.restype = ctypes.c_bool
            sym.argtypes = [ctypes.c_void_p, ctypes.c_size_t, ctypes.POINTER(_LL)]
            ok = sym(h, idx, ctypes.byref(ll))
            if not ok:
                raise RuntimeError(f"Failed to set nested aggregate '{name}'")
            return

        if nested_entity:
            idx = self._attr_index(name)
            if idx < 0:
                raise TypeError(f"Unknown attribute '{name}'")
            inner_arrays = []
            l_items = (_Int32List * len(items))()
            for i, row in enumerate(items):
                ids = []
                for v in row:
                    if not isinstance(v, entity_instance):
                        raise TypeError(f"Cannot set nested aggregate '{name}' with {type(v)}")
                    ids.append(v.id())
                buf = (ctypes.c_int32 * len(ids))(*ids)
                inner_arrays.append(buf)
                l_items[i].items = ctypes.cast(buf, ctypes.POINTER(ctypes.c_int32))
                l_items[i].size = len(ids)
            ll = _Int32ListList()
            ll.items = l_items
            ll.size = len(items)
            lib.ifcopenshell_ifc_instance_set_argument_as_aggregate_of_aggregate_of_entity_instance.restype = ctypes.c_bool
            lib.ifcopenshell_ifc_instance_set_argument_as_aggregate_of_aggregate_of_entity_instance.argtypes = [
                ctypes.c_void_p,
                ctypes.c_size_t,
                ctypes.POINTER(_Int32ListList),
            ]
            ok = lib.ifcopenshell_ifc_instance_set_argument_as_aggregate_of_aggregate_of_entity_instance(
                h, idx, ctypes.byref(ll)
            )
            if not ok:
                err = lib.ifcopenshell_last_error_message()
                raise RuntimeError(err.decode("utf-8", errors="replace") if err else f"Failed to set nested aggregate '{name}'")
            return

        if isinstance(first, _tv_class()):
            from . import ifcopenshell_wrapper as W

            W._bind()
            type_names, type_keepalive = _make_string_list(v._type_name for v in items)
            str_vals, str_keepalive = _make_string_list(
                _typed_value_str(v) if v._wrapped is not None else "" for v in items
            )
            ok = ctypes.c_bool(False)
            if (
                not lib.ifcopenshell_ifcapi_entity_set_aggregate_typed_value(
                    _instance_handle_ptr(h), attr, ctypes.byref(type_names), ctypes.byref(str_vals), ctypes.byref(ok)
                )
                or not ok.value
            ):
                err = lib.ifcopenshell_last_error_message()
                raise RuntimeError(err.decode("utf-8", errors="replace") if err else f"Failed to set aggregate '{name}'")
            return
        if isinstance(first, entity_instance):
            set_generated_instance_list(items)
            return

        idx = self._attr_index(name)
        handle = _instance_handle_ptr(h)
        _configure_attribute_value_lib(lib)
        if elem_kind == "entity":
            set_generated_instance_list(items)
        elif elem_kind == "float":
            arr = (ctypes.c_double * len(items))(*[float(v) for v in items])
            from ifcopenshell import _generated_capi

            values = _generated_capi.ifcopenshell_double_list_t(arr, len(items))
            if not lib.ifcopenshell_ifc_instance_set_argument_double_list(handle, idx, ctypes.byref(values)):
                raise RuntimeError(f"Failed to set aggregate '{name}'")
        elif elem_kind == "integer":
            arr = (ctypes.c_int32 * len(items))(*[int(v) for v in items])
            from ifcopenshell import _generated_capi

            values = _generated_capi.ifcopenshell_int32_list_t(arr, len(items))
            if not lib.ifcopenshell_ifc_instance_set_argument_int32_list(handle, idx, ctypes.byref(values)):
                raise RuntimeError(f"Failed to set aggregate '{name}'")
        elif elem_kind == "boolean":
            arr = (ctypes.c_int32 * len(items))(*[int(bool(v)) for v in items])
            from ifcopenshell import _generated_capi

            values = _generated_capi.ifcopenshell_int32_list_t(arr, len(items))
            if not lib.ifcopenshell_ifc_instance_set_argument_int32_list(handle, idx, ctypes.byref(values)):
                raise RuntimeError(f"Failed to set aggregate '{name}'")
        elif elem_kind in ("string", "enum", "binary"):
            from . import ifcopenshell_wrapper as W

            keepalive = [ctypes.create_string_buffer(_enc(str(s))) for s in items]
            arr = (W.ifcopenshell_string_t * len(items))()
            for i, buffer in enumerate(keepalive):
                arr[i].data = ctypes.cast(buffer, ctypes.c_void_p).value
                arr[i].size = len(buffer.value)
                arr[i].owned = False
            values = W.ifcopenshell_string_list_t(arr, len(items))
            if not lib.ifcopenshell_ifc_instance_set_argument_string_list(handle, idx, ctypes.byref(values)):
                raise RuntimeError(f"Failed to set aggregate '{name}'")
        else:
            raise TypeError(f"Cannot set aggregate '{name}' with declared primitive {pt!r}")

    def __repr__(self):
        spf = _instance_to_string(self._handle, False)
        if spf:
            return spf
        return f"#{self.id()}=???"

    def __eq__(self, other):
        if isinstance(other, entity_instance):
            return self._file is other._file and self.id() == other.id()
        return NotImplemented

    def __hash__(self):
        return hash((id(self._file), self.id()))

    @staticmethod
    def walk(f, g, value):
        if f(value):
            return g(value)
        if isinstance(value, (list, tuple)):
            return type(value)(entity_instance.walk(f, g, v) for v in value)
        return value

    @staticmethod
    def unwrap_value(v):
        if isinstance(v, entity_instance):
            return v
        if isinstance(v, (list, tuple)):
            return type(v)(entity_instance.unwrap_value(i) for i in v)
        return v

    @staticmethod
    def wrap_value(v, file_obj):
        if isinstance(v, entity_instance):
            return v
        if isinstance(v, (list, tuple)):
            return type(v)(entity_instance.wrap_value(i, file_obj) for i in v)
        return v
