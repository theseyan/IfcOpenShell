# SPDX-License-Identifier: LGPL-3.0-or-later

from .add_resource import add_resource
from .add_resource_quantity import add_resource_quantity
from .add_resource_time import add_resource_time
from .assign_resource import assign_resource
from .calculate_resource_usage import calculate_resource_usage
from .calculate_resource_work import calculate_resource_work
from .edit_resource import edit_resource
from .edit_resource_quantity import edit_resource_quantity
from .edit_resource_time import edit_resource_time
from .remove_resource import remove_resource
from .remove_resource_quantity import remove_resource_quantity
from .unassign_resource import unassign_resource

__all__ = [
    "add_resource",
    "add_resource_quantity",
    "add_resource_time",
    "assign_resource",
    "calculate_resource_usage",
    "calculate_resource_work",
    "edit_resource",
    "edit_resource_quantity",
    "edit_resource_time",
    "remove_resource",
    "remove_resource_quantity",
    "unassign_resource",
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
