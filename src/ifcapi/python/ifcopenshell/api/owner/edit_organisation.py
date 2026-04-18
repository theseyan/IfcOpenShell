# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell


def edit_organisation(file, organisation=None, attributes=None):
    for name, value in (attributes or {}).items():
        setattr(organisation, name, value)
