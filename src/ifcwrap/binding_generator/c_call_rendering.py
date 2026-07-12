# This file was generated with the assistance of an AI coding tool.

from __future__ import annotations

import json

try:
    from .authored_spec import ParamSpec, TypeSpec, _extract_optional_inner_type
    from .binding_ir import (
        ArrayElementFieldOp,
        BindingIR,
        BoolOutParamCallOp,
        CallIR,
        CcomponentsAccessorOp,
        ChildrenAddOp,
        ChildrenAtOp,
        ChildrenCountOp,
        ConstructorOp,
        DirectCallOp,
        FieldGetOp,
        FieldSetterOp,
        InlineImplementationOp,
        ListAtOp,
        ListCountOp,
        MethodAtOp,
        MethodSizeOp,
        OptionalGetOp,
        OptionalPresenceCheckOp,
        PointerPresenceCheckOp,
        SpecMethodFunctionCallOp,
        StaticCastOp,
        ValueHandleFieldGetOp,
        VariantGetOp,
        VariantSetOp,
    )
    from .c_handle_rendering import _wrap_handle_expr
    from .c_sequence_helpers import (
        _handle_list_c_type,
        _handle_list_helper_name,
        _handle_list_list_c_type,
        _handle_list_list_helper_name,
        _sequence_make_helper,
        _sequence_to_cpp_helper,
        _snake_name,
        _type_spec_sequence_kind,
    )
    from .c_type_rendering import (
        _BUFFER_TYPE_MAP,
        _SCALAR_TYPE_MAP,
        _cpp_param_type,
        _normalize_cpp_type,
        _out_param_type,
        _qualify_handle_cpp_fragment,
    )
except ImportError:  # pragma: no cover - script execution fallback
    from authored_spec import ParamSpec, TypeSpec, _extract_optional_inner_type
    from binding_ir import (
        ArrayElementFieldOp,
        BindingIR,
        BoolOutParamCallOp,
        CallIR,
        CcomponentsAccessorOp,
        ChildrenAddOp,
        ChildrenAtOp,
        ChildrenCountOp,
        ConstructorOp,
        DirectCallOp,
        FieldGetOp,
        FieldSetterOp,
        InlineImplementationOp,
        ListAtOp,
        ListCountOp,
        MethodAtOp,
        MethodSizeOp,
        OptionalGetOp,
        OptionalPresenceCheckOp,
        PointerPresenceCheckOp,
        SpecMethodFunctionCallOp,
        StaticCastOp,
        ValueHandleFieldGetOp,
        VariantGetOp,
        VariantSetOp,
    )
    from c_handle_rendering import _wrap_handle_expr
    from c_sequence_helpers import (
        _handle_list_c_type,
        _handle_list_helper_name,
        _handle_list_list_c_type,
        _handle_list_list_helper_name,
        _sequence_make_helper,
        _sequence_to_cpp_helper,
        _snake_name,
        _type_spec_sequence_kind,
    )
    from c_type_rendering import (
        _BUFFER_TYPE_MAP,
        _SCALAR_TYPE_MAP,
        _cpp_param_type,
        _normalize_cpp_type,
        _out_param_type,
        _qualify_handle_cpp_fragment,
    )


def _set_element_cpp_type(cpp_type: str | None) -> str | None:
    normalized = _normalize_cpp_type(cpp_type)
    while normalized.startswith("const "):
        normalized = normalized[len("const ") :].strip()
    normalized = normalized.removesuffix("&").strip()
    if not normalized.startswith("std::set<") or not normalized.endswith(">"):
        return None
    return normalized[len("std::set<") : -1].strip()


def _cpp_string_literal(value: str) -> str:
    return json.dumps(value)


def _value_handle_empty_expr(handle: object, value_expr: str) -> str:
    empty_check = getattr(handle, "empty_check", None)
    if empty_check:
        return empty_check.replace("{value}", value_expr)
    return f"!static_cast<bool>({value_expr})"


def _is_handle_cast(call: CallIR) -> bool:
    return (
        call.expose_as.startswith("as_")
        and call.receiver is not None
        and call.returns.kind == "handle"
        and call.returns.sequence_depth == 0
        and call.returns.handle != call.receiver
    )


def _handle_result_empty_expr(
    call: CallIR, type_spec: TypeSpec, handle: object, value_expr: str
) -> str:
    if handle.ptr_type == "value":
        return _value_handle_empty_expr(handle, value_expr)
    normalized_cpp_type = _normalize_cpp_type(type_spec.cpp_type)
    if handle.ptr_type == "shared_ptr" or normalized_cpp_type.startswith(
        "std::unique_ptr<"
    ):
        return f"!{value_expr}"
    return f"{value_expr} == nullptr"


def _should_null_check_handle_result(
    call: CallIR, type_spec: TypeSpec, handle: object
) -> bool:
    if handle.ptr_type == "value":
        return type_spec.nullable or (getattr(handle, "empty_check", None) is not None)
    return type_spec.nullable or _is_handle_cast(call)


def _render_result_assignment(call: CallIR, spec: BindingIR, expr: str) -> str:
    type_spec = call.returns
    kind = type_spec.kind
    sequence_kind = _type_spec_sequence_kind(type_spec)
    if kind == "void":
        return f"{expr};"
    if sequence_kind is not None:
        if _set_element_cpp_type(type_spec.cpp_type) is not None:
            return (
                f"*out_result = {_sequence_make_helper(sequence_kind)}(([&]() {{ auto tmp = {expr}; "
                f"return std::vector(tmp.begin(), tmp.end()); }})());"
            )
        return f"*out_result = {_sequence_make_helper(sequence_kind)}({expr});"
    if kind in _SCALAR_TYPE_MAP:
        return _SCALAR_TYPE_MAP[kind][2].format(expr=expr)
    if kind == "string":
        helper = (
            "make_static_string" if type_spec.ownership == "static" else "make_string"
        )
        if type_spec.nullable:
            return (
                f"auto result_value = {expr};\n"
                f"        if (!result_value) {{\n"
                f"            if (!g_last_error.empty()) {{ return false; }}\n"
                f"            *out_result = ifcopenshell_string_t{{nullptr, 0, false}};\n"
                f"        }} else {{\n"
                f"            *out_result = {helper}(*result_value);\n"
                f"        }}"
            )
        return f"*out_result = {helper}({expr});"
    if kind in _BUFFER_TYPE_MAP:
        normalized_cpp_type = _normalize_cpp_type(type_spec.cpp_type)
        if normalized_cpp_type.endswith("*"):
            return f"*out_result = {expr};"
        return f"*out_result = ({expr}).data();"
    if kind == "handle":
        if type_spec.sequence_depth == 1:
            helper = _handle_list_helper_name(spec.handles[type_spec.handle])
            if _set_element_cpp_type(type_spec.cpp_type) is not None:
                return (
                    f"*out_result = {helper}(([&]() {{ auto tmp = {expr}; "
                    f"return std::vector(tmp.begin(), tmp.end()); }})());"
                )
            return f"*out_result = {helper}({expr});"
        if type_spec.sequence_depth == 2:
            helper = _handle_list_list_helper_name(spec.handles[type_spec.handle])
            return f"*out_result = {helper}({expr});"
        handle = spec.handles[type_spec.handle]
        if type_spec.nullable and _is_optional_cpp_type(type_spec):
            inner_cpp_type = (
                _extract_optional_inner_type(type_spec.cpp_type) or handle.cpp_type
            )
            unwrapped_type = TypeSpec(
                kind=type_spec.kind,
                handle=type_spec.handle,
                struct=type_spec.struct,
                ownership=type_spec.ownership,
                nullable=False,
                cpp_type=inner_cpp_type,
                sequence_depth=type_spec.sequence_depth,
            )
            return (
                f"auto result_value = {expr};\n"
                f"        if (!result_value) {{\n"
                f"            *out_result = nullptr;\n"
                f"        }} else {{\n"
                f"            auto unwrapped_result = *result_value;\n"
                f"            if ({_handle_result_empty_expr(call, unwrapped_type, handle, 'unwrapped_result')}) {{\n"
                f"                *out_result = nullptr;\n"
                f"            }} else {{\n"
                f"                *out_result = {_wrap_handle_expr(unwrapped_type, 'unwrapped_result', spec)};\n"
                f"            }}\n"
                f"        }}"
            )
        if (
            type_spec.ownership == "owned"
            and handle.name not in {"attribute_value", "instance_list"}
            and handle.ptr_type != "shared_ptr"
            and handle.ptr_type != "value"
            and handle.destructor == "delete"
        ):
            normalized_cpp_type = _normalize_cpp_type(type_spec.cpp_type)
            if normalized_cpp_type == _normalize_cpp_type(handle.cpp_type):
                if type_spec.nullable:
                    return (
                        f"auto result_value = {expr};\n"
                        f"        if (result_value == nullptr) {{\n"
                        f"            *out_result = nullptr;\n"
                        f"        }} else {{\n"
                        f"            *out_result = new {handle.c_type}{{new {handle.cpp_type}(result_value), true}};\n"
                        f"        }}"
                    )
                return f"*out_result = new {handle.c_type}{{new {handle.cpp_type}({expr}), true}};"
            if not normalized_cpp_type.startswith("std::unique_ptr<"):
                if type_spec.nullable:
                    return (
                        f"auto result_value = std::unique_ptr<{handle.cpp_type}>({expr});\n"
                        f"        if (!result_value) {{\n"
                        f"            *out_result = nullptr;\n"
                        f"        }} else {{\n"
                        f"            *out_result = new {handle.c_type}{{result_value.release(), true}};\n"
                        f"        }}"
                    )
                return (
                    f"auto result_value = std::unique_ptr<{handle.cpp_type}>({expr});\n"
                    f"        *out_result = new {handle.c_type}{{result_value.release(), true}};"
                )
        if _should_null_check_handle_result(call, type_spec, handle):
            normalized_cpp_type = _normalize_cpp_type(type_spec.cpp_type)
            wrapped_expr = "std::move(result_value)"
            if handle.ptr_type not in {
                "value",
                "shared_ptr",
            } and not normalized_cpp_type.startswith("std::unique_ptr<"):
                wrapped_expr = "result_value"
            return (
                f"auto result_value = {expr};\n"
                f"        if ({_handle_result_empty_expr(call, type_spec, handle, 'result_value')}) {{\n"
                f"            *out_result = nullptr;\n"
                f"        }} else {{\n"
                f"            *out_result = {_wrap_handle_expr(type_spec, wrapped_expr, spec)};\n"
                f"        }}"
            )
        return f"*out_result = {_wrap_handle_expr(type_spec, expr, spec)};"
    if kind == "opaque_ptr":
        return f"*out_result = static_cast<void*>({expr});"
    if kind == "struct":
        if type_spec.struct is None:
            raise ValueError(f"{call.c_name} struct return is missing struct name")
        struct = spec.result_structs[type_spec.struct]
        lines = [f"auto result_value = {expr};"]
        target = "out_result"
        value_expr_prefix = "result_value"
        if type_spec.nullable:
            lines.extend(
                [
                    "out_result->has_value = static_cast<bool>(result_value);",
                    "if (result_value) {",
                ]
            )
            target = "out_result->value"
            value_expr_prefix = "(*result_value)"
        for field in struct.fields:
            cpp_field = field.cpp_field or field.name
            field_expr = f"{value_expr_prefix}.{cpp_field}"
            field_type = field.type
            field_sequence_kind = _type_spec_sequence_kind(field_type)
            if field_sequence_kind is not None:
                assignment = (
                    f"{_sequence_make_helper(field_sequence_kind)}({field_expr})"
                )
            elif field_type.kind in _SCALAR_TYPE_MAP:
                assignment = (
                    f"static_cast<{_SCALAR_TYPE_MAP[field_type.kind][0]}>({field_expr})"
                )
            elif field_type.kind == "string":
                helper = (
                    "make_static_string"
                    if field_type.ownership == "static"
                    else "make_string"
                )
                assignment = f"{helper}({field_expr})"
            elif field_type.kind == "handle":
                if field_type.sequence_depth == 1:
                    assignment = f"{_handle_list_helper_name(spec.handles[field_type.handle])}({field_expr})"
                elif field_type.sequence_depth == 2:
                    assignment = f"{_handle_list_list_helper_name(spec.handles[field_type.handle])}({field_expr})"
                else:
                    assignment = _wrap_handle_expr(field_type, field_expr, spec)
            elif field_type.kind == "opaque_ptr":
                assignment = f"static_cast<void*>({field_expr})"
            else:
                raise ValueError(
                    f"Unsupported result struct field kind: {field_type.kind}"
                )
            separator = "." if target.endswith("value") else "->"
            prefix = "    " if type_spec.nullable else ""
            lines.append(f"{prefix}{target}{separator}{field.name} = {assignment};")
        if type_spec.nullable:
            lines.append("}")
        return "\n        ".join(lines)
    if kind == "variant":
        lines = [f"auto result_value = {expr};"]
        for index, alt in enumerate(type_spec.variants):
            alt_cpp_type = alt.cpp_type
            if alt_cpp_type is None:
                raise ValueError(
                    f"{call.c_name} variant alternative is missing C++ type"
                )
            alt_expr = f"std::get<{alt_cpp_type}>(result_value)"
            alt_sequence_kind = _type_spec_sequence_kind(alt)
            if alt_sequence_kind is not None:
                assignment = f"{_sequence_make_helper(alt_sequence_kind)}({alt_expr})"
            elif alt.kind in _SCALAR_TYPE_MAP:
                assignment = f"static_cast<{_SCALAR_TYPE_MAP[alt.kind][0]}>({alt_expr})"
            elif alt.kind == "string":
                helper = (
                    "make_static_string" if alt.ownership == "static" else "make_string"
                )
                assignment = f"{helper}({alt_expr})"
            elif alt.kind == "handle":
                if alt.sequence_depth == 1:
                    assignment = f"{_handle_list_helper_name(spec.handles[alt.handle])}({alt_expr})"
                elif alt.sequence_depth == 2:
                    assignment = f"{_handle_list_list_helper_name(spec.handles[alt.handle])}({alt_expr})"
                else:
                    assignment = _wrap_handle_expr(alt, alt_expr, spec)
            elif alt.kind == "opaque_ptr":
                assignment = f"static_cast<void*>({alt_expr})"
            else:
                raise ValueError(f"Unsupported variant alternative kind: {alt.kind}")
            prefix = "if" if index == 0 else "else if"
            lines.extend(
                [
                    f"{prefix} (std::holds_alternative<{alt_cpp_type}>(result_value)) {{",
                    f"            out_result->kind = {index};",
                    f"            out_result->value_{index} = {assignment};",
                    "        }",
                ]
            )
        lines.append(
            'else { throw std::runtime_error("Unsupported variant alternative"); }'
        )
        return "\n        ".join(lines)
    msg = f"Unsupported return kind: {kind}"
    raise ValueError(msg)


def _null_check(param_name: str, label: str) -> str:
    return f'    if ({param_name} == nullptr) {{ throw std::runtime_error("{label} \\"{param_name}\\" must not be null"); }}'


def _is_optional_cpp_type(type_spec: TypeSpec) -> bool:
    cpp_type = _normalize_cpp_type(type_spec.cpp_type)
    return cpp_type.startswith("std::optional<")


def _render_param_prelude(param: ParamSpec, spec: BindingIR) -> str:
    type_spec = param.type
    kind = type_spec.kind
    sequence_kind = _type_spec_sequence_kind(type_spec)
    if sequence_kind is not None:
        to_cpp = _sequence_to_cpp_helper(sequence_kind)
        set_element = _set_element_cpp_type(type_spec.cpp_type)
        if set_element is not None:
            return (
                f"{_null_check(param.name, 'Parameter')}\n"
                f"    auto {param.name}_vec = {to_cpp}({param.name});\n"
                f"    std::set<{set_element}> {param.name}_cpp({param.name}_vec.begin(), {param.name}_vec.end());"
            )
        return (
            f"{_null_check(param.name, 'Parameter')}\n"
            f"    auto {param.name}_cpp = {to_cpp}({param.name});"
        )
    if kind in _SCALAR_TYPE_MAP and type_spec.cpp_type is not None:
        if type_spec.nullable and _is_optional_cpp_type(type_spec):
            raise ValueError(
                f'Standalone optional scalar parameter "{param.name}" needs an options struct to preserve presence'
            )
        if kind == "size":
            return f"    auto {param.name}_cpp = static_cast<size_t>({param.name});"
        return f"    auto {param.name}_cpp = static_cast<{type_spec.cpp_type}>({param.name});"
    if kind == "string":
        if type_spec.nullable:
            if _is_optional_cpp_type(type_spec):
                return (
                    f"    std::optional<std::string> {param.name}_cpp;\n"
                    f"    if ({param.name} != nullptr) {{ {param.name}_cpp = std::string({param.name}); }}"
                )
            return f"    const char* {param.name}_str = {param.name};"
        return (
            f"{_null_check(param.name, 'Parameter')}\n"
            f"    std::string {param.name}_cpp({param.name});"
        )
    if kind == "handle":
        if type_spec.sequence_depth == 1:
            handle = spec.handles[type_spec.handle]
            helper_name = f"to_cpp_{_snake_name(_handle_list_c_type(handle))}"
            set_element = _set_element_cpp_type(type_spec.cpp_type)
            if set_element is not None:
                set_element = _qualify_handle_cpp_fragment(set_element, handle.cpp_type)
                return (
                    f"{_null_check(param.name, 'Parameter')}\n"
                    f"    auto {param.name}_vec = {helper_name}({param.name});\n"
                    f"    std::set<{set_element}> {param.name}_cpp({param.name}_vec.begin(), {param.name}_vec.end());"
                )
            return (
                f"{_null_check(param.name, 'Parameter')}\n"
                f"    auto {param.name}_cpp = {helper_name}({param.name});"
            )
        if type_spec.sequence_depth == 2:
            handle = spec.handles[type_spec.handle]
            helper_name = f"to_cpp_{_snake_name(_handle_list_list_c_type(handle))}"
            return (
                f"{_null_check(param.name, 'Parameter')}\n"
                f"    auto {param.name}_cpp = {helper_name}({param.name});"
            )
        handle = spec.handles[type_spec.handle]
        if type_spec.nullable and _is_optional_cpp_type(type_spec):
            inner_type = (
                _extract_optional_inner_type(type_spec.cpp_type) or handle.cpp_type
            )
            if handle.ptr_type == "value":
                return (
                    f"    std::optional<{inner_type}> {param.name}_cpp;\n"
                    f"    if ({param.name} != nullptr) {{ {param.name}_cpp = {param.name}->value; }}"
                )
            if handle.ptr_type == "shared_ptr":
                return (
                    f"    std::optional<{inner_type}> {param.name}_cpp;\n"
                    f"    if ({param.name} != nullptr && {param.name}->ptr != nullptr) {{ {param.name}_cpp = {param.name}->ptr; }}"
                )
            if inner_type.endswith("*"):
                return (
                    f"    std::optional<{inner_type}> {param.name}_cpp;\n"
                    f"    if ({param.name} != nullptr && {param.name}->ptr != nullptr) {{ {param.name}_cpp = {param.name}->ptr; }}"
                )
            return (
                f"    std::optional<{inner_type}> {param.name}_cpp;\n"
                f"    if ({param.name} != nullptr && {param.name}->ptr != nullptr) {{ {param.name}_cpp = *{param.name}->ptr; }}"
            )
        if handle.name == "attribute_value":
            return (
                f"{_null_check(param.name, 'Handle parameter')}\n"
                f"    auto {param.name}_cpp = {param.name}->value;"
            )
        if handle.ptr_type == "value":
            cpp_type = _normalize_cpp_type(type_spec.cpp_type)
            if not cpp_type:
                cpp_type = f"{handle.cpp_type}*"
            if type_spec.nullable:
                if cpp_type.endswith("*"):
                    return f"    auto {param.name}_cpp = ({param.name} != nullptr) ? &{param.name}->value : nullptr;"
                raise ValueError(
                    f'Value handle parameter "{param.name}" can only be nullable with pointer cpp_type "{type_spec.cpp_type}"'
                )
            if cpp_type.endswith("*"):
                return (
                    f"{_null_check(param.name, 'Handle parameter')}\n"
                    f"    auto {param.name}_cpp = &{param.name}->value;"
                )
            if cpp_type.endswith("&"):
                auto_kw = "const auto&" if cpp_type.startswith("const ") else "auto&"
                return (
                    f"{_null_check(param.name, 'Handle parameter')}\n"
                    f"    {auto_kw} {param.name}_cpp = {param.name}->value;"
                )
            return (
                f"{_null_check(param.name, 'Handle parameter')}\n"
                f"    auto {param.name}_cpp = {param.name}->value;"
            )
        cpp_type = _normalize_cpp_type(type_spec.cpp_type)
        is_shared = handle.ptr_type == "shared_ptr"
        # Check if cpp_type refers to the shared_ptr itself (e.g., "const T::ptr &")
        refers_to_shared_ptr = (
            is_shared
            and cpp_type is not None
            and ("::ptr" in cpp_type or "shared_ptr" in cpp_type)
        )
        if type_spec.nullable and cpp_type.endswith("&") and not refers_to_shared_ptr:
            raise ValueError(
                f'Handle parameter "{param.name}" cannot be nullable with reference cpp_type "{type_spec.cpp_type}"'
            )
        if type_spec.cpp_type is not None and cpp_type.endswith("&"):
            if refers_to_shared_ptr:
                # Reference to the shared_ptr: pass ptr member directly as reference
                value_expr = f"{param.name}->ptr"
                auto_kw = "const auto&"
            else:
                # Reference to underlying object: dereference pointer
                value_expr = f"*{param.name}->ptr"
                auto_kw = "auto&"
        elif (
            type_spec.cpp_type is not None
            and not cpp_type.endswith("*")
            and not is_shared
        ):
            # Value parameter (discovered, raw-ptr handle): dereference to copy
            value_expr = f"*{param.name}->ptr"
            auto_kw = "auto"
        else:
            # Pointer, shared_ptr, or unspecified (adapter): keep as-is
            value_expr = f"{param.name}->ptr"
            auto_kw = "auto"
        if type_spec.nullable:
            return f"    auto {param.name}_cpp = ({param.name} != nullptr && {param.name}->ptr != nullptr) ? {value_expr} : nullptr;"
        return (
            f'    if ({param.name} == nullptr || {param.name}->ptr == nullptr) {{ throw std::runtime_error("Handle parameter \\"{param.name}\\" is invalid"); }}\n'
            f"    {auto_kw} {param.name}_cpp = {value_expr};"
        )
    if kind == "opaque_ptr":
        cpp_type = type_spec.cpp_type
        if type_spec.nullable:
            if _is_optional_cpp_type(type_spec):
                inner_type = _extract_optional_inner_type(type_spec.cpp_type)
                if inner_type is None:
                    raise ValueError(
                        f'Opaque pointer parameter "{param.name}" has invalid optional cpp_type'
                    )
                return (
                    f"    std::optional<{inner_type}> {param.name}_cpp;\n"
                    f"    if ({param.name} != nullptr) {{ {param.name}_cpp = static_cast<{inner_type}>({param.name}); }}"
                )
            return f"    auto {param.name}_cpp = static_cast<{cpp_type}>({param.name});"
        return (
            f"{_null_check(param.name, 'Parameter')}\n"
            f"    auto {param.name}_cpp = static_cast<{cpp_type}>({param.name});"
        )
    if kind == "option":
        return _render_option_param_prelude(param, spec)
    return ""


def _render_option_param_prelude(param: ParamSpec, spec: BindingIR) -> str:
    if param.type.struct is None:
        raise ValueError(
            f'Option parameter "{param.name}" is missing option struct name'
        )
    option = spec.option_structs[param.type.struct]
    lines = [
        _null_check(param.name, "Options parameter"),
        f"    {option.cpp_type} {param.name}_cpp{{}};",
    ]
    for field in option.fields:
        source = f"{param.name}->{field.name}"
        target = f"{param.name}_cpp.{field.cpp_field or field.name}"
        if field.type.nullable:
            lines.append(f"    if ({param.name}->has_{field.name}) {{")
            field_check = _render_option_required_field_check(
                param.name, field, indent="        "
            )
            if field_check:
                lines.append(field_check)
            lines.append(
                f"        {target} = {_option_field_cpp_expr(field.type, source, spec)};"
            )
            lines.append("    }")
        else:
            lines.append(_render_option_required_field_check(param.name, field))
            lines.append(
                f"    {target} = {_option_field_cpp_expr(field.type, source, spec)};"
            )
    return "\n".join(line for line in lines if line)


def _render_option_required_field_check(
    option_param: str, field: object, *, indent: str = "    "
) -> str:
    if _type_spec_sequence_kind(field.type) is not None:
        return (
            f"{indent}if ({option_param}->{field.name} == nullptr) "
            f'{{ throw std::runtime_error("Options field \\"{field.name}\\" must not be null"); }}'
        )
    if field.type.kind == "string":
        return (
            f"{indent}if ({option_param}->{field.name} == nullptr) "
            f'{{ throw std::runtime_error("Options field \\"{field.name}\\" must not be null"); }}'
        )
    if field.type.kind == "handle" and field.type.sequence_depth == 0:
        return (
            f"{indent}if ({option_param}->{field.name} == nullptr) "
            f'{{ throw std::runtime_error("Options field \\"{field.name}\\" must not be null"); }}'
        )
    return ""


def _option_field_cpp_expr(type_spec: TypeSpec, source: str, spec: BindingIR) -> str:
    sequence_kind = _type_spec_sequence_kind(type_spec)
    if sequence_kind is not None:
        return f"{_sequence_to_cpp_helper(sequence_kind)}({source})"
    if type_spec.kind == "string":
        return f"std::string({source})"
    if type_spec.kind in _SCALAR_TYPE_MAP:
        if type_spec.cpp_type is not None and not _is_optional_cpp_type(type_spec):
            return f"static_cast<{type_spec.cpp_type}>({source})"
        return source
    if type_spec.kind == "handle":
        if type_spec.sequence_depth == 1:
            handle = spec.handles[type_spec.handle]
            helper_name = f"to_cpp_{_snake_name(_handle_list_c_type(handle))}"
            return f"{helper_name}({source})"
        if type_spec.sequence_depth == 2:
            handle = spec.handles[type_spec.handle]
            helper_name = f"to_cpp_{_snake_name(_handle_list_list_c_type(handle))}"
            return f"{helper_name}({source})"
        handle = spec.handles[type_spec.handle]
        if handle.ptr_type == "value":
            return f"{source}->value"
        if handle.ptr_type == "shared_ptr":
            return f"{source}->ptr"
        return f"{source}->ptr"
    if type_spec.kind == "opaque_ptr":
        cpp_type = _normalize_cpp_type(type_spec.cpp_type)
        if cpp_type.startswith("std::optional<") and cpp_type.endswith(">"):
            inner_type = cpp_type[len("std::optional<") : -1]
            return f"static_cast<{inner_type}>({source})"
        return f"static_cast<{type_spec.cpp_type}>({source})"
    raise ValueError(f"Unsupported option field kind: {type_spec.kind}")


def _uses_cpp_arg_name(type_spec: TypeSpec) -> bool:
    sequence_kind = _type_spec_sequence_kind(type_spec)
    if sequence_kind is not None:
        return True
    if type_spec.kind in _SCALAR_TYPE_MAP:
        return type_spec.cpp_type is not None
    if type_spec.kind == "string":
        return not type_spec.nullable or _is_optional_cpp_type(type_spec)
    if type_spec.kind == "handle":
        return True
    if type_spec.kind == "opaque_ptr":
        return True
    if type_spec.kind == "option":
        return True
    return False


def _constructor_arg(p: ParamSpec) -> str:
    """Build the expression for a single constructor argument."""
    if p.type.kind == "handle":
        if p.type.sequence_depth > 0:
            return f"{p.name}_cpp"
        if not _normalize_cpp_type(p.type.cpp_type):
            raise ValueError(
                f'Constructor handle parameter "{p.name}" requires cpp_type for source-derived passing'
            )
        return f"{p.name}_cpp"
    if _uses_cpp_arg_name(p.type):
        return f"{p.name}_cpp"
    return p.name


def _render_constructor(call: CallIR, op: ConstructorOp, spec: BindingIR) -> str:
    handle = spec.handles[call.returns.handle]
    cpp_class = op.cpp_class or handle.cpp_type
    arg_str = ", ".join(_constructor_arg(p) for p in call.params)
    new_expr = f"new {cpp_class}({arg_str})"

    if op.cpp_class and op.cpp_class != handle.cpp_type:
        new_expr = f"static_cast<{handle.cpp_type}*>({new_expr})"

    result_line = _render_result_assignment(call, spec, new_expr)

    if op.compile_guard:
        guard = op.compile_guard
        guard_message = op.compile_guard_message or f"{call.c_name} requires {guard}"
        # Use #if defined() for all guards; compound guards (containing "defined(") are used as-is
        guard_expr = guard if "defined(" in guard else f"defined({guard})"
        return (
            f"#if {guard_expr}\n"
            f"        {result_line}\n"
            f"#else\n"
            f"        throw std::runtime_error({_cpp_string_literal(guard_message)});\n"
            f"#endif"
        )

    return result_line


def _call_expr_args(call: CallIR) -> str:
    return ", ".join(
        f"{p.name}_cpp" if _uses_cpp_arg_name(p.type) else p.name for p in call.params
    )


def _render_bool_out_param_assignment(call: CallIR, op: BoolOutParamCallOp) -> str:
    if call.returns.kind not in _SCALAR_TYPE_MAP:
        raise ValueError(
            f"{call.c_name} bool out-param lowering only supports scalar return types"
        )
    out_name = "result_value"
    cpp_type = op.out_param_cpp_type.rstrip("&").strip()
    args = [
        f"{p.name}_cpp" if _uses_cpp_arg_name(p.type) else p.name for p in call.params
    ]
    args.append(out_name)
    call_expr = (
        f"self_cpp->{op.cpp_name}({', '.join(args)})"
        if call.receiver is not None
        else f"{op.cpp_name}({', '.join(args)})"
    )
    assign = _SCALAR_TYPE_MAP[call.returns.kind][2].format(expr=out_name)
    lines = [
        f"{cpp_type} {out_name}{{}};",
        f"if ({call_expr}) {{",
        f"    {assign}",
        "} else {",
    ]
    if call.returns.kind == "double":
        lines.append("    *out_result = std::numeric_limits<double>::quiet_NaN();")
    else:
        lines.append(f'    throw std::runtime_error("{op.cpp_name} failed");')
    lines.append("}")
    return "\n        ".join(lines)


def _receiver_arg_expr(receiver_cpp_type: str, receiver_handle: object) -> str:
    normalized = _normalize_cpp_type(receiver_cpp_type)
    while normalized.startswith("const "):
        normalized = normalized[len("const ") :].strip()
    if normalized.endswith("*"):
        return (
            "&self_cpp"
            if getattr(receiver_handle, "name", None) == "attribute_value"
            else "self_cpp"
        )
    if getattr(receiver_handle, "name", None) == "attribute_value":
        return "self_cpp"
    return "*self_cpp"


def _render_call_impl(call: CallIR, spec: BindingIR) -> str:
    params = [f"{_cpp_param_type(param, spec)} {param.name}" for param in call.params]
    if call.returns.kind != "void":
        params.append(f"{_out_param_type(call.returns, spec)} out_result")
    signature = f"bool {call.c_name}({', '.join(params) if params else 'void'})"

    prelude_lines = []
    if call.returns.kind != "void":
        prelude_lines.append(
            '    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }'
        )
    if call.receiver is not None:
        receiver_c_type = spec.handles[call.receiver].c_type
        receiver_name = "self"
        receiver_handle = spec.handles[call.receiver]
        if receiver_handle.name == "instance_list":
            prelude_lines.append(
                f'    if ({receiver_name} == nullptr) {{ throw std::runtime_error("Receiver handle is invalid"); }}'
            )
            prelude_lines.append(f"    auto* self_cpp = &{receiver_name}->value;")
        elif receiver_handle.name == "attribute_value":
            prelude_lines.append(
                f'    if ({receiver_name} == nullptr) {{ throw std::runtime_error("Receiver handle is invalid"); }}'
            )
            prelude_lines.append(f"    auto& self_cpp = {receiver_name}->value;")
        elif receiver_handle.ptr_type == "value":
            prelude_lines.append(
                f'    if ({receiver_name} == nullptr) {{ throw std::runtime_error("Receiver handle is invalid"); }}'
            )
            if receiver_handle.empty_check:
                empty_check = receiver_handle.empty_check.format(
                    value=f"{receiver_name}->value"
                )
                # Methods that are defined to return a scalar zero / false for empty
                # receiver values (e.g. instance id() and identity()) must not throw
                # but instead return the zero value and true.
                _EMPTY_SAFE_RETURN_ZERO = {
                    "ifcopenshell_instance_id",
                    "ifcopenshell_instance_identity",
                }
                if call.c_name in _EMPTY_SAFE_RETURN_ZERO:
                    zero_val = "0" if call.returns.kind == "uint32_t" else "false"
                    prelude_lines.append(f"    if ({empty_check}) {{")
                    prelude_lines.append(f"        *out_result = {zero_val};")
                    prelude_lines.append(f"        return true;")
                    prelude_lines.append(f"    }}")
                else:
                    prelude_lines.append(
                        f'    if ({empty_check}) {{ throw std::runtime_error("Receiver handle is invalid"); }}'
                    )
            prelude_lines.append(f"    auto* self_cpp = &{receiver_name}->value;")
        elif receiver_handle.ptr_type == "shared_ptr":
            # For shared_ptr handles, check that the shared_ptr is not null and use .get()
            prelude_lines.append(
                f'    if ({receiver_name} == nullptr || {receiver_name}->ptr == nullptr) {{ throw std::runtime_error("Receiver handle is invalid"); }}'
            )
            prelude_lines.append(f"    auto* self_cpp = {receiver_name}->ptr.get();")
        else:
            prelude_lines.append(
                f'    if ({receiver_name} == nullptr || {receiver_name}->ptr == nullptr) {{ throw std::runtime_error("Receiver handle is invalid"); }}'
            )
            prelude_lines.append(f"    auto* self_cpp = {receiver_name}->ptr;")
        params = [f"{receiver_c_type}* self"] + [
            f"{_cpp_param_type(param, spec)} {param.name}" for param in call.params
        ]
        if call.returns.kind != "void":
            params.append(f"{_out_param_type(call.returns, spec)} out_result")
        signature = f"bool {call.c_name}({', '.join(params)})"

    for param in call.params:
        block = _render_param_prelude(param, spec)
        if block:
            prelude_lines.append(block)

    op = call.operation
    if isinstance(op, DirectCallOp):
        call_target = (
            f"self_cpp->{op.cpp_name}" if call.receiver is not None else op.cpp_name
        )
        expr = f"{call_target}({_call_expr_args(call)})"
        body_line = _render_result_assignment(call, spec, expr)
    elif isinstance(op, SpecMethodFunctionCallOp):
        if call.receiver is None:
            raise ValueError(
                f"{call.c_name} has a spec-method operation without a receiver"
            )
        args = [_receiver_arg_expr(op.receiver_cpp_type, spec.handles[call.receiver])]
        call_args = _call_expr_args(call)
        if call_args:
            args.append(call_args)
        expr = f"{op.cpp_name}({', '.join(args)})"
        body_line = _render_result_assignment(call, spec, expr)
    elif isinstance(op, BoolOutParamCallOp):
        body_line = _render_bool_out_param_assignment(call, op)
    elif isinstance(op, FieldGetOp):
        expr = f"self_cpp->{op.field_name}"
        if op.array_element_cpp_type is not None:
            expr = f"std::vector<{op.array_element_cpp_type}>({expr}.begin(), {expr}.end())"
        if op.null_check:
            field_expr = f"self_cpp->{op.field_name}"
            null_guard = f'if (!{field_expr}) {{ throw std::runtime_error("{op.field_name} is not set"); }}\n        '
            body_line = null_guard + _render_result_assignment(call, spec, expr)
        else:
            body_line = _render_result_assignment(call, spec, expr)
    elif isinstance(op, ValueHandleFieldGetOp):
        target_handle = spec.handles[call.returns.handle]
        if target_handle.ptr_type == "value":
            expr = f"self_cpp->{op.field_name}"
        else:
            expr = (
                f"std::make_shared<{target_handle.cpp_type}>(self_cpp->{op.field_name})"
            )
        body_line = _render_result_assignment(call, spec, expr)
    elif isinstance(op, PointerPresenceCheckOp):
        body_line = f"*out_result = (self_cpp->{op.field_name} != nullptr);"
    elif isinstance(op, ChildrenCountOp):
        body_line = f"*out_result = self_cpp->{op.field_name}.size();"
    elif isinstance(op, ChildrenAtOp):
        body_line = (
            f'if (index >= self_cpp->{op.field_name}.size()) {{ throw std::runtime_error("Index out of bounds"); }}\n'
            f"        {_render_result_assignment(call, spec, f'self_cpp->{op.field_name}[index]')}"
        )
    elif isinstance(op, ChildrenAddOp):
        if op.cast_cpp_type:
            body_line = (
                f"auto cast_item = ifcopenshell::geometry::taxonomy::dcast<{op.cast_cpp_type}>(item_cpp);\n"
                f'        if (!cast_item) {{ throw std::runtime_error("Invalid item type"); }}\n'
                f"        self_cpp->{op.field_name}.push_back(cast_item);"
            )
        else:
            body_line = f"self_cpp->{op.field_name}.push_back(item_cpp);"
    elif isinstance(op, FieldSetterOp):
        body_line = f"self_cpp->{op.field_name} = value_cpp;"
    elif isinstance(op, MethodSizeOp):
        body_line = f"*out_result = self_cpp->{op.method_name}().size();"
    elif isinstance(op, MethodAtOp):
        body_line = (
            f"const auto& items = self_cpp->{op.method_name}();\n"
            f"        if (index >= items.size()) "
            f"{{ throw {op.exception_type}({_cpp_string_literal(op.out_of_range_message)}); }}\n"
            f"        {_render_result_assignment(call, spec, 'items[index]')}"
        )
    elif isinstance(op, ListCountOp):
        body_line = (
            f"(void)self_cpp;\n        *out_result = {op.list_param}_cpp->size();"
        )
    elif isinstance(op, ListAtOp):
        body_line = (
            f"(void)self_cpp;\n"
            f"        if (index >= {op.list_param}_cpp->size()) "
            f"{{ throw std::out_of_range({_cpp_string_literal(op.out_of_range_message)}); }}\n"
            f"        {_render_result_assignment(call, spec, f'new {op.item_cpp_type}((*{op.list_param}_cpp)[index])')}"
        )
    elif isinstance(op, ArrayElementFieldOp):
        body_line = _render_result_assignment(call, spec, f"self_cpp->{op.expression}")
    elif isinstance(op, OptionalPresenceCheckOp):
        body_line = f"*out_result = static_cast<bool>(self_cpp->{op.field_name});"
    elif isinstance(op, OptionalGetOp):
        null_guard = (
            f"if (!self_cpp->{op.field_name}) "
            f'{{ throw std::runtime_error("{op.field_name} is not set"); }}\n        '
        )
        body_line = null_guard + _render_result_assignment(
            call, spec, f"*self_cpp->{op.field_name}"
        )
    elif isinstance(op, StaticCastOp):
        target_handle = spec.handles[call.returns.handle]
        body_line = _render_result_assignment(
            call,
            spec,
            f"std::static_pointer_cast<{target_handle.cpp_type}>({op.expression})",
        )
    elif isinstance(op, CcomponentsAccessorOp):
        if op.dimensions <= 3:
            body_line = (
                f"const auto& v = self_cpp->{op.access_via}();\n"
                f"        {_render_result_assignment(call, spec, 'std::vector<double>{{v(0), v(1), v(2)}}')}"
            )
        else:
            body_line = (
                f"std::vector<double> data(16);\n"
                f"        const auto& mat = self_cpp->{op.access_via}();\n"
                f"        for (int i = 0; i < 4; ++i) {{\n"
                f"            for (int j = 0; j < 4; ++j) {{\n"
                f"                data[i * 4 + j] = mat(i, j);\n"
                f"            }}\n"
                f"        }}\n"
                f"        {_render_result_assignment(call, spec, 'data')}"
            )
    elif isinstance(op, VariantGetOp):
        getter_lines = [
            f"auto val = self_cpp->{op.method_name}(name_cpp);",
            "        bool matched = false;",
        ]
        for getter_type in op.getter_types:
            getter_lines.append(
                f"        if (auto* p = std::get_if<{getter_type}>(&val)) {{ {_render_result_assignment(call, spec, '*p')} matched = true; }}"
            )
        getter_lines.append(
            '        if (!matched) { throw std::runtime_error("Setting is not of expected type"); }'
        )
        body_line = "\n".join(getter_lines)
    elif isinstance(op, VariantSetOp):
        value_param = call.params[1].type if len(call.params) > 1 else None
        if (
            value_param is not None
            and _type_spec_sequence_kind(value_param) is not None
        ):
            value_expr = f"{op.variant_type}(value_cpp)"
        elif op.cpp_type == "int64_t":
            value_expr = f"{op.variant_type}(static_cast<int64_t>(value))"
        elif op.cpp_type == "double":
            value_expr = f"{op.variant_type}(value)"
        elif op.cpp_type == "bool":
            value_expr = f"{op.variant_type}(value)"
        elif op.cpp_type == "std::string":
            value_expr = f"{op.variant_type}(value_cpp)"
        else:
            value_expr = f"{op.variant_type}({op.cpp_type}(value))"
        body_line = f"self_cpp->{op.method_name}(name_cpp, {value_expr});"
    elif isinstance(op, ConstructorOp):
        body_line = _render_constructor(call, op, spec)
    else:
        if not isinstance(op, InlineImplementationOp):
            raise ValueError(f"Unsupported call operation for {call.c_name}")
        body = op.implementation.body.rstrip()
        if call.returns.kind == "void":
            body_line = f"[&]() {{\n{body}\n        }}();"
        else:
            body_line = (
                "auto generated_result = [&]() {\n" + body + "\n        }();\n        "
            )
            body_line += _render_result_assignment(call, spec, "generated_result")

    prelude = "\n".join(prelude_lines)
    if prelude:
        prelude += "\n"

    return f"""{signature} {{
    try {{
        {spec.c_prefix}_clear_error();
{prelude}        {body_line}
        return true;
    }} catch (const std::exception& e) {{
        set_last_error(e.what());
        return false;
    }} catch (...) {{
        set_last_error("Unknown C++ exception");
        return false;
    }}
}}"""
