# SPDX-License-Identifier: LGPL-3.0-or-later
"""SWIG-compatible facade for schema introspection backed by _ifcopenshell_capi."""

from __future__ import annotations

from typing import Optional

import ifcopenshell

from . import _ifcopenshell_capi as _capi

_SIMPLE_NAMES = {0: "binary", 1: "boolean", 2: "integer", 3: "logical", 4: "number", 5: "real", 6: "string"}
_AGG_NAMES = {0: "array", 1: "bag", 2: "list", 3: "set"}
_INV_AGG_NAMES = {0: "bag", 1: "set", 2: ""}


class _Handle:
    __slots__ = ("_h",)
    _destroy_fn_name: Optional[str] = None

    def __init__(self, h):
        self._h = h

    def __del__(self):
        try:
            h = self._h
        except Exception:
            return
        if h and self._destroy_fn_name:
            try:
                getattr(_capi, self._destroy_fn_name)(h)
            except Exception:
                pass
            self._h = None

    def __bool__(self) -> bool:
        return bool(self._h)

    def _cppptr(self) -> int:
        return self._h.handle if self._h else 0

    @property
    def _ptr(self):
        return self._h.handle if self._h else 0

    def __eq__(self, other) -> bool:
        if isinstance(other, _Handle):
            return self._cppptr() == other._cppptr()
        return NotImplemented

    def __hash__(self) -> int:
        return hash(self._cppptr())


def _wrap_optional(h, ctor):
    return ctor(h) if h else None


def _wrap_decl(h):
    if not h:
        return None
    for capi_fn, ctor in (
        (_capi.declaration_as_entity, entity),
        (_capi.declaration_as_type_declaration, type_declaration),
        (_capi.declaration_as_select_type, select_type),
        (_capi.declaration_as_enumeration_type, enumeration_type),
    ):
        typed = capi_fn(h)
        if typed:
            _capi.declaration_destroy(h)
            return ctor(typed)
    return declaration(h)


def _wrap_parameter_type(h):
    if not h:
        return None
    for capi_fn, ctor in (
        (_capi.parameter_type_as_named_type, named_type),
        (_capi.parameter_type_as_simple_type, simple_type),
        (_capi.parameter_type_as_aggregation_type, aggregation_type),
    ):
        typed = capi_fn(h)
        if typed:
            _capi.parameter_type_destroy(h)
            return ctor(typed)
    return parameter_type(h)


class parameter_type(_Handle):
    _destroy_fn_name = "parameter_type_destroy"

    def as_named_type(self) -> Optional[named_type]:
        return _wrap_optional(_capi.parameter_type_as_named_type(self._h), named_type)

    def as_simple_type(self) -> Optional[simple_type]:
        return _wrap_optional(_capi.parameter_type_as_simple_type(self._h), simple_type)

    def as_aggregation_type(self) -> Optional[aggregation_type]:
        return _wrap_optional(_capi.parameter_type_as_aggregation_type(self._h), aggregation_type)

    def __repr__(self) -> str:
        for cast in (self.as_named_type, self.as_simple_type, self.as_aggregation_type):
            v = cast()
            if v is not None:
                return repr(v)
        return "<?>"

    __str__ = __repr__


class named_type(parameter_type):
    _destroy_fn_name = "named_type_destroy"

    def declared_type(self) -> Optional[declaration]:
        return _wrap_decl(_capi.named_type_declared_type(self._h))

    def __repr__(self) -> str:
        d = self.declared_type()
        return repr(d) if d is not None else "<?>"

    __str__ = __repr__


class simple_type(parameter_type):
    _destroy_fn_name = "simple_type_destroy"

    def declared_type(self) -> str:
        return _SIMPLE_NAMES.get(int(_capi.simple_type_declared_type(self._h)), "")

    def __repr__(self) -> str:
        return "<%s>" % self.declared_type()

    __str__ = __repr__


class aggregation_type(parameter_type):
    _destroy_fn_name = "aggregation_type_destroy"

    def type_of_aggregation(self) -> int:
        return int(_capi.aggregation_type_type_of_aggregation(self._h))

    def type_of_aggregation_string(self) -> str:
        return _AGG_NAMES.get(self.type_of_aggregation(), "")

    def bound1(self) -> int:
        return int(_capi.aggregation_type_bound1(self._h))

    def bound2(self) -> int:
        return int(_capi.aggregation_type_bound2(self._h))

    def type_of_element(self) -> Optional[parameter_type]:
        return _wrap_parameter_type(_capi.aggregation_type_type_of_element(self._h))

    def __repr__(self) -> str:
        fmt = lambda i: "?" if i == -1 else str(i)
        return "<%s [%s:%s] of %r>" % (
            self.type_of_aggregation_string(),
            fmt(self.bound1()),
            fmt(self.bound2()),
            self.type_of_element(),
        )

    __str__ = __repr__


class declaration(_Handle):
    _destroy_fn_name = "declaration_destroy"

    def name(self) -> str:
        return _capi.declaration_name(self._h) or ""

    def is_(self, name: str) -> bool:
        return bool(_capi.declaration_is_a(self._h, name))

    _is = is_

    def __getattr__(self, name):
        if name == "is":
            return self.is_
        raise AttributeError(name)

    def as_entity(self) -> Optional[entity]:
        return _wrap_optional(_capi.declaration_as_entity(self._h), entity)

    def as_type_declaration(self) -> Optional[type_declaration]:
        return _wrap_optional(_capi.declaration_as_type_declaration(self._h), type_declaration)

    def as_select_type(self) -> Optional[select_type]:
        return _wrap_optional(_capi.declaration_as_select_type(self._h), select_type)

    def as_enumeration_type(self) -> Optional[enumeration_type]:
        return _wrap_optional(_capi.declaration_as_enumeration_type(self._h), enumeration_type)

    def schema(self) -> Optional[schema_definition]:
        return _wrap_optional(_capi.declaration_schema(self._h), schema_definition)

    def index_in_schema(self) -> int:
        return int(_capi.declaration_index_in_schema(self._h))

    def __repr__(self) -> str:
        return "<entity %s>" % self.name() if self.as_entity() else "<declaration %s>" % self.name()


class entity(declaration):
    _destroy_fn_name = "entity_destroy"

    def is_abstract(self) -> bool:
        return bool(_capi.entity_is_abstract(self._h))

    def supertype(self) -> Optional[entity]:
        return _wrap_optional(_capi.entity_supertype(self._h), entity)

    def subtypes(self) -> tuple:
        return tuple(entity(h) for h in (_capi.entity_subtypes(self._h) or ()))

    def attribute_count(self) -> int:
        return int(_capi.entity_attribute_count(self._h))

    def attributes(self) -> tuple:
        return tuple(attribute(h) for h in (_capi.entity_attributes(self._h) or ()))

    def attribute_by_index(self, index: int) -> Optional[attribute]:
        return _wrap_optional(_capi.entity_attribute_by_index(self._h, index), attribute)

    def attribute_index(self, name: str) -> int:
        return int(_capi.entity_attribute_index(self._h, name))

    def all_attributes(self) -> tuple:
        return tuple(attribute(h) for h in (_capi.entity_all_attributes(self._h) or ()))

    def all_inverse_attributes(self) -> tuple:
        return tuple(inverse_attribute(h) for h in (_capi.entity_all_inverse_attributes(self._h) or ()))

    def derived(self) -> tuple:
        own_derived = tuple(_capi.entity_derived(self._h) or ())
        if len(own_derived) == len(self.all_attributes()):
            return own_derived
        supertype = self.supertype()
        return (supertype.derived() if supertype is not None else ()) + own_derived

    def __repr__(self) -> str:
        return "<entity %s>" % self.name()


class type_declaration(declaration):
    _destroy_fn_name = "type_declaration_destroy"
    def declared_type(self) -> Optional[parameter_type]:
        return _wrap_parameter_type(_capi.type_declaration_declared_type(self._h))
    def __repr__(self) -> str:
        return "<type %s: %r>" % (self.name(), self.declared_type())


class enumeration_type(declaration):
    _destroy_fn_name = "enumeration_destroy"
    def enumeration_items(self) -> tuple:
        return tuple(_capi.enumeration_enumeration_items(self._h) or ())
    def __repr__(self) -> str:
        return "<enumeration %s: (%s)>" % (self.name(), ", ".join(self.enumeration_items()))


class select_type(declaration):
    _destroy_fn_name = "select_type_destroy"
    def select_list(self) -> tuple:
        return tuple(_wrap_decl(h) for h in (_capi.select_type_select_list(self._h) or ()))
    def __repr__(self) -> str:
        return "<select %s: (%s)>" % (self.name(), " | ".join(map(repr, self.select_list())))


class attribute(_Handle):
    _destroy_fn_name = "attribute_destroy"
    def name(self) -> str:
        return _capi.attribute_name(self._h) or ""
    def optional(self) -> bool:
        return bool(_capi.attribute_optional(self._h))
    def type_of_attribute(self) -> Optional[parameter_type]:
        return _wrap_parameter_type(_capi.attribute_type_of_attribute(self._h))
    def __repr__(self) -> str:
        return "<attribute %s%s: %s>" % (self.name(), "?" if self.optional() else "", self.type_of_attribute())


class inverse_attribute(_Handle):
    _destroy_fn_name = "inverse_attribute_destroy"
    def name(self) -> str:
        return _capi.inverse_attribute_name(self._h) or ""
    def type_of_aggregation(self) -> int:
        return int(_capi.inverse_attribute_type_of_aggregation(self._h))
    def type_of_aggregation_string(self) -> str:
        return _INV_AGG_NAMES.get(self.type_of_aggregation(), "")
    def bound1(self) -> int:
        return int(_capi.inverse_attribute_bound1(self._h))
    def bound2(self) -> int:
        return int(_capi.inverse_attribute_bound2(self._h))
    def entity_reference(self) -> Optional[entity]:
        return _wrap_optional(_capi.inverse_attribute_entity_reference(self._h), entity)
    def attribute_reference(self) -> Optional[attribute]:
        return _wrap_optional(_capi.inverse_attribute_attribute_reference(self._h), attribute)
    def __repr__(self) -> str:
        ref = self.entity_reference()
        aref = self.attribute_reference()
        return "<inverse %s: %s for %s.%s>" % (
            self.name(), self.type_of_aggregation_string() or "", ref.name() if ref else "?", aref.name() if aref else "?"
        )


class schema_definition(_Handle):
    _destroy_fn_name = "schema_destroy"
    def name(self) -> str:
        return _capi.schema_name(self._h) or ""
    def declaration_by_name(self, name) -> Optional[declaration]:
        if isinstance(name, int):
            raise NotImplementedError("declaration_by_name(int) not supported")
        return _wrap_decl(_capi.schema_declaration_by_name(self._h, name))
    def declarations(self) -> tuple:
        return tuple(_wrap_decl(h) for h in (_capi.schema_declarations(self._h) or ()))
    def entities(self) -> tuple:
        return tuple(d.as_entity() for d in self.declarations() if d.as_entity() is not None)
    def __repr__(self) -> str:
        return "<schema %s>" % self.name()


def schema_by_name(name: str) -> schema_definition:
    schema = _capi.schema_by_name(name)
    if not schema:
        raise RuntimeError("Schema not found: %s" % name)
    return schema_definition(schema)


def schema_names() -> tuple:
    return tuple(_capi.schema_names() or ())


def register_schema(schema: schema_definition) -> None:
    if not isinstance(schema, schema_definition):
        raise TypeError("register_schema() expects an ifcopenshell_wrapper.schema_definition")
    _capi.register_schema(schema._h)


def clear_schemas() -> None:
    _capi.clear_schemas()


class _IfcBaseClassInfo:
    INVALID, FORWARD, INVERSE = range(3)
    def __init__(self, schema_name: str, ifc_class: str):
        self.schema_name = schema_name
        self.ifc_class = ifc_class
        declaration = schema_by_name(schema_name).declaration_by_name(ifc_class)
        self.entity = declaration.as_entity() if declaration is not None else None
        self.forward_names = {a.name().lower() for a in (self.entity.all_attributes() if self.entity else ())}
        self.inverse_names = {a.name().lower() for a in (self.entity.all_inverse_attributes() if self.entity else ())}
    def is_a(self, full=False):
        return f"{self.schema_name}.{self.ifc_class}" if full else self.ifc_class
    def get_attribute_category(self, name: str) -> int:
        lowered = name.lower()
        if lowered in self.forward_names:
            return self.FORWARD
        if lowered in self.inverse_names:
            return self.INVERSE
        return self.INVALID


def new_IfcBaseClass(schema_name: str, ifc_class: str):
    return _IfcBaseClassInfo(schema_name, ifc_class)


def instance_declaration(handle_value) -> declaration:
    decl = _capi.instance_declaration(handle_value)
    if not decl:
        raise RuntimeError("Instance has no declaration")
    return _wrap_decl(decl)


class _FunctionItem:
    def __init__(self, item):
        self._item = item
        self._h = item._h
    def _distance(self, fn_name):
        return float(getattr(_capi, fn_name)(self._h))
    def start(self):
        return self._distance("geom_taxonomy_function_item_start")
    def end(self):
        return self._distance("geom_taxonomy_function_item_end")


class function_item_evaluator:
    def __init__(self, settings, fn):
        self._h = _capi.geom_create_function_item_evaluator(settings._h, fn._h)
    def __del__(self):
        h = getattr(self, "_h", None)
        if h:
            try:
                _capi.geom_function_item_evaluator_destroy(h)
            except Exception:
                pass
            self._h = None
    def evaluate(self, distance_along):
        values = _capi.geom_function_item_evaluator_evaluate_at(self._h, float(distance_along))
        return [values[i : i + 4] for i in range(0, 16, 4)]


def map_shape(settings, wrapped_data):
    import ifcopenshell.geom
    return _FunctionItem(ifcopenshell.geom.map_shape(settings, wrapped_data))


def helmert_curve_point(A0, A1, A2, length):
    return tuple(_capi.geom_helmert_curve_point(float(A0), float(A1), float(A2), float(length)))


def convert_loop_to_function_item(fn):
    from ifcopenshell.geom.main import TaxonomyItem
    return _FunctionItem(TaxonomyItem(_capi.geom_convert_loop_to_function_item(fn._h)))


_GEOM_CLASS_ALIASES = {
    "Element", "BRepElement", "TriangulationElement", "SerializedElement", "Triangulation", "BRepRepresentation",
    "SerializedRepresentation",
}


def __getattr__(name):
    if name in _GEOM_CLASS_ALIASES:
        import ifcopenshell.geom
        value = getattr(ifcopenshell.geom, name)
        globals()[name] = value
        return value
    raise AttributeError(name)


CURVES = 0
SURFACES_AND_SOLIDS = 1
CURVES_SURFACES_AND_SOLIDS = 2
TRIANGULATED = 0
NATIVE = 1
SERIALIZED = 2
TRIANGLE_MESH = 0
POLYHEDRON_WITHOUT_HOLES = 1
POLYHEDRON_WITH_HOLES = 2


def version():
    return ifcopenshell.version


_FEATURES: dict = {}


class attribute_value_derived:
    pass


def get_feature(name: str):
    return _FEATURES.get(name)


def set_feature(name: str, value) -> None:
    _FEATURES[name] = value


_LOG_BUFFER: list = []
_LOG_FORMAT = "text"


def _split_json_objects(s: str) -> str:
    if not s or "}{" not in s:
        return s
    out = []
    depth = 0
    in_str = False
    escape = False
    for i, ch in enumerate(s):
        out.append(ch)
        if in_str:
            if escape:
                escape = False
            elif ch == "\\":
                escape = True
            elif ch == '"':
                in_str = False
            continue
        if ch == '"':
            in_str = True
        elif ch == "{":
            depth += 1
        elif ch == "}":
            depth -= 1
            if depth == 0 and i + 1 < len(s) and s[i + 1] == "{":
                out.append("\n")
    return "".join(out)


def get_log() -> str:
    global _LOG_BUFFER
    cpp_log = _capi.get_log() or ""
    if _LOG_FORMAT == "json":
        cpp_log = _split_json_objects(cpp_log)
    if _LOG_BUFFER:
        py_log = "\n".join(_LOG_BUFFER)
        _LOG_BUFFER = []
        return (cpp_log + ("\n" if cpp_log and py_log else "") + py_log) if (cpp_log or py_log) else ""
    return cpp_log


def turn_on_detailed_logging() -> None:
    _capi.turn_on_detailed_logging()


def turn_off_detailed_logging() -> None:
    _capi.turn_off_detailed_logging()


def set_log_format_json() -> None:
    global _LOG_FORMAT
    _LOG_FORMAT = "json"
    _capi.set_log_format_json()


def set_log_format_text() -> None:
    global _LOG_FORMAT
    _LOG_FORMAT = "text"
    _capi.set_log_format_text()


class _FileTypeAlias:
    def __class_getitem__(cls, item):
        return cls


file = _FileTypeAlias  # noqa: A001 - intentional shadowing of builtin name
