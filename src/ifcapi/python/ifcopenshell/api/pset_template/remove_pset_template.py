# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell.util.element


def remove_pset_template(file, pset_template):
    ifcopenshell.util.element.remove_deep2(file, pset_template)
