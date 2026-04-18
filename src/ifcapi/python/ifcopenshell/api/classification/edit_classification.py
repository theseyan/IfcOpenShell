# SPDX-License-Identifier: LGPL-3.0-or-later

"""classification.edit_classification — edit attributes of an IfcClassification."""


def edit_classification(file, classification, attributes):
    for name, value in attributes.items():
        setattr(classification, name, value)
