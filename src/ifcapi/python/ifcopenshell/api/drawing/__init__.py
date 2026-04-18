# SPDX-License-Identifier: LGPL-3.0-or-later

from .assign_product import assign_product
from .edit_text_literal import edit_text_literal
from .unassign_product import unassign_product

__all__ = ["assign_product", "edit_text_literal", "unassign_product"]

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
