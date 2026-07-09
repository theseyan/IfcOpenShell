# This file was generated with the assistance of an AI coding tool.

from __future__ import annotations

from pathlib import Path
import re

try:
    from ...host_metadata import HostBindingMetadata
    from .._base import GeneratedArtifact, GeneratedTargetArtifacts, TargetGenerationRequest
    from .api_bridge import render_api_direct
    from .js_glue import render_js_glue
    from .typescript import render_typescript_declarations
except ImportError:  # pragma: no cover - script execution fallback
    from host_metadata import HostBindingMetadata
    from targets._base import GeneratedArtifact, GeneratedTargetArtifacts, TargetGenerationRequest
    from targets.wasm.api_bridge import render_api_direct
    from targets.wasm.js_glue import render_js_glue
    from targets.wasm.typescript import render_typescript_declarations


_C_FUNCTION_DECL_RE = re.compile(
    r"^(?!typedef\b)[^();{}]*?\b([A-Za-z_][A-Za-z0-9_]*)\s*\([^;{}]*\)\s*;",
    re.MULTILINE,
)


def _declared_c_functions(api_header_path: Path | None) -> set[str] | None:
    if api_header_path is None or not api_header_path.exists():
        return None
    return set(_C_FUNCTION_DECL_RE.findall(api_header_path.read_text(encoding="utf-8")))


def _required_c_functions(metadata: HostBindingMetadata) -> set[str]:
    required = set(metadata.functions)
    required.update(metadata.error_functions.values())
    required.update(handle.destroy_function for handle in metadata.handles.values() if handle.destroy_function)
    required.update(value.destroy_function for value in metadata.value_types.values() if value.destroy_function)
    return required


def _validate_metadata_matches_declared_functions(metadata: HostBindingMetadata, api_header_path: Path | None) -> None:
    if api_header_path is None:
        return
    if not api_header_path.exists():
        msg = f"WASM target requires generated C API header at '{api_header_path}'"
        raise FileNotFoundError(msg)
    declared = _declared_c_functions(api_header_path)
    if not declared:
        msg = f"WASM target could not find any C function declarations in '{api_header_path}'"
        raise ValueError(msg)
    missing = sorted(_required_c_functions(metadata) - declared)
    if missing:
        missing_list = ", ".join(missing)
        msg = (
            "WASM target metadata does not match the generated C ABI. "
            f"Missing declarations in '{api_header_path}': {missing_list}"
        )
        raise ValueError(msg)


def render_wasm_bindings(metadata: HostBindingMetadata, api_header_path: Path | None = None) -> tuple[str, str]:
    _validate_metadata_matches_declared_functions(metadata, api_header_path)
    return render_js_glue(metadata, metadata.handles), render_typescript_declarations(metadata, metadata.handles)


def render_export_list(metadata: HostBindingMetadata, api_header_path: Path | None = None) -> str:
    _validate_metadata_matches_declared_functions(metadata, api_header_path)
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

    export_list = render_export_list(request.metadata, request.api_header_path)

    exports_path = output_dir / "ifcopenshell_exports.txt"
    exports_path.write_text(export_list, encoding="utf-8")

    if request.exports_only:
        return GeneratedTargetArtifacts(artifacts=(GeneratedArtifact(kind="exports", path=exports_path),))

    javascript, declarations = render_wasm_bindings(request.metadata, api_header_path=request.api_header_path)

    js_path = output_dir / "ifcopenshell_api.js"
    js_path.write_text(javascript, encoding="utf-8")

    dts_path = output_dir / "ifcopenshell_api.d.ts"
    dts_path.write_text(declarations, encoding="utf-8")

    api_path = output_dir / "ifcopenshell_api.ts"
    api_path.write_text(render_api_direct(request.metadata), encoding="utf-8")

    return GeneratedTargetArtifacts(
        artifacts=(
            GeneratedArtifact(kind="javascript", path=js_path),
            GeneratedArtifact(kind="typescript", path=dts_path),
            GeneratedArtifact(kind="api", path=api_path),
            GeneratedArtifact(kind="exports", path=exports_path),
        )
    )


__all__ = ["generate", "render_export_list", "render_wasm_bindings"]
