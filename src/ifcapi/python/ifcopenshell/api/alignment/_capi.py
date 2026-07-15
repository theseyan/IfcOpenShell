# SPDX-License-Identifier: LGPL-3.0-or-later

from __future__ import annotations

import ifcopenshell
from ifcopenshell._capi_utils import call_handle as _call_handle
from ifcopenshell._capi_utils import call_handle_list as _call_handle_list
from ifcopenshell._capi_utils import call_status as _call_status
from ifcopenshell._capi_utils import (
    file_handle,
    instance_handle,
    owner_context,
    wrap_handle,
)

from ... import _ifcopenshell_capi as _capi


class OwnerOptions:
    def __init__(self, values, refs=()):
        self.values = values
        self.refs = refs


def call_handle(file: ifcopenshell.file, fn_name: str, *args, nullable: bool = False):
    return _call_handle(file, fn_name, *args, nullable=nullable)


def call_handle_list(file: ifcopenshell.file, fn_name: str, *args):
    return _call_handle_list(file, fn_name, *args)


def call_status(fn_name: str, *args) -> None:
    _call_status(fn_name, *args)


def call_value(fn_name: str, *args):
    return getattr(_capi, fn_name)(*args)


def instance_list(values):
    return [instance_handle(value) for value in values]


def point_list(values):
    return [list(value) for value in values]


def owner_options(file):
    if not file.schema.upper().startswith("IFC4X3"):
        return OwnerOptions({})
    owner_history, user, application = owner_context(file)
    same_file = lambda entity: entity is not None and entity.file is file
    if same_file(user) and same_file(application):
        # Owner settings callbacks may create the application after returning the
        # user. Reacquire both after those callbacks have finished so the native
        # call never receives a wrapper invalidated by the intervening mutation.
        user = file.by_id(user.id())
        application = file.by_id(application.id())
    else:
        user = application = None
    values = {"owner_history": owner_history, "user": user, "application": application}
    return OwnerOptions(
        {
            name: instance_handle(value)
            for name, value in values.items()
            if value is not None
        },
        tuple(value for value in values.values() if value is not None),
    )
