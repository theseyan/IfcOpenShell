# IfcOpenShell - IFC toolkit and geometry engine
# SPDX-License-Identifier: LGPL-3.0-or-later

from typing import Optional

import ifcopenshell
from ifcopenshell import _ifcopenshell_capi as _capi


def reassign_class(
    file: ifcopenshell.file,
    product: ifcopenshell.entity_instance,
    ifc_class: str = "IfcBuildingElementProxy",
    predefined_type: Optional[str] = None,
    occurrence_class: Optional[str] = None,
) -> ifcopenshell.entity_instance:
    """Changes the class of a product while retaining applicable data.

    Reassigning a type also reassigns its occurrences. Reassigning a typed
    occurrence also updates its related type and sibling occurrences where the
    schema mapping is unambiguous. Switching between occurrence and type
    classes is supported.

    :param product: The product or type product to reassign. Its old handle is
        invalid after this call succeeds.
    :param ifc_class: The target IFC class.
    :param predefined_type: An optional predefined type. Unsupported values are
        stored as USERDEFINED with the custom label in ObjectType or ElementType.
    :param occurrence_class: Optional occurrence class override when the target
        is an IfcTypeProduct, primarily for ambiguous IFC2X3 mappings.
    :return: The reassigned product, retaining its original STEP id.
    """
    handle = _capi.root_reassign_class(
        file._handle,
        {
            "product": product._handle,
            "ifc_class": ifc_class,
            "predefined_type": predefined_type,
            "occurrence_class": occurrence_class,
        },
    )
    _capi.instance_destroy(product._handle)
    product._handle = None
    return ifcopenshell.entity_instance(file, handle)
