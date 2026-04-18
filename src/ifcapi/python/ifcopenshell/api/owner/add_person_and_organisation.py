# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell


def add_person_and_organisation(file, person=None, organisation=None):
    return file.create_entity("IfcPersonAndOrganization", person, organisation)
