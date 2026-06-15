# This file was generated with the assistance of an AI coding tool.

from __future__ import annotations

try:
    from .._base import GeneratedTargetArtifacts, TargetBackend, TargetGenerationRequest
    from .backend import generate
except ImportError:  # pragma: no cover - script execution fallback
    from targets._base import GeneratedTargetArtifacts, TargetBackend, TargetGenerationRequest
    from targets.wasm.backend import generate


class WasmTargetBackend(TargetBackend):
    name = "wasm"

    def generate(self, request: TargetGenerationRequest) -> GeneratedTargetArtifacts:
        return generate(request)


__all__ = ["WasmTargetBackend"]
