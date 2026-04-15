from __future__ import annotations

from pathlib import Path
import re


def test_ifcparse_smoke_sources_reference_every_generated_function() -> None:
    tests_dir = Path(__file__).resolve().parent
    header_path = tests_dir.parent / "binding_generator" / "generated" / "ifcopenshell_api.h"

    header = header_path.read_text(encoding="utf-8")
    smoke_sources = list(tests_dir.glob("*c_api*smoke.c"))
    assert smoke_sources

    referenced = set()
    for source_path in smoke_sources:
        source = source_path.read_text(encoding="utf-8")
        referenced.update(re.findall(r"(ifcopenshell_[a-zA-Z0-9_]+)\s*\(", source))

    generated = {
        name
        for name in re.findall(r"^bool\s+(ifcopenshell_[a-zA-Z0-9_]+)\(", header, flags=re.MULTILINE)
        if not name.endswith("_clear_error") and not name.endswith("_last_error_message")
    }

    missing = sorted(generated - referenced)
    assert missing == []
