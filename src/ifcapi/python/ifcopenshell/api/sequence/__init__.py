# SPDX-License-Identifier: LGPL-3.0-or-later

from .add_date_time import add_date_time
from .add_task import add_task
from .add_task_time import add_task_time
from .add_time_period import add_time_period
from .add_work_calendar import add_work_calendar
from .add_work_plan import add_work_plan
from .add_work_schedule import add_work_schedule
from .add_work_time import add_work_time
from .assign_lag_time import assign_lag_time
from .assign_process import assign_process
from .assign_product import assign_product
from .assign_recurrence_pattern import assign_recurrence_pattern
from .assign_sequence import assign_sequence
from .assign_work_plan import assign_work_plan
from .calculate_task_duration import calculate_task_duration
from .cascade_schedule import cascade_schedule
from .copy_work_schedule import copy_work_schedule
from .create_baseline import create_baseline
from .duplicate_task import duplicate_task
from .edit_lag_time import edit_lag_time
from .edit_recurrence_pattern import edit_recurrence_pattern
from .edit_sequence import edit_sequence
from .edit_task import edit_task
from .edit_task_time import edit_task_time
from .edit_work_calendar import edit_work_calendar
from .edit_work_plan import edit_work_plan
from .edit_work_schedule import edit_work_schedule
from .edit_work_time import edit_work_time

try:
    from .recalculate_schedule import recalculate_schedule
except ModuleNotFoundError as e:
    print(f"Note: API not available due to missing dependencies: sequence.recalculate_schedule - {e}")

from .remove_task import remove_task
from .remove_time_period import remove_time_period
from .remove_work_calendar import remove_work_calendar
from .remove_work_plan import remove_work_plan
from .remove_work_schedule import remove_work_schedule
from .remove_work_time import remove_work_time
from .unassign_lag_time import unassign_lag_time
from .unassign_process import unassign_process
from .unassign_product import unassign_product
from .unassign_recurrence_pattern import unassign_recurrence_pattern
from .unassign_sequence import unassign_sequence

__all__ = [
    "add_date_time",
    "add_task",
    "add_task_time",
    "add_time_period",
    "add_work_calendar",
    "add_work_plan",
    "add_work_schedule",
    "add_work_time",
    "assign_lag_time",
    "assign_process",
    "assign_product",
    "assign_recurrence_pattern",
    "assign_sequence",
    "assign_work_plan",
    "calculate_task_duration",
    "cascade_schedule",
    "copy_work_schedule",
    "create_baseline",
    "duplicate_task",
    "edit_lag_time",
    "edit_recurrence_pattern",
    "edit_sequence",
    "edit_task",
    "edit_task_time",
    "edit_work_calendar",
    "edit_work_plan",
    "edit_work_schedule",
    "edit_work_time",
    "recalculate_schedule",
    "remove_task",
    "remove_time_period",
    "remove_work_calendar",
    "remove_work_plan",
    "remove_work_schedule",
    "remove_work_time",
    "unassign_lag_time",
    "unassign_process",
    "unassign_product",
    "unassign_recurrence_pattern",
    "unassign_sequence",
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
