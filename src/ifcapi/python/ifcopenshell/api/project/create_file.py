# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

"""Ported from ifcopenshell.api.project.create_file."""

import datetime

import ifcopenshell


def create_file(version="IFC4") -> ifcopenshell.file:
    """Create a blank IFC model file object."""
    f = ifcopenshell.file(schema=version)
    f.header.file_name.name = "/dev/null"
    f.header.file_name.time_stamp = datetime.datetime.now().astimezone().replace(microsecond=0).isoformat()
    f.header.file_name.preprocessor_version = f"IfcOpenShell {ifcopenshell.version}"
    f.header.file_name.originating_system = f"IfcOpenShell {ifcopenshell.version}"
    f.header.file_name.authorization = "Nobody"
    f.header.file_description.description = ("ViewDefinition[DesignTransferView]",)
    return f
