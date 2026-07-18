# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell.api.structural
import pytest

import test.bootstrap


class TestAddStructuralActivity(test.bootstrap.IFC4):
    ifc_class = "IfcStructuralCurveAction"
    predefined_attribute = "PredefinedType"

    def setup_inputs(self):
        load = self.file.create_entity("IfcStructuralLoadLinearForce")
        member = self.file.create_entity("IfcStructuralCurveMember")
        return load, member

    def test_preserves_schema_dynamic_class_and_predefined_type(self):
        load, member = self.setup_inputs()
        activity = ifcopenshell.api.structural.add_structural_activity(
            self.file,
            applied_load=load,
            structural_member=member,
            ifc_class=self.ifc_class,
            predefined_type="LINEAR",
            global_or_local="LOCAL_COORDS",
        )
        assert activity.is_a(self.ifc_class)
        assert getattr(activity, self.predefined_attribute) == "LINEAR"
        assert activity.GlobalOrLocal == "LOCAL_COORDS"

    def test_rejects_unknown_coordinate_system_without_mutation(self):
        load, member = self.setup_inputs()
        before = len(list(self.file))
        with pytest.raises(
            ValueError, match="Unsupported StructuralGlobalOrLocal value"
        ):
            ifcopenshell.api.structural.add_structural_activity(
                self.file,
                applied_load=load,
                structural_member=member,
                global_or_local="global_coords",
            )
        assert len(list(self.file)) == before


class TestAddStructuralActivityIFC2X3(test.bootstrap.IFC2X3, TestAddStructuralActivity):
    ifc_class = "IfcStructuralLinearAction"
    predefined_attribute = "ObjectType"
