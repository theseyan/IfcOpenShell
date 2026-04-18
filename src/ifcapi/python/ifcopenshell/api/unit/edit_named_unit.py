# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

"""unit.edit_named_unit — edit attributes of an IfcNamedUnit."""


def edit_named_unit(file, unit, attributes):
    for name, value in attributes.items():
        if name == "Dimensions":
            dimensions = unit.Dimensions
            if file.get_total_inverses(dimensions) > 1:
                unit.Dimensions = file.createIfcDimensionalExponents(*value)
            else:
                for i, exponent in enumerate(value):
                    dimensions[i] = exponent
            continue
        setattr(unit, name, value)
