# This file was generated with the assistance of an AI coding tool.

from __future__ import annotations

import sys
from dataclasses import replace
from pathlib import Path

import pytest

# Ensure the repo root is on sys.path so `from src.ifcwrap...` works
# regardless of the working directory.
_REPO_ROOT = str(Path(__file__).resolve().parents[3])
if _REPO_ROOT not in sys.path:
    sys.path.insert(0, _REPO_ROOT)

from src.ifcwrap.binding_generator.abi_ir import (
    BindingABI,
    CFieldIR,
    CFunctionIR,
    COptionFieldIR,
    COptionIR,
    CParamIR,
    CTypeIR,
    ErrorCatalogEntryIR,
    _finalize_function,
)
from src.ifcwrap.binding_generator.binding_ir import BindingIR, CallIR, DirectCallOp
from src.ifcwrap.binding_generator.binding_model import ParamSpec, TypeSpec
from src.ifcwrap.binding_generator.targets.wasm.api_bridge import render_api_direct
from src.ifcwrap.binding_generator.targets.wasm.backend import (
    render_export_list,
    render_wasm_bindings,
)
from src.ifcwrap.binding_generator.targets.wasm.js_glue import render_js_glue
from src.ifcwrap.binding_generator.targets.wasm.typescript import (
    render_typescript_declarations,
)

_DEFAULT_ERROR_FUNCTIONS = {
    "clear_error": "ifcopenshell_demo_clear_error",
    "last_error_message": "ifcopenshell_demo_last_error_message",
    "last_error_kind": "ifcopenshell_demo_last_error_kind",
    "last_error_code": "ifcopenshell_demo_last_error_code",
}


def _make_ir(
    module: str = "ifcopenshell_wrapper", c_prefix: str = "ifcopenshell_demo"
) -> BindingIR:
    return BindingIR(
        module=module,
        c_prefix=c_prefix,
        public_headers=(),
        handles={},
        result_structs={},
        calls=(),
    )


def _make_handle(c_type: str, destroy_function: str | None = None) -> CTypeIR:
    return CTypeIR(
        c_type=c_type,
        kind="handle",
        fields=(CFieldIR("ptr", "void*"), CFieldIR("owned", "bool")),
        destroy_function=destroy_function
        or f"ifcopenshell_{c_type.removeprefix('ifcopenshell_').removesuffix('_t')}_destroy",
        layout="ptr_owned",
    )


def _make_function(
    *,
    c_name: str,
    params: tuple[CParamIR, ...] = (),
    returns: TypeSpec | None = None,
    receiver: str | None = None,
    doc: str | None = None,
    public_module: str | None = None,
) -> CFunctionIR:
    if returns is None:
        returns = TypeSpec(kind="void")
    full_params = list(params)
    if receiver is not None:
        full_params.insert(
            0,
            CParamIR(
                name="self",
                c_type=f"ifcopenshell_demo_{receiver}_t*",
                role="receiver",
                type_kind="handle",
            ),
        )
    if returns.kind != "void":
        out_type = {
            "bool": "bool*",
            "double": "double*",
            "int32": "int32_t*",
            "uint32": "uint32_t*",
            "size": "size_t*",
            "int64": "int64_t*",
            "string": "ifcopenshell_string_t*",
            "handle": f"ifcopenshell_demo_{returns.handle}_t**",
            "struct": "void*",
            "variant": "void*",
            "double_buffer": "const double**",
            "int32_buffer": "const int32_t**",
        }[returns.kind]
        full_params.append(
            CParamIR(
                name="out_result",
                c_type=out_type,
                role="out_result",
                type_kind=returns.kind,
            )
        )
    return CFunctionIR(
        c_name=c_name,
        restype="bool",
        params=tuple(full_params),
        error_policy="bool_return_last_error",
        returns=returns,
        receiver=receiver,
        doc=doc,
        public_module=public_module,
    )


def _make_metadata(
    *,
    c_prefix: str = "ifcopenshell_demo",
    handles: dict[str, CTypeIR] | None = None,
    value_types: dict[str, CTypeIR] | None = None,
    option_structs: dict[str, COptionIR] | None = None,
    functions: dict[str, CFunctionIR] | None = None,
) -> BindingABI:
    return BindingABI(
        module="ifcopenshell_wrapper",
        c_prefix=c_prefix,
        handles=handles or {},
        value_types=value_types or {},
        option_structs=option_structs or {},
        functions=functions or {},
        error_functions=_DEFAULT_ERROR_FUNCTIONS,
    )


def test_typescript_preserves_fixed_aliases_and_enum_literals() -> None:
    metadata = _make_metadata(
        c_prefix="ifcopenshell",
        option_structs={
            "TransformOptions": COptionIR(
                name="TransformOptions",
                c_type="ifcopenshell_demo_transform_options_t",
                fields=(
                    COptionFieldIR(
                        name="origin",
                        type=TypeSpec(
                            kind="double",
                            sequence_depth=1,
                            alias="Vec3",
                            fixed_lengths=(3,),
                        ),
                        c_type="const ifcopenshell_double_list_t*",
                    ),
                    COptionFieldIR(
                        name="direction",
                        type=TypeSpec(
                            kind="int32",
                            alias="Direction",
                            enum_values=("Positive", "Negative"),
                            enum_numeric_values=(1, -1),
                        ),
                        c_type="int32_t",
                    ),
                ),
            ),
            "PlaneClipping": COptionIR(
                name="PlaneClipping",
                c_type="ifcopenshell_demo_plane_clipping_t",
                fields=(
                    COptionFieldIR(
                        name="location",
                        type=TypeSpec(
                            kind="double",
                            sequence_depth=1,
                            alias="Vec3",
                            fixed_lengths=(3,),
                        ),
                        c_type="const ifcopenshell_double_list_t*",
                    ),
                ),
            ),
            "EntityClipping": COptionIR(
                name="EntityClipping",
                c_type="ifcopenshell_demo_entity_clipping_t",
                fields=(
                    COptionFieldIR(
                        name="entity_id",
                        type=TypeSpec(kind="int32"),
                        c_type="int32_t",
                    ),
                ),
            ),
            "ApplyOptions": COptionIR(
                name="ApplyOptions",
                c_type="ifcopenshell_demo_apply_options_t",
                fields=(
                    COptionFieldIR(
                        name="clipping",
                        type=TypeSpec(
                            kind="variant",
                            variants=(
                                TypeSpec(kind="option", struct="PlaneClipping"),
                                TypeSpec(kind="option", struct="EntityClipping"),
                            ),
                        ),
                        c_type="const ifcopenshell_demo_clipping_variant_t*",
                    ),
                ),
            ),
        },
        functions={
            "ifcopenshell_demo_transform": _make_function(
                c_name="ifcopenshell_demo_transform",
                params=(
                    CParamIR(
                        name="options",
                        c_type="const ifcopenshell_demo_transform_options_t*",
                        role="param",
                        type_kind="option",
                    ),
                ),
                public_module="demo",
            ),
            "ifcopenshell_demo_apply": _make_function(
                c_name="ifcopenshell_demo_apply",
                params=(
                    CParamIR(
                        name="options",
                        c_type="const ifcopenshell_demo_apply_options_t*",
                        role="param",
                        type_kind="option",
                    ),
                ),
                public_module="demo",
            ),
        },
    )

    declarations = render_typescript_declarations(metadata)
    direct = render_api_direct(metadata)

    for code in (declarations, direct):
        assert "export type Vec3 = [number, number, number];" in code
        assert "export type Direction = 'Positive' | 'Negative';" in code
        assert "origin: Vec3;" in code
        assert "direction: Direction;" in code
    assert (
        "clipping: IfcOpenshellDemoPlaneClipping | IfcOpenshellDemoEntityClipping;"
        in declarations
    )
    assert (
        "clipping: IfcOpenShellDemoPlaneClipping | IfcOpenShellDemoEntityClipping;"
        in direct
    )
    assert '"origin": [3]' in direct
    assert "validateFixedLengths(publicName, value, fixedLengths)" in direct
    assert '"required": ["location"]' in direct
    assert '"required": ["entityId"]' in direct
    assert "match exactly one variant alternative" in direct


def test_typescript_and_js_preserve_native_default_omission() -> None:
    function = _make_function(
        c_name="ifcopenshell_demo_update",
        params=(
            CParamIR(
                "tolerance",
                "const double*",
                "param",
                "double",
                nullable=True,
                has_default=True,
            ),
            CParamIR(
                "label",
                "const char*",
                "param",
                "string",
                nullable=True,
                has_default=True,
            ),
        ),
        returns=TypeSpec(kind="bool"),
        public_module="demo",
    )
    metadata = _make_metadata(
        c_prefix="ifcopenshell",
        option_structs={
            "DefaultsOptions": COptionIR(
                "DefaultsOptions",
                "ifcopenshell_demo_defaults_options_t",
                (
                    COptionFieldIR(
                        "unit_scale",
                        TypeSpec(kind="double", nullable=True),
                        "double",
                        presence_field="has_unit_scale",
                        has_default=True,
                    ),
                    COptionFieldIR(
                        "required_zero",
                        TypeSpec(kind="double"),
                        "double",
                    ),
                ),
            )
        },
        functions={function.c_name: function},
    )

    declarations = render_typescript_declarations(metadata)
    direct = render_api_direct(metadata)
    glue = render_js_glue(metadata)

    signature = "update(tolerance?: number | null, label?: string | null): boolean;"
    assert signature in declarations
    assert signature in direct
    assert "type ValueInput," not in direct
    assert (
        "update: (tolerance?: number | null, label?: string | null) => boolean;"
        in direct
    )
    assert "unit_scale?: number;" in declarations
    assert "required_zero: number;" in declarations
    assert "tolerance == null ? 0 : module._malloc(8)" in glue
    assert 'module.setValue(_tolerancePtr, tolerance, "double")' in glue


def test_direct_api_guards_omitted_nullable_option_records() -> None:
    option = COptionIR(
        "DefaultsOptions",
        "ifcopenshell_demo_defaults_options_t",
        (COptionFieldIR("enabled", TypeSpec(kind="bool"), "bool"),),
    )
    function = _make_function(
        c_name="ifcopenshell_demo_update",
        params=(
            CParamIR(
                name="options",
                c_type="const ifcopenshell_demo_defaults_options_t*",
                role="param",
                type_kind="option",
                nullable=True,
                has_default=True,
                type=TypeSpec(kind="option", struct="DefaultsOptions", nullable=True),
            ),
        ),
        returns=TypeSpec(kind="bool"),
        public_module="demo",
    )
    metadata = _make_metadata(
        c_prefix="ifcopenshell",
        option_structs={"DefaultsOptions": option},
        functions={function.c_name: function},
    )

    direct = render_api_direct(metadata)

    assert (
        "update(options?: IfcOpenShellDemoDefaultsOptions | null): boolean;" in direct
    )
    assert "options == null ? null : encodeOptions(options," in direct


def test_direct_api_guards_omitted_nullable_instance_lists() -> None:
    function = _make_function(
        c_name="ifcopenshell_demo_update",
        params=(
            CParamIR(
                name="items",
                c_type="ifcopenshell_parse_instance_list_t*",
                role="param",
                type_kind="handle",
                nullable=True,
                has_default=True,
            ),
        ),
        public_module="demo",
    )
    metadata = _make_metadata(
        c_prefix="ifcopenshell",
        handles={
            "parse_instance_list": CTypeIR(
                c_type="ifcopenshell_parse_instance_list_t",
                kind="handle",
                fields=(),
                destroy_function="ifcopenshell_parse_instance_list_destroy",
            )
        },
        functions={function.c_name: function},
    )

    direct = render_api_direct(metadata)

    assert "update(items?: Entity[] | null): void;" in direct
    assert "items == null ? null : toRaw(items, shell, temps)" in direct


def test_direct_api_preserves_fixed_sequence_variant_contracts() -> None:
    points_type = TypeSpec(
        kind="variant",
        variants=(
            TypeSpec(
                kind="double",
                sequence_depth=2,
                fixed_lengths=(None, 2),
            ),
            TypeSpec(
                kind="double",
                sequence_depth=2,
                fixed_lengths=(None, 3),
            ),
        ),
    )
    function = _make_function(
        c_name="ifcopenshell_demo_transform",
        params=(
            CParamIR(
                "points",
                "const ifcopenshell_demo_points_variant_t*",
                "param",
                "variant",
                type=points_type,
            ),
            CParamIR(
                "origin",
                "const ifcopenshell_double_list_t*",
                "param",
                "sequence",
                type=TypeSpec(
                    kind="double",
                    sequence_depth=1,
                    fixed_lengths=(3,),
                ),
            ),
        ),
        returns=TypeSpec(
            kind="double",
            sequence_depth=1,
            fixed_lengths=(16,),
        ),
        public_module="demo",
    )
    metadata = _make_metadata(
        c_prefix="ifcopenshell",
        value_types={
            "double_list": CTypeIR(
                c_type="ifcopenshell_double_list_t",
                kind="sequence",
                fields=(
                    CFieldIR("items", "double*"),
                    CFieldIR("size", "size_t"),
                    CFieldIR("owner", "void*"),
                ),
                destroy_function=None,
                element_type="double",
                sequence_depth=1,
            ),
            "double_list_list": CTypeIR(
                c_type="ifcopenshell_double_list_list_t",
                kind="sequence",
                fields=(
                    CFieldIR("items", "ifcopenshell_double_list_t*"),
                    CFieldIR("size", "size_t"),
                    CFieldIR("owner", "void*"),
                ),
                destroy_function=None,
                element_type="ifcopenshell_double_list_t",
                sequence_depth=2,
            ),
            "points_variant": CTypeIR(
                c_type="ifcopenshell_demo_points_variant_t",
                kind="variant",
                fields=(
                    CFieldIR("kind", "int32_t"),
                    CFieldIR("value_0", "ifcopenshell_double_list_list_t"),
                    CFieldIR("value_1", "ifcopenshell_double_list_list_t"),
                ),
                destroy_function="ifcopenshell_demo_points_variant_destroy",
                element_type=points_type.cpp_type,
            ),
        },
        functions={function.c_name: function},
    )

    declarations = render_typescript_declarations(metadata)
    direct = render_api_direct(metadata)

    assert (
        "transform(points: [number, number][] | [number, number, number][], "
        "origin: [number, number, number]): "
        "[number, number, number, number, number, number, number, number, "
        "number, number, number, number, number, number, number, number];"
        in declarations
    )
    assert (
        "transform(points: [number, number][] | [number, number, number][], "
        "origin: [number, number, number]): "
        "[number, number, number, number, number, number, number, number, "
        "number, number, number, number, number, number, number, number];" in direct
    )
    assert '"fixedLengths": [null, 2]' in direct
    assert '"fixedLengths": [null, 3]' in direct
    assert '"mode": "sequence"' in direct
    assert 'encodeOptionValue("origin", origin' in direct
    assert "[3]" in direct
    glue = render_js_glue(metadata)
    assert (
        "var _pointsPtr = _allocInputVariant(module, points, "
        '"ifcopenshell_demo_points_variant_t");' in glue
    )
    assert (
        "if (_pointsPtr) _freeInputVariant(module, _pointsPtr, "
        '"ifcopenshell_demo_points_variant_t");' in glue
    )


def test_sequence_types_preserve_enum_and_literal_domains() -> None:
    modes = TypeSpec(
        kind="int32",
        sequence_depth=1,
        enum_values=("WALL", "SLAB"),
        enum_numeric_values=(0, 1),
    )
    labels = TypeSpec(
        kind="string",
        sequence_depth=1,
        literal_value="ACTIVE",
    )
    function = _make_function(
        c_name="ifcopenshell_demo_domains",
        params=(
            CParamIR(
                "modes",
                "const ifcopenshell_int32_list_t*",
                "param",
                "sequence",
                type=modes,
            ),
            CParamIR(
                "labels",
                "const ifcopenshell_string_list_t*",
                "param",
                "sequence",
                type=labels,
            ),
        ),
        returns=modes,
        public_module="demo",
    )
    aliased_modes = replace(modes, alias="Mode")
    aliased_function = _make_function(
        c_name="ifcopenshell_demo_aliased_domains",
        params=(
            CParamIR(
                "modes",
                "const ifcopenshell_int32_list_t*",
                "param",
                "sequence",
                type=aliased_modes,
            ),
        ),
        returns=TypeSpec(kind="void"),
        public_module="demo",
    )
    metadata = _make_metadata(
        c_prefix="ifcopenshell",
        functions={
            function.c_name: function,
            aliased_function.c_name: aliased_function,
        },
    )

    declarations = render_typescript_declarations(metadata)
    direct = render_api_direct(metadata)

    assert (
        "domains(modes: ('WALL' | 'SLAB')[], labels: 'ACTIVE'[]): "
        "('WALL' | 'SLAB')[];" in declarations
    )
    assert "modes: ('WALL' | 'SLAB')[]" in direct
    assert "labels: 'ACTIVE'[]" in direct
    assert "): ('WALL' | 'SLAB')[];" in direct
    assert "export type Mode = 'WALL' | 'SLAB';" in declarations
    assert "aliasedDomains(modes: Mode[]): void;" in declarations
    assert "export type Mode = 'WALL' | 'SLAB';" in direct
    assert "aliasedDomains(modes: Mode[]): void;" in direct


def test_js_glue_marshals_enum_domains_at_the_native_boundary() -> None:
    mode = TypeSpec(
        kind="int32",
        alias="Mode",
        enum_values=("WALL", "SLAB"),
        enum_numeric_values=(0, 1),
    )
    modes = replace(mode, sequence_depth=1)
    scalar_function = _make_function(
        c_name="ifcopenshell_demo_round_trip_mode",
        params=(CParamIR("mode", "int32_t", "param", "int32", type=mode),),
        returns=mode,
        public_module="demo",
    )
    sequence_function = _make_function(
        c_name="ifcopenshell_demo_round_trip_modes",
        params=(
            CParamIR(
                "modes",
                "const ifcopenshell_int32_list_t*",
                "param",
                "sequence",
                type=modes,
            ),
        ),
        returns=modes,
        public_module="demo",
    )
    option = COptionIR(
        "ModeOptions",
        "ifcopenshell_demo_mode_options_t",
        (COptionFieldIR("mode", mode, "int32_t"),),
    )
    option_function = _make_function(
        c_name="ifcopenshell_demo_set_mode",
        params=(
            CParamIR(
                "options",
                "const ifcopenshell_demo_mode_options_t*",
                "param",
                "option",
            ),
        ),
        returns=TypeSpec(kind="void"),
        public_module="demo",
    )
    metadata = _make_metadata(
        c_prefix="ifcopenshell",
        value_types={
            "int32_list": CTypeIR(
                c_type="ifcopenshell_int32_list_t",
                kind="sequence",
                fields=(
                    CFieldIR("items", "int32_t*"),
                    CFieldIR("size", "size_t"),
                    CFieldIR("owner", "void*"),
                ),
                destroy_function="ifcopenshell_int32_list_destroy",
                element_type="int32_t",
                sequence_depth=1,
            ),
        },
        option_structs={option.name: option},
        functions={
            scalar_function.c_name: scalar_function,
            sequence_function.c_name: sequence_function,
            option_function.c_name: option_function,
        },
    )

    glue = render_js_glue(metadata)
    direct = render_api_direct(metadata)

    assert "roundTripMode(mode: Mode): Mode;" in direct
    assert "roundTripMode: (mode: Mode) => Mode;" in direct
    assert '_enumInputValue(mode, {"WALL": 0, "SLAB": 1}, "mode")' in glue
    assert '_mapEnumInput(modes, {"WALL": 0, "SLAB": 1}, 1, "modes")' in glue
    assert (
        '_enumOutputValue(module.getValue(outResultPtr, \'i32\'), {"0": "WALL", "1": "SLAB"}, \'result\')'
        in glue
    )
    assert "_mapEnumOutput(_readValueType(module, outResultPtr" in glue
    assert '"enumValues": {' in glue
    assert "if (field.enumValues) value = field.sequenceDepth > 0" in glue
    assert "Invalid literal for ${name}: ${String(value)}" in glue


def test_direct_api_recursively_encodes_nested_option_records() -> None:
    nested = COptionIR(
        "PanelProperties",
        "ifcopenshell_demo_panel_properties_t",
        (
            COptionFieldIR(
                "panel_width",
                TypeSpec(kind="double", nullable=True),
                "double",
                presence_field="has_panel_width",
            ),
        ),
    )
    parent = COptionIR(
        "DoorOptions",
        "ifcopenshell_demo_door_options_t",
        (
            COptionFieldIR(
                "panel_properties",
                TypeSpec(
                    kind="option",
                    struct="PanelProperties",
                    nullable=True,
                    sequence_depth=1,
                ),
                "const ifcopenshell_demo_panel_properties_t*",
                presence_field="has_panel_properties",
            ),
        ),
    )
    function = _make_function(
        c_name="ifcopenshell_demo_add_door",
        params=(
            CParamIR(
                "options",
                "const ifcopenshell_demo_door_options_t*",
                "param",
                "option",
            ),
        ),
        returns=TypeSpec(kind="bool"),
        public_module="demo",
    )
    metadata = _make_metadata(
        c_prefix="ifcopenshell",
        option_structs={"DoorOptions": parent, "PanelProperties": nested},
        functions={function.c_name: function},
    )

    direct = render_api_direct(metadata)

    assert "panelProperties?: IfcOpenShellDemoPanelProperties[];" in direct
    assert '"panelWidth": "panel_width"' in direct
    assert '"panelProperties": {' in direct
    assert '"sequenceDepth": 1' in direct
    assert "if (record)" in direct
    assert "return item.map((nested) => encodeRecord(nested, depth + 1));" in direct
    assert "record.records" in direct


def test_direct_api_encodes_semantic_variant_sequences() -> None:
    plane = COptionIR(
        "PlaneClipping",
        "ifcopenshell_demo_plane_clipping_t",
        (
            COptionFieldIR(
                "location",
                TypeSpec(kind="double", sequence_depth=1),
                "const ifcopenshell_double_list_t*",
            ),
        ),
    )
    entity = COptionIR(
        "EntityClipping",
        "ifcopenshell_demo_entity_clipping_t",
        (
            COptionFieldIR(
                "entity",
                TypeSpec(kind="handle", handle="instance"),
                "ifcopenshell_instance_t*",
            ),
        ),
    )
    clipping_type = TypeSpec(
        kind="variant",
        nullable=True,
        sequence_depth=1,
        variants=(
            TypeSpec(kind="option", struct="PlaneClipping"),
            TypeSpec(kind="option", struct="EntityClipping"),
        ),
    )
    parent = COptionIR(
        "ApplyOptions",
        "ifcopenshell_demo_apply_options_t",
        (
            COptionFieldIR(
                "clippings",
                clipping_type,
                "const ifcopenshell_demo_clipping_variant_list_t*",
                presence_field="has_clippings",
            ),
        ),
    )
    variant = CTypeIR(
        c_type="ifcopenshell_demo_clipping_variant_t",
        kind="variant",
        fields=(
            CFieldIR("kind", "int32_t"),
            CFieldIR("value_0", "const ifcopenshell_demo_plane_clipping_t*"),
            CFieldIR("value_1", "const ifcopenshell_demo_entity_clipping_t*"),
        ),
        destroy_function=None,
    )
    variant_list = CTypeIR(
        c_type="ifcopenshell_demo_clipping_variant_list_t",
        kind="input_variant_sequence",
        fields=(
            CFieldIR("items", "ifcopenshell_demo_clipping_variant_t*"),
            CFieldIR("size", "size_t"),
        ),
        destroy_function=None,
        element_type="ifcopenshell_demo_clipping_variant_t",
        sequence_depth=1,
    )
    function = _make_function(
        c_name="ifcopenshell_demo_apply",
        params=(
            CParamIR(
                "options", "const ifcopenshell_demo_apply_options_t*", "param", "option"
            ),
        ),
        public_module="demo",
    )
    metadata = _make_metadata(
        c_prefix="ifcopenshell",
        handles={"instance": _make_handle("ifcopenshell_instance_t")},
        value_types={
            "demo_clipping_variant": variant,
            "demo_clipping_variant_list": variant_list,
        },
        option_structs={
            "ApplyOptions": parent,
            "PlaneClipping": plane,
            "EntityClipping": entity,
        },
        functions={function.c_name: function},
    )

    direct = render_api_direct(metadata)
    javascript, _ = render_wasm_bindings(metadata)

    assert (
        "clippings?: (IfcOpenShellDemoPlaneClipping | IfcOpenShellDemoEntityClipping)[];"
        in direct
    )
    assert '"sequenceDepth": 1' in direct
    assert "return item.map((nested) => encodeVariant(nested, depth + 1));" in direct
    assert "input_variant_sequence" in javascript
    assert "_writeInputVariant(module, itemPtr" in javascript
    assert (
        "const layout = _getStructLayout(metadata);\n    const kind = value?.kind;"
        in javascript
    )
    assert "Invalid variant alternative for ${metadata.cType}." in javascript


class TestHostMetadata:
    def test_finalize_function_preserves_param_semantics(self):
        call = CallIR(
            expose_as="demo.use",
            c_name="ifcopenshell_demo_use",
            receiver=None,
            returns=TypeSpec(kind="void"),
            params=(
                ParamSpec(
                    "properties",
                    TypeSpec(
                        kind="opaque_ptr",
                        cpp_type="arbitrary_property_builder_t*",
                        semantic="property_map",
                    ),
                ),
                ParamSpec(
                    "opaque",
                    TypeSpec(kind="opaque_ptr", cpp_type="arbitrary_opaque_t*"),
                ),
            ),
            operation=DirectCallOp(cpp_name="demo_use"),
            public_module="demo",
        )

        metadata = _finalize_function(call, _make_ir())
        params = {param.name: param for param in metadata.params}

        assert params["properties"].semantic == "property_map"
        assert params["opaque"].semantic is None
        assert metadata.public_module == "demo"


class TestWasmTypescript:
    def test_generates_handle_class_and_destroy_method(self):
        metadata = _make_metadata(
            handles={"file": _make_handle("ifcopenshell_demo_file_t")}
        )
        code = render_typescript_declarations(metadata)
        assert "export class IfcOpenshellDemoFile" in code
        assert "destroy(): void;" in code
        assert "readonly ptr: number;" in code

    def test_input_record_sequences_have_exact_types_and_failure_cleanup(self):
        option = COptionIR(
            name="TextureOptions",
            c_type="ifcopenshell_demo_texture_options_t",
            fields=(
                COptionFieldIR("repeat_s", TypeSpec(kind="bool"), "bool"),
                COptionFieldIR(
                    "mode", TypeSpec(kind="string", nullable=True), "const char*"
                ),
                COptionFieldIR(
                    "transform",
                    TypeSpec(kind="handle", handle="instance", nullable=True),
                    "ifcopenshell_instance_t*",
                ),
                COptionFieldIR(
                    "parameter",
                    TypeSpec(kind="string", sequence_depth=1, nullable=True),
                    "const ifcopenshell_string_list_t*",
                ),
            ),
        )
        record_list = CTypeIR(
            c_type="ifcopenshell_demo_texture_options_list_t",
            kind="input_record_sequence",
            fields=(
                CFieldIR("items", "ifcopenshell_demo_texture_options_t*"),
                CFieldIR("size", "size_t"),
            ),
            destroy_function=None,
            element_type="ifcopenshell_demo_texture_options_t",
            sequence_depth=1,
        )
        batch = COptionIR(
            name="BatchOptions",
            c_type="ifcopenshell_demo_batch_options_t",
            fields=(
                COptionFieldIR(
                    "textures",
                    TypeSpec(
                        kind="option",
                        struct="TextureOptions",
                        sequence_depth=1,
                        nullable=True,
                    ),
                    "const ifcopenshell_demo_texture_options_list_t*",
                    presence_field="has_textures",
                ),
            ),
        )
        metadata = _make_metadata(
            c_prefix="ifcopenshell",
            handles={"instance": _make_handle("ifcopenshell_instance_t")},
            value_types={"texture_options_list": record_list},
            option_structs={"BatchOptions": batch, "TextureOptions": option},
            functions={
                "ifcopenshell_style_add_textures": _make_function(
                    c_name="ifcopenshell_style_add_textures",
                    params=(
                        CParamIR(
                            "textures",
                            "const ifcopenshell_demo_texture_options_list_t*",
                            "param",
                            "option",
                        ),
                    ),
                )
            },
        )
        declarations = render_typescript_declarations(metadata)
        glue = render_js_glue(metadata)
        bridge = render_api_direct(metadata)
        assert "repeat_s: boolean;" in declarations
        assert "mode?: string;" in declarations
        assert "transform?: IfcOpenshellInstance;" in declarations
        assert "parameter?: string[];" in declarations
        assert "textures: IfcOpenshellDemoTextureOptions[]" in declarations
        assert '"kind": "input_record_sequence"' in glue
        assert "for (let index = 0; index < value.length; index += 1)" in glue
        assert (
            "_freeInputOptionFields(module, itemsPtr + index * elementInfo.size, elementMetadata)"
            in glue
        )
        assert "if (itemsPtr) module._free(itemsPtr);" in glue
        assert "textures: IfcOpenShellDemoTextureOptions[]" in bridge
        assert (
            'textures.map((item) => encodeOptions(item, {"mode": "mode", '
            '"parameter": "parameter", "repeatS": "repeat_s", '
            '"transform": "transform"}, shell, temps))' in bridge
        )
        assert "textures?: IfcOpenShellDemoTextureOptions[];" in bridge

    def test_maps_scalar_and_handle_types(self):
        metadata = _make_metadata(
            handles={"file": _make_handle("ifcopenshell_demo_file_t")},
            functions={
                "ifcopenshell_demo_open_file": _make_function(
                    c_name="ifcopenshell_demo_open_file",
                    params=(CParamIR("path", "const char*", "param", "string"),),
                    returns=TypeSpec(kind="handle", handle="file"),
                ),
                "ifcopenshell_demo_set_flag": _make_function(
                    c_name="ifcopenshell_demo_set_flag",
                    params=(CParamIR("flag", "bool", "param", "bool"),),
                ),
            },
        )
        code = render_typescript_declarations(metadata)
        assert "openFile(path: string): IfcOpenshellDemoFile;" in code
        assert "setFlag(flag: boolean): void;" in code

    def test_generates_typed_array_buffer_overloads(self):
        metadata = _make_metadata(
            handles={"mesh": _make_handle("ifcopenshell_demo_mesh_t")},
            functions={
                "ifcopenshell_demo_mesh_verts_buffer": _make_function(
                    c_name="ifcopenshell_demo_mesh_verts_buffer",
                    receiver="mesh",
                    returns=TypeSpec(kind="double_buffer"),
                ),
                "ifcopenshell_demo_mesh_verts_buffer_size": _make_function(
                    c_name="ifcopenshell_demo_mesh_verts_buffer_size",
                    receiver="mesh",
                    returns=TypeSpec(kind="size"),
                ),
            },
        )

        code = render_typescript_declarations(metadata)

        assert "export type IfcOpenshellNumericTypedArray" in code
        assert "vertsBuffer(): Float64Array;" in code
        assert (
            "vertsBuffer<T extends IfcOpenshellNumericTypedArray>(arrayType: "
            "IfcOpenshellNumericArrayConstructor<T>): T;"
        ) in code

    def test_generates_nested_module_interfaces(self):
        metadata = BindingABI(
            module="ifcopenshell_wrapper",
            c_prefix="ifcopenshell",
            handles={"settings": _make_handle("ifcopenshell_geom_settings_t")},
            value_types={},
            functions={
                "ifcopenshell_parse_open": _make_function(
                    c_name="ifcopenshell_parse_open",
                    params=(CParamIR("path", "const char*", "param", "string"),),
                ),
                "ifcopenshell_geom_create_settings": _make_function(
                    c_name="ifcopenshell_geom_create_settings",
                    returns=TypeSpec(kind="handle", handle="settings"),
                ),
                "ifcopenshell_unit_add_si_unit": _make_function(
                    c_name="ifcopenshell_unit_add_si_unit",
                    params=(CParamIR("unit_type", "const char*", "param", "string"),),
                ),
            },
            error_functions=_DEFAULT_ERROR_FUNCTIONS,
        )
        code = render_typescript_declarations(metadata)
        assert "export interface IfcOpenshellParseModule" in code
        assert "open(path: string): void;" in code
        assert (
            "openBytes(bytes: Uint8Array | ArrayBuffer | ArrayBufferView, filename?: string, readonly?: boolean): void;"
        ) in code
        assert "export interface IfcOpenshellGeomModule" in code
        assert "createSettings(): IfcOpenshellGeomSettings;" in code
        assert "export interface IfcOpenshellUnitModule" in code
        assert "addSiUnit(unit_type: string): void;" in code
        assert "parse: IfcOpenshellParseModule;" in code
        assert "geom: IfcOpenshellGeomModule;" in code
        assert "unit: IfcOpenshellUnitModule;" in code
        assert (
            "    createSettings(): IfcOpenshellGeomSettings;"
            not in code.split("export interface IfcOpenshellModule")[1]
        )

    def test_renders_source_docs_as_jsdoc(self):
        metadata = _make_metadata(
            functions={
                "ifcopenshell_demo_root_create_entity": _make_function(
                    c_name="ifcopenshell_demo_root_create_entity",
                    params=(CParamIR("ifc_class", "const char*", "param", "string"),),
                    doc="Create an IFC entity.\nInitializes identity and ownership metadata.",
                ),
            },
        )
        code = render_typescript_declarations(metadata)
        assert (
            "    /**\n"
            "     * Create an IFC entity.\n"
            "     * Initializes identity and ownership metadata.\n"
            "     */\n"
            "    rootCreateEntity(ifc_class: string): void;"
        ) in code

    def test_renders_option_struct_interfaces(self):
        metadata = _make_metadata(
            option_structs={
                "CreateEntityOptions": COptionIR(
                    name="CreateEntityOptions",
                    c_type="ifcopenshell_demo_create_entity_options_t",
                    fields=(
                        COptionFieldIR(
                            "ifc_class", TypeSpec(kind="string"), "const char*"
                        ),
                        COptionFieldIR(
                            "name",
                            TypeSpec(kind="string", nullable=True),
                            "const char*",
                        ),
                    ),
                )
            }
        )

        code = render_typescript_declarations(metadata)

        assert "export interface IfcOpenshellDemoCreateEntityOptions" in code
        assert "ifc_class: string;" in code
        assert "name?: string;" in code

    def test_preserves_native_option_field_casing(self):
        metadata = _make_metadata(
            handles={"instance": _make_handle("ifcopenshell_demo_instance_t")},
            option_structs={
                "AssignObjectOptions": COptionIR(
                    name="AssignObjectOptions",
                    c_type="ifcopenshell_demo_assign_object_options_t",
                    fields=(
                        COptionFieldIR(
                            "owner_history",
                            TypeSpec(kind="handle", handle="instance", nullable=True),
                            "ifcopenshell_demo_instance_t*",
                        ),
                        COptionFieldIR(
                            "relating_object",
                            TypeSpec(kind="handle", handle="instance"),
                            "ifcopenshell_demo_instance_t*",
                        ),
                        COptionFieldIR(
                            "sync_predefined_type",
                            TypeSpec(kind="bool"),
                            "bool",
                        ),
                    ),
                )
            },
        )

        code = render_typescript_declarations(metadata)

        assert "export interface IfcOpenshellDemoAssignObjectOptions" in code
        assert "owner_history?: IfcOpenshellDemoInstance;" in code
        assert "relating_object: IfcOpenshellDemoInstance;" in code
        assert "sync_predefined_type: boolean;" in code
        assert "ownerHistory" not in code
        assert "relatingObject" not in code
        assert "syncPredefinedType" not in code

    def test_renders_option_struct_parameters(self):
        metadata = _make_metadata(
            option_structs={
                "CreateEntityOptions": COptionIR(
                    name="CreateEntityOptions",
                    c_type="ifcopenshell_demo_create_entity_options_t",
                    fields=(
                        COptionFieldIR(
                            "ifc_class", TypeSpec(kind="string"), "const char*"
                        ),
                    ),
                )
            },
            functions={
                "ifcopenshell_demo_root_create_entity": _make_function(
                    c_name="ifcopenshell_demo_root_create_entity",
                    params=(
                        CParamIR(
                            "options",
                            "const ifcopenshell_demo_create_entity_options_t*",
                            "param",
                            "option",
                        ),
                    ),
                )
            },
        )

        code = render_typescript_declarations(metadata)

        assert (
            "rootCreateEntity(options: IfcOpenshellDemoCreateEntityOptions): void;"
            in code
        )


class TestWasmJsGlue:
    def test_generates_handle_wrapper_and_destroy_logic(self):
        metadata = _make_metadata(
            handles={"file": _make_handle("ifcopenshell_demo_file_t")}
        )
        code = render_js_glue(metadata)
        assert "export class IfcOpenshellDemoFile" in code
        assert "destroy()" in code
        assert "module._ifcopenshell_demo_file_destroy" in code

    def test_generates_module_factory_and_error_handling(self):
        metadata = _make_metadata(
            handles={"file": _make_handle("ifcopenshell_demo_file_t")},
            functions={
                "ifcopenshell_demo_open_file": _make_function(
                    c_name="ifcopenshell_demo_open_file",
                    params=(CParamIR("path", "const char*", "param", "string"),),
                    returns=TypeSpec(kind="handle", handle="file"),
                )
            },
        )
        code = render_js_glue(metadata)
        assert "export async function createIfcOpenshellModule" in code
        assert "module._ifcopenshell_demo_clear_error()" in code
        assert "throw _lastError(module, 'ifcopenshell_demo_open_file failed')" in code
        assert "export class IfcOpenShellError extends Error" in code
        assert "const kind = module._ifcopenshell_demo_last_error_kind();" in code
        assert "const code = module._ifcopenshell_demo_last_error_code();" in code
        assert "export function isIfcOpenShellAbortError" in code
        assert (
            "openFile: (path) => invoke_ifcopenshell_demo_open_file(module, path)"
            in code
        )
        assert "createIfcOpenshellModule(initModule, wasmUrl, options = {})" in code
        assert (
            "const pluginBaseUrl = options.pluginBaseUrl ?? new URL('.', import.meta.url).href;"
            in code
        )
        assert (
            "const pluginLoader = options.pluginLoader ?? defaultPluginLoader;" in code
        )
        assert "return new URL(entry.wasm, pluginBaseUrl).href;" in code
        assert "module.FS.writeFile(path, bytes);" in code
        assert (
            "await module.loadDynamicLibrary(path, { global: true, allowUndefined: true, loadAsync: true });"
            in code
        )
        assert "module.FS.unlink(path);" in code
        assert "import initIfcOpenShellWasmModule" not in code

    def test_wraps_owned_and_borrowed_handle_returns_with_envelope_ownership(self):
        metadata = _make_metadata(
            handles={"template": _make_handle("ifcopenshell_demo_template_t")},
            functions={
                "ifcopenshell_demo_template_create": _make_function(
                    c_name="ifcopenshell_demo_template_create",
                    returns=TypeSpec(
                        kind="handle", handle="template", ownership="owned"
                    ),
                ),
                "ifcopenshell_demo_template_cached": _make_function(
                    c_name="ifcopenshell_demo_template_cached",
                    returns=TypeSpec(
                        kind="handle", handle="template", ownership="borrowed"
                    ),
                ),
            },
        )

        code = render_js_glue(metadata)

        assert (
            "_wrapIfcOpenshellDemoTemplate(module.getValue(outResultPtr, '*'), true, module)"
            in code
        )
        assert (
            code.count(
                "_wrapIfcOpenshellDemoTemplate(module.getValue(outResultPtr, '*'), true, module)"
            )
            == 2
        )

    def test_destroys_variant_results_after_transferring_handles(self):
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
        metadata = _make_metadata(
            handles={"instance": _make_handle("ifcopenshell_demo_instance_t")},
            value_types={
                "instance_string_variant": CTypeIR(
                    c_type="ifcopenshell_demo_instance_string_variant_t",
                    kind="variant",
                    fields=(
                        CFieldIR("kind", "int32_t"),
                        CFieldIR("value_0", "ifcopenshell_demo_instance_t*"),
                        CFieldIR("value_1", "ifcopenshell_string_t"),
                    ),
                    destroy_function="ifcopenshell_demo_instance_string_variant_destroy",
                    element_type=variant.cpp_type,
                )
            },
            functions={
                "ifcopenshell_demo_value": _make_function(
                    c_name="ifcopenshell_demo_value", returns=variant
                )
            },
        )

        code = render_js_glue(metadata)

        assert (
            "module._ifcopenshell_demo_instance_string_variant_destroy(outResultPtr);"
            in code
        )
        assert "const handlePtr = module.getValue(fieldPtr, '*');" in code
        assert "module.setValue(fieldPtr, 0, '*');" in code

    def test_transfers_wasm_handle_sequence_items_before_destroying_containers(self):
        metadata = _make_metadata(
            handles={"item": _make_handle("ifcopenshell_demo_item_t")},
            value_types={
                "demo_item_list": CTypeIR(
                    c_type="ifcopenshell_demo_item_list_t",
                    kind="handle_sequence",
                    fields=(
                        CFieldIR("items", "ifcopenshell_demo_item_t**"),
                        CFieldIR("size", "size_t"),
                    ),
                    destroy_function="ifcopenshell_demo_item_list_destroy",
                    element_type="ifcopenshell_demo_item_t",
                    sequence_depth=1,
                ),
                "demo_item_list_list": CTypeIR(
                    c_type="ifcopenshell_demo_item_list_list_t",
                    kind="handle_sequence",
                    fields=(
                        CFieldIR("items", "ifcopenshell_demo_item_list_t*"),
                        CFieldIR("size", "size_t"),
                    ),
                    destroy_function="ifcopenshell_demo_item_list_list_destroy",
                    element_type="ifcopenshell_demo_item_list_t",
                    sequence_depth=2,
                ),
            },
            functions={
                "ifcopenshell_demo_items": _make_function(
                    c_name="ifcopenshell_demo_items",
                    returns=TypeSpec(kind="handle", handle="item", sequence_depth=1),
                ),
                "ifcopenshell_demo_rows": _make_function(
                    c_name="ifcopenshell_demo_rows",
                    returns=TypeSpec(kind="handle", handle="item", sequence_depth=2),
                ),
            },
        )

        code = render_js_glue(metadata)

        assert (
            "const handle = _wrapHandleByType(module, elementType, module.getValue(elementPtr, '*'), true);"
            in code
        )
        assert "module.setValue(elementPtr, 0, '*');" in code
        assert "module._ifcopenshell_demo_item_list_destroy(outResultPtr);" in code
        assert "module._ifcopenshell_demo_item_list_list_destroy(outResultPtr);" in code

    def test_result_struct_and_nullable_result_struct_cleanup_is_generated(self):
        metadata = _make_metadata(
            handles={"item": _make_handle("ifcopenshell_demo_item_t")},
            value_types={
                "string": CTypeIR(
                    c_type="ifcopenshell_string_t",
                    kind="string",
                    fields=(
                        CFieldIR("data", "char*"),
                        CFieldIR("size", "size_t"),
                        CFieldIR("owned", "bool"),
                        CFieldIR("owner", "void*"),
                    ),
                    destroy_function="ifcopenshell_string_destroy",
                ),
                "demo_item_list": CTypeIR(
                    c_type="ifcopenshell_demo_item_list_t",
                    kind="handle_sequence",
                    fields=(
                        CFieldIR("items", "ifcopenshell_demo_item_t**"),
                        CFieldIR("size", "size_t"),
                    ),
                    destroy_function="ifcopenshell_demo_item_list_destroy",
                    element_type="ifcopenshell_demo_item_t",
                    sequence_depth=1,
                ),
                "double_list": CTypeIR(
                    c_type="ifcopenshell_double_list_t",
                    kind="sequence",
                    fields=(CFieldIR("items", "double*"), CFieldIR("size", "size_t")),
                    destroy_function="ifcopenshell_double_list_destroy",
                    element_type="double",
                    sequence_depth=1,
                ),
                "demo_result": CTypeIR(
                    c_type="ifcopenshell_demo_result_t",
                    kind="result_struct",
                    fields=(
                        CFieldIR("item", "ifcopenshell_demo_item_t*"),
                        CFieldIR("items", "ifcopenshell_demo_item_list_t"),
                        CFieldIR("values", "ifcopenshell_double_list_t"),
                        CFieldIR("label", "ifcopenshell_string_t"),
                    ),
                    destroy_function="ifcopenshell_demo_result_destroy",
                ),
                "optional_demo_result": CTypeIR(
                    c_type="ifcopenshell_optional_demo_result_t",
                    kind="optional_result_struct",
                    fields=(
                        CFieldIR("has_value", "bool"),
                        CFieldIR("value", "ifcopenshell_demo_result_t"),
                    ),
                    destroy_function="ifcopenshell_optional_demo_result_destroy",
                    element_type="demo_result",
                ),
            },
            functions={
                "ifcopenshell_demo_result": _make_function(
                    c_name="ifcopenshell_demo_result",
                    returns=TypeSpec(kind="struct", struct="demo_result"),
                ),
                "ifcopenshell_demo_optional": _make_function(
                    c_name="ifcopenshell_demo_optional",
                    returns=TypeSpec(
                        kind="struct", struct="demo_result", nullable=True
                    ),
                ),
            },
        )

        code = render_js_glue(metadata)

        assert "result[field.name] = _wrapHandleByType" in code
        assert "module.setValue(fieldPtr, 0, '*');" in code
        assert "module._ifcopenshell_demo_result_destroy(outResultPtr);" in code
        assert (
            "module._ifcopenshell_optional_demo_result_destroy(outResultPtr);" in code
        )

    def test_handle_destroy_is_idempotent_after_envelope_transfer(self):
        metadata = _make_metadata(
            handles={"item": _make_handle("ifcopenshell_demo_item_t")},
        )

        code = render_js_glue(metadata)

        assert "if (this.#ptr && this.#envelopeOwned" in code
        assert "this.#ptr = 0;" in code
        assert "this.#envelopeOwned = false;" in code

    def test_generates_nested_api_modules(self):
        metadata = BindingABI(
            module="ifcopenshell_wrapper",
            c_prefix="ifcopenshell",
            handles={"settings": _make_handle("ifcopenshell_geom_settings_t")},
            value_types={},
            functions={
                "ifcopenshell_parse_open": _make_function(
                    c_name="ifcopenshell_parse_open",
                    params=(CParamIR("path", "const char*", "param", "string"),),
                ),
                "ifcopenshell_geom_create_settings": _make_function(
                    c_name="ifcopenshell_geom_create_settings",
                    returns=TypeSpec(kind="handle", handle="settings"),
                ),
                "ifcopenshell_unit_add_si_unit": _make_function(
                    c_name="ifcopenshell_unit_add_si_unit",
                    params=(CParamIR("unit_type", "const char*", "param", "string"),),
                ),
            },
            error_functions=_DEFAULT_ERROR_FUNCTIONS,
        )
        code = render_js_glue(metadata)
        assert "parse: Object.freeze({" in code
        assert "open: (path) => invoke_ifcopenshell_parse_open(module, path)" in code
        assert "openBytes," in code
        assert "module.FS.writeFile(path, normalizeVirtualFileBytes(bytes));" in code
        assert "module.FS.unlink(path);" in code
        assert "geom: Object.freeze({" in code
        assert (
            "createSettings: () => invoke_ifcopenshell_geom_create_settings(module)"
            in code
        )
        assert "unit: Object.freeze({" in code
        assert (
            "addSiUnit: (unit_type) => invoke_ifcopenshell_unit_add_si_unit(module, unit_type)"
            in code
        )
        assert (
            "        createSettings: () => invoke_ifcopenshell_geom_create_settings(module),\n"
            "        geom: Object.freeze({" not in code
        )

    def test_generates_instance_methods_for_receiver_functions(self):
        metadata = _make_metadata(
            handles={"file": _make_handle("ifcopenshell_demo_file_t")},
            functions={
                "ifcopenshell_demo_file_set_name": _make_function(
                    c_name="ifcopenshell_demo_file_set_name",
                    receiver="file",
                    params=(CParamIR("name", "const char*", "param", "string"),),
                )
            },
        )
        code = render_js_glue(metadata)
        assert "setName(name)" in code
        assert (
            "invoke_ifcopenshell_demo_file_set_name(this.#module, this, name)" in code
        )

    def test_dispatches_receiver_overloads_by_argument_count(self):
        metadata = _make_metadata(
            handles={"file": _make_handle("ifcopenshell_demo_file_t")},
            functions={
                "ifcopenshell_demo_file_build_inverses": _make_function(
                    c_name="ifcopenshell_demo_file_build_inverses",
                    receiver="file",
                ),
                "ifcopenshell_demo_file_build_inverses_": _make_function(
                    c_name="ifcopenshell_demo_file_build_inverses_",
                    receiver="file",
                    params=(
                        CParamIR(
                            "entity",
                            "ifcopenshell_demo_file_t*",
                            "param",
                            "handle",
                        ),
                    ),
                ),
            },
        )

        code = render_js_glue(metadata)

        assert code.count("    buildInverses(") == 1
        assert "buildInverses(...args)" in code
        assert (
            "case 0: return invoke_ifcopenshell_demo_file_build_inverses(this.#module, this, ...args);"
            in code
        )
        assert (
            "case 1: return invoke_ifcopenshell_demo_file_build_inverses_(this.#module, this, ...args);"
            in code
        )

    def test_rejects_ambiguous_receiver_overloads(self):
        metadata = _make_metadata(
            handles={"file": _make_handle("ifcopenshell_demo_file_t")},
            functions={
                "ifcopenshell_demo_file_find": _make_function(
                    c_name="ifcopenshell_demo_file_find",
                    receiver="file",
                    params=(CParamIR("name", "const char*", "param", "string"),),
                ),
                "ifcopenshell_demo_file_find_": _make_function(
                    c_name="ifcopenshell_demo_file_find_",
                    receiver="file",
                    params=(CParamIR("id", "int32_t", "param", "int32"),),
                ),
            },
        )

        with pytest.raises(ValueError, match="both take 1 arguments"):
            render_js_glue(metadata)

    def test_snapshots_borrowed_buffers_directly_into_typed_arrays(self):
        metadata = _make_metadata(
            handles={"mesh": _make_handle("ifcopenshell_demo_mesh_t")},
            functions={
                "ifcopenshell_demo_mesh_verts_buffer": _make_function(
                    c_name="ifcopenshell_demo_mesh_verts_buffer",
                    receiver="mesh",
                    returns=TypeSpec(kind="double_buffer"),
                ),
                "ifcopenshell_demo_mesh_verts_buffer_size": _make_function(
                    c_name="ifcopenshell_demo_mesh_verts_buffer_size",
                    receiver="mesh",
                    returns=TypeSpec(kind="size"),
                ),
            },
        )

        code = render_js_glue(metadata)

        assert "vertsBuffer(arrayType)" in code
        assert "let outSizePtr = 0;" in code
        assert (
            "module._ifcopenshell_demo_mesh_verts_buffer_size(self.ptr, outSizePtr)"
            in code
        )
        assert "_copyNumericBuffer(module, module.getValue(outResultPtr, '*')" in code
        assert "_NUMERIC_ARRAY_TYPES.has(TargetArray)" in code
        assert "const result = new TargetArray(source);" in code
        assert "WASM memory grew during numeric buffer snapshot" in code
        assert (
            "if (!sizeOk) throw _lastError(module, 'ifcopenshell_demo_mesh_verts_buffer_size failed')"
            in code
        )

    def test_typed_errors_cover_void_nullable_and_scalar_calls(self):
        metadata = _make_metadata(
            handles={"item": _make_handle("ifcopenshell_demo_item_t")},
            functions={
                "ifcopenshell_demo_void": _make_function(
                    c_name="ifcopenshell_demo_void",
                    returns=TypeSpec(kind="void"),
                ),
                "ifcopenshell_demo_nullable": _make_function(
                    c_name="ifcopenshell_demo_nullable",
                    returns=TypeSpec(kind="handle", handle="item", nullable=True),
                ),
                "ifcopenshell_demo_scalar": _make_function(
                    c_name="ifcopenshell_demo_scalar",
                    returns=TypeSpec(kind="int32"),
                ),
            },
        )

        code = render_js_glue(metadata)

        for name in ("void", "nullable", "scalar"):
            assert (
                f"if (!ok) throw _lastError(module, 'ifcopenshell_demo_{name} failed')"
                in code
            )

    def test_typescript_exports_typed_error_contract(self):
        metadata = _make_metadata()
        code = render_typescript_declarations(metadata)

        assert "export class IfcOpenShellError extends Error" in code
        assert "readonly kind: IfcOpenShellErrorKind;" in code
        assert "readonly code: IfcOpenShellErrorCode;" in code
        assert "readonly INVALID_QUADRANT_BEARING: 100" in code
        assert "isIfcOpenShellAbortError(error: unknown)" in code

        custom = replace(
            metadata,
            error_catalog=replace(
                metadata.error_catalog,
                kinds=(ErrorCatalogEntryIR("NONE", 27),),
                codes=(ErrorCatalogEntryIR("NONE", 42),),
            ),
        )
        js = render_js_glue(custom)
        declarations = render_typescript_declarations(custom)
        assert "NONE: 27" in js
        assert "NONE: 42" in js
        assert "readonly NONE: 27" in declarations
        assert "readonly NONE: 42" in declarations

    def test_snapshots_owner_backed_numeric_sequences_without_number_arrays(self):
        metadata = _make_metadata(
            handles={"mesh": _make_handle("ifcopenshell_demo_mesh_t")},
            value_types={
                "double_list": CTypeIR(
                    c_type="ifcopenshell_double_list_t",
                    kind="sequence",
                    fields=(
                        CFieldIR("items", "double*"),
                        CFieldIR("size", "size_t"),
                        CFieldIR("owner", "void*"),
                    ),
                    destroy_function="ifcopenshell_double_list_destroy",
                    element_type="double",
                    sequence_depth=1,
                )
            },
            functions={
                "ifcopenshell_demo_mesh_colors_buffer": _make_function(
                    c_name="ifcopenshell_demo_mesh_colors_buffer",
                    receiver="mesh",
                    returns=TypeSpec(kind="double", sequence_depth=1),
                ),
            },
        )

        code = render_js_glue(metadata)

        assert "colorsBuffer(arrayType)" in code
        assert (
            '_readValueType(module, outResultPtr, _VALUE_TYPES["ifcopenshell_double_list_t"], true, arrayType)'
            in code
        )
        assert "if (typedSnapshot) return _copyNumericBuffer" in code
        assert "module._ifcopenshell_double_list_destroy(outResultPtr);" in code

    def test_reads_int64_from_heap32_and_marshals_scalar_sequences(self):
        metadata = _make_metadata(
            value_types={
                "string": CTypeIR(
                    c_type="ifcopenshell_string_t",
                    kind="string",
                    fields=(
                        CFieldIR("data", "char*"),
                        CFieldIR("size", "size_t"),
                        CFieldIR("owned", "bool"),
                    ),
                    destroy_function="ifcopenshell_string_destroy",
                ),
                "int32_list": CTypeIR(
                    c_type="ifcopenshell_int32_list_t",
                    kind="sequence",
                    fields=(
                        CFieldIR("items", "int32_t*"),
                        CFieldIR("size", "size_t"),
                        CFieldIR("owner", "void*"),
                    ),
                    destroy_function="ifcopenshell_int32_list_destroy",
                    element_type="int32_t",
                    sequence_depth=1,
                ),
                "int32_list_list": CTypeIR(
                    c_type="ifcopenshell_int32_list_list_t",
                    kind="sequence",
                    fields=(
                        CFieldIR("items", "ifcopenshell_int32_list_t*"),
                        CFieldIR("size", "size_t"),
                        CFieldIR("owner", "void*"),
                    ),
                    destroy_function="ifcopenshell_int32_list_list_destroy",
                    element_type="ifcopenshell_int32_list_t",
                    sequence_depth=2,
                ),
                "string_list": CTypeIR(
                    c_type="ifcopenshell_string_list_t",
                    kind="sequence",
                    fields=(
                        CFieldIR("items", "ifcopenshell_string_t*"),
                        CFieldIR("size", "size_t"),
                        CFieldIR("owner", "void*"),
                    ),
                    destroy_function="ifcopenshell_string_list_destroy",
                    element_type="ifcopenshell_string_t",
                    sequence_depth=1,
                ),
            },
            functions={
                "ifcopenshell_demo_big": _make_function(
                    c_name="ifcopenshell_demo_big",
                    returns=TypeSpec(kind="int64"),
                ),
                "ifcopenshell_demo_sum_values": _make_function(
                    c_name="ifcopenshell_demo_sum_values",
                    params=(
                        CParamIR(
                            "values",
                            "const ifcopenshell_int32_list_t*",
                            "param",
                            "int32",
                        ),
                    ),
                    returns=TypeSpec(kind="int32"),
                ),
                "ifcopenshell_demo_names": _make_function(
                    c_name="ifcopenshell_demo_names",
                    returns=TypeSpec(kind="string", sequence_depth=1),
                ),
                "ifcopenshell_demo_values": _make_function(
                    c_name="ifcopenshell_demo_values",
                    returns=TypeSpec(kind="int32", sequence_depth=1),
                ),
                "ifcopenshell_demo_set_rows": _make_function(
                    c_name="ifcopenshell_demo_set_rows",
                    params=(
                        CParamIR(
                            "rows",
                            "const ifcopenshell_int32_list_list_t*",
                            "param",
                            "int32",
                        ),
                    ),
                ),
            },
        )
        code = render_js_glue(metadata)
        assert "const low = module.HEAP32[index];" in code
        assert "const high = module.HEAP32[index + 1];" in code
        assert "_allocInputSequence(module, values" in code
        assert "_freeInputSequence(module, _valuesPtr" in code
        assert (
            '_allocInputSequence(module, rows, "ifcopenshell_int32_list_list_t")'
            in code
        )
        assert (
            '_freeInputSequence(module, _rowsPtr, "ifcopenshell_int32_list_list_t")'
            in code
        )
        assert "outResultPtr = module._malloc(8);" in code
        values_wrapper = code[code.index("function invoke_ifcopenshell_demo_values") :]
        assert (
            "outResultPtr = module._malloc(12);" in values_wrapper.split("\n}\n", 1)[0]
        )
        assert "_HANDLE_C_TYPES.has(_normalizeCType(elementType))" in code
        assert "_normalizeCType(elementType).endsWith('_t')" not in code
        assert (
            '_readValueType(module, outResultPtr, _VALUE_TYPES["ifcopenshell_string_list_t"])'
            in code
        )
        assert "module._ifcopenshell_string_list_destroy(outResultPtr);" in code
        assert '"bufferMode": "snapshot"' in code
        assert "_zeroMemory(module, structPtr, layout.size);" in code
        assert (
            "if (itemsPtr) _zeroMemory(module, itemsPtr, elementInfo.size * value.length);"
            in code
        )

    def test_marshals_handle_sequence_parameters(self):
        metadata = _make_metadata(
            handles={"instance": _make_handle("ifcopenshell_instance_t")},
            value_types={
                "instance_list": CTypeIR(
                    c_type="ifcopenshell_instance_list_t",
                    kind="handle_sequence",
                    fields=(
                        CFieldIR("items", "ifcopenshell_instance_t**"),
                        CFieldIR("size", "size_t"),
                    ),
                    destroy_function=None,
                    element_type="ifcopenshell_instance_t",
                    sequence_depth=1,
                ),
            },
            functions={
                "ifcopenshell_demo_from_handles": _make_function(
                    c_name="ifcopenshell_demo_from_handles",
                    params=(
                        CParamIR(
                            "instances",
                            "const ifcopenshell_instance_list_t*",
                            "param",
                            "handle",
                        ),
                    ),
                )
            },
        )

        code = render_js_glue(metadata)

        assert "_allocInputHandleSequence(module, instances" in code
        assert "_freeInputHandleSequence(module, _instancesPtr" in code
        assert (
            "module.setValue(itemsPtr + index * POINTER_SIZE, item == null ? 0 : item.ptr, '*');"
            in code
        )

    def test_generates_dependency_aware_plugin_loader(self):
        metadata = _make_metadata()
        code = render_js_glue(metadata)
        assert "const loadingPlugins = new Map();" in code
        assert "function pluginDependencies(kind, id)" in code
        assert "for (const dependency of pluginDependencies(kind, id))" in code
        assert (
            "await loadPlugin(dependency.slice(0, separator), dependency.slice(separator + 1));"
            in code
        )
        assert "async function loadPluginLibrary(kind, id, entry)" in code
        assert (
            "await module.loadDynamicLibrary(path, { global: true, allowUndefined: true, loadAsync: true });"
            in code
        )
        assert "invoke_ifcopenshell_geom_plugin_registry_address" not in code

    def test_marshals_option_struct_parameters(self):
        metadata = _make_metadata(
            value_types={
                "double_list": CTypeIR(
                    c_type="ifcopenshell_double_list_t",
                    kind="sequence",
                    fields=(
                        CFieldIR("items", "double*"),
                        CFieldIR("size", "size_t"),
                    ),
                    destroy_function="ifcopenshell_double_list_destroy",
                    element_type="double",
                    sequence_depth=1,
                ),
                "double_list_list": CTypeIR(
                    c_type="ifcopenshell_double_list_list_t",
                    kind="sequence",
                    fields=(
                        CFieldIR("items", "ifcopenshell_double_list_t*"),
                        CFieldIR("size", "size_t"),
                    ),
                    destroy_function="ifcopenshell_double_list_list_destroy",
                    element_type="ifcopenshell_double_list_t",
                    sequence_depth=2,
                ),
            },
            option_structs={
                "CreateEntityOptions": COptionIR(
                    name="CreateEntityOptions",
                    c_type="ifcopenshell_demo_create_entity_options_t",
                    fields=(
                        COptionFieldIR(
                            "ifc_class", TypeSpec(kind="string"), "const char*"
                        ),
                        COptionFieldIR(
                            "axis",
                            TypeSpec(kind="double", sequence_depth=2),
                            "const ifcopenshell_double_list_list_t*",
                        ),
                        COptionFieldIR(
                            "name",
                            TypeSpec(kind="string", nullable=True),
                            "const char*",
                        ),
                    ),
                )
            },
            functions={
                "ifcopenshell_demo_root_create_entity": _make_function(
                    c_name="ifcopenshell_demo_root_create_entity",
                    params=(
                        CParamIR(
                            "options",
                            "const ifcopenshell_demo_create_entity_options_t*",
                            "param",
                            "option",
                        ),
                    ),
                )
            },
        )

        code = render_js_glue(metadata)

        assert '"ifcopenshell_demo_create_entity_options_t": {' in code
        assert '"optionFields": [' in code
        assert '"name": "has_name"' in code
        assert (
            'var _optionsPtr = _allocInputOption(module, options, "ifcopenshell_demo_create_entity_options_t");'
            in code
        )
        assert (
            'if (_optionsPtr) _freeInputOption(module, _optionsPtr, "ifcopenshell_demo_create_entity_options_t");'
            in code
        )
        assert "function _allocInputOption(module, value, cType)" in code
        assert "function _writeInputOption(module, ptr, value, metadata)" in code
        assert "        const layout = _getStructLayout(metadata);" in code
        assert "function _zeroMemory(module, ptr, size)" in code
        assert "module.HEAPU32.fill(0, start, start + wordCount);" in code
        assert "module.HEAPU8.fill" not in code
        assert "    _zeroMemory(module, ptr, layout.size);" in code
        assert "function _freeInputOption(module, ptr, cType)" in code
        assert (
            "function _writeInputSequence(module, structPtr, value, metadata)" in code
        )
        assert (
            "else _writeInputSequence(module, itemPtr, value[index], elementMetadata);"
            in code
        )
        assert "function _freeInputSequenceItems(module, ptr, metadata)" in code


class TestWasmApiBridge:
    def test_generates_direct_api_facade(self):
        metadata = _make_metadata(
            c_prefix="ifcopenshell",
            handles={
                "file": _make_handle("ifcopenshell_file_t"),
                "instance": _make_handle("ifcopenshell_instance_t"),
            },
            option_structs={
                "CreateEntityOptions": COptionIR(
                    name="CreateEntityOptions",
                    c_type="ifcopenshell_root_create_entity_options_t",
                    fields=(
                        COptionFieldIR(
                            "ifc_class", TypeSpec(kind="string"), "const char*"
                        ),
                        COptionFieldIR(
                            "name",
                            TypeSpec(kind="string", nullable=True),
                            "const char*",
                        ),
                        COptionFieldIR(
                            "owner_history",
                            TypeSpec(kind="handle", handle="instance", nullable=True),
                            "ifcopenshell_instance_t*",
                        ),
                    ),
                )
            },
            functions={
                "ifcopenshell_root_create_entity": _make_function(
                    c_name="ifcopenshell_root_create_entity",
                    params=(
                        CParamIR("file", "ifcopenshell_file_t*", "param", "handle"),
                        CParamIR(
                            "options",
                            "const ifcopenshell_root_create_entity_options_t*",
                            "param",
                            "option",
                        ),
                    ),
                    returns=TypeSpec(kind="handle", handle="instance"),
                    doc="Create an IFC entity.",
                )
            },
        )

        code = render_api_direct(metadata)

        assert "export interface IfcOpenShellRootCreateEntityOptions" in code
        assert "ownerHistory?: Entity;" in code
        assert "export interface RootApi" in code
        assert "export interface Api" in code
        assert (
            "createEntity(file: IfcFile, options: IfcOpenShellRootCreateEntityOptions): Entity;"
            in code
        )
        assert (
            "/**\n * @internal\n */\nexport function createApi(shell: IfcOpenShell): Api {"
            in code
        )
        assert "/**\n * @internal\n */\nexport interface Api" not in code
        assert "raw.root.createEntity(file.raw, encodeOptions(options" in code
        assert '"ifcClass": "ifc_class"' in code
        assert '"name": "name"' in code
        assert '"ownerHistory": "owner_history"' in code
        assert "type ApiInput = ApiData | PsetProperties | PsetInput;" in code
        assert (
            "function toRaw(value: ApiInput, shell: IfcOpenShell, temps: Disposable[]): RawValue"
            in code
        )
        assert (
            "function toRawSequence(value: ApiInput, shell: IfcOpenShell, temps: Disposable[]): RawValue"
            in code
        )
        assert "disposeAll(temps);" in code
        assert "return wrapEntity(shell, result) as Entity;" in code

    def test_contract_modules_retain_first_prefix_compatibility_aliases(self):
        functions = {
            name: _make_function(c_name=name, public_module=public_module)
            for name, public_module in (
                ("ifcopenshell_material_edit_material", "material"),
                ("ifcopenshell_material_reorder_set_item", "material"),
                ("ifcopenshell_shape_assign_representation", "shape"),
                ("ifcopenshell_shape_builder_get_polyline_coords", "shape_builder"),
                ("ifcopenshell_shape_builder_set_polyline_coords", "shape_builder"),
                ("ifcopenshell_pset_template_edit_pset_template", "pset_template"),
            )
        }
        metadata = _make_metadata(c_prefix="ifcopenshell", functions=functions)

        declarations = render_typescript_declarations(metadata)
        glue = render_js_glue(metadata)
        bridge = render_api_direct(metadata)

        assert "export interface MaterialReorderApi" not in bridge
        assert "editMaterial(): void;" in bridge
        assert "reorderSetItem(): void;" in bridge
        assert "export interface ShapeBuilderApi" in bridge
        assert "getPolylineCoords(): void;" in bridge
        assert "setPolylineCoords(): void;" in bridge
        assert "assignRepresentation(): void;" in bridge
        assert "builderGetPolylineCoords(): void;" in bridge
        assert "builderSetPolylineCoords(): void;" in bridge
        assert "export interface PsetTemplateApi" in bridge
        assert "editPsetTemplate(): void;" in bridge
        assert "templateEditPsetTemplate(): void;" in bridge
        assert bridge.count("const shapeBuilderApi = Object.freeze({") == 1
        assert "shapeBuilder: shapeBuilderApi," in bridge
        assert "shape_builder: shapeBuilderApi," in bridge
        assert "shape_builder: Object.freeze({" not in bridge
        assert bridge.count("const psetTemplateApi = Object.freeze({") == 1
        assert "psetTemplate: psetTemplateApi," in bridge
        assert "pset_template: psetTemplateApi," in bridge
        assert "pset_template: Object.freeze({" not in bridge

        assert "material: IfcOpenshellMaterialModule;" in declarations
        assert (
            "material_reorder: IfcOpenshellMaterialReorderModule;" not in declarations
        )
        assert "shape_builder: IfcOpenshellShapeBuilderModule;" in declarations
        assert "shape: IfcOpenshellShapeModule;" in declarations
        assert "pset_template: IfcOpenshellPsetTemplateModule;" in declarations
        assert "pset: IfcOpenshellPsetModule;" in declarations
        assert "material: Object.freeze({" in glue
        assert "shape_builder: Object.freeze({" in glue
        assert "shape: Object.freeze({" in glue
        assert "pset_template: Object.freeze({" in glue
        assert "pset: Object.freeze({" in glue

    def test_generates_exact_raw_api_members_and_signatures(self):
        metadata = _make_metadata(
            c_prefix="ifcopenshell",
            handles={
                "file": _make_handle("ifcopenshell_file_t"),
                "instance": _make_handle("ifcopenshell_instance_t"),
            },
            option_structs={
                "AssignObjectOptions": COptionIR(
                    name="AssignObjectOptions",
                    c_type="ifcopenshell_aggregate_assign_object_options_t",
                    fields=(
                        COptionFieldIR(
                            "owner_history",
                            TypeSpec(kind="handle", handle="instance", nullable=True),
                            "ifcopenshell_instance_t*",
                        ),
                        COptionFieldIR(
                            "relating_object",
                            TypeSpec(kind="handle", handle="instance"),
                            "ifcopenshell_instance_t*",
                        ),
                        COptionFieldIR(
                            "sync_predefined_type",
                            TypeSpec(kind="bool"),
                            "bool",
                        ),
                    ),
                )
            },
            functions={
                "ifcopenshell_aggregate_assign_object": _make_function(
                    c_name="ifcopenshell_aggregate_assign_object",
                    params=(
                        CParamIR("file", "ifcopenshell_file_t*", "param", "handle"),
                        CParamIR(
                            "options",
                            "const ifcopenshell_aggregate_assign_object_options_t*",
                            "param",
                            "option",
                        ),
                    ),
                    returns=TypeSpec(kind="handle", handle="instance"),
                ),
                "ifcopenshell_alpha_lookup": _make_function(
                    c_name="ifcopenshell_alpha_lookup",
                    params=(CParamIR("value", "const char*", "param", "string"),),
                ),
                "ifcopenshell_beta_lookup": _make_function(
                    c_name="ifcopenshell_beta_lookup",
                    params=(CParamIR("value", "const char*", "param", "string"),),
                ),
                "ifcopenshell_demo_use": _make_function(
                    c_name="ifcopenshell_demo_use",
                    params=(CParamIR("value", "const char*", "param", "string"),),
                ),
                "ifcopenshell_guid_generate": _make_function(
                    c_name="ifcopenshell_guid_generate",
                    returns=TypeSpec(kind="string"),
                ),
            },
        )

        code = render_api_direct(metadata)
        raw_start = code.index("type RawApi = {")
        raw_end = code.index("export interface", raw_start)
        raw_api = code[raw_start:raw_end]

        assert "type RawFn" not in code
        assert "generate: () => string;" in raw_api
        assert "use: (value: string) => void;" in raw_api
        assert (
            "assignObject: (file: RawValue, options: RawValue) => RawValue;" in raw_api
        )
        assert "  alpha: {\n    lookup: (value: string) => void;\n  };" in raw_api
        assert "  beta: {\n    lookup: (value: string) => void;\n  };" in raw_api
        assert "[key: string]" not in raw_api
        assert "...args" not in raw_api
        assert "raw.guid.generate();" in code
        assert "raw.demo.use(value);" in code
        assert "raw.aggregate.assignObject(file.raw, encodeOptions(options" in code
        assert "raw.alpha.lookup(value);" in code
        assert "raw.beta.lookup(value);" in code
        low_level = render_typescript_declarations(metadata)
        assert "owner_history?: IfcOpenshellInstance;" in low_level
        assert "relating_object: IfcOpenshellInstance;" in low_level
        assert "sync_predefined_type: boolean;" in low_level
        assert "ownerHistory?: Entity;" in code
        assert "relatingObject: Entity;" in code
        assert "syncPredefinedType: boolean;" in code
        assert '"ownerHistory": "owner_history"' in code
        assert '"relatingObject": "relating_object"' in code
        assert '"syncPredefinedType": "sync_predefined_type"' in code

    def test_direct_api_facade_renders_field_documentation(self):
        metadata = _make_metadata(
            c_prefix="ifcopenshell",
            option_structs={
                "DemoOptions": COptionIR(
                    name="DemoOptions",
                    c_type="ifcopenshell_demo_options_t",
                    fields=(
                        COptionFieldIR(
                            "enabled",
                            TypeSpec(kind="bool", nullable=True),
                            "bool",
                            doc="Whether the feature is enabled.",
                        ),
                        COptionFieldIR("label", TypeSpec(kind="string"), "const char*"),
                    ),
                )
            },
            value_types={
                "DemoResult": CTypeIR(
                    c_type="ifcopenshell_demo_result_t",
                    kind="result_struct",
                    fields=(
                        CFieldIR(
                            "value",
                            "double",
                            "Computed result value.\n\nMeasured in model units.",
                        ),
                        CFieldIR("undocumented", "bool"),
                    ),
                    destroy_function=None,
                )
            },
        )

        code = render_api_direct(metadata)

        assert "/** Whether the feature is enabled. */\n  enabled?: boolean;" in code
        assert (
            "/**\n   * Computed result value.\n   *\n   * Measured in model units.\n   */\n  value: number;"
            in code
        )
        assert "  label: string;" in code
        assert "  undocumented: boolean;" in code
        assert "/** */" not in code

    def test_result_record_sequences_have_exact_nested_types_and_lowering(self):
        metadata = _make_metadata(
            c_prefix="ifcopenshell",
            value_types={
                "double_list": CTypeIR(
                    c_type="ifcopenshell_double_list_t",
                    kind="sequence",
                    fields=(CFieldIR("items", "double*"), CFieldIR("size", "size_t")),
                    destroy_function="ifcopenshell_double_list_destroy",
                    element_type="double",
                    sequence_depth=1,
                ),
                "support": CTypeIR(
                    c_type="ifcopenshell_geometry_railing_support_t",
                    kind="result_struct",
                    fields=(CFieldIR("arc_polyline", "ifcopenshell_double_list_t"),),
                    destroy_function="ifcopenshell_geometry_railing_support_destroy",
                ),
                "support_list": CTypeIR(
                    c_type="ifcopenshell_geometry_railing_support_list_t",
                    kind="result_record_sequence",
                    fields=(
                        CFieldIR("items", "ifcopenshell_geometry_railing_support_t*"),
                        CFieldIR("size", "size_t"),
                    ),
                    destroy_function="ifcopenshell_geometry_railing_support_list_destroy",
                    element_type="ifcopenshell_geometry_railing_support_t",
                    sequence_depth=1,
                ),
                "result": CTypeIR(
                    c_type="ifcopenshell_geometry_wall_mounted_handrail_result_t",
                    kind="result_struct",
                    fields=(
                        CFieldIR(
                            "supports", "ifcopenshell_geometry_railing_support_list_t"
                        ),
                    ),
                    destroy_function="ifcopenshell_geometry_wall_mounted_handrail_result_destroy",
                ),
            },
        )

        declarations = render_typescript_declarations(metadata)
        bridge = render_api_direct(metadata)
        glue = render_js_glue(metadata)

        assert "supports: IfcOpenshellGeometryRailingSupport[];" in declarations
        assert "arcPolyline: number[];" in bridge
        assert "supports: IfcOpenShellGeometryRailingSupport[];" in bridge
        assert "supports: any" not in bridge
        assert "case 'result_record_sequence': return _readSequenceValue" in glue

    def test_direct_api_facade_wraps_option_handles(self):
        metadata = _make_metadata(
            c_prefix="ifcopenshell",
            handles={
                "file": _make_handle("ifcopenshell_file_t"),
                "instance": _make_handle("ifcopenshell_instance_t"),
            },
            option_structs={
                "CreateEntityOptions": COptionIR(
                    name="CreateEntityOptions",
                    c_type="ifcopenshell_root_create_entity_options_t",
                    fields=(
                        COptionFieldIR(
                            "ifc_class", TypeSpec(kind="string"), "const char*"
                        ),
                        COptionFieldIR(
                            "owner_history",
                            TypeSpec(kind="handle", handle="instance", nullable=True),
                            "ifcopenshell_instance_t*",
                        ),
                    ),
                )
            },
            functions={
                "ifcopenshell_root_create_entity": _make_function(
                    c_name="ifcopenshell_root_create_entity",
                    params=(
                        CParamIR("file", "ifcopenshell_file_t*", "param", "handle"),
                        CParamIR(
                            "options",
                            "const ifcopenshell_root_create_entity_options_t*",
                            "param",
                            "option",
                        ),
                    ),
                    returns=TypeSpec(kind="handle", handle="instance"),
                )
            },
        )

        code = render_api_direct(metadata)

        assert "ApiHandle" not in code
        assert "ApiTransport" not in code
        assert "apiWorkerDispatch" not in code
        assert "if (value instanceof Entity) return value.raw;" in code
        assert "raw.root.createEntity(file.raw, encodeOptions(options" in code

    def test_direct_api_facade_marshals_sequences_without_collapsing_instance_lists(
        self,
    ):
        metadata = _make_metadata(
            c_prefix="ifcopenshell",
            handles={
                "file": _make_handle("ifcopenshell_file_t"),
                "instance": _make_handle("ifcopenshell_instance_t"),
                "parse_instance_list": _make_handle(
                    "ifcopenshell_parse_instance_list_t"
                ),
            },
            value_types={
                "instance_list": CTypeIR(
                    c_type="ifcopenshell_instance_list_t",
                    kind="handle_sequence",
                    fields=(
                        CFieldIR("items", "ifcopenshell_instance_t**"),
                        CFieldIR("size", "size_t"),
                    ),
                    destroy_function="ifcopenshell_instance_list_destroy",
                    element_type="ifcopenshell_instance_t",
                    sequence_depth=1,
                ),
                "double_list": CTypeIR(
                    c_type="ifcopenshell_double_list_t",
                    kind="sequence",
                    fields=(
                        CFieldIR("items", "double*"),
                        CFieldIR("size", "size_t"),
                    ),
                    destroy_function="ifcopenshell_double_list_destroy",
                    element_type="double",
                    sequence_depth=1,
                ),
                "double_list_list": CTypeIR(
                    c_type="ifcopenshell_double_list_list_t",
                    kind="sequence",
                    fields=(
                        CFieldIR("items", "ifcopenshell_double_list_t*"),
                        CFieldIR("size", "size_t"),
                    ),
                    destroy_function="ifcopenshell_double_list_list_destroy",
                    element_type="ifcopenshell_double_list_t",
                    sequence_depth=2,
                ),
            },
            option_structs={
                "CreateOptions": COptionIR(
                    name="CreateOptions",
                    c_type="ifcopenshell_demo_create_options_t",
                    fields=(
                        COptionFieldIR(
                            "products",
                            TypeSpec(
                                kind="handle", handle="instance", sequence_depth=1
                            ),
                            "ifcopenshell_instance_list_t",
                        ),
                    ),
                )
            },
            functions={
                "ifcopenshell_demo_assign": _make_function(
                    c_name="ifcopenshell_demo_assign",
                    params=(
                        CParamIR("file", "ifcopenshell_file_t*", "param", "handle"),
                        CParamIR(
                            "products",
                            "const ifcopenshell_instance_list_t*",
                            "param",
                            "handle",
                        ),
                    ),
                ),
                "ifcopenshell_demo_set_axis": _make_function(
                    c_name="ifcopenshell_demo_set_axis",
                    params=(
                        CParamIR(
                            "axis",
                            "const ifcopenshell_double_list_list_t*",
                            "param",
                            "double",
                        ),
                    ),
                ),
                "ifcopenshell_demo_use_opaque": _make_function(
                    c_name="ifcopenshell_demo_use_opaque",
                    params=(
                        CParamIR(
                            "items",
                            "ifcopenshell_parse_instance_list_t*",
                            "param",
                            "handle",
                        ),
                    ),
                ),
                "ifcopenshell_demo_create": _make_function(
                    c_name="ifcopenshell_demo_create",
                    params=(
                        CParamIR(
                            "options",
                            "const ifcopenshell_demo_create_options_t*",
                            "param",
                            "option",
                        ),
                    ),
                ),
            },
        )

        code = render_api_direct(metadata)

        assert "assign(file: IfcFile, products: Entity[]): void;" in code
        assert "setAxis(axis: number[][]): void;" in code
        assert "create(options: IfcOpenShellDemoCreateOptions): void;" in code
        assert (
            "raw.demo.assign(file.raw, toRawSequence(products, shell, temps));" in code
        )
        assert "raw.demo.setAxis(toRawSequence(axis, shell, temps));" in code
        assert "raw.demo.useOpaque(toRaw(items, shell, temps));" in code
        assert "raw.demo.create(encodeOptions(options" in code
        assert (
            "function toRawSequence(value: ApiInput, shell: IfcOpenShell, temps: Disposable[]): RawValue"
            in code
        )
        assert (
            "if (Array.isArray(value)) return value.map((item) => toRawSequence(item, shell, temps));"
            in code
        )
        assert "if (isEntityArray(value)) {" in code
        assert "instanceListCreateFromHandles" in code
        assert "raw.demo.assign(file.raw, toRaw(products, shell, temps))" not in code

    def test_direct_api_facade_maps_pset_properties(self):
        metadata = _make_metadata(
            c_prefix="ifcopenshell",
            handles={
                "file": _make_handle("ifcopenshell_file_t"),
                "instance": _make_handle("ifcopenshell_instance_t"),
            },
            option_structs={
                "PsetEditPsetOptions": COptionIR(
                    name="PsetEditPsetOptions",
                    c_type="ifcopenshell_pset_edit_pset_options_t",
                    fields=(
                        COptionFieldIR(
                            "pset",
                            TypeSpec(kind="handle", handle="instance"),
                            "ifcopenshell_instance_t*",
                        ),
                        COptionFieldIR(
                            "properties",
                            TypeSpec(kind="opaque_ptr", semantic="property_map"),
                            "void*",
                        ),
                    ),
                )
            },
            functions={
                "ifcopenshell_pset_edit_pset": _make_function(
                    c_name="ifcopenshell_pset_edit_pset",
                    params=(
                        CParamIR("file", "ifcopenshell_file_t*", "param", "handle"),
                        CParamIR(
                            "options",
                            "const ifcopenshell_pset_edit_pset_options_t*",
                            "param",
                            "option",
                        ),
                    ),
                    returns=TypeSpec(kind="bool"),
                ),
                "ifcopenshell_pset_props_new": _make_function(
                    c_name="ifcopenshell_pset_props_new",
                    returns=TypeSpec(kind="int32"),
                ),
                "ifcopenshell_pset_props_free": _make_function(
                    c_name="ifcopenshell_pset_props_free"
                ),
                "ifcopenshell_pset_props_set_string": _make_function(
                    c_name="ifcopenshell_pset_props_set_string"
                ),
            },
        )

        code = render_api_direct(metadata)

        assert "properties: PsetProperties | PsetInput;" in code
        assert "propsNew" not in code
        assert (
            "return toRawPsetProperties(shell, value as PsetProperties | PsetInput, temps);"
            in code
        )

    def test_direct_api_facade_wraps_pset_template_handles(self):
        metadata = _make_metadata(
            c_prefix="ifcopenshell",
            handles={
                "file": _make_handle("ifcopenshell_file_t"),
                "instance": _make_handle("ifcopenshell_instance_t"),
                "pset_template_handle": _make_handle(
                    "ifcopenshell_pset_template_handle_t"
                ),
            },
            value_types={
                "file_list": CTypeIR(
                    c_type="ifcopenshell_file_list_t",
                    kind="handle_sequence",
                    fields=(
                        CFieldIR("items", "ifcopenshell_file_t**"),
                        CFieldIR("size", "size_t"),
                    ),
                    destroy_function=None,
                    element_type="ifcopenshell_file_t",
                    sequence_depth=1,
                ),
            },
            functions={
                "ifcopenshell_pset_template_create_from_files": _make_function(
                    c_name="ifcopenshell_pset_template_create_from_files",
                    params=(
                        CParamIR("schema_identifier", "const char*", "param", "string"),
                        CParamIR(
                            "template_files",
                            "const ifcopenshell_file_list_t*",
                            "param",
                            "handle",
                        ),
                    ),
                    returns=TypeSpec(
                        kind="handle",
                        handle="pset_template_handle",
                        ownership="owned",
                        nullable=True,
                    ),
                ),
                "ifcopenshell_pset_template_get_template": _make_function(
                    c_name="ifcopenshell_pset_template_get_template",
                    params=(
                        CParamIR("schema_identifier", "const char*", "param", "string"),
                    ),
                    returns=TypeSpec(
                        kind="handle",
                        handle="pset_template_handle",
                        ownership="borrowed",
                        nullable=True,
                    ),
                ),
                "ifcopenshell_pset_template_get_by_name": _make_function(
                    c_name="ifcopenshell_pset_template_get_by_name",
                    params=(
                        CParamIR(
                            "pqt",
                            "ifcopenshell_pset_template_handle_t*",
                            "param",
                            "handle",
                        ),
                        CParamIR("name", "const char*", "param", "string"),
                    ),
                    returns=TypeSpec(kind="handle", handle="instance", nullable=True),
                ),
                "ifcopenshell_pset_template_free": _make_function(
                    c_name="ifcopenshell_pset_template_free",
                    params=(
                        CParamIR(
                            "pqt",
                            "ifcopenshell_pset_template_handle_t*",
                            "param",
                            "handle",
                        ),
                    ),
                ),
            },
        )

        code = render_api_direct(metadata)

        assert "export interface PsetTemplate" in code
        assert (
            "templateCreateFromFiles(schema_identifier: string, template_files: IfcFile[]): PsetTemplate | null;"
            in code
        )
        assert (
            "templateGetTemplate(schema_identifier: string): PsetTemplate | null;"
            in code
        )
        assert (
            "templateGetByName(pqt: PsetTemplate, name: string): Entity | null;" in code
        )
        assert "templateFree" not in code
        assert "free(pqt" not in code
        assert (
            "const result = raw.pset.templateCreateFromFiles(schema_identifier, toRawSequence(template_files, shell, temps));"
            in code
        )
        assert "return result as PsetTemplate | null;" in code
        assert "const result = raw.pset.templateGetByName(pqt, name);" in code
        assert "pqt.raw" not in code
        assert "PsetTemplate | null" in code
        assert "number | null" not in code

    def test_direct_api_facade_maps_pset_props_by_c_type(self):
        metadata = _make_metadata(
            c_prefix="ifcopenshell",
            handles={
                "file": _make_handle("ifcopenshell_file_t"),
                "instance": _make_handle("ifcopenshell_instance_t"),
            },
            option_structs={
                "AttributeEditAttributesOptions": COptionIR(
                    name="AttributeEditAttributesOptions",
                    c_type="ifcopenshell_attribute_edit_attributes_options_t",
                    fields=(
                        COptionFieldIR(
                            "product",
                            TypeSpec(kind="handle", handle="instance"),
                            "ifcopenshell_instance_t*",
                        ),
                        COptionFieldIR(
                            "attributes",
                            TypeSpec(kind="opaque_ptr", semantic="property_map"),
                            "void*",
                        ),
                    ),
                ),
                "GeoreferenceEditGeoreferencingOptions": COptionIR(
                    name="GeoreferenceEditGeoreferencingOptions",
                    c_type="ifcopenshell_georeference_edit_georeferencing_options_t",
                    fields=(
                        COptionFieldIR(
                            "coordinate_operation",
                            TypeSpec(
                                kind="opaque_ptr",
                                nullable=True,
                                semantic="property_map",
                            ),
                            "void*",
                        ),
                        COptionFieldIR(
                            "projected_crs",
                            TypeSpec(
                                kind="opaque_ptr",
                                nullable=True,
                                semantic="property_map",
                            ),
                            "void*",
                        ),
                    ),
                ),
            },
            functions={
                "ifcopenshell_attribute_edit_attributes": _make_function(
                    c_name="ifcopenshell_attribute_edit_attributes",
                    params=(
                        CParamIR("file", "ifcopenshell_file_t*", "param", "handle"),
                        CParamIR(
                            "options",
                            "const ifcopenshell_attribute_edit_attributes_options_t*",
                            "param",
                            "option",
                        ),
                    ),
                    returns=TypeSpec(kind="bool"),
                ),
                "ifcopenshell_georeference_edit_georeferencing": _make_function(
                    c_name="ifcopenshell_georeference_edit_georeferencing",
                    params=(
                        CParamIR("file", "ifcopenshell_file_t*", "param", "handle"),
                        CParamIR(
                            "options",
                            "const ifcopenshell_georeference_edit_georeferencing_options_t*",
                            "param",
                            "option",
                        ),
                    ),
                    returns=TypeSpec(kind="bool"),
                ),
            },
        )

        code = render_api_direct(metadata)

        assert "attributes: PsetProperties | PsetInput;" in code
        assert "coordinateOperation?: PsetProperties | PsetInput;" in code
        assert "projectedCrs?: PsetProperties | PsetInput;" in code
        assert "attributes: number" not in code
        assert "coordinateOperation?: number" not in code
        assert "projectedCrs?: number" not in code
        assert (
            "return toRawPsetProperties(shell, value as PsetProperties | PsetInput, temps);"
            in code
        )
        assert "propsFree" not in code
        assert "propsSetString" not in code

    def test_direct_api_facade_maps_property_maps_by_semantic(self):
        metadata = _make_metadata(
            c_prefix="ifcopenshell",
            handles={
                "file": _make_handle("ifcopenshell_file_t"),
                "instance": _make_handle("ifcopenshell_instance_t"),
            },
            functions={
                "ifcopenshell_resource_edit_resource_time": _make_function(
                    c_name="ifcopenshell_resource_edit_resource_time",
                    params=(
                        CParamIR("file", "ifcopenshell_file_t*", "param", "handle"),
                        CParamIR(
                            "resource_time",
                            "ifcopenshell_instance_t*",
                            "param",
                            "handle",
                        ),
                        CParamIR(
                            "attributes",
                            "arbitrary_property_builder_t*",
                            "param",
                            "opaque_ptr",
                            semantic="property_map",
                        ),
                    ),
                ),
                "ifcopenshell_demo_use_opaque": _make_function(
                    c_name="ifcopenshell_demo_use_opaque",
                    params=(
                        CParamIR(
                            "value",
                            "arbitrary_property_builder_t*",
                            "param",
                            "opaque_ptr",
                        ),
                    ),
                ),
                "ifcopenshell_demo_use_dynamic": _make_function(
                    c_name="ifcopenshell_demo_use_dynamic",
                    params=(CParamIR("value", "void*", "param", "opaque_ptr"),),
                ),
                "ifcopenshell_demo_use_nullable": _make_function(
                    c_name="ifcopenshell_demo_use_nullable",
                    params=(
                        CParamIR(
                            "attributes",
                            "arbitrary_property_builder_t*",
                            "param",
                            "opaque_ptr",
                            nullable=True,
                            semantic="property_map",
                        ),
                    ),
                ),
            },
        )

        code = render_api_direct(metadata)

        assert (
            "editResourceTime(file: IfcFile, resource_time: Entity, attributes: PsetProperties | PsetInput): void;"
            in code
        )
        assert (
            "raw.resource.editResourceTime(file.raw, resource_time.raw, toRawPsetProperties(shell, attributes as PsetProperties | PsetInput, temps));"
            in code
        )
        assert "useOpaque(value: ApiData): void;" in code
        assert "raw.demo.useOpaque(value);" in code
        assert "useDynamic(value: ApiData): void;" in code
        assert "raw.demo.useDynamic(value);" in code
        assert (
            "useNullable(attributes: PsetProperties | PsetInput | null): void;" in code
        )
        assert (
            "raw.demo.useNullable(attributes == null ? null : toRawPsetProperties(shell, attributes as PsetProperties | PsetInput, temps));"
            in code
        )
        assert "raw.demo.useOpaque(toRawPsetProperties" not in code

    def test_direct_api_facade_hides_selector_internals(self):
        metadata = _make_metadata(
            c_prefix="ifcopenshell",
            handles={"file": _make_handle("ifcopenshell_file_t")},
            functions={
                "ifcopenshell_selector_filter_all": _make_function(
                    c_name="ifcopenshell_selector_filter_all",
                    params=(
                        CParamIR("file", "ifcopenshell_file_t*", "param", "handle"),
                        CParamIR("query", "const char*", "param", "string"),
                    ),
                    returns=TypeSpec(kind="handle", handle="value"),
                ),
                "ifcopenshell_selector_parse_keys": _make_function(
                    c_name="ifcopenshell_selector_parse_keys",
                    params=(CParamIR("query", "const char*", "param", "string"),),
                    returns=TypeSpec(kind="int32", nullable=True),
                ),
            },
        )

        code = render_api_direct(metadata)

        assert "filterAll(file: IfcFile, query: string)" in code
        assert "parseKeys" not in code

    def test_direct_api_facade_excludes_value_module(self):
        metadata = _make_metadata(
            c_prefix="ifcopenshell",
            handles={
                "file": _make_handle("ifcopenshell_file_t"),
                "value": _make_handle("ifcopenshell_value_t"),
            },
            functions={
                "ifcopenshell_value_new_string": _make_function(
                    c_name="ifcopenshell_value_new_string",
                    params=(CParamIR("value", "const char*", "param", "string"),),
                    returns=TypeSpec(kind="handle", handle="value"),
                ),
                "ifcopenshell_value_new_list": _make_function(
                    c_name="ifcopenshell_value_new_list",
                    returns=TypeSpec(kind="handle", handle="value"),
                ),
                "ifcopenshell_value_new_dict": _make_function(
                    c_name="ifcopenshell_value_new_dict",
                    returns=TypeSpec(kind="handle", handle="value"),
                ),
                "ifcopenshell_value_list_append": _make_function(
                    c_name="ifcopenshell_value_list_append",
                    params=(
                        CParamIR("list", "ifcopenshell_value_t*", "param", "handle"),
                        CParamIR("item", "ifcopenshell_value_t*", "param", "handle"),
                    ),
                    returns=TypeSpec(kind="bool"),
                ),
                "ifcopenshell_value_dict_set": _make_function(
                    c_name="ifcopenshell_value_dict_set",
                    params=(
                        CParamIR("dict", "ifcopenshell_value_t*", "param", "handle"),
                        CParamIR("key", "const char*", "param", "string"),
                        CParamIR("value", "ifcopenshell_value_t*", "param", "handle"),
                    ),
                    returns=TypeSpec(kind="bool"),
                ),
                "ifcopenshell_selector_filter_all": _make_function(
                    c_name="ifcopenshell_selector_filter_all",
                    params=(
                        CParamIR("file", "ifcopenshell_file_t*", "param", "handle"),
                        CParamIR("query", "const char*", "param", "string"),
                    ),
                    returns=TypeSpec(kind="handle", handle="value"),
                ),
            },
        )

        code = render_api_direct(metadata)

        assert "export interface ValueApi" not in code
        assert "value: ValueApi;" not in code
        assert "newString" not in code
        assert "newList" not in code
        assert "newDict" not in code
        assert "listAppend" not in code
        assert "dictSet" not in code
        assert "filterAll(file: IfcFile, query: string): ValueData | null;" in code

    def test_direct_api_facade_maps_selector_value_params_to_value_input(self):
        metadata = _make_metadata(
            c_prefix="ifcopenshell",
            handles={
                "file": _make_handle("ifcopenshell_file_t"),
                "value": _make_handle("ifcopenshell_value_t"),
            },
            functions={
                "ifcopenshell_selector_set_element_value": _make_function(
                    c_name="ifcopenshell_selector_set_element_value",
                    params=(
                        CParamIR("file", "ifcopenshell_file_t*", "param", "handle"),
                        CParamIR(
                            "element", "ifcopenshell_instance_t*", "param", "handle"
                        ),
                        CParamIR("query", "const char*", "param", "string"),
                        CParamIR("value", "ifcopenshell_value_t*", "param", "handle"),
                        CParamIR("concat", "const char*", "param", "string"),
                    ),
                    returns=TypeSpec(kind="bool"),
                ),
            },
        )

        code = render_api_direct(metadata)

        assert "value: ValueInput" in code
        assert "type ValueInput," in code
        assert "setElementValue(file: IfcFile" in code

    def test_direct_api_facade_maps_nullable_result_structs(self):
        metadata = _make_metadata(
            c_prefix="ifcopenshell",
            handles={
                "file": _make_handle("ifcopenshell_file_t"),
                "instance": _make_handle("ifcopenshell_instance_t"),
            },
            value_types={
                "mep_result": CTypeIR(
                    c_type="ifcopenshell_shape_builder_mep_transition_shape_result_t",
                    kind="result_struct",
                    fields=(
                        CFieldIR("representation", "ifcopenshell_instance_t*"),
                        CFieldIR("start_length", "double"),
                    ),
                    destroy_function=None,
                ),
                "optional_mep_result": CTypeIR(
                    c_type="ifcopenshell_optional_shape_builder_mep_transition_shape_result_t",
                    kind="optional_result_struct",
                    fields=(
                        CFieldIR("has_value", "bool"),
                        CFieldIR(
                            "value",
                            "ifcopenshell_shape_builder_mep_transition_shape_result_t",
                        ),
                    ),
                    destroy_function=None,
                    element_type="mep_result",
                ),
            },
            functions={
                "ifcopenshell_shape_builder_mep_transition_shape": _make_function(
                    c_name="ifcopenshell_shape_builder_mep_transition_shape",
                    params=(
                        CParamIR("file", "ifcopenshell_file_t*", "param", "handle"),
                    ),
                    returns=TypeSpec(kind="struct", struct="mep_result", nullable=True),
                )
            },
        )

        code = render_api_direct(metadata)

        assert (
            "builderMepTransitionShape(file: IfcFile): IfcOpenShellShapeBuilderMepTransitionShapeResult | null;"
            in code
        )
        assert "if (result === null) return null;" in code
        assert (
            "const data = result as { representation: RawValue; start_length: number };"
            in code
        )
        assert "startLength: data.start_length as number" in code
        assert "data.startLength" not in code
        assert "result as any" not in code
        assert "hasResult" not in code

    def test_direct_api_facade_maps_variants_to_unions(self):
        metadata = _make_metadata(
            c_prefix="ifcopenshell",
            handles={
                "file": _make_handle("ifcopenshell_file_t"),
                "instance": _make_handle("ifcopenshell_instance_t"),
            },
            value_types={
                "entity_string_variant": CTypeIR(
                    c_type="ifcopenshell_instance_string_variant_t",
                    kind="variant",
                    fields=(
                        CFieldIR("kind", "int32_t"),
                        CFieldIR("value_0", "ifcopenshell_instance_t*"),
                        CFieldIR("value_1", "ifcopenshell_string_t"),
                    ),
                    destroy_function=None,
                    element_type="std::variant<express::Base, std::string>",
                ),
            },
            functions={
                "ifcopenshell_sequence_add_date_time": _make_function(
                    c_name="ifcopenshell_sequence_add_date_time",
                    params=(
                        CParamIR("file", "ifcopenshell_file_t*", "param", "handle"),
                        CParamIR("date_time", "const char*", "param", "string"),
                    ),
                    returns=TypeSpec(
                        kind="variant",
                        variants=(
                            TypeSpec(
                                kind="handle",
                                handle="instance",
                                cpp_type="express::Base",
                            ),
                            TypeSpec(kind="string", cpp_type="std::string"),
                        ),
                        cpp_type="std::variant<express::Base, std::string>",
                    ),
                )
            },
        )

        code = render_api_direct(metadata)

        assert "addDateTime(file: IfcFile, date_time: string): Entity | string;" in code
        assert "return wrap(shell, result) as Entity | string;" in code
        assert "isEntity:" not in code
        assert "dateTimeString" not in code


class TestWasmBackend:
    def test_render_wasm_bindings_returns_both_outputs(self):
        metadata = _make_metadata(
            handles={"file": _make_handle("ifcopenshell_demo_file_t")}
        )
        javascript, declarations = render_wasm_bindings(metadata)
        assert "createIfcOpenshellModule" in javascript
        assert "IfcOpenshellModule" in declarations

    def test_render_export_list_includes_runtime_and_destroy_helpers(self):
        metadata = _make_metadata(
            handles={"file": _make_handle("ifcopenshell_demo_file_t")},
            value_types={
                "string": CTypeIR(
                    c_type="ifcopenshell_string_t",
                    kind="string",
                    fields=(
                        CFieldIR("data", "char*"),
                        CFieldIR("size", "size_t"),
                        CFieldIR("owned", "bool"),
                    ),
                    destroy_function="ifcopenshell_string_destroy",
                )
            },
            functions={
                "ifcopenshell_demo_open": _make_function(
                    c_name="ifcopenshell_demo_open"
                )
            },
        )
        export_list = render_export_list(metadata)
        assert "_malloc" in export_list
        assert "_free" in export_list
        assert "_ifcopenshell_demo_open" in export_list
        assert "_ifcopenshell_demo_file_destroy" in export_list
        assert "_ifcopenshell_string_destroy" in export_list
