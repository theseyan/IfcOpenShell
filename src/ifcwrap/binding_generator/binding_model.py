# This file was generated with the assistance of an AI coding tool.

from __future__ import annotations

from dataclasses import dataclass


@dataclass(frozen=True)
class TypeSpec:
    kind: str
    handle: str | None = None
    ownership: str | None = None
    nullable: bool = False
    cpp_type: str | None = None
    sequence_depth: int = 0


@dataclass(frozen=True)
class ParamSpec:
    name: str
    type: TypeSpec


@dataclass(frozen=True)
class HandleSpec:
    name: str
    cpp_type: str
    c_type: str
    destructor: str
    ptr_type: str = "raw"


@dataclass(frozen=True)
class ImplementationSpec:
    kind: str
    body: str


@dataclass(frozen=True)
class CallSpec:
    expose_as: str
    c_name: str
    receiver: str | None
    returns: TypeSpec
    params: tuple[ParamSpec, ...]
    policy_operation: object


@dataclass(frozen=True)
class DiscoveryDiagnostic:
    owner: str
    member: str
    code: str
    message: str
