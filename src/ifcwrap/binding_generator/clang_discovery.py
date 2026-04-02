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


def _extract_public_methods(record: dict) -> dict[str, tuple[DiscoveredMethod, ...]]:
    methods: dict[str, list[DiscoveredMethod]] = defaultdict(list)
    access = "private"
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
    records = [
        obj
        for obj in objects
        if obj.get("kind") == "CXXRecordDecl" and obj.get("name") == class_name and obj.get("completeDefinition")
    ]
    if not records:
        msg = f"Class '{class_name}' not found in AST for '{translation_unit}'"
        raise ValueError(msg)
    return _extract_public_methods(records[-1])


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
