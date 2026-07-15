#!/usr/bin/env python3
# SPDX-License-Identifier: LGPL-3.0-or-later

import argparse
import json
from pathlib import Path


def load(path: Path) -> dict[str, list[str]]:
    with path.open(encoding="utf-8") as stream:
        return json.load(stream)


def quote(value: str) -> str:
    return json.dumps(value)


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--output", type=Path, required=True)
    parser.add_argument("--ifc2x3", type=Path, required=True)
    parser.add_argument("--ifc4", type=Path, required=True)
    parser.add_argument("--ifc4x3", type=Path, required=True)
    args = parser.parse_args()

    schemas = [
        ("IFC2X3", load(args.ifc2x3)),
        ("IFC4", load(args.ifc4)),
        ("IFC4X3", load(args.ifc4x3)),
    ]
    lines = ["// Generated from ifcopenshell.util entity-to-type maps. Do not edit."]
    for schema, mapping in schemas:
        lines.append(f"static const TypeMapEntry TYPE_MAP_{schema}[] = {{")
        for occurrence, types in mapping.items():
            values = ", ".join(quote(value) for value in types)
            lines.append(f"    {{{quote(occurrence)}, {{{values}}}}},")
        lines.append("};")
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text("\n".join(lines) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()
