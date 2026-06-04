# This file was generated with the assistance of an AI coding tool.

from __future__ import annotations

import re
from pathlib import Path

try:
    from .binding_ir import BindingIR
    from .host_metadata import (
        HostBindingMetadata,
        HostFunctionMetadata,
        HostStructMetadata,
        build_host_metadata,
    )
except ImportError:
    from binding_ir import BindingIR
    from host_metadata import (
        HostBindingMetadata,
        HostFunctionMetadata,
        HostStructMetadata,
        build_host_metadata,
    )


def _snake_name(c_type: str) -> str:
    return c_type.removeprefix("ifcopenshell_").removesuffix("_t")


def _py_type_name(c_type: str) -> str:
    name = c_type.removesuffix("_t")
    name = name.removeprefix("ifcopenshell_")
    parts = name.split("_")
    return "IfcOpenshell" + "".join(p.capitalize() for p in parts if p)


def _c_api_destroy_name(c_type: str) -> str:
    return f"ifcopenshell_{_snake_name(c_type)}_destroy"


def _is_handle_type(struct: HostStructMetadata) -> bool:
    return struct.kind == "handle"


def _is_value_sequence(struct: HostStructMetadata) -> bool:
    return struct.kind in ("sequence", "handle_sequence")


def _python_type_check_expr(handle: HostStructMetadata, py_type_name: str) -> str:
    return f"PyObject_TypeCheck(arg_{_snake_name(handle.c_type)}, &{py_type_name}Type)"


def _discover_all_handle_types(api_header_path: Path) -> dict[str, HostStructMetadata]:
    """Parse the C API header to discover all opaque handle types.

    Looks for ``typedef struct ifcopenshell_*_t ifcopenshell_*_t;`` lines.
    """
    header = api_header_path.read_text(encoding="utf-8")
    pattern = re.compile(r"^typedef\s+struct\s+(ifcopenshell_\w+_t)\s+\1\s*;$", re.MULTILINE)
    result: dict[str, HostStructMetadata] = {}
    for match in pattern.finditer(header):
        c_type = match.group(1)
        destroy_name = f"ifcopenshell_{_snake_name(c_type)}_destroy"
        result[_snake_name(c_type)] = HostStructMetadata(
            c_type=c_type,
            kind="handle",
            fields=(),
            destroy_function=destroy_name,
            element_type=None,
            layout="opaque",
        )
    return result


def _render_handle_type_decl(handle: HostStructMetadata, py_type_name: str) -> str:
    snake = _snake_name(handle.c_type)
    doc = f"Opaque handle to {handle.c_type}"
    return f"""\
typedef struct {{
    PyObject_HEAD
    void *handle;
    int owned;
}} {py_type_name}Object;

static void {py_type_name}_dealloc({py_type_name}Object *self) {{
    if (self->owned && self->handle) {{
        {_c_api_destroy_name(handle.c_type)}(({handle.c_type} *)self->handle);
    }}
    Py_TYPE(self)->tp_free((PyObject *)self);
}}

static PyObject *{py_type_name}_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {{
    {py_type_name}Object *self = ({py_type_name}Object *)type->tp_alloc(type, 0);
    if (self) {{
        self->handle = NULL;
        self->owned = 0;
    }}
    return (PyObject *)self;
}}

static int {py_type_name}_init({py_type_name}Object *self, PyObject *args, PyObject *kwds) {{
    PyErr_SetString(PyExc_TypeError, "Cannot create instances directly");
    return -1;
}}

static PyTypeObject {py_type_name}Type = {{
    PyVarObject_HEAD_INIT(NULL, 0)
    "{py_type_name}",                              /* tp_name */
    sizeof({py_type_name}Object),                  /* tp_basicsize */
    0,                                             /* tp_itemsize */
    (destructor){py_type_name}_dealloc,            /* tp_dealloc */
    0,                                             /* tp_vectorcall_offset */
    0,                                             /* tp_getattr */
    0,                                             /* tp_setattr */
    0,                                             /* tp_as_async */
    0,                                             /* tp_repr */
    0,                                             /* tp_as_number */
    0,                                             /* tp_as_sequence */
    0,                                             /* tp_as_mapping */
    0,                                             /* tp_hash */
    0,                                             /* tp_call */
    0,                                             /* tp_str */
    0,                                             /* tp_getattro */
    0,                                             /* tp_setattro */
    0,                                             /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,     /* tp_flags */
    "{doc}",                                       /* tp_doc */
    0,                                             /* tp_traverse */
    0,                                             /* tp_clear */
    0,                                             /* tp_richcompare */
    0,                                             /* tp_weaklistoffset */
    0,                                             /* tp_iter */
    0,                                             /* tp_iternext */
    0,                                             /* tp_methods */
    0,                                             /* tp_members */
    0,                                             /* tp_getset */
    0,                                             /* tp_base */
    0,                                             /* tp_dict */
    0,                                             /* tp_descr_get */
    0,                                             /* tp_descr_set */
    0,                                             /* tp_dictoffset */
    (initproc){py_type_name}_init,                 /* tp_init */
    0,                                             /* tp_alloc */
    {py_type_name}_new,                            /* tp_new */
    0,                                             /* tp_free */
}};

"""



def _render_value_converters() -> str:
    return r"""
static PyObject *convert_ifcopenshell_string(ifcopenshell_string_t *value) {
    if (!value->data) {
        ifcopenshell_string_destroy(value);
        Py_RETURN_NONE;
    }
    PyObject *result = PyUnicode_FromStringAndSize(value->data, (Py_ssize_t)value->size);
    ifcopenshell_string_destroy(value);
    return result;
}

static PyObject *convert_ifcopenshell_string_list(ifcopenshell_string_list_t *value) {
    if (!value->items || value->size == 0) {
        ifcopenshell_string_list_destroy(value);
        return PyTuple_New(0);
    }
    PyObject *result = PyTuple_New((Py_ssize_t)value->size);
    if (!result) {
        ifcopenshell_string_list_destroy(value);
        return NULL;
    }
    for (size_t i = 0; i < value->size; i++) {
        ifcopenshell_string_t item = value->items[i];
        PyObject *s;
        if (item.data && item.size) {
            s = PyUnicode_FromStringAndSize(item.data, (Py_ssize_t)item.size);
        } else {
            s = PyUnicode_FromStringAndSize("", 0);
        }
        if (!s) {
            Py_DECREF(result);
            ifcopenshell_string_list_destroy(value);
            return NULL;
        }
        PyTuple_SET_ITEM(result, (Py_ssize_t)i, s);
    }
    ifcopenshell_string_list_destroy(value);
    return result;
}

static PyObject *convert_ifcopenshell_int32_list(ifcopenshell_int32_list_t *value) {
    if (!value->items || value->size == 0) {
        ifcopenshell_int32_list_destroy(value);
        return PyTuple_New(0);
    }
    PyObject *result = PyTuple_New((Py_ssize_t)value->size);
    if (!result) {
        ifcopenshell_int32_list_destroy(value);
        return NULL;
    }
    for (size_t i = 0; i < value->size; i++) {
        PyObject *n = PyLong_FromLong(value->items[i]);
        if (!n) {
            Py_DECREF(result);
            ifcopenshell_int32_list_destroy(value);
            return NULL;
        }
        PyTuple_SET_ITEM(result, (Py_ssize_t)i, n);
    }
    ifcopenshell_int32_list_destroy(value);
    return result;
}

static PyObject *convert_ifcopenshell_uint32_list(ifcopenshell_uint32_list_t *value) {
    if (!value->items || value->size == 0) {
        ifcopenshell_uint32_list_destroy(value);
        return PyTuple_New(0);
    }
    PyObject *result = PyTuple_New((Py_ssize_t)value->size);
    if (!result) {
        ifcopenshell_uint32_list_destroy(value);
        return NULL;
    }
    for (size_t i = 0; i < value->size; i++) {
        PyObject *n = PyLong_FromUnsignedLong(value->items[i]);
        if (!n) {
            Py_DECREF(result);
            ifcopenshell_uint32_list_destroy(value);
            return NULL;
        }
        PyTuple_SET_ITEM(result, (Py_ssize_t)i, n);
    }
    ifcopenshell_uint32_list_destroy(value);
    return result;
}

static PyObject *convert_ifcopenshell_int64_list(ifcopenshell_int64_list_t *value) {
    if (!value->items || value->size == 0) {
        ifcopenshell_int64_list_destroy(value);
        return PyTuple_New(0);
    }
    PyObject *result = PyTuple_New((Py_ssize_t)value->size);
    if (!result) {
        ifcopenshell_int64_list_destroy(value);
        return NULL;
    }
    for (size_t i = 0; i < value->size; i++) {
        PyObject *n = PyLong_FromLongLong(value->items[i]);
        if (!n) {
            Py_DECREF(result);
            ifcopenshell_int64_list_destroy(value);
            return NULL;
        }
        PyTuple_SET_ITEM(result, (Py_ssize_t)i, n);
    }
    ifcopenshell_int64_list_destroy(value);
    return result;
}

static PyObject *convert_ifcopenshell_double_list(ifcopenshell_double_list_t *value) {
    if (!value->items || value->size == 0) {
        ifcopenshell_double_list_destroy(value);
        return PyTuple_New(0);
    }
    PyObject *result = PyTuple_New((Py_ssize_t)value->size);
    if (!result) {
        ifcopenshell_double_list_destroy(value);
        return NULL;
    }
    for (size_t i = 0; i < value->size; i++) {
        PyObject *n = PyFloat_FromDouble(value->items[i]);
        if (!n) {
            Py_DECREF(result);
            ifcopenshell_double_list_destroy(value);
            return NULL;
        }
        PyTuple_SET_ITEM(result, (Py_ssize_t)i, n);
    }
    ifcopenshell_double_list_destroy(value);
    return result;
}

static PyObject *convert_ifcopenshell_bool_list(ifcopenshell_bool_list_t *value) {
    if (!value->items || value->size == 0) {
        ifcopenshell_bool_list_destroy(value);
        return PyTuple_New(0);
    }
    PyObject *result = PyTuple_New((Py_ssize_t)value->size);
    if (!result) {
        ifcopenshell_bool_list_destroy(value);
        return NULL;
    }
    for (size_t i = 0; i < value->size; i++) {
        PyObject *b = PyBool_FromLong(value->items[i]);
        PyTuple_SET_ITEM(result, (Py_ssize_t)i, b);
    }
    ifcopenshell_bool_list_destroy(value);
    return result;
}

static PyObject *convert_ifcopenshell_double_list_list(ifcopenshell_double_list_list_t *value) {
    if (!value->items || value->size == 0) {
        ifcopenshell_double_list_list_destroy(value);
        return PyTuple_New(0);
    }
    PyObject *result = PyTuple_New((Py_ssize_t)value->size);
    if (!result) {
        ifcopenshell_double_list_list_destroy(value);
        return NULL;
    }
    for (size_t i = 0; i < value->size; i++) {
        ifcopenshell_double_list_t inner = value->items[i];
        PyObject *inner_tuple;
        if (inner.items && inner.size) {
            inner_tuple = PyTuple_New((Py_ssize_t)inner.size);
            if (!inner_tuple) {
                Py_DECREF(result);
                ifcopenshell_double_list_list_destroy(value);
                return NULL;
            }
            for (size_t j = 0; j < inner.size; j++) {
                PyObject *n = PyFloat_FromDouble(inner.items[j]);
                if (!n) {
                    Py_DECREF(inner_tuple);
                    Py_DECREF(result);
                    ifcopenshell_double_list_list_destroy(value);
                    return NULL;
                }
                PyTuple_SET_ITEM(inner_tuple, (Py_ssize_t)j, n);
            }
        } else {
            inner_tuple = PyTuple_New(0);
        }
        PyTuple_SET_ITEM(result, (Py_ssize_t)i, inner_tuple);
    }
    ifcopenshell_double_list_list_destroy(value);
    return result;
}

static PyObject *convert_ifcopenshell_int32_list_list(ifcopenshell_int32_list_list_t *value) {
    if (!value->items || value->size == 0) {
        ifcopenshell_int32_list_list_destroy(value);
        return PyTuple_New(0);
    }
    PyObject *result = PyTuple_New((Py_ssize_t)value->size);
    if (!result) {
        ifcopenshell_int32_list_list_destroy(value);
        return NULL;
    }
    for (size_t i = 0; i < value->size; i++) {
        ifcopenshell_int32_list_t inner = value->items[i];
        PyObject *inner_tuple;
        if (inner.items && inner.size) {
            inner_tuple = PyTuple_New((Py_ssize_t)inner.size);
            if (!inner_tuple) {
                Py_DECREF(result);
                ifcopenshell_int32_list_list_destroy(value);
                return NULL;
            }
            for (size_t j = 0; j < inner.size; j++) {
                PyObject *n = PyLong_FromLong(inner.items[j]);
                if (!n) {
                    Py_DECREF(inner_tuple);
                    Py_DECREF(result);
                    ifcopenshell_int32_list_list_destroy(value);
                    return NULL;
                }
                PyTuple_SET_ITEM(inner_tuple, (Py_ssize_t)j, n);
            }
        } else {
            inner_tuple = PyTuple_New(0);
        }
        PyTuple_SET_ITEM(result, (Py_ssize_t)i, inner_tuple);
    }
    ifcopenshell_int32_list_list_destroy(value);
    return result;
}

static PyObject *convert_ifcopenshell_uint8_list(ifcopenshell_uint8_list_t *value) {
    if (!value->items || value->size == 0) {
        ifcopenshell_uint8_list_destroy(value);
        return PyBytes_FromStringAndSize("", 0);
    }
    PyObject *result = PyBytes_FromStringAndSize((const char *)value->items, (Py_ssize_t)value->size);
    ifcopenshell_uint8_list_destroy(value);
    return result;
}"""


def _render_subset_function_wrappers(metadata: HostBindingMetadata) -> str:
    file_handle = metadata.handles.get("ifc_file")
    instance_handle = metadata.handles.get("ifc_instance")
    if not file_handle or not instance_handle:
        return ""

    file_py = _py_type_name(file_handle.c_type)
    inst_py = _py_type_name(instance_handle.c_type)

    return f"""\
static PyObject *py_ifcopenshell_clear_error(PyObject *self, PyObject *args) {{
    ifcopenshell_clear_error();
    Py_RETURN_NONE;
}}

static PyObject *py_ifcopenshell_last_error_message(PyObject *self, PyObject *args) {{
    const char *msg = ifcopenshell_last_error_message();
    if (msg && msg[0])
        return PyUnicode_FromString(msg);
    Py_RETURN_NONE;
}}

static PyObject *py_ifcopenshell_last_error_kind(PyObject *self, PyObject *args) {{
    return PyLong_FromLong(ifcopenshell_last_error_kind());
}}

static PyObject *py_ifcopenshell_ifcparse_open(PyObject *self, PyObject *args) {{
    const char *path;
    int readonly = 0;
    if (!PyArg_ParseTuple(args, "s|p", &path, &readonly))
        return NULL;
    ifcopenshell_clear_error();
    {file_handle.c_type} *handle = NULL;
    if (!ifcopenshell_ifcparse_open(path, readonly, &handle)) {{
        return raise_last_error("Failed to open IFC file");
    }}
    {file_py}Object *result = ({file_py}Object *){file_py}Type.tp_alloc(&{file_py}Type, 0);
    if (!result) {{
        ifcopenshell_ifc_file_destroy(handle);
        return NULL;
    }}
    result->handle = (void *)handle;
    result->owned = 1;
    return (PyObject *)result;
}}

static PyObject *py_ifcopenshell_ifc_instance_destroy(PyObject *self, PyObject *args) {{
    PyObject *obj;
    if (!PyArg_ParseTuple(args, "O", &obj))
        return NULL;
    if (!PyObject_TypeCheck(obj, &{inst_py}Type)) {{
        PyErr_SetString(PyExc_TypeError, "Expected an {inst_py} handle");
        return NULL;
    }}
    {inst_py}Object *h = ({inst_py}Object *)obj;
    if (h->owned && h->handle) {{
        ifcopenshell_ifc_instance_destroy(({instance_handle.c_type} *)h->handle);
        h->handle = NULL;
        h->owned = 0;
    }}
    Py_RETURN_NONE;
}}

static PyObject *py_ifcopenshell_ifc_file_destroy(PyObject *self, PyObject *args) {{
    PyObject *obj;
    if (!PyArg_ParseTuple(args, "O", &obj))
        return NULL;
    if (!PyObject_TypeCheck(obj, &{file_py}Type)) {{
        PyErr_SetString(PyExc_TypeError, "Expected an {file_py} handle");
        return NULL;
    }}
    {file_py}Object *h = ({file_py}Object *)obj;
    if (h->owned && h->handle) {{
        ifcopenshell_ifc_file_destroy(({file_handle.c_type} *)h->handle);
        h->handle = NULL;
        h->owned = 0;
    }}
    Py_RETURN_NONE;
}}

static PyObject *py_ifcopenshell_ifc_file_by_id(PyObject *self, PyObject *args) {{
    PyObject *file_obj;
    int instance_id;
    if (!PyArg_ParseTuple(args, "Oi", &file_obj, &instance_id))
        return NULL;
    if (!PyObject_TypeCheck(file_obj, &{file_py}Type)) {{
        PyErr_SetString(PyExc_TypeError, "Expected an {file_py} handle");
        return NULL;
    }}
    {file_py}Object *fh = ({file_py}Object *)file_obj;
    ifcopenshell_clear_error();
    {instance_handle.c_type} *inst = NULL;
    if (!ifcopenshell_ifc_file_by_id(({file_handle.c_type} *)fh->handle, instance_id, &inst)) {{
        return raise_last_error("Failed to get instance by id");
    }}
    {inst_py}Object *result = ({inst_py}Object *){inst_py}Type.tp_alloc(&{inst_py}Type, 0);
    if (!result) {{
        ifcopenshell_ifc_instance_destroy(inst);
        return NULL;
    }}
    result->handle = (void *)inst;
    result->owned = 1;
    return (PyObject *)result;
}}

static PyObject *py_ifcopenshell_ifc_instance_id(PyObject *self, PyObject *args) {{
    PyObject *inst_obj;
    if (!PyArg_ParseTuple(args, "O", &inst_obj))
        return NULL;
    if (!PyObject_TypeCheck(inst_obj, &{inst_py}Type)) {{
        PyErr_SetString(PyExc_TypeError, "Expected an {inst_py} handle");
        return NULL;
    }}
    {inst_py}Object *ih = ({inst_py}Object *)inst_obj;
    ifcopenshell_clear_error();
    uint32_t id = 0;
    if (!ifcopenshell_ifc_instance_id(({instance_handle.c_type} *)ih->handle, &id)) {{
        return raise_last_error("Failed to get instance id");
    }}
    return PyLong_FromUnsignedLong(id);
}}

static PyObject *py_ifcopenshell_ifcparse_version(PyObject *self, PyObject *args) {{
    ifcopenshell_clear_error();
    ifcopenshell_string_t value = {{0}};
    if (!ifcopenshell_ifcparse_version(&value)) {{
        return raise_last_error("Failed to get version");
    }}
    return convert_ifcopenshell_string(&value);
}}

static PyObject *py_ifcopenshell_ifc_file_by_type(PyObject *self, PyObject *args) {{
    PyObject *file_obj;
    const char *type_name;
    if (!PyArg_ParseTuple(args, "Os", &file_obj, &type_name))
        return NULL;
    if (!PyObject_TypeCheck(file_obj, &{file_py}Type)) {{
        PyErr_SetString(PyExc_TypeError, "Expected an {file_py} handle");
        return NULL;
    }}
    {file_py}Object *fh = ({file_py}Object *)file_obj;
    ifcopenshell_clear_error();
    ifcopenshell_ifcparse_instance_list_t *list = NULL;
    if (!ifcopenshell_ifc_file_by_type(({file_handle.c_type} *)fh->handle, type_name, &list)) {{
        return raise_last_error("Failed to get instances by type");
    }}
    if (!list) {{
        return PyTuple_New(0);
    }}
    size_t list_size = 0;
    if (!ifcopenshell_ifcparse_instance_list_size(list, &list_size)) {{
        ifcopenshell_ifcparse_instance_list_destroy(list);
        return raise_last_error("Failed to get instance list size");
    }}
    if (list_size == 0) {{
        ifcopenshell_ifcparse_instance_list_destroy(list);
        return PyTuple_New(0);
    }}
    PyObject *result = PyTuple_New((Py_ssize_t)list_size);
    if (!result) {{
        ifcopenshell_ifcparse_instance_list_destroy(list);
        return NULL;
    }}
    for (size_t i = 0; i < list_size; i++) {{
        ifcopenshell_ifc_instance_t *inst = NULL;
        if (!ifcopenshell_ifcparse_instance_list_get(list, i, &inst)) {{
            Py_DECREF(result);
            ifcopenshell_ifcparse_instance_list_destroy(list);
            return raise_last_error("Failed to get instance from list");
        }}
        {inst_py}Object *item = ({inst_py}Object *){inst_py}Type.tp_alloc(&{inst_py}Type, 0);
        if (!item) {{
            if (inst) ifcopenshell_ifc_instance_destroy(inst);
            Py_DECREF(result);
            ifcopenshell_ifcparse_instance_list_destroy(list);
            return NULL;
        }}
        item->handle = (void *)inst;
        item->owned = 1;
        PyTuple_SET_ITEM(result, (Py_ssize_t)i, (PyObject *)item);
    }}
    ifcopenshell_ifcparse_instance_list_destroy(list);
    return result;
}}

static PyObject *py_ifcopenshell_ifcparse_schema_names(PyObject *self, PyObject *args) {{
    ifcopenshell_clear_error();
    ifcopenshell_string_list_t value = {{0}};
    if (!ifcopenshell_ifcparse_schema_names(&value)) {{
        return raise_last_error("Failed to get schema names");
    }}
    return convert_ifcopenshell_string_list(&value);
}}

static PyObject *py_ifcopenshell_ifc_file_by_guid(PyObject *self, PyObject *args) {{
    PyObject *file_obj;
    const char *global_id;
    if (!PyArg_ParseTuple(args, "Os", &file_obj, &global_id))
        return NULL;
    if (!PyObject_TypeCheck(file_obj, &{file_py}Type)) {{
        PyErr_SetString(PyExc_TypeError, "Expected an {file_py} handle");
        return NULL;
    }}
    {file_py}Object *fh = ({file_py}Object *)file_obj;
    ifcopenshell_clear_error();
    {instance_handle.c_type} *inst = NULL;
    if (!ifcopenshell_ifc_file_by_guid(({file_handle.c_type} *)fh->handle, global_id, &inst)) {{
        return raise_last_error("Failed to get instance by guid");
    }}
    if (!inst) {{
        Py_RETURN_NONE;
    }}
    {inst_py}Object *result = ({inst_py}Object *){inst_py}Type.tp_alloc(&{inst_py}Type, 0);
    if (!result) {{
        ifcopenshell_ifc_instance_destroy(inst);
        return NULL;
    }}
    result->handle = (void *)inst;
    result->owned = 1;
    return (PyObject *)result;
}}

static PyObject *py_ifcopenshell_ifc_instance_declaration(PyObject *self, PyObject *args) {{
    PyObject *inst_obj;
    if (!PyArg_ParseTuple(args, "O", &inst_obj))
        return NULL;
    if (!PyObject_TypeCheck(inst_obj, &{inst_py}Type)) {{
        PyErr_SetString(PyExc_TypeError, "Expected an {inst_py} handle");
        return NULL;
    }}
    {inst_py}Object *ih = ({inst_py}Object *)inst_obj;
    ifcopenshell_clear_error();
    ifcopenshell_ifc_declaration_t *decl = NULL;
    if (!ifcopenshell_ifc_instance_declaration(({instance_handle.c_type} *)ih->handle, &decl)) {{
        return raise_last_error("Failed to get declaration");
    }}
    if (!decl) {{
        Py_RETURN_NONE;
    }}
    IfcOpenshellIfcDeclarationObject *result = (IfcOpenshellIfcDeclarationObject *)IfcOpenshellIfcDeclarationType.tp_alloc(&IfcOpenshellIfcDeclarationType, 0);
    if (!result) {{
        ifcopenshell_ifc_declaration_destroy(decl);
        return NULL;
    }}
    result->handle = (void *)decl;
    result->owned = 1;
    return (PyObject *)result;
}}

static PyObject *py_ifcopenshell_ifc_declaration_name(PyObject *self, PyObject *args) {{
    PyObject *decl_obj;
    if (!PyArg_ParseTuple(args, "O", &decl_obj))
        return NULL;
    if (!PyObject_TypeCheck(decl_obj, &IfcOpenshellIfcDeclarationType)) {{
        PyErr_SetString(PyExc_TypeError, "Expected an IfcOpenshellIfcDeclaration handle");
        return NULL;
    }}
    IfcOpenshellIfcDeclarationObject *dh = (IfcOpenshellIfcDeclarationObject *)decl_obj;
    ifcopenshell_clear_error();
    ifcopenshell_string_t value = {{0}};
    if (!ifcopenshell_ifc_declaration_name((ifcopenshell_ifc_declaration_t *)dh->handle, &value)) {{
        return raise_last_error("Failed to get declaration name");
    }}
    return convert_ifcopenshell_string(&value);
}}

static PyObject *py_ifcopenshell_ifcgeom_create_settings(PyObject *self, PyObject *args) {{
    ifcopenshell_clear_error();
    ifcopenshell_ifcgeom_settings_t *settings = NULL;
    if (!ifcopenshell_ifcgeom_create_settings(&settings)) {{
        return raise_last_error("Failed to create geometry settings");
    }}
    IfcOpenshellIfcgeomSettingsObject *result = (IfcOpenshellIfcgeomSettingsObject *)IfcOpenshellIfcgeomSettingsType.tp_alloc(&IfcOpenshellIfcgeomSettingsType, 0);
    if (!result) {{
        ifcopenshell_ifcgeom_settings_destroy(settings);
        return NULL;
    }}
    result->handle = (void *)settings;
    result->owned = 1;
    return (PyObject *)result;
}}

static PyObject *py_ifcopenshell_ifcgeom_create_tree_from_file(PyObject *self, PyObject *args) {{
    PyObject *file_obj;
    if (!PyArg_ParseTuple(args, "O", &file_obj))
        return NULL;
    if (!PyObject_TypeCheck(file_obj, &{file_py}Type)) {{
        PyErr_SetString(PyExc_TypeError, "Expected an {file_py} handle");
        return NULL;
    }}
    {file_py}Object *fh = ({file_py}Object *)file_obj;
    ifcopenshell_clear_error();
    ifcopenshell_ifcgeom_tree_t *tree = NULL;
    if (!ifcopenshell_ifcgeom_create_tree_from_file(({file_handle.c_type} *)fh->handle, &tree)) {{
        return raise_last_error("Failed to create geometry tree");
    }}
    IfcOpenshellIfcgeomTreeObject *result = (IfcOpenshellIfcgeomTreeObject *)IfcOpenshellIfcgeomTreeType.tp_alloc(&IfcOpenshellIfcgeomTreeType, 0);
    if (!result) {{
        ifcopenshell_ifcgeom_tree_destroy(tree);
        return NULL;
    }}
    result->handle = (void *)tree;
    result->owned = 1;
    return (PyObject *)result;
}}

static PyObject *py_ifcopenshell_ifcgeom_tree_destroy(PyObject *self, PyObject *args) {{
    PyObject *obj;
    if (!PyArg_ParseTuple(args, "O", &obj))
        return NULL;
    if (!PyObject_TypeCheck(obj, &IfcOpenshellIfcgeomTreeType)) {{
        PyErr_SetString(PyExc_TypeError, "Expected an IfcOpenshellIfcgeomTree handle");
        return NULL;
    }}
    IfcOpenshellIfcgeomTreeObject *h = (IfcOpenshellIfcgeomTreeObject *)obj;
    if (h->owned && h->handle) {{
        ifcopenshell_ifcgeom_tree_destroy((ifcopenshell_ifcgeom_tree_t *)h->handle);
        h->handle = NULL;
        h->owned = 0;
    }}
    Py_RETURN_NONE;
}}

static PyObject *py_ifcopenshell_ifcapi_guid_new(PyObject *self, PyObject *args) {{
    ifcopenshell_clear_error();
    ifcopenshell_string_t value = {{0}};
    if (!ifcopenshell_ifcapi_guid_new(&value)) {{
        return raise_last_error("Failed to generate GUID");
    }}
    return convert_ifcopenshell_string(&value);
}}

static PyObject *py_ifcopenshell_ifcapi_root_create_entity(PyObject *self, PyObject *args) {{
    PyObject *file_obj;
    const char *ifc_class;
    const char *predefined_type = NULL;
    const char *name = NULL;
    PyObject *owner_history_obj = NULL;
    if (!PyArg_ParseTuple(args, "Os|zzO", &file_obj, &ifc_class, &predefined_type, &name, &owner_history_obj))
        return NULL;
    if (!PyObject_TypeCheck(file_obj, &{file_py}Type)) {{
        PyErr_SetString(PyExc_TypeError, "Expected an {file_py} handle");
        return NULL;
    }}
    {file_py}Object *fh = ({file_py}Object *)file_obj;
    ifcopenshell_ifc_instance_t *oh = NULL;
    if (owner_history_obj && owner_history_obj != Py_None) {{
        if (!PyObject_TypeCheck(owner_history_obj, &{inst_py}Type)) {{
            PyErr_SetString(PyExc_TypeError, "Expected an {inst_py} handle for owner_history");
            return NULL;
        }}
        oh = (ifcopenshell_ifc_instance_t *)(({inst_py}Object *)owner_history_obj)->handle;
    }}
    ifcopenshell_clear_error();
    ifcopenshell_ifc_instance_t *inst = NULL;
    if (!ifcopenshell_ifcapi_root_create_entity(({file_handle.c_type} *)fh->handle, ifc_class, predefined_type, name, oh, &inst)) {{
        return raise_last_error("Failed to create entity");
    }}
    if (!inst) {{
        Py_RETURN_NONE;
    }}
    {inst_py}Object *result = ({inst_py}Object *){inst_py}Type.tp_alloc(&{inst_py}Type, 0);
    if (!result) {{
        ifcopenshell_ifc_instance_destroy(inst);
        return NULL;
    }}
    result->handle = (void *)inst;
    result->owned = 1;
    return (PyObject *)result;
}}

static PyObject *py_ifcopenshell_ifc_instance_get_argument(PyObject *self, PyObject *args) {{
    PyObject *inst_obj;
    Py_ssize_t index;
    if (!PyArg_ParseTuple(args, "On", &inst_obj, &index))
        return NULL;
    if (!PyObject_TypeCheck(inst_obj, &{inst_py}Type)) {{
        PyErr_SetString(PyExc_TypeError, "Expected an {inst_py} handle");
        return NULL;
    }}
    {inst_py}Object *ih = ({inst_py}Object *)inst_obj;
    ifcopenshell_clear_error();
    ifcopenshell_ifcparse_attribute_value_t *attr = NULL;
    if (!ifcopenshell_ifc_instance_get_argument(({instance_handle.c_type} *)ih->handle, (size_t)index, &attr)) {{
        return raise_last_error("Failed to get argument");
    }}
    if (!attr) {{
        Py_RETURN_NONE;
    }}
    IfcOpenshellIfcparseAttributeValueObject *result = (IfcOpenshellIfcparseAttributeValueObject *)IfcOpenshellIfcparseAttributeValueType.tp_alloc(&IfcOpenshellIfcparseAttributeValueType, 0);
    if (!result) {{
        ifcopenshell_ifcparse_attribute_value_destroy(attr);
        return NULL;
    }}
    result->handle = (void *)attr;
    result->owned = 1;
    return (PyObject *)result;
}}

static PyObject *py_ifcopenshell_ifcparse_attribute_value_destroy(PyObject *self, PyObject *args) {{
    PyObject *obj;
    if (!PyArg_ParseTuple(args, "O", &obj))
        return NULL;
    if (!PyObject_TypeCheck(obj, &IfcOpenshellIfcparseAttributeValueType)) {{
        PyErr_SetString(PyExc_TypeError, "Expected an IfcOpenshellIfcparseAttributeValue handle");
        return NULL;
    }}
    IfcOpenshellIfcparseAttributeValueObject *h = (IfcOpenshellIfcparseAttributeValueObject *)obj;
    if (h->owned && h->handle) {{
        ifcopenshell_ifcparse_attribute_value_destroy((ifcopenshell_ifcparse_attribute_value_t *)h->handle);
        h->handle = NULL;
        h->owned = 0;
    }}
    Py_RETURN_NONE;
}}

static PyObject *py_ifcopenshell_ifcparse_attribute_value_is_null(PyObject *self, PyObject *args) {{
    PyObject *attr_obj;
    if (!PyArg_ParseTuple(args, "O", &attr_obj))
        return NULL;
    if (!PyObject_TypeCheck(attr_obj, &IfcOpenshellIfcparseAttributeValueType)) {{
        PyErr_SetString(PyExc_TypeError, "Expected an IfcOpenshellIfcparseAttributeValue handle");
        return NULL;
    }}
    IfcOpenshellIfcparseAttributeValueObject *ah = (IfcOpenshellIfcparseAttributeValueObject *)attr_obj;
    ifcopenshell_clear_error();
    bool is_null = false;
    if (!ifcopenshell_ifcparse_attribute_value_is_null((ifcopenshell_ifcparse_attribute_value_t *)ah->handle, &is_null)) {{
        return raise_last_error("Failed to check null");
    }}
    return PyBool_FromLong(is_null);
}}

static PyObject *py_ifcopenshell_ifcparse_attribute_value_type(PyObject *self, PyObject *args) {{
    PyObject *attr_obj;
    if (!PyArg_ParseTuple(args, "O", &attr_obj))
        return NULL;
    if (!PyObject_TypeCheck(attr_obj, &IfcOpenshellIfcparseAttributeValueType)) {{
        PyErr_SetString(PyExc_TypeError, "Expected an IfcOpenshellIfcparseAttributeValue handle");
        return NULL;
    }}
    IfcOpenshellIfcparseAttributeValueObject *ah = (IfcOpenshellIfcparseAttributeValueObject *)attr_obj;
    ifcopenshell_clear_error();
    ifcopenshell_string_t value = {{0}};
    if (!ifcopenshell_ifcparse_attribute_value_type((ifcopenshell_ifcparse_attribute_value_t *)ah->handle, &value)) {{
        return raise_last_error("Failed to get attribute type");
    }}
    return convert_ifcopenshell_string(&value);
}}

static PyObject *py_ifcopenshell_ifcparse_attribute_value_as_string(PyObject *self, PyObject *args) {{
    PyObject *attr_obj;
    if (!PyArg_ParseTuple(args, "O", &attr_obj))
        return NULL;
    if (!PyObject_TypeCheck(attr_obj, &IfcOpenshellIfcparseAttributeValueType)) {{
        PyErr_SetString(PyExc_TypeError, "Expected an IfcOpenshellIfcparseAttributeValue handle");
        return NULL;
    }}
    IfcOpenshellIfcparseAttributeValueObject *ah = (IfcOpenshellIfcparseAttributeValueObject *)attr_obj;
    ifcopenshell_clear_error();
    ifcopenshell_string_t value = {{0}};
    if (!ifcopenshell_ifcparse_attribute_value_as_string((ifcopenshell_ifcparse_attribute_value_t *)ah->handle, &value)) {{
        return raise_last_error("Failed to get attribute as string");
    }}
    return convert_ifcopenshell_string(&value);
}}

static PyObject *py_ifcopenshell_ifcparse_attribute_value_as_double(PyObject *self, PyObject *args) {{
    PyObject *attr_obj;
    if (!PyArg_ParseTuple(args, "O", &attr_obj))
        return NULL;
    if (!PyObject_TypeCheck(attr_obj, &IfcOpenshellIfcparseAttributeValueType)) {{
        PyErr_SetString(PyExc_TypeError, "Expected an IfcOpenshellIfcparseAttributeValue handle");
        return NULL;
    }}
    IfcOpenshellIfcparseAttributeValueObject *ah = (IfcOpenshellIfcparseAttributeValueObject *)attr_obj;
    ifcopenshell_clear_error();
    double value = 0;
    if (!ifcopenshell_ifcparse_attribute_value_as_double((ifcopenshell_ifcparse_attribute_value_t *)ah->handle, &value)) {{
        return raise_last_error("Failed to get attribute as double");
    }}
    return PyFloat_FromDouble(value);
}}

static PyObject *py_ifcopenshell_ifcparse_attribute_value_as_int32(PyObject *self, PyObject *args) {{
    PyObject *attr_obj;
    if (!PyArg_ParseTuple(args, "O", &attr_obj))
        return NULL;
    if (!PyObject_TypeCheck(attr_obj, &IfcOpenshellIfcparseAttributeValueType)) {{
        PyErr_SetString(PyExc_TypeError, "Expected an IfcOpenshellIfcparseAttributeValue handle");
        return NULL;
    }}
    IfcOpenshellIfcparseAttributeValueObject *ah = (IfcOpenshellIfcparseAttributeValueObject *)attr_obj;
    ifcopenshell_clear_error();
    int32_t value = 0;
    if (!ifcopenshell_ifcparse_attribute_value_as_int32((ifcopenshell_ifcparse_attribute_value_t *)ah->handle, &value)) {{
        return raise_last_error("Failed to get attribute as int32");
    }}
    return PyLong_FromLong(value);
}}

static PyObject *py_ifcopenshell_ifcparse_attribute_value_as_bool(PyObject *self, PyObject *args) {{
    PyObject *attr_obj;
    if (!PyArg_ParseTuple(args, "O", &attr_obj))
        return NULL;
    if (!PyObject_TypeCheck(attr_obj, &IfcOpenshellIfcparseAttributeValueType)) {{
        PyErr_SetString(PyExc_TypeError, "Expected an IfcOpenshellIfcparseAttributeValue handle");
        return NULL;
    }}
    IfcOpenshellIfcparseAttributeValueObject *ah = (IfcOpenshellIfcparseAttributeValueObject *)attr_obj;
    ifcopenshell_clear_error();
    bool value = false;
    if (!ifcopenshell_ifcparse_attribute_value_as_bool((ifcopenshell_ifcparse_attribute_value_t *)ah->handle, &value)) {{
        return raise_last_error("Failed to get attribute as bool");
    }}
    return PyBool_FromLong(value);
}}

static PyObject *py_ifcopenshell_ifcparse_attribute_value_as_instance(PyObject *self, PyObject *args) {{
    PyObject *attr_obj;
    if (!PyArg_ParseTuple(args, "O", &attr_obj))
        return NULL;
    if (!PyObject_TypeCheck(attr_obj, &IfcOpenshellIfcparseAttributeValueType)) {{
        PyErr_SetString(PyExc_TypeError, "Expected an IfcOpenshellIfcparseAttributeValue handle");
        return NULL;
    }}
    IfcOpenshellIfcparseAttributeValueObject *ah = (IfcOpenshellIfcparseAttributeValueObject *)attr_obj;
    ifcopenshell_clear_error();
    ifcopenshell_ifc_instance_t *inst = NULL;
    if (!ifcopenshell_ifcparse_attribute_value_as_instance((ifcopenshell_ifcparse_attribute_value_t *)ah->handle, &inst)) {{
        return raise_last_error("Failed to get attribute as instance");
    }}
    if (!inst) {{
        Py_RETURN_NONE;
    }}
    {inst_py}Object *result = ({inst_py}Object *){inst_py}Type.tp_alloc(&{inst_py}Type, 0);
    if (!result) {{
        ifcopenshell_ifc_instance_destroy(inst);
        return NULL;
    }}
    result->handle = (void *)inst;
    result->owned = 1;
    return (PyObject *)result;
}}

static PyObject *py_ifcopenshell_ifcparse_attribute_value_as_instance_list(PyObject *self, PyObject *args) {{
    PyObject *attr_obj;
    if (!PyArg_ParseTuple(args, "O", &attr_obj))
        return NULL;
    if (!PyObject_TypeCheck(attr_obj, &IfcOpenshellIfcparseAttributeValueType)) {{
        PyErr_SetString(PyExc_TypeError, "Expected an IfcOpenshellIfcparseAttributeValue handle");
        return NULL;
    }}
    IfcOpenshellIfcparseAttributeValueObject *ah = (IfcOpenshellIfcparseAttributeValueObject *)attr_obj;
    ifcopenshell_clear_error();
    ifcopenshell_ifcparse_instance_list_t *list = NULL;
    if (!ifcopenshell_ifcparse_attribute_value_as_instance_list((ifcopenshell_ifcparse_attribute_value_t *)ah->handle, &list)) {{
        return raise_last_error("Failed to get attribute as instance list");
    }}
    if (!list) {{
        return PyTuple_New(0);
    }}
    size_t list_size = 0;
    if (!ifcopenshell_ifcparse_instance_list_size(list, &list_size)) {{
        ifcopenshell_ifcparse_instance_list_destroy(list);
        return raise_last_error("Failed to get instance list size");
    }}
    if (list_size == 0) {{
        ifcopenshell_ifcparse_instance_list_destroy(list);
        return PyTuple_New(0);
    }}
    PyObject *result = PyTuple_New((Py_ssize_t)list_size);
    if (!result) {{
        ifcopenshell_ifcparse_instance_list_destroy(list);
        return NULL;
    }}
    for (size_t i = 0; i < list_size; i++) {{
        ifcopenshell_ifc_instance_t *inst = NULL;
        if (!ifcopenshell_ifcparse_instance_list_get(list, i, &inst)) {{
            Py_DECREF(result);
            ifcopenshell_ifcparse_instance_list_destroy(list);
            return raise_last_error("Failed to get instance from list");
        }}
        {inst_py}Object *item = ({inst_py}Object *){inst_py}Type.tp_alloc(&{inst_py}Type, 0);
        if (!item) {{
            if (inst) ifcopenshell_ifc_instance_destroy(inst);
            Py_DECREF(result);
            ifcopenshell_ifcparse_instance_list_destroy(list);
            return NULL;
        }}
        item->handle = (void *)inst;
        item->owned = 1;
        PyTuple_SET_ITEM(result, (Py_ssize_t)i, (PyObject *)item);
    }}
    ifcopenshell_ifcparse_instance_list_destroy(list);
    return result;
}}

static PyObject *py_ifcopenshell_ifcparse_attribute_value_as_string_list(PyObject *self, PyObject *args) {{
    PyObject *attr_obj;
    if (!PyArg_ParseTuple(args, "O", &attr_obj))
        return NULL;
    if (!PyObject_TypeCheck(attr_obj, &IfcOpenshellIfcparseAttributeValueType)) {{
        PyErr_SetString(PyExc_TypeError, "Expected an IfcOpenshellIfcparseAttributeValue handle");
        return NULL;
    }}
    IfcOpenshellIfcparseAttributeValueObject *ah = (IfcOpenshellIfcparseAttributeValueObject *)attr_obj;
    ifcopenshell_clear_error();
    ifcopenshell_string_list_t value = {{0}};
    if (!ifcopenshell_ifcparse_attribute_value_as_string_list((ifcopenshell_ifcparse_attribute_value_t *)ah->handle, &value)) {{
        return raise_last_error("Failed to get attribute as string list");
    }}
    return convert_ifcopenshell_string_list(&value);
}}

static PyObject *py_ifcopenshell_ifcparse_attribute_value_as_double_list(PyObject *self, PyObject *args) {{
    PyObject *attr_obj;
    if (!PyArg_ParseTuple(args, "O", &attr_obj))
        return NULL;
    if (!PyObject_TypeCheck(attr_obj, &IfcOpenshellIfcparseAttributeValueType)) {{
        PyErr_SetString(PyExc_TypeError, "Expected an IfcOpenshellIfcparseAttributeValue handle");
        return NULL;
    }}
    IfcOpenshellIfcparseAttributeValueObject *ah = (IfcOpenshellIfcparseAttributeValueObject *)attr_obj;
    ifcopenshell_clear_error();
    ifcopenshell_double_list_t value = {{0}};
    if (!ifcopenshell_ifcparse_attribute_value_as_double_list((ifcopenshell_ifcparse_attribute_value_t *)ah->handle, &value)) {{
        return raise_last_error("Failed to get attribute as double list");
    }}
    return convert_ifcopenshell_double_list(&value);
}}

static PyObject *py_ifcopenshell_ifcparse_attribute_value_as_double_list_list(PyObject *self, PyObject *args) {{
    PyObject *attr_obj;
    if (!PyArg_ParseTuple(args, "O", &attr_obj))
        return NULL;
    if (!PyObject_TypeCheck(attr_obj, &IfcOpenshellIfcparseAttributeValueType)) {{
        PyErr_SetString(PyExc_TypeError, "Expected an IfcOpenshellIfcparseAttributeValue handle");
        return NULL;
    }}
    IfcOpenshellIfcparseAttributeValueObject *ah = (IfcOpenshellIfcparseAttributeValueObject *)attr_obj;
    ifcopenshell_clear_error();
    ifcopenshell_double_list_list_t value = {{0}};
    if (!ifcopenshell_ifcparse_attribute_value_as_double_list_list((ifcopenshell_ifcparse_attribute_value_t *)ah->handle, &value)) {{
        return raise_last_error("Failed to get attribute as double list list");
    }}
    return convert_ifcopenshell_double_list_list(&value);
}}

static PyObject *py_ifcopenshell_ifcparse_attribute_value_as_int32_list(PyObject *self, PyObject *args) {{
    PyObject *attr_obj;
    if (!PyArg_ParseTuple(args, "O", &attr_obj))
        return NULL;
    if (!PyObject_TypeCheck(attr_obj, &IfcOpenshellIfcparseAttributeValueType)) {{
        PyErr_SetString(PyExc_TypeError, "Expected an IfcOpenshellIfcparseAttributeValue handle");
        return NULL;
    }}
    IfcOpenshellIfcparseAttributeValueObject *ah = (IfcOpenshellIfcparseAttributeValueObject *)attr_obj;
    ifcopenshell_clear_error();
    ifcopenshell_int32_list_t value = {{0}};
    if (!ifcopenshell_ifcparse_attribute_value_as_int32_list((ifcopenshell_ifcparse_attribute_value_t *)ah->handle, &value)) {{
        return raise_last_error("Failed to get attribute as int32 list");
    }}
    return convert_ifcopenshell_int32_list(&value);
}}

static PyObject *py_ifcopenshell_ifcparse_attribute_value_as_int32_list_list(PyObject *self, PyObject *args) {{
    PyObject *attr_obj;
    if (!PyArg_ParseTuple(args, "O", &attr_obj))
        return NULL;
    if (!PyObject_TypeCheck(attr_obj, &IfcOpenshellIfcparseAttributeValueType)) {{
        PyErr_SetString(PyExc_TypeError, "Expected an IfcOpenshellIfcparseAttributeValue handle");
        return NULL;
    }}
    IfcOpenshellIfcparseAttributeValueObject *ah = (IfcOpenshellIfcparseAttributeValueObject *)attr_obj;
    ifcopenshell_clear_error();
    ifcopenshell_int32_list_list_t value = {{0}};
    if (!ifcopenshell_ifcparse_attribute_value_as_int32_list_list((ifcopenshell_ifcparse_attribute_value_t *)ah->handle, &value)) {{
        return raise_last_error("Failed to get attribute as int32 list list");
    }}
    return convert_ifcopenshell_int32_list_list(&value);
}}

static PyObject *py_ifcopenshell_ifcparse_attribute_value_as_enumeration_value(PyObject *self, PyObject *args) {{
    PyObject *attr_obj;
    if (!PyArg_ParseTuple(args, "O", &attr_obj))
        return NULL;
    if (!PyObject_TypeCheck(attr_obj, &IfcOpenshellIfcparseAttributeValueType)) {{
        PyErr_SetString(PyExc_TypeError, "Expected an IfcOpenshellIfcparseAttributeValue handle");
        return NULL;
    }}
    IfcOpenshellIfcparseAttributeValueObject *ah = (IfcOpenshellIfcparseAttributeValueObject *)attr_obj;
    ifcopenshell_clear_error();
    ifcopenshell_string_t value = {{0}};
    if (!ifcopenshell_ifcparse_attribute_value_as_enumeration_value((ifcopenshell_ifcparse_attribute_value_t *)ah->handle, &value)) {{
        return raise_last_error("Failed to get attribute as enumeration value");
    }}
    return convert_ifcopenshell_string(&value);
}}

static PyObject *py_ifcopenshell_ifcparse_instance_list_size(PyObject *self, PyObject *args) {{
    PyObject *list_obj;
    if (!PyArg_ParseTuple(args, "O", &list_obj))
        return NULL;
    if (!PyObject_TypeCheck(list_obj, &IfcOpenshellIfcparseInstanceListType)) {{
        PyErr_SetString(PyExc_TypeError, "Expected an IfcOpenshellIfcparseInstanceList handle");
        return NULL;
    }}
    IfcOpenshellIfcparseInstanceListObject *lh = (IfcOpenshellIfcparseInstanceListObject *)list_obj;
    ifcopenshell_clear_error();
    size_t size = 0;
    if (!ifcopenshell_ifcparse_instance_list_size((ifcopenshell_ifcparse_instance_list_t *)lh->handle, &size)) {{
        return raise_last_error("Failed to get instance list size");
    }}
    return PyLong_FromSize_t(size);
}}

static PyObject *py_ifcopenshell_ifcparse_instance_list_get(PyObject *self, PyObject *args) {{
    PyObject *list_obj;
    Py_ssize_t index;
    if (!PyArg_ParseTuple(args, "On", &list_obj, &index))
        return NULL;
    if (!PyObject_TypeCheck(list_obj, &IfcOpenshellIfcparseInstanceListType)) {{
        PyErr_SetString(PyExc_TypeError, "Expected an IfcOpenshellIfcparseInstanceList handle");
        return NULL;
    }}
    IfcOpenshellIfcparseInstanceListObject *lh = (IfcOpenshellIfcparseInstanceListObject *)list_obj;
    ifcopenshell_clear_error();
    ifcopenshell_ifc_instance_t *inst = NULL;
    if (!ifcopenshell_ifcparse_instance_list_get((ifcopenshell_ifcparse_instance_list_t *)lh->handle, (size_t)index, &inst)) {{
        return raise_last_error("Failed to get instance from list");
    }}
    if (!inst) {{
        Py_RETURN_NONE;
    }}
    {inst_py}Object *result = ({inst_py}Object *){inst_py}Type.tp_alloc(&{inst_py}Type, 0);
    if (!result) {{
        ifcopenshell_ifc_instance_destroy(inst);
        return NULL;
    }}
    result->handle = (void *)inst;
    result->owned = 1;
    return (PyObject *)result;
}}

static PyObject *py_ifcopenshell_ifcparse_instance_list_destroy(PyObject *self, PyObject *args) {{
    PyObject *obj;
    if (!PyArg_ParseTuple(args, "O", &obj))
        return NULL;
    if (!PyObject_TypeCheck(obj, &IfcOpenshellIfcparseInstanceListType)) {{
        PyErr_SetString(PyExc_TypeError, "Expected an IfcOpenshellIfcparseInstanceList handle");
        return NULL;
    }}
    IfcOpenshellIfcparseInstanceListObject *h = (IfcOpenshellIfcparseInstanceListObject *)obj;
    if (h->owned && h->handle) {{
        ifcopenshell_ifcparse_instance_list_destroy((ifcopenshell_ifcparse_instance_list_t *)h->handle);
        h->handle = NULL;
        h->owned = 0;
    }}
    Py_RETURN_NONE;
}}

static PyObject *py_ifcopenshell_ifcparse_instance_list_create_from_handles(PyObject *self, PyObject *args) {{
    PyObject *list_obj;
    if (!PyArg_ParseTuple(args, "O", &list_obj))
        return NULL;
    if (!PyTuple_Check(list_obj) && !PyList_Check(list_obj)) {{
        PyErr_SetString(PyExc_TypeError, "Expected a tuple or list of instance handles");
        return NULL;
    }}
    Py_ssize_t count = PySequence_Size(list_obj);
    ifcopenshell_ifc_instance_list_t input_list;
    ifcopenshell_ifc_instance_t **items = NULL;
    if (count > 0) {{
        items = (ifcopenshell_ifc_instance_t **)PyMem_Malloc(sizeof(ifcopenshell_ifc_instance_t *) * count);
        if (!items) return PyErr_NoMemory();
        for (Py_ssize_t i = 0; i < count; i++) {{
            PyObject *item = PySequence_GetItem(list_obj, i);
            if (!item) {{ PyMem_Free(items); return NULL; }}
            if (!PyObject_TypeCheck(item, &{inst_py}Type)) {{
                Py_DECREF(item);
                PyMem_Free(items);
                PyErr_SetString(PyExc_TypeError, "Expected {inst_py} handles in list");
                return NULL;
            }}
            items[i] = (ifcopenshell_ifc_instance_t *)(({inst_py}Object *)item)->handle;
            Py_DECREF(item);
        }}
    }}
    input_list.items = items;
    input_list.size = (size_t)count;
    ifcopenshell_clear_error();
    ifcopenshell_ifcparse_instance_list_t *result_list = NULL;
    if (!ifcopenshell_ifcparse_instance_list_create_from_handles(&input_list, &result_list)) {{
        PyMem_Free(items);
        return raise_last_error("Failed to create instance list");
    }}
    PyMem_Free(items);
    if (!result_list) {{
        return raise_last_error("Failed to create instance list");
    }}
    IfcOpenshellIfcparseInstanceListObject *result = (IfcOpenshellIfcparseInstanceListObject *)IfcOpenshellIfcparseInstanceListType.tp_alloc(&IfcOpenshellIfcparseInstanceListType, 0);
    if (!result) {{
        if (result_list) ifcopenshell_ifcparse_instance_list_destroy(result_list);
        return NULL;
    }}
    result->handle = (void *)result_list;
    result->owned = 1;
    return (PyObject *)result;
}}

static PyObject *py_ifcopenshell_ifc_file_remove(PyObject *self, PyObject *args) {{
    PyObject *file_obj;
    PyObject *inst_obj;
    if (!PyArg_ParseTuple(args, "OO", &file_obj, &inst_obj))
        return NULL;
    if (!PyObject_TypeCheck(file_obj, &{file_py}Type)) {{
        PyErr_SetString(PyExc_TypeError, "Expected an {file_py} handle");
        return NULL;
    }}
    if (!PyObject_TypeCheck(inst_obj, &{inst_py}Type)) {{
        PyErr_SetString(PyExc_TypeError, "Expected an {inst_py} handle");
        return NULL;
    }}
    {file_py}Object *fh = ({file_py}Object *)file_obj;
    {inst_py}Object *ih = ({inst_py}Object *)inst_obj;
    ifcopenshell_clear_error();
    if (!ifcopenshell_ifc_file_remove(({file_handle.c_type} *)fh->handle, ({instance_handle.c_type} *)ih->handle)) {{
        return raise_last_error("Failed to remove instance");
    }}
    ih->owned = 0;
    Py_RETURN_NONE;
}}

static PyObject *py_ifcopenshell_ifc_file_add(PyObject *self, PyObject *args) {{
    PyObject *file_obj;
    PyObject *inst_obj;
    int instance_id = -1;
    if (!PyArg_ParseTuple(args, "OO|i", &file_obj, &inst_obj, &instance_id))
        return NULL;
    if (!PyObject_TypeCheck(file_obj, &{file_py}Type)) {{
        PyErr_SetString(PyExc_TypeError, "Expected an {file_py} handle");
        return NULL;
    }}
    if (!PyObject_TypeCheck(inst_obj, &{inst_py}Type)) {{
        PyErr_SetString(PyExc_TypeError, "Expected an {inst_py} handle");
        return NULL;
    }}
    {file_py}Object *fh = ({file_py}Object *)file_obj;
    {inst_py}Object *ih = ({inst_py}Object *)inst_obj;
    ifcopenshell_clear_error();
    ifcopenshell_ifc_instance_t *new_inst = NULL;
    if (!ifcopenshell_ifc_file_add(({file_handle.c_type} *)fh->handle, ({instance_handle.c_type} *)ih->handle, instance_id, &new_inst)) {{
        return raise_last_error("Failed to add instance");
    }}
    if (!new_inst) {{
        Py_RETURN_NONE;
    }}
    {inst_py}Object *result = ({inst_py}Object *){inst_py}Type.tp_alloc(&{inst_py}Type, 0);
    if (!result) {{
        ifcopenshell_ifc_instance_destroy(new_inst);
        return NULL;
    }}
    result->handle = (void *)new_inst;
    result->owned = 1;
    return (PyObject *)result;
}}

static PyObject *py_ifcopenshell_ifcgeom_create_iterator(PyObject *self, PyObject *args) {{
    const char *geometry_library;
    PyObject *settings_obj;
    PyObject *file_obj;
    int num_threads = 1;
    if (!PyArg_ParseTuple(args, "sOO|i", &geometry_library, &settings_obj, &file_obj, &num_threads))
        return NULL;
    if (!PyObject_TypeCheck(settings_obj, &IfcOpenshellIfcgeomSettingsType)) {{
        PyErr_SetString(PyExc_TypeError, "Expected an IfcOpenshellIfcgeomSettings handle");
        return NULL;
    }}
    if (!PyObject_TypeCheck(file_obj, &{file_py}Type)) {{
        PyErr_SetString(PyExc_TypeError, "Expected an {file_py} handle");
        return NULL;
    }}
    IfcOpenshellIfcgeomSettingsObject *sh = (IfcOpenshellIfcgeomSettingsObject *)settings_obj;
    {file_py}Object *fh = ({file_py}Object *)file_obj;
    ifcopenshell_clear_error();
    ifcopenshell_ifcgeom_iterator_t *iterator = NULL;
    if (!ifcopenshell_ifcgeom_create_iterator(geometry_library, (ifcopenshell_ifcgeom_settings_t *)sh->handle, ({file_handle.c_type} *)fh->handle, num_threads, &iterator)) {{
        return raise_last_error("Failed to create geometry iterator");
    }}
    IfcOpenshellIfcgeomIteratorObject *result = (IfcOpenshellIfcgeomIteratorObject *)IfcOpenshellIfcgeomIteratorType.tp_alloc(&IfcOpenshellIfcgeomIteratorType, 0);
    if (!result) {{
        ifcopenshell_ifcgeom_iterator_destroy(iterator);
        return NULL;
    }}
    result->handle = (void *)iterator;
    result->owned = 1;
    return (PyObject *)result;
}}

static PyObject *py_ifcopenshell_ifcgeom_iterator_destroy(PyObject *self, PyObject *args) {{
    PyObject *obj;
    if (!PyArg_ParseTuple(args, "O", &obj))
        return NULL;
    if (!PyObject_TypeCheck(obj, &IfcOpenshellIfcgeomIteratorType)) {{
        PyErr_SetString(PyExc_TypeError, "Expected an IfcOpenshellIfcgeomIterator handle");
        return NULL;
    }}
    IfcOpenshellIfcgeomIteratorObject *h = (IfcOpenshellIfcgeomIteratorObject *)obj;
    if (h->owned && h->handle) {{
        ifcopenshell_ifcgeom_iterator_destroy((ifcopenshell_ifcgeom_iterator_t *)h->handle);
        h->handle = NULL;
        h->owned = 0;
    }}
    Py_RETURN_NONE;
}}

static PyObject *py_ifcopenshell_ifcgeom_iterator_initialize(PyObject *self, PyObject *args) {{
    PyObject *iter_obj;
    if (!PyArg_ParseTuple(args, "O", &iter_obj))
        return NULL;
    if (!PyObject_TypeCheck(iter_obj, &IfcOpenshellIfcgeomIteratorType)) {{
        PyErr_SetString(PyExc_TypeError, "Expected an IfcOpenshellIfcgeomIterator handle");
        return NULL;
    }}
    IfcOpenshellIfcgeomIteratorObject *ih = (IfcOpenshellIfcgeomIteratorObject *)iter_obj;
    ifcopenshell_clear_error();
    bool result = false;
    if (!ifcopenshell_ifcgeom_iterator_initialize((ifcopenshell_ifcgeom_iterator_t *)ih->handle, &result)) {{
        return raise_last_error("Failed to initialize geometry iterator");
    }}
    return PyBool_FromLong(result);
}}

static PyObject *py_ifcopenshell_ifcgeom_iterator_get(PyObject *self, PyObject *args) {{
    PyObject *iter_obj;
    if (!PyArg_ParseTuple(args, "O", &iter_obj))
        return NULL;
    if (!PyObject_TypeCheck(iter_obj, &IfcOpenshellIfcgeomIteratorType)) {{
        PyErr_SetString(PyExc_TypeError, "Expected an IfcOpenshellIfcgeomIterator handle");
        return NULL;
    }}
    IfcOpenshellIfcgeomIteratorObject *ih = (IfcOpenshellIfcgeomIteratorObject *)iter_obj;
    ifcopenshell_clear_error();
    ifcopenshell_ifcgeom_element_t *element = NULL;
    if (!ifcopenshell_ifcgeom_iterator_get((ifcopenshell_ifcgeom_iterator_t *)ih->handle, &element)) {{
        return raise_last_error("Failed to get geometry element");
    }}
    if (!element) {{
        Py_RETURN_NONE;
    }}
    IfcOpenshellIfcgeomElementObject *result = (IfcOpenshellIfcgeomElementObject *)IfcOpenshellIfcgeomElementType.tp_alloc(&IfcOpenshellIfcgeomElementType, 0);
    if (!result) {{
        ifcopenshell_ifcgeom_element_destroy(element);
        return NULL;
    }}
    result->handle = (void *)element;
    result->owned = 1;
    return (PyObject *)result;
}}

static PyObject *py_ifcopenshell_ifcgeom_iterator_progress(PyObject *self, PyObject *args) {{
    PyObject *iter_obj;
    if (!PyArg_ParseTuple(args, "O", &iter_obj))
        return NULL;
    if (!PyObject_TypeCheck(iter_obj, &IfcOpenshellIfcgeomIteratorType)) {{
        PyErr_SetString(PyExc_TypeError, "Expected an IfcOpenshellIfcgeomIterator handle");
        return NULL;
    }}
    IfcOpenshellIfcgeomIteratorObject *ih = (IfcOpenshellIfcgeomIteratorObject *)iter_obj;
    ifcopenshell_clear_error();
    int32_t progress = 0;
    if (!ifcopenshell_ifcgeom_iterator_progress((ifcopenshell_ifcgeom_iterator_t *)ih->handle, &progress)) {{
        return raise_last_error("Failed to get iterator progress");
    }}
    return PyLong_FromLong(progress);
}}

static PyObject *py_ifcopenshell_ifcgeom_element_id(PyObject *self, PyObject *args) {{
    PyObject *elem_obj;
    if (!PyArg_ParseTuple(args, "O", &elem_obj))
        return NULL;
    if (!PyObject_TypeCheck(elem_obj, &IfcOpenshellIfcgeomElementType)) {{
        PyErr_SetString(PyExc_TypeError, "Expected an IfcOpenshellIfcgeomElement handle");
        return NULL;
    }}
    IfcOpenshellIfcgeomElementObject *eh = (IfcOpenshellIfcgeomElementObject *)elem_obj;
    ifcopenshell_clear_error();
    int32_t id = 0;
    if (!ifcopenshell_ifcgeom_element_id((ifcopenshell_ifcgeom_element_t *)eh->handle, &id)) {{
        return raise_last_error("Failed to get element id");
    }}
    return PyLong_FromLong(id);
}}

static PyObject *py_ifcopenshell_ifcgeom_element_guid(PyObject *self, PyObject *args) {{
    PyObject *elem_obj;
    if (!PyArg_ParseTuple(args, "O", &elem_obj))
        return NULL;
    if (!PyObject_TypeCheck(elem_obj, &IfcOpenshellIfcgeomElementType)) {{
        PyErr_SetString(PyExc_TypeError, "Expected an IfcOpenshellIfcgeomElement handle");
        return NULL;
    }}
    IfcOpenshellIfcgeomElementObject *eh = (IfcOpenshellIfcgeomElementObject *)elem_obj;
    ifcopenshell_clear_error();
    ifcopenshell_string_t value = {{0}};
    if (!ifcopenshell_ifcgeom_element_guid((ifcopenshell_ifcgeom_element_t *)eh->handle, &value)) {{
        return raise_last_error("Failed to get element guid");
    }}
    return convert_ifcopenshell_string(&value);
}}

static PyObject *py_ifcopenshell_ifcgeom_element_name(PyObject *self, PyObject *args) {{
    PyObject *elem_obj;
    if (!PyArg_ParseTuple(args, "O", &elem_obj))
        return NULL;
    if (!PyObject_TypeCheck(elem_obj, &IfcOpenshellIfcgeomElementType)) {{
        PyErr_SetString(PyExc_TypeError, "Expected an IfcOpenshellIfcgeomElement handle");
        return NULL;
    }}
    IfcOpenshellIfcgeomElementObject *eh = (IfcOpenshellIfcgeomElementObject *)elem_obj;
    ifcopenshell_clear_error();
    ifcopenshell_string_t value = {{0}};
    if (!ifcopenshell_ifcgeom_element_name((ifcopenshell_ifcgeom_element_t *)eh->handle, &value)) {{
        return raise_last_error("Failed to get element name");
    }}
    return convert_ifcopenshell_string(&value);
}}

static PyObject *py_ifcopenshell_ifcgeom_element_destroy(PyObject *self, PyObject *args) {{
    PyObject *obj;
    if (!PyArg_ParseTuple(args, "O", &obj))
        return NULL;
    if (!PyObject_TypeCheck(obj, &IfcOpenshellIfcgeomElementType)) {{
        PyErr_SetString(PyExc_TypeError, "Expected an IfcOpenshellIfcgeomElement handle");
        return NULL;
    }}
    IfcOpenshellIfcgeomElementObject *h = (IfcOpenshellIfcgeomElementObject *)obj;
    if (h->owned && h->handle) {{
        ifcopenshell_ifcgeom_element_destroy((ifcopenshell_ifcgeom_element_t *)h->handle);
        h->handle = NULL;
        h->owned = 0;
    }}
    Py_RETURN_NONE;
}}

static PyObject *py_ifcopenshell_ifcgeom_element_product(PyObject *self, PyObject *args) {{
    PyObject *elem_obj;
    if (!PyArg_ParseTuple(args, "O", &elem_obj))
        return NULL;
    if (!PyObject_TypeCheck(elem_obj, &IfcOpenshellIfcgeomElementType)) {{
        PyErr_SetString(PyExc_TypeError, "Expected an IfcOpenshellIfcgeomElement handle");
        return NULL;
    }}
    IfcOpenshellIfcgeomElementObject *eh = (IfcOpenshellIfcgeomElementObject *)elem_obj;
    ifcopenshell_clear_error();
    ifcopenshell_ifc_instance_t *inst = NULL;
    if (!ifcopenshell_ifcgeom_element_product((ifcopenshell_ifcgeom_element_t *)eh->handle, &inst)) {{
        return raise_last_error("Failed to get element product");
    }}
    if (!inst) {{
        Py_RETURN_NONE;
    }}
    {inst_py}Object *result = ({inst_py}Object *){inst_py}Type.tp_alloc(&{inst_py}Type, 0);
    if (!result) {{
        ifcopenshell_ifc_instance_destroy(inst);
        return NULL;
    }}
    result->handle = (void *)inst;
    result->owned = 1;
    return (PyObject *)result;
}}

static PyObject *py_ifcopenshell_ifcgeom_element_context(PyObject *self, PyObject *args) {{
    PyObject *elem_obj;
    if (!PyArg_ParseTuple(args, "O", &elem_obj))
        return NULL;
    if (!PyObject_TypeCheck(elem_obj, &IfcOpenshellIfcgeomElementType)) {{
        PyErr_SetString(PyExc_TypeError, "Expected an IfcOpenshellIfcgeomElement handle");
        return NULL;
    }}
    IfcOpenshellIfcgeomElementObject *eh = (IfcOpenshellIfcgeomElementObject *)elem_obj;
    ifcopenshell_clear_error();
    ifcopenshell_string_t value = {{0}};
    if (!ifcopenshell_ifcgeom_element_context((ifcopenshell_ifcgeom_element_t *)eh->handle, &value)) {{
        return raise_last_error("Failed to get element context");
    }}
    return convert_ifcopenshell_string(&value);
}}

static PyObject *py_ifcopenshell_ifcgeom_element_transformation(PyObject *self, PyObject *args) {{
    PyObject *elem_obj;
    if (!PyArg_ParseTuple(args, "O", &elem_obj))
        return NULL;
    if (!PyObject_TypeCheck(elem_obj, &IfcOpenshellIfcgeomElementType)) {{
        PyErr_SetString(PyExc_TypeError, "Expected an IfcOpenshellIfcgeomElement handle");
        return NULL;
    }}
    IfcOpenshellIfcgeomElementObject *eh = (IfcOpenshellIfcgeomElementObject *)elem_obj;
    ifcopenshell_clear_error();
    ifcopenshell_ifcgeom_transformation_t *transformation = NULL;
    if (!ifcopenshell_ifcgeom_element_transformation((ifcopenshell_ifcgeom_element_t *)eh->handle, &transformation)) {{
        return raise_last_error("Failed to get element transformation");
    }}
    if (!transformation) {{
        Py_RETURN_NONE;
    }}
    IfcOpenshellIfcgeomTransformationObject *result = (IfcOpenshellIfcgeomTransformationObject *)IfcOpenshellIfcgeomTransformationType.tp_alloc(&IfcOpenshellIfcgeomTransformationType, 0);
    if (!result) {{
        ifcopenshell_ifcgeom_transformation_destroy(transformation);
        return NULL;
    }}
    result->handle = (void *)transformation;
    result->owned = 1;
    return (PyObject *)result;
}}

static PyObject *py_ifcopenshell_ifcgeom_triangulation_element_geometry(PyObject *self, PyObject *args) {{
    PyObject *elem_obj;
    if (!PyArg_ParseTuple(args, "O", &elem_obj))
        return NULL;
    if (!PyObject_TypeCheck(elem_obj, &IfcOpenshellIfcgeomTriangulationElementType)) {{
        PyErr_SetString(PyExc_TypeError, "Expected an IfcOpenshellIfcgeomTriangulationElement handle");
        return NULL;
    }}
    IfcOpenshellIfcgeomTriangulationElementObject *eh = (IfcOpenshellIfcgeomTriangulationElementObject *)elem_obj;
    ifcopenshell_clear_error();
    ifcopenshell_ifcgeom_triangulation_t *tri = NULL;
    if (!ifcopenshell_ifcgeom_triangulation_element_geometry((ifcopenshell_ifcgeom_triangulation_element_t *)eh->handle, &tri)) {{
        return raise_last_error("Failed to get triangulation geometry");
    }}
    if (!tri) {{
        Py_RETURN_NONE;
    }}
    IfcOpenshellIfcgeomTriangulationObject *result = (IfcOpenshellIfcgeomTriangulationObject *)IfcOpenshellIfcgeomTriangulationType.tp_alloc(&IfcOpenshellIfcgeomTriangulationType, 0);
    if (!result) {{
        ifcopenshell_ifcgeom_triangulation_destroy(tri);
        return NULL;
    }}
    result->handle = (void *)tri;
    result->owned = 1;
    return (PyObject *)result;
}}

static PyObject *py_ifcopenshell_ifcgeom_triangulation_verts(PyObject *self, PyObject *args) {{
    PyObject *tri_obj;
    if (!PyArg_ParseTuple(args, "O", &tri_obj))
        return NULL;
    if (!PyObject_TypeCheck(tri_obj, &IfcOpenshellIfcgeomTriangulationType)) {{
        PyErr_SetString(PyExc_TypeError, "Expected an IfcOpenshellIfcgeomTriangulation handle");
        return NULL;
    }}
    IfcOpenshellIfcgeomTriangulationObject *th = (IfcOpenshellIfcgeomTriangulationObject *)tri_obj;
    ifcopenshell_clear_error();
    ifcopenshell_double_list_t value = {{0}};
    if (!ifcopenshell_ifcgeom_triangulation_verts((ifcopenshell_ifcgeom_triangulation_t *)th->handle, &value)) {{
        return raise_last_error("Failed to get triangulation verts");
    }}
    return convert_ifcopenshell_double_list(&value);
}}

static PyObject *py_ifcopenshell_ifcgeom_triangulation_faces(PyObject *self, PyObject *args) {{
    PyObject *tri_obj;
    if (!PyArg_ParseTuple(args, "O", &tri_obj))
        return NULL;
    if (!PyObject_TypeCheck(tri_obj, &IfcOpenshellIfcgeomTriangulationType)) {{
        PyErr_SetString(PyExc_TypeError, "Expected an IfcOpenshellIfcgeomTriangulation handle");
        return NULL;
    }}
    IfcOpenshellIfcgeomTriangulationObject *th = (IfcOpenshellIfcgeomTriangulationObject *)tri_obj;
    ifcopenshell_clear_error();
    ifcopenshell_int32_list_t value = {{0}};
    if (!ifcopenshell_ifcgeom_triangulation_faces((ifcopenshell_ifcgeom_triangulation_t *)th->handle, &value)) {{
        return raise_last_error("Failed to get triangulation faces");
    }}
    return convert_ifcopenshell_int32_list(&value);
}}

static PyObject *py_ifcopenshell_ifcgeom_triangulation_normals(PyObject *self, PyObject *args) {{
    PyObject *tri_obj;
    if (!PyArg_ParseTuple(args, "O", &tri_obj))
        return NULL;
    if (!PyObject_TypeCheck(tri_obj, &IfcOpenshellIfcgeomTriangulationType)) {{
        PyErr_SetString(PyExc_TypeError, "Expected an IfcOpenshellIfcgeomTriangulation handle");
        return NULL;
    }}
    IfcOpenshellIfcgeomTriangulationObject *th = (IfcOpenshellIfcgeomTriangulationObject *)tri_obj;
    ifcopenshell_clear_error();
    ifcopenshell_double_list_t value = {{0}};
    if (!ifcopenshell_ifcgeom_triangulation_normals((ifcopenshell_ifcgeom_triangulation_t *)th->handle, &value)) {{
        return raise_last_error("Failed to get triangulation normals");
    }}
    return convert_ifcopenshell_double_list(&value);
}}

static PyObject *py_ifcopenshell_ifcgeom_triangulation_uvs(PyObject *self, PyObject *args) {{
    PyObject *tri_obj;
    if (!PyArg_ParseTuple(args, "O", &tri_obj))
        return NULL;
    if (!PyObject_TypeCheck(tri_obj, &IfcOpenshellIfcgeomTriangulationType)) {{
        PyErr_SetString(PyExc_TypeError, "Expected an IfcOpenshellIfcgeomTriangulation handle");
        return NULL;
    }}
    IfcOpenshellIfcgeomTriangulationObject *th = (IfcOpenshellIfcgeomTriangulationObject *)tri_obj;
    ifcopenshell_clear_error();
    ifcopenshell_double_list_t value = {{0}};
    if (!ifcopenshell_ifcgeom_triangulation_uvs((ifcopenshell_ifcgeom_triangulation_t *)th->handle, &value)) {{
        return raise_last_error("Failed to get triangulation uvs");
    }}
    return convert_ifcopenshell_double_list(&value);
}}

static PyObject *py_ifcopenshell_ifcgeom_triangulation_destroy(PyObject *self, PyObject *args) {{
    PyObject *obj;
    if (!PyArg_ParseTuple(args, "O", &obj))
        return NULL;
    if (!PyObject_TypeCheck(obj, &IfcOpenshellIfcgeomTriangulationType)) {{
        PyErr_SetString(PyExc_TypeError, "Expected an IfcOpenshellIfcgeomTriangulation handle");
        return NULL;
    }}
    IfcOpenshellIfcgeomTriangulationObject *h = (IfcOpenshellIfcgeomTriangulationObject *)obj;
    if (h->owned && h->handle) {{
        ifcopenshell_ifcgeom_triangulation_destroy((ifcopenshell_ifcgeom_triangulation_t *)h->handle);
        h->handle = NULL;
        h->owned = 0;
    }}
    Py_RETURN_NONE;
}}

static PyObject *py_ifcopenshell_ifcgeom_create_shape(PyObject *self, PyObject *args) {{
    PyObject *settings_obj;
    PyObject *inst_obj;
    PyObject *rep_obj = Py_None;
    const char *geometry_library = "opencascade";
    if (!PyArg_ParseTuple(args, "OO|Os", &settings_obj, &inst_obj, &rep_obj, &geometry_library))
        return NULL;
    if (!PyObject_TypeCheck(settings_obj, &IfcOpenshellIfcgeomSettingsType)) {{
        PyErr_SetString(PyExc_TypeError, "Expected an IfcOpenshellIfcgeomSettings handle");
        return NULL;
    }}
    if (!PyObject_TypeCheck(inst_obj, &{inst_py}Type)) {{
        PyErr_SetString(PyExc_TypeError, "Expected an {inst_py} handle");
        return NULL;
    }}
    ifcopenshell_ifc_instance_t *rep = NULL;
    if (rep_obj != Py_None) {{
        if (!PyObject_TypeCheck(rep_obj, &{inst_py}Type)) {{
            PyErr_SetString(PyExc_TypeError, "Expected an {inst_py} handle for representation");
            return NULL;
        }}
        rep = (ifcopenshell_ifc_instance_t *)(({inst_py}Object *)rep_obj)->handle;
    }}
    IfcOpenshellIfcgeomSettingsObject *sh = (IfcOpenshellIfcgeomSettingsObject *)settings_obj;
    {inst_py}Object *ih = ({inst_py}Object *)inst_obj;
    ifcopenshell_clear_error();
    ifcopenshell_ifcgeom_element_t *element = NULL;
    if (!ifcopenshell_ifcgeom_create_shape((ifcopenshell_ifcgeom_settings_t *)sh->handle, (ifcopenshell_ifc_instance_t *)ih->handle, rep, geometry_library, &element)) {{
        return raise_last_error("Failed to create geometry shape");
    }}
    if (!element) {{
        Py_RETURN_NONE;
    }}
    IfcOpenshellIfcgeomElementObject *result = (IfcOpenshellIfcgeomElementObject *)IfcOpenshellIfcgeomElementType.tp_alloc(&IfcOpenshellIfcgeomElementType, 0);
    if (!result) {{
        ifcopenshell_ifcgeom_element_destroy(element);
        return NULL;
    }}
    result->handle = (void *)element;
    result->owned = 1;
    return (PyObject *)result;
}}

static PyObject *py_ifcopenshell_ifcgeom_triangulation_material_count(PyObject *self, PyObject *args) {{
    PyObject *tri_obj;
    if (!PyArg_ParseTuple(args, "O", &tri_obj))
        return NULL;
    if (!PyObject_TypeCheck(tri_obj, &IfcOpenshellIfcgeomTriangulationType)) {{
        PyErr_SetString(PyExc_TypeError, "Expected an IfcOpenshellIfcgeomTriangulation handle");
        return NULL;
    }}
    IfcOpenshellIfcgeomTriangulationObject *th = (IfcOpenshellIfcgeomTriangulationObject *)tri_obj;
    ifcopenshell_clear_error();
    size_t count = 0;
    if (!ifcopenshell_ifcgeom_triangulation_material_count((ifcopenshell_ifcgeom_triangulation_t *)th->handle, &count)) {{
        return raise_last_error("Failed to get material count");
    }}
    return PyLong_FromSize_t(count);
}}"""


def render_python_extension(metadata: HostBindingMetadata, api_header_path: Path | None = None) -> str:
    # Merge handles from metadata with those discovered from the C API header.
    # metadata.handles only contains handles from the binding IR; the function
    # wrappers reference many additional handle types declared in the header.
    all_handles: dict[str, HostStructMetadata] = {}
    if api_header_path is not None:
        all_handles.update(_discover_all_handle_types(api_header_path))
    all_handles.update(metadata.handles)  # IR handles take precedence

    handles = sorted(all_handles.values(), key=lambda h: h.c_type)
    handle_type_decls = []
    type_ready_calls = []
    add_type_calls = []

    for handle in handles:
        py_name = _py_type_name(handle.c_type)
        handle_type_decls.append(_render_handle_type_decl(handle, py_name))
        type_ready_calls.append(f"    if (PyType_Ready(&{py_name}Type) < 0) return NULL;")
        add_type_calls.append(f"    Py_INCREF(&{py_name}Type);")
        add_type_calls.append(
            f"    if (PyModule_AddObject(m, \"{py_name}\", (PyObject *)&{py_name}Type) < 0) {{"
        )
        add_type_calls.append(f"        Py_DECREF(&{py_name}Type);")
        add_type_calls.append(f"        return NULL;")
        add_type_calls.append(f"    }}")

    error_kind_constants = """\
    PyModule_AddIntConstant(m, "IFCOPENSHELL_ERROR_NONE", 0);
    PyModule_AddIntConstant(m, "IFCOPENSHELL_ERROR_RUNTIME", 1);
    PyModule_AddIntConstant(m, "IFCOPENSHELL_ERROR_VALUE", 2);
    PyModule_AddIntConstant(m, "IFCOPENSHELL_ERROR_TYPE", 3);
    PyModule_AddIntConstant(m, "IFCOPENSHELL_ERROR_NOT_IMPLEMENTED", 4);
    PyModule_AddIntConstant(m, "IFCOPENSHELL_ERROR_KEY", 5);
    PyModule_AddIntConstant(m, "IFCOPENSHELL_LOGICAL_UNKNOWN", -1);
    PyModule_AddIntConstant(m, "IFCOPENSHELL_LOGICAL_FALSE", 0);
    PyModule_AddIntConstant(m, "IFCOPENSHELL_LOGICAL_TRUE", 1);"""

    type_ready_block = "\n".join(type_ready_calls)
    add_type_block = "\n".join(add_type_calls)

    return f"""\
// This file was generated by IfcOpenShell's binding generator. Do not edit manually.

#include <Python.h>
#include "ifcopenshell_api.h"

static PyObject *module_state = NULL;

static PyObject *raise_last_error(const char *fallback) {{
    const char *msg = ifcopenshell_last_error_message();
    int kind = ifcopenshell_last_error_kind();
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

{"".join(handle_type_decls)}

{_render_value_converters()}

{_render_subset_function_wrappers(metadata)}

static PyMethodDef module_methods[] = {{
    {{"clear_error", py_ifcopenshell_clear_error, METH_NOARGS, "Clear the last error"}},
    {{"last_error_message", py_ifcopenshell_last_error_message, METH_NOARGS, "Get the last error message"}},
    {{"last_error_kind", py_ifcopenshell_last_error_kind, METH_NOARGS, "Get the last error kind"}},
    {{"open", py_ifcopenshell_ifcparse_open, METH_VARARGS, "Open an IFC file"}},
    {{"file_destroy", py_ifcopenshell_ifc_file_destroy, METH_VARARGS, "Destroy an IFC file handle"}},
    {{"file_by_id", py_ifcopenshell_ifc_file_by_id, METH_VARARGS, "Get instance by id"}},
    {{"file_by_type", py_ifcopenshell_ifc_file_by_type, METH_VARARGS, "Get instances by type"}},
    {{"file_by_guid", py_ifcopenshell_ifc_file_by_guid, METH_VARARGS, "Get instance by guid"}},
    {{"file_remove", py_ifcopenshell_ifc_file_remove, METH_VARARGS, "Remove an instance from file"}},
    {{"file_add", py_ifcopenshell_ifc_file_add, METH_VARARGS, "Add an instance to file"}},
    {{"instance_id", py_ifcopenshell_ifc_instance_id, METH_VARARGS, "Get instance id"}},
    {{"instance_declaration", py_ifcopenshell_ifc_instance_declaration, METH_VARARGS, "Get instance declaration"}},
    {{"instance_get_argument", py_ifcopenshell_ifc_instance_get_argument, METH_VARARGS, "Get instance argument"}},
    {{"instance_destroy", py_ifcopenshell_ifc_instance_destroy, METH_VARARGS, "Destroy an instance handle"}},
    {{"version", py_ifcopenshell_ifcparse_version, METH_NOARGS, "Get library version"}},
    {{"schema_names", py_ifcopenshell_ifcparse_schema_names, METH_NOARGS, "Get available schema names"}},
    {{"guid_new", py_ifcopenshell_ifcapi_guid_new, METH_NOARGS, "Generate a new GUID"}},
    {{"root_create_entity", py_ifcopenshell_ifcapi_root_create_entity, METH_VARARGS, "Create a new entity"}},
    {{"declaration_name", py_ifcopenshell_ifc_declaration_name, METH_VARARGS, "Get declaration name"}},
    {{"attribute_value_destroy", py_ifcopenshell_ifcparse_attribute_value_destroy, METH_VARARGS, "Destroy attribute value"}},
    {{"attribute_value_is_null", py_ifcopenshell_ifcparse_attribute_value_is_null, METH_VARARGS, "Check if attribute is null"}},
    {{"attribute_value_type", py_ifcopenshell_ifcparse_attribute_value_type, METH_VARARGS, "Get attribute type"}},
    {{"attribute_value_as_string", py_ifcopenshell_ifcparse_attribute_value_as_string, METH_VARARGS, "Get attribute as string"}},
    {{"attribute_value_as_double", py_ifcopenshell_ifcparse_attribute_value_as_double, METH_VARARGS, "Get attribute as double"}},
    {{"attribute_value_as_int32", py_ifcopenshell_ifcparse_attribute_value_as_int32, METH_VARARGS, "Get attribute as int32"}},
    {{"attribute_value_as_bool", py_ifcopenshell_ifcparse_attribute_value_as_bool, METH_VARARGS, "Get attribute as bool"}},
    {{"attribute_value_as_instance", py_ifcopenshell_ifcparse_attribute_value_as_instance, METH_VARARGS, "Get attribute as instance"}},
    {{"attribute_value_as_instance_list", py_ifcopenshell_ifcparse_attribute_value_as_instance_list, METH_VARARGS, "Get attribute as instance list"}},
    {{"attribute_value_as_string_list", py_ifcopenshell_ifcparse_attribute_value_as_string_list, METH_VARARGS, "Get attribute as string list"}},
    {{"attribute_value_as_double_list", py_ifcopenshell_ifcparse_attribute_value_as_double_list, METH_VARARGS, "Get attribute as double list"}},
    {{"attribute_value_as_double_list_list", py_ifcopenshell_ifcparse_attribute_value_as_double_list_list, METH_VARARGS, "Get attribute as double list list"}},
    {{"attribute_value_as_int32_list", py_ifcopenshell_ifcparse_attribute_value_as_int32_list, METH_VARARGS, "Get attribute as int32 list"}},
    {{"attribute_value_as_int32_list_list", py_ifcopenshell_ifcparse_attribute_value_as_int32_list_list, METH_VARARGS, "Get attribute as int32 list list"}},
    {{"attribute_value_as_enumeration_value", py_ifcopenshell_ifcparse_attribute_value_as_enumeration_value, METH_VARARGS, "Get attribute as enumeration value"}},
    {{"instance_list_size", py_ifcopenshell_ifcparse_instance_list_size, METH_VARARGS, "Get instance list size"}},
    {{"instance_list_get", py_ifcopenshell_ifcparse_instance_list_get, METH_VARARGS, "Get instance from list by index"}},
    {{"instance_list_destroy", py_ifcopenshell_ifcparse_instance_list_destroy, METH_VARARGS, "Destroy instance list"}},
    {{"instance_list_create_from_handles", py_ifcopenshell_ifcparse_instance_list_create_from_handles, METH_VARARGS, "Create instance list from handles"}},
    {{"geom_create_settings", py_ifcopenshell_ifcgeom_create_settings, METH_NOARGS, "Create geometry settings"}},
    {{"geom_create_tree_from_file", py_ifcopenshell_ifcgeom_create_tree_from_file, METH_VARARGS, "Create geometry tree from file"}},
    {{"geom_tree_destroy", py_ifcopenshell_ifcgeom_tree_destroy, METH_VARARGS, "Destroy geometry tree"}},
    {{"geom_create_iterator", py_ifcopenshell_ifcgeom_create_iterator, METH_VARARGS, "Create geometry iterator"}},
    {{"geom_iterator_destroy", py_ifcopenshell_ifcgeom_iterator_destroy, METH_VARARGS, "Destroy geometry iterator"}},
    {{"geom_iterator_initialize", py_ifcopenshell_ifcgeom_iterator_initialize, METH_VARARGS, "Initialize geometry iterator"}},
    {{"geom_iterator_get", py_ifcopenshell_ifcgeom_iterator_get, METH_VARARGS, "Get geometry element from iterator"}},
    {{"geom_iterator_progress", py_ifcopenshell_ifcgeom_iterator_progress, METH_VARARGS, "Get iterator progress"}},
    {{"geom_element_id", py_ifcopenshell_ifcgeom_element_id, METH_VARARGS, "Get geometry element id"}},
    {{"geom_element_guid", py_ifcopenshell_ifcgeom_element_guid, METH_VARARGS, "Get geometry element guid"}},
    {{"geom_element_name", py_ifcopenshell_ifcgeom_element_name, METH_VARARGS, "Get geometry element name"}},
    {{"geom_element_destroy", py_ifcopenshell_ifcgeom_element_destroy, METH_VARARGS, "Destroy geometry element"}},
    {{"geom_element_product", py_ifcopenshell_ifcgeom_element_product, METH_VARARGS, "Get geometry element product"}},
    {{"geom_element_context", py_ifcopenshell_ifcgeom_element_context, METH_VARARGS, "Get geometry element context"}},
    {{"geom_element_transformation", py_ifcopenshell_ifcgeom_element_transformation, METH_VARARGS, "Get geometry element transformation"}},
    {{"geom_triangulation_element_geometry", py_ifcopenshell_ifcgeom_triangulation_element_geometry, METH_VARARGS, "Get triangulation geometry"}},
    {{"geom_triangulation_verts", py_ifcopenshell_ifcgeom_triangulation_verts, METH_VARARGS, "Get triangulation verts"}},
    {{"geom_triangulation_faces", py_ifcopenshell_ifcgeom_triangulation_faces, METH_VARARGS, "Get triangulation faces"}},
    {{"geom_triangulation_normals", py_ifcopenshell_ifcgeom_triangulation_normals, METH_VARARGS, "Get triangulation normals"}},
    {{"geom_triangulation_uvs", py_ifcopenshell_ifcgeom_triangulation_uvs, METH_VARARGS, "Get triangulation uvs"}},
    {{"geom_triangulation_destroy", py_ifcopenshell_ifcgeom_triangulation_destroy, METH_VARARGS, "Destroy triangulation"}},
    {{"geom_triangulation_material_count", py_ifcopenshell_ifcgeom_triangulation_material_count, METH_VARARGS, "Get triangulation material count"}},
    {{"geom_create_shape", py_ifcopenshell_ifcgeom_create_shape, METH_VARARGS, "Create geometry shape"}},
    {{NULL, NULL, 0, NULL}}
}};

static PyModuleDef_Slot module_slots[] = {{
    {{0, NULL}}
}};

static struct PyModuleDef moduledef = {{
    PyModuleDef_HEAD_INIT,
    "_ifcopenshell_capi",
    "CPython C extension wrapping the ifcopenshell C API",
    -1,
    module_methods,
    module_slots,
    NULL,
    NULL,
    NULL
}};

PyMODINIT_FUNC PyInit__ifcopenshell_capi(void) {{
{type_ready_block}

    PyObject *m = PyModule_Create(&moduledef);
    if (!m) return NULL;

    module_state = m;

{add_type_block}

{error_kind_constants}

    return m;
}}
"""


def generate_python_extension(ir: BindingIR, output_path: Path, api_header_path: Path | None = None) -> None:
    output_path.parent.mkdir(parents=True, exist_ok=True)
    output_path.write_text(
        render_python_extension(build_host_metadata(ir), api_header_path=api_header_path),
        encoding="utf-8",
    )
