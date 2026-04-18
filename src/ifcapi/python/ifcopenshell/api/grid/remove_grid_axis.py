# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell.util.element


def remove_grid_axis(file, axis=None):
    axis_curve = axis.AxisCurve
    file.remove(axis)
    if axis_curve:
        ifcopenshell.util.element.remove_deep2(file, axis_curve)
