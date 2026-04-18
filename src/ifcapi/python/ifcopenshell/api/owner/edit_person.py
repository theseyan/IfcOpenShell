# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell


def edit_person(file, person=None, attributes=None):
    for name, value in (attributes or {}).items():
        setattr(person, name, value)
