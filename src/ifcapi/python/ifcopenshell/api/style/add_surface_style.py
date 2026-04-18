# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell.api.style


def add_surface_style(file, style=None, ifc_class="IfcSurfaceStyleShading", attributes=None):
    attributes = attributes or {}
    style_item = file.create_entity(ifc_class)
    ifcopenshell.api.style.edit_surface_style(file, style=style_item, attributes=attributes)
    styles = list(style.Styles or [])

    # IfcSurfaceStyleRendering is a subtype of IfcSurfaceStyleShading;
    # they are mutually exclusive in a single IfcSurfaceStyle.
    select_class = ifc_class
    if select_class == "IfcSurfaceStyleRendering":
        select_class = "IfcSurfaceStyleShading"
    duplicate_items = [s for s in styles if s.is_a(select_class)]
    for duplicate_item in duplicate_items:
        ifcopenshell.api.style.remove_surface_style(file, style=duplicate_item)

    styles = list(style.Styles or [])
    styles.append(style_item)
    style.Styles = styles
    return style_item
