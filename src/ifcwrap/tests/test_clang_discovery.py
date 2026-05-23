from __future__ import annotations

import json
from pathlib import Path
import shutil

import pytest

from src.ifcwrap.binding_generator.clang_discovery import (
    CompileCommand,
    TranslationUnitIndex,
    _ast_filter_for_lookup,
    discover_namespace_functions_with_compile_commands,
    discover_namespace_functions_with_synthetic_source,
    discover_public_fields_with_compile_commands,
    discover_public_methods_with_compile_commands,
)


def test_discover_public_methods_with_compile_commands(tmp_path: Path) -> None:
    compiler = shutil.which("clang++")
    if compiler is None:
        pytest.skip("clang++ is not available")

    header = tmp_path / "sample.h"
    source = tmp_path / "sample.cpp"
    compile_commands = tmp_path / "compile_commands.json"

    header.write_text(
        """
#include <string>

namespace Demo {
class Foo {
public:
    int bar(int id);
    const std::string& baz(const std::string& guid) const;
    void qux(int id);
    void qux(const std::string& guid);
private:
    void hidden();
};

int walk(int steps);
void hop(int count);
void hop(const std::string& guid);
}
""".strip()
        + "\n",
        encoding="utf-8",
    )
    source.write_text('#include "sample.h"\n', encoding="utf-8")
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

    methods = discover_public_methods_with_compile_commands(compile_commands, source, "Foo")

    assert set(methods) == {"bar", "baz", "qux"}
    assert len(methods["bar"]) == 1
    assert methods["bar"][0].return_cpp_type == "int"
    assert methods["bar"][0].params[0].name == "id"
    assert methods["bar"][0].params[0].cpp_type == "int"
    assert len(methods["baz"]) == 1
    assert methods["baz"][0].return_cpp_type == "const std::string &"
    assert methods["baz"][0].return_type_ref.is_const
    assert methods["baz"][0].return_type_ref.is_lvalue_reference
    assert methods["baz"][0].return_type_ref.base_name == "std::string"
    assert methods["baz"][0].params[0].name == "guid"
    assert methods["baz"][0].params[0].cpp_type == "const std::string &"
    assert methods["baz"][0].params[0].cpp_type_ref.base_name == "std::string"
    assert len(methods["qux"]) == 2
    assert [param.cpp_type for param in methods["qux"][0].params] == ["int"]
    assert [param.cpp_type for param in methods["qux"][1].params] == ["const std::string &"]

    qualified_methods = discover_public_methods_with_compile_commands(compile_commands, source, "Demo::Foo")
    assert set(qualified_methods) == {"bar", "baz", "qux"}

    functions = discover_namespace_functions_with_compile_commands(compile_commands, source, "Demo")
    assert set(functions) == {"walk", "hop"}
    assert len(functions["walk"]) == 1
    assert functions["walk"][0].return_cpp_type == "int"
    assert functions["walk"][0].params[0].cpp_type == "int"
    assert len(functions["hop"]) == 2


def test_discover_namespace_functions_with_nested_qualified_namespace(tmp_path: Path) -> None:
    compiler = shutil.which("clang++")
    if compiler is None:
        pytest.skip("clang++ is not available")

    header = tmp_path / "bindings.h"
    source = tmp_path / "bindings.cpp"
    compile_commands = tmp_path / "compile_commands.json"

    header.write_text(
        """
#include <string>

namespace ifcapi {
namespace bindings {
int nested_count(const std::string& name);
}
}
""".strip()
        + "\n",
        encoding="utf-8",
    )
    source.write_text(
        """
#include "bindings.h"

namespace ifcapi::bindings {
double qualified_scale(double value) { return value; }
}
""".strip()
        + "\n",
        encoding="utf-8",
    )
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

    functions = discover_namespace_functions_with_compile_commands(compile_commands, source, "ifcapi::bindings")

    assert set(functions) == {"nested_count", "qualified_scale"}
    assert functions["nested_count"][0].return_cpp_type == "int"
    assert functions["nested_count"][0].params[0].cpp_type == "const std::string &"
    assert functions["qualified_scale"][0].return_cpp_type == "double"


def test_discover_namespace_functions_with_synthetic_contract_source(tmp_path: Path) -> None:
    compiler = shutil.which("clang++")
    if compiler is None:
        pytest.skip("clang++ is not available")

    header_a = tmp_path / "contract_a.h"
    header_b = tmp_path / "contract_b.h"
    source = tmp_path / "reference.cpp"
    compile_commands = tmp_path / "compile_commands.json"

    header_a.write_text(
        """
#include <string>

namespace ifcapi::bindings {
int contract_count(const std::string& name);
}
""".strip()
        + "\n",
        encoding="utf-8",
    )
    header_b.write_text(
        """
namespace ifcapi::bindings {
double contract_scale(double value);
}
""".strip()
        + "\n",
        encoding="utf-8",
    )
    source.write_text("int reference() { return 0; }\n", encoding="utf-8")
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

    functions = discover_namespace_functions_with_synthetic_source(
        compile_commands,
        f'#include "{header_a.as_posix()}"\n#include "{header_b.as_posix()}"\n',
        "ifcapi::bindings",
        selected_names={"contract_count", "contract_scale"},
        reference_source_root=tmp_path,
    )

    assert set(functions) == {"contract_count", "contract_scale"}
    assert functions["contract_count"][0].params[0].cpp_type == "const std::string &"
    assert functions["contract_scale"][0].return_cpp_type == "double"


def test_namespace_discovery_uses_simple_fallback_lazily(tmp_path: Path) -> None:
    index = TranslationUnitIndex(
        CompileCommand(directory=tmp_path, file=tmp_path / "bindings.cpp", arguments=("clang++", "-c", "bindings.cpp"))
    )
    ast_filters: list[str] = []

    def fake_ast_dump(ast_filter: str) -> tuple[dict, ...]:
        ast_filters.append(ast_filter)
        if ast_filter == "ifcapi::bindings":
            return (
                {
                    "kind": "NamespaceDecl",
                    "name": "ifcapi",
                    "inner": [
                        {
                            "kind": "NamespaceDecl",
                            "name": "bindings",
                            "inner": [
                                {
                                    "kind": "FunctionDecl",
                                    "name": "count",
                                    "type": {"qualType": "int ()"},
                                }
                            ],
                        }
                    ],
                },
            )
        raise AssertionError(f"Unexpected fallback AST filter: {ast_filter}")

    index._run_ast_dump = fake_ast_dump  # type: ignore[method-assign]

    functions = index.discover_namespace_functions("ifcapi::bindings")

    assert set(functions) == {"count"}
    assert ast_filters == ["ifcapi::bindings"]


def test_record_lookup_misses_are_cached(tmp_path: Path) -> None:
    index = TranslationUnitIndex(
        CompileCommand(directory=tmp_path, file=tmp_path / "bindings.cpp", arguments=("clang++", "-c", "bindings.cpp"))
    )
    ast_filters: list[str] = []

    def fake_ast_dump(ast_filter: str) -> tuple[dict, ...]:
        ast_filters.append(ast_filter)
        return ()

    index._run_ast_dump = fake_ast_dump  # type: ignore[method-assign]

    assert index.resolve_record("MissingType") is None
    assert index.resolve_record("MissingType") is None
    assert ast_filters == ["MissingType"]


def test_ast_objects_are_cached_per_filter(tmp_path: Path) -> None:
    index = TranslationUnitIndex(
        CompileCommand(directory=tmp_path, file=tmp_path / "bindings.cpp", arguments=("clang++", "-c", "bindings.cpp"))
    )
    ast_filters: list[str] = []

    def fake_ast_dump(ast_filter: str) -> tuple[dict, ...]:
        ast_filters.append(ast_filter)
        return ()

    index._run_ast_dump = fake_ast_dump  # type: ignore[method-assign]

    assert index.ast_objects("Demo") == ()
    assert index.ast_objects("Demo") == ()
    index.ensure_ast_filter_loaded("Demo")

    assert ast_filters == ["Demo"]


def test_namespace_discovery_skips_unselected_signatures(tmp_path: Path) -> None:
    index = TranslationUnitIndex(
        CompileCommand(directory=tmp_path, file=tmp_path / "bindings.cpp", arguments=("clang++", "-c", "bindings.cpp"))
    )
    ast_filters: list[str] = []

    def fake_ast_dump(ast_filter: str) -> tuple[dict, ...]:
        ast_filters.append(ast_filter)
        if ast_filter == "MissingType":
            raise AssertionError("Unselected function signature should not be resolved")
        return (
            {
                "kind": "NamespaceDecl",
                "name": "Demo",
                "inner": [
                    {
                        "kind": "FunctionDecl",
                        "name": "wanted",
                        "type": {"qualType": "int ()"},
                    },
                    {
                        "kind": "FunctionDecl",
                        "name": "skipped",
                        "type": {"qualType": "MissingType ()"},
                    },
                ],
            },
        )

    index._run_ast_dump = fake_ast_dump  # type: ignore[method-assign]

    functions = index.discover_namespace_functions("Demo", selected_names={"wanted"})

    assert set(functions) == {"wanted"}
    assert ast_filters == ["Demo"]


def test_qualified_record_lookup_uses_coarse_namespace_filter(tmp_path: Path) -> None:
    index = TranslationUnitIndex(
        CompileCommand(directory=tmp_path, file=tmp_path / "bindings.cpp", arguments=("clang++", "-c", "bindings.cpp"))
    )
    ast_filters: list[str] = []

    def fake_ast_dump(ast_filter: str) -> tuple[dict, ...]:
        ast_filters.append(ast_filter)
        assert ast_filter == "Demo"
        return (
            {
                "kind": "NamespaceDecl",
                "name": "Demo",
                "inner": [
                    {
                        "kind": "CXXRecordDecl",
                        "name": "Widget",
                        "completeDefinition": True,
                    }
                ],
            },
        )

    index._run_ast_dump = fake_ast_dump  # type: ignore[method-assign]

    assert index.resolve_record("Demo::Widget").qualified_name == "Demo::Widget"
    assert ast_filters == ["Demo"]


def test_scoped_lookup_prefers_longest_matching_suffix(tmp_path: Path) -> None:
    index = TranslationUnitIndex(
        CompileCommand(directory=tmp_path, file=tmp_path / "bindings.cpp", arguments=("clang++", "-c", "bindings.cpp"))
    )
    ast_filters: list[str] = []

    def fake_ast_dump(ast_filter: str) -> tuple[dict, ...]:
        ast_filters.append(ast_filter)
        assert ast_filter == "ifcopenshell::geometry::taxonomy"
        return (
            {
                "kind": "NamespaceDecl",
                "name": "geometry",
                "inner": [
                    {
                        "kind": "NamespaceDecl",
                        "name": "taxonomy",
                        "inner": [
                            {
                                "kind": "CXXRecordDecl",
                                "name": "item",
                                "completeDefinition": True,
                            }
                        ],
                    },
                ],
            },
            {
                "kind": "NamespaceDecl",
                "name": "taxonomy",
                "inner": [
                    {
                        "kind": "CXXRecordDecl",
                        "name": "item",
                        "completeDefinition": True,
                    }
                ],
            },
        )

    index._run_ast_dump = fake_ast_dump  # type: ignore[method-assign]

    record = index.resolve_record("ifcopenshell::geometry::taxonomy::item")

    assert record.qualified_name == "geometry::taxonomy::item"
    assert ast_filters == ["ifcopenshell::geometry::taxonomy"]


def test_scoped_lookup_rejects_true_ambiguity(tmp_path: Path) -> None:
    index = TranslationUnitIndex(
        CompileCommand(directory=tmp_path, file=tmp_path / "bindings.cpp", arguments=("clang++", "-c", "bindings.cpp"))
    )

    def fake_ast_dump(ast_filter: str) -> tuple[dict, ...]:
        assert ast_filter == "Demo"
        return (
            {
                "kind": "NamespaceDecl",
                "name": "Alpha",
                "inner": [
                    {
                        "kind": "NamespaceDecl",
                        "name": "Demo",
                        "inner": [
                            {
                                "kind": "CXXRecordDecl",
                                "name": "Widget",
                                "completeDefinition": True,
                            }
                        ],
                    },
                ],
            },
            {
                "kind": "NamespaceDecl",
                "name": "Beta",
                "inner": [
                    {
                        "kind": "NamespaceDecl",
                        "name": "Demo",
                        "inner": [
                            {
                                "kind": "CXXRecordDecl",
                                "name": "Widget",
                                "completeDefinition": True,
                            }
                        ],
                    },
                ],
            },
        )

    index._run_ast_dump = fake_ast_dump  # type: ignore[method-assign]

    with pytest.raises(ValueError, match="Ambiguous declaration lookup"):
        index.resolve_record("Demo::Widget")


def test_ast_filter_for_lookup_coarsens_known_qualified_names() -> None:
    assert _ast_filter_for_lookup("IfcGeom::Iterator") == "IfcGeom"
    assert _ast_filter_for_lookup("IfcParse::schema_definition") == "IfcParse"
    assert _ast_filter_for_lookup("ifcopenshell::geometry::taxonomy::item") == "ifcopenshell::geometry::taxonomy"
    assert _ast_filter_for_lookup("ifcopenshell::geometry::Settings") == "ifcopenshell::geometry"
    assert _ast_filter_for_lookup("BareType") == "BareType"


def test_discovery_skips_known_namespace_roots_before_clang_lookup(tmp_path: Path) -> None:
    index = TranslationUnitIndex(
        CompileCommand(directory=tmp_path, file=tmp_path / "bindings.cpp", arguments=("clang++", "-c", "bindings.cpp"))
    )
    ast_filters: list[str] = []

    def fake_ast_dump(ast_filter: str) -> tuple[dict, ...]:
        ast_filters.append(ast_filter)
        if ast_filter.startswith("ifcopenshell"):
            raise AssertionError("Known namespace roots should not trigger record/enum AST filters")
        return (
            {
                "kind": "NamespaceDecl",
                "name": "Demo",
                "inner": [
                    {
                        "kind": "FunctionDecl",
                        "name": "make_item",
                        "type": {"qualType": "ifcopenshell::geometry::taxonomy::item *()"},
                    },
                ],
            },
        )

    index._run_ast_dump = fake_ast_dump  # type: ignore[method-assign]

    functions = index.discover_namespace_functions("Demo", selected_names={"make_item"})

    assert functions["make_item"][0].return_type_ref.storage_spelling == "ifcopenshell::geometry::taxonomy::item*"
    assert ast_filters == ["Demo"]


def test_discover_public_fields_with_inheritance(tmp_path: Path) -> None:
    compiler = shutil.which("clang++")
    if compiler is None:
        pytest.skip("clang++ is not available")

    header = tmp_path / "fields.h"
    source = tmp_path / "fields.cpp"
    compile_commands = tmp_path / "compile_commands.json"

    header.write_text(
        """
#include <memory>

namespace Demo {
struct Node {
    using ptr = std::shared_ptr<Node>;
};

struct Base {
public:
    int inherited;
};

struct Derived : Base {
public:
    Node::ptr axis;
private:
    int hidden;
};
}

namespace Other {
struct Base {
public:
    int wrong;
};
}
""".strip()
        + "\n",
        encoding="utf-8",
    )
    source.write_text('#include "fields.h"\n', encoding="utf-8")
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

    own_fields = discover_public_fields_with_compile_commands(compile_commands, source, "Derived")
    assert set(own_fields) == {"axis"}
    assert own_fields["axis"].cpp_type == "Node::ptr"
    assert own_fields["axis"].cpp_type_ref.desugared_spelling == "std::shared_ptr<Demo::Node>"
    assert own_fields["axis"].cpp_type_ref.base_name == "std::shared_ptr"
    assert own_fields["axis"].cpp_type_ref.template_args[0].base_name == "Demo::Node"

    inherited_fields = discover_public_fields_with_compile_commands(
        compile_commands, source, "Derived", include_inherited=True
    )
    assert set(inherited_fields) == {"axis", "inherited"}
    assert inherited_fields["inherited"].cpp_type == "int"
    assert inherited_fields["inherited"].cpp_type_ref.canonical_spelling == "int"

    qualified_inherited_fields = discover_public_fields_with_compile_commands(
        compile_commands, source, "Demo::Derived", include_inherited=True
    )
    assert set(qualified_inherited_fields) == {"axis", "inherited"}


def test_discover_cpp_types_marks_enums(tmp_path: Path) -> None:
    compiler = shutil.which("clang++")
    if compiler is None:
        pytest.skip("clang++ is not available")

    header = tmp_path / "enums.h"
    source = tmp_path / "enums.cpp"
    compile_commands = tmp_path / "compile_commands.json"

    header.write_text(
        """
namespace Demo {
enum class Mode { A, B };

struct Widget {
    Mode mode() const;
    void set_mode(Mode value);
};
}
""".strip()
        + "\n",
        encoding="utf-8",
    )
    source.write_text('#include "enums.h"\n', encoding="utf-8")
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

    methods = discover_public_methods_with_compile_commands(compile_commands, source, "Demo::Widget")

    assert methods["mode"][0].return_type_ref.is_enum
    assert methods["mode"][0].return_type_ref.base_name == "Mode"
    assert methods["set_mode"][0].params[0].cpp_type_ref.is_enum


def test_discover_cpp_types_marks_typedef_enums(tmp_path: Path) -> None:
    compiler = shutil.which("clang++")
    if compiler is None:
        pytest.skip("clang++ is not available")

    header = tmp_path / "typedef_enums.h"
    source = tmp_path / "typedef_enums.cpp"
    compile_commands = tmp_path / "compile_commands.json"

    header.write_text(
        """
namespace Demo {
struct SimpleType {
    typedef enum {
        integer_type,
        string_type
    } data_type;

    data_type declared_type() const;
};
}
""".strip()
        + "\n",
        encoding="utf-8",
    )
    source.write_text('#include "typedef_enums.h"\n', encoding="utf-8")
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

    methods = discover_public_methods_with_compile_commands(compile_commands, source, "Demo::SimpleType")

    assert methods["declared_type"][0].return_type_ref.is_enum
    assert methods["declared_type"][0].return_type_ref.enum_qualified_name == "Demo::SimpleType::data_type"


def test_discover_cpp_types_marks_enum_fields_under_skipped_root(tmp_path: Path) -> None:
    compiler = shutil.which("clang++")
    if compiler is None:
        pytest.skip("clang++ is not available")

    header = tmp_path / "enum_fields.h"
    source = tmp_path / "enum_fields.cpp"
    compile_commands = tmp_path / "compile_commands.json"

    header.write_text(
        """
namespace ifcopenshell {
namespace demo {
struct Widget {
    enum Mode { A, B };
    Mode mode;
};
}
}
""".strip()
        + "\n",
        encoding="utf-8",
    )
    source.write_text('#include "enum_fields.h"\n', encoding="utf-8")
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

    fields = discover_public_fields_with_compile_commands(compile_commands, source, "ifcopenshell::demo::Widget")

    assert fields["mode"].cpp_type_ref.is_enum
    assert fields["mode"].cpp_type_ref.enum_qualified_name == "ifcopenshell::demo::Widget::Mode"


def test_discovery_avoids_unscoped_and_std_ast_filters(tmp_path: Path, monkeypatch: pytest.MonkeyPatch) -> None:
    compiler = shutil.which("clang++")
    if compiler is None:
        pytest.skip("clang++ is not available")

    header = tmp_path / "scoped.h"
    source = tmp_path / "scoped.cpp"
    compile_commands = tmp_path / "compile_commands.json"

    header.write_text(
        """
#include <string>

namespace Demo {
struct Outer {
    struct Inner {};
};

struct Container {
    Outer::Inner inner() const;
    const std::string& name() const;
};
}
""".strip()
        + "\n",
        encoding="utf-8",
    )
    source.write_text('#include "scoped.h"\n', encoding="utf-8")
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

    seen_filters: list[str] = []
    original = TranslationUnitIndex._run_ast_dump

    def _recording_run_ast_dump(self: TranslationUnitIndex, ast_filter: str):
        seen_filters.append(ast_filter)
        return original(self, ast_filter)

    monkeypatch.setattr(TranslationUnitIndex, "_run_ast_dump", _recording_run_ast_dump)

    methods = discover_public_methods_with_compile_commands(compile_commands, source, "Demo::Container")

    assert methods["inner"][0].return_type_ref.storage_spelling == "Outer::Inner"
    assert methods["name"][0].return_type_ref.storage_spelling == "const std::string&"
    assert "std" not in seen_filters
    assert "Inner" not in seen_filters


def test_discovery_avoids_lowercase_bare_type_filters(tmp_path: Path, monkeypatch: pytest.MonkeyPatch) -> None:
    compiler = shutil.which("clang++")
    if compiler is None:
        pytest.skip("clang++ is not available")

    header = tmp_path / "lowercase.h"
    source = tmp_path / "lowercase.cpp"
    compile_commands = tmp_path / "compile_commands.json"

    header.write_text(
        """
namespace Demo {
struct declaration {};

struct schema_definition {
    declaration declared() const;
};
}
""".strip()
        + "\n",
        encoding="utf-8",
    )
    source.write_text('#include "lowercase.h"\n', encoding="utf-8")
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

    seen_filters: list[str] = []
    original = TranslationUnitIndex._run_ast_dump

    def _recording_run_ast_dump(self: TranslationUnitIndex, ast_filter: str):
        seen_filters.append(ast_filter)
        return original(self, ast_filter)

    monkeypatch.setattr(TranslationUnitIndex, "_run_ast_dump", _recording_run_ast_dump)

    methods = discover_public_methods_with_compile_commands(compile_commands, source, "Demo::schema_definition")

    assert methods["declared"][0].return_type_ref.storage_spelling == "Demo::declaration"
    assert "declaration" not in seen_filters


def test_discovery_avoids_bare_ptr_and_it_alias_filters(tmp_path: Path, monkeypatch: pytest.MonkeyPatch) -> None:
    compiler = shutil.which("clang++")
    if compiler is None:
        pytest.skip("clang++ is not available")

    header = tmp_path / "aliases.h"
    source = tmp_path / "aliases.cpp"
    compile_commands = tmp_path / "compile_commands.json"

    header.write_text(
        """
#include <memory>

namespace Demo {
struct Derived {
public:
    using ptr = std::shared_ptr<Derived>;
    using it = int;

    ptr axis;
    it index() const;
};
}
""".strip()
        + "\n",
        encoding="utf-8",
    )
    source.write_text('#include "aliases.h"\n', encoding="utf-8")
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

    seen_filters: list[str] = []
    original = TranslationUnitIndex._run_ast_dump

    def _recording_run_ast_dump(self: TranslationUnitIndex, ast_filter: str):
        seen_filters.append(ast_filter)
        return original(self, ast_filter)

    monkeypatch.setattr(TranslationUnitIndex, "_run_ast_dump", _recording_run_ast_dump)

    fields = discover_public_fields_with_compile_commands(compile_commands, source, "Demo::Derived")
    methods = discover_public_methods_with_compile_commands(compile_commands, source, "Demo::Derived")

    assert fields["axis"].cpp_type_ref.desugared_spelling == "std::shared_ptr<Demo::Derived>"
    assert methods["index"][0].return_cpp_type == "it"
    assert "ptr" not in seen_filters
    assert "it" not in seen_filters
