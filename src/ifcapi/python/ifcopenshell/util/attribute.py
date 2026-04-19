# IfcOpenShell - IFC toolkit and geometry engine
# Copyright (C) 2021 Dion Moult <dion@thinkmoult.com>
#
# This file is part of IfcOpenShell.
#
# IfcOpenShell is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# IfcOpenShell is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with IfcOpenShell.  If not, see <http://www.gnu.org/licenses/>.

from typing import Literal, Union

import ctypes

import ifcopenshell
import ifcopenshell.ifcopenshell_wrapper as ifcopenshell_wrapper

PrimitiveType = Literal["entity", "string", "float", "integer", "boolean", "enum", "binary"]
ComplexPrimitiveType = Literal["list", "array", "set"]
PrimitiveTypeOutput = Union[
    PrimitiveType,
    tuple[ComplexPrimitiveType, "PrimitiveTypeOutput"],
    tuple[Literal["select"], tuple["PrimitiveTypeOutput", ...]],
    None,
]


_attr_lib_configured = False


def _configure_attr_lib(lib) -> None:
    global _attr_lib_configured
    if _attr_lib_configured:
        return
    lib.ifcopenshell_util_attribute_get_primitive_type.restype = ctypes.c_char_p
    lib.ifcopenshell_util_attribute_get_primitive_type.argtypes = [ctypes.c_void_p]
    lib.ifcopenshell_util_attribute_get_enum_items.restype = ctypes.POINTER(ctypes.c_char_p)
    lib.ifcopenshell_util_attribute_get_enum_items.argtypes = [
        ctypes.c_void_p, ctypes.POINTER(ctypes.c_uint32)
    ]
    lib.ifcopenshell_free_string_array.restype = None
    lib.ifcopenshell_free_string_array.argtypes = [ctypes.POINTER(ctypes.c_char_p), ctypes.c_uint32]
    _attr_lib_configured = True


_AGG_TO_PY = {0: "array", 1: "bag", 2: "list", 3: "set"}


def _primitive_for_pt(pt) -> PrimitiveTypeOutput:
    if pt is None:
        return None
    agg = pt.as_aggregation_type()
    if agg is not None:
        kind = _AGG_TO_PY.get(agg.type_of_aggregation(), "list")
        return (kind, _primitive_for_pt(agg.type_of_element()))
    nt = pt.as_named_type()
    if nt is not None:
        decl = nt.declared_type()
        if decl is None:
            return None
        sel = decl.as_select_type()
        if sel is not None:
            return ("select", tuple(_primitive_for_decl(d) for d in sel.select_list()))
        td = decl.as_type_declaration()
        if td is not None:
            return _primitive_for_pt(td.declared_type())
        if decl.as_enumeration_type() is not None:
            return "enum"
        if decl.as_entity() is not None:
            return "entity"
        return None
    st = pt.as_simple_type()
    if st is not None:
        name = st.declared_type()
        return {
            "string": "string", "real": "float",
            "number": "integer", "integer": "integer",
            "boolean": "boolean", "logical": "enum", "binary": "binary",
        }.get(name)
    return None


def _primitive_for_decl(decl) -> PrimitiveTypeOutput:
    if decl is None:
        return None
    if decl.as_entity() is not None:
        return "entity"
    if decl.as_enumeration_type() is not None:
        return "enum"
    sel = decl.as_select_type()
    if sel is not None:
        return ("select", tuple(_primitive_for_decl(d) for d in sel.select_list()))
    td = decl.as_type_declaration()
    if td is not None:
        return _primitive_for_pt(td.declared_type())
    return None


def get_primitive_type(
    attribute_or_data_type: Union[ifcopenshell_wrapper.attribute, ifcopenshell_wrapper.parameter_type],
) -> PrimitiveTypeOutput:
    if isinstance(attribute_or_data_type, ifcopenshell_wrapper.attribute):
        # Use the C ABI for the leaf-flat answer; recurse in Python for the
        # nested-tuple shape of aggregate/select types.
        lib = ifcopenshell._get_lib()
        _configure_attr_lib(lib)
        flat = lib.ifcopenshell_util_attribute_get_primitive_type(attribute_or_data_type._ptr)
        flat_str = flat.decode("ascii") if flat else "unknown"
        if flat_str not in ("aggregate", "select"):
            if flat_str in ("string", "float", "integer", "boolean", "entity", "enum", "binary"):
                return flat_str
            return None
        return _primitive_for_pt(attribute_or_data_type.type_of_attribute())
    return _primitive_for_pt(attribute_or_data_type)


def get_enum_items(attribute: ifcopenshell_wrapper.attribute) -> tuple[str, ...]:
    lib = ifcopenshell._get_lib()
    _configure_attr_lib(lib)
    count = ctypes.c_uint32(0)
    arr = lib.ifcopenshell_util_attribute_get_enum_items(attribute._ptr, ctypes.byref(count))
    if not arr or count.value == 0:
        if arr:
            lib.ifcopenshell_free_string_array(arr, count.value)
        return ()
    try:
        return tuple(arr[i].decode("utf-8") for i in range(count.value))
    finally:
        lib.ifcopenshell_free_string_array(arr, count.value)


def get_select_items(attribute: ifcopenshell_wrapper.attribute) -> tuple[ifcopenshell_wrapper.declaration, ...]:
    named_type = attribute.type_of_attribute().as_named_type()
    assert named_type
    select_type = named_type.declared_type().as_select_type()
    assert select_type
    return select_type.select_list()
