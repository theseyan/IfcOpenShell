# This file was generated with the assistance of an AI coding tool.

from __future__ import annotations

import json
from pathlib import Path
import shutil
from textwrap import dedent

import pytest

from src.ifcwrap.binding_generator.authored_spec import load_authored_spec, load_merged_specs
from src.ifcwrap.binding_generator.binding_model import HandleSpec
from src.ifcwrap.binding_generator.c_backend import generate, generate_merged
from src.ifcwrap.binding_generator.policy_ir import (
    BoolOutParamPolicyOp,
    ChildrenAddPolicyOp,
    ConstructorPolicyOp,
    DirectFieldPolicyOp,
    DirectFunctionPolicyOp,
    DirectMethodPolicyOp,
    InlineAdapterPolicyOp,
    OptionalGetPolicyOp,
    TaxonomyMakeFactoryPolicyOp,
    VariantGetPolicyOp,
)


def _write_compile_commands(tmp_path: Path, source: Path) -> Path:
    compiler = shutil.which("clang++")
    if compiler is None:
        pytest.skip("clang++ is not available")

    compile_commands = tmp_path / "compile_commands.json"
    compile_commands.write_text(
        json.dumps(
            [
                {
                    "directory": str(tmp_path),
                    "command": f"{compiler} -std=c++17 -I {tmp_path} -c {source}",
                    "file": str(source),
                }
            ]
        ),
        encoding="utf-8",
    )
    return compile_commands


def test_load_merged_specs_resolves_cross_slice_handles(tmp_path: Path) -> None:
    core_spec = tmp_path / "core.yml"
    geom_spec = tmp_path / "geom.yml"

    core_spec.write_text(
        dedent(
            """
            schema_version: 1
            module: demo
            slice: core
            c_prefix: ifcopenshell_demo
            public_headers:
              - core.h
            handles:
              - name: file
                cpp_type: Demo::File
                c_type: ifcopenshell_demo_file_t
                destructor: delete
            functions:
              - kind: constructor
                expose_as: create_file
                handle: file
                params: []
            """
        ).strip()
        + "\n",
        encoding="utf-8",
    )
    geom_spec.write_text(
        dedent(
            """
            schema_version: 1
            module: demo
            slice: geom
            c_prefix: ifcopenshell_demo
            public_headers:
              - geom.h
            handles:
              - name: serializer
                cpp_type: Demo::Serializer
                c_type: ifcopenshell_demo_serializer_t
                destructor: delete
            functions:
              - kind: constructor
                expose_as: create_serializer
                handle: serializer
                params:
                  - name: file
                    type:
                      kind: handle
                      handle: file
                      ownership: borrowed
                      cpp_type: Demo::File&
            """
        ).strip()
        + "\n",
        encoding="utf-8",
    )

    spec = load_merged_specs([core_spec, geom_spec], module="demo", c_prefix="ifcopenshell_demo")
    calls = {call.c_name: call for call in spec.functions}

    assert set(spec.handles) == {"file", "serializer"}
    assert calls["ifcopenshell_demo_create_serializer"].params[0].type.handle == "file"

    header_out = tmp_path / "demo_api.h"
    cpp_out = tmp_path / "demo_api.cpp"
    generate_merged([core_spec, geom_spec], "demo", "ifcopenshell_demo", header_out, cpp_out)

    header = header_out.read_text(encoding="utf-8")
    assert "bool ifcopenshell_demo_create_file(ifcopenshell_demo_file_t** out_result);" in header
    assert (
        "bool ifcopenshell_demo_create_serializer(ifcopenshell_demo_file_t* file, "
        "ifcopenshell_demo_serializer_t** out_result);"
    ) in header


def test_discovery_class_defaults_and_cpp_class_handle_resolution(tmp_path: Path) -> None:
    spec_path = tmp_path / "demo.yml"
    spec_path.write_text(
        dedent(
            """
            schema_version: 1
            module: demo
            slice: demo
            c_prefix: ifcopenshell_demo
            public_headers: []
            discover:
              include_dir: .
              class_defaults:
                include_all: false
                translation_unit: demo.cpp
              classes:
                - class: Demo::Widget
                  extra_fields:
                    value: int
            """
        ).strip()
        + "\n",
        encoding="utf-8",
    )

    spec = load_authored_spec(
        spec_path,
        existing_handles={
            "widget": HandleSpec(
                name="widget",
                cpp_type="Demo::Widget",
                c_type="ifcopenshell_demo_widget_t",
                destructor="delete",
            )
        },
    )

    assert spec.discovery is not None
    assert spec.discovery.classes[0].handle == "widget"
    assert spec.discovery.classes[0].translation_unit == "demo.cpp"
    assert any(call.receiver == "widget" and call.expose_as == "value" for call in spec.methods)


def test_constructor_handle_args_require_and_follow_cpp_type_passing_policy(tmp_path: Path) -> None:
    spec_path = tmp_path / "constructors.yml"
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
              - name: serializer
                cpp_type: Demo::Serializer
                c_type: ifcopenshell_demo_serializer_t
                destructor: delete
            functions:
              - kind: constructor
                expose_as: create_missing_policy_serializer
                handle: serializer
                params:
                  - name: file
                    type:
                      kind: handle
                      handle: file
                      ownership: borrowed
              - kind: constructor
                expose_as: create_pointer_serializer
                handle: serializer
                cpp_class: Demo::PointerSerializer
                params:
                  - name: file
                    type:
                      kind: handle
                      handle: file
                      ownership: borrowed
                      cpp_type: Demo::File*
              - kind: constructor
                expose_as: create_reference_serializer
                handle: serializer
                cpp_class: Demo::ReferenceSerializer
                params:
                  - name: file
                    type:
                      kind: handle
                      handle: file
                      ownership: borrowed
                      cpp_type: const Demo::File&
              - kind: constructor
                expose_as: create_value_serializer
                handle: serializer
                cpp_class: Demo::ValueSerializer
                params:
                  - name: file
                    type:
                      kind: handle
                      handle: file
                      ownership: borrowed
                      cpp_type: Demo::File
            """
        ).strip()
        + "\n",
        encoding="utf-8",
    )

    header_out = tmp_path / "demo_api.h"
    cpp_out = tmp_path / "demo_api.cpp"
    with pytest.raises(ValueError, match='Constructor handle parameter "file" requires cpp_type'):
        generate(spec_path, header_out, cpp_out)

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
              - name: serializer
                cpp_type: Demo::Serializer
                c_type: ifcopenshell_demo_serializer_t
                destructor: delete
            functions:
              - kind: constructor
                expose_as: create_pointer_serializer
                handle: serializer
                cpp_class: Demo::PointerSerializer
                params:
                  - name: file
                    type:
                      kind: handle
                      handle: file
                      ownership: borrowed
                      cpp_type: Demo::File*
              - kind: constructor
                expose_as: create_reference_serializer
                handle: serializer
                cpp_class: Demo::ReferenceSerializer
                params:
                  - name: file
                    type:
                      kind: handle
                      handle: file
                      ownership: borrowed
                      cpp_type: const Demo::File&
              - kind: constructor
                expose_as: create_value_serializer
                handle: serializer
                cpp_class: Demo::ValueSerializer
                params:
                  - name: file
                    type:
                      kind: handle
                      handle: file
                      ownership: borrowed
                      cpp_type: Demo::File
            """
        ).strip()
        + "\n",
        encoding="utf-8",
    )
    generate(spec_path, header_out, cpp_out)

    generated_cpp = cpp_out.read_text(encoding="utf-8")
    assert "auto file_cpp = file->ptr;" in generated_cpp
    assert "new Demo::PointerSerializer(file_cpp)" in generated_cpp
    assert "auto& file_cpp = *file->ptr;" in generated_cpp
    assert "new Demo::ReferenceSerializer(file_cpp)" in generated_cpp
    assert "auto file_cpp = *file->ptr;" in generated_cpp
    assert "new Demo::ValueSerializer(file_cpp)" in generated_cpp


def test_constructor_rejects_nullable_reference_handle_params(tmp_path: Path) -> None:
    spec_path = tmp_path / "nullable_reference.yml"
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
              - name: serializer
                cpp_type: Demo::Serializer
                c_type: ifcopenshell_demo_serializer_t
                destructor: delete
            functions:
              - kind: constructor
                expose_as: create_serializer
                handle: serializer
                params:
                  - name: file
                    type:
                      kind: handle
                      handle: file
                      ownership: borrowed
                      nullable: true
                      cpp_type: const Demo::File&
            """
        ).strip()
        + "\n",
        encoding="utf-8",
    )

    with pytest.raises(ValueError, match='cannot be nullable with reference cpp_type "const Demo::File&"'):
        generate(spec_path, tmp_path / "demo_api.h", tmp_path / "demo_api.cpp")


def test_guarded_constructor_discovery_uses_fallback_signature_when_unavailable(tmp_path: Path) -> None:
    include_dir = tmp_path / "include"
    include_dir.mkdir()
    (include_dir / "optional.hpp").write_text(
        dedent(
            """
            #include <string>

            namespace Demo {
            struct Settings {};
            #ifdef WITH_DEMO_OPTIONAL
            struct OptionalSerializer {
                OptionalSerializer(const std::string& filename, const Settings& settings);
            };
            #endif
            }
            """
        ).strip()
        + "\n",
        encoding="utf-8",
    )
    spec_path = tmp_path / "guarded_constructor.yml"
    spec_path.write_text(
        dedent(
            f"""
            schema_version: 1
            module: demo
            slice: demo
            c_prefix: ifcopenshell_demo
            public_headers:
              - demo.h
            handles:
              - name: serializer
                cpp_type: Demo::OptionalSerializer
                c_type: ifcopenshell_demo_serializer_t
                destructor: delete
              - name: settings
                cpp_type: Demo::Settings
                c_type: ifcopenshell_demo_settings_t
                destructor: delete
            discover:
              include_dir: {include_dir.as_posix()}
              constructors:
                - handle: serializer
                  cpp_class: Demo::OptionalSerializer
                  translation_unit: optional.hpp
                  expose_as: create_optional_serializer
                  compile_guard: WITH_DEMO_OPTIONAL
                  params:
                    - const std::string&
                    - const Demo::Settings&
                  param_names:
                    - filename
                    - settings
            """
        ).strip()
        + "\n",
        encoding="utf-8",
    )
    _write_compile_commands(tmp_path, include_dir / "optional.hpp")

    spec = load_authored_spec(spec_path, compile_commands_path=tmp_path / "compile_commands.json")
    assert len(spec.functions) == 1
    call = spec.functions[0]
    assert call.c_name == "ifcopenshell_demo_create_optional_serializer"
    assert [param.name for param in call.params] == ["filename", "settings"]
    assert call.params[1].type.handle == "settings"

    header_out = tmp_path / "demo_api.h"
    cpp_out = tmp_path / "demo_api.cpp"
    generate(spec_path, header_out, cpp_out, compile_commands_path=tmp_path / "compile_commands.json")
    cpp = cpp_out.read_text(encoding="utf-8")
    assert "#if defined(WITH_DEMO_OPTIONAL)" in cpp
    assert "new Demo::OptionalSerializer(filename_cpp, settings_cpp)" in cpp
    assert 'throw std::runtime_error("ifcopenshell_demo_create_optional_serializer requires WITH_DEMO_OPTIONAL");' in cpp


def test_handle_list_accessors_generate_count_and_at_methods(tmp_path: Path) -> None:
    spec_path = tmp_path / "list_accessors.yml"
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
              - name: tree
                cpp_type: Demo::Tree
                c_type: ifcopenshell_demo_tree_t
                destructor: delete
              - name: result_list
                cpp_type: std::vector<Demo::Result>
                c_type: ifcopenshell_demo_result_list_t
                destructor: delete
                list_accessors:
                  receiver: tree
                  list_param: results
                  item_handle: result
                  count_as: result_count
                  at_as: result_at
                  out_of_range_message: Result index out of range
              - name: result
                cpp_type: Demo::Result
                c_type: ifcopenshell_demo_result_t
                destructor: delete
            """
        ).strip()
        + "\n",
        encoding="utf-8",
    )

    spec = load_authored_spec(spec_path)
    methods = {call.c_name: call for call in spec.methods}
    assert set(methods) == {"ifcopenshell_demo_tree_result_count", "ifcopenshell_demo_tree_result_at"}
    assert methods["ifcopenshell_demo_tree_result_count"].params[0].type.handle == "result_list"
    assert methods["ifcopenshell_demo_tree_result_at"].returns.handle == "result"
    assert methods["ifcopenshell_demo_tree_result_at"].returns.cpp_type == "Demo::Result*"

    header_out = tmp_path / "demo_api.h"
    cpp_out = tmp_path / "demo_api.cpp"
    generate(spec_path, header_out, cpp_out)

    header = header_out.read_text(encoding="utf-8")
    cpp = cpp_out.read_text(encoding="utf-8")
    assert (
        "bool ifcopenshell_demo_tree_result_count(ifcopenshell_demo_tree_t* self, "
        "ifcopenshell_demo_result_list_t* results, size_t* out_result);"
    ) in header
    assert (
        "bool ifcopenshell_demo_tree_result_at(ifcopenshell_demo_tree_t* self, "
        "ifcopenshell_demo_result_list_t* results, size_t index, ifcopenshell_demo_result_t** out_result);"
    ) in header
    assert "*out_result = results_cpp->size();" in cpp
    assert 'throw std::out_of_range("Result index out of range");' in cpp
    assert "auto result_value = std::unique_ptr<Demo::Result>(new Demo::Result((*results_cpp)[index]));" in cpp
    assert "*out_result = new ifcopenshell_demo_result_t{result_value.release(), true};" in cpp


def test_top_level_handle_list_accessors_for_cpp_spec_handles(tmp_path: Path) -> None:
    spec_path = tmp_path / "cpp_handle_list_accessors.yml"
    spec_path.write_text(
        dedent(
            """
            schema_version: 1
            module: demo
            slice: demo
            c_prefix: ifcopenshell_demo
            public_headers:
              - demo.h
            handle_list_accessors:
              - list_handle: result_list
                receiver: tree
                list_param: results
                item_handle: result
                count_as: result_count
                at_as: result_at
                out_of_range_message: Result index out of range
            """
        ).strip()
        + "\n",
        encoding="utf-8",
    )

    existing_handles = {
        "tree": HandleSpec("tree", "Demo::Tree", "ifcopenshell_demo_tree_t", "delete"),
        "result_list": HandleSpec(
            "result_list", "std::vector<Demo::Result>", "ifcopenshell_demo_result_list_t", "delete"
        ),
        "result": HandleSpec("result", "Demo::Result", "ifcopenshell_demo_result_t", "delete"),
    }
    spec = load_authored_spec(spec_path, existing_handles=existing_handles)

    methods = {call.c_name: call for call in spec.methods}
    assert set(methods) == {"ifcopenshell_demo_tree_result_count", "ifcopenshell_demo_tree_result_at"}
    assert methods["ifcopenshell_demo_tree_result_count"].params[0].type.handle == "result_list"
    assert methods["ifcopenshell_demo_tree_result_at"].returns.handle == "result"


def test_method_at_accessors_generate_indexed_method_items(tmp_path: Path) -> None:
    include_dir = tmp_path / "include"
    include_dir.mkdir()
    (include_dir / "demo.hpp").write_text(
        dedent(
            """
            #include <memory>
            #include <vector>

            namespace Demo {
            struct Style {};
            struct Tree {
                const std::vector<std::shared_ptr<Style>>& styles() const;
            };
            }
            """
        ).strip()
        + "\n",
        encoding="utf-8",
    )
    spec_path = tmp_path / "method_at.yml"
    spec_path.write_text(
        dedent(
            f"""
            schema_version: 1
            module: demo
            slice: demo
            c_prefix: ifcopenshell_demo
            public_headers:
              - demo.h
            handles:
              - name: tree
                cpp_type: Demo::Tree
                c_type: ifcopenshell_demo_tree_t
                destructor: delete
              - name: style
                cpp_type: std::shared_ptr<Demo::Style>
                c_type: ifcopenshell_demo_style_t
                destructor: shared_ptr
                ptr_type: shared_ptr
            discover:
              include_dir: {include_dir.as_posix()}
              classes:
                - handle: tree
                  translation_unit: demo.hpp
                  method_at_accessors:
                    - method: styles
                      expose_as: style_at
                      item_handle: style
                      ownership: owned
                      out_of_range_message: Style index out of range
                      exception: std::runtime_error
            """
        ).strip()
        + "\n",
        encoding="utf-8",
    )
    _write_compile_commands(tmp_path, include_dir / "demo.hpp")

    spec = load_authored_spec(spec_path, compile_commands_path=tmp_path / "compile_commands.json")
    assert len(spec.methods) == 1
    call = spec.methods[0]
    assert call.c_name == "ifcopenshell_demo_tree_style_at"
    assert call.params[0].name == "index"
    assert call.returns.handle == "style"
    assert call.returns.ownership == "owned"

    header_out = tmp_path / "demo_api.h"
    cpp_out = tmp_path / "demo_api.cpp"
    generate(spec_path, header_out, cpp_out, compile_commands_path=tmp_path / "compile_commands.json")
    cpp = cpp_out.read_text(encoding="utf-8")
    assert "const auto& items = self_cpp->styles();" in cpp
    assert 'throw std::runtime_error("Style index out of range");' in cpp
    assert "*out_result = new ifcopenshell_demo_style_t{items[index]};" in cpp


def test_discovery_supports_public_constructors(tmp_path: Path) -> None:
    header = tmp_path / "constructors.h"
    source = tmp_path / "constructors.cpp"
    spec_path = tmp_path / "constructors.yml"

    header.write_text(
        dedent(
            """
            #include <string>

            namespace Demo {
            struct File {};

            struct Serializer {
                virtual ~Serializer() = default;
            };

            struct XmlSerializer : Serializer {
            public:
                XmlSerializer(File* file, const std::string& xml_filename) {}
                XmlSerializer(const XmlSerializer&) = default;
            };

            struct Tree {
            public:
                explicit Tree(int index) {}
                explicit Tree(File& file) {}
            };
            }
            """
        ).strip()
        + "\n",
        encoding="utf-8",
    )
    source.write_text('#include "constructors.h"\n', encoding="utf-8")
    compile_commands = _write_compile_commands(tmp_path, source)
    spec_path.write_text(
        dedent(
            """
            schema_version: 1
            module: demo
            slice: demo
            c_prefix: ifcopenshell_demo
            public_headers:
              - constructors.h
            handles:
              - name: file
                cpp_type: Demo::File
                c_type: ifcopenshell_demo_file_t
                destructor: delete
              - name: serializer
                cpp_type: Demo::Serializer
                c_type: ifcopenshell_demo_serializer_t
                destructor: delete
              - name: tree
                cpp_type: Demo::Tree
                c_type: ifcopenshell_demo_tree_t
                destructor: delete
            discover:
              include_dir: .
              constructors:
                - handle: serializer
                  cpp_class: Demo::XmlSerializer
                  translation_unit: constructors.cpp
                  expose_as: create_xml_serializer
                  param_renames:
                    xml_filename: filename
                - handle: tree
                  cpp_class: Demo::Tree
                  translation_unit: constructors.cpp
                  expose_as: create_tree_from_file
                  params:
                    - Demo::File&
            """
        ).strip()
        + "\n",
        encoding="utf-8",
    )

    spec = load_authored_spec(spec_path, compile_commands_path=compile_commands)
    calls = {call.c_name: call for call in spec.functions}

    xml_call = calls["ifcopenshell_demo_create_xml_serializer"]
    assert isinstance(xml_call.policy_operation, ConstructorPolicyOp)
    assert xml_call.policy_operation.cpp_class == "Demo::XmlSerializer"
    assert [param.name for param in xml_call.params] == ["file", "filename"]
    assert xml_call.params[0].type.cpp_type == "Demo::File*"
    assert xml_call.params[1].type.kind == "string"
    assert "ifcopenshell_demo_create_tree_from_file" in calls

    header_out = tmp_path / "demo_api.h"
    cpp_out = tmp_path / "demo_api.cpp"
    generate(spec_path, header_out, cpp_out, compile_commands_path=compile_commands)

    generated_cpp = cpp_out.read_text(encoding="utf-8")
    assert "new Demo::XmlSerializer(file_cpp, filename_cpp)" in generated_cpp
    assert "new Demo::Tree(file_cpp)" in generated_cpp
    assert "XmlSerializer(const XmlSerializer&" not in generated_cpp


def test_discovery_supports_implicit_default_constructors(tmp_path: Path) -> None:
    header = tmp_path / "constructors.h"
    source = tmp_path / "constructors.cpp"
    spec_path = tmp_path / "constructors.yml"

    header.write_text(
        dedent(
            """
            namespace Demo {
            class Settings {};
            }
            """
        ).strip()
        + "\n",
        encoding="utf-8",
    )
    source.write_text('#include "constructors.h"\n', encoding="utf-8")
    compile_commands = _write_compile_commands(tmp_path, source)
    spec_path.write_text(
        dedent(
            """
            schema_version: 1
            module: demo
            slice: demo
            c_prefix: ifcopenshell_demo
            public_headers:
              - constructors.h
            handles:
              - name: settings
                cpp_type: Demo::Settings
                c_type: ifcopenshell_demo_settings_t
                destructor: delete
            discover:
              include_dir: .
              constructors:
                - handle: settings
                  cpp_class: Demo::Settings
                  translation_unit: constructors.cpp
                  expose_as: create_settings
            """
        ).strip()
        + "\n",
        encoding="utf-8",
    )

    spec = load_authored_spec(spec_path, compile_commands_path=compile_commands)
    call = next(call for call in spec.functions if call.c_name == "ifcopenshell_demo_create_settings")
    assert call.params == ()

    cpp_out = tmp_path / "demo_api.cpp"
    generate(spec_path, tmp_path / "demo_api.h", cpp_out, compile_commands_path=compile_commands)
    generated_cpp = cpp_out.read_text(encoding="utf-8")
    assert "new Demo::Settings()" in generated_cpp


def test_constructor_discovery_requires_params_for_overloaded_constructors(tmp_path: Path) -> None:
    header = tmp_path / "constructors.h"
    source = tmp_path / "constructors.cpp"
    spec_path = tmp_path / "constructors.yml"

    header.write_text(
        dedent(
            """
            namespace Demo {
            struct File {};
            struct Tree {
            public:
                Tree() = default;
                explicit Tree(File& file) {}
            };
            }
            """
        ).strip()
        + "\n",
        encoding="utf-8",
    )
    source.write_text('#include "constructors.h"\n', encoding="utf-8")
    compile_commands = _write_compile_commands(tmp_path, source)
    spec_path.write_text(
        dedent(
            """
            schema_version: 1
            module: demo
            slice: demo
            c_prefix: ifcopenshell_demo
            public_headers:
              - constructors.h
            handles:
              - name: tree
                cpp_type: Demo::Tree
                c_type: ifcopenshell_demo_tree_t
                destructor: delete
            discover:
              include_dir: .
              constructors:
                - handle: tree
                  cpp_class: Demo::Tree
                  translation_unit: constructors.cpp
                  expose_as: create_tree
            """
        ).strip()
        + "\n",
        encoding="utf-8",
    )

    with pytest.raises(ValueError, match="constructor params are required when overloads are present"):
        load_authored_spec(spec_path, compile_commands_path=compile_commands)


def test_constructor_discovery_reports_missing_source_constructors(tmp_path: Path) -> None:
    header = tmp_path / "constructors.h"
    source = tmp_path / "constructors.cpp"
    spec_path = tmp_path / "constructors.yml"

    header.write_text(
        dedent(
            """
            namespace Demo {
            struct Settings {
            private:
                explicit Settings(int index) {}
            };
            }
            """
        ).strip()
        + "\n",
        encoding="utf-8",
    )
    source.write_text('#include "constructors.h"\n', encoding="utf-8")
    compile_commands = _write_compile_commands(tmp_path, source)
    spec_path.write_text(
        dedent(
            """
            schema_version: 1
            module: demo
            slice: demo
            c_prefix: ifcopenshell_demo
            public_headers:
              - constructors.h
            handles:
              - name: settings
                cpp_type: Demo::Settings
                c_type: ifcopenshell_demo_settings_t
                destructor: delete
            discover:
              include_dir: .
              constructors:
                - handle: settings
                  cpp_class: Demo::Settings
                  translation_unit: constructors.cpp
                  expose_as: create_settings
            """
        ).strip()
        + "\n",
        encoding="utf-8",
    )

    with pytest.raises(ValueError, match="no public constructors are available"):
        load_authored_spec(spec_path, compile_commands_path=compile_commands)


def test_constructor_discovery_rejects_guarded_factories_without_fallback_params(tmp_path: Path) -> None:
    spec_path = tmp_path / "constructors.yml"
    spec_path.write_text(
        dedent(
            """
            schema_version: 1
            module: demo
            slice: demo
            c_prefix: ifcopenshell_demo
            public_headers:
              - constructors.h
            handles:
              - name: serializer
                cpp_type: Demo::Serializer
                c_type: ifcopenshell_demo_serializer_t
                destructor: delete
            discover:
              include_dir: .
              constructors:
                - handle: serializer
                  cpp_class: Demo::OptionalSerializer
                  translation_unit: constructors.cpp
                  expose_as: create_optional_serializer
                  compile_guard: WITH_OPTIONAL_SERIALIZER
            """
        ).strip()
        + "\n",
        encoding="utf-8",
    )

    with pytest.raises(ValueError, match="compile_guard requires params"):
        load_authored_spec(spec_path, compile_commands_path=tmp_path / "compile_commands.json")


def test_discovery_supports_taxonomy_make_factories(tmp_path: Path) -> None:
    header = tmp_path / "taxonomy.h"
    source = tmp_path / "taxonomy.cpp"
    spec_path = tmp_path / "taxonomy.yml"

    header.write_text(
        dedent(
            """
            #include <array>
            #include <memory>
            #include <utility>

            namespace ifcopenshell::geometry::taxonomy {
            template <typename T, typename... Args>
            std::shared_ptr<T> make(Args&&... args) {
                return std::make_shared<T>(std::forward<Args>(args)...);
            }

            struct item {};
            struct direction3 {};
            struct node {};
            struct point3 {
                point3(double x, double y, double z) {}
            };
            struct bspline_curve {
                int degree;
            };
            struct bspline_surface {
                std::array<int, 2> degree;
            };
            struct boolean_result {
                enum operation_t { UNION, SUBTRACTION, INTERSECTION };
                operation_t operation;
            };
            struct offset_curve {
                std::shared_ptr<item> basis;
                std::shared_ptr<direction3> reference;
                double offset;
            };
            }
            """
        ).strip()
        + "\n",
        encoding="utf-8",
    )
    source.write_text('#include "taxonomy.h"\n', encoding="utf-8")
    compile_commands = _write_compile_commands(tmp_path, source)
    spec_path.write_text(
        dedent(
            """
            schema_version: 1
            module: demo
            slice: demo
            c_prefix: ifcopenshell_demo
            public_headers:
              - taxonomy.h
            handles:
              - name: taxonomy_node
                cpp_type: ifcopenshell::geometry::taxonomy::node
                c_type: ifcopenshell_demo_taxonomy_node_t
                destructor: shared_ptr
                ptr_type: shared_ptr
              - name: taxonomy_item
                cpp_type: ifcopenshell::geometry::taxonomy::item
                c_type: ifcopenshell_demo_taxonomy_item_t
                destructor: shared_ptr
                ptr_type: shared_ptr
              - name: taxonomy_direction3
                cpp_type: ifcopenshell::geometry::taxonomy::direction3
                c_type: ifcopenshell_demo_taxonomy_direction3_t
                destructor: shared_ptr
                ptr_type: shared_ptr
              - name: taxonomy_point3
                cpp_type: ifcopenshell::geometry::taxonomy::point3
                c_type: ifcopenshell_demo_taxonomy_point3_t
                destructor: shared_ptr
                ptr_type: shared_ptr
              - name: taxonomy_bspline_curve
                cpp_type: ifcopenshell::geometry::taxonomy::bspline_curve
                c_type: ifcopenshell_demo_taxonomy_bspline_curve_t
                destructor: shared_ptr
                ptr_type: shared_ptr
              - name: taxonomy_bspline_surface
                cpp_type: ifcopenshell::geometry::taxonomy::bspline_surface
                c_type: ifcopenshell_demo_taxonomy_bspline_surface_t
                destructor: shared_ptr
                ptr_type: shared_ptr
              - name: taxonomy_boolean_result
                cpp_type: ifcopenshell::geometry::taxonomy::boolean_result
                c_type: ifcopenshell_demo_taxonomy_boolean_result_t
                destructor: shared_ptr
                ptr_type: shared_ptr
              - name: taxonomy_offset_curve
                cpp_type: ifcopenshell::geometry::taxonomy::offset_curve
                c_type: ifcopenshell_demo_taxonomy_offset_curve_t
                destructor: shared_ptr
                ptr_type: shared_ptr
            discover:
              include_dir: .
              constructors:
                - handle: taxonomy_node
                  cpp_class: ifcopenshell::geometry::taxonomy::node
                  translation_unit: taxonomy.cpp
                  expose_as: taxonomy_create_node
                  factory: taxonomy_make
                - handle: taxonomy_point3
                  cpp_class: ifcopenshell::geometry::taxonomy::point3
                  translation_unit: taxonomy.cpp
                  expose_as: taxonomy_create_point3
                  factory: taxonomy_make
                  params:
                    - double
                    - double
                    - double
                - handle: taxonomy_bspline_curve
                  cpp_class: ifcopenshell::geometry::taxonomy::bspline_curve
                  translation_unit: taxonomy.cpp
                  expose_as: taxonomy_create_bspline_curve
                  factory: taxonomy_make
                  field_initializers:
                    - field: degree
                      min: 1
                      error: B-spline curve degree must be >= 1
                - handle: taxonomy_bspline_surface
                  cpp_class: ifcopenshell::geometry::taxonomy::bspline_surface
                  translation_unit: taxonomy.cpp
                  expose_as: taxonomy_create_bspline_surface
                  factory: taxonomy_make
                  field_initializers:
                    - field: degree
                      params:
                        - degree_u
                        - degree_v
                      min: 1
                      error: B-spline surface degrees must be >= 1
                - handle: taxonomy_boolean_result
                  cpp_class: ifcopenshell::geometry::taxonomy::boolean_result
                  translation_unit: taxonomy.cpp
                  expose_as: taxonomy_create_boolean_result
                  factory: taxonomy_make
                  field_initializers:
                    - field: operation
                      min: 0
                      max: 2
                      error: Boolean operation must be 0 (UNION), 1 (SUBTRACTION), or 2 (INTERSECTION)
                - handle: taxonomy_offset_curve
                  cpp_class: ifcopenshell::geometry::taxonomy::offset_curve
                  translation_unit: taxonomy.cpp
                  expose_as: taxonomy_create_offset_curve
                  factory: taxonomy_make
                  field_initializers:
                    - field: basis
                    - field: reference
                    - field: offset
            """
        ).strip()
        + "\n",
        encoding="utf-8",
    )

    spec = load_authored_spec(spec_path, compile_commands_path=compile_commands)
    call = next(call for call in spec.functions if call.c_name == "ifcopenshell_demo_taxonomy_create_node")
    assert isinstance(call.policy_operation, TaxonomyMakeFactoryPolicyOp)
    assert call.params == ()
    point_call = next(call for call in spec.functions if call.c_name == "ifcopenshell_demo_taxonomy_create_point3")
    assert isinstance(point_call.policy_operation, TaxonomyMakeFactoryPolicyOp)
    assert [param.name for param in point_call.params] == ["x", "y", "z"]
    curve_call = next(call for call in spec.functions if call.c_name == "ifcopenshell_demo_taxonomy_create_bspline_curve")
    assert isinstance(curve_call.policy_operation, TaxonomyMakeFactoryPolicyOp)
    assert [param.name for param in curve_call.params] == ["degree"]
    surface_call = next(
        call for call in spec.functions if call.c_name == "ifcopenshell_demo_taxonomy_create_bspline_surface"
    )
    assert isinstance(surface_call.policy_operation, TaxonomyMakeFactoryPolicyOp)
    assert [param.name for param in surface_call.params] == ["degree_u", "degree_v"]
    boolean_call = next(
        call for call in spec.functions if call.c_name == "ifcopenshell_demo_taxonomy_create_boolean_result"
    )
    assert isinstance(boolean_call.policy_operation, TaxonomyMakeFactoryPolicyOp)
    assert [param.name for param in boolean_call.params] == ["operation"]
    offset_call = next(call for call in spec.functions if call.c_name == "ifcopenshell_demo_taxonomy_create_offset_curve")
    assert isinstance(offset_call.policy_operation, TaxonomyMakeFactoryPolicyOp)
    assert [param.name for param in offset_call.params] == ["basis", "reference", "offset"]

    cpp_out = tmp_path / "demo_api.cpp"
    generate(spec_path, tmp_path / "demo_api.h", cpp_out, compile_commands_path=compile_commands)
    generated_cpp = cpp_out.read_text(encoding="utf-8")
    assert (
        "ifcopenshell::geometry::taxonomy::make<ifcopenshell::geometry::taxonomy::node>()"
        in generated_cpp
    )
    assert (
        "ifcopenshell::geometry::taxonomy::make<ifcopenshell::geometry::taxonomy::point3>(x_cpp, y_cpp, z_cpp)"
        in generated_cpp
    )
    assert 'throw std::runtime_error("B-spline curve degree must be >= 1");' in generated_cpp
    assert "result_value->degree = static_cast<int>(degree_cpp);" in generated_cpp
    assert 'throw std::runtime_error("B-spline surface degrees must be >= 1");' in generated_cpp
    assert "result_value->degree = { degree_u_cpp, degree_v_cpp };" in generated_cpp
    assert 'throw std::runtime_error("Boolean operation must be 0 (UNION), 1 (SUBTRACTION), or 2 (INTERSECTION)");' in generated_cpp
    assert (
        "result_value->operation = "
        "static_cast<ifcopenshell::geometry::taxonomy::boolean_result::operation_t>(operation_cpp);"
        in generated_cpp
    )
    assert "result_value->basis = basis_cpp;" in generated_cpp
    assert "result_value->reference = reference_cpp;" in generated_cpp
    assert "result_value->offset = static_cast<double>(offset_cpp);" in generated_cpp


def test_taxonomy_make_factories_require_shared_ptr_handles(tmp_path: Path) -> None:
    header = tmp_path / "taxonomy.h"
    source = tmp_path / "taxonomy.cpp"
    spec_path = tmp_path / "taxonomy.yml"

    header.write_text("namespace Demo { struct Node {}; }\n", encoding="utf-8")
    source.write_text('#include "taxonomy.h"\n', encoding="utf-8")
    compile_commands = _write_compile_commands(tmp_path, source)
    spec_path.write_text(
        dedent(
            """
            schema_version: 1
            module: demo
            slice: demo
            c_prefix: ifcopenshell_demo
            public_headers:
              - taxonomy.h
            handles:
              - name: node
                cpp_type: Demo::Node
                c_type: ifcopenshell_demo_node_t
                destructor: delete
            discover:
              include_dir: .
              constructors:
                - handle: node
                  cpp_class: Demo::Node
                  translation_unit: taxonomy.cpp
                  expose_as: create_node
                  factory: taxonomy_make
            """
        ).strip()
        + "\n",
        encoding="utf-8",
    )

    with pytest.raises(ValueError, match="taxonomy_make requires a shared_ptr handle"):
        load_authored_spec(spec_path, compile_commands_path=compile_commands)


def test_discovery_rejects_stale_constructor_signature(tmp_path: Path) -> None:
    header = tmp_path / "constructors.h"
    source = tmp_path / "constructors.cpp"
    spec_path = tmp_path / "constructors.yml"

    header.write_text(
        dedent(
            """
            namespace Demo {
            struct File {};
            struct Tree {
            public:
                explicit Tree(File& file) {}
            };
            }
            """
        ).strip()
        + "\n",
        encoding="utf-8",
    )
    source.write_text('#include "constructors.h"\n', encoding="utf-8")
    compile_commands = _write_compile_commands(tmp_path, source)
    spec_path.write_text(
        dedent(
            """
            schema_version: 1
            module: demo
            slice: demo
            c_prefix: ifcopenshell_demo
            public_headers:
              - constructors.h
            handles:
              - name: file
                cpp_type: Demo::File
                c_type: ifcopenshell_demo_file_t
                destructor: delete
              - name: tree
                cpp_type: Demo::Tree
                c_type: ifcopenshell_demo_tree_t
                destructor: delete
            discover:
              include_dir: .
              constructors:
                - handle: tree
                  cpp_class: Demo::Tree
                  translation_unit: constructors.cpp
                  expose_as: create_tree_from_file
                  params:
                    - Demo::File*
            """
        ).strip()
        + "\n",
        encoding="utf-8",
    )

    with pytest.raises(ValueError, match=r"unable to resolve constructor.*available: Demo::Tree"):
        load_authored_spec(spec_path, compile_commands_path=compile_commands)


def test_load_authored_spec_infers_simple_call_kinds(tmp_path: Path) -> None:
    spec_path = tmp_path / "simple.yml"
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
              - name: thing
                cpp_type: Demo::Thing
                c_type: ifcopenshell_demo_thing_t
                destructor: delete
            functions:
              - expose_as: version
                returns:
                  kind: string
                params: []
              - expose_as: parse
                returns:
                  kind: bool
                params:
                  - name: text
                    type:
                      kind: string
                implementation:
                  kind: inline_cpp
                  body: return true;
              - expose_as: create_thing
                handle: thing
                params: []
            methods:
              - receiver: thing
                expose_as: reset
                returns:
                  kind: void
                params: []
              - receiver: thing
                expose_as: rename
                returns:
                  kind: bool
                params:
                  - name: value
                    type:
                      kind: string
                implementation:
                  kind: inline_cpp
                  body: return true;
            """
        ).strip()
        + "\n",
        encoding="utf-8",
    )

    spec = load_authored_spec(spec_path)

    assert isinstance(spec.functions[0].policy_operation, DirectFunctionPolicyOp)
    assert isinstance(spec.functions[1].policy_operation, InlineAdapterPolicyOp)
    assert isinstance(spec.functions[2].policy_operation, ConstructorPolicyOp)
    assert isinstance(spec.methods[0].policy_operation, DirectMethodPolicyOp)
    assert isinstance(spec.methods[1].policy_operation, InlineAdapterPolicyOp)


def test_load_authored_spec_expands_handle_families(tmp_path: Path) -> None:
    spec_path = tmp_path / "families.yml"
    (tmp_path / "demo.h").write_text(
        dedent(
            """
            #include <memory>
            #define DECLARE_PTR(item) typedef std::shared_ptr<item> ptr;
            namespace Demo { namespace Taxonomy {
            struct item {
                DECLARE_PTR(item)
            };
            struct curve {
                using ptr = std::shared_ptr<curve>;
            };
            }}
            """
        ).strip()
        + "\n",
        encoding="utf-8",
    )
    spec_path.write_text(
        dedent(
            """
            schema_version: 1
            module: demo
            slice: demo
            c_prefix: ifcopenshell_demo
            public_headers:
              - demo.h
            handle_families:
              - namespace: Demo::Taxonomy
                prefix: taxonomy
                destructor: shared_ptr
                ptr_type: shared_ptr
                validate_against:
                  header: demo.h
                  marker_macro: DECLARE_PTR
                types:
                  - item
                  - curve
            """
        ).strip()
        + "\n",
        encoding="utf-8",
    )

    spec = load_authored_spec(spec_path)

    assert spec.handles["taxonomy_item"].cpp_type == "Demo::Taxonomy::item"
    assert spec.handles["taxonomy_item"].c_type == "ifcopenshell_demo_taxonomy_item_t"
    assert spec.handles["taxonomy_item"].destructor == "shared_ptr"
    assert spec.handles["taxonomy_item"].ptr_type == "shared_ptr"
    assert spec.handles["taxonomy_curve"].cpp_type == "Demo::Taxonomy::curve"


def test_load_authored_spec_rejects_unmarked_handle_family_type(tmp_path: Path) -> None:
    spec_path = tmp_path / "families.yml"
    (tmp_path / "demo.h").write_text(
        dedent(
            """
            #include <memory>
            #define DECLARE_PTR(item) typedef std::shared_ptr<item> ptr;
            namespace Demo { namespace Taxonomy {
            struct item {
                DECLARE_PTR(item)
            };
            struct curve {
            };
            }}
            """
        ).strip()
        + "\n",
        encoding="utf-8",
    )
    spec_path.write_text(
        dedent(
            """
            schema_version: 1
            module: demo
            slice: demo
            c_prefix: ifcopenshell_demo
            public_headers:
              - demo.h
            handle_families:
              - namespace: Demo::Taxonomy
                prefix: taxonomy
                destructor: shared_ptr
                ptr_type: shared_ptr
                validate_against:
                  header: demo.h
                  marker_macro: DECLARE_PTR
                types:
                  - item
                  - curve
            """
        ).strip()
        + "\n",
        encoding="utf-8",
    )

    with pytest.raises(ValueError, match=r"types not marked.*curve"):
        load_authored_spec(spec_path)


def test_handle_family_validation_ignores_macro_definition_signatures(tmp_path: Path) -> None:
    spec_path = tmp_path / "families.yml"
    (tmp_path / "demo.h").write_text(
        dedent(
            """
            #define DECLARE_PTR(phantom) typedef int ignored;
            namespace Demo {
            struct item {
                DECLARE_PTR(item)
            };
            struct phantom {
            };
            }
            """
        ).strip()
        + "\n",
        encoding="utf-8",
    )
    spec_path.write_text(
        dedent(
            """
            schema_version: 1
            module: demo
            slice: demo
            c_prefix: ifcopenshell_demo
            public_headers:
              - demo.h
            handle_families:
              - namespace: Demo
                prefix: taxonomy
                destructor: shared_ptr
                ptr_type: shared_ptr
                validate_against:
                  header: demo.h
                  marker_macro: DECLARE_PTR
                types:
                  - phantom
            """
        ).strip()
        + "\n",
        encoding="utf-8",
    )

    with pytest.raises(ValueError, match=r"types not marked.*phantom"):
        load_authored_spec(spec_path)


def test_load_authored_spec_rejects_handle_family_collisions(tmp_path: Path) -> None:
    spec_path = tmp_path / "family_collision.yml"
    spec_path.write_text(
        dedent(
            """
            schema_version: 1
            module: demo
            slice: demo
            c_prefix: ifcopenshell_demo
            public_headers:
              - demo.h
            handle_families:
              - namespace: Demo
                prefix: thing
                destructor: delete
                types:
                  - item
            handles:
              - name: thing_item
                cpp_type: Demo::Other
                c_type: ifcopenshell_demo_other_t
                destructor: delete
            """
        ).strip()
        + "\n",
        encoding="utf-8",
    )

    with pytest.raises(ValueError, match="handles\\[0\\]\\.name 'thing_item' is duplicated"):
        load_authored_spec(spec_path)


def test_load_authored_spec_rejects_removed_legacy_call_kinds(tmp_path: Path) -> None:
    spec_path = tmp_path / "legacy.yml"
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
              - name: thing
                cpp_type: Demo::Thing
                c_type: ifcopenshell_demo_thing_t
                destructor: delete
            methods:
              - kind: field
                receiver: thing
                expose_as: value
                cpp_name: value
                returns:
                  kind: int32
                params: []
            """
        ).strip()
        + "\n",
        encoding="utf-8",
    )

    with pytest.raises(ValueError, match=r"methods\[0\]\.kind must be one of \['adapter_method', 'method'\]"):
        load_authored_spec(spec_path)


def test_load_authored_spec_rejects_explicit_c_name(tmp_path: Path) -> None:
    spec_path = tmp_path / "explicit_c_name.yml"
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
              - name: thing
                cpp_type: Demo::Thing
                c_type: ifcopenshell_demo_thing_t
                destructor: delete
            functions:
              - expose_as: version
                c_name: ifcopenshell_demo_version
                returns:
                  kind: string
                params: []
            """
        ).strip()
        + "\n",
        encoding="utf-8",
    )

    with pytest.raises(
        ValueError,
        match=r"functions\[0\]\.c_name is not supported; C names are derived from expose_as and receiver",
    ):
        load_authored_spec(spec_path)


def test_load_authored_spec_supports_c_expose_as_overrides(tmp_path: Path) -> None:
    spec_path = tmp_path / "c_expose_as.yml"
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
              - name: thing
                cpp_type: Demo::Thing
                c_type: ifcopenshell_demo_thing_t
                destructor: delete
            functions:
              - expose_as: read
                c_expose_as: read_memory
                returns:
                  kind: bool
                params: []
            methods:
              - receiver: thing
                expose_as: aggregation_kind
                c_expose_as: kind
                returns:
                  kind: string
                params: []
            """
        ).strip()
        + "\n",
        encoding="utf-8",
    )

    spec = load_authored_spec(spec_path)

    assert spec.functions[0].expose_as == "read"
    assert spec.functions[0].c_name == "ifcopenshell_demo_read_memory"
    assert spec.methods[0].expose_as == "aggregation_kind"
    assert spec.methods[0].c_name == "ifcopenshell_demo_thing_kind"


def test_generate_synthetic_autodiscovery_features(tmp_path: Path) -> None:
    header = tmp_path / "sample.h"
    source = tmp_path / "sample.cpp"
    spec_path = tmp_path / "sample.yml"

    header.write_text(
        dedent(
            """
            #include <array>
            #include <cstdint>
            #include <cstddef>
            #include <memory>
            #include <set>
            #include <string>
            #include <vector>

            namespace boost {
            template <typename T>
            struct optional {
                optional() = default;
                optional(const T&) {}
                bool is_initialized() const { return true; }
                explicit operator bool() const { return is_initialized(); }
                T& operator*();
                const T& operator*() const;
            };

            template <typename... Ts>
            struct variant {
                variant() = default;

                template <typename T>
                variant(const T&) {}
            };

            template <typename T, typename... Ts>
            T* get(variant<Ts...>*);
            }

            namespace Demo {
            enum class Mode { A, B };

            struct Node {
                using ptr = std::shared_ptr<Node>;
                int value = 0;
            };

            struct Base {
            public:
                int inherited = 7;
            };

            struct Derived : Base {
            public:
                boost::optional<double> weight;
                Node::ptr axis;
                std::vector<Node::ptr> children;
                std::array<int, 2> uv = {1, 2};
            };

            struct Settings {
            public:
                using value_variant_t = boost::variant<
                    bool,
                    int64_t,
                    Mode,
                    double,
                    std::string,
                    std::set<int>,
                    std::set<std::string>,
                    std::vector<int>,
                    std::vector<std::string>,
                    std::vector<double>
                >;

                value_variant_t value = false;

                value_variant_t get(const std::string&) const { return value; }
                void set(const std::string&, const value_variant_t& next) { value = next; }
            };

            struct Components3 {
                std::array<double, 3> values;
                double operator()(int index) const { return values[static_cast<std::size_t>(index)]; }
            };

            struct Point3 {
            public:
                Components3 ccomponents() const { return {{1.0, 2.0, 3.0}}; }
            };
            }
            """
        ).strip()
        + "\n",
        encoding="utf-8",
    )
    source.write_text('#include "sample.h"\n', encoding="utf-8")
    compile_commands = _write_compile_commands(tmp_path, source)

    spec_path.write_text(
        dedent(
            """
            schema_version: 1
            module: demo
            slice: demo
            c_prefix: ifcopenshell_demo
            public_headers:
              - sample.h
            handles:
              - name: node
                cpp_type: Demo::Node
                c_type: ifcopenshell_demo_node_t
                destructor: shared_ptr
                ptr_type: shared_ptr
              - name: derived
                cpp_type: Demo::Derived
                c_type: ifcopenshell_demo_derived_t
                destructor: delete
              - name: settings
                cpp_type: Demo::Settings
                c_type: ifcopenshell_demo_settings_t
                destructor: delete
              - name: point3
                cpp_type: Demo::Point3
                c_type: ifcopenshell_demo_point3_t
                destructor: delete
            discover:
              include_dir: .
              classes:
                - handle: derived
                  translation_unit: sample.cpp
                  include_all: false
                  discover_fields: true
                  include_inherited_fields: true
                  discover_has_fields: true
                  discover_optional_fields: true
                  field_setters:
                    - axis
                  discover_children:
                    cpp_field: children
                    count_as: child_count
                    at_as: child_at
                    add_as: add_child
                  array_pair_fields:
                    - uv
                - handle: settings
                  translation_unit: sample.cpp
                  include_all: false
                  variant_accessors:
                    get_method: get
                    set_method: set
                    variant_type: "Demo::Settings::value_variant_t"
                    types:
                      bool: "bool"
                      int:
                        cpp_type: "int64_t"
                        getter_types:
                          - "int64_t"
                          - "Demo::Mode"
                      double: "double"
                      string: "std::string"
                      int_set: "std::set<int>"
                      string_set: "std::set<std::string>"
                      int_list: "std::vector<int>"
                      string_list: "std::vector<std::string>"
                      double_list: "std::vector<double>"
                - handle: point3
                  translation_unit: sample.cpp
                  include_all: false
                  ccomponents_accessor:
                    expose_as: get_data
                    dimensions: 3
            functions:
              - kind: constructor
                expose_as: create_derived
                handle: derived
                params: []
            """
        ).strip()
        + "\n",
        encoding="utf-8",
    )

    spec = load_authored_spec(spec_path, compile_commands_path=compile_commands)
    calls = {call.c_name: call for call in (*spec.functions, *spec.methods)}

    expected_calls = {
        "ifcopenshell_demo_create_derived",
        "ifcopenshell_demo_derived_axis",
        "ifcopenshell_demo_derived_child_at",
        "ifcopenshell_demo_derived_child_count",
        "ifcopenshell_demo_derived_add_child",
        "ifcopenshell_demo_derived_has_axis",
        "ifcopenshell_demo_derived_has_weight",
        "ifcopenshell_demo_derived_inherited",
        "ifcopenshell_demo_derived_set_axis",
        "ifcopenshell_demo_derived_uv_u",
        "ifcopenshell_demo_derived_uv_v",
        "ifcopenshell_demo_derived_weight",
        "ifcopenshell_demo_point3_get_data",
        "ifcopenshell_demo_settings_get_bool",
        "ifcopenshell_demo_settings_get_double",
        "ifcopenshell_demo_settings_get_int",
        "ifcopenshell_demo_settings_get_int_set",
        "ifcopenshell_demo_settings_get_int_list",
        "ifcopenshell_demo_settings_get_double_list",
        "ifcopenshell_demo_settings_get_string",
        "ifcopenshell_demo_settings_get_string_list",
        "ifcopenshell_demo_settings_get_string_set",
        "ifcopenshell_demo_settings_set_bool",
        "ifcopenshell_demo_settings_set_double",
        "ifcopenshell_demo_settings_set_double_list",
        "ifcopenshell_demo_settings_set_int",
        "ifcopenshell_demo_settings_set_int_set",
        "ifcopenshell_demo_settings_set_int_list",
        "ifcopenshell_demo_settings_set_string",
        "ifcopenshell_demo_settings_set_string_list",
        "ifcopenshell_demo_settings_set_string_set",
    }
    assert expected_calls.issubset(calls)
    assert isinstance(calls["ifcopenshell_demo_derived_inherited"].policy_operation, DirectFieldPolicyOp)
    assert isinstance(calls["ifcopenshell_demo_derived_add_child"].policy_operation, ChildrenAddPolicyOp)
    assert isinstance(calls["ifcopenshell_demo_derived_weight"].policy_operation, OptionalGetPolicyOp)
    assert isinstance(calls["ifcopenshell_demo_settings_get_bool"].policy_operation, VariantGetPolicyOp)
    assert calls["ifcopenshell_demo_derived_uv_u"].returns.kind == "int32"
    assert calls["ifcopenshell_demo_derived_uv_v"].returns.kind == "int32"

    header_out = tmp_path / "demo_api.h"
    cpp_out = tmp_path / "demo_api.cpp"
    generate(spec_path, header_out, cpp_out, compile_commands_path=compile_commands)

    generated_header = header_out.read_text(encoding="utf-8")
    generated_cpp = cpp_out.read_text(encoding="utf-8")

    assert "bool ifcopenshell_demo_create_derived(ifcopenshell_demo_derived_t** out_result);" in generated_header
    assert "bool ifcopenshell_demo_derived_has_weight(ifcopenshell_demo_derived_t* self, bool* out_result);" in generated_header
    assert "bool ifcopenshell_demo_derived_weight(ifcopenshell_demo_derived_t* self, double* out_result);" in generated_header
    assert "bool ifcopenshell_demo_derived_set_axis(" in generated_header
    assert "bool ifcopenshell_demo_derived_child_at(" in generated_header
    assert "bool ifcopenshell_demo_settings_get_string(" in generated_header
    assert "bool ifcopenshell_demo_settings_get_int(ifcopenshell_demo_settings_t* self, const char* name, int64_t* out_result);" in generated_header
    assert "bool ifcopenshell_demo_settings_set_int(ifcopenshell_demo_settings_t* self, const char* name, int64_t value);" in generated_header
    assert "bool ifcopenshell_demo_settings_get_int_set(ifcopenshell_demo_settings_t* self, const char* name, ifcopenshell_int32_list_t* out_result);" in generated_header
    assert "bool ifcopenshell_demo_settings_set_int_set(ifcopenshell_demo_settings_t* self, const char* name, const ifcopenshell_int32_list_t* value);" in generated_header
    assert "bool ifcopenshell_demo_settings_get_int_list(ifcopenshell_demo_settings_t* self, const char* name, ifcopenshell_int32_list_t* out_result);" in generated_header
    assert "bool ifcopenshell_demo_settings_set_string_list(ifcopenshell_demo_settings_t* self, const char* name, const ifcopenshell_string_list_t* value);" in generated_header
    assert "bool ifcopenshell_demo_point3_get_data(ifcopenshell_demo_point3_t* self, ifcopenshell_double_list_t* out_result);" in generated_header

    assert "self_cpp->axis = value_cpp;" in generated_cpp
    assert "self_cpp->children.push_back(item_cpp);" in generated_cpp
    assert "*out_result = static_cast<bool>(self_cpp->weight);" in generated_cpp
    assert 'if (!self_cpp->weight) { throw std::runtime_error("weight is not set"); }' in generated_cpp
    assert "if (auto* p = std::get_if<bool>(&val))" in generated_cpp
    assert "if (auto* p = std::get_if<int64_t>(&val))" in generated_cpp
    assert "if (auto* p = std::get_if<Demo::Mode>(&val))" in generated_cpp
    assert "if (auto* p = std::get_if<std::set<int>>(&val))" in generated_cpp
    assert "if (auto* p = std::get_if<std::vector<int>>(&val))" in generated_cpp
    assert "if (auto* p = std::get_if<std::vector<std::string>>(&val))" in generated_cpp
    assert "std::set<int> value_cpp(value_vec.begin(), value_vec.end());" in generated_cpp
    assert "auto value_cpp = to_cpp_int32_list(value);" in generated_cpp
    assert "auto value_cpp = to_cpp_string_list(value);" in generated_cpp
    assert "self_cpp->set(name_cpp, Demo::Settings::value_variant_t(static_cast<int64_t>(value)));" in generated_cpp
    assert "self_cpp->set(name_cpp, Demo::Settings::value_variant_t(value_cpp));" in generated_cpp
    assert "const auto& v = self_cpp->ccomponents();" in generated_cpp


def test_autodiscovery_lowers_bool_double_reference_out_params(tmp_path: Path) -> None:
    header = tmp_path / "out_param_sample.h"
    source = tmp_path / "out_param_sample.cpp"
    spec_path = tmp_path / "out_param_sample.yml"

    header.write_text(
        dedent(
            """
            namespace Demo {
            struct Shape {
            public:
                bool calculate_volume(double& value) const { value = 12.5; return true; }
            };
            }
            """
        ).strip()
        + "\n",
        encoding="utf-8",
    )
    source.write_text('#include "out_param_sample.h"\n', encoding="utf-8")
    compile_commands = _write_compile_commands(tmp_path, source)

    spec_path.write_text(
        dedent(
            """
            schema_version: 1
            module: demo
            slice: demo
            c_prefix: ifcopenshell_demo
            public_headers:
              - out_param_sample.h
            handles:
              - name: shape
                cpp_type: Demo::Shape
                c_type: ifcopenshell_demo_shape_t
                destructor: delete
            discover:
              include_dir: .
              classes:
                - handle: shape
                  translation_unit: out_param_sample.cpp
                  include_all: true
            """
        ).strip()
        + "\n",
        encoding="utf-8",
    )

    spec = load_authored_spec(spec_path, compile_commands_path=compile_commands)
    call = {call.c_name: call for call in spec.methods}["ifcopenshell_demo_shape_calculate_volume"]
    assert call.returns.kind == "double"
    assert call.params == ()
    assert isinstance(call.policy_operation, BoolOutParamPolicyOp)

    header_out = tmp_path / "demo_api.h"
    cpp_out = tmp_path / "demo_api.cpp"
    generate(spec_path, header_out, cpp_out, compile_commands_path=compile_commands)

    generated_header = header_out.read_text(encoding="utf-8")
    generated_cpp = cpp_out.read_text(encoding="utf-8")
    assert "bool ifcopenshell_demo_shape_calculate_volume(ifcopenshell_demo_shape_t* self, double* out_result);" in generated_header
    assert "double result_value{};" in generated_cpp
    assert "if (self_cpp->calculate_volume(result_value))" in generated_cpp
    assert "std::numeric_limits<double>::quiet_NaN()" in generated_cpp


def test_autodiscovery_supports_enum_methods(tmp_path: Path) -> None:
    header = tmp_path / "enum_sample.h"
    source = tmp_path / "enum_sample.cpp"
    spec_path = tmp_path / "enum_sample.yml"

    header.write_text(
        dedent(
            """
            namespace Demo {
            enum class Mode { A, B };

            struct Widget {
            public:
                Mode mode() const { return Mode::A; }
                void set_mode(Mode next) {}
            };
            }
            """
        ).strip()
        + "\n",
        encoding="utf-8",
    )
    source.write_text('#include "enum_sample.h"\n', encoding="utf-8")
    compile_commands = _write_compile_commands(tmp_path, source)

    spec_path.write_text(
        dedent(
            """
            schema_version: 1
            module: demo
            slice: demo
            c_prefix: ifcopenshell_demo
            public_headers:
              - enum_sample.h
            handles:
              - name: widget
                cpp_type: Demo::Widget
                c_type: ifcopenshell_demo_widget_t
                destructor: delete
            discover:
              include_dir: .
              classes:
                - handle: widget
                  translation_unit: enum_sample.cpp
                  include:
                    - mode
                    - set_mode
            """
        ).strip()
        + "\n",
        encoding="utf-8",
    )

    spec = load_authored_spec(spec_path, compile_commands_path=compile_commands)
    calls = {call.c_name: call for call in spec.methods}

    assert calls["ifcopenshell_demo_widget_mode"].returns.kind == "int32"
    assert calls["ifcopenshell_demo_widget_mode"].returns.cpp_type == "Demo::Mode"
    assert calls["ifcopenshell_demo_widget_set_mode"].params[0].type.kind == "int32"
    assert calls["ifcopenshell_demo_widget_set_mode"].params[0].type.cpp_type == "Demo::Mode"

    header_out = tmp_path / "enum_api.h"
    cpp_out = tmp_path / "enum_api.cpp"
    generate(spec_path, header_out, cpp_out, compile_commands_path=compile_commands)

    generated_cpp = cpp_out.read_text(encoding="utf-8")
    assert "auto next_cpp = static_cast<Demo::Mode>(next);" in generated_cpp
    assert "*out_result = static_cast<int32_t>(self_cpp->mode());" in generated_cpp


def test_autodiscovery_supports_sets_and_opaque_coordinates(tmp_path: Path) -> None:
    header = tmp_path / "containers.h"
    source = tmp_path / "containers.cpp"
    spec_path = tmp_path / "containers.yml"

    header.write_text(
        dedent(
            """
            #include <set>
            #include <string>

            namespace IfcGeom {
            class OpaqueNumber {
            public:
                virtual double to_double() const = 0;
                virtual ~OpaqueNumber() = default;
            };

            class NumberNativeDouble : public OpaqueNumber {
            public:
                explicit NumberNativeDouble(double value) : value_(value) {}
                double to_double() const override { return value_; }
            private:
                double value_;
            };

            template <size_t N>
            struct OpaqueCoordinate {
                OpaqueCoordinate() = default;

                template <typename... Args>
                explicit OpaqueCoordinate(Args... args) {
                    OpaqueNumber* values[] = {args...};
                    for (size_t i = 0; i < N; ++i) {
                        values_[i] = values[i];
                    }
                }

                OpaqueNumber* get(size_t i) const { return values_[i]; }
                OpaqueNumber* values_[N] = {};
            };
            }

            namespace Demo {
            struct Declaration {};

            struct Widget {
            public:
                void set_names(const std::set<std::string>& names) {}
                void set_types(const std::set<const Declaration*>& decls) {}
                std::set<std::string> names() const { return {"alpha", "beta"}; }
                std::set<const Declaration*> types() const { return {}; }
                IfcGeom::OpaqueCoordinate<3> axis() const {
                    return IfcGeom::OpaqueCoordinate<3>(
                        new IfcGeom::NumberNativeDouble(1.0),
                        new IfcGeom::NumberNativeDouble(2.0),
                        new IfcGeom::NumberNativeDouble(3.0)
                    );
                }
            };
            }
            """
        ).strip()
        + "\n",
        encoding="utf-8",
    )
    source.write_text('#include "containers.h"\n', encoding="utf-8")
    compile_commands = _write_compile_commands(tmp_path, source)

    spec_path.write_text(
        dedent(
            """
            schema_version: 1
            module: demo
            slice: demo
            c_prefix: ifcopenshell_demo
            public_headers:
              - containers.h
            handles:
              - name: widget
                cpp_type: Demo::Widget
                c_type: ifcopenshell_demo_widget_t
                destructor: delete
              - name: declaration
                cpp_type: Demo::Declaration
                c_type: ifcopenshell_demo_declaration_t
                destructor: none
            discover:
              include_dir: .
              classes:
                - handle: widget
                  translation_unit: containers.cpp
                  include:
                    - set_names
                    - set_types
                    - names
                    - types
            methods:
              - receiver: widget
                expose_as: axis
                returns:
                  kind: double_list
                params: []
                implementation:
                  kind: inline_cpp
                  body: |
                    auto tmp = self_cpp->axis();
                    return std::vector<double>{
                        tmp.get(0) ? tmp.get(0)->to_double() : 0.0,
                        tmp.get(1) ? tmp.get(1)->to_double() : 0.0,
                        tmp.get(2) ? tmp.get(2)->to_double() : 0.0
                    };
                """
            ).strip()
        + "\n",
        encoding="utf-8",
    )

    spec = load_authored_spec(spec_path, compile_commands_path=compile_commands)
    calls = {call.c_name: call for call in spec.methods}

    assert calls["ifcopenshell_demo_widget_set_names"].params[0].type.kind == "string"
    assert calls["ifcopenshell_demo_widget_set_names"].params[0].type.sequence_depth == 1
    assert calls["ifcopenshell_demo_widget_set_types"].params[0].type.kind == "handle"
    assert calls["ifcopenshell_demo_widget_set_types"].params[0].type.sequence_depth == 1
    assert calls["ifcopenshell_demo_widget_set_types"].params[0].type.handle == "declaration"
    assert calls["ifcopenshell_demo_widget_names"].returns.kind == "string"
    assert calls["ifcopenshell_demo_widget_names"].returns.sequence_depth == 1
    assert calls["ifcopenshell_demo_widget_types"].returns.kind == "handle"
    assert calls["ifcopenshell_demo_widget_types"].returns.sequence_depth == 1
    assert calls["ifcopenshell_demo_widget_types"].returns.handle == "declaration"
    assert calls["ifcopenshell_demo_widget_axis"].returns.kind == "double"
    assert calls["ifcopenshell_demo_widget_axis"].returns.sequence_depth == 1

    header_out = tmp_path / "containers_api.h"
    cpp_out = tmp_path / "containers_api.cpp"
    generate(spec_path, header_out, cpp_out, compile_commands_path=compile_commands)

    generated_cpp = cpp_out.read_text(encoding="utf-8")
    assert "std::set<std::string> names_cpp(names_vec.begin(), names_vec.end());" in generated_cpp
    assert "std::set<const Demo::Declaration*> decls_cpp(decls_vec.begin(), decls_vec.end());" in generated_cpp
    assert "return std::vector(tmp.begin(), tmp.end());" in generated_cpp
    assert "tmp.get(0) ? tmp.get(0)->to_double() : 0.0" in generated_cpp


def test_autodiscovery_supports_nested_vectors(tmp_path: Path) -> None:
    header = tmp_path / "nested_vectors.h"
    source = tmp_path / "nested_vectors.cpp"
    spec_path = tmp_path / "nested_vectors.yml"

    header.write_text(
        dedent(
            """
            #include <vector>

            namespace Demo {
            struct Widget {
            public:
                void set_faces(const std::vector<std::vector<int>>& faces) {}
                void set_uvs(const std::vector<std::vector<double>>& uvs) {}
                std::vector<std::vector<int>> faces() const { return {{1, 2}, {3, 4}}; }
                std::vector<std::vector<double>> uvs() const { return {{1.0, 2.0}, {3.0, 4.0}}; }
            };
            }
            """
        ).strip()
        + "\n",
        encoding="utf-8",
    )
    source.write_text('#include "nested_vectors.h"\n', encoding="utf-8")
    compile_commands = _write_compile_commands(tmp_path, source)

    spec_path.write_text(
        dedent(
            """
            schema_version: 1
            module: demo
            slice: demo
            c_prefix: ifcopenshell_demo
            public_headers:
              - nested_vectors.h
            handles:
              - name: widget
                cpp_type: Demo::Widget
                c_type: ifcopenshell_demo_widget_t
                destructor: delete
            discover:
              include_dir: .
              classes:
                - handle: widget
                  translation_unit: nested_vectors.cpp
                  include:
                    - set_faces
                    - set_uvs
                    - faces
                    - uvs
            """
        ).strip()
        + "\n",
        encoding="utf-8",
    )

    spec = load_authored_spec(spec_path, compile_commands_path=compile_commands)
    calls = {call.c_name: call for call in spec.methods}

    assert calls["ifcopenshell_demo_widget_set_faces"].params[0].type.kind == "int32"
    assert calls["ifcopenshell_demo_widget_set_faces"].params[0].type.sequence_depth == 2
    assert calls["ifcopenshell_demo_widget_set_uvs"].params[0].type.kind == "double"
    assert calls["ifcopenshell_demo_widget_set_uvs"].params[0].type.sequence_depth == 2
    assert calls["ifcopenshell_demo_widget_faces"].returns.kind == "int32"
    assert calls["ifcopenshell_demo_widget_faces"].returns.sequence_depth == 2
    assert calls["ifcopenshell_demo_widget_uvs"].returns.kind == "double"
    assert calls["ifcopenshell_demo_widget_uvs"].returns.sequence_depth == 2
    assert calls["ifcopenshell_demo_widget_faces"].returns.cpp_type == "std::vector<std::vector<int>>"
    assert calls["ifcopenshell_demo_widget_uvs"].returns.cpp_type == "std::vector<std::vector<double>>"

    header_out = tmp_path / "nested_vectors_api.h"
    cpp_out = tmp_path / "nested_vectors_api.cpp"
    generate(spec_path, header_out, cpp_out, compile_commands_path=compile_commands)

    generated_cpp = cpp_out.read_text(encoding="utf-8")
    assert "auto faces_cpp = to_cpp_int32_list_list(faces);" in generated_cpp
    assert "auto uvs_cpp = to_cpp_double_list_list(uvs);" in generated_cpp
    assert "*out_result = make_int32_list_list(self_cpp->faces());" in generated_cpp
    assert "*out_result = make_double_list_list(self_cpp->uvs());" in generated_cpp


def test_autodiscovery_supports_nested_namespace_functions(tmp_path: Path) -> None:
    header = tmp_path / "bindings.h"
    source = tmp_path / "bindings.cpp"
    spec_path = tmp_path / "bindings.yml"

    header.write_text(
        dedent(
            """
            #include <string>

            namespace ifcapi {
            namespace bindings {
            int nested_count(const std::string& name);
            }
            }
            """
        ).strip()
        + "\n",
        encoding="utf-8",
    )
    source.write_text(
        dedent(
            """
            #include "bindings.h"

            int ifcapi::bindings::nested_count(const std::string& name) { return static_cast<int>(name.size()); }

            namespace ifcapi::bindings {
            double qualified_scale(double value) { return value; }
            }
            """
        ).strip()
        + "\n",
        encoding="utf-8",
    )
    compile_commands = _write_compile_commands(tmp_path, source)

    spec_path.write_text(
        dedent(
            """
            schema_version: 1
            module: demo
            slice: demo
            c_prefix: ifcopenshell_demo
            public_headers:
              - bindings.h
            discover:
              include_dir: .
              functions:
                - namespace: ifcapi::bindings
                  translation_unit: bindings.cpp
                  include:
                    - nested_count
                    - qualified_scale
                  type_overrides:
                    qualified_scale:
                      returns:
                        kind: double
            """
        ).strip()
        + "\n",
        encoding="utf-8",
    )

    spec = load_authored_spec(spec_path, compile_commands_path=compile_commands)
    calls = {call.c_name: call for call in spec.functions}

    assert calls["ifcopenshell_demo_nested_count"].policy_operation.cpp_name == "ifcapi::bindings::nested_count"
    assert calls["ifcopenshell_demo_nested_count"].params[0].type.kind == "string"
    assert calls["ifcopenshell_demo_qualified_scale"].policy_operation.cpp_name == "ifcapi::bindings::qualified_scale"
    assert calls["ifcopenshell_demo_qualified_scale"].returns.kind == "double"

    header_out = tmp_path / "bindings_api.h"
    cpp_out = tmp_path / "bindings_api.cpp"
    generate(spec_path, header_out, cpp_out, compile_commands_path=compile_commands)

    generated_cpp = cpp_out.read_text(encoding="utf-8")
    assert "ifcapi::bindings::nested_count(name_cpp)" in generated_cpp
    assert "ifcapi::bindings::qualified_scale(value_cpp)" in generated_cpp


def test_authored_spec_discovery_uses_explicit_compilation_without_compile_commands(tmp_path: Path) -> None:
    compiler = shutil.which("clang++")
    if compiler is None:
        pytest.skip("clang++ is not available")

    header = tmp_path / "bindings.h"
    source = tmp_path / "bindings.cpp"
    spec_path = tmp_path / "bindings.yml"

    header.write_text(
        dedent(
            """
            #include <string>

            namespace ifcapi::bindings {
            int nested_count(const std::string& name);
            }
            """
        ).strip()
        + "\n",
        encoding="utf-8",
    )
    source.write_text('#include "bindings.h"\n', encoding="utf-8")
    spec_path.write_text(
        dedent(
            f"""
            schema_version: 1
            module: demo
            slice: demo
            c_prefix: ifcopenshell_demo
            public_headers:
              - bindings.h
            discover:
              include_dir: .
              compilation:
                compiler: {compiler}
                include_dirs:
                  - .
              functions:
                - namespace: ifcapi::bindings
                  translation_unit: bindings.cpp
                  include:
                    - nested_count
            """
        ).strip()
        + "\n",
        encoding="utf-8",
    )

    spec = load_authored_spec(spec_path, compile_commands_path=None)
    calls = {call.c_name: call for call in spec.functions}

    assert calls["ifcopenshell_demo_nested_count"].policy_operation.cpp_name == "ifcapi::bindings::nested_count"
    assert calls["ifcopenshell_demo_nested_count"].params[0].type.kind == "string"

    header_out = tmp_path / "bindings_api.h"
    cpp_out = tmp_path / "bindings_api.cpp"
    generate(spec_path, header_out, cpp_out, compile_commands_path=None)

    generated_cpp = cpp_out.read_text(encoding="utf-8")
    assert "ifcapi::bindings::nested_count(name_cpp)" in generated_cpp


def test_authored_spec_discovery_accepts_global_include_dirs_without_compile_commands(tmp_path: Path) -> None:
    compiler = shutil.which("clang++")
    if compiler is None:
        pytest.skip("clang++ is not available")

    dependency_dir = tmp_path / "dependency"
    dependency_dir.mkdir()
    (dependency_dir / "dep.hpp").write_text("using external_int = int;\n", encoding="utf-8")

    header = tmp_path / "bindings.h"
    source = tmp_path / "bindings.cpp"
    spec_path = tmp_path / "bindings.yml"

    header.write_text(
        dedent(
            """
            #include <dep.hpp>

            namespace ifcapi::bindings {
            int dependency_value(external_int value);
            }
            """
        ).strip()
        + "\n",
        encoding="utf-8",
    )
    source.write_text('#include "bindings.h"\n', encoding="utf-8")
    spec_path.write_text(
        dedent(
            f"""
            schema_version: 1
            module: demo
            slice: demo
            c_prefix: ifcopenshell_demo
            public_headers:
              - bindings.h
            discover:
              include_dir: .
              compilation:
                compiler: {compiler}
              functions:
                - namespace: ifcapi::bindings
                  translation_unit: bindings.cpp
                  include:
                    - dependency_value
            """
        ).strip()
        + "\n",
        encoding="utf-8",
    )

    spec = load_authored_spec(
        spec_path,
        compile_commands_path=None,
        discovery_include_dirs=(dependency_dir,),
    )
    calls = {call.c_name: call for call in spec.functions}
    assert calls["ifcopenshell_demo_dependency_value"].params[0].type.kind == "int32"

    header_out = tmp_path / "bindings_api.h"
    cpp_out = tmp_path / "bindings_api.cpp"
    generate(
        spec_path,
        header_out,
        cpp_out,
        compile_commands_path=None,
        discovery_include_dirs=(dependency_dir,),
    )
    assert "int32_t value" in header_out.read_text(encoding="utf-8")


def test_value_handle_discovery_wraps_values_without_pointer_storage(tmp_path: Path) -> None:
    compiler = shutil.which("clang++")
    if compiler is None:
        pytest.skip("clang++ is not available")

    header = tmp_path / "values.h"
    source = tmp_path / "values.cpp"
    spec_path = tmp_path / "values.yml"

    header.write_text(
        dedent(
            """
            #include <vector>

            namespace Demo {
            struct Value {
                bool valid = true;
                explicit operator bool() const { return valid; }
            };

            Value find_value(int id);
            int consume_ref(const Value& value);
            int consume_value(Value value);
            std::vector<Value> all_values();
            }
            """
        ).strip()
        + "\n",
        encoding="utf-8",
    )
    source.write_text('#include "values.h"\n', encoding="utf-8")
    spec_path.write_text(
        dedent(
            f"""
            schema_version: 1
            module: demo
            slice: demo
            c_prefix: ifcopenshell_demo
            public_headers:
              - values.h
            handles:
              - name: value
                cpp_type: Demo::Value
                c_type: ifcopenshell_demo_value_t
                destructor: none
                ptr_type: value
                empty_check: "!static_cast<bool>({{value}})"
            discover:
              include_dir: .
              compilation:
                compiler: {compiler}
              functions:
                - namespace: Demo
                  translation_unit: values.cpp
                  include:
                    - find_value
                    - consume_ref
                    - consume_value
                    - all_values
                  type_overrides:
                    find_value:
                      returns:
                        nullable: true
            """
        ).strip()
        + "\n",
        encoding="utf-8",
    )

    spec = load_authored_spec(spec_path)
    calls = {call.c_name: call for call in spec.functions}
    assert calls["ifcopenshell_demo_find_value"].returns.handle == "value"
    assert calls["ifcopenshell_demo_find_value"].returns.nullable
    assert calls["ifcopenshell_demo_consume_ref"].params[0].type.handle == "value"
    assert calls["ifcopenshell_demo_all_values"].returns.sequence_depth == 1

    header_out = tmp_path / "values_api.h"
    cpp_out = tmp_path / "values_api.cpp"
    generate(spec_path, header_out, cpp_out)

    generated_cpp = cpp_out.read_text(encoding="utf-8")
    generated_internal = cpp_out.with_name("values_api_internal.hpp").read_text(encoding="utf-8")
    assert "struct ifcopenshell_demo_value_t {\n    Demo::Value value;\n};" in generated_internal
    assert "if (!static_cast<bool>(result_value))" in generated_cpp
    assert "*out_result = new ifcopenshell_demo_value_t{std::move(result_value)};" in generated_cpp
    assert "const auto& value_cpp = value->value;" in generated_cpp
    assert "auto value_cpp = value->value;" in generated_cpp
    assert "new ifcopenshell_demo_value_t{Demo::Value(values[i])}" in generated_cpp
    assert "result.push_back(item->value);" in generated_cpp
    assert "value->ptr" not in generated_cpp


def test_value_handle_requires_none_destructor(tmp_path: Path) -> None:
    spec_path = tmp_path / "bad_value.yml"
    spec_path.write_text(
        dedent(
            """
            schema_version: 1
            module: demo
            slice: demo
            c_prefix: ifcopenshell_demo
            public_headers: []
            handles:
              - name: value
                cpp_type: Demo::Value
                c_type: ifcopenshell_demo_value_t
                destructor: delete
                ptr_type: value
            """
        ).strip()
        + "\n",
        encoding="utf-8",
    )

    with pytest.raises(ValueError, match="destructor must be 'none' for ptr_type: value"):
        load_authored_spec(spec_path)


def test_discovery_type_overrides_can_target_canonical_overload_signature(tmp_path: Path) -> None:
    header = tmp_path / "overloaded.h"
    source = tmp_path / "overloaded.cpp"
    spec_path = tmp_path / "overloaded.yml"

    header.write_text(
        dedent(
            """
            namespace Demo {
            class Overloaded {
            public:
                int value() const;
                int value(int amount) const;
            };
            }
            """
        ).strip()
        + "\n",
        encoding="utf-8",
    )
    source.write_text('#include "overloaded.h"\n', encoding="utf-8")
    compile_commands = _write_compile_commands(tmp_path, source)

    spec_path.write_text(
        dedent(
            """
            schema_version: 1
            module: demo
            slice: demo
            c_prefix: ifcopenshell_demo
            public_headers:
              - overloaded.h
            handles:
              - name: overloaded
                cpp_type: Demo::Overloaded
                c_type: ifcopenshell_demo_overloaded_t
                destructor: delete
            discover:
              include_dir: .
              classes:
                - handle: overloaded
                  translation_unit: overloaded.cpp
                  overloads:
                    - cpp_name: value
                      expose_as: value
                      params: []
                    - cpp_name: value
                      expose_as: value_with_amount
                      params:
                        - int
                  type_overrides:
                    "value()":
                      returns:
                        kind: double
            """
        ).strip()
        + "\n",
        encoding="utf-8",
    )

    spec = load_authored_spec(spec_path, compile_commands_path=compile_commands)
    calls = {call.c_name: call for call in spec.methods}

    assert calls["ifcopenshell_demo_overloaded_value"].returns.kind == "double"
    assert calls["ifcopenshell_demo_overloaded_value_with_amount"].returns.kind == "int32"


def test_discovery_rejects_name_scoped_type_override_for_overloaded_member(tmp_path: Path) -> None:
    header = tmp_path / "overloaded.h"
    source = tmp_path / "overloaded.cpp"
    spec_path = tmp_path / "overloaded.yml"

    header.write_text(
        dedent(
            """
            namespace Demo {
            class Overloaded {
            public:
                int value() const;
                int value(int amount) const;
            };
            }
            """
        ).strip()
        + "\n",
        encoding="utf-8",
    )
    source.write_text('#include "overloaded.h"\n', encoding="utf-8")
    compile_commands = _write_compile_commands(tmp_path, source)

    spec_path.write_text(
        dedent(
            """
            schema_version: 1
            module: demo
            slice: demo
            c_prefix: ifcopenshell_demo
            public_headers:
              - overloaded.h
            handles:
              - name: overloaded
                cpp_type: Demo::Overloaded
                c_type: ifcopenshell_demo_overloaded_t
                destructor: delete
            discover:
              include_dir: .
              classes:
                - handle: overloaded
                  translation_unit: overloaded.cpp
                  overloads:
                    - cpp_name: value
                      expose_as: value
                      params: []
                    - cpp_name: value
                      expose_as: value_with_amount
                      params:
                        - int
                  type_overrides:
                    value:
                      returns:
                        kind: double
            """
        ).strip()
        + "\n",
        encoding="utf-8",
    )

    with pytest.raises(ValueError, match="canonical signature keys"):
        load_authored_spec(spec_path, compile_commands_path=compile_commands)


def test_autodiscovery_uses_marked_contract_when_translation_unit_is_omitted(tmp_path: Path) -> None:
    header = tmp_path / "bindings.h"
    source = tmp_path / "reference.cpp"
    spec_path = tmp_path / "bindings.yml"

    header.write_text(
        dedent(
            """
            #include <string>
            #include <vector>
            #define IFCAPI_BINDING
            #define IFCAPI_OWNED
            #define IFCAPI_COPY
            #define IFCAPI_NULLABLE
            struct DemoValue {};

            namespace ifcapi::bindings {
            IFCAPI_BINDING int contract_count(const std::string& name);
            IFCAPI_BINDING double contract_scale(double value);
            IFCAPI_BINDING IFCAPI_OWNED IFCAPI_NULLABLE DemoValue* contract_value(
                IFCAPI_NULLABLE DemoValue* input,
                const std::vector<int>& values);
            IFCAPI_BINDING IFCAPI_COPY std::vector<DemoValue*> contract_copies();
            int internal_helper();
            }
            """
        ).strip()
        + "\n",
        encoding="utf-8",
    )
    source.write_text("int reference() { return 0; }\n", encoding="utf-8")
    compile_commands = _write_compile_commands(tmp_path, source)

    spec_path.write_text(
        dedent(
            """
            schema_version: 1
            module: demo
            slice: demo
            c_prefix: ifcopenshell_demo
            public_headers:
              - bindings.h
            handles:
              - name: demo_value
                cpp_type: DemoValue
                c_type: ifcopenshell_demo_value_t
                destructor: delete
            discover:
              include_dir: .
              functions:
                - namespace: ifcapi::bindings
                  type_overrides:
                    contract_scale:
                      returns:
                        kind: double
            """
        ).strip()
        + "\n",
        encoding="utf-8",
    )

    spec = load_authored_spec(spec_path, compile_commands_path=compile_commands)
    calls = {call.c_name: call for call in spec.functions}

    assert set(calls) == {
        "ifcopenshell_demo_contract_count",
        "ifcopenshell_demo_contract_copies",
        "ifcopenshell_demo_contract_scale",
        "ifcopenshell_demo_contract_value",
    }
    assert calls["ifcopenshell_demo_contract_count"].params[0].type.kind == "string"
    assert calls["ifcopenshell_demo_contract_copies"].returns.ownership == "copy"
    assert calls["ifcopenshell_demo_contract_scale"].returns.kind == "double"
    value_call = calls["ifcopenshell_demo_contract_value"]
    assert value_call.returns.handle == "demo_value"
    assert value_call.returns.ownership == "owned"
    assert value_call.returns.nullable is True
    assert value_call.params[0].type.nullable is True
    assert value_call.params[1].name == "values"


def test_contract_discovery_rejects_unmarked_policy(tmp_path: Path) -> None:
    header = tmp_path / "bindings.h"
    source = tmp_path / "reference.cpp"
    spec_path = tmp_path / "bindings.yml"

    header.write_text(
        dedent(
            """
            #define IFCAPI_BINDING

            namespace ifcapi::bindings {
            IFCAPI_BINDING int contract_count();
            int internal_helper();
            }
            """
        ).strip()
        + "\n",
        encoding="utf-8",
    )
    source.write_text("int reference() { return 0; }\n", encoding="utf-8")
    compile_commands = _write_compile_commands(tmp_path, source)

    spec_path.write_text(
        dedent(
            """
            schema_version: 1
            module: demo
            slice: demo
            c_prefix: ifcopenshell_demo
            public_headers:
              - bindings.h
            discover:
              include_dir: .
              functions:
                - namespace: ifcapi::bindings
                  type_overrides:
                    internal_helper:
                      returns:
                        kind: int32
            """
        ).strip()
        + "\n",
        encoding="utf-8",
    )

    with pytest.raises(ValueError, match="type overrides for unmarked functions"):
        load_authored_spec(spec_path, compile_commands_path=compile_commands)


def test_contract_discovery_rejects_duplicate_header_policy(tmp_path: Path) -> None:
    header = tmp_path / "bindings.h"
    source = tmp_path / "reference.cpp"
    spec_path = tmp_path / "bindings.yml"

    header.write_text(
        dedent(
            """
            #define IFCAPI_BINDING
            #define IFCAPI_OWNED
            struct DemoValue {};

            namespace ifcapi::bindings {
            IFCAPI_BINDING IFCAPI_OWNED DemoValue* contract_value();
            }
            """
        ).strip()
        + "\n",
        encoding="utf-8",
    )
    source.write_text("int reference() { return 0; }\n", encoding="utf-8")
    compile_commands = _write_compile_commands(tmp_path, source)

    spec_path.write_text(
        dedent(
            """
            schema_version: 1
            module: demo
            slice: demo
            c_prefix: ifcopenshell_demo
            public_headers:
              - bindings.h
            handles:
              - name: demo_value
                cpp_type: DemoValue
                c_type: ifcopenshell_demo_value_t
                destructor: delete
            discover:
              include_dir: .
              functions:
                - namespace: ifcapi::bindings
                  type_overrides:
                    contract_value:
                      returns:
                        ownership: owned
            """
        ).strip()
        + "\n",
        encoding="utf-8",
    )

    with pytest.raises(ValueError, match="duplicate return policy"):
        load_authored_spec(spec_path, compile_commands_path=compile_commands)


def test_contract_discovery_rejects_duplicate_parameter_policy(tmp_path: Path) -> None:
    header = tmp_path / "bindings.h"
    source = tmp_path / "reference.cpp"
    spec_path = tmp_path / "bindings.yml"

    header.write_text(
        dedent(
            """
            #define IFCAPI_BINDING
            #define IFCAPI_NULLABLE
            struct DemoValue {};

            namespace ifcapi::bindings {
            IFCAPI_BINDING void contract_value(IFCAPI_NULLABLE DemoValue* value);
            }
            """
        ).strip()
        + "\n",
        encoding="utf-8",
    )
    source.write_text("int reference() { return 0; }\n", encoding="utf-8")
    compile_commands = _write_compile_commands(tmp_path, source)

    spec_path.write_text(
        dedent(
            """
            schema_version: 1
            module: demo
            slice: demo
            c_prefix: ifcopenshell_demo
            public_headers:
              - bindings.h
            handles:
              - name: demo_value
                cpp_type: DemoValue
                c_type: ifcopenshell_demo_value_t
                destructor: delete
            discover:
              include_dir: .
              functions:
                - namespace: ifcapi::bindings
                  type_overrides:
                    contract_value:
                      params:
                        value:
                          nullable: true
            """
        ).strip()
        + "\n",
        encoding="utf-8",
    )

    with pytest.raises(ValueError, match="duplicate parameter policy"):
        load_authored_spec(spec_path, compile_commands_path=compile_commands)


def test_contract_discovery_rejects_missing_public_header(tmp_path: Path) -> None:
    source = tmp_path / "reference.cpp"
    spec_path = tmp_path / "bindings.yml"

    source.write_text("int reference() { return 0; }\n", encoding="utf-8")
    compile_commands = _write_compile_commands(tmp_path, source)

    spec_path.write_text(
        dedent(
            """
            schema_version: 1
            module: demo
            slice: demo
            c_prefix: ifcopenshell_demo
            public_headers:
              - missing_bindings.h
            discover:
              include_dir: .
              functions:
                - namespace: ifcapi::bindings
            """
        ).strip()
        + "\n",
        encoding="utf-8",
    )

    with pytest.raises(FileNotFoundError, match="missing_bindings.h"):
        load_authored_spec(spec_path, compile_commands_path=compile_commands)


def test_namespace_function_discovery_rejects_stale_type_override(tmp_path: Path) -> None:
    header = tmp_path / "bindings.h"
    source = tmp_path / "bindings.cpp"
    spec_path = tmp_path / "bindings.yml"

    header.write_text(
        dedent(
            """
            namespace Demo {
            int count();
            }
            """
        ).strip()
        + "\n",
        encoding="utf-8",
    )
    source.write_text('#include "bindings.h"\n', encoding="utf-8")
    compile_commands = _write_compile_commands(tmp_path, source)

    spec_path.write_text(
        dedent(
            """
            schema_version: 1
            module: demo
            slice: demo
            c_prefix: ifcopenshell_demo
            public_headers:
              - bindings.h
            discover:
              include_dir: .
              functions:
                - namespace: Demo
                  translation_unit: bindings.cpp
                  include:
                    - count
                  type_overrides:
                    missing:
                      returns:
                        kind: int32
            """
        ).strip()
        + "\n",
        encoding="utf-8",
    )

    with pytest.raises(ValueError, match="type overrides for unknown functions"):
        load_authored_spec(spec_path, compile_commands_path=compile_commands)


def test_autodiscovery_treats_char_pointer_params_as_nullable_strings(tmp_path: Path) -> None:
    header = tmp_path / "nullable_strings.h"
    source = tmp_path / "nullable_strings.cpp"
    spec_path = tmp_path / "nullable_strings.yml"

    header.write_text(
        dedent(
            """
            namespace Demo {
            int maybe_named(const char* name);
            int definitely_named(const std::string& name);
            }
            """
        ).strip()
        + "\n",
        encoding="utf-8",
    )
    source.write_text('#include <string>\n#include "nullable_strings.h"\n', encoding="utf-8")
    compile_commands = _write_compile_commands(tmp_path, source)

    spec_path.write_text(
        dedent(
            """
            schema_version: 1
            module: demo
            slice: demo
            c_prefix: ifcopenshell_demo
            public_headers:
              - nullable_strings.h
            discover:
              include_dir: .
              functions:
                - namespace: Demo
                  translation_unit: nullable_strings.cpp
                  include:
                    - maybe_named
                    - definitely_named
            """
        ).strip()
        + "\n",
        encoding="utf-8",
    )

    spec = load_authored_spec(spec_path, compile_commands_path=compile_commands)
    calls = {call.c_name: call for call in spec.functions}

    assert calls["ifcopenshell_demo_maybe_named"].params[0].type.kind == "string"
    assert calls["ifcopenshell_demo_maybe_named"].params[0].type.nullable
    assert calls["ifcopenshell_demo_definitely_named"].params[0].type.kind == "string"
    assert not calls["ifcopenshell_demo_definitely_named"].params[0].type.nullable

    header_out = tmp_path / "nullable_strings_api.h"
    cpp_out = tmp_path / "nullable_strings_api.cpp"
    generate(spec_path, header_out, cpp_out, compile_commands_path=compile_commands)
    generated_cpp = cpp_out.read_text(encoding="utf-8")
    assert "Demo::maybe_named(name)" in generated_cpp
    assert "std::string name_cpp(name);" in generated_cpp
    assert "Demo::definitely_named(name_cpp)" in generated_cpp


def test_namespace_function_discovery_infers_result_struct_returns(tmp_path: Path) -> None:
    header = tmp_path / "result_structs.h"
    source = tmp_path / "result_structs.cpp"
    spec_path = tmp_path / "result_structs.yml"

    header.write_text(
        dedent(
            """
            namespace Demo {
            struct PairResult {
                int left;
                int right;
            };

            PairResult make_pair_result();
            }
            """
        ).strip()
        + "\n",
        encoding="utf-8",
    )
    source.write_text('#include "result_structs.h"\n', encoding="utf-8")
    compile_commands = _write_compile_commands(tmp_path, source)

    spec_path.write_text(
        dedent(
            """
            schema_version: 1
            module: demo
            slice: demo
            c_prefix: ifcopenshell_demo
            public_headers:
              - result_structs.h
            result_structs:
              - name: pair_result
                cpp_type: Demo::PairResult
                c_type: ifcopenshell_demo_pair_result_t
                fields:
                  - name: left
                    type: {kind: int32}
                  - name: right
                    type: {kind: int32}
            discover:
              include_dir: .
              functions:
                - namespace: Demo
                  translation_unit: result_structs.cpp
                  include:
                    - make_pair_result
            """
        ).strip()
        + "\n",
        encoding="utf-8",
    )

    spec = load_authored_spec(spec_path, compile_commands_path=compile_commands)
    calls = {call.c_name: call for call in spec.functions}

    assert calls["ifcopenshell_demo_make_pair_result"].returns.kind == "struct"
    assert calls["ifcopenshell_demo_make_pair_result"].returns.struct == "pair_result"


def test_namespace_function_discovery_infers_tribool_as_logical(tmp_path: Path) -> None:
    header = tmp_path / "logical.h"
    source = tmp_path / "logical.cpp"
    spec_path = tmp_path / "logical.yml"

    header.write_text(
        dedent(
            """
            namespace boost::logic {
            class tribool {};
            }

            namespace Demo {
            bool set_flags(boost::logic::tribool enabled);
            }
            """
        ).strip()
        + "\n",
        encoding="utf-8",
    )
    source.write_text('#include "logical.h"\n', encoding="utf-8")
    compile_commands = _write_compile_commands(tmp_path, source)

    spec_path.write_text(
        dedent(
            """
            schema_version: 1
            module: demo
            slice: demo
            c_prefix: ifcopenshell_demo
            public_headers:
              - logical.h
            discover:
              include_dir: .
              functions:
                - namespace: Demo
                  translation_unit: logical.cpp
                  include:
                    - set_flags
            """
        ).strip()
        + "\n",
        encoding="utf-8",
    )

    spec = load_authored_spec(spec_path, compile_commands_path=compile_commands)
    calls = {call.c_name: call for call in spec.functions}

    assert calls["ifcopenshell_demo_set_flags"].params[0].type.kind == "logical"


def test_namespace_function_discovery_infers_unknown_raw_pointers_as_opaque(tmp_path: Path) -> None:
    header = tmp_path / "opaque.h"
    source = tmp_path / "opaque.cpp"
    spec_path = tmp_path / "opaque.yml"

    header.write_text(
        dedent(
            """
            namespace Demo {
            struct Options;
            Options* create_options();
            bool use_options(Options* options);
            }
            """
        ).strip()
        + "\n",
        encoding="utf-8",
    )
    source.write_text('#include "opaque.h"\n', encoding="utf-8")
    compile_commands = _write_compile_commands(tmp_path, source)

    spec_path.write_text(
        dedent(
            """
            schema_version: 1
            module: demo
            slice: demo
            c_prefix: ifcopenshell_demo
            public_headers:
              - opaque.h
            discover:
              include_dir: .
              functions:
                - namespace: Demo
                  translation_unit: opaque.cpp
                  include:
                    - create_options
                    - use_options
                  type_overrides:
                    use_options:
                      params:
                        options:
                          nullable: true
            """
        ).strip()
        + "\n",
        encoding="utf-8",
    )

    spec = load_authored_spec(spec_path, compile_commands_path=compile_commands)
    calls = {call.c_name: call for call in spec.functions}

    assert calls["ifcopenshell_demo_create_options"].returns.kind == "opaque_ptr"
    assert calls["ifcopenshell_demo_create_options"].returns.cpp_type == "Options*"
    assert calls["ifcopenshell_demo_create_options"].returns.nullable
    assert calls["ifcopenshell_demo_use_options"].params[0].type.kind == "opaque_ptr"
    assert calls["ifcopenshell_demo_use_options"].params[0].type.cpp_type == "Options*"
    assert calls["ifcopenshell_demo_use_options"].params[0].type.nullable

    header_out = tmp_path / "opaque_api.h"
    cpp_out = tmp_path / "opaque_api.cpp"
    generate(spec_path, header_out, cpp_out, compile_commands_path=compile_commands)
    generated_cpp = cpp_out.read_text(encoding="utf-8")
    assert 'Parameter "options" must not be null' not in generated_cpp
    assert "auto options_cpp = static_cast<Options*>(options);" in generated_cpp


def test_autodiscovery_supports_shared_ptr_handle_vectors(tmp_path: Path) -> None:
    header = tmp_path / "shared_ptr_vectors.h"
    source = tmp_path / "shared_ptr_vectors.cpp"
    spec_path = tmp_path / "shared_ptr_vectors.yml"

    header.write_text(
        dedent(
            """
            #include <memory>
            #include <vector>

            namespace Demo {
            struct Child {
                using ptr = std::shared_ptr<Child>;
            };

            struct Widget {
            public:
                void set_children(const std::vector<Child::ptr>& children) {}
                std::vector<Child::ptr> children() const { return {}; }
            };
            }
            """
        ).strip()
        + "\n",
        encoding="utf-8",
    )
    source.write_text('#include "shared_ptr_vectors.h"\n', encoding="utf-8")
    compile_commands = _write_compile_commands(tmp_path, source)

    spec_path.write_text(
        dedent(
            """
            schema_version: 1
            module: demo
            slice: demo
            c_prefix: ifcopenshell_demo
            public_headers:
              - shared_ptr_vectors.h
            handles:
              - name: widget
                cpp_type: Demo::Widget
                c_type: ifcopenshell_demo_widget_t
                destructor: delete
              - name: child
                cpp_type: Demo::Child
                c_type: ifcopenshell_demo_child_t
                destructor: shared_ptr
                ptr_type: shared_ptr
            discover:
              include_dir: .
              classes:
                - handle: widget
                  translation_unit: shared_ptr_vectors.cpp
                  include:
                    - set_children
                    - children
            """
        ).strip()
        + "\n",
        encoding="utf-8",
    )

    spec = load_authored_spec(spec_path, compile_commands_path=compile_commands)
    calls = {call.c_name: call for call in spec.methods}

    assert calls["ifcopenshell_demo_widget_set_children"].params[0].type.kind == "handle"
    assert calls["ifcopenshell_demo_widget_set_children"].params[0].type.sequence_depth == 1
    assert calls["ifcopenshell_demo_widget_set_children"].params[0].type.handle == "child"
    assert calls["ifcopenshell_demo_widget_children"].returns.kind == "handle"
    assert calls["ifcopenshell_demo_widget_children"].returns.sequence_depth == 1
    assert calls["ifcopenshell_demo_widget_children"].returns.handle == "child"
    assert calls["ifcopenshell_demo_widget_children"].returns.cpp_type == "std::vector<Demo::Child::ptr>"

    header_out = tmp_path / "shared_ptr_vectors_api.h"
    cpp_out = tmp_path / "shared_ptr_vectors_api.cpp"
    generate(spec_path, header_out, cpp_out, compile_commands_path=compile_commands)

    generated_cpp = cpp_out.read_text(encoding="utf-8")
    assert "auto children_cpp = to_cpp_demo_child_list(children);" in generated_cpp
    assert "*out_result = make_demo_child_list(self_cpp->children());" in generated_cpp


def test_autodiscovery_supports_boost_shared_ptr_handles(tmp_path: Path) -> None:
    header = tmp_path / "boost_shared_ptr_handles.h"
    source = tmp_path / "boost_shared_ptr_handles.cpp"
    spec_path = tmp_path / "boost_shared_ptr_handles.yml"

    header.write_text(
        dedent(
            """
            namespace boost {
            template <typename T>
            class shared_ptr {
            public:
                shared_ptr() = default;
            };
            }

            namespace Demo {
            struct Collection {
                using ptr = boost::shared_ptr<Collection>;
            };

            struct Widget {
                void set_collection(Collection::ptr collection) {}
                boost::shared_ptr<Collection> collection() const { return {}; }
            };
            }
            """
        ).strip()
        + "\n",
        encoding="utf-8",
    )
    source.write_text('#include "boost_shared_ptr_handles.h"\n', encoding="utf-8")
    compile_commands = _write_compile_commands(tmp_path, source)

    spec_path.write_text(
        dedent(
            """
            schema_version: 1
            module: demo
            slice: demo
            c_prefix: ifcopenshell_demo
            public_headers:
              - boost_shared_ptr_handles.h
            handles:
              - name: widget
                cpp_type: Demo::Widget
                c_type: ifcopenshell_demo_widget_t
                destructor: delete
              - name: collection
                cpp_type: Demo::Collection
                c_type: ifcopenshell_demo_collection_t
                destructor: shared_ptr
                ptr_type: shared_ptr
            discover:
              include_dir: .
              classes:
                - handle: widget
                  translation_unit: boost_shared_ptr_handles.cpp
                  include:
                    - set_collection
                    - collection
            """
        ).strip()
        + "\n",
        encoding="utf-8",
    )

    spec = load_authored_spec(spec_path, compile_commands_path=compile_commands)
    calls = {call.c_name: call for call in spec.methods}

    assert calls["ifcopenshell_demo_widget_set_collection"].params[0].type.kind == "handle"
    assert calls["ifcopenshell_demo_widget_set_collection"].params[0].type.handle == "collection"
    assert calls["ifcopenshell_demo_widget_collection"].returns.kind == "handle"
    assert calls["ifcopenshell_demo_widget_collection"].returns.handle == "collection"


def test_autodiscovery_supports_unique_ptr_handles(tmp_path: Path) -> None:
    header = tmp_path / "unique_ptr_handles.h"
    source = tmp_path / "unique_ptr_handles.cpp"
    spec_path = tmp_path / "unique_ptr_handles.yml"

    header.write_text(
        dedent(
            """
            #include <memory>
            #include <vector>

            namespace Demo {
            struct Child {};

            struct Widget {
                std::unique_ptr<Child> take_child() const { return {}; }
                std::vector<std::unique_ptr<Child>> steal_children() const { return {}; }
            };
            }
            """
        ).strip()
        + "\n",
        encoding="utf-8",
    )
    source.write_text('#include "unique_ptr_handles.h"\n', encoding="utf-8")
    compile_commands = _write_compile_commands(tmp_path, source)

    spec_path.write_text(
        dedent(
            """
            schema_version: 1
            module: demo
            slice: demo
            c_prefix: ifcopenshell_demo
            public_headers:
              - unique_ptr_handles.h
            handles:
              - name: widget
                cpp_type: Demo::Widget
                c_type: ifcopenshell_demo_widget_t
                destructor: delete
              - name: child
                cpp_type: Demo::Child
                c_type: ifcopenshell_demo_child_t
                destructor: delete
            discover:
              include_dir: .
              classes:
                - handle: widget
                  translation_unit: unique_ptr_handles.cpp
                  include:
                    - take_child
                    - steal_children
            """
        ).strip()
        + "\n",
        encoding="utf-8",
    )

    spec = load_authored_spec(spec_path, compile_commands_path=compile_commands)
    calls = {call.c_name: call for call in spec.methods}

    assert calls["ifcopenshell_demo_widget_take_child"].returns.kind == "handle"
    assert calls["ifcopenshell_demo_widget_take_child"].returns.handle == "child"
    assert calls["ifcopenshell_demo_widget_take_child"].returns.ownership == "owned"
    assert calls["ifcopenshell_demo_widget_steal_children"].returns.kind == "handle"
    assert calls["ifcopenshell_demo_widget_steal_children"].returns.handle == "child"
    assert calls["ifcopenshell_demo_widget_steal_children"].returns.sequence_depth == 1

    header_out = tmp_path / "unique_ptr_handles_api.h"
    cpp_out = tmp_path / "unique_ptr_handles_api.cpp"
    generate(spec_path, header_out, cpp_out, compile_commands_path=compile_commands)
    generated_cpp = cpp_out.read_text(encoding="utf-8")

    assert "*out_result = new ifcopenshell_demo_child_t{self_cpp->take_child().release(), true};" in generated_cpp
    assert "static ifcopenshell_demo_child_list_t make_demo_child_list(std::vector<std::unique_ptr<Demo::Child>> values)" in generated_cpp
    assert "items[i] = new ifcopenshell_demo_child_t{values[i].release(), true};" in generated_cpp


def test_autodiscovery_matches_shared_ptr_handles_by_qualified_suffix(tmp_path: Path) -> None:
    header = tmp_path / "qualified_shared_ptr_suffix.h"
    source = tmp_path / "qualified_shared_ptr_suffix.cpp"
    spec_path = tmp_path / "qualified_shared_ptr_suffix.yml"

    header.write_text(
        dedent(
            """
            #include <memory>

            namespace Demo::geometry::taxonomy {
            struct item {
                using ptr = std::shared_ptr<item>;
            };
            }

            namespace Demo::geometry {
            struct Evaluator {
                taxonomy::item::ptr evaluate() const { return {}; }
            };
            }
            """
        ).strip()
        + "\n",
        encoding="utf-8",
    )
    source.write_text('#include "qualified_shared_ptr_suffix.h"\n', encoding="utf-8")
    compile_commands = _write_compile_commands(tmp_path, source)

    spec_path.write_text(
        dedent(
            """
            schema_version: 1
            module: demo
            slice: demo
            c_prefix: ifcopenshell_demo
            public_headers:
              - qualified_shared_ptr_suffix.h
            handles:
              - name: evaluator
                cpp_type: Demo::geometry::Evaluator
                c_type: ifcopenshell_demo_evaluator_t
                destructor: delete
              - name: taxonomy_item
                cpp_type: Demo::geometry::taxonomy::item
                c_type: ifcopenshell_demo_taxonomy_item_t
                destructor: shared_ptr
                ptr_type: shared_ptr
            discover:
              include_dir: .
              classes:
                - handle: evaluator
                  translation_unit: qualified_shared_ptr_suffix.cpp
                  include:
                    - evaluate
            """
        ).strip()
        + "\n",
        encoding="utf-8",
    )

    spec = load_authored_spec(spec_path, compile_commands_path=compile_commands)
    calls = {call.c_name: call for call in spec.methods}

    assert calls["ifcopenshell_demo_evaluator_evaluate"].returns.kind == "handle"
    assert calls["ifcopenshell_demo_evaluator_evaluate"].returns.handle == "taxonomy_item"
    assert calls["ifcopenshell_demo_evaluator_evaluate"].returns.ownership == "owned"
    assert calls["ifcopenshell_demo_evaluator_evaluate"].returns.cpp_type == "taxonomy::item::ptr"


def test_autodiscovery_supports_opaque_coordinate_vectors(tmp_path: Path) -> None:
    header = tmp_path / "opaque_coordinate_vectors.h"
    source = tmp_path / "opaque_coordinate_vectors.cpp"
    spec_path = tmp_path / "opaque_coordinate_vectors.yml"

    header.write_text(
        dedent(
            """
            #include <array>
            #include <vector>

            namespace IfcGeom {
            class OpaqueNumber {
            public:
                virtual double to_double() const = 0;
                virtual OpaqueNumber* clone() const = 0;
                virtual ~OpaqueNumber() = default;
            };

            class NumberNativeDouble : public OpaqueNumber {
            public:
                explicit NumberNativeDouble(double value) : value_(value) {}
                double to_double() const override { return value_; }
                OpaqueNumber* clone() const override { return new NumberNativeDouble(value_); }
            private:
                double value_;
            };

            template <size_t N>
            struct OpaqueCoordinate {
                OpaqueCoordinate() {
                    values_.fill(nullptr);
                }

                template <typename... Args>
                explicit OpaqueCoordinate(Args... args) {
                    static_assert(sizeof...(args) == N);
                    OpaqueNumber* values[] = {args...};
                    for (size_t i = 0; i < N; ++i) {
                        values_[i] = values[i];
                    }
                }

                OpaqueNumber* get(size_t i) const {
                    return i < N ? values_[i] : nullptr;
                }

                std::array<OpaqueNumber*, N> values_;
            };
            }

            namespace Demo {
            struct Widget {
            public:
                void set_planes(const std::vector<IfcGeom::OpaqueCoordinate<4>>& planes) {}
                std::vector<IfcGeom::OpaqueCoordinate<4>> planes() const { return {}; }
            };
            }
            """
        ).strip()
        + "\n",
        encoding="utf-8",
    )
    source.write_text('#include "opaque_coordinate_vectors.h"\n', encoding="utf-8")
    compile_commands = _write_compile_commands(tmp_path, source)

    spec_path.write_text(
        dedent(
            """
            schema_version: 1
            module: demo
            slice: demo
            c_prefix: ifcopenshell_demo
            public_headers:
              - opaque_coordinate_vectors.h
            handles:
              - name: widget
                cpp_type: Demo::Widget
                c_type: ifcopenshell_demo_widget_t
                destructor: delete
            methods:
              - receiver: widget
                expose_as: set_planes
                returns:
                  kind: void
                params:
                  - name: planes
                    type:
                      kind: double_list_list
                implementation:
                  kind: inline_cpp
                  body: |
                    std::vector<IfcGeom::OpaqueCoordinate<4>> opaque_planes;
                    opaque_planes.reserve(planes_cpp.size());
                    for (const auto& row : planes_cpp) {
                        if (row.size() != 4) {
                            throw std::runtime_error("double_list_list row has incorrect length for opaque coordinate");
                        }
                        opaque_planes.emplace_back(
                            new IfcGeom::NumberNativeDouble(row.at(0)),
                            new IfcGeom::NumberNativeDouble(row.at(1)),
                            new IfcGeom::NumberNativeDouble(row.at(2)),
                            new IfcGeom::NumberNativeDouble(row.at(3))
                        );
                    }
                    self_cpp->set_planes(opaque_planes);
              - receiver: widget
                expose_as: planes
                returns:
                  kind: double_list_list
                params: []
                implementation:
                  kind: inline_cpp
                  body: |
                    auto tmp = self_cpp->planes();
                    std::vector<std::vector<double>> rows_out;
                    rows_out.reserve(tmp.size());
                    for (const auto& coord : tmp) {
                        rows_out.push_back(std::vector<double>{
                            coord.get(0) ? coord.get(0)->to_double() : 0.0,
                            coord.get(1) ? coord.get(1)->to_double() : 0.0,
                            coord.get(2) ? coord.get(2)->to_double() : 0.0,
                            coord.get(3) ? coord.get(3)->to_double() : 0.0
                        });
                    }
                    return rows_out;
                """
            ).strip()
        + "\n",
        encoding="utf-8",
    )

    spec = load_authored_spec(spec_path, compile_commands_path=compile_commands)
    calls = {call.c_name: call for call in spec.methods}

    assert calls["ifcopenshell_demo_widget_set_planes"].params[0].type.kind == "double"
    assert calls["ifcopenshell_demo_widget_set_planes"].params[0].type.sequence_depth == 2
    assert calls["ifcopenshell_demo_widget_planes"].returns.kind == "double"
    assert calls["ifcopenshell_demo_widget_planes"].returns.sequence_depth == 2

    header_out = tmp_path / "opaque_coordinate_vectors_api.h"
    cpp_out = tmp_path / "opaque_coordinate_vectors_api.cpp"
    generate(spec_path, header_out, cpp_out, compile_commands_path=compile_commands)

    generated_cpp = cpp_out.read_text(encoding="utf-8")
    assert "auto planes_cpp = to_cpp_double_list_list(planes);" in generated_cpp
    assert "opaque_planes.emplace_back(" in generated_cpp
    assert 'throw std::runtime_error("double_list_list row has incorrect length for opaque coordinate")' in generated_cpp
    assert "rows_out.push_back(std::vector<double>{" in generated_cpp
    assert "coord.get(0) ? coord.get(0)->to_double() : 0.0" in generated_cpp


def test_autodiscovery_supports_uint8_vectors(tmp_path: Path) -> None:
    header = tmp_path / "uint8_vectors.h"
    source = tmp_path / "uint8_vectors.cpp"
    spec_path = tmp_path / "uint8_vectors.yml"

    header.write_text(
        dedent(
            """
            #include <cstdint>
            #include <vector>

            namespace Demo {
            struct Widget {
            public:
                void set_bytes(const std::vector<uint8_t>& bytes) {}
                std::vector<uint8_t> bytes() const { return {}; }
            };
            }
            """
        ).strip()
        + "\n",
        encoding="utf-8",
    )
    source.write_text('#include "uint8_vectors.h"\n', encoding="utf-8")
    compile_commands = _write_compile_commands(tmp_path, source)

    spec_path.write_text(
        dedent(
            """
            schema_version: 1
            module: demo
            slice: demo
            c_prefix: ifcopenshell_demo
            public_headers:
              - uint8_vectors.h
            handles:
              - name: widget
                cpp_type: Demo::Widget
                c_type: ifcopenshell_demo_widget_t
                destructor: delete
            discover:
              include_dir: .
              classes:
                - handle: widget
                  translation_unit: uint8_vectors.cpp
                  include:
                    - set_bytes
                    - bytes
            """
        ).strip()
        + "\n",
        encoding="utf-8",
    )

    spec = load_authored_spec(spec_path, compile_commands_path=compile_commands)
    calls = {call.c_name: call for call in spec.methods}

    assert calls["ifcopenshell_demo_widget_set_bytes"].params[0].type.kind == "uint8"
    assert calls["ifcopenshell_demo_widget_set_bytes"].params[0].type.sequence_depth == 1
    assert calls["ifcopenshell_demo_widget_bytes"].returns.kind == "uint8"
    assert calls["ifcopenshell_demo_widget_bytes"].returns.sequence_depth == 1
    assert calls["ifcopenshell_demo_widget_bytes"].returns.cpp_type == "std::vector<uint8_t>"

    header_out = tmp_path / "uint8_vectors_api.h"
    cpp_out = tmp_path / "uint8_vectors_api.cpp"
    generate(spec_path, header_out, cpp_out, compile_commands_path=compile_commands)

    generated_header = header_out.read_text(encoding="utf-8")
    generated_cpp = cpp_out.read_text(encoding="utf-8")
    assert "typedef struct ifcopenshell_uint8_list_t {" in generated_header
    assert "void ifcopenshell_uint8_list_destroy(ifcopenshell_uint8_list_t* value);" in generated_header
    assert "auto bytes_cpp = to_cpp_uint8_list(bytes);" in generated_cpp
    assert "*out_result = make_uint8_list(self_cpp->bytes());" in generated_cpp


def test_autodiscovery_matches_handles_via_base_classes(tmp_path: Path) -> None:
    header = tmp_path / "base_handle_match.h"
    source = tmp_path / "base_handle_match.cpp"
    spec_path = tmp_path / "base_handle_match.yml"

    header.write_text(
        dedent(
            """
            namespace Demo {
            struct Base {
                virtual ~Base() = default;
            };

            struct Derived : Base {
            };

            struct Widget {
                const Derived* product() const { return nullptr; }
            };
            }
            """
        ).strip()
        + "\n",
        encoding="utf-8",
    )
    source.write_text('#include "base_handle_match.h"\n', encoding="utf-8")
    compile_commands = _write_compile_commands(tmp_path, source)

    spec_path.write_text(
        dedent(
            """
            schema_version: 1
            module: demo
            slice: demo
            c_prefix: ifcopenshell_demo
            public_headers:
              - base_handle_match.h
            handles:
              - name: base
                cpp_type: Demo::Base
                c_type: ifcopenshell_demo_base_t
                destructor: delete
              - name: widget
                cpp_type: Demo::Widget
                c_type: ifcopenshell_demo_widget_t
                destructor: delete
            discover:
              include_dir: .
              classes:
                - handle: widget
                  translation_unit: base_handle_match.cpp
                  include:
                    - product
            """
        ).strip()
        + "\n",
        encoding="utf-8",
    )

    spec = load_authored_spec(spec_path, compile_commands_path=compile_commands)
    calls = {call.c_name: call for call in spec.methods}

    assert calls["ifcopenshell_demo_widget_product"].returns.kind == "handle"
    assert calls["ifcopenshell_demo_widget_product"].returns.handle == "base"
    assert calls["ifcopenshell_demo_widget_product"].returns.cpp_type == "const Demo::Derived*"


def test_autodiscovery_supports_handle_list_list(tmp_path: Path) -> None:
    header = tmp_path / "handle_list_list.h"
    source = tmp_path / "handle_list_list.cpp"
    spec_path = tmp_path / "handle_list_list.yml"

    header.write_text(
        dedent(
            """
            #include <vector>

            namespace Demo {
            struct Base {
                virtual ~Base() = default;
            };

            struct Container {
                std::vector<std::vector<Base*>> groups() const { return {}; }
            };
            }
            """
        ).strip()
        + "\n",
        encoding="utf-8",
    )
    source.write_text('#include "handle_list_list.h"\n', encoding="utf-8")
    compile_commands = _write_compile_commands(tmp_path, source)

    spec_path.write_text(
        dedent(
            """
            schema_version: 1
            module: demo
            slice: demo
            c_prefix: ifcopenshell_demo
            public_headers:
              - handle_list_list.h
            handles:
              - name: base
                cpp_type: Demo::Base
                c_type: ifcopenshell_demo_base_t
                destructor: delete
              - name: container
                cpp_type: Demo::Container
                c_type: ifcopenshell_demo_container_t
                destructor: delete
            discover:
              include_dir: .
              classes:
                - handle: container
                  translation_unit: handle_list_list.cpp
                  include:
                    - groups
            """
        ).strip()
        + "\n",
        encoding="utf-8",
    )

    spec = load_authored_spec(spec_path, compile_commands_path=compile_commands)
    calls = {call.c_name: call for call in spec.methods}
    assert calls["ifcopenshell_demo_container_groups"].returns.kind == "handle"
    assert calls["ifcopenshell_demo_container_groups"].returns.sequence_depth == 2
    assert calls["ifcopenshell_demo_container_groups"].returns.handle == "base"

    header_out = tmp_path / "handle_list_list_api.h"
    cpp_out = tmp_path / "handle_list_list_api.cpp"
    generate(spec_path, header_out, cpp_out, compile_commands_path=compile_commands)

    generated_header = header_out.read_text(encoding="utf-8")
    generated_cpp = cpp_out.read_text(encoding="utf-8")
    assert "typedef struct ifcopenshell_demo_base_list_list_t {" in generated_header
    assert "void ifcopenshell_demo_base_list_list_destroy(ifcopenshell_demo_base_list_list_t* value);" in generated_header
    assert "*out_result = make_demo_base_list_list(self_cpp->groups());" in generated_cpp


def test_autodiscovery_supports_instance_aggregate_aggregate_ptr(tmp_path: Path) -> None:
    header = tmp_path / "aggregate_ptr.h"
    source = tmp_path / "aggregate_ptr.cpp"
    spec_path = tmp_path / "aggregate_ptr.yml"

    header.write_text(
        dedent(
            """
            namespace boost {
            template <typename T>
            class shared_ptr {
            public:
                shared_ptr() = default;
            };
            }

            namespace Demo {
            struct Base {
                virtual ~Base() = default;
            };

            struct aggregate_of_aggregate_of_instance {
                using ptr = boost::shared_ptr<aggregate_of_aggregate_of_instance>;
            };

            struct Iterator {
                aggregate_of_aggregate_of_instance::ptr get_task_products() const { return {}; }
            };
            }
            """
        ).strip()
        + "\n",
        encoding="utf-8",
    )
    source.write_text('#include "aggregate_ptr.h"\n', encoding="utf-8")
    compile_commands = _write_compile_commands(tmp_path, source)

    spec_path.write_text(
        dedent(
            """
            schema_version: 1
            module: demo
            slice: demo
            c_prefix: ifcopenshell_demo
            public_headers:
              - aggregate_ptr.h
            handles:
              - name: instance
                cpp_type: Demo::Base
                c_type: ifcopenshell_demo_instance_t
                destructor: delete
              - name: iterator
                cpp_type: Demo::Iterator
                c_type: ifcopenshell_demo_iterator_t
                destructor: delete
            discover:
              include_dir: .
              classes:
                    - handle: iterator
                      translation_unit: aggregate_ptr.cpp
                      include:
                        - get_task_products
                      type_overrides:
                        get_task_products:
                          returns:
                            kind: handle_list_list
                            handle: instance
                            ownership: borrowed
                """
            ).strip()
        + "\n",
        encoding="utf-8",
    )

    spec = load_authored_spec(spec_path, compile_commands_path=compile_commands)
    calls = {call.c_name: call for call in spec.methods}
    assert calls["ifcopenshell_demo_iterator_get_task_products"].returns.kind == "handle"
    assert calls["ifcopenshell_demo_iterator_get_task_products"].returns.sequence_depth == 2
    assert calls["ifcopenshell_demo_iterator_get_task_products"].returns.handle == "instance"


def test_autodiscovery_supports_int32_list_list_list(tmp_path: Path) -> None:
    header = tmp_path / "triple_int_vectors.h"
    source = tmp_path / "triple_int_vectors.cpp"
    spec_path = tmp_path / "triple_int_vectors.yml"

    header.write_text(
        dedent(
            """
            #include <vector>

            namespace Demo {
            struct Mesh {
                std::vector<std::vector<std::vector<int>>> polyhedral_faces_with_holes() const { return {}; }
            };
            }
            """
        ).strip()
        + "\n",
        encoding="utf-8",
    )
    source.write_text('#include "triple_int_vectors.h"\n', encoding="utf-8")
    compile_commands = _write_compile_commands(tmp_path, source)

    spec_path.write_text(
        dedent(
            """
            schema_version: 1
            module: demo
            slice: demo
            c_prefix: ifcopenshell_demo
            public_headers:
              - triple_int_vectors.h
            handles:
              - name: mesh
                cpp_type: Demo::Mesh
                c_type: ifcopenshell_demo_mesh_t
                destructor: delete
            discover:
              include_dir: .
              classes:
                - handle: mesh
                  translation_unit: triple_int_vectors.cpp
                  include:
                    - polyhedral_faces_with_holes
            """
        ).strip()
        + "\n",
        encoding="utf-8",
    )

    spec = load_authored_spec(spec_path, compile_commands_path=compile_commands)
    calls = {call.c_name: call for call in spec.methods}
    assert calls["ifcopenshell_demo_mesh_polyhedral_faces_with_holes"].returns.kind == "int32"
    assert calls["ifcopenshell_demo_mesh_polyhedral_faces_with_holes"].returns.sequence_depth == 3

    header_out = tmp_path / "triple_int_vectors_api.h"
    cpp_out = tmp_path / "triple_int_vectors_api.cpp"
    generate(spec_path, header_out, cpp_out, compile_commands_path=compile_commands)

    generated_header = header_out.read_text(encoding="utf-8")
    generated_cpp = cpp_out.read_text(encoding="utf-8")
    assert "typedef struct ifcopenshell_int32_list_list_list_t {" in generated_header
    assert "void ifcopenshell_int32_list_list_list_destroy(ifcopenshell_int32_list_list_list_t* value);" in generated_header
    assert "*out_result = make_int32_list_list_list(self_cpp->polyhedral_faces_with_holes());" in generated_cpp


def test_load_authored_spec_reports_discovery_diagnostics(tmp_path: Path) -> None:
    header = tmp_path / "diagnostics.h"
    source = tmp_path / "diagnostics.cpp"
    spec_path = tmp_path / "diagnostics.yml"

    header.write_text(
        dedent(
            """
            #include <map>

            namespace boost {
            template <typename T>
            struct optional {
                optional() = default;
                optional(const T&) {}
                bool is_initialized() const { return true; }
                T& operator*();
                const T& operator*() const;
            };
            }

            namespace Demo {
            struct Diagnostics {
            public:
                int ok() const { return 1; }
                int overloaded(int) const { return 1; }
                double overloaded(double) const { return 1.0; }
                std::map<int, int> unsupported_method() const { return {}; }

                std::map<int, int> unsupported_field;
                boost::optional<std::map<int, int>> unsupported_optional;
            };
            }
            """
        ).strip()
        + "\n",
        encoding="utf-8",
    )
    source.write_text('#include "diagnostics.h"\n', encoding="utf-8")
    compile_commands = _write_compile_commands(tmp_path, source)

    spec_path.write_text(
        dedent(
            """
            schema_version: 1
            module: demo
            slice: diagnostics
            c_prefix: ifcopenshell_demo
            public_headers:
              - diagnostics.h
            handles:
              - name: diagnostics
                cpp_type: Demo::Diagnostics
                c_type: ifcopenshell_demo_diagnostics_t
                destructor: delete
            discover:
              include_dir: .
              classes:
                - handle: diagnostics
                  translation_unit: diagnostics.cpp
                  include_all: true
                  discover_fields: true
                  discover_optional_fields: true
            """
        ).strip()
        + "\n",
        encoding="utf-8",
    )

    spec = load_authored_spec(spec_path, compile_commands_path=compile_commands)
    calls = {call.c_name: call for call in (*spec.functions, *spec.methods)}

    assert "ifcopenshell_demo_diagnostics_ok" in calls
    assert "ifcopenshell_demo_diagnostics_unsupported_method" not in calls
    assert "ifcopenshell_demo_diagnostics_unsupported_field" not in calls
    assert "ifcopenshell_demo_diagnostics_unsupported_optional" not in calls

    diagnostics = {(item.owner, item.member, item.code): item for item in spec.discovery_diagnostics}
    assert ("diagnostics", "overloaded", "overloaded_method_requires_policy") in diagnostics
    assert ("diagnostics", "unsupported_method", "unsupported_method_signature") in diagnostics
    assert ("diagnostics", "unsupported_field", "unsupported_field_type") in diagnostics
    assert ("diagnostics", "unsupported_optional", "unsupported_optional_inner_type") in diagnostics
    assert "std::map<int, int> unsupported_method()" in diagnostics[("diagnostics", "unsupported_method", "unsupported_method_signature")].message
    assert "std::map<int, int>" in diagnostics[("diagnostics", "unsupported_field", "unsupported_field_type")].message
