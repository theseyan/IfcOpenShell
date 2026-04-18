
import pytest

import ifcopenshell.api.aggregate
import ifcopenshell.api.cogo
import ifcopenshell.api.context
import ifcopenshell.guid
import test.bootstrap


class TestAddSurveyPoint(test.bootstrap.IFC4X3):
    def test_add_survey_point(self):
        project = self.file.createIfcProject(Name="Test")
        site = self.file.createIfcSite(GlobalId=ifcopenshell.guid.new(), Name="MySite")
        ifcopenshell.api.aggregate.assign_object(self.file, relating_object=project, products=[site])
        geometric_representation_context = ifcopenshell.api.context.add_context(self.file, context_type="Model")
        ifcopenshell.api.context.add_context(
            self.file,
            context_type="Model",
            context_identifier="Annotation",
            target_view="MODEL_VIEW",
            parent=geometric_representation_context,
        )

        annotation = ifcopenshell.api.cogo.add_survey_point(
            self.file, self.file.createIfcCartesianPoint((50.0, 10.0)))
        assert annotation
        assert annotation.PredefinedType == "SURVEY"
        assert annotation.Representation.Representations[0].RepresentationIdentifier == "Annotation"
        assert annotation.Representation.Representations[0].RepresentationType == "Point"
        assert annotation.Representation.Representations[0].Items[0].Coordinates == pytest.approx((50.0, 10.0))
