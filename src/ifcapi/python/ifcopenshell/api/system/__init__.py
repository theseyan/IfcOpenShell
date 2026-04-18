# SPDX-License-Identifier: LGPL-3.0-or-later

from .add_port import add_port
from .add_system import add_system
from .assign_system import assign_system
from .remove_system import remove_system
from .unassign_system import unassign_system

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
