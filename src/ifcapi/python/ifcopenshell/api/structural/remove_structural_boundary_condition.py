# SPDX-License-Identifier: LGPL-3.0-or-later


def remove_structural_boundary_condition(file, connection=None, boundary_condition=None):
    if connection:
        if not connection.AppliedCondition:
            return
        applied_condition = connection.AppliedCondition
        if file.get_total_inverses(applied_condition) == 1:
            file.remove(applied_condition)
        connection.AppliedCondition = None
    else:
        assert boundary_condition, "Either connection or boundary_condition must be provided."
        for conn in file.get_inverse(boundary_condition):
            conn.AppliedCondition = None
        file.remove(boundary_condition)
