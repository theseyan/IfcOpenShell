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


# ifcopenshell_value_kind_t constants (must match value.h)
_IFCSEL_VALUE_NONE     = 0
_IFCSEL_VALUE_BOOL     = 1
_IFCSEL_VALUE_INT      = 2
_IFCSEL_VALUE_DOUBLE   = 3
_IFCSEL_VALUE_STRING   = 4
_IFCSEL_VALUE_INSTANCE = 5
_IFCSEL_VALUE_LIST     = 6
_IFCSEL_VALUE_DICT     = 7

_value_lib_configured = False
_filter_lib_configured = False
_format_lib_configured = False
_keys_lib_configured = False


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


def _configure_format_lib(lib) -> None:
    global _format_lib_configured
    if _format_lib_configured:
        return
    lib.ifcopenshell_selector_format.restype = ctypes.c_void_p
    lib.ifcopenshell_selector_format.argtypes = [
        ctypes.c_void_p, ctypes.c_void_p, ctypes.c_char_p
    ]
    lib.ifcopenshell_free_string.restype = None
    lib.ifcopenshell_free_string.argtypes = [ctypes.c_void_p]
    _format_lib_configured = True


def _configure_keys_lib(lib) -> None:
    global _keys_lib_configured
    if _keys_lib_configured:
        return
    lib.ifcopenshell_selector_parse_keys.restype = ctypes.c_void_p
    lib.ifcopenshell_selector_parse_keys.argtypes = [ctypes.c_char_p]
    lib.ifcopenshell_selector_keys_count.restype = ctypes.c_uint32
    lib.ifcopenshell_selector_keys_count.argtypes = [ctypes.c_void_p]
    lib.ifcopenshell_selector_keys_get.restype = ctypes.c_char_p
    lib.ifcopenshell_selector_keys_get.argtypes = [ctypes.c_void_p, ctypes.c_uint32]
    lib.ifcopenshell_selector_keys_is_regex.restype = ctypes.c_bool
    lib.ifcopenshell_selector_keys_is_regex.argtypes = [ctypes.c_void_p, ctypes.c_uint32]
    lib.ifcopenshell_selector_keys_free.restype = None
    lib.ifcopenshell_selector_keys_free.argtypes = [ctypes.c_void_p]
    _keys_lib_configured = True


def _value_to_python(lib, ptr, element):
    """Recursively convert an ifcopenshell_value_t* to a Python object."""
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



def format(query: str, element: Optional[ifcopenshell.entity_instance] = None) -> Optional[str]:
    """Format a query string with optional element context for variable substitution.

    :param query: Format query string (can include {{variable}} placeholders)
    :param element: Optional IFC element for variable substitution
    :return: Formatted string, or ``None`` if the query evaluated to None
        (e.g. ``{{undefined}}`` with no matching element).

    Example:
        format("{{z}} / 2", element)  # Substitutes element's z value
        format("imperial_length({{z}} / 2, 4)", element)  # Uses z in calculation
    """
    lib = _get_lib()
    _configure_format_lib(lib)
    file_ptr = None
    elem_ptr = None
    if element is not None:
        file_ptr = getattr(element.file, "_ptr", None)
        elem_ptr = element._handle
    raw = lib.ifcopenshell_selector_format(file_ptr, elem_ptr, query.encode("utf-8"))
    if not raw:
        return None
    try:
        result = ctypes.string_at(raw).decode("utf-8", errors="replace")
    finally:
        lib.ifcopenshell_free_string(raw)
    return result


def get_element_value(element: ifcopenshell.entity_instance, query: str) -> Any:
    lib = _get_lib()
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




def _parse_selector_keys(query: str) -> list[Union[str, re.Pattern]]:
    """Parse a get_element query into a list of keys (strings or compiled regexes)."""
    lib = _get_lib()
    _configure_keys_lib(lib)
    h = lib.ifcopenshell_selector_parse_keys(query.encode("utf-8"))
    if not h:
        err = lib.ifcopenshell_last_error_message()
        err_str = err.decode("utf-8", errors="replace") if err else "unknown"
        raise ValueError(f"selector parse error: {err_str}")
    try:
        n = lib.ifcopenshell_selector_keys_count(h)
        keys: list[Union[str, re.Pattern]] = []
        for i in range(n):
            raw = lib.ifcopenshell_selector_keys_get(h, i)
            text = raw.decode("utf-8", errors="replace") if raw else ""
            if lib.ifcopenshell_selector_keys_is_regex(h, i):
                keys.append(re.compile(text))
            else:
                keys.append(text)
    finally:
        lib.ifcopenshell_selector_keys_free(h)
    return keys




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
        keys = _parse_selector_keys(query)

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


