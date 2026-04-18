# SPDX-License-Identifier: LGPL-3.0-or-later

from .add_style import add_style
from .add_surface_style import add_surface_style
from .add_surface_textures import add_surface_textures
from .assign_item_style import assign_item_style
from .assign_material_style import assign_material_style
from .assign_representation_styles import assign_representation_styles
from .edit_presentation_style import edit_presentation_style
from .edit_surface_style import edit_surface_style
from .remove_style import remove_style
from .remove_styled_representation import remove_styled_representation
from .remove_surface_style import remove_surface_style
from .unassign_material_style import unassign_material_style
from .unassign_representation_styles import unassign_representation_styles

__all__ = [
    "add_style",
    "add_surface_style",
    "add_surface_textures",
    "assign_item_style",
    "assign_material_style",
    "assign_representation_styles",
    "edit_presentation_style",
    "edit_surface_style",
    "remove_style",
    "remove_styled_representation",
    "remove_surface_style",
    "unassign_material_style",
    "unassign_representation_styles",
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
