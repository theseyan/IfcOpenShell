# SPDX-License-Identifier: LGPL-3.0-or-later


def edit_metric(file, metric, attributes):
    for name, value in attributes.items():
        setattr(metric, name, value)
