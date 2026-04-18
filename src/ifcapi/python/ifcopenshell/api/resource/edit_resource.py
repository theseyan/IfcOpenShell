# SPDX-License-Identifier: LGPL-3.0-or-later


def edit_resource(file, resource=None, attributes=None):
    for name, value in (attributes or {}).items():
        setattr(resource, name, value)
