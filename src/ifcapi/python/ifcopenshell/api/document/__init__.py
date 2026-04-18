# SPDX-License-Identifier: LGPL-3.0-or-later

from .add_information import add_information
from .add_reference import add_reference
from .assign_document import assign_document
from .edit_information import edit_information
from .edit_reference import edit_reference
from .remove_information import remove_information
from .remove_reference import remove_reference
from .unassign_document import unassign_document

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
