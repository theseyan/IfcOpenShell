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
from ifcopenshell import _generated_capi
from ifcopenshell.api.library import _capi


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
    lib = _capi.get_lib()
    owner_history, user, application = _capi.owner_context(file)
    product_list = _capi.instance_list(products)
    handle = _generated_capi.call_handle_or_raise(
        lib,
        lib.ifcopenshell_ifcapi_library_assign_reference,
        "Failed to assign library reference",
        _capi.file_handle(file),
        product_list,
        _capi.instance_handle(reference),
        _capi.instance_handle(owner_history),
        _capi.instance_handle(user),
        _capi.instance_handle(application),
        destroy=lib.ifcopenshell_ifc_instance_destroy,
    )
    return _capi.wrap_handle(file, handle)
