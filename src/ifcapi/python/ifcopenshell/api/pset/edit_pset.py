# SPDX-License-Identifier: LGPL-3.0-or-later

"""Edit a property set: rename, add/edit/delete properties."""

import ifcopenshell
import ifcopenshell.util.pset as _util_pset
from ifcopenshell import _ifcopenshell_capi as _capi
from ifcopenshell._capi_utils import raise_last_error
from ifcopenshell.api.pset._capi import build_props, free_props


def edit_pset(file, pset, name=None, properties=None, pset_template=None, should_purge=True):
    if pset_template is None:
        try:
            _util_pset.get_template(file.schema_identifier)
        except Exception:
            pass
    props_handle = build_props(properties or {})
    template_handle = pset_template._handle if pset_template is not None else None
    try:
        _capi.pset_edit_pset(
            file._handle,
            {
                "pset": pset._handle,
                "name": name,
                "properties": props_handle,
                "pset_template": template_handle,
                "should_purge": bool(should_purge),
            },
        )
        if _capi.last_error_kind() != _capi.IFCOPENSHELL_ERROR_NONE:
            raise_last_error("edit_pset failed")
    finally:
        free_props(props_handle)
