# SPDX-License-Identifier: LGPL-3.0-or-later


def unassign_representation_styles(file, shape_representation=None, styles=None,
                                   should_use_presentation_style_assignment=False):
    if not styles:
        return
    use_style_assignment = file.schema == "IFC2X3" or should_use_presentation_style_assignment

    for element in file.traverse(shape_representation):
        if not element.is_a("IfcShapeRepresentation"):
            continue
        for item in element.Items:
            if not item.is_a("IfcGeometricRepresentationItem"):
                continue
            if not item.StyledByItem:
                continue

            styled_item = item.StyledByItem[0]
            if use_style_assignment:
                for style_ in styled_item.Styles:
                    if style_.is_a("IfcPresentationStyleAssignment"):
                        _remove_styles(file, style_, styles)
            _remove_styles(file, styled_item, styles)


def _remove_styles(file, item, styles_to_remove):
    remaining = [s for s in item.Styles if s not in styles_to_remove]
    if not remaining:
        file.remove(item)
    elif len(remaining) != len(item.Styles):
        item.Styles = remaining
