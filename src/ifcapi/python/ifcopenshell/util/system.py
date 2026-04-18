# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

"""Utility functions for distribution systems."""

group_types = {
    "IfcZone": ("IfcZone", "IfcSpace", "IfcSpatialZone"),
    "IfcBuiltSystem": (
        "IfcBuiltElement", "IfcFurnishingElement",
        "IfcElementAssembly", "IfcTransportElement",
    ),
    "IfcBuildingSystem": (
        "IfcBuildingElement", "IfcFurnishingElement",
        "IfcElementAssembly", "IfcTransportElement",
    ),
    "IfcDistributionSystem": ("IfcDistributionElement",),
    "IfcStructuralAnalysisModel": ("IfcStructuralMember", "IfcStructuralConnection"),
    "IfcSystem": ("IfcProduct",),
    "IfcGroup": ("IfcObjectDefinition",),
}
group_types["IfcDistributionCircuit"] = group_types["IfcDistributionSystem"]
group_types["IfcElectricalCircuit"] = group_types["IfcSystem"]


def is_assignable(product, system):
    for assignable in group_types.get(system.is_a(), ()):
        if product.is_a(assignable):
            return True
    return False


def get_system_elements(system):
    results = []
    for rel in system.IsGroupedBy:
        results.extend(rel.RelatedObjects)
    return results
