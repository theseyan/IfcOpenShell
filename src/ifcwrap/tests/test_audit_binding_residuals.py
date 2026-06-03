# This file was generated with the assistance of an AI coding tool.

from __future__ import annotations

from pathlib import Path

from src.ifcwrap.binding_generator.audit_binding_residuals import audit_binding_residuals


def _repo_root() -> Path:
    return Path(__file__).resolve().parents[3]


def _has_residual(report: dict, *, spec: str, category: str, path_contains: str) -> bool:
    return any(
        residual["spec"] == spec
        and residual["category"] == category
        and path_contains in residual["path"]
        for residual in report["residuals"]
    )


def test_binding_residual_audit_classifies_generator_inference_candidates() -> None:
    repo_root = _repo_root()
    spec_dir = repo_root / "src" / "ifcwrap" / "binding_generator" / "specs"
    swig_dir = repo_root / "src" / "ifcwrap"
    report = audit_binding_residuals(
        spec_paths=[spec_dir / "ifcparse.yml", spec_dir / "ifcgeom.yml"],
        swig_paths=[
            swig_dir / "IfcParseWrapper.i",
            swig_dir / "IfcGeomWrapper.i",
            swig_dir / "utils" / "typemaps_in.i",
            swig_dir / "utils" / "typemaps_out.i",
        ],
    )

    summary = report["summary"]
    assert summary["residual_count"] >= 0
    assert summary["redundant_candidate_count"] == 0
    assert summary["category_counts"]["abi_type_policy"] >= 1

    swig_patterns = report["swig_patterns"]
    assert swig_patterns["counts"]["shared_ptr_types"] >= 1
    assert swig_patterns["counts"]["vector_typemap_types"] >= 1
    assert swig_patterns["counts"]["vector_item_types"] >= 1
    assert "ifcopenshell::geometry::taxonomy::item" in swig_patterns["shared_ptr_types"]
    assert "ifcopenshell::geometry::taxonomy::item" in swig_patterns["vector_item_types"]
    assert "int" in swig_patterns["vector_typemap_types"]

    assert _has_residual(report, spec="ifcgeom.yml", category="abi_type_policy", path_contains="function_item_evaluator")
    assert not _has_residual(report, spec="ifcgeom.yml", category="type_override", path_contains="function_item_evaluator")
    assert not _has_residual(report, spec="ifcparse.yml", category="type_override", path_contains="simple_type")

    candidate_reasons = {candidate["reason"] for candidate in report["redundant_candidates"]}
    assert "cxx_inference_gap" not in candidate_reasons
    assert "swig_typemap_pattern" not in candidate_reasons
    assert "swig_extend_pattern" not in candidate_reasons
    assert "swig_policy_derived" not in candidate_reasons
