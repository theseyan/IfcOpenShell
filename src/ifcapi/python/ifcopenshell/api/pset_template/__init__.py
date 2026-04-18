# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

from .add_prop_template import add_prop_template
from .add_pset_template import add_pset_template
from .edit_prop_template import edit_prop_template
from .edit_pset_template import edit_pset_template
from .remove_prop_template import remove_prop_template
from .remove_pset_template import remove_pset_template

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
