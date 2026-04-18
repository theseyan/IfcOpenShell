# SPDX-License-Identifier: LGPL-3.0-or-later

import ifcopenshell


def add_metric(file, objective):
    metric = file.create_entity(
        "IfcMetric", Name="Unnamed", ConstraintGrade="NOTDEFINED", Benchmark="EQUALTO"
    )
    if objective:
        benchmark_values = list(objective.BenchmarkValues or [])
        benchmark_values.append(metric)
        objective.BenchmarkValues = benchmark_values
    return metric
