# SPDX-License-Identifier: LGPL-3.0-or-later

def remove_resource_quantity(file, resource=None):
    old_quantity = resource.BaseQuantity
    resource.BaseQuantity = None
    if old_quantity:
        file.remove(old_quantity)
