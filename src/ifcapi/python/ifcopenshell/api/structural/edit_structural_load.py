# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later


def edit_structural_load(file, structural_load=None, attributes=None):
    for name, value in (attributes or {}).items():
        setattr(structural_load, name, value)
