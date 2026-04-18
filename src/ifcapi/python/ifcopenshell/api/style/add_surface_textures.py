# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later


def add_surface_textures(file, material=None, textures=None, uv_maps=None):
    """Add surface textures from texture data dicts.

    The Blender ``material`` path is not supported in this port; only the
    ``textures`` list-of-dicts path is implemented.
    """
    textures = textures or []
    uv_maps = uv_maps or []

    if file.schema == "IFC2X3":
        return []

    results = []
    for tex_data in textures:
        uv_mode = tex_data.get("uv_mode", None)
        create_kwargs = {k: v for k, v in tex_data.items() if k != "uv_mode"}
        texture = file.create_entity("IfcImageTexture", **create_kwargs)

        if uv_mode == "Generated":
            file.create_entity("IfcTextureCoordinateGenerator", Maps=[texture], Mode="COORD")
        elif uv_mode == "Camera":
            file.create_entity("IfcTextureCoordinateGenerator", Maps=[texture], Mode="COORD-EYE")
        elif uv_mode == "UV":
            _apply_uv_map_to_texture(texture, uv_maps)

        results.append(texture)

    return results


def _apply_uv_map_to_texture(texture, uv_maps):
    for uv_map in uv_maps:
        maps = set(uv_map.Maps or [])
        maps.add(texture)
        uv_map.Maps = list(maps)
