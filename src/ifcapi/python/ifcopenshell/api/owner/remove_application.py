# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell


def remove_application(file, application=None):
    file.remove(application)
