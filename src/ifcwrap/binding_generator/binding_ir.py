# This file was generated with the assistance of an AI coding tool.

from __future__ import annotations

from dataclasses import dataclass
import re
from typing import Union

try:
    from .authored_spec import AuthoredBindingSpec, MergedBindingSpec
    from .binding_model import CallSpec, HandleSpec, ImplementationSpec, ParamSpec, TypeSpec
    from .policy_ir import (
        ArrayElementFieldPolicyOp,
        AsItemCastPolicyOp,
        ChildrenAddPolicyOp,
        ChildrenAtPolicyOp,
        ChildrenCountPolicyOp,
        CcomponentsAccessorPolicyOp,
        ConstructorPolicyOp,
        DirectFieldPolicyOp,
        DirectFunctionPolicyOp,
        DirectMethodPolicyOp,
        FieldSetterPolicyOp,
        InlineAdapterPolicyOp,
        MethodSizePolicyOp,
        OptionalGetPolicyOp,
        OptionalHasPolicyOp,
        PointerPresencePolicyOp,
        ValueHandleFieldPolicyOp,
        VariantGetPolicyOp,
        VariantSetPolicyOp,
    )
    from .debug import debug_log
except ImportError:  # pragma: no cover - script execution fallback
    from authored_spec import AuthoredBindingSpec, MergedBindingSpec
    from binding_model import CallSpec, HandleSpec, ImplementationSpec, ParamSpec, TypeSpec
    from policy_ir import (
        ArrayElementFieldPolicyOp,
        AsItemCastPolicyOp,
        ChildrenAddPolicyOp,
        ChildrenAtPolicyOp,
        ChildrenCountPolicyOp,
        CcomponentsAccessorPolicyOp,
        ConstructorPolicyOp,
        DirectFieldPolicyOp,
        DirectFunctionPolicyOp,
        DirectMethodPolicyOp,
        FieldSetterPolicyOp,
        InlineAdapterPolicyOp,
        MethodSizePolicyOp,
        OptionalGetPolicyOp,
        OptionalHasPolicyOp,
        PointerPresencePolicyOp,
        ValueHandleFieldPolicyOp,
        VariantGetPolicyOp,
        VariantSetPolicyOp,
    )
    from debug import debug_log


SourceBindingSpec = Union[AuthoredBindingSpec, MergedBindingSpec]


@dataclass(frozen=True)
class DirectCallOp:
    cpp_name: str


@dataclass(frozen=True)
class FieldGetOp:
    field_name: str
    null_check: bool
    array_element_cpp_type: str | None = None


@dataclass(frozen=True)
class ValueHandleFieldGetOp:
    field_name: str


@dataclass(frozen=True)
class PointerPresenceCheckOp:
    field_name: str


@dataclass(frozen=True)
class ChildrenCountOp:
    field_name: str


@dataclass(frozen=True)
class ChildrenAtOp:
    field_name: str


@dataclass(frozen=True)
class ChildrenAddOp:
    field_name: str
    cast_cpp_type: str | None


@dataclass(frozen=True)
class FieldSetterOp:
    field_name: str


@dataclass(frozen=True)
class MethodSizeOp:
    method_name: str


@dataclass(frozen=True)
class ArrayElementFieldOp:
    expression: str


@dataclass(frozen=True)
class OptionalPresenceCheckOp:
    field_name: str


@dataclass(frozen=True)
class OptionalGetOp:
    field_name: str


@dataclass(frozen=True)
class StaticCastOp:
    expression: str


@dataclass(frozen=True)
class CcomponentsAccessorOp:
    access_via: str
    dimensions: int


@dataclass(frozen=True)
class VariantGetOp:
    method_name: str
    cpp_type: str
    getter_types: tuple[str, ...]


@dataclass(frozen=True)
class VariantSetOp:
    method_name: str
    variant_type: str
    cpp_type: str


@dataclass(frozen=True)
class ConstructorOp:
    cpp_class: str | None
    compile_guard: str | None


@dataclass(frozen=True)
class InlineImplementationOp:
    implementation: ImplementationSpec


OperationIR = Union[
    DirectCallOp,
    FieldGetOp,
    ValueHandleFieldGetOp,
    PointerPresenceCheckOp,
    ChildrenCountOp,
    ChildrenAtOp,
    ChildrenAddOp,
    FieldSetterOp,
    MethodSizeOp,
    ArrayElementFieldOp,
    OptionalPresenceCheckOp,
    OptionalGetOp,
    StaticCastOp,
    CcomponentsAccessorOp,
    VariantGetOp,
    VariantSetOp,
    ConstructorOp,
    InlineImplementationOp,
]


@dataclass(frozen=True)
class CallIR:
    expose_as: str
    c_name: str
    receiver: str | None
    returns: TypeSpec
    params: tuple[ParamSpec, ...]
    operation: OperationIR


@dataclass(frozen=True)
class BindingIR:
    module: str
    c_prefix: str
    public_headers: tuple[str, ...]
    handles: dict[str, HandleSpec]
    functions: tuple[CallIR, ...]
    methods: tuple[CallIR, ...]
    depends_on_common: str | None = None


def _array_element_cpp_type(cpp_type: str | None) -> str | None:
    if not cpp_type or "std::array<" not in cpp_type:
        return None
    match = re.match(r".*std::array<\s*([^,>]+)", cpp_type)
    if match is None:
        return None
    return match.group(1).strip()


def _lower_policy_operation(call: CallSpec, operation: object) -> OperationIR:
    if isinstance(operation, DirectFunctionPolicyOp | DirectMethodPolicyOp):
        if operation.cpp_name is None:
            raise ValueError(f"{call.c_name} is missing cpp_name")
        return DirectCallOp(cpp_name=operation.cpp_name)
    if isinstance(operation, DirectFieldPolicyOp):
        return FieldGetOp(
            field_name=operation.field_name,
            null_check=call.returns.kind == "handle" and call.returns.sequence_depth == 0,
            array_element_cpp_type=_array_element_cpp_type(call.returns.cpp_type),
        )
    if isinstance(operation, ValueHandleFieldPolicyOp):
        return ValueHandleFieldGetOp(field_name=operation.field_name)
    if isinstance(operation, ConstructorPolicyOp):
        return ConstructorOp(cpp_class=operation.cpp_class, compile_guard=operation.compile_guard)
    if isinstance(operation, InlineAdapterPolicyOp):
        return InlineImplementationOp(implementation=operation.implementation)
    if isinstance(operation, PointerPresencePolicyOp):
        return PointerPresenceCheckOp(field_name=operation.field_name)
    if isinstance(operation, ChildrenCountPolicyOp):
        return ChildrenCountOp(field_name=operation.field_name)
    if isinstance(operation, ChildrenAtPolicyOp):
        return ChildrenAtOp(field_name=operation.field_name)
    if isinstance(operation, ChildrenAddPolicyOp):
        return ChildrenAddOp(field_name=operation.field_name, cast_cpp_type=operation.cast_cpp_type)
    if isinstance(operation, FieldSetterPolicyOp):
        return FieldSetterOp(field_name=operation.field_name)
    if isinstance(operation, MethodSizePolicyOp):
        return MethodSizeOp(method_name=operation.method_name)
    if isinstance(operation, ArrayElementFieldPolicyOp):
        return ArrayElementFieldOp(expression=operation.expression)
    if isinstance(operation, OptionalHasPolicyOp):
        return OptionalPresenceCheckOp(field_name=operation.field_name)
    if isinstance(operation, OptionalGetPolicyOp):
        return OptionalGetOp(field_name=operation.field_name)
    if isinstance(operation, AsItemCastPolicyOp):
        return StaticCastOp(expression="self->ptr")
    if isinstance(operation, CcomponentsAccessorPolicyOp):
        return CcomponentsAccessorOp(access_via=operation.access_via, dimensions=operation.dimensions)
    if isinstance(operation, VariantGetPolicyOp):
        return VariantGetOp(
            method_name=operation.method_name,
            cpp_type=operation.cpp_type,
            getter_types=operation.getter_types,
        )
    if isinstance(operation, VariantSetPolicyOp):
        return VariantSetOp(
            method_name=operation.method_name,
            variant_type=operation.variant_type,
            cpp_type=operation.cpp_type,
        )
    raise ValueError(f"Unsupported policy operation for {call.c_name}")


def lower_call(call: CallSpec) -> CallIR:
    if call.policy_operation is None:
        raise ValueError(f"{call.c_name} is missing typed policy_operation")
    operation = _lower_policy_operation(call, call.policy_operation)

    return CallIR(
        expose_as=call.expose_as,
        c_name=call.c_name,
        receiver=call.receiver,
        returns=call.returns,
        params=call.params,
        operation=operation,
    )


def lower_binding_spec(spec: SourceBindingSpec) -> BindingIR:
    debug_log(
        "binding_ir.lower.start",
        f"module={spec.module} functions={len(spec.functions)} methods={len(spec.methods)} handles={len(spec.handles)}",
    )
    result = BindingIR(
        module=spec.module,
        c_prefix=spec.c_prefix,
        public_headers=spec.public_headers,
        handles=spec.handles,
        functions=tuple(lower_call(call) for call in spec.functions),
        methods=tuple(lower_call(call) for call in spec.methods),
        depends_on_common=getattr(spec, "depends_on_common", None),
    )
    debug_log(
        "binding_ir.lower.done",
        f"module={result.module} functions={len(result.functions)} methods={len(result.methods)} handles={len(result.handles)}",
    )
    return result
