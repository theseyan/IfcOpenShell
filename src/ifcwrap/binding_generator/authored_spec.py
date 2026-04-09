# This file was generated with the assistance of an AI coding tool.

from __future__ import annotations

from dataclasses import dataclass
from pathlib import Path
import re
from typing import Any

import yaml

try:
    from .clang_discovery import (
        DiscoveredField,
        DiscoveredFunction,
        DiscoveredMethod,
        discover_namespace_functions_with_compile_commands,
        discover_public_fields_with_compile_commands,
        discover_public_methods_with_compile_commands,
    )
except ImportError:  # pragma: no cover - script execution fallback
    from clang_discovery import (
        DiscoveredField,
        DiscoveredFunction,
        DiscoveredMethod,
        discover_namespace_functions_with_compile_commands,
        discover_public_fields_with_compile_commands,
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
    "double_buffer",  # Zero-copy borrowed pointer to double array
    "int32_buffer",   # Zero-copy borrowed pointer to int32 array
    "uint32",
    "size",
    "string",
    "string_list",
    "int32_list",
    "int32_list_list",
    "uint32_list",
    "handle",
    "handle_list",
    "opaque_ptr",  # Raw pointer to an external type (passed through as void*)
}
_ALLOWED_OWNERSHIP = {"owned", "borrowed", "static", "copy"}
_ALLOWED_DESTRUCTORS = {"delete", "none", "shared_ptr"}
_ALLOWED_PTR_TYPES = {"raw", "shared_ptr"}
_ALLOWED_CALL_KINDS = {"function", "adapter_function", "method", "adapter_method", "constructor", "field", "value_handle_field", "has_field", "field_setter", "children_count", "children_at", "children_add", "as_item_cast", "optional_has", "optional_get", "method_size", "array_field", "ccomponents_vector", "ccomponents_matrix", "variant_get", "variant_set"}
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
    ptr_type: str = "raw"  # "raw" or "shared_ptr"


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
    cpp_class: str | None = None
    compile_guard: str | None = None


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
class VariantAccessorsSpec:
    get_method: str  # C++ method name for getter (e.g. "get")
    set_method: str  # C++ method name for setter (e.g. "set")
    variant_type: str  # full C++ variant type for constructing set values
    types: dict[str, str]  # expose_suffix -> C++ type (e.g. {"bool": "bool", "int": "int64_t"})


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


@dataclass(frozen=True)
class MergedBindingSpec:
    """A merged binding spec containing multiple modules."""
    module: str  # Common module name (e.g., "ifcopenshell")
    c_prefix: str  # Common C prefix (e.g., "ifcopenshell")
    public_headers: tuple[str, ...]  # Merged public headers
    handles: dict[str, HandleSpec]  # All handles from all modules
    functions: tuple[CallSpec, ...]  # All functions from all modules
    methods: tuple[CallSpec, ...]  # All methods from all modules


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

    cpp_type = mapping.get("cpp_type")
    if cpp_type is not None:
        cpp_type = _expect_str(cpp_type, f"{context}.cpp_type")
    if kind == "opaque_ptr" and cpp_type is None:
        msg = f"{context}.cpp_type is required for kind=opaque_ptr"
        raise ValueError(msg)

    return TypeSpec(kind=kind, handle=handle, ownership=ownership, nullable=nullable, cpp_type=cpp_type)


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

    return CallSpec(
        kind=kind,
        expose_as=expose_as,
        c_name=c_name,
        receiver=receiver,
        cpp_name=cpp_name,
        returns=returns,
        params=params,
        implementation=implementation,
        cpp_class=cpp_class,
        compile_guard=compile_guard,
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
            va_types: dict[str, str] = {}
            for suffix, cpp_type_val in va_types_raw.items():
                if not isinstance(suffix, str) or not suffix:
                    msg = f"{va_context}.types keys must be non-empty strings"
                    raise ValueError(msg)
                va_types[suffix] = _expect_str(cpp_type_val, f"{va_context}.types[{suffix}]")
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

        has_any_feature = (include_all or include or overloads or discover_fields
                          or discover_children is not None or discover_as_item
                          or extra_fields or field_setters or discover_optional_fields
                          or method_sizes or array_pair_fields
                          or binary_operators or unary_operators or comparison_operators
                          or ccomponents_accessor is not None or variant_accessors is not None)
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
    base = normalized.removesuffix("&").removesuffix("*").strip().removeprefix("const ")
    for handle_name, handle in handles.items():
        handle_base = handle.cpp_type.rsplit("::", 1)[-1]
        if handle.cpp_type == base or handle_base == base:
            return handle_name
    # T::ptr → look up T (shared_ptr typedef)
    if base.endswith("::ptr"):
        sp_base = base.removesuffix("::ptr")
        for handle_name, handle in handles.items():
            handle_base = handle.cpp_type.rsplit("::", 1)[-1]
            if handle.cpp_type == sp_base or handle_base == sp_base:
                return handle_name
    # std::shared_ptr<T> → look up T
    m = re.match(r"(?:const\s+)?(?:std::)?shared_ptr<\s*(.+?)\s*>", base)
    if m:
        inner = m.group(1).removeprefix("const ").strip()
        for handle_name, handle in handles.items():
            handle_base = handle.cpp_type.rsplit("::", 1)[-1]
            if handle.cpp_type == inner or handle_base == inner:
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
    if vector_element is not None:
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
    if base in {"double", "float"}:
        return TypeSpec(kind="double", cpp_type=cpp_type)
    if base in {"unsigned int", "uint32_t"}:
        return TypeSpec(kind="uint32", cpp_type=cpp_type)
    if base in {"size_t", "std::size_t"}:
        return TypeSpec(kind="size", cpp_type=cpp_type)
    if base in {"std::vector<std::string>", "std::vector<std::string >"}:
        return TypeSpec(kind="string_list", ownership="copy", cpp_type=cpp_type)
    if (base == "char" and normalized.endswith("*")) or base in {"std::string"}:
        return TypeSpec(kind="string", ownership="copy", cpp_type=cpp_type)
    # std::array<double, N> → double_list (copy via begin/end)
    if re.match(r"std::array<\s*double\s*,\s*\d+\s*>", base):
        return TypeSpec(kind="double_list", ownership="copy", cpp_type=cpp_type)
    # std::array<int, N> → int32_list
    if re.match(r"std::array<\s*int\s*,\s*\d+\s*>", base):
        return TypeSpec(kind="int32_list", ownership="copy", cpp_type=cpp_type)
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
    if base in {"double", "float"}:
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


def _simple_type_spec(kind_str: str) -> TypeSpec:
    """Create a TypeSpec from a simple kind string (e.g. 'int32', 'double_list')."""
    if kind_str in ("int32_list", "double_list", "string_list"):
        return TypeSpec(kind=kind_str, ownership="copy")
    return TypeSpec(kind=kind_str)


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


def _extract_optional_inner_type(cpp_type: str) -> str | None:
    """Extract T from boost::optional<T>. Returns None if not an optional type."""
    normalized = _normalize_cpp_type(cpp_type)
    if normalized.startswith("boost::optional<") and normalized.endswith(">"):
        return normalized[len("boost::optional<"):-1].strip()
    return None


def _emit_optional_field_calls(
    item: DiscoveryClassSpec,
    handle: HandleSpec,
    field_name: str,
    cpp_name: str,
    inner_cpp_type: str,
    handles: dict[str, HandleSpec],
    calls: list[CallSpec],
    calls_by_c_name: dict[str, CallSpec],
    reserved_c_names: frozenset[str],
) -> None:
    """Generate has_X / X pair for a boost::optional<T> field."""
    try:
        returns = _infer_return_type(inner_cpp_type, handles)
    except ValueError:
        return  # Can't infer inner type — skip silently

    expose_as = item.rename.get(field_name, _snake_case_identifier(field_name))

    # has_X
    has_expose = f"has_{expose_as}"
    has_call = CallSpec(
        kind="optional_has",
        expose_as=has_expose,
        c_name=_make_c_name("ifcopenshell", handle, has_expose),
        receiver=item.handle,
        cpp_name=cpp_name,
        returns=TypeSpec(kind="bool", cpp_type=None),
        params=(),
        implementation=None,
    )
    if has_call.c_name not in reserved_c_names and has_call.c_name not in calls_by_c_name:
        calls_by_c_name[has_call.c_name] = has_call
        calls.append(has_call)

    # X (the getter)
    get_call = CallSpec(
        kind="optional_get",
        expose_as=expose_as,
        c_name=_make_c_name("ifcopenshell", handle, expose_as),
        receiver=item.handle,
        cpp_name=cpp_name,
        returns=returns,
        params=(),
        implementation=None,
    )
    if get_call.c_name not in reserved_c_names and get_call.c_name not in calls_by_c_name:
        calls_by_c_name[get_call.c_name] = get_call
        calls.append(get_call)


def _discover_method_calls(
    spec_path: Path,
    discovery: DiscoverySpec,
    handles: dict[str, HandleSpec],
    compile_commands_path: Path,
    authored_c_names: frozenset[str] = frozenset(),
) -> tuple[CallSpec, ...]:
    include_dir = (spec_path.parent / discovery.include_dir).resolve()
    class_cache: dict[tuple[str, str], dict[str, tuple[DiscoveredMethod, ...]]] = {}
    calls: list[CallSpec] = []
    calls_by_c_name: dict[str, CallSpec] = {}
    reserved_c_names: set[str] = set(authored_c_names)

    for item in discovery.classes:
        handle = handles[item.handle]
        excluded = set(item.exclude)

        # Only run AST method discovery if there's something to discover
        needs_method_discovery = item.include_all or item.include or item.overloads
        methods_by_name: dict[str, tuple[DiscoveredMethod, ...]] = {}
        if needs_method_discovery:
            cache_key = (handle.cpp_type, item.translation_unit, item.include_inherited_methods)
            cached = class_cache.get(cache_key)
            if cached is None:
                class_name = handle.cpp_type.rsplit("::", 1)[-1]
                translation_unit = (include_dir / item.translation_unit).resolve()
                cached = discover_public_methods_with_compile_commands(
                    compile_commands_path, translation_unit, class_name,
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
            if call.c_name in reserved_c_names:
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
                    continue
                msg = f"Method '{cpp_name}' on handle '{item.handle}' is overloaded and requires an explicit policy"
                raise ValueError(msg)

            discovered = overloads[0]
            try:
                ret_type = discovered.return_cpp_type
                if _normalize_cpp_type(ret_type) in item.enum_types_as_int32:
                    returns = TypeSpec(kind="int32", cpp_type=ret_type)
                else:
                    returns = _infer_return_type(ret_type, handles)
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
            if call.c_name in reserved_c_names:
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
                class_name = handle.cpp_type.rsplit("::", 1)[-1]
                translation_unit = (include_dir / item.translation_unit).resolve()
                fields_by_name = discover_public_fields_with_compile_commands(
                    compile_commands_path, translation_unit, class_name,
                    include_inherited=item.include_inherited_fields,
                )
                class_cache[field_cache_key] = fields_by_name

            for field_name in sorted(fields_by_name):
                if field_name in excluded:
                    continue
                field = fields_by_name[field_name]

                # Handle boost::optional<T> fields
                optional_inner = _extract_optional_inner_type(field.cpp_type)
                if optional_inner is not None:
                    if item.discover_optional_fields:
                        _emit_optional_field_calls(
                            item, handle, field_name, field.cpp_name, optional_inner,
                            handles, calls, calls_by_c_name, reserved_c_names,
                        )
                    continue

                try:
                    cpp_type = field.cpp_type
                    if _normalize_cpp_type(cpp_type) in item.enum_types_as_int32:
                        returns = TypeSpec(kind="int32", cpp_type=cpp_type)
                    else:
                        returns = _infer_return_type(cpp_type, handles)
                except ValueError:
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
                    kind=field_kind,
                    expose_as=expose_as,
                    c_name=_make_c_name("ifcopenshell", handle, expose_as),
                    receiver=item.handle,
                    cpp_name=field.cpp_name,
                    returns=returns,
                    params=(),
                    implementation=None,
                )
                existing = calls_by_c_name.get(call.c_name)
                if existing is not None or call.c_name in reserved_c_names:
                    # Skip silently — adapter_method or method already provides this
                    continue
                calls_by_c_name[call.c_name] = call
                calls.append(call)

                # For nullable handle fields, optionally generate has_X
                if item.discover_has_fields and field_kind == "field" and returns.kind == "handle":
                    has_expose = f"has_{expose_as}"
                    has_call = CallSpec(
                        kind="has_field",
                        expose_as=has_expose,
                        c_name=_make_c_name("ifcopenshell", handle, has_expose),
                        receiver=item.handle,
                        cpp_name=field.cpp_name,
                        returns=TypeSpec(kind="bool", cpp_type=None),
                        params=(),
                        implementation=None,
                    )
                    if has_call.c_name not in reserved_c_names and has_call.c_name not in calls_by_c_name:
                        calls_by_c_name[has_call.c_name] = has_call
                        calls.append(has_call)

        # Generate children count/at pair if requested
        if item.discover_children is not None:
            dc = item.discover_children
            # Count function
            count_call = CallSpec(
                kind="children_count",
                expose_as=dc.count_as,
                c_name=_make_c_name("ifcopenshell", handle, dc.count_as),
                receiver=item.handle,
                cpp_name=dc.cpp_field,
                returns=TypeSpec(kind="size", handle=None, ownership=None, nullable=False, cpp_type=None),
                params=(),
                implementation=None,
            )
            if count_call.c_name not in reserved_c_names and count_call.c_name not in calls_by_c_name:
                calls_by_c_name[count_call.c_name] = count_call
                calls.append(count_call)

            # At function
            at_call = CallSpec(
                kind="children_at",
                expose_as=dc.at_as,
                c_name=_make_c_name("ifcopenshell", handle, dc.at_as),
                receiver=item.handle,
                cpp_name=dc.cpp_field,
                returns=TypeSpec(kind="handle", handle=dc.element_handle, ownership="borrowed", nullable=False, cpp_type=None),
                params=(ParamSpec(name="index", type=TypeSpec(kind="size", handle=None, ownership=None, nullable=False, cpp_type=None)),),
                implementation=None,
            )
            if at_call.c_name not in reserved_c_names and at_call.c_name not in calls_by_c_name:
                calls_by_c_name[at_call.c_name] = at_call
                calls.append(at_call)

            # Add function with dcast
            if dc.add_as is not None:
                param_handle_name = dc.element_handle
                add_call = CallSpec(
                    kind="children_add",
                    expose_as=dc.add_as,
                    c_name=_make_c_name("ifcopenshell", handle, dc.add_as),
                    receiver=item.handle,
                    cpp_name=dc.add_cast_cpp_type,  # stash cast target in cpp_name
                    returns=TypeSpec(kind="void"),
                    params=(ParamSpec(name="item", type=TypeSpec(kind="handle", handle=param_handle_name, ownership="borrowed")),),
                    implementation=None,
                )
                if add_call.c_name not in reserved_c_names and add_call.c_name not in calls_by_c_name:
                    calls_by_c_name[add_call.c_name] = add_call
                    calls.append(add_call)

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
                kind=field_kind,
                expose_as=expose_as,
                c_name=_make_c_name("ifcopenshell", handle, expose_as),
                receiver=item.handle,
                cpp_name=field_name,
                returns=returns,
                params=(),
                implementation=None,
            )
            if call.c_name not in reserved_c_names and call.c_name not in calls_by_c_name:
                calls_by_c_name[call.c_name] = call
                calls.append(call)

            # Also generate has_X for nullable handle extra fields
            if item.discover_has_fields and field_kind == "field" and returns.kind == "handle":
                has_expose = f"has_{expose_as}"
                has_call = CallSpec(
                    kind="has_field",
                    expose_as=has_expose,
                    c_name=_make_c_name("ifcopenshell", handle, has_expose),
                    receiver=item.handle,
                    cpp_name=field_name,
                    returns=TypeSpec(kind="bool", cpp_type=None),
                    params=(),
                    implementation=None,
                )
                if has_call.c_name not in reserved_c_names and has_call.c_name not in calls_by_c_name:
                    calls_by_c_name[has_call.c_name] = has_call
                    calls.append(has_call)

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
            set_call = CallSpec(
                kind="field_setter",
                expose_as=set_expose,
                c_name=_make_c_name("ifcopenshell", handle, set_expose),
                receiver=item.handle,
                cpp_name=setter_field,
                returns=TypeSpec(kind="void"),
                params=(ParamSpec(name="value", type=param_type),),
                implementation=None,
            )
            if set_call.c_name not in reserved_c_names and set_call.c_name not in calls_by_c_name:
                calls_by_c_name[set_call.c_name] = set_call
                calls.append(set_call)

        # Generate method_size calls (method().size())
        for method_name, expose_as in item.method_sizes.items():
            ms_call = CallSpec(
                kind="method_size",
                expose_as=expose_as,
                c_name=_make_c_name("ifcopenshell", handle, expose_as),
                receiver=item.handle,
                cpp_name=method_name,
                returns=TypeSpec(kind="size"),
                params=(),
                implementation=None,
            )
            if ms_call.c_name not in reserved_c_names and ms_call.c_name not in calls_by_c_name:
                calls_by_c_name[ms_call.c_name] = ms_call
                calls.append(ms_call)

        # Generate array_pair_field calls (field[0] as _u, field[1] as _v)
        for field_name, return_kind in item.array_pair_fields.items():
            ret_type = _simple_type_spec(return_kind)
            for suffix, index in [("_u", 0), ("_v", 1)]:
                expose = f"{field_name}{suffix}"
                ap_call = CallSpec(
                    kind="array_field",
                    expose_as=expose,
                    c_name=_make_c_name("ifcopenshell", handle, expose),
                    receiver=item.handle,
                    cpp_name=f"{field_name}[{index}]",
                    returns=ret_type,
                    params=(),
                    implementation=None,
                )
                if ap_call.c_name not in reserved_c_names and ap_call.c_name not in calls_by_c_name:
                    calls_by_c_name[ap_call.c_name] = ap_call
                    calls.append(ap_call)

        # Generate operator calls (binary, unary, comparison)
        def _add_op_call(expose_as: str, call_spec: CallSpec) -> None:
            if call_spec.c_name not in reserved_c_names and call_spec.c_name not in calls_by_c_name:
                calls_by_c_name[call_spec.c_name] = call_spec
                calls.append(call_spec)

        handle_return_owned = TypeSpec(kind="handle", handle=item.handle, ownership="owned", nullable=False, cpp_type=None)
        other_param = ParamSpec(name="other", type=TypeSpec(kind="handle", handle=item.handle, ownership="borrowed", nullable=False, cpp_type=None))

        for expose_as, op in item.binary_operators.items():
            _add_op_call(expose_as, CallSpec(
                kind="adapter_method",
                expose_as=expose_as,
                c_name=_make_c_name("ifcopenshell", handle, expose_as),
                receiver=item.handle,
                cpp_name=None,
                returns=handle_return_owned,
                params=(other_param,),
                implementation=ImplementationSpec(kind="inline_cpp", body=f"return (*self_cpp) {op} other_cpp;\n"),
            ))

        for expose_as, op in item.unary_operators.items():
            _add_op_call(expose_as, CallSpec(
                kind="adapter_method",
                expose_as=expose_as,
                c_name=_make_c_name("ifcopenshell", handle, expose_as),
                receiver=item.handle,
                cpp_name=None,
                returns=handle_return_owned,
                params=(),
                implementation=ImplementationSpec(kind="inline_cpp", body=f"return {op}(*self_cpp);\n"),
            ))

        for expose_as, op in item.comparison_operators.items():
            _add_op_call(expose_as, CallSpec(
                kind="adapter_method",
                expose_as=expose_as,
                c_name=_make_c_name("ifcopenshell", handle, expose_as),
                receiver=item.handle,
                cpp_name=None,
                returns=TypeSpec(kind="bool"),
                params=(other_param,),
                implementation=ImplementationSpec(kind="inline_cpp", body=f"return (*self_cpp) {op} other_cpp;\n"),
            ))

        # Generate as_item cast if requested
        if item.discover_as_item:
            as_item_call = CallSpec(
                kind="as_item_cast",
                expose_as="as_item",
                c_name=_make_c_name("ifcopenshell", handle, "as_item"),
                receiver=item.handle,
                cpp_name=None,
                returns=TypeSpec(kind="handle", handle="taxonomy_item", ownership="owned", nullable=False, cpp_type=None),
                params=(),
                implementation=None,
            )
            if as_item_call.c_name not in reserved_c_names and as_item_call.c_name not in calls_by_c_name:
                calls_by_c_name[as_item_call.c_name] = as_item_call
                calls.append(as_item_call)

        # Generate ccomponents accessor (Eigen data extraction)
        if item.ccomponents_accessor is not None:
            cc = item.ccomponents_accessor
            cc_kind = "ccomponents_matrix" if cc.dimensions > 3 else "ccomponents_vector"
            cc_call = CallSpec(
                kind=cc_kind,
                expose_as=cc.expose_as,
                c_name=_make_c_name("ifcopenshell", handle, cc.expose_as),
                receiver=item.handle,
                cpp_name=cc.access_via,  # stash access path in cpp_name
                returns=TypeSpec(kind="double_list", ownership="copy"),
                params=(),
                implementation=None,
            )
            if cc_call.c_name not in reserved_c_names and cc_call.c_name not in calls_by_c_name:
                calls_by_c_name[cc_call.c_name] = cc_call
                calls.append(cc_call)

        # Generate variant accessors (typed get/set for variant methods)
        if item.variant_accessors is not None:
            va = item.variant_accessors
            for suffix, cpp_type in va.types.items():
                # Map C++ type to return kind
                type_map = {
                    "bool": "bool", "int64_t": "int64", "double": "double",
                    "std::string": "string", "int": "int32",
                }
                ret_kind = type_map.get(cpp_type, "string")
                # Generate getter: get_SUFFIX(name) -> TYPE
                get_expose = f"get_{suffix}"
                get_call = CallSpec(
                    kind="variant_get",
                    expose_as=get_expose,
                    c_name=_make_c_name("ifcopenshell", handle, get_expose),
                    receiver=item.handle,
                    cpp_name=f"{va.get_method}|{cpp_type}",  # stash method|type
                    returns=TypeSpec(kind=ret_kind),
                    params=(ParamSpec(name="name", type=TypeSpec(kind="string")),),
                    implementation=None,
                )
                if get_call.c_name not in reserved_c_names and get_call.c_name not in calls_by_c_name:
                    calls_by_c_name[get_call.c_name] = get_call
                    calls.append(get_call)

                # Generate setter: set_SUFFIX(name, value) -> void
                set_expose = f"set_{suffix}"
                param_type = TypeSpec(kind=ret_kind)
                set_call = CallSpec(
                    kind="variant_set",
                    expose_as=set_expose,
                    c_name=_make_c_name("ifcopenshell", handle, set_expose),
                    receiver=item.handle,
                    cpp_name=f"{va.set_method}|{va.variant_type}|{cpp_type}",  # stash method|variant|type
                    returns=TypeSpec(kind="void"),
                    params=(
                        ParamSpec(name="name", type=TypeSpec(kind="string")),
                        ParamSpec(name="value", type=param_type),
                    ),
                    implementation=None,
                )
                if set_call.c_name not in reserved_c_names and set_call.c_name not in calls_by_c_name:
                    calls_by_c_name[set_call.c_name] = set_call
                    calls.append(set_call)

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
        if handle.destructor not in _ALLOWED_DESTRUCTORS:
            msg = f"{context}.destructor must be one of {sorted(_ALLOWED_DESTRUCTORS)}"
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

    adapter_functions = tuple(
        _parse_call(item, context=f"functions[{index}]", known_handles=known_handles, expect_receiver=False)
        for index, item in enumerate(_expect_list(root.get("functions", []), "functions"))
    )
    adapter_methods = tuple(
        _parse_call(item, context=f"methods[{index}]", known_handles=known_handles, expect_receiver=True)
        for index, item in enumerate(_expect_list(root.get("methods", []), "methods"))
    )

    # Collect authored c_names so discovery can skip collisions
    authored_c_names = frozenset(c.c_name for c in (*adapter_functions, *adapter_methods))
    discovered_methods = _discover_method_calls(path, discovery, handles, compile_commands_path, authored_c_names) if discovery is not None else tuple()
    discovered_functions = (
        _discover_function_calls(path, discovery, handles, c_prefix, compile_commands_path) if discovery is not None else tuple()
    )

    depends_on_common = root.get("depends_on_common")
    if depends_on_common is not None and not isinstance(depends_on_common, str):
        msg = "depends_on_common must be a string"
        raise ValueError(msg)

    return AuthoredBindingSpec(
        schema_version=schema_version,
        module=module,
        slice=slice_name,
        c_prefix=c_prefix,
        public_headers=public_headers,
        handles=handles,
        imports=tuple(imports),
        depends_on_common=depends_on_common,
        discovery=discovery,
        functions=discovered_functions + adapter_functions,
        methods=discovered_methods + adapter_methods,
    )


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
    all_handles: dict[str, HandleSpec] = {}
    all_headers: list[str] = []
    all_functions: list[CallSpec] = []
    all_methods: list[CallSpec] = []
    
    for spec_path in spec_paths:
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
    
    return MergedBindingSpec(
        module=module,
        c_prefix=c_prefix,
        public_headers=tuple(all_headers),
        handles=all_handles,
        functions=tuple(all_functions),
        methods=tuple(all_methods),
    )
