# This file was generated with the assistance of an AI coding tool.

from __future__ import annotations

import argparse
from collections import defaultdict
from dataclasses import dataclass
from pathlib import Path
import re

try:
    from .authored_spec import load_authored_spec
    from .clang_discovery import discover_namespace_functions_with_compile_commands, discover_public_methods_with_compile_commands
except ImportError:  # pragma: no cover - script execution fallback
    from authored_spec import load_authored_spec
    from clang_discovery import discover_namespace_functions_with_compile_commands, discover_public_methods_with_compile_commands

AI_HEADER = "<!-- This file was generated with the assistance of an AI coding tool. -->\n"

IGNORE_RE = re.compile(r"^\s*%ignore\s+(.+?);$")
RENAME_RE = re.compile(r'^\s*%rename\("([^"]+)"\)\s+(.+?);$')
EXTEND_RE = re.compile(r"^\s*%extend\s+([^\s{]+)\s*\{")
INLINE_RE = re.compile(r"^\s*%inline\s+%{$")
PYTHONCODE_RE = re.compile(r"^\s*%pythoncode\s+%{$")
CPP_CALLABLE_RE = re.compile(
    r"""
    ^\s*
    (?:[\w:\<\>\~\*\&\,\s]+?)      # return type / qualifiers
    \s+
    (?P<name>[A-Za-z_][A-Za-z0-9_]*)
    \s*
    \([^;{{}}]*\)
    \s*(?:const\s*)?
    \{                              # definition start
    """,
    re.VERBOSE,
)

CONTROL_KEYWORDS = {"if", "for", "while", "switch", "catch"}
SKIPPED_SWIG_NAMES = {"__eq__", "__len__", "__repr__"}


@dataclass(frozen=True)
class AuditTarget:
    swig_name: str
    owner: str | None
    translation_unit: str | None
    class_name: str | None


@dataclass(frozen=True)
class SurfaceEntry:
    owner: str
    public_name: str
    cpp_name: str
    source_kind: str
    origin: str
    swig_target: str | None = None


AUDIT_TARGETS = (
    AuditTarget("IfcParse::IfcFile", "file", "src/ifcparse/IfcFile.cpp", "IfcFile"),
    AuditTarget("IfcUtil::IfcBaseClass", "instance", "src/ifcparse/IfcParse.cpp", "IfcBaseClass"),
    AuditTarget("IfcParse::schema_definition", "schema", "src/ifcparse/IfcSchema.cpp", "schema_definition"),
    AuditTarget("IfcParse::declaration", "declaration", "src/ifcparse/IfcSchema.cpp", "declaration"),
    AuditTarget("IfcParse::type_declaration", "type_declaration", "src/ifcparse/IfcSchema.cpp", "type_declaration"),
    AuditTarget("IfcParse::select_type", "select_type", "src/ifcparse/IfcSchema.cpp", "select_type"),
    AuditTarget("IfcParse::enumeration_type", "enumeration", "src/ifcparse/IfcSchema.cpp", "enumeration_type"),
    AuditTarget("IfcParse::named_type", "named_type", "src/ifcparse/IfcSchema.cpp", "named_type"),
    AuditTarget("IfcParse::simple_type", "simple_type", "src/ifcparse/IfcSchema.cpp", "simple_type"),
    AuditTarget("IfcParse::aggregation_type", "aggregation_type", "src/ifcparse/IfcSchema.cpp", "aggregation_type"),
    AuditTarget("IfcParse::attribute", "attribute", "src/ifcparse/IfcSchema.cpp", "attribute"),
    AuditTarget("IfcParse::inverse_attribute", "inverse_attribute", "src/ifcparse/IfcSchema.cpp", "inverse_attribute"),
    AuditTarget("IfcParse::entity", "entity", "src/ifcparse/IfcSchema.cpp", "entity"),
    AuditTarget("IfcParse::InstanceStreamer", "instance_streamer", "src/ifcparse/IfcFile.cpp", "InstanceStreamer"),
    AuditTarget("IfcParse::IfcSpfHeader", "header", "src/ifcparse/IfcSpfHeader.cpp", "IfcSpfHeader"),
    AuditTarget("IfcParse::FileDescription", "file_description", "src/ifcparse/Header_section_schema.cpp", "file_description"),
    AuditTarget("IfcParse::FileName", "file_name", "src/ifcparse/Header_section_schema.cpp", "file_name"),
    AuditTarget("IfcParse::FileSchema", "file_schema", "src/ifcparse/Header_section_schema.cpp", "file_schema"),
    AuditTarget("attribute_value_derived", None, None, None),
)

AUDIT_NAMESPACES = (
    ("IfcParse", "src/ifcparse/IfcParse.cpp"),
    ("IfcParse", "src/ifcparse/IfcSchema.cpp"),
)


def _brace_block(lines: list[str], start: int) -> tuple[list[str], int]:
    block_lines: list[str] = []
    depth = 0
    seen_open = False
    for index in range(start, len(lines)):
        line = lines[index]
        block_lines.append(line)
        for char in line:
            if char == "{":
                depth += 1
                seen_open = True
            elif char == "}":
                depth -= 1
        if seen_open and depth == 0:
            return block_lines, index
    msg = f"Unterminated brace block starting at line {start + 1}"
    raise ValueError(msg)


def _percent_block(lines: list[str], start: int) -> tuple[list[str], int]:
    block_lines: list[str] = []
    for index in range(start, len(lines)):
        line = lines[index]
        block_lines.append(line)
        if line.strip() == "%}":
            return block_lines, index
    msg = f"Unterminated percent block starting at line {start + 1}"
    raise ValueError(msg)


def _parse_wrapper_policy(wrapper_path: Path) -> tuple[dict[str, str], set[str], dict[str, list[str]], list[str]]:
    rename_map: dict[str, str] = {}
    ignore_set: set[str] = set()
    extend_blocks: dict[str, list[str]] = defaultdict(list)
    inline_blocks: list[str] = []

    lines = wrapper_path.read_text(encoding="utf-8").splitlines(keepends=True)
    index = 0
    while index < len(lines):
        line = lines[index]
        if match := IGNORE_RE.match(line):
            ignore_set.add(match.group(1))
            index += 1
            continue
        if match := RENAME_RE.match(line):
            rename_map[match.group(2)] = match.group(1)
            index += 1
            continue
        if match := EXTEND_RE.match(line):
            body_lines, end_index = _brace_block(lines, index)
            extend_blocks[match.group(1)].append("".join(body_lines[1:-1]))
            index = end_index + 1
            continue
        if INLINE_RE.match(line):
            body_lines, end_index = _percent_block(lines, index)
            inline_blocks.append("".join(body_lines[1:-1]))
            index = end_index + 1
            continue
        index += 1

    return rename_map, ignore_set, extend_blocks, inline_blocks


def _iter_top_level_callable_names(block: str) -> list[str]:
    lines = block.splitlines()
    names: list[str] = []
    brace_depth = 0
    comment_depth = 0
    index = 0
    while index < len(lines):
        line = lines[index]
        stripped = line.strip()

        if comment_depth > 0:
            comment_depth += line.count("/*")
            comment_depth -= line.count("*/")
            index += 1
            continue
        if "/*" in line:
            comment_depth += line.count("/*")
            comment_depth -= line.count("*/")
            if comment_depth > 0 or stripped.startswith("/*"):
                index += 1
                continue

        if stripped.startswith("%pythoncode"):
            while index < len(lines) and lines[index].strip() != "%}":
                index += 1
            index += 1
            continue

        if brace_depth == 0 and stripped and not stripped.startswith("//") and not stripped.startswith("/*"):
            signature = stripped
            lookahead = index
            while "{" not in signature and lookahead + 1 < len(lines):
                lookahead += 1
                signature += " " + lines[lookahead].strip()
            match = CPP_CALLABLE_RE.match(signature)
            if match is not None:
                name = match.group("name")
                if name not in CONTROL_KEYWORDS and name not in SKIPPED_SWIG_NAMES:
                    names.append(name)

        brace_depth += line.count("{")
        brace_depth -= line.count("}")
        index += 1
    return names


def _is_ignored(ignore_set: set[str], fq_name: str, cpp_name: str) -> bool:
    for ignored in ignore_set:
        if ignored == fq_name or ignored.startswith(f"{fq_name}("):
            return True
        if ignored == cpp_name or ignored.startswith(f"{cpp_name}("):
            return True
    return False


def _public_name(rename_map: dict[str, str], fq_name: str, cpp_name: str) -> str:
    return rename_map.get(fq_name, rename_map.get(cpp_name, cpp_name))


def _unique_entries(entries: list[SurfaceEntry]) -> list[SurfaceEntry]:
    deduped: dict[tuple[str, str], SurfaceEntry] = {}
    for entry in entries:
        deduped.setdefault((entry.owner, entry.public_name), entry)
    return sorted(deduped.values(), key=lambda entry: (entry.owner, entry.public_name))


def _canonical_name(name: str) -> str:
    chars: list[str] = []
    previous_is_lower_or_digit = False
    previous_was_separator = False
    for index, char in enumerate(name):
        if char in {"_", "."}:
            chars.append(char)
            previous_is_lower_or_digit = False
            previous_was_separator = True
            continue
        is_upper = char.isalpha() and char.upper() == char and char.lower() != char
        next_is_lower = index + 1 < len(name) and name[index + 1].islower()
        if chars and not previous_was_separator and is_upper and (previous_is_lower_or_digit or next_is_lower):
            chars.append("_")
        chars.append(char.lower())
        previous_is_lower_or_digit = char.islower() or char.isdigit()
        previous_was_separator = False
    canonical = "".join(chars)
    aliases = {
        "declaration.is": "declaration.is_a",
        "named_type.is": "named_type.is_a",
        "header.file_description_py": "header.file_description",
        "header.file_name_py": "header.file_name",
        "header.file_schema_py": "header.file_schema",
        "instance.set_argument_as_null": "instance.unset_argument",
        "instance.set_argument_as_int": "instance.set_argument_int32",
        "instance.set_argument_as_bool": "instance.set_argument_bool",
        "instance.set_argument_as_logical": "instance.set_argument_logical",
        "instance.set_argument_as_double": "instance.set_argument_double",
        "instance.set_argument_as_string": "instance.set_argument_string",
        "instance.set_argument_as_entity_instance": "instance.set_argument_instance",
        "instance.set_argument_as_aggregate_of_entity_instance": "instance.set_argument_instance_list",
        "instance.set_argument_as_aggregate_of_int": "instance.set_argument_int32_list",
        "instance.set_argument_as_aggregate_of_double": "instance.set_argument_double_list",
        "instance.set_argument_as_aggregate_of_string": "instance.set_argument_string_list",
        "instance.set_argument_as_aggregate_of_aggregate_of_int": "instance.set_argument_int32_list_list",
        "instance.set_argument_as_aggregate_of_aggregate_of_double": "instance.set_argument_double_list_list",
    }
    return aliases.get(canonical, canonical)


def collect_swig_surface(wrapper_path: Path, compile_commands_path: Path) -> list[SurfaceEntry]:
    rename_map, ignore_set, extend_blocks, inline_blocks = _parse_wrapper_policy(wrapper_path)
    entries: list[SurfaceEntry] = []

    for target in AUDIT_TARGETS:
        if target.translation_unit is not None and target.class_name is not None:
            methods_by_name = discover_public_methods_with_compile_commands(
                compile_commands_path,
                Path(target.translation_unit),
                target.class_name,
            )
            for cpp_name in sorted(methods_by_name):
                fq_name = f"{target.swig_name}::{cpp_name}"
                if _is_ignored(ignore_set, fq_name, cpp_name):
                    continue
                owner = target.owner or target.swig_name
                entries.append(
                    SurfaceEntry(
                        owner=owner,
                        public_name=_public_name(rename_map, fq_name, cpp_name),
                        cpp_name=cpp_name,
                        source_kind="swig_discovery",
                        origin=f"{target.swig_name}::{cpp_name}",
                        swig_target=target.swig_name,
                    )
                )

        for block in extend_blocks.get(target.swig_name, []):
            for method_name in _iter_top_level_callable_names(block):
                owner = target.owner or target.swig_name
                entries.append(
                    SurfaceEntry(
                        owner=owner,
                        public_name=method_name,
                        cpp_name=method_name,
                        source_kind="swig_extend",
                        origin=f"{target.swig_name}::{method_name}",
                        swig_target=target.swig_name,
                    )
                )

    for namespace_name, translation_unit in AUDIT_NAMESPACES:
        functions_by_name = discover_namespace_functions_with_compile_commands(
            compile_commands_path,
            Path(translation_unit),
            namespace_name,
        )
        for cpp_name in sorted(functions_by_name):
            fq_name = f"{namespace_name}::{cpp_name}"
            if _is_ignored(ignore_set, fq_name, cpp_name):
                continue
            entries.append(
                SurfaceEntry(
                    owner="global",
                    public_name=_public_name(rename_map, fq_name, cpp_name),
                    cpp_name=cpp_name,
                    source_kind="swig_discovery",
                    origin=f"{namespace_name}::{cpp_name}",
                    swig_target=None,
                )
            )

    for block in inline_blocks:
        for function_name in _iter_top_level_callable_names(block):
            if function_name in SKIPPED_SWIG_NAMES:
                continue
            entries.append(
                SurfaceEntry(
                    owner="global",
                    public_name=function_name,
                    cpp_name=function_name,
                    source_kind="swig_inline",
                    origin=function_name,
                    swig_target=None,
                )
            )

    return _unique_entries(entries)


def collect_generated_surface(spec_path: Path, compile_commands_path: Path) -> list[SurfaceEntry]:
    spec = load_authored_spec(spec_path, compile_commands_path=compile_commands_path)
    entries: list[SurfaceEntry] = []
    for call in spec.functions:
        entries.append(
            SurfaceEntry(
                owner="global",
                public_name=call.expose_as,
                cpp_name=call.cpp_name or call.expose_as,
                source_kind="generated_function",
                origin=call.c_name,
            )
        )
    for call in spec.methods:
        entries.append(
            SurfaceEntry(
                owner=call.receiver or "global",
                public_name=call.expose_as,
                cpp_name=call.cpp_name or call.expose_as,
                source_kind="generated_method",
                origin=call.c_name,
            )
        )
    return _unique_entries(entries)


def audit_ifcparse_parity(
    *,
    wrapper_path: Path,
    spec_path: Path,
    compile_commands_path: Path,
) -> dict:
    swig_entries = collect_swig_surface(wrapper_path, compile_commands_path)
    generated_entries = collect_generated_surface(spec_path, compile_commands_path)

    generated_keys = {(entry.owner, _canonical_name(f"{entry.owner}.{entry.public_name}")) for entry in generated_entries}
    generated_by_key = {
        (entry.owner, _canonical_name(f"{entry.owner}.{entry.public_name}")): entry for entry in generated_entries
    }
    mapped_owners = {entry.owner for entry in generated_entries}

    covered: list[dict] = []
    missing: list[dict] = []
    unmapped: list[dict] = []
    swig_keys: set[tuple[str, str]] = set()

    for entry in swig_entries:
        key = (entry.owner, _canonical_name(f"{entry.owner}.{entry.public_name}"))
        swig_keys.add(key)
        record = {
            "owner": entry.owner,
            "name": entry.public_name,
            "origin": entry.origin,
            "source_kind": entry.source_kind,
        }
        if entry.owner in mapped_owners:
            if key in generated_keys:
                record["generated_c_name"] = generated_by_key[key].origin
                covered.append(record)
            else:
                missing.append(record)
        else:
            unmapped.append(record)

    generated_only = [
        {
            "owner": entry.owner,
            "name": entry.public_name,
            "generated_c_name": entry.origin,
            "source_kind": entry.source_kind,
        }
        for entry in generated_entries
        if (entry.owner, _canonical_name(f"{entry.owner}.{entry.public_name}")) not in swig_keys
    ]

    return {
        "summary": {
            "swig_total": len(swig_entries),
            "generated_total": len(generated_entries),
            "covered_total": len(covered),
            "missing_total": len(missing),
            "unmapped_total": len(unmapped),
            "generated_only_total": len(generated_only),
        },
        "covered": sorted(covered, key=lambda item: (item["owner"], item["name"])),
        "missing": sorted(missing, key=lambda item: (item["owner"], item["name"])),
        "unmapped": sorted(unmapped, key=lambda item: (item["owner"], item["name"])),
        "generated_only": sorted(generated_only, key=lambda item: (item["owner"], item["name"])),
    }


def render_markdown(report: dict) -> str:
    summary = report["summary"]

    def section(title: str, items: list[dict], *, limit: int | None = None) -> str:
        if not items:
            return f"## {title}\n\nNone.\n"
        rows = items if limit is None else items[:limit]
        lines = [f"## {title}", ""]
        for item in rows:
            detail = item.get("generated_c_name", item.get("origin", ""))
            lines.append(f"- `{item['owner']}.{item['name']}`")
            if detail:
                lines.append(f"  `{detail}`")
        if limit is not None and len(items) > limit:
            lines.append(f"- `{len(items) - limit}` more")
        lines.append("")
        return "\n".join(lines)

    parts = [
        AI_HEADER.rstrip(),
        "",
        "# IfcParse Parity Audit",
        "",
        "## Summary",
        "",
        f"- SWIG surface entries: `{summary['swig_total']}`",
        f"- Generated C surface entries: `{summary['generated_total']}`",
        f"- Covered entries: `{summary['covered_total']}`",
        f"- Missing on mapped receivers: `{summary['missing_total']}`",
        f"- SWIG entries on unmapped receivers: `{summary['unmapped_total']}`",
        f"- Generated-only entries: `{summary['generated_only_total']}`",
        "",
        section("Missing", report["missing"], limit=80).rstrip(),
        "",
        section("Unmapped", report["unmapped"], limit=80).rstrip(),
        "",
        section("Generated Only", report["generated_only"], limit=80).rstrip(),
    ]
    return "\n".join(parts) + "\n"


def _build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(description="Audit SWIG-vs-generated IfcParse surface parity.")
    parser.add_argument("--wrapper", type=Path, required=True, help="Path to IfcParseWrapper.i")
    parser.add_argument("--spec", type=Path, required=True, help="Path to the authored IfcParse binding spec")
    parser.add_argument("--compile-commands", type=Path, required=True, help="Path to compile_commands.json")
    parser.add_argument("--report-out", type=Path, required=True, help="Markdown report output path")
    return parser


def main() -> int:
    args = _build_parser().parse_args()
    report = audit_ifcparse_parity(
        wrapper_path=args.wrapper,
        spec_path=args.spec,
        compile_commands_path=args.compile_commands,
    )
    args.report_out.parent.mkdir(parents=True, exist_ok=True)
    args.report_out.write_text(render_markdown(report), encoding="utf-8")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
