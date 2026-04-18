# SPDX-License-Identifier: LGPL-3.0-or-later

from .add_library import add_library
from .add_reference import add_reference
from .assign_reference import assign_reference
from .edit_library import edit_library
from .edit_reference import edit_reference
from .remove_library import remove_library
from .remove_reference import remove_reference
from .unassign_reference import unassign_reference

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
