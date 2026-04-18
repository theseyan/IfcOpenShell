# SPDX-License-Identifier: LGPL-3.0-or-later

"""unit.edit_monetary_unit — edit attributes of an IfcMonetaryUnit."""


def edit_monetary_unit(file, unit, attributes):
    for name, value in attributes.items():
        setattr(unit, name, value)
