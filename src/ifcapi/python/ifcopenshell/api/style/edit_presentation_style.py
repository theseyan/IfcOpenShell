# SPDX-License-Identifier: LGPL-3.0-or-later


def edit_presentation_style(file, style=None, attributes=None):
    for name, value in (attributes or {}).items():
        setattr(style, name, value)
