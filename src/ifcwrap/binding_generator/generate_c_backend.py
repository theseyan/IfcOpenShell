# This file was generated with the assistance of an AI coding tool.

from __future__ import annotations

import argparse
from pathlib import Path
import re

try:
    from .authored_spec import AuthoredBindingSpec, CallSpec, HandleSpec, ParamSpec, TypeSpec, load_authored_spec
except ImportError:  # pragma: no cover - script execution fallback
    from authored_spec import AuthoredBindingSpec, CallSpec, HandleSpec, ParamSpec, TypeSpec, load_authored_spec

AI_HEADER = "// This file was generated with the assistance of an AI coding tool.\n"


def _snake_name(c_type: str) -> str:
    base = c_type.removeprefix("ifcopenshell_").removesuffix("_t")
    return base


def _handle_list_c_type(handle: HandleSpec) -> str:
    return f"{handle.c_type.removesuffix('_t')}_list_t"


def _handle_list_helper_name(handle: HandleSpec) -> str:
    return f"make_{_snake_name(_handle_list_c_type(handle))}"


def _used_handle_list_handles(spec: AuthoredBindingSpec) -> tuple[HandleSpec, ...]:
    seen: set[str] = set()
    handles: list[HandleSpec] = []
    for call in (*spec.functions, *spec.methods):
        if call.returns.kind == "handle_list":
            handle_name = call.returns.handle
            if handle_name not in seen:
                seen.add(handle_name)
                handles.append(spec.handles[handle_name])
        for param in call.params:
            if param.type.kind != "handle_list":
                continue
            handle_name = param.type.handle
            if handle_name in seen:
                continue
            seen.add(handle_name)
            handles.append(spec.handles[handle_name])
    return tuple(handles)


def _normalize_cpp_type(cpp_type: str | None) -> str:
    if not cpp_type:
        return ""
    cpp_type = re.sub(r"/\*.*?\*/", "", cpp_type)
    return " ".join(
        cpp_type.replace(" &", "&").replace(" *", "*").replace("< ", "<").replace(" >", ">").split()
    )


def _cpp_param_type(param: ParamSpec, spec: AuthoredBindingSpec) -> str:
    type_spec = param.type
    if type_spec.kind == "bool":
        return "bool"
    if type_spec.kind == "bool_list":
        return "const ifcopenshell_bool_list_t*"
    if type_spec.kind == "int32":
        return "int32_t"
    if type_spec.kind == "double":
        return "double"
    if type_spec.kind == "uint32":
        return "uint32_t"
    if type_spec.kind == "size":
        return "size_t"
    if type_spec.kind == "string":
        return "const char*"
    if type_spec.kind == "string_list":
        return "const ifcopenshell_string_list_t*"
    if type_spec.kind == "int32_list":
        return "const ifcopenshell_int32_list_t*"
    if type_spec.kind == "uint32_list":
        return "const ifcopenshell_uint32_list_t*"
    if type_spec.kind == "int32_list_list":
        return "const ifcopenshell_int32_list_list_t*"
    if type_spec.kind == "double_list":
        return "const ifcopenshell_double_list_t*"
    if type_spec.kind == "double_list_list":
        return "const ifcopenshell_double_list_list_t*"
    if type_spec.kind == "handle":
        return f"{spec.handles[type_spec.handle].c_type}*"
    if type_spec.kind == "handle_list":
        return f"const {_handle_list_c_type(spec.handles[type_spec.handle])}*"
    msg = f"Unsupported parameter kind: {type_spec.kind}"
    raise ValueError(msg)


def _header_param_type(param: ParamSpec, spec: AuthoredBindingSpec) -> str:
    return _cpp_param_type(param, spec)


def _out_param_type(type_spec: TypeSpec, spec: AuthoredBindingSpec) -> str:
    if type_spec.kind == "void":
        msg = "void has no out parameter"
        raise ValueError(msg)
    if type_spec.kind in {"bool", "int32", "double", "uint32", "size"}:
        mapping = {
            "bool": "bool",
            "int32": "int32_t",
            "double": "double",
            "uint32": "uint32_t",
            "size": "size_t",
        }
        return f"{mapping[type_spec.kind]}*"
    if type_spec.kind == "bool_list":
        return "ifcopenshell_bool_list_t*"
    if type_spec.kind == "string":
        return "ifcopenshell_string_t*"
    if type_spec.kind == "string_list":
        return "ifcopenshell_string_list_t*"
    if type_spec.kind == "int32_list":
        return "ifcopenshell_int32_list_t*"
    if type_spec.kind == "uint32_list":
        return "ifcopenshell_uint32_list_t*"
    if type_spec.kind == "int32_list_list":
        return "ifcopenshell_int32_list_list_t*"
    if type_spec.kind == "double_list":
        return "ifcopenshell_double_list_t*"
    if type_spec.kind == "double_list_list":
        return "ifcopenshell_double_list_list_t*"
    if type_spec.kind == "handle":
        return f"{spec.handles[type_spec.handle].c_type}**"
    if type_spec.kind == "handle_list":
        return f"{_handle_list_c_type(spec.handles[type_spec.handle])}*"
    msg = f"Unsupported return kind: {type_spec.kind}"
    raise ValueError(msg)


def _render_call_decl(call: CallSpec, spec: AuthoredBindingSpec) -> str:
    parts = []
    if call.receiver is not None:
        parts.append(f"{spec.handles[call.receiver].c_type}* self")
    parts.extend(f"{_header_param_type(param, spec)} {param.name}" for param in call.params)
    if call.returns.kind != "void":
        parts.append(f"{_out_param_type(call.returns, spec)} out_result")
    params = ", ".join(parts) if parts else "void"
    return f"bool {call.c_name}({params});"


def _render_handle_destroy_decl(handle: HandleSpec) -> str:
    return f"void ifcopenshell_{_snake_name(handle.c_type)}_destroy({handle.c_type}* handle);"


def _render_handle_list_destroy_decl(handle: HandleSpec) -> str:
    list_c_type = _handle_list_c_type(handle)
    return f"void ifcopenshell_{_snake_name(list_c_type)}_destroy({list_c_type}* value);"


def _render_header(spec: AuthoredBindingSpec) -> str:
    guard = f"{spec.c_prefix.upper()}_API_H"
    handle_forwards = "\n".join(f"typedef struct {handle.c_type} {handle.c_type};" for handle in spec.handles.values())
    handle_list_types = _used_handle_list_handles(spec)
    handle_list_forwards = "\n".join(
        f"typedef struct {_handle_list_c_type(handle)} {{\n"
        f"    {handle.c_type}** items;\n"
        f"    size_t size;\n"
        f"}} {_handle_list_c_type(handle)};"
        for handle in handle_list_types
    )
    destroy_decls = "\n".join(_render_handle_destroy_decl(handle) for handle in spec.handles.values())
    handle_list_destroy_decls = "\n".join(_render_handle_list_destroy_decl(handle) for handle in handle_list_types)
    call_decls = "\n".join(_render_call_decl(call, spec) for call in (*spec.functions, *spec.methods))

    return f"""{AI_HEADER}
#ifndef {guard}
#define {guard}

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {{
#endif

typedef struct ifcopenshell_string_t {{
    char* data;
    size_t size;
    bool owned;
}} ifcopenshell_string_t;

typedef struct ifcopenshell_string_list_t {{
    ifcopenshell_string_t* items;
    size_t size;
}} ifcopenshell_string_list_t;

typedef struct ifcopenshell_bool_list_t {{
    bool* items;
    size_t size;
}} ifcopenshell_bool_list_t;

typedef struct ifcopenshell_int32_list_t {{
    int32_t* items;
    size_t size;
}} ifcopenshell_int32_list_t;

typedef struct ifcopenshell_uint32_list_t {{
    uint32_t* items;
    size_t size;
}} ifcopenshell_uint32_list_t;

typedef struct ifcopenshell_int32_list_list_t {{
    ifcopenshell_int32_list_t* items;
    size_t size;
}} ifcopenshell_int32_list_list_t;

typedef struct ifcopenshell_double_list_t {{
    double* items;
    size_t size;
}} ifcopenshell_double_list_t;

typedef struct ifcopenshell_double_list_list_t {{
    ifcopenshell_double_list_t* items;
    size_t size;
}} ifcopenshell_double_list_list_t;

{handle_forwards}

{handle_list_forwards}

void ifcopenshell_string_destroy(ifcopenshell_string_t* value);
void ifcopenshell_string_list_destroy(ifcopenshell_string_list_t* value);
void ifcopenshell_bool_list_destroy(ifcopenshell_bool_list_t* value);
void ifcopenshell_int32_list_destroy(ifcopenshell_int32_list_t* value);
void ifcopenshell_uint32_list_destroy(ifcopenshell_uint32_list_t* value);
void ifcopenshell_int32_list_list_destroy(ifcopenshell_int32_list_list_t* value);
void ifcopenshell_double_list_destroy(ifcopenshell_double_list_t* value);
void ifcopenshell_double_list_list_destroy(ifcopenshell_double_list_list_t* value);
void {spec.c_prefix}_clear_error(void);
const char* {spec.c_prefix}_last_error_message(void);

{destroy_decls}
{handle_list_destroy_decls}

{call_decls}

#ifdef __cplusplus
}}
#endif

#endif
"""


def _handle_storage_type(handle: HandleSpec) -> str:
    if handle.name == "attribute_value":
        return "AttributeValue"
    if handle.name == "instance_list":
        return "aggregate_of_instance::ptr"
    return f"{handle.cpp_type}*"


def _destroy_body(handle: HandleSpec) -> str:
    if handle.name in {"attribute_value", "instance_list"}:
        return "delete handle;"
    if handle.destructor == "delete":
        return "if (handle->owned && handle->ptr) { delete handle->ptr; }\n    delete handle;"
    return "delete handle;"


def _wrap_handle_expr(type_spec: TypeSpec, expr: str, spec: AuthoredBindingSpec) -> str:
    handle = spec.handles[type_spec.handle]
    owned = "true" if type_spec.ownership == "owned" else "false"
    if handle.name in {"attribute_value", "instance_list"}:
        return f"new {handle.c_type}{{{expr}}}"
    pointer_expr = expr
    if _normalize_cpp_type(type_spec.cpp_type).endswith("&"):
        pointer_expr = f"&({expr})"
    if _normalize_cpp_type(type_spec.cpp_type).startswith("const "):
        pointer_expr = f"const_cast<{handle.cpp_type}*>({pointer_expr})"
    return f"new {handle.c_type}{{{pointer_expr}, {owned}}}"


def _render_result_assignment(call: CallSpec, spec: AuthoredBindingSpec, expr: str) -> str:
    type_spec = call.returns
    if type_spec.kind == "void":
        return f"{expr};"
    if type_spec.kind == "bool":
        return f"*out_result = {expr};"
    if type_spec.kind == "bool_list":
        return f"*out_result = make_bool_list({expr});"
    if type_spec.kind == "int32":
        return f"*out_result = static_cast<int32_t>({expr});"
    if type_spec.kind == "double":
        return f"*out_result = static_cast<double>({expr});"
    if type_spec.kind == "uint32":
        return f"*out_result = static_cast<uint32_t>({expr});"
    if type_spec.kind == "size":
        return f"*out_result = static_cast<size_t>({expr});"
    if type_spec.kind == "string":
        helper = "make_static_string" if type_spec.ownership == "static" else "make_string"
        return f"*out_result = {helper}({expr});"
    if type_spec.kind == "string_list":
        return f"*out_result = make_string_list({expr});"
    if type_spec.kind == "int32_list":
        return f"*out_result = make_int32_list({expr});"
    if type_spec.kind == "uint32_list":
        return f"*out_result = make_uint32_list({expr});"
    if type_spec.kind == "int32_list_list":
        return f"*out_result = make_int32_list_list({expr});"
    if type_spec.kind == "double_list":
        return f"*out_result = make_double_list({expr});"
    if type_spec.kind == "double_list_list":
        return f"*out_result = make_double_list_list({expr});"
    if type_spec.kind == "handle":
        return f"*out_result = {_wrap_handle_expr(type_spec, expr, spec)};"
    if type_spec.kind == "handle_list":
        helper = _handle_list_helper_name(spec.handles[type_spec.handle])
        return f"*out_result = {helper}({expr});"
    msg = f"Unsupported return kind: {type_spec.kind}"
    raise ValueError(msg)


def _render_param_prelude(param: ParamSpec, spec: AuthoredBindingSpec) -> str:
    type_spec = param.type
    if type_spec.kind == "string":
        return (
            f'    if ({param.name} == nullptr) {{ throw std::runtime_error("Parameter \\"{param.name}\\" must not be null"); }}\n'
            f"    std::string {param.name}_cpp({param.name});"
        )
    if type_spec.kind == "string_list":
        return (
            f'    if ({param.name} == nullptr) {{ throw std::runtime_error("Parameter \\"{param.name}\\" must not be null"); }}\n'
            f"    auto {param.name}_cpp = to_cpp_string_list({param.name});"
        )
    if type_spec.kind == "bool_list":
        return (
            f'    if ({param.name} == nullptr) {{ throw std::runtime_error("Parameter \\"{param.name}\\" must not be null"); }}\n'
            f"    auto {param.name}_cpp = to_cpp_bool_list({param.name});"
        )
    if type_spec.kind == "int32_list":
        return (
            f'    if ({param.name} == nullptr) {{ throw std::runtime_error("Parameter \\"{param.name}\\" must not be null"); }}\n'
            f"    auto {param.name}_cpp = to_cpp_int32_list({param.name});"
        )
    if type_spec.kind == "uint32_list":
        return (
            f'    if ({param.name} == nullptr) {{ throw std::runtime_error("Parameter \\"{param.name}\\" must not be null"); }}\n'
            f"    auto {param.name}_cpp = to_cpp_uint32_list({param.name});"
        )
    if type_spec.kind == "int32_list_list":
        return (
            f'    if ({param.name} == nullptr) {{ throw std::runtime_error("Parameter \\"{param.name}\\" must not be null"); }}\n'
            f"    auto {param.name}_cpp = to_cpp_int32_list_list({param.name});"
        )
    if type_spec.kind == "double_list":
        return (
            f'    if ({param.name} == nullptr) {{ throw std::runtime_error("Parameter \\"{param.name}\\" must not be null"); }}\n'
            f"    auto {param.name}_cpp = to_cpp_double_list({param.name});"
        )
    if type_spec.kind == "double_list_list":
        return (
            f'    if ({param.name} == nullptr) {{ throw std::runtime_error("Parameter \\"{param.name}\\" must not be null"); }}\n'
            f"    auto {param.name}_cpp = to_cpp_double_list_list({param.name});"
        )
    if type_spec.kind == "handle":
        handle = spec.handles[type_spec.handle]
        if handle.name in {"attribute_value", "instance_list"}:
            return (
                f'    if ({param.name} == nullptr) {{ throw std::runtime_error("Handle parameter \\"{param.name}\\" is invalid"); }}\n'
                f"    auto {param.name}_cpp = {param.name}->value;"
            )
        cpp_type = _normalize_cpp_type(type_spec.cpp_type)
        value_expr = f"{param.name}->ptr"
        if cpp_type.endswith("&"):
            value_expr = f"*{value_expr}"
        return (
            f'    if ({param.name} == nullptr || {param.name}->ptr == nullptr) {{ throw std::runtime_error("Handle parameter \\"{param.name}\\" is invalid"); }}\n'
            f"    auto {param.name}_cpp = {value_expr};"
        )
    if type_spec.kind == "handle_list":
        handle = spec.handles[type_spec.handle]
        helper_name = f"to_cpp_{_snake_name(_handle_list_c_type(handle))}"
        return (
            f'    if ({param.name} == nullptr) {{ throw std::runtime_error("Parameter \\"{param.name}\\" must not be null"); }}\n'
            f"    auto {param.name}_cpp = {helper_name}({param.name});"
        )
    return ""


def _call_expr_args(call: CallSpec) -> str:
    args: list[str] = []
    for param in call.params:
        if param.type.kind == "string":
            args.append(f"{param.name}_cpp")
        elif param.type.kind in {"string_list", "bool_list", "int32_list", "uint32_list", "int32_list_list", "double_list", "double_list_list"}:
            args.append(f"{param.name}_cpp")
        elif param.type.kind in {"handle", "handle_list"}:
            args.append(f"{param.name}_cpp")
        else:
            args.append(param.name)
    return ", ".join(args)


def _render_call_impl(call: CallSpec, spec: AuthoredBindingSpec) -> str:
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

    if call.kind in {"function", "method"}:
        call_target = f"self_cpp->{call.cpp_name}" if call.receiver is not None else call.cpp_name
        expr = f"{call_target}({_call_expr_args(call)})"
        body_line = _render_result_assignment(call, spec, expr)
    else:
        body = call.implementation.body.rstrip()
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


def _render_cpp(spec: AuthoredBindingSpec, header_name: str) -> str:
    handle_structs = []
    for handle in spec.handles.values():
        storage_type = _handle_storage_type(handle)
        if handle.name in {"attribute_value", "instance_list"}:
            handle_structs.append(f"struct {handle.c_type} {{\n    {storage_type} value;\n}};")
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

    call_impls = "\n\n".join(_render_call_impl(call, spec) for call in (*spec.functions, *spec.methods))
    includes = "\n".join(f'#include "{header}"' for header in spec.public_headers)
    handle_structs_block = "\n\n".join(handle_structs)
    destroy_impls_block = "\n\n".join(destroy_impls)
    handle_list_types = _used_handle_list_handles(spec)
    handle_list_helpers = "\n\n".join(
        f"""static {_handle_list_c_type(handle)} {_handle_list_helper_name(handle)}(const std::vector<const {handle.cpp_type}*>& values) {{
    auto** items = values.empty() ? nullptr : new {handle.c_type}*[values.size()];
    for (size_t i = 0; i < values.size(); ++i) {{
        items[i] = new {handle.c_type}{{const_cast<{handle.cpp_type}*>(values[i]), false}};
    }}
    return {_handle_list_c_type(handle)}{{items, values.size()}};
}}

static std::vector<const {handle.cpp_type}*> to_cpp_{_snake_name(_handle_list_c_type(handle))}(const {_handle_list_c_type(handle)}* values) {{
    validate_list_items("{_snake_name(_handle_list_c_type(handle))}", values->items, values->size);
    std::vector<const {handle.cpp_type}*> result;
    result.reserve(values->size);
    for (size_t i = 0; i < values->size; ++i) {{
        auto* item = values->items[i];
        if (item == nullptr || item->ptr == nullptr) {{
            throw std::runtime_error("handle_list contains an invalid handle");
        }}
        result.push_back(item->ptr);
    }}
    return result;
}}"""
        for handle in handle_list_types
    )
    handle_list_destroy_impls = "\n\n".join(
        f"""void ifcopenshell_{_snake_name(_handle_list_c_type(handle))}_destroy({_handle_list_c_type(handle)}* value) {{
    if (value == nullptr || value->items == nullptr) {{
        return;
    }}
    for (size_t i = 0; i < value->size; ++i) {{
        ifcopenshell_{_snake_name(handle.c_type)}_destroy(value->items[i]);
    }}
    delete[] value->items;
    value->items = nullptr;
    value->size = 0;
}}"""
        for handle in handle_list_types
    )

    return f"""{AI_HEADER}
#include "{header_name}"

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

{includes}
#include "aggregate_of_instance.h"
#include "IfcEntityInstanceData.h"
#include "utils.h"

namespace {{
thread_local std::string g_last_error;
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

void set_last_error(const std::string& message) {{
    g_last_error = message;
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

ifcopenshell_string_list_t make_string_list(const std::vector<std::string>& values) {{
    auto* items = new ifcopenshell_string_t[values.size()];
    for (size_t i = 0; i < values.size(); ++i) {{
        items[i] = make_string(values[i]);
    }}
    return ifcopenshell_string_list_t{{items, values.size()}};
}}

std::vector<std::string> to_cpp_string_list(const ifcopenshell_string_list_t* value) {{
    validate_list_items("string_list", value->items, value->size);
    std::vector<std::string> result;
    result.reserve(value->size);
    for (size_t i = 0; i < value->size; ++i) {{
        const auto& item = value->items[i];
        if (item.data == nullptr && item.size > 0) {{
            throw std::runtime_error("string_list contains a null string buffer");
        }}
        result.emplace_back(item.data == nullptr ? "" : item.data, item.size);
    }}
    return result;
}}

ifcopenshell_bool_list_t make_bool_list(const std::vector<bool>& values) {{
    bool* items = values.empty() ? nullptr : new bool[values.size()];
    for (size_t i = 0; i < values.size(); ++i) {{
        items[i] = values[i];
    }}
    return ifcopenshell_bool_list_t{{items, values.size()}};
}}

std::vector<bool> to_cpp_bool_list(const ifcopenshell_bool_list_t* value) {{
    validate_list_items("bool_list", value->items, value->size);
    std::vector<bool> result;
    result.reserve(value->size);
    for (size_t i = 0; i < value->size; ++i) {{
        result.push_back(value->items[i]);
    }}
    return result;
}}

ifcopenshell_int32_list_t make_int32_list(const std::vector<int>& values) {{
    int32_t* items = values.empty() ? nullptr : new int32_t[values.size()];
    for (size_t i = 0; i < values.size(); ++i) {{
        items[i] = static_cast<int32_t>(values[i]);
    }}
    return ifcopenshell_int32_list_t{{items, values.size()}};
}}

std::vector<int> to_cpp_int32_list(const ifcopenshell_int32_list_t* value) {{
    validate_list_items("int32_list", value->items, value->size);
    if (value->size == 0) {{
        return {{}};
    }}
    return std::vector<int>(value->items, value->items + value->size);
}}

ifcopenshell_uint32_list_t make_uint32_list(const std::vector<unsigned int>& values) {{
    uint32_t* items = values.empty() ? nullptr : new uint32_t[values.size()];
    for (size_t i = 0; i < values.size(); ++i) {{
        items[i] = static_cast<uint32_t>(values[i]);
    }}
    return ifcopenshell_uint32_list_t{{items, values.size()}};
}}

std::vector<unsigned int> to_cpp_uint32_list(const ifcopenshell_uint32_list_t* value) {{
    validate_list_items("uint32_list", value->items, value->size);
    if (value->size == 0) {{
        return {{}};
    }}
    return std::vector<unsigned int>(value->items, value->items + value->size);
}}

ifcopenshell_int32_list_list_t make_int32_list_list(const std::vector<std::vector<int>>& values) {{
    auto* items = values.empty() ? nullptr : new ifcopenshell_int32_list_t[values.size()];
    for (size_t i = 0; i < values.size(); ++i) {{
        items[i] = make_int32_list(values[i]);
    }}
    return ifcopenshell_int32_list_list_t{{items, values.size()}};
}}

std::vector<std::vector<int>> to_cpp_int32_list_list(const ifcopenshell_int32_list_list_t* value) {{
    validate_list_items("int32_list_list", value->items, value->size);
    std::vector<std::vector<int>> result;
    result.reserve(value->size);
    for (size_t i = 0; i < value->size; ++i) {{
        result.push_back(to_cpp_int32_list(&value->items[i]));
    }}
    return result;
}}

ifcopenshell_double_list_t make_double_list(const std::vector<double>& values) {{
    double* items = values.empty() ? nullptr : new double[values.size()];
    for (size_t i = 0; i < values.size(); ++i) {{
        items[i] = values[i];
    }}
    return ifcopenshell_double_list_t{{items, values.size()}};
}}

std::vector<double> to_cpp_double_list(const ifcopenshell_double_list_t* value) {{
    validate_list_items("double_list", value->items, value->size);
    if (value->size == 0) {{
        return {{}};
    }}
    return std::vector<double>(value->items, value->items + value->size);
}}

ifcopenshell_double_list_list_t make_double_list_list(const std::vector<std::vector<double>>& values) {{
    auto* items = values.empty() ? nullptr : new ifcopenshell_double_list_t[values.size()];
    for (size_t i = 0; i < values.size(); ++i) {{
        items[i] = make_double_list(values[i]);
    }}
    return ifcopenshell_double_list_list_t{{items, values.size()}};
}}

std::vector<std::vector<double>> to_cpp_double_list_list(const ifcopenshell_double_list_list_t* value) {{
    validate_list_items("double_list_list", value->items, value->size);
    std::vector<std::vector<double>> result;
    result.reserve(value->size);
    for (size_t i = 0; i < value->size; ++i) {{
        result.push_back(to_cpp_double_list(&value->items[i]));
    }}
    return result;
}}

template <typename T>
void set_instance_argument(IfcUtil::IfcBaseClass* instance, size_t index, const T& value) {{
    instance->set_attribute_value(index, value);
    if (auto* entity = instance->as<IfcUtil::IfcBaseEntity>()) {{
        entity->populate_derived();
    }}
}}

void unset_instance_argument(IfcUtil::IfcBaseClass* instance, size_t index) {{
    instance->unset_attribute_value(index);
    if (auto* entity = instance->as<IfcUtil::IfcBaseEntity>()) {{
        entity->populate_derived();
    }}
}}

void set_instance_attribute_from_attribute_value(IfcUtil::IfcBaseClass* instance, size_t index, const AttributeValue& value) {{
    value.apply_visitor([&](const auto& inner) -> void {{
        using T = std::decay_t<decltype(inner)>;
        if constexpr (std::is_same_v<T, Derived>) {{
            throw std::runtime_error("Cannot assign a derived attribute sentinel.");
        }} else {{
            set_instance_argument(instance, index, inner);
        }}
    }});
}}
}} // namespace

{handle_structs_block}

{handle_list_helpers}

void ifcopenshell_string_destroy(ifcopenshell_string_t* value) {{
    if (value == nullptr) {{
        return;
    }}
    if (value->owned && value->data != nullptr) {{
        delete[] value->data;
    }}
    value->data = nullptr;
    value->size = 0;
    value->owned = false;
}}

void ifcopenshell_string_list_destroy(ifcopenshell_string_list_t* value) {{
    if (value == nullptr || value->items == nullptr) {{
        return;
    }}
    for (size_t i = 0; i < value->size; ++i) {{
        ifcopenshell_string_destroy(&value->items[i]);
    }}
    delete[] value->items;
    value->items = nullptr;
    value->size = 0;
}}

void ifcopenshell_bool_list_destroy(ifcopenshell_bool_list_t* value) {{
    if (value == nullptr || value->items == nullptr) {{
        return;
    }}
    delete[] value->items;
    value->items = nullptr;
    value->size = 0;
}}

void ifcopenshell_int32_list_destroy(ifcopenshell_int32_list_t* value) {{
    if (value == nullptr || value->items == nullptr) {{
        return;
    }}
    delete[] value->items;
    value->items = nullptr;
    value->size = 0;
}}

void ifcopenshell_uint32_list_destroy(ifcopenshell_uint32_list_t* value) {{
    if (value == nullptr || value->items == nullptr) {{
        return;
    }}
    delete[] value->items;
    value->items = nullptr;
    value->size = 0;
}}

void ifcopenshell_int32_list_list_destroy(ifcopenshell_int32_list_list_t* value) {{
    if (value == nullptr || value->items == nullptr) {{
        return;
    }}
    for (size_t i = 0; i < value->size; ++i) {{
        ifcopenshell_int32_list_destroy(&value->items[i]);
    }}
    delete[] value->items;
    value->items = nullptr;
    value->size = 0;
}}

void ifcopenshell_double_list_destroy(ifcopenshell_double_list_t* value) {{
    if (value == nullptr || value->items == nullptr) {{
        return;
    }}
    delete[] value->items;
    value->items = nullptr;
    value->size = 0;
}}

void ifcopenshell_double_list_list_destroy(ifcopenshell_double_list_list_t* value) {{
    if (value == nullptr || value->items == nullptr) {{
        return;
    }}
    for (size_t i = 0; i < value->size; ++i) {{
        ifcopenshell_double_list_destroy(&value->items[i]);
    }}
    delete[] value->items;
    value->items = nullptr;
    value->size = 0;
}}

void {spec.c_prefix}_clear_error(void) {{
    g_last_error.clear();
}}

const char* {spec.c_prefix}_last_error_message(void) {{
    return g_last_error.c_str();
}}

{destroy_impls_block}
{handle_list_destroy_impls}

{call_impls}
"""


def generate(spec_path: Path, header_out: Path, cpp_out: Path, compile_commands_path: Path | None = None) -> None:
    spec = load_authored_spec(spec_path, compile_commands_path=compile_commands_path)
    header_out.parent.mkdir(parents=True, exist_ok=True)
    cpp_out.parent.mkdir(parents=True, exist_ok=True)
    header_out.write_text(_render_header(spec), encoding="utf-8")
    cpp_out.write_text(_render_cpp(spec, header_out.name), encoding="utf-8")


def _build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(description="Generate the first C backend skeleton from a handwritten binding spec.")
    parser.add_argument("--spec", type=Path, required=True, help="Path to the handwritten binding spec YAML.")
    parser.add_argument("--header-out", type=Path, required=True, help="Output path for the generated C header.")
    parser.add_argument("--cpp-out", type=Path, required=True, help="Output path for the generated C++ glue source.")
    parser.add_argument(
        "--compile-commands",
        type=Path,
        default=None,
        help="Optional path to compile_commands.json for AST-backed source discovery.",
    )
    return parser


def main() -> int:
    args = _build_parser().parse_args()
    generate(args.spec, args.header_out, args.cpp_out, compile_commands_path=args.compile_commands)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
