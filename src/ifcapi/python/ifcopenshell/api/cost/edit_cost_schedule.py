# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later


def edit_cost_schedule(file, cost_schedule=None, attributes=None):
    for name, value in (attributes or {}).items():
        setattr(cost_schedule, name, value)
