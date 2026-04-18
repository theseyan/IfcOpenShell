# SPDX-License-Identifier: LGPL-3.0-or-later

"""Tests for ifcopenshell_wrapper schema introspection (native ifcapi)
and util.attribute on top of it."""

import pytest

import ifcopenshell.ifcopenshell_wrapper as W
import ifcopenshell.util.attribute as subject


class TestSchemaByName:
    def test_loads_ifc4(self):
        s = W.schema_by_name("IFC4")
        assert s.name() == "IFC4"

    def test_loads_ifc2x3(self):
        s = W.schema_by_name("IFC2X3")
        assert s.name() == "IFC2X3"

    def test_missing_schema_raises(self):
        with pytest.raises(Exception):
            W.schema_by_name("BOGUS")


class TestDeclarations:
    def test_entity(self):
        s = W.schema_by_name("IFC4")
        d = s.declaration_by_name("IfcWall")
        assert d.name() == "IfcWall"
        e = d.as_entity()
        assert e is not None
        assert e.supertype().name() == "IfcBuildingElement"
        assert e.attribute_count() == 9
        assert d.as_enumeration_type() is None
        assert d.as_select_type() is None
        assert d.as_type_declaration() is None

    def test_enumeration(self):
        s = W.schema_by_name("IFC4")
        d = s.declaration_by_name("IfcWallTypeEnum")
        en = d.as_enumeration_type()
        assert en is not None
        items = en.enumeration_items()
        assert "STANDARD" in items
        assert "NOTDEFINED" in items
        assert d.as_entity() is None

    def test_select(self):
        s = W.schema_by_name("IFC4")
        d = s.declaration_by_name("IfcValue")
        se = d.as_select_type()
        assert se is not None
        assert len(se.select_list()) >= 2
        assert d.as_entity() is None

    def test_type_declaration(self):
        s = W.schema_by_name("IFC4")
        d = s.declaration_by_name("IfcLabel")
        td = d.as_type_declaration()
        assert td is not None
        inner = td.declared_type()
        assert inner is not None
        # <string> underlying
        assert inner.as_simple_type() is not None
        assert inner.as_simple_type().declared_type() == "string"


class TestAttributes:
    def test_wall_attributes(self):
        s = W.schema_by_name("IFC4")
        wall = s.declaration_by_name("IfcWall").as_entity()
        attrs = wall.all_attributes()
        names = [a.name() for a in attrs]
        assert names == [
            "GlobalId",
            "OwnerHistory",
            "Name",
            "Description",
            "ObjectType",
            "ObjectPlacement",
            "Representation",
            "Tag",
            "PredefinedType",
        ]
        # OwnerHistory optional, GlobalId not
        assert attrs[0].optional() is False
        assert attrs[1].optional() is True

    def test_inverse_attributes(self):
        s = W.schema_by_name("IFC4")
        wall = s.declaration_by_name("IfcWall").as_entity()
        inv = wall.all_inverse_attributes()
        names = [a.name() for a in inv]
        assert "IsDefinedBy" in names
        # Verify at least one inverse has a valid entity/attribute reference
        for i in inv:
            if i.name() == "IsDefinedBy":
                assert i.entity_reference() is not None
                assert i.attribute_reference() is not None
                assert i.attribute_reference().name() == "RelatedObjects"
                break

    def test_attribute_index(self):
        s = W.schema_by_name("IFC4")
        wall = s.declaration_by_name("IfcWall").as_entity()
        assert wall.attribute_index("GlobalId") == 0
        assert wall.attribute_index("PredefinedType") == 8


class TestParameterType:
    def test_simple(self):
        s = W.schema_by_name("IFC4")
        wall = s.declaration_by_name("IfcWall").as_entity()
        ga = wall.all_attributes()[0]  # GlobalId -> IfcGloballyUniqueId -> string
        assert repr(ga.type_of_attribute()) == "<type IfcGloballyUniqueId: <string>>"

    def test_aggregation(self):
        s = W.schema_by_name("IFC4")
        # IfcPolyline.Points is LIST [2:?] OF IfcCartesianPoint
        d = s.declaration_by_name("IfcPolyline").as_entity()
        idx = d.attribute_index("Points")
        attr = d.attribute_by_index(idx)
        pt = attr.type_of_attribute()
        agg = pt.as_aggregation_type()
        assert agg is not None
        assert agg.type_of_aggregation_string() == "list"
        assert agg.bound1() == 2
        assert agg.bound2() == -1  # unbounded


class TestGetPrimitiveType:
    def _wall_attrs(self):
        s = W.schema_by_name("IFC4")
        wall = s.declaration_by_name("IfcWall").as_entity()
        return {a.name(): a for a in wall.all_attributes()}

    def test_string(self):
        a = self._wall_attrs()["GlobalId"]
        assert subject.get_primitive_type(a) == "string"

    def test_entity(self):
        a = self._wall_attrs()["OwnerHistory"]
        assert subject.get_primitive_type(a) == "entity"

    def test_enum(self):
        a = self._wall_attrs()["PredefinedType"]
        assert subject.get_primitive_type(a) == "enum"

    def test_list_of_entity(self):
        s = W.schema_by_name("IFC4")
        d = s.declaration_by_name("IfcPolyline").as_entity()
        a = d.attribute_by_index(d.attribute_index("Points"))
        t = subject.get_primitive_type(a)
        assert t == ("list", "entity")


class TestGetEnumItems:
    def test_wall_predefined_type(self):
        s = W.schema_by_name("IFC4")
        wall = s.declaration_by_name("IfcWall").as_entity()
        a = wall.attribute_by_index(wall.attribute_index("PredefinedType"))
        items = subject.get_enum_items(a)
        assert "STANDARD" in items
        assert "NOTDEFINED" in items


class TestDeclarationIs:
    def test_is_self(self):
        s = W.schema_by_name("IFC4")
        d = s.declaration_by_name("IfcWall")
        # is_ form (safe Python name)
        assert d.is_("IfcWall")
        assert d.is_("IfcBuildingElement")
        assert not d.is_("IfcDoor")
