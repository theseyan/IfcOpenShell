# This file was generated with the assistance of an AI coding tool.

from __future__ import annotations

import re

try:
    from .authored_spec import HandleSpec, TypeSpec
    from .binding_ir import BindingIR
except ImportError:  # pragma: no cover - script execution fallback
    from authored_spec import HandleSpec, TypeSpec
    from binding_ir import BindingIR


_SEQUENCE_LEAF_CPP_TYPE: dict[str, str] = {
    "string": "std::string",
    "bool": "bool",
    "int32": "int",
    "int64": "int64_t",
    "uint8": "uint8_t",
    "uint32": "unsigned int",
    "double": "double",
}

_SEQUENCE_LEAF_C_TYPE: dict[str, str] = {
    "string": "ifcopenshell_string_t",
    "bool": "bool",
    "int32": "int32_t",
    "int64": "int64_t",
    "uint8": "uint8_t",
    "uint32": "uint32_t",
    "double": "double",
}


def _sequence_kind_parts(kind: str) -> tuple[str, int] | None:
    match = re.fullmatch(r"([a-z0-9]+)((?:_list)+)", kind)
    if match is None:
        return None
    leaf = match.group(1)
    depth = match.group(2).count("_list")
    if leaf not in _SEQUENCE_LEAF_CPP_TYPE:
        return None
    return leaf, depth


def _is_sequence_kind(kind: str) -> bool:
    return _sequence_kind_parts(kind) is not None


def _sequence_c_type(kind: str) -> str:
    return f"ifcopenshell_{kind}_t"


def _sequence_destroy_name(kind: str) -> str:
    return f"ifcopenshell_{kind}_destroy"


def _sequence_make_name(kind: str) -> str:
    return f"make_{kind}"


def _sequence_to_cpp_name(kind: str) -> str:
    return f"to_cpp_{kind}"


def _sequence_prev_kind(kind: str) -> str:
    leaf, depth = _sequence_kind_parts(kind) or ("", 0)
    if depth <= 1:
        raise ValueError(f"{kind} has no previous sequence kind")
    return f"{leaf}{'_list' * (depth - 1)}"


def _sequence_cpp_type(kind: str) -> str:
    leaf, depth = _sequence_kind_parts(kind) or ("", 0)
    cpp_type = _SEQUENCE_LEAF_CPP_TYPE[leaf]
    for _ in range(depth):
        cpp_type = f"std::vector<{cpp_type}>"
    return cpp_type


def _sequence_items_c_type(kind: str) -> str:
    leaf, depth = _sequence_kind_parts(kind) or ("", 0)
    if depth == 1:
        return _SEQUENCE_LEAF_C_TYPE[leaf]
    return _sequence_c_type(_sequence_prev_kind(kind))


def _render_common_type_decls(sequence_kinds: tuple[str, ...]) -> str:
    typedefs = [
        """typedef struct ifcopenshell_string_t {
    char* data;
    size_t size;
    bool owned;
} ifcopenshell_string_t;""",
        """typedef enum ifcopenshell_logical_t {
    IFCOPENSHELL_LOGICAL_UNKNOWN = -1,
    IFCOPENSHELL_LOGICAL_FALSE = 0,
    IFCOPENSHELL_LOGICAL_TRUE = 1
} ifcopenshell_logical_t;""",
    ]
    for kind in sequence_kinds:
        typedefs.append(
            f"""typedef struct {_sequence_c_type(kind)} {{
    {_sequence_items_c_type(kind)}* items;
    size_t size;
}} {_sequence_c_type(kind)};"""
        )
    decls = ["void ifcopenshell_string_destroy(ifcopenshell_string_t* value);"]
    decls.extend(f"void {_sequence_destroy_name(kind)}({_sequence_c_type(kind)}* value);" for kind in sequence_kinds)
    return "\n\n".join((*typedefs, *decls))


def _render_sequence_destroy_impl(kind: str) -> str:
    leaf, depth = _sequence_kind_parts(kind) or ("", 0)
    if depth == 1 and leaf != "string":
        body = "    delete[] value->items;"
    else:
        child_destroy = (
            "ifcopenshell_string_destroy"
            if depth == 1 and leaf == "string"
            else _sequence_destroy_name(_sequence_prev_kind(kind))
        )
        body = (
            "    for (size_t i = 0; i < value->size; ++i) {\n"
            f"        {child_destroy}(&value->items[i]);\n"
            "    }\n"
            "    delete[] value->items;"
        )
    return f"""void {_sequence_destroy_name(kind)}({_sequence_c_type(kind)}* value) {{
    if (value == nullptr || value->items == nullptr) {{
        return;
    }}
{body}
    value->items = nullptr;
    value->size = 0;
}}"""


def _render_common_type_impls(sequence_kinds: tuple[str, ...]) -> str:
    impls = [
        """void ifcopenshell_string_destroy(ifcopenshell_string_t* value) {
    if (value == nullptr) {
        return;
    }
    if (value->owned && value->data != nullptr) {
        delete[] value->data;
    }
    value->data = nullptr;
    value->size = 0;
    value->owned = false;
}"""
    ]
    impls.extend(_render_sequence_destroy_impl(kind) for kind in sequence_kinds)
    return "\n\n".join(impls)


def _render_sequence_make_impl(kind: str) -> str:
    leaf, depth = _sequence_kind_parts(kind) or ("", 0)
    cpp_type = _sequence_cpp_type(kind)
    c_type = _sequence_c_type(kind)
    if depth == 1:
        if leaf == "string":
            item_init = "        items[i] = make_string(values[i]);"
            return f"""static {c_type} {_sequence_make_name(kind)}(const {cpp_type}& values) {{
    auto* items = values.empty() ? nullptr : new {_sequence_items_c_type(kind)}[values.size()];
    size_t initialized = 0;
    try {{
        for (size_t i = 0; i < values.size(); ++i) {{
{item_init}
            ++initialized;
        }}
    }} catch (...) {{
        for (size_t j = 0; j < initialized; ++j) {{
            delete[] items[j].data;
        }}
        delete[] items;
        throw;
    }}
    return {c_type}{{items, values.size()}};
}}"""
        if leaf == "int32":
            item_init = "        items[i] = static_cast<int32_t>(values[i]);"
        elif leaf == "uint32":
            item_init = "        items[i] = static_cast<uint32_t>(values[i]);"
        else:
            item_init = "        items[i] = values[i];"
        return f"""static {c_type} {_sequence_make_name(kind)}(const {cpp_type}& values) {{
    auto* items = values.empty() ? nullptr : new {_sequence_items_c_type(kind)}[values.size()];
    for (size_t i = 0; i < values.size(); ++i) {{
{item_init}
    }}
    return {c_type}{{items, values.size()}};
}}"""
    prev_kind = _sequence_prev_kind(kind)
    return f"""static {c_type} {_sequence_make_name(kind)}(const {cpp_type}& values) {{
    auto* items = values.empty() ? nullptr : new {_sequence_items_c_type(kind)}[values.size()];
    for (size_t i = 0; i < values.size(); ++i) {{
        items[i] = {_sequence_make_name(prev_kind)}(values[i]);
    }}
    return {c_type}{{items, values.size()}};
}}"""


def _render_sequence_to_cpp_impl(kind: str) -> str:
    leaf, depth = _sequence_kind_parts(kind) or ("", 0)
    cpp_type = _sequence_cpp_type(kind)
    c_type = _sequence_c_type(kind)
    if depth == 1:
        if leaf == "string":
            body = (
                "    std::vector<std::string> result;\n"
                "    result.reserve(value->size);\n"
                "    for (size_t i = 0; i < value->size; ++i) {\n"
                "        const auto& item = value->items[i];\n"
                '        if (item.data == nullptr && item.size > 0) {\n'
                '            throw std::runtime_error("string_list contains a null string buffer");\n'
                "        }\n"
                '        result.emplace_back(item.data == nullptr ? "" : item.data, item.size);\n'
                "    }\n"
                "    return result;"
            )
        elif leaf == "bool":
            body = (
                "    std::vector<bool> result;\n"
                "    result.reserve(value->size);\n"
                "    for (size_t i = 0; i < value->size; ++i) {\n"
                "        result.push_back(value->items[i]);\n"
                "    }\n"
                "    return result;"
            )
        else:
            body = (
                "    if (value->size == 0) {\n"
                "        return {};\n"
                "    }\n"
                f"    return {cpp_type}(value->items, value->items + value->size);"
            )
        return f"""static {cpp_type} {_sequence_to_cpp_name(kind)}(const {c_type}* value) {{
    validate_list_items("{kind}", value->items, value->size);
{body}
}}"""
    prev_kind = _sequence_prev_kind(kind)
    return f"""static {cpp_type} {_sequence_to_cpp_name(kind)}(const {c_type}* value) {{
    validate_list_items("{kind}", value->items, value->size);
    {cpp_type} result;
    result.reserve(value->size);
    for (size_t i = 0; i < value->size; ++i) {{
        result.push_back({_sequence_to_cpp_name(prev_kind)}(&value->items[i]));
    }}
    return result;
}}"""


def _render_sequence_helpers(sequence_kinds: tuple[str, ...]) -> str:
    blocks: list[str] = []
    for kind in sequence_kinds:
        blocks.append(_render_sequence_make_impl(kind))
        blocks.append(_render_sequence_to_cpp_impl(kind))
    return "\n\n".join(blocks)


def _snake_name(c_type: str) -> str:
    return c_type.removeprefix("ifcopenshell_").removesuffix("_t")


def _handle_list_c_type(handle: HandleSpec) -> str:
    return f"{handle.c_type.removesuffix('_t')}_list_t"


def _handle_list_list_c_type(handle: HandleSpec) -> str:
    return f"{handle.c_type.removesuffix('_t')}_list_list_t"


def _handle_list_helper_name(handle: HandleSpec) -> str:
    return f"make_{_snake_name(_handle_list_c_type(handle))}"


def _handle_list_list_helper_name(handle: HandleSpec) -> str:
    return f"make_{_snake_name(_handle_list_list_c_type(handle))}"


def _render_handle_list_helpers(handle: HandleSpec) -> str:
    list_c = _handle_list_c_type(handle)
    helper_name = _handle_list_helper_name(handle)
    snake = _snake_name(list_c)
    if handle.ptr_type == "shared_ptr":
        return f"""static {list_c} {helper_name}(const std::vector<std::shared_ptr<{handle.cpp_type}>>& values) {{
    auto** items = values.empty() ? nullptr : new {handle.c_type}*[values.size()];
    size_t initialized = 0;
    try {{
        for (size_t i = 0; i < values.size(); ++i) {{
            items[i] = new {handle.c_type}{{values[i]}};
            ++initialized;
        }}
    }} catch (...) {{
        for (size_t j = 0; j < initialized; ++j) {{ delete items[j]; }}
        delete[] items;
        throw;
    }}
    return {list_c}{{items, values.size()}};
}}

static std::vector<std::shared_ptr<{handle.cpp_type}>> to_cpp_{snake}(const {list_c}* values) {{
    validate_list_items("{snake}", values->items, values->size);
    std::vector<std::shared_ptr<{handle.cpp_type}>> result;
    result.reserve(values->size);
    for (size_t i = 0; i < values->size; ++i) {{
        auto* item = values->items[i];
        if (item == nullptr) {{
            throw std::runtime_error("handle_list contains an invalid handle");
        }}
        result.push_back(item->ptr);
    }}
    return result;
}}"""
    return f"""static {list_c} {helper_name}(const std::vector<{handle.cpp_type}*>& values) {{
    auto** items = values.empty() ? nullptr : new {handle.c_type}*[values.size()];
    size_t initialized = 0;
    try {{
        for (size_t i = 0; i < values.size(); ++i) {{
            items[i] = new {handle.c_type}{{values[i], false}};
            ++initialized;
        }}
    }} catch (...) {{
        for (size_t j = 0; j < initialized; ++j) {{ delete items[j]; }}
        delete[] items;
        throw;
    }}
    return {list_c}{{items, values.size()}};
}}

static {list_c} {helper_name}(const std::vector<const {handle.cpp_type}*>& values) {{
    auto** items = values.empty() ? nullptr : new {handle.c_type}*[values.size()];
    size_t initialized = 0;
    try {{
        for (size_t i = 0; i < values.size(); ++i) {{
            items[i] = new {handle.c_type}{{const_cast<{handle.cpp_type}*>(values[i]), false}};
            ++initialized;
        }}
    }} catch (...) {{
        for (size_t j = 0; j < initialized; ++j) {{ delete items[j]; }}
        delete[] items;
        throw;
    }}
    return {list_c}{{items, values.size()}};
}}

static {list_c} {helper_name}(std::vector<std::unique_ptr<{handle.cpp_type}>> values) {{
    auto** items = values.empty() ? nullptr : new {handle.c_type}*[values.size()];
    size_t initialized = 0;
    try {{
        for (size_t i = 0; i < values.size(); ++i) {{
            items[i] = new {handle.c_type}{{values[i].release(), true}};
            ++initialized;
        }}
    }} catch (...) {{
        for (size_t j = 0; j < initialized; ++j) {{ delete items[j]; }}
        delete[] items;
        throw;
    }}
    return {list_c}{{items, values.size()}};
}}

static std::vector<const {handle.cpp_type}*> to_cpp_{snake}(const {list_c}* values) {{
    validate_list_items("{snake}", values->items, values->size);
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


def _render_handle_list_list_helpers(handle: HandleSpec) -> str:
    list_list_c = _handle_list_list_c_type(handle)
    list_c = _handle_list_c_type(handle)
    helper_name = _handle_list_list_helper_name(handle)
    row_helper_name = _handle_list_helper_name(handle)
    snake = _snake_name(list_list_c)
    row_snake = _snake_name(list_c)
    if handle.ptr_type == "shared_ptr":
        shared_vector = f"std::vector<std::vector<std::shared_ptr<{handle.cpp_type}>>>"
        return f"""static {list_list_c} {helper_name}(const {shared_vector}& values) {{
    auto* items = values.empty() ? nullptr : new {list_c}[values.size()];
    for (size_t i = 0; i < values.size(); ++i) {{
        items[i] = {row_helper_name}(values[i]);
    }}
    return {list_list_c}{{items, values.size()}};
}}

static std::vector<std::vector<std::shared_ptr<{handle.cpp_type}>>> to_cpp_{snake}(const {list_list_c}* values) {{
    validate_list_items("{snake}", values->items, values->size);
    std::vector<std::vector<std::shared_ptr<{handle.cpp_type}>>> result;
    result.reserve(values->size);
    for (size_t i = 0; i < values->size; ++i) {{
        result.push_back(to_cpp_{row_snake}(&values->items[i]));
    }}
    return result;
}}"""
    return f"""static {list_list_c} {helper_name}(const std::vector<std::vector<{handle.cpp_type}*>>& values) {{
    auto* items = values.empty() ? nullptr : new {list_c}[values.size()];
    for (size_t i = 0; i < values.size(); ++i) {{
        items[i] = {row_helper_name}(values[i]);
    }}
    return {list_list_c}{{items, values.size()}};
}}

static {list_list_c} {helper_name}(const std::vector<std::vector<const {handle.cpp_type}*>>& values) {{
    auto* items = values.empty() ? nullptr : new {list_c}[values.size()];
    for (size_t i = 0; i < values.size(); ++i) {{
        items[i] = {row_helper_name}(values[i]);
    }}
    return {list_list_c}{{items, values.size()}};
}}

static std::vector<std::vector<const {handle.cpp_type}*>> to_cpp_{snake}(const {list_list_c}* values) {{
    validate_list_items("{snake}", values->items, values->size);
    std::vector<std::vector<const {handle.cpp_type}*>> result;
    result.reserve(values->size);
    for (size_t i = 0; i < values->size; ++i) {{
        result.push_back(to_cpp_{row_snake}(&values->items[i]));
    }}
    return result;
}}"""


def _render_handle_list_destroy_decl(handle: HandleSpec) -> str:
    list_c_type = _handle_list_c_type(handle)
    return f"void ifcopenshell_{_snake_name(list_c_type)}_destroy({list_c_type}* value);"


def _render_handle_list_list_destroy_decl(handle: HandleSpec) -> str:
    list_list_c_type = _handle_list_list_c_type(handle)
    return f"void ifcopenshell_{_snake_name(list_list_c_type)}_destroy({list_list_c_type}* value);"


def _render_handle_list_destroy_impl(handle: HandleSpec) -> str:
    list_c_type = _handle_list_c_type(handle)
    return f"""void ifcopenshell_{_snake_name(list_c_type)}_destroy({list_c_type}* value) {{
    if (value == nullptr || value->items == nullptr) {{
        return;
    }}
    for (size_t i = 0; i < value->size; ++i) {{
        if (value->items[i] != nullptr) {{
            ifcopenshell_{_snake_name(handle.c_type)}_destroy(value->items[i]);
        }}
    }}
    delete[] value->items;
    value->items = nullptr;
    value->size = 0;
}}"""


def _render_handle_list_list_destroy_impl(handle: HandleSpec) -> str:
    list_list_c_type = _handle_list_list_c_type(handle)
    return f"""void ifcopenshell_{_snake_name(list_list_c_type)}_destroy({list_list_c_type}* value) {{
    if (value == nullptr || value->items == nullptr) {{
        return;
    }}
    for (size_t i = 0; i < value->size; ++i) {{
        ifcopenshell_{_snake_name(_handle_list_c_type(handle))}_destroy(&value->items[i]);
    }}
    delete[] value->items;
    value->items = nullptr;
    value->size = 0;
}}"""


def _used_handle_list_handles(spec: BindingIR) -> tuple[HandleSpec, ...]:
    seen: set[str] = set()
    handles: list[HandleSpec] = []
    for call in (*spec.functions, *spec.methods):
        if call.returns.kind == "handle" and call.returns.sequence_depth > 0:
            handle_name = call.returns.handle
            if handle_name not in seen:
                seen.add(handle_name)
                handles.append(spec.handles[handle_name])
        for param in call.params:
            if param.type.kind != "handle" or param.type.sequence_depth == 0:
                continue
            handle_name = param.type.handle
            if handle_name in seen:
                continue
            seen.add(handle_name)
            handles.append(spec.handles[handle_name])
    return tuple(handles)


def _type_spec_sequence_kind(type_spec: TypeSpec) -> str | None:
    if type_spec.sequence_depth <= 0:
        return None
    if type_spec.kind == "handle":
        return None
    return f"{type_spec.kind}{'_list' * type_spec.sequence_depth}"


def _used_scalar_sequence_kinds(spec: BindingIR) -> tuple[str, ...]:
    seen: set[str] = set()
    ordered: list[str] = []

    def add_type(type_spec: TypeSpec) -> None:
        kind = _type_spec_sequence_kind(type_spec)
        if kind is None:
            return
        parts = _sequence_kind_parts(kind)
        if parts is None:
            return
        leaf, depth = parts
        for current_depth in range(1, depth + 1):
            current_kind = f"{leaf}{'_list' * current_depth}"
            if current_kind not in seen:
                seen.add(current_kind)
                ordered.append(current_kind)

    for call in (*spec.functions, *spec.methods):
        add_type(call.returns)
        for param in call.params:
            add_type(param.type)
    return tuple(ordered)


def _sequence_param_type(kind: str) -> str:
    return f"const {_sequence_c_type(kind)}*"


def _sequence_out_type(kind: str) -> str:
    return f"{_sequence_c_type(kind)}*"


def _sequence_make_helper(kind: str) -> str:
    return _sequence_make_name(kind)


def _sequence_to_cpp_helper(kind: str) -> str:
    return _sequence_to_cpp_name(kind)
