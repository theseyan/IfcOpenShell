# SPDX-License-Identifier: LGPL-3.0-or-later

"""document.edit_information — edit document information attributes."""


def edit_information(file, information, attributes):
    for name, value in attributes.items():
        setattr(information, name, value)
