from __future__ import annotations

import json
from collections import defaultdict
from dataclasses import dataclass, field
from pathlib import Path
import re
import shlex
import subprocess

try:
    from .debug import debug_log, debug_path
except ImportError:  # pragma: no cover - script execution fallback
    from debug import debug_log, debug_path


_RECORD_KINDS = {"CXXRecordDecl", "ClassTemplateSpecializationDecl"}
_TYPE_PREFIXES = ("class ", "struct ", "union ", "enum ")
_EXTERNAL_NAMESPACE_PREFIXES = ("std::", "boost::")
_BUILTIN_TYPE_NAMES = {
    "bool",
    "char",
    "double",
    "float",
    "int",
    "int8_t",
    "int16_t",
    "int32_t",
    "int64_t",
    "long",
    "long double",
    "long long",
    "ptrdiff_t",
    "short",
    "signed",
    "size_t",
    "ssize_t",
    "std::int64_t",
    "std::size_t",
    "uint8_t",
    "uint16_t",
    "uint32_t",
    "uint64_t",
    "unsigned",
    "unsigned char",
    "unsigned int",
    "unsigned long",
    "unsigned long long",
    "unsigned short",
    "void",
    "wchar_t",
}


def _is_low_signal_unqualified_lookup(text: str) -> bool:
    normalized = text.strip()
    if not normalized or "::" in normalized:
        return False
    if not re.fullmatch(r"[A-Za-z_]\w*", normalized):
        return False
    return normalized.islower() and len(normalized) <= 3


def _is_external_qualified_type(text: str) -> bool:
    normalized = text.strip()
    return normalized.startswith(_EXTERNAL_NAMESPACE_PREFIXES)


@dataclass(frozen=True)
class DiscoveredParam:
    name: str
    cpp_type: str
    cpp_type_ref: "DiscoveredCppType"


@dataclass(frozen=True)
class DiscoveredCppType:
    spelling: str
    desugared_spelling: str | None
    normalized_spelling: str
    normalized_desugared_spelling: str | None
    canonical_spelling: str
    storage_spelling: str
    base_name: str
    base_record_names: tuple[str, ...]
    template_name: str | None
    template_args: tuple["DiscoveredCppType", ...]
    is_enum: bool
    enum_qualified_name: str | None
    is_const: bool
    pointer_depth: int
    is_lvalue_reference: bool
    is_rvalue_reference: bool


@dataclass(frozen=True)
class DiscoveredMethod:
    class_name: str
    cpp_name: str
    return_cpp_type: str
    return_type_ref: DiscoveredCppType
    params: tuple[DiscoveredParam, ...]
    is_const: bool


@dataclass(frozen=True)
class DiscoveredField:
    class_name: str
    cpp_name: str
    cpp_type: str
    cpp_type_ref: DiscoveredCppType


@dataclass(frozen=True)
class DiscoveredFunction:
    namespace: str
    cpp_name: str
    return_cpp_type: str
    return_type_ref: DiscoveredCppType
    params: tuple[DiscoveredParam, ...]


@dataclass(frozen=True)
class CompileCommand:
    directory: Path
    file: Path
    arguments: tuple[str, ...]


@dataclass(frozen=True)
class IndexedRecord:
    qualified_name: str
    simple_name: str
    node: dict


@dataclass(frozen=True)
class IndexedEnum:
    qualified_name: str
    simple_name: str
    node: dict


@dataclass
class TranslationUnitIndex:
    command: CompileCommand
    _loaded_ast_filters: set[str] = field(default_factory=set)
    _records_by_qualified: dict[str, IndexedRecord] = field(default_factory=dict)
    _record_names_by_simple: dict[str, list[str]] = field(default_factory=lambda: defaultdict(list))
    _enums_by_qualified: dict[str, IndexedEnum] = field(default_factory=dict)
    _enum_names_by_simple: dict[str, list[str]] = field(default_factory=lambda: defaultdict(list))
    _namespace_function_cache: dict[str, dict[str, tuple[DiscoveredFunction, ...]]] = field(default_factory=dict)

    def _run_ast_dump(self, ast_filter: str) -> tuple[dict, ...]:
        debug_log("clang.ast_dump.start", f"tu={debug_path(self.command.file)} filter={ast_filter}")
        proc = subprocess.run(
            _build_ast_dump_command(self.command, ast_filter=ast_filter),
            cwd=self.command.directory,
            capture_output=True,
            text=True,
            check=False,
        )
        if proc.returncode != 0:
            msg = proc.stderr.strip() or f"clang AST dump failed for '{self.command.file}'"
            raise RuntimeError(msg)

        objects = tuple(_decode_json_stream(proc.stdout))
        debug_log("clang.ast_dump.done", f"tu={debug_path(self.command.file)} filter={ast_filter} nodes={len(objects)}")
        return objects

    def ensure_ast_filter_loaded(self, ast_filter: str) -> None:
        if ast_filter in self._loaded_ast_filters:
            return
        objects = self._run_ast_dump(ast_filter)
        self._loaded_ast_filters.add(ast_filter)
        self._index_records(objects)

    def ast_objects(self, ast_filter: str) -> tuple[dict, ...]:
        objects = self._run_ast_dump(ast_filter)
        self._index_records(objects)
        return objects

    def resolve_record(self, class_name: str, current_scope: str = "") -> IndexedRecord | None:
        lookup_name = _normalize_record_lookup_name(class_name)
        if not lookup_name:
            return None

        for candidate in _scoped_lookup_candidates(lookup_name, current_scope):
            if (
                current_scope
                and "::" not in lookup_name
                and candidate.startswith(f"{current_scope}::")
                and candidate != lookup_name
                and not self._record_declares_nested_name(current_scope, lookup_name, kinds=_RECORD_KINDS)
            ):
                continue
            if "::" not in candidate and _is_low_signal_unqualified_lookup(candidate):
                continue
            self.ensure_ast_filter_loaded(candidate)

            record = self._records_by_qualified.get(candidate)
            if record is not None:
                return record
            if "::" not in candidate:
                continue
            simple = _simple_name(candidate)
            simple_candidates = self._record_names_by_simple.get(simple, [])
            if simple_candidates:
                resolved = self._records_by_qualified[simple_candidates[-1]]
                if "::" not in resolved.qualified_name:
                    return IndexedRecord(
                        qualified_name=candidate,
                        simple_name=resolved.simple_name,
                        node=resolved.node,
                    )

        return self._resolve_scoped_decl(
            lookup_name,
            current_scope=current_scope,
            qualified=self._records_by_qualified,
            simple=self._record_names_by_simple,
        )

    def resolve_enum(self, enum_name: str, current_scope: str = "") -> IndexedEnum | None:
        lookup_name = _normalize_record_lookup_name(enum_name)
        if not lookup_name:
            return None

        for candidate in _scoped_lookup_candidates(lookup_name, current_scope):
            if (
                current_scope
                and "::" not in lookup_name
                and candidate.startswith(f"{current_scope}::")
                and candidate != lookup_name
                and not self._record_declares_nested_name(current_scope, lookup_name, kinds={"EnumDecl"})
            ):
                continue
            if "::" not in candidate and _is_low_signal_unqualified_lookup(candidate):
                continue
            self.ensure_ast_filter_loaded(candidate)
            enum = self._enums_by_qualified.get(candidate)
            if enum is not None:
                return enum
            if "::" in candidate:
                simple = _simple_name(candidate)
                simple_candidates = self._enum_names_by_simple.get(simple, [])
                if simple_candidates:
                    resolved = self._enums_by_qualified[simple_candidates[-1]]
                    if "::" not in resolved.qualified_name:
                        return IndexedEnum(
                            qualified_name=candidate,
                            simple_name=resolved.simple_name,
                            node=resolved.node,
                        )
        return self._resolve_scoped_decl(
            lookup_name,
            current_scope=current_scope,
            qualified=self._enums_by_qualified,
            simple=self._enum_names_by_simple,
        )

    def discover_namespace_functions(self, namespace_name: str) -> dict[str, tuple[DiscoveredFunction, ...]]:
        cached = self._namespace_function_cache.get(namespace_name)
        if cached is not None:
            return cached

        objects = self.ast_objects(namespace_name)
        functions: dict[str, list[DiscoveredFunction]] = defaultdict(list)
        for obj in objects:
            for func_name, overloads in _extract_namespace_functions(obj, namespace_name, self).items():
                functions[func_name].extend(overloads)

        if not functions:
            msg = f"Namespace '{namespace_name}' not found in AST for '{self.command.file}'"
            raise ValueError(msg)

        result = {func_name: tuple(overloads) for func_name, overloads in functions.items()}
        self._namespace_function_cache[namespace_name] = result
        return result

    def _index_records(self, nodes: tuple[dict, ...]) -> None:
        def visit(node: dict, current_scope: str) -> None:
            kind = node.get("kind")
            name = node.get("name", "")
            next_scope = current_scope

            if kind == "NamespaceDecl":
                next_scope = _qualified_name(current_scope, name)
            elif kind in _RECORD_KINDS and name and node.get("completeDefinition"):
                qualified_name = _qualified_name(current_scope, name)
                if qualified_name not in self._records_by_qualified:
                    self._records_by_qualified[qualified_name] = IndexedRecord(
                        qualified_name=qualified_name,
                        simple_name=name,
                        node=node,
                    )
                    if qualified_name not in self._record_names_by_simple[name]:
                        self._record_names_by_simple[name].append(qualified_name)
                next_scope = qualified_name
            elif kind == "EnumDecl" and name:
                qualified_name = _qualified_name(current_scope, name)
                if qualified_name not in self._enums_by_qualified:
                    self._enums_by_qualified[qualified_name] = IndexedEnum(
                        qualified_name=qualified_name,
                        simple_name=name,
                        node=node,
                    )
                    if qualified_name not in self._enum_names_by_simple[name]:
                        self._enum_names_by_simple[name].append(qualified_name)

            for child in node.get("inner", []):
                visit(child, next_scope)

        for node in nodes:
            visit(node, "")

    def _resolve_scoped_decl(self, name: str, current_scope: str, qualified: dict[str, object], simple: dict[str, list[str]]):
        if name in qualified:
            return qualified[name]
        if "::" not in name and current_scope:
            scope = current_scope
            while scope:
                candidate = f"{scope}::{name}"
                if candidate in qualified:
                    return qualified[candidate]
                scope = scope.rsplit("::", 1)[0] if "::" in scope else ""
        candidates = simple.get(_simple_name(name), [])
        if not candidates:
            return None
        return qualified[candidates[-1]]

    def _record_declares_nested_name(self, record_name: str, nested_name: str, *, kinds: set[str]) -> bool:
        record = self._records_by_qualified.get(record_name)
        if record is None:
            return False
        for child in record.node.get("inner", []):
            if child.get("kind") not in kinds:
                continue
            if child.get("name") != nested_name:
                continue
            if child.get("kind") in _RECORD_KINDS and not child.get("completeDefinition"):
                continue
            return True
        return False


_COMPILE_COMMAND_CACHE: dict[Path, tuple[CompileCommand, ...]] = {}
_TRANSLATION_UNIT_INDEX_CACHE: dict[tuple[Path, Path], TranslationUnitIndex] = {}


def _resolve_path(path: Path, base_dir: Path) -> Path:
    if path.is_absolute():
        return path.resolve()
    return (base_dir / path).resolve()


def _parse_compile_commands(path: Path) -> tuple[CompileCommand, ...]:
    resolved_path = path.resolve()
    cached = _COMPILE_COMMAND_CACHE.get(resolved_path)
    if cached is not None:
        return cached

    debug_log("compile_commands.load.start", f"path={debug_path(resolved_path)}")
    raw = json.loads(resolved_path.read_text(encoding="utf-8"))
    commands: list[CompileCommand] = []
    for item in raw:
        directory = Path(item["directory"]).resolve()
        file_path = _resolve_path(Path(item["file"]), directory)
        if "arguments" in item:
            arguments = tuple(item["arguments"])
        else:
            arguments = tuple(shlex.split(item["command"]))
        commands.append(CompileCommand(directory=directory, file=file_path, arguments=arguments))

    result = tuple(commands)
    _COMPILE_COMMAND_CACHE[resolved_path] = result
    debug_log("compile_commands.load.done", f"path={debug_path(resolved_path)} entries={len(result)}")
    return result


def _translation_unit_index(compile_commands_path: Path, translation_unit: Path) -> TranslationUnitIndex:
    compile_commands_key = compile_commands_path.resolve()
    tu_resolved = translation_unit.resolve()
    cache_key = (compile_commands_key, tu_resolved)

    cached = _TRANSLATION_UNIT_INDEX_CACHE.get(cache_key)
    if cached is not None:
        return cached

    commands = _parse_compile_commands(compile_commands_key)
    command = next((item for item in commands if item.file == tu_resolved), None)
    if command is None:
        msg = f"Translation unit '{translation_unit}' not found in '{compile_commands_path}'"
        raise ValueError(msg)

    index = TranslationUnitIndex(command=command)
    _TRANSLATION_UNIT_INDEX_CACHE[cache_key] = index
    debug_log("clang.tu_index.create", f"compile_commands={debug_path(compile_commands_key)} tu={debug_path(tu_resolved)}")
    return index


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


def _extract_public_methods(record: dict, index: TranslationUnitIndex, current_scope: str) -> dict[str, tuple[DiscoveredMethod, ...]]:
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
                name=param.get("name") or f"arg_{param_index}",
                cpp_type=param.get("type", {}).get("qualType", ""),
                cpp_type_ref=_parse_discovered_cpp_type(param.get("type", {}), index=index, current_scope=current_scope),
            )
            for param_index, param in enumerate(item for item in child.get("inner", []) if item.get("kind") == "ParmVarDecl")
        )
        return_cpp_type = child.get("type", {}).get("qualType", "").rsplit("(", 1)[0].strip()
        methods[child["name"]].append(
            DiscoveredMethod(
                class_name=record.get("name", ""),
                cpp_name=child["name"],
                return_cpp_type=return_cpp_type,
                return_type_ref=_parse_discovered_cpp_type(return_cpp_type, index=index, current_scope=current_scope),
                params=params,
                is_const=child.get("type", {}).get("qualType", "").endswith(" const"),
            )
        )
    return {name: tuple(overloads) for name, overloads in methods.items()}


def _extract_public_fields(record: dict, index: TranslationUnitIndex, current_scope: str) -> dict[str, DiscoveredField]:
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
            cpp_type_ref=_parse_discovered_cpp_type(child.get("type", {}), index=index, current_scope=current_scope),
        )
    return fields


def _qualified_name(prefix: str, name: str) -> str:
    if not prefix:
        return name
    if not name:
        return prefix
    return f"{prefix}::{name}"


def _simple_name(name: str) -> str:
    return name.rsplit("::", 1)[-1]


def _enclosing_scope(current_scope: str) -> str:
    if "::" not in current_scope:
        return ""
    return current_scope.rsplit("::", 1)[0]


def _looks_like_named_type(text: str) -> bool:
    if text in _BUILTIN_TYPE_NAMES:
        return False
    return re.fullmatch(r"[A-Za-z_]\w*(?:::[A-Za-z_]\w*)*", text) is not None


def _scoped_lookup_candidates(name: str, current_scope: str) -> tuple[str, ...]:
    if "::" in name or not current_scope:
        return (name,)
    candidates: list[str] = []
    scope = current_scope
    while scope:
        candidates.append(f"{scope}::{name}")
        scope = scope.rsplit("::", 1)[0] if "::" in scope else ""
    candidates.append(name)
    return tuple(candidates)


def _strip_template_args(text: str) -> str:
    chars: list[str] = []
    depth = 0
    for char in text:
        if char == "<":
            depth += 1
            continue
        if char == ">":
            depth = max(0, depth - 1)
            continue
        if depth == 0:
            chars.append(char)
    return "".join(chars)


def _normalize_record_lookup_name(name: str) -> str:
    normalized = " ".join(name.replace(" &", "&").replace(" *", "*").split())
    while normalized.startswith("const "):
        normalized = normalized[len("const ") :].strip()
    for prefix in _TYPE_PREFIXES:
        if normalized.startswith(prefix):
            normalized = normalized[len(prefix) :].strip()
            break
    normalized = normalized.removesuffix("&").removesuffix("*").strip()
    normalized = _strip_template_args(normalized).strip()
    return normalized


def _base_record_lookup_names(record: dict) -> list[str]:
    base_names: list[str] = []
    for base in record.get("bases", []):
        type_info = base.get("type", {})
        raw_name = type_info.get("desugaredQualType") or type_info.get("qualType", "")
        lookup_name = _normalize_record_lookup_name(raw_name)
        if lookup_name:
            base_names.append(lookup_name)
    return base_names


def _extract_namespace_functions(
    node: dict,
    namespace_name: str,
    index: TranslationUnitIndex,
    current_namespace: str = "",
) -> dict[str, tuple[DiscoveredFunction, ...]]:
    functions: dict[str, list[DiscoveredFunction]] = defaultdict(list)

    kind = node.get("kind")
    name = node.get("name", "")
    next_namespace = current_namespace
    if kind == "NamespaceDecl":
        next_namespace = _qualified_name(current_namespace, name)

    if kind == "FunctionDecl" and current_namespace == namespace_name:
        params = tuple(
            DiscoveredParam(
                name=param.get("name") or f"arg_{param_index}",
                cpp_type=param.get("type", {}).get("qualType", ""),
                cpp_type_ref=_parse_discovered_cpp_type(param.get("type", {}), index=index, current_scope=current_namespace),
            )
            for param_index, param in enumerate(item for item in node.get("inner", []) if item.get("kind") == "ParmVarDecl")
        )
        return_cpp_type = node.get("type", {}).get("qualType", "").rsplit("(", 1)[0].strip()
        functions[name].append(
            DiscoveredFunction(
                namespace=current_namespace,
                cpp_name=name,
                return_cpp_type=return_cpp_type,
                return_type_ref=_parse_discovered_cpp_type(return_cpp_type, index=index, current_scope=current_namespace),
                params=params,
            )
        )

    for child in node.get("inner", []):
        for func_name, overloads in _extract_namespace_functions(child, namespace_name, index, next_namespace).items():
            functions[func_name].extend(overloads)

    return {func_name: tuple(overloads) for func_name, overloads in functions.items()}


def discover_public_methods_with_compile_commands(
    compile_commands_path: Path,
    translation_unit: Path,
    class_name: str,
    include_inherited: bool = False,
) -> dict[str, tuple[DiscoveredMethod, ...]]:
    index = _translation_unit_index(compile_commands_path, translation_unit)
    record = index.resolve_record(class_name)
    if record is None:
        msg = f"Class '{class_name}' not found in AST for '{translation_unit}'"
        raise ValueError(msg)

    methods = _extract_public_methods(record.node, index, record.qualified_name)

    if include_inherited:
        visited: set[str] = {record.qualified_name}
        queue = _base_record_lookup_names(record.node)
        while queue:
            base_name = queue.pop(0)
            base_record = index.resolve_record(base_name)
            if base_record is None or base_record.qualified_name in visited:
                continue
            visited.add(base_record.qualified_name)
            base_methods = _extract_public_methods(base_record.node, index, base_record.qualified_name)
            for method_name, overloads in base_methods.items():
                if method_name not in methods:
                    methods[method_name] = overloads
            queue.extend(_base_record_lookup_names(base_record.node))

    return methods


def discover_public_fields_with_compile_commands(
    compile_commands_path: Path,
    translation_unit: Path,
    class_name: str,
    include_inherited: bool = False,
) -> dict[str, DiscoveredField]:
    index = _translation_unit_index(compile_commands_path, translation_unit)
    record = index.resolve_record(class_name)
    if record is None:
        msg = f"Class '{class_name}' not found in AST for '{translation_unit}'"
        raise ValueError(msg)

    fields = _extract_public_fields(record.node, index, record.qualified_name)

    if include_inherited:
        visited: set[str] = {record.qualified_name}
        queue = _base_record_lookup_names(record.node)
        while queue:
            base_name = queue.pop(0)
            base_record = index.resolve_record(base_name)
            if base_record is None or base_record.qualified_name in visited:
                continue
            visited.add(base_record.qualified_name)
            base_fields = _extract_public_fields(base_record.node, index, base_record.qualified_name)
            for field_name, field_value in base_fields.items():
                if field_name not in fields:
                    fields[field_name] = field_value
            queue.extend(_base_record_lookup_names(base_record.node))

    return fields


def discover_namespace_functions_with_compile_commands(
    compile_commands_path: Path,
    translation_unit: Path,
    namespace_name: str,
) -> dict[str, tuple[DiscoveredFunction, ...]]:
    index = _translation_unit_index(compile_commands_path, translation_unit)
    return index.discover_namespace_functions(namespace_name)


def _normalize_cpp_type_text(text: str) -> str:
    return " ".join(text.replace(" &", "&").replace(" *", "*").split())


def _split_template_args(text: str) -> tuple[str, ...]:
    args: list[str] = []
    current: list[str] = []
    depth = 0
    for char in text:
        if char == "<":
            depth += 1
        elif char == ">":
            depth = max(0, depth - 1)
        elif char == "," and depth == 0:
            arg = "".join(current).strip()
            if arg:
                args.append(arg)
            current = []
            continue
        current.append(char)
    tail = "".join(current).strip()
    if tail:
        args.append(tail)
    return tuple(args)


def _parse_type_core(text: str) -> tuple[bool, int, bool, bool, str]:
    core = _normalize_cpp_type_text(text)
    is_const = False
    while core.startswith("const "):
        is_const = True
        core = core[len("const ") :].strip()

    pointer_depth = 0
    is_lvalue_reference = False
    is_rvalue_reference = False
    while True:
        if core.endswith("&&"):
            is_rvalue_reference = True
            core = core[:-2].strip()
            continue
        if core.endswith("&"):
            is_lvalue_reference = True
            core = core[:-1].strip()
            continue
        if core.endswith("*"):
            pointer_depth += 1
            core = core[:-1].strip()
            continue
        break
    return is_const, pointer_depth, is_lvalue_reference, is_rvalue_reference, core


def _rebuild_type_text(
    *,
    is_const: bool,
    pointer_depth: int,
    is_lvalue_reference: bool,
    is_rvalue_reference: bool,
    core: str,
) -> str:
    text = core.strip()
    if is_const:
        text = f"const {text}"
    text += "*" * pointer_depth
    if is_lvalue_reference:
        text += "&"
    if is_rvalue_reference:
        text += "&&"
    return text


def _parse_template_name_and_args(
    text: str,
    *,
    index: TranslationUnitIndex | None = None,
    current_scope: str = "",
) -> tuple[str | None, tuple[DiscoveredCppType, ...]]:
    if not text.endswith(">"):
        return None, ()
    start = text.find("<")
    if start == -1:
        return None, ()
    template_name = text[:start].strip()
    inner = text[start + 1 : -1].strip()
    if not inner:
        return template_name, ()
    return template_name, tuple(
        _parse_discovered_cpp_type(arg, index=index, current_scope=current_scope)
        for arg in _split_template_args(inner)
    )


def _qualified_type_core(text: str, *, index: TranslationUnitIndex | None = None, current_scope: str = "") -> str:
    template_name, template_args = _parse_template_name_and_args(text, index=index, current_scope=current_scope)
    if template_name is not None:
        if template_args:
            rendered_args = ", ".join(
                arg.storage_spelling
                for arg in template_args
            )
            return f"{template_name}<{rendered_args}>"
        return f"{template_name}<>"

    if _is_external_qualified_type(text):
        return text

    if index is not None:
        if _looks_like_named_type(text):
            record = index.resolve_record(text, current_scope=current_scope)
            if record is None:
                record = index._resolve_scoped_decl(  # noqa: SLF001
                    text,
                    current_scope=current_scope,
                    qualified=index._records_by_qualified,  # noqa: SLF001
                    simple=index._record_names_by_simple,  # noqa: SLF001
                )
            if record is not None:
                if "::" in text and "::" not in record.qualified_name:
                    return text
                if "::" not in record.qualified_name and "::" not in text:
                    enclosing_scope = _enclosing_scope(current_scope)
                    if enclosing_scope:
                        return f"{enclosing_scope}::{text}"
                return record.qualified_name

            enum = index._resolve_scoped_decl(  # noqa: SLF001
                text,
                current_scope=current_scope,
                qualified=index._enums_by_qualified,  # noqa: SLF001
                simple=index._enum_names_by_simple,  # noqa: SLF001
            )
            if enum is None:
                enum = index.resolve_enum(text, current_scope)
            if enum is not None:
                if "::" in text and "::" not in enum.qualified_name:
                    return text
                if "::" not in enum.qualified_name and "::" not in text:
                    enclosing_scope = _enclosing_scope(current_scope)
                    if enclosing_scope:
                        return f"{enclosing_scope}::{text}"
                return enum.qualified_name

            if "::" in text:
                root, suffix = text.split("::", 1)
                root_record = index.resolve_record(root, current_scope=current_scope)
                if root_record is None:
                    root_record = index._resolve_scoped_decl(  # noqa: SLF001
                        root,
                        current_scope=current_scope,
                        qualified=index._records_by_qualified,  # noqa: SLF001
                        simple=index._record_names_by_simple,  # noqa: SLF001
                    )
                if root_record is not None and "::" not in root:
                    qualified_root = root_record.qualified_name
                    if "::" not in qualified_root:
                        enclosing_scope = _enclosing_scope(current_scope)
                        if enclosing_scope:
                            qualified_root = f"{enclosing_scope}::{root}"
                    if "::" in qualified_root:
                        return f"{qualified_root}::{suffix}"

    if "::" not in text and _looks_like_named_type(text):
        enclosing_scope = _enclosing_scope(current_scope)
        if enclosing_scope:
            return f"{enclosing_scope}::{text}"
    return text


def _resolved_enum(index: TranslationUnitIndex | None, text: str, current_scope: str) -> IndexedEnum | None:
    if index is None or not _looks_like_named_type(text) or _is_external_qualified_type(text):
        return None
    enum = index._resolve_scoped_decl(  # noqa: SLF001
        text,
        current_scope=current_scope,
        qualified=index._enums_by_qualified,  # noqa: SLF001
        simple=index._enum_names_by_simple,  # noqa: SLF001
    )
    if enum is not None:
        if "::" in text and "::" not in enum.qualified_name:
            return IndexedEnum(
                qualified_name=text,
                simple_name=enum.simple_name,
                node=enum.node,
            )
        if "::" not in enum.qualified_name and "::" not in text:
            enclosing_scope = _enclosing_scope(current_scope)
            if enclosing_scope:
                return IndexedEnum(
                    qualified_name=f"{enclosing_scope}::{text}",
                    simple_name=enum.simple_name,
                    node=enum.node,
                )
        return enum
    return index.resolve_enum(text, current_scope)


def _parse_discovered_cpp_type(
    raw: dict | str,
    *,
    index: TranslationUnitIndex | None = None,
    current_scope: str = "",
) -> DiscoveredCppType:
    if isinstance(raw, dict):
        spelling = raw.get("qualType", "")
        desugared_spelling = raw.get("desugaredQualType")
    else:
        spelling = raw
        desugared_spelling = None

    normalized_spelling = _normalize_cpp_type_text(spelling)
    normalized_desugared = _normalize_cpp_type_text(desugared_spelling) if desugared_spelling else None
    canonical_input = normalized_desugared or normalized_spelling
    is_const, pointer_depth, is_lvalue_reference, is_rvalue_reference, core = _parse_type_core(canonical_input)
    template_name, template_args = _parse_template_name_and_args(core, index=index, current_scope=current_scope)
    base_name = template_name or core
    canonical = _rebuild_type_text(
        is_const=is_const,
        pointer_depth=pointer_depth,
        is_lvalue_reference=is_lvalue_reference,
        is_rvalue_reference=is_rvalue_reference,
        core=base_name,
    )
    storage_core = _qualified_type_core(core, index=index, current_scope=current_scope)
    storage_spelling = _rebuild_type_text(
        is_const=is_const,
        pointer_depth=pointer_depth,
        is_lvalue_reference=is_lvalue_reference,
        is_rvalue_reference=is_rvalue_reference,
        core=storage_core,
    )
    resolved_record = None
    if index is not None and _looks_like_named_type(base_name) and not _is_external_qualified_type(base_name):
        resolved_record = index._resolve_scoped_decl(  # noqa: SLF001
            base_name,
            current_scope=current_scope,
            qualified=index._records_by_qualified,  # noqa: SLF001
            simple=index._record_names_by_simple,  # noqa: SLF001
        )
    resolved_enum = None if resolved_record is not None else _resolved_enum(index, base_name, current_scope)
    is_enum = resolved_enum is not None
    return DiscoveredCppType(
        spelling=spelling,
        desugared_spelling=desugared_spelling,
        normalized_spelling=normalized_spelling,
        normalized_desugared_spelling=normalized_desugared,
        canonical_spelling=canonical,
        storage_spelling=storage_spelling,
        base_name=base_name,
        base_record_names=tuple(_base_record_lookup_names(resolved_record.node)) if resolved_record is not None else (),
        template_name=template_name,
        template_args=template_args,
        is_enum=is_enum,
        enum_qualified_name=resolved_enum.qualified_name if resolved_enum is not None else None,
        is_const=is_const,
        pointer_depth=pointer_depth,
        is_lvalue_reference=is_lvalue_reference,
        is_rvalue_reference=is_rvalue_reference,
    )
