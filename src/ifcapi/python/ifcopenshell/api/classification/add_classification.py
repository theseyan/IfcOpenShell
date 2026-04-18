# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

"""classification.add_classification — add a classification system to the project."""

import ifcopenshell
import ifcopenshell.guid


def add_classification(file, classification):
    if isinstance(classification, str):
        cls = file.create_entity("IfcClassification", Name=classification)
    else:
        cls = _copy_classification(file, classification)
    file.create_entity("IfcRelAssociatesClassification",
                       GlobalId=ifcopenshell.guid.new(),
                       RelatedObjects=[file.by_type("IfcProject")[0]],
                       RelatingClassification=cls)
    return cls


def _copy_classification(file, src):
    """Copy an IfcClassification from another file into this file."""
    info = src.get_info()
    info.pop("id", None)
    info.pop("type", None)
    # Remove None values and entity references (simple copy only)
    kwargs = {}
    for k, v in info.items():
        if v is None:
            continue
        kwargs[k] = v
    return file.create_entity("IfcClassification", **kwargs)
