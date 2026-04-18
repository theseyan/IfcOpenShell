# SPDX-License-Identifier: LGPL-3.0-or-later


def add_style(file, name=None, ifc_class="IfcSurfaceStyle"):
    kwargs = {"Name": name}
    if ifc_class == "IfcSurfaceStyle":
        # Name is filled out because Revit treats this incorrectly as the material name
        kwargs["Side"] = "BOTH"
    return file.create_entity(ifc_class, **kwargs)
