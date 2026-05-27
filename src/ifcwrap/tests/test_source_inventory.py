# This file was generated with the assistance of an AI coding tool.

from __future__ import annotations

from pathlib import Path
import shutil

import pytest

from src.ifcwrap.binding_generator import (
    CompilationConfig,
    DiscoveryEnvironment,
    SourceClassRequest,
    SourceInventoryRequest,
    SourceNamespaceRequest,
    discover_source_inventory,
)
from src.ifcwrap.binding_generator.semantic_types import RecordSemanticType, ScalarSemanticType


def test_source_inventory_discovers_classes_and_namespaces_without_compile_commands(tmp_path: Path) -> None:
    compiler = shutil.which("clang++")
    if compiler is None:
        pytest.skip("clang++ is not available")

    header = tmp_path / "inventory.h"
    source = tmp_path / "inventory.cpp"
    header.write_text(
        """
namespace Demo {
struct Base {
public:
    int inherited() const;
};

struct Widget : Base {
public:
    Widget();
    int value() const;
    int field;
};

int make_count(int value);
}
""".strip()
        + "\n",
        encoding="utf-8",
    )
    source.write_text('#include "inventory.h"\n', encoding="utf-8")
    environment = DiscoveryEnvironment(
        compilation=CompilationConfig(compiler=compiler, include_dirs=(tmp_path,), working_directory=tmp_path)
    )

    inventory = discover_source_inventory(
        SourceInventoryRequest(
            environment=environment,
            classes=(
                SourceClassRequest(
                    cpp_name="Demo::Widget",
                    translation_unit=source,
                    include_inherited_methods=True,
                    selected_methods=("value", "inherited"),
                ),
            ),
            namespaces=(
                SourceNamespaceRequest(
                    namespace="Demo",
                    translation_unit=source,
                    selected_functions=("make_count",),
                ),
            ),
        )
    )

    widget = inventory.classes[("Demo::Widget", source.resolve())]
    assert set(widget.methods) == {"value", "inherited"}
    assert widget.constructors[0].class_name == "Demo::Widget"
    assert set(widget.fields) == {"field"}
    assert widget.bases[0].cpp_type_ref.base_name == "Demo::Base"
    assert set(inventory.namespaces[("Demo", source.resolve())].functions) == {"make_count"}
    assert isinstance(widget.method_semantics["value"][0].return_type.semantic, ScalarSemanticType)
    assert widget.method_semantics["value"][0].return_type.semantic.family == "int32"
    assert widget.constructor_semantics[0].return_type is None
    assert isinstance(widget.field_semantics["field"].semantic, ScalarSemanticType)
    assert widget.field_semantics["field"].ownership_hint == "value"
    assert isinstance(widget.base_semantics[0].semantic, RecordSemanticType)
    make_count = inventory.namespaces[("Demo", source.resolve())].function_semantics["make_count"][0]
    assert isinstance(make_count.return_type.semantic, ScalarSemanticType)
    assert make_count.params[0].type.ownership_hint == "value"


def _boost_include_dir() -> Path | None:
    for path in (
        Path("/opt/homebrew/include"),
        Path("/usr/local/include"),
        Path("/usr/include"),
    ):
        if (path / "boost" / "lexical_cast.hpp").exists():
            return path
    return None


def test_source_inventory_discovers_datamodel_v1_core_types_without_compile_commands() -> None:
    compiler = shutil.which("clang++")
    if compiler is None:
        pytest.skip("clang++ is not available")
    boost_include = _boost_include_dir()
    if boost_include is None:
        pytest.skip("Boost headers are required for datamodel-v1 source inventory smoke test")

    repo_root = Path(__file__).resolve().parents[3]
    ifcparse = repo_root / "src" / "ifcparse"
    environment = DiscoveryEnvironment(
        compilation=CompilationConfig(
            compiler=compiler,
            include_dirs=(ifcparse, ifcparse / "schemas", boost_include),
            working_directory=ifcparse,
        )
    )

    inventory = discover_source_inventory(
        SourceInventoryRequest(
            environment=environment,
            classes=(
                SourceClassRequest(
                    cpp_name="ifcopenshell::file",
                    translation_unit=ifcparse / "file.cpp",
                    selected_methods=("schema", "header"),
                    include_constructors=False,
                    include_fields=False,
                ),
                SourceClassRequest(
                    cpp_name="ifcopenshell::schema_definition",
                    translation_unit=ifcparse / "schema.cpp",
                    selected_methods=("name", "declarations", "entities"),
                    include_constructors=False,
                    include_fields=False,
                ),
                SourceClassRequest(
                    cpp_name="ifcopenshell::instance_streamer<>",
                    translation_unit=ifcparse / "file.cpp",
                    selected_methods=("status", "schema", "header"),
                    include_constructors=False,
                    include_fields=False,
                ),
                SourceClassRequest(
                    cpp_name="express::Base",
                    translation_unit=ifcparse / "parse.cpp",
                    selected_methods=("declaration", "identity", "file"),
                    include_constructors=False,
                    include_fields=False,
                ),
            ),
            namespaces=(
                SourceNamespaceRequest(
                    namespace="ifcopenshell",
                    translation_unit=ifcparse / "schema.cpp",
                    selected_functions=("schema_by_name", "schema_names"),
                ),
            ),
        )
    )

    file_methods = inventory.classes[("ifcopenshell::file", (ifcparse / "file.cpp").resolve())].methods
    assert set(file_methods) >= {"schema", "header"}
    assert set(inventory.classes[("ifcopenshell::schema_definition", (ifcparse / "schema.cpp").resolve())].methods) >= {
        "name",
        "declarations",
        "entities",
    }
    assert set(inventory.classes[("ifcopenshell::instance_streamer<>", (ifcparse / "file.cpp").resolve())].methods) >= {
        "status",
        "schema",
        "header",
    }
    assert set(inventory.classes[("express::Base", (ifcparse / "parse.cpp").resolve())].methods) >= {
        "declaration",
        "identity",
        "file",
    }
    assert set(inventory.namespaces[("ifcopenshell", (ifcparse / "schema.cpp").resolve())].functions) == {
        "schema_by_name",
        "schema_names",
    }
    schema_by_name = inventory.namespaces[("ifcopenshell", (ifcparse / "schema.cpp").resolve())].function_semantics[
        "schema_by_name"
    ][0]
    assert isinstance(schema_by_name.return_type.semantic, RecordSemanticType)
    assert schema_by_name.return_type.ownership_hint == "raw_pointer"
