# SPDX-License-Identifier: LGPL-3.0-or-later

"""Edit a property set: rename, add/edit/delete properties."""

import ifcopenshell
import ifcopenshell.util.pset as _util_pset
from ifcopenshell import _ifcopenshell_capi as _capi
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
        ok = _capi.ifcopenshell_ifcapi_pset_edit_pset(
            file._handle,
            pset._handle,
            name,
            props_handle,
            template_handle,
            bool(should_purge),
        )
        if not ok:
            raise RuntimeError(_capi.last_error_message() or "edit_pset failed")
    finally:
        free_props(props_handle)
