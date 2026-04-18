# This file was generated with the assistance of an AI coding tool.
# IfcOpenShell - IFC toolkit and geometry engine
# Copyright (C) 2021 Dion Moult <dion@thinkmoult.com>
#
# This file is part of IfcOpenShell.
#
# IfcOpenShell is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# IfcOpenShell is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with IfcOpenShell.  If not, see <http://www.gnu.org/licenses/>.

import ctypes
from collections.abc import Generator, Sequence
from typing import Literal, Optional, TypedDict, Union

import numpy as np
import numpy.typing as npt

import ifcopenshell
from ifcopenshell.entity_instance import entity_instance
import ifcopenshell.util.placement
import ifcopenshell.util.representation


_bound = False


def _bind():
    global _bound
    lib = ifcopenshell._get_lib()
    if _bound:
        return lib
    lib.ifcopenshell_representation_get_context.restype = ctypes.c_void_p
    lib.ifcopenshell_representation_get_context.argtypes = [ctypes.c_void_p, ctypes.c_char_p, ctypes.c_char_p, ctypes.c_char_p]
    lib.ifcopenshell_representation_resolve.restype = ctypes.c_void_p
    lib.ifcopenshell_representation_resolve.argtypes = [ctypes.c_void_p, ctypes.c_void_p]
    lib.ifcopenshell_representation_get_product_representation.restype = ctypes.c_void_p
    lib.ifcopenshell_representation_get_product_representation.argtypes = [
        ctypes.c_void_p, ctypes.c_void_p, ctypes.c_void_p,
        ctypes.c_char_p, ctypes.c_char_p, ctypes.c_char_p,
    ]
    lib.ifcopenshell_representation_resolve_base_items.restype = ctypes.POINTER(ctypes.c_void_p)
    lib.ifcopenshell_representation_resolve_base_items.argtypes = [ctypes.c_void_p, ctypes.c_void_p, ctypes.POINTER(ctypes.c_uint32)]
    lib.ifcopenshell_representation_get_prioritised_contexts.restype = ctypes.POINTER(ctypes.c_void_p)
    lib.ifcopenshell_representation_get_prioritised_contexts.argtypes = [ctypes.c_void_p, ctypes.POINTER(ctypes.c_uint32)]
    lib.ifcopenshell_free_instance_array_only.restype = None
    lib.ifcopenshell_free_instance_array_only.argtypes = [ctypes.POINTER(ctypes.c_void_p)]
    _bound = True
    return lib


def _enc(s):
    return s.encode("utf-8") if s else None

CONTEXT_TYPE = Literal["Model", "Plan", "NotDefined"]
REPRESENTATION_IDENTIFIER = Literal[
    "CoG",
    "Box",
    "Annotation",
    "Axis",
    "FootPrint",
    "Profile",
    "Surface",
    "Reference",
    "Body",
    "Body-Fallback",
    "Clearance",
    "Lighting",
]
TARGET_VIEW = Literal[
    "ELEVATION_VIEW",
    "GRAPH_VIEW",
    "MODEL_VIEW",
    "PLAN_VIEW",
    "REFLECTED_PLAN_VIEW",
    "SECTION_VIEW",
    "SKETCH_VIEW",
    "USERDEFINED",
    "NOTDEFINED",
]


def get_context(
    ifc_file: ifcopenshell.file,
    context: CONTEXT_TYPE,
    subcontext: Optional[REPRESENTATION_IDENTIFIER] = None,
    target_view: Optional[TARGET_VIEW] = None,
) -> Union[ifcopenshell.entity_instance, None]:
    """Get IfcGeometricRepresentationSubContext by the provided context type, identifier, and target view.

    :param context: ContextType.
    :param subcontext: A ContextIdentifier string, or any if left blank.
    :param target_view: A TargetView string, or any if left blank.
    """
    lib = _bind()
    rid = lib.ifcopenshell_representation_get_context(
        ifc_file._ptr, _enc(context), _enc(subcontext), _enc(target_view),
    )
    return entity_instance(ifc_file, rid) if rid else None


def is_representation_of_context(
    representation: ifcopenshell.entity_instance,
    context: Union[ifcopenshell.entity_instance, CONTEXT_TYPE],
    subcontext: Optional[REPRESENTATION_IDENTIFIER] = None,
    target_view: Optional[TARGET_VIEW] = None,
) -> bool:
    """Check if representation has specified context or context type, identifier, and target view.

    :param representation: IfcShapeRepresentation.
    :param context: Either a specific IfcGeometricRepresentationContext or a ContextType.
    :param subcontext: A ContextIdentifier string, or any if left blank.
    :param target_view: A TargetView string, or any if left blank.
    """

    if isinstance(context, ifcopenshell.entity_instance):
        return representation.ContextOfItems == context

    if target_view is not None:
        return (
            representation.ContextOfItems.is_a("IfcGeometricRepresentationSubContext")
            and representation.ContextOfItems.TargetView == target_view
            and representation.ContextOfItems.ContextIdentifier == subcontext
            and representation.ContextOfItems.ContextType == context
        )
    elif subcontext is not None:
        return (
            representation.ContextOfItems.is_a("IfcGeometricRepresentationSubContext")
            and representation.ContextOfItems.ContextIdentifier == subcontext
            and representation.ContextOfItems.ContextType == context
        )

    return representation.ContextOfItems.ContextType == context


def get_representations_iter(
    element: ifcopenshell.entity_instance,
) -> Generator[ifcopenshell.entity_instance, None, None]:
    """Get an iterator with element's IfcShapeRepresentations.

    :param element: An IfcProduct or IfcTypeProduct
    """
    if element.is_a("IfcProduct") and (rep := element.Representation):
        for r in rep.Representations:
            yield r
    elif element.is_a("IfcTypeProduct") and (maps := element.RepresentationMaps):
        for r in maps:
            yield r.MappedRepresentation


def get_representation(
    element: ifcopenshell.entity_instance,
    context: Union[ifcopenshell.entity_instance, CONTEXT_TYPE],
    subcontext: Optional[REPRESENTATION_IDENTIFIER] = None,
    target_view: Optional[TARGET_VIEW] = None,
) -> Union[ifcopenshell.entity_instance, None]:
    """Gets a IfcShapeRepresentation filtered by the context type, identifier, and target view

    :param element: An IfcProduct or IfcTypeProduct
    :param context: Either a specific IfcGeometricRepresentationContext or a ContextType
    :param subcontext: A ContextIdentifier string, or any if left blank.
    :param target_view: A TargetView string, or any if left blank.
    :return: The first IfcShapeRepresentation matching the criteria.
    """
    lib = _bind()
    ifc_file = element.file
    if isinstance(context, ifcopenshell.entity_instance):
        rid = lib.ifcopenshell_representation_get_product_representation(
            ifc_file._ptr, element._handle, context._handle, None, None, None,
        )
    else:
        rid = lib.ifcopenshell_representation_get_product_representation(
            ifc_file._ptr, element._handle, None,
            _enc(context), _enc(subcontext), _enc(target_view),
        )
    return entity_instance(ifc_file, rid) if rid else None


def guess_type(items: Sequence[ifcopenshell.entity_instance]) -> Union[str, None]:
    """Guesses the appropriate RepresentationType attribute based on a list of items

    :param items: A list of IfcRepresentationItem, typically in an IfcShapeRepresentation
    :return: The appropriate RepresentationType value, or None if no valid value
    """
    if all([True if i.is_a("IfcMappedItem") else False for i in items]):
        return "MappedRepresentation"
    elif all([True if i.is_a("IfcPoint") or i.is_a("IfcCartesianPointList") else False for i in items]):
        return "Point"
    elif all([True if i.is_a("IfcCartesianPointList3d") else False for i in items]):
        return "PointCloud"
    elif all([True if i.is_a("IfcCurve") and i.Dim == 2 else False for i in items]):
        return "Curve2D"
    elif all([True if i.is_a("IfcCurve") and i.Dim == 3 else False for i in items]):
        return "Curve3D"
    elif all([True if i.is_a("IfcCurve") else False for i in items]):
        return "Curve"
    elif all([True if i.is_a("IfcSegment") else False for i in items]):
        return "Segment"
    elif all([True if i.is_a("IfcSurface") and i.Dim == 2 else False for i in items]):
        return "Surface2D"
    elif all([True if i.is_a("IfcSurface") and i.Dim == 3 else False for i in items]):
        return "Surface3D"
    elif all([True if i.is_a("IfcSurface") else False for i in items]):
        return "Surface"
    elif all([True if i.is_a("IfcSectionedSurface") else False for i in items]):
        return "SectionedSurface"
    elif all([True if i.is_a("IfcAnnotationFillArea") else False for i in items]):
        return "FillArea"
    elif all([True if i.is_a("IfcTextLiteral") else False for i in items]):
        return "Text"
    elif all([True if i.is_a("IfcBSplineSurface") else False for i in items]):
        return "AdvancedSurface"
    elif all(
        [
            (
                True
                if i.is_a("IfcGeometricSet") or i.is_a("IfcPoint") or i.is_a("IfcCurve") or i.is_a("IfcSurface")
                else False
            )
            for i in items
        ]
    ):
        return "GeometricSet"
    elif all(
        [
            (
                True
                if i.is_a("IfcGeometricCurveSet")
                or (i.is_a("IfcGeometricSet") and all([e.is_a("IfcSurface") for e in i.Elements]))
                or i.is_a("IfcPoint")
                or i.is_a("IfcCurve")
                else False
            )
            for i in items
        ]
    ):
        return "GeometricCurveSet"
    elif all(
        [
            (
                True
                if i.is_a("IfcPoint")
                or i.is_a("IfcCurve")
                or i.is_a("IfcGeometricCurveSet")
                or i.is_a("IfcAnnotationFillArea")
                or i.is_a("IfcTextLiteral")
                else False
            )
            for i in items
        ]
    ):
        return "Annotation2D"
    elif all([True if i.is_a("IfcTessellatedItem") else False for i in items]):
        return "Tessellation"
    elif all(
        [
            (
                True
                if i.is_a("IfcTessellatedItem")
                or i.is_a("IfcShellBasedSurfaceModel")
                or i.is_a("IfcFaceBasedSurfaceModel")
                else False
            )
            for i in items
        ]
    ):
        return "SurfaceModel"
    elif all(
        [True if i.is_a() == "IfcExtrudedAreaSolid" or i.is_a() == "IfcRevolvedAreaSolid" else False for i in items]
    ):
        return "SweptSolid"
    elif all([True if i.is_a("IfcSolidModel") else False for i in items]):
        return "SolidModel"
    elif all(
        [
            (
                True
                if i.is_a("IfcTessellatedItem")
                or i.is_a("IfcShellBasedSurfaceModel")
                or i.is_a("IfcFaceBasedSurfaceModel")
                or i.is_a("IfcSolidModel")
                else False
            )
            for i in items
        ]
    ):
        return "SurfaceOrSolidModel"
    elif all(
        [
            (
                True
                if i.is_a("IfcSweptAreaSolid") or i.is_a("IfcSweptDiskSolid") or i.is_a("IfcSectionedSolidHorizontal")
                else False
            )
            for i in items
        ]
    ):
        return "AdvancedSweptSolid"
    elif all([True if i.is_a("IfcCsgSolid") or i.is_a("IfcBooleanClippingResult") else False for i in items]):
        return "Clipping"
    elif all(
        [
            True if i.is_a("IfcBooleanResult") or i.is_a("IfcCsgPrimitive3d") or i.is_a("IfcCsgSolid") else False
            for i in items
        ]
    ):
        return "CSG"
    elif all([True if i.is_a("IfcFacetedBrep") else False for i in items]):
        return "Brep"
    elif all([True if i.is_a("IfcManifoldSolidBrep") else False for i in items]):
        return "AdvancedBrep"
    elif all([True if i.is_a("IfcBoundingBox") else False for i in items]):
        return "BoundingBox"
    elif all([True if i.is_a("IfcSectionedSpine") else False for i in items]):
        return "SectionedSpine"
    elif all([True if i.is_a("IfcLightSource") else False for i in items]):
        return "LightSource"
    elif all([True if i.is_a("IfcVertex") else False for i in items]):
        return "Vertex"
    elif all([True if i.is_a("IfcEdge") else False for i in items]):
        return "Edge"
    elif all([True if i.is_a("IfcPath") else False for i in items]):
        return "Path"
    elif all([True if i.is_a("IfcFace") else False for i in items]):
        return "Face"
    elif all([True if i.is_a("IfcOpenShell") else False for i in items]):
        return "Shell"


def resolve_representation(representation: ifcopenshell.entity_instance) -> ifcopenshell.entity_instance:
    """Resolve possibly mapped representation.

    :param representation: IfcRepresentation
    :return: Representation resolved from mappings
    """
    lib = _bind()
    ifc_file = representation.file
    rid = lib.ifcopenshell_representation_resolve(ifc_file._ptr, representation._handle)
    return entity_instance(ifc_file, rid) if rid else representation


class ResolvedItemDict(TypedDict):
    matrix: npt.NDArray[np.float64]
    item: ifcopenshell.entity_instance


def _get_mappeditem_transformation(item: ifcopenshell.entity_instance) -> npt.NDArray[np.float64]:
    """Minimal fallback for ifcopenshell.util.placement.get_mappeditem_transformation.

    Used when the full function is not available in this package.
    """
    m4 = ifcopenshell.util.placement.get_axis2placement(item.MappingSource.MappingOrigin)
    target = item.MappingTarget
    if target.is_a("IfcCartesianTransformationOperator3D"):
        origin = np.array(target.LocalOrigin.Coordinates)
        axis1 = np.array(target.Axis1.DirectionRatios if target.Axis1 else (1.0, 0.0, 0.0))
        axis2 = np.array(target.Axis2.DirectionRatios if target.Axis2 else (0.0, 1.0, 0.0))
        axis3 = np.array(target.Axis3.DirectionRatios if target.Axis3 else (0.0, 0.0, 1.0))
        scale1 = target.Scale if target.Scale else 1.0
        scale2 = target.Scale2 if hasattr(target, "Scale2") and target.Scale2 else scale1
        scale3 = target.Scale3 if hasattr(target, "Scale3") and target.Scale3 else scale1
        axis1 = axis1 / np.linalg.norm(axis1) * scale1
        axis2 = axis2 / np.linalg.norm(axis2) * scale2
        axis3 = axis3 / np.linalg.norm(axis3) * scale3
        t = np.eye(4)
        t[0, :3] = axis1
        t[1, :3] = axis2
        t[2, :3] = axis3
        t[:3, 3] = origin
        return t @ m4
    return m4


def resolve_items(
    representation: ifcopenshell.entity_instance, matrix: Optional[npt.NDArray[np.float64]] = None
) -> list[ResolvedItemDict]:
    if matrix is None:
        matrix = np.eye(4)
    results: list[ResolvedItemDict] = []
    for item in representation.Items or []:  # Be forgiving of invalid IFCs because Revit :(
        if item.is_a("IfcMappedItem"):
            try:
                rep_matrix = ifcopenshell.util.placement.get_mappeditem_transformation(item)
            except AttributeError:
                rep_matrix = _get_mappeditem_transformation(item)
            if not np.allclose(rep_matrix, np.eye(4)):
                rep_matrix = rep_matrix @ matrix.copy()
            results.extend(resolve_items(item.MappingSource.MappedRepresentation, rep_matrix))
        else:
            results.append(ResolvedItemDict(matrix=matrix.copy(), item=item))
    return results


def resolve_base_items(
    representation: ifcopenshell.entity_instance,
) -> Generator[ifcopenshell.entity_instance, None, None]:
    """Resolve representation to it's base items resolving mapped items and boolean results to it's operands."""
    lib = _bind()
    ifc_file = representation.file
    count = ctypes.c_uint32(0)
    ptr = lib.ifcopenshell_representation_resolve_base_items(
        ifc_file._ptr, representation._handle, ctypes.byref(count)
    )
    try:
        for i in range(count.value):
            yield entity_instance(ifc_file, ptr[i])
    finally:
        if ptr:
            lib.ifcopenshell_free_instance_array_only(ptr)


def get_prioritised_contexts(ifc_file: ifcopenshell.file) -> list[ifcopenshell.entity_instance]:
    """Gets a list of contexts ordered from high priority to low priority

    Models can contain multiple geometric contexts. When visualising models,
    you may want to prioritise visualising certain contexts over others,
    determined by the context type, identifier, target view, and target scale.

    The default prioritises 3D, then 2D. It then prioritises subcontexts, then
    contexts. It then prioritises bodies, then others. It also prioritises
    model views, then plan views, then others.

    :param ifc_file: The model containing contexts
    :return: A list of IfcGeometricRepresentationContext (or SubContext) from
        high priority to low priority.
    """
    lib = _bind()
    count = ctypes.c_uint32(0)
    ptr = lib.ifcopenshell_representation_get_prioritised_contexts(ifc_file._ptr, ctypes.byref(count))
    try:
        return [entity_instance(ifc_file, ptr[i]) for i in range(count.value)]
    finally:
        if ptr:
            lib.ifcopenshell_free_instance_array_only(ptr)


def get_part_of_product(
    element: ifcopenshell.entity_instance, context: ifcopenshell.entity_instance
) -> Union[ifcopenshell.entity_instance, None]:
    """Gets the product definition or representation map of an element

    This is typically used for setting shape aspects. Note that this will
    return None for IFC2X3 element types.

    :param element: An IfcProduct or IfcTypeProduct
    :param context: A IfcGeometricRepresentationContext
    :return: IfcProductRepresentationSelect
    """
    if element.is_a("IfcProduct"):
        return element.Representation
    elif element.is_a("IfcTypeProduct") and element.file.schema != "IFX2X3":
        if maps := [r for r in element.RepresentationMaps if r.MappedRepresentation.ContextOfItems == context]:
            return maps[0]


def get_item_shape_aspect(
    representation: ifcopenshell.entity_instance, item: ifcopenshell.entity_instance
) -> Union[ifcopenshell.entity_instance, None]:
    """Gets the shape aspect relating to an item

    :param representation: The IfcShapeRepresentation that the item is part of
    :param item: The IfcRepresentationItem you want to get the shape aspect of
    :return: IfcShapeAspect, or None if none exists
    """
    for inverse in item.file.get_inverse(item):
        if (
            inverse.is_a("IfcShapeRepresentation")
            and inverse.ContextOfItems == representation.ContextOfItems
            and (of_shape_aspect := inverse.OfShapeAspect)
        ):
            return of_shape_aspect[0]


def get_material_style(
    material: ifcopenshell.entity_instance, context: ifcopenshell.entity_instance, ifc_class: str = "IfcSurfaceStyle"
) -> Union[ifcopenshell.entity_instance, None]:
    """Get a presentation style associated with a material

    :param material: the IfcMaterial
    :param context: IfcGeometricRepresentationContext that the style belongs to
    :param ifc_class: The class name of the type of style you need, typically
        IfcSurfaceStyle for 3D styling.
    :return: IfcPresentationStyle
    """
    if definition_representation := material.HasRepresentation:
        for styled_rep in definition_representation[0].Representations:
            if styled_rep.ContextOfItems == context:
                for item in styled_rep.Items:
                    for style in item.Styles:
                        if style.is_a(ifc_class):
                            return style


def get_reference_line(wall: ifcopenshell.entity_instance, fallback_length: float = 1.0) -> list[npt.NDArray]:
    """Fetch the reference axis that goes in the +X direction

    A base line will then be offset from this reference line based on the
    material usage. From that base line, the layer thicknesses will offset
    again, and be extruded to form the body representation.

    :param wall: ifcopenshell.entity_instance
    :param fallback_length: If there is no reference axis, assume it starts at
        the object placement (i.e. 0.0, 0.0) and extends for this fallback
        length along the +X axis.
    :return: A list of two 2D coordinates representing the start and end of the
        axis. The axis always goes in the +X direction.
    """
    if axis := ifcopenshell.util.representation.get_representation(wall, "Plan", "Axis", "GRAPH_VIEW"):
        for item in ifcopenshell.util.representation.resolve_representation(axis).Items:
            if item.is_a("IfcPolyline"):
                points = [p[0] for p in item.Points]
            elif item.is_a("IfcIndexedPolyCurve"):
                points = item.Points.CoordList
            else:
                continue
            if points[0][0] < points[1][0]:  # An axis always goes in the +X direction
                return [np.array(points[0]), np.array(points[1])]
            return [np.array(points[1]), np.array(points[0])]
    # ifcopenshell.util.shape.get_base_extrusions dependency – optional
    try:
        import ifcopenshell.util.shape

        if extrusions := ifcopenshell.util.shape.get_base_extrusions(wall):
            for extrusion in extrusions:
                profile = extrusion.SweptArea
                curve = getattr(profile, "OuterCurve", None)
                if not curve:
                    continue
                elif curve.is_a("IfcPolyline"):
                    x = [p[0][0] for p in curve.Points]
                elif curve.is_a("IfcIndexedPolyCurve"):
                    x = [p[0] for p in curve.Points.CoordList]
                else:
                    continue
                return [np.array((min(x), 0.0)), np.array((max(x), 0.0))]
    except (ImportError, AttributeError):
        pass
    return [np.array((0.0, 0.0)), np.array((fallback_length, 0.0))]
