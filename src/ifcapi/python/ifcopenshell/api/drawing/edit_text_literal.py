# SPDX-License-Identifier: LGPL-3.0-or-later


def edit_text_literal(file, text_literal=None, attributes=None):
    for name, value in (attributes or {}).items():
        setattr(text_literal, name, value)
