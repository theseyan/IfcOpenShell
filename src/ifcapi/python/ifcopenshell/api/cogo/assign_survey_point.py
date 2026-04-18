# SPDX-License-Identifier: LGPL-3.0-or-later


def assign_survey_point(annotation, survey_point):
    annotation.Representation.Representations[0].Items = [survey_point]
