# This file was generated with the assistance of an AI coding tool.

from __future__ import annotations

import json
from pathlib import Path
import shutil

import pytest

from src.ifcwrap.binding_generator.clang_discovery import (
    discover_namespace_functions_with_compile_commands,
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
    assert methods["baz"][0].params[0].name == "guid"
    assert methods["baz"][0].params[0].cpp_type == "const std::string &"
    assert len(methods["qux"]) == 2
    assert [param.cpp_type for param in methods["qux"][0].params] == ["int"]
    assert [param.cpp_type for param in methods["qux"][1].params] == ["const std::string &"]

    functions = discover_namespace_functions_with_compile_commands(compile_commands, source, "Demo")
    assert set(functions) == {"walk", "hop"}
    assert len(functions["walk"]) == 1
    assert functions["walk"][0].return_cpp_type == "int"
    assert functions["walk"][0].params[0].cpp_type == "int"
    assert len(functions["hop"]) == 2
