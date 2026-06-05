# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

from __future__ import annotations

import ifcopenshell
from ifcopenshell._capi_utils import (
    call_status as _call_status,
)
from ifcopenshell._capi_utils import (
    file_handle,
    instance_handle,
)

from ... import _ifcopenshell_capi as _capi


def owner_context(file: ifcopenshell.file):
    import ifcopenshell.api.owner.settings

    user = ifcopenshell.api.owner.settings.get_user(file)
    application = ifcopenshell.api.owner.settings.get_application(file)
    return user, application


def call_status(fn_name: str, *args) -> None:
    _call_status(fn_name, *args)
