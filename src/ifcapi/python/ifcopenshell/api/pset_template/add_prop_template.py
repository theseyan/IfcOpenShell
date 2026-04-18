# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell
import ifcopenshell.guid
import ifcopenshell.util.pset


def add_prop_template(file, pset_template, name="NewProperty", description=None,
                      template_type=None, primary_measure_type=None):
    assumed_pset_type = ifcopenshell.util.pset.get_pset_template_type(pset_template) or "PSET"

    if template_type is None:
        template_type = "Q_LENGTH" if assumed_pset_type == "QTO" else "P_SINGLEVALUE"

    if assumed_pset_type == "PSET":
        if primary_measure_type is None:
            primary_measure_type = "IfcLabel"
    else:
        primary_measure_type = None

    prop_template = file.create_entity(
        "IfcSimplePropertyTemplate",
        GlobalId=ifcopenshell.guid.new(),
        Name=name,
        Description=description,
        PrimaryMeasureType=primary_measure_type,
        TemplateType=template_type,
        AccessState="READWRITE",
        Enumerators=None,
    )
    has_property_templates = list(pset_template.HasPropertyTemplates or [])
    has_property_templates.append(prop_template)
    has_property_templates.sort(key=lambda pt: pt.Name)
    pset_template.HasPropertyTemplates = has_property_templates
    return prop_template
