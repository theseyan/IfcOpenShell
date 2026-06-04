# This file was generated with the assistance of an AI coding tool.

from __future__ import annotations

import re
from pathlib import Path

try:
    from .binding_ir import BindingIR
    from .host_metadata import (
        HostBindingMetadata,
        HostFunctionMetadata,
        HostParamMetadata,
        HostStructMetadata,
        build_host_metadata,
    )
except ImportError:  # pragma: no cover - script execution fallback
    from binding_ir import BindingIR
    from host_metadata import (
        HostBindingMetadata,
        HostFunctionMetadata,
        HostParamMetadata,
        HostStructMetadata,
        build_host_metadata,
    )


_SCALAR_DECLS = {
    "bool": ("int", "p", "PyBool_FromLong({name})"),
    "double": ("double", "d", "PyFloat_FromDouble({name})"),
    "ifcopenshell_logical_t": ("int", "i", "PyLong_FromLong({name})"),
    "int32_t": ("int32_t", "i", "PyLong_FromLong({name})"),
    "int64_t": ("int64_t", "L", "PyLong_FromLongLong({name})"),
    "uint32_t": ("uint32_t", "I", "PyLong_FromUnsignedLong({name})"),
    "size_t": ("Py_ssize_t", "n", "PyLong_FromSsize_t((Py_ssize_t){name})"),
}

_LEAF_CONVERTERS = {
    "bool": "PyBool_FromLong({expr})",
    "double": "PyFloat_FromDouble({expr})",
    "int32_t": "PyLong_FromLong({expr})",
    "int64_t": "PyLong_FromLongLong({expr})",
    "uint8_t": "PyLong_FromUnsignedLong({expr})",
    "uint32_t": "PyLong_FromUnsignedLong({expr})",
    "ifcopenshell_string_t": "convert_ifcopenshell_string_copy(&{expr})",
}

_LEAF_READERS = {
    "bool": "PyObject_IsTrue({obj})",
    "double": "PyFloat_AsDouble({obj})",
    "int32_t": "(int32_t)PyLong_AsLong({obj})",
    "int64_t": "(int64_t)PyLong_AsLongLong({obj})",
    "uint8_t": "(uint8_t)PyLong_AsUnsignedLong({obj})",
    "uint32_t": "(uint32_t)PyLong_AsUnsignedLong({obj})",
}


def _snake_name(c_type: str) -> str:
    return c_type.removeprefix("ifcopenshell_").removesuffix("_t")


def _normalize_c_type(c_type: str) -> str:
    return " ".join(c_type.replace(" *", "*").split())


def _base_pointer_type(c_type: str) -> tuple[str, int]:
    normalized = _normalize_c_type(c_type)
    pointer_depth = 0
    while normalized.endswith("*"):
        pointer_depth += 1
        normalized = normalized[:-1].strip()
    return normalized.removeprefix("const ").strip(), pointer_depth


def _pointee_type(c_type: str) -> str:
    normalized = _normalize_c_type(c_type)
    while normalized.endswith("*"):
        normalized = normalized[:-1].strip()
    return normalized


def _py_type_name(c_type: str) -> str:
    parts = _snake_name(c_type).split("_")
    return "IfcOpenshell" + "".join(part.capitalize() for part in parts if part)


def _wrapper_name(c_name: str) -> str:
    return f"py_{c_name}"


_PREFIXES = sorted(
    [
        "ifcopenshell_ifcparse_",
        "ifcopenshell_ifcapi_",
        "ifcopenshell_ifc_",
        "ifcopenshell_ifc",
    ],
    key=len,
    reverse=True,
)


def _method_name(c_name: str, c_prefix: str) -> str:
    for prefix in _PREFIXES:
        if c_name.startswith(prefix):
            return c_name[len(prefix) :]
    return c_name.removeprefix(f"{c_prefix}_")


def _discover_all_handle_types(api_header_path: Path) -> dict[str, HostStructMetadata]:
    header = api_header_path.read_text(encoding="utf-8")
    pattern = re.compile(r"^typedef\s+struct\s+(ifcopenshell_\w+_t)\s+\1\s*;$", re.MULTILINE)
    return {
        _snake_name(match.group(1)): HostStructMetadata(
            c_type=match.group(1),
            kind="handle",
            fields=(),
            destroy_function=f"ifcopenshell_{_snake_name(match.group(1))}_destroy",
            element_type=None,
            layout="opaque",
        )
        for match in pattern.finditer(header)
    }


def _render_handle_type_decl(handle: HostStructMetadata) -> str:
    py_name = _py_type_name(handle.c_type)
    destroy = handle.destroy_function or f"ifcopenshell_{_snake_name(handle.c_type)}_destroy"
    return f"""\
typedef struct {{
    PyObject_HEAD
    void *handle;
    int owned;
}} {py_name}Object;

static void {py_name}_dealloc({py_name}Object *self) {{
    if (self->owned && self->handle) {{
        {destroy}(({handle.c_type} *)self->handle);
    }}
    self->handle = NULL;
    self->owned = 0;
    Py_TYPE(self)->tp_free((PyObject *)self);
}}

static PyObject *{py_name}_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {{
    {py_name}Object *self = ({py_name}Object *)type->tp_alloc(type, 0);
    if (self) {{
        self->handle = NULL;
        self->owned = 0;
    }}
    return (PyObject *)self;
}}

static int {py_name}_init({py_name}Object *self, PyObject *args, PyObject *kwds) {{
    PyErr_SetString(PyExc_TypeError, "Cannot create instances directly");
    return -1;
}}

static PyTypeObject {py_name}Type = {{
    PyVarObject_HEAD_INIT(NULL, 0)
    "{py_name}", sizeof({py_name}Object), 0, (destructor){py_name}_dealloc,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
    "Opaque handle to {handle.c_type}",
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    (initproc){py_name}_init, 0, {py_name}_new, 0,
}};

"""


def _list_prev_type(c_type: str) -> str | None:
    base = c_type.removesuffix("_t")
    if not base.endswith("_list"):
        return None
    return base.removesuffix("_list") + "_t"


def _list_leaf_type(c_type: str) -> str:
    current = c_type
    while True:
        prev = _list_prev_type(current)
        if prev is None:
            return current
        current = prev


def _is_sequence(struct: HostStructMetadata | None) -> bool:
    return struct is not None and struct.kind in {"sequence", "handle_sequence"}


def _render_sequence_converter(struct: HostStructMetadata) -> str:
    c_type = struct.c_type
    name = _snake_name(c_type)
    elem = (struct.element_type or "").removeprefix("const ").removesuffix("*").strip()
    destroy = struct.destroy_function or f"ifcopenshell_{name}_destroy"
    if struct.kind == "handle_sequence":
        item_py = _py_type_name(elem) if elem.endswith("_t") else None
        if struct.sequence_depth == 1 and item_py is not None:
            item = f"wrap_{_snake_name(elem)}(value->items[i], owned)"
            null_item = "        value->items[i] = NULL;\n"
        else:
            item = f"convert_{_snake_name(elem)}(&value->items[i], owned)"
            null_item = ""
    elif elem == "uint8_t":
        return f"""\
static PyObject *convert_{name}({c_type} *value, int owned) {{
    PyObject *result = PyBytes_FromStringAndSize((const char *)value->items, (Py_ssize_t)value->size);
    {destroy}(value);
    return result;
}}
"""
    elif elem in _LEAF_CONVERTERS:
        item = _LEAF_CONVERTERS[elem].format(expr="value->items[i]")
        null_item = ""
    else:
        item = f"convert_{_snake_name(elem)}(&value->items[i], owned)"
        null_item = ""
    return f"""\
static PyObject *convert_{name}({c_type} *value, int owned) {{
    PyObject *result = PyTuple_New((Py_ssize_t)value->size);
    if (!result) {{
        {destroy}(value);
        return NULL;
    }}
    for (size_t i = 0; i < value->size; ++i) {{
        PyObject *item = {item};
        if (!item) {{
            Py_DECREF(result);
            {destroy}(value);
            return NULL;
        }}
        PyTuple_SET_ITEM(result, (Py_ssize_t)i, item);
{null_item}    }}
    {destroy}(value);
    return result;
}}
"""


def _render_result_struct_converter(struct: HostStructMetadata, handles: dict[str, HostStructMetadata], owned: int = 1) -> str:
    assignments = []
    for index, field in enumerate(struct.fields):
        assignments.append(f"    item = {_convert_expr(field.c_type, f'value->{field.name}', handles, owned=owned)};")
        assignments.append("    if (!item) { Py_DECREF(result); return NULL; }")
        assignments.append(f"    PyTuple_SET_ITEM(result, {index}, item);")
    body = "\n".join(assignments)
    return f"""\
static PyObject *convert_{_snake_name(struct.c_type)}({struct.c_type} *value, int owned) {{
    PyObject *result = PyTuple_New({len(struct.fields)});
    if (!result) return NULL;
    PyObject *item = NULL;
{body}
    return result;
}}
"""


def _render_value_converters(metadata: HostBindingMetadata, handles: dict[str, HostStructMetadata]) -> str:
    sequences = sorted((s for s in metadata.value_types.values() if _is_sequence(s)), key=lambda s: s.sequence_depth)
    result_structs = [s for s in metadata.value_types.values() if s.kind == "result_struct"]
    converters = [
        """
static PyObject *convert_ifcopenshell_string_copy(ifcopenshell_string_t *value) {
    if (!value->data) Py_RETURN_NONE;
    return PyUnicode_FromStringAndSize(value->data, (Py_ssize_t)value->size);
}

static PyObject *convert_string(ifcopenshell_string_t *value);

static PyObject *convert_string(ifcopenshell_string_t *value) {
    PyObject *result = convert_ifcopenshell_string_copy(value);
    ifcopenshell_string_destroy(value);
    return result;
}
"""
    ]
    converters.extend(_render_sequence_converter(struct) for struct in sequences)
    converters.extend(_render_result_struct_converter(struct, handles) for struct in result_structs)
    return "\n".join(converters)


def _convert_expr(c_type: str, name: str, handles: dict[str, HostStructMetadata], *, owned: int = 1) -> str:
    normalized = _normalize_c_type(c_type)
    base, pointer_depth = _base_pointer_type(normalized)
    if pointer_depth == 0 and normalized in _SCALAR_DECLS:
        return _SCALAR_DECLS[normalized][2].format(name=name)
    if normalized == "ifcopenshell_string_t":
        return f"convert_string(&{name})"
    if pointer_depth == 1 and base in {h.c_type for h in handles.values()}:
        return f"wrap_{_snake_name(base)}({name}, {owned})"
    if pointer_depth == 0 and base in {h.c_type for h in handles.values()}:
        return f"wrap_{_snake_name(base)}(&{name}, {owned})"
    if pointer_depth > 0 and base == "void":
        return f"PyCapsule_New((void *){name}, NULL, NULL)"
    if pointer_depth > 0 and base in _SCALAR_DECLS:
        return f"PyCapsule_New((void *){name}, NULL, NULL)"
    if re.fullmatch(r"ifcopenshell_.*_list_t", base):
        return f"convert_{_snake_name(base)}(&{name}, {owned})"
    if re.fullmatch(r"ifcopenshell_.*_t", base):
        return f"convert_{_snake_name(base)}(&{name}, {owned})"
    raise ValueError(f"Unsupported C return type for extension backend: {c_type}")


def _render_wrap_handle(handle: HostStructMetadata) -> str:
    py_name = _py_type_name(handle.c_type)
    snake = _snake_name(handle.c_type)
    return f"""\
static PyObject *wrap_{snake}({handle.c_type} *handle, int owned) {{
    if (!handle) Py_RETURN_NONE;
    {py_name}Object *result = ({py_name}Object *){py_name}Type.tp_alloc(&{py_name}Type, 0);
    if (!result) return NULL;
    result->handle = handle;
    result->owned = owned;
    return (PyObject *)result;
}}
"""


def _render_input_sequence_helpers(metadata: HostBindingMetadata, handles: dict[str, HostStructMetadata]) -> str:
    helpers = []
    for struct in sorted((s for s in metadata.value_types.values() if _is_sequence(s)), key=lambda s: s.sequence_depth):
        helpers.append(_render_input_sequence_helper(struct, metadata, handles))
    return "\n".join(helpers)


def _render_input_sequence_helper(struct: HostStructMetadata, metadata: HostBindingMetadata, handles: dict[str, HostStructMetadata]) -> str:
    name = _snake_name(struct.c_type)
    elem = (struct.element_type or "").removeprefix("const ").removesuffix("*").strip()
    if struct.kind == "handle_sequence" and struct.sequence_depth == 1:
        py_type = _py_type_name(elem)
        read_item = f"""\
        PyObject *item_obj = PySequence_Fast_GET_ITEM(seq, (Py_ssize_t)i);
        if (!PyObject_TypeCheck(item_obj, &{py_type}Type)) {{
            PyErr_SetString(PyExc_TypeError, "Expected {py_type} item");
            free_input_{name}(out);
            Py_DECREF(seq);
            return 0;
        }}
        out->items[i] = ({elem} *)(({py_type}Object *)item_obj)->handle;"""
        item_type = f"{elem}*"
    elif struct.kind == "handle_sequence":
        read_item = f"""\
        if (!make_input_{_snake_name(elem)}(PySequence_Fast_GET_ITEM(seq, (Py_ssize_t)i), &out->items[i])) {{
            free_input_{name}(out);
            Py_DECREF(seq);
            return 0;
        }}"""
        item_type = elem
    elif elem == "ifcopenshell_string_t":
        read_item = """\
        Py_ssize_t size = 0;
        const char *data = PyUnicode_AsUTF8AndSize(PySequence_Fast_GET_ITEM(seq, (Py_ssize_t)i), &size);
        if (!data) {
            free_input_STRING_NAME(out);
            Py_DECREF(seq);
            return 0;
        }
        out->items[i].data = (char *)PyMem_Malloc((size_t)size + 1);
        if (!out->items[i].data) {
            free_input_STRING_NAME(out);
            Py_DECREF(seq);
            PyErr_NoMemory();
            return 0;
        }
        memcpy(out->items[i].data, data, (size_t)size + 1);
        out->items[i].size = (size_t)size;
        out->items[i].owned = true;""".replace("STRING_NAME", name)
        item_type = elem
    elif elem in _LEAF_READERS:
        read = _LEAF_READERS[elem].format(obj="PySequence_Fast_GET_ITEM(seq, (Py_ssize_t)i)")
        read_item = f"""\
        out->items[i] = {read};
        if (PyErr_Occurred()) {{
            free_input_{name}(out);
            Py_DECREF(seq);
            return 0;
        }}"""
        item_type = elem
    else:
        read_item = f"""\
        if (!make_input_{_snake_name(elem)}(PySequence_Fast_GET_ITEM(seq, (Py_ssize_t)i), &out->items[i])) {{
            free_input_{name}(out);
            Py_DECREF(seq);
            return 0;
        }}"""
        item_type = elem
    if elem == "ifcopenshell_string_t":
        free_body = f"""\
    if (value->items) {{
        for (size_t j = 0; j < value->size; ++j) {{
            if (value->items[j].owned) {{
                PyMem_Free(value->items[j].data);
            }}
        }}
    }}
    PyMem_Free(value->items);"""
    elif _is_sequence(metadata.value_types.get(_snake_name(elem))):
        free_body = f"""\
    if (value->items) {{
        for (size_t j = 0; j < value->size; ++j) {{
            free_input_{_snake_name(elem)}(&value->items[j]);
        }}
    }}
    PyMem_Free(value->items);"""
    else:
        free_body = "    PyMem_Free(value->items);"
    return f"""\
static void free_input_{name}({struct.c_type} *value) {{
{free_body}
    value->items = NULL;
    value->size = 0;
}}

static int make_input_{name}(PyObject *obj, {struct.c_type} *out) {{
    PyObject *seq = PySequence_Fast(obj, "Expected a sequence");
    if (!seq) return 0;
    Py_ssize_t size = PySequence_Fast_GET_SIZE(seq);
    out->items = size ? ({item_type} *)PyMem_Calloc((size_t)size, sizeof({item_type})) : NULL;
    out->size = (size_t)size;
    if (size && !out->items) {{
        Py_DECREF(seq);
        PyErr_NoMemory();
        return 0;
    }}
    for (size_t i = 0; i < (size_t)size; ++i) {{
{read_item}
    }}
    Py_DECREF(seq);
    return 1;
}}
"""


def _param_parse(param: HostParamMetadata, handles: dict[str, HostStructMetadata]) -> tuple[list[str], list[str], list[str], str]:
    c_type = _normalize_c_type(param.c_type)
    base, pointer_depth = _base_pointer_type(c_type)
    name = param.name
    declarations: list[str] = []
    parse_args: list[str] = []
    call_args: list[str] = []
    cleanup: list[str] = []
    fmt = ""
    if c_type in {"const char*", "char*"}:
        declarations.append(f"    const char *arg_{name} = NULL;")
        fmt = "z" if param.nullable else "s"
        parse_args.append(f"&arg_{name}")
        call_args.append(f"arg_{name}")
    elif c_type in _SCALAR_DECLS:
        decl, fmt, _ = _SCALAR_DECLS[c_type]
        declarations.append(f"    {decl} arg_{name} = 0;")
        parse_args.append(f"&arg_{name}")
        call_args.append(f"({c_type})arg_{name}" if c_type in {"ifcopenshell_logical_t", "size_t"} else f"arg_{name}")
    elif pointer_depth == 1 and base in {h.c_type for h in handles.values()}:
        py_name = _py_type_name(base)
        declarations.append(f"    PyObject *arg_{name}_obj = NULL;")
        fmt = "O" if param.nullable else "O!"
        if param.nullable:
            parse_args.append(f"&arg_{name}_obj")
            declarations.append(f"    {base} *arg_{name} = NULL;")
            call_args.append(f"arg_{name}")
        else:
            parse_args.extend([f"&{py_name}Type", f"&arg_{name}_obj"])
            call_args.append(f"({base} *)(({py_name}Object *)arg_{name}_obj)->handle")
    elif pointer_depth == 1 and re.fullmatch(r"ifcopenshell_.*_list_t", base):
        declarations.append(f"    PyObject *arg_{name}_obj = NULL;")
        declarations.append(f"    {base} arg_{name} = {{0}};")
        fmt = "O"
        parse_args.append(f"&arg_{name}_obj")
        call_args.append(f"&arg_{name}")
        cleanup.append(f"    free_input_{_snake_name(base)}(&arg_{name});")
    elif pointer_depth > 0 and base == "void":
        declarations.append(f"    PyObject *arg_{name}_obj = NULL;")
        fmt = "O"
        parse_args.append(f"&arg_{name}_obj")
        call_args.append(f"PyCapsule_IsValid(arg_{name}_obj, NULL) ? PyCapsule_GetPointer(arg_{name}_obj, NULL) : (void *)arg_{name}_obj")
    elif c_type in {"const double*", "const int32_t*"}:
        declarations.append(f"    Py_buffer arg_{name}_view = {{0}};")
        fmt = "y*"
        parse_args.append(f"&arg_{name}_view")
        call_args.append(f"({base} *)arg_{name}_view.buf")
        cleanup.append(f"    PyBuffer_Release(&arg_{name}_view);")
    else:
        raise ValueError(f"Unsupported C parameter type for extension backend: {param.c_type}")
    return declarations, parse_args, call_args, fmt, cleanup


def _render_function_wrapper(function: HostFunctionMetadata, metadata: HostBindingMetadata, handles: dict[str, HostStructMetadata]) -> str:
    declarations: list[str] = []
    parse_args: list[str] = []
    call_args: list[str] = []
    cleanup: list[str] = []
    format_parts: list[str] = []
    out_params = [p for p in function.params if p.role == "out_result"]
    out_param = out_params[0] if out_params else None
    optional_inserted = False
    for param in function.params:
        if param.role == "out_result":
            continue
        decl, args, calls, fmt, clean = _param_parse(param, handles)
        declarations.extend(decl)
        parse_args.extend(args)
        call_args.extend(calls)
        if param.nullable and not optional_inserted:
            format_parts.append("|")
            optional_inserted = True
        format_parts.append(fmt)
        cleanup.extend(clean)
    nullable_handle_assignments = []
    for param in function.params:
        if param.role == "out_result" or not param.nullable:
            continue
        base, pointer_depth = _base_pointer_type(param.c_type)
        if pointer_depth == 1 and base in {h.c_type for h in handles.values()}:
            py_name = _py_type_name(base)
            nullable_handle_assignments.append(
                f"    if (arg_{param.name}_obj != Py_None) {{\n"
                f"        if (!PyObject_TypeCheck(arg_{param.name}_obj, &{py_name}Type)) {{\n"
                f"            PyErr_SetString(PyExc_TypeError, \"Expected {py_name} or None\");\n"
                f"            goto __cleanup;\n"
                f"        }}\n"
                f"        arg_{param.name} = ({base} *)(({py_name}Object *)arg_{param.name}_obj)->handle;\n"
                f"    }}"
            )
    out_decl = ""
    result_assign = ""
    owned = 0 if function.returns.ownership in ("borrowed", "static") else 1
    if len(out_params) > 1:
        out_decls = []
        result_items = []
        for i, op in enumerate(out_params):
            out_base, out_depth = _base_pointer_type(op.c_type)
            if out_depth == 1:
                out_decls.append(f"    {_pointee_type(op.c_type)} result_{i} = {{0}};")
                call_args.append(f"&result_{i}")
                result_items.append(_convert_expr(out_base, f"result_{i}", handles, owned=owned))
            elif out_depth == 2:
                out_decls.append(f"    {_pointee_type(op.c_type)} *result_{i} = NULL;")
                call_args.append(f"&result_{i}")
                result_items.append(_convert_expr(out_base + "*", f"result_{i}", handles, owned=owned))
            else:
                raise ValueError(f"Unsupported out-result type for extension backend: {op.c_type}")
        out_decl = "\n".join(out_decls)
        tuple_items = []
        for i, expr in enumerate(result_items):
            tuple_items.append(f"    {{\n        PyObject *tmp = {expr};\n        if (!tmp) {{ Py_DECREF(__py_result); __py_result = NULL; goto __cleanup; }}\n        PyTuple_SET_ITEM(__py_result, {i}, tmp);\n    }}")
        result_assign = f"    __py_result = PyTuple_New({len(out_params)});\n    if (!__py_result) goto __cleanup;\n" + "\n".join(tuple_items)
    elif out_param is not None:
        out_base, out_depth = _base_pointer_type(out_param.c_type)
        if out_depth == 1:
            out_decl = f"    {_pointee_type(out_param.c_type)} result = {{0}};"
            call_args.append("&result")
            result_assign = f"    __py_result = {_convert_expr(out_base, 'result', handles, owned=owned)};"
        elif out_depth == 2:
            out_decl = f"    {_pointee_type(out_param.c_type)} *result = NULL;"
            call_args.append("&result")
            result_assign = f"    __py_result = {_convert_expr(out_base + '*', 'result', handles, owned=owned)};"
        else:
            raise ValueError(f"Unsupported out-result type for extension backend: {out_param.c_type}")
    else:
        result_assign = "    Py_INCREF(Py_None);\n    __py_result = Py_None;"
    fmt = "".join(format_parts)
    if parse_args:
        parse_block = f"    if (!PyArg_ParseTuple(args, \"{fmt}\", {', '.join(parse_args)})) return NULL;\n"
    else:
        parse_block = '    if (!PyArg_ParseTuple(args, "")) return NULL;\n'
    conversion_block = "\n".join(nullable_handle_assignments)
    input_make = []
    for param in function.params:
        if param.role == "out_result":
            continue
        base, pointer_depth = _base_pointer_type(param.c_type)
        if (
            pointer_depth == 1
            and re.fullmatch(r"ifcopenshell_.*_list_t", base)
            and f"    {base} arg_{param.name} = {{0}};" in declarations
        ):
            input_make.append(
                f"    if (!make_input_{_snake_name(base)}(arg_{param.name}_obj, &arg_{param.name})) {{\n"
                f"        goto __cleanup;\n"
                f"    }}"
            )
    input_make_block = "\n".join(input_make)
    cleanup_lines = list(reversed(cleanup))
    cleanup_block = "\n".join(cleanup_lines)
    if cleanup_block:
        cleanup_block = "    " + cleanup_block.replace("\n", "\n    ") + "\n"
    call = f"{function.c_name}({', '.join(call_args)})" if call_args else f"{function.c_name}()"
    return f"""\
static PyObject *{_wrapper_name(function.c_name)}(PyObject *self, PyObject *args) {{
    PyObject *__py_result = NULL;
    bool ok = false;
{chr(10).join(declarations)}
{out_decl}
{parse_block}{conversion_block}
{input_make_block}
    {metadata.error_functions['clear_error']}();
    ok = {call};
    if (!ok) {{
        raise_last_error("{function.c_name} failed");
        goto __cleanup;
    }}
{result_assign}
__cleanup:
{cleanup_block}    return __py_result;
}}
"""


def render_python_extension(metadata: HostBindingMetadata, api_header_path: Path | None = None) -> str:
    handles = {handle.c_type: handle for handle in metadata.handles.values()}
    if api_header_path is not None:
        discovered = {handle.c_type: handle for handle in _discover_all_handle_types(api_header_path).values()}
        discovered.update(handles)
        handles = discovered
    sorted_handles = sorted(handles.values(), key=lambda item: item.c_type)
    handle_decls = "\n".join(_render_handle_type_decl(handle) for handle in sorted_handles)
    wrap_decls = "\n".join(_render_wrap_handle(handle) for handle in sorted_handles)
    input_helpers = _render_input_sequence_helpers(metadata, handles)
    value_converters = _render_value_converters(metadata, handles)
    wrappers = "\n".join(_render_function_wrapper(function, metadata, handles) for function in metadata.functions.values())
    methods = "\n".join(
        f'    {{"{_method_name(function.c_name, metadata.c_prefix)}", {_wrapper_name(function.c_name)}, METH_VARARGS, "Wrap {function.c_name}"}},'
        for function in metadata.functions.values()
    )
    type_ready = "\n".join(f"    if (PyType_Ready(&{_py_type_name(h.c_type)}Type) < 0) return NULL;" for h in sorted_handles)
    add_types = "\n".join(
        f"    Py_INCREF(&{_py_type_name(h.c_type)}Type);\n"
        f"    if (PyModule_AddObject(m, \"{_py_type_name(h.c_type)}\", (PyObject *)&{_py_type_name(h.c_type)}Type) < 0) {{\n"
        f"        Py_DECREF(&{_py_type_name(h.c_type)}Type);\n"
        f"        return NULL;\n"
        f"    }}"
        for h in sorted_handles
    )
    return f"""\
// This file was generated by IfcOpenShell's binding generator. Do not edit manually.

#include <Python.h>
#include "ifcopenshell_api.h"

static PyObject *raise_last_error(const char *fallback) {{
    const char *msg = {metadata.error_functions['last_error_message']}();
    int kind = {metadata.error_functions['last_error_kind']}();
    PyObject *exc;
    switch (kind) {{
        case IFCOPENSHELL_ERROR_VALUE: exc = PyExc_ValueError; break;
        case IFCOPENSHELL_ERROR_TYPE: exc = PyExc_TypeError; break;
        case IFCOPENSHELL_ERROR_NOT_IMPLEMENTED: exc = PyExc_NotImplementedError; break;
        case IFCOPENSHELL_ERROR_KEY: exc = PyExc_KeyError; break;
        default: exc = PyExc_RuntimeError; break;
    }}
    PyErr_SetString(exc, (msg && msg[0]) ? msg : fallback);
    return NULL;
}}

{handle_decls}
{wrap_decls}
{input_helpers}
{value_converters}
{wrappers}

static PyObject *py_clear_error(PyObject *self, PyObject *args) {{
    {metadata.error_functions['clear_error']}();
    Py_RETURN_NONE;
}}

static PyObject *py_last_error_message(PyObject *self, PyObject *args) {{
    const char *msg = {metadata.error_functions['last_error_message']}();
    if (msg && msg[0]) return PyUnicode_FromString(msg);
    Py_RETURN_NONE;
}}

static PyObject *py_last_error_kind(PyObject *self, PyObject *args) {{
    return PyLong_FromLong({metadata.error_functions['last_error_kind']}());
}}

static PyMethodDef module_methods[] = {{
    {{"clear_error", py_clear_error, METH_NOARGS, "Clear the last error"}},
    {{"last_error_message", py_last_error_message, METH_NOARGS, "Get the last error message"}},
    {{"last_error_kind", py_last_error_kind, METH_NOARGS, "Get the last error kind"}},
{methods}
    {{NULL, NULL, 0, NULL}}
}};

static struct PyModuleDef moduledef = {{
    PyModuleDef_HEAD_INIT,
    "_ifcopenshell_capi",
    "CPython C extension wrapping the ifcopenshell C API",
    -1,
    module_methods,
    NULL,
    NULL,
    NULL,
    NULL
}};

PyMODINIT_FUNC PyInit__ifcopenshell_capi(void) {{
{type_ready}
    PyObject *m = PyModule_Create(&moduledef);
    if (!m) return NULL;
{add_types}
    PyModule_AddIntConstant(m, "IFCOPENSHELL_ERROR_NONE", 0);
    PyModule_AddIntConstant(m, "IFCOPENSHELL_ERROR_RUNTIME", 1);
    PyModule_AddIntConstant(m, "IFCOPENSHELL_ERROR_VALUE", 2);
    PyModule_AddIntConstant(m, "IFCOPENSHELL_ERROR_TYPE", 3);
    PyModule_AddIntConstant(m, "IFCOPENSHELL_ERROR_NOT_IMPLEMENTED", 4);
    PyModule_AddIntConstant(m, "IFCOPENSHELL_ERROR_KEY", 5);
    PyModule_AddIntConstant(m, "IFCOPENSHELL_LOGICAL_UNKNOWN", -1);
    PyModule_AddIntConstant(m, "IFCOPENSHELL_LOGICAL_FALSE", 0);
    PyModule_AddIntConstant(m, "IFCOPENSHELL_LOGICAL_TRUE", 1);
    return m;
}}
"""


def generate_python_extension(ir: BindingIR, output_path: Path, api_header_path: Path | None = None) -> None:
    output_path.parent.mkdir(parents=True, exist_ok=True)
    output_path.write_text(render_python_extension(build_host_metadata(ir), api_header_path=api_header_path), encoding="utf-8")
