# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell

# Maps (ifc_class, attribute_name) -> attribute type name.
# Only the attributes that require special handling are listed.
_COLOUR_RGB_ATTRS = {
    "SurfaceColour",
    # IfcSurfaceStyleLighting attributes
    "DiffuseTransmissionColour",
    "DiffuseReflectionColour",
    "TransmissionColour",
    "ReflectanceColour",
}

_COLOUR_OR_FACTOR_ATTRS = {
    "DiffuseColour",
    "TransmissionColour",
    "DiffuseTransmissionColour",
    "ReflectionColour",
    "SpecularColour",
}


def edit_surface_style(file, style=None, attributes=None):
    attributes = attributes or {}
    ifc_class = style.is_a()

    for key, value in attributes.items():
        if key == "SpecularHighlight":
            _edit_specular_highlight(file, style, value)
        elif key == "SurfaceColour" or (ifc_class == "IfcSurfaceStyleLighting" and key in _COLOUR_RGB_ATTRS):
            _edit_colour_rgb(file, style, key, value)
        elif ifc_class == "IfcSurfaceStyleRendering" and key in _COLOUR_OR_FACTOR_ATTRS:
            _edit_colour_or_factor(file, style, key, value)
        else:
            setattr(style, key, value)


def _edit_colour_rgb(file, style, name, value):
    attribute = getattr(style, name)
    if attribute is None:
        attribute = file.createIfcColourRgb()
        setattr(style, name, attribute)
    attribute.Name = value.get("Name", None)
    attribute.Red = value["Red"]
    attribute.Green = value["Green"]
    attribute.Blue = value["Blue"]


def _edit_colour_or_factor(file, style, name, value):
    if isinstance(value, dict):
        attribute = getattr(style, name)
        if not attribute or not attribute.is_a("IfcColourRgb"):
            colour = file.createIfcColourRgb(None, 0, 0, 0)
            setattr(style, name, colour)
            attribute = getattr(style, name)
        attribute[1] = value["Red"]
        attribute[2] = value["Green"]
        attribute[3] = value["Blue"]
    else:
        # float value for IfcNormalisedRatioMeasure, or None
        existing_value = getattr(style, name)
        if existing_value and existing_value.id():
            file.remove(existing_value)
        if value is not None:
            value = file.create_entity("IfcNormalisedRatioMeasure", value)
        setattr(style, name, value)


def _edit_specular_highlight(file, style, value):
    if value is None:
        style.SpecularHighlight = None
    elif value.get("IfcSpecularExponent", None):
        style.SpecularHighlight = file.createIfcSpecularExponent(value["IfcSpecularExponent"])
    elif value.get("IfcSpecularRoughness", None):
        style.SpecularHighlight = file.createIfcSpecularRoughness(value["IfcSpecularRoughness"])
