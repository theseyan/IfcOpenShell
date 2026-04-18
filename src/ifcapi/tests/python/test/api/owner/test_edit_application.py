# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell.api.owner
import test.bootstrap


class TestEditApplication(test.bootstrap.IFC4):
    def test_editing_a_application(self):
        application = self.file.create_entity("IfcApplication")
        organization = self.file.create_entity("IfcOrganization")
        attributes = {
            "ApplicationDeveloper": organization,
            "Version": "v001",
            "ApplicationFullName": "App Name",
            "ApplicationIdentifier": "App Name",
        }
        ifcopenshell.api.owner.edit_application(
            self.file,
            application=application,
            attributes=attributes,
        )
        for attr, value in attributes.items():
            assert getattr(application, attr) == value


class TestEditApplicationIFC2X3(test.bootstrap.IFC2X3, TestEditApplication):
    pass
