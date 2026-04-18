# SPDX-License-Identifier: LGPL-3.0-or-later
"""Thin ctypes wrapper around ifcopenshell_geometry_unassign_representation.

The native implementation handles the ``IfcProduct`` case in full. The
``IfcTypeProduct`` case (which touches MapUsage, shape aspects and
``remove_deep2``) is still orchestrated in Python until ported.
"""

from __future__ import annotations

import ctypes

import ifcopenshell
import ifcopenshell.api.geometry
import ifcopenshell.util.element
from ... import _get_lib


def unassign_representation(
    file: ifcopenshell.file,
    product: ifcopenshell.entity_instance,
    representation: ifcopenshell.entity_instance,
) -> None:
    if product.is_a("IfcProduct"):
        lib = _get_lib()
        lib.ifcopenshell_geometry_unassign_representation.restype = None
        lib.ifcopenshell_geometry_unassign_representation.argtypes = [
            ctypes.c_void_p, ctypes.c_void_p, ctypes.c_void_p,
        ]
        lib.ifcopenshell_geometry_unassign_representation(
            file._ptr, product._handle, representation._handle
        )
        return
    if product.is_a("IfcTypeProduct"):
        _unassign_type_representation(file, product, representation)


def _unassign_type_representation(file, product, representation):
    matching_representation_map = None
    for representation_map in product.RepresentationMaps or []:
        if representation_map.MappedRepresentation == representation:
            matching_representation_map = representation_map
            break
    if not matching_representation_map:
        return
    _unassign_products_using_mapped_representation(file, matching_representation_map)
    product.RepresentationMaps = [
        rm for rm in product.RepresentationMaps if rm != matching_representation_map
    ] or None
    _process_shape_aspects(file, matching_representation_map)
    _remove_representation_map_only(file, matching_representation_map)


def _process_shape_aspects(file, product_representation):
    if file.schema == "IFC2X3" and product_representation.is_a("IfcRepresentationMap"):
        shape_aspects = [
            a for a in file.by_type("IfcShapeAspect")
            if a.PartOfProductDefinitionShape == product_representation
        ]
    else:
        shape_aspects = product_representation.HasShapeAspects
    for shape_aspect in shape_aspects:
        representations = shape_aspect.ShapeRepresentations
        file.remove(shape_aspect)
        for rep in representations:
            ifcopenshell.api.geometry.remove_representation(file, rep)


def _remove_representation_map_only(file, representation_map):
    representation_map.MappedRepresentation = file.createIfcShapeRepresentation()
    ifcopenshell.util.element.remove_deep2(file, representation_map)


def _unassign_products_using_mapped_representation(file, representation_map):
    mapped_representations = []
    just_representations = []
    for map_usage in representation_map.MapUsage or []:
        for inverse in file.get_inverse(map_usage):
            if not inverse.is_a("IfcShapeRepresentation"):
                continue
            for definition in inverse.OfProductRepresentation or []:
                for product in definition.ShapeOfProduct or []:
                    mapped_representations.append({"product": product, "representation": inverse})
                    just_representations.append(inverse)
    for item in mapped_representations:
        unassign_representation(file, item["product"], item["representation"])
    for representation in just_representations:
        ifcopenshell.api.geometry.remove_representation(file, representation=representation)
