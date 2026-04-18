# SPDX-License-Identifier: LGPL-3.0-or-later

from .add_classification import add_classification
from .add_reference import add_reference
from .remove_classification import remove_classification
from .remove_reference import remove_reference

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
