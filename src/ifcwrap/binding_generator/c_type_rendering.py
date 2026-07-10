# This file was generated with the assistance of an AI coding tool.

from __future__ import annotations

import re

try:
    from .authored_spec import ParamSpec, TypeSpec
    from .binding_ir import BindingIR, CallIR
    from .binding_model import OptionStructSpec
    from .c_sequence_helpers import (
        _handle_list_c_type,
        _handle_list_list_c_type,
        _sequence_c_type,
        _sequence_out_type,
        _sequence_param_type,
        _type_spec_sequence_kind,
    )
except ImportError:  # pragma: no cover - script execution fallback
    from authored_spec import ParamSpec, TypeSpec
    from binding_ir import BindingIR, CallIR
    from binding_model import OptionStructSpec
    from c_sequence_helpers import (
        _handle_list_c_type,
        _handle_list_list_c_type,
        _sequence_c_type,
        _sequence_out_type,
        _sequence_param_type,
        _type_spec_sequence_kind,
    )


# Mapping from type kind to (param_type, out_type, result_template).
# Entries that are not scalar require handle/string/list-specific logic.
_SCALAR_TYPE_MAP: dict[str, tuple[str, str, str]] = {
    "bool": ("bool", "bool*", "*out_result = {expr};"),
    "int32": ("int32_t", "int32_t*", "*out_result = static_cast<int32_t>({expr});"),
    "int64": ("int64_t", "int64_t*", "*out_result = static_cast<int64_t>({expr});"),
    "double": ("double", "double*", "*out_result = static_cast<double>({expr});"),
    "uint32": ("uint32_t", "uint32_t*", "*out_result = static_cast<uint32_t>({expr});"),
    "size": ("size_t", "size_t*", "*out_result = static_cast<size_t>({expr});"),
}

_BUFFER_TYPE_MAP: dict[str, tuple[str, str]] = {
    "double_buffer": ("const double*", "const double**"),
    "int32_buffer": ("const int32_t*", "const int32_t**"),
}


def _normalize_cpp_type(cpp_type: str | None) -> str:
    if not cpp_type:
        return ""
    cpp_type = re.sub(r"/\*.*?\*/", "", cpp_type)
    return " ".join(
        cpp_type.replace(" &", "&").replace(" *", "*").replace("< ", "<").replace(" >", ">").split()
    )


def _qualify_handle_cpp_fragment(cpp_fragment: str, handle_cpp_type: str) -> str:
    qualified = _normalize_cpp_type(handle_cpp_type)
    simple = qualified.rsplit("::", 1)[-1]
    if not simple or "::" in cpp_fragment:
        return cpp_fragment
    return re.sub(rf"\b{re.escape(simple)}\b", qualified, cpp_fragment)


def _variant_alt_name(type_spec: TypeSpec, spec: BindingIR) -> str:
    sequence_kind = _type_spec_sequence_kind(type_spec)
    if sequence_kind is not None:
        return sequence_kind
    if type_spec.kind == "handle" and type_spec.handle is not None:
        return type_spec.handle
    if type_spec.kind == "struct" and type_spec.struct is not None:
        return type_spec.struct
    return type_spec.kind


def _variant_c_type(type_spec: TypeSpec, spec: BindingIR) -> str:
    parts = "_".join(_variant_alt_name(alt, spec) for alt in type_spec.variants)
    return f"{spec.c_prefix}_{parts}_variant_t"


def _optional_struct_c_type(type_spec: TypeSpec, spec: BindingIR) -> str:
    if type_spec.struct is None:
        raise ValueError("nullable struct return is missing struct name")
    base = spec.result_structs[type_spec.struct].c_type.removeprefix(f"{spec.c_prefix}_")
    return f"{spec.c_prefix}_optional_{base}"


def _cpp_param_type(param: ParamSpec, spec: BindingIR) -> str:
    type_spec = param.type
    kind = type_spec.kind
    sequence_kind = _type_spec_sequence_kind(type_spec)
    if sequence_kind is not None:
        return _sequence_param_type(sequence_kind)
    if kind in _SCALAR_TYPE_MAP:
        return _SCALAR_TYPE_MAP[kind][0]
    if kind == "string":
        return "const char*"
    if kind in _BUFFER_TYPE_MAP:
        return _BUFFER_TYPE_MAP[kind][0]
    if kind == "handle":
        if param.type.sequence_depth == 1:
            return f"const {_handle_list_c_type(spec.handles[param.type.handle])}*"
        if param.type.sequence_depth == 2:
            return f"const {_handle_list_list_c_type(spec.handles[param.type.handle])}*"
        return f"{spec.handles[param.type.handle].c_type}*"
    if kind == "opaque_ptr":
        return "void*"
    if kind == "struct":
        if type_spec.struct is None:
            raise ValueError("struct parameter is missing struct name")
        return spec.result_structs[type_spec.struct].c_type
    if kind == "variant":
        return _variant_c_type(type_spec, spec)
    if kind == "option":
        if type_spec.struct is None:
            raise ValueError("option parameter is missing option struct name")
        return f"const {spec.option_structs[type_spec.struct].c_type}*"
    msg = f"Unsupported parameter kind: {kind}"
    raise ValueError(msg)


def _out_param_type(type_spec: TypeSpec, spec: BindingIR) -> str:
    kind = type_spec.kind
    sequence_kind = _type_spec_sequence_kind(type_spec)
    if kind == "void":
        msg = "void has no out parameter"
        raise ValueError(msg)
    if sequence_kind is not None:
        return _sequence_out_type(sequence_kind)
    if kind in _SCALAR_TYPE_MAP:
        return _SCALAR_TYPE_MAP[kind][1]
    if kind == "string":
        return "ifcopenshell_string_t*"
    if kind in _BUFFER_TYPE_MAP:
        return _BUFFER_TYPE_MAP[kind][1]
    if kind == "handle":
        if type_spec.sequence_depth == 1:
            return f"{_handle_list_c_type(spec.handles[type_spec.handle])}*"
        if type_spec.sequence_depth == 2:
            return f"{_handle_list_list_c_type(spec.handles[type_spec.handle])}*"
        return f"{spec.handles[type_spec.handle].c_type}**"
    if kind == "opaque_ptr":
        return "void**"
    if kind == "struct":
        if type_spec.struct is None:
            raise ValueError("struct return is missing struct name")
        if type_spec.nullable:
            return f"{_optional_struct_c_type(type_spec, spec)}*"
        return f"{spec.result_structs[type_spec.struct].c_type}*"
    if kind == "variant":
        return f"{_variant_c_type(type_spec, spec)}*"
    msg = f"Unsupported return kind: {kind}"
    raise ValueError(msg)


def _result_struct_field_c_type(type_spec: TypeSpec, spec: BindingIR) -> str:
    sequence_kind = _type_spec_sequence_kind(type_spec)
    if sequence_kind is not None:
        return _sequence_c_type(sequence_kind)
    if type_spec.kind in _SCALAR_TYPE_MAP:
        return _SCALAR_TYPE_MAP[type_spec.kind][0]
    if type_spec.kind == "string":
        return "ifcopenshell_string_t"
    if type_spec.kind == "handle":
        if type_spec.sequence_depth == 1:
            return _handle_list_c_type(spec.handles[type_spec.handle])
        if type_spec.sequence_depth == 2:
            return _handle_list_list_c_type(spec.handles[type_spec.handle])
        return f"{spec.handles[type_spec.handle].c_type}*"
    if type_spec.kind == "opaque_ptr":
        return "void*"
    if type_spec.kind == "variant":
        return _variant_c_type(type_spec, spec)
    raise ValueError(f"Unsupported result struct field kind: {type_spec.kind}")


def _render_result_struct_decl(struct: object, spec: BindingIR) -> str:
    fields = "\n".join(f"    {_result_struct_field_c_type(field.type, spec)} {field.name};" for field in struct.fields)
    return f"typedef struct {struct.c_type} {{\n{fields}\n}} {struct.c_type};"


def _render_optional_result_struct_decl(type_spec: TypeSpec, spec: BindingIR) -> str:
    c_type = _optional_struct_c_type(type_spec, spec)
    value_type = spec.result_structs[type_spec.struct].c_type
    return f"typedef struct {c_type} {{\n    bool has_value;\n    {value_type} value;\n}} {c_type};"


def _render_variant_decl(type_spec: TypeSpec, spec: BindingIR) -> str:
    fields = ["    int32_t kind;"]
    fields.extend(
        f"    {_result_struct_field_c_type(alt, spec)} value_{index};"
        for index, alt in enumerate(type_spec.variants)
    )
    c_type = _variant_c_type(type_spec, spec)
    return f"typedef struct {c_type} {{\n" + "\n".join(fields) + f"\n}} {c_type};"


def _render_option_struct_decl(struct: OptionStructSpec, spec: BindingIR) -> str:
    lines: list[str] = []
    for field in struct.fields:
        field_type = _cpp_param_type(ParamSpec(field.name, field.type), spec)
        lines.append(f"    {field_type} {field.name};")
        if field.type.nullable:
            lines.append(f"    bool has_{field.name};")
    fields = "\n".join(lines)
    return f"typedef struct {struct.c_type} {{\n{fields}\n}} {struct.c_type};"


def _render_call_decl(call: CallIR, spec: BindingIR) -> str:
    parts = []
    if call.receiver is not None:
        parts.append(f"{spec.handles[call.receiver].c_type}* self")
    try:
        parts.extend(f"{_cpp_param_type(param, spec)} {param.name}" for param in call.params)
    except ValueError as exc:
        msg = f"{exc} in call {call.c_name}"
        raise ValueError(msg) from exc
    if call.returns.kind != "void":
        parts.append(f"{_out_param_type(call.returns, spec)} out_result")
    params = ", ".join(parts) if parts else "void"
    declaration = f"bool {call.c_name}({params});"
    if not call.doc:
        return declaration
    return _render_doc_comment(call.doc) + "\n" + declaration


def _render_doc_comment(doc: str) -> str:
    escaped = doc.replace("*/", "* /").strip()
    lines = escaped.splitlines()
    if len(lines) == 1:
        return f"/** {lines[0]} */"
    body = "\n".join(f" * {line}" if line else " *" for line in lines)
    return f"/**\n{body}\n */"
