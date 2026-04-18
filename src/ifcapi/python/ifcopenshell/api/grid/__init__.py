# SPDX-License-Identifier: LGPL-3.0-or-later

from .create_axis_curve import create_axis_curve
from .create_grid_axis import create_grid_axis
from .remove_grid_axis import remove_grid_axis

__all__ = ["create_axis_curve", "create_grid_axis", "remove_grid_axis"]

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
