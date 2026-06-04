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

from typing import Union

import ifcopenshell
import ifcopenshell.api.owner.settings
from ifcopenshell import _ifcopenshell_capi as _capi


def assign_reference(
    file: ifcopenshell.file, products: ifcopenshell.entity_instance, reference: ifcopenshell.entity_instance
) -> Union[ifcopenshell.entity_instance, None]:
    """Associates a list products with a library reference

    A product may be associated with zero, one, or many references across
    multiple libraries. See ifcopenshell.api.library.add_reference for more
    detail about how references work.

    :param products: The list of IfcProducts you want to associate with the reference
    :param reference: The IfcLibraryReference you want the product to be
        associated with.
    :return: The IfcRelAssociatesLibrary relationship entity
        or `None` if `products` was an empty list or all products were
        already assigned to the `reference`.

    Example:

    .. code:: python

        library = ifcopenshell.api.library.add_library(model, name="Brickschema")

        # Let's create a reference to a single AHU in our Brickschema dataset
        reference = ifcopenshell.api.library.add_reference(model, library=library)
        ifcopenshell.api.library.edit_reference(model,
            reference=reference, attributes={"Identification": "http://example.org/digitaltwin#AHU01"})

        # Let's assume we have an AHU in our model.
        ahu = ifcopenshell.api.root.create_entity(model,
            ifc_class="IfcUnitaryEquipment", predefined_type="AIRHANDLER")

        # And now assign the IFC model's AHU with its Brickschema counterpart
        ifcopenshell.api.library.assign_reference(model, reference=reference, products=[ahu])
    """
    user = ifcopenshell.api.owner.settings.get_user(file)
    application = ifcopenshell.api.owner.settings.get_application(file)
    product_list = [e._handle for e in products]
    handle = _capi.library_assign_reference(
        file._handle,
        product_list,
        reference._handle,
        None,
        user._handle if user is not None else None,
        application._handle if application is not None else None,
    )
    if handle:
        return ifcopenshell.entity_instance(file, handle)
    raise RuntimeError(_capi.last_error_message() or "Failed to assign library reference")
