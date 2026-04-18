# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell.api.group


def assign_structural_analysis_model(file, products=None, structural_analysis_model=None):
    return ifcopenshell.api.group.assign_group(file, products, structural_analysis_model)
