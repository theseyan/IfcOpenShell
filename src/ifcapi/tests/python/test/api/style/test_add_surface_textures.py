# IfcOpenShell - IFC toolkit and geometry engine
# Copyright (C) 2022 Dion Moult <dion@thinkmoult.com>
#
# This file is part of IfcOpenShell.
#
# IfcOpenShell is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# IfcOpenShell is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with IfcOpenShell.  If not, see <http://www.gnu.org/licenses/>.


import ifcopenshell.api.style
import pytest

import test.bootstrap


# TODO: add ifc2x3 tests after add_surface_textures will support ifc2x3
class TestAddSurfaceTexture(test.bootstrap.IFC4):
    def get_default_texture_data(self):
        return [
            {
                "Mode": "DIFFUSE",
                "RepeatS": True,
                "RepeatT": True,
                "URLReference": "diffuse.jpg",
            },
            {
                "Mode": "NORMAL",
                "RepeatS": False,
                "RepeatT": False,
                "URLReference": "normal.jpg",
            },
            {
                "Mode": "METALLICROUGHNESS",
                "RepeatS": True,
                "RepeatT": True,
                "URLReference": "metallic_roughness.jpg",
            },
            {
                "Mode": "OCCLUSION",
                "RepeatS": True,
                "RepeatT": True,
                "URLReference": "ambient_occlusion.jpg",
            },
        ]

    def compare_texture_to_data(self, texture, data, uv_maps=[]):
        texture_data = texture.get_info()
        for attribute in ("Mode", "RepeatS", "RepeatT", "URLReference"):
            assert texture_data[attribute] == data.get(attribute, None)

        uv_mode = data.get("uv_mode", None)
        if uv_mode is None:
            assert texture.IsMappedBy == ()
        elif uv_mode == "Generated":
            assert len(texture.IsMappedBy) == 1
            assert texture.IsMappedBy[0].Mode == "COORD"
        elif uv_mode == "Camera":
            assert len(texture.IsMappedBy) == 1
            assert texture.IsMappedBy[0].Mode == "COORD-EYE"
        elif uv_mode == "UV":
            assert set(texture.IsMappedBy) == set(uv_maps)

    def test_add_surface_textures_from_data(self):
        texture_data = self.get_default_texture_data()

        textures = ifcopenshell.api.style.add_surface_textures(
            self.file, textures=texture_data
        )
        assert len(list(self.file)) == len(texture_data)

        for texture, data in zip(textures, texture_data):
            self.compare_texture_to_data(texture, data)

    def test_add_surface_textures_from_data_with_uv_mode(self):
        texture_data = self.get_default_texture_data()
        texture_data[0]["uv_mode"] = "Generated"
        texture_data[1]["uv_mode"] = "Camera"
        texture_data[2]["uv_mode"] = "UV"
        texture_data[3]["uv_mode"] = None

        textures = ifcopenshell.api.style.add_surface_textures(
            self.file, textures=texture_data
        )
        for texture, data in zip(textures, texture_data):
            self.compare_texture_to_data(texture, data)

    # NOTE: IfcTextureCoordinate doesn't have Maps in IFC2X3
    def test_add_surface_textures_from_data_with_uv_maps(self):
        texture_data = self.get_default_texture_data()
        texture_data[0]["uv_mode"] = "Generated"
        texture_data[1]["uv_mode"] = "Camera"
        texture_data[2]["uv_mode"] = "UV"
        texture_data[3]["uv_mode"] = None

        uv_maps = [
            self.file.create_entity(
                "IfcTextureCoordinateGenerator", Maps=[], Mode="COORD"
            )
            for i in range(5)
        ]
        textures = ifcopenshell.api.style.add_surface_textures(
            self.file, textures=texture_data, uv_maps=uv_maps
        )
        for texture, data in zip(textures, texture_data):
            self.compare_texture_to_data(texture, data, uv_maps)

    def test_preserves_optional_image_texture_fields_and_input_data(self):
        transform = self.file.create_entity(
            "IfcCartesianTransformationOperator2D", Scale=2.0
        )
        texture_data = [
            {
                "RepeatS": False,
                "RepeatT": True,
                "URLReference": "texture.png",
                "TextureTransform": transform,
                "Parameter": ["A", "B"],
            }
        ]
        original = texture_data[0].copy()
        textures = ifcopenshell.api.style.add_surface_textures(
            self.file, textures=texture_data
        )
        assert texture_data[0] == original
        assert textures[0].Mode is None
        assert textures[0].TextureTransform == transform
        assert textures[0].Parameter == ("A", "B")
        assert textures[0].IsMappedBy == ()

    def test_rejects_unknown_uv_mode_without_mutation(self):
        before = len(list(self.file))
        with pytest.raises(ValueError, match="Unsupported StyleUvMode value"):
            ifcopenshell.api.style.add_surface_textures(
                self.file,
                textures=[
                    {
                        "RepeatS": True,
                        "RepeatT": True,
                        "URLReference": "texture.png",
                        "uv_mode": "unknown",
                    }
                ],
            )
        assert len(list(self.file)) == before

    def test_multiple_uv_textures_preserve_existing_maps_without_duplicates(self):
        old = self.file.create_entity(
            "IfcImageTexture", RepeatS=True, RepeatT=True, URLReference="old.png"
        )
        uv_maps = [
            self.file.create_entity(
                "IfcTextureCoordinateGenerator", Maps=[old], Mode="COORD"
            )
            for _ in range(2)
        ]
        data = [
            {
                "RepeatS": True,
                "RepeatT": True,
                "URLReference": f"{i}.png",
                "uv_mode": "UV",
            }
            for i in range(2)
        ]
        textures = ifcopenshell.api.style.add_surface_textures(
            self.file, textures=data, uv_maps=uv_maps + uv_maps
        )
        assert [texture.URLReference for texture in textures] == ["0.png", "1.png"]
        for uv_map in uv_maps:
            assert uv_map.Maps == (old, *textures)

    def test_omitted_inputs_are_empty(self):
        assert ifcopenshell.api.style.add_surface_textures(self.file) == []


class TestAddSurfaceTextureIFC2X3(test.bootstrap.IFC2X3):
    def test_returns_immediately_without_mutation(self):
        before = len(list(self.file))
        descriptors = [
            {"RepeatS": True, "RepeatT": True, "URLReference": "ignored.png"}
        ]
        assert (
            ifcopenshell.api.style.add_surface_textures(self.file, textures=descriptors)
            == []
        )
        assert len(list(self.file)) == before

    def test_rejects_unknown_uv_mode_without_mutation(self):
        before = len(list(self.file))
        with pytest.raises(ValueError):
            ifcopenshell.api.style.add_surface_textures(
                self.file,
                textures=[
                    {
                        "RepeatS": True,
                        "RepeatT": True,
                        "URLReference": "ignored.png",
                        "uv_mode": "unknown",
                    }
                ],
            )
        assert len(list(self.file)) == before
