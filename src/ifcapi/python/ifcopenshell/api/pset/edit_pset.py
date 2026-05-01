# SPDX-License-Identifier: LGPL-3.0-or-later

"""Edit a property set: rename, add/edit/delete properties."""

import ctypes

from ifcopenshell import _generated_capi
from ifcopenshell.entity_instance import _generated_instance_handle_ptr
import ifcopenshell.util.pset as _util_pset
from ifcopenshell.api.pset._capi import build_props, free_props, get_lib, raise_last_error


def edit_pset(file, pset, name=None, properties=None, pset_template=None, should_purge=True):
    lib = get_lib()
    _generated_capi.bind(lib, names=("ifcopenshell_ifcapi_pset_edit_pset",))
    # Ensure the buildingSMART template directory is registered with the
    # native layer; the C edit path will look templates up to drive enum /
    # list value coercion and primary measure types.
    if pset_template is None:
        try:
            _util_pset.get_template(file.schema_identifier)
        except Exception:
            pass
    props_handle = build_props(properties or {})
    template_handle = _generated_instance_handle_ptr(pset_template._handle) if pset_template is not None else None
    name_arg = _generated_capi.encode_string(name) if name else None
    try:
        ok = _generated_capi.call_scalar(
            lib.ifcopenshell_ifcapi_pset_edit_pset,
            ctypes.c_bool,
            _generated_instance_handle_ptr(file._ptr),
            _generated_instance_handle_ptr(pset._handle),
            name_arg,
            props_handle,
            template_handle,
            bool(should_purge),
        )
        if not ok:
            raise_last_error("edit_pset failed")
    finally:
        free_props(props_handle)
