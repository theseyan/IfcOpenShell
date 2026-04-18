# SPDX-License-Identifier: LGPL-3.0-or-later


def edit_survey_point(annotation, x, y, z=0.0):
    item = annotation.Representation.Representations[0].Items[0]
    if len(item.Coordinates) == 2:
        item.Coordinates = (x, y)
    else:
        item.Coordinates = (x, y, z)
