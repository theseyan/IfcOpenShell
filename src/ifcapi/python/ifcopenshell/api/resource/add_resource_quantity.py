# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

RESOURCES_TO_QUANTITIES = {
    "IfcConstructionEquipmentResource": ("IfcQuantityTime",),
    "IfcConstructionMaterialResource": (
        "IfcQuantityArea",
        "IfcQuantityLength",
        "IfcQuantityVolume",
        "IfcQuantityWeight",
    ),
    "IfcConstructionProductResource": ("IfcQuantityCount",),
    "IfcCrewResource": ("IfcQuantityTime",),
    "IfcLaborResource": ("IfcQuantityTime",),
    "IfcSubContractResource": ("IfcQuantityTime",),
}


def add_resource_quantity(file, resource=None, ifc_class="IfcQuantityCount"):
    resource_type = resource.is_a()
    supported_quantities = RESOURCES_TO_QUANTITIES.get(resource_type, ())
    if ifc_class not in supported_quantities:
        raise ValueError(
            f"Resource type '{resource_type}' does not support quantity type '{ifc_class}'. "
            f"Supported quantities: {','.join(supported_quantities)}"
        )

    quantity = file.create_entity(ifc_class, Name="Unnamed")
    # IfcPhysicalSimpleQuantity value is attribute index 3
    if ifc_class == "IfcQuantityCount":
        quantity[3] = 0
    else:
        quantity[3] = 0.0
    old_quantity = resource.BaseQuantity
    resource.BaseQuantity = quantity
    if old_quantity:
        file.remove(old_quantity)
    return quantity
