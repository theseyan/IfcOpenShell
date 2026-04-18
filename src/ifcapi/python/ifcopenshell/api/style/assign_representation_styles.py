# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later


def assign_representation_styles(file, shape_representation=None, styles=None,
                                 replace_previous_same_type_style=True,
                                 should_use_presentation_style_assignment=False):
    if not styles:
        return []
    styles = list(styles)
    results = []
    use_style_assignment = file.schema == "IFC2X3" or should_use_presentation_style_assignment

    for element in file.traverse(shape_representation):
        if not element.is_a("IfcShapeModel"):
            continue
        for item in element.Items:
            if (not item.is_a("IfcGeometricRepresentationItem")
                    and not item.is_a("IfcTopologicalRepresentationItem")):
                continue
            if styles:
                style = styles.pop(0)
            name = style.Name
            current_style_type = style.is_a()

            prev_styled_item = next((i for i in item.StyledByItem), None)
            style_assignment = None

            if prev_styled_item is None:
                if use_style_assignment:
                    style_assignment = file.createIfcPresentationStyleAssignment([style])
                    results.append(file.createIfcStyledItem(item, [style_assignment], name))
                else:
                    results.append(file.createIfcStyledItem(item, [style], name))
                continue

            if replace_previous_same_type_style:
                _remove_same_type_styles(file, prev_styled_item, current_style_type, remove_item=False)
                for style_ in prev_styled_item.Styles:
                    if style_.is_a("IfcPresentationStyleAssignment"):
                        if use_style_assignment and style_assignment is None:
                            style_assignment = style_
                            _remove_same_type_styles(file, style_assignment, current_style_type, remove_item=False)
                        else:
                            _remove_same_type_styles(file, style_assignment, current_style_type, remove_item=True)

                if use_style_assignment:
                    if style_assignment:
                        style_assignment.Styles = style_assignment.Styles + (style,)
                    else:
                        style_assignment = file.createIfcPresentationStyleAssignment([style])
                        prev_styled_item.Styles = prev_styled_item.Styles + (style_assignment,)
                else:
                    prev_styled_item.Styles = prev_styled_item.Styles + (style,)
                continue

            # Collect previously assigned styles
            assigned_styles = []
            for style_ in prev_styled_item.Styles:
                if style_.is_a("IfcPresentationStyleAssignment"):
                    if style_assignment is None:
                        style_assignment = style_
                    assigned_styles.extend(style_.Styles)
                else:
                    assigned_styles.append(style_)

            if style in assigned_styles:
                continue

            if use_style_assignment:
                if style_assignment is not None:
                    style_assignment.Styles = style_assignment.Styles + (style,)
                else:
                    style_assignment = file.createIfcPresentationStyleAssignment([style])
                    prev_styled_item.Styles = prev_styled_item.Styles + (style_assignment,)
            else:
                prev_styled_item.Styles = prev_styled_item.Styles + (style,)

    return results


def _remove_same_type_styles(file, style_item, current_style_type, remove_item):
    styles = [s for s in style_item.Styles if s.is_a() != current_style_type]
    if remove_item and not styles:
        file.remove(style_item)
    else:
        style_item.Styles = styles
