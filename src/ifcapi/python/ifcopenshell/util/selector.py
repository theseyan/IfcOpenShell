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

import re
from collections.abc import Iterable
from decimal import Decimal
from types import EllipsisType
from typing import Any, Optional, Union

import ctypes

import numpy as np

import ifcopenshell.api.geometry
import ifcopenshell.api.pset
import ifcopenshell.util
import ifcopenshell.util.attribute
import ifcopenshell.util.classification
import ifcopenshell.util.element
import ifcopenshell.util.geolocation
import ifcopenshell.util.placement
import ifcopenshell.util.pset
import ifcopenshell.util.schema
import ifcopenshell.util.shape
import ifcopenshell.util.system
import ifcopenshell.util.unit
from ifcopenshell import _get_lib


# Node-kind constants mirroring selector_ast.h
_NK_TOKEN_FIRST = 100

_NK_RULE_NAMES = {
    0:  "start",           1:  "filter_group",    2:  "facet_list",
    3:  "facet",           4:  "instance",         5:  "entity",
    6:  "attribute",       7:  "type",             8:  "material",
    9:  "query",           10: "classification",   11: "location",
    12: "group",           13: "parent",           14: "property",
    15: "pset",            16: "prop",             17: "keys",
    18: "attribute_name",  19: "ifc_class",        20: "globalid",
    21: "value",           22: "unquoted_string",  23: "regex_string",
    24: "quoted_string",   25: "special",          26: "null",
    27: "true",            28: "false",            29: "comparison",
    30: "not",             31: "equals",           32: "morethanequalto",
    33: "lessthanequalto", 34: "morethan",         35: "lessthan",
    36: "contains",        37: "keys",             38: "key",
    39: "add",             40: "subtract",         41: "multiply",
    42: "divide",          43: "function",         44: "variable",
    45: "query_path",      46: "round",            47: "number",
    48: "int",             49: "format_length",    50: "metric_length",
    51: "imperial_length", 52: "lower",            53: "upper",
    54: "title",           55: "concat",           56: "substr",
    57: "sort",            58: "reverse",          59: "join",
    60: "boolean",
}

_NK_TOKEN_NAMES = {
    100: "ESCAPED_STRING", 101: "SIGNED_NUMBER", 102: "NUMBER",
    103: "SIGNED_INT",     104: "TRUE",          105: "FALSE",
    106: "__ANON_0",
}


class SelectorToken(str):
    """Leaf node from the native C parser. A str subclass with .type and .value."""

    __slots__ = ("type",)

    def __new__(cls, type_: str, value: str):
        instance = str.__new__(cls, value)
        instance.type = type_
        return instance

    @property
    def value(self) -> str:
        return str(self)

    def __repr__(self) -> str:
        return f"Token({self.type!r}, {str(self)!r})"


class SelectorNode:
    """Rule node from the native C parser. Provides .data and .children."""

    __slots__ = ("data", "children")

    def __init__(self, data: str, children: list):
        self.data = data
        self.children = children

    def __repr__(self) -> str:
        return f"Tree({self.data!r}, {self.children!r})"


_selector_lib_configured = False
_value_lib_configured = False
_filter_lib_configured = False

# ifcopenshell_value_kind_t constants (must match value.h)
_IFCSEL_VALUE_NONE     = 0
_IFCSEL_VALUE_BOOL     = 1
_IFCSEL_VALUE_INT      = 2
_IFCSEL_VALUE_DOUBLE   = 3
_IFCSEL_VALUE_STRING   = 4
_IFCSEL_VALUE_INSTANCE = 5
_IFCSEL_VALUE_LIST     = 6
_IFCSEL_VALUE_DICT     = 7


def _configure_selector_lib(lib) -> None:
    global _selector_lib_configured
    if _selector_lib_configured:
        return
    lib.ifcopenshell_selector_parse_filter.restype = ctypes.c_void_p
    lib.ifcopenshell_selector_parse_filter.argtypes = [ctypes.c_char_p]
    lib.ifcopenshell_selector_parse_get_element.restype = ctypes.c_void_p
    lib.ifcopenshell_selector_parse_get_element.argtypes = [ctypes.c_char_p]
    lib.ifcopenshell_selector_parse_format.restype = ctypes.c_void_p
    lib.ifcopenshell_selector_parse_format.argtypes = [ctypes.c_char_p]
    lib.ifcopenshell_selector_node_kind.restype = ctypes.c_int
    lib.ifcopenshell_selector_node_kind.argtypes = [ctypes.c_void_p]
    lib.ifcopenshell_selector_node_child_count.restype = ctypes.c_size_t
    lib.ifcopenshell_selector_node_child_count.argtypes = [ctypes.c_void_p]
    lib.ifcopenshell_selector_node_child.restype = ctypes.c_void_p
    lib.ifcopenshell_selector_node_child.argtypes = [ctypes.c_void_p, ctypes.c_size_t]
    lib.ifcopenshell_selector_node_text.restype = ctypes.c_char_p
    lib.ifcopenshell_selector_node_text.argtypes = [ctypes.c_void_p]
    lib.ifcopenshell_selector_node_free.restype = None
    lib.ifcopenshell_selector_node_free.argtypes = [ctypes.c_void_p]
    _selector_lib_configured = True


def _configure_value_lib(lib) -> None:
    global _value_lib_configured
    if _value_lib_configured:
        return
    lib.ifcopenshell_selector_get_element_value.restype = ctypes.c_void_p
    lib.ifcopenshell_selector_get_element_value.argtypes = [
        ctypes.c_void_p, ctypes.c_void_p, ctypes.c_char_p
    ]
    lib.ifcopenshell_value_free.restype = None
    lib.ifcopenshell_value_free.argtypes = [ctypes.c_void_p]
    lib.ifcopenshell_value_kind.restype = ctypes.c_int
    lib.ifcopenshell_value_kind.argtypes = [ctypes.c_void_p]
    lib.ifcopenshell_value_as_bool.restype = ctypes.c_bool
    lib.ifcopenshell_value_as_bool.argtypes = [ctypes.c_void_p]
    lib.ifcopenshell_value_as_int64.restype = ctypes.c_int64
    lib.ifcopenshell_value_as_int64.argtypes = [ctypes.c_void_p]
    lib.ifcopenshell_value_as_double.restype = ctypes.c_double
    lib.ifcopenshell_value_as_double.argtypes = [ctypes.c_void_p]
    lib.ifcopenshell_value_as_string.restype = ctypes.c_char_p
    lib.ifcopenshell_value_as_string.argtypes = [ctypes.c_void_p]
    lib.ifcopenshell_value_as_instance.restype = ctypes.c_void_p
    lib.ifcopenshell_value_as_instance.argtypes = [ctypes.c_void_p]
    lib.ifcopenshell_value_list_size.restype = ctypes.c_size_t
    lib.ifcopenshell_value_list_size.argtypes = [ctypes.c_void_p]
    lib.ifcopenshell_value_list_at.restype = ctypes.c_void_p
    lib.ifcopenshell_value_list_at.argtypes = [ctypes.c_void_p, ctypes.c_size_t]
    lib.ifcopenshell_value_dict_size.restype = ctypes.c_size_t
    lib.ifcopenshell_value_dict_size.argtypes = [ctypes.c_void_p]
    lib.ifcopenshell_value_dict_key_at.restype = ctypes.c_char_p
    lib.ifcopenshell_value_dict_key_at.argtypes = [ctypes.c_void_p, ctypes.c_size_t]
    lib.ifcopenshell_value_dict_value_at.restype = ctypes.c_void_p
    lib.ifcopenshell_value_dict_value_at.argtypes = [ctypes.c_void_p, ctypes.c_size_t]
    _value_lib_configured = True


def _configure_filter_lib(lib) -> None:
    global _filter_lib_configured
    if _filter_lib_configured:
        return
    lib.ifcopenshell_selector_filter_elements.restype = ctypes.c_void_p
    lib.ifcopenshell_selector_filter_elements.argtypes = [
        ctypes.c_void_p,   # ifcopenshell_ifc_file_t*
        ctypes.c_char_p,   # query
        ctypes.c_void_p,   # ifcopenshell_ifc_instance_t* const* elements
        ctypes.c_size_t,   # elements_count
        ctypes.c_int,      # edit_in_place
    ]
    _filter_lib_configured = True


def _value_to_python(lib, ptr, element):
    """Recursively convert an ifcopenshell_value_t* to a Python object.

    *ptr* is a ctypes c_void_p integer.  Children are owned by their parent
    and must NOT be freed individually.
    """
    if not ptr:
        return None
    kind = lib.ifcopenshell_value_kind(ptr)
    if kind == _IFCSEL_VALUE_NONE:
        return None
    if kind == _IFCSEL_VALUE_BOOL:
        return bool(lib.ifcopenshell_value_as_bool(ptr))
    if kind == _IFCSEL_VALUE_INT:
        return int(lib.ifcopenshell_value_as_int64(ptr))
    if kind == _IFCSEL_VALUE_DOUBLE:
        return float(lib.ifcopenshell_value_as_double(ptr))
    if kind == _IFCSEL_VALUE_STRING:
        raw = lib.ifcopenshell_value_as_string(ptr)
        return raw.decode("utf-8", errors="replace") if raw else None
    if kind == _IFCSEL_VALUE_INSTANCE:
        h = lib.ifcopenshell_value_as_instance(ptr)
        if not h:
            return None
        from ifcopenshell.entity_instance import entity_instance as _ei
        return _ei(element.file, h)
    if kind == _IFCSEL_VALUE_LIST:
        n = lib.ifcopenshell_value_list_size(ptr)
        return [
            _value_to_python(lib, lib.ifcopenshell_value_list_at(ptr, i), element)
            for i in range(n)
        ]
    if kind == _IFCSEL_VALUE_DICT:
        n = lib.ifcopenshell_value_dict_size(ptr)
        result = {}
        for i in range(n):
            key_raw = lib.ifcopenshell_value_dict_key_at(ptr, i)
            key = key_raw.decode("utf-8", errors="replace") if key_raw else ""
            val = _value_to_python(lib, lib.ifcopenshell_value_dict_value_at(ptr, i), element)
            result[key] = val
        return result
    return None

def _build_tree(lib, ptr: int) -> "SelectorNode | SelectorToken":
    """Recursively build a SelectorNode / SelectorToken tree from a C AST pointer."""
    kind = lib.ifcopenshell_selector_node_kind(ptr)
    if kind >= _NK_TOKEN_FIRST:
        tok_type = _NK_TOKEN_NAMES.get(kind, "__ANON_0")
        raw = lib.ifcopenshell_selector_node_text(ptr)
        text = raw.decode("utf-8", errors="replace") if raw else ""
        return SelectorToken(tok_type, text)
    rule_name = _NK_RULE_NAMES.get(kind, f"__unknown_{kind}")
    n = lib.ifcopenshell_selector_node_child_count(ptr)
    children = [_build_tree(lib, lib.ifcopenshell_selector_node_child(ptr, i)) for i in range(n)]
    return SelectorNode(rule_name, children)


def _native_parse(query: str, kind: str) -> SelectorNode:
    """Parse *query* with the native C parser and return a SelectorNode tree.

    *kind* is one of ``"filter"``, ``"get_element"``, or ``"format"``.
    Raises ``ValueError`` on a parse error.
    """
    lib = _get_lib()
    _configure_selector_lib(lib)
    encoded = query.encode("utf-8")
    if kind == "filter":
        root_ptr = lib.ifcopenshell_selector_parse_filter(encoded)
    elif kind == "get_element":
        root_ptr = lib.ifcopenshell_selector_parse_get_element(encoded)
    else:
        root_ptr = lib.ifcopenshell_selector_parse_format(encoded)
    if not root_ptr:
        err = lib.ifcopenshell_last_error_message()
        err_str = err.decode("utf-8", errors="replace") if err else "unknown"
        raise ValueError(f"selector parse error: {err_str}")
    try:
        tree = _build_tree(lib, root_ptr)
    finally:
        lib.ifcopenshell_selector_node_free(root_ptr)
    return tree


def _selector_transform(tree: "SelectorNode | SelectorToken", transformer) -> object:
    """Bottom-up tree transformation.

    Dispatches terminal nodes to same-named methods on *transformer* (e.g.
    ESCAPED_STRING), and rule nodes to same-named methods (e.g. attribute).
    Returns the transformed value.
    """
    if isinstance(tree, SelectorToken):
        method = getattr(transformer, tree.type, None)
        if method is not None:
            return method(tree)
        return tree
    transformed = [_selector_transform(child, transformer) for child in tree.children]
    method = getattr(transformer, tree.data, None)
    if method is not None:
        return method(transformed)
    return SelectorNode(tree.data, transformed)


class FormatTransformer:
    def __init__(self, element=None):
        self.element = element

    def transform(self, tree):
        return _selector_transform(tree, self)

    def start(self, args):
        if isinstance(args[0], (list, tuple)):
            return ", ".join(args[0])
        return args[0]

    def expression(self, args):
        return args[0]

    def variable(self, args):
        """Handle variable substitution like {{z}} or {{Pset_Wall.FireRating}}"""
        if self.element:
            try:
                return get_element_value(self.element, args[0])
            except:
                pass

    def query_path(self, args):
        """Extract the query path from variable"""
        return str(args[0]).strip()

    def add(self, args):
        """Handle addition operation"""
        left, right = args
        try:
            left_val = float(left) if left != "None" and left is not None else 0.0
            right_val = float(right) if right != "None" and right is not None else 0.0
            result = left_val + right_val
            # Return integer if result has no decimal part
            if result % 1 == 0:
                return str(int(result))
            return str(result)
        except (ValueError, TypeError):
            # If can't convert to numbers, concatenate as strings
            return str(left) + str(right)

    def subtract(self, args):
        """Handle subtraction operation"""
        left, right = args
        left_val = float(left) if left != "None" and left is not None else 0.0
        right_val = float(right) if right != "None" and right is not None else 0.0
        result = left_val - right_val
        if result % 1 == 0:
            return str(int(result))
        return str(result)

    def multiply(self, args):
        """Handle multiplication operation"""
        left, right = args
        left_val = float(left) if left != "None" and left is not None else 0.0
        right_val = float(right) if right != "None" and right is not None else 0.0
        result = left_val * right_val
        if result % 1 == 0:
            return str(int(result))
        return str(result)

    def divide(self, args):
        """Handle division operation"""
        left, right = args
        left_val = float(left) if left != "None" and left is not None else 0.0
        right_val = float(right) if right != "None" and right is not None else 1.0
        if right_val == 0:
            return "inf"  # or raise an error, or return "0"
        result = left_val / right_val
        if result % 1 == 0:
            return str(int(result))
        return str(result)

    def function(self, args):
        return args[0]

    def ESCAPED_STRING(self, args):
        return args[1:-1].replace("\\", "")

    def NUMBER(self, args):
        return str(args)

    def lower(self, args):
        return str(args[0]).lower()

    def upper(self, args):
        return str(args[0]).upper()

    def title(self, args):
        return str(args[0]).title()

    def concat(self, args):
        return "".join(str(arg) for arg in args)

    def substr(self, args):
        if len(args) == 3:
            if args[2] is None:
                return str(args[0])[int(args[1]) :]
            return str(args[0])[int(args[1]) : int(args[2])]
        elif len(args) == 2:
            return str(args[0])[int(args[1]) :]

    def sort(self, args):
        return sorted(args[0])

    def reverse(self, args):
        return list(reversed(args[0]))

    def join(self, args):
        return args[0].join(args[1])

    def boolean(self, args):
        if not args:
            return True
        token = args[0]
        if hasattr(token, "type"):
            return token.type == "TRUE"
        value = str(token).lower()
        if hasattr(token, "value"):
            value = str(token.value).lower()
        return value in ("true", "1", "yes")

    def round(self, args):
        value = Decimal(0.0 if args[0] == "None" else args[0] or 0.0)
        nearest = Decimal(args[1])
        result = round(value / nearest) * nearest
        if nearest % 1 == 0:
            return str(int(result))
        return str(result)

    def number(self, args):
        arg_val = args[0]
        if isinstance(arg_val, str):
            arg_val = float(arg_val) if "." in arg_val else int(arg_val)
        if len(args) >= 3 and args[2]:
            return "{:,}".format(arg_val).replace(".", "*").replace(",", args[2]).replace("*", args[1])
        elif len(args) >= 2 and args[1]:
            return "{}".format(arg_val).replace(".", args[1])
        return "{:,}".format(arg_val)

    def format_length(self, args):
        return args[0]

    def metric_length(self, args):
        value, precision, decimal_places = args
        return ifcopenshell.util.unit.format_length(
            float(value), float(precision), int(decimal_places), unit_system="metric"
        )

    def imperial_length(self, args):
        args = list(filter(lambda x: x is not None, args))
        if len(args) == 2:
            input_unit, output_unit = "foot", "foot"
            value, precision = args
            suppress_zero_inches = True
        elif len(args) == 3:
            value, precision, suppress_zero_inches = args
            input_unit, output_unit = "foot", "foot"
        elif len(args) == 4:
            value, precision, input_unit, output_unit = args
            input_unit = "inch" if input_unit == "inch" else "foot"
            output_unit = "inch" if output_unit == "inch" else "foot"
            suppress_zero_inches = True
        else:
            value, precision, input_unit, output_unit, suppress_zero_inches = args
            input_unit = "inch" if input_unit == "inch" else "foot"
            output_unit = "inch" if output_unit == "inch" else "foot"

        return ifcopenshell.util.unit.format_length(
            float(value),
            int(precision),
            suppress_zero_inches=(suppress_zero_inches if suppress_zero_inches is not None else False),
            unit_system="imperial",
            input_unit=input_unit,
            output_unit=output_unit,
        )

    def int(self, args: list[str]) -> str:
        value = 0.0 if args[0] == "None" else args[0] or 0.0
        return str(int(float(value)))


class GetElementTransformer:
    def transform(self, tree):
        return _selector_transform(tree, self)

    def start(self, args):
        return args[0]

    def keys(self, args):
        return args

    def key(self, args):
        return args[0]

    def quoted_string(self, args):
        return str(args[0])

    def regex_string(self, args):
        return re.compile(args[0])

    def unquoted_string(self, args):
        return str(args[0])

    def ESCAPED_STRING(self, args):
        return args[1:-1].replace("\\", "")


def format(query: str, element: Optional[ifcopenshell.entity_instance] = None) -> str:
    """Format a query string with optional element context for variable substitution.

    :param query: Format query string (can include {{variable}} placeholders)
    :param element: Optional IFC element for variable substitution
    :return: Formatted string

    Example:
        format("{{z}} / 2", element)  # Substitutes element's z value
        format("imperial_length({{z}} / 2, 4)", element)  # Uses z in calculation
    """
    return FormatTransformer(element).transform(_native_parse(query, "format"))


def get_element_value(element: ifcopenshell.entity_instance, query: str) -> Any:
    lib = _get_lib()
    _configure_selector_lib(lib)
    _configure_value_lib(lib)
    file_ptr = getattr(element.file, "_ptr", None)
    ptr = lib.ifcopenshell_selector_get_element_value(
        file_ptr, element._handle, query.encode("utf-8")
    )
    if ptr is None:
        return None
    result = _value_to_python(lib, ptr, element)
    lib.ifcopenshell_value_free(ptr)
    return result


def _get_element_value(element: ifcopenshell.entity_instance, keys: list[str]) -> Any:
    value = element
    for key in keys:
        if value is None:
            return
        if key == "type":
            value = ifcopenshell.util.element.get_type(value)
        elif key in ("material", "mat"):
            value = ifcopenshell.util.element.get_material(value, should_skip_usage=True)
        elif key in ("materials", "mats"):
            value = ifcopenshell.util.element.get_materials(value)
        elif key == "profiles":
            value = ifcopenshell.util.shape.get_profiles(value)
        elif key == "styles":
            value = ifcopenshell.util.element.get_styles(value)
        elif key in ("item", "i"):
            if value.is_a("IfcMaterialLayerSet"):
                value = value.MaterialLayers
            elif value.is_a("IfcMaterialProfileSet"):
                value = value.MaterialProfiles
            elif value.is_a("IfcMaterialConstituentSet"):
                value = value.MaterialConstituents
        elif key == "container":
            value = ifcopenshell.util.element.get_container(value)
        elif key == "space":
            value = ifcopenshell.util.element.get_parent(value, ifc_class="IfcSpace")
        elif key == "storey":
            value = ifcopenshell.util.element.get_parent(value, ifc_class="IfcBuildingStorey")
        elif key == "building":
            value = ifcopenshell.util.element.get_parent(value, ifc_class="IfcBuilding")
        elif key == "site":
            value = ifcopenshell.util.element.get_parent(value, ifc_class="IfcSite")
        elif key == "parent":
            value = ifcopenshell.util.element.get_parent(value)
        elif key in ("types", "occurrences"):
            value = ifcopenshell.util.element.get_types(value)
        elif key == "count":
            if isinstance(value, set):
                value = len(list(value))
            elif isinstance(value, (list, tuple)):
                value = len(value)
            else:
                value = 1
        elif key == "class":
            value = value.is_a()
        elif key == "predefined_type":
            value = ifcopenshell.util.element.get_predefined_type(value)
        elif key == "id":
            value = value.id()
        elif key == "classification":
            value = ifcopenshell.util.classification.get_references(value)
        elif key == "group":
            value = ifcopenshell.util.element.get_groups(value)
        elif key == "system":
            value = ifcopenshell.util.system.get_element_systems(value)
        elif key == "zone":
            value = ifcopenshell.util.system.get_element_zones(value)
        elif key in ("x", "y", "z", "easting", "northing", "elevation") and hasattr(value, "ObjectPlacement"):
            if getattr(value, "ObjectPlacement", None):
                matrix = ifcopenshell.util.placement.get_local_placement(value.ObjectPlacement)
                xyz = matrix[:, 3][:3]
                if key in ("x", "y", "z"):
                    value = xyz["xyz".index(key)]
                else:
                    enh = ifcopenshell.util.geolocation.auto_xyz2enh(element.wrapped_data.file, *xyz)
                    value = enh[("easting", "northing", "elevation").index(key)]
            else:
                value = None
        elif isinstance(value, ifcopenshell.entity_instance):
            if key == "Name" and value.is_a("IfcMaterialLayerSet"):
                key = "LayerSetName"  # This oddity in the IFC spec is annoying so we account for it.

            if isinstance(key, re.Pattern):
                attribute = None  # Should we support regex attributes? Probably not for now.
            else:
                attribute = getattr(value, key, None)

            if attribute is not None:
                value = attribute
            else:
                # Try to extract pset
                if isinstance(key, re.Pattern):
                    psets = ifcopenshell.util.element.get_psets(value)
                    matching_psets = []
                    for pset_name, pset in psets.items():
                        if key.match(pset_name):
                            del pset["id"]
                            matching_psets.append(pset)
                    result = matching_psets or None
                    if result and len(result) == 1:
                        result = result[0]
                else:
                    result = ifcopenshell.util.element.get_pset(value, key)
                    if result:
                        del result["id"]

                value = result
        elif isinstance(value, dict):  # Such as from the result of a prior get_pset
            if isinstance(key, re.Pattern):
                results = []
                for prop_name, prop_value in value.items():
                    if key.match(prop_name):
                        if isinstance(prop_value, (list, tuple)):
                            results.extend(prop_value)
                        else:
                            results.append(prop_value)
                value = results or None
                if value and len(value) == 1:
                    value = value[0]
            else:
                value = value.get(key, None)
        elif isinstance(value, (list, tuple, set)):  # If we use regex
            if isinstance(key, str) and key.isnumeric():
                try:
                    value = value[int(key)]
                except IndexError:
                    return
            else:
                results = []
                for v in value:
                    subvalue = _get_element_value(v, [key])
                    if isinstance(subvalue, list):
                        results.extend(subvalue)
                    else:
                        results.append(subvalue)
                value = results
    return value


def filter_elements(
    ifc_file: ifcopenshell.file,
    query: str,
    elements: Optional[set[ifcopenshell.entity_instance]] = None,
    edit_in_place=False,
) -> set[ifcopenshell.entity_instance]:
    """
    Filter elements based on the provided `query`.

    :param ifc_file: The IFC file object
    :param query: Query to execute
    :param elements: Base set of IFC elements for the query. If not provided,
        all elements in the IFC are queried. If provided, the query will be
        applied to this set of elements, so the result will be a subset of
        elements.
    :param edit_in_place: If `True`, mutate the provided `elements` in place. Defaults to `False`
    :return: Set of filtered elements

    Example:

    .. code:: python

        # Select all the walls and slabs in the file.
        elements = ifcopenshell.util.selector.filter_elements(ifc_file, "IfcWall, IfcSlab")

        # Add doors to the elements too.
        elements = ifcopenshell.util.selector.filter_elements(ifc_file, "IfcDoor", elements)

        # Changed our mind, exclude the slabs.
        elements = ifcopenshell.util.selector.filter_elements(ifc_file, "! IfcSlab", elements)

        # {#1=IfcWall(...), #2=IfcDoor(...)}
        print(elements)
    """
    if not query:
        return elements or set()

    lib = _get_lib()
    _configure_value_lib(lib)
    _configure_filter_lib(lib)

    from ifcopenshell.entity_instance import entity_instance as _ei

    # Build the optional elements array.
    elem_list = list(elements) if elements else []
    if elem_list:
        arr = (ctypes.c_void_p * len(elem_list))(*[e._handle for e in elem_list])
        elem_ptr = ctypes.cast(arr, ctypes.c_void_p)
        elem_count = len(elem_list)
    else:
        arr = None
        elem_ptr = None
        elem_count = 0

    val_ptr = lib.ifcopenshell_selector_filter_elements(
        ifc_file._ptr,
        query.encode("utf-8"),
        elem_ptr,
        elem_count,
        int(edit_in_place),
    )

    if not val_ptr:
        return set()

    result: set[ifcopenshell.entity_instance] = set()
    n = lib.ifcopenshell_value_list_size(val_ptr)
    for i in range(n):
        item_ptr = lib.ifcopenshell_value_list_at(val_ptr, i)
        if item_ptr and lib.ifcopenshell_value_kind(item_ptr) == _IFCSEL_VALUE_INSTANCE:
            h = lib.ifcopenshell_value_as_instance(item_ptr)
            if h:
                result.add(_ei(ifc_file, h))

    lib.ifcopenshell_value_free(val_ptr)

    if edit_in_place and elements is not None:
        elements.clear()
        elements.update(result)
        return elements
    return result


class SetElementValueException(Exception): ...


def set_element_value(
    ifc_file: ifcopenshell.file,
    element: Union[
        ifcopenshell.entity_instance,
        dict[str, Any],
        Iterable[ifcopenshell.entity_instance],
        None,
    ],
    query: Union[str, list[str]],
    value: Any,
    *,
    concat: str = ", ",
) -> None:
    """Set element value based on the provided query.

    :param element: IFC element to change.
    :param query: String query to identify the attribute to change.
    :param value: Value to set.
    :param concat: Concatenation symbol, used only to deserialize property
        set enum values from string values.
    """
    original_element = element
    if isinstance(query, (list, tuple)):
        keys = query
    else:
        keys = GetElementTransformer().transform(_native_parse(query, "get_element"))

    for i, key in enumerate(keys):
        if element is None:
            return
        if key == "type":
            element = ifcopenshell.util.element.get_type(element)
        elif key in ("material", "mat"):
            element = ifcopenshell.util.element.get_material(element, should_skip_usage=True)
        elif key in ("materials", "mats"):
            element = ifcopenshell.util.element.get_materials(element)
        elif key == "styles":
            element = ifcopenshell.util.element.get_styles(element)
        elif key in ("item", "i"):
            if element.is_a("IfcMaterialLayerSet"):
                element = element.MaterialLayers
            elif element.is_a("IfcMaterialProfileSet"):
                element = element.MaterialProfiles
            elif element.is_a("IfcMaterialConstituentSet"):
                element = element.MaterialConstituents
        elif key == "container":
            element = ifcopenshell.util.element.get_container(element)
        elif key == "space":
            element = ifcopenshell.util.element.get_container(element, ifc_class="IfcSpace")
        elif key == "storey":
            element = ifcopenshell.util.element.get_container(element, ifc_class="IfcBuildingStorey")
        elif key == "building":
            element = ifcopenshell.util.element.get_container(element, ifc_class="IfcBuilding")
        elif key == "site":
            element = ifcopenshell.util.element.get_container(element, ifc_class="IfcSite")
        elif key == "parent":
            element = ifcopenshell.util.element.get_parent(element)
        elif key == "class":
            if element.is_a().lower() != value.lower():
                return ifcopenshell.util.schema.reassign_class(ifc_file, element, value)
            return
        elif key == "id":
            return
        elif key == "predefined_type":
            current_value = ifcopenshell.util.element.get_predefined_type(element)
            if current_value == value:
                return

            def set_predefined_type(
                element: ifcopenshell.entity_instance, value: Union[str, None], *, is_type: bool
            ) -> None:
                predefined_type = element.PredefinedType
                declaration = element.wrapped_data.declaration()
                entity = declaration.as_entity()
                enum_attr = next(attr for attr in entity.attributes() if attr.name() == "PredefinedType")
                enum_items = ifcopenshell.util.attribute.get_enum_items(enum_attr)

                # USERDEFINED shouldn't occur here, if it does then it means
                # then it was artificially added and PredefinedType is actually unset.
                if value in (None, "NOTDEFINED", "USERDEFINED"):
                    element.PredefinedType = "NOTDEFINED"
                    setattr(element, "ElementType" if is_type else "ObjectType", None)
                elif value in enum_items:
                    if predefined_type == value:
                        return
                    element.PredefinedType = value
                    return

                # Value not in PredefinedType enum items.
                if predefined_type != "USERDEFINED":
                    element.PredefinedType = "USERDEFINED"
                setattr(element, "ElementType" if is_type else "ObjectType", value)
                return

            if element_type := ifcopenshell.util.element.get_type(element):
                set_predefined_type(element_type, value, is_type=True)
                return
            set_predefined_type(element, value, is_type=False)
            return
        elif key == "classification":
            element = ifcopenshell.util.classification.get_references(element)
        elif key in ("x", "y", "z", "easting", "northing", "elevation") and hasattr(element, "ObjectPlacement"):
            # TODO: add support
            if key in ("easting", "northing", "elevation"):
                return

            placement = element.ObjectPlacement
            if placement is None:
                matrix = np.eye(4)
            else:
                matrix = ifcopenshell.util.placement.get_local_placement(placement)

            # check if value is within tolerance to avoid api calls
            coord_i = "xyz".index(key)
            prev_value = matrix[coord_i][3]
            new_value = float(value) if value else 0.0
            if ifcopenshell.util.shape.is_x(new_value, prev_value):
                return

            matrix[coord_i][3] = new_value
            ifcopenshell.api.geometry.edit_object_placement(ifc_file, product=element, matrix=matrix, is_si=False)
            return
        elif isinstance(element, ifcopenshell.entity_instance):
            if key == "Name" and element.is_a("IfcMaterialLayerSet"):
                key = "LayerSetName"  # This oddity in the IFC spec is annoying so we account for it.

            if isinstance(key, str) and ((current_value := getattr(element, key, ...)) is not ...):
                # check if key is not last
                if len(keys) != i + 1:
                    element = current_value
                    continue

                if current_value == value:
                    return
                else:
                    # check if key is not last
                    try:
                        # Try our luck
                        return setattr(element, key, value)
                    except:
                        # Try to cast
                        data_type = ifcopenshell.util.attribute.get_primitive_type(
                            element.wrapped_data.declaration()
                            .as_entity()
                            .attribute_by_index(element.wrapped_data.get_argument_index(key))
                        )
                        if data_type == "string":
                            value = str(value)
                        elif data_type == "float":
                            value = float(value)
                        elif data_type == "integer":
                            value = int(value)
                        elif data_type == "boolean":
                            if value in ("True", "true", "TRUE", "Yes", "1"):
                                value = True
                            elif value in ("False", "false", "FALSE", "No", "0"):
                                value = False
                            else:
                                value = bool(value)
                        elif data_type == "entity":
                            value = ifc_file.by_guid(value)
                        if current_value == value:
                            return
                        return setattr(element, key, value)
            else:
                # Try to extract pset
                if isinstance(key, re.Pattern):
                    psets = ifcopenshell.util.element.get_psets(element)
                    matching_psets = []
                    for pset_name, pset in psets.items():
                        if key.match(pset_name):
                            matching_psets.append(pset)
                    result = matching_psets or None
                    if result and len(result) == 1:
                        result = result[0]
                else:
                    result = ifcopenshell.util.element.get_pset(element, key)

                    if value and not result and len(keys) == i + 2:  # The next key is the prop name
                        if "qto" in key.lower() or "quantity" in key.lower() or "quantities" in key.lower():
                            pset = ifcopenshell.api.pset.add_qto(ifc_file, product=element, name=key)
                        else:
                            pset = ifcopenshell.api.pset.add_pset(ifc_file, product=element, name=key)
                        result = {"id": pset.id()}

                element = result
        elif isinstance(element, dict):  # Such as from the result of a prior get_pset
            pset = ifc_file.by_id(element["id"])
            if isinstance(key, re.Pattern):
                for prop, prop_value in element.items():
                    if key.match(prop):
                        if pset.is_a("IfcPropertySet") and prop_value != value:
                            ifcopenshell.api.pset.edit_pset(ifc_file, pset=pset, properties={prop: value})
                        elif pset.is_a("IfcElementQuantity") and prop_value != float(value):
                            ifcopenshell.api.pset.edit_qto(ifc_file, qto=pset, properties={prop: float(value)})
            elif pset.is_a("IfcPropertySet") and element.get(key, None) != value:

                def process_pset_prop_value(
                    pset: ifcopenshell.entity_instance, prop: str, value: Any
                ) -> Union[Any, EllipsisType]:
                    """Try to process value for edit_pset.

                    `edit_pset` is expecting a sequence of values
                    for enum properties, not just a string of some-symbol-separated values.

                    Return `...` if property can be skipped as it has the same value.
                    """
                    if not isinstance(value, str):
                        return value

                    current_value = element.get(key, ...)
                    # Check if previous value is a list as a fast way to identify enum properties.
                    if not isinstance(current_value, (EllipsisType, list)):
                        return value

                    if isinstance(current_value, list):
                        # Value won't change, safe to skip editing IFC.
                        enum_values = value.split(concat)
                        if len(enum_values) == len(current_value) and set(enum_values) == set(current_value):
                            return ...

                    template = ifcopenshell.util.pset.get_template(ifc_file.schema_identifier)
                    pset_template = template.get_by_name(pset.Name)
                    if pset_template is None:
                        return value
                    for prop_template in pset_template.HasPropertyTemplates:
                        # 2 IfcSimplePropertyTemplate.Name
                        if prop_template[2] != prop:
                            continue

                        # 4 IfcSimplePropertyTemplate.TemplateType
                        if prop_template[4] != "P_ENUMERATEDVALUE":
                            # Not a enum property.
                            return value

                        # 7 IfcSimplePropertyTemplate.Enumerators
                        if (enumeration := prop_template[7]) is None:
                            # Enum property but without enumerators,
                            # make it a sequence to keep it assignable as a enum.
                            return (value,)

                        # 1 IfcPropertyEnumeration.EnumerationValues
                        available_enum_values = {v.wrappedValue for v in enumeration[1]}
                        if value in available_enum_values:
                            # Valid enum item, just keep it a sequence.
                            return (value,)

                        # Taking a wild guess that it's `concat` separated list.
                        enum_values = value.split(concat)
                        if not all(v in available_enum_values for v in enum_values):
                            raise Exception(
                                "Error setting pset enum property.\n"
                                f"Invalid enum values for property '{prop} in pset '{pset}': '{', '.join(enum_values)}'.\n"
                                f"Possible enum values for this property: {', '.join(available_enum_values)}."
                            )
                        return enum_values

                    # Couldn't find property template for this prop - delegate decision to edit_pset.
                    return value

                value = process_pset_prop_value(pset, key, value)
                if value == ...:
                    return
                ifcopenshell.api.pset.edit_pset(ifc_file, pset=pset, properties={key: value})
            elif pset.is_a("IfcElementQuantity"):
                try:
                    value = float(value)
                    if element.get(key, None) != value:
                        ifcopenshell.api.pset.edit_qto(ifc_file, qto=pset, properties={key: value})
                except:
                    pass
            return
        elif isinstance(element, (list, tuple, set)):  # If we use regex
            if key.isnumeric():
                try:
                    element = element[int(key)]
                except IndexError:
                    return
            else:
                for v in element:
                    set_element_value(ifc_file, v, keys[i:], value)
                return

    raise SetElementValueException(
        f"Failed to set value '{value}' for element '{original_element}' with query '{query}' (invalid or unsupported query)."
    )


