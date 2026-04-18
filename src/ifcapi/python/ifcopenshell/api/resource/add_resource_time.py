# SPDX-License-Identifier: LGPL-3.0-or-later


def add_resource_time(file, resource=None):
    resource_time = file.create_entity("IfcResourceTime")
    resource.Usage = resource_time
    return resource_time
