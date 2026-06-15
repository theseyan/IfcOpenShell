# This file was generated with the assistance of an AI coding tool.

from __future__ import annotations

from abc import ABC, abstractmethod
from dataclasses import dataclass
from importlib import import_module
from inspect import isclass
from pathlib import Path
from typing import Any, ClassVar

try:
    from ..binding_ir import BindingIR
    from ..host_metadata import HostBindingMetadata
except ImportError:  # pragma: no cover - script execution fallback
    from binding_ir import BindingIR
    from host_metadata import HostBindingMetadata


@dataclass(frozen=True)
class TargetGenerationRequest:
    ir: BindingIR
    metadata: HostBindingMetadata
    api_header_path: Path | None
    options: dict[str, Any]
    output_dir: Path
    exports_only: bool = False


@dataclass(frozen=True)
class GeneratedArtifact:
    kind: str
    path: Path


@dataclass(frozen=True)
class GeneratedTargetArtifacts:
    artifacts: tuple[GeneratedArtifact, ...]


class TargetBackend(ABC):
    name: ClassVar[str]

    @abstractmethod
    def generate(self, request: TargetGenerationRequest) -> GeneratedTargetArtifacts:
        raise NotImplementedError

    @classmethod
    def target_name(cls) -> str:
        return cls.name


_TARGET_MODULES = ("python", "wasm")


def discover_targets() -> tuple[TargetBackend, ...]:
    backends: list[TargetBackend] = []
    for module_name in _TARGET_MODULES:
        module = import_module(f"{__package__}.{module_name}")
        for value in module.__dict__.values():
            if not isclass(value) or value is TargetBackend or not issubclass(value, TargetBackend):
                continue
            if not getattr(value, "name", None):
                continue
            backends.append(value())
    return tuple(sorted(backends, key=lambda backend: backend.target_name()))


def get_target(name: str) -> TargetBackend:
    for backend in discover_targets():
        if backend.target_name() == name:
            return backend
    raise ValueError(f"Unknown target backend: {name}")
