# SPDX-License-Identifier: LGPL-3.0-or-later


def remove_styled_representation(file, representation=None):
    for inverse in file.get_inverse(representation):
        if inverse.is_a("IfcMaterialDefinitionRepresentation") and len(inverse.Representations) == 1:
            file.remove(inverse)

    for item in representation.Items:
        if item.is_a("IfcStyledItem") and file.get_total_inverses(item) == 1:
            for style in item.Styles:
                if style.is_a("IfcPresentationStyleAssignment"):
                    file.remove(style)
            file.remove(item)

    file.remove(representation)
