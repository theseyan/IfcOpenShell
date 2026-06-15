# This file was generated with the assistance of an AI coding tool.

from __future__ import annotations

from pathlib import Path

try:
    from ...host_metadata import HostBindingMetadata
    from .._base import GeneratedArtifact, GeneratedTargetArtifacts, TargetGenerationRequest
    from .js_glue import render_js_glue
    from .typescript import render_typescript_declarations
except ImportError:  # pragma: no cover - script execution fallback
    from host_metadata import HostBindingMetadata
    from targets._base import GeneratedArtifact, GeneratedTargetArtifacts, TargetGenerationRequest
    from targets.wasm.js_glue import render_js_glue
    from targets.wasm.typescript import render_typescript_declarations


def render_wasm_bindings(metadata: HostBindingMetadata, api_header_path: Path | None = None) -> tuple[str, str]:
    del api_header_path
    return render_js_glue(metadata, metadata.handles), render_typescript_declarations(metadata, metadata.handles)


def render_export_list(metadata: HostBindingMetadata) -> str:
    exports = {
        "_malloc",
        "_free",
        f"_{metadata.error_functions['clear_error']}",
        f"_{metadata.error_functions['last_error_message']}",
        f"_{metadata.error_functions['last_error_kind']}",
    }
    exports.update(f"_{function.c_name}" for function in metadata.functions.values())
    exports.update(f"_{handle.destroy_function}" for handle in metadata.handles.values() if handle.destroy_function)
    exports.update(f"_{value.destroy_function}" for value in metadata.value_types.values() if value.destroy_function)
    return "\n".join(sorted(exports)) + "\n"


def generate(request: TargetGenerationRequest) -> GeneratedTargetArtifacts:
    output_dir = request.output_dir.resolve()
    output_dir.mkdir(parents=True, exist_ok=True)

    export_list = render_export_list(request.metadata)

    exports_path = output_dir / "ifcopenshell_exports.txt"
    exports_path.write_text(export_list, encoding="utf-8")

    if request.exports_only:
        return GeneratedTargetArtifacts(artifacts=(GeneratedArtifact(kind="exports", path=exports_path),))

    javascript, declarations = render_wasm_bindings(request.metadata, api_header_path=request.api_header_path)

    js_path = output_dir / "ifcopenshell_api.js"
    js_path.write_text(javascript, encoding="utf-8")

    dts_path = output_dir / "ifcopenshell_api.d.ts"
    dts_path.write_text(declarations, encoding="utf-8")

    return GeneratedTargetArtifacts(
        artifacts=(
            GeneratedArtifact(kind="javascript", path=js_path),
            GeneratedArtifact(kind="typescript", path=dts_path),
            GeneratedArtifact(kind="exports", path=exports_path),
        )
    )


__all__ = ["generate", "render_export_list", "render_wasm_bindings"]
