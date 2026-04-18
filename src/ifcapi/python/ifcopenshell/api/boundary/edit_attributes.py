# SPDX-License-Identifier: LGPL-3.0-or-later


def edit_attributes(file, entity=None, relating_space=None, related_building_element=None,
                    parent_boundary=None, corresponding_boundary=None,
                    physical_or_virtual="NOTDEFINED", internal_or_external="NOTDEFINED"):
    entity.RelatingSpace = relating_space
    entity.RelatedBuildingElement = related_building_element
    if hasattr(entity, "ParentBoundary"):
        entity.ParentBoundary = parent_boundary
    if hasattr(entity, "CorrespondingBoundary"):
        entity.CorrespondingBoundary = corresponding_boundary
    entity.PhysicalOrVirtualBoundary = physical_or_virtual
    entity.InternalOrExternalBoundary = internal_or_external
