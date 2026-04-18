# SPDX-License-Identifier: LGPL-3.0-or-later

"""classification.edit_reference — edit attributes of an IfcClassificationReference."""


def edit_reference(file, reference, attributes):
    for name, value in attributes.items():
        setattr(reference, name, value)
