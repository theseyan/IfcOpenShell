from __future__ import annotations

from dataclasses import dataclass
from pathlib import Path
import re
from typing import Any

import yaml

try:
    from .binding_model import CallSpec, DiscoveryDiagnostic, HandleSpec, ImplementationSpec, ParamSpec, TypeSpec
except ImportError:  # pragma: no cover - script execution fallback
    from binding_model import CallSpec, DiscoveryDiagnostic, HandleSpec, ImplementationSpec, ParamSpec, TypeSpec

try:
    from .debug import debug_log, debug_path
except ImportError:  # pragma: no cover - script execution fallback
    from debug import debug_log, debug_path

try:
    from .policy_ir import (
        ArrayElementFieldPolicyOp,
        AsItemCastPolicyOp,
        ChildrenAddPolicyOp,
        ChildrenAtPolicyOp,
        ChildrenCountPolicyOp,
        CcomponentsAccessorPolicyOp,
        ConstructorPolicyOp,
        DirectFieldPolicyOp,
        DirectFunctionPolicyOp,
        DirectMethodPolicyOp,
        FieldSetterPolicyOp,
        InlineAdapterPolicyOp,
        MethodSizePolicyOp,
        OptionalGetPolicyOp,
        OptionalHasPolicyOp,
        PointerPresencePolicyOp,
        PolicyCallSpec,
        ValueHandleFieldPolicyOp,
        VariantGetPolicyOp,
        VariantSetPolicyOp,
    )
except ImportError:  # pragma: no cover - script execution fallback
    from policy_ir import (
        ArrayElementFieldPolicyOp,
        AsItemCastPolicyOp,
        ChildrenAddPolicyOp,
        ChildrenAtPolicyOp,
        ChildrenCountPolicyOp,
        CcomponentsAccessorPolicyOp,
        ConstructorPolicyOp,
        DirectFieldPolicyOp,
        DirectFunctionPolicyOp,
        DirectMethodPolicyOp,
        FieldSetterPolicyOp,
        InlineAdapterPolicyOp,
        MethodSizePolicyOp,
        OptionalGetPolicyOp,
        OptionalHasPolicyOp,
        PointerPresencePolicyOp,
        PolicyCallSpec,
        ValueHandleFieldPolicyOp,
        VariantGetPolicyOp,
        VariantSetPolicyOp,
    )

try:
    from .clang_discovery import (
        DiscoveredCppType,
        DiscoveredField,
        DiscoveredFunction,
        DiscoveredMethod,
        discover_namespace_functions_with_compile_commands,
        discover_public_fields_with_compile_commands,
        discover_public_methods_with_compile_commands,
    )
except ImportError:  # pragma: no cover - script execution fallback
    from clang_discovery import (
        DiscoveredCppType,
        DiscoveredField,
        DiscoveredFunction,
        DiscoveredMethod,
        discover_namespace_functions_with_compile_commands,
        discover_public_fields_with_compile_commands,
        discover_public_methods_with_compile_commands,
    )

try:
    from .semantic_types import (
        EnumSemanticType,
        RecordSemanticType,
        ScalarSemanticType,
        SequenceSemanticType,
        StringSemanticType,
        VoidSemanticType,
        analyze_cpp_type,
        semantic_leaf_type,
        semantic_record_match_names,
        semantic_sequence_depth,
    )
except ImportError:  # pragma: no cover - script execution fallback
    from semantic_types import (
        EnumSemanticType,
        RecordSemanticType,
        ScalarSemanticType,
        SequenceSemanticType,
        StringSemanticType,
        VoidSemanticType,
        analyze_cpp_type,
        semantic_leaf_type,
        semantic_record_match_names,
        semantic_sequence_depth,
    )


_ALLOWED_TYPE_KINDS = {
    "void",
    "bool",
    "int32",
    "int64",
    "double",
    "double_buffer",  # Zero-copy borrowed pointer to double array
    "int32_buffer",   # Zero-copy borrowed pointer to int32 array
    "uint32",
    "size",
    "string",
    "handle",
    "handle_list",
    "handle_list_list",
    "opaque_ptr",  # Raw pointer to an external type (passed through as void*)
}
_ALLOWED_OWNERSHIP = {"owned", "borrowed", "static", "copy"}
_ALLOWED_DESTRUCTORS = {"delete", "none", "shared_ptr"}
_ALLOWED_PTR_TYPES = {"raw", "shared_ptr"}
_FUNCTION_CALL_KINDS = {"function", "adapter_function", "constructor"}
_METHOD_CALL_KINDS = {"method", "adapter_method"}
_ALLOWED_IMPLEMENTATION_KINDS = {"inline_cpp"}
_SCALAR_SEQUENCE_FAMILIES = frozenset({"bool", "string", "int32", "uint8", "uint32", "double"})


@dataclass(frozen=True)
class DiscoveryChildrenSpec:
    element_handle: str
    count_as: str
    at_as: str
    cpp_field: str = "children"
    add_as: str | None = None
    add_cast_cpp_type: str | None = None


@dataclass(frozen=True)
class CcomponentsAccessorSpec:
    expose_as: str
    dimensions: int  # 3 for vector, 16 for matrix (4x4)
    access_via: str  # e.g. "ccomponents" or "data()->ccomponents"


@dataclass(frozen=True)
class VariantAccessorTypeSpec:
    cpp_type: str
    getter_types: tuple[str, ...]


@dataclass(frozen=True)
class VariantAccessorsSpec:
    get_method: str  # C++ method name for getter (e.g. "get")
    set_method: str  # C++ method name for setter (e.g. "set")
    variant_type: str  # full C++ variant type for constructing set values
    types: dict[str, VariantAccessorTypeSpec]  # expose_suffix -> getter/setter type policy


@dataclass(frozen=True)
class DiscoveryTypeOverrideSpec:
    returns: TypeSpec | None
    params: dict[str, TypeSpec]


@dataclass(frozen=True)
class DiscoveryClassSpec:
    handle: str
    translation_unit: str
    include_all: bool
    include: tuple[str, ...]
    exclude: tuple[str, ...]
    rename: dict[str, str]
    overloads: tuple["DiscoveryOverloadSpec", ...]
    discover_fields: bool
    include_inherited_fields: bool
    include_inherited_methods: bool
    discover_has_fields: bool
    discover_optional_fields: bool
    discover_children: DiscoveryChildrenSpec | None
    discover_as_item: bool
    extra_fields: dict[str, str]  # field_name -> cpp_type (for template classes)
    field_setters: tuple[str, ...]  # field names to generate setters for
    method_sizes: dict[str, str]  # method_name -> expose_as (generates X().size())
    array_pair_fields: dict[str, str]  # field_name -> return_kind (generates field_u/field_v for std::array<T,2>)
    binary_operators: dict[str, str]  # expose_as -> C++ operator (e.g. add: "+")
    unary_operators: dict[str, str]   # expose_as -> C++ operator (e.g. negate: "-")
    comparison_operators: dict[str, str]  # expose_as -> C++ operator (e.g. equals: "==")
    ccomponents_accessor: CcomponentsAccessorSpec | None  # generates Eigen ccomponents data extraction
    variant_accessors: VariantAccessorsSpec | None  # generates typed get/set for variant methods
    enum_types_as_int32: frozenset[str]  # type names treated as enums → int32 with static_cast
    type_overrides: dict[str, DiscoveryTypeOverrideSpec]  # cpp member name -> explicit type override policy


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
    type_overrides: dict[str, DiscoveryTypeOverrideSpec]


@dataclass(frozen=True)
class DiscoveryOverloadSpec:
    cpp_name: str
    expose_as: str
    params: tuple[str, ...]


@dataclass(frozen=True)
class ImportedHandle:
    """A handle imported from another slice."""
    slice: str
    handle: str


@dataclass(frozen=True)
class AuthoredBindingSpec:
    schema_version: int
    module: str
    slice: str
    c_prefix: str
    public_headers: tuple[str, ...]
    handles: dict[str, HandleSpec]
    imports: tuple[ImportedHandle, ...]  # Handles imported from other slices
    depends_on_common: str | None  # If set, skip emitting common type implementations
    discovery: DiscoverySpec | None
    functions: tuple[CallSpec, ...]
    methods: tuple[CallSpec, ...]
    discovery_diagnostics: tuple[DiscoveryDiagnostic, ...] = ()


@dataclass(frozen=True)
class MergedBindingSpec:
    """A merged binding spec containing multiple modules."""
    module: str  # Common module name (e.g., "ifcopenshell")
    c_prefix: str  # Common C prefix (e.g., "ifcopenshell")
    public_headers: tuple[str, ...]  # Merged public headers
    handles: dict[str, HandleSpec]  # All handles from all modules
    functions: tuple[CallSpec, ...]  # All functions from all modules
    methods: tuple[CallSpec, ...]  # All methods from all modules
    discovery_diagnostics: tuple[DiscoveryDiagnostic, ...] = ()


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


def _sequence_kind_parts(kind: str) -> tuple[str, int] | None:
    match = re.fullmatch(r"([a-z0-9]+)((?:_list)+)", kind)
    if match is None:
        return None
    leaf = match.group(1)
    depth = match.group(2).count("_list")
    if leaf not in _SCALAR_SEQUENCE_FAMILIES:
        return None
    return leaf, depth


def _is_sequence_kind(kind: str) -> bool:
    return _sequence_kind_parts(kind) is not None


def _scalar_sequence_kind(family: str, depth: int) -> str | None:
    if family not in _SCALAR_SEQUENCE_FAMILIES or depth <= 0:
        return None
    return f"{family}{'_list' * depth}"


def _scalar_sequence_depth(kind: str) -> int | None:
    parts = _sequence_kind_parts(kind)
    if parts is None:
        return None
    return parts[1]


def _normalized_scalar_kind(kind: str) -> str:
    parts = _sequence_kind_parts(kind)
    return parts[0] if parts is not None else kind


def _handle_sequence_depth(kind: str) -> int | None:
    if kind == "handle":
        return 0
    if kind == "handle_list":
        return 1
    if kind == "handle_list_list":
        return 2
    return None


def _parse_type(raw: Any, *, context: str, known_handles: set[str]) -> TypeSpec:
    mapping = _expect_mapping(raw, context)
    kind = _expect_str(mapping.get("kind"), f"{context}.kind")
    if kind not in _ALLOWED_TYPE_KINDS and not _is_sequence_kind(kind):
        msg = f"{context}.kind must be one of {sorted(_ALLOWED_TYPE_KINDS)} or a supported '*_list' sequence kind"
        raise ValueError(msg)
    handle_sequence_depth = _handle_sequence_depth(kind)
    scalar_sequence_depth = _scalar_sequence_depth(kind) if handle_sequence_depth is None else None

    handle = mapping.get("handle")
    if handle_sequence_depth is not None:
        handle = _expect_str(handle, f"{context}.handle")
        if handle not in known_handles:
            msg = f"{context}.handle refers to unknown handle '{handle}'"
            raise ValueError(msg)
    elif handle is not None:
        msg = f"{context}.handle is only valid for kind=handle, kind=handle_list, or kind=handle_list_list"
        raise ValueError(msg)

    ownership = mapping.get("ownership")
    if ownership is not None:
        ownership = _expect_str(ownership, f"{context}.ownership")
        if ownership not in _ALLOWED_OWNERSHIP:
            msg = f"{context}.ownership must be one of {sorted(_ALLOWED_OWNERSHIP)}"
            raise ValueError(msg)
    if handle_sequence_depth == 0 and ownership is None:
        msg = f"{context}.ownership is required for kind=handle"
        raise ValueError(msg)

    nullable = mapping.get("nullable", False)
    if not isinstance(nullable, bool):
        msg = f"{context}.nullable must be a boolean"
        raise ValueError(msg)

    cpp_type = mapping.get("cpp_type")
    if cpp_type is not None:
        cpp_type = _expect_str(cpp_type, f"{context}.cpp_type")
    if kind == "opaque_ptr" and cpp_type is None:
        msg = f"{context}.cpp_type is required for kind=opaque_ptr"
        raise ValueError(msg)

    normalized_kind = "handle" if handle_sequence_depth is not None else _normalized_scalar_kind(kind)
    return TypeSpec(
        kind=normalized_kind,
        handle=handle,
        ownership=ownership,
        nullable=nullable,
        cpp_type=cpp_type,
        sequence_depth=handle_sequence_depth if handle_sequence_depth is not None else (scalar_sequence_depth or 0),
    )


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
    c_prefix: str,
    handles: dict[str, HandleSpec],
    known_handles: set[str],
    expect_receiver: bool,
) -> CallSpec:
    mapping = _expect_mapping(raw, context)
    raw_kind = mapping.get("kind")
    if raw_kind is not None:
        kind = _expect_str(raw_kind, f"{context}.kind")
    elif expect_receiver:
        kind = "adapter_method" if "implementation" in mapping else "method"
    elif "handle" in mapping:
        kind = "constructor"
    else:
        kind = "adapter_function" if "implementation" in mapping else "function"

    allowed_kinds = _METHOD_CALL_KINDS if expect_receiver else _FUNCTION_CALL_KINDS
    if kind not in allowed_kinds:
        msg = f"{context}.kind must be one of {sorted(allowed_kinds)}"
        raise ValueError(msg)

    expose_as = _expect_str(mapping.get("expose_as"), f"{context}.expose_as")
    if "c_name" in mapping:
        msg = f"{context}.c_name is not supported; C names are derived from expose_as and receiver"
        raise ValueError(msg)
    raw_c_expose_as = mapping.get("c_expose_as")
    c_expose_as = expose_as if raw_c_expose_as is None else _expect_str(raw_c_expose_as, f"{context}.c_expose_as")
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

    if receiver is None:
        c_name = _make_function_c_name(c_prefix, c_expose_as)
    else:
        c_name = _make_c_name(handles[receiver], c_expose_as)

    returns = None
    if kind == "constructor":
        raw_handle = mapping.get("handle")
        if raw_handle is not None:
            handle_name = _expect_str(raw_handle, f"{context}.handle")
            if handle_name not in known_handles:
                msg = f"{context}.handle refers to unknown handle '{handle_name}'"
                raise ValueError(msg)
            returns = TypeSpec(kind="handle", handle=handle_name, ownership="owned")
        elif "returns" in mapping:
            returns = _parse_type(mapping.get("returns"), context=f"{context}.returns", known_handles=known_handles)
        else:
            msg = f"{context}: constructor requires either 'handle' or 'returns'"
            raise ValueError(msg)
    else:
        if "handle" in mapping:
            msg = f"{context}.handle is only valid for constructors"
            raise ValueError(msg)
        returns = _parse_type(mapping.get("returns"), context=f"{context}.returns", known_handles=known_handles)
    params = _parse_params(mapping.get("params", []), context=f"{context}.params", known_handles=known_handles)

    implementation = None
    if kind.startswith("adapter_"):
        implementation = _parse_implementation(mapping.get("implementation"), context=f"{context}.implementation")
    elif "implementation" in mapping:
        msg = f"{context}.implementation is only valid for adapter calls"
        raise ValueError(msg)

    cpp_class = None
    compile_guard = None
    if kind == "constructor":
        raw_cpp_class = mapping.get("cpp_class")
        if raw_cpp_class is not None:
            cpp_class = _expect_str(raw_cpp_class, f"{context}.cpp_class")
        raw_compile_guard = mapping.get("compile_guard")
        if raw_compile_guard is not None:
            compile_guard = _expect_str(raw_compile_guard, f"{context}.compile_guard")
    else:
        if "cpp_class" in mapping:
            msg = f"{context}.cpp_class is only valid for constructors"
            raise ValueError(msg)
        if "compile_guard" in mapping:
            msg = f"{context}.compile_guard is only valid for constructors"
            raise ValueError(msg)

    if kind == "function":
        policy_operation = DirectFunctionPolicyOp(cpp_name=cpp_name)
    elif kind == "method":
        policy_operation = DirectMethodPolicyOp(cpp_name=cpp_name)
    elif kind == "field":
        if cpp_name is None:
            msg = f"{context}.cpp_name is required for field calls"
            raise ValueError(msg)
        policy_operation = DirectFieldPolicyOp(field_name=cpp_name)
    elif kind == "value_handle_field":
        if cpp_name is None:
            msg = f"{context}.cpp_name is required for value_handle_field calls"
            raise ValueError(msg)
        policy_operation = ValueHandleFieldPolicyOp(field_name=cpp_name)
    elif kind == "constructor":
        policy_operation = ConstructorPolicyOp(cpp_class=cpp_class, compile_guard=compile_guard)
    elif kind.startswith("adapter_"):
        if implementation is None:
            msg = f"{context}.implementation is required for adapter calls"
            raise ValueError(msg)
        policy_operation = InlineAdapterPolicyOp(implementation=implementation)
    else:
        msg = f"{context}.kind '{kind}' must use typed policy operations, not legacy call kinds"
        raise ValueError(msg)

    return CallSpec(
        expose_as=expose_as,
        c_name=c_name,
        receiver=receiver,
        returns=returns,
        params=params,
        policy_operation=policy_operation,
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
        discover_fields = item_mapping.get("discover_fields", False)
        if not isinstance(discover_fields, bool):
            msg = f"{item_context}.discover_fields must be a boolean"
            raise ValueError(msg)
        include_inherited_fields = item_mapping.get("include_inherited_fields", False)
        if not isinstance(include_inherited_fields, bool):
            msg = f"{item_context}.include_inherited_fields must be a boolean"
            raise ValueError(msg)
        include_inherited_methods = item_mapping.get("include_inherited_methods", False)
        if not isinstance(include_inherited_methods, bool):
            msg = f"{item_context}.include_inherited_methods must be a boolean"
            raise ValueError(msg)
        discover_has_fields = item_mapping.get("discover_has_fields", False)
        if not isinstance(discover_has_fields, bool):
            msg = f"{item_context}.discover_has_fields must be a boolean"
            raise ValueError(msg)
        discover_optional_fields = item_mapping.get("discover_optional_fields", False)
        if not isinstance(discover_optional_fields, bool):
            msg = f"{item_context}.discover_optional_fields must be a boolean"
            raise ValueError(msg)
        # Parse discover_children
        discover_children_raw = item_mapping.get("discover_children")
        discover_children: DiscoveryChildrenSpec | None = None
        if discover_children_raw is not None:
            dc_context = f"{item_context}.discover_children"
            dc_mapping = _expect_mapping(discover_children_raw, dc_context)
            element_handle = _expect_str(dc_mapping.get("element_handle"), f"{dc_context}.element_handle")
            if element_handle not in known_handles:
                msg = f"{dc_context}.element_handle refers to unknown handle '{element_handle}'"
                raise ValueError(msg)
            count_as = _expect_str(dc_mapping.get("count_as"), f"{dc_context}.count_as")
            at_as = _expect_str(dc_mapping.get("at_as"), f"{dc_context}.at_as")
            add_as_raw = dc_mapping.get("add_as")
            add_as: str | None = None
            add_cast_cpp_type: str | None = None
            if add_as_raw is not None:
                add_as = _expect_str(add_as_raw, f"{dc_context}.add_as")
                add_cast_raw = dc_mapping.get("add_cast_cpp_type")
                if add_cast_raw is not None:
                    add_cast_cpp_type = _expect_str(add_cast_raw, f"{dc_context}.add_cast_cpp_type")
            cpp_field_raw = dc_mapping.get("cpp_field")
            cpp_field = _expect_str(cpp_field_raw, f"{dc_context}.cpp_field") if cpp_field_raw is not None else "children"
            discover_children = DiscoveryChildrenSpec(
                element_handle=element_handle, count_as=count_as, at_as=at_as,
                cpp_field=cpp_field, add_as=add_as, add_cast_cpp_type=add_cast_cpp_type,
            )
        discover_as_item = item_mapping.get("discover_as_item", False)
        if not isinstance(discover_as_item, bool):
            msg = f"{item_context}.discover_as_item must be a boolean"
            raise ValueError(msg)
        # Parse extra_fields: manually specified fields for template classes
        extra_fields_raw = _expect_mapping(item_mapping.get("extra_fields", {}), f"{item_context}.extra_fields")
        extra_fields: dict[str, str] = {}
        for field_name, cpp_type in extra_fields_raw.items():
            if not isinstance(field_name, str) or not field_name:
                msg = f"{item_context}.extra_fields keys must be non-empty strings"
                raise ValueError(msg)
            extra_fields[field_name] = _expect_str(cpp_type, f"{item_context}.extra_fields[{field_name}]")
        # Parse field_setters: field names to generate setters for
        field_setters = tuple(
            _expect_str(name, f"{item_context}.field_setters[{fs_index}]")
            for fs_index, name in enumerate(_expect_list(item_mapping.get("field_setters", []), f"{item_context}.field_setters"))
        )
        # Parse method_sizes: method_name -> expose_as (generates method().size())
        method_sizes_raw = _expect_mapping(item_mapping.get("method_sizes", {}), f"{item_context}.method_sizes")
        method_sizes: dict[str, str] = {}
        for method_name, expose_as_val in method_sizes_raw.items():
            if not isinstance(method_name, str) or not method_name:
                msg = f"{item_context}.method_sizes keys must be non-empty strings"
                raise ValueError(msg)
            method_sizes[method_name] = _expect_str(expose_as_val, f"{item_context}.method_sizes[{method_name}]")
        # Parse array_pair_fields: field_name -> return_kind (generates field_u/field_v)
        array_pair_raw = _expect_mapping(item_mapping.get("array_pair_fields", {}), f"{item_context}.array_pair_fields")
        array_pair_fields: dict[str, str] = {}
        for field_name, return_kind_val in array_pair_raw.items():
            if not isinstance(field_name, str) or not field_name:
                msg = f"{item_context}.array_pair_fields keys must be non-empty strings"
                raise ValueError(msg)
            array_pair_fields[field_name] = _expect_str(return_kind_val, f"{item_context}.array_pair_fields[{field_name}]")
        # Parse operator dicts: expose_as -> C++ operator symbol
        def _parse_op_dict(key: str) -> dict[str, str]:
            raw = _expect_mapping(item_mapping.get(key, {}), f"{item_context}.{key}")
            result: dict[str, str] = {}
            for name, op_val in raw.items():
                if not isinstance(name, str) or not name:
                    msg = f"{item_context}.{key} keys must be non-empty strings"
                    raise ValueError(msg)
                result[name] = _expect_str(op_val, f"{item_context}.{key}[{name}]")
            return result
        binary_operators = _parse_op_dict("binary_operators")
        unary_operators = _parse_op_dict("unary_operators")
        comparison_operators = _parse_op_dict("comparison_operators")

        # Parse ccomponents_accessor
        ccomponents_raw = item_mapping.get("ccomponents_accessor")
        ccomponents_accessor: CcomponentsAccessorSpec | None = None
        if ccomponents_raw is not None:
            cc_context = f"{item_context}.ccomponents_accessor"
            cc_mapping = _expect_mapping(ccomponents_raw, cc_context)
            cc_expose = _expect_str(cc_mapping.get("expose_as"), f"{cc_context}.expose_as")
            cc_dims = cc_mapping.get("dimensions")
            if not isinstance(cc_dims, int) or cc_dims <= 0:
                msg = f"{cc_context}.dimensions must be a positive integer"
                raise ValueError(msg)
            cc_access = _expect_str(cc_mapping.get("access_via", "ccomponents"), f"{cc_context}.access_via")
            ccomponents_accessor = CcomponentsAccessorSpec(
                expose_as=cc_expose, dimensions=cc_dims, access_via=cc_access,
            )

        # Parse variant_accessors
        variant_raw = item_mapping.get("variant_accessors")
        variant_accessors: VariantAccessorsSpec | None = None
        if variant_raw is not None:
            va_context = f"{item_context}.variant_accessors"
            va_mapping = _expect_mapping(variant_raw, va_context)
            va_get = _expect_str(va_mapping.get("get_method"), f"{va_context}.get_method")
            va_set = _expect_str(va_mapping.get("set_method"), f"{va_context}.set_method")
            va_variant_type = _expect_str(va_mapping.get("variant_type"), f"{va_context}.variant_type")
            va_types_raw = _expect_mapping(va_mapping.get("types", {}), f"{va_context}.types")
            va_types: dict[str, VariantAccessorTypeSpec] = {}
            for suffix, cpp_type_val in va_types_raw.items():
                if not isinstance(suffix, str) or not suffix:
                    msg = f"{va_context}.types keys must be non-empty strings"
                    raise ValueError(msg)
                type_context = f"{va_context}.types[{suffix}]"
                if isinstance(cpp_type_val, str):
                    cpp_type = _expect_str(cpp_type_val, type_context)
                    getter_types = (cpp_type,)
                else:
                    type_mapping = _expect_mapping(cpp_type_val, type_context)
                    cpp_type = _expect_str(type_mapping.get("cpp_type"), f"{type_context}.cpp_type")
                    getter_types_raw = type_mapping.get("getter_types", [cpp_type])
                    getter_types_list = _expect_list(getter_types_raw, f"{type_context}.getter_types")
                    getter_types = tuple(
                        _expect_str(value, f"{type_context}.getter_types[{index}]")
                        for index, value in enumerate(getter_types_list)
                    )
                    if not getter_types:
                        msg = f"{type_context}.getter_types must not be empty"
                        raise ValueError(msg)
                va_types[suffix] = VariantAccessorTypeSpec(cpp_type=cpp_type, getter_types=getter_types)
            variant_accessors = VariantAccessorsSpec(
                get_method=va_get, set_method=va_set,
                variant_type=va_variant_type, types=va_types,
            )

        # Parse enum_types_as_int32
        enum_types_raw = item_mapping.get("enum_types_as_int32", [])
        if not isinstance(enum_types_raw, list):
            msg = f"{item_context}.enum_types_as_int32 must be a list of strings"
            raise ValueError(msg)
        enum_types_as_int32 = frozenset(str(t) for t in enum_types_raw)

        type_overrides_raw = _expect_mapping(item_mapping.get("type_overrides", {}), f"{item_context}.type_overrides")
        type_overrides: dict[str, DiscoveryTypeOverrideSpec] = {}
        for member_name, override_raw in type_overrides_raw.items():
            if not isinstance(member_name, str) or not member_name:
                msg = f"{item_context}.type_overrides keys must be non-empty strings"
                raise ValueError(msg)
            override_context = f"{item_context}.type_overrides[{member_name}]"
            override_mapping = _expect_mapping(override_raw, override_context)
            returns_raw = override_mapping.get("returns")
            returns = None
            if returns_raw is not None:
                returns = _parse_type(returns_raw, context=f"{override_context}.returns", known_handles=known_handles)
            params_raw = _expect_mapping(override_mapping.get("params", {}), f"{override_context}.params")
            params: dict[str, TypeSpec] = {}
            for param_name, param_type_raw in params_raw.items():
                if not isinstance(param_name, str) or not param_name:
                    msg = f"{override_context}.params keys must be non-empty strings"
                    raise ValueError(msg)
                params[param_name] = _parse_type(
                    param_type_raw,
                    context=f"{override_context}.params[{param_name}]",
                    known_handles=known_handles,
                )
            type_overrides[member_name] = DiscoveryTypeOverrideSpec(returns=returns, params=params)

        has_any_feature = (include_all or include or overloads or discover_fields
                          or discover_children is not None or discover_as_item
                          or extra_fields or field_setters or discover_optional_fields
                          or method_sizes or array_pair_fields
                          or binary_operators or unary_operators or comparison_operators
                          or ccomponents_accessor is not None or variant_accessors is not None
                          or type_overrides)
        if not has_any_feature:
            msg = f"{item_context} must specify at least one discovery feature"
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
                discover_fields=discover_fields,
                include_inherited_fields=include_inherited_fields,
                include_inherited_methods=include_inherited_methods,
                discover_has_fields=discover_has_fields,
                discover_optional_fields=discover_optional_fields,
                discover_children=discover_children,
                discover_as_item=discover_as_item,
                extra_fields=extra_fields,
                field_setters=field_setters,
                method_sizes=method_sizes,
                array_pair_fields=array_pair_fields,
                binary_operators=binary_operators,
                unary_operators=unary_operators,
                comparison_operators=comparison_operators,
                ccomponents_accessor=ccomponents_accessor,
                variant_accessors=variant_accessors,
                enum_types_as_int32=enum_types_as_int32,
                type_overrides=type_overrides,
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
        type_overrides_raw = _expect_mapping(item_mapping.get("type_overrides", {}), f"{item_context}.type_overrides")
        type_overrides: dict[str, DiscoveryTypeOverrideSpec] = {}
        for function_name, override_raw in type_overrides_raw.items():
            if not isinstance(function_name, str) or not function_name:
                msg = f"{item_context}.type_overrides keys must be non-empty strings"
                raise ValueError(msg)
            override_context = f"{item_context}.type_overrides[{function_name}]"
            override_mapping = _expect_mapping(override_raw, override_context)
            returns = None
            if "returns" in override_mapping:
                returns = _parse_type(override_mapping["returns"], context=f"{override_context}.returns", known_handles=known_handles)
            params_raw = _expect_mapping(override_mapping.get("params", {}), f"{override_context}.params")
            params: dict[str, TypeSpec] = {}
            for param_name, param_raw in params_raw.items():
                if not isinstance(param_name, str) or not param_name:
                    msg = f"{override_context}.params keys must be non-empty strings"
                    raise ValueError(msg)
                params[param_name] = _parse_type(param_raw, context=f"{override_context}.params[{param_name}]", known_handles=known_handles)
            type_overrides[function_name] = DiscoveryTypeOverrideSpec(returns=returns, params=params)
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
                type_overrides=type_overrides,
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


def _cpp_type_variants(cpp_type: str | DiscoveredCppType) -> tuple[str, ...]:
    if isinstance(cpp_type, DiscoveredCppType):
        ordered = [
            cpp_type.canonical_spelling,
            cpp_type.normalized_desugared_spelling,
            cpp_type.normalized_spelling,
            cpp_type.desugared_spelling,
            cpp_type.spelling,
        ]
    else:
        normalized = _normalize_cpp_type(cpp_type)
        ordered = [normalized, cpp_type]
    variants: list[str] = []
    seen: set[str] = set()
    for value in ordered:
        if not value:
            continue
        normalized_value = _normalize_cpp_type(value)
        if normalized_value in seen:
            continue
        seen.add(normalized_value)
        variants.append(normalized_value)
    return tuple(variants)


def _cpp_type_primary(cpp_type: str | DiscoveredCppType) -> str:
    variants = _cpp_type_variants(cpp_type)
    return variants[0] if variants else ""


def _base_cpp_type(cpp_type: str | DiscoveredCppType) -> str:
    if isinstance(cpp_type, DiscoveredCppType):
        return cpp_type.base_name
    return _normalize_cpp_type(cpp_type).removesuffix("&").removesuffix("*").strip()


def _cpp_type_bases(cpp_type: str | DiscoveredCppType) -> set[str]:
    if isinstance(cpp_type, DiscoveredCppType):
        bases = {_base_cpp_type(cpp_type)}
        if cpp_type.normalized_spelling:
            bases.add(_base_cpp_type(cpp_type.normalized_spelling))
        if cpp_type.normalized_desugared_spelling:
            bases.add(_base_cpp_type(cpp_type.normalized_desugared_spelling))
        return {base for base in bases if base}
    return {_base_cpp_type(cpp_type)}


def _cpp_type_debug(cpp_type: str | DiscoveredCppType) -> str:
    if isinstance(cpp_type, DiscoveredCppType):
        if cpp_type.desugared_spelling and _normalize_cpp_type(cpp_type.desugared_spelling) != _normalize_cpp_type(cpp_type.spelling):
            return f"{cpp_type.spelling} -> {cpp_type.desugared_spelling}"
        return cpp_type.spelling
    return cpp_type


def _cpp_type_storage(cpp_type: str | DiscoveredCppType) -> str:
    if isinstance(cpp_type, DiscoveredCppType):
        return cpp_type.storage_spelling or cpp_type.canonical_spelling or cpp_type.normalized_spelling or cpp_type.spelling
    return _normalize_cpp_type(cpp_type)


def _qualified_name_suffixes(name: str) -> tuple[str, ...]:
    normalized = _normalize_cpp_type(name)
    parts = [part for part in normalized.split("::") if part]
    if not parts:
        return tuple()
    return tuple("::".join(parts[index:]) for index in range(len(parts)))


def _cpp_type_names_match(candidate: str, target: str) -> bool:
    candidate_suffixes = set(_qualified_name_suffixes(candidate))
    target_suffixes = set(_qualified_name_suffixes(target))
    return bool(candidate_suffixes and target_suffixes and candidate_suffixes.intersection(target_suffixes))


def _find_handle_for_cpp_type(cpp_type: str | DiscoveredCppType, handles: dict[str, HandleSpec]) -> str | None:
    bases = [
        variant.removesuffix("&").removesuffix("*").strip().removeprefix("const ")
        for variant in _cpp_type_variants(cpp_type)
    ]
    seen_bases: set[str] = set()
    ordered_bases: list[str] = []
    for base in bases:
        if base and base not in seen_bases:
            seen_bases.add(base)
            ordered_bases.append(base)
    fallback_bases: tuple[str, ...] = ()
    if isinstance(cpp_type, DiscoveredCppType):
        fallback_bases = tuple(base for base in cpp_type.base_record_names if base and base not in seen_bases)

    for base in (*ordered_bases, *fallback_bases):
        for handle_name, handle in handles.items():
            if _cpp_type_names_match(handle.cpp_type, base):
                return handle_name
        if base.endswith("::ptr"):
            sp_base = base.removesuffix("::ptr")
            for handle_name, handle in handles.items():
                if _cpp_type_names_match(handle.cpp_type, sp_base):
                    return handle_name
        m = re.match(r"(?:const\s+)?(?:(?:std|boost)::)?shared_ptr<\s*(.+?)\s*>", base)
        if m:
            inner = m.group(1).removeprefix("const ").strip()
            for handle_name, handle in handles.items():
                if _cpp_type_names_match(handle.cpp_type, inner):
                    return handle_name
    return None


def _type_spec_from_record_semantic(
    semantic: RecordSemanticType,
    *,
    handles: dict[str, HandleSpec],
    ownership: str,
) -> TypeSpec | None:
    for match_name in semantic_record_match_names(semantic):
        handle_name = _find_handle_for_cpp_type(match_name, handles)
        if handle_name is not None:
            resolved_ownership = "owned" if semantic.pointer_wrapper == "unique_ptr" else ownership
            return TypeSpec(
                kind="handle",
                handle=handle_name,
                ownership=resolved_ownership,
                nullable=_normalize_cpp_type(semantic.cpp_type).endswith("*"),
                cpp_type=semantic.cpp_type,
            )
    return None


def _sequence_scalar_kind(semantic: ScalarSemanticType | StringSemanticType) -> str | None:
    if isinstance(semantic, StringSemanticType):
        return _scalar_sequence_kind("string", 1)
    return _scalar_sequence_kind(semantic.family, 1)


def _sequence_leaf_kind(semantic: ScalarSemanticType | StringSemanticType) -> str:
    if isinstance(semantic, StringSemanticType):
        return "string"
    return semantic.family


def _lower_generic_sequence_type(
    semantic: SequenceSemanticType,
    *,
    handles: dict[str, HandleSpec],
    ownership: str,
) -> TypeSpec | None:
    leaf = semantic_leaf_type(semantic)
    depth = semantic_sequence_depth(semantic)

    if isinstance(leaf, RecordSemanticType):
        record_spec = _type_spec_from_record_semantic(leaf, handles=handles, ownership=ownership)
        if record_spec is None:
            return None
        return TypeSpec(
            kind="handle",
            handle=record_spec.handle,
            ownership=ownership,
            cpp_type=semantic.cpp_type,
            sequence_depth=depth,
        )

    if isinstance(leaf, StringSemanticType):
        return TypeSpec(kind="string", ownership="copy", cpp_type=semantic.cpp_type, sequence_depth=depth)

    if isinstance(leaf, ScalarSemanticType):
        if leaf.family in _SCALAR_SEQUENCE_FAMILIES:
            return TypeSpec(kind=_sequence_leaf_kind(leaf), ownership="copy", cpp_type=semantic.cpp_type, sequence_depth=depth)
    return None


def _infer_type(cpp_type: str | DiscoveredCppType, handles: dict[str, HandleSpec], *, ownership: str) -> TypeSpec:
    semantic = analyze_cpp_type(cpp_type)
    if isinstance(semantic, VoidSemanticType):
        return TypeSpec(kind="void", cpp_type=_cpp_type_storage(cpp_type))
    if isinstance(semantic, EnumSemanticType):
        return TypeSpec(kind="int32", cpp_type=_cpp_type_storage(cpp_type))
    if isinstance(semantic, StringSemanticType):
        return TypeSpec(kind="string", ownership="copy", cpp_type=_cpp_type_storage(cpp_type))
    if isinstance(semantic, ScalarSemanticType):
        scalar_kind = {
            "bool": "bool",
            "int32": "int32",
            "int64": "int64",
            "uint32": "uint32",
            "size": "size",
            "double": "double",
        }.get(semantic.family)
        if scalar_kind is not None:
            return TypeSpec(kind=scalar_kind, cpp_type=_cpp_type_storage(cpp_type))
    if isinstance(semantic, RecordSemanticType):
        record_spec = _type_spec_from_record_semantic(semantic, handles=handles, ownership=ownership)
        if record_spec is not None:
            return record_spec
    if isinstance(semantic, SequenceSemanticType):
        sequence_spec = _lower_generic_sequence_type(semantic, handles=handles, ownership=ownership)
        if sequence_spec is not None:
            return sequence_spec

    msg = f"Unsupported discovered type '{_cpp_type_debug(cpp_type)}'"
    raise ValueError(msg)


def _infer_return_type(cpp_type: str | DiscoveredCppType, handles: dict[str, HandleSpec]) -> TypeSpec:
    try:
        return _infer_type(cpp_type, handles, ownership="borrowed")
    except ValueError as exc:
        msg = str(exc).replace("Unsupported discovered type", "Unsupported discovered return type")
        raise ValueError(msg) from exc


def _infer_param_type(cpp_type: str | DiscoveredCppType, handles: dict[str, HandleSpec]) -> TypeSpec:
    try:
        return _infer_type(cpp_type, handles, ownership="borrowed")
    except ValueError as exc:
        msg = str(exc).replace("Unsupported discovered type", "Unsupported discovered parameter type")
        raise ValueError(msg) from exc


def _apply_method_type_override(
    discovered: DiscoveredMethod,
    *,
    override: DiscoveryTypeOverrideSpec | None,
    inferred_returns: TypeSpec,
    inferred_params: tuple[ParamSpec, ...],
) -> tuple[TypeSpec, tuple[ParamSpec, ...]]:
    if override is None:
        return inferred_returns, inferred_params
    returns = _merge_type_override(inferred_returns, override.returns)
    param_names = {param.name for param in discovered.params}
    unknown = set(override.params) - param_names
    if unknown:
        msg = f"type override for '{discovered.cpp_name}' references unknown params {sorted(unknown)}"
        raise ValueError(msg)
    params = tuple(
        ParamSpec(
            name=param.name,
            type=_merge_type_override(inferred_param.type, override.params.get(param.name)),
        )
        for param, inferred_param in zip(discovered.params, inferred_params, strict=True)
    )
    return returns, params


def _apply_function_type_override(
    discovered: DiscoveredFunction,
    *,
    override: DiscoveryTypeOverrideSpec | None,
    inferred_returns: TypeSpec,
    inferred_params: tuple[ParamSpec, ...],
) -> tuple[TypeSpec, tuple[ParamSpec, ...]]:
    if override is None:
        return inferred_returns, inferred_params
    returns = _merge_type_override(inferred_returns, override.returns)
    param_names = {param.name for param in discovered.params}
    unknown = set(override.params) - param_names
    if unknown:
        msg = f"type override for function '{discovered.cpp_name}' references unknown params {sorted(unknown)}"
        raise ValueError(msg)
    params = tuple(
        ParamSpec(
            name=param.name,
            type=_merge_type_override(inferred_param.type, override.params.get(param.name)),
        )
        for param, inferred_param in zip(discovered.params, inferred_params, strict=True)
    )
    return returns, params


def _apply_field_type_override(
    field: DiscoveredField,
    *,
    override: DiscoveryTypeOverrideSpec | None,
    inferred_returns: TypeSpec,
) -> TypeSpec:
    if override is None:
        return inferred_returns
    if override.params:
        msg = f"type override for field '{field.cpp_name}' cannot define params"
        raise ValueError(msg)
    return _merge_type_override(inferred_returns, override.returns)


def _merge_type_override(inferred: TypeSpec, override: TypeSpec | None) -> TypeSpec:
    if override is None:
        return inferred
    return TypeSpec(
        kind=override.kind or inferred.kind,
        handle=override.handle if override.handle is not None else inferred.handle,
        ownership=override.ownership if override.ownership is not None else inferred.ownership,
        nullable=override.nullable,
        cpp_type=override.cpp_type if override.cpp_type is not None else inferred.cpp_type,
        sequence_depth=override.sequence_depth if override.sequence_depth != 0 else inferred.sequence_depth,
    )


def _infer_method_signature(
    discovered: DiscoveredMethod,
    *,
    handles: dict[str, HandleSpec],
    enum_types_as_int32: frozenset[str],
    override: DiscoveryTypeOverrideSpec | None,
) -> tuple[TypeSpec, tuple[ParamSpec, ...]]:
    if override is not None and override.returns is not None:
        returns = _merge_type_override(
            TypeSpec(kind=override.returns.kind, cpp_type=_cpp_type_storage(discovered.return_type_ref)),
            override.returns,
        )
    else:
        ret_type = discovered.return_cpp_type
        if _normalize_cpp_type(ret_type) in enum_types_as_int32:
            returns = TypeSpec(kind="int32", cpp_type=_cpp_type_storage(discovered.return_type_ref))
        else:
            returns = _infer_return_type(discovered.return_type_ref, handles)

    params_list: list[ParamSpec] = []
    override_params = override.params if override is not None else {}
    for param in discovered.params:
        param_type = override_params.get(param.name)
        if param_type is None:
            param_type = _infer_param_type(param.cpp_type_ref, handles)
        else:
            param_type = _merge_type_override(
                TypeSpec(kind=param_type.kind, cpp_type=_cpp_type_storage(param.cpp_type_ref)),
                param_type,
            )
        params_list.append(ParamSpec(name=param.name, type=param_type))

    return _apply_method_type_override(
        discovered,
        override=override,
        inferred_returns=returns,
        inferred_params=tuple(params_list),
    )


def _make_c_name(handle: HandleSpec, expose_as: str) -> str:
    receiver = handle.c_type.removeprefix("ifcopenshell_").removesuffix("_t")
    return f"ifcopenshell_{receiver}_{expose_as}"


def _simple_type_spec(kind_str: str) -> TypeSpec:
    """Create a TypeSpec from a simple kind string (e.g. 'int32', 'double_list')."""
    parts = _sequence_kind_parts(kind_str)
    if parts is not None:
        return TypeSpec(kind=parts[0], ownership="copy", sequence_depth=parts[1])
    return TypeSpec(kind=kind_str)


def _make_function_c_name(prefix: str, expose_as: str) -> str:
    return f"{prefix}_{expose_as}"


def _append_discovery_diagnostic(
    diagnostics: list[DiscoveryDiagnostic],
    *,
    owner: str,
    member: str,
    code: str,
    message: str,
) -> None:
    diagnostics.append(
        DiscoveryDiagnostic(owner=owner, member=member, code=code, message=message)
    )


def _register_generated_call(
    call: CallSpec,
    *,
    calls: list[CallSpec],
    calls_by_c_name: dict[str, CallSpec],
    reserved_c_names: frozenset[str] | set[str],
    diagnostics: list[DiscoveryDiagnostic],
    owner: str,
    member: str,
) -> bool:
    if call.c_name in reserved_c_names:
        _append_discovery_diagnostic(
            diagnostics,
            owner=owner,
            member=member,
            code="reserved_c_name",
            message=f"Skipped generated call '{call.c_name}' because an authored entry takes precedence",
        )
        return False
    existing = calls_by_c_name.get(call.c_name)
    if existing is not None:
        if existing != call:
            _append_discovery_diagnostic(
                diagnostics,
                owner=owner,
                member=member,
                code="generated_name_collision",
                message=f"Skipped generated call '{call.c_name}' because another generated call already uses that name",
            )
        return False
    calls_by_c_name[call.c_name] = call
    calls.append(call)
    return True


def _register_policy_call(
    policy_call: PolicyCallSpec,
    *,
    handle: HandleSpec | None,
    c_prefix: str = "ifcopenshell",
    calls: list[CallSpec],
    calls_by_c_name: dict[str, CallSpec],
    reserved_c_names: frozenset[str] | set[str],
    diagnostics: list[DiscoveryDiagnostic],
    owner: str,
    member: str,
) -> bool:
    if handle is None:
        c_name = _make_function_c_name(c_prefix, policy_call.expose_as)
    else:
        c_name = _make_c_name(handle, policy_call.expose_as)
    call = _materialize_policy_call(policy_call, c_name=c_name)
    return _register_generated_call(
        call,
        calls=calls,
        calls_by_c_name=calls_by_c_name,
        reserved_c_names=reserved_c_names,
        diagnostics=diagnostics,
        owner=owner,
        member=member,
    )


def _materialize_policy_call(
    call: PolicyCallSpec,
    *,
    c_name: str,
) -> CallSpec:
    return CallSpec(
        expose_as=call.expose_as,
        c_name=c_name,
        receiver=call.receiver,
        returns=call.returns,
        params=call.params,
        policy_operation=call.operation,
    )


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


def _extract_optional_inner_type(cpp_type: str | DiscoveredCppType) -> str | DiscoveredCppType | None:
    """Extract T from boost::optional<T>. Returns None if not an optional type."""
    if isinstance(cpp_type, DiscoveredCppType):
        if cpp_type.template_name == "boost::optional" and len(cpp_type.template_args) == 1:
            return cpp_type.template_args[0]
        normalized = cpp_type.canonical_spelling
    else:
        normalized = _normalize_cpp_type(cpp_type)
    if normalized.startswith("boost::optional<") and normalized.endswith(">"):
        return normalized[len("boost::optional<"):-1].strip()
    return None


def _method_signature_debug(method: DiscoveredMethod) -> str:
    params = ", ".join(_cpp_type_debug(param.cpp_type_ref) for param in method.params)
    return f"{_cpp_type_debug(method.return_type_ref)} {method.cpp_name}({params})"


def _function_signature_debug(function: DiscoveredFunction) -> str:
    params = ", ".join(_cpp_type_debug(param.cpp_type_ref) for param in function.params)
    return f"{_cpp_type_debug(function.return_type_ref)} {function.cpp_name}({params})"


def _emit_optional_field_calls(
    item: DiscoveryClassSpec,
    handle: HandleSpec,
    field_name: str,
    cpp_name: str,
    inner_cpp_type: str,
    handles: dict[str, HandleSpec],
    calls: list[CallSpec],
    calls_by_c_name: dict[str, CallSpec],
    reserved_c_names: frozenset[str] | set[str],
    diagnostics: list[DiscoveryDiagnostic],
) -> None:
    """Generate has_X / X pair for a boost::optional<T> field."""
    try:
        returns = _infer_return_type(inner_cpp_type, handles)
    except ValueError:
        _append_discovery_diagnostic(
            diagnostics,
            owner=item.handle,
            member=field_name,
            code="unsupported_optional_inner_type",
            message=f"Skipped optional field '{field_name}' because inner type '{inner_cpp_type}' is not supported",
        )
        return

    expose_as = item.rename.get(field_name, _snake_case_identifier(field_name))

    # has_X
    has_expose = f"has_{expose_as}"
    has_call = PolicyCallSpec(
        expose_as=has_expose,
        receiver=item.handle,
        returns=TypeSpec(kind="bool", cpp_type=None),
        params=(),
        operation=OptionalHasPolicyOp(field_name=cpp_name),
    )
    _register_policy_call(
        has_call,
        handle=handle,
        calls=calls,
        calls_by_c_name=calls_by_c_name,
        reserved_c_names=reserved_c_names,
        diagnostics=diagnostics,
        owner=item.handle,
        member=field_name,
    )

    # X (the getter)
    get_call = PolicyCallSpec(
        expose_as=expose_as,
        receiver=item.handle,
        returns=returns,
        params=(),
        operation=OptionalGetPolicyOp(field_name=cpp_name),
    )
    _register_policy_call(
        get_call,
        handle=handle,
        calls=calls,
        calls_by_c_name=calls_by_c_name,
        reserved_c_names=reserved_c_names,
        diagnostics=diagnostics,
        owner=item.handle,
        member=field_name,
    )


def _emit_children_calls(
    item: DiscoveryClassSpec,
    handle: HandleSpec,
    calls: list[CallSpec],
    calls_by_c_name: dict[str, CallSpec],
    reserved_c_names: frozenset[str] | set[str],
    diagnostics: list[DiscoveryDiagnostic],
) -> None:
    dc = item.discover_children
    if dc is None:
        return

    count_call = PolicyCallSpec(
        expose_as=dc.count_as,
        receiver=item.handle,
        returns=TypeSpec(kind="size", handle=None, ownership=None, nullable=False, cpp_type=None),
        params=(),
        operation=ChildrenCountPolicyOp(field_name=dc.cpp_field),
    )
    _register_policy_call(
        count_call,
        handle=handle,
        calls=calls,
        calls_by_c_name=calls_by_c_name,
        reserved_c_names=reserved_c_names,
        diagnostics=diagnostics,
        owner=item.handle,
        member=dc.cpp_field,
    )

    at_call = PolicyCallSpec(
        expose_as=dc.at_as,
        receiver=item.handle,
        returns=TypeSpec(kind="handle", handle=dc.element_handle, ownership="borrowed", nullable=False, cpp_type=None),
        params=(ParamSpec(name="index", type=TypeSpec(kind="size", handle=None, ownership=None, nullable=False, cpp_type=None)),),
        operation=ChildrenAtPolicyOp(field_name=dc.cpp_field),
    )
    _register_policy_call(
        at_call,
        handle=handle,
        calls=calls,
        calls_by_c_name=calls_by_c_name,
        reserved_c_names=reserved_c_names,
        diagnostics=diagnostics,
        owner=item.handle,
        member=dc.cpp_field,
    )

    if dc.add_as is None:
        return

    add_call = PolicyCallSpec(
        expose_as=dc.add_as,
        receiver=item.handle,
        returns=TypeSpec(kind="void"),
        params=(ParamSpec(name="item", type=TypeSpec(kind="handle", handle=dc.element_handle, ownership="borrowed")),),
        operation=ChildrenAddPolicyOp(field_name=dc.cpp_field, cast_cpp_type=dc.add_cast_cpp_type),
    )
    _register_policy_call(
        add_call,
        handle=handle,
        calls=calls,
        calls_by_c_name=calls_by_c_name,
        reserved_c_names=reserved_c_names,
        diagnostics=diagnostics,
        owner=item.handle,
        member=dc.cpp_field,
    )


def _emit_ccomponents_call(
    item: DiscoveryClassSpec,
    handle: HandleSpec,
    calls: list[CallSpec],
    calls_by_c_name: dict[str, CallSpec],
    reserved_c_names: frozenset[str] | set[str],
    diagnostics: list[DiscoveryDiagnostic],
) -> None:
    cc = item.ccomponents_accessor
    if cc is None:
        return
    cc_call = PolicyCallSpec(
        expose_as=cc.expose_as,
        receiver=item.handle,
        returns=TypeSpec(kind="double", ownership="copy", sequence_depth=1),
        params=(),
        operation=CcomponentsAccessorPolicyOp(access_via=cc.access_via, dimensions=cc.dimensions),
    )
    _register_policy_call(
        cc_call,
        handle=handle,
        calls=calls,
        calls_by_c_name=calls_by_c_name,
        reserved_c_names=reserved_c_names,
        diagnostics=diagnostics,
        owner=item.handle,
        member=cc.access_via,
    )


def _emit_variant_accessor_calls(
    item: DiscoveryClassSpec,
    handle: HandleSpec,
    calls: list[CallSpec],
    calls_by_c_name: dict[str, CallSpec],
    reserved_c_names: frozenset[str] | set[str],
    diagnostics: list[DiscoveryDiagnostic],
) -> None:
    va = item.variant_accessors
    if va is None:
        return

    type_map = {
        "bool": "bool",
        "int": "int32",
        "int64_t": "int64",
        "std::int64_t": "int64",
        "double": "double",
        "std::string": "string",
    }
    for suffix, type_spec in va.types.items():
        cpp_type = type_spec.cpp_type
        ret_kind = type_map.get(cpp_type)
        if ret_kind is None:
            _append_discovery_diagnostic(
                diagnostics,
                owner=item.handle,
                member=suffix,
                code="unsupported_variant_accessor_type",
                message=f"Skipped variant accessor '{suffix}' because type '{cpp_type}' is not supported",
            )
            continue

        get_expose = f"get_{suffix}"
        get_call = PolicyCallSpec(
            expose_as=get_expose,
            receiver=item.handle,
            returns=TypeSpec(kind=ret_kind),
            params=(ParamSpec(name="name", type=TypeSpec(kind="string")),),
            operation=VariantGetPolicyOp(method_name=va.get_method, cpp_type=cpp_type, getter_types=type_spec.getter_types),
        )
        _register_policy_call(
            get_call,
            handle=handle,
            calls=calls,
            calls_by_c_name=calls_by_c_name,
            reserved_c_names=reserved_c_names,
            diagnostics=diagnostics,
            owner=item.handle,
            member=suffix,
        )

        set_expose = f"set_{suffix}"
        set_call = PolicyCallSpec(
            expose_as=set_expose,
            receiver=item.handle,
            returns=TypeSpec(kind="void"),
            params=(
                ParamSpec(name="name", type=TypeSpec(kind="string")),
                ParamSpec(name="value", type=TypeSpec(kind=ret_kind)),
            ),
            operation=VariantSetPolicyOp(method_name=va.set_method, variant_type=va.variant_type, cpp_type=cpp_type),
        )
        _register_policy_call(
            set_call,
            handle=handle,
            calls=calls,
            calls_by_c_name=calls_by_c_name,
            reserved_c_names=reserved_c_names,
            diagnostics=diagnostics,
            owner=item.handle,
            member=suffix,
        )


def _discover_method_calls(
    spec_path: Path,
    discovery: DiscoverySpec,
    handles: dict[str, HandleSpec],
    compile_commands_path: Path,
    authored_c_names: frozenset[str] = frozenset(),
) -> tuple[tuple[CallSpec, ...], tuple[DiscoveryDiagnostic, ...]]:
    include_dir = (spec_path.parent / discovery.include_dir).resolve()
    class_cache: dict[tuple[str, str], dict[str, tuple[DiscoveredMethod, ...]]] = {}
    calls: list[CallSpec] = []
    calls_by_c_name: dict[str, CallSpec] = {}
    reserved_c_names: set[str] = set(authored_c_names)
    diagnostics: list[DiscoveryDiagnostic] = []

    debug_log(
        "spec.discover_methods.start",
        f"spec={debug_path(spec_path)} classes={len(discovery.classes)} include_dir={debug_path(include_dir)}",
    )

    for item_index, item in enumerate(discovery.classes, start=1):
        handle = handles[item.handle]
        excluded = set(item.exclude)
        debug_log(
            "spec.discover_methods.class",
            f"{item_index}/{len(discovery.classes)} handle={item.handle} cpp={handle.cpp_type} tu={item.translation_unit}",
        )

        # Only run AST method discovery if there's something to discover
        needs_method_discovery = item.include_all or item.include or item.overloads
        methods_by_name: dict[str, tuple[DiscoveredMethod, ...]] = {}
        if needs_method_discovery:
            cache_key = (handle.cpp_type, item.translation_unit, item.include_inherited_methods)
            cached = class_cache.get(cache_key)
            if cached is None:
                translation_unit = (include_dir / item.translation_unit).resolve()
                cached = discover_public_methods_with_compile_commands(
                    compile_commands_path, translation_unit, handle.cpp_type,
                    include_inherited=item.include_inherited_methods,
                )
                class_cache[cache_key] = cached
            methods_by_name = cached

        for overload_spec in item.overloads:
            overloads = methods_by_name.get(overload_spec.cpp_name)
            if overloads is None:
                msg = f"Unable to discover method '{overload_spec.cpp_name}' on handle '{item.handle}'"
                raise ValueError(msg)
            discovered = _select_overload(overloads, overload_spec)
            returns, params = _infer_method_signature(
                discovered,
                handles=handles,
                enum_types_as_int32=item.enum_types_as_int32,
                override=item.type_overrides.get(overload_spec.cpp_name),
            )
            call = CallSpec(
                expose_as=overload_spec.expose_as,
                c_name=_make_c_name(handle, overload_spec.expose_as),
                receiver=item.handle,
                returns=returns,
                params=params,
                policy_operation=DirectMethodPolicyOp(cpp_name=discovered.cpp_name),
            )
            existing = calls_by_c_name.get(call.c_name)
            if call.c_name in reserved_c_names:
                _append_discovery_diagnostic(
                    diagnostics,
                    owner=item.handle,
                    member=overload_spec.cpp_name,
                    code="reserved_c_name",
                    message=f"Skipped discovered method '{overload_spec.cpp_name}' because an authored entry takes precedence",
                )
                continue  # Authored entry takes precedence
            if existing is not None:
                if existing == call:
                    continue
                msg = f"Discovered method collision for generated name '{call.c_name}'"
                raise ValueError(msg)
            calls_by_c_name[call.c_name] = call
            calls.append(call)

        explicit_includes = set(item.include)
        candidate_names = set(methods_by_name) if item.include_all else set()
        candidate_names.update(explicit_includes)
        candidate_names.difference_update(excluded)
        # Auto-skip C++ operators and destructors (invalid C identifiers)
        if item.include_all:
            candidate_names = {n for n in candidate_names if not n.startswith("operator") and not n.startswith("~")}

        for cpp_name in sorted(candidate_names):
            overloads = methods_by_name.get(cpp_name)
            if overloads is None:
                msg = f"Unable to discover method '{cpp_name}' on handle '{item.handle}'"
                raise ValueError(msg)
            is_explicit = cpp_name in explicit_includes
            if len(overloads) != 1:
                if item.include_all and not is_explicit:
                    _append_discovery_diagnostic(
                        diagnostics,
                        owner=item.handle,
                        member=cpp_name,
                        code="overloaded_method_requires_policy",
                        message=f"Skipped auto-discovered method '{cpp_name}' because it is overloaded",
                    )
                    continue
                msg = f"Method '{cpp_name}' on handle '{item.handle}' is overloaded and requires an explicit policy"
                raise ValueError(msg)

            discovered = overloads[0]
            try:
                returns, params = _infer_method_signature(
                    discovered,
                    handles=handles,
                    enum_types_as_int32=item.enum_types_as_int32,
                    override=item.type_overrides.get(cpp_name),
                )
            except ValueError as exc:
                if item.include_all and not is_explicit:
                    _append_discovery_diagnostic(
                        diagnostics,
                        owner=item.handle,
                        member=cpp_name,
                        code="unsupported_method_signature",
                        message=(
                            f"Skipped auto-discovered method '{cpp_name}' because signature "
                            f"'{_method_signature_debug(discovered)}' is not supported: {exc}"
                        ),
                    )
                    continue
                raise

            expose_as = item.rename.get(cpp_name, _snake_case_identifier(cpp_name))
            call = CallSpec(
                expose_as=expose_as,
                c_name=_make_c_name(handle, expose_as),
                receiver=item.handle,
                returns=returns,
                params=params,
                policy_operation=DirectMethodPolicyOp(cpp_name=discovered.cpp_name),
            )
            existing = calls_by_c_name.get(call.c_name)
            if call.c_name in reserved_c_names:
                _append_discovery_diagnostic(
                    diagnostics,
                    owner=item.handle,
                    member=cpp_name,
                    code="reserved_c_name",
                    message=f"Skipped discovered method '{cpp_name}' because an authored entry takes precedence",
                )
                continue  # Authored entry takes precedence
            if existing is not None:
                if existing == call:
                    continue
                msg = f"Discovered method collision for generated name '{call.c_name}'"
                raise ValueError(msg)
            calls_by_c_name[call.c_name] = call
            calls.append(call)

        # Discover public fields if requested
        if item.discover_fields:
            field_cache_key = ("fields", handle.cpp_type, item.translation_unit, item.include_inherited_fields)
            fields_by_name = class_cache.get(field_cache_key)
            if fields_by_name is None:
                translation_unit = (include_dir / item.translation_unit).resolve()
                fields_by_name = discover_public_fields_with_compile_commands(
                    compile_commands_path, translation_unit, handle.cpp_type,
                    include_inherited=item.include_inherited_fields,
                )
                class_cache[field_cache_key] = fields_by_name

            for field_name in sorted(fields_by_name):
                if field_name in excluded:
                    continue
                field = fields_by_name[field_name]

                # Handle boost::optional<T> fields
                optional_inner = _extract_optional_inner_type(field.cpp_type_ref)
                if optional_inner is not None:
                    if item.discover_optional_fields:
                        _emit_optional_field_calls(
                            item, handle, field_name, field.cpp_name, optional_inner,
                            handles, calls, calls_by_c_name, reserved_c_names, diagnostics,
                        )
                    continue

                try:
                    field_override = item.type_overrides.get(field_name)
                    if field_override is not None and field_override.returns is not None:
                        returns = field_override.returns
                    else:
                        cpp_type = field.cpp_type
                        if _normalize_cpp_type(cpp_type) in item.enum_types_as_int32:
                            returns = TypeSpec(kind="int32", cpp_type=_cpp_type_storage(field.cpp_type_ref))
                        else:
                            returns = _infer_return_type(field.cpp_type_ref, handles)
                    returns = _apply_field_type_override(
                        field,
                        override=field_override,
                        inferred_returns=returns,
                    )
                except ValueError as exc:
                    _append_discovery_diagnostic(
                        diagnostics,
                        owner=item.handle,
                        member=field_name,
                        code="unsupported_field_type",
                        message=(
                            f"Skipped auto-discovered field '{field_name}' because type "
                            f"'{_cpp_type_debug(field.cpp_type_ref)}' is not supported: {exc}"
                        ),
                    )
                    continue

                # Determine if this is a value-typed handle field (not a pointer/shared_ptr).
                # Value handle fields need make_shared wrapping; pointer fields use direct access.
                field_kind = "field"
                if returns.kind == "handle":
                    normalized_field_type = _normalize_cpp_type(field.cpp_type)
                    is_pointer_field = (
                        normalized_field_type.endswith("*")
                        or normalized_field_type.endswith("::ptr")
                        or "shared_ptr" in normalized_field_type
                    )
                    if not is_pointer_field:
                        field_kind = "value_handle_field"
                        returns = TypeSpec(
                            kind="handle", handle=returns.handle,
                            ownership="owned", nullable=False, cpp_type=returns.cpp_type,
                        )

                expose_as = item.rename.get(field_name, _snake_case_identifier(field_name))
                call = CallSpec(
                    expose_as=expose_as,
                    c_name=_make_c_name(handle, expose_as),
                    receiver=item.handle,
                    returns=returns,
                    params=(),
                    policy_operation=(
                        DirectFieldPolicyOp(field_name=field.cpp_name)
                        if field_kind == "field"
                        else ValueHandleFieldPolicyOp(field_name=field.cpp_name)
                    ),
                )
                existing = calls_by_c_name.get(call.c_name)
                if existing is not None or call.c_name in reserved_c_names:
                    _append_discovery_diagnostic(
                        diagnostics,
                        owner=item.handle,
                        member=field_name,
                        code="field_name_collision",
                        message=f"Skipped generated field accessor '{call.c_name}' because another call already uses that name",
                    )
                    continue
                calls_by_c_name[call.c_name] = call
                calls.append(call)

                # For nullable handle fields, optionally generate has_X
                if item.discover_has_fields and field_kind == "field" and returns.kind == "handle":
                    has_expose = f"has_{expose_as}"
                    has_call = PolicyCallSpec(
                        expose_as=has_expose,
                        receiver=item.handle,
                        returns=TypeSpec(kind="bool", cpp_type=None),
                        params=(),
                        operation=PointerPresencePolicyOp(field_name=field.cpp_name),
                    )
                    _register_policy_call(
                        has_call,
                        handle=handle,
                        calls=calls,
                        calls_by_c_name=calls_by_c_name,
                        reserved_c_names=reserved_c_names,
                        diagnostics=diagnostics,
                        owner=item.handle,
                        member=field_name,
                    )

        _emit_children_calls(
            item,
            handle,
            calls,
            calls_by_c_name,
            reserved_c_names,
            diagnostics,
        )

        # Generate extra fields (manually specified for template classes)
        for field_name, cpp_type in item.extra_fields.items():
            if field_name in excluded:
                continue
            try:
                returns = _infer_return_type(cpp_type, handles)
            except ValueError:
                msg = f"Cannot infer return type for extra_field '{field_name}' with cpp_type '{cpp_type}' on {item.handle}"
                raise ValueError(msg)

            normalized = _normalize_cpp_type(cpp_type)
            is_pointer = (
                normalized.endswith("*")
                or normalized.endswith("::ptr")
                or "shared_ptr" in normalized
            )
            field_kind = "field" if is_pointer else "value_handle_field"
            if field_kind == "value_handle_field" and returns.kind == "handle":
                returns = TypeSpec(
                    kind="handle", handle=returns.handle,
                    ownership="owned", nullable=False, cpp_type=returns.cpp_type,
                )

            expose_as = item.rename.get(field_name, _snake_case_identifier(field_name))
            call = CallSpec(
                expose_as=expose_as,
                c_name=_make_c_name(handle, expose_as),
                receiver=item.handle,
                returns=returns,
                params=(),
                policy_operation=(
                    DirectFieldPolicyOp(field_name=field_name)
                    if field_kind == "field"
                    else ValueHandleFieldPolicyOp(field_name=field_name)
                ),
            )
            _register_generated_call(
                call,
                calls=calls,
                calls_by_c_name=calls_by_c_name,
                reserved_c_names=reserved_c_names,
                diagnostics=diagnostics,
                owner=item.handle,
                member=field_name,
            )

            # Also generate has_X for nullable handle extra fields
            if item.discover_has_fields and field_kind == "field" and returns.kind == "handle":
                has_expose = f"has_{expose_as}"
                has_call = PolicyCallSpec(
                    expose_as=has_expose,
                    receiver=item.handle,
                    returns=TypeSpec(kind="bool", cpp_type=None),
                    params=(),
                    operation=PointerPresencePolicyOp(field_name=field_name),
                )
                _register_policy_call(
                    has_call,
                    handle=handle,
                    calls=calls,
                    calls_by_c_name=calls_by_c_name,
                    reserved_c_names=reserved_c_names,
                    diagnostics=diagnostics,
                    owner=item.handle,
                    member=field_name,
                )

        # Generate field setters
        for setter_field in item.field_setters:
            param_type: TypeSpec | None = None
            if setter_field in item.extra_fields:
                cpp_type_str = item.extra_fields[setter_field]
                try:
                    param_type = _infer_return_type(cpp_type_str, handles)
                except ValueError:
                    pass
            elif item.discover_fields:
                field_cache_key = ("fields", handle.cpp_type, item.translation_unit, item.include_inherited_fields)
                cached = class_cache.get(field_cache_key)
                if cached and setter_field in cached:
                    try:
                        param_type = _infer_return_type(cached[setter_field].cpp_type, handles)
                    except ValueError:
                        pass
            if param_type is None:
                msg = f"Cannot determine type for field_setter '{setter_field}' on {item.handle}"
                raise ValueError(msg)

            set_expose = f"set_{_snake_case_identifier(setter_field)}"
            set_call = PolicyCallSpec(
                expose_as=set_expose,
                receiver=item.handle,
                returns=TypeSpec(kind="void"),
                params=(ParamSpec(name="value", type=param_type),),
                operation=FieldSetterPolicyOp(field_name=setter_field),
            )
            _register_policy_call(
                set_call,
                handle=handle,
                calls=calls,
                calls_by_c_name=calls_by_c_name,
                reserved_c_names=reserved_c_names,
                diagnostics=diagnostics,
                owner=item.handle,
                member=setter_field,
            )

        # Generate method_size calls (method().size())
        for method_name, expose_as in item.method_sizes.items():
            ms_call = PolicyCallSpec(
                expose_as=expose_as,
                receiver=item.handle,
                returns=TypeSpec(kind="size"),
                params=(),
                operation=MethodSizePolicyOp(method_name=method_name),
            )
            _register_policy_call(
                ms_call,
                handle=handle,
                calls=calls,
                calls_by_c_name=calls_by_c_name,
                reserved_c_names=reserved_c_names,
                diagnostics=diagnostics,
                owner=item.handle,
                member=method_name,
            )

        # Generate array_pair_field calls (field[0] as _u, field[1] as _v)
        for field_name, return_kind in item.array_pair_fields.items():
            ret_type = _simple_type_spec(return_kind)
            for suffix, index in [("_u", 0), ("_v", 1)]:
                expose = f"{field_name}{suffix}"
                ap_call = PolicyCallSpec(
                    expose_as=expose,
                    receiver=item.handle,
                    returns=ret_type,
                    params=(),
                    operation=ArrayElementFieldPolicyOp(expression=f"{field_name}[{index}]"),
                )
                _register_policy_call(
                    ap_call,
                    handle=handle,
                    calls=calls,
                    calls_by_c_name=calls_by_c_name,
                    reserved_c_names=reserved_c_names,
                    diagnostics=diagnostics,
                    owner=item.handle,
                    member=field_name,
                )

        # Generate operator calls (binary, unary, comparison)
        def _add_op_call(expose_as: str, call_spec: CallSpec) -> None:
            _register_generated_call(
                call_spec,
                calls=calls,
                calls_by_c_name=calls_by_c_name,
                reserved_c_names=reserved_c_names,
                diagnostics=diagnostics,
                owner=item.handle,
                member=expose_as,
            )

        handle_return_owned = TypeSpec(kind="handle", handle=item.handle, ownership="owned", nullable=False, cpp_type=None)
        other_param = ParamSpec(name="other", type=TypeSpec(kind="handle", handle=item.handle, ownership="borrowed", nullable=False, cpp_type=None))

        for expose_as, op in item.binary_operators.items():
            _add_op_call(expose_as, CallSpec(
                expose_as=expose_as,
                c_name=_make_c_name(handle, expose_as),
                receiver=item.handle,
                returns=handle_return_owned,
                params=(other_param,),
                policy_operation=InlineAdapterPolicyOp(
                    implementation=ImplementationSpec(kind="inline_cpp", body=f"return (*self_cpp) {op} other_cpp;\n")
                ),
            ))

        for expose_as, op in item.unary_operators.items():
            _add_op_call(expose_as, CallSpec(
                expose_as=expose_as,
                c_name=_make_c_name(handle, expose_as),
                receiver=item.handle,
                returns=handle_return_owned,
                params=(),
                policy_operation=InlineAdapterPolicyOp(
                    implementation=ImplementationSpec(kind="inline_cpp", body=f"return {op}(*self_cpp);\n")
                ),
            ))

        for expose_as, op in item.comparison_operators.items():
            _add_op_call(expose_as, CallSpec(
                expose_as=expose_as,
                c_name=_make_c_name(handle, expose_as),
                receiver=item.handle,
                returns=TypeSpec(kind="bool"),
                params=(other_param,),
                policy_operation=InlineAdapterPolicyOp(
                    implementation=ImplementationSpec(kind="inline_cpp", body=f"return (*self_cpp) {op} other_cpp;\n")
                ),
            ))

        # Generate as_item cast if requested
        if item.discover_as_item:
            as_item_call = PolicyCallSpec(
                expose_as="as_item",
                receiver=item.handle,
                returns=TypeSpec(kind="handle", handle="taxonomy_item", ownership="owned", nullable=False, cpp_type=None),
                params=(),
                operation=AsItemCastPolicyOp(),
            )
            _register_policy_call(
                as_item_call,
                handle=handle,
                calls=calls,
                calls_by_c_name=calls_by_c_name,
                reserved_c_names=reserved_c_names,
                diagnostics=diagnostics,
                owner=item.handle,
                member="as_item",
            )

        _emit_ccomponents_call(
            item,
            handle,
            calls,
            calls_by_c_name,
            reserved_c_names,
            diagnostics,
        )
        _emit_variant_accessor_calls(
            item,
            handle,
            calls,
            calls_by_c_name,
            reserved_c_names,
            diagnostics,
        )

    debug_log(
        "spec.discover_methods.done",
        f"spec={debug_path(spec_path)} calls={len(calls)} diagnostics={len(diagnostics)}",
    )
    return tuple(calls), tuple(diagnostics)


def _discover_function_calls(
    spec_path: Path,
    discovery: DiscoverySpec,
    handles: dict[str, HandleSpec],
    c_prefix: str,
    compile_commands_path: Path,
) -> tuple[tuple[CallSpec, ...], tuple[DiscoveryDiagnostic, ...]]:
    include_dir = (spec_path.parent / discovery.include_dir).resolve()
    namespace_cache: dict[tuple[str, str], dict[str, tuple[DiscoveredFunction, ...]]] = {}
    calls: list[CallSpec] = []
    calls_by_c_name: dict[str, CallSpec] = {}
    diagnostics: list[DiscoveryDiagnostic] = []

    debug_log(
        "spec.discover_functions.start",
        f"spec={debug_path(spec_path)} namespaces={len(discovery.functions)} include_dir={debug_path(include_dir)}",
    )

    for item_index, item in enumerate(discovery.functions, start=1):
        debug_log(
            "spec.discover_functions.namespace",
            f"{item_index}/{len(discovery.functions)} namespace={item.namespace} tu={item.translation_unit}",
        )
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
            inferred_returns = _infer_return_type(discovered.return_type_ref, handles)
            inferred_params = tuple(
                ParamSpec(name=param.name, type=_infer_param_type(param.cpp_type_ref, handles))
                for param in discovered.params
            )
            returns, params = _apply_function_type_override(
                discovered,
                override=item.type_overrides.get(overload_spec.cpp_name),
                inferred_returns=inferred_returns,
                inferred_params=inferred_params,
            )
            call = CallSpec(
                expose_as=overload_spec.expose_as,
                c_name=_make_function_c_name(c_prefix, overload_spec.expose_as),
                receiver=None,
                returns=returns,
                params=params,
                policy_operation=DirectFunctionPolicyOp(cpp_name=f"{item.namespace}::{discovered.cpp_name}"),
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
                    _append_discovery_diagnostic(
                        diagnostics,
                        owner=item.namespace,
                        member=cpp_name,
                        code="overloaded_function_requires_policy",
                        message=f"Skipped auto-discovered function '{cpp_name}' because it is overloaded",
                    )
                    continue
                msg = f"Function '{cpp_name}' in namespace '{item.namespace}' is overloaded and requires an explicit policy"
                raise ValueError(msg)

            discovered = overloads[0]
            try:
                inferred_returns = _infer_return_type(discovered.return_type_ref, handles)
                inferred_params = tuple(
                    ParamSpec(name=param.name, type=_infer_param_type(param.cpp_type_ref, handles))
                    for param in discovered.params
                )
                returns, params = _apply_function_type_override(
                    discovered,
                    override=item.type_overrides.get(cpp_name),
                    inferred_returns=inferred_returns,
                    inferred_params=inferred_params,
                )
            except ValueError as exc:
                if item.include_all and not is_explicit:
                    _append_discovery_diagnostic(
                        diagnostics,
                        owner=item.namespace,
                        member=cpp_name,
                        code="unsupported_function_signature",
                        message=(
                            f"Skipped auto-discovered function '{cpp_name}' because signature "
                            f"'{_function_signature_debug(discovered)}' is not supported: {exc}"
                        ),
                    )
                    continue
                raise

            expose_as = item.rename.get(cpp_name, _snake_case_identifier(cpp_name))
            call = CallSpec(
                expose_as=expose_as,
                c_name=_make_function_c_name(c_prefix, expose_as),
                receiver=None,
                returns=returns,
                params=params,
                policy_operation=DirectFunctionPolicyOp(cpp_name=f"{item.namespace}::{discovered.cpp_name}"),
            )
            existing = calls_by_c_name.get(call.c_name)
            if existing is not None:
                if existing == call:
                    continue
                msg = f"Discovered function collision for generated name '{call.c_name}'"
                raise ValueError(msg)
            calls_by_c_name[call.c_name] = call
            calls.append(call)

    debug_log(
        "spec.discover_functions.done",
        f"spec={debug_path(spec_path)} calls={len(calls)} diagnostics={len(diagnostics)}",
    )
    return tuple(calls), tuple(diagnostics)


def load_authored_spec(
    path: Path,
    compile_commands_path: Path | None = None,
    existing_handles: dict[str, HandleSpec] | None = None,
) -> AuthoredBindingSpec:
    """Load a single authored binding spec.
    
    Args:
        path: Path to the YAML spec file.
        compile_commands_path: Optional path to compile_commands.json for AST discovery.
        existing_handles: Optional dict of handles from previously loaded specs.
                         These will be available for reference in this spec.
    """
    debug_log(
        "spec.load.start",
        f"path={debug_path(path)} compile_commands={debug_path(compile_commands_path)} existing_handles={len(existing_handles or {})}",
    )
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
    # Start with existing handles if provided
    if existing_handles:
        handles.update(existing_handles)
    for index, item in enumerate(raw_handles):
        context = f"handles[{index}]"
        mapping = _expect_mapping(item, context)
        destructor = _expect_str(mapping.get("destructor"), f"{context}.destructor")
        ptr_type = mapping.get("ptr_type", "raw")
        if not isinstance(ptr_type, str):
            msg = f"{context}.ptr_type must be a string"
            raise ValueError(msg)
        if ptr_type not in _ALLOWED_PTR_TYPES:
            msg = f"{context}.ptr_type must be one of {sorted(_ALLOWED_PTR_TYPES)}"
            raise ValueError(msg)
        handle = HandleSpec(
            name=_expect_str(mapping.get("name"), f"{context}.name"),
            cpp_type=_expect_str(mapping.get("cpp_type"), f"{context}.cpp_type"),
            c_type=_expect_str(mapping.get("c_type"), f"{context}.c_type"),
            destructor=destructor,
            ptr_type=ptr_type,
        )
        if handle.destructor not in _ALLOWED_DESTRUCTORS and not handle.destructor.startswith("function:"):
            msg = f"{context}.destructor must be one of {sorted(_ALLOWED_DESTRUCTORS)} or function:<qualified_name>"
            raise ValueError(msg)
        if handle.name in handles:
            msg = f"{context}.name '{handle.name}' is duplicated"
            raise ValueError(msg)
        handles[handle.name] = handle

    # Parse imports section
    raw_imports = _expect_list(root.get("imports", []), "imports")
    imports: list[ImportedHandle] = []
    for index, item in enumerate(raw_imports):
        context = f"imports[{index}]"
        mapping = _expect_mapping(item, context)
        slice_name_import = _expect_str(mapping.get("slice"), f"{context}.slice")
        imported_handles = _expect_list(mapping.get("handles", []), f"{context}.handles")
        for handle_index, handle_name in enumerate(imported_handles):
            handle_name = _expect_str(handle_name, f"{context}.handles[{handle_index}]")
            imports.append(ImportedHandle(slice=slice_name_import, handle=handle_name))

    # Imported handles are added to known_handles for type resolution
    known_handles = set(handles)
    for imp in imports:
        known_handles.add(imp.handle)
    discovery = _parse_discovery(root.get("discover"), context="discover", known_handles=known_handles)
    if discovery is not None and compile_commands_path is None:
        msg = "compile_commands.json is required for AST-backed discovery"
        raise ValueError(msg)

    authored_functions = tuple(
        _parse_call(
            item,
            context=f"functions[{index}]",
            c_prefix=c_prefix,
            handles=handles,
            known_handles=known_handles,
            expect_receiver=False,
        )
        for index, item in enumerate(_expect_list(root.get("functions", []), "functions"))
    )
    authored_methods = tuple(
        _parse_call(
            item,
            context=f"methods[{index}]",
            c_prefix=c_prefix,
            handles=handles,
            known_handles=known_handles,
            expect_receiver=True,
        )
        for index, item in enumerate(_expect_list(root.get("methods", []), "methods"))
    )

    # Collect authored c_names so discovery can skip collisions
    authored_c_names = frozenset(c.c_name for c in (*authored_functions, *authored_methods))
    discovered_methods: tuple[CallSpec, ...] = tuple()
    discovered_functions: tuple[CallSpec, ...] = tuple()
    discovery_diagnostics: tuple[DiscoveryDiagnostic, ...] = tuple()
    if discovery is not None:
        discovered_methods, method_diagnostics = _discover_method_calls(
            path, discovery, handles, compile_commands_path, authored_c_names
        )
        discovered_functions, function_diagnostics = _discover_function_calls(
            path, discovery, handles, c_prefix, compile_commands_path
        )
        discovery_diagnostics = method_diagnostics + function_diagnostics

    depends_on_common = root.get("depends_on_common")
    if depends_on_common is not None and not isinstance(depends_on_common, str):
        msg = "depends_on_common must be a string"
        raise ValueError(msg)

    spec = AuthoredBindingSpec(
        schema_version=schema_version,
        module=module,
        slice=slice_name,
        c_prefix=c_prefix,
        public_headers=public_headers,
        handles=handles,
        imports=tuple(imports),
        depends_on_common=depends_on_common,
        discovery=discovery,
        functions=discovered_functions + authored_functions,
        methods=discovered_methods + authored_methods,
        discovery_diagnostics=discovery_diagnostics,
    )
    debug_log(
        "spec.load.done",
        f"path={debug_path(path)} handles={len(spec.handles)} functions={len(spec.functions)} methods={len(spec.methods)} diagnostics={len(spec.discovery_diagnostics)}",
    )
    return spec


def load_merged_specs(
    spec_paths: list[Path],
    module: str,
    c_prefix: str,
    compile_commands_path: Path | None = None,
) -> MergedBindingSpec:
    """Load multiple binding specs and merge them into a unified spec.
    
    Handles from earlier specs are available to later specs automatically.
    This enables cross-module references like ifcgeom using ifcparse::file.
    """
    debug_log(
        "spec.merge.start",
        f"specs={len(spec_paths)} module={module} compile_commands={debug_path(compile_commands_path)}",
    )
    all_handles: dict[str, HandleSpec] = {}
    all_headers: list[str] = []
    all_functions: list[CallSpec] = []
    all_methods: list[CallSpec] = []
    all_diagnostics: list[DiscoveryDiagnostic] = []
    
    for index, spec_path in enumerate(spec_paths, start=1):
        debug_log("spec.merge.item", f"{index}/{len(spec_paths)} path={debug_path(spec_path)}")
        # Load each spec - later specs can reference handles from earlier ones
        spec = load_authored_spec(
            spec_path,
            compile_commands_path=compile_commands_path,
            existing_handles=all_handles.copy(),  # Pass accumulated handles
        )
        
        # Merge new handles (check for collisions with exact match to handle
        # re-definitions gracefully - e.g., if both specs define the same handle)
        for handle_name, handle_spec in spec.handles.items():
            if handle_name in all_handles:
                existing = all_handles[handle_name]
                # Allow re-definitions if they're identical (from existing_handles)
                if existing != handle_spec:
                    msg = f"Handle collision: '{handle_name}' defined differently in multiple specs"
                    raise ValueError(msg)
            else:
                all_handles[handle_name] = handle_spec
        
        # Merge headers (deduplicate)
        for header in spec.public_headers:
            if header not in all_headers:
                all_headers.append(header)
        
        # Merge functions and methods
        all_functions.extend(spec.functions)
        all_methods.extend(spec.methods)
        all_diagnostics.extend(spec.discovery_diagnostics)
    
    merged = MergedBindingSpec(
        module=module,
        c_prefix=c_prefix,
        public_headers=tuple(all_headers),
        handles=all_handles,
        functions=tuple(all_functions),
        methods=tuple(all_methods),
        discovery_diagnostics=tuple(all_diagnostics),
    )
    debug_log(
        "spec.merge.done",
        f"module={module} handles={len(merged.handles)} functions={len(merged.functions)} methods={len(merged.methods)} diagnostics={len(merged.discovery_diagnostics)}",
    )
    return merged
