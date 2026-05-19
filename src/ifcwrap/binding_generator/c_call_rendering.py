# This file was generated with the assistance of an AI coding tool.

from __future__ import annotations

try:
    from .authored_spec import ParamSpec, TypeSpec
    from .binding_ir import (
        ArrayElementFieldOp,
        BindingIR,
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
        MethodSizeOp,
        OptionalGetOp,
        OptionalPresenceCheckOp,
        PointerPresenceCheckOp,
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
    from authored_spec import ParamSpec, TypeSpec
    from binding_ir import (
        ArrayElementFieldOp,
        BindingIR,
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
        MethodSizeOp,
        OptionalGetOp,
        OptionalPresenceCheckOp,
        PointerPresenceCheckOp,
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
        helper = "make_static_string" if type_spec.ownership == "static" else "make_string"
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
        return f"*out_result = {_wrap_handle_expr(type_spec, expr, spec)};"
    if kind == "opaque_ptr":
        return f"*out_result = static_cast<void*>({expr});"
    if kind == "struct":
        if type_spec.struct is None:
            raise ValueError(f"{call.c_name} struct return is missing struct name")
        struct = spec.result_structs[type_spec.struct]
        lines = [f"auto result_value = {expr};"]
        for field in struct.fields:
            cpp_field = field.cpp_field or field.name
            field_expr = f"result_value.{cpp_field}"
            field_type = field.type
            field_sequence_kind = _type_spec_sequence_kind(field_type)
            if field_sequence_kind is not None:
                assignment = f"{_sequence_make_helper(field_sequence_kind)}({field_expr})"
            elif field_type.kind in _SCALAR_TYPE_MAP:
                assignment = f"static_cast<{_SCALAR_TYPE_MAP[field_type.kind][0]}>({field_expr})"
            elif field_type.kind == "string":
                helper = "make_static_string" if field_type.ownership == "static" else "make_string"
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
                raise ValueError(f"Unsupported result struct field kind: {field_type.kind}")
            lines.append(f"out_result->{field.name} = {assignment};")
        return "\n        ".join(lines)
    msg = f"Unsupported return kind: {kind}"
    raise ValueError(msg)


def _null_check(param_name: str, label: str) -> str:
    return f'    if ({param_name} == nullptr) {{ throw std::runtime_error("{label} \\"{param_name}\\" must not be null"); }}'


def _render_param_prelude(param: ParamSpec, spec: BindingIR) -> str:
    type_spec = param.type
    kind = type_spec.kind
    sequence_kind = _type_spec_sequence_kind(type_spec)
    if sequence_kind is not None:
        to_cpp = _sequence_to_cpp_helper(sequence_kind)
        set_element = _set_element_cpp_type(type_spec.cpp_type)
        if set_element is not None:
            return (
                f'{_null_check(param.name, "Parameter")}\n'
                f"    auto {param.name}_vec = {to_cpp}({param.name});\n"
                f"    std::set<{set_element}> {param.name}_cpp({param.name}_vec.begin(), {param.name}_vec.end());"
            )
        return (
            f'{_null_check(param.name, "Parameter")}\n'
            f"    auto {param.name}_cpp = {to_cpp}({param.name});"
        )
    if kind == "logical":
        return (
            f"    boost::logic::tribool {param.name}_cpp;\n"
            f"    if ({param.name} == IFCOPENSHELL_LOGICAL_FALSE) {{\n"
            f"        {param.name}_cpp = false;\n"
            f"    }} else if ({param.name} == IFCOPENSHELL_LOGICAL_TRUE) {{\n"
            f"        {param.name}_cpp = true;\n"
            f"    }} else if ({param.name} == IFCOPENSHELL_LOGICAL_UNKNOWN) {{\n"
            f"        {param.name}_cpp = boost::logic::indeterminate;\n"
            f"    }} else {{\n"
            f"        throw std::runtime_error(\"Logical parameter \\\"{param.name}\\\" must be -1, 0, or 1\");\n"
            f"    }}"
        )
    if kind in _SCALAR_TYPE_MAP and type_spec.cpp_type is not None:
        return f"    auto {param.name}_cpp = static_cast<{type_spec.cpp_type}>({param.name});"
    if kind == "string":
        if type_spec.nullable:
            return f"    const char* {param.name}_str = {param.name};"
        return (
            f'{_null_check(param.name, "Parameter")}\n'
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
                    f'{_null_check(param.name, "Parameter")}\n'
                    f"    auto {param.name}_vec = {helper_name}({param.name});\n"
                    f"    std::set<{set_element}> {param.name}_cpp({param.name}_vec.begin(), {param.name}_vec.end());"
                )
            return (
                f'{_null_check(param.name, "Parameter")}\n'
                f"    auto {param.name}_cpp = {helper_name}({param.name});"
            )
        if type_spec.sequence_depth == 2:
            handle = spec.handles[type_spec.handle]
            helper_name = f"to_cpp_{_snake_name(_handle_list_list_c_type(handle))}"
            return (
                f'{_null_check(param.name, "Parameter")}\n'
                f"    auto {param.name}_cpp = {helper_name}({param.name});"
            )
        handle = spec.handles[type_spec.handle]
        if handle.name in {"attribute_value", "instance_list"}:
            return (
                f'{_null_check(param.name, "Handle parameter")}\n'
                f"    auto {param.name}_cpp = {param.name}->value;"
            )
        cpp_type = _normalize_cpp_type(type_spec.cpp_type)
        is_shared = handle.ptr_type == "shared_ptr"
        # Check if cpp_type refers to the shared_ptr itself (e.g., "const T::ptr &")
        refers_to_shared_ptr = is_shared and cpp_type is not None and (
            "::ptr" in cpp_type or "shared_ptr" in cpp_type
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
        elif type_spec.cpp_type is not None and not cpp_type.endswith("*") and not is_shared:
            # Value parameter (discovered, raw-ptr handle): dereference to copy
            value_expr = f"*{param.name}->ptr"
            auto_kw = "auto"
        else:
            # Pointer, shared_ptr, or unspecified (adapter): keep as-is
            value_expr = f"{param.name}->ptr"
            auto_kw = "auto"
        if type_spec.nullable:
            return (
                f"    auto {param.name}_cpp = ({param.name} != nullptr && {param.name}->ptr != nullptr) ? {value_expr} : nullptr;"
            )
        return (
            f'    if ({param.name} == nullptr || {param.name}->ptr == nullptr) {{ throw std::runtime_error("Handle parameter \\"{param.name}\\" is invalid"); }}\n'
            f"    {auto_kw} {param.name}_cpp = {value_expr};"
        )
    if kind == "opaque_ptr":
        cpp_type = type_spec.cpp_type
        return (
            f'{_null_check(param.name, "Parameter")}\n'
            f"    auto {param.name}_cpp = static_cast<{cpp_type}>({param.name});"
        )
    return ""


def _uses_cpp_arg_name(type_spec: TypeSpec) -> bool:
    sequence_kind = _type_spec_sequence_kind(type_spec)
    if sequence_kind is not None:
        return True
    if type_spec.kind in _SCALAR_TYPE_MAP:
        return type_spec.cpp_type is not None
    if type_spec.kind == "string":
        return not type_spec.nullable
    if type_spec.kind == "handle":
        return True
    if type_spec.kind == "opaque_ptr":
        return True
    return False


def _constructor_arg(p: ParamSpec) -> str:
    """Build the expression for a single constructor argument."""
    if p.type.kind == "handle":
        if p.type.sequence_depth > 0:
            return f"{p.name}_cpp"
        return f"*{p.name}_cpp"
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
        # Use #if defined() for all guards; compound guards (containing "defined(") are used as-is
        guard_expr = guard if "defined(" in guard else f"defined({guard})"
        return (
            f"#if {guard_expr}\n"
            f"        {result_line}\n"
            f"#else\n"
            f'        throw std::runtime_error("{call.c_name} requires {guard}");\n'
            f"#endif"
        )

    return result_line


def _call_expr_args(call: CallIR) -> str:
    return ", ".join(
        f"{p.name}_cpp" if _uses_cpp_arg_name(p.type) else p.name
        for p in call.params
    )


def _render_call_impl(call: CallIR, spec: BindingIR) -> str:
    params = [f"{_cpp_param_type(param, spec)} {param.name}" for param in call.params]
    if call.returns.kind != "void":
        params.append(f"{_out_param_type(call.returns, spec)} out_result")
    signature = f"bool {call.c_name}({', '.join(params) if params else 'void'})"

    prelude_lines = []
    if call.returns.kind != "void":
        prelude_lines.append('    if (out_result == nullptr) { throw std::runtime_error("out_result must not be null"); }')
    if call.receiver is not None:
        receiver_c_type = spec.handles[call.receiver].c_type
        receiver_name = "self"
        receiver_handle = spec.handles[call.receiver]
        if receiver_handle.name == "instance_list":
            prelude_lines.append(
                f'    if ({receiver_name} == nullptr || !{receiver_name}->value) {{ throw std::runtime_error("Receiver handle is invalid"); }}'
            )
            prelude_lines.append(f"    auto self_cpp = {receiver_name}->value;")
        elif receiver_handle.name == "attribute_value":
            prelude_lines.append(
                f'    if ({receiver_name} == nullptr) {{ throw std::runtime_error("Receiver handle is invalid"); }}'
            )
            prelude_lines.append(f"    auto& self_cpp = {receiver_name}->value;")
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
        params = [f"{receiver_c_type}* self"] + [f"{_cpp_param_type(param, spec)} {param.name}" for param in call.params]
        if call.returns.kind != "void":
            params.append(f"{_out_param_type(call.returns, spec)} out_result")
        signature = f"bool {call.c_name}({', '.join(params)})"

    for param in call.params:
        block = _render_param_prelude(param, spec)
        if block:
            prelude_lines.append(block)

    op = call.operation
    if isinstance(op, DirectCallOp):
        call_target = f"self_cpp->{op.cpp_name}" if call.receiver is not None else op.cpp_name
        expr = f"{call_target}({_call_expr_args(call)})"
        body_line = _render_result_assignment(call, spec, expr)
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
        expr = f"std::make_shared<{target_handle.cpp_type}>(self_cpp->{op.field_name})"
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
    elif isinstance(op, ArrayElementFieldOp):
        body_line = _render_result_assignment(call, spec, f"self_cpp->{op.expression}")
    elif isinstance(op, OptionalPresenceCheckOp):
        body_line = f"*out_result = self_cpp->{op.field_name}.is_initialized();"
    elif isinstance(op, OptionalGetOp):
        null_guard = (
            f'if (!self_cpp->{op.field_name}.is_initialized()) '
            f'{{ throw std::runtime_error("{op.field_name} is not set"); }}\n        '
        )
        body_line = null_guard + _render_result_assignment(call, spec, f"*self_cpp->{op.field_name}")
    elif isinstance(op, StaticCastOp):
        target_handle = spec.handles[call.returns.handle]
        body_line = _render_result_assignment(
            call, spec,
            f"std::static_pointer_cast<{target_handle.cpp_type}>({op.expression})"
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
        getter_lines = [f"auto val = self_cpp->{op.method_name}(name_cpp);", "        bool matched = false;"]
        for getter_type in op.getter_types:
            getter_lines.append(
                f"        if (auto* p = boost::get<{getter_type}>(&val)) {{ {_render_result_assignment(call, spec, '*p')} matched = true; }}"
            )
        getter_lines.append('        if (!matched) { throw std::runtime_error("Setting is not of expected type"); }')
        body_line = "\n".join(getter_lines)
    elif isinstance(op, VariantSetOp):
        value_param = call.params[1].type if len(call.params) > 1 else None
        if value_param is not None and _type_spec_sequence_kind(value_param) is not None:
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
            body_line = "auto generated_result = [&]() {\n" + body + "\n        }();\n        "
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
