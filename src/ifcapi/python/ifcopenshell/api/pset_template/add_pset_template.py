# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell
import ifcopenshell.guid


def add_pset_template(file, name="New_Pset", template_type="PSET_TYPEDRIVENOVERRIDE",
                      applicable_entity="IfcObject,IfcTypeObject"):
    return file.create_entity(
        "IfcPropertySetTemplate",
        GlobalId=ifcopenshell.guid.new(),
        Name=name,
        TemplateType=template_type,
        ApplicableEntity=applicable_entity,
    )
