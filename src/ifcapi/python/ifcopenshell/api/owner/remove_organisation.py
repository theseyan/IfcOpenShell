# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell.api.owner
import ifcopenshell.api.root


def remove_organisation(file, organisation=None):
    for role in organisation.Roles or []:
        if (file.get_total_inverses(role)) == 1:
            ifcopenshell.api.owner.remove_role(file, role=role)
    for address in organisation.Addresses or []:
        if (file.get_total_inverses(address)) == 1:
            ifcopenshell.api.owner.remove_address(file, address=address)
    for inverse in file.get_inverse(organisation):
        if inverse.is_a("IfcOrganizationRelationship"):
            if inverse.RelatingOrganization == organisation:
                file.remove(inverse)
            elif inverse.RelatedOrganizations == (organisation,):
                file.remove(inverse)
        elif inverse.is_a("IfcDocumentInformation"):
            if inverse.Editors == (organisation,):
                inverse.Editors = None
        elif inverse.is_a("IfcPersonAndOrganization"):
            ifcopenshell.api.owner.remove_person_and_organisation(file, person_and_organisation=inverse)
        elif inverse.is_a("IfcActor"):
            ifcopenshell.api.root.remove_product(file, product=inverse)
        elif inverse.is_a("IfcResourceLevelRelationship") and not inverse.is_a("IfcOrganizationRelationship"):
            if inverse.RelatedResourceObjects == (organisation,):
                file.remove(inverse)
        elif inverse.is_a("IfcApplication"):
            ifcopenshell.api.owner.remove_application(file, application=inverse)

    file.remove(organisation)
