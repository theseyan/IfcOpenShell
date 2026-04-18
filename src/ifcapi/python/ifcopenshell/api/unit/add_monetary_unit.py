# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

"""unit.add_monetary_unit — create a new monetary unit."""

import ifcopenshell


def add_monetary_unit(file, currency="DOLLARYDOO"):
    return file.create_entity("IfcMonetaryUnit", currency)
