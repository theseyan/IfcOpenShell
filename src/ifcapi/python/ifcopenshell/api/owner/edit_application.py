# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell


def edit_application(file, application=None, attributes=None):
    for name, value in (attributes or {}).items():
        setattr(application, name, value)
