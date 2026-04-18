# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell.api.owner
import ifcopenshell.api.root


def remove_person(file, person=None):
    for role in person.Roles or []:
        if file.get_total_inverses(role) == 1:
            ifcopenshell.api.owner.remove_role(file, role=role)
    for address in person.Addresses or []:
        if file.get_total_inverses(address) == 1:
            ifcopenshell.api.owner.remove_address(file, address=address)
    for inverse in file.get_inverse(person):
        if inverse.is_a("IfcWorkControl"):
            if inverse.Creators == (person,):
                inverse.Creators = None
        elif inverse.is_a("IfcInventory"):
            if inverse.ResponsiblePersons == (person,):
                # in IFC2X3 ResponsiblePersons is not optional and without it IfcInventory is not valid
                if file.schema == "IFC2X3":
                    ifcopenshell.api.root.remove_product(file, product=inverse)
        elif inverse.is_a("IfcDocumentInformation"):
            if inverse.Editors == (person,):
                inverse.Editors = None
        elif inverse.is_a("IfcPersonAndOrganization"):
            ifcopenshell.api.owner.remove_person_and_organisation(file, person_and_organisation=inverse)
        elif inverse.is_a("IfcActor"):
            ifcopenshell.api.root.remove_product(file, product=inverse)
        elif inverse.is_a("IfcResourceLevelRelationship"):
            if inverse.RelatedResourceObjects == (person,):
                file.remove(inverse)
    file.remove(person)
