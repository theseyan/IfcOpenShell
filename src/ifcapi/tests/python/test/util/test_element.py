# SPDX-License-Identifier: LGPL-3.0-or-later

"""Tests for the native-backed helpers in ``ifcopenshell.util.element``.

These cover the ported C++ functions: ``get_type``, ``get_aggregate``,
``get_nest``, ``get_container``, ``get_decomposition``. Scenarios are
derived from the upstream SWIG test suite plus additional edge cases
to validate IFC2X3-vs-IFC4 schema branching.
"""

import pytest

import ifcopenshell.util.element as subject
import test.bootstrap


def _rel(file_, rel_type, **kwargs):
    return file_.create_entity(rel_type, **kwargs)


# -----------------------------------------------------------------------------
# get_type
# -----------------------------------------------------------------------------


class TestGetTypeIFC4(test.bootstrap.IFC4):
    def test_none_when_no_type(self):
        wall = self.file.createIfcWall()
        assert subject.get_type(wall) is None

    def test_returns_self_for_type_object(self):
        wall_type = self.file.createIfcWallType()
        assert subject.get_type(wall_type) == wall_type

    def test_returns_type_via_is_typed_by(self):
        wall = self.file.createIfcWall()
        wt = self.file.createIfcWallType()
        _rel(
            self.file,
            "IfcRelDefinesByType",
            GlobalId="0" * 22,
            RelatingType=wt,
            RelatedObjects=[wall],
        )
        assert subject.get_type(wall) == wt


class TestGetTypeIFC2X3(test.bootstrap.IFC2X3):
    def test_returns_type_via_is_defined_by(self):
        wall = self.file.createIfcWall()
        wt = self.file.createIfcWallType()
        _rel(
            self.file,
            "IfcRelDefinesByType",
            GlobalId="0" * 22,
            RelatingType=wt,
            RelatedObjects=[wall],
        )
        assert subject.get_type(wall) == wt

    def test_ignores_non_type_relations(self):
        wall = self.file.createIfcWall()
        pset = self.file.createIfcPropertySet(GlobalId="1" * 22, Name="X")
        _rel(
            self.file,
            "IfcRelDefinesByProperties",
            GlobalId="0" * 22,
            RelatingPropertyDefinition=pset,
            RelatedObjects=[wall],
        )
        assert subject.get_type(wall) is None


# -----------------------------------------------------------------------------
# get_aggregate
# -----------------------------------------------------------------------------


class TestGetAggregateIFC4(test.bootstrap.IFC4):
    def test_none_when_not_aggregated(self):
        wall = self.file.createIfcWall()
        assert subject.get_aggregate(wall) is None

    def test_returns_aggregate_parent(self):
        building = self.file.createIfcBuilding()
        storey = self.file.createIfcBuildingStorey()
        _rel(
            self.file,
            "IfcRelAggregates",
            GlobalId="0" * 22,
            RelatingObject=building,
            RelatedObjects=[storey],
        )
        assert subject.get_aggregate(storey) == building


class TestGetAggregateIFC2X3(test.bootstrap.IFC2X3, TestGetAggregateIFC4):
    pass


# -----------------------------------------------------------------------------
# get_nest
# -----------------------------------------------------------------------------


class TestGetNestIFC4(test.bootstrap.IFC4):
    def test_none_when_not_nested(self):
        task = self.file.createIfcTask()
        assert subject.get_nest(task) is None

    def test_returns_nest_host(self):
        outer = self.file.createIfcTask()
        inner = self.file.createIfcTask()
        _rel(
            self.file,
            "IfcRelNests",
            GlobalId="0" * 22,
            RelatingObject=outer,
            RelatedObjects=[inner],
        )
        assert subject.get_nest(inner) == outer


class TestGetNestIFC2X3(test.bootstrap.IFC2X3):
    def test_none_when_not_nested(self):
        task = self.file.createIfcTask()
        assert subject.get_nest(task) is None

    def test_returns_nest_host_via_decomposes(self):
        outer = self.file.createIfcTask()
        inner = self.file.createIfcTask()
        _rel(
            self.file,
            "IfcRelNests",
            GlobalId="0" * 22,
            RelatingObject=outer,
            RelatedObjects=[inner],
        )
        assert subject.get_nest(inner) == outer

    def test_ignores_aggregates(self):
        b = self.file.createIfcBuilding()
        s = self.file.createIfcBuildingStorey()
        _rel(
            self.file,
            "IfcRelAggregates",
            GlobalId="0" * 22,
            RelatingObject=b,
            RelatedObjects=[s],
        )
        assert subject.get_nest(s) is None


# -----------------------------------------------------------------------------
# get_container
# -----------------------------------------------------------------------------


class TestGetContainerIFC4(test.bootstrap.IFC4):
    def test_none_when_uncontained(self):
        wall = self.file.createIfcWall()
        assert subject.get_container(wall) is None

    def test_returns_direct_container(self):
        storey = self.file.createIfcBuildingStorey()
        wall = self.file.createIfcWall()
        _rel(
            self.file,
            "IfcRelContainedInSpatialStructure",
            GlobalId="0" * 22,
            RelatingStructure=storey,
            RelatedElements=[wall],
        )
        assert subject.get_container(wall) == storey
        assert subject.get_container(wall, should_get_direct=True) == storey

    def test_walks_aggregation_to_find_class(self):
        building = self.file.createIfcBuilding()
        storey = self.file.createIfcBuildingStorey()
        wall = self.file.createIfcWall()
        _rel(
            self.file,
            "IfcRelAggregates",
            GlobalId="0" * 22,
            RelatingObject=building,
            RelatedObjects=[storey],
        )
        _rel(
            self.file,
            "IfcRelContainedInSpatialStructure",
            GlobalId="1" * 22,
            RelatingStructure=storey,
            RelatedElements=[wall],
        )
        assert subject.get_container(wall, ifc_class="IfcBuilding") == building
        assert subject.get_container(wall, should_get_direct=True, ifc_class="IfcBuilding") is None

    def test_container_via_parent_aggregate(self):
        building = self.file.createIfcBuilding()
        assembly = self.file.createIfcElementAssembly()
        wall = self.file.createIfcWall()
        _rel(
            self.file,
            "IfcRelContainedInSpatialStructure",
            GlobalId="0" * 22,
            RelatingStructure=building,
            RelatedElements=[assembly],
        )
        _rel(
            self.file,
            "IfcRelAggregates",
            GlobalId="1" * 22,
            RelatingObject=assembly,
            RelatedObjects=[wall],
        )
        assert subject.get_container(wall) == building


# -----------------------------------------------------------------------------
# get_decomposition
# -----------------------------------------------------------------------------


class TestGetDecompositionIFC4(test.bootstrap.IFC4):
    def test_empty_for_leaf_element(self):
        wall = self.file.createIfcWall()
        assert subject.get_decomposition(wall) == set()

    def test_contains_elements(self):
        storey = self.file.createIfcBuildingStorey()
        wall = self.file.createIfcWall()
        _rel(
            self.file,
            "IfcRelContainedInSpatialStructure",
            GlobalId="0" * 22,
            RelatingStructure=storey,
            RelatedElements=[wall],
        )
        assert subject.get_decomposition(storey) == {wall}

    def test_recursive_decomposition(self):
        building = self.file.createIfcBuilding()
        storey = self.file.createIfcBuildingStorey()
        wall = self.file.createIfcWall()
        _rel(
            self.file,
            "IfcRelAggregates",
            GlobalId="0" * 22,
            RelatingObject=building,
            RelatedObjects=[storey],
        )
        _rel(
            self.file,
            "IfcRelContainedInSpatialStructure",
            GlobalId="1" * 22,
            RelatingStructure=storey,
            RelatedElements=[wall],
        )
        assert subject.get_decomposition(building) == {storey, wall}
        # Non-recursive: only the direct decomposition members (spec: recursive
        # flag only gates IsNestedBy per upstream behaviour)
        assert storey in subject.get_decomposition(building, is_recursive=False)

    def test_includes_openings(self):
        wall = self.file.createIfcWall()
        opening = self.file.createIfcOpeningElement()
        _rel(
            self.file,
            "IfcRelVoidsElement",
            GlobalId="0" * 22,
            RelatingBuildingElement=wall,
            RelatedOpeningElement=opening,
        )
        assert opening in subject.get_decomposition(wall)

    def test_is_nested_gated_by_recursive(self):
        # IsNestedBy traversal only runs when is_recursive is True; this matches
        # the upstream behaviour (see ifcopenshell.util.element.get_decomposition).
        outer = self.file.createIfcTask()
        inner = self.file.createIfcTask()
        _rel(
            self.file,
            "IfcRelNests",
            GlobalId="0" * 22,
            RelatingObject=outer,
            RelatedObjects=[inner],
        )
        assert inner in subject.get_decomposition(outer, is_recursive=True)
        assert inner not in subject.get_decomposition(outer, is_recursive=False)


# -----------------------------------------------------------------------------
# get_psets (native traversal + Python value marshalling)
# -----------------------------------------------------------------------------


def _pset(file_, name, props, guid):
    ps = file_.createIfcPropertySet(
        GlobalId=guid, Name=name, HasProperties=props
    )
    return ps


def _single(file_, name, value_type, value):
    return file_.createIfcPropertySingleValue(
        Name=name, NominalValue=file_.create_entity(value_type, value)
    )


class TestGetPsetsOnObjectIFC4(test.bootstrap.IFC4):
    def test_empty_when_no_relations(self):
        wall = self.file.createIfcWall()
        assert subject.get_psets(wall) == {}

    def test_direct_pset(self):
        wall = self.file.createIfcWall()
        p = _single(self.file, "Foo", "IfcLabel", "Bar")
        ps = _pset(self.file, "MyPset", [p], "0" * 22)
        _rel(
            self.file,
            "IfcRelDefinesByProperties",
            GlobalId="1" * 22,
            RelatingPropertyDefinition=ps,
            RelatedObjects=[wall],
        )
        psets = subject.get_psets(wall)
        assert "MyPset" in psets
        assert psets["MyPset"]["Foo"] == "Bar"
        assert psets["MyPset"]["id"] == ps.id()

    def test_psets_only_filter(self):
        wall = self.file.createIfcWall()
        p = _single(self.file, "Foo", "IfcLabel", "Bar")
        ps = _pset(self.file, "MyPset", [p], "0" * 22)
        qto = self.file.createIfcElementQuantity(
            GlobalId="2" * 22, Name="MyQto", Quantities=[]
        )
        _rel(self.file, "IfcRelDefinesByProperties", GlobalId="1" * 22,
             RelatingPropertyDefinition=ps, RelatedObjects=[wall])
        _rel(self.file, "IfcRelDefinesByProperties", GlobalId="3" * 22,
             RelatingPropertyDefinition=qto, RelatedObjects=[wall])
        psets = subject.get_psets(wall, psets_only=True)
        assert set(psets.keys()) == {"MyPset"}
        qtos = subject.get_psets(wall, qtos_only=True)
        assert set(qtos.keys()) == {"MyQto"}

    def test_inheritance_from_type(self):
        wall = self.file.createIfcWall()
        wt = self.file.createIfcWallType(HasPropertySets=[
            _pset(self.file, "Inherited",
                  [_single(self.file, "From", "IfcLabel", "Type")],
                  "A" * 22),
        ])
        _rel(self.file, "IfcRelDefinesByType", GlobalId="0" * 22,
             RelatingType=wt, RelatedObjects=[wall])
        psets = subject.get_psets(wall)
        assert psets["Inherited"]["From"] == "Type"
        # Without should_inherit, only own psets are returned.
        assert subject.get_psets(wall, should_inherit=False) == {}

    def test_own_overrides_inherited(self):
        wall = self.file.createIfcWall()
        wt = self.file.createIfcWallType(HasPropertySets=[
            _pset(self.file, "Shared",
                  [_single(self.file, "From", "IfcLabel", "Type")],
                  "A" * 22),
        ])
        _rel(self.file, "IfcRelDefinesByType", GlobalId="0" * 22,
             RelatingType=wt, RelatedObjects=[wall])
        own = _pset(self.file, "Shared",
                    [_single(self.file, "From", "IfcLabel", "Own")],
                    "B" * 22)
        _rel(self.file, "IfcRelDefinesByProperties", GlobalId="1" * 22,
             RelatingPropertyDefinition=own, RelatedObjects=[wall])
        psets = subject.get_psets(wall)
        assert psets["Shared"]["From"] == "Own"


class TestGetPsetsOnTypeObjectIFC4(test.bootstrap.IFC4):
    def test_direct_type_pset(self):
        wt = self.file.createIfcWallType(HasPropertySets=[
            _pset(self.file, "TypePset",
                  [_single(self.file, "K", "IfcLabel", "V")],
                  "0" * 22),
        ])
        psets = subject.get_psets(wt)
        assert psets["TypePset"]["K"] == "V"


# -----------------------------------------------------------------------------
# get_material
# -----------------------------------------------------------------------------


class TestGetMaterialIFC4(test.bootstrap.IFC4):
    def test_none_without_association(self):
        wall = self.file.createIfcWall()
        assert subject.get_material(wall) is None

    def test_direct_material(self):
        wall = self.file.createIfcWall()
        mat = self.file.createIfcMaterial(Name="Concrete")
        _rel(
            self.file,
            "IfcRelAssociatesMaterial",
            GlobalId="0" * 22,
            RelatingMaterial=mat,
            RelatedObjects=[wall],
        )
        assert subject.get_material(wall) == mat

    def test_layer_set_usage_not_unwrapped_by_default(self):
        wall = self.file.createIfcWall()
        ls = self.file.createIfcMaterialLayerSet(MaterialLayers=[])
        usage = self.file.createIfcMaterialLayerSetUsage(
            ForLayerSet=ls, LayerSetDirection="AXIS2",
            DirectionSense="POSITIVE", OffsetFromReferenceLine=0.0,
        )
        _rel(
            self.file,
            "IfcRelAssociatesMaterial",
            GlobalId="0" * 22,
            RelatingMaterial=usage,
            RelatedObjects=[wall],
        )
        assert subject.get_material(wall) == usage
        assert subject.get_material(wall, should_skip_usage=True) == ls

    def test_inherits_from_type(self):
        wall = self.file.createIfcWall()
        wt = self.file.createIfcWallType()
        _rel(
            self.file,
            "IfcRelDefinesByType",
            GlobalId="0" * 22,
            RelatingType=wt,
            RelatedObjects=[wall],
        )
        mat = self.file.createIfcMaterial(Name="Inherited")
        _rel(
            self.file,
            "IfcRelAssociatesMaterial",
            GlobalId="1" * 22,
            RelatingMaterial=mat,
            RelatedObjects=[wt],
        )
        assert subject.get_material(wall) == mat
        assert subject.get_material(wall, should_inherit=False) is None
