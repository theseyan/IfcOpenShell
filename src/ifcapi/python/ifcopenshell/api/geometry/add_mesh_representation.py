# IfcOpenShell - IFC toolkit and geometry engine
# Copyright (C) 2022 Dion Moult <dion@thinkmoult.com>
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

from typing import Optional

import numpy as np

import ifcopenshell
from ifcopenshell.util.shape_builder import SequenceOfVectors, VectorType

from . import _capi


def add_mesh_representation(
    file: ifcopenshell.file,
    context: ifcopenshell.entity_instance,
    vertices: list[SequenceOfVectors],
    edges: Optional[list[list[tuple[int, int]]]] = None,
    # Optional faces is not supported currently.
    faces: list[list[list[int]]] = None,
    coordinate_offset: Optional[VectorType] = None,
    unit_scale: Optional[float] = None,
    force_faceted_brep: bool = False,
) -> ifcopenshell.entity_instance:
    """
    Add a mesh representation.

    Vertices, edges, and faces are given in the form of: ``[item1, item2, item3, ...]``.
    Each ``itemN`` is a sublist representing data for a separate IfcRepresentationItem to add.

    You can provide either ``edges`` or ``faces``, no need to provide both.
    But currently ``edges`` argument is not supported.

    :param context: The IfcGeometricRepresentationContext for the representation.
    :param vertices: A list of coordinates.
        where ``itemN = [(0., 0., 0.), (1., 1., 1.), (x, y, z), ...]``
    :param edges: A list of edges, represented by vertex index pairs
        where ``itemN = [(0, 1), (1, 2), (v1, v2), ...]``
        This remains accepted for upstream API compatibility but is ignored;
        faces are still required.
    :param faces: A list of polygons, represented by vertex indices.
        where ``itemN = [(0, 1, 2), (5, 4, 2, 3), (v1, v2, v3, ... vN), ...]``
    :param coordinate_offset: Optionally apply a vector offset to all coordinates.
        In project units.
    :param unit_scale: Scale factor for ``vertices`` units.

        If omitted, it is assumed that ``vertices`` are in SI units.

        If other value is provided ``vertices`` coords will be divided by ``unit_scale``.
    :param force_faceted_brep: Force using IfcFacetedBreps instead of IfcPolygonalFaceSets.
    :return: IfcShapeRepresentation.
    """
    # TODO: Support edges without faces.
    assert faces is not None, f"Currently 'faces' argument is not optional."
    assert len(faces) != 0
    assert len(vertices) != 0
    assert len(faces) == len(vertices)

    normalized_faces = _mesh_faces(faces)
    items = [
        {
            "vertices": np.asarray(item_vertices, dtype=np.float64).tolist(),
            "faces": item_faces,
        }
        for item_vertices, item_faces in zip(vertices, normalized_faces)
    ]
    return _capi.call_handle(
        file,
        "geometry_add_mesh_representation",
        _capi.file_handle(file),
        _capi.instance_handle(context),
        {
            "items": items,
            "coordinate_offset": None
            if coordinate_offset is None
            else list(coordinate_offset),
            "unit_scale": unit_scale,
            "force_faceted_brep": force_faceted_brep,
        },
    )


def _mesh_faces(faces):
    def is_sequence_of_ints(value):
        return isinstance(value, (list, tuple)) and all(
            isinstance(item, int) for item in value
        )

    def is_sequence_of_sequence_of_ints(value):
        return isinstance(value, (list, tuple)) and all(
            is_sequence_of_ints(item) for item in value
        )

    result = []
    for item_faces in faces:
        normalized_item_faces = []
        for face in item_faces:
            if is_sequence_of_ints(face):
                loops = [face]
            elif is_sequence_of_sequence_of_ints(face):
                loops = face
            else:
                raise ValueError(
                    "Expected a sequence of int or sequence of sequence of int for each face"
                )
            if not loops:
                raise ValueError("Expected at least one loop for each face")
            normalized_item_faces.append(
                {
                    "outer": list(loops[0]),
                    "inner_loops": [list(loop) for loop in loops[1:]] or None,
                }
            )
        result.append(normalized_item_faces)
    return result
