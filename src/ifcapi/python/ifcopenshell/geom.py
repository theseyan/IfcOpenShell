# SPDX-License-Identifier: LGPL-3.0-or-later
"""Stub for ifcopenshell.geom – provides a minimal settings class."""


class settings:
    """Minimal geometry settings stub for the native ifcapi backend."""
    use_python_opencascade = False
    COMPUTE_CURVATURE = "COMPUTE_CURVATURE"

    def __init__(self, **kwargs):
        self._settings = {}
        for k, v in kwargs.items():
            setattr(self, k, v)

    def set(self, key, value):
        self._settings[key] = value

    def get(self, key):
        return self._settings.get(key)


def create_shape(settings_obj, entity):
    raise NotImplementedError(
        "ifcopenshell.geom.create_shape is not available in the native ifcapi backend"
    )
