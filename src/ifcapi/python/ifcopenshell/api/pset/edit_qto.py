# SPDX-License-Identifier: LGPL-3.0-or-later

"""Edit an IfcElementQuantity: rename, add/edit/delete quantities."""

import ifcopenshell.util.pset as _util_pset
from ifcopenshell.api.pset._capi import build_props, free_props, get_lib, raise_last_error


def edit_qto(file, qto, name=None, properties=None, pset_template=None):
    lib = get_lib()
    if pset_template is None:
        try:
            _util_pset.get_template(file.schema_identifier)
        except Exception:
            pass
    props_handle = build_props(properties or {})
    template_handle = pset_template._handle if pset_template is not None else 0
    name_arg = name.encode("utf-8") if name else None
    try:
        ok = lib.ifcopenshell_api_pset_edit_qto(
            file._ptr, qto._handle, name_arg, props_handle, template_handle
        )
        if not ok:
            raise_last_error("edit_qto failed")
    finally:
        free_props(props_handle)
