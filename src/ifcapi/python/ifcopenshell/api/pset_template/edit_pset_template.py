# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later


def edit_pset_template(file, pset_template, attributes):
    for name, value in attributes.items():
        setattr(pset_template, name, value)
