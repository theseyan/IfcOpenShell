# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell.geom
import ifcopenshell.util.representation
import ifcopenshell.util.shape
from ifcopenshell.geom import ShapeType


def edit_profile_usage(file, usage, attributes):
    cardinal_point = attributes.get("CardinalPoint")
    if cardinal_point and cardinal_point != usage.CardinalPoint:
        _update_cardinal_point(file, usage, cardinal_point)

    for name, value in attributes.items():
        setattr(usage, name, value)


def _update_cardinal_point(file, usage, cardinal_point):
    material_set = usage.ForProfileSet
    profile = material_set.CompositeProfile
    if not profile and material_set.MaterialProfiles:
        profile = material_set.MaterialProfiles[0].Profile
    if not profile:
        return

    position = _calculate_position(file, profile, cardinal_point)

    if file.schema == "IFC2X3":
        for rel in file.get_inverse(usage):
            if not rel.is_a("IfcRelAssociatesMaterial"):
                continue
            for element in rel.RelatedObjects:
                _update_representation(file, element, profile, position)
    else:
        for rel in usage.AssociatedTo:
            for element in rel.RelatedObjects:
                _update_representation(file, element, profile, position)


def _calculate_position(file, profile, cardinal_point):
    dummy = ifcopenshell.file(schema=file.schema)
    dummy_profile = dummy.add(profile)
    for i, attribute in enumerate(dummy_profile):
        name = dummy_profile.attribute_name(i)
        if "Radius" in name and name != "RoundingRadius":
            dummy_profile[i] = None
    dummy_solid = dummy.create_entity(
        "IfcExtrudedAreaSolid",
        **{
            "SweptArea": dummy_profile,
            "ExtrudedDirection": dummy.createIfcDirection((0.0, 0.0, 1.0)),
            "Depth": 1,
        }
    )
    settings_2d = ifcopenshell.geom.settings()
    settings_2d.set("dimensionality", ifcopenshell.ifcopenshell_wrapper.CURVES_SURFACES_AND_SOLIDS)
    shape = ifcopenshell.geom.create_shape(settings_2d, dummy_solid)

    if cardinal_point == 1:
        return _get_bottom_left(file, shape)
    elif cardinal_point == 2:
        return _get_bottom_centre(file, shape)
    elif cardinal_point == 3:
        return _get_bottom_right(file, shape)
    elif cardinal_point == 4:
        return _get_mid_depth_left(file, shape)
    elif cardinal_point == 5:
        return _get_mid_depth_centre(file, shape)
    elif cardinal_point == 6:
        return _get_mid_depth_right(file, shape)
    elif cardinal_point == 7:
        return _get_top_left(file, shape)
    elif cardinal_point == 8:
        return _get_top_centre(file, shape)
    elif cardinal_point == 9:
        return _get_top_right(file, shape)


def _get_bottom_left(file, shape):
    width = ifcopenshell.util.shape.get_x(shape)
    height = ifcopenshell.util.shape.get_y(shape)
    return file.createIfcAxis2Placement3D(file.createIfcCartesianPoint((-width / 2, height / 2, 0.0)))


def _get_bottom_centre(file, shape):
    height = ifcopenshell.util.shape.get_y(shape)
    return file.createIfcAxis2Placement3D(file.createIfcCartesianPoint((0.0, height / 2, 0.0)))


def _get_bottom_right(file, shape):
    width = ifcopenshell.util.shape.get_x(shape)
    height = ifcopenshell.util.shape.get_y(shape)
    return file.createIfcAxis2Placement3D(file.createIfcCartesianPoint((width / 2, height / 2, 0.0)))


def _get_mid_depth_left(file, shape):
    width = ifcopenshell.util.shape.get_x(shape)
    return file.createIfcAxis2Placement3D(file.createIfcCartesianPoint((-width / 2, 0.0, 0.0)))


def _get_mid_depth_centre(file, shape):
    return file.createIfcAxis2Placement3D(file.createIfcCartesianPoint((0.0, 0.0, 0.0)))


def _get_mid_depth_right(file, shape):
    width = ifcopenshell.util.shape.get_x(shape)
    return file.createIfcAxis2Placement3D(file.createIfcCartesianPoint((width / 2, 0.0, 0.0)))


def _get_top_left(file, shape):
    width = ifcopenshell.util.shape.get_x(shape)
    height = ifcopenshell.util.shape.get_y(shape)
    return file.createIfcAxis2Placement3D(file.createIfcCartesianPoint((-width / 2, -height / 2, 0.0)))


def _get_top_centre(file, shape):
    height = ifcopenshell.util.shape.get_y(shape)
    return file.createIfcAxis2Placement3D(file.createIfcCartesianPoint((0.0, -height / 2, 0.0)))


def _get_top_right(file, shape):
    width = ifcopenshell.util.shape.get_x(shape)
    height = ifcopenshell.util.shape.get_y(shape)
    return file.createIfcAxis2Placement3D(file.createIfcCartesianPoint((width / 2, -height / 2, 0.0)))


def _update_representation(file, element, profile, position):
    representation = ifcopenshell.util.representation.get_representation(element, "Model", "Body", "MODEL_VIEW")
    if not representation:
        return

    for subelement in file.traverse(representation):
        if subelement.is_a("IfcSweptAreaSolid") and subelement.SweptArea == profile:
            subelement.Position = position
