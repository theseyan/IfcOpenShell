# Tests for inverse attribute access, remove_deep2, and relationship patterns
# needed by the high-level APIs (aggregate, spatial, type, etc.).

import os
import sys

import pytest

sys.path.insert(0, os.path.join(os.path.dirname(__file__), "..", "..", "..", "python"))
import ifcopenshell
import ifcopenshell.guid
import ifcopenshell.settings
import ifcopenshell.util.element


class TestInverseAttributes:
    """Test that inverse attributes are accessible on entities."""

    def setup_method(self):
        self.f = ifcopenshell.file(schema="IFC4")
        ifcopenshell.settings.unpack_non_aggregate_inverses = False

    def test_is_decomposed_by(self):
        site = self.f.create_entity("IfcSite")
        building = self.f.create_entity("IfcBuilding")
        rel = self.f.create_entity(
            "IfcRelAggregates",
            GlobalId=ifcopenshell.guid.new(),
            RelatingObject=site,
            RelatedObjects=[building],
        )
        decomps = site.IsDecomposedBy
        assert len(decomps) == 1
        assert decomps[0].id() == rel.id()

    def test_decomposes(self):
        site = self.f.create_entity("IfcSite")
        building = self.f.create_entity("IfcBuilding")
        self.f.create_entity(
            "IfcRelAggregates",
            GlobalId=ifcopenshell.guid.new(),
            RelatingObject=site,
            RelatedObjects=[building],
        )
        decomps = building.Decomposes
        assert len(decomps) == 1
        assert decomps[0].RelatingObject.id() == site.id()

    def test_contained_in_structure(self):
        storey = self.f.create_entity("IfcBuildingStorey")
        wall = self.f.create_entity("IfcWall")
        self.f.create_entity(
            "IfcRelContainedInSpatialStructure",
            GlobalId=ifcopenshell.guid.new(),
            RelatedElements=[wall],
            RelatingStructure=storey,
        )
        containers = wall.ContainedInStructure
        assert len(containers) == 1
        assert containers[0].RelatingStructure.id() == storey.id()

    def test_contains_elements(self):
        storey = self.f.create_entity("IfcBuildingStorey")
        wall = self.f.create_entity("IfcWall")
        self.f.create_entity(
            "IfcRelContainedInSpatialStructure",
            GlobalId=ifcopenshell.guid.new(),
            RelatedElements=[wall],
            RelatingStructure=storey,
        )
        contains = storey.ContainsElements
        assert len(contains) == 1
        assert wall.id() in [e.id() for e in contains[0].RelatedElements]

    def test_inverse_empty(self):
        wall = self.f.create_entity("IfcWall")
        containers = wall.ContainedInStructure
        assert containers == ()

    def test_inverse_attr_not_found(self):
        wall = self.f.create_entity("IfcWall")
        with pytest.raises(AttributeError):
            wall.NonExistentAttr

    def test_has_associations(self):
        wall = self.f.create_entity("IfcWall")
        mat = self.f.create_entity("IfcMaterial", Name="Concrete")
        self.f.create_entity(
            "IfcRelAssociatesMaterial",
            GlobalId=ifcopenshell.guid.new(),
            RelatedObjects=[wall],
            RelatingMaterial=mat,
        )
        assocs = wall.HasAssociations
        assert len(assocs) == 1
        assert assocs[0].RelatingMaterial.id() == mat.id()

    def test_is_typed_by(self):
        wall = self.f.create_entity("IfcWall")
        wall_type = self.f.create_entity("IfcWallType")
        self.f.create_entity(
            "IfcRelDefinesByType",
            GlobalId=ifcopenshell.guid.new(),
            RelatedObjects=[wall],
            RelatingType=wall_type,
        )
        typed = wall.IsTypedBy
        assert len(typed) == 1
        assert typed[0].RelatingType.id() == wall_type.id()

    def test_is_defined_by(self):
        wall = self.f.create_entity("IfcWall")
        pset = self.f.create_entity(
            "IfcPropertySet",
            GlobalId=ifcopenshell.guid.new(),
            Name="Pset_WallCommon",
        )
        self.f.create_entity(
            "IfcRelDefinesByProperties",
            GlobalId=ifcopenshell.guid.new(),
            RelatedObjects=[wall],
            RelatingPropertyDefinition=pset,
        )
        defined = wall.IsDefinedBy
        assert len(defined) == 1

    def test_types_elements(self):
        """TypesElements inverse on IfcTypeObject."""
        wall_type = self.f.create_entity("IfcWallType")
        wall = self.f.create_entity("IfcWall")
        self.f.create_entity(
            "IfcRelDefinesByType",
            GlobalId=ifcopenshell.guid.new(),
            RelatedObjects=[wall],
            RelatingType=wall_type,
        )
        types = wall_type.Types
        assert len(types) == 1

    def test_unpack_non_aggregate_inverses_setting(self):
        wall = self.f.create_entity("IfcWall")
        opening = self.f.create_entity("IfcOpeningElement")
        rel = self.f.create_entity(
            "IfcRelVoidsElement",
            GlobalId=ifcopenshell.guid.new(),
            RelatingBuildingElement=wall,
            RelatedOpeningElement=opening,
        )
        assert isinstance(opening.VoidsElements, tuple)
        ifcopenshell.settings.unpack_non_aggregate_inverses = True
        try:
            assert opening.VoidsElements.id() == rel.id()
        finally:
            ifcopenshell.settings.unpack_non_aggregate_inverses = False


class TestRelationshipAggregate:
    """Test setting/getting aggregate attributes on relationship entities."""

    def setup_method(self):
        self.f = ifcopenshell.file(schema="IFC4")

    def test_set_related_objects(self):
        site = self.f.create_entity("IfcSite")
        b1 = self.f.create_entity("IfcBuilding")
        b2 = self.f.create_entity("IfcBuilding")
        rel = self.f.create_entity(
            "IfcRelAggregates",
            GlobalId=ifcopenshell.guid.new(),
            RelatingObject=site,
            RelatedObjects=[b1],
        )
        rel.RelatedObjects = [b1, b2]
        objs = rel.RelatedObjects
        ids = {o.id() for o in objs}
        assert b1.id() in ids
        assert b2.id() in ids

    def test_related_elements_on_container(self):
        storey = self.f.create_entity("IfcBuildingStorey")
        w1 = self.f.create_entity("IfcWall")
        w2 = self.f.create_entity("IfcWall")
        rel = self.f.create_entity(
            "IfcRelContainedInSpatialStructure",
            GlobalId=ifcopenshell.guid.new(),
            RelatedElements=[w1, w2],
            RelatingStructure=storey,
        )
        elems = rel.RelatedElements
        assert len(elems) == 2


class TestRemoveDeep2:
    """Test the native remove_deep2 utility."""

    def setup_method(self):
        self.f = ifcopenshell.file(schema="IFC4")

    def test_remove_deep2_removes_orphaned(self):
        wall = self.f.create_entity("IfcWall")
        site = self.f.create_entity("IfcSite")
        rel = self.f.create_entity(
            "IfcRelAggregates",
            GlobalId=ifcopenshell.guid.new(),
            RelatingObject=site,
            RelatedObjects=[wall],
        )
        rel_id = rel.id()
        ifcopenshell.util.element.remove_deep2(self.f, rel)
        with pytest.raises(RuntimeError):
            self.f.by_id(rel_id)

    def test_remove_deep2_file_consistency(self):
        """File should remain valid after remove_deep2."""
        wall = self.f.create_entity("IfcWall")
        storey = self.f.create_entity("IfcBuildingStorey")
        rel = self.f.create_entity(
            "IfcRelContainedInSpatialStructure",
            GlobalId=ifcopenshell.guid.new(),
            RelatedElements=[wall],
            RelatingStructure=storey,
        )
        initial_count = len(self.f)
        ifcopenshell.util.element.remove_deep2(self.f, rel)
        assert len(self.f) < initial_count
