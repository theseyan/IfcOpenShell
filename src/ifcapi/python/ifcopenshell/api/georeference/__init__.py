# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

from .add_georeferencing import add_georeferencing
from .remove_georeferencing import remove_georeferencing
from .edit_georeferencing import edit_georeferencing
from .edit_true_north import edit_true_north
from .edit_wcs import edit_wcs

__all__ = [
    "add_georeferencing",
    "remove_georeferencing",
    "edit_georeferencing",
    "edit_true_north",
    "edit_wcs",
]

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
