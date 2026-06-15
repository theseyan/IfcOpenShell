# This file was generated with the assistance of an AI coding tool.

from __future__ import annotations

from typing import TYPE_CHECKING

if TYPE_CHECKING:
    from ..host_metadata import HostFunctionMetadata, HostParamMetadata

_PREFIXES = tuple(
    sorted(
        (
            "ifcopenshell_ifcparse_",
            "ifcopenshell_ifcapi_",
            "ifcopenshell_ifc_",
            "ifcopenshell_ifc",
        ),
        key=len,
        reverse=True,
    )
)


def _snake_name(c_type: str) -> str:
    return c_type.removeprefix("ifcopenshell_").removesuffix("_t")


def _type_name(c_type: str) -> str:
    return "IfcOpenshell" + "".join(part.capitalize() for part in _snake_name(c_type).split("_") if part)


def _method_name(c_name: str, c_prefix: str) -> str:
    for prefix in _PREFIXES:
        if c_name.startswith(prefix):
            return c_name[len(prefix) :]
    return c_name.removeprefix(f"{c_prefix}_")


def _camel_name(name: str) -> str:
    parts = [part for part in name.split("_") if part]
    if not parts:
        return name
    return parts[0] + "".join(part.capitalize() for part in parts[1:])


def _public_name(function: HostFunctionMetadata, c_prefix: str) -> str:
    name = _method_name(function.c_name, c_prefix)
    if function.receiver is not None:
        name = name.removeprefix(f"{function.receiver}_")
    return _camel_name(name)


def _public_params(function: HostFunctionMetadata) -> tuple[HostParamMetadata, ...]:
    return tuple(param for param in function.params if param.role == "param")


_INTERNAL_C_FUNCTIONS = frozenset({
    "ifcopenshell_ifcparse_set_plugin_search_paths",
    "ifcopenshell_ifcparse_clear_plugin_search_paths",
    "ifcopenshell_ifcgeom_plugin_is_loaded",
    "ifcopenshell_ifcgeom_plugin_load",
})


__all__ = ["_INTERNAL_C_FUNCTIONS", "_camel_name", "_method_name", "_public_name", "_public_params", "_snake_name", "_type_name"]
