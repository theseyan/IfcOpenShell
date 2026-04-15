# This file was generated with the assistance of an AI coding tool.

from __future__ import annotations

import os
from pathlib import Path

import pytest


def repo_root() -> Path:
    return Path(__file__).resolve().parents[3]


def find_repo_compile_commands() -> Path | None:
    candidates: list[Path] = []

    env_path = os.environ.get("IFCOPENSHELL_CAPI_COMPILE_COMMANDS")
    if env_path:
        candidates.append(Path(env_path))

    root = repo_root()
    candidates.extend(
        [
            root / "build-capi-stable" / "compile_commands.json",
            root / "build" / "compile_commands.json",
            Path("/tmp/ifcopenshell-capi-build/compile_commands.json"),
        ]
    )

    for candidate in candidates:
        if candidate.exists():
            return candidate
    return None


def require_repo_compile_commands(reason: str) -> Path:
    path = find_repo_compile_commands()
    if path is None:
        pytest.skip(reason)
    return path
