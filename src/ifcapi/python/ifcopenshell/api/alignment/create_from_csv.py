# SPDX-License-Identifier: LGPL-3.0-or-later
from ifcopenshell.api.alignment import _capi


def create_from_csv(file, filepath):
    with open(filepath, encoding="utf-8", newline="") as stream:
        text = stream.read()
    owner = _capi.owner_options(file)
    options = {
        "csv_text": text,
        **owner.values,
    }
    return _capi.call_handle(
        file, "alignment_create_from_csv_text", _capi.file_handle(file), options
    )
