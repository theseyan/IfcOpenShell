# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later


def edit_prop_template(file, prop_template, attributes):
    if enum_values := attributes.get("Enumerators", None):
        prop_name = attributes.get("Name", None) or getattr(prop_template, "Name", None) or "Unnamed"
        primary_measure_type = (
            attributes.get("PrimaryMeasureType", None)
            or getattr(prop_template, "PrimaryMeasureType", None)
            or "IfcLabel"
        )
        enum_values = [file.create_entity(primary_measure_type, v) for v in enum_values]
        if enumerators := prop_template.Enumerators:
            enumerators.Name = prop_name
            enumerators.EnumerationValues = enum_values
        else:
            prop_template.Enumerators = file.create_entity("IfcPropertyEnumeration", prop_name, enum_values)

    if "Enumerators" in attributes:
        del attributes["Enumerators"]

    for name, value in attributes.items():
        setattr(prop_template, name, value)
