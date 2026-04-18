# SPDX-License-Identifier: LGPL-3.0-or-later

from .assign_control import assign_control
from .unassign_control import unassign_control

__all__ = ["assign_control", "unassign_control"]

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
