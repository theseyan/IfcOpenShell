# This file was generated with the assistance of an AI coding tool.

from __future__ import annotations

from pathlib import Path

import pytest

from src.ifcwrap.binding_generator.audit_ifcparse_parity import audit_ifcparse_parity, render_markdown


def _compile_commands() -> Path:
    path = Path("/tmp/ifcopenshell-capi-build/compile_commands.json")
    if not path.exists():
        pytest.skip("compile_commands.json is required for IfcParse parity audit tests")
    return path


def _has_entry(items: list[dict], owner: str, name: str) -> bool:
    return any(item["owner"] == owner and item["name"] == name for item in items)


def test_audit_ifcparse_parity_report() -> None:
    report = audit_ifcparse_parity(
        wrapper_path=Path(__file__).resolve().parents[1] / "IfcParseWrapper.i",
        spec_path=Path(__file__).resolve().parents[1] / "binding_generator" / "specs" / "ifcparse.yml",
        compile_commands_path=_compile_commands(),
    )

    assert report["summary"]["swig_total"] >= 150
    assert report["summary"]["generated_total"] >= 140

    assert _has_entry(report["covered"], "global", "open")
    assert _has_entry(report["covered"], "file", "FreshId")
    assert _has_entry(report["covered"], "file", "by_type")
    assert _has_entry(report["covered"], "schema", "declarations")
    assert _has_entry(report["covered"], "instance", "get_argument")
    assert _has_entry(report["covered"], "file", "to_string")
    assert _has_entry(report["covered"], "instance", "get_argument_name")
    assert _has_entry(report["covered"], "aggregation_type", "type_of_aggregation_string")
    assert _has_entry(report["covered"], "attribute", "name")
    assert _has_entry(report["covered"], "entity", "attribute_count")
    assert _has_entry(report["covered"], "file", "remove")
    assert _has_entry(report["covered"], "header", "file_description_py")
    assert _has_entry(report["covered"], "file_description", "description")
    assert _has_entry(report["covered"], "instance_streamer", "status")
    assert _has_entry(report["covered"], "global", "get_info_cpp")
    assert _has_entry(report["covered"], "global", "IfcSIPrefixToValue")
    assert _has_entry(report["covered"], "file", "getUnit")
    assert _has_entry(report["covered"], "instance_streamer", "readInstancePy")
    assert _has_entry(report["covered"], "instance", "set_attribute_value")

    assert report["missing"] == []

    assert report["unmapped"] == []

    assert _has_entry(report["generated_only"], "attribute_value", "as_string")
    assert _has_entry(report["generated_only"], "file", "instances_by_reference")

    markdown = render_markdown(report)
    assert "# IfcParse Parity Audit" in markdown
    assert "## Summary" in markdown
