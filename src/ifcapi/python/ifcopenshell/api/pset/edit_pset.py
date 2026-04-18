# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

"""Edit a property set: rename, add/edit/delete properties."""

import datetime

import ifcopenshell


def edit_pset(file, pset, name=None, properties=None, pset_template=None, should_purge=True):
    """Edit the name and/or properties of *pset*.

    *properties* is a dict mapping property names to values. A ``None`` value
    deletes the property when *should_purge* is True (the default).
    """
    properties = dict(properties) if properties else {}

    if name:
        pset.Name = name

    # Autodiscover the pset template when one wasn't supplied so that
    # aggregate / enumerated property values can be encoded correctly.
    if pset_template is None and getattr(pset, "Name", None):
        try:
            import ifcopenshell.util.pset as _util_pset

            psetqto = _util_pset.get_template(file.schema_identifier)
            pset_template = psetqto.get_by_name(pset.Name)
        except Exception:
            pset_template = None

    # --- helpers -----------------------------------------------------------

    def _get_properties():
        if hasattr(pset, "HasProperties"):
            return list(pset.HasProperties or ())
        if hasattr(pset, "Properties"):
            return list(pset.Properties or ())
        if hasattr(pset, "ExtendedProperties"):
            return list(pset.ExtendedProperties or ())
        raise TypeError(f"'{pset}' is not a valid pset")

    def _assign_properties(props):
        if hasattr(pset, "HasProperties"):
            pset.HasProperties = props
        elif hasattr(pset, "Properties"):
            pset.Properties = props
        elif pset.is_a("IfcMaterialProperties"):
            pset.ExtendedProperties = props

    def _primary_measure_type(prop_name, old_value=None, new_value=None):
        if old_value:
            return old_value.is_a()
        if pset_template:
            for pt in getattr(pset_template, "HasPropertyTemplates", ()) or ():
                if pt.Name == prop_name:
                    return getattr(pt, "PrimaryMeasureType", None) or "IfcLabel"
        if isinstance(new_value, ifcopenshell.entity_instance):
            return new_value.is_a()
        if isinstance(new_value, _typed_value):
            return new_value.is_a()
        if new_value is not None:
            if isinstance(new_value, str):
                return "IfcLabel"
            if isinstance(new_value, float):
                return "IfcReal"
            if isinstance(new_value, bool):
                return "IfcBoolean"
            if isinstance(new_value, int):
                return "IfcInteger"
            if isinstance(new_value, datetime.datetime):
                return "IfcDateTime"
            if isinstance(new_value, datetime.date):
                return "IfcDate"
        return None

    def _cast_value(value, primary_measure_type):
        pmt_lower = primary_measure_type.lower()
        if "integer" in pmt_lower or "count" in pmt_lower:
            type_fn = int
        elif "boolean" in pmt_lower:
            type_fn = bool
        elif "logical" in pmt_lower:
            type_fn = str
        elif "label" in pmt_lower or "text" in pmt_lower or "identifier" in pmt_lower or "uri" in pmt_lower or "name" in pmt_lower or "date" in pmt_lower or "time" in pmt_lower or "duration" in pmt_lower:
            type_fn = str
        else:
            # Most IFC measure types are real-valued
            type_fn = float
        if isinstance(value, (datetime.date, datetime.datetime)):
            return value.isoformat()
        return type_fn(value)

    # ---- update existing properties ----------------------------------------

    existing_props = []
    for prop in _get_properties():
        if prop.Name not in properties:
            existing_props.append(prop)
            continue

        if file.get_total_inverses(prop) > 1:
            continue  # shared property — treat as new to avoid side effects

        value = properties[prop.Name]
        if prop.is_a("IfcPropertySingleValue"):
            if value is None:
                if should_purge:
                    del properties[prop.Name]
                    file.remove(prop)
                    continue
                prop.NominalValue = None
            elif isinstance(value, (ifcopenshell.entity_instance, _typed_value)):
                prop.NominalValue = value
            else:
                pmt = _primary_measure_type(prop.Name, old_value=prop.NominalValue, new_value=value)
                value = _cast_value(value, pmt)
                prop.NominalValue = file.create_entity(pmt, value)
            del properties[prop.Name]
            existing_props.append(prop)

        elif prop.is_a("IfcPropertyEnumeratedValue"):
            if isinstance(value, (tuple, list)):
                if not value:
                    if should_purge:
                        del properties[prop.Name]
                        file.remove(prop)
                        continue
                ref = getattr(prop, "EnumerationReference", None)
                if ref and ref.EnumerationValues:
                    pmt = ref.EnumerationValues[0].is_a()
                elif prop.EnumerationValues:
                    pmt = prop.EnumerationValues[0].is_a()
                else:
                    pmt = _primary_measure_type(prop.Name, new_value=value[0])
                sel_vals = [file.create_entity(pmt, v) for v in value]
                prop.EnumerationValues = tuple(sel_vals) or None
            del properties[prop.Name]
            existing_props.append(prop)
        else:
            existing_props.append(prop)
            del properties[prop.Name]

    # ---- add new properties ------------------------------------------------

    new_props = []
    for prop_name, value in properties.items():
        if value is None and should_purge:
            continue

        if isinstance(value, (ifcopenshell.entity_instance, _typed_value)):
            if hasattr(value, "is_entity") and value.is_entity() is False:
                new_props.append(file.create_entity("IfcPropertySingleValue", Name=prop_name, NominalValue=value))
            elif hasattr(value, "wrappedValue"):
                new_props.append(file.create_entity("IfcPropertySingleValue", Name=prop_name, NominalValue=value))
            elif hasattr(value, "is_a") and value.is_a("IfcProperty"):
                new_props.append(value)
            else:
                raise ValueError(f"{value.is_a()} cannot be assigned to the property set '{prop_name}'")

        elif isinstance(value, (tuple, list)):
            if not value:
                continue
            if pset_template:
                for pt in getattr(pset_template, "HasPropertyTemplates", ()) or ():
                    if pt.Name != prop_name:
                        continue
                    tt = getattr(pt, "TemplateType", None)
                    if tt == "P_ENUMERATEDVALUE":
                        prop_enum = file.create_entity(
                            "IfcPropertyEnumeration",
                            Name=prop_name,
                            EnumerationValues=pt.Enumerators.EnumerationValues,
                        )
                        prop_enum_value = file.create_entity(
                            "IfcPropertyEnumeratedValue",
                            Name=prop_name,
                            EnumerationValues=tuple(
                                file.create_entity(pt.PrimaryMeasureType, v) for v in value
                            ),
                            EnumerationReference=prop_enum,
                        )
                        new_props.append(prop_enum_value)
                        break
                    elif tt == "P_LISTVALUE":
                        ifc_class = getattr(pt, "PrimaryMeasureType", None)
                        if ifc_class is None:
                            raise ValueError(f"pset template '{pt.Name}' is missing PrimaryMeasureType")
                        new_props.append(file.create_entity(
                            "IfcPropertyListValue",
                            Name=prop_name,
                            ListValues=[file.create_entity(ifc_class, v) for v in value],
                        ))
                        break

        else:
            pmt = _primary_measure_type(prop_name, new_value=value)
            if value is None:
                nominal_value = value
            else:
                value = _cast_value(value, pmt)
                nominal_value = file.create_entity(pmt, value)
            new_props.append(file.create_entity("IfcPropertySingleValue", Name=prop_name, NominalValue=nominal_value))

    _assign_properties(existing_props + new_props)


# Reference to the private _typed_value class from __init__
from ifcopenshell import _typed_value
