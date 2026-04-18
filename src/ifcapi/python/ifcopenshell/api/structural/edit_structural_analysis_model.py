# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later


def edit_structural_analysis_model(file, structural_analysis_model=None, attributes=None):
    for name, value in (attributes or {}).items():
        setattr(structural_analysis_model, name, value)
