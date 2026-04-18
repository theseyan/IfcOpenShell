# SPDX-License-Identifier: LGPL-3.0-or-later

"""document.edit_reference — edit document reference attributes."""


def edit_reference(file, reference, attributes):
    for name, value in attributes.items():
        setattr(reference, name, value)
