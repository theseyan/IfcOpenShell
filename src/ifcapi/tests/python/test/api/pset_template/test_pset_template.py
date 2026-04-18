# This file was generated with the assistance of an AI coding tool.
import ifcopenshell.api.pset_template
import test.bootstrap


class TestAddPsetTemplate(test.bootstrap.IFC4):
    def test_adding_a_pset_template(self):
        template = ifcopenshell.api.pset_template.add_pset_template(self.file, name="ABC_RiskFactors")
        assert template.is_a("IfcPropertySetTemplate")
        assert template.Name == "ABC_RiskFactors"
        assert template.TemplateType == "PSET_TYPEDRIVENOVERRIDE"


class TestAddPropTemplate(test.bootstrap.IFC4):
    def test_adding_a_prop_template(self):
        template = ifcopenshell.api.pset_template.add_pset_template(self.file, name="ABC_RiskFactors")
        prop = ifcopenshell.api.pset_template.add_prop_template(self.file, pset_template=template)
        assert prop.is_a("IfcSimplePropertyTemplate")
        assert prop.TemplateType == "P_SINGLEVALUE"
        assert prop.PrimaryMeasureType == "IfcLabel"
        assert template.HasPropertyTemplates == (prop,)

    def test_adding_a_qto_prop_template(self):
        template = ifcopenshell.api.pset_template.add_pset_template(
            self.file, name="Qto_Test", template_type="QTO_TYPEDRIVENOVERRIDE")
        prop = ifcopenshell.api.pset_template.add_prop_template(self.file, pset_template=template)
        assert prop.TemplateType == "Q_LENGTH"
        assert prop.PrimaryMeasureType is None


class TestEditPropTemplate(test.bootstrap.IFC4):
    def test_editing_a_simple_template(self):
        template = ifcopenshell.api.pset_template.add_pset_template(self.file, name="ABC_RiskFactors")
        prop = ifcopenshell.api.pset_template.add_prop_template(self.file, pset_template=template)
        ifcopenshell.api.pset_template.edit_prop_template(
            self.file, prop_template=prop, attributes={"Name": "DemoA", "PrimaryMeasureType": "IfcLabel"})
        ifcopenshell.api.pset_template.edit_prop_template(
            self.file, prop_template=prop, attributes={"Name": "DemoB"})
        assert prop.Name == "DemoB"

    def test_editing_an_enumeration(self):
        template = ifcopenshell.api.pset_template.add_pset_template(self.file, name="ABC_RiskFactors")
        prop = ifcopenshell.api.pset_template.add_prop_template(self.file, pset_template=template)
        ifcopenshell.api.pset_template.edit_prop_template(
            self.file, prop_template=prop,
            attributes={"Name": "DemoA", "PrimaryMeasureType": "IfcLabel"})
        ifcopenshell.api.pset_template.edit_prop_template(
            self.file, prop_template=prop,
            attributes={"Enumerators": ["FOO", "BAR"]})
        assert prop.Enumerators.EnumerationValues == tuple(self.file.createIfcLabel(v) for v in ("FOO", "BAR"))
        ifcopenshell.api.pset_template.edit_prop_template(
            self.file, prop_template=prop,
            attributes={"Name": "DemoC", "Enumerators": ["BAZ", "BAR"]})
        assert prop.Enumerators.Name == "DemoC"
        assert prop.Enumerators.EnumerationValues == tuple(self.file.createIfcLabel(v) for v in ("BAZ", "BAR"))
        assert len(self.file.by_type("IfcPropertyEnumeration")) == 1


class TestRemovePropTemplate(test.bootstrap.IFC4):
    def test_removing_a_prop_template(self):
        template = ifcopenshell.api.pset_template.add_pset_template(self.file, name="ABC_RiskFactors")
        prop1 = ifcopenshell.api.pset_template.add_prop_template(self.file, pset_template=template)
        prop2 = ifcopenshell.api.pset_template.add_prop_template(self.file, pset_template=template)
        ifcopenshell.api.pset_template.remove_prop_template(self.file, prop_template=prop2)
        assert len(self.file.by_type("IfcSimplePropertyTemplate")) == 1
        assert template.HasPropertyTemplates == (prop1,)

    def test_not_removing_the_last_prop_template(self):
        template = ifcopenshell.api.pset_template.add_pset_template(self.file, name="ABC_RiskFactors")
        prop = ifcopenshell.api.pset_template.add_prop_template(self.file, pset_template=template)
        ifcopenshell.api.pset_template.remove_prop_template(self.file, prop_template=prop)
        assert len(self.file.by_type("IfcSimplePropertyTemplate")) == 1
        assert template.HasPropertyTemplates == (prop,)


class TestRemovePsetTemplate(test.bootstrap.IFC4):
    def test_removing_a_pset_template(self):
        template = ifcopenshell.api.pset_template.add_pset_template(self.file, name="ABC_RiskFactors")
        ifcopenshell.api.pset_template.remove_pset_template(self.file, pset_template=template)
        assert len(self.file.by_type("IfcPropertySetTemplate")) == 0

    def test_removing_a_pset_template_with_property_templates(self):
        template = ifcopenshell.api.pset_template.add_pset_template(self.file, name="ABC_RiskFactors")
        ifcopenshell.api.pset_template.add_prop_template(self.file, pset_template=template)
        ifcopenshell.api.pset_template.add_prop_template(self.file, pset_template=template)
        ifcopenshell.api.pset_template.remove_pset_template(self.file, pset_template=template)
        assert len(self.file.by_type("IfcPropertySetTemplate")) == 0
        assert len(self.file.by_type("IfcSimplePropertyTemplate")) == 0
