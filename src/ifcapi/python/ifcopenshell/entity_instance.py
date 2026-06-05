"""entity_instance class backed by the native ifcapi C library."""

from __future__ import annotations

from ifcopenshell import (
    ATTR_AGGREGATE,
    ATTR_BOOL,
    ATTR_DERIVED,
    ATTR_DOUBLE,
    ATTR_ENUM,
    ATTR_INT,
    ATTR_NULL,
    ATTR_REFERENCE,
    ATTR_STRING,
    ATTR_UNKNOWN,
)

from . import _ifcopenshell_capi as _capi

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


def _instance_type_name(handle) -> str:
    return _capi.declaration_name(_capi.instance_declaration(handle))


def _instance_to_string(handle, valid_spf: bool) -> str:
    return _capi.instance_to_string(handle, valid_spf) or ""


def _instance_attribute_names(handle) -> tuple[str, ...]:
    return _capi.instance_get_attribute_names(handle) or ()


def _instance_inverse_attribute_names(handle) -> tuple[str, ...]:
    return _capi.instance_get_inverse_attribute_names(handle) or ()


def _attribute_value_to_python(av, file_obj):
    """Convert an attribute value handle to a Python value."""
    if _capi.attribute_value_is_null(av):
        return None
    value_type = _capi.attribute_value_type(av) or ""
    if value_type == "ENUMERATION":
        return _capi.attribute_value_as_enumeration_value(av)
    if value_type in ("STRING", "BINARY"):
        return _capi.attribute_value_as_string(av)
    if value_type == "ENTITY INSTANCE":
        return entity_instance(file_obj, _capi.attribute_value_as_instance(av))
    if value_type == "INT":
        return int(_capi.attribute_value_as_int32(av))
    if value_type in ("BOOL", "LOGICAL"):
        return bool(_capi.attribute_value_as_bool(av))
    if value_type == "DOUBLE":
        return float(_capi.attribute_value_as_double(av))
    if value_type == "AGGREGATE OF INT":
        return _capi.attribute_value_as_int32_list(av)
    if value_type == "AGGREGATE OF DOUBLE":
        return _capi.attribute_value_as_double_list(av)
    if value_type == "AGGREGATE OF ENTITY INSTANCE":
        return tuple(entity_instance(file_obj, item) for item in _capi.attribute_value_as_instance_list(av))
    if value_type in ("AGGREGATE OF STRING", "AGGREGATE OF BINARY"):
        return _capi.attribute_value_as_string_list(av)
    return None


def _take_attribute_value_type(attribute_value) -> str:
    return _capi.attribute_value_type(attribute_value) or ""


def _attribute_type_from_value(attribute_value) -> int:
    if _capi.attribute_value_is_null(attribute_value):
        return ATTR_NULL
    return _ATTRIBUTE_VALUE_TYPES.get(_take_attribute_value_type(attribute_value), ATTR_UNKNOWN)


def register_schema_attributes(schema) -> None:
    """Validate a registered schema for upstream API parity.

    The native wrapper resolves setter/getter behaviour from schema declarations
    at the point of use, so it does not maintain SWIG's Python-side setter table.
    """
    if schema is None or not hasattr(schema, "declarations"):
        raise TypeError("register_schema_attributes() expects a schema_definition")
    tuple(schema.declarations())


def _should_unpack_inverse(file_obj: file, type_name: str, attr_name: str) -> bool:
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

    def __init__(self, file_obj: file, handle):
        """Wrap a native ifcopenshell_ifc_instance_t* handle.

        ``handle`` is a native _ifcopenshell_capi handle returned from a C API call.
        Ownership of the handle is transferred to this instance.
        """
        self._file = file_obj
        self._handle = handle

    def __del__(self):
        try:
            if self._handle:
                _capi.instance_destroy(self._handle)
                self._handle = None
        except Exception:
            pass

    @property
    def file(self) -> file:
        return self._file

    @property
    def wrapped_data(self):
        """Compatibility shim for code expecting SWIG wrapped_data."""
        return getattr(self, "_wrapped_data", self)

    @wrapped_data.setter
    def wrapped_data(self, value):
        object.__setattr__(self, "_wrapped_data", value)

    def id(self) -> int:
        h = self._handle
        if not h:
            return 0
        # The C API raises RuntimeError for destroyed handles (handle ptr is NULL).
        # Check the raw pointer first to avoid the exception path.
        if not h.handle:
            return 0
        return int(_capi.instance_id(h))

    def identity(self) -> tuple:
        """Stable identity key for this instance (parity with SWIG wrapped_data.identity()).

        Returns a tuple unique per (file, instance) so it can be used as a
        dict key for caching, e.g. by ifcopenshell.api.project.append_asset.
        """
        return (id(self._file), self.id(), self.is_a())

    def is_a(self, type_name=None):
        if type_name is None:
            return _instance_type_name(self._handle)
        if isinstance(type_name, bool) and type_name:
            schema = self._file.schema
            name = _instance_type_name(self._handle)
            return f"{schema}.{name}" if name else ""
        return bool(_capi.instance_is_a(self._handle, type_name))

    def is_entity(self) -> bool:
        return True

    def declaration(self):
        """Return the schema declaration for this entity's type.

        Routes through the autogen ABI (`ifcopenshell_ifc_instance_declaration`)
        so header-section entities (which live in `Header_section_schema` rather
        than the file's IFC schema) are also resolvable.
        """
        from ifcopenshell import ifcopenshell_wrapper

        return ifcopenshell_wrapper.instance_declaration(self._handle)

    def attribute_name(self, index: int) -> str:
        names = _instance_attribute_names(self._handle)
        return names[index] if 0 <= index < len(names) else ""

    def attribute_type(self, attr) -> str:
        if isinstance(attr, int):
            attr = self.attribute_name(attr)
        atype = ATTR_UNKNOWN
        idx = self._attr_index(attr)
        if idx >= 0:
            av = _capi.instance_get_argument(self._handle, idx)
            try:
                atype = _attribute_type_from_value(av)
            finally:
                _capi.attribute_value_destroy(av)
        _type_names = {
            ATTR_NULL: "NULL", ATTR_STRING: "STRING", ATTR_INT: "INT",
            ATTR_DOUBLE: "DOUBLE", ATTR_BOOL: "BOOL", ATTR_ENUM: "ENUMERATION",
            ATTR_REFERENCE: "ENTITY_INSTANCE", ATTR_AGGREGATE: "AGGREGATE",
            ATTR_DERIVED: "DERIVED",
        }
        return _type_names.get(atype, "UNKNOWN")

    def get_info(self, include_identifier=True, recursive=False, return_type=dict, ignore=(), scalar_only=False):
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
            if index == 0 and self._is_wrapped_value_instance():
                self._set_wrapped_value(value)
                return
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
        h = self._handle

        # Try direct attribute first.
        if name in _instance_attribute_names(h):
            atype = ATTR_UNKNOWN
            idx = self._attr_index(name)
            av = _capi.instance_get_argument(h, idx)
            if av:
                try:
                    atype = _attribute_type_from_value(av)
                    if atype == ATTR_NULL:
                        if self._is_schema_derived(name):
                            derived = self._get_derived(name)
                            if derived is not None:
                                return derived
                        return None
                    if atype == ATTR_AGGREGATE:
                        return self._get_aggregate(h, name)
                    if atype == ATTR_REFERENCE:
                        typed_value = self._get_typed_value(name)
                        if typed_value is not None:
                            return typed_value
                        ref = self._attribute_value_to_python(av, "entity")
                        if ref is not _MISSING:
                            return ref
                    elif atype != ATTR_AGGREGATE:
                        value = self._attribute_value_to_python(av, "")
                        if value is not _MISSING:
                            return value
                finally:
                    _capi.attribute_value_destroy(av)
            if atype == ATTR_DERIVED:
                from . import ifcopenshell_wrapper as W

                if W.get_feature("use_attribute_value_derived"):
                    return W.attribute_value_derived()
                return self._get_derived(name)
            return None

        # Try inverse attribute (e.g. IsDecomposedBy, Decomposes, ContainedInStructure).
        if name in _instance_inverse_attribute_names(h):
            result = tuple(entity_instance(self._file, item) for item in _capi.instance_get_inverse_attribute_by_name(h, name))
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

        av = _capi.instance_get_argument(self._handle, 0)
        if not av:
            return _MISSING
        try:
            wrapped = self._attribute_value_to_python(av, primitive)
            return None if wrapped is _MISSING else wrapped
        finally:
            _capi.attribute_value_destroy(av)

    def _set_wrapped_value(self, value):
        """Set the sole payload of a native type-declaration instance."""
        if not self._handle:
            raise AttributeError("wrappedValue")
        idx = 0
        if value is None:
            _capi.instance_unset_argument(self._handle, idx)
            return
        if isinstance(value, bool):
            _capi.instance_set_argument_bool(self._handle, idx, value)
            return
        if isinstance(value, int):
            _capi.instance_set_argument_int32(self._handle, idx, value)
            return
        if isinstance(value, float):
            _capi.instance_set_argument_double(self._handle, idx, value)
            return
        if isinstance(value, str):
            _capi.instance_set_argument_string(self._handle, idx, value)
            return
        if isinstance(value, (list, tuple)):
            try:
                from ifcopenshell import _resolve_typed_value_kind

                kind = _resolve_typed_value_kind(self._file, self.is_a())
            except Exception:
                kind = ("unknown",)
            if kind[0] != "aggregate":
                raise TypeError(f"Cannot set wrappedValue to {type(value)}")
            elem_kind = kind[1]
            if elem_kind in ("real", "number"):
                _capi.instance_set_argument_double_list(self._handle, idx, value)
                return
            if elem_kind == "integer":
                _capi.instance_set_argument_int32_list(self._handle, idx, value)
                return
            if elem_kind == "boolean":
                _capi.instance_set_argument_int32_list(self._handle, idx, [int(bool(v)) for v in value])
                return
            if elem_kind in ("string", "binary"):
                _capi.instance_set_argument_string_list(self._handle, idx, value)
                return
        raise TypeError(f"Cannot set wrappedValue to {type(value)}")

    def _attribute_value_to_python(self, av, primitive):
        if _capi.attribute_value_is_null(av):
            return None

        value_type = _capi.attribute_value_type(av) or ""

        if value_type == "ENUMERATION":
            return _capi.attribute_value_as_enumeration_value(av)
        if value_type in ("STRING", "BINARY"):
            return _capi.attribute_value_as_string(av)
        if primitive == "entity" or value_type == "ENTITY INSTANCE":
            return entity_instance(self._file, _capi.attribute_value_as_instance(av))
        # Read using the actual stored type (value_type) first, then fall back to
        # the schema-declared primitive. The stored type is authoritative because
        # IFC types like "NUMBER" can hold either integer or double at runtime.
        if value_type == "INT":
            return int(_capi.attribute_value_as_int32(av))
        if value_type == "DOUBLE":
            return float(_capi.attribute_value_as_double(av))
        if value_type in ("BOOL", "LOGICAL"):
            return bool(_capi.attribute_value_as_bool(av))
        if primitive == "integer":
            return int(_capi.attribute_value_as_int32(av))
        if primitive == "boolean":
            return bool(_capi.attribute_value_as_bool(av))
        if primitive == "float":
            return float(_capi.attribute_value_as_double(av))
        if primitive in ("string", "binary", "enum"):
            return _capi.attribute_value_as_string(av)
        if isinstance(primitive, tuple) and primitive[0] in ("list", "array", "set"):
            return self._attribute_value_list_to_python(av, primitive[1], value_type)
        return _MISSING

    def _attribute_value_list_to_python(self, av, primitive, value_type):
        # Read using the actual stored aggregate type first, then fall back to
        # the schema-declared primitive for the element type.
        if value_type == "AGGREGATE OF INT":
            return _capi.attribute_value_as_int32_list(av)
        if value_type == "AGGREGATE OF DOUBLE":
            return _capi.attribute_value_as_double_list(av)
        if value_type == "AGGREGATE OF ENTITY INSTANCE":
            return tuple(entity_instance(self._file, item) for item in _capi.attribute_value_as_instance_list(av))
        if value_type in ("AGGREGATE OF STRING", "AGGREGATE OF BINARY"):
            return _capi.attribute_value_as_string_list(av)
        if primitive == "integer":
            return _capi.attribute_value_as_int32_list(av)
        if primitive == "float":
            return _capi.attribute_value_as_double_list(av)
        if primitive == "entity":
            return tuple(entity_instance(self._file, item) for item in _capi.attribute_value_as_instance_list(av))
        if primitive in ("string", "binary", "enum"):
            return _capi.attribute_value_as_string_list(av)
        return _MISSING

    def _get_derived(self, name):
        """Compute a DERIVE attribute via the native ``ifcopenshell_compute_derived``
        C ABI. Returns ``None`` when no rule is registered for this entity/attr
        or when the rule evaluates to INDETERMINATE."""
        if not self._handle:
            return None
        av = _capi.compute_derived(self._handle, name)
        if av is None:
            return None
        try:
            # _capi.compute_derived returns an ifcapi value, not an
            # ifcparse attribute_value.  Use the value API directly.
            # Value kinds: 0=NONE, 1=BOOL, 2=INT, 3=DOUBLE, 4=STRING,
            # 5=INSTANCE, 6=LIST, 7=DICT
            kind = int(_capi.value_kind(av))
            if kind == 2:  # INT
                return int(_capi.value_as_int64(av))
            if kind == 3:  # DOUBLE
                return float(_capi.value_as_double(av))
            if kind == 1:  # BOOL
                return bool(_capi.value_as_bool(av))
            if kind == 4:  # STRING
                return _capi.value_as_string(av)
            if kind == 5:  # INSTANCE
                h = _capi.value_as_instance(av)
                if h:
                    entity_id = int(_capi.instance_id(h))
                    return self._file.by_id(entity_id)
                return None
            if kind == 6:  # LIST
                size = int(_capi.value_list_size(av))
                return tuple(
                    self._get_derived_value_at(av, i) for i in range(size)
                )
            # NONE or DICT: return None
            return None
        finally:
            _capi.value_destroy(av)

    def _get_derived_value_at(self, av, index):
        """Read a single element from a derived list value."""
        item = _capi.value_list_at(av, index)
        if item is None:
            return None
        try:
            kind = int(_capi.value_kind(item))
            if kind == 2:  # INT
                return int(_capi.value_as_int64(item))
            if kind == 3:  # DOUBLE
                return float(_capi.value_as_double(item))
            if kind == 1:  # BOOL
                return bool(_capi.value_as_bool(item))
            if kind == 4:  # STRING
                return _capi.value_as_string(item)
            if kind == 5:  # INSTANCE
                h = _capi.value_as_instance(item)
                if h:
                    entity_id = int(_capi.instance_id(h))
                    return self._file.by_id(entity_id)
            return None
        finally:
            _capi.value_destroy(item)

    def _get_aggregate(self, h, name):
        """Read an aggregate attribute, returning a Python tuple.

        Codec is selected from the schema-declared element type."""
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
            idx = self._attr_index(name)
            if idx < 0:
                return _MISSING
            av = _capi.instance_get_argument(h, idx)
            if not av:
                return _MISSING
            try:
                return self._attribute_value_list_to_python(av, primitive, _take_attribute_value_type(av))
            finally:
                _capi.attribute_value_destroy(av)

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
            entity_result = read_generated_list("entity")
            if entity_result is not _MISSING:
                return entity_result
            return tuple()

        raise TypeError(f"Unsupported aggregate element type for '{name}': {pt!r}")

    def _get_aggregate_ref_list_list(self, name):
        idx = self._attr_index(name)
        if idx < 0:
            return None
        av = _capi.instance_get_argument(self._handle, idx)
        if not av:
            return None
        try:
            return tuple(tuple(self._file.by_id(entity_id) for entity_id in row) for row in _capi.attribute_value_as_instance_id_list_list(av))
        finally:
            _capi.attribute_value_destroy(av)

    def _get_typed_value(self, name):
        idx = self._attr_index(name)
        if idx < 0:
            return None
        av = _capi.instance_get_argument(self._handle, idx)
        if not av:
            return None
        try:
            typed_handle = _capi.attribute_value_as_instance(av)
            if not typed_handle:
                return None
            type_name = _instance_type_name(typed_handle)
            if not type_name:
                return None
            try:
                from ifcopenshell import _resolve_typed_value_kind

                typed_value_kind = _resolve_typed_value_kind(self._file, type_name)
            except Exception:
                typed_value_kind = ("unknown",)
            if typed_value_kind[0] == "unknown":
                return None
            return entity_instance(self._file, typed_handle)
        finally:
            _capi.attribute_value_destroy(av)

    def _get_aggregate_int_list_list(self, name):
        """Read LIST OF LIST OF INTEGER via autogen attribute_value_as_int32_list_list."""
        idx = self._attr_index(name)
        if idx < 0:
            return None
        av = _capi.instance_get_argument(self._handle, idx)
        if not av:
            return None
        try:
            return _capi.attribute_value_as_int32_list_list(av)
        finally:
            _capi.attribute_value_destroy(av)

    def _get_aggregate_double_list_list(self, name):
        """Read LIST OF LIST OF REAL via autogen attribute_value_as_double_list_list."""
        idx = self._attr_index(name)
        if idx < 0:
            return None
        av = _capi.instance_get_argument(self._handle, idx)
        if not av:
            return None
        try:
            return _capi.attribute_value_as_double_list_list(av)
        finally:
            _capi.attribute_value_destroy(av)

    def __setattr__(self, name, value):
        if name.startswith("_"):
            super().__setattr__(name, value)
            return
        if name == "wrappedValue" and self._is_wrapped_value_instance():
            self._set_wrapped_value(value)
            return
        h = self._handle

        if name not in _instance_attribute_names(h):
            super().__setattr__(name, value)
            return

        value = self._coerce_to_declared_type(name, value)
        idx = self._attr_index(name)

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
            _capi.instance_unset_argument(h, idx)
        elif isinstance(value, bool):
            _capi.instance_set_argument_bool(h, idx, value)
        elif isinstance(value, int):
            _capi.instance_set_argument_int32(h, idx, value)
        elif isinstance(value, float):
            _capi.instance_set_argument_double(h, idx, value)
        elif isinstance(value, entity_instance):
            _capi.instance_set_argument_instance(h, idx, value._handle)
        elif isinstance(value, str):
            if self._declared_attribute_primitive(name) == "enum":
                _capi.instance_set_argument_enumeration_by_name(h, idx, value)
            else:
                _capi.instance_set_argument_string(h, idx, value)
        elif isinstance(value, (list, tuple)):
            self._set_aggregate(h, name, value)
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
                    if pt is None:
                        pt = self._declared_attribute_primitive(name)
                    try:
                        attr_type = str(a.type_of_attribute()).lower()
                    except Exception:
                        attr_type = ""
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
            entity = decl.as_entity() if hasattr(decl, "as_entity") else None
            if entity is None:
                return None
            for a in entity.all_attributes():
                if a.name() == name:
                    try:
                        primitive = get_primitive_type(a)
                    except Exception:
                        primitive = None
                    if primitive is not None:
                        return primitive
                    pt = a.type_of_attribute()
                    pt_handle = getattr(pt, "_h", None) or pt
                    arg_type = _capi.argument_type_to_string(_capi.from_parameter_type(pt_handle))
                    return _primitive_from_argument_type(arg_type)
        except Exception:
            return None
        return None

    def _set_aggregate(self, h, name, items):
        """Write a Python list to an aggregate attribute.

        Codec is chosen from the schema-declared element type whenever
        possible; the value-driven path is only taken when no declaration
        is available (e.g. derived attributes)."""
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
            idx = self._attr_index(name)
            if idx < 0:
                raise TypeError(f"Unknown attribute '{name}'")
            for value in values:
                if not isinstance(value, entity_instance):
                    raise TypeError(f"Cannot set aggregate '{name}' with {type(value)}")
            _capi.instance_set_argument_instance_list(h, idx, [value._handle for value in values])

        first = items[0] if items else None
        if first is None and isinstance(pt, tuple) and len(pt) == 2 and isinstance(pt[1], tuple) and pt[1][0] == "select":
            set_generated_instance_list(items)
            return
        if nested_double or nested_int:
            idx = self._attr_index(name)
            if idx < 0:
                raise TypeError(f"Unknown attribute '{name}'")
            if nested_double:
                _capi.instance_set_argument_double_list_list(h, idx, items)
            else:
                _capi.instance_set_argument_int32_list_list(h, idx, items)
            return

        if nested_entity:
            idx = self._attr_index(name)
            if idx < 0:
                raise TypeError(f"Unknown attribute '{name}'")
            id_rows = []
            for row in items:
                ids = []
                for v in row:
                    if not isinstance(v, entity_instance):
                        raise TypeError(f"Cannot set nested aggregate '{name}' with {type(v)}")
                    ids.append(v.id())
                id_rows.append(ids)
            _capi.instance_set_argument_as_aggregate_of_aggregate_of_entity_instance(h, idx, id_rows)
            return

        if isinstance(first, entity_instance):
            set_generated_instance_list(items)
            return

        idx = self._attr_index(name)
        if elem_kind == "entity":
            set_generated_instance_list(items)
        elif elem_kind == "float":
            _capi.instance_set_argument_double_list(h, idx, items)
        elif elem_kind == "integer":
            _capi.instance_set_argument_int32_list(h, idx, items)
        elif elem_kind == "boolean":
            _capi.instance_set_argument_int32_list(h, idx, [int(bool(v)) for v in items])
        elif elem_kind in ("string", "enum", "binary"):
            _capi.instance_set_argument_string_list(h, idx, items)
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
def _primitive_from_argument_type(type_name):
    type_name = (type_name or "").upper()
    if type_name.startswith("AGGREGATE OF AGGREGATE OF "):
        inner = _primitive_from_argument_type(type_name[len("AGGREGATE OF ") :])
        return ("list", inner) if inner is not None else None
    if type_name.startswith("AGGREGATE OF "):
        inner = _primitive_from_argument_type(type_name[len("AGGREGATE OF ") :])
        return ("list", inner) if inner is not None else None
    return {
        "STRING": "string",
        "BINARY": "binary",
        "DOUBLE": "float",
        "REAL": "float",
        "NUMBER": "float",
        "INT": "integer",
        "INTEGER": "integer",
        "BOOL": "boolean",
        "BOOLEAN": "boolean",
        "LOGICAL": "enum",
        "ENUMERATION": "enum",
        "ENTITY INSTANCE": "entity",
    }.get(type_name)
