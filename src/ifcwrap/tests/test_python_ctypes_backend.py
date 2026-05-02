# This file was generated with the assistance of an AI coding tool.

from __future__ import annotations

import ctypes
import importlib.util
from pathlib import Path
import shutil
import subprocess
import sys
from textwrap import dedent

import pytest

from src.ifcwrap.binding_generator.authored_spec import load_authored_spec
from src.ifcwrap.binding_generator.binding_ir import lower_binding_spec
from src.ifcwrap.binding_generator.python_ctypes_backend import generate_python_ctypes


def _write_fixture_spec(tmp_path: Path) -> Path:
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
            result_structs:
              - name: shape_result
                cpp_type: Demo::ShapeResult
                c_type: ifcopenshell_demo_shape_result_t
                fields:
                  - name: offsets
                    type: {kind: double_list}
            handles:
              - name: file
                cpp_type: Demo::File
                c_type: ifcopenshell_demo_file_t
                destructor: delete
            functions:
              - expose_as: create_file
                handle: file
                params:
                  - name: schema
                    type:
                      kind: string
            methods:
              - receiver: file
                expose_as: schema_name
                cpp_name: schema_name
                returns:
                  kind: string
                params: []
              - receiver: file
                expose_as: shape
                cpp_name: shape
                returns:
                  kind: struct
                  struct: shape_result
                params: []
            """
        ).strip()
        + "\n",
        encoding="utf-8",
    )
    return spec_path


def _load_generated_module(path: Path):
    spec = importlib.util.spec_from_file_location("generated_demo_ctypes", path)
    assert spec is not None
    assert spec.loader is not None
    module = importlib.util.module_from_spec(spec)
    sys.modules[spec.name] = module
    spec.loader.exec_module(module)
    return module


def test_render_python_ctypes_emits_structs_and_signatures(tmp_path: Path) -> None:
    ir = lower_binding_spec(load_authored_spec(_write_fixture_spec(tmp_path)))

    generated = tmp_path / "demo_ctypes.py"
    generate_python_ctypes(ir, generated)
    text = generated.read_text(encoding="utf-8")

    assert "class ifcopenshell_demo_file_t(ctypes.Structure):" in text
    assert text.index("class ifcopenshell_double_list_t(ctypes.Structure):") < text.index(
        "class ifcopenshell_demo_shape_result_t(ctypes.Structure):"
    )
    assert '("offsets", ifcopenshell_double_list_t)' in text
    assert '("ptr", ctypes.c_void_p)' in text
    assert '("data", ctypes.c_void_p)' in text
    assert '"ifcopenshell_demo_create_file": (ctypes.c_bool, [ctypes.c_char_p, ctypes.POINTER(ctypes.POINTER(ifcopenshell_demo_file_t))])' in text
    assert '"ifcopenshell_demo_file_schema_name": (ctypes.c_bool, [ctypes.POINTER(ifcopenshell_demo_file_t), ctypes.POINTER(ifcopenshell_string_t)])' in text
    assert "def status_or_raise(lib, status, fallback):" in text
    assert "def call_string(lib, fn, *args, decode=True, value_type=None):" in text
    assert "def call_string_list(lib, fn, *args, decode=True, value_type=None):" in text
    assert "def call_int32_list(lib, fn, *args):" in text
    assert "def call_uint32_list(lib, fn, *args):" in text
    assert "def call_double_list(lib, fn, *args):" in text
    assert "def call_string_or_raise(lib, fn, fallback, *args, decode=True, value_type=None):" in text
    assert "def call_string_list_or_raise(lib, fn, fallback, *args, decode=True, value_type=None):" in text
    assert "def call_int32_list_or_raise(lib, fn, fallback, *args):" in text
    assert "def call_uint32_list_or_raise(lib, fn, fallback, *args):" in text
    assert "def call_double_list_or_raise(lib, fn, fallback, *args):" in text
    assert "def call_scalar(fn, c_type, *args):" in text
    assert "def call_scalar_or_raise(lib, fn, c_type, fallback, *args):" in text
    assert "def take_nullable_handle(lib, value, *, destroy=None):" in text
    assert "def call_handle(lib, fn, *args, destroy=None, handle_pointer_type=None):" in text
    assert "def call_handle_or_raise(lib, fn, fallback, *args, destroy=None, handle_pointer_type=None):" in text
    assert "def take_string(lib, value, *, decode=True):" in text
    assert 'return "" if decode else b""' in text
    assert "def take_bool_list(lib, value):" in text
    assert "def make_int32_list(values):" in text
    assert "def make_int64_list(values):" in text
    assert "def make_double_list(values):" in text
    assert "def take_int32_list_list(lib, value):" in text
    assert "def take_double_list_list(lib, value):" in text
    assert "def make_int32_list_list(values):" in text
    assert "def make_double_list_list(values):" in text
    assert "def move_handle_list(lib, value, destroy, handle_pointer_type):" in text

    generic_generated = tmp_path / "demo_ctypes_generic.py"
    generate_python_ctypes(ir, generic_generated, generic_handles=True)
    generic_text = generic_generated.read_text(encoding="utf-8")

    assert "class _HandleStruct(ctypes.Structure):" in generic_text
    assert "ifcopenshell_demo_file_t = _HandleStruct" in generic_text
    assert '"ifcopenshell_demo_create_file": (ctypes.c_bool, [ctypes.c_char_p, ctypes.POINTER(ctypes.POINTER(_HandleStruct))])' in generic_text


def test_generated_python_ctypes_binds_and_calls_tiny_c_fixture(tmp_path: Path) -> None:
    compiler = shutil.which("cc") or shutil.which("clang")
    if compiler is None:
        pytest.skip("C compiler is not available")

    ir = lower_binding_spec(load_authored_spec(_write_fixture_spec(tmp_path)))
    generated = tmp_path / "demo_ctypes.py"
    generate_python_ctypes(ir, generated, generic_handles=True)
    module = _load_generated_module(generated)

    source = tmp_path / "demo.c"
    source.write_text(
        dedent(
            """
            #include <stdbool.h>
            #include <stddef.h>
            #include <stdlib.h>
            #include <string.h>

            typedef struct ifcopenshell_string_t {
                char* data;
                size_t size;
                bool owned;
            } ifcopenshell_string_t;

            typedef struct ifcopenshell_demo_file_t {
                void* ptr;
                bool owned;
            } ifcopenshell_demo_file_t;

            typedef struct ifcopenshell_double_list_t {
                double* items;
                size_t size;
            } ifcopenshell_double_list_t;

            typedef struct ifcopenshell_demo_shape_result_t {
                ifcopenshell_double_list_t offsets;
            } ifcopenshell_demo_shape_result_t;

            static const char* last_error = "";

            void ifcopenshell_demo_clear_error(void) { last_error = ""; }
            const char* ifcopenshell_demo_last_error_message(void) { return last_error; }

            bool ifcopenshell_demo_create_file(const char* schema, ifcopenshell_demo_file_t** out_result) {
                if (!schema || !out_result) {
                    last_error = "invalid arguments";
                    return false;
                }
                ifcopenshell_demo_file_t* file = (ifcopenshell_demo_file_t*)malloc(sizeof(ifcopenshell_demo_file_t));
                file->ptr = (void*)schema;
                file->owned = true;
                *out_result = file;
                return true;
            }

            bool ifcopenshell_demo_file_schema_name(ifcopenshell_demo_file_t* self, ifcopenshell_string_t* out_result) {
                if (!self || !out_result) {
                    last_error = "invalid arguments";
                    return false;
                }
                const char* value = "IFC4";
                out_result->size = 4;
                out_result->data = (char*)malloc(out_result->size);
                memcpy(out_result->data, value, out_result->size);
                out_result->owned = true;
                return true;
            }

            bool ifcopenshell_demo_file_shape(ifcopenshell_demo_file_t* self, ifcopenshell_demo_shape_result_t* out_result) {
                if (!self || !out_result) {
                    last_error = "invalid arguments";
                    return false;
                }
                out_result->offsets.items = NULL;
                out_result->offsets.size = 0;
                return true;
            }

            void ifcopenshell_demo_file_destroy(ifcopenshell_demo_file_t* handle) { free(handle); }
            void ifcopenshell_string_destroy(ifcopenshell_string_t* value) {
                if (value && value->owned) {
                    free(value->data);
                }
                if (value) {
                    value->data = NULL;
                    value->size = 0;
                    value->owned = false;
                }
            }
            void ifcopenshell_double_list_destroy(ifcopenshell_double_list_t* value) {
                if (value) {
                    free(value->items);
                    value->items = NULL;
                    value->size = 0;
                }
            }
            """
        ),
        encoding="utf-8",
    )
    library = tmp_path / ("demo.dll" if sys.platform == "win32" else "libdemo.dylib" if sys.platform == "darwin" else "libdemo.so")
    command = [compiler, "-shared", "-fPIC", str(source), "-o", str(library)]
    result = subprocess.run(command, cwd=tmp_path, capture_output=True, text=True, check=False)
    if result.returncode != 0:
        pytest.skip(f"Could not build C fixture: {result.stderr}")

    lib = module.bind(ctypes.CDLL(str(library)))
    assert module.bind(ctypes.CDLL(str(library)), strict=False)
    file_handle = ctypes.POINTER(module.ifcopenshell_demo_file_t)()
    assert lib.ifcopenshell_demo_create_file(b"IFC4", ctypes.byref(file_handle))
    assert file_handle

    value = module.ifcopenshell_string_t()
    assert lib.ifcopenshell_demo_file_schema_name(file_handle, ctypes.byref(value))
    assert module.take_string(lib, value) == "IFC4"

    with pytest.raises(RuntimeError, match="invalid arguments"):
        module.status_or_raise(lib, lib.ifcopenshell_demo_create_file(None, ctypes.byref(file_handle)), "fallback")
    lib.ifcopenshell_demo_file_destroy(file_handle)
