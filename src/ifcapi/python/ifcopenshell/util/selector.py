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
import ifcopenshell.util.placement
import ifcopenshell.util.pset
import ifcopenshell.util.schema
import ifcopenshell.util.shape
from ifcopenshell import _get_lib
from ifcopenshell._value_api import (
    IFCSEL_VALUE_NONE     as _IFCSEL_VALUE_NONE,
    IFCSEL_VALUE_BOOL     as _IFCSEL_VALUE_BOOL,
    IFCSEL_VALUE_INT      as _IFCSEL_VALUE_INT,
    IFCSEL_VALUE_DOUBLE   as _IFCSEL_VALUE_DOUBLE,
    IFCSEL_VALUE_STRING   as _IFCSEL_VALUE_STRING,
    IFCSEL_VALUE_INSTANCE as _IFCSEL_VALUE_INSTANCE,
    IFCSEL_VALUE_LIST     as _IFCSEL_VALUE_LIST,
    IFCSEL_VALUE_DICT     as _IFCSEL_VALUE_DICT,
    configure_value_lib   as _configure_value_lib_core,
    value_to_python       as _value_to_python,
)


_value_lib_configured = False
_filter_lib_configured = False
_format_lib_configured = False
_keys_lib_configured = False
_set_lib_configured = False


def _selector_error(lib, fallback: str) -> Exception:
    msg = lib.ifcopenshell_last_error_message()
    text = msg.decode("utf-8", errors="replace") if msg else fallback
    return ValueError(text)


def _configure_set_lib(lib) -> None:
    global _set_lib_configured
    if _set_lib_configured:
        return
    lib.ifcopenshell_selector_keylist_create.restype = ctypes.c_void_p
    lib.ifcopenshell_selector_keylist_create.argtypes = []
    lib.ifcopenshell_selector_keylist_destroy.restype = None
    lib.ifcopenshell_selector_keylist_destroy.argtypes = [ctypes.c_void_p]
    lib.ifcopenshell_selector_keylist_append_string.restype = None
    lib.ifcopenshell_selector_keylist_append_string.argtypes = [ctypes.c_void_p, ctypes.c_char_p]
    lib.ifcopenshell_selector_keylist_append_regex.restype = None
    lib.ifcopenshell_selector_keylist_append_regex.argtypes = [ctypes.c_void_p, ctypes.c_char_p]
    lib.ifcopenshell_util_selector_set_element_value.restype = ctypes.c_int
    lib.ifcopenshell_util_selector_set_element_value.argtypes = [
        ctypes.c_void_p, ctypes.c_void_p, ctypes.c_void_p, ctypes.c_void_p, ctypes.c_char_p,
    ]
    lib.ifcopenshell_value_new_none.restype = ctypes.c_void_p
    lib.ifcopenshell_value_new_none.argtypes = []
    lib.ifcopenshell_value_new_bool.restype = ctypes.c_void_p
    lib.ifcopenshell_value_new_bool.argtypes = [ctypes.c_bool]
    lib.ifcopenshell_value_new_int.restype = ctypes.c_void_p
    lib.ifcopenshell_value_new_int.argtypes = [ctypes.c_int64]
    lib.ifcopenshell_value_new_double.restype = ctypes.c_void_p
    lib.ifcopenshell_value_new_double.argtypes = [ctypes.c_double]
    lib.ifcopenshell_value_new_string.restype = ctypes.c_void_p
    lib.ifcopenshell_value_new_string.argtypes = [ctypes.c_char_p]
    lib.ifcopenshell_value_new_instance.restype = ctypes.c_void_p
    lib.ifcopenshell_value_new_instance.argtypes = [ctypes.c_void_p]
    lib.ifcopenshell_value_new_list.restype = ctypes.c_void_p
    lib.ifcopenshell_value_new_list.argtypes = []
    lib.ifcopenshell_value_list_append.restype = None
    lib.ifcopenshell_value_list_append.argtypes = [ctypes.c_void_p, ctypes.c_void_p]
    lib.ifcopenshell_value_free.restype = None
    lib.ifcopenshell_value_free.argtypes = [ctypes.c_void_p]
    _set_lib_configured = True


def _build_native_value(lib, value) -> Optional[int]:
    """Marshal a Python value to a freshly allocated ifcopenshell_value_t*."""
    if value is None:
        return lib.ifcopenshell_value_new_none()
    if isinstance(value, bool):
        return lib.ifcopenshell_value_new_bool(value)
    if isinstance(value, int):
        return lib.ifcopenshell_value_new_int(value)
    if isinstance(value, float):
        return lib.ifcopenshell_value_new_double(value)
    if isinstance(value, str):
        return lib.ifcopenshell_value_new_string(value.encode("utf-8"))
    if isinstance(value, ifcopenshell.entity_instance):
        return lib.ifcopenshell_value_new_instance(value._handle)
    if isinstance(value, (list, tuple, set)):
        list_ptr = lib.ifcopenshell_value_new_list()
        for item in value:
            item_ptr = _build_native_value(lib, item)
            lib.ifcopenshell_value_list_append(list_ptr, item_ptr)
        return list_ptr
    # Fallback: convert via str.
    return lib.ifcopenshell_value_new_string(str(value).encode("utf-8"))


def _configure_value_lib(lib) -> None:
    global _value_lib_configured
    if _value_lib_configured:
        return
    lib.ifcopenshell_selector_get_element_value.restype = ctypes.c_void_p
    lib.ifcopenshell_selector_get_element_value.argtypes = [
        ctypes.c_void_p, ctypes.c_void_p, ctypes.c_char_p
    ]
    _configure_value_lib_core(lib)
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
    lib.ifcopenshell_clear_error()
    file_ptr = None
    elem_ptr = None
    if element is not None:
        file_ptr = getattr(element.file, "_ptr", None)
        elem_ptr = element._handle
    raw = lib.ifcopenshell_selector_format(file_ptr, elem_ptr, query.encode("utf-8"))
    if not raw:
        msg = lib.ifcopenshell_last_error_message()
        if msg:
            raise _selector_error(lib, "selector format failed")
        return None
    try:
        result = ctypes.string_at(raw).decode("utf-8", errors="replace")
    finally:
        lib.ifcopenshell_free_string(raw)
    return result


def get_element_value(element: ifcopenshell.entity_instance, query: str) -> Any:
    _parse_selector_keys(query)
    lib = _get_lib()
    _configure_value_lib(lib)
    lib.ifcopenshell_clear_error()
    file_ptr = getattr(element.file, "_ptr", None)
    ptr = lib.ifcopenshell_selector_get_element_value(
        file_ptr, element._handle, query.encode("utf-8")
    )
    if ptr is None:
        msg = lib.ifcopenshell_last_error_message()
        if msg:
            raise _selector_error(lib, "selector get_element_value failed")
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
    lib.ifcopenshell_clear_error()

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
        msg = lib.ifcopenshell_last_error_message()
        if msg:
            raise _selector_error(lib, "selector filter_elements failed")
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
    lib = _get_lib()
    _configure_set_lib(lib)

    if isinstance(query, (list, tuple)):
        keys = list(query)
    else:
        keys = _parse_selector_keys(query)

    if isinstance(element, ifcopenshell.entity_instance) or element is None:
        klist = lib.ifcopenshell_selector_keylist_create()
        try:
            for k in keys:
                if isinstance(k, re.Pattern):
                    lib.ifcopenshell_selector_keylist_append_regex(klist, k.pattern.encode("utf-8"))
                else:
                    lib.ifcopenshell_selector_keylist_append_string(klist, str(k).encode("utf-8"))
            val_ptr = _build_native_value(lib, value)
            try:
                file_ptr = getattr(ifc_file, "_ptr", None)
                elem_ptr = element._handle if isinstance(element, ifcopenshell.entity_instance) else None
                rc = lib.ifcopenshell_util_selector_set_element_value(
                    file_ptr, elem_ptr, klist, val_ptr, concat.encode("utf-8")
                )
                if rc != 0:
                    msg = lib.ifcopenshell_last_error_message()
                    msg = msg.decode("utf-8", errors="replace") if msg else (
                        f"Failed to set value '{value}' for element '{element}' "
                        f"with query '{query}' (invalid or unsupported query)."
                    )
                    raise SetElementValueException(msg)
            finally:
                if val_ptr:
                    lib.ifcopenshell_value_free(val_ptr)
        finally:
            lib.ifcopenshell_selector_keylist_destroy(klist)
        return

    if isinstance(element, dict):
        pset_id = element.get("id")
        if pset_id is None:
            return
        pset_inst = ifc_file.by_id(pset_id)
        set_element_value(ifc_file, pset_inst, keys, value, concat=concat)
        return

    if isinstance(element, (list, tuple, set)):
        for v in element:
            set_element_value(ifc_file, v, keys, value, concat=concat)
        return
