# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell


def add_library(file, name):
    return file.create_entity("IfcLibraryInformation", Name=name)
