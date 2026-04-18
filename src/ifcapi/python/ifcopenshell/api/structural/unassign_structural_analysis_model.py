# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell.api.group


def unassign_structural_analysis_model(file, products=None, structural_analysis_model=None):
    ifcopenshell.api.group.unassign_group(file, products, structural_analysis_model)
