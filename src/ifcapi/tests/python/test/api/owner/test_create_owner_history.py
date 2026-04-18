# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

import time

import pytest

import ifcopenshell.api
import ifcopenshell.api.owner.settings
import test.bootstrap


class TestCreateOwnerHistory(test.bootstrap.IFC4):
    def test_creating_nothing_if_no_user_or_application_is_available(self):
        if self.file.schema != "IFC2X3":
            history = ifcopenshell.api.owner.create_owner_history(self.file)
            assert history is None
        else:
            ifcopenshell.api.owner.settings.factory_reset()
            # create new file as bootstrap is creating users in ifc2x3 by default
            file = ifcopenshell.file(schema="IFC2X3")
            with pytest.raises(Exception) as e:
                ifcopenshell.api.owner.create_owner_history(file)
            assert "Please create a user to continue" in str(e.value)
            ifcopenshell.api.owner.settings.restore()

    def test_creating_a_history_using_a_specified_user_and_application(self):
        ifcopenshell.api.owner.settings.factory_reset()

        user = self.file.createIfcPersonAndOrganization()
        application = self.file.createIfcApplication()
        ifcopenshell.api.owner.settings.get_user = lambda x: user
        ifcopenshell.api.owner.settings.get_application = lambda x: application
        history = ifcopenshell.api.owner.create_owner_history(self.file)
        ifcopenshell.api.owner.settings.restore()

        assert history.is_a("IfcOwnerHistory")
        assert history.OwningUser == user
        assert history.OwningApplication == application
        assert history.State == "READWRITE"
        assert history.ChangeAction == "ADDED"
        assert abs(time.time() - history.LastModifiedDate) < 5
        assert history.LastModifyingUser == user
        assert history.LastModifyingApplication == application
        assert abs(time.time() - history.CreationDate) < 5


class TestCreateOwnerHistoryIFC2X3(test.bootstrap.IFC2X3, TestCreateOwnerHistory):
    pass
