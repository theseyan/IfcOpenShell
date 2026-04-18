# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell.api.style
import ifcopenshell.util.element


def remove_style(file, style=None):
    _purge_inverses(file, style)
    ifc_class = style.is_a()
    if ifc_class == "IfcSurfaceStyle":
        for style_ in style.Styles:
            ifcopenshell.api.style.remove_surface_style(file, style=style_)
    elif ifc_class == "IfcFillAreaStyle":
        for style_ in style.FillStyles:
            ifcopenshell.util.element.remove_deep2(file, style_)
    file.remove(style)


def _purge_inverses(file, style, do_not_delete=()):
    for inverse in file.get_inverse(style):
        if inverse.is_a("IfcStyledItem"):
            if len(inverse.Styles) == 1:
                _purge_styled_representations(file, inverse)
                file.remove(inverse)
        elif inverse.is_a("IfcFillAreaStyleHatching"):
            _purge_fill_area_style_hatching(file, inverse, style, do_not_delete)


def _purge_styled_representations(file, styled_item):
    for inverse in file.get_inverse(styled_item):
        if inverse.is_a("IfcStyledRepresentation") and len(inverse.Items) == 1:
            _purge_material_definition_representations(file, inverse)
            file.remove(inverse)


def _purge_material_definition_representations(file, styled_representation):
    for inverse in file.get_inverse(styled_representation):
        if inverse.is_a("IfcMaterialDefinitionRepresentation") and len(inverse.Representations) == 1:
            file.remove(inverse)


def _purge_fill_area_style_hatching(file, fill_area_style_hatching, style, do_not_delete):
    for inverse in file.get_inverse(fill_area_style_hatching):
        if inverse.is_a("IfcFillAreaStyle"):
            # Recurse: remove the IfcFillAreaStyle that references this hatching
            _purge_inverses(file, inverse, do_not_delete=(fill_area_style_hatching, style))
            ifc_class = inverse.is_a()
            if ifc_class == "IfcFillAreaStyle":
                for fs in inverse.FillStyles:
                    if fs not in do_not_delete and fs != fill_area_style_hatching and fs != style:
                        ifcopenshell.util.element.remove_deep2(file, fs)
            file.remove(inverse)
    ifcopenshell.util.element.remove_deep2(file, fill_area_style_hatching)
