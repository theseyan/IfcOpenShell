# SPDX-License-Identifier: LGPL-3.0-or-later

# Simplified version: the upstream uses ifcopenshell.util.date for datetime
# conversion and ifcopenshell.util.constraint for locked attribute checks.
# Those utilities are not yet ported, so we set attributes directly.
# ISO 8601 duration strings (e.g. "PT16H") can be passed as-is.


def edit_resource_time(file, resource_time=None, attributes=None):
    for name, value in (attributes or {}).items():
        setattr(resource_time, name, value)
