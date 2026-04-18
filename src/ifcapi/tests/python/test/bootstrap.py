# SPDX-License-Identifier: LGPL-3.0-or-later

"""Test bootstrap adapted for the native ifcapi backend."""

import pytest

import ifcopenshell
import ifcopenshell.api.owner.settings
import ifcopenshell.api.project


class IFC4X3:
    @pytest.fixture(autouse=True)
    def setup(self):
        self.file = ifcopenshell.api.project.create_file(version="IFC4X3")
        ifcopenshell.api.owner.settings.get_user = lambda ifc: (ifc.by_type("IfcPersonAndOrganization") or [None])[0]
        ifcopenshell.api.owner.settings.get_application = lambda ifc: (ifc.by_type("IfcApplication") or [None])[0]
        ifcopenshell.api.pre_listeners = {}
        ifcopenshell.api.post_listeners = {}


class IFC4:
    @pytest.fixture(autouse=True)
    def setup(self):
        self.file = ifcopenshell.api.project.create_file()
        ifcopenshell.api.owner.settings.get_user = lambda ifc: (ifc.by_type("IfcPersonAndOrganization") or [None])[0]
        ifcopenshell.api.owner.settings.get_application = lambda ifc: (ifc.by_type("IfcApplication") or [None])[0]
        ifcopenshell.api.pre_listeners = {}
        ifcopenshell.api.post_listeners = {}


class IFC2X3:
    @pytest.fixture(autouse=True)
    def setup(self):
        self.file = ifcopenshell.api.project.create_file(version="IFC2X3")

        def get_user(ifc):
            user = next(iter(ifc.by_type("IfcPersonAndOrganization")), None)
            if user:
                return user
            person = ifc.create_entity("IfcPerson")
            organization = ifc.create_entity("IfcOrganization")
            return ifc.create_entity("IfcPersonAndOrganization", ThePerson=person, TheOrganization=organization)

        ifcopenshell.api.owner.settings.get_user = get_user

        def get_application(ifc):
            application = next(iter(ifc.by_type("IfcApplication")), None)
            if application:
                return application
            return ifc.create_entity("IfcApplication")

        ifcopenshell.api.owner.settings.get_application = get_application

        ifcopenshell.api.pre_listeners = {}
        ifcopenshell.api.post_listeners = {}
