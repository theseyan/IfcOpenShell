# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later


def add_constituent(file, constituent_set=None, material=None, name=None):
    constituents = list(constituent_set.MaterialConstituents or [])
    constituent = file.create_entity("IfcMaterialConstituent", Material=material, Name=name)
    constituents.append(constituent)
    constituent_set.MaterialConstituents = constituents
    return constituent
