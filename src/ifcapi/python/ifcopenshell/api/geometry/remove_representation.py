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

import ifcopenshell
from ifcopenshell.api.geometry import _capi


def remove_representation(
    file: ifcopenshell.file, representation: ifcopenshell.entity_instance, should_keep_named_profiles: bool = True
) -> None:
    """Remove a representation.

    Also purges representation items and their related elements
    like IfcStyledItem, tessellated facesets colours and UV map.

    By default, named profiles are assumed to be significant (i.e. curated as
    part of a profile library) and will not be removed.

    :param representation: IfcRepresentation to remove.
        Note that it's expected that IfcRepresentation won't be in use
        before calling this method (in such elements as IfcProductRepresentation, IfcShapeAspect)
        otherwise representation won't be removed.
    :param should_keep_named_profiles: If true, named profile defs will not be
        removed as they are assumed to be significant.
    """
    _capi.call_status(
        "geometry_remove_representation",
        _capi.file_handle(file),
        _capi.instance_handle(representation),
        {
            "should_keep_named_profiles": should_keep_named_profiles,
        },
    )
