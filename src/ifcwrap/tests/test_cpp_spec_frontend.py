# This file was generated with the assistance of an AI coding tool.

from __future__ import annotations

import shutil
import subprocess
import sys
from pathlib import Path
from textwrap import dedent

import pytest

from src.ifcwrap.binding_generator.binding_ir import BindingIR, CallIR, DirectCallOp
from src.ifcwrap.binding_generator.binding_model import (
    HandleSpec,
    OptionStructFieldSpec,
    OptionStructSpec,
    ParamSpec,
    TypeSpec,
)
from src.ifcwrap.binding_generator.c_backend import _render_cpp, generate_cpp_specs
from src.ifcwrap.binding_generator.c_header_rendering import _render_header
from src.ifcwrap.binding_generator.clang_discovery import (
    CompilationConfig,
    DiscoveryEnvironment,
)
from src.ifcwrap.binding_generator.contract_discovery import (
    discover_marked_functions_in_headers,
)
from src.ifcwrap.binding_generator.cpp_spec_frontend import (
    discover_cpp_spec_functions,
    discover_cpp_spec_handles,
    discover_cpp_spec_option_structs,
    discover_cpp_spec_result_structs,
    lower_cpp_spec_functions_to_calls,
    lower_cpp_spec_result_structs_to_specs,
)
from src.ifcwrap.binding_generator.host_metadata import build_host_metadata


def _environment(tmp_path: Path) -> DiscoveryEnvironment:
    compiler = shutil.which("clang++")
    if compiler is None:
        pytest.skip("clang++ is not available")
    return DiscoveryEnvironment(
        compilation=CompilationConfig(
            compiler=compiler,
            include_dirs=(tmp_path,),
            working_directory=tmp_path,
        )
    )


def test_cpp_spec_generation_supports_per_spec_namespaces_and_prefixes(
    tmp_path: Path,
) -> None:
    spec_a = tmp_path / "spec_a.cpp"
    spec_a.write_text(
        dedent(
            """
            namespace example::a {
            inline int value() {
                return 1;
            }
            }
            """
        ),
        encoding="utf-8",
    )
    spec_b = tmp_path / "spec_b.cpp"
    spec_b.write_text(
        dedent(
            """
            namespace example::b {
            inline int value() {
                return 2;
            }
            }
            """
        ),
        encoding="utf-8",
    )

    header_out = tmp_path / "generated.h"
    cpp_out = tmp_path / "generated.cpp"
    generate_cpp_specs(
        [spec_a, spec_b],
        ["example::a", "example::b"],
        "example",
        "example",
        header_out,
        cpp_out,
        discovery_include_dirs=(tmp_path,),
        function_c_prefix=["example_a", "example_b"],
    )

    header = header_out.read_text(encoding="utf-8")
    assert "bool example_a_value(int32_t* out_result);" in header
    assert "bool example_b_value(int32_t* out_result);" in header
    generated_cpp = cpp_out.read_text(encoding="utf-8")
    assert "example::a::value()" in generated_cpp
    assert "example::b::value()" in generated_cpp


def test_cpp_spec_generation_rejects_mismatched_per_spec_namespaces(
    tmp_path: Path,
) -> None:
    spec_a = tmp_path / "spec_a.cpp"
    spec_b = tmp_path / "spec_b.cpp"
    for spec_path in (spec_a, spec_b):
        spec_path.write_text(
            dedent(
                """
                namespace example {
                inline int value() {
                    return 1;
                }
                }
                """
            ),
            encoding="utf-8",
        )

    with pytest.raises(
        ValueError,
        match="cpp_spec_namespace must be provided once or exactly once per --cpp-spec",
    ):
        generate_cpp_specs(
            [spec_a, spec_b],
            ["example", "example", "extra"],
            "example",
            "example",
            tmp_path / "generated.h",
            tmp_path / "generated.cpp",
            discovery_include_dirs=(tmp_path,),
        )


def test_cpp_spec_frontend_rejects_mutable_void_pointer_params(tmp_path: Path) -> None:
    spec_path = tmp_path / "demo_spec.cpp"
    spec_path.write_text(
        dedent(
            """
            namespace ifcopenshell::capi_spec {
            inline int ifcopenshell_demo_set_box(void* data) {
                return data ? 1 : 0;
            }
            }
            """
        ),
        encoding="utf-8",
    )

    functions = discover_cpp_spec_functions(
        _environment(tmp_path),
        spec_path,
        "ifcopenshell::capi_spec",
    )

    with pytest.raises(ValueError, match="Unsupported discovered parameter type 'void \\*'"):
        lower_cpp_spec_functions_to_calls(functions, {})


def test_contract_discovery_preserves_source_docs(tmp_path: Path) -> None:
    header = tmp_path / "demo.h"
    header.write_text(
        dedent(
            """
            #define IFCAPI_BINDING

            namespace demo {
            /**
             * Create an IFC entity.
             *
             * Initializes identity and ownership metadata.
             */
            IFCAPI_BINDING int root_create_entity(int file);
            }
            """
        ),
        encoding="utf-8",
    )

    functions = discover_marked_functions_in_headers((header,))

    assert len(functions) == 1
    assert functions[0].name == "root_create_entity"
    assert functions[0].doc == "Create an IFC entity.\n\nInitializes identity and ownership metadata."


def test_contract_discovery_uses_only_adjacent_function_docs(tmp_path: Path) -> None:
    header = tmp_path / "demo.h"
    header.write_text(
        dedent(
            """
            #define IFCAPI_BINDING

            namespace demo {
            /**
             * Options for entity creation.
             */
            struct CreateEntityOptions {
                /// IFC class name.
                const char* ifc_class;
            };

            /**
             * Create an IFC entity.
             */
            IFCAPI_BINDING int root_create_entity(const CreateEntityOptions& options);
            }
            """
        ),
        encoding="utf-8",
    )

    functions = discover_marked_functions_in_headers((header,))

    assert len(functions) == 1
    assert functions[0].name == "root_create_entity"
    assert functions[0].doc == "Create an IFC entity."


def test_cpp_spec_frontend_discovers_option_structs(tmp_path: Path) -> None:
    spec_path = tmp_path / "demo_spec.cpp"
    spec_path.write_text(
        dedent(
            """
            #include <optional>
            #include <string>

            namespace ifcopenshell::capi_spec {
            struct CreateEntityOptions {
                /**
                 * IFC class name.
                 *
                 * This paragraph should survive field documentation lowering.
                 */
                std::string ifc_class;
                /// Optional predefined type.
                std::optional<std::string> predefined_type;
                std::optional<std::string> name;
            };

            inline int root_create_entity(const CreateEntityOptions& options) {
                return options.ifc_class.empty() ? 0 : 1;
            }
            }
            """
        ),
        encoding="utf-8",
    )

    functions = discover_cpp_spec_functions(
        _environment(tmp_path),
        spec_path,
        "ifcopenshell::capi_spec",
    )
    option_structs = discover_cpp_spec_option_structs(
        _environment(tmp_path),
        spec_path,
        functions,
        {},
        c_prefix="ifcopenshell_demo",
    )

    options = option_structs["CreateEntityOptions"]
    assert options.cpp_type == "ifcopenshell::capi_spec::CreateEntityOptions"
    assert options.c_type == "ifcopenshell_demo_create_entity_options_t"
    fields = {field.name: field for field in options.fields}
    assert fields["ifc_class"].type.kind == "string"
    assert fields["ifc_class"].type.nullable is False
    assert fields["ifc_class"].doc == "IFC class name.\n\nThis paragraph should survive field documentation lowering."
    assert fields["predefined_type"].type.kind == "string"
    assert fields["predefined_type"].type.nullable is True
    assert fields["predefined_type"].doc == "Optional predefined type."
    assert fields["name"].type.kind == "string"
    assert fields["name"].type.nullable is True
    assert fields["name"].doc is None


def test_cpp_spec_result_field_docs_come_from_semantic_type(tmp_path: Path) -> None:
    spec_path = tmp_path / "demo_spec.cpp"
    spec_path.write_text(
        dedent(
            """
            #define IFCAPI_RESULT_STRUCT(...)

            namespace demo {
            struct SemanticResult {
                /// Documentation for the first semantic field.
                double first;
                /// Documentation for the second semantic field.
                double second;
            };
            }

            IFCAPI_RESULT_STRUCT(demo::SemanticResult)
            struct ifcopenshell_demo_result_t {
                /// This mirror comment must not be used.
                double second;
                /// This mirror comment must not be used either.
                double first;
            };
            """
        ),
        encoding="utf-8",
    )

    environment = _environment(tmp_path)
    structs = discover_cpp_spec_result_structs(spec_path, "demo")
    result = lower_cpp_spec_result_structs_to_specs(
        structs,
        {},
        environment=environment,
        translation_unit=spec_path,
    )["ifcopenshell_demo_result_t"]

    assert [(field.name, field.doc) for field in result.fields] == [
        ("second", "Documentation for the second semantic field."),
        ("first", "Documentation for the first semantic field."),
    ]


def test_c_header_renders_option_structs() -> None:
    code = _render_header(
        BindingIR(
            module="demo",
            c_prefix="ifcopenshell_demo",
            public_headers=(),
            handles={},
            result_structs={},
            functions=(),
            methods=(),
            option_structs={
                "CreateEntityOptions": OptionStructSpec(
                    name="CreateEntityOptions",
                    cpp_type="demo::CreateEntityOptions",
                    c_type="ifcopenshell_demo_create_entity_options_t",
                    fields=(
                        OptionStructFieldSpec("ifc_class", TypeSpec(kind="string")),
                        OptionStructFieldSpec("name", TypeSpec(kind="string", nullable=True)),
                    ),
                )
            },
        )
    )

    assert "typedef struct ifcopenshell_demo_create_entity_options_t {" in code
    assert "    const char* ifc_class;" in code
    assert "    const char* name;" in code
    assert "    bool has_name;" in code
    assert "} ifcopenshell_demo_create_entity_options_t;" in code


def test_c_abi_variants_destroy_owned_alternatives() -> None:
    variant = TypeSpec(
        kind="variant",
        cpp_type="std::variant<Demo::Instance*, std::string>",
        variants=(
            TypeSpec(
                kind="handle",
                handle="instance",
                cpp_type="Demo::Instance*",
                ownership="owned",
            ),
            TypeSpec(kind="string", cpp_type="std::string"),
        ),
    )
    spec = BindingIR(
        module="demo",
        c_prefix="ifcopenshell_demo",
        public_headers=(),
        handles={
            "instance": HandleSpec(
                name="instance",
                cpp_type="Demo::Instance",
                c_type="ifcopenshell_demo_instance_t",
                destructor="delete",
            )
        },
        result_structs={},
        functions=(
            CallIR(
                expose_as="value",
                c_name="ifcopenshell_demo_value",
                receiver=None,
                returns=variant,
                params=(),
                operation=DirectCallOp(cpp_name="Demo::value"),
            ),
        ),
        methods=(),
    )

    header = _render_header(spec)
    cpp = _render_cpp(spec, "demo_api.h")
    metadata = build_host_metadata(spec)

    assert (
        "void ifcopenshell_demo_instance_string_variant_destroy(ifcopenshell_demo_instance_string_variant_t* value);"
        in header
    )
    assert "case 0:\n        ifcopenshell_demo_instance_destroy(value->value_0);" in cpp
    assert "case 1:\n        ifcopenshell_string_destroy(&value->value_1);" in cpp
    assert metadata.value_types["demo_instance_string_variant"].destroy_function == (
        "ifcopenshell_demo_instance_string_variant_destroy"
    )


def test_c_backend_emits_sequence_helpers_used_only_by_option_structs() -> None:
    spec = BindingIR(
        module="demo",
        c_prefix="ifcopenshell_demo",
        public_headers=(),
        handles={},
        result_structs={},
        functions=(
            CallIR(
                expose_as="add_mesh",
                c_name="ifcopenshell_demo_add_mesh",
                receiver=None,
                returns=TypeSpec(kind="void"),
                params=(ParamSpec("options", TypeSpec(kind="option", struct="AddMeshOptions")),),
                operation=DirectCallOp(cpp_name="Demo::add_mesh"),
            ),
        ),
        methods=(),
        option_structs={
            "AddMeshOptions": OptionStructSpec(
                name="AddMeshOptions",
                cpp_type="Demo::AddMeshOptions",
                c_type="ifcopenshell_demo_add_mesh_options_t",
                fields=(
                    OptionStructFieldSpec("vertices", TypeSpec(kind="double", sequence_depth=3)),
                    OptionStructFieldSpec("faces", TypeSpec(kind="int32", sequence_depth=4)),
                ),
            )
        },
    )

    header = _render_header(spec)
    cpp = _render_cpp(spec, "demo_api.h")

    assert "typedef struct ifcopenshell_int32_list_list_list_list_t {" in header
    assert "void ifcopenshell_int32_list_list_list_list_destroy" in header
    assert "static std::vector<std::vector<std::vector<std::vector<int>>>> to_cpp_int32_list_list_list_list" in cpp
    assert "options_cpp.faces = to_cpp_int32_list_list_list_list(options->faces);" in cpp


def test_cpp_spec_generation_lowers_option_parameters(tmp_path: Path) -> None:
    spec_path = tmp_path / "demo_spec.cpp"
    spec_path.write_text(
        dedent(
            """
            #include <optional>
            #include <string>

            namespace demo {
            struct CreateEntityOptions {
                std::string ifc_class;
                std::optional<std::string> name;
                std::optional<bool> force;
            };

            inline int root_create_entity(const CreateEntityOptions& options) {
                return options.ifc_class.empty() ? 0 : 1;
            }
            }
            """
        ),
        encoding="utf-8",
    )
    header_out = tmp_path / "demo_api.h"
    cpp_out = tmp_path / "demo_api.cpp"

    generate_cpp_specs(
        [spec_path],
        ["demo"],
        "demo",
        "ifcopenshell_demo",
        header_out,
        cpp_out,
        discovery_include_dirs=(tmp_path,),
    )

    header = header_out.read_text(encoding="utf-8")
    generated_cpp = cpp_out.read_text(encoding="utf-8")
    assert "typedef struct ifcopenshell_demo_create_entity_options_t {" in header
    assert "    const char* ifc_class;" in header
    assert "    const char* name;" in header
    assert "    bool has_name;" in header
    assert "    bool force;" in header
    assert "    bool has_force;" in header
    assert (
        "bool ifcopenshell_demo_root_create_entity("
        "const ifcopenshell_demo_create_entity_options_t* options, int32_t* out_result);"
    ) in header
    assert "demo::CreateEntityOptions options_cpp{};" in generated_cpp
    assert 'Options field \\"ifc_class\\" must not be null' in generated_cpp
    assert "options_cpp.ifc_class = std::string(options->ifc_class);" in generated_cpp
    assert "if (options->has_name) {" in generated_cpp
    assert "options_cpp.name = std::string(options->name);" in generated_cpp
    assert "if (options->has_force) {" in generated_cpp
    assert "options_cpp.force = options->force;" in generated_cpp
    assert "static_cast<std::optional<bool>>" not in generated_cpp
    assert "demo::root_create_entity(options_cpp)" in generated_cpp


def test_cpp_spec_generation_lowers_std_optional_option_fields(tmp_path: Path) -> None:
    spec_path = tmp_path / "demo_spec.cpp"
    spec_path.write_text(
        dedent(
            """
            #include <optional>

            namespace demo {
            struct UpdateOptions {
                std::optional<bool> force;
            };

            inline bool update(const UpdateOptions& options) {
                return true;
            }
            }
            """
        ),
        encoding="utf-8",
    )
    header_out = tmp_path / "demo_api.h"
    cpp_out = tmp_path / "demo_api.cpp"

    generate_cpp_specs(
        [spec_path],
        ["demo"],
        "demo",
        "ifcopenshell_demo",
        header_out,
        cpp_out,
        discovery_include_dirs=(tmp_path,),
    )

    header = header_out.read_text(encoding="utf-8")
    generated_cpp = cpp_out.read_text(encoding="utf-8")
    assert "bool force;" in header
    assert "bool has_force;" in header
    assert "options_cpp.force = options->force;" in generated_cpp
    assert "static_cast<std::optional<bool>>" not in generated_cpp


def test_cpp_spec_generation_lowers_standalone_optional_handle_and_string_params(
    tmp_path: Path,
) -> None:
    spec_path = tmp_path / "demo_spec.cpp"
    spec_path.write_text(
        dedent(
            """
            #include <optional>
            #include <string>

            struct DemoValue {};
            #define IFCAPI_HANDLE(name, cpp_type, destructor)
            IFCAPI_HANDLE(demo_value, DemoValue, none)

            namespace demo {
            inline int update(std::optional<DemoValue> value, std::optional<std::string> name) {
                return value.has_value() || name.has_value() ? 1 : 0;
            }
            }
            """
        ),
        encoding="utf-8",
    )
    header_out = tmp_path / "demo_api.h"
    cpp_out = tmp_path / "demo_api.cpp"

    generate_cpp_specs(
        [spec_path],
        ["demo"],
        "demo",
        "ifcopenshell_demo",
        header_out,
        cpp_out,
        discovery_include_dirs=(tmp_path,),
    )

    header = header_out.read_text(encoding="utf-8")
    generated_cpp = cpp_out.read_text(encoding="utf-8")
    assert (
        "bool ifcopenshell_demo_update(ifcopenshell_demo_demo_value_t* value, const char* name, int32_t* out_result);"
        in header
    )
    assert "std::optional<DemoValue> value_cpp;" in generated_cpp
    assert "if (value != nullptr && value->ptr != nullptr) { value_cpp = *value->ptr; }" in generated_cpp
    assert "std::optional<std::string> name_cpp;" in generated_cpp
    assert "if (name != nullptr) { name_cpp = std::string(name); }" in generated_cpp
    assert "demo::update(value_cpp, name_cpp)" in generated_cpp


def test_cpp_spec_generation_tracks_default_parameters(tmp_path: Path) -> None:
    spec_path = tmp_path / "demo_spec.cpp"
    spec_path.write_text(
        dedent(
            """
            #include <optional>
            #include <string>

            namespace demo {
            inline bool update(std::optional<std::string> value, std::optional<std::string> suffix = std::nullopt) {
                return value.has_value() || suffix.has_value();
            }
            }
            """
        ),
        encoding="utf-8",
    )

    functions = discover_cpp_spec_functions(
        _environment(tmp_path),
        spec_path,
        "demo",
    )
    calls = lower_cpp_spec_functions_to_calls(functions, {}, c_prefix="ifcopenshell_demo")
    ir = BindingIR(
        module="demo",
        c_prefix="ifcopenshell_demo",
        public_headers=(),
        handles={},
        result_structs={},
        functions=tuple(
            CallIR(
                expose_as=call.expose_as,
                c_name=call.c_name,
                receiver=call.receiver,
                returns=call.returns,
                params=call.params,
                operation=DirectCallOp(cpp_name="demo::update"),
                doc=call.doc,
            )
            for call in calls
        ),
        methods=(),
    )
    metadata = build_host_metadata(ir)
    function = metadata.functions["ifcopenshell_demo_update"]
    params = {param.name: param for param in function.params}

    assert params["value"].nullable is True
    assert params["value"].has_default is False
    assert params["suffix"].nullable is True
    assert params["suffix"].has_default is True


def test_cpp_spec_generation_lowers_optional_owned_handle_returns(
    tmp_path: Path,
) -> None:
    spec_path = tmp_path / "demo_spec.cpp"
    spec_path.write_text(
        dedent(
            """
            #include <optional>

            struct DemoValue {};
            #define IFCAPI_HANDLE(name, cpp_type, destructor)
            #define IFCAPI_OWNED
            IFCAPI_HANDLE(demo_value, DemoValue, delete)

            namespace demo {
            inline IFCAPI_OWNED std::optional<DemoValue*> find_value(bool found) {
                return found ? std::optional<DemoValue*>(new DemoValue()) : std::nullopt;
            }
            }
            """
        ),
        encoding="utf-8",
    )
    header_out = tmp_path / "demo_api.h"
    cpp_out = tmp_path / "demo_api.cpp"

    generate_cpp_specs(
        [spec_path],
        ["demo"],
        "demo",
        "ifcopenshell_demo",
        header_out,
        cpp_out,
        discovery_include_dirs=(tmp_path,),
    )

    header = header_out.read_text(encoding="utf-8")
    generated_cpp = cpp_out.read_text(encoding="utf-8")
    assert "bool ifcopenshell_demo_find_value(bool found, ifcopenshell_demo_demo_value_t** out_result);" in header
    assert "auto result_value = demo::find_value(found_cpp);" in generated_cpp
    assert "if (!result_value) {" in generated_cpp
    assert "*out_result = nullptr;" in generated_cpp
    assert "auto unwrapped_result = *result_value;" in generated_cpp
    assert "*out_result = new ifcopenshell_demo_demo_value_t{unwrapped_result, true};" in generated_cpp


def test_cpp_spec_generation_lowers_optional_opaque_pointer_params(
    tmp_path: Path,
) -> None:
    spec_path = tmp_path / "demo_spec.cpp"
    spec_path.write_text(
        dedent(
            """
            #include <optional>

            struct DemoOpaque;

            namespace demo {
            inline int count(std::optional<const DemoOpaque*> value) {
                return value.has_value() ? 1 : 0;
            }
            }
            """
        ),
        encoding="utf-8",
    )
    header_out = tmp_path / "demo_api.h"
    cpp_out = tmp_path / "demo_api.cpp"

    generate_cpp_specs(
        [spec_path],
        ["demo"],
        "demo",
        "ifcopenshell_demo",
        header_out,
        cpp_out,
        discovery_include_dirs=(tmp_path,),
    )

    header = header_out.read_text(encoding="utf-8")
    generated_cpp = cpp_out.read_text(encoding="utf-8")
    assert "bool ifcopenshell_demo_count(void* value, int32_t* out_result);" in header
    assert "std::optional<DemoOpaque*> value_cpp;" in generated_cpp
    assert "if (value != nullptr) { value_cpp = static_cast<DemoOpaque*>(value); }" in generated_cpp
    assert "demo::count(value_cpp)" in generated_cpp


def test_cpp_spec_frontend_cli_generates_fixture_header(tmp_path: Path) -> None:
    spec_path = tmp_path / "demo_spec.cpp"
    spec_path.write_text(
        dedent(
            """
            #include <string>

            #define IFCAPI_HANDLE(cpp_type, destructor)
            #define IFCAPI_OWNED

            namespace ifcopenshell::capi_spec {
            struct DemoFile {};
            IFCAPI_HANDLE(ifcopenshell::capi_spec::DemoFile, delete)
            struct ifcopenshell_demo_file_t;

            /**
             * Return a display label for the file.
             */
            inline IFCAPI_OWNED std::string ifcopenshell_demo_label(const DemoFile* file) {
                return file ? "ok" : "empty";
            }
            }
            """
        ),
        encoding="utf-8",
    )
    header_out = tmp_path / "demo_api.h"
    cpp_out = tmp_path / "demo_api.cpp"
    backend = Path(__file__).resolve().parents[1] / "binding_generator" / "c_backend.py"

    result = subprocess.run(
        [
            sys.executable,
            str(backend),
            "--cpp-spec",
            str(spec_path),
            "--cpp-spec-namespace",
            "ifcopenshell::capi_spec",
            "--header-out",
            str(header_out),
            "--cpp-out",
            str(cpp_out),
            "--discovery-include-dir",
            str(tmp_path),
            "--module",
            "demo",
            "--c-prefix",
            "ifcopenshell_demo",
        ],
        check=False,
        capture_output=True,
        text=True,
    )
    assert result.returncode == 0, result.stderr
    header = header_out.read_text(encoding="utf-8")
    assert "typedef struct ifcopenshell_demo_file_t ifcopenshell_demo_file_t;" in header
    assert (
        "/** Return a display label for the file. */\n"
        "bool ifcopenshell_demo_label(ifcopenshell_demo_file_t* file, "
        "ifcopenshell_string_t* out_result);"
    ) in header


def test_cpp_spec_frontend_rejects_overloaded_exports(tmp_path: Path) -> None:
    spec_path = tmp_path / "demo_spec.cpp"
    spec_path.write_text(
        dedent(
            """
            namespace ifcopenshell::capi_spec {
            inline int ifcopenshell_demo_value(int value) {
                return value;
            }
            inline int ifcopenshell_demo_value(double value) {
                return static_cast<int>(value);
            }
            }
            """
        ),
        encoding="utf-8",
    )

    with pytest.raises(ValueError, match="declared more than once"):
        discover_cpp_spec_functions(
            _environment(tmp_path),
            spec_path,
            "ifcopenshell::capi_spec",
        )


def test_cpp_spec_frontend_excludes_private_helpers(tmp_path: Path) -> None:
    from unittest.mock import patch

    spec_path = tmp_path / "demo_spec.cpp"
    spec_path.write_text(
        dedent(
            """
            namespace ifcopenshell::capi_spec {
            inline int ifcopenshell_demo_exported() {
                return 42;
            }
            inline int ifcopenshell_demo_internal_helper() {
                return 0;
            }
            }
            """
        ),
        encoding="utf-8",
    )

    with patch(
        "src.ifcwrap.binding_generator.cpp_spec_frontend._PRIVATE_NAMES",
        frozenset({"to_base_vector", "ifcopenshell_demo_internal_helper"}),
    ):
        functions = discover_cpp_spec_functions(
            _environment(tmp_path),
            spec_path,
            "ifcopenshell::capi_spec",
        )

    exported_names = {f.name for f in functions}
    assert "ifcopenshell_demo_exported" in exported_names
    assert "ifcopenshell_demo_internal_helper" not in exported_names


def test_cpp_spec_frontend_rejects_duplicate_handles(tmp_path: Path) -> None:
    spec_path = tmp_path / "demo_spec.cpp"
    spec_path.write_text(
        dedent(
            """
            #define IFCAPI_HANDLE(cpp_type, destructor)

            namespace ifcopenshell::capi_spec {
            struct DemoFile {};
            IFCAPI_HANDLE(ifcopenshell::capi_spec::DemoFile, delete)
            struct ifcopenshell_demo_file_t;
            IFCAPI_HANDLE(ifcopenshell::capi_spec::DemoFile, delete)
            struct ifcopenshell_demo_file_t;
            }
            """
        ),
        encoding="utf-8",
    )

    with pytest.raises(ValueError, match="declared more than once"):
        discover_cpp_spec_handles(spec_path)
