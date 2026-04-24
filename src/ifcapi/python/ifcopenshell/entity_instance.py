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


def _configure_derived_lib(lib) -> None:
    """Bind ctypes signatures for the native DERIVE rule dispatcher."""
    global _derived_lib_configured
    if _derived_lib_configured:
        return
    configure_value_lib(lib)
    lib.ifcopenshell_compute_derived.restype = ctypes.c_void_p
    lib.ifcopenshell_compute_derived.argtypes = [ctypes.c_void_p, ctypes.c_char_p]
    _derived_lib_configured = True


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
                lib.ifcopenshell_ifc_instance_destroy(self._handle)
                self._handle = 0
        except Exception:
            pass

    @property
    def file(self) -> "file":
        return self._file

    @property
    def wrapped_data(self):
        """Compatibility shim for code expecting SWIG wrapped_data."""
        return self

    def id(self) -> int:
        if not self._handle:
            return 0
        lib = _get_lib()
        out = ctypes.c_uint32(0)
        if lib.ifcopenshell_ifc_instance_id(self._handle, ctypes.byref(out)):
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
            result = lib.ifcopenshell_entity_type(self._handle)
            return result.decode("utf-8") if result else ""
        if isinstance(type_name, bool) and type_name:
            schema = self._file.schema
            name = lib.ifcopenshell_entity_type(self._handle)
            return f"{schema}.{name.decode('utf-8')}" if name else ""
        return lib.ifcopenshell_entity_is_a(self._handle, _enc(type_name))

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
        lib = _get_lib()
        val = lib.ifcopenshell_entity_attr_name(self._handle, index)
        return val.decode("utf-8") if val else ""

    def attribute_type(self, attr) -> str:
        lib = _get_lib()
        if isinstance(attr, int):
            attr = self.attribute_name(attr)
        atype = lib.ifcopenshell_entity_attr_type(self._handle, _enc(attr))
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
        n = lib.ifcopenshell_entity_attr_count(self._handle)
        for i in range(n):
            name = self.attribute_name(i)
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
                val = type(val)(
                    v.get_info(include_identifier, recursive, return_type, ignore, scalar_only)
                    if isinstance(v, entity_instance) else v
                    for v in val
                )
            info[name] = val
        return info

    def to_string(self, valid_spf=True) -> str:
        lib = _get_lib()
        val = lib.ifcopenshell_entity_to_string(self._handle)
        return val.decode("utf-8") if val else ""

    def __len__(self) -> int:
        lib = _get_lib()
        return lib.ifcopenshell_entity_attr_count(self._handle)

    def __getitem__(self, index: int):
        name = self.attribute_name(index)
        if not name:
            raise IndexError(f"Attribute index {index} out of range")
        return getattr(self, name)

    def __setitem__(self, index: int, value):
        name = self.attribute_name(index)
        if not name:
            raise IndexError(f"Attribute index {index} out of range")
        setattr(self, name, value)

    def __getattr__(self, name):
        if name.startswith("_"):
            raise AttributeError(name)
        lib = _get_lib()
        h = self._handle
        attr = _enc(name)

        # Try direct attribute first.
        if lib.ifcopenshell_entity_has_attr(h, attr):
            atype = lib.ifcopenshell_entity_attr_type(h, attr)

            if atype == ATTR_NULL:
                return None
            elif atype == ATTR_STRING:
                val = lib.ifcopenshell_entity_get_string(h, attr)
                return val.decode("utf-8") if val is not None else None
            elif atype == ATTR_INT:
                return lib.ifcopenshell_entity_get_int(h, attr)
            elif atype == ATTR_DOUBLE:
                return lib.ifcopenshell_entity_get_double(h, attr)
            elif atype == ATTR_BOOL:
                val = lib.ifcopenshell_entity_get_bool(h, attr)
                return None if val < 0 else bool(val)
            elif atype == ATTR_ENUM:
                val = lib.ifcopenshell_entity_get_enum(h, attr)
                return val.decode("utf-8") if val else None
            elif atype == ATTR_REFERENCE:
                # Check for inline typed value (IfcLabel, IfcReal, etc.) first,
                # as the C layer represents those as a reference to a type
                # declaration entity which we want surfaced as _typed_value.
                value_out = ctypes.c_char_p()
                type_name = lib.ifcopenshell_entity_get_typed_value(
                    h, attr, ctypes.byref(value_out))
                if type_name:
                    return _tv_class()(
                        self._file, type_name.decode("utf-8"),
                        value_out.value.decode("utf-8") if value_out.value else None)
                ref_h = lib.ifcopenshell_entity_get_reference(h, attr)
                if ref_h:
                    return entity_instance(self._file, ref_h)
                return None
            elif atype == ATTR_AGGREGATE:
                return self._get_aggregate(h, attr, name)
            elif atype == ATTR_DERIVED:
                return self._get_derived(name)
            return None

        # Try inverse attribute (e.g. IsDecomposedBy, Decomposes, ContainedInStructure).
        if lib.ifcopenshell_entity_is_inverse_attr(h, attr):
            count = ctypes.c_uint32(0)
            arr = lib.ifcopenshell_entity_get_inverse_attr(h, attr, ctypes.byref(count))
            if arr and count.value > 0:
                result = tuple(entity_instance(self._file, arr[i]) for i in range(count.value))
                lib.ifcopenshell_free_instance_array_only(arr)
                if _should_unpack_inverse(self._file, self.is_a(), name):
                    return result[0]
                return result
            return tuple()

        # Try computed derived attributes before giving up
        derived = self._get_derived(name)
        if derived is not None:
            return derived

        raise AttributeError(f"Entity #{self.id()} has no attribute '{name}'")

    def _get_derived(self, name):
        """Compute a DERIVE attribute via the native ``ifcopenshell_compute_derived``
        C ABI. Returns ``None`` when no rule is registered for this entity/attr
        or when the rule evaluates to INDETERMINATE."""
        if not self._handle:
            return None

        lib = _get_lib()
        _configure_derived_lib(lib)
        ptr = lib.ifcopenshell_compute_derived(self._handle, _enc(name))
        if not ptr:
            return None
        try:
            return value_to_python(lib, ptr, self)
        finally:
            lib.ifcopenshell_value_free(ptr)

    def _get_aggregate(self, h, attr, name):
        """Read an aggregate attribute, returning a Python tuple.

        Codec is selected from the schema-declared element type."""
        lib = _get_lib()

        pt = self._declared_attribute_primitive(name)
        elem_kind = None
        nested_double = False
        if isinstance(pt, tuple) and len(pt) == 2:
            inner = pt[1]
            if isinstance(inner, tuple) and len(inner) == 2 and inner[1] == "float":
                nested_double = True
            elif isinstance(inner, str):
                elem_kind = inner
            elif isinstance(inner, tuple) and inner and inner[0] == "select":
                elem_kind = "select"
        elif isinstance(pt, str):
            # Schema declares scalar but runtime is aggregate: caller stored
            # a list in a scalar attribute. Treat declared scalar as element type.
            elem_kind = pt
        elif isinstance(pt, tuple) and pt and pt[0] == "select":
            elem_kind = "select"

        if elem_kind == "entity":
            count = ctypes.c_uint32(0)
            arr = lib.ifcopenshell_entity_get_aggregate_ref(h, attr, ctypes.byref(count))
            if arr and count.value > 0:
                result = tuple(entity_instance(self._file, arr[i]) for i in range(count.value))
                lib.ifcopenshell_free_instance_array_only(arr)
                return result
            if arr:
                lib.ifcopenshell_free_instance_array(arr, count.value)
            return tuple()

        if elem_kind == "float":
            count = ctypes.c_uint32(0)
            darr = lib.ifcopenshell_entity_get_aggregate_double(h, attr, ctypes.byref(count))
            if darr and count.value > 0:
                result = tuple(darr[i] for i in range(count.value))
                lib.ifcopenshell_free_double_array(darr)
                return result
            return tuple()

        if elem_kind in ("integer", "boolean"):
            count = ctypes.c_uint32(0)
            iarr = lib.ifcopenshell_entity_get_aggregate_int(h, attr, ctypes.byref(count))
            if iarr and count.value > 0:
                if elem_kind == "boolean":
                    result = tuple(bool(iarr[i]) for i in range(count.value))
                else:
                    result = tuple(iarr[i] for i in range(count.value))
                lib.ifcopenshell_free_int_array(iarr)
                return result
            return tuple()

        if elem_kind in ("string", "enum", "binary"):
            count = ctypes.c_uint32(0)
            sarr = lib.ifcopenshell_entity_get_aggregate_string(h, attr, ctypes.byref(count))
            if sarr and count.value > 0:
                result = tuple(sarr[i].decode("utf-8") if sarr[i] else "" for i in range(count.value))
                lib.ifcopenshell_free_string_array(sarr, count)
                return result
            return tuple()

        if nested_double:
            nested = self._get_aggregate_double_list_list(name)
            return nested if nested is not None else tuple()

        if elem_kind == "select":
            tv_result = self._get_aggregate_typed_value(h, attr)
            if tv_result is not None and len(tv_result) > 0:
                return tv_result
            count = ctypes.c_uint32(0)
            arr = lib.ifcopenshell_entity_get_aggregate_ref(h, attr, ctypes.byref(count))
            if arr and count.value > 0:
                non_null = [arr[i] for i in range(count.value) if arr[i]]
                if non_null and len(non_null) == count.value:
                    result = tuple(entity_instance(self._file, p) for p in non_null)
                    lib.ifcopenshell_free_instance_array_only(arr)
                    return result
                lib.ifcopenshell_free_instance_array(arr, count.value)
            return tv_result if tv_result is not None else tuple()

        raise TypeError(f"Unsupported aggregate element type for '{name}': {pt!r}")

    def _get_aggregate_double_list_list(self, name):
        """Read LIST OF LIST OF REAL via autogen attribute_value_as_double_list_list."""
        lib = _get_lib()

        class _DL(ctypes.Structure):
            _fields_ = [("items", ctypes.POINTER(ctypes.c_double)), ("size", ctypes.c_size_t)]

        class _DLL(ctypes.Structure):
            _fields_ = [("items", ctypes.POINTER(_DL)), ("size", ctypes.c_size_t)]

        lib.ifcopenshell_ifc_instance_get_argument_by_name.restype = ctypes.c_bool
        lib.ifcopenshell_ifc_instance_get_argument_by_name.argtypes = [
            ctypes.c_void_p, ctypes.c_char_p, ctypes.POINTER(ctypes.c_void_p),
        ]
        lib.ifcopenshell_ifcparse_attribute_value_as_double_list_list.restype = ctypes.c_bool
        lib.ifcopenshell_ifcparse_attribute_value_as_double_list_list.argtypes = [
            ctypes.c_void_p, ctypes.POINTER(_DLL),
        ]
        lib.ifcopenshell_double_list_list_destroy.restype = None
        lib.ifcopenshell_double_list_list_destroy.argtypes = [ctypes.POINTER(_DLL)]

        av = ctypes.c_void_p()
        if not lib.ifcopenshell_ifc_instance_get_argument_by_name(
                self._handle, _enc(name), ctypes.byref(av)) or not av.value:
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
            # attribute_value ownership belongs to caller; check if we need to free it.
            pass

    def _get_aggregate_typed_value(self, h, attr):
        """Try reading an aggregate of inline typed values."""
        lib = _get_lib()
        type_names_ptr = ctypes.POINTER(ctypes.c_char_p)()
        values_ptr = ctypes.POINTER(ctypes.c_char_p)()
        n = lib.ifcopenshell_entity_get_aggregate_typed_value(
            h, attr,
            ctypes.byref(type_names_ptr), ctypes.byref(values_ptr))
        if n <= 0:
            return None
        result = []
        for i in range(n):
            tn = type_names_ptr[i].decode("utf-8") if type_names_ptr[i] else ""
            sv = values_ptr[i].decode("utf-8") if values_ptr[i] else ""
            result.append(_tv_class()(self._file, tn, sv))
        lib.ifcopenshell_free_string_array(type_names_ptr, n)
        lib.ifcopenshell_free_string_array(values_ptr, n)
        return tuple(result)

    def __setattr__(self, name, value):
        if name.startswith("_"):
            super().__setattr__(name, value)
            return
        lib = _get_lib()
        h = self._handle
        attr = _enc(name)

        if not lib.ifcopenshell_entity_has_attr(h, attr):
            super().__setattr__(name, value)
            return

        value = self._coerce_to_declared_type(name, value)

        # Record the edit before mutation so the transaction captures the
        # pre-image. Only entities that belong to a file (id != 0) are tracked.
        file_obj = getattr(self, "_file", None)
        transaction = getattr(file_obj, "transaction", None) if file_obj is not None else None
        if transaction is not None and self.id():
            idx = self._attr_index(name)
            if idx >= 0:
                try:
                    transaction.store_edit(self, idx, value)
                except Exception:
                    pass

        if value is None:
            lib.ifcopenshell_entity_set_null(h, attr)
        elif isinstance(value, bool):
            lib.ifcopenshell_entity_set_bool(h, attr, int(value))
        elif isinstance(value, int):
            lib.ifcopenshell_entity_set_int(h, attr, value)
        elif isinstance(value, float):
            lib.ifcopenshell_entity_set_double(h, attr, value)
        elif isinstance(value, _tv_class()):
            lib.ifcopenshell_entity_set_typed_value(
                h, attr, _enc(value._type_name),
                _enc(_typed_value_str(value)) if value._wrapped is not None else None)
        elif isinstance(value, entity_instance):
            lib.ifcopenshell_entity_set_reference(h, attr, value._handle)
        elif isinstance(value, str):
            lib.ifcopenshell_clear_error()
            if not lib.ifcopenshell_entity_set_enum(h, attr, _enc(value)):
                err = lib.ifcopenshell_last_error_message()
                if err:
                    msg = err.decode("utf-8", errors="replace")
                    lib.ifcopenshell_clear_error()
                    raise RuntimeError(msg)
                lib.ifcopenshell_entity_set_string(h, attr, _enc(value))
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
        lib = _get_lib()
        n = lib.ifcopenshell_entity_attr_count(self._handle)
        for i in range(n):
            if self.attribute_name(i) == name:
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
        if not items:
            lib.ifcopenshell_entity_set_aggregate_ref(h, attr, None, 0)
            return

        pt = self._declared_attribute_primitive(name)
        elem_kind = None
        nested_double = False
        nested_int = False
        if isinstance(pt, tuple) and len(pt) == 2:
            inner = pt[1]
            if isinstance(inner, tuple) and len(inner) == 2:
                if inner[1] == "float":
                    nested_double = True
                elif inner[1] == "integer":
                    nested_int = True
            elif isinstance(inner, str):
                elem_kind = inner
        elif isinstance(pt, str):
            elem_kind = pt

        first = items[0]
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

        if isinstance(first, _tv_class()):
            type_names = (ctypes.c_char_p * len(items))(*[_enc(v._type_name) for v in items])
            str_vals = (ctypes.c_char_p * len(items))(
                *[_enc(_typed_value_str(v)) if v._wrapped is not None else _enc("") for v in items])
            lib.ifcopenshell_entity_set_aggregate_typed_value(h, attr, type_names, str_vals, len(items))
            return
        if isinstance(first, entity_instance):
            arr = (ctypes.c_void_p * len(items))(*[e._handle for e in items])
            lib.ifcopenshell_entity_set_aggregate_ref(h, attr, arr, len(items))
            return

        if elem_kind == "entity":
            arr = (ctypes.c_void_p * len(items))(
                *[e._handle if isinstance(e, entity_instance) else None for e in items])
            lib.ifcopenshell_entity_set_aggregate_ref(h, attr, arr, len(items))
        elif elem_kind == "float":
            arr = (ctypes.c_double * len(items))(*[float(v) for v in items])
            lib.ifcopenshell_entity_set_aggregate_double(h, attr, arr, len(items))
        elif elem_kind == "integer":
            arr = (ctypes.c_int64 * len(items))(*[int(v) for v in items])
            lib.ifcopenshell_entity_set_aggregate_int(h, attr, arr, len(items))
        elif elem_kind == "boolean":
            arr = (ctypes.c_int64 * len(items))(*[int(bool(v)) for v in items])
            lib.ifcopenshell_entity_set_aggregate_int(h, attr, arr, len(items))
        elif elem_kind in ("string", "enum", "binary"):
            arr = (ctypes.c_char_p * len(items))(*[_enc(str(s)) for s in items])
            lib.ifcopenshell_entity_set_aggregate_string(h, attr, arr, len(items))
        else:
            raise TypeError(f"Cannot set aggregate '{name}' with declared primitive {pt!r}")

    def __repr__(self):
        lib = _get_lib()
        spf = lib.ifcopenshell_entity_to_string(self._handle)
        if spf:
            return spf.decode("utf-8")
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
