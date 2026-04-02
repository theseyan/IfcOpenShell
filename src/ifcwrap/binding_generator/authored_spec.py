# This file was generated with the assistance of an AI coding tool.

from __future__ import annotations

from dataclasses import dataclass
from pathlib import Path
import re
from typing import Any

import yaml

try:
    from .clang_discovery import (
        DiscoveredFunction,
        DiscoveredMethod,
        discover_namespace_functions_with_compile_commands,
        discover_public_methods_with_compile_commands,
    )
except ImportError:  # pragma: no cover - script execution fallback
    from clang_discovery import (
        DiscoveredFunction,
        DiscoveredMethod,
        discover_namespace_functions_with_compile_commands,
        discover_public_methods_with_compile_commands,
    )


_ALLOWED_TYPE_KINDS = {
    "void",
    "bool",
    "bool_list",
    "int32",
    "double",
    "double_list",
    "double_list_list",
    "uint32",
    "size",
    "string",
    "string_list",
    "int32_list",
    "int32_list_list",
    "uint32_list",
    "handle",
    "handle_list",
}
_ALLOWED_OWNERSHIP = {"owned", "borrowed", "static", "copy"}
_ALLOWED_DESTRUCTORS = {"delete", "none"}
_ALLOWED_CALL_KINDS = {"function", "adapter_function", "method", "adapter_method"}
_ALLOWED_IMPLEMENTATION_KINDS = {"inline_cpp"}


@dataclass(frozen=True)
class TypeSpec:
    kind: str
    handle: str | None = None
    ownership: str | None = None
    nullable: bool = False
    cpp_type: str | None = None


@dataclass(frozen=True)
class ParamSpec:
    name: str
    type: TypeSpec


@dataclass(frozen=True)
class HandleSpec:
    name: str
    cpp_type: str
    c_type: str
    destructor: str


@dataclass(frozen=True)
class ImplementationSpec:
    kind: str
    body: str


@dataclass(frozen=True)
class CallSpec:
    kind: str
    expose_as: str
    c_name: str
    receiver: str | None
    cpp_name: str | None
    returns: TypeSpec
    params: tuple[ParamSpec, ...]
    implementation: ImplementationSpec | None


@dataclass(frozen=True)
class DiscoveryClassSpec:
    handle: str
    translation_unit: str
    include_all: bool
    include: tuple[str, ...]
    exclude: tuple[str, ...]
    rename: dict[str, str]
    overloads: tuple["DiscoveryOverloadSpec", ...]


@dataclass(frozen=True)
class DiscoverySpec:
    include_dir: Path
    classes: tuple[DiscoveryClassSpec, ...]
    functions: tuple["DiscoveryFunctionSpec", ...]


@dataclass(frozen=True)
class DiscoveryFunctionSpec:
    namespace: str
    translation_unit: str
    include_all: bool
    include: tuple[str, ...]
    exclude: tuple[str, ...]
    rename: dict[str, str]
    overloads: tuple["DiscoveryOverloadSpec", ...]


@dataclass(frozen=True)
class DiscoveryOverloadSpec:
    cpp_name: str
    expose_as: str
    params: tuple[str, ...]


@dataclass(frozen=True)
class AuthoredBindingSpec:
    schema_version: int
    module: str
    slice: str
    c_prefix: str
    public_headers: tuple[str, ...]
    handles: dict[str, HandleSpec]
    discovery: DiscoverySpec | None
    functions: tuple[CallSpec, ...]
    methods: tuple[CallSpec, ...]


def _expect_mapping(value: Any, context: str) -> dict[str, Any]:
    if not isinstance(value, dict):
        msg = f"{context} must be a mapping"
        raise ValueError(msg)
    return value


def _expect_list(value: Any, context: str) -> list[Any]:
    if not isinstance(value, list):
        msg = f"{context} must be a list"
        raise ValueError(msg)
    return value


def _expect_str(value: Any, context: str) -> str:
    if not isinstance(value, str) or not value:
        msg = f"{context} must be a non-empty string"
        raise ValueError(msg)
    return value


def _expect_int(value: Any, context: str) -> int:
    if not isinstance(value, int):
        msg = f"{context} must be an integer"
        raise ValueError(msg)
    return value


def _parse_type(raw: Any, *, context: str, known_handles: set[str]) -> TypeSpec:
    mapping = _expect_mapping(raw, context)
    kind = _expect_str(mapping.get("kind"), f"{context}.kind")
    if kind not in _ALLOWED_TYPE_KINDS:
        msg = f"{context}.kind must be one of {sorted(_ALLOWED_TYPE_KINDS)}"
        raise ValueError(msg)

    handle = mapping.get("handle")
    if kind in {"handle", "handle_list"}:
        handle = _expect_str(handle, f"{context}.handle")
        if handle not in known_handles:
            msg = f"{context}.handle refers to unknown handle '{handle}'"
            raise ValueError(msg)
    elif handle is not None:
        msg = f"{context}.handle is only valid for kind=handle or kind=handle_list"
        raise ValueError(msg)

    ownership = mapping.get("ownership")
    if ownership is not None:
        ownership = _expect_str(ownership, f"{context}.ownership")
        if ownership not in _ALLOWED_OWNERSHIP:
            msg = f"{context}.ownership must be one of {sorted(_ALLOWED_OWNERSHIP)}"
            raise ValueError(msg)
    if kind == "handle" and ownership is None:
        msg = f"{context}.ownership is required for kind=handle"
        raise ValueError(msg)

    nullable = mapping.get("nullable", False)
    if not isinstance(nullable, bool):
        msg = f"{context}.nullable must be a boolean"
        raise ValueError(msg)

    return TypeSpec(kind=kind, handle=handle, ownership=ownership, nullable=nullable)


def _parse_params(raw: Any, *, context: str, known_handles: set[str]) -> tuple[ParamSpec, ...]:
    params: list[ParamSpec] = []
    seen_names: set[str] = set()
    for index, item in enumerate(_expect_list(raw, context)):
        item_context = f"{context}[{index}]"
        mapping = _expect_mapping(item, item_context)
        name = _expect_str(mapping.get("name"), f"{item_context}.name")
        if name in seen_names:
            msg = f"{item_context}.name '{name}' is duplicated"
            raise ValueError(msg)
        seen_names.add(name)
        params.append(
            ParamSpec(
                name=name,
                type=_parse_type(mapping.get("type"), context=f"{item_context}.type", known_handles=known_handles),
            )
        )
    return tuple(params)


def _parse_implementation(raw: Any, *, context: str) -> ImplementationSpec:
    mapping = _expect_mapping(raw, context)
    kind = _expect_str(mapping.get("kind"), f"{context}.kind")
    if kind not in _ALLOWED_IMPLEMENTATION_KINDS:
        msg = f"{context}.kind must be one of {sorted(_ALLOWED_IMPLEMENTATION_KINDS)}"
        raise ValueError(msg)
    body = _expect_str(mapping.get("body"), f"{context}.body")
    return ImplementationSpec(kind=kind, body=body)


def _parse_call(
    raw: Any,
    *,
    context: str,
    known_handles: set[str],
    expect_receiver: bool,
) -> CallSpec:
    mapping = _expect_mapping(raw, context)
    kind = _expect_str(mapping.get("kind"), f"{context}.kind")
    if kind not in _ALLOWED_CALL_KINDS:
        msg = f"{context}.kind must be one of {sorted(_ALLOWED_CALL_KINDS)}"
        raise ValueError(msg)

    expose_as = _expect_str(mapping.get("expose_as"), f"{context}.expose_as")
    c_name = _expect_str(mapping.get("c_name"), f"{context}.c_name")
    cpp_name = mapping.get("cpp_name")
    if cpp_name is not None:
        cpp_name = _expect_str(cpp_name, f"{context}.cpp_name")

    receiver = mapping.get("receiver")
    if expect_receiver:
        receiver = _expect_str(receiver, f"{context}.receiver")
        if receiver not in known_handles:
            msg = f"{context}.receiver refers to unknown handle '{receiver}'"
            raise ValueError(msg)
    elif receiver is not None:
        msg = f"{context}.receiver is only valid for methods"
        raise ValueError(msg)

    returns = _parse_type(mapping.get("returns"), context=f"{context}.returns", known_handles=known_handles)
    params = _parse_params(mapping.get("params", []), context=f"{context}.params", known_handles=known_handles)

    implementation = None
    if kind.startswith("adapter_"):
        implementation = _parse_implementation(mapping.get("implementation"), context=f"{context}.implementation")
    elif "implementation" in mapping:
        msg = f"{context}.implementation is only valid for adapter calls"
        raise ValueError(msg)

    return CallSpec(
        kind=kind,
        expose_as=expose_as,
        c_name=c_name,
        receiver=receiver,
        cpp_name=cpp_name,
        returns=returns,
        params=params,
        implementation=implementation,
    )


def _parse_discovery(raw: Any, *, context: str, known_handles: set[str]) -> DiscoverySpec | None:
    if raw is None:
        return None

    mapping = _expect_mapping(raw, context)
    include_dir = Path(_expect_str(mapping.get("include_dir"), f"{context}.include_dir"))
    class_defaults = _expect_mapping(mapping.get("class_defaults", {}), f"{context}.class_defaults")
    function_defaults = _expect_mapping(mapping.get("function_defaults", {}), f"{context}.function_defaults")

    def default_include_all(defaults: dict[str, Any], default_context: str) -> bool:
        include_all = defaults.get("include_all", False)
        if not isinstance(include_all, bool):
            msg = f"{default_context}.include_all must be a boolean"
            raise ValueError(msg)
        return include_all

    class_include_all_default = default_include_all(class_defaults, f"{context}.class_defaults")
    function_include_all_default = default_include_all(function_defaults, f"{context}.function_defaults")

    def parse_overloads(raw_overloads: Any, overload_context: str) -> tuple[DiscoveryOverloadSpec, ...]:
        overloads: list[DiscoveryOverloadSpec] = []
        for overload_index, overload in enumerate(_expect_list(raw_overloads, overload_context)):
            item_context = f"{overload_context}[{overload_index}]"
            item_mapping = _expect_mapping(overload, item_context)
            cpp_name = _expect_str(item_mapping.get("cpp_name"), f"{item_context}.cpp_name")
            expose_as = _expect_str(item_mapping.get("expose_as"), f"{item_context}.expose_as")
            params = tuple(
                _expect_str(param, f"{item_context}.params[{param_index}]")
                for param_index, param in enumerate(_expect_list(item_mapping.get("params", []), f"{item_context}.params"))
            )
            overloads.append(DiscoveryOverloadSpec(cpp_name=cpp_name, expose_as=expose_as, params=params))
        return tuple(overloads)

    classes: list[DiscoveryClassSpec] = []
    for index, item in enumerate(_expect_list(mapping.get("classes", []), f"{context}.classes")):
        item_context = f"{context}.classes[{index}]"
        item_mapping = _expect_mapping(item, item_context)
        handle = _expect_str(item_mapping.get("handle"), f"{item_context}.handle")
        if handle not in known_handles:
            msg = f"{item_context}.handle refers to unknown handle '{handle}'"
            raise ValueError(msg)
        translation_unit = _expect_str(item_mapping.get("translation_unit"), f"{item_context}.translation_unit")
        include_all = item_mapping.get("include_all", class_include_all_default)
        if not isinstance(include_all, bool):
            msg = f"{item_context}.include_all must be a boolean"
            raise ValueError(msg)
        include = tuple(
            _expect_str(name, f"{item_context}.include[{include_index}]")
            for include_index, name in enumerate(_expect_list(item_mapping.get("include", []), f"{item_context}.include"))
        )
        exclude = tuple(
            _expect_str(name, f"{item_context}.exclude[{exclude_index}]")
            for exclude_index, name in enumerate(_expect_list(item_mapping.get("exclude", []), f"{item_context}.exclude"))
        )
        rename_raw = _expect_mapping(item_mapping.get("rename", {}), f"{item_context}.rename")
        rename: dict[str, str] = {}
        for cpp_name, expose_as in rename_raw.items():
            if not isinstance(cpp_name, str) or not cpp_name:
                msg = f"{item_context}.rename keys must be non-empty strings"
                raise ValueError(msg)
            rename[cpp_name] = _expect_str(expose_as, f"{item_context}.rename[{cpp_name}]")
        overloads = parse_overloads(item_mapping.get("overloads", []), f"{item_context}.overloads")
        if not include_all and not include and not overloads:
            msg = f"{item_context} must specify include_all: true or a non-empty include list"
            raise ValueError(msg)
        classes.append(
            DiscoveryClassSpec(
                handle=handle,
                translation_unit=translation_unit,
                include_all=include_all,
                include=include,
                exclude=exclude,
                rename=rename,
                overloads=overloads,
            )
        )

    functions: list[DiscoveryFunctionSpec] = []
    for index, item in enumerate(_expect_list(mapping.get("functions", []), f"{context}.functions")):
        item_context = f"{context}.functions[{index}]"
        item_mapping = _expect_mapping(item, item_context)
        translation_unit = _expect_str(item_mapping.get("translation_unit"), f"{item_context}.translation_unit")
        namespace = _expect_str(item_mapping.get("namespace"), f"{item_context}.namespace")
        include_all = item_mapping.get("include_all", function_include_all_default)
        if not isinstance(include_all, bool):
            msg = f"{item_context}.include_all must be a boolean"
            raise ValueError(msg)
        include = tuple(
            _expect_str(name, f"{item_context}.include[{include_index}]")
            for include_index, name in enumerate(_expect_list(item_mapping.get("include", []), f"{item_context}.include"))
        )
        exclude = tuple(
            _expect_str(name, f"{item_context}.exclude[{exclude_index}]")
            for exclude_index, name in enumerate(_expect_list(item_mapping.get("exclude", []), f"{item_context}.exclude"))
        )
        rename_raw = _expect_mapping(item_mapping.get("rename", {}), f"{item_context}.rename")
        rename: dict[str, str] = {}
        for cpp_name, expose_as in rename_raw.items():
            if not isinstance(cpp_name, str) or not cpp_name:
                msg = f"{item_context}.rename keys must be non-empty strings"
                raise ValueError(msg)
            rename[cpp_name] = _expect_str(expose_as, f"{item_context}.rename[{cpp_name}]")
        overloads = parse_overloads(item_mapping.get("overloads", []), f"{item_context}.overloads")
        if not include_all and not include and not overloads:
            msg = f"{item_context} must specify include_all: true or a non-empty include list"
            raise ValueError(msg)
        functions.append(
            DiscoveryFunctionSpec(
                namespace=namespace,
                translation_unit=translation_unit,
                include_all=include_all,
                include=include,
                exclude=exclude,
                rename=rename,
                overloads=overloads,
            )
        )

    return DiscoverySpec(include_dir=include_dir, classes=tuple(classes), functions=tuple(functions))


def _normalize_cpp_type(cpp_type: str) -> str:
    cpp_type = re.sub(r"/\*.*?\*/", "", cpp_type)
    cpp_type = " ".join(cpp_type.replace(" &", "&").replace(" *", "*").split())
    cpp_type = (
        cpp_type.replace("< ", "<")
        .replace(" >", ">")
        .replace(", ", ", ")
        .replace(" ,", ",")
    )
    cpp_type = cpp_type.replace("const ", "").strip()
    return cpp_type


def _base_cpp_type(cpp_type: str) -> str:
    return _normalize_cpp_type(cpp_type).removesuffix("&").removesuffix("*").strip()


def _find_handle_for_cpp_type(cpp_type: str, handles: dict[str, HandleSpec]) -> str | None:
    normalized = _normalize_cpp_type(cpp_type)
    base = normalized.removesuffix("&").removesuffix("*").strip()
    for handle_name, handle in handles.items():
        handle_base = handle.cpp_type.rsplit("::", 1)[-1]
        if handle.cpp_type == base or handle_base == base:
            return handle_name
    if normalized.endswith("::ptr"):
        base = normalized.removesuffix("::ptr")
        for handle_name, handle in handles.items():
            handle_base = handle.cpp_type.rsplit("::", 1)[-1]
            if handle.cpp_type == base or handle_base == base:
                return handle_name
    return None


def _vector_element_cpp_type(cpp_type: str) -> str | None:
    base = _base_cpp_type(cpp_type)
    if not base.startswith("std::vector<") or not base.endswith(">"):
        return None
    return base[len("std::vector<") : -1].strip()


def _infer_return_type(cpp_type: str, handles: dict[str, HandleSpec]) -> TypeSpec:
    normalized = _normalize_cpp_type(cpp_type)
    base = _base_cpp_type(cpp_type)
    vector_element = _vector_element_cpp_type(cpp_type)
    if base == "void":
        return TypeSpec(kind="void", cpp_type=cpp_type)
    if vector_element is not None and vector_element.endswith("*"):
        handle_name = _find_handle_for_cpp_type(vector_element, handles)
        if handle_name is not None:
            return TypeSpec(kind="handle_list", handle=handle_name, ownership="copy", cpp_type=cpp_type)
    handle_name = _find_handle_for_cpp_type(cpp_type, handles)
    if handle_name is not None:
        return TypeSpec(
            kind="handle",
            handle=handle_name,
            ownership="borrowed",
            nullable=normalized.endswith("*"),
            cpp_type=cpp_type,
        )
    if base in {"bool"}:
        return TypeSpec(kind="bool", cpp_type=cpp_type)
    if base in {"std::vector<bool>"}:
        return TypeSpec(kind="bool_list", ownership="copy", cpp_type=cpp_type)
    if base in {"std::vector<int>"}:
        return TypeSpec(kind="int32_list", ownership="copy", cpp_type=cpp_type)
    if base in {"std::vector<unsigned>", "std::vector<unsigned int>", "std::vector<uint32_t>"}:
        return TypeSpec(kind="uint32_list", ownership="copy", cpp_type=cpp_type)
    if base in {"std::vector<std::vector<int>>"}:
        return TypeSpec(kind="int32_list_list", ownership="copy", cpp_type=cpp_type)
    if base in {"std::vector<double>"}:
        return TypeSpec(kind="double_list", ownership="copy", cpp_type=cpp_type)
    if base in {"std::vector<std::vector<double>>"}:
        return TypeSpec(kind="double_list_list", ownership="copy", cpp_type=cpp_type)
    if base in {"int", "int32_t"}:
        return TypeSpec(kind="int32", cpp_type=cpp_type)
    if base in {"ptrdiff_t"}:
        return TypeSpec(kind="int32", cpp_type=cpp_type)
    if base in {"double"}:
        return TypeSpec(kind="double", cpp_type=cpp_type)
    if base in {"std::vector<int>"}:
        return TypeSpec(kind="int32_list", ownership="copy", cpp_type=cpp_type)
    if base in {"std::vector<unsigned>", "std::vector<unsigned int>", "std::vector<uint32_t>"}:
        return TypeSpec(kind="uint32_list", ownership="copy", cpp_type=cpp_type)
    if base in {"std::vector<std::vector<int>>"}:
        return TypeSpec(kind="int32_list_list", ownership="copy", cpp_type=cpp_type)
    if base in {"std::vector<double>"}:
        return TypeSpec(kind="double_list", ownership="copy", cpp_type=cpp_type)
    if base in {"std::vector<std::vector<double>>"}:
        return TypeSpec(kind="double_list_list", ownership="copy", cpp_type=cpp_type)
    if base in {"unsigned int", "uint32_t"}:
        return TypeSpec(kind="uint32", cpp_type=cpp_type)
    if base in {"size_t", "std::size_t"}:
        return TypeSpec(kind="size", cpp_type=cpp_type)
    if base in {"std::vector<std::string>", "std::vector<std::string >"}:
        return TypeSpec(kind="string_list", ownership="copy", cpp_type=cpp_type)
    if (base == "char" and normalized.endswith("*")) or base in {"std::string"}:
        return TypeSpec(kind="string", ownership="copy", cpp_type=cpp_type)
    msg = f"Unsupported discovered return type '{cpp_type}'"
    raise ValueError(msg)


def _infer_param_type(cpp_type: str, handles: dict[str, HandleSpec]) -> TypeSpec:
    normalized = _normalize_cpp_type(cpp_type)
    base = _base_cpp_type(cpp_type)
    handle_name = _find_handle_for_cpp_type(cpp_type, handles)
    if handle_name is not None:
        return TypeSpec(kind="handle", handle=handle_name, ownership="borrowed", nullable=normalized.endswith("*"), cpp_type=cpp_type)
    vector_element = _vector_element_cpp_type(cpp_type)
    if vector_element is not None and vector_element.endswith("*"):
        handle_name = _find_handle_for_cpp_type(vector_element, handles)
        if handle_name is not None:
            return TypeSpec(kind="handle_list", handle=handle_name, ownership="borrowed", cpp_type=cpp_type)
    if base in {"bool"}:
        return TypeSpec(kind="bool", cpp_type=cpp_type)
    if base in {"std::vector<bool>"}:
        return TypeSpec(kind="bool_list", ownership="copy", cpp_type=cpp_type)
    if base in {"std::vector<int>"}:
        return TypeSpec(kind="int32_list", ownership="copy", cpp_type=cpp_type)
    if base in {"std::vector<unsigned>", "std::vector<unsigned int>", "std::vector<uint32_t>"}:
        return TypeSpec(kind="uint32_list", ownership="copy", cpp_type=cpp_type)
    if base in {"std::vector<std::vector<int>>"}:
        return TypeSpec(kind="int32_list_list", ownership="copy", cpp_type=cpp_type)
    if base in {"std::vector<double>"}:
        return TypeSpec(kind="double_list", ownership="copy", cpp_type=cpp_type)
    if base in {"std::vector<std::vector<double>>"}:
        return TypeSpec(kind="double_list_list", ownership="copy", cpp_type=cpp_type)
    if base in {"int", "int32_t"}:
        return TypeSpec(kind="int32", cpp_type=cpp_type)
    if base in {"ptrdiff_t"}:
        return TypeSpec(kind="int32", cpp_type=cpp_type)
    if base in {"double"}:
        return TypeSpec(kind="double", cpp_type=cpp_type)
    if base in {"unsigned int", "uint32_t"}:
        return TypeSpec(kind="uint32", cpp_type=cpp_type)
    if base in {"size_t", "std::size_t"}:
        return TypeSpec(kind="size", cpp_type=cpp_type)
    if base in {"std::vector<std::string>", "std::vector<std::string >"}:
        return TypeSpec(kind="string_list", ownership="copy", cpp_type=cpp_type)
    if (base == "char" and normalized.endswith("*")) or base in {"std::string"}:
        return TypeSpec(kind="string", cpp_type=cpp_type)
    msg = f"Unsupported discovered parameter type '{cpp_type}'"
    raise ValueError(msg)


def _make_c_name(prefix: str, handle: HandleSpec, expose_as: str) -> str:
    receiver = handle.c_type.removeprefix("ifcopenshell_").removesuffix("_t")
    return f"ifcopenshell_{receiver}_{expose_as}"


def _make_function_c_name(prefix: str, expose_as: str) -> str:
    return f"{prefix}_{expose_as}"


def _snake_case_identifier(name: str) -> str:
    chars: list[str] = []
    previous_is_lower_or_digit = False
    for index, char in enumerate(name):
        if char == ":":
            continue
        if char == "_":
            chars.append(char)
            previous_is_lower_or_digit = False
            continue
        is_upper = char.isalpha() and char.upper() == char and char.lower() != char
        next_is_lower = index + 1 < len(name) and name[index + 1].islower()
        if chars and is_upper and (previous_is_lower_or_digit or next_is_lower):
            chars.append("_")
        chars.append(char.lower())
        previous_is_lower_or_digit = char.islower() or char.isdigit()
    return "".join(chars)


def _select_overload(
    overloads: tuple[DiscoveredMethod, ...] | tuple[DiscoveredFunction, ...],
    spec: DiscoveryOverloadSpec,
):
    target_params = tuple(_normalize_cpp_type(param) for param in spec.params)
    for overload in overloads:
        overload_params = tuple(_normalize_cpp_type(param.cpp_type) for param in overload.params)
        if overload.cpp_name == spec.cpp_name and overload_params == target_params:
            return overload
    msg = f"Unable to resolve overload '{spec.cpp_name}({', '.join(spec.params)})'"
    raise ValueError(msg)


def _discover_method_calls(
    spec_path: Path,
    discovery: DiscoverySpec,
    handles: dict[str, HandleSpec],
    compile_commands_path: Path,
) -> tuple[CallSpec, ...]:
    include_dir = (spec_path.parent / discovery.include_dir).resolve()
    class_cache: dict[tuple[str, str], dict[str, tuple[DiscoveredMethod, ...]]] = {}
    calls: list[CallSpec] = []
    calls_by_c_name: dict[str, CallSpec] = {}

    for item in discovery.classes:
        handle = handles[item.handle]
        cache_key = (handle.cpp_type, item.translation_unit)
        methods_by_name = class_cache.get(cache_key)
        if methods_by_name is None:
            class_name = handle.cpp_type.rsplit("::", 1)[-1]
            translation_unit = (include_dir / item.translation_unit).resolve()
            methods_by_name = discover_public_methods_with_compile_commands(compile_commands_path, translation_unit, class_name)
            class_cache[cache_key] = methods_by_name

        for overload_spec in item.overloads:
            overloads = methods_by_name.get(overload_spec.cpp_name)
            if overloads is None:
                msg = f"Unable to discover method '{overload_spec.cpp_name}' on handle '{item.handle}'"
                raise ValueError(msg)
            discovered = _select_overload(overloads, overload_spec)
            returns = _infer_return_type(discovered.return_cpp_type, handles)
            params = tuple(
                ParamSpec(name=param.name, type=_infer_param_type(param.cpp_type, handles))
                for param in discovered.params
            )
            call = CallSpec(
                kind="method",
                expose_as=overload_spec.expose_as,
                c_name=_make_c_name("ifcopenshell", handle, overload_spec.expose_as),
                receiver=item.handle,
                cpp_name=discovered.cpp_name,
                returns=returns,
                params=params,
                implementation=None,
            )
            existing = calls_by_c_name.get(call.c_name)
            if existing is not None:
                if existing == call:
                    continue
                msg = f"Discovered method collision for generated name '{call.c_name}'"
                raise ValueError(msg)
            calls_by_c_name[call.c_name] = call
            calls.append(call)

        explicit_includes = set(item.include)
        excluded = set(item.exclude)
        candidate_names = set(methods_by_name) if item.include_all else set()
        candidate_names.update(explicit_includes)
        candidate_names.difference_update(excluded)

        for cpp_name in sorted(candidate_names):
            overloads = methods_by_name.get(cpp_name)
            if overloads is None:
                msg = f"Unable to discover method '{cpp_name}' on handle '{item.handle}'"
                raise ValueError(msg)
            is_explicit = cpp_name in explicit_includes
            if len(overloads) != 1:
                if item.include_all and not is_explicit:
                    continue
                msg = f"Method '{cpp_name}' on handle '{item.handle}' is overloaded and requires an explicit policy"
                raise ValueError(msg)

            discovered = overloads[0]
            try:
                returns = _infer_return_type(discovered.return_cpp_type, handles)
                params = tuple(
                    ParamSpec(name=param.name, type=_infer_param_type(param.cpp_type, handles))
                    for param in discovered.params
                )
            except ValueError:
                if item.include_all and not is_explicit:
                    continue
                raise

            expose_as = item.rename.get(cpp_name, _snake_case_identifier(cpp_name))
            call = CallSpec(
                kind="method",
                expose_as=expose_as,
                c_name=_make_c_name("ifcopenshell", handle, expose_as),
                receiver=item.handle,
                cpp_name=discovered.cpp_name,
                returns=returns,
                params=params,
                implementation=None,
            )
            existing = calls_by_c_name.get(call.c_name)
            if existing is not None:
                if existing == call:
                    continue
                msg = f"Discovered method collision for generated name '{call.c_name}'"
                raise ValueError(msg)
            calls_by_c_name[call.c_name] = call
            calls.append(call)

    return tuple(calls)


def _discover_function_calls(
    spec_path: Path,
    discovery: DiscoverySpec,
    handles: dict[str, HandleSpec],
    c_prefix: str,
    compile_commands_path: Path,
) -> tuple[CallSpec, ...]:
    include_dir = (spec_path.parent / discovery.include_dir).resolve()
    namespace_cache: dict[tuple[str, str], dict[str, tuple[DiscoveredFunction, ...]]] = {}
    calls: list[CallSpec] = []
    calls_by_c_name: dict[str, CallSpec] = {}

    for item in discovery.functions:
        cache_key = (item.namespace, item.translation_unit)
        functions_by_name = namespace_cache.get(cache_key)
        if functions_by_name is None:
            translation_unit = (include_dir / item.translation_unit).resolve()
            functions_by_name = discover_namespace_functions_with_compile_commands(
                compile_commands_path, translation_unit, item.namespace
            )
            namespace_cache[cache_key] = functions_by_name

        for overload_spec in item.overloads:
            overloads = functions_by_name.get(overload_spec.cpp_name)
            if overloads is None:
                msg = f"Unable to discover function '{overload_spec.cpp_name}' in namespace '{item.namespace}'"
                raise ValueError(msg)
            discovered = _select_overload(overloads, overload_spec)
            returns = _infer_return_type(discovered.return_cpp_type, handles)
            params = tuple(
                ParamSpec(name=param.name, type=_infer_param_type(param.cpp_type, handles))
                for param in discovered.params
            )
            call = CallSpec(
                kind="function",
                expose_as=overload_spec.expose_as,
                c_name=_make_function_c_name(c_prefix, overload_spec.expose_as),
                receiver=None,
                cpp_name=f"{item.namespace}::{discovered.cpp_name}",
                returns=returns,
                params=params,
                implementation=None,
            )
            existing = calls_by_c_name.get(call.c_name)
            if existing is not None:
                if existing == call:
                    continue
                msg = f"Discovered function collision for generated name '{call.c_name}'"
                raise ValueError(msg)
            calls_by_c_name[call.c_name] = call
            calls.append(call)

        explicit_includes = set(item.include)
        excluded = set(item.exclude)
        candidate_names = set(functions_by_name) if item.include_all else set()
        candidate_names.update(explicit_includes)
        candidate_names.difference_update(excluded)

        for cpp_name in sorted(candidate_names):
            overloads = functions_by_name.get(cpp_name)
            if overloads is None:
                msg = f"Unable to discover function '{cpp_name}' in namespace '{item.namespace}'"
                raise ValueError(msg)
            is_explicit = cpp_name in explicit_includes
            if len(overloads) != 1:
                if item.include_all and not is_explicit:
                    continue
                msg = f"Function '{cpp_name}' in namespace '{item.namespace}' is overloaded and requires an explicit policy"
                raise ValueError(msg)

            discovered = overloads[0]
            try:
                returns = _infer_return_type(discovered.return_cpp_type, handles)
                params = tuple(
                    ParamSpec(name=param.name, type=_infer_param_type(param.cpp_type, handles))
                    for param in discovered.params
                )
            except ValueError:
                if item.include_all and not is_explicit:
                    continue
                raise

            expose_as = item.rename.get(cpp_name, _snake_case_identifier(cpp_name))
            call = CallSpec(
                kind="function",
                expose_as=expose_as,
                c_name=_make_function_c_name(c_prefix, expose_as),
                receiver=None,
                cpp_name=f"{item.namespace}::{discovered.cpp_name}",
                returns=returns,
                params=params,
                implementation=None,
            )
            existing = calls_by_c_name.get(call.c_name)
            if existing is not None:
                if existing == call:
                    continue
                msg = f"Discovered function collision for generated name '{call.c_name}'"
                raise ValueError(msg)
            calls_by_c_name[call.c_name] = call
            calls.append(call)

    return tuple(calls)


def load_authored_spec(path: Path, compile_commands_path: Path | None = None) -> AuthoredBindingSpec:
    raw = yaml.safe_load(path.read_text(encoding="utf-8"))
    root = _expect_mapping(raw, "binding spec")

    schema_version = _expect_int(root.get("schema_version"), "schema_version")
    module = _expect_str(root.get("module"), "module")
    slice_name = _expect_str(root.get("slice"), "slice")
    c_prefix = _expect_str(root.get("c_prefix"), "c_prefix")
    public_headers = tuple(
        _expect_str(header, f"public_headers[{index}]")
        for index, header in enumerate(_expect_list(root.get("public_headers", []), "public_headers"))
    )

    raw_handles = _expect_list(root.get("handles", []), "handles")
    handles: dict[str, HandleSpec] = {}
    for index, item in enumerate(raw_handles):
        context = f"handles[{index}]"
        mapping = _expect_mapping(item, context)
        handle = HandleSpec(
            name=_expect_str(mapping.get("name"), f"{context}.name"),
            cpp_type=_expect_str(mapping.get("cpp_type"), f"{context}.cpp_type"),
            c_type=_expect_str(mapping.get("c_type"), f"{context}.c_type"),
            destructor=_expect_str(mapping.get("destructor"), f"{context}.destructor"),
        )
        if handle.destructor not in _ALLOWED_DESTRUCTORS:
            msg = f"{context}.destructor must be one of {sorted(_ALLOWED_DESTRUCTORS)}"
            raise ValueError(msg)
        if handle.name in handles:
            msg = f"{context}.name '{handle.name}' is duplicated"
            raise ValueError(msg)
        handles[handle.name] = handle

    known_handles = set(handles)
    discovery = _parse_discovery(root.get("discover"), context="discover", known_handles=known_handles)
    if discovery is not None and compile_commands_path is None:
        msg = "compile_commands.json is required for AST-backed discovery"
        raise ValueError(msg)
    discovered_methods = _discover_method_calls(path, discovery, handles, compile_commands_path) if discovery is not None else tuple()
    discovered_functions = (
        _discover_function_calls(path, discovery, handles, c_prefix, compile_commands_path) if discovery is not None else tuple()
    )

    adapter_functions = tuple(
        _parse_call(item, context=f"functions[{index}]", known_handles=known_handles, expect_receiver=False)
        for index, item in enumerate(_expect_list(root.get("functions", []), "functions"))
    )
    adapter_methods = tuple(
        _parse_call(item, context=f"methods[{index}]", known_handles=known_handles, expect_receiver=True)
        for index, item in enumerate(_expect_list(root.get("methods", []), "methods"))
    )

    return AuthoredBindingSpec(
        schema_version=schema_version,
        module=module,
        slice=slice_name,
        c_prefix=c_prefix,
        public_headers=public_headers,
        handles=handles,
        discovery=discovery,
        functions=discovered_functions + adapter_functions,
        methods=discovered_methods + adapter_methods,
    )
