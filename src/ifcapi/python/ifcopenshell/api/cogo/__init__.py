# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

from .add_survey_point import add_survey_point
from .assign_survey_point import assign_survey_point
from .bearing2dd import bearing2dd
from .edit_survey_point import edit_survey_point

__all__ = ["add_survey_point", "assign_survey_point", "bearing2dd", "edit_survey_point"]

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
