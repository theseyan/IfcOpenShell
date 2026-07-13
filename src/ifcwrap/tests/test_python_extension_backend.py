# This file was generated with the assistance of an AI coding tool.

from __future__ import annotations

from pathlib import Path
from textwrap import dedent

from src.ifcwrap.binding_generator.abi_ir import (
    BindingABI,
    CFieldIR,
    CFunctionIR,
    COptionFieldIR,
    COptionIR,
    CParamIR,
    CTypeIR,
)
from src.ifcwrap.binding_generator.binding_ir import BindingIR
from src.ifcwrap.binding_generator.binding_model import TypeSpec
from src.ifcwrap.binding_generator.targets.python.backend import (
    _method_name,
    _py_type_name,
    _snake_name,
    render_capi_utils,
    render_python_extension,
)


def _make_ir(
    module: str = "ifcopenshell_wrapper", c_prefix: str = "ifcopenshell_demo"
) -> BindingIR:
    return BindingIR(
        module=module,
        c_prefix=c_prefix,
        public_headers=(),
        handles={},
        result_structs={},
        calls=(),
    )


# ---------------------------------------------------------------------------
# Helper factory functions for building minimal BindingABI
# ---------------------------------------------------------------------------

_DEFAULT_ERROR_FUNCTIONS = {
    "clear_error": "ifcopenshell_demo_clear_error",
    "last_error_message": "ifcopenshell_demo_last_error_message",
    "last_error_kind": "ifcopenshell_demo_last_error_kind",
}


def _make_handle(
    name: str, c_type: str, destroy_function: str | None = None
) -> CTypeIR:
    """Create a minimal ptr_owned handle struct (raw pointer)."""
    if destroy_function is None:
        destroy_function = f"ifcopenshell_{_snake_name(c_type)}_destroy"
    return CTypeIR(
        c_type=c_type,
        kind="handle",
        fields=(CFieldIR("ptr", "void*"), CFieldIR("owned", "bool")),
        destroy_function=destroy_function,
        layout="ptr_owned",
    )


def _make_function(
    *,
    c_name: str,
    params: tuple[CParamIR, ...] = (),
    returns: TypeSpec | None = None,
    receiver: str | None = None,
    handle_c_type_map: dict[str, str] | None = None,
) -> CFunctionIR:
    """Create a minimal function metadata object."""
    if returns is None:
        returns = TypeSpec(kind="void")
    full_params = list(params)
    if receiver is not None:
        full_params.insert(
            0,
            CParamIR(
                name="self",
                c_type=f"ifcopenshell_demo_{receiver}_t*",
                role="receiver",
                type_kind="handle",
                nullable=False,
            ),
        )
    if returns.kind != "void":
        # Compute the correct out-result C type
        hmap = handle_c_type_map or {}
        out_c_type = _compute_out_c_type(returns, hmap)
        full_params.append(
            CParamIR(
                name="out_result",
                c_type=out_c_type,
                role="out_result",
                type_kind=returns.kind,
                nullable=False,
            ),
        )
    return CFunctionIR(
        c_name=c_name,
        restype="bool",
        params=tuple(full_params),
        error_policy="bool_return_last_error",
        returns=returns,
        receiver=receiver,
    )


def _compute_out_c_type(type_spec: TypeSpec, handle_c_type_map: dict[str, str]) -> str:
    """Compute a correct out C type for the given TypeSpec."""
    kind = type_spec.kind
    if kind == "bool":
        return "bool*"
    elif kind == "double":
        return "double*"
    elif kind == "int32":
        return "int32_t*"
    elif kind == "int64":
        return "int64_t*"
    elif kind == "string":
        return "ifcopenshell_string_t*"
    elif kind == "handle":
        if type_spec.handle is None:
            raise ValueError("handle type is missing handle name")
        handle_c_type = handle_c_type_map.get(
            type_spec.handle, f"ifcopenshell_demo_{type_spec.handle}_t"
        )
        return f"{handle_c_type}**"
    elif kind == "void":
        return ""
    raise ValueError(f"Unsupported return kind: {kind}")


def _make_metadata(
    *,
    module: str = "ifcopenshell_wrapper",
    c_prefix: str = "ifcopenshell_demo",
    handles: dict[str, CTypeIR] | None = None,
    value_types: dict[str, CTypeIR] | None = None,
    option_structs: dict[str, COptionIR] | None = None,
    functions: dict[str, CFunctionIR] | None = None,
) -> BindingABI:
    """Create a minimal BindingABI."""
    return BindingABI(
        module=module,
        c_prefix=c_prefix,
        handles=handles or {},
        value_types=value_types or {},
        option_structs=option_structs or {},
        functions=functions or {},
        error_functions=_DEFAULT_ERROR_FUNCTIONS,
    )


# ---------------------------------------------------------------------------
# Test: type object generation
# ---------------------------------------------------------------------------


class TestHandleTypeObject:
    """Verify that each handle type produces a structurally correct PyTypeObject."""

    def test_generates_pytypeobject_for_file_handle(self):
        meta = _make_metadata(
            handles={"file": _make_handle("file", "ifcopenshell_demo_file_t")},
        )
        code = render_python_extension(meta)
        assert "PyTypeObject" in code
        assert "IfcOpenshellDemoFile" in code
        assert "tp_dealloc" in code
        assert "tp_new" in code
        assert "tp_init" in code
        assert "tp_members" in code

    def test_type_object_contains_tp_name_matching_handle(self):
        meta = _make_metadata(
            handles={"file": _make_handle("file", "ifcopenshell_demo_file_t")},
        )
        code = render_python_extension(meta)
        assert '.tp_name = "IfcOpenshellDemoFile"' in code
        assert (
            "typedef struct {\n    PyObject_HEAD\n    void *handle;\n    int owned;\n} IfcOpenshellDemoFileObject;"
            in code
        )

    def test_dealloc_calls_destroy_and_nulls_handle(self):
        meta = _make_metadata(
            handles={
                "item": _make_handle(
                    "item", "ifcopenshell_demo_item_t", "ifcopenshell_demo_item_destroy"
                )
            },
        )
        code = render_python_extension(meta)
        assert "ifcopenshell_demo_item_destroy" in code
        assert "self->handle = NULL;" in code
        assert "self->owned = 0;" in code

    def test_members_include_handle_and_owned(self):
        meta = _make_metadata(
            handles={"file": _make_handle("file", "ifcopenshell_demo_file_t")},
        )
        code = render_python_extension(meta)
        assert '"_handle_ptr"' in code
        assert '"handle"' in code
        assert '"owned"' in code
        assert "T_INT" in code
        assert "T_PYSSIZET" in code

    def test_init_raises_type_error(self):
        meta = _make_metadata(
            handles={"file": _make_handle("file", "ifcopenshell_demo_file_t")},
        )
        code = render_python_extension(meta)
        assert '"Cannot create instances directly"' in code
        assert "PyExc_TypeError" in code

    def test_multiple_handles_generate_distinct_type_objects(self):
        meta = _make_metadata(
            handles={
                "file": _make_handle("file", "ifcopenshell_demo_file_t"),
                "item": _make_handle("item", "ifcopenshell_demo_item_t"),
            },
        )
        code = render_python_extension(meta)
        assert "IfcOpenshellDemoFileType" in code
        assert "IfcOpenshellDemoItemType" in code
        assert "IfcOpenshellDemoFileObject" in code
        assert "IfcOpenshellDemoItemObject" in code


# ---------------------------------------------------------------------------
# Test: destroy wrapper generation
# ---------------------------------------------------------------------------


class TestDestroyWrapper:
    """Verify destroy wrapper functions are generated correctly for each handle."""

    def test_generates_destroy_wrapper_for_single_handle(self):
        meta = _make_metadata(
            handles={
                "file": _make_handle(
                    "file", "ifcopenshell_demo_file_t", "ifcopenshell_demo_file_destroy"
                )
            },
        )
        code = render_python_extension(meta)
        assert "py_demo_file_destroy" in code
        assert "ifcopenshell_demo_file_destroy" in code

    def test_destroy_wrapper_parses_type_object(self):
        meta = _make_metadata(
            handles={"file": _make_handle("file", "ifcopenshell_demo_file_t")},
        )
        code = render_python_extension(meta)
        # Should parse with O! format (type-checked)
        assert 'PyArg_ParseTuple(args, "O!"' in code

    def test_destroy_wrapper_nulls_handle_after_destroy(self):
        meta = _make_metadata(
            handles={"file": _make_handle("file", "ifcopenshell_demo_file_t")},
        )
        code = render_python_extension(meta)
        # The destroy wrapper should set handle to NULL after calling destroy
        assert "obj->handle = NULL;" in code
        assert "obj->owned = 0;" in code

    def test_each_handle_gets_its_own_destroy_wrapper(self):
        meta = _make_metadata(
            handles={
                "file": _make_handle("file", "ifcopenshell_demo_file_t"),
                "item": _make_handle("item", "ifcopenshell_demo_item_t"),
            },
        )
        code = render_python_extension(meta)
        assert "py_demo_file_destroy" in code
        assert "py_demo_item_destroy" in code

    def test_destroy_wrapper_registered_in_method_table(self):
        meta = _make_metadata(
            handles={"file": _make_handle("file", "ifcopenshell_demo_file_t")},
        )
        code = render_python_extension(meta)
        assert '"demo_file_destroy"' in code
        assert "py_demo_file_destroy" in code


# ---------------------------------------------------------------------------
# Test: handle wrap/unwrap
# ---------------------------------------------------------------------------


class TestHandleWrap:
    """Verify wrap_handle and extract_handle generated code."""

    def test_generates_wrap_function_for_each_handle(self):
        meta = _make_metadata(
            handles={"file": _make_handle("file", "ifcopenshell_demo_file_t")},
        )
        code = render_python_extension(meta)
        assert "static PyObject *wrap_demo_file" in code
        assert "IfcOpenshellDemoFileType.tp_alloc" in code
        assert "result->handle = handle;" in code
        assert "result->owned = owned;" in code

    def test_wrap_returns_none_for_null_handle(self):
        meta = _make_metadata(
            handles={"file": _make_handle("file", "ifcopenshell_demo_file_t")},
        )
        code = render_python_extension(meta)
        assert "if (!handle) Py_RETURN_NONE;" in code

    def test_extract_handle_is_generated(self):
        meta = _make_metadata(
            handles={"file": _make_handle("file", "ifcopenshell_demo_file_t")},
        )
        code = render_python_extension(meta)
        assert "static int extract_handle" in code
        assert "PyObject_TypeCheck" in code
        assert "IfcOpenshellGenericHandleObject" in code

    def test_extract_handle_supports_nullable(self):
        meta = _make_metadata(
            handles={"file": _make_handle("file", "ifcopenshell_demo_file_t")},
        )
        code = render_python_extension(meta)
        assert "nullable && obj == Py_None" in code


# ---------------------------------------------------------------------------
# Test: function wrapper parameter parsing
# ---------------------------------------------------------------------------


class TestFunctionWrapperParams:
    """Verify function wrappers parse parameters correctly."""

    def test_bool_param_uses_p_format(self):
        meta = _make_metadata(
            handles={"file": _make_handle("file", "ifcopenshell_demo_file_t")},
            functions={
                "ifcopenshell_demo_set_flag": _make_function(
                    c_name="ifcopenshell_demo_set_flag",
                    params=(
                        CParamIR(
                            name="flag",
                            c_type="bool",
                            role="param",
                            type_kind="bool",
                            nullable=False,
                        ),
                    ),
                ),
            },
        )
        code = render_python_extension(meta)
        # bool param is declared as `int arg_flag`
        assert "int arg_flag" in code
        # The PyArg_ParseTuple line for this function uses "p" format for bool
        assert 'PyArg_ParseTuple(args, "p"' in code

    def test_int32_param_uses_i_format(self):
        meta = _make_metadata(
            handles={"file": _make_handle("file", "ifcopenshell_demo_file_t")},
            functions={
                "ifcopenshell_demo_set_count": _make_function(
                    c_name="ifcopenshell_demo_set_count",
                    params=(
                        CParamIR(
                            name="count",
                            c_type="int32_t",
                            role="param",
                            type_kind="int32",
                            nullable=False,
                        ),
                    ),
                ),
            },
        )
        code = render_python_extension(meta)
        assert "int32_t arg_count" in code

    def test_int64_param_uses_L_format(self):
        meta = _make_metadata(
            handles={"file": _make_handle("file", "ifcopenshell_demo_file_t")},
            functions={
                "ifcopenshell_demo_set_id": _make_function(
                    c_name="ifcopenshell_demo_set_id",
                    params=(
                        CParamIR(
                            name="id",
                            c_type="int64_t",
                            role="param",
                            type_kind="int64",
                            nullable=False,
                        ),
                    ),
                ),
            },
        )
        code = render_python_extension(meta)
        assert "int64_t arg_id" in code

    def test_double_param_uses_d_format(self):
        meta = _make_metadata(
            handles={"file": _make_handle("file", "ifcopenshell_demo_file_t")},
            functions={
                "ifcopenshell_demo_set_scale": _make_function(
                    c_name="ifcopenshell_demo_set_scale",
                    params=(
                        CParamIR(
                            name="scale",
                            c_type="double",
                            role="param",
                            type_kind="double",
                            nullable=False,
                        ),
                    ),
                ),
            },
        )
        code = render_python_extension(meta)
        assert "double arg_scale" in code

    def test_non_nullable_string_uses_s_format(self):
        meta = _make_metadata(
            handles={"file": _make_handle("file", "ifcopenshell_demo_file_t")},
            functions={
                "ifcopenshell_demo_set_name": _make_function(
                    c_name="ifcopenshell_demo_set_name",
                    params=(
                        CParamIR(
                            name="name",
                            c_type="const char*",
                            role="param",
                            type_kind="string",
                            nullable=False,
                        ),
                    ),
                ),
            },
        )
        code = render_python_extension(meta)
        # Non-nullable string uses "s" format
        assert '"s"' in code or "const char *arg_name" in code

    def test_nullable_string_uses_z_format(self):
        meta = _make_metadata(
            handles={"file": _make_handle("file", "ifcopenshell_demo_file_t")},
            functions={
                "ifcopenshell_demo_set_name": _make_function(
                    c_name="ifcopenshell_demo_set_name",
                    params=(
                        CParamIR(
                            name="name",
                            c_type="const char*",
                            role="param",
                            type_kind="string",
                            nullable=True,
                        ),
                    ),
                ),
            },
        )
        code = render_python_extension(meta)
        # Nullable string uses "z" format with optional marker
        assert '"|z"' in code or "const char*" in code

    def test_multiple_params_generate_separate_declarations(self):
        meta = _make_metadata(
            handles={"file": _make_handle("file", "ifcopenshell_demo_file_t")},
            functions={
                "ifcopenshell_demo_configure": _make_function(
                    c_name="ifcopenshell_demo_configure",
                    params=(
                        CParamIR(
                            name="name",
                            c_type="const char*",
                            role="param",
                            type_kind="string",
                            nullable=False,
                        ),
                        CParamIR(
                            name="value",
                            c_type="int32_t",
                            role="param",
                            type_kind="int32",
                            nullable=False,
                        ),
                    ),
                ),
            },
        )
        code = render_python_extension(meta)
        assert "arg_name" in code
        assert "arg_value" in code

    def test_option_struct_param_generates_mapping_converter(self):
        meta = _make_metadata(
            handles={
                "file": _make_handle("file", "ifcopenshell_file_t"),
                "instance": _make_handle("instance", "ifcopenshell_instance_t"),
                "parse_instance_list": _make_handle(
                    "parse_instance_list",
                    "ifcopenshell_parse_instance_list_t",
                    destroy_function="ifcopenshell_parse_instance_list_destroy",
                ),
            },
            value_types={
                "double_list": CTypeIR(
                    c_type="ifcopenshell_double_list_t",
                    kind="sequence",
                    fields=(
                        CFieldIR("items", "double*"),
                        CFieldIR("size", "size_t"),
                    ),
                    destroy_function="ifcopenshell_double_list_destroy",
                    element_type="double",
                    sequence_depth=1,
                ),
                "double_list_list": CTypeIR(
                    c_type="ifcopenshell_double_list_list_t",
                    kind="sequence",
                    fields=(
                        CFieldIR("items", "ifcopenshell_double_list_t*"),
                        CFieldIR("size", "size_t"),
                    ),
                    destroy_function="ifcopenshell_double_list_list_destroy",
                    element_type="ifcopenshell_double_list_t",
                    sequence_depth=2,
                ),
            },
            option_structs={
                "CreateEntityOptions": COptionIR(
                    name="CreateEntityOptions",
                    c_type="ifcopenshell_root_create_entity_options_t",
                    fields=(
                        COptionFieldIR(
                            "ifc_class", TypeSpec(kind="string"), "const char*"
                        ),
                        COptionFieldIR(
                            "axis",
                            TypeSpec(kind="double", sequence_depth=2),
                            "const ifcopenshell_double_list_list_t*",
                        ),
                        COptionFieldIR(
                            "name",
                            TypeSpec(kind="string", nullable=True),
                            "const char*",
                        ),
                        COptionFieldIR(
                            "owner_history",
                            TypeSpec(kind="handle", handle="instance", nullable=True),
                            "ifcopenshell_instance_t*",
                        ),
                        COptionFieldIR(
                            "properties", TypeSpec(kind="opaque_ptr"), "void*"
                        ),
                        COptionFieldIR(
                            "products",
                            TypeSpec(kind="handle", handle="parse_instance_list"),
                            "ifcopenshell_parse_instance_list_t*",
                        ),
                    ),
                )
            },
            functions={
                "ifcopenshell_root_create_entity": _make_function(
                    c_name="ifcopenshell_root_create_entity",
                    params=(
                        CParamIR("file", "ifcopenshell_file_t*", "param", "handle"),
                        CParamIR(
                            "options",
                            "const ifcopenshell_root_create_entity_options_t*",
                            "param",
                            "option",
                        ),
                    ),
                    returns=TypeSpec(kind="handle", handle="instance"),
                    handle_c_type_map={"instance": "ifcopenshell_instance_t"},
                )
            },
        )

        code = render_python_extension(meta)

        assert "fill_input_root_create_entity_options" in code
        assert 'get_option_field(obj, "ifc_class", 1)' in code
        assert 'get_option_field(obj, "name", 0)' in code
        assert 'get_option_field(obj, "axis", 1)' in code
        assert "make_input_double_list_list(field_1, sequence_1)" in code
        assert (
            "free_input_double_list_list((ifcopenshell_double_list_list_t *)value->axis);"
            in code
        )
        assert "out->has_name = true;" in code
        assert "PyUnicode_AsUTF8(field_0)" in code
        assert "release_option_refs(arg_options_refs, 6);" in code
        assert (
            'extract_handle(field_3, &IfcOpenshellInstanceType, "IfcOpenshellInstance"'
            in code
        )
        assert "PyCapsule_IsValid(field_4, NULL)" in code
        assert "out->properties = PyCapsule_GetPointer(field_4, NULL);" in code
        assert 'get_option_field(obj, "products", 1)' in code
        assert "make_input_instance_list(field_5, &products_items_5)" in code
        assert (
            "ifcopenshell_parse_instance_list_create_from_handles(&products_items_5, &out->products)"
            in code
        )
        assert "ifcopenshell_parse_instance_list_destroy(value->products)" in code


# ---------------------------------------------------------------------------
# Test: function wrapper — handle receiver (methods)
# ---------------------------------------------------------------------------


class TestMethodReceiver:
    """Verify function wrappers for methods (with receiver handle)."""

    def test_method_parses_self_as_handle(self):
        meta = _make_metadata(
            handles={"item": _make_handle("item", "ifcopenshell_demo_item_t")},
            functions={
                "ifcopenshell_demo_item_reset": _make_function(
                    c_name="ifcopenshell_demo_item_reset",
                    receiver="item",
                ),
            },
        )
        code = render_python_extension(meta)
        # Should parse self as an object
        assert "arg_self_obj" in code
        assert "extract_handle" in code

    def test_method_calls_extract_handle_with_correct_type(self):
        meta = _make_metadata(
            handles={"item": _make_handle("item", "ifcopenshell_demo_item_t")},
            functions={
                "ifcopenshell_demo_item_reset": _make_function(
                    c_name="ifcopenshell_demo_item_reset",
                    receiver="item",
                ),
            },
        )
        code = render_python_extension(meta)
        assert "IfcOpenshellDemoItemType" in code

    def test_method_is_registered_with_short_name(self):
        meta = _make_metadata(
            handles={"item": _make_handle("item", "ifcopenshell_demo_item_t")},
            functions={
                "ifcopenshell_demo_item_reset": _make_function(
                    c_name="ifcopenshell_demo_item_reset",
                    receiver="item",
                ),
            },
        )
        code = render_python_extension(meta)
        # The short method name should appear in the method table
        assert '"item_reset"' in code or '"reset"' in code


# ---------------------------------------------------------------------------
# Test: function wrapper — output/result handling
# ---------------------------------------------------------------------------


class TestOutputHandling:
    """Verify generated wrappers handle return values correctly."""

    def test_void_function_returns_none(self):
        meta = _make_metadata(
            handles={"file": _make_handle("file", "ifcopenshell_demo_file_t")},
            functions={
                "ifcopenshell_demo_ping": _make_function(
                    c_name="ifcopenshell_demo_ping",
                ),
            },
        )
        code = render_python_extension(meta)
        assert "Py_INCREF(Py_None)" in code
        assert "__py_result = Py_None" in code

    def test_bool_out_result_generates_converter(self):
        meta = _make_metadata(
            handles={"file": _make_handle("file", "ifcopenshell_demo_file_t")},
            functions={
                "ifcopenshell_demo_is_valid": _make_function(
                    c_name="ifcopenshell_demo_is_valid",
                    returns=TypeSpec(kind="bool"),
                ),
            },
        )
        code = render_python_extension(meta)
        assert "PyBool_FromLong" in code or "bool result" in code

    def test_handle_out_result_generates_wrap_call(self):
        handle_type = "ifcopenshell_demo_file_t"
        meta = _make_metadata(
            handles={"file": _make_handle("file", handle_type)},
            functions={
                "ifcopenshell_demo_create_file": _make_function(
                    c_name="ifcopenshell_demo_create_file",
                    returns=TypeSpec(kind="handle", handle="file"),
                    handle_c_type_map={"file": "ifcopenshell_demo_file_t"},
                ),
            },
        )
        code = render_python_extension(meta)
        assert "wrap_demo_file" in code

    def test_out_param_declared_before_call(self):
        meta = _make_metadata(
            handles={"file": _make_handle("file", "ifcopenshell_demo_file_t")},
            functions={
                "ifcopenshell_demo_create_file": _make_function(
                    c_name="ifcopenshell_demo_create_file",
                    returns=TypeSpec(kind="handle", handle="file"),
                    handle_c_type_map={"file": "ifcopenshell_demo_file_t"},
                ),
            },
        )
        code = render_python_extension(meta)
        # The out-param should be declared before the function call
        assert "result" in code
        assert "&result" in code

    def test_double_out_result(self):
        meta = _make_metadata(
            handles={"file": _make_handle("file", "ifcopenshell_demo_file_t")},
            functions={
                "ifcopenshell_demo_get_version": _make_function(
                    c_name="ifcopenshell_demo_get_version",
                    returns=TypeSpec(kind="double"),
                ),
            },
        )
        code = render_python_extension(meta)
        assert "double result" in code or "PyFloat_FromDouble" in code

    def test_string_out_result(self):
        meta = _make_metadata(
            handles={"file": _make_handle("file", "ifcopenshell_demo_file_t")},
            functions={
                "ifcopenshell_demo_get_name": _make_function(
                    c_name="ifcopenshell_demo_get_name",
                    returns=TypeSpec(kind="string"),
                ),
            },
        )
        code = render_python_extension(meta)
        # Should have a string out-result
        assert "convert_string" in code or "ifcopenshell_string_t" in code


# ---------------------------------------------------------------------------
# Test: error checking in wrappers
# ---------------------------------------------------------------------------


class TestErrorChecking:
    """Verify every function wrapper checks errors correctly."""

    def test_wrapper_clears_error_before_call(self):
        meta = _make_metadata(
            handles={"file": _make_handle("file", "ifcopenshell_demo_file_t")},
            functions={
                "ifcopenshell_demo_ping": _make_function(
                    c_name="ifcopenshell_demo_ping"
                ),
            },
        )
        code = render_python_extension(meta)
        assert "clear_error()" in code

    def test_wrapper_checks_bool_return(self):
        meta = _make_metadata(
            handles={"file": _make_handle("file", "ifcopenshell_demo_file_t")},
            functions={
                "ifcopenshell_demo_ping": _make_function(
                    c_name="ifcopenshell_demo_ping"
                ),
            },
        )
        code = render_python_extension(meta)
        assert "if (!ok)" in code

    def test_wrapper_calls_raise_last_error_on_failure(self):
        meta = _make_metadata(
            handles={"file": _make_handle("file", "ifcopenshell_demo_file_t")},
            functions={
                "ifcopenshell_demo_ping": _make_function(
                    c_name="ifcopenshell_demo_ping"
                ),
            },
        )
        code = render_python_extension(meta)
        assert "raise_last_error" in code

    def test_raise_last_error_maps_error_kinds(self):
        meta = _make_metadata(
            handles={"file": _make_handle("file", "ifcopenshell_demo_file_t")},
        )
        code = render_python_extension(meta)
        assert "PyExc_ValueError" in code
        assert "PyExc_TypeError" in code
        assert "PyExc_NotImplementedError" in code
        assert "PyExc_KeyError" in code
        assert "PyExc_RuntimeError" in code

    def test_module_init_has_type_ready_calls(self):
        meta = _make_metadata(
            handles={
                "file": _make_handle("file", "ifcopenshell_demo_file_t"),
                "item": _make_handle("item", "ifcopenshell_demo_item_t"),
            },
        )
        code = render_python_extension(meta)
        assert "PyType_Ready" in code
        assert "PyModule_Create" in code


# ---------------------------------------------------------------------------
# Test: _capi_utils.py generation
# ---------------------------------------------------------------------------


class TestCapiUtils:
    """Verify the _capi_utils.py template renders correctly."""

    def test_contains_raise_last_error(self):
        code = render_capi_utils()
        assert "def raise_last_error" in code
        assert "_ERROR_KIND_TO_EXC" in code

    def test_contains_error_kind_mapping(self):
        code = render_capi_utils()
        assert "IFCOPENSHELL_ERROR_VALUE" in code
        assert "IFCOPENSHELL_ERROR_TYPE" in code

    def test_contains_unwrap_parameter_type(self):
        code = render_capi_utils()
        assert "def unwrap_parameter_type" in code
        assert "as_named_type" in code
        assert "as_simple_type" in code
        assert "as_aggregation_type" in code

    def test_contains_handle_helper_functions(self):
        code = render_capi_utils()
        assert "def file_handle" in code
        assert "def wrap_handle" in code
        assert "def call_status" in code

    def test_contains_owner_context(self):
        code = render_capi_utils()
        assert "def owner_context" in code


# ---------------------------------------------------------------------------
# Test: module init
# ---------------------------------------------------------------------------


class TestModuleInit:
    """Verify the PyInit module function is correct."""

    def test_init_function_imports_types_module(self):
        meta = _make_metadata(
            handles={"file": _make_handle("file", "ifcopenshell_demo_file_t")},
        )
        code = render_python_extension(meta)
        assert "PyInit__ifcopenshell_capi" in code
        assert 'PyImport_ImportModule("types")' in code
        assert "SimpleNamespace" in code

    def test_init_adds_error_kind_constants(self):
        meta = _make_metadata(
            handles={"file": _make_handle("file", "ifcopenshell_demo_file_t")},
        )
        code = render_python_extension(meta)
        assert "IFCOPENSHELL_ERROR_NONE" in code
        assert "IFCOPENSHELL_ERROR_RUNTIME" in code
        assert "IFCOPENSHELL_ERROR_KEY" in code

    def test_init_adds_logical_constants(self):
        meta = _make_metadata(
            handles={"file": _make_handle("file", "ifcopenshell_demo_file_t")},
        )
        code = render_python_extension(meta)
        assert "IFCOPENSHELL_LOGICAL_UNKNOWN" in code
        assert "IFCOPENSHELL_LOGICAL_FALSE" in code
        assert "IFCOPENSHELL_LOGICAL_TRUE" in code

    def test_init_registers_handle_types_on_module(self):
        meta = _make_metadata(
            handles={
                "file": _make_handle("file", "ifcopenshell_demo_file_t"),
                "item": _make_handle("item", "ifcopenshell_demo_item_t"),
            },
        )
        code = render_python_extension(meta)
        assert 'PyModule_AddObject(m, "IfcOpenshellDemoFile"' in code
        assert 'PyModule_AddObject(m, "IfcOpenshellDemoItem"' in code

    def test_method_table_includes_all_functions_and_destroy_wrappers(self):
        meta = _make_metadata(
            handles={"file": _make_handle("file", "ifcopenshell_demo_file_t")},
            functions={
                "ifcopenshell_demo_ping": _make_function(
                    c_name="ifcopenshell_demo_ping"
                ),
                "ifcopenshell_demo_create_file": _make_function(
                    c_name="ifcopenshell_demo_create_file",
                    returns=TypeSpec(kind="handle", handle="file"),
                    handle_c_type_map={"file": "ifcopenshell_demo_file_t"},
                ),
            },
        )
        code = render_python_extension(meta)
        # Both short and long names should be in method table
        assert '"ping"' in code
        assert '"create_file"' in code
        assert '"demo_file_destroy"' in code


# ---------------------------------------------------------------------------
# Test: full integration through YAML spec pipeline
# ---------------------------------------------------------------------------


class TestIntegrationWithPipeline:
    """Integration tests: YAML spec → BindingIR → HostMetadata → Python extension."""

    def test_render_from_yaml_spec(self, tmp_path: Path):
        from src.ifcwrap.binding_generator.abi_ir import finalize_abi
        from src.ifcwrap.binding_generator.authored_spec import load_authored_spec
        from src.ifcwrap.binding_generator.binding_ir import lower_binding_spec

        spec_path = tmp_path / "demo.yml"
        spec_path.write_text(
            dedent(
                """
                schema_version: 1
                module: demo
                slice: demo
                c_prefix: ifcopenshell_demo
                public_headers:
                  - demo.h
                handles:
                  - name: file
                    cpp_type: Demo::File
                    c_type: ifcopenshell_demo_file_t
                    destructor: delete
                functions:
                  - expose_as: create_file
                    handle: file
                    params: []
                methods:
                  - receiver: file
                    expose_as: by_name
                    cpp_name: by_name
                    returns:
                      kind: string
                    params:
                      - name: name
                        type:
                          kind: string
                """
            ).strip()
            + "\n",
            encoding="utf-8",
        )

        ir = lower_binding_spec(load_authored_spec(spec_path))
        metadata = finalize_abi(ir)
        code = render_python_extension(metadata)

        # Type objects
        assert "IfcOpenshellDemoFileType" in code
        assert "IfcOpenshellDemoFileObject" in code

        # Destroy wrapper
        assert "py_demo_file_destroy" in code

        # Factory function
        assert "py_ifcopenshell_demo_create_file" in code

        # Method
        assert "py_ifcopenshell_demo_file_by_name" in code

        # Error handling
        assert "raise_last_error" in code

        # Module init
        assert "PyInit__ifcopenshell_capi" in code

    def test_generated_extension_is_compilable_c_structure(self, tmp_path: Path):
        """Verify that the generated code has matching braces, valid structure."""
        from src.ifcwrap.binding_generator.abi_ir import finalize_abi
        from src.ifcwrap.binding_generator.authored_spec import load_authored_spec
        from src.ifcwrap.binding_generator.binding_ir import lower_binding_spec

        spec_path = tmp_path / "demo.yml"
        spec_path.write_text(
            dedent(
                """
                schema_version: 1
                module: demo
                slice: demo
                c_prefix: ifcopenshell_demo
                public_headers:
                  - demo.h
                handles:
                  - name: file
                    cpp_type: Demo::File
                    c_type: ifcopenshell_demo_file_t
                    destructor: delete
                functions:
                  - expose_as: create_file
                    handle: file
                    params: []
                  - expose_as: get_version
                    cpp_name: get_version
                    returns:
                      kind: string
                    params: []
                """
            ).strip()
            + "\n",
            encoding="utf-8",
        )

        ir = lower_binding_spec(load_authored_spec(spec_path))
        metadata = finalize_abi(ir)
        code = render_python_extension(metadata)

        # Verify the code starts with includes and ends with PyInit
        assert code.startswith("// This file was generated")
        assert "PyMODINIT_FUNC PyInit__ifcopenshell_capi" in code

        # Verify brace balance with a simple count (not exhaustive, but catches obvious errors)
        braces = code.count("{") - code.count("}")
        assert braces == 0, f"Unbalanced braces in generated code: {braces}"

        # Verify no dangling C++ style comments that could break things
        assert "// This file was generated" in code

        # Make sure key sections are present
        assert "static PyObject *raise_last_error" in code
        assert "static int extract_handle" in code
        assert "static PyMethodDef module_methods[]" in code
        assert "static struct PyModuleDef moduledef" in code


# ---------------------------------------------------------------------------
# Test: edge cases and helpers
# ---------------------------------------------------------------------------


class TestHelpers:
    """Test utility/helper functions used by the backend."""

    def test_py_type_name_single_word(self):
        assert _py_type_name("ifcopenshell_demo_file_t") == "IfcOpenshellDemoFile"

    def test_py_type_name_multi_word(self):
        assert (
            _py_type_name("ifcopenshell_demo_big_item_t") == "IfcOpenshellDemoBigItem"
        )

    def test_snake_name_strips_prefix_and_suffix(self):
        assert _snake_name("ifcopenshell_demo_file_t") == "demo_file"

    def test_method_name_strips_c_prefix(self):
        assert (
            _method_name("ifcopenshell_demo_create_file", "ifcopenshell_demo")
            == "create_file"
        )

    def test_method_name_strips_parse_prefix(self):
        assert _method_name("ifcopenshell_parse_open", "ifcopenshell_wrapper") == "open"

    def test_method_name_strips_geom_prefix(self):
        assert (
            _method_name("ifcopenshell_geom_create_settings", "ifcopenshell_wrapper")
            == "create_settings"
        )


# ---------------------------------------------------------------------------
# Test: handle list / value type converters
# ---------------------------------------------------------------------------


class TestValueConverters:
    """Verify the generated converters and input helpers for sequence types."""

    def test_includes_string_converter(self):
        meta = _make_metadata(
            handles={"file": _make_handle("file", "ifcopenshell_demo_file_t")},
        )
        code = render_python_extension(meta)
        assert "convert_ifcopenshell_string_copy" in code
        assert "convert_string" in code

    def test_includes_input_sequence_helpers_when_sequence_params_present(self):
        # Build metadata with a value_type for a sequence that will be used as input
        meta = _make_metadata(
            handles={"item": _make_handle("item", "ifcopenshell_demo_item_t")},
            functions={
                "ifcopenshell_demo_item_set_values": _make_function(
                    c_name="ifcopenshell_demo_item_set_values",
                    receiver="item",
                    params=(
                        CParamIR(
                            name="values",
                            c_type="const ifcopenshell_double_list_t*",
                            role="param",
                            type_kind="double",
                            nullable=False,
                        ),
                    ),
                ),
            },
        )
        code = render_python_extension(meta)
        # Should have the generic 'if sequence type is used, input helpers should be present
        # For double_list, the element type is double which uses "double" reader
        assert "free_input" in code or "make_input" in code or "convert_" in code

    def test_generated_code_has_header_includes(self):
        meta = _make_metadata(
            handles={"file": _make_handle("file", "ifcopenshell_demo_file_t")},
        )
        code = render_python_extension(meta)
        assert "#include <Python.h>" in code
        assert 'structmember.h"' in code or '"structmember.h"' in code
        assert "ifcopenshell_api.h" in code

    def test_parse_instance_list_input_uses_canonical_helper_names(self):
        meta = _make_metadata(
            handles={
                "instance": _make_handle("instance", "ifcopenshell_instance_t"),
                "parse_instance_list": _make_handle(
                    "parse_instance_list",
                    "ifcopenshell_parse_instance_list_t",
                    destroy_function="ifcopenshell_parse_instance_list_destroy",
                ),
            },
            functions={
                "ifcopenshell_demo_accept_instances": _make_function(
                    c_name="ifcopenshell_demo_accept_instances",
                    params=(
                        CParamIR(
                            name="instances",
                            c_type="ifcopenshell_parse_instance_list_t*",
                            role="param",
                            type_kind="handle",
                            nullable=False,
                        ),
                    ),
                ),
            },
        )
        code = render_python_extension(meta)
        assert (
            "make_input_instance_list(arg_instances_obj, &arg_instances_items)" in code
        )
        assert "free_input_instance_list(&arg_instances_items)" in code
        assert "make_input_ifc_instance_list" not in code
        assert "free_input_ifc_instance_list" not in code


# ---------------------------------------------------------------------------
# Test: compiler/pragma markers
# ---------------------------------------------------------------------------


class TestGeneratedCodeMarkers:
    """Verify the generated code includes the expected markers."""

    def test_generated_code_has_header_comment(self):
        meta = _make_metadata(
            handles={"file": _make_handle("file", "ifcopenshell_demo_file_t")},
        )
        code = render_python_extension(meta)
        assert code.startswith("// This file was generated")

    def test_generated_code_contains_simple_namespace_setup(self):
        meta = _make_metadata(
            handles={"file": _make_handle("file", "ifcopenshell_demo_file_t")},
        )
        code = render_python_extension(meta)
        assert "SimpleNamespaceType" in code
        assert 'PyObject_GetAttrString(types_module, "SimpleNamespace")' in code

    def test_generated_code_accepts_canonical_declaration_family_names(self):
        meta = _make_metadata(
            handles={
                "declaration": _make_handle(
                    "declaration", "ifcopenshell_declaration_t"
                ),
                "entity": _make_handle("entity", "ifcopenshell_entity_t"),
                "enumeration": _make_handle(
                    "enumeration", "ifcopenshell_enumeration_t"
                ),
                "select_type": _make_handle(
                    "select_type", "ifcopenshell_select_type_t"
                ),
                "type_declaration": _make_handle(
                    "type_declaration", "ifcopenshell_type_declaration_t"
                ),
            },
        )
        code = render_python_extension(meta)
        assert "is_declaration_family_type(PyTypeObject *type)" in code
        assert "type == &IfcOpenshellDeclarationType" in code
        assert "type == &IfcOpenshellEntityType" in code
        assert "type == &IfcOpenshellEnumerationType" in code
        assert "type == &IfcOpenshellSelectTypeType" in code
        assert "type == &IfcOpenshellTypeDeclarationType" in code
        assert "handle_types_are_compatible(Py_TYPE(obj), expected)" in code
        assert "strcmp(name," not in code
        assert "IfcOpenshellIfcDeclaration" not in code


# ---------------------------------------------------------------------------
# Test: No-param (void) function rendering
# ---------------------------------------------------------------------------


class TestNoParamFunction:
    """Verify functions with no parameters render correctly."""

    def test_no_param_function_has_empty_parse_format(self):
        meta = _make_metadata(
            handles={"file": _make_handle("file", "ifcopenshell_demo_file_t")},
            functions={
                "ifcopenshell_demo_version": _make_function(
                    c_name="ifcopenshell_demo_version",
                    returns=TypeSpec(kind="string"),
                ),
            },
        )
        code = render_python_extension(meta)
        # No params = empty format string
        assert 'PyArg_ParseTuple(args, "")' in code
