# SPDX-License-Identifier: LGPL-3.0-or-later

"""Edit an IfcElementQuantity: rename, add/edit/delete quantities."""

import ifcopenshell
import ifcopenshell.util.pset as _util_pset
from ifcopenshell import _ifcopenshell_capi as _capi
from ifcopenshell.api.pset._capi import build_props, free_props


def edit_qto(file, qto, name=None, properties=None, pset_template=None):
    if pset_template is None:
        try:
            _util_pset.get_template(file.schema_identifier)
        except Exception:
            pass
    props_handle = build_props(properties or {})
    template_handle = pset_template._handle if pset_template is not None else None
    try:
        ok = _capi.pset_edit_qto(
            file._handle,
            qto._handle,
            name,
            props_handle,
            template_handle,
        )
        if not ok:
            raise RuntimeError(_capi.last_error_message() or "edit_qto failed")
    finally:
        free_props(props_handle)
