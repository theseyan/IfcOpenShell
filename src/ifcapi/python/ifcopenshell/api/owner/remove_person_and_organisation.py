# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell.api.root


def remove_person_and_organisation(file, person_and_organisation=None):
    for inverse in file.get_inverse(person_and_organisation):
        if inverse.is_a("IfcDocumentInformation"):
            if inverse.Editors == (person_and_organisation,):
                inverse.Editors = None
        elif inverse.is_a("IfcActor"):
            ifcopenshell.api.root.remove_product(file, product=inverse)
        elif inverse.is_a("IfcResourceLevelRelationship"):
            if inverse.RelatedResourceObjects == (person_and_organisation,):
                file.remove(inverse)
        elif inverse.is_a("IfcOwnerHistory"):
            file.remove(inverse)
    file.remove(person_and_organisation)
