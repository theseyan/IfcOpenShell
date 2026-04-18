# SPDX-License-Identifier: LGPL-3.0-or-later


def edit_reference(file, reference, attributes):
    for name, value in attributes.items():
        setattr(reference, name, value)
