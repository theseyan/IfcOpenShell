# SPDX-License-Identifier: LGPL-3.0-or-later

from .assign_connection_geometry import assign_connection_geometry
from .copy_boundary import copy_boundary
from .edit_attributes import edit_attributes
from .remove_boundary import remove_boundary

__all__ = ["assign_connection_geometry", "copy_boundary", "edit_attributes", "remove_boundary"]

import importlib as _importlib

def __getattr__(name):
    try:
        module = _importlib.import_module(f".{name}", __name__)
    except ModuleNotFoundError:
        raise AttributeError(f"module {__name__!r} has no attribute {name!r}")
    func = getattr(module, name, None)
    if func is None:
        raise AttributeError(f"module {__name__!r} has no attribute {name!r}")
    globals()[name] = func
    return func
