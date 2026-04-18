# SPDX-License-Identifier: LGPL-3.0-or-later

from .add_structural_activity import add_structural_activity
from .add_structural_analysis_model import add_structural_analysis_model
from .add_structural_boundary_condition import add_structural_boundary_condition
from .add_structural_load import add_structural_load
from .add_structural_load_case import add_structural_load_case
from .add_structural_load_group import add_structural_load_group
from .add_structural_member_connection import add_structural_member_connection
from .assign_product import assign_product
from .assign_structural_analysis_model import assign_structural_analysis_model
from .assign_to_building import assign_to_building
from .edit_structural_analysis_model import edit_structural_analysis_model
from .edit_structural_boundary_condition import edit_structural_boundary_condition
from .edit_structural_connection_cs import edit_structural_connection_cs
from .edit_structural_item_axis import edit_structural_item_axis
from .edit_structural_load import edit_structural_load
from .edit_structural_load_case import edit_structural_load_case
from .remove_structural_analysis_model import remove_structural_analysis_model
from .remove_structural_boundary_condition import remove_structural_boundary_condition
from .remove_structural_connection_condition import remove_structural_connection_condition
from .remove_structural_load import remove_structural_load
from .remove_structural_load_case import remove_structural_load_case
from .remove_structural_load_group import remove_structural_load_group
from .unassign_structural_analysis_model import unassign_structural_analysis_model

__all__ = [
    "add_structural_activity",
    "add_structural_analysis_model",
    "add_structural_boundary_condition",
    "add_structural_load",
    "add_structural_load_case",
    "add_structural_load_group",
    "add_structural_member_connection",
    "assign_product",
    "assign_structural_analysis_model",
    "assign_to_building",
    "edit_structural_analysis_model",
    "edit_structural_boundary_condition",
    "edit_structural_connection_cs",
    "edit_structural_item_axis",
    "edit_structural_load",
    "edit_structural_load_case",
    "remove_structural_analysis_model",
    "remove_structural_boundary_condition",
    "remove_structural_connection_condition",
    "remove_structural_load",
    "remove_structural_load_case",
    "remove_structural_load_group",
    "unassign_structural_analysis_model",
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
