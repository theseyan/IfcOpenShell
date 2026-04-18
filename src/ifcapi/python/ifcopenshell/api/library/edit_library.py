# SPDX-License-Identifier: LGPL-3.0-or-later


def edit_library(file, library, attributes):
    for name, value in attributes.items():
        setattr(library, name, value)
