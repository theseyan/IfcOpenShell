# This file was generated with the assistance of an AI coding tool.

from __future__ import annotations

from dataclasses import dataclass
from pathlib import Path
import re

try:
    from .authored_spec import _infer_param_type, _infer_return_type
    from .clang_discovery import DiscoveredFunction, DiscoveryEnvironment, discover_namespace_functions
    from .contract_discovery import (
        discover_marked_functions_in_headers,
        _leading_annotations,
        _param_name,
        _split_params,
        _strip_comments,
    )
    from .binding_model import CallSpec, HandleSpec, ParamSpec, ResultStructFieldSpec, ResultStructSpec, TypeSpec
    from .policy_ir import DirectFunctionPolicyOp
except ImportError:  # pragma: no cover - script execution fallback
    from authored_spec import _infer_param_type, _infer_return_type
    from clang_discovery import DiscoveredFunction, DiscoveryEnvironment, discover_namespace_functions
    from contract_discovery import (
        discover_marked_functions_in_headers,
        _leading_annotations,
        _param_name,
        _split_params,
        _strip_comments,
    )
    from binding_model import CallSpec, HandleSpec, ParamSpec, ResultStructFieldSpec, ResultStructSpec, TypeSpec
    from policy_ir import DirectFunctionPolicyOp


@dataclass(frozen=True)
class CppSpecFunction:
    name: str
    namespace: str
    discovered: DiscoveredFunction
    return_annotations: frozenset[str]
    param_annotations: dict[str, frozenset[str]]


@dataclass(frozen=True)
class CppSpecHandle:
    name: str
    cpp_type: str
    c_type: str
    destructor: str
    ptr_type: str = "raw"


@dataclass(frozen=True)
class CppSpecResultStruct:
    name: str
    cpp_type: str
    c_type: str
    fields: tuple[tuple[str, str], ...]


def _split_macro_args(args: str) -> tuple[str, ...]:
    result: list[str] = []
    start = 0
    angle_depth = paren_depth = bracket_depth = 0
    for index, char in enumerate(args):
        if char == "<":
            angle_depth += 1
        elif char == ">" and angle_depth:
            angle_depth -= 1
        elif char == "(":
            paren_depth += 1
        elif char == ")" and paren_depth:
            paren_depth -= 1
        elif char == "[":
            bracket_depth += 1
        elif char == "]" and bracket_depth:
            bracket_depth -= 1
        elif char == "," and not angle_depth and not paren_depth and not bracket_depth:
            result.append(args[start:index].strip())
            start = index + 1
    result.append(args[start:].strip())
    return tuple(item for item in result if item)


def _handle_name_from_c_type(c_type: str) -> str:
    return c_type.removeprefix("ifcopenshell_").removesuffix("_t")


def discover_cpp_spec_handles(
    translation_unit: Path,
    *,
    marker: str = "IFCAPI_HANDLE",
) -> tuple[CppSpecHandle, ...]:
    """Discover explicitly declared C ABI handles from a C++ binding spec translation unit."""
    text = _strip_comments(translation_unit.read_text(encoding="utf-8"))
    marker_re = re.escape(marker)
    handle_re = re.compile(
        rf"\b{marker_re}\s*\((?P<args>[^)]*)\)\s*"
        r"(?:struct|class)\s+(?P<c_type>ifcopenshell_[A-Za-z0-9_]+_t)\s*;",
        re.DOTALL,
    )
    handles: list[CppSpecHandle] = []
    seen_c_types: set[str] = set()
    for match in handle_re.finditer(text):
        args = _split_macro_args(match.group("args"))
        c_type = match.group("c_type")
        if len(args) not in {2, 3, 4}:
            msg = f"{marker} expects cpp_type/destructor, optional handle name, and optional ptr_type"
            raise ValueError(msg)
        if len(args) >= 3 and re.fullmatch(r"[A-Za-z_]\w*", args[0]) and args[2] not in {"raw", "shared_ptr", "value"}:
            handle_name = args[0]
            cpp_type = args[1]
            destructor = args[2]
            ptr_type = args[3] if len(args) == 4 else "raw"
        else:
            handle_name = _handle_name_from_c_type(c_type)
            cpp_type = args[0]
            destructor = args[1]
            ptr_type = args[2] if len(args) == 3 else "raw"
        if c_type in seen_c_types:
            msg = f"C++ spec handle '{c_type}' is declared more than once"
            raise ValueError(msg)
        seen_c_types.add(c_type)
        handles.append(
            CppSpecHandle(
                name=handle_name,
                cpp_type=cpp_type,
                c_type=c_type,
                destructor=destructor,
                ptr_type=ptr_type,
            )
        )
    return tuple(handles)


def discover_cpp_spec_result_structs(
    translation_unit: Path,
    namespace: str,
    *,
    marker: str = "IFCAPI_RESULT_STRUCT",
) -> tuple[CppSpecResultStruct, ...]:
    """Discover explicitly declared generated result structs from a C++ spec translation unit."""
    text = _strip_comments(translation_unit.read_text(encoding="utf-8"))
    marker_re = re.escape(marker)
    struct_re = re.compile(
        rf"\b{marker_re}\s*\((?P<args>[^)]*)\)\s*"
        r"struct\s+(?P<name>[A-Za-z_]\w*)\s*\{(?P<body>.*?)\}\s*;",
        re.DOTALL,
    )
    result_structs: list[CppSpecResultStruct] = []
    seen_c_types: set[str] = set()
    for match in struct_re.finditer(text):
        args = _split_macro_args(match.group("args"))
        if len(args) not in {1, 2}:
            msg = f"{marker} expects c_type/cpp_type and optional cpp_type"
            raise ValueError(msg)
        name = match.group("name")
        if re.fullmatch(r"ifcopenshell_[A-Za-z0-9_]+_t", name):
            c_type = name
            cpp_type = args[0]
            if len(args) == 2:
                msg = f"{marker} with an ifcopenshell_*_t struct name expects only cpp_type"
                raise ValueError(msg)
        else:
            c_type = args[0]
            cpp_type = args[1] if len(args) == 2 else f"{namespace}::{name}"
        if not re.fullmatch(r"ifcopenshell_[A-Za-z0-9_]+_t", c_type):
            msg = f"{marker} c_type must be an ifcopenshell_*_t type"
            raise ValueError(msg)
        if c_type in seen_c_types:
            msg = f"C++ spec result struct '{c_type}' is declared more than once"
            raise ValueError(msg)
        seen_c_types.add(c_type)
        fields: list[tuple[str, str]] = []
        for raw_field in match.group("body").split(";"):
            field = raw_field.strip()
            if not field:
                continue
            field_match = re.match(r"(?P<type>.+?)\s+(?P<name>[A-Za-z_]\w*)$", field, re.DOTALL)
            if field_match is None:
                msg = f"Unable to parse result struct field declaration: {field!r}"
                raise ValueError(msg)
            fields.append((" ".join(field_match.group("type").split()), field_match.group("name")))
        result_structs.append(
            CppSpecResultStruct(
                name=name,
                cpp_type=cpp_type,
                c_type=c_type,
                fields=tuple(fields),
            )
        )
    return tuple(result_structs)


def _discover_marked_spec_signatures(
    source: Path,
    marker: str,
) -> dict[str, tuple[frozenset[str], dict[str, frozenset[str]]]]:
    text = _strip_comments(source.read_text(encoding="utf-8"))
    marker_re = re.escape(marker)
    signature_re = re.compile(
        rf"\b{marker_re}\s+"
        r"(?P<return_decl>[\w:<>~,\s*&]+?)\s+"
        r"(?P<name>[A-Za-z_]\w*)\s*\("
        r"(?P<params>[^;{}]*)\)\s*(?:[;{])",
        re.DOTALL,
    )
    signatures: dict[str, tuple[frozenset[str], dict[str, frozenset[str]]]] = {}
    for match in signature_re.finditer(text):
        name = match.group("name")
        if name in signatures:
            msg = f"C++ spec export '{name}' is declared more than once; exported spec functions must be unique"
            raise ValueError(msg)
        return_annotations, _ = _leading_annotations(match.group("return_decl"))
        param_annotations: dict[str, frozenset[str]] = {}
        for param in _split_params(match.group("params")):
            annotations, rest = _leading_annotations(param)
            if annotations:
                param_annotations[_param_name(rest)] = annotations
        signatures[name] = (return_annotations, param_annotations)
    return signatures


def discover_cpp_spec_contract_headers(
    translation_unit: Path,
    include_dirs: tuple[Path, ...],
) -> tuple[Path, ...]:
    text = _strip_comments(translation_unit.read_text(encoding="utf-8"))
    headers: list[Path] = []
    seen: set[Path] = set()
    for match in re.finditer(r'^\s*#\s*include\s+"(?P<header>[^"]+)"', text, re.MULTILINE):
        header = match.group("header")
        candidates = (translation_unit.parent / header, *(include_dir / header for include_dir in include_dirs))
        for candidate in candidates:
            if not candidate.exists():
                continue
            resolved = candidate.resolve()
            if resolved in seen:
                break
            if discover_marked_functions_in_headers([resolved]):
                headers.append(resolved)
                seen.add(resolved)
            break
    return tuple(headers)


def discover_cpp_spec_functions(
    environment: DiscoveryEnvironment,
    translation_unit: Path,
    namespace: str,
    *,
    marker: str = "IFCAPI_EXPORT",
    contract_headers: tuple[Path, ...] = (),
) -> tuple[CppSpecFunction, ...]:
    """Discover explicitly exported functions from a C++ binding spec translation unit."""
    marked = _discover_marked_spec_signatures(translation_unit, marker)
    for function in discover_marked_functions_in_headers(contract_headers):
        if function.name in marked:
            msg = (
                f"C++ spec export '{function.name}' is declared more than once; "
                "exported spec functions must be unique"
            )
            raise ValueError(msg)
        marked[function.name] = (function.return_annotations, function.param_annotations)
    if not marked:
        msg = f"No {marker} functions were found in C++ spec '{translation_unit}'"
        raise ValueError(msg)

    discovered = discover_namespace_functions(
        environment,
        translation_unit,
        namespace,
        selected_names=marked.keys(),
    )
    result: list[CppSpecFunction] = []
    for name in sorted(marked):
        overloads = discovered.get(name, ())
        if not overloads:
            msg = f"C++ spec export '{name}' was marked but not discovered by Clang"
            raise ValueError(msg)
        if len(overloads) != 1:
            msg = f"C++ spec export '{name}' has {len(overloads)} overloads; exported spec functions must be unique"
            raise ValueError(msg)
        return_annotations, param_annotations = marked[name]
        result.append(
            CppSpecFunction(
                name=name,
                namespace=namespace,
                discovered=overloads[0],
                return_annotations=return_annotations,
                param_annotations=param_annotations,
            )
        )
    return tuple(result)


def _apply_type_annotations(type_spec: TypeSpec, annotations: frozenset[str]) -> TypeSpec:
    ownership = type_spec.ownership
    nullable = type_spec.nullable
    if "IFCAPI_OWNED" in annotations:
        ownership = "owned"
    elif "IFCAPI_COPY" in annotations:
        ownership = "copy"
    if "IFCAPI_NULLABLE" in annotations:
        nullable = True
    if ownership == type_spec.ownership and nullable == type_spec.nullable:
        return type_spec
    return TypeSpec(
        kind=type_spec.kind,
        handle=type_spec.handle,
        struct=type_spec.struct,
        ownership=ownership,
        nullable=nullable,
        cpp_type=type_spec.cpp_type,
        sequence_depth=type_spec.sequence_depth,
    )


def lower_cpp_spec_functions_to_calls(
    functions: tuple[CppSpecFunction, ...],
    handles: dict[str, HandleSpec],
    result_structs: dict[str, ResultStructSpec] | None = None,
    c_prefix: str | None = None,
) -> tuple[CallSpec, ...]:
    """Lower discovered C++ spec functions to the existing authored call model."""
    result_structs = result_structs or {}
    calls: list[CallSpec] = []
    for function in functions:
        discovered = function.discovered
        returns = _apply_type_annotations(
            _infer_return_type(discovered.return_type_ref, handles, result_structs),
            function.return_annotations,
        )
        params = []
        for param in discovered.params:
            params.append(
                ParamSpec(
                    name=param.name,
                    type=_apply_type_annotations(
                        _infer_param_type(param.cpp_type_ref, handles),
                        function.param_annotations.get(param.name, frozenset()),
                    ),
                )
            )
        calls.append(
            CallSpec(
                expose_as=function.name,
                c_name=f"{c_prefix}_{function.name}" if c_prefix else function.name,
                receiver=None,
                returns=returns,
                params=tuple(params),
                policy_operation=DirectFunctionPolicyOp(cpp_name=f"{function.namespace}::{discovered.cpp_name}"),
            )
        )
    return tuple(calls)


def lower_cpp_spec_handles_to_specs(handles: tuple[CppSpecHandle, ...]) -> dict[str, HandleSpec]:
    return {
        handle.name: HandleSpec(
            name=handle.name,
            cpp_type=handle.cpp_type,
            c_type=handle.c_type,
            destructor=handle.destructor,
            ptr_type=handle.ptr_type,
        )
        for handle in handles
    }


def lower_cpp_spec_result_structs_to_specs(
    structs: tuple[CppSpecResultStruct, ...],
    handles: dict[str, HandleSpec],
) -> dict[str, ResultStructSpec]:
    result: dict[str, ResultStructSpec] = {}
    for struct in structs:
        fields = tuple(
            ResultStructFieldSpec(
                name=field_name,
                type=_infer_return_type(field_cpp_type, handles),
                cpp_field=field_name,
            )
            for field_cpp_type, field_name in struct.fields
        )
        result[struct.name] = ResultStructSpec(
            name=struct.name,
            cpp_type=struct.cpp_type,
            c_type=struct.c_type,
            fields=fields,
        )
    return result
