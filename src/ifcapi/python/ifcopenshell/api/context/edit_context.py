# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

"""context.edit_context — edit attributes of an IfcGeometricRepresentationContext."""


def edit_context(file, context, attributes):
    for name, value in attributes.items():
        setattr(context, name, value)
