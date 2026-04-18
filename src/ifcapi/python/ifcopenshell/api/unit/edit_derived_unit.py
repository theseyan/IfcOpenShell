# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

"""unit.edit_derived_unit — edit attributes of an IfcDerivedUnit."""


def edit_derived_unit(file, unit, attributes):
    for name, value in attributes.items():
        setattr(unit, name, value)
