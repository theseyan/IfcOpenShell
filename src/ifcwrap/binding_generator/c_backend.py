from __future__ import annotations

import argparse
from pathlib import Path
from typing import Union

try:
    from .authored_spec import AuthoredBindingSpec, HandleSpec, MergedBindingSpec, ParamSpec, TypeSpec, load_authored_spec, load_merged_specs
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
        lower_binding_spec,
    )
    from .c_sequence_helpers import (
        _handle_list_c_type,
        _handle_list_helper_name,
        _handle_list_list_c_type,
        _handle_list_list_helper_name,
        _render_common_type_impls,
        _render_handle_list_destroy_impl,
        _render_handle_list_helpers,
        _render_handle_list_list_destroy_impl,
        _render_handle_list_list_helpers,
        _render_sequence_helpers,
        _sequence_make_helper,
        _sequence_to_cpp_helper,
        _snake_name,
        _type_spec_sequence_kind,
        _used_handle_list_handles,
        _used_scalar_sequence_kinds,
    )
    from .c_handle_rendering import _destroy_body, _handle_storage_type, _wrap_handle_expr
    from .c_header_rendering import _render_header
    from .c_internal_header import _render_internal_header
    from .debug import debug_log, debug_path
    from .python_ctypes_backend import generate_python_ctypes
    from .c_type_rendering import (
        _BUFFER_TYPE_MAP,
        _SCALAR_TYPE_MAP,
        _cpp_param_type,
        _normalize_cpp_type,
        _out_param_type,
        _qualify_handle_cpp_fragment,
    )
except ImportError:  # pragma: no cover - script execution fallback
    from authored_spec import AuthoredBindingSpec, HandleSpec, MergedBindingSpec, ParamSpec, TypeSpec, load_authored_spec, load_merged_specs
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
        lower_binding_spec,
    )
    from c_sequence_helpers import (
        _handle_list_c_type,
        _handle_list_helper_name,
        _handle_list_list_c_type,
        _handle_list_list_helper_name,
        _render_common_type_impls,
        _render_handle_list_destroy_impl,
        _render_handle_list_helpers,
        _render_handle_list_list_destroy_impl,
        _render_handle_list_list_helpers,
        _render_sequence_helpers,
        _sequence_make_helper,
        _sequence_to_cpp_helper,
        _snake_name,
        _type_spec_sequence_kind,
        _used_handle_list_handles,
        _used_scalar_sequence_kinds,
    )
    from c_handle_rendering import _destroy_body, _handle_storage_type, _wrap_handle_expr
    from c_header_rendering import _render_header
    from c_internal_header import _render_internal_header
    from debug import debug_log, debug_path
    from python_ctypes_backend import generate_python_ctypes
    from c_type_rendering import (
        _BUFFER_TYPE_MAP,
        _SCALAR_TYPE_MAP,
        _cpp_param_type,
        _normalize_cpp_type,
        _out_param_type,
        _qualify_handle_cpp_fragment,
    )

# Type alias for spec types
SourceBindingSpec = Union[AuthoredBindingSpec, MergedBindingSpec]

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


def _render_cpp(spec: BindingIR, header_name: str) -> str:
    debug_log(
        "c_backend.render_cpp.start",
        f"module={spec.module} handles={len(spec.handles)} functions={len(spec.functions)} methods={len(spec.methods)}",
    )
    handle_structs = []
    for handle in spec.handles.values():
        storage_type = _handle_storage_type(handle)
        if handle.name in {"attribute_value", "instance_list"}:
            handle_structs.append(f"struct {handle.c_type} {{\n    {storage_type} value;\n}};")
        elif handle.ptr_type == "shared_ptr":
            # shared_ptr handles don't need owned field - ref counting handles ownership
            handle_structs.append(f"struct {handle.c_type} {{\n    {storage_type} ptr;\n}};")
        else:
            handle_structs.append(f"struct {handle.c_type} {{\n    {storage_type} ptr;\n    bool owned;\n}};")

    destroy_impls = []
    for handle in spec.handles.values():
        destroy_impls.append(
            f"""void ifcopenshell_{_snake_name(handle.c_type)}_destroy({handle.c_type}* handle) {{
    if (handle == nullptr) {{
        return;
    }}
    {_destroy_body(handle)}
}}"""
        )

    rendered_calls: list[str] = []
    total_calls = len(spec.functions) + len(spec.methods)
    for index, call in enumerate((*spec.functions, *spec.methods), start=1):
        if index == 1 or index % 100 == 0 or index == total_calls:
            debug_log("c_backend.render_cpp.calls", f"{index}/{total_calls} current={call.c_name}")
        rendered_calls.append(_render_call_impl(call, spec))
    call_impls = "\n\n".join(rendered_calls)
    includes = "\n".join(
        f'#include {header}' if header.startswith('<') else f'#include "{header}"'
        for header in spec.public_headers
    )
    handle_structs_block = "\n\n".join(handle_structs)
    destroy_impls_block = "\n\n".join(destroy_impls)
    handle_list_types = _used_handle_list_handles(spec)
    sequence_kinds = _used_scalar_sequence_kinds(spec)
    handle_list_helpers = "\n\n".join(
        _render_handle_list_helpers(handle) for handle in handle_list_types
    )
    handle_list_list_helpers = "\n\n".join(
        _render_handle_list_list_helpers(handle) for handle in handle_list_types
    )
    handle_list_destroy_impls = "\n\n".join(_render_handle_list_destroy_impl(handle) for handle in handle_list_types)
    handle_list_list_destroy_impls = "\n\n".join(
        _render_handle_list_list_destroy_impl(handle) for handle in handle_list_types
    )

    # Only emit common type implementations if this is not a dependent module.
    depends_on_common = spec.depends_on_common
    common_type_impls = "" if depends_on_common else _render_common_type_impls(sequence_kinds)
    sequence_helpers = _render_sequence_helpers(sequence_kinds)

    internal_header_name = header_name.removesuffix(".h") + "_internal.hpp"
    rendered = f"""#include "{header_name}"
#include "{internal_header_name}"

#include <algorithm>
#include <cstring>
#include <fstream>
#include <iterator>
#include <memory>
#include <new>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

// Note: project-specific headers (geometry, serializers, schema, etc.) are
// pulled in transitively via {internal_header_name}; do not re-include them
// here to avoid header-guard-less redefinitions in third-party headers.
#include "utils.h"

// Error reporting state. Defined in the named ifcopenshell::capi namespace so
// that external translation units can participate via the internal header.
namespace ifcopenshell {{
namespace capi {{
thread_local std::string g_last_error;
thread_local int g_last_error_kind = 0;

void set_last_error(const std::string& message) {{
    g_last_error_kind = 1;
    g_last_error = message;
}}

void set_last_error(int kind, const std::string& message) {{
    g_last_error_kind = kind;
    g_last_error = message;
}}
}} // namespace capi
}} // namespace ifcopenshell

namespace {{
using ifcopenshell::capi::g_last_error;
using ifcopenshell::capi::set_last_error;

bool feature_use_attribute_value_derived = false;
std::stringstream ifcopenshell_log_stream;
bool g_log_stream_initialized = false;

void ensure_log_stream_initialized() {{
    if (!g_log_stream_initialized) {{
        Logger::SetOutput(nullptr, &ifcopenshell_log_stream);
        g_log_stream_initialized = true;
    }}
}}

IfcUtil::ArgumentType helper_fn_attribute_type(const IfcUtil::IfcBaseClass* inst, unsigned index) {{
    const IfcParse::parameter_type* parameter_type = nullptr;
    if (inst->declaration().as_entity()) {{
        parameter_type = inst->declaration().as_entity()->attribute_by_index(index)->type_of_attribute();
        if (inst->declaration().as_entity()->derived()[index]) {{
            return IfcUtil::Argument_DERIVED;
        }}
    }} else if (inst->declaration().as_type_declaration() && index == 0) {{
        parameter_type = inst->declaration().as_type_declaration()->declared_type();
    }} else if (inst->declaration().as_enumeration_type() && index == 0) {{
        return IfcUtil::Argument_STRING;
    }}

    if (parameter_type == nullptr) {{
        return IfcUtil::Argument_UNKNOWN;
    }}
    return IfcUtil::from_parameter_type(parameter_type);
}}

void validate_list_items(const char* name, const void* items, size_t size) {{
    if (size > 0 && items == nullptr) {{
        throw std::runtime_error(std::string("Parameter '") + name + "' has a null items pointer");
    }}
}}

template <typename T>
struct is_std_vector : std::false_type {{}};

template <typename T, typename Alloc>
struct is_std_vector<std::vector<T, Alloc>> : std::true_type {{}};

template <typename T>
inline constexpr bool is_std_vector_v = is_std_vector<T>::value;

std::string json_escape_string(const std::string& value) {{
    std::ostringstream out;
    for (unsigned char ch : value) {{
        switch (ch) {{
        case '\\\\': out << "\\\\\\\\"; break;
        case '"': out << "\\\\\\""; break;
        case '\\b': out << "\\\\b"; break;
        case '\\f': out << "\\\\f"; break;
        case '\\n': out << "\\\\n"; break;
        case '\\r': out << "\\\\r"; break;
        case '\\t': out << "\\\\t"; break;
        default:
            if (ch < 0x20) {{
                out << "\\\\u"
                    << "00"
                    << "0123456789abcdef"[ch >> 4]
                    << "0123456789abcdef"[ch & 0x0f];
            }} else {{
                out << static_cast<char>(ch);
            }}
        }}
    }}
    return out.str();
}}

std::string json_quote(const std::string& value) {{
    return std::string(1, '"') + json_escape_string(value) + '"';
}}

std::string instance_to_info_json_string(const IfcUtil::IfcBaseClass* instance, bool include_identifier);

template <typename T>
std::string value_to_json_string(const T& value, bool include_identifier);

template <typename T>
std::string vector_to_json_string(const std::vector<T>& values, bool include_identifier) {{
    std::ostringstream out;
    out << "[";
    for (size_t i = 0; i < values.size(); ++i) {{
        if (i != 0) {{
            out << ",";
        }}
        out << value_to_json_string(values[i], include_identifier);
    }}
    out << "]";
    return out.str();
}}

std::string reference_or_simple_type_to_json_string(const IfcParse::reference_or_simple_type& value, bool include_identifier) {{
    if (auto* instance = std::get_if<IfcUtil::IfcBaseClass*>(&value)) {{
        return *instance ? instance_to_info_json_string(*instance, include_identifier) : "null";
    }}
    auto reference = std::get<IfcParse::InstanceReference>(value);
    return std::string(R"({{"ref":)") + std::to_string(reference.v) + "}}";
}}

template <typename T>
std::string value_to_json_string(const T& value, bool include_identifier) {{
    if constexpr (is_std_vector_v<T>) {{
        return vector_to_json_string(value, include_identifier);
    }} else if constexpr (std::is_same_v<T, std::string>) {{
        return json_quote(value);
    }} else if constexpr (std::is_same_v<T, const char*>) {{
        return value ? json_quote(value) : "null";
    }} else if constexpr (std::is_same_v<T, bool>) {{
        return value ? "true" : "false";
    }} else if constexpr (std::is_same_v<T, boost::logic::tribool>) {{
        if (boost::logic::indeterminate(value)) {{
            return "null";
        }}
        return value ? "true" : "false";
    }} else if constexpr (std::is_integral_v<T> || std::is_floating_point_v<T>) {{
        std::ostringstream out;
        out << value;
        return out.str();
    }} else if constexpr (std::is_same_v<T, EnumerationReference>) {{
        return json_quote(std::string(value.value()));
    }} else if constexpr (std::is_same_v<T, IfcParse::reference_or_simple_type>) {{
        return reference_or_simple_type_to_json_string(value, include_identifier);
    }} else if constexpr (std::is_same_v<T, IfcUtil::IfcBaseClass*>) {{
        return value ? instance_to_info_json_string(value, include_identifier) : "null";
    }} else if constexpr (std::is_same_v<T, aggregate_of_instance::ptr>) {{
        if (!value) {{
            return "null";
        }}
        std::ostringstream out;
        out << "[";
        for (size_t i = 0; i < value->size(); ++i) {{
            if (i != 0) {{
                out << ",";
            }}
            out << ((*value)[static_cast<int>(i)] ? instance_to_info_json_string((*value)[static_cast<int>(i)], include_identifier) : "null");
        }}
        out << "]";
        return out.str();
    }} else if constexpr (std::is_same_v<T, aggregate_of_aggregate_of_instance::ptr>) {{
        if (!value) {{
            return "null";
        }}
        std::ostringstream out;
        out << "[";
        size_t outer_index = 0;
        for (auto outer = value->begin(); outer != value->end(); ++outer, ++outer_index) {{
            if (outer_index != 0) {{
                out << ",";
            }}
            out << "[";
            for (size_t inner_index = 0; inner_index < outer->size(); ++inner_index) {{
                if (inner_index != 0) {{
                    out << ",";
                }}
                auto* instance = (*outer)[inner_index];
                out << (instance ? instance_to_info_json_string(instance, include_identifier) : "null");
            }}
            out << "]";
        }}
        out << "]";
        return out.str();
    }} else if constexpr (
        std::is_same_v<T, empty_aggregate_t> ||
        std::is_same_v<T, empty_aggregate_of_aggregate_t> ||
        std::is_same_v<T, Blank> ||
        std::is_same_v<T, Derived>) {{
        return "null";
    }} else {{
        return json_quote("<unsupported>");
    }}
}}

std::string attribute_value_to_json_string(const AttributeValue& value, bool include_identifier) {{
    return value.apply_visitor([include_identifier](const auto& inner) -> std::string {{
        return value_to_json_string(inner, include_identifier);
    }});
}}

std::string instance_to_info_json_string(const IfcUtil::IfcBaseClass* instance, bool include_identifier) {{
    if (instance == nullptr) {{
        return "null";
    }}
    std::ostringstream out;
    out << "{{";
    bool first = true;
    auto emit_field = [&](const std::string& key, const std::string& json_value) {{
        if (!first) {{
            out << ",";
        }}
        first = false;
        out << json_quote(key) << ":" << json_value;
    }};

    if (instance->declaration().as_entity()) {{
        const auto attributes = instance->declaration().as_entity()->all_attributes();
        for (size_t i = 0; i < attributes.size(); ++i) {{
            emit_field(attributes[i]->name(), attribute_value_to_json_string(instance->get_attribute_value(i), include_identifier));
        }}
        if (include_identifier) {{
            emit_field("id", std::to_string(instance->as<IfcUtil::IfcBaseEntity>()->id()));
        }}
    }} else {{
        emit_field("wrappedValue", attribute_value_to_json_string(instance->get_attribute_value(0), include_identifier));
    }}

    emit_field("type", json_quote(instance->declaration().name()));
    out << "}}";
    return out.str();
}}

std::string unresolved_reference_variant_to_json_string(
    const std::variant<
        IfcParse::reference_or_simple_type,
        std::vector<IfcParse::reference_or_simple_type>,
        std::vector<std::vector<IfcParse::reference_or_simple_type>>>& value,
    bool include_identifier) {{
    return std::visit(
        [include_identifier](const auto& inner) -> std::string {{
            return value_to_json_string(inner, include_identifier);
        }},
        value);
}}

std::string unresolved_references_to_json_string(
    const IfcParse::unresolved_references& references,
    const IfcParse::declaration* declaration) {{
    std::ostringstream out;
    out << "[";
    bool first = true;
    for (const auto& entry : references) {{
        if (!first) {{
            out << ",";
        }}
        first = false;
        const auto& mutable_value = entry.first;
        out << "{{"
            << R"("entity_name":)" << mutable_value.name_
            << R"(,"attribute_index":)" << static_cast<int>(mutable_value.index_);
        if (declaration && declaration->as_entity() && mutable_value.index_ < declaration->as_entity()->attribute_count()) {{
            out << R"(,"attribute_name":)" << json_quote(declaration->as_entity()->attribute_by_index(mutable_value.index_)->name());
        }}
        out << R"(,"value":)" << unresolved_reference_variant_to_json_string(entry.second, true) << "}}";
    }}
    out << "]";
    return out.str();
}}

template <typename Inverses>
std::string inverses_to_json_string(const Inverses& inverses) {{
    std::ostringstream out;
    out << "[";
    bool first = true;
    for (const auto& entry : inverses) {{
        if (!first) {{
            out << ",";
        }}
        first = false;
        out << "{{"
            << R"("instance_id":)" << std::get<0>(entry.first)
            << R"(,"instance_type":)" << std::get<1>(entry.first)
            << R"(,"attribute_index":)" << std::get<2>(entry.first)
            << R"(,"referencing_ids":)" << vector_to_json_string(entry.second, true)
            << "}}";
    }}
    out << "]";
    return out.str();
}}

std::string instance_stream_read_instance_json(IfcParse::InstanceStreamer* streamer, bool type_as_declaration_instance) {{
    if (!(*streamer)) {{
        return "null";
    }}
    auto inst = streamer->readInstance();
    if (!inst) {{
        return "null";
    }}

    std::ostringstream out;
    out << "{{";
    bool first = true;
    auto emit_field = [&](const std::string& key, const std::string& json_value) {{
        if (!first) {{
            out << ",";
        }}
        first = false;
        out << json_quote(key) << ":" << json_value;
    }};

    emit_field("id", std::to_string(std::get<0>(*inst)));
    if (type_as_declaration_instance) {{
        emit_field("type", json_quote(std::get<1>(*inst)->name()));
    }} else {{
        emit_field("type", json_quote(std::get<1>(*inst)->name()));
    }}

    const auto* declaration = std::get<1>(*inst);
    const auto& data = std::get<2>(*inst);
    if (declaration->as_entity()) {{
        for (size_t i = 0; i < declaration->as_entity()->attribute_count(); ++i) {{
            emit_field(
                declaration->as_entity()->attribute_by_index(i)->name(),
                attribute_value_to_json_string(data.get_attribute_value(nullptr, declaration, 0, i), true));
        }}
    }}

    for (const auto& reference : streamer->references()) {{
        std::string key = std::to_string(reference.first.index_);
        if (declaration->as_entity() && reference.first.index_ < declaration->as_entity()->attribute_count()) {{
            key = declaration->as_entity()->attribute_by_index(reference.first.index_)->name();
        }}
        emit_field(key, unresolved_reference_variant_to_json_string(reference.second, true));
    }}

    streamer->references().clear();
    streamer->inverses().clear();
    out << "}}";
    return out.str();
}}

ifcopenshell_string_t make_string(const std::string& value) {{
    char* data = new char[value.size() + 1];
    std::memcpy(data, value.c_str(), value.size() + 1);
    return ifcopenshell_string_t{{data, value.size(), true}};
}}

ifcopenshell_string_t make_static_string(const char* value) {{
    if (value == nullptr) {{
        return ifcopenshell_string_t{{nullptr, 0, false}};
    }}
    return ifcopenshell_string_t{{const_cast<char*>(value), std::strlen(value), false}};
}}

template <typename T>
void set_instance_argument(IfcUtil::IfcBaseClass* instance, size_t index, const T& value) {{
    instance->set_attribute_value(index, value);
    if (auto* entity = instance->as<IfcUtil::IfcBaseEntity>()) {{
        entity->populate_derived();
    }}
}}

void unset_instance_argument(IfcUtil::IfcBaseClass* instance, size_t index) {{
    set_instance_argument(instance, index, Blank{{}});
}}

void set_instance_attribute_from_attribute_value(IfcUtil::IfcBaseClass* instance, size_t index, const AttributeValue& value) {{
    value.apply_visitor([&](const auto& inner) -> void {{
        using T = std::decay_t<decltype(inner)>;
        if constexpr (std::is_same_v<T, Derived>) {{
            throw std::runtime_error("Cannot assign a derived attribute sentinel.");
        }} else if constexpr (std::is_same_v<T, empty_aggregate_t> ||
                              std::is_same_v<T, empty_aggregate_of_aggregate_t>) {{
            // Empty-aggregate sentinels arise when reading absent aggregate
            // values; assigning them is equivalent to unsetting the attribute.
            unset_instance_argument(instance, index);
        }} else {{
            set_instance_argument(instance, index, inner);
        }}
    }});
}}
}} // namespace

{handle_list_helpers}
{handle_list_list_helpers}

{common_type_impls}

{sequence_helpers}

void {spec.c_prefix}_clear_error(void) {{
    ifcopenshell::capi::g_last_error.clear();
    ifcopenshell::capi::g_last_error_kind = 0;
}}

const char* {spec.c_prefix}_last_error_message(void) {{
    return ifcopenshell::capi::g_last_error.c_str();
}}

int {spec.c_prefix}_last_error_kind(void) {{
    return ifcopenshell::capi::g_last_error_kind;
}}

{destroy_impls_block}
{handle_list_destroy_impls}
{handle_list_list_destroy_impls}

{call_impls}
"""
    debug_log("c_backend.render_cpp.done", f"module={spec.module} bytes={len(rendered)}")
    return rendered


def generate(
    spec_path: Path,
    header_out: Path,
    cpp_out: Path,
    compile_commands_path: Path | None = None,
    internal_header_out: Path | None = None,
    python_out: Path | None = None,
) -> None:
    debug_log(
        "c_backend.generate.start",
        f"spec={debug_path(spec_path)} header_out={debug_path(header_out)} cpp_out={debug_path(cpp_out)} internal_header_out={debug_path(internal_header_out)} compile_commands={debug_path(compile_commands_path)}",
    )
    spec = lower_binding_spec(load_authored_spec(spec_path, compile_commands_path=compile_commands_path))
    header_out.parent.mkdir(parents=True, exist_ok=True)
    cpp_out.parent.mkdir(parents=True, exist_ok=True)
    header_out.write_text(_render_header(spec), encoding="utf-8")
    cpp_out.write_text(_render_cpp(spec, header_out.name), encoding="utf-8")
    if internal_header_out is None:
        internal_header_out = cpp_out.with_name(header_out.stem + "_internal.hpp")
    internal_header_out.parent.mkdir(parents=True, exist_ok=True)
    internal_header_out.write_text(_render_internal_header(spec, header_out.name), encoding="utf-8")
    if python_out is not None:
        python_out.parent.mkdir(parents=True, exist_ok=True)
        generate_python_ctypes(spec, python_out, generic_handles=True)
    debug_log("c_backend.generate.done", f"spec={debug_path(spec_path)}")


def generate_merged(
    spec_paths: list[Path],
    module: str,
    c_prefix: str,
    header_out: Path,
    cpp_out: Path,
    compile_commands_path: Path | None = None,
    internal_header_out: Path | None = None,
    python_out: Path | None = None,
) -> None:
    """Generate bindings from multiple specs merged together."""
    debug_log(
        "c_backend.generate_merged.start",
        f"specs={len(spec_paths)} header_out={debug_path(header_out)} cpp_out={debug_path(cpp_out)} internal_header_out={debug_path(internal_header_out)} compile_commands={debug_path(compile_commands_path)}",
    )
    spec = lower_binding_spec(load_merged_specs(spec_paths, module, c_prefix, compile_commands_path=compile_commands_path))
    header_out.parent.mkdir(parents=True, exist_ok=True)
    cpp_out.parent.mkdir(parents=True, exist_ok=True)
    header_out.write_text(_render_header(spec), encoding="utf-8")
    cpp_out.write_text(_render_cpp(spec, header_out.name), encoding="utf-8")
    if internal_header_out is None:
        internal_header_out = cpp_out.with_name(header_out.stem + "_internal.hpp")
    internal_header_out.parent.mkdir(parents=True, exist_ok=True)
    internal_header_out.write_text(_render_internal_header(spec, header_out.name), encoding="utf-8")
    if python_out is not None:
        python_out.parent.mkdir(parents=True, exist_ok=True)
        generate_python_ctypes(spec, python_out, generic_handles=True)
    debug_log("c_backend.generate_merged.done", f"module={module}")


def _build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(description="Generate the first C backend skeleton from a handwritten binding spec.")
    parser.add_argument("--spec", type=Path, action="append", required=True, 
                        help="Path to a binding spec YAML. Can be specified multiple times for merged output.")
    parser.add_argument("--header-out", type=Path, required=True, help="Output path for the generated C header.")
    parser.add_argument("--cpp-out", type=Path, required=True, help="Output path for the generated C++ glue source.")
    parser.add_argument(
        "--internal-header-out",
        type=Path,
        default=None,
        help="Optional output path for the internal C++ header that exposes handle struct definitions and error helpers. Defaults to <cpp-out-dir>/<header-stem>_internal.hpp.",
    )
    parser.add_argument(
        "--python-out",
        type=Path,
        default=None,
        help="Optional output path for generated Python ctypes glue.",
    )
    parser.add_argument(
        "--compile-commands",
        type=Path,
        default=None,
        help="Optional path to compile_commands.json for AST-backed source discovery.",
    )
    parser.add_argument(
        "--module",
        type=str,
        default="ifcopenshell",
        help="Module name for merged output (default: ifcopenshell).",
    )
    parser.add_argument(
        "--c-prefix",
        type=str,
        default="ifcopenshell",
        help="C function prefix for merged output (default: ifcopenshell).",
    )
    return parser


def main() -> int:
    args = _build_parser().parse_args()
    if len(args.spec) == 1:
        # Single spec - use original behavior
        generate(
            args.spec[0],
            args.header_out,
            args.cpp_out,
            compile_commands_path=args.compile_commands,
            internal_header_out=args.internal_header_out,
            python_out=args.python_out,
        )
    else:
        # Multiple specs - merge them
        generate_merged(
            args.spec,
            args.module,
            args.c_prefix,
            args.header_out,
            args.cpp_out,
            compile_commands_path=args.compile_commands,
            internal_header_out=args.internal_header_out,
            python_out=args.python_out,
        )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
