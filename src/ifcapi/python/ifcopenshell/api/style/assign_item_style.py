# SPDX-License-Identifier: LGPL-3.0-or-later


def assign_item_style(file, item=None, style=None, should_use_presentation_style_assignment=False):
    styled_item = next(iter(item.StyledByItem), None)

    if not styled_item:
        if style is None:
            return
        if file.schema == "IFC2X3" or should_use_presentation_style_assignment:
            style = file.create_entity("IfcPresentationStyleAssignment", (style,))
        return file.create_entity("IfcStyledItem", item, (style,))

    styled_item_styles = styled_item.Styles
    if style and styled_item_styles == (style,):
        return styled_item

    if file.schema == "IFC4X3":
        if style is None:
            file.remove(styled_item)
            return
        styled_item.Styles = (style,)
        return styled_item

    # < IFC4X3: handle IfcPresentationStyleAssignment
    assignment = None
    for style_ in styled_item_styles:
        if not style_.is_a("IfcPresentationStyleAssignment"):
            continue
        if style is None or assignment:
            file.remove(style_)
        else:
            assignment = style_
            if assignment.Styles != (style,):
                assignment.Styles = (style,)

    if style is None:
        file.remove(styled_item)
        return

    if assignment:
        if styled_item_styles == (assignment,):
            return styled_item
        styled_item.Styles = (assignment,)
        return styled_item

    styled_item.Styles = (style,)
    return styled_item
