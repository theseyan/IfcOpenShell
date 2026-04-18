# SPDX-License-Identifier: LGPL-3.0-or-later

"""Owner history and actor/person/organisation management for the native ifcapi backend."""

from . import settings
from .add_actor import add_actor
from .add_address import add_address
from .add_application import add_application
from .add_organisation import add_organisation
from .add_person import add_person
from .add_person_and_organisation import add_person_and_organisation
from .add_role import add_role
from .assign_actor import assign_actor
from .create_owner_history import create_owner_history
from .edit_actor import edit_actor
from .edit_address import edit_address
from .edit_application import edit_application
from .edit_organisation import edit_organisation
from .edit_person import edit_person
from .edit_role import edit_role
from .remove_actor import remove_actor
from .remove_address import remove_address
from .remove_application import remove_application
from .remove_organisation import remove_organisation
from .remove_person import remove_person
from .remove_person_and_organisation import remove_person_and_organisation
from .remove_role import remove_role
from .unassign_actor import unassign_actor
from .update_owner_history import update_owner_history

__all__ = [
    "add_actor",
    "add_address",
    "add_application",
    "add_organisation",
    "add_person",
    "add_person_and_organisation",
    "add_role",
    "assign_actor",
    "create_owner_history",
    "edit_actor",
    "edit_address",
    "edit_application",
    "edit_organisation",
    "edit_person",
    "edit_role",
    "remove_actor",
    "remove_address",
    "remove_application",
    "remove_organisation",
    "remove_person",
    "remove_person_and_organisation",
    "remove_role",
    "unassign_actor",
    "update_owner_history",
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
