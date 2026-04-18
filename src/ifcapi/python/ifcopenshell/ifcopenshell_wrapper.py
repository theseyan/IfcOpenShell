# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later
"""SWIG-compatible facade for schema introspection backed by ifcapi C API.

Presents the same object shape as the original
``ifcopenshell.ifcopenshell_wrapper`` module (attribute, parameter_type,
declaration, entity, type_declaration, select_type, enumeration_type,
named_type, simple_type, aggregation_type, inverse_attribute,
schema_definition) so that ifcopenshell.util.attribute / util.schema /
util.selector can import this module unchanged.

Handles are plain opaque pointers; all string/array memory is owned by
the schema singletons in libIfcApi — we copy Python strings out so no
ownership crosses the FFI boundary to the caller.

Also provides a couple of stubs (map_shape, function_item_evaluator,
helmert_curve_point, convert_loop_to_function_item) used by alignment
fallbacks that don't have a native geometry backend.
"""

from __future__ import annotations

import ctypes
from typing import Optional

import numpy as np

import ifcopenshell


# ---------------------------------------------------------------------------
# ctypes signature binding (lazy, once)
# ---------------------------------------------------------------------------

_bound = False


def _bind():
    global _bound
    lib = ifcopenshell._get_lib()
    if _bound:
        return lib

    voidp = ctypes.c_void_p
    cstr = ctypes.c_char_p
    u32p = ctypes.POINTER(ctypes.c_uint32)

    sigs = {
        "ifcopenshell_free_ptr_array": (None, [voidp]),
        "ifcopenshell_schema_by_name": (voidp, [cstr]),
        "ifcopenshell_file_schema_def": (voidp, [voidp]),
        "ifcopenshell_schema_name": (cstr, [voidp]),
        "ifcopenshell_schema_declaration_by_name": (voidp, [voidp, cstr]),
        "ifcopenshell_schema_declarations": (ctypes.POINTER(voidp), [voidp, u32p]),
        "ifcopenshell_declaration_name": (cstr, [voidp]),
        "ifcopenshell_declaration_is": (ctypes.c_bool, [voidp, cstr]),
        "ifcopenshell_declaration_as_entity": (voidp, [voidp]),
        "ifcopenshell_declaration_as_type": (voidp, [voidp]),
        "ifcopenshell_declaration_as_select": (voidp, [voidp]),
        "ifcopenshell_declaration_as_enumeration": (voidp, [voidp]),
        "ifcopenshell_declaration_schema": (voidp, [voidp]),
        "ifcopenshell_entity_is_abstract": (ctypes.c_bool, [voidp]),
        "ifcopenshell_entity_supertype": (voidp, [voidp]),
        "ifcopenshell_entity_attribute_count": (ctypes.c_uint32, [voidp]),
        "ifcopenshell_entity_attribute_by_index": (voidp, [voidp, ctypes.c_uint32]),
        "ifcopenshell_entity_attribute_index_by_name": (ctypes.c_int32, [voidp, cstr]),
        "ifcopenshell_entity_all_attributes": (ctypes.POINTER(voidp), [voidp, u32p]),
        "ifcopenshell_entity_all_inverse_attributes": (ctypes.POINTER(voidp), [voidp, u32p]),
        "ifcopenshell_entity_attribute_derived": (ctypes.c_bool, [voidp, ctypes.c_uint32]),
        "ifcopenshell_type_declaration_declared_type": (voidp, [voidp]),
        "ifcopenshell_enumeration_items": (ctypes.POINTER(cstr), [voidp, u32p]),
        "ifcopenshell_select_list": (ctypes.POINTER(voidp), [voidp, u32p]),
        "ifcopenshell_attribute_name": (cstr, [voidp]),
        "ifcopenshell_attribute_optional": (ctypes.c_bool, [voidp]),
        "ifcopenshell_attribute_type_of_attribute": (voidp, [voidp]),
        "ifcopenshell_inverse_attribute_name": (cstr, [voidp]),
        "ifcopenshell_inverse_attribute_type_of_aggregation": (ctypes.c_int, [voidp]),
        "ifcopenshell_inverse_attribute_bound1": (ctypes.c_int32, [voidp]),
        "ifcopenshell_inverse_attribute_bound2": (ctypes.c_int32, [voidp]),
        "ifcopenshell_inverse_attribute_entity_reference": (voidp, [voidp]),
        "ifcopenshell_inverse_attribute_attribute_reference": (voidp, [voidp]),
        "ifcopenshell_parameter_type_kind": (ctypes.c_int, [voidp]),
        "ifcopenshell_named_type_declared_type": (voidp, [voidp]),
        "ifcopenshell_simple_type_data_type": (ctypes.c_int, [voidp]),
        "ifcopenshell_aggregation_type_kind": (ctypes.c_int, [voidp]),
        "ifcopenshell_aggregation_type_bound1": (ctypes.c_int32, [voidp]),
        "ifcopenshell_aggregation_type_bound2": (ctypes.c_int32, [voidp]),
        "ifcopenshell_aggregation_type_element": (voidp, [voidp]),
    }
    for name, (restype, argtypes) in sigs.items():
        fn = getattr(lib, name)
        fn.restype = restype
        fn.argtypes = argtypes
    _bound = True
    return lib


# ---------------------------------------------------------------------------
# constants
# ---------------------------------------------------------------------------

_PT_NAMED, _PT_SIMPLE, _PT_AGGREGATION = 0, 1, 2

_SIMPLE_NAMES = {
    0: "binary",
    1: "boolean",
    2: "integer",
    3: "logical",
    4: "number",
    5: "real",
    6: "string",
}

_AGG_NAMES = {0: "array", 1: "bag", 2: "list", 3: "set"}
_INV_AGG_NAMES = {0: "bag", 1: "set", 2: ""}


def _free_ptr_array(ptr) -> None:
    if ptr:
        _bind().ifcopenshell_free_ptr_array(ctypes.cast(ptr, ctypes.c_void_p))


# ---------------------------------------------------------------------------
# Handle-backed wrapper types
# ---------------------------------------------------------------------------


class _Handle:
    __slots__ = ("_h",)

    def __init__(self, handle):
        # handle may be an int (address) or ctypes.c_void_p; store as int
        if isinstance(handle, ctypes.c_void_p):
            handle = handle.value or 0
        self._h = int(handle) if handle else 0

    def __bool__(self) -> bool:
        return self._h != 0

    def __eq__(self, other) -> bool:
        return isinstance(other, _Handle) and self._h == other._h

    def __hash__(self) -> int:
        return self._h

    @property
    def _ptr(self):
        return ctypes.c_void_p(self._h)


def _wrap_decl(h):
    """Create a declaration subclass wrapper based on polymorphic queries."""
    if not h:
        return None
    lib = _bind()
    hv = ctypes.c_void_p(int(h) if not isinstance(h, ctypes.c_void_p) else (h.value or 0))
    if not hv.value:
        return None
    if lib.ifcopenshell_declaration_as_entity(hv):
        return entity(hv)
    if lib.ifcopenshell_declaration_as_enumeration(hv):
        return enumeration_type(hv)
    if lib.ifcopenshell_declaration_as_select(hv):
        return select_type(hv)
    if lib.ifcopenshell_declaration_as_type(hv):
        return type_declaration(hv)
    return declaration(hv)


def _wrap_parameter_type(h):
    if not h:
        return None
    lib = _bind()
    hv = ctypes.c_void_p(int(h) if not isinstance(h, ctypes.c_void_p) else (h.value or 0))
    if not hv.value:
        return None
    k = lib.ifcopenshell_parameter_type_kind(hv)
    if k == _PT_NAMED:
        return named_type(hv)
    if k == _PT_SIMPLE:
        return simple_type(hv)
    if k == _PT_AGGREGATION:
        return aggregation_type(hv)
    return parameter_type(hv)


class parameter_type(_Handle):
    def as_named_type(self) -> Optional["named_type"]:
        if _bind().ifcopenshell_parameter_type_kind(self._ptr) == _PT_NAMED:
            return named_type(self._ptr)
        return None

    def as_simple_type(self) -> Optional["simple_type"]:
        if _bind().ifcopenshell_parameter_type_kind(self._ptr) == _PT_SIMPLE:
            return simple_type(self._ptr)
        return None

    def as_aggregation_type(self) -> Optional["aggregation_type"]:
        if _bind().ifcopenshell_parameter_type_kind(self._ptr) == _PT_AGGREGATION:
            return aggregation_type(self._ptr)
        return None

    # SWIG wrappers produce str/repr based on the concrete subtype; if someone
    # calls str() on a plain parameter_type, dispatch by kind.
    def __repr__(self) -> str:
        n = self.as_named_type()
        if n is not None:
            return repr(n)
        s = self.as_simple_type()
        if s is not None:
            return repr(s)
        a = self.as_aggregation_type()
        if a is not None:
            return repr(a)
        return "<?>"

    __str__ = __repr__


class named_type(parameter_type):
    def declared_type(self) -> Optional["declaration"]:
        return _wrap_decl(_bind().ifcopenshell_named_type_declared_type(self._ptr))

    def __repr__(self) -> str:
        d = self.declared_type()
        return repr(d) if d is not None else "<?>"

    __str__ = __repr__


class simple_type(parameter_type):
    def declared_type(self) -> str:
        k = _bind().ifcopenshell_simple_type_data_type(self._ptr)
        return _SIMPLE_NAMES.get(k, "")

    def __repr__(self) -> str:
        return "<%s>" % self.declared_type()

    __str__ = __repr__


class aggregation_type(parameter_type):
    def type_of_aggregation(self) -> int:
        return _bind().ifcopenshell_aggregation_type_kind(self._ptr)

    def type_of_aggregation_string(self) -> str:
        return _AGG_NAMES.get(self.type_of_aggregation(), "")

    def bound1(self) -> int:
        return _bind().ifcopenshell_aggregation_type_bound1(self._ptr)

    def bound2(self) -> int:
        return _bind().ifcopenshell_aggregation_type_bound2(self._ptr)

    def type_of_element(self) -> Optional[parameter_type]:
        return _wrap_parameter_type(_bind().ifcopenshell_aggregation_type_element(self._ptr))

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
    def name(self) -> str:
        s = _bind().ifcopenshell_declaration_name(self._ptr)
        return s.decode("utf-8") if s else ""

    def is_(self, name: str) -> bool:
        return _bind().ifcopenshell_declaration_is(self._ptr, name.encode("utf-8"))

    # SWIG exposes the method as `is()` but that's a Python keyword; the real
    # SWIG wrapper works because SWIG renames it. Provide both.
    def __getattr__(self, name):
        if name == "is":
            return self.is_
        raise AttributeError(name)

    def as_entity(self) -> Optional["entity"]:
        h = _bind().ifcopenshell_declaration_as_entity(self._ptr)
        return entity(h) if h else None

    def as_type_declaration(self) -> Optional["type_declaration"]:
        h = _bind().ifcopenshell_declaration_as_type(self._ptr)
        return type_declaration(h) if h else None

    def as_select_type(self) -> Optional["select_type"]:
        h = _bind().ifcopenshell_declaration_as_select(self._ptr)
        return select_type(h) if h else None

    def as_enumeration_type(self) -> Optional["enumeration_type"]:
        h = _bind().ifcopenshell_declaration_as_enumeration(self._ptr)
        return enumeration_type(h) if h else None

    def schema(self) -> Optional["schema_definition"]:
        h = _bind().ifcopenshell_declaration_schema(self._ptr)
        return schema_definition(h) if h else None

    def __repr__(self) -> str:
        return "<entity %s>" % self.name() if self.as_entity() else "<declaration %s>" % self.name()


class entity(declaration):
    def is_abstract(self) -> bool:
        return _bind().ifcopenshell_entity_is_abstract(self._ptr)

    def supertype(self) -> Optional["entity"]:
        h = _bind().ifcopenshell_entity_supertype(self._ptr)
        return entity(h) if h else None

    def attribute_count(self) -> int:
        return int(_bind().ifcopenshell_entity_attribute_count(self._ptr))

    def attributes(self) -> tuple:
        """Return this entity's own declared attributes (not inherited)."""
        n = self.attribute_count()
        return tuple(self.attribute_by_index(i) for i in range(n))

    def attribute_by_index(self, index: int) -> Optional["attribute"]:
        h = _bind().ifcopenshell_entity_attribute_by_index(self._ptr, ctypes.c_uint32(index))
        return attribute(h) if h else None

    def attribute_index(self, name: str) -> int:
        return int(_bind().ifcopenshell_entity_attribute_index_by_name(self._ptr, name.encode("utf-8")))

    def all_attributes(self) -> tuple:
        lib = _bind()
        n = ctypes.c_uint32(0)
        arr = lib.ifcopenshell_entity_all_attributes(self._ptr, ctypes.byref(n))
        try:
            return tuple(attribute(arr[i]) for i in range(n.value))
        finally:
            _free_ptr_array(arr)

    def all_inverse_attributes(self) -> tuple:
        lib = _bind()
        n = ctypes.c_uint32(0)
        arr = lib.ifcopenshell_entity_all_inverse_attributes(self._ptr, ctypes.byref(n))
        try:
            return tuple(inverse_attribute(arr[i]) for i in range(n.value))
        finally:
            _free_ptr_array(arr)

    def derived(self) -> tuple:
        # Flattened derived-bits for all_attributes(); walk each index.
        lib = _bind()
        n = lib.ifcopenshell_entity_attribute_count(self._ptr)
        return tuple(lib.ifcopenshell_entity_attribute_derived(self._ptr, ctypes.c_uint32(i)) for i in range(n))

    def __repr__(self) -> str:
        return "<entity %s>" % self.name()


class type_declaration(declaration):
    def declared_type(self) -> Optional[parameter_type]:
        return _wrap_parameter_type(_bind().ifcopenshell_type_declaration_declared_type(self._ptr))

    def __repr__(self) -> str:
        return "<type %s: %r>" % (self.name(), self.declared_type())


class enumeration_type(declaration):
    def enumeration_items(self) -> tuple:
        lib = _bind()
        n = ctypes.c_uint32(0)
        arr = lib.ifcopenshell_enumeration_items(self._ptr, ctypes.byref(n))
        try:
            return tuple(arr[i].decode("utf-8") for i in range(n.value))
        finally:
            _free_ptr_array(arr)

    def __repr__(self) -> str:
        return "<enumeration %s: (%s)>" % (self.name(), ", ".join(self.enumeration_items()))


class select_type(declaration):
    def select_list(self) -> tuple:
        lib = _bind()
        n = ctypes.c_uint32(0)
        arr = lib.ifcopenshell_select_list(self._ptr, ctypes.byref(n))
        try:
            return tuple(_wrap_decl(arr[i]) for i in range(n.value))
        finally:
            _free_ptr_array(arr)

    def __repr__(self) -> str:
        return "<select %s: (%s)>" % (self.name(), " | ".join(map(repr, self.select_list())))


class attribute(_Handle):
    def name(self) -> str:
        s = _bind().ifcopenshell_attribute_name(self._ptr)
        return s.decode("utf-8") if s else ""

    def optional(self) -> bool:
        return _bind().ifcopenshell_attribute_optional(self._ptr)

    def type_of_attribute(self) -> Optional[parameter_type]:
        return _wrap_parameter_type(_bind().ifcopenshell_attribute_type_of_attribute(self._ptr))

    def __repr__(self) -> str:
        return "<attribute %s%s: %s>" % (
            self.name(),
            "?" if self.optional() else "",
            self.type_of_attribute(),
        )


class inverse_attribute(_Handle):
    def name(self) -> str:
        s = _bind().ifcopenshell_inverse_attribute_name(self._ptr)
        return s.decode("utf-8") if s else ""

    def type_of_aggregation(self) -> int:
        return _bind().ifcopenshell_inverse_attribute_type_of_aggregation(self._ptr)

    def type_of_aggregation_string(self) -> str:
        return _INV_AGG_NAMES.get(self.type_of_aggregation(), "")

    def bound1(self) -> int:
        return _bind().ifcopenshell_inverse_attribute_bound1(self._ptr)

    def bound2(self) -> int:
        return _bind().ifcopenshell_inverse_attribute_bound2(self._ptr)

    def entity_reference(self) -> Optional[entity]:
        h = _bind().ifcopenshell_inverse_attribute_entity_reference(self._ptr)
        return entity(h) if h else None

    def attribute_reference(self) -> Optional[attribute]:
        h = _bind().ifcopenshell_inverse_attribute_attribute_reference(self._ptr)
        return attribute(h) if h else None

    def __repr__(self) -> str:
        ref = self.entity_reference()
        aref = self.attribute_reference()
        return "<inverse %s: %s for %s.%s>" % (
            self.name(),
            self.type_of_aggregation_string() or "",
            ref.name() if ref else "?",
            aref.name() if aref else "?",
        )


class schema_definition(_Handle):
    def name(self) -> str:
        s = _bind().ifcopenshell_schema_name(self._ptr)
        return s.decode("utf-8") if s else ""

    def declaration_by_name(self, name) -> Optional[declaration]:
        # Accept either a string or an int index (SWIG supports both overloads,
        # but we only need the string form for our use cases).
        if isinstance(name, int):
            raise NotImplementedError("declaration_by_name(int) not supported")
        h = _bind().ifcopenshell_schema_declaration_by_name(self._ptr, name.encode("utf-8"))
        return _wrap_decl(h)

    def declarations(self) -> tuple:
        lib = _bind()
        n = ctypes.c_uint32(0)
        arr = lib.ifcopenshell_schema_declarations(self._ptr, ctypes.byref(n))
        try:
            return tuple(_wrap_decl(arr[i]) for i in range(n.value))
        finally:
            _free_ptr_array(arr)

    def __repr__(self) -> str:
        return "<schema %s>" % self.name()


# ---------------------------------------------------------------------------
# module-level accessors (SWIG parity)
# ---------------------------------------------------------------------------


def schema_by_name(name: str) -> schema_definition:
    lib = _bind()
    h = lib.ifcopenshell_schema_by_name(name.encode("utf-8"))
    if not h:
        raise RuntimeError("Schema not found: %s" % name)
    return schema_definition(h)


# ---------------------------------------------------------------------------
# Geometry/alignment stubs (no native geometry kernel yet)
# ---------------------------------------------------------------------------


class _MockFunctionItem:
    def start(self):
        return 0.0

    def end(self):
        return 0.0


class _MockEvaluator:
    def evaluate(self, distance_along):
        return np.eye(4)


def map_shape(settings, wrapped_data):
    return _MockFunctionItem()


def function_item_evaluator(settings, fn):
    return _MockEvaluator()


def helmert_curve_point(A0, A1, A2, length):
    return (0.0, 0.0, 0.0)


def convert_loop_to_function_item(fn):
    return _MockFunctionItem()
