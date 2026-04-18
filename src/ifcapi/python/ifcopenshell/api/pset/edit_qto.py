# SPDX-License-Identifier: LGPL-3.0-or-later

from typing_extensions import assert_never

import ifcopenshell
import ifcopenshell.api.pset
import ifcopenshell.util.pset

FLOAT_TYPE_KEYWORDS = (
    ("Area", ("area",)),
    ("Volume", ("volume",)),
    ("Weight", ("weight", "mass")),
    ("Length", ("length", "width", "height", "depth", "distance")),
    ("Time", ("time", "duration")),
)


def edit_qto(file, qto, name=None, properties=None, pset_template=None):
    properties = properties or {}

    qto_idx = 5
    if qto.is_a("IfcPhysicalComplexQuantity"):
        qto_idx = 2

    if name:
        qto.Name = name

    if pset_template:
        qto_template = pset_template
    else:
        psetqto = ifcopenshell.util.pset.get_template(file.schema_identifier)
        qto_template = psetqto.get_by_name(qto.Name)

    for prop in qto[qto_idx] or []:
        _update_existing_property(file, prop, properties, qto_template)

    new_properties = _add_new_properties(file, properties, qto_template)

    props = list(qto[qto_idx]) if qto[qto_idx] else []
    props.extend(new_properties)
    qto[qto_idx] = props


def _update_existing_property(file, prop, properties, qto_template):
    if prop.Name not in properties:
        return
    value = properties[prop.Name]
    name = prop.Name
    if value is None:
        file.remove(prop)
    elif prop.is_a("IfcPhysicalComplexQuantity") and isinstance(value, dict):
        prop.Discrimination = value.get("Discrimination", prop.Discrimination)
        ifcopenshell.api.pset.edit_qto(file, qto=prop, properties=value["HasQuantities"])
    elif prop.is_a("IfcPhysicalSimpleQuantity"):
        value = value.wrappedValue if isinstance(value, ifcopenshell.entity_instance) else value
        if file.schema == "IFC4X3" and prop.is_a("IfcQuantityCount"):
            prop[3] = int(value)
        else:
            prop[3] = float(value)
    del properties[name]


def _add_new_properties(file, properties, qto_template):
    result = []
    for name, value in properties.items():
        if value is None:
            continue
        if isinstance(value, dict):
            complex_qto = file.create_entity(
                "IfcPhysicalComplexQuantity", Name=name, Discrimination=value["Discrimination"]
            )
            result.append(complex_qto)
            ifcopenshell.api.pset.edit_qto(file, qto=complex_qto, properties=value["HasQuantities"])
        else:
            property_type = _get_canonical_property_type(name, value, qto_template)
            value = value.wrappedValue if isinstance(value, ifcopenshell.entity_instance) else value
            result.append(
                file.create_entity(
                    "IfcQuantity{}".format(property_type),
                    **{"Name": name, "{}Value".format(property_type): value},
                )
            )
    return result


def _get_canonical_property_type(name, value, qto_template):
    if isinstance(value, ifcopenshell.entity_instance):
        result = value.is_a().replace("Ifc", "").replace("Measure", "")
        if result == "Numeric":
            result = "Number"
        elif result == "Mass":
            result = "Weight"
        return result
    if qto_template:
        for prop_template in qto_template.HasPropertyTemplates:
            if prop_template.Name != name:
                continue
            return prop_template.TemplateType[2:].lower().capitalize()
    return infer_property_type(name, value)


def infer_property_type(name, value):
    name_lower = name.lower()
    if isinstance(value, float):
        for category, keywords in FLOAT_TYPE_KEYWORDS:
            if any(keyword in name_lower for keyword in keywords):
                return category
        return "Length"
    elif isinstance(value, int):
        return "Count"
    else:
        assert_never(value)
