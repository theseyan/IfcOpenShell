# This file was generated with the assistance of an AI coding tool.

from __future__ import annotations

import json
import re

from ...abi_ir import BindingABI, CFunctionIR, COptionIR, CParamIR, CTypeIR
from ...binding_model import TypeSpec
from .._shared import (
    _INTERNAL_C_FUNCTIONS,
    _camel_name,
    _module_public_name,
    _module_type_name,
    _public_module_members,
    _public_params,
    _sequence_leaf_type,
)
from .typescript import _render_doc_comment, _render_semantic_aliases, _ts_type

_HANDLE_TS = {
    "file": "IfcFile",
    "instance": "Entity",
    "attribute": "AttributeValue",
    "geom_settings": "GeomSettings",
    "serializer_settings": "SerializerSettings",
    "iterator": "GeomIterator",
    "tree": "GeometryTree",
    "parse_instance_list": "Entity[]",
    "instance_list": "Entity[]",
    "value": "Value",
    "pset_template_handle": "PsetTemplate",
    "project_append_asset_cache": "IfcOpenshellProjectAppendAssetCache",
}

_HANDLE_KIND_NAMES = {
    "file": "file",
    "instance": "entity",
    "attribute": "attribute",
    "parse_instance_list": "instance_list",
    "instance_list": "instance_list",
    "value": "value",
    "pset_template_handle": "pset_template",
    "project_append_asset_cache": "project_append_asset_cache",
}

_INTERNAL_MODULES = {"value"}

_GENERATED_HANDLE_TS = {
    "pset_template_handle": "PsetTemplate",
    "project_append_asset_cache": "IfcOpenshellProjectAppendAssetCache",
}

_RAW_PARAM_SCALAR_TS = {
    "bool": "boolean",
    "double": "number",
    "int32_t": "number",
    "int64_t": "bigint",
    "size_t": "number",
    "uint32_t": "number",
    "uint8_t": "number",
    "const char*": "string",
    "ifcopenshell_string_t": "string",
}

_RAW_RETURN_SCALAR_TS = {
    "bool": "boolean",
    "double": "number",
    "int32": "number",
    "int64": "bigint",
    "size": "number",
    "string": "string",
    "uint32": "number",
    "uint8": "number",
}


def _is_public_api_module(module_name: str) -> bool:
    return module_name not in _INTERNAL_MODULES


_HIDDEN_FUNCTIONS = {
    "ifcopenshell_pset_props_free",
    "ifcopenshell_pset_props_new",
    "ifcopenshell_pset_props_set_bool",
    "ifcopenshell_pset_props_set_date",
    "ifcopenshell_pset_props_set_datetime",
    "ifcopenshell_pset_props_set_dict",
    "ifcopenshell_pset_props_set_double",
    "ifcopenshell_pset_props_set_double_list",
    "ifcopenshell_pset_props_set_duration",
    "ifcopenshell_pset_props_set_instance",
    "ifcopenshell_pset_props_set_instance_list",
    "ifcopenshell_pset_props_set_int",
    "ifcopenshell_pset_props_set_int_list",
    "ifcopenshell_pset_props_set_null",
    "ifcopenshell_pset_props_set_string",
    "ifcopenshell_pset_props_set_string_list",
    "ifcopenshell_pset_props_set_typed_bool",
    "ifcopenshell_pset_props_set_typed_double",
    "ifcopenshell_pset_props_set_typed_int",
    "ifcopenshell_pset_props_set_typed_string",
    "ifcopenshell_pset_props_set_unit_for_last",
    "ifcopenshell_pset_template_free",
    "ifcopenshell_project_append_asset_cache_free",
    "ifcopenshell_selector_keys_count",
    "ifcopenshell_selector_keys_free",
    "ifcopenshell_selector_keys_get",
    "ifcopenshell_selector_keys_is_regex",
    "ifcopenshell_selector_node_child",
    "ifcopenshell_selector_node_child_count",
    "ifcopenshell_selector_node_free",
    "ifcopenshell_selector_node_kind",
    "ifcopenshell_selector_node_text",
    "ifcopenshell_selector_parse_filter",
    "ifcopenshell_selector_parse_format",
    "ifcopenshell_selector_parse_get_element",
    "ifcopenshell_selector_parse_keys",
}


def _api_functions(
    metadata: BindingABI,
) -> dict[str, list[tuple[str, CFunctionIR]]]:
    modules: dict[str, list[tuple[str, CFunctionIR]]] = {}
    for function in sorted(metadata.functions.values(), key=lambda item: item.c_name):
        if (
            function.receiver is not None
            or function.c_name in _INTERNAL_C_FUNCTIONS
            or function.c_name in _HIDDEN_FUNCTIONS
        ):
            continue
        members = _public_module_members(function, metadata.c_prefix)
        if not members:
            continue
        for module_name, function_name in members:
            if module_name in {"parse", "geom"}:
                continue
            if not _is_public_api_module(module_name):
                continue
            modules.setdefault(module_name, []).append((function_name, function))
    return modules


def _handle_name(handle: str) -> str:
    return _HANDLE_TS.get(handle, "ApiData")


def _handle_kind_from_c_type(c_type: str, metadata: BindingABI) -> str | None:
    normalized = " ".join(c_type.replace(" *", "*").split())
    for name, handle in metadata.handles.items():
        if normalized in {f"{handle.c_type}*", f"const {handle.c_type}*"}:
            return name
    return None


def _handle_kind_from_type(type_spec: TypeSpec) -> str | None:
    if type_spec.kind != "handle" or type_spec.handle is None:
        return None
    return type_spec.handle


def _uses_generated_handle(handle: str | None) -> bool:
    return handle in _GENERATED_HANDLE_TS


def _is_instance_list_handle(handle: str | None) -> bool:
    return handle in {"parse_instance_list", "instance_list"}


def _option_by_c_type(c_type: str, metadata: BindingABI) -> COptionIR | None:
    normalized = (
        " ".join(c_type.replace(" *", "*").split())
        .removeprefix("const ")
        .removesuffix("*")
        .strip()
    )
    return next(
        (
            option
            for option in metadata.option_structs.values()
            if option.c_type == normalized
        ),
        None,
    )


def _option_type_name(option: COptionIR) -> str:
    base = option.c_type.removeprefix("ifcopenshell_").removesuffix("_t")
    return "IfcOpenShell" + "".join(
        part.capitalize() for part in base.split("_") if part
    )


def _struct_type_name(struct: CTypeIR) -> str:
    base = struct.c_type.removeprefix("ifcopenshell_").removesuffix("_t")
    return "IfcOpenShell" + "".join(
        part.capitalize() for part in base.split("_") if part
    )


def _member_name(name: str) -> str:
    if re.fullmatch(r"[A-Za-z_$][A-Za-z0-9_$]*", name) and name not in {"new"}:
        return name
    return json.dumps(name)


def _property_access(receiver: str, name: str) -> str:
    member = _member_name(name)
    return f"{receiver}.{member}" if member == name else f"{receiver}[{member}]"


def _direct_ts_type(type_spec: TypeSpec, metadata: BindingABI) -> str:
    if type_spec.sequence_depth > 0:
        inner_spec = _sequence_leaf_type(type_spec)
        inner = _direct_ts_type(inner_spec, metadata).removesuffix(" | null")
        if " | " in inner:
            inner = f"({inner})"
        dimensions = list(type_spec.fixed_lengths or (None,) * type_spec.sequence_depth)
        if type_spec.alias and dimensions and dimensions[-1] is not None:
            inner = type_spec.alias
            dimensions.pop()
        for length in reversed(dimensions):
            inner = (
                "[" + ", ".join(inner for _ in range(length)) + "]"
                if length is not None
                else f"{inner}[]"
            )
        return f"{inner} | null" if type_spec.nullable else inner

    handle = _handle_kind_from_type(type_spec)
    if handle is not None:
        result = _handle_name(handle)
    elif type_spec.kind == "struct" and type_spec.struct is not None:
        struct = metadata.value_types.get(type_spec.struct)
        result = _struct_type_name(struct) if struct is not None else "ApiData"
    elif type_spec.kind == "option" and type_spec.struct is not None:
        option = metadata.option_structs.get(type_spec.struct)
        result = _option_type_name(option) if option is not None else "ApiData"
    elif type_spec.kind == "variant":
        result = " | ".join(
            _direct_ts_type(alt, metadata).removesuffix(" | null")
            for alt in type_spec.variants
        )
    else:
        result = _ts_type(type_spec, metadata).removesuffix(" | null")
        if result == "unknown":
            result = "ApiData"
    return f"{result} | null" if type_spec.nullable else result


def _sequence_ts_type(struct: CTypeIR, metadata: BindingABI) -> str:
    if struct.kind == "input_record_sequence":
        option = _option_by_c_type(struct.element_type or "", metadata)
        return f"{_option_type_name(option)}[]" if option is not None else "ApiData[]"
    if struct.kind == "handle_sequence":
        elem = (
            (struct.element_type or "").removeprefix("const ").removesuffix("*").strip()
        )
        handle = next(
            (name for name, item in metadata.handles.items() if item.c_type == elem),
            None,
        )
        item_type = _handle_name(handle) if handle is not None else "ApiData"
        for _ in range(struct.sequence_depth):
            item_type = f"{item_type}[]"
        return item_type

    if struct.kind == "result_record_sequence":
        item = next(
            (
                value
                for value in metadata.value_types.values()
                if value.c_type == struct.element_type and value.kind == "result_struct"
            ),
            None,
        )
        return f"{_struct_type_name(item)}[]" if item is not None else "ApiData[]"

    elem = (struct.element_type or "").removeprefix("const ").removesuffix("*").strip()
    scalar = {
        "bool": "boolean",
        "int32_t": "number",
        "uint32_t": "number",
        "uint8_t": "number",
        "double": "number",
        "int64_t": "bigint",
        "ifcopenshell_string_t": "string",
    }.get(elem)
    if scalar is not None:
        return f"{scalar}[]"

    nested = next(
        (item for item in metadata.value_types.values() if item.c_type == elem), None
    )
    if nested is not None and nested.kind in {
        "sequence",
        "handle_sequence",
        "result_record_sequence",
    }:
        return f"{_sequence_ts_type(nested, metadata)}[]"
    return "ApiData[]"


def _direct_ts_type_from_c_type(c_type: str, metadata: BindingABI) -> str:
    normalized = " ".join(c_type.replace(" *", "*").split())
    normalized_base = normalized.removeprefix("const ").removesuffix("*").strip()
    handle = _handle_kind_from_c_type(normalized, metadata)
    if handle is not None:
        return _handle_name(handle)
    option = _option_by_c_type(normalized, metadata)
    if option is not None:
        return _option_type_name(option)
    value_type = next(
        (item for item in metadata.value_types.values() if item.c_type == normalized),
        None,
    )
    if value_type is not None and value_type.kind == "result_struct":
        return _struct_type_name(value_type)
    if normalized == "ifcopenshell_string_t":
        return "string"
    if normalized in {"bool", "ifcopenshell_logical_t"}:
        return "boolean"
    if normalized in {"int32_t", "uint32_t", "uint8_t", "double", "size_t", "void*"}:
        return "number"
    if normalized == "int64_t":
        return "bigint"
    sequence = next(
        (
            item
            for item in metadata.value_types.values()
            if item.c_type == normalized_base
            and item.kind
            in {
                "sequence",
                "handle_sequence",
                "input_record_sequence",
                "result_record_sequence",
            }
        ),
        None,
    )
    if sequence is not None:
        return _sequence_ts_type(sequence, metadata)
    return "ApiData"


def _option_field_type(
    field_name: str, type_spec: TypeSpec, metadata: BindingABI
) -> str:
    if type_spec.semantic == "property_map":
        return "PsetProperties | PsetInput"
    return _direct_ts_type(type_spec, metadata).removesuffix(" | null")


def _pset_props_field_names(option: COptionIR) -> set[str]:
    return {
        _camel_name(field.name)
        for field in option.fields
        if field.type.semantic == "property_map"
    }


def _entity_list_field_names(option: COptionIR, metadata: BindingABI) -> set[str]:
    return {
        _camel_name(field.name)
        for field in option.fields
        if _is_instance_list_handle(_handle_kind_from_type(field.type))
    }


def _fixed_field_lengths(option: COptionIR) -> dict[str, tuple[int | None, ...]]:
    return {
        _camel_name(field.name): field.type.fixed_lengths
        for field in option.fields
        if field.type.fixed_lengths
        and any(length is not None for length in field.type.fixed_lengths)
    }


def _variant_field_descriptors(
    option: COptionIR, metadata: BindingABI
) -> dict[str, dict[str, object]]:
    result: dict[str, dict[str, object]] = {}
    for field in option.fields:
        if field.type.kind != "variant":
            continue
        alternatives: list[dict[str, object]] = []
        for index, alternative in enumerate(field.type.variants):
            if alternative.sequence_depth > 0:
                alternatives.append(
                    {
                        "kind": index,
                        "mode": "sequence",
                        "fixedLengths": alternative.fixed_lengths,
                    }
                )
                continue
            if alternative.kind != "option" or alternative.struct is None:
                raise ValueError(
                    "Direct API input variants require semantic record or sequence alternatives"
                )
            nested = metadata.option_structs[alternative.struct]
            alternatives.append(
                {
                    "kind": index,
                    "mode": "record",
                    "sequenceDepth": 0,
                    "fields": {
                        _camel_name(item.name): item.name for item in nested.fields
                    },
                    "required": sorted(
                        _camel_name(item.name)
                        for item in nested.fields
                        if not item.type.nullable
                    ),
                    "pset": sorted(_pset_props_field_names(nested)),
                    "entities": sorted(_entity_list_field_names(nested, metadata)),
                    "fixed": _fixed_field_lengths(nested),
                    "variants": _variant_field_descriptors(nested, metadata),
                    "records": _record_field_descriptors(nested, metadata),
                }
            )
        result[_camel_name(field.name)] = {
            "sequenceDepth": field.type.sequence_depth,
            "alternatives": alternatives,
        }
    return result


def _record_field_descriptors(
    option: COptionIR, metadata: BindingABI
) -> dict[str, dict[str, object]]:
    result: dict[str, dict[str, object]] = {}
    for field in option.fields:
        if field.type.kind != "option" or field.type.struct is None:
            continue
        nested = metadata.option_structs[field.type.struct]
        result[_camel_name(field.name)] = {
            "sequenceDepth": field.type.sequence_depth,
            "fields": {_camel_name(item.name): item.name for item in nested.fields},
            "pset": sorted(_pset_props_field_names(nested)),
            "entities": sorted(_entity_list_field_names(nested, metadata)),
            "fixed": _fixed_field_lengths(nested),
            "variants": _variant_field_descriptors(nested, metadata),
            "records": _record_field_descriptors(nested, metadata),
        }
    return result


def _option_codec_args(option: COptionIR, metadata: BindingABI) -> str:
    pset_fields = sorted(_pset_props_field_names(option))
    entity_list_fields = sorted(_entity_list_field_names(option, metadata))
    fixed_fields = _fixed_field_lengths(option)
    variant_fields = _variant_field_descriptors(option, metadata)
    record_fields = _record_field_descriptors(option, metadata)
    if not any(
        (
            pset_fields,
            entity_list_fields,
            fixed_fields,
            variant_fields,
            record_fields,
        )
    ):
        return ""
    if not any((fixed_fields, variant_fields, record_fields)):
        extra_args = ", " + json.dumps(pset_fields)
        if entity_list_fields:
            extra_args += ", " + json.dumps(entity_list_fields)
        return extra_args
    return ", " + ", ".join(
        (
            json.dumps(pset_fields),
            json.dumps(entity_list_fields),
            json.dumps(fixed_fields, sort_keys=True),
            json.dumps(variant_fields, sort_keys=True),
            json.dumps(record_fields, sort_keys=True),
        )
    )


def _is_property_map_param(param: CParamIR) -> bool:
    return param.semantic == "property_map"


def _param_ts_type(
    param: CParamIR,
    metadata: BindingABI,
    module_name: str | None = None,
) -> str:
    if _is_property_map_param(param):
        base = "PsetProperties | PsetInput"
        return f"{base} | null" if param.nullable else base
    if param.type is not None and (
        param.type.sequence_depth > 0 or param.type.kind == "variant"
    ):
        return _direct_ts_type(param.type, metadata)
    normalized_base = (
        " ".join(param.c_type.replace(" *", "*").split())
        .removeprefix("const ")
        .removesuffix("*")
        .strip()
    )
    sequence = next(
        (
            item
            for item in metadata.value_types.values()
            if item.c_type == normalized_base
            and item.kind
            in {
                "sequence",
                "handle_sequence",
                "input_record_sequence",
                "result_record_sequence",
            }
        ),
        None,
    )
    if sequence is not None:
        return _sequence_ts_type(sequence, metadata)
    handle = _handle_kind_from_c_type(param.c_type, metadata)
    if handle is not None:
        if handle == "value" and module_name != "value":
            result = "ValueInput"
        else:
            result = _handle_name(handle)
        return f"{result} | null" if param.nullable else result
    option = _option_by_c_type(param.c_type, metadata)
    if option is not None:
        result = _option_type_name(option)
        return f"{result} | null" if param.nullable else result
    if param.type_kind == "string":
        result = "string"
        return f"{result} | null" if param.nullable else result
    if param.type_kind == "bool":
        result = "boolean"
        return f"{result} | null" if param.nullable else result
    if param.type_kind in {"int32", "uint32", "size", "double"}:
        result = "number"
        return f"{result} | null" if param.nullable else result
    if param.type_kind == "int64":
        result = "bigint"
        return f"{result} | null" if param.nullable else result
    return "ApiData"


def _raw_param_type(param: CParamIR) -> str:
    normalized = " ".join(param.c_type.replace(" *", "*").split())
    if param.nullable and normalized in {
        "const bool*",
        "const double*",
        "const int32_t*",
        "const int64_t*",
        "const size_t*",
        "const uint32_t*",
    }:
        scalar = {
            "bool": "boolean",
            "double": "number",
            "int32": "number",
            "int64": "bigint",
            "size": "number",
            "uint32": "number",
        }[param.type_kind]
        return f"{scalar} | null"
    result = _RAW_PARAM_SCALAR_TS.get(normalized)
    if result is None:
        return "RawValue"
    return f"{result} | null" if param.nullable else result


def _raw_return_type(function: CFunctionIR) -> str:
    returns = function.returns
    if returns.kind == "void":
        return "void"
    if returns.sequence_depth > 0:
        return "RawValue"
    result = _RAW_RETURN_SCALAR_TS.get(returns.kind)
    if result is None:
        return "RawValue"
    return f"{result} | null" if returns.nullable else result


def _render_raw_method_signature(name: str, function: CFunctionIR) -> str:
    params = ", ".join(
        f"{param.name}{'?' if param.has_default else ''}: {_raw_param_type(param)}"
        for param in _public_params(function)
    )
    return f"    {_member_name(name)}: ({params}) => {_raw_return_type(function)};"


def _render_interface_field(declaration: str, doc: str | None) -> str:
    if not doc:
        return f"  {declaration}"
    return f"{_render_doc_comment(doc, '  ')}\n  {declaration}"


def _render_option_interfaces(metadata: BindingABI) -> str:
    chunks = []
    for option in sorted(
        metadata.option_structs.values(), key=lambda item: item.c_type
    ):
        fields = "\n".join(
            _render_interface_field(
                f"{_camel_name(field.name)}{'?' if field.type.nullable or field.has_default else ''}: "
                f"{_option_field_type(field.name, field.type, metadata)};",
                field.doc,
            )
            for field in option.fields
        )
        chunks.append(f"export interface {_option_type_name(option)} {{\n{fields}\n}}")
    return "\n\n".join(chunks)


def _render_generated_handle_interfaces(metadata: BindingABI) -> str:
    chunks = []
    for handle, name in sorted(_GENERATED_HANDLE_TS.items(), key=lambda item: item[1]):
        if handle not in metadata.handles:
            continue
        chunks.append(
            f"export interface {name} {{\n  readonly ptr: number;\n  destroy(): void;\n}}"
        )
    return "\n\n".join(chunks)


def _render_result_interfaces(metadata: BindingABI) -> str:
    chunks = []
    for struct in sorted(metadata.value_types.values(), key=lambda item: item.c_type):
        if struct.kind != "result_struct":
            continue
        fields = "\n".join(
            _render_interface_field(
                f"{_camel_name(field.name)}: {_direct_ts_type_from_c_type(field.c_type, metadata)};",
                field.doc,
            )
            for field in struct.fields
        )
        chunks.append(f"export interface {_struct_type_name(struct)} {{\n{fields}\n}}")
    return "\n\n".join(chunks)


def _param_expr(
    param: CParamIR,
    metadata: BindingABI,
    module_name: str | None = None,
) -> str:
    if _is_property_map_param(param):
        if param.nullable:
            return (
                f"{param.name} == null ? null : "
                f"toRawPsetProperties(shell, {param.name} as PsetProperties | PsetInput, temps)"
            )
        return f"toRawPsetProperties(shell, {param.name} as PsetProperties | PsetInput, temps)"
    if param.type is not None and param.type.kind == "variant":
        descriptor = _variant_descriptor_for_type(param.type, metadata)
        return (
            f"encodeOptionValue({json.dumps(param.name)}, {param.name}, shell, temps, "
            f"undefined, undefined, undefined, {json.dumps(descriptor, sort_keys=True)})"
        )
    normalized_base = (
        " ".join(param.c_type.replace(" *", "*").split())
        .removeprefix("const ")
        .removesuffix("*")
        .strip()
    )
    sequence = next(
        (
            item
            for item in metadata.value_types.values()
            if item.c_type == normalized_base
            and item.kind
            in {
                "sequence",
                "handle_sequence",
                "input_record_sequence",
                "result_record_sequence",
            }
        ),
        None,
    )
    if sequence is not None:
        if param.type is not None and any(
            length is not None for length in param.type.fixed_lengths
        ):
            return (
                f"encodeOptionValue({json.dumps(param.name)}, {param.name}, shell, temps, "
                f"undefined, undefined, {json.dumps(param.type.fixed_lengths)})"
            )
        if sequence.kind == "input_record_sequence":
            option = _option_by_c_type(sequence.element_type or "", metadata)
            if option is None:
                return param.name
            fields = {_camel_name(field.name): field.name for field in option.fields}
            extra_args = _option_codec_args(option, metadata)
            return (
                f"{param.name}.map((item) => encodeOptions(item, "
                f"{json.dumps(fields, sort_keys=True)}, shell, temps{extra_args}))"
            )
        return f"toRawSequence({param.name}, shell, temps)"
    handle = _handle_kind_from_c_type(param.c_type, metadata)
    if _is_instance_list_handle(handle):
        return f"toRaw({param.name}, shell, temps)"
    if handle == "value" and module_name != "value":
        return f"{param.name} == null ? null : toRawValue(shell, {param.name}, temps)"
    if _uses_generated_handle(handle):
        return (
            f"{param.name} == null ? null : {param.name}"
            if param.nullable
            else param.name
        )
    if handle is not None:
        raw = f"{param.name}.raw"
        return f"{param.name} == null ? null : {raw}" if param.nullable else raw
    option = _option_by_c_type(param.c_type, metadata)
    if option is not None:
        fields = {_camel_name(field.name): field.name for field in option.fields}
        extra_args = _option_codec_args(option, metadata)
        return f"encodeOptions({param.name}, {json.dumps(fields, sort_keys=True)}, shell, temps{extra_args})"
    return param.name


def _variant_descriptor_for_type(
    type_spec: TypeSpec, metadata: BindingABI
) -> dict[str, object]:
    alternatives: list[dict[str, object]] = []
    for index, alternative in enumerate(type_spec.variants):
        if alternative.sequence_depth > 0:
            alternatives.append(
                {
                    "kind": index,
                    "mode": "sequence",
                    "fixedLengths": alternative.fixed_lengths,
                }
            )
            continue
        if alternative.kind != "option" or alternative.struct is None:
            raise ValueError(
                "Direct API input variants require semantic record or sequence alternatives"
            )
        nested = metadata.option_structs[alternative.struct]
        alternatives.append(
            {
                "kind": index,
                "mode": "record",
                "sequenceDepth": 0,
                "fields": {_camel_name(item.name): item.name for item in nested.fields},
                "required": sorted(
                    _camel_name(item.name)
                    for item in nested.fields
                    if not item.type.nullable
                ),
                "pset": sorted(_pset_props_field_names(nested)),
                "entities": sorted(_entity_list_field_names(nested, metadata)),
                "fixed": _fixed_field_lengths(nested),
                "variants": _variant_field_descriptors(nested, metadata),
                "records": _record_field_descriptors(nested, metadata),
            }
        )
    return {"sequenceDepth": type_spec.sequence_depth, "alternatives": alternatives}


def _result_wrap_expr(value_expr: str, c_type: str, metadata: BindingABI) -> str:
    ts_type = _direct_ts_type_from_c_type(c_type, metadata)
    handle = _handle_kind_from_c_type(c_type, metadata)
    if handle == "instance":
        return f"wrapEntity(shell, {value_expr})"
    if handle in {"parse_instance_list", "instance_list"}:
        return f"wrapEntities(shell, {value_expr} as never)"
    if handle == "value":
        return f"wrapValue(shell, {value_expr} as never)"
    if ts_type in {"string", "boolean", "number", "bigint"}:
        return f"{value_expr} as {ts_type}"
    sequence = next(
        (
            item
            for item in metadata.value_types.values()
            if item.c_type
            == " ".join(c_type.replace(" *", "*").split())
            .removeprefix("const ")
            .removesuffix("*")
            .strip()
            and item.kind in {"sequence", "handle_sequence", "result_record_sequence"}
        ),
        None,
    )
    if sequence is not None:
        if sequence.kind == "result_record_sequence":
            item = next(
                (
                    value
                    for value in metadata.value_types.values()
                    if value.c_type == sequence.element_type
                    and value.kind == "result_struct"
                ),
                None,
            )
            if item is not None:
                return (
                    f"({value_expr} as RawValue[]).map((item) => "
                    f"{_result_record_expr('item', item, metadata, 'itemData')})"
                )
        return f"wrap(shell, {value_expr})"
    return f"wrap(shell, {value_expr})"


def _raw_result_record_type(struct: CTypeIR, metadata: BindingABI) -> str:
    fields = []
    for field in struct.fields:
        field_type = _direct_ts_type_from_c_type(field.c_type, metadata)
        raw_type = (
            field_type
            if field_type in {"string", "boolean", "number", "bigint"}
            else "RawValue"
        )
        fields.append(f"{field.name}: {raw_type}")
    return f"{{ {'; '.join(fields)} }}"


def _result_record_expr(
    value_expr: str,
    struct: CTypeIR,
    metadata: BindingABI,
    data_name: str,
) -> str:
    fields = ", ".join(
        f"{_camel_name(field.name)}: "
        f"{_result_wrap_expr(f'{data_name}.{field.name}', field.c_type, metadata)}"
        for field in struct.fields
    )
    return (
        f"(() => {{ const {data_name} = {value_expr} as "
        f"{_raw_result_record_type(struct, metadata)}; return {{ {fields} }}; }})()"
    )


def _raw_result_struct_type(function: CFunctionIR, metadata: BindingABI) -> str:
    if function.returns.struct is None:
        return "RawValue"
    struct = metadata.value_types.get(function.returns.struct)
    if struct is None or struct.kind != "result_struct":
        return "RawValue"
    return _raw_result_record_type(struct, metadata)


def _result_struct_expr(
    function: CFunctionIR, return_type: str, metadata: BindingABI
) -> str:
    if function.returns.struct is None:
        return f"wrap(shell, result) as {return_type}"
    struct = metadata.value_types.get(function.returns.struct)
    if struct is None or struct.kind != "result_struct":
        return f"wrap(shell, result) as {return_type}"
    if function.returns.sequence_depth == 1:
        return (
            "(result as RawValue[]).map((item) => "
            f"{_result_record_expr('item', struct, metadata, 'itemData')})"
        )
    fields = ", ".join(
        f"{_camel_name(field.name)}: {_result_wrap_expr(f'data.{field.name}', field.c_type, metadata)}"
        for field in struct.fields
    )
    return f"{{ {fields} }}"


def _result_expr(function: CFunctionIR, return_type: str, module_name: str) -> str:
    handle = _handle_kind_from_type(function.returns)
    if handle is None:
        return "wrap(shell, result)"
    public = _HANDLE_KIND_NAMES.get(handle, handle)
    if public == "file":
        return "IfcFile.wrap(shell, result as never, true)"
    if public == "entity":
        return "wrapEntity(shell, result)"
    if public == "instance_list":
        return "wrapEntities(shell, result as never)"
    if public == "value" and module_name == "value":
        return "wrapValue(shell, result as never)"
    if public == "value":
        return "fromRawValue(shell, result as never)"
    if _uses_generated_handle(handle):
        return "result"
    return "wrap(shell, result)"


def _render_direct_method(
    module_name: str,
    name: str,
    function: CFunctionIR,
    metadata: BindingABI,
) -> str:
    params = _public_params(function)
    signature_params = ", ".join(
        f"{param.name}{'?' if param.has_default else ''}: {_param_ts_type(param, metadata, module_name)}"
        for param in params
    )
    args = ", ".join(_param_expr(param, metadata, module_name) for param in params)
    return_type = (
        "ValueData | null"
        if _handle_kind_from_type(function.returns) == "value"
        and module_name != "value"
        else _direct_ts_type(function.returns, metadata)
    )
    lines = []
    if function.doc:
        lines.append(_render_doc_comment(function.doc, "    "))
    if function.returns.kind == "void":
        lines.extend(
            [
                f"    {_member_name(name)}({signature_params}): void {{",
                "      const temps: Disposable[] = [];",
                "      try {",
                f"        {_property_access(_property_access('raw', module_name), name)}({args});",
                "      } finally {",
                "        disposeAll(temps);",
                "      }",
                "    },",
            ]
        )
        return "\n".join(lines)
    if function.returns.kind == "struct":
        struct_expr = _result_struct_expr(function, return_type, metadata)
        raw_struct_type = _raw_result_struct_type(function, metadata)
        null_guard = (
            ["        if (result === null) return null;"]
            if function.returns.nullable
            else []
        )
        lines.extend(
            [
                f"    {_member_name(name)}({signature_params}): {return_type} {{",
                "      const temps: Disposable[] = [];",
                "      try {",
                f"        const result = {_property_access(_property_access('raw', module_name), name)}({args});",
                *null_guard,
                f"        const data = result as {raw_struct_type};",
                f"        return {struct_expr} as {return_type};",
                "      } finally {",
                "        disposeAll(temps);",
                "      }",
                "    },",
            ]
        )
        return "\n".join(lines)
    if function.returns.kind == "variant":
        lines.extend(
            [
                f"    {_member_name(name)}({signature_params}): {return_type} {{",
                "      const temps: Disposable[] = [];",
                "      try {",
                f"        const result = {_property_access(_property_access('raw', module_name), name)}({args});",
                f"        return wrap(shell, result) as {return_type};",
                "      } finally {",
                "        disposeAll(temps);",
                "      }",
                "    },",
            ]
        )
        return "\n".join(lines)
    lines.extend(
        [
            f"    {_member_name(name)}({signature_params}): {return_type} {{",
            "      const temps: Disposable[] = [];",
            "      try {",
            f"        const result = {_property_access(_property_access('raw', module_name), name)}({args});",
            f"        return {_result_expr(function, return_type, module_name)} as {return_type};",
            "      } finally {",
            "        disposeAll(temps);",
            "      }",
            "    },",
        ]
    )
    return "\n".join(lines)


def _render_direct_interface(
    module_name: str,
    functions: list[tuple[str, CFunctionIR]],
    metadata: BindingABI,
) -> str:
    methods = []
    for name, function in functions:
        return_type = (
            "ValueData | null"
            if _handle_kind_from_type(function.returns) == "value"
            and module_name != "value"
            else _direct_ts_type(function.returns, metadata)
        )
        params = ", ".join(
            f"{param.name}{'?' if param.has_default else ''}: {_param_ts_type(param, metadata, module_name)}"
            for param in _public_params(function)
        )
        signature = f"    {_member_name(name)}({params}): {return_type};"
        if function.doc:
            signature = _render_doc_comment(function.doc, "    ") + "\n" + signature
        methods.append(signature)
    return (
        f"export interface {_module_type_name(module_name)} {{\n"
        + "\n".join(methods)
        + "\n}"
    )


def _render_raw_api_type(
    modules: dict[str, list[tuple[str, CFunctionIR]]],
) -> str:
    module_types = []
    for module_name, functions in sorted(modules.items()):
        methods = "\n".join(
            _render_raw_method_signature(name, function) for name, function in functions
        )
        module_types.append(f"  {_member_name(module_name)}: {{\n{methods}\n  }};")
    return "type RawApi = {\n" + "\n".join(module_types) + "\n};"


def render_api_direct(metadata: BindingABI) -> str:
    modules = _api_functions(metadata)
    module_interfaces = [
        _render_direct_interface(name, functions, metadata)
        for name, functions in sorted(modules.items())
    ]
    api_member_lines = []
    for name in sorted(modules):
        public_name = _module_public_name(name)
        interface_name = _module_type_name(name)
        api_member_lines.append(f"  {public_name}: {interface_name};")
        if public_name != name:
            api_member_lines.append(f"  {name}: {interface_name};")
    api_members = "\n".join(api_member_lines)
    module_definitions = []
    module_values = []
    for module_name, functions in sorted(modules.items()):
        methods = "\n".join(
            _render_direct_method(module_name, name, function, metadata)
            for name, function in functions
        )
        public_name = _module_public_name(module_name)
        if public_name != module_name:
            value_name = f"{public_name}Api"
            module_definitions.append(
                f"  const {value_name} = Object.freeze({{\n{methods}\n  }});"
            )
            module_values.append(f"    {public_name}: {value_name},")
            module_values.append(f"    {module_name}: {value_name},")
        else:
            module_values.append(
                f"    {public_name}: Object.freeze({{\n{methods}\n    }}),"
            )
    return "\n".join(
        [
            "// This file was generated with the assistance of an AI coding tool.",
            "",
            "import { AttributeValue } from '../attribute.js';",
            "import { Entity } from '../entity.js';",
            "import { IfcFile } from '../file.js';",
            "import { GeomIterator, GeomSettings, GeometryTree } from '../geom/index.js';",
            "import { SerializerSettings } from '../serializers/index.js';",
            "import {",
            "  InstanceList,",
            "  Value,",
            "  fromRawValue,",
            "  toRawValue,",
            "  wrapList,",
            "  wrapValue,",
            "  type ApiData,",
            "  type ValueData,",
            "  type ValueInput,",
            "} from '../api.js';",
            "import { PsetProperties, toRawPsetProperties, type PsetInput } from '../pset.js';",
            "import type { IfcOpenShell } from '../init.js';",
            "",
            "type RawValue = ApiData | object | RawValue[];",
            "type ApiInput = ApiData | PsetProperties | PsetInput;",
            "type Disposable = { destroy(): void };",
            "type FixedLength = null | number;",
            _render_raw_api_type(modules),
            "",
            _render_result_interfaces(metadata),
            "",
            _render_generated_handle_interfaces(metadata),
            "",
            _render_semantic_aliases(metadata),
            "",
            _render_option_interfaces(metadata),
            "",
            *module_interfaces,
            "",
            "export interface Api {",
            api_members,
            "}",
            "",
            "/**",
            " * @internal",
            " */",
            "export function createApi(shell: IfcOpenShell): Api {",
            "  const raw = shell.raw as object as RawApi;",
            *module_definitions,
            "  return Object.freeze({",
            "\n".join(module_values),
            "  });",
            "}",
            "",
            "function encodeOptions(",
            "  value: object,",
            "  fields: Record<string, string>,",
            "  shell: IfcOpenShell,",
            "  temps: Disposable[],",
            "  psetFields?: string[],",
            "  entityListFields?: string[],",
            "  fixedFields?: Record<string, FixedLength[]>,",
            "  variantFields?: Record<string, VariantDescriptor>,",
            "  recordFields?: Record<string, RecordDescriptor>,",
            "): Record<string, RawValue> {",
            "  const data = value as Record<string, ApiInput | undefined>;",
            "  const psetFieldSet = psetFields ? new Set(psetFields) : undefined;",
            "  const entityListFieldSet = entityListFields ? new Set(entityListFields) : undefined;",
            "  return Object.fromEntries(",
            "    Object.entries(fields)",
            "      .filter(([publicName]) => data[publicName] !== undefined)",
            "      .map(([publicName, nativeName]) => [nativeName, encodeOptionValue(publicName, data[publicName] as ApiInput, shell, temps, psetFieldSet, entityListFieldSet, fixedFields?.[publicName], variantFields?.[publicName], recordFields?.[publicName])]),",
            "  ) as Record<string, RawValue>;",
            "}",
            "",
            "function toRawSequence(value: ApiInput, shell: IfcOpenShell, temps: Disposable[]): RawValue {",
            "  if (value === null) return null;",
            "  if (Array.isArray(value)) return value.map((item) => toRawSequence(item, shell, temps));",
            "  return toRaw(value, shell, temps);",
            "}",
            "",
            "function toRaw(value: ApiInput, shell: IfcOpenShell, temps: Disposable[]): RawValue {",
            "  if (value instanceof IfcFile) return value.raw;",
            "  if (value instanceof Entity) return value.raw;",
            "  if (value instanceof AttributeValue) return value.raw;",
            "  if (value instanceof GeomSettings) return value.raw;",
            "  if (value instanceof SerializerSettings) return value.raw;",
            "  if (value instanceof GeomIterator) return value.raw;",
            "  if (value instanceof GeometryTree) return value.raw;",
            "  if (value instanceof InstanceList) return value.raw;",
            "  if (value instanceof Value) return value.raw;",
            "  if (value instanceof PsetProperties) return value.raw;",
            "  if (isEntityArray(value)) {",
            "    const raw = shell.raw.parse.instanceListCreateFromHandles(value.map((item) => item.raw));",
            "    temps.push(raw);",
            "    return raw;",
            "  }",
            "  if (Array.isArray(value)) return value.map((item) => toRaw(item, shell, temps));",
            "  if (isPlainObject(value)) {",
            "    return Object.fromEntries(Object.entries(value).map(([key, item]) => [key, toRaw(item, shell, temps)]));",
            "  }",
            "  return value;",
            "}",
            "",
            "function toRawEntityList(value: ApiInput, shell: IfcOpenShell, temps: Disposable[]): RawValue {",
            "  if (!Array.isArray(value)) throw new TypeError('Expected an entity array.');",
            "  const raw = shell.raw.parse.instanceListCreateFromHandles(value.map((item) => (item as Entity).raw));",
            "  temps.push(raw);",
            "  return raw;",
            "}",
            "",
            "type RecordDescriptor = { sequenceDepth: number; fields: Record<string, string>; pset: string[]; entities: string[]; fixed: Record<string, FixedLength[]>; variants: Record<string, VariantDescriptor>; records: Record<string, RecordDescriptor> };",
            "type VariantAlternative = (RecordDescriptor & { kind: number; mode: 'record'; required: string[] }) | { kind: number; mode: 'sequence'; fixedLengths: FixedLength[] };",
            "type VariantDescriptor = { sequenceDepth: number; alternatives: VariantAlternative[] };",
            "",
            "function encodeOptionValue(publicName: string, value: ApiInput, shell: IfcOpenShell, temps: Disposable[], psetFields?: Set<string>, entityListFields?: Set<string>, fixedLengths?: FixedLength[], variantDescriptor?: VariantDescriptor, record?: RecordDescriptor): RawValue {",
            "  if (variantDescriptor) {",
            "    const encodeVariant = (item: ApiInput, depth: number): RawValue => {",
            "      if (depth < variantDescriptor.sequenceDepth) {",
            "        if (!Array.isArray(item)) throw new TypeError(`Expected ${publicName} to be an array.`);",
            "        return item.map((nested) => encodeVariant(nested, depth + 1));",
            "      }",
            "      const matches = variantDescriptor.alternatives.filter((alternative) => {",
            "        if (alternative.mode === 'sequence') return matchesFixedLengths(item, alternative.fixedLengths);",
            "        return isPlainObject(item) && alternative.required.every((name) => Object.prototype.hasOwnProperty.call(item, name));",
            "      });",
            "      if (matches.length !== 1) throw new TypeError(`Expected ${publicName} to match exactly one variant alternative.`);",
            "      const alternative = matches[0]!;",
            "      if (alternative.mode === 'sequence') {",
            "        return { kind: alternative.kind, [`value_${alternative.kind}`]: toRaw(item, shell, temps) };",
            "      }",
            "      if (!isPlainObject(item)) throw new TypeError(`Expected ${publicName} to be an object.`);",
            "      return {",
            "        kind: alternative.kind,",
            "        [`value_${alternative.kind}`]: encodeOptions(item, alternative.fields, shell, temps, alternative.pset, alternative.entities, alternative.fixed, alternative.variants, alternative.records),",
            "      };",
            "    };",
            "    return encodeVariant(value, 0);",
            "  }",
            "  if (record) {",
            "    const encodeRecord = (item: ApiInput, depth: number): RawValue => {",
            "      if (depth < record.sequenceDepth) {",
            "        if (!Array.isArray(item)) throw new TypeError(`Expected ${publicName} to be an array.`);",
            "        return item.map((nested) => encodeRecord(nested, depth + 1));",
            "      }",
            "      if (!isPlainObject(item)) throw new TypeError(`Expected ${publicName} to be an object.`);",
            "      return encodeOptions(item, record.fields, shell, temps, record.pset, record.entities, record.fixed, record.variants, record.records);",
            "    };",
            "    return encodeRecord(value, 0);",
            "  }",
            "  if (fixedLengths) validateFixedLengths(publicName, value, fixedLengths);",
            "  if (psetFields?.has(publicName)) {",
            "    return toRawPsetProperties(shell, value as PsetProperties | PsetInput, temps);",
            "  }",
            "  if (entityListFields?.has(publicName)) {",
            "    return toRawEntityList(value, shell, temps);",
            "  }",
            "  return toRaw(value, shell, temps);",
            "}",
            "",
            "function validateFixedLengths(publicName: string, value: ApiInput, lengths: FixedLength[]): void {",
            "  const visit = (level: ApiInput, depth: number): void => {",
            "    if (depth >= lengths.length) return;",
            "    if (!Array.isArray(level)) throw new TypeError(`Expected ${publicName} to be an array.`);",
            "    const expected = lengths[depth];",
            "    if (expected !== null && expected !== undefined && level.length !== expected) {",
            "      throw new TypeError(`Expected ${publicName} to contain ${expected} items.`);",
            "    }",
            "    for (const item of level) visit(item as ApiInput, depth + 1);",
            "  };",
            "  visit(value, 0);",
            "}",
            "",
            "function matchesFixedLengths(value: ApiInput, lengths: FixedLength[]): boolean {",
            "  try {",
            "    validateFixedLengths('variant', value, lengths);",
            "    return true;",
            "  } catch {",
            "    return false;",
            "  }",
            "}",
            "",
            "function wrapEntities(shell: IfcOpenShell, value: RawValue): Entity[] {",
            "  const list = wrapList(shell, value as never);",
            "  if (list === null) return [];",
            "  try {",
            "    return list.items();",
            "  } finally {",
            "    list.dispose();",
            "  }",
            "}",
            "",
            "function wrap(shell: IfcOpenShell, value: RawValue): ApiData {",
            "  if (isRawEntity(value)) return wrapEntity(shell, value);",
            "  if (isRawList(value)) return wrapEntities(shell, value);",
            "  if (isRawValue(value)) return wrapValue(shell, value as never);",
            "  if (Array.isArray(value)) return value.map((item) => wrap(shell, item));",
            "  if (isPlainObject(value)) return Object.fromEntries(Object.entries(value).map(([key, item]) => [key, wrap(shell, item)]));",
            "  return value as never;",
            "}",
            "",
            "function wrapEntity(shell: IfcOpenShell, value: RawValue): Entity | null {",
            "  return Entity.wrap(shell, value as never);",
            "}",
            "",
            "function isRawEntity(value: RawValue): boolean {",
            "  return Boolean(value && typeof value === 'object' && 'id' in value && 'className' in value);",
            "}",
            "",
            "function isRawList(value: RawValue): boolean {",
            "  return Boolean(value && typeof value === 'object' && 'size' in value && 'get' in value);",
            "}",
            "",
            "function isRawValue(value: RawValue): boolean {",
            "  return Boolean(value && typeof value === 'object' && 'ptr' in value && Object.getPrototypeOf(value)?.constructor?.name === 'IfcOpenshellValue');",
            "}",
            "",
            "function isEntityArray(value: ApiInput): value is Entity[] {",
            "  return Array.isArray(value) && value.length > 0 && value.every((item) => item instanceof Entity);",
            "}",
            "",
            "function isPlainObject(value: RawValue): value is Record<string, ApiInput> {",
            "  return typeof value === 'object' && value !== null && Object.getPrototypeOf(value) === Object.prototype;",
            "}",
            "",
            "function disposeAll(values: Disposable[]): void {",
            "  for (let index = values.length - 1; index >= 0; index -= 1) values[index]?.destroy();",
            "}",
            "",
        ]
    )


__all__ = ["render_api_direct"]
