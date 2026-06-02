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
    from .policy_ir import DirectFunctionPolicyOp, SpecMethodFunctionPolicyOp
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
    from policy_ir import DirectFunctionPolicyOp, SpecMethodFunctionPolicyOp


@dataclass(frozen=True)
class CppSpecFunction:
    name: str
    namespace: str
    discovered: DiscoveredFunction
    return_annotations: frozenset[str]
    param_annotations: dict[str, frozenset[str]]
    receiver: str | None = None


@dataclass(frozen=True)
class CppSpecHandle:
    name: str
    cpp_type: str
    c_type: str
    destructor: str
    ptr_type: str = "raw"
    empty_check: str | None = None


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


def _strip_string_literal(value: str) -> str:
    value = value.strip()
    if len(value) >= 2 and value[0] == value[-1] and value[0] in {'"', "'"}:
        return value[1:-1]
    return value


def _find_macro_invocations(text: str, marker: str) -> tuple[tuple[str, int], ...]:
    pattern = re.compile(rf"\b{re.escape(marker)}\s*\(")
    invocations: list[tuple[str, int]] = []
    for match in pattern.finditer(text):
        line_start = text.rfind("\n", 0, match.start()) + 1
        if text[line_start:match.start()].lstrip().startswith("#"):
            continue
        start = match.end()
        depth = 1
        quote: str | None = None
        escaped = False
        index = start
        while index < len(text):
            char = text[index]
            if quote is not None:
                if escaped:
                    escaped = False
                elif char == "\\":
                    escaped = True
                elif char == quote:
                    quote = None
            elif char in {'"', "'"}:
                quote = char
            elif char == "(":
                depth += 1
            elif char == ")":
                depth -= 1
                if depth == 0:
                    invocations.append((text[start:index], index + 1))
                    break
            index += 1
    return tuple(invocations)


def discover_cpp_spec_handles(
    translation_unit: Path,
    *,
    marker: str = "IFCAPI_HANDLE",
    c_prefix: str | None = None,
) -> tuple[CppSpecHandle, ...]:
    """Discover explicitly declared C ABI handles from a C++ binding spec translation unit."""
    text = translation_unit.read_text(encoding="utf-8")
    handles: list[CppSpecHandle] = []
    seen_c_types: set[str] = set()
    for raw_args, end in _find_macro_invocations(text, marker):
        args = _split_macro_args(raw_args)
        if len(args) not in {2, 3, 4, 5}:
            msg = f"{marker} expects cpp_type/destructor, optional handle name, optional ptr_type, and optional empty_check"
            raise ValueError(msg)
        if len(args) >= 3 and re.fullmatch(r"[A-Za-z_]\w*", args[0]):
            handle_name = args[0]
            cpp_type = args[1]
            destructor = args[2]
            ptr_type = args[3] if len(args) >= 4 else "raw"
            empty_check = _strip_string_literal(args[4]) if len(args) == 5 else None
            c_type_match = re.match(
                r"\s*(?:struct|class)\s+(?P<c_type>ifcopenshell_[A-Za-z0-9_]+_t)\s*;",
                text[end:],
                re.DOTALL,
            )
            if c_prefix is not None:
                c_type = c_type_match.group("c_type") if c_type_match is not None else f"{c_prefix}_{handle_name}_t"
            elif c_type_match is not None:
                c_type = c_type_match.group("c_type")
            else:
                msg = f"{marker} must be followed by an ifcopenshell_*_t struct/class declaration"
                raise ValueError(msg)
        else:
            c_type_match = re.match(
                r"\s*(?:struct|class)\s+(?P<c_type>ifcopenshell_[A-Za-z0-9_]+_t)\s*;",
                text[end:],
                re.DOTALL,
            )
            if c_type_match is None:
                msg = f"{marker} must be followed by an ifcopenshell_*_t struct/class declaration"
                raise ValueError(msg)
            c_type = c_type_match.group("c_type")
            handle_name = _handle_name_from_c_type(c_type)
            cpp_type = args[0]
            destructor = args[1]
            ptr_type = args[2] if len(args) >= 3 else "raw"
            empty_check = _strip_string_literal(args[3]) if len(args) == 4 else None
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
                empty_check=empty_check,
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


CppSpecSignature = tuple[frozenset[str], dict[str, frozenset[str]], str | None, str | None]


def _param_type_decl(param: str) -> str:
    name = _param_name(param)
    return param[: param.rfind(name)].strip() if name else param.strip()


_PRIVATE_NAMES = frozenset({"to_base_vector"})

def _discover_spec_signatures(
    source: Path,
    selected_names: set[str],
) -> dict[str, tuple[CppSpecSignature, ...]]:
    text = _strip_comments(source.read_text(encoding="utf-8"))
    signatures: dict[str, list[CppSpecSignature]] = {}
    for name in selected_names:
        if name in _PRIVATE_NAMES:
            continue
        signature_re = re.compile(
            r"(?P<annotations>(?:IFCAPI_\w+(?:\([^)]*\))?\s+)*)"
            r"(?P<return_decl>[\w:<>~,\s*&()]+?)\s+"
            + re.escape(name) +
            r"\s*\("
            r"(?P<params>[^;{}]*)\)\s*\{",
            re.DOTALL,
        )
        for match in signature_re.finditer(text):
            return_decl = f"{match.group('annotations')}{match.group('return_decl')}"
            return_decl = re.sub(r"^\s*inline\s+", "", return_decl.strip())
            receiver, return_decl = _method_receiver_from_return_decl(return_decl)
            if name in signatures and (receiver is None or any(item[2] == receiver for item in signatures[name])):
                msg = f"C++ spec export '{name}' is declared more than once; exported spec functions must be unique"
                raise ValueError(msg)
            return_annotations, _ = _leading_annotations(return_decl)
            param_annotations: dict[str, frozenset[str]] = {}
            first_param_type = None
            for param in _split_params(match.group("params")):
                annotations, rest = _leading_annotations(param)
                if first_param_type is None:
                    first_param_type = _param_type_decl(rest)
                if annotations:
                    param_annotations[_param_name(rest)] = annotations
            signatures.setdefault(name, []).append((return_annotations, param_annotations, receiver, first_param_type))
    return {name: tuple(entries) for name, entries in signatures.items()}


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
    contract_headers: tuple[Path, ...] = (),
) -> tuple[CppSpecFunction, ...]:
    discovered = discover_namespace_functions(
        environment,
        translation_unit,
        namespace,
    )

    spec_sigs = _discover_spec_signatures(translation_unit, set(discovered.keys()))

    combined = dict(spec_sigs)
    for function in discover_marked_functions_in_headers(contract_headers):
        if function.name in combined:
            msg = (
                f"C++ spec export '{function.name}' is declared more than once; "
                "exported spec functions must be unique"
            )
            raise ValueError(msg)
        combined[function.name] = ((function.return_annotations, function.param_annotations, None, None),)
    if not combined:
        msg = f"No exported functions were found in C++ spec '{translation_unit}'"
        raise ValueError(msg)

    result: list[CppSpecFunction] = []
    for name in sorted(combined):
        overloads = discovered.get(name, ())
        if not overloads:
            msg = f"C++ spec export '{name}' was found in spec but not discovered by Clang"
            raise ValueError(msg)
        for return_annotations, param_annotations, receiver, first_param_type in combined[name]:
            selected_overloads = overloads
            if len(overloads) != 1 and first_param_type is not None:
                selected_overloads = tuple(
                    overload
                    for overload in overloads
                    if overload.params
                    and _canonical_cpp_type(overload.params[0].cpp_type_ref) == _canonical_cpp_type(first_param_type)
                )
            if len(selected_overloads) != 1:
                msg = f"C++ spec export '{name}' has {len(overloads)} overloads; exported spec functions must be unique"
                raise ValueError(msg)
            result.append(
                CppSpecFunction(
                    name=name,
                    namespace=namespace,
                    discovered=selected_overloads[0],
                    return_annotations=return_annotations,
                    param_annotations=param_annotations,
                    receiver=receiver,
                )
            )
    return tuple(result)


def _method_receiver_from_return_decl(return_decl: str) -> tuple[str | None, str]:
    match = re.match(r"\s*IFCAPI_METHOD\s*\((?P<receiver>[A-Za-z_]\w*)\)\s*(?P<rest>.*)", return_decl, re.DOTALL)
    if match is None:
        return None, return_decl
    return match.group("receiver"), match.group("rest")


def _receiver_c_name(handle: HandleSpec, expose_as: str) -> str:
    receiver = handle.c_type.removeprefix("ifcopenshell_").removesuffix("_t")
    return f"ifcopenshell_{receiver}_{expose_as}"


def _canonical_cpp_type(cpp_type: object) -> str:
    if not isinstance(cpp_type, str):
        cpp_type = (
            getattr(cpp_type, "normalized_spelling", None)
            or getattr(cpp_type, "spelling", None)
            or str(cpp_type)
        )
    normalized = " ".join(cpp_type.replace(" *", "*").replace(" &", "&").split())
    while normalized.startswith("const "):
        normalized = normalized[len("const ") :].strip()
    normalized = normalized.removesuffix("&").removesuffix("*").strip()
    return normalized.split("::")[-1]


def _apply_type_annotations(type_spec: TypeSpec, annotations: frozenset[str]) -> TypeSpec:
    ownership = type_spec.ownership
    nullable = type_spec.nullable
    if "IFCAPI_OWNED" in annotations:
        ownership = "owned"
    elif "IFCAPI_COPY" in annotations:
        ownership = "copy"
    elif "IFCAPI_STATIC" in annotations:
        ownership = "static"
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


def _handle_annotation(annotations: frozenset[str], macro: str) -> str | None:
    matches = [
        annotation
        for annotation in annotations
        if annotation.startswith(f"{macro}(") and annotation.endswith(")")
    ]
    if not matches:
        return None
    if len(matches) != 1:
        msg = f"C++ spec declaration has more than one {macro} annotation"
        raise ValueError(msg)
    handle = matches[0][len(f"{macro}(") : -1].strip()
    if not re.fullmatch(r"[A-Za-z_]\w*", handle):
        msg = f"Invalid {macro} handle name: {handle!r}"
        raise ValueError(msg)
    return handle


def _with_handle_override(type_spec: TypeSpec, handle_name: str, handles: dict[str, HandleSpec], context: str) -> TypeSpec:
    if handle_name not in handles:
        msg = f"C++ spec {context} declares unknown handle override '{handle_name}'"
        raise ValueError(msg)
    return TypeSpec(
        kind="handle",
        handle=handle_name,
        ownership=type_spec.ownership,
        nullable=type_spec.nullable,
        cpp_type=type_spec.cpp_type,
    )


def _apply_param_annotations(
    type_spec: TypeSpec,
    annotations: frozenset[str],
    handles: dict[str, HandleSpec],
) -> TypeSpec:
    annotated = _apply_type_annotations(type_spec, annotations)
    handle_name = _handle_annotation(annotations, "IFCAPI_HANDLE_PARAM")
    if handle_name is None:
        return annotated
    return _with_handle_override(annotated, handle_name, handles, "parameter")


def _apply_return_annotations(
    type_spec: TypeSpec,
    annotations: frozenset[str],
    handles: dict[str, HandleSpec],
) -> TypeSpec:
    annotated = _apply_type_annotations(type_spec, annotations)
    if "IFCAPI_DOUBLE_BUFFER" in annotations:
        annotated = TypeSpec(
            kind="double_buffer",
            ownership=annotated.ownership,
            nullable=annotated.nullable,
            cpp_type=annotated.cpp_type,
        )
    if "IFCAPI_INT32_BUFFER" in annotations:
        annotated = TypeSpec(
            kind="int32_buffer",
            ownership=annotated.ownership,
            nullable=annotated.nullable,
            cpp_type=annotated.cpp_type,
        )
    handle_name = _handle_annotation(annotations, "IFCAPI_HANDLE_RESULT")
    if handle_name is None:
        return annotated
    return _with_handle_override(annotated, handle_name, handles, "return")


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
        returns = _apply_return_annotations(
            _infer_return_type(discovered.return_type_ref, handles, result_structs),
            function.return_annotations,
            handles,
        )
        discovered_params = discovered.params
        receiver_cpp_type = None
        if function.receiver is not None:
            if function.receiver not in handles:
                msg = f"C++ spec export '{function.name}' declares unknown receiver handle '{function.receiver}'"
                raise ValueError(msg)
            if not discovered_params:
                msg = f"C++ spec method export '{function.name}' must declare an explicit receiver parameter"
                raise ValueError(msg)
            receiver_param = discovered_params[0]
            receiver_cpp_type = receiver_param.cpp_type_ref.normalized_spelling or receiver_param.cpp_type_ref.spelling
            handle_cpp_type = handles[function.receiver].cpp_type
            if _canonical_cpp_type(receiver_cpp_type) != _canonical_cpp_type(handle_cpp_type):
                msg = (
                    f"C++ spec method export '{function.name}' receiver parameter type "
                    f"'{receiver_cpp_type}' does not match handle '{function.receiver}' type '{handle_cpp_type}'"
                )
                raise ValueError(msg)
            discovered_params = discovered_params[1:]
        params = []
        for param in discovered_params:
            params.append(
                ParamSpec(
                    name=param.name,
                    type=_apply_param_annotations(
                        _infer_param_type(param.cpp_type_ref, handles),
                        function.param_annotations.get(param.name, frozenset()),
                        handles,
                    ),
                )
            )
        cpp_name = f"{function.namespace}::{discovered.cpp_name}"
        calls.append(
            CallSpec(
                expose_as=function.name,
                c_name=(
                    _receiver_c_name(handles[function.receiver], function.name)
                    if function.receiver is not None
                    else f"{c_prefix}_{function.name}" if c_prefix else function.name
                ),
                receiver=function.receiver,
                returns=returns,
                params=tuple(params),
                policy_operation=(
                    SpecMethodFunctionPolicyOp(cpp_name=cpp_name, receiver_cpp_type=receiver_cpp_type)
                    if receiver_cpp_type is not None
                    else DirectFunctionPolicyOp(cpp_name=cpp_name)
                ),
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
            empty_check=handle.empty_check,
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
