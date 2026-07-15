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

from typing import Any

import ifcopenshell
from ifcopenshell.api.attribute import _capi


def _edit_attributes(
    file: ifcopenshell.file,
    product: ifcopenshell.entity_instance,
    attributes: dict[str, Any],
    *,
    sync_predefined_type: bool = False,
    update_owner_history: bool = False,
) -> None:
    from ifcopenshell.api.pset import _capi as pset_capi

    props = pset_capi.build_props(attributes)
    try:
        user, application = _capi.owner_context(file)
        _capi.call_status(
            "attribute_edit_attributes",
            _capi.file_handle(file),
            {
                "product": _capi.instance_handle(product),
                "attributes": props,
                "sync_predefined_type": sync_predefined_type,
                "update_owner_history": update_owner_history,
                "user": _capi.instance_handle(user),
                "application": _capi.instance_handle(application),
            },
        )
    finally:
        pset_capi.free_props(props)


def _edit_named_attributes(
    file: ifcopenshell.file,
    target: ifcopenshell.entity_instance,
    attributes: dict[str, Any],
    operation: str,
    *entity_arguments,
) -> None:
    from ifcopenshell.api.pset import _capi as pset_capi

    props = pset_capi.build_props(attributes)
    try:
        _capi.call_status(
            operation,
            _capi.file_handle(file),
            _capi.instance_handle(target),
            props,
            *(_capi.instance_handle(argument) for argument in entity_arguments),
        )
    finally:
        pset_capi.free_props(props)


def edit_attributes(file: ifcopenshell.file, product: ifcopenshell.entity_instance, attributes: dict[str, Any]) -> None:
    """Edit the attributes of a product

    All IFC entities have attributes. Normally they can be edited directly,
    by simply assigning a new value to them. In some scenarios, you may wish
    to also ensure that ownership history is updated. This function provides
    that convenience.

    The method will maintain consistency for PredefinedType attribute
    based on whether ElementType/ObjectType and whether occurrence is typed:

    - PredefinedType and ObjectType to be `None` if occurrence is typed
    - PredefinedType to be "NOTDEFINED" if ElementType/ObjectType is None
    - PredefinedType to be "USERDEFINED" if ElementType/ObjectType is not None

    :param product: The product you want to edit. This may be any rooted IFC
        entity.
    :param attributes: a dictionary of attribute names and values.
    :return: None

    Example:

    .. code:: python

        element = ifcopenshell.api.root.create_entity(model, ifc_class="IfcWall")
        ifcopenshell.api.attribute.edit_attributes(model,
            product=element, attributes={"Name": "Waldo"})
    """

    _edit_attributes(file, product, attributes, sync_predefined_type=True, update_owner_history=True)
