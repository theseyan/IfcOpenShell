# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

from .add_cost_item import add_cost_item
from .add_cost_item_quantity import add_cost_item_quantity
from .add_cost_schedule import add_cost_schedule
from .add_cost_value import add_cost_value
from .assign_cost_item_quantity import assign_cost_item_quantity
from .assign_cost_value import assign_cost_value
from .calculate_cost_item_resource_value import calculate_cost_item_resource_value
from .copy_cost_item import copy_cost_item
from .copy_cost_item_values import copy_cost_item_values
from .copy_cost_schedule import copy_cost_schedule
from .edit_cost_item import edit_cost_item
from .edit_cost_item_quantity import edit_cost_item_quantity
from .edit_cost_schedule import edit_cost_schedule
from .edit_cost_value import edit_cost_value
from .edit_cost_value_formula import edit_cost_value_formula
from .remove_cost_item import remove_cost_item
from .remove_cost_item_quantity import remove_cost_item_quantity
from .remove_cost_schedule import remove_cost_schedule
from .remove_cost_value import remove_cost_value
from .unassign_cost_item_quantity import unassign_cost_item_quantity

__all__ = [
    "add_cost_item",
    "add_cost_item_quantity",
    "add_cost_schedule",
    "add_cost_value",
    "assign_cost_item_quantity",
    "assign_cost_value",
    "calculate_cost_item_resource_value",
    "copy_cost_item",
    "copy_cost_item_values",
    "copy_cost_schedule",
    "edit_cost_item",
    "edit_cost_item_quantity",
    "edit_cost_schedule",
    "edit_cost_value",
    "edit_cost_value_formula",
    "remove_cost_item",
    "remove_cost_item_quantity",
    "remove_cost_schedule",
    "remove_cost_value",
    "unassign_cost_item_quantity",
]

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
