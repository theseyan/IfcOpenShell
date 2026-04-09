# This file was generated with the assistance of an AI coding tool.

from __future__ import annotations

import json
from pathlib import Path
import shlex
import subprocess
from collections import defaultdict

from dataclasses import dataclass


@dataclass(frozen=True)
class DiscoveredParam:
    name: str
    cpp_type: str


@dataclass(frozen=True)
class DiscoveredMethod:
    class_name: str
    cpp_name: str
    return_cpp_type: str
    params: tuple[DiscoveredParam, ...]
    is_const: bool


@dataclass(frozen=True)
class DiscoveredField:
    class_name: str
    cpp_name: str
    cpp_type: str


@dataclass(frozen=True)
class DiscoveredFunction:
    namespace: str
    cpp_name: str
    return_cpp_type: str
    params: tuple[DiscoveredParam, ...]


@dataclass(frozen=True)
class CompileCommand:
    directory: Path
    file: Path
    arguments: tuple[str, ...]


def _parse_compile_commands(path: Path) -> list[CompileCommand]:
    raw = json.loads(path.read_text(encoding="utf-8"))
    commands: list[CompileCommand] = []
    for item in raw:
        directory = Path(item["directory"])
        file_path = Path(item["file"])
        if "arguments" in item:
            arguments = tuple(item["arguments"])
        else:
            arguments = tuple(shlex.split(item["command"]))
        commands.append(CompileCommand(directory=directory, file=file_path, arguments=arguments))
    return commands


def _decode_json_stream(text: str) -> list[dict]:
    decoder = json.JSONDecoder()
    objects: list[dict] = []
    index = 0
    while index < len(text):
        while index < len(text) and text[index].isspace():
            index += 1
        if index >= len(text):
            break
        obj, end = decoder.raw_decode(text, index)
        objects.append(obj)
        index = end
    return objects


def _build_ast_dump_command(command: CompileCommand, *, ast_filter: str) -> list[str]:
    args: list[str] = []
    skip_next = False
    source = str(command.file)
    for token in command.arguments:
        if skip_next:
            skip_next = False
            continue
        if token == "-o":
            skip_next = True
            continue
        if token == "-c" or token == source:
            continue
        args.append(token)

    args.extend(["-Xclang", "-ast-dump=json", "-Xclang", f"-ast-dump-filter={ast_filter}", "-fsyntax-only", source])
    return args


def _default_access(record: dict) -> str:
    return "public" if record.get("tagUsed") == "struct" else "private"


def _extract_public_methods(record: dict) -> dict[str, tuple[DiscoveredMethod, ...]]:
    methods: dict[str, list[DiscoveredMethod]] = defaultdict(list)
    access = _default_access(record)
    for child in record.get("inner", []):
        if child.get("kind") == "AccessSpecDecl":
            access = child.get("access", access)
            continue
        if access != "public":
            continue
        if child.get("kind") != "CXXMethodDecl":
            continue
        if child.get("name") in {"operator=", "operator[]"}:
            continue

        params = tuple(
            DiscoveredParam(
                name=param.get("name") or f"arg_{index}",
                cpp_type=param.get("type", {}).get("qualType", ""),
            )
            for index, param in enumerate(item for item in child.get("inner", []) if item.get("kind") == "ParmVarDecl")
        )
        methods[child["name"]].append(
            DiscoveredMethod(
                class_name=record.get("name", ""),
                cpp_name=child["name"],
                return_cpp_type=child.get("type", {}).get("qualType", "").rsplit("(", 1)[0].strip(),
                params=params,
                is_const=child.get("type", {}).get("qualType", "").endswith(" const"),
            )
        )
    return {name: tuple(overloads) for name, overloads in methods.items()}


def _extract_public_fields(record: dict) -> dict[str, DiscoveredField]:
    fields: dict[str, DiscoveredField] = {}
    access = _default_access(record)
    for child in record.get("inner", []):
        if child.get("kind") == "AccessSpecDecl":
            access = child.get("access", access)
            continue
        if access != "public":
            continue
        if child.get("kind") != "FieldDecl":
            continue
        name = child.get("name")
        if not name:
            continue
        fields[name] = DiscoveredField(
            class_name=record.get("name", ""),
            cpp_name=name,
            cpp_type=child.get("type", {}).get("qualType", ""),
        )
    return fields


def _qualified_namespace(prefix: str, name: str) -> str:
    if not prefix:
        return name
    if not name:
        return prefix
    return f"{prefix}::{name}"


def _extract_namespace_functions(node: dict, namespace_name: str, current_namespace: str = "") -> dict[str, tuple[DiscoveredFunction, ...]]:
    functions: dict[str, list[DiscoveredFunction]] = defaultdict(list)

    kind = node.get("kind")
    name = node.get("name", "")
    next_namespace = current_namespace
    if kind == "NamespaceDecl":
        next_namespace = _qualified_namespace(current_namespace, name)

    if kind == "FunctionDecl" and current_namespace == namespace_name:
        params = tuple(
            DiscoveredParam(
                name=param.get("name") or f"arg_{index}",
                cpp_type=param.get("type", {}).get("qualType", ""),
            )
            for index, param in enumerate(item for item in node.get("inner", []) if item.get("kind") == "ParmVarDecl")
        )
        functions[name].append(
            DiscoveredFunction(
                namespace=current_namespace,
                cpp_name=name,
                return_cpp_type=node.get("type", {}).get("qualType", "").rsplit("(", 1)[0].strip(),
                params=params,
            )
        )

    for child in node.get("inner", []):
        for func_name, overloads in _extract_namespace_functions(child, namespace_name, next_namespace).items():
            functions[func_name].extend(overloads)

    return {func_name: tuple(overloads) for func_name, overloads in functions.items()}


def discover_public_methods_with_compile_commands(
    compile_commands_path: Path,
    translation_unit: Path,
    class_name: str,
    include_inherited: bool = False,
) -> dict[str, tuple[DiscoveredMethod, ...]]:
    compile_commands = _parse_compile_commands(compile_commands_path)
    tu_resolved = translation_unit.resolve()
    command = next((item for item in compile_commands if item.file.resolve() == tu_resolved), None)
    if command is None:
        msg = f"Translation unit '{translation_unit}' not found in '{compile_commands_path}'"
        raise ValueError(msg)

    proc = subprocess.run(
        _build_ast_dump_command(command, ast_filter=class_name),
        cwd=command.directory,
        capture_output=True,
        text=True,
        check=False,
    )
    if proc.returncode != 0:
        msg = proc.stderr.strip() or f"clang AST dump failed for '{translation_unit}'"
        raise RuntimeError(msg)

    objects = _decode_json_stream(proc.stdout)
    record = _find_record_in_nodes(objects, class_name)
    if record is None:
        msg = f"Class '{class_name}' not found in AST for '{translation_unit}'"
        raise ValueError(msg)
    methods = _extract_public_methods(record)

    if include_inherited:
        visited: set[str] = {class_name}
        queue = _get_base_class_names(record)
        while queue:
            base_name = queue.pop(0)
            if base_name in visited:
                continue
            visited.add(base_name)
            base_record = _ast_dump_record(command, base_name)
            if base_record is None:
                continue
            base_methods = _extract_public_methods(base_record)
            for mname, overloads in base_methods.items():
                if mname not in methods:
                    methods[mname] = overloads
            queue.extend(_get_base_class_names(base_record))

    return methods


def _get_base_class_names(record: dict) -> list[str]:
    """Extract direct base class names from a CXXRecordDecl AST node."""
    bases = []
    for base in record.get("bases", []):
        qual_type = base.get("type", {}).get("qualType", "")
        # Strip template args first (before ::), then extract simple name
        template_start = qual_type.find("<")
        name_part = qual_type[:template_start] if template_start >= 0 else qual_type
        simple_name = name_part.rsplit("::", 1)[-1]
        if simple_name:
            bases.append(simple_name)
    return bases


def _find_record_in_nodes(nodes: list[dict], class_name: str) -> dict | None:
    """Find the last CXXRecordDecl by name in AST nodes, searching inside namespace and template nodes."""
    result = None
    for obj in nodes:
        if obj.get("kind") == "CXXRecordDecl" and obj.get("name") == class_name and obj.get("completeDefinition"):
            result = obj
        # Search inside namespace and class template nodes
        if obj.get("kind") in ("NamespaceDecl", "ClassTemplateDecl"):
            inner_result = _find_record_in_nodes(obj.get("inner", []), class_name)
            if inner_result is not None:
                result = inner_result
    return result


def _ast_dump_record(command: _CompileCommand, class_name: str) -> dict | None:
    """Run clang AST dump for a class and return the CXXRecordDecl node, or None."""
    proc = subprocess.run(
        _build_ast_dump_command(command, ast_filter=class_name),
        cwd=command.directory,
        capture_output=True,
        text=True,
        check=False,
    )
    if proc.returncode != 0:
        return None
    objects = _decode_json_stream(proc.stdout)
    return _find_record_in_nodes(objects, class_name)


def discover_public_fields_with_compile_commands(
    compile_commands_path: Path,
    translation_unit: Path,
    class_name: str,
    include_inherited: bool = False,
) -> dict[str, DiscoveredField]:
    compile_commands = _parse_compile_commands(compile_commands_path)
    tu_resolved = translation_unit.resolve()
    command = next((item for item in compile_commands if item.file.resolve() == tu_resolved), None)
    if command is None:
        msg = f"Translation unit '{translation_unit}' not found in '{compile_commands_path}'"
        raise ValueError(msg)

    record = _ast_dump_record(command, class_name)
    if record is None:
        msg = f"Class '{class_name}' not found in AST for '{translation_unit}'"
        raise ValueError(msg)

    fields = _extract_public_fields(record)

    if include_inherited:
        # Walk base classes, collecting inherited fields (cache to avoid re-dumps)
        visited: set[str] = {class_name}
        queue = _get_base_class_names(record)
        while queue:
            base_name = queue.pop(0)
            if base_name in visited:
                continue
            visited.add(base_name)
            base_record = _ast_dump_record(command, base_name)
            if base_record is None:
                continue
            base_fields = _extract_public_fields(base_record)
            for fname, fval in base_fields.items():
                if fname not in fields:
                    fields[fname] = fval
            queue.extend(_get_base_class_names(base_record))

    return fields


def discover_namespace_functions_with_compile_commands(
    compile_commands_path: Path,
    translation_unit: Path,
    namespace_name: str,
) -> dict[str, tuple[DiscoveredFunction, ...]]:
    compile_commands = _parse_compile_commands(compile_commands_path)
    tu_resolved = translation_unit.resolve()
    command = next((item for item in compile_commands if item.file.resolve() == tu_resolved), None)
    if command is None:
        msg = f"Translation unit '{translation_unit}' not found in '{compile_commands_path}'"
        raise ValueError(msg)

    proc = subprocess.run(
        _build_ast_dump_command(command, ast_filter=namespace_name.split("::")[-1]),
        cwd=command.directory,
        capture_output=True,
        text=True,
        check=False,
    )
    if proc.returncode != 0:
        msg = proc.stderr.strip() or f"clang AST dump failed for '{translation_unit}'"
        raise RuntimeError(msg)

    functions: dict[str, list[DiscoveredFunction]] = defaultdict(list)
    for obj in _decode_json_stream(proc.stdout):
        for func_name, overloads in _extract_namespace_functions(obj, namespace_name).items():
            functions[func_name].extend(overloads)
    if not functions:
        msg = f"Namespace '{namespace_name}' not found in AST for '{translation_unit}'"
        raise ValueError(msg)
    return {func_name: tuple(overloads) for func_name, overloads in functions.items()}
