# IfcOpenShell - IFC toolkit and geometry engine
# Copyright (C) 2021 Thomas Krijnen <thomas@aecgeeks.com>
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

import ifcopenshell
from ifcopenshell.api.alignment import _capi


def add_linear_placement_fallback_position(
    file: ifcopenshell.file,
) -> ifcopenshell.file:
    import ifcopenshell.api.alignment

    patched_file = ifcopenshell.file.from_string(file.wrapped_data.to_string())

    linear_placements = patched_file.by_type("IfcLinearPlacement")
    for lp in linear_placements:
        ifcopenshell.api.alignment.update_fallback_position(patched_file, lp)

    return patched_file


def create_alignment_geometry(file: ifcopenshell.file) -> ifcopenshell.file:
    import ifcopenshell.api.alignment

    patched_file = ifcopenshell.file.from_string(file.wrapped_data.to_string())

    alignments = patched_file.by_type("IfcAlignment")
    for alignment in alignments:
        ifcopenshell.api.alignment.create_representation(patched_file, alignment)

    return patched_file


def append_zero_length_segments(file: ifcopenshell.file) -> ifcopenshell.file:
    """Appends zero length segments to all alignment layouts and layout geometry, if missing."""
    import ifcopenshell.api.alignment

    patched_file = ifcopenshell.file.from_string(file.wrapped_data.to_string())

    alignments = patched_file.by_type("IfcAlignment")
    for alignment in alignments:
        layouts = ifcopenshell.api.alignment.get_alignment_layouts(alignment)
        for layout in layouts:
            ifcopenshell.api.alignment.add_zero_length_segment(patched_file, layout)
            curve = ifcopenshell.api.alignment.get_layout_curve(layout)
            if curve:
                ifcopenshell.api.alignment.add_zero_length_segment(patched_file, curve)

    return patched_file


def station_as_string(file: ifcopenshell.file, sta: float):
    return _capi.call_value("alignment_station_as_string", _capi.file_handle(file), sta)
