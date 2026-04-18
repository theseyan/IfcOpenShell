# SPDX-License-Identifier: LGPL-3.0-or-later

from .add_si_unit import add_si_unit
from .add_monetary_unit import add_monetary_unit
from .add_conversion_based_unit import add_conversion_based_unit
from .assign_unit import assign_unit
from .remove_unit import remove_unit
from .unassign_unit import unassign_unit

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
