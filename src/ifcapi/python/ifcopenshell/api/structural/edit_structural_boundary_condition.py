# SPDX-License-Identifier: LGPL-3.0-or-later


def edit_structural_boundary_condition(file, condition=None, attributes=None):
    for name, data in (attributes or {}).items():
        if data["type"] == "string" or data["type"] == "null":
            value = data["value"]
        elif data["type"] == "IfcBoolean":
            value = file.createIfcBoolean(data["value"])
        else:
            value = file.create_entity(data["type"], data["value"])
        setattr(condition, name, value)
