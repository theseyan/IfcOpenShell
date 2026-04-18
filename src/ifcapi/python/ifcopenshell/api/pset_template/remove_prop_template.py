# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell.util.element


def remove_prop_template(file, prop_template):
    for inverse in file.get_inverse(prop_template):
        if hasattr(inverse, "HasPropertyTemplates"):
            if len(inverse.HasPropertyTemplates) > 1:
                has_property_templates = list(inverse.HasPropertyTemplates)
                has_property_templates.remove(prop_template)
                inverse.HasPropertyTemplates = has_property_templates
                ifcopenshell.util.element.remove_deep2(file, prop_template)
                return
            else:
                # Don't remove the last prop template
                return
    ifcopenshell.util.element.remove_deep2(file, prop_template)
