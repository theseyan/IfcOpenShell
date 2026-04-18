# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

from .add_axis_representation import add_axis_representation
from .add_boolean import add_boolean
from .add_footprint_representation import add_footprint_representation
from .add_mesh_representation import add_mesh_representation
from .add_profile_representation import add_profile_representation
from .add_shape_aspect import add_shape_aspect
from .assign_representation import assign_representation
from .edit_object_placement import edit_object_placement
from .map_representation import map_representation
from .remove_representation import remove_representation
from .unassign_representation import unassign_representation

# Modules that may not yet be ported to the native ifcapi backend.
_optional_modules = [
    "add_slab_representation",
    "add_topology_representation",
    "add_wall_representation",
    "clip_solid",
    "clip_solid_bounded",
    "connect_element",
    "connect_path",
    "connect_wall",
    "copy_representation",
    "create_2pt_wall",
    "disconnect_element",
    "disconnect_path",
    "remove_boolean",
    "validate_type",
    "add_door_representation",
    "add_window_representation",
    "add_railing_representation",
    "regenerate_wall_representation",
]

import importlib as _importlib

for _mod_name in _optional_modules:
    try:
        _mod = _importlib.import_module(f".{_mod_name}", __name__)
        globals()[_mod_name] = getattr(_mod, _mod_name)
    except (ImportError, ModuleNotFoundError):
        pass

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
