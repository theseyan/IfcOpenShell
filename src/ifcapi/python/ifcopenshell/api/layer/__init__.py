# SPDX-License-Identifier: LGPL-3.0-or-later

from .add_layer import add_layer
from .assign_layer import assign_layer
from .remove_layer import remove_layer
from .unassign_layer import unassign_layer

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
