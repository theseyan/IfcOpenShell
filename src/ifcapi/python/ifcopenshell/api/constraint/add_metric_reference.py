# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell


def add_metric_reference(file, metric, reference_path):
    references_created = []
    if reference_path:
        attributes = reference_path.split(".")
        for i, attr_name in enumerate(attributes):
            reference = file.create_entity("IfcReference")
            reference.AttributeIdentifier = attr_name
            if i == 0:
                metric.ReferencePath = reference
            else:
                references_created[i - 1].InnerReference = reference
            references_created.append(reference)
    return references_created
