# SPDX-License-Identifier: LGPL-3.0-or-later


def edit_objective(file, objective, attributes):
    for name, value in attributes.items():
        setattr(objective, name, value)
