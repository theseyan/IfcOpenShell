# IfcOpenShell - IFC toolkit and geometry engine
# Copyright (C) 2021 Dion Moult <dion@thinkmoult.com>
#
# This file is part of IfcOpenShell.
#
# IfcOpenShell is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# IfcOpenShell is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with IfcOpenShell.  If not, see <http://www.gnu.org/licenses/>.

import pathlib
import re
import ctypes
from functools import lru_cache
from typing import Literal, NamedTuple, Optional, Union

import ifcopenshell
import ifcopenshell.ifcopenshell_wrapper as W
from ifcopenshell import _generated_capi
import ifcopenshell.util.schema
import ifcopenshell.util.type
from ifcopenshell.entity_instance import entity_instance

templates: dict[ifcopenshell.util.schema.IFC_SCHEMA, "PsetQto"] = {}
_BOUND = False


def _get_lib() -> ctypes.CDLL:
    global _BOUND
    lib = ifcopenshell._get_lib()
    if not _BOUND:
        _generated_capi.bind(
            lib,
            names=(
                "ifcopenshell_ifcapi_pset_template_get_applicable",
                "ifcopenshell_ifcapi_pset_template_get_applicable_names",
                "ifcopenshell_ifcapi_pset_template_get_by_name",
                "ifcopenshell_ifcapi_pset_template_create_from_files",
                "ifcopenshell_ifcapi_pset_template_free",
                "ifcopenshell_ifcapi_pset_template_get_template",
                "ifcopenshell_ifcapi_pset_template_is_templated",
                "ifcopenshell_ifcapi_pset_template_pset_type",
                "ifcopenshell_ifc_file_list_destroy",
                "ifcopenshell_ifc_instance_destroy",
                "ifcopenshell_ifc_instance_file_pointer",
                "ifcopenshell_ifc_instance_list_destroy",
                "ifcopenshell_string_list_destroy",
                "ifcopenshell_last_error_kind",
                "ifcopenshell_last_error_message",
            ),
        )
        _BOUND = True
    return lib


def _encode_optional(value: str | None):
    return _generated_capi.encode_string(value) if value else None


def _template_file_for_handle(
    handle: int, template_files_by_ptr: dict[int, ifcopenshell.file] | None = None
) -> ifcopenshell.file:
    ptr = ifcopenshell._instance_file_ptr(handle)
    if template_files_by_ptr and ptr in template_files_by_ptr:
        return template_files_by_ptr[ptr]
    return ifcopenshell._borrow_file_ptr(ptr)


def _wrap_template_instance(
    handle: int | None, template_files_by_ptr: dict[int, ifcopenshell.file] | None = None
) -> Optional[entity_instance]:
    if not handle:
        return None
    return entity_instance(_template_file_for_handle(handle, template_files_by_ptr), handle)


def _file_list_arg(files: list[ifcopenshell.file]) -> _generated_capi.ifcopenshell_ifc_file_list_t:
    handles = [
        ctypes.cast(ctypes.c_void_p(file._ptr), ctypes.POINTER(_generated_capi._HandleStruct))
        for file in files
    ]
    items = (ctypes.POINTER(_generated_capi._HandleStruct) * len(handles))(*handles)
    result = _generated_capi.ifcopenshell_ifc_file_list_t()
    result.items = items
    result.size = len(items)
    result._keepalive = (items, handles)  # type: ignore[attr-defined]
    return result


def _take_template_instances(
    value: _generated_capi.ifcopenshell_ifc_instance_list_t,
    template_files_by_ptr: dict[int, ifcopenshell.file] | None = None,
) -> list[entity_instance]:
    lib = _get_lib()
    handles = _generated_capi.move_handle_list(
        lib,
        value,
        lib.ifcopenshell_ifc_instance_list_destroy,
        ctypes.POINTER(_generated_capi.ifcopenshell_ifc_instance_t),
    )
    result = []
    for handle in handles:
        if not handle:
            continue
        handle_value = ctypes.cast(handle, ctypes.c_void_p).value
        result.append(entity_instance(_template_file_for_handle(handle_value, template_files_by_ptr), handle_value))
    return result


def get_template(schema_identiier: str) -> "PsetQto":
    """
    :param schema_identiier: As in ``file.schema_identifier``, not ``file.schema``.
    """
    global templates
    schema = ifcopenshell.util.schema.get_fallback_schema(schema_identiier)
    if schema not in templates:
        templates[schema] = PsetQto(schema)
    return templates[schema]


class PsetQto:
    # fmt: off
    templates_path: dict[ifcopenshell.util.schema.IFC_SCHEMA, str] = {
        "IFC2X3": "Pset_IFC2X3.ifc",
        "IFC4": "Pset_IFC4_ADD2.ifc",
        "IFC4X3": "Pset_IFC4X3.ifc"
    }
    # fmt: on
    templates: list[ifcopenshell.file]

    def __init__(
        self,
        schema: ifcopenshell.util.schema.IFC_SCHEMA,
        templates: Optional[list[ifcopenshell.file]] = None,
    ) -> None:
        self.schema = ifcopenshell.schema_by_name(schema)
        self._native_ptr: ctypes.c_void_p | None = None
        self._owns_native_ptr = False
        self._template_keepalive: tuple[ifcopenshell.file, ...] = ()
        self._template_files_by_ptr: dict[int, ifcopenshell.file] = {}
        if not templates:
            lib = _get_lib()
            native_ptr = ctypes.c_void_p()
            _generated_capi.status_or_raise(
                lib,
                lib.ifcopenshell_ifcapi_pset_template_get_template(
                    _generated_capi.encode_string(schema),
                    ctypes.byref(native_ptr),
                ),
                ifcopenshell.get_log() or "ifcopenshell_ifcapi_pset_template_get_template",
            )
            self._native_ptr = native_ptr
            self.templates = []
        else:
            lib = _get_lib()
            native_ptr = ctypes.c_void_p()
            template_files = _file_list_arg(templates)
            _generated_capi.status_or_raise(
                lib,
                lib.ifcopenshell_ifcapi_pset_template_create_from_files(
                    _generated_capi.encode_string(schema),
                    ctypes.byref(template_files),
                    ctypes.byref(native_ptr),
                ),
                ifcopenshell.get_log() or "ifcopenshell_ifcapi_pset_template_create_from_files",
            )
            self._native_ptr = native_ptr
            self._owns_native_ptr = True
            self._template_keepalive = tuple(templates)
            self.templates = templates
            self._template_files_by_ptr = {template.file_pointer(): template for template in self._template_keepalive}

    def __del__(self) -> None:
        if self._native_ptr is None or not self._owns_native_ptr:
            return
        try:
            _get_lib().ifcopenshell_ifcapi_pset_template_free(self._native_ptr)
        except Exception:
            pass

    @lru_cache
    def get_applicable(
        self,
        ifc_class="",
        predefined_type="",
        pset_only=False,
        qto_only=False,
        schema: ifcopenshell.util.schema.IFC_SCHEMA = "IFC4",
    ) -> list[entity_instance]:
        """Get applicable property set templates."""
        if self._native_ptr is not None:
            lib = _get_lib()
            out = _generated_capi.ifcopenshell_ifc_instance_list_t()
            _generated_capi.status_or_raise(
                lib,
                lib.ifcopenshell_ifcapi_pset_template_get_applicable(
                    self._native_ptr,
                    _encode_optional(ifc_class),
                    _encode_optional(predefined_type),
                    bool(pset_only),
                    bool(qto_only),
                    _encode_optional(schema),
                    ctypes.byref(out),
                ),
                ifcopenshell.get_log() or "ifcopenshell_ifcapi_pset_template_get_applicable",
            )
            return _take_template_instances(out, self._template_files_by_ptr)

        any_class = not ifc_class
        entity = None
        if not any_class:
            entity = self.schema.declaration_by_name(ifc_class).as_entity()
            assert entity
        result = []
        for template in self.templates:
            for prop_set in template.by_type("IfcPropertySetTemplate"):
                if pset_only:
                    if prop_set.TemplateType and prop_set.TemplateType.startswith("QTO_"):
                        continue
                if qto_only:
                    if prop_set.TemplateType and prop_set.TemplateType.startswith("PSET_"):
                        continue
                if any_class or (
                    entity
                    and self.is_applicable(
                        entity, prop_set.ApplicableEntity or "IfcRoot", predefined_type, prop_set.TemplateType, schema
                    )
                ):
                    result.append(prop_set)
        return result

    @lru_cache
    def get_applicable_names(
        self,
        ifc_class: str,
        predefined_type: str = "",
        pset_only: bool = False,
        qto_only: bool = False,
        schema: ifcopenshell.util.schema.IFC_SCHEMA = "IFC4",
    ) -> list[str]:
        """Return names instead of objects for other use eg. enum"""
        if self._native_ptr is not None:
            lib = _get_lib()
            return list(
                _generated_capi.call_string_list_or_raise(
                    lib,
                    lib.ifcopenshell_ifcapi_pset_template_get_applicable_names,
                    ifcopenshell.get_log() or "ifcopenshell_ifcapi_pset_template_get_applicable_names",
                    self._native_ptr,
                    _encode_optional(ifc_class),
                    _encode_optional(predefined_type),
                    bool(pset_only),
                    bool(qto_only),
                    _encode_optional(schema),
                )
            )
        return [
            prop_set.Name for prop_set in self.get_applicable(ifc_class, predefined_type, pset_only, qto_only, schema)
        ]

    def is_applicable(
        self,
        entity: W.entity,
        applicables: str,
        predefined_type: str = "",
        template_type: str = "NOTDEFINED",
        schema: ifcopenshell.util.schema.IFC_SCHEMA = "IFC4",
    ) -> bool:
        """

        applicables can have multiple possible patterns :

        .. code-block:: text

            IfcBoilerType                               (IfcClass)
            IfcBoilerType/STEAM                         (IfcClass/PREDEFINEDTYPE)
            IfcBoilerType[PerformanceHistory]           (IfcClass[PerformanceHistory])
            IfcBoilerType/STEAM[PerformanceHistory]     (IfcClass/PREDEFINEDTYPE[PerformanceHistory])

        """
        for applicable in applicables.split(","):
            match = re.match(r"(\w+)(\[\w+\])*/*(\w+)*(\[\w+\])*", applicable)
            if not match:
                continue
            # Uncomment if usage found
            # applicable_perf_history = match.group(2) or match.group(4)
            matched_type = match.group(3)
            if matched_type and not predefined_type:
                continue
            # Case insensitive to handle things like material categories
            elif matched_type and predefined_type.lower() != match.group(3).lower():
                continue

            applicable_class = match.group(1)
            if ifcopenshell.util.schema.is_a(entity, applicable_class):
                return True
            # There is an implementer agreement that if the template type is
            # type based, the type need not be explicitly mentioned
            # https://github.com/buildingSMART/IFC4.3.x-development/issues/22
            # This will be fixed in IFC4.3
            template_type = template_type or ""
            if "TYPE" in template_type and ifcopenshell.util.schema.is_a(entity, "IfcTypeObject"):
                types = ifcopenshell.util.type.get_applicable_types(applicable_class, schema)
                if not types:
                    # Abstract classes will not have an "applicable type" but
                    # the implementer agreement still applies to them.
                    occurrence_class = None
                    try:
                        occurrence_class = self.schema.declaration_by_name(applicable_class + "Type")
                    except:
                        try:
                            occurrence_class = self.schema.declaration_by_name("IfcType" + applicable_class[3:])
                        except:
                            pass
                    if occurrence_class:
                        types = [occurrence_class.name()]
                for ifc_type in types:
                    if ifcopenshell.util.schema.is_a(entity, ifc_type):
                        return True
        return False

    @lru_cache
    def get_by_name(self, name: str) -> Optional[entity_instance]:
        if self._native_ptr is not None:
            lib = _get_lib()
            handle = _generated_capi.call_handle_or_raise(
                lib,
                lib.ifcopenshell_ifcapi_pset_template_get_by_name,
                ifcopenshell.get_log() or "ifcopenshell_ifcapi_pset_template_get_by_name",
                self._native_ptr,
                _generated_capi.encode_string(name),
                destroy=lib.ifcopenshell_ifc_instance_destroy,
                handle_pointer_type=ctypes.POINTER(_generated_capi.ifcopenshell_ifc_instance_t),
            )
            return _wrap_template_instance(handle, self._template_files_by_ptr)
        for template in self.templates:
            for prop_set in template.by_type("IfcPropertySetTemplate"):
                if prop_set.Name == name:
                    return prop_set
        return None

    def is_templated(self, name: str) -> bool:
        if self._native_ptr is not None:
            lib = _get_lib()
            return bool(
                _generated_capi.call_scalar_or_raise(
                    lib,
                    lib.ifcopenshell_ifcapi_pset_template_is_templated,
                    ctypes.c_bool,
                    ifcopenshell.get_log() or "ifcopenshell_ifcapi_pset_template_is_templated",
                    self._native_ptr,
                    _generated_capi.encode_string(name),
                )
            )
        return bool(self.get_by_name(name))


def get_pset_template_type(pset_template: entity_instance) -> Literal["PSET", "QTO", None]:
    """Get the type of the pset template.
    If type is mixed or not defined, return None."""
    lib = _get_lib()
    pset_type = _generated_capi.call_string_or_raise(
        lib,
        lib.ifcopenshell_ifcapi_pset_template_pset_type,
        ifcopenshell.get_log() or "ifcopenshell_ifcapi_pset_template_pset_type",
        ctypes.cast(
            ctypes.c_void_p(pset_template._handle),
            ctypes.POINTER(_generated_capi.ifcopenshell_ifc_instance_t),
        ),
    )
    if pset_type:
        return pset_type  # type: ignore[return-value]

    # Try to identify whether it's pset or qto from the template type.
    template_type = pset_template.TemplateType
    if template_type:
        if template_type.startswith("PSET_"):
            return "PSET"
        elif template_type.startswith("QTO_"):
            return "QTO"
        # Can also be 'NOTDEFINED'.

    pset_types = set()
    for prop in pset_template.HasPropertyTemplates:
        prop_template_type = prop.TemplateType
        if prop_template_type:
            if prop_template_type.startswith("P_"):
                pset_types.add("PSET")
            else:  # All other values are Q_.
                pset_types.add("QTO")

    pset_type = next(iter(pset_types)) if len(pset_types) == 1 else None
    return pset_type


class ApplicableEntity(NamedTuple):
    value: str
    ifc_class: str
    predefined_type: Union[str, None]
    performance_history: bool


def parse_applicable_entity(applicable_entity: str) -> list[ApplicableEntity]:
    """Parse ApplicableEntity string query to tuples.

    :param applicable_entity: IfcPropertySetTemplate.ApplicableEntity query.
    :return: List of ApplicableEntity tuples.
    """
    items: list[ApplicableEntity] = []
    for item in applicable_entity.split(","):
        value = item
        item, predefined_type = parts if len(parts := item.split("/")) > 1 else (item, None)
        ifc_class, performance_history = (parts[0], True) if len(parts := item.split("[")) > 1 else (item, False)
        items.append(ApplicableEntity(value, ifc_class, predefined_type, performance_history))
    return items


def convert_applicable_entities_to_query(applicable_entities: list[ApplicableEntity]) -> str:
    """Get query supported by :func:`ifcopenshell.util.selector.filter_elements`."""
    parts: list[str] = []
    for entity in applicable_entities:
        # NOTE: selector currently doesn't support checking if element has performance history.
        part = entity.ifc_class
        if entity.predefined_type:
            part += f', PredefinedType="{entity.predefined_type}"'
        parts.append(part)
    return " + ".join(parts)
