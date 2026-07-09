# This file was generated with the assistance of an AI coding tool.

from __future__ import annotations

try:
    from .authored_spec import TypeSpec
    from .binding_ir import BindingIR
    from .c_sequence_helpers import (
        _handle_list_c_type,
        _handle_list_list_c_type,
        _sequence_destroy_name,
        _snake_name,
        _type_spec_sequence_kind,
    )
    from .c_type_rendering import _variant_c_type
except ImportError:  # pragma: no cover - script execution fallback
    from authored_spec import TypeSpec
    from binding_ir import BindingIR
    from c_sequence_helpers import (
        _handle_list_c_type,
        _handle_list_list_c_type,
        _sequence_destroy_name,
        _snake_name,
        _type_spec_sequence_kind,
    )
    from c_type_rendering import _variant_c_type


def _variant_destroy_name(type_spec: TypeSpec, spec: BindingIR) -> str:
    return f"ifcopenshell_{_snake_name(_variant_c_type(type_spec, spec))}_destroy"


def _variant_return_types(spec: BindingIR) -> tuple[TypeSpec, ...]:
    seen: set[str] = set()
    variants: list[TypeSpec] = []
    for call in (*spec.functions, *spec.methods):
        if call.returns.kind != "variant":
            continue
        c_type = _variant_c_type(call.returns, spec)
        if c_type in seen:
            continue
        seen.add(c_type)
        variants.append(call.returns)
    return tuple(variants)


def _variant_alternative_destroy(alt: TypeSpec, field: str, spec: BindingIR) -> str | None:
    sequence_kind = _type_spec_sequence_kind(alt)
    if sequence_kind is not None:
        return f"{_sequence_destroy_name(sequence_kind)}(&value->{field});"
    if alt.kind == "string":
        return f"ifcopenshell_string_destroy(&value->{field});"
    if alt.kind != "handle" or alt.handle is None:
        return None
    handle = spec.handles[alt.handle]
    if alt.sequence_depth == 1:
        destroy = f"ifcopenshell_{_snake_name(_handle_list_c_type(handle))}_destroy"
        return f"{destroy}(&value->{field});"
    if alt.sequence_depth == 2:
        destroy = f"ifcopenshell_{_snake_name(_handle_list_list_c_type(handle))}_destroy"
        return f"{destroy}(&value->{field});"
    destroy = f"ifcopenshell_{_snake_name(handle.c_type)}_destroy"
    return f"{destroy}(value->{field});\n        value->{field} = nullptr;"


def _render_variant_destroy_decls(spec: BindingIR) -> str:
    return "\n".join(
        f"void {_variant_destroy_name(type_spec, spec)}({_variant_c_type(type_spec, spec)}* value);"
        for type_spec in _variant_return_types(spec)
    )


def _render_variant_destroy_impls(spec: BindingIR) -> str:
    impls: list[str] = []
    for type_spec in _variant_return_types(spec):
        cases: list[str] = []
        for index, alt in enumerate(type_spec.variants):
            destroy = _variant_alternative_destroy(alt, f"value_{index}", spec)
            body = f"        {destroy}\n" if destroy is not None else ""
            cases.append(f"    case {index}:\n{body}        break;")
        cases.append("    default:\n        break;")
        impls.append(
            f"""void {_variant_destroy_name(type_spec, spec)}({_variant_c_type(type_spec, spec)}* value) {{
    if (value == nullptr) {{
        return;
    }}
    switch (value->kind) {{
{chr(10).join(cases)}
    }}
    value->kind = -1;
}}"""
        )
    return "\n\n".join(impls)


__all__ = ["_render_variant_destroy_decls", "_render_variant_destroy_impls", "_variant_destroy_name"]
