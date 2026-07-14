# This file was generated with the assistance of an AI coding tool.

from __future__ import annotations

import re
from pathlib import Path

from ...abi_ir import BindingABI, CFunctionIR, COptionIR, CParamIR, CTypeIR
from .._shared import _method_name, _snake_name, _type_name

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
    return _type_name(c_type)


def _wrapper_name(c_name: str) -> str:
    return f"py_{c_name}"


def _destroy_method_name(c_type: str) -> str:
    name = _snake_name(c_type)
    for prefix in ("parse_", "geom_"):
        if name.startswith(prefix):
            name = name[len(prefix) :]
            break
    return f"{name}_destroy"


def _render_family_type_check(
    name: str, c_types: tuple[str, ...], handles: dict[str, CTypeIR]
) -> str:
    checks = [
        f"type == &{_py_type_name(c_type)}Type"
        for c_type in c_types
        if c_type in handles
    ]
    condition = " ||\n        ".join(checks) if checks else "0"
    return f"""\
static int is_{name}_family_type(PyTypeObject *type) {{
    return type && (
        {condition}
    );
}}"""


def _render_handle_family_helpers(handles: dict[str, CTypeIR]) -> str:
    declaration_types = (
        "ifcopenshell_declaration_t",
        "ifcopenshell_entity_t",
        "ifcopenshell_enumeration_t",
        "ifcopenshell_select_type_t",
        "ifcopenshell_type_declaration_t",
    )
    geom_element_types = (
        "ifcopenshell_geom_element_t",
        "ifcopenshell_geom_brep_element_t",
        "ifcopenshell_geom_triangulation_element_t",
        "ifcopenshell_geom_serialized_element_t",
    )
    return "\n\n".join(
        [
            _render_family_type_check("declaration", declaration_types, handles),
            _render_family_type_check("geom_element", geom_element_types, handles),
            """\
static int handle_types_are_compatible(PyTypeObject *actual, PyTypeObject *expected) {
    if (!actual || !expected) return 0;
    if (actual == expected) return 1;
    if (is_declaration_family_type(actual) && is_declaration_family_type(expected)) return 1;
    if (is_geom_element_family_type(actual) && is_geom_element_family_type(expected)) return 1;
    return 0;
}""",
        ]
    )


def _option_by_c_type(c_type: str, metadata: BindingABI) -> COptionIR | None:
    normalized = (
        _normalize_c_type(c_type).removeprefix("const ").removesuffix("*").strip()
    )
    return next(
        (
            option
            for option in metadata.option_structs.values()
            if option.c_type == normalized
        ),
        None,
    )


def _render_handle_type_decl(handle: CTypeIR) -> str:
    py_name = _py_type_name(handle.c_type)
    destroy = (
        handle.destroy_function or f"ifcopenshell_{_snake_name(handle.c_type)}_destroy"
    )
    sequence_support = ""
    type_sequence_field = ""
    if handle.c_type == "ifcopenshell_parse_instance_list_t":
        instance_py_name = _py_type_name("ifcopenshell_instance_t")
        sequence_support = f"""
static Py_ssize_t {py_name}_len(PyObject *self_obj) {{
    size_t result = 0;
    if (!ifcopenshell_parse_instance_list_size((ifcopenshell_parse_instance_list_t *)(({py_name}Object *)self_obj)->handle, &result)) {{
        raise_last_error("ifcopenshell_parse_instance_list_size failed");
        return -1;
    }}
    if (result > (size_t)PY_SSIZE_T_MAX) {{
        PyErr_SetString(PyExc_OverflowError, "instance list is too large");
        return -1;
    }}
    return (Py_ssize_t)result;
}}

static PyObject *{py_name}_item(PyObject *self_obj, Py_ssize_t index) {{
    Py_ssize_t size = {py_name}_len(self_obj);
    if (size < 0) return NULL;
    if (index < 0) index += size;
    if (index < 0 || index >= size) {{
        PyErr_SetString(PyExc_IndexError, "instance list index out of range");
        return NULL;
    }}
    ifcopenshell_instance_t *handle = NULL;
    if (!ifcopenshell_parse_instance_list_get((ifcopenshell_parse_instance_list_t *)(({py_name}Object *)self_obj)->handle, (size_t)index, &handle)) {{
        raise_last_error("ifcopenshell_parse_instance_list_get failed");
        return NULL;
    }}
    if (!handle) Py_RETURN_NONE;
    {instance_py_name}Object *result = ({instance_py_name}Object *){instance_py_name}Type.tp_alloc(&{instance_py_name}Type, 0);
    if (!result) {{
        ifcopenshell_instance_destroy(handle);
        return NULL;
    }}
    result->handle = handle;
    result->owned = 1;
    return (PyObject *)result;
}}

static PySequenceMethods {py_name}_sequence_methods = {{
    .sq_length = {py_name}_len,
    .sq_item = {py_name}_item,
}};
"""
        type_sequence_field = f"    .tp_as_sequence = &{py_name}_sequence_methods,\n"
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

static PyMemberDef {py_name}_members[] = {{
    {{"_handle_ptr", T_PYSSIZET, offsetof({py_name}Object, handle), READONLY, "Raw C pointer"}},
    {{"handle", T_PYSSIZET, offsetof({py_name}Object, handle), READONLY, "Raw C pointer"}},
    {{"owned", T_INT, offsetof({py_name}Object, owned), READONLY, "C handle envelope ownership flag"}},
    {{NULL}}
}};

{sequence_support}

static PyTypeObject {py_name}Type = {{
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "{py_name}",
    .tp_basicsize = sizeof({py_name}Object),
    .tp_dealloc = (destructor){py_name}_dealloc,
{type_sequence_field}    .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
    .tp_doc = "Opaque handle to {handle.c_type}",
    .tp_init = (initproc){py_name}_init,
    .tp_new = {py_name}_new,
    .tp_members = {py_name}_members,
}};

"""


def _render_destroy_wrapper(handle: CTypeIR) -> str:
    py_name = _py_type_name(handle.c_type)
    destroy = (
        handle.destroy_function or f"ifcopenshell_{_snake_name(handle.c_type)}_destroy"
    )
    wrapper = f"py_{_destroy_method_name(handle.c_type)}"
    return f"""\
static PyObject *{wrapper}(PyObject *self, PyObject *args) {{
    PyObject *arg_obj = NULL;
    if (!PyArg_ParseTuple(args, "O!", &{py_name}Type, &arg_obj)) return NULL;
    {py_name}Object *obj = ({py_name}Object *)arg_obj;
    if (obj->handle) {{
        {destroy}(({handle.c_type} *)obj->handle);
        obj->handle = NULL;
        obj->owned = 0;
    }}
    Py_RETURN_NONE;
}}
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


def _is_sequence(struct: CTypeIR | None) -> bool:
    return struct is not None and struct.kind in {"sequence", "handle_sequence"}


def _render_sequence_converter(struct: CTypeIR) -> str:
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


def _render_result_struct_converter(
    struct: CTypeIR, handles: dict[str, CTypeIR], owned: int = 1
) -> str:
    destroy = (
        struct.destroy_function or f"ifcopenshell_{_snake_name(struct.c_type)}_destroy"
    )
    assignments = []
    for field in struct.fields:
        normalized = _normalize_c_type(field.c_type)
        base, pointer_depth = _base_pointer_type(normalized)
        transfer = (
            f"    value->{field.name} = NULL;"
            if pointer_depth == 1 and base in {h.c_type for h in handles.values()}
            else ""
        )
        assignments.append(
            f"    item = {_convert_expr(field.c_type, f'value->{field.name}', handles, owned=owned)};"
        )
        if transfer:
            assignments.append(transfer)
        assignments.append(
            f"    if (!item) {{ Py_DECREF(result); {destroy}(value); return NULL; }}"
        )
        assignments.append(
            f'    if (PyObject_SetAttrString(result, "{field.name}", item) < 0) {{ Py_DECREF(item); Py_DECREF(result); {destroy}(value); return NULL; }}'
        )
        assignments.append("    Py_DECREF(item);")
    assignments.append(f"    {destroy}(value);")
    body = "\n".join(assignments)
    return f"""\
static PyObject *convert_{_snake_name(struct.c_type)}({struct.c_type} *value, int owned) {{
    if (!SimpleNamespaceType) {{
        {destroy}(value);
        PyErr_SetString(PyExc_RuntimeError, \"types.SimpleNamespace is not available\");
        return NULL;
    }}
    PyObject *result = PyObject_CallNoArgs(SimpleNamespaceType);
    if (!result) {{ {destroy}(value); return NULL; }}
    PyObject *item = NULL;
{body}
    return result;
}}
"""


def _render_optional_result_struct_converter(
    struct: CTypeIR,
    value_types: dict[str, CTypeIR],
    handles: dict[str, CTypeIR],
) -> str:
    payload = next(field for field in struct.fields if field.name == "value")
    payload_c_type = payload.c_type.removeprefix("const ").removesuffix("*").strip()
    payload_type = next(
        item for item in value_types.values() if item.c_type == payload_c_type
    )
    return f"""\
static PyObject *convert_{_snake_name(struct.c_type)}({struct.c_type} *value, int owned) {{
    if (!value->has_value) {{
        {struct.destroy_function or f"ifcopenshell_{_snake_name(struct.c_type)}_destroy"}(value);
        Py_RETURN_NONE;
    }}
    PyObject *result = convert_{_snake_name(payload_type.c_type)}(&value->value, owned);
    {struct.destroy_function or f"ifcopenshell_{_snake_name(struct.c_type)}_destroy"}(value);
    return result;
}}
"""


def _render_variant_converter(struct: CTypeIR, handles: dict[str, CTypeIR]) -> str:
    destroy = (
        struct.destroy_function or f"ifcopenshell_{_snake_name(struct.c_type)}_destroy"
    )
    branches = []
    for index, field in enumerate(
        field for field in struct.fields if field.name.startswith("value_")
    ):
        normalized = _normalize_c_type(field.c_type)
        base, pointer_depth = _base_pointer_type(normalized)
        transfer = (
            f"        value->{field.name} = NULL;"
            if pointer_depth == 1 and base in {h.c_type for h in handles.values()}
            else ""
        )
        branches.append(
            f"    if (value->kind == {index}) {{\n"
            f"        result = {_convert_expr(field.c_type, f'value->{field.name}', handles, owned=1)};\n"
            f"{transfer}\n"
            "    }"
        )
    body = "\n".join(branches)
    return f"""\
static PyObject *convert_{_snake_name(struct.c_type)}({struct.c_type} *value, int owned) {{
    (void)owned;
    PyObject *result = NULL;
{body}
    if (!result) {{
        if (!PyErr_Occurred()) PyErr_SetString(PyExc_RuntimeError, "Unsupported variant alternative");
        {destroy}(value);
        return NULL;
    }}
    {destroy}(value);
    return result;
}}
"""


def _render_value_converters(metadata: BindingABI, handles: dict[str, CTypeIR]) -> str:
    sequences = sorted(
        (s for s in metadata.value_types.values() if _is_sequence(s)),
        key=lambda s: s.sequence_depth,
    )
    result_structs = [
        s for s in metadata.value_types.values() if s.kind == "result_struct"
    ]
    optional_result_structs = [
        s for s in metadata.value_types.values() if s.kind == "optional_result_struct"
    ]
    variants = [s for s in metadata.value_types.values() if s.kind == "variant"]
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
    converters.extend(
        _render_result_struct_converter(struct, handles) for struct in result_structs
    )
    converters.extend(
        _render_optional_result_struct_converter(struct, metadata.value_types, handles)
        for struct in optional_result_structs
    )
    converters.extend(_render_variant_converter(struct, handles) for struct in variants)
    return "\n".join(converters)


def _convert_expr(
    c_type: str, name: str, handles: dict[str, CTypeIR], *, owned: int = 1
) -> str:
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


def _render_wrap_handle(handle: CTypeIR) -> str:
    py_name = _py_type_name(handle.c_type)
    snake = _snake_name(handle.c_type)
    destroy = (
        handle.destroy_function or f"ifcopenshell_{_snake_name(handle.c_type)}_destroy"
    )
    return f"""\
static PyObject *wrap_{snake}({handle.c_type} *handle, int owned) {{
    if (!handle) Py_RETURN_NONE;
    {py_name}Object *result = ({py_name}Object *){py_name}Type.tp_alloc(&{py_name}Type, 0);
    if (!result) {{
        {destroy}(handle);
        return NULL;
    }}
    result->handle = handle;
    result->owned = owned;
    return (PyObject *)result;
}}
"""


def _render_input_sequence_helpers(
    metadata: BindingABI, handles: dict[str, CTypeIR]
) -> str:
    helpers = []
    for struct in sorted(
        (s for s in metadata.value_types.values() if _is_sequence(s)),
        key=lambda s: s.sequence_depth,
    ):
        helpers.append(_render_input_sequence_helper(struct, metadata, handles))
    return "\n".join(helpers)


def _render_input_sequence_helper(
    struct: CTypeIR,
    metadata: BindingABI,
    handles: dict[str, CTypeIR],
) -> str:
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
        read = _LEAF_READERS[elem].format(
            obj="PySequence_Fast_GET_ITEM(seq, (Py_ssize_t)i)"
        )
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


def _render_option_field_assignment(
    option: COptionIR,
    field_index: int,
    handles: dict[str, CTypeIR],
) -> str:
    field = option.fields[field_index]
    field_ref = f"field_{field_index}"
    required = 0 if field.type.nullable else 1
    lines = [
        f'    PyObject *{field_ref} = get_option_field(obj, "{field.name}", {required});'
    ]
    if field.type.nullable:
        lines.extend(
            [
                f"    if (!{field_ref}) {{",
                "        if (PyErr_Occurred()) return 0;",
                "    } else {",
                f"        refs[{field_index}] = {field_ref};",
                f"        if ({field_ref} != Py_None) {{",
            ]
        )
        indent = "        "
    else:
        lines.extend(
            [
                f"    if (!{field_ref}) {{",
                "        return 0;",
                "    }",
                f"    refs[{field_index}] = {field_ref};",
            ]
        )
        indent = ""
    sequence_base, sequence_depth = _base_pointer_type(field.c_type)
    if (
        field.type.kind == "handle"
        and sequence_base == "ifcopenshell_parse_instance_list_t"
        and sequence_depth == 1
    ):
        lines.extend(
            [
                f"    {indent}ifcopenshell_instance_list_t {field.name}_items_{field_index} = {{0}};",
                f"    {indent}if (!make_input_instance_list({field_ref}, &{field.name}_items_{field_index})) {{",
                f"    {indent}    return 0;",
                f"    {indent}}}",
                f"    {indent}if (!ifcopenshell_parse_instance_list_create_from_handles(&{field.name}_items_{field_index}, &out->{field.name})) {{",
                f"    {indent}    free_input_instance_list(&{field.name}_items_{field_index});",
                f'    {indent}    raise_last_error("ifcopenshell_parse_instance_list_create_from_handles failed");',
                f"    {indent}    return 0;",
                f"    {indent}}}",
                f"    {indent}free_input_instance_list(&{field.name}_items_{field_index});",
            ]
        )
    elif field.type.sequence_depth > 0:
        sequence_name = _snake_name(sequence_base)
        lines.extend(
            [
                f"    {indent}{sequence_base} *sequence_{field_index} = ({sequence_base} *)PyMem_Calloc(1, sizeof({sequence_base}));",
                f"    {indent}if (!sequence_{field_index}) {{",
                f"    {indent}    PyErr_NoMemory();",
                f"    {indent}    return 0;",
                f"    {indent}}}",
                f"    {indent}if (!make_input_{sequence_name}({field_ref}, sequence_{field_index})) {{",
                f"    {indent}    PyMem_Free(sequence_{field_index});",
                f"    {indent}    return 0;",
                f"    {indent}}}",
                f"    {indent}out->{field.name} = sequence_{field_index};",
            ]
        )
    elif field.type.kind == "string":
        lines.extend(
            [
                f"    {indent}out->{field.name} = PyUnicode_AsUTF8({field_ref});",
                f"    {indent}if (!out->{field.name}) return 0;",
            ]
        )
    elif field.type.kind == "bool":
        lines.extend(
            [
                f"    {indent}int value_{field_index} = PyObject_IsTrue({field_ref});",
                f"    {indent}if (value_{field_index} < 0) return 0;",
                f"    {indent}out->{field.name} = ({field.c_type})value_{field_index};",
            ]
        )
    elif field.type.kind in {"int32", "uint32", "size"}:
        reader = (
            "PyLong_AsUnsignedLong" if field.type.kind == "uint32" else "PyLong_AsLong"
        )
        lines.extend(
            [
                f"    {indent}out->{field.name} = ({field.c_type}){reader}({field_ref});",
                f"    {indent}if (PyErr_Occurred()) return 0;",
            ]
        )
    elif field.type.kind == "int64":
        lines.extend(
            [
                f"    {indent}out->{field.name} = ({field.c_type})PyLong_AsLongLong({field_ref});",
                f"    {indent}if (PyErr_Occurred()) return 0;",
            ]
        )
    elif field.type.kind == "double":
        lines.extend(
            [
                f"    {indent}out->{field.name} = PyFloat_AsDouble({field_ref});",
                f"    {indent}if (PyErr_Occurred()) return 0;",
            ]
        )
    elif field.type.kind == "handle" and field.type.handle is not None:
        handle_c_type = field.c_type.removeprefix("const ").removesuffix("*").strip()
        handle = handles.get(handle_c_type)
        if handle is None:
            msg = f"Unsupported option handle field type for Python backend: {option.c_type}.{field.name}"
            raise ValueError(msg)
        py_name = _py_type_name(handle.c_type)
        lines.extend(
            [
                f'    {indent}if (!extract_handle({field_ref}, &{py_name}Type, "{py_name}", (void **)&out->{field.name}, 0)) {{',
                f"    {indent}    return 0;",
                f"    {indent}}}",
            ]
        )
    elif field.type.kind == "opaque_ptr":
        lines.extend(
            [
                f"    {indent}if (PyCapsule_IsValid({field_ref}, NULL)) {{",
                f"    {indent}    out->{field.name} = PyCapsule_GetPointer({field_ref}, NULL);",
                f"    {indent}}} else {{",
                f'    {indent}    PyErr_SetString(PyExc_TypeError, "Expected a capsule");',
                f"    {indent}    return 0;",
                f"    {indent}}}",
            ]
        )
    else:
        msg = f"Unsupported option field type for Python backend: {option.c_type}.{field.name}"
        raise ValueError(msg)
    if field.type.nullable:
        lines.extend(
            [
                f"        out->has_{field.name} = true;",
                "        }",
                "    }",
            ]
        )
    return "\n".join(lines)


def _render_option_free_helper(option: COptionIR) -> str:
    lines: list[str] = []
    for field in option.fields:
        base, pointer_depth = _base_pointer_type(field.c_type)
        if (
            field.type.kind == "handle"
            and base == "ifcopenshell_parse_instance_list_t"
            and pointer_depth == 1
        ):
            lines.extend(
                [
                    f"    if (value->{field.name}) {{",
                    f"        ifcopenshell_parse_instance_list_destroy(value->{field.name});",
                    f"        value->{field.name} = NULL;",
                    "    }",
                ]
            )
            continue
        if field.type.sequence_depth <= 0:
            continue
        sequence_name = _snake_name(base)
        lines.extend(
            [
                f"    if (value->{field.name}) {{",
                f"        free_input_{sequence_name}(({base} *)value->{field.name});",
                f"        PyMem_Free((void *)value->{field.name});",
                f"        value->{field.name} = NULL;",
                "    }",
            ]
        )
    body = "\n".join(lines) if lines else "    (void)value;"
    return f"""\
static void free_input_{_snake_name(option.c_type)}({option.c_type} *value) {{
{body}
}}
"""


def _render_option_input_helper(option: COptionIR, handles: dict[str, CTypeIR]) -> str:
    field_blocks = "\n".join(
        _render_option_field_assignment(option, index, handles)
        for index, _ in enumerate(option.fields)
    )
    return (
        _render_option_free_helper(option)
        + "\n"
        + f"""\
static int fill_input_{_snake_name(option.c_type)}(PyObject *obj, {option.c_type} *out, PyObject **refs) {{
    if (!PyMapping_Check(obj)) {{
        PyErr_SetString(PyExc_TypeError, "Expected an option mapping");
        return 0;
    }}
{field_blocks}
    return 1;
}}
"""
    )


def _render_option_input_helpers(
    metadata: BindingABI, handles: dict[str, CTypeIR]
) -> str:
    return "\n\n".join(
        _render_option_input_helper(option, handles)
        for option in sorted(
            metadata.option_structs.values(), key=lambda item: item.c_type
        )
    )


def _param_parse(
    param: CParamIR,
    metadata: BindingABI,
    handles: dict[str, CTypeIR],
) -> tuple[list[str], list[str], list[str], str, list[str], list[str]]:
    c_type = _normalize_c_type(param.c_type)
    base, pointer_depth = _base_pointer_type(c_type)
    name = param.name
    declarations: list[str] = []
    parse_args: list[str] = []
    call_args: list[str] = []
    cleanup: list[str] = []
    setup: list[str] = []
    fmt = ""
    if c_type in {"const char*", "char*"}:
        declarations.append(f"    const char *arg_{name} = NULL;")
        fmt = "z" if param.nullable else "s"
        parse_args.append(f"&arg_{name}")
        call_args.append(f"arg_{name}")
    elif (
        pointer_depth == 1
        and (option := _option_by_c_type(c_type, metadata)) is not None
    ):
        declarations.append(f"    PyObject *arg_{name}_obj = NULL;")
        declarations.append(f"    {option.c_type} arg_{name} = {{0}};")
        declarations.append(
            f"    PyObject *arg_{name}_refs[{len(option.fields)}] = {{0}};"
        )
        fmt = "O"
        parse_args.append(f"&arg_{name}_obj")
        call_args.append(f"&arg_{name}")
        setup.append(
            f"    if (!fill_input_{_snake_name(option.c_type)}(arg_{name}_obj, &arg_{name}, arg_{name}_refs)) {{\n"
            f"        goto __cleanup;\n"
            f"    }}"
        )
        cleanup.append(f"    free_input_{_snake_name(option.c_type)}(&arg_{name});")
        cleanup.append(
            f"    release_option_refs(arg_{name}_refs, {len(option.fields)});"
        )
    elif c_type in _SCALAR_DECLS:
        decl, fmt, _ = _SCALAR_DECLS[c_type]
        declarations.append(f"    {decl} arg_{name} = 0;")
        parse_args.append(f"&arg_{name}")
        call_args.append(
            f"({c_type})arg_{name}"
            if c_type in {"ifcopenshell_logical_t", "size_t"}
            else f"arg_{name}"
        )
    elif pointer_depth == 1 and base in {h.c_type for h in handles.values()}:
        py_name = _py_type_name(base)
        declarations.append(f"    PyObject *arg_{name}_obj = NULL;")
        if (
            base == "ifcopenshell_parse_instance_list_t"
            and not param.nullable
            and name != "self"
        ):
            declarations.append(
                f"    ifcopenshell_instance_list_t arg_{name}_items = {{0}};"
            )
            declarations.append(f"    {base} *arg_{name} = NULL;")
            fmt = "O"
            parse_args.append(f"&arg_{name}_obj")
            call_args.append(f"arg_{name}")
            cleanup.append(f"    ifcopenshell_parse_instance_list_destroy(arg_{name});")
            cleanup.append(f"    free_input_instance_list(&arg_{name}_items);")
        else:
            fmt = "O"
            parse_args.append(f"&arg_{name}_obj")
            declarations.append(f"    {base} *arg_{name} = NULL;")
            call_args.append(f"arg_{name}")
            setup.append(
                f'    if (!extract_handle(arg_{name}_obj, &{py_name}Type, "{py_name}", (void **)&arg_{name}, {1 if param.nullable else 0})) {{\n'
                f"        goto __cleanup;\n"
                f"    }}"
            )
    elif pointer_depth == 1 and re.fullmatch(r"ifcopenshell_.*_list_t", base):
        declarations.append(f"    PyObject *arg_{name}_obj = NULL;")
        declarations.append(f"    {base} arg_{name} = {{0}};")
        fmt = "O"
        parse_args.append(f"&arg_{name}_obj")
        call_args.append(f"&arg_{name}")
        cleanup.append(f"    free_input_{_snake_name(base)}(&arg_{name});")
    elif pointer_depth > 0 and base == "void":
        declarations.append(f"    PyObject *arg_{name}_obj = NULL;")
        declarations.append(f"    Py_buffer arg_{name}_view = {{0}};")
        declarations.append(f"    void *arg_{name} = NULL;")
        declarations.append(f"    int arg_{name}_has_view = 0;")
        fmt = "O"
        parse_args.append(f"&arg_{name}_obj")
        call_args.append(f"arg_{name}")
        setup.append(
            f"    if (PyCapsule_IsValid(arg_{name}_obj, NULL)) {{\n"
            f"        arg_{name} = PyCapsule_GetPointer(arg_{name}_obj, NULL);\n"
            f"    }} else if (PyObject_GetBuffer(arg_{name}_obj, &arg_{name}_view, PyBUF_SIMPLE) == 0) {{\n"
            f"        arg_{name} = arg_{name}_view.buf;\n"
            f"        arg_{name}_has_view = 1;\n"
            f"    }} else {{\n"
            f'        PyErr_SetString(PyExc_TypeError, "Expected a capsule or buffer-compatible object");\n'
            f"        goto __cleanup;\n"
            f"    }}"
        )
        cleanup.append(
            f"    if (arg_{name}_has_view) PyBuffer_Release(&arg_{name}_view);"
        )
    elif c_type in {"const double*", "const int32_t*"}:
        declarations.append(f"    Py_buffer arg_{name}_view = {{0}};")
        fmt = "y*"
        parse_args.append(f"&arg_{name}_view")
        call_args.append(f"({base} *)arg_{name}_view.buf")
        cleanup.append(f"    PyBuffer_Release(&arg_{name}_view);")
    else:
        raise ValueError(
            f"Unsupported C parameter type for extension backend: {param.c_type}"
        )
    return declarations, parse_args, call_args, fmt, cleanup, setup


def _render_function_wrapper(
    function: CFunctionIR,
    metadata: BindingABI,
    handles: dict[str, CTypeIR],
) -> str:
    declarations: list[str] = []
    parse_args: list[str] = []
    call_args: list[str] = []
    cleanup: list[str] = []
    setup: list[str] = []
    format_parts: list[str] = []
    out_params = [p for p in function.params if p.role == "out_result"]
    out_param = out_params[0] if out_params else None
    optional_inserted = False
    for param in function.params:
        if param.role == "out_result":
            continue
        decl, args, calls, fmt, clean, param_setup = _param_parse(
            param, metadata, handles
        )
        declarations.extend(decl)
        parse_args.extend(args)
        call_args.extend(calls)
        if param.nullable and not optional_inserted:
            format_parts.append("|")
            optional_inserted = True
        format_parts.append(fmt)
        cleanup.extend(clean)
        setup.extend(param_setup)
    null_result_check = ""
    if out_param is not None:
        out_base, out_depth = _base_pointer_type(out_param.c_type)
        if out_depth == 2:
            null_result_check = f"""    if (result == nullptr && {metadata.error_functions["last_error_kind"]}() != 0) {{
        raise_last_error("{function.c_name} failed");
        goto __cleanup;
    }}
"""
    # For void/status functions (no out_result), also check for errors set
    # without a failure return. Some C++ implementation functions catch
    # exceptions internally, set the error, but return normally, so ok==true
    # even though an error occurred.
    status_error_check = ""
    if out_param is None:
        status_error_check = f"""    if ({metadata.error_functions["last_error_kind"]}() != 0) {{
        raise_last_error("{function.c_name} failed");
        goto __cleanup;
    }}
"""
    out_decl = ""
    result_assign = ""
    # Every successful result envelope is allocated by the C ABI for this call.
    # The envelope's internal owned flag controls only pointee destruction.
    owned = 1
    if len(out_params) > 1:
        out_decls = []
        result_items = []
        for i, op in enumerate(out_params):
            out_base, out_depth = _base_pointer_type(op.c_type)
            if out_depth == 1:
                out_decls.append(f"    {_pointee_type(op.c_type)} result_{i} = {{0}};")
                call_args.append(f"&result_{i}")
                result_items.append(
                    _convert_expr(out_base, f"result_{i}", handles, owned=owned)
                )
            elif out_depth == 2:
                out_decls.append(f"    {_pointee_type(op.c_type)} *result_{i} = NULL;")
                call_args.append(f"&result_{i}")
                result_items.append(
                    _convert_expr(out_base + "*", f"result_{i}", handles, owned=owned)
                )
            else:
                raise ValueError(
                    f"Unsupported out-result type for extension backend: {op.c_type}"
                )
        out_decl = "\n".join(out_decls)
        tuple_items = []
        for i, expr in enumerate(result_items):
            tuple_items.append(
                f"    {{\n        PyObject *tmp = {expr};\n        if (!tmp) {{ Py_DECREF(__py_result); __py_result = NULL; goto __cleanup; }}\n        PyTuple_SET_ITEM(__py_result, {i}, tmp);\n    }}"
            )
        result_assign = (
            f"    __py_result = PyTuple_New({len(out_params)});\n    if (!__py_result) goto __cleanup;\n"
            + "\n".join(tuple_items)
        )
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
            raise ValueError(
                f"Unsupported out-result type for extension backend: {out_param.c_type}"
            )
    else:
        result_assign = "    Py_INCREF(Py_None);\n    __py_result = Py_None;"
    fmt = "".join(format_parts)
    if parse_args:
        parse_block = f'    if (!PyArg_ParseTuple(args, "{fmt}", {", ".join(parse_args)})) return NULL;\n'
    else:
        parse_block = '    if (!PyArg_ParseTuple(args, "")) return NULL;\n'
    conversion_block = ""
    setup_block = "\n".join(setup)
    input_make = []
    for param in function.params:
        if param.role == "out_result":
            continue
        base, pointer_depth = _base_pointer_type(param.c_type)
        if (
            f"    ifcopenshell_instance_list_t arg_{param.name}_items = {{0}};"
            in declarations
        ):
            input_make.append(
                f"    if (!make_input_instance_list(arg_{param.name}_obj, &arg_{param.name}_items)) {{\n"
                f"        goto __cleanup;\n"
                f"    }}\n"
                f"    if (!ifcopenshell_parse_instance_list_create_from_handles(&arg_{param.name}_items, &arg_{param.name})) {{\n"
                f'        raise_last_error("ifcopenshell_parse_instance_list_create_from_handles failed");\n'
                f"        goto __cleanup;\n"
                f"    }}"
            )
            continue
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
    call = (
        f"{function.c_name}({', '.join(call_args)})"
        if call_args
        else f"{function.c_name}()"
    )
    return f"""\
static PyObject *{_wrapper_name(function.c_name)}(PyObject *self, PyObject *args) {{
    PyObject *__py_result = NULL;
    bool ok = false;
{chr(10).join(declarations)}
{out_decl}
{parse_block}{conversion_block}
{setup_block}
{input_make_block}
    {metadata.error_functions["clear_error"]}();
    ok = {call};
    if (!ok) {{
        raise_last_error("{function.c_name} failed");
        goto __cleanup;
    }}
{status_error_check}{null_result_check}{result_assign}
__cleanup:
{cleanup_block}    return __py_result;
}}
"""


def render_python_extension(metadata: BindingABI) -> str:
    handles = {handle.c_type: handle for handle in metadata.handles.values()}
    functions = list(metadata.functions.values())
    sorted_handles = sorted(handles.values(), key=lambda item: item.c_type)
    handle_family_helpers = _render_handle_family_helpers(handles)
    handle_decls = "\n".join(
        _render_handle_type_decl(handle) for handle in sorted_handles
    )
    destroy_wrappers = "\n".join(
        _render_destroy_wrapper(handle) for handle in sorted_handles
    )
    wrap_decls = "\n".join(_render_wrap_handle(handle) for handle in sorted_handles)
    input_helpers = _render_input_sequence_helpers(metadata, handles)
    option_helpers = _render_option_input_helpers(metadata, handles)
    value_converters = _render_value_converters(metadata, handles)
    wrappers = "\n".join(
        _render_function_wrapper(function, metadata, handles) for function in functions
    )
    methods = "\n".join(
        [
            f'    {{"{_destroy_method_name(handle.c_type)}", py_{_destroy_method_name(handle.c_type)}, METH_VARARGS, "Destroy {handle.c_type}"}},'
            for handle in sorted_handles
        ]
        + [
            f'    {{"{_method_name(function.c_name, metadata.c_prefix)}", {_wrapper_name(function.c_name)}, METH_VARARGS, "Wrap {function.c_name}"}},'
            for function in functions
        ]
        + [
            f'    {{"{function.c_name}", {_wrapper_name(function.c_name)}, METH_VARARGS, "Wrap {function.c_name}"}},'
            for function in functions
        ]
    )
    type_ready = "\n".join(
        f"    if (PyType_Ready(&{_py_type_name(h.c_type)}Type) < 0) return NULL;"
        for h in sorted_handles
    )
    add_types = "\n".join(
        f"    Py_INCREF(&{_py_type_name(h.c_type)}Type);\n"
        f'    if (PyModule_AddObject(m, "{_py_type_name(h.c_type)}", (PyObject *)&{_py_type_name(h.c_type)}Type) < 0) {{\n'
        f"        Py_DECREF(&{_py_type_name(h.c_type)}Type);\n"
        f"        return NULL;\n"
        f"    }}"
        for h in sorted_handles
    )
    return f"""\
// This file was generated by IfcOpenShell's binding generator. Do not edit manually.

#include <Python.h>
#include <stddef.h>
#include <string.h>
#include "structmember.h"
#include "ifcopenshell_api.h"

static PyObject *raise_last_error(const char *fallback) {{
    const char *msg = {metadata.error_functions["last_error_message"]}();
    int kind = {metadata.error_functions["last_error_kind"]}();
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

typedef struct {{
    PyObject_HEAD
    void *handle;
    int owned;
}} IfcOpenshellGenericHandleObject;

static PyObject *SimpleNamespaceType = NULL;

{handle_decls}

{handle_family_helpers}

static int extract_handle(PyObject *obj, PyTypeObject *expected, const char *expected_name, void **out, int nullable) {{
    if (nullable && obj == Py_None) {{
        *out = NULL;
        return 1;
    }}
    if (!obj) {{
        PyErr_SetString(PyExc_TypeError, "Expected a handle object");
        return 0;
    }}
    if (!PyObject_TypeCheck(obj, expected) && !handle_types_are_compatible(Py_TYPE(obj), expected)) {{
        PyErr_Format(PyExc_TypeError, nullable ? "Expected %s or None" : "Expected %s", expected_name);
        return 0;
    }}
    *out = ((IfcOpenshellGenericHandleObject *)obj)->handle;
    return 1;
}}

static PyObject *get_option_field(PyObject *obj, const char *name, int required) {{
    PyObject *key = PyUnicode_FromString(name);
    if (!key) return NULL;
    PyObject *value = PyObject_GetItem(obj, key);
    Py_DECREF(key);
    if (value) return value;
    if (!PyErr_ExceptionMatches(PyExc_KeyError)) return NULL;
    PyErr_Clear();
    if (required) {{
        PyErr_Format(PyExc_KeyError, "Missing required option field '%s'", name);
    }}
    return NULL;
}}

static void release_option_refs(PyObject **refs, size_t count) {{
    for (size_t i = 0; i < count; ++i) {{
        Py_XDECREF(refs[i]);
    }}
}}

{destroy_wrappers}
{wrap_decls}
{input_helpers}
{option_helpers}
{value_converters}
{wrappers}

static PyObject *py_clear_error(PyObject *self, PyObject *args) {{
    {metadata.error_functions["clear_error"]}();
    Py_RETURN_NONE;
}}

static PyObject *py_last_error_message(PyObject *self, PyObject *args) {{
    const char *msg = {metadata.error_functions["last_error_message"]}();
    if (msg && msg[0]) return PyUnicode_FromString(msg);
    Py_RETURN_NONE;
}}

static PyObject *py_last_error_kind(PyObject *self, PyObject *args) {{
    return PyLong_FromLong({metadata.error_functions["last_error_kind"]}());
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
    PyObject *types_module = PyImport_ImportModule("types");
    if (!types_module) {{
        Py_DECREF(m);
        return NULL;
    }}
    SimpleNamespaceType = PyObject_GetAttrString(types_module, "SimpleNamespace");
    Py_DECREF(types_module);
    if (!SimpleNamespaceType) {{
        Py_DECREF(m);
        return NULL;
    }}
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


def render_capi_utils() -> str:
    return """\
# Auto-generated by python_extension_backend.py. Do not edit.
# SPDX-License-Identifier: LGPL-3.0-or-later

from __future__ import annotations

import ifcopenshell
from . import _ifcopenshell_capi as _capi


def file_handle(file):
    return file._handle


def instance_handle(entity):
    return entity._handle if entity is not None else None


def instance_list(entities):
    return [entity._handle for entity in entities]


def wrap_handle(file, handle):
    return ifcopenshell.entity_instance(file, handle) if handle else None


_ERROR_KIND_TO_EXC = {
    _capi.IFCOPENSHELL_ERROR_VALUE: ValueError,
    _capi.IFCOPENSHELL_ERROR_TYPE: TypeError,
    _capi.IFCOPENSHELL_ERROR_NOT_IMPLEMENTED: NotImplementedError,
    _capi.IFCOPENSHELL_ERROR_KEY: KeyError,
}


def raise_last_error(default_msg):
    msg = _capi.last_error_message() or default_msg
    kind = _capi.last_error_kind()
    raise _ERROR_KIND_TO_EXC.get(kind, RuntimeError)(msg)


def call_handle(file, fn_name, *args, nullable=False, message=None):
    fn = getattr(_capi, fn_name)
    handle = fn(*args)
    if handle:
        return ifcopenshell.entity_instance(file, handle)
    if nullable and _capi.last_error_kind() == _capi.IFCOPENSHELL_ERROR_NONE:
        return None
    raise_last_error(message or f"{fn_name} failed")


def call_status(fn_name, *args):
    getattr(_capi, fn_name)(*args)


def call_handle_list(file, fn_name, *args):
    handles = getattr(_capi, fn_name)(*args)
    return [ifcopenshell.entity_instance(file, h) for h in handles]


def unwrap_parameter_type(param_type):
    \"\"\"Unwrap a parameter_type wrapper to its inner named/simple/aggregation type.

    When the C API (or SWIG) returns a parameter_type that wraps a concrete
    (named, simple, or aggregation) type, this function tries each unwrap
    method and returns the first non-None concrete type.  If the input is
    already a concrete type (not a bare parameter_type), it is returned as-is.

    This replaces the duplicate pattern::

        if type(param_type) is parameter_type:
            param_type = param_type.as_named_type() or \\
                         param_type.as_simple_type() or \\
                         param_type.as_aggregation_type()
    \"\"\"
    from ifcopenshell import ifcopenshell_wrapper as _W

    if type(param_type) is not _W.parameter_type:
        return param_type
    inner = param_type.as_named_type()
    if inner is not None:
        return inner
    inner = param_type.as_simple_type()
    if inner is not None:
        return inner
    inner = param_type.as_aggregation_type()
    if inner is not None:
        return inner
    return param_type


def owner_context(file):
    import ifcopenshell.api.owner.settings

    return (
        None,
        ifcopenshell.api.owner.settings.get_user(file),
        ifcopenshell.api.owner.settings.get_application(file),
    )
"""
