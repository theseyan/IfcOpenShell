# SPDX-License-Identifier: LGPL-3.0-or-later

"""Edit an IfcElementQuantity: rename, add/edit/delete quantities."""

import ifcopenshell
import ifcopenshell.util.pset as _util_pset
from ifcopenshell import _ifcopenshell_capi as _capi
from ifcopenshell._capi_utils import raise_last_error
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
        _capi.pset_edit_qto(
            file._handle,
            {
                "qto": qto._handle,
                "name": name,
                "properties": props_handle,
                "qto_template": template_handle,
            },
        )
        if _capi.last_error_kind() != _capi.IFCOPENSHELL_ERROR_NONE:
            raise_last_error("edit_qto failed")
    finally:
        free_props(props_handle)
