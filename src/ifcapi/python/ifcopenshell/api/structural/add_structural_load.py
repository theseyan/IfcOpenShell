# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later


def add_structural_load(file, name=None, ifc_class="IfcStructuralLoadLinearForce"):
    return file.create_entity(ifc_class, Name=name)
