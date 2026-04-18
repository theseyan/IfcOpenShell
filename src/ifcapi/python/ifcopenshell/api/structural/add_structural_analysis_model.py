# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell.api.root


def add_structural_analysis_model(file):
    return ifcopenshell.api.root.create_entity(
        file, ifc_class="IfcStructuralAnalysisModel", predefined_type="LOADING_3D"
    )
