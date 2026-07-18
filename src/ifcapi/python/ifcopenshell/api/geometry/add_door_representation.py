# IfcOpenShell - IFC toolkit and geometry engine
# Copyright (C) 2023 @Andrej730
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

from __future__ import annotations

import dataclasses
from typing import Any, Literal, Optional, Union, get_args

from . import _capi

DOOR_TYPE = Literal[
    "SINGLE_SWING_LEFT",
    "SINGLE_SWING_RIGHT",
    "DOUBLE_SWING_RIGHT",
    "DOUBLE_SWING_LEFT",
    "DOUBLE_DOOR_SINGLE_SWING",
    "DOUBLE_DOOR_DOUBLE_SWING",
    "SLIDING_TO_LEFT",
    "SLIDING_TO_RIGHT",
    "DOUBLE_DOOR_SLIDING",
]
SUPPORTED_DOOR_TYPES = get_args(DOOR_TYPE)

DOOR_LINING_PROPERTY_ORDER = (
    "LiningDepth",
    "LiningThickness",
    "LiningOffset",
    "LiningToPanelOffsetX",
    "LiningToPanelOffsetY",
    "TransomThickness",
    "TransomOffset",
    "CasingDepth",
    "CasingThickness",
    "ThresholdDepth",
    "ThresholdThickness",
    "ThresholdOffset",
)

DOOR_PANEL_PROPERTY_ORDER = ("PanelDepth", "PanelWidth", "FrameDepth", "FrameThickness")


def mm(x: float) -> float:
    """mm to meters shortcut for readability"""
    return x / 1000




# we use dataclass as we need default values for arguments
# it's okay to use slots since we don't need dynamic attributes
@dataclasses.dataclass(slots=True)
class DoorLiningProperties:
    LiningDepth: Optional[float] = None
    """Optional, defaults to 50mm."""

    LiningThickness: Optional[float] = None
    """Optional, defaults to 50mm."""

    LiningOffset: Optional[float] = None
    """Offset from the outer side of the wall (by Y-axis). Optional, defaults to 0.0."""

    LiningToPanelOffsetX: Optional[float] = None
    """Offset from the wall. Optional, defaults to 25mm."""

    LiningToPanelOffsetY: Optional[float] = None
    """Offset from the X-axis (unlike windows). Optional, defaults to 25mm."""

    TransomThickness: Optional[float] = None
    """Vertical distance between door and window panels. Optional, defaults to 0.0."""

    TransomOffset: Optional[float] = None
    """Distance from the bottom door opening
    to the beginning of the transom
    unlike windows TransomOffset which goes to the center of the transom.
    Optional, defaults 1.525m."""

    ShapeAspectStyle: None = None
    """Optional. Deprecated argument."""

    CasingDepth: Optional[float] = None
    """Casing cover wall faces around the opening
    on the left, right and upper sides
    Casing should be either on both sides of the wall or no casing
    If `LiningOffset` is present then therefore casing is not possible on outer wall
    therefore there will be no casing on inner wall either. Optional, defaults to 5mm."""

    CasingThickness: Optional[float] = None
    """Casing thickness by Z-axis. Optional, defaults to 75mm."""

    ThresholdDepth: Optional[float] = None
    """Threshold covers the bottom side of the opening. Optional, defaults to 100mm."""

    ThresholdThickness: Optional[float] = None
    """Theshold thickness by Z-axis. Optional, defaults to 25mm."""

    ThresholdOffset: Optional[float] = None
    """Threshold offset by Y-axis. Optional, defaults to 0.0."""

    def initialize_properties(self, unit_scale: float) -> None:
        # in meters
        # fmt: off
        default_values: dict[str, float] = dict(
            LiningDepth          = mm(50),
            LiningThickness      = mm(50),
            LiningOffset         = 0.0,
            LiningToPanelOffsetX = mm(25),
            LiningToPanelOffsetY = mm(25),
            TransomThickness     = 0.0,
            TransomOffset        = mm(1525),
            CasingDepth          = mm(5),
            CasingThickness      = mm(75),
            ThresholdDepth       = mm(100),
            ThresholdThickness   = mm(25),
            ThresholdOffset      = 0.0,
        )
        # fmt: on

        si_conversion = 1 / unit_scale
        for attr, default_value in default_values.items():
            if getattr(self, attr) is not None:
                continue
            setattr(self, attr, default_value * si_conversion)


@dataclasses.dataclass(slots=True)
class DoorPanelProperties:
    PanelDepth: Optional[float] = None
    """Frame thickness by Y axis. Optional, defaults to 35 mm."""

    PanelWidth: float = 1.0
    """Ratio to the clear door opening. Optional, defaults to 1.0."""

    FrameDepth: Optional[float] = None
    """Frame thickness by Y axis. Optional, defaults to 35 mm."""

    FrameThickness: Optional[float] = None
    """Frame thickness by X axis. Optional, defaults to 35 mm."""

    PanelPosition: None = None
    """Optional, value is never used"""

    PanelOperation: None = None
    """Optional, value is never used.
    Defines the basic ways to describe how door panels operate."""

    ShapeAspectStyle: None = None
    """Optional. Deprecated argument."""

    def initialize_properties(self, unit_scale: float) -> None:
        # in meters
        # fmt: off
        default_values: dict[str, float] = dict(
            PanelDepth     = mm(35),
            FrameDepth     = mm(35),
            FrameThickness = mm(35),
        )
        # fmt: on

        si_conversion = 1 / unit_scale
        for attr, default_value in default_values.items():
            if getattr(self, attr) is not None:
                continue
            setattr(self, attr, default_value * si_conversion)


def add_door_representation(
    file: ifcopenshell.file,
    *,  # keywords only as this API implementation is probably not final
    context: ifcopenshell.entity_instance,
    overall_height: Optional[float] = None,
    overall_width: Optional[float] = None,
    # door type
    # http://ifc43-docs.standards.buildingsmart.org/IFC/RELEASE/IFC4x3/HTML/lexical/IfcDoorTypeOperationEnum.htm
    operation_type: DOOR_TYPE = "SINGLE_SWING_LEFT",
    lining_properties: Optional[Union[DoorLiningProperties, dict[str, Any]]] = None,
    panel_properties: Optional[Union[DoorPanelProperties, dict[str, Any]]] = None,
    part_of_product: Optional[ifcopenshell.entity_instance] = None,
    unit_scale: Optional[float] = None,
) -> Union[ifcopenshell.entity_instance, None]:
    """Add a geometric representation for a door.

    units in usecase_settings expected to be in ifc project units

    :param context: IfcGeometricRepresentationContext for the representation.
    :param overall_height: Overall door height. Defaults to 2m.
    :param overall_width: Overall door width. Defaults to 0.9m.
    :param operation_type: Type of the door. Defaults to SINGLE_SWING_LEFT.
    :param lining_properties: DoorLiningProperties or a dictionary to create one.
        See DoorLiningProperties description for details.
    :param panel_properties: DoorPanelProperties or a dictionary to create one.
        See DoorPanelProperties description for details.
    :param unit_scale: The unit scale as calculated by
        ifcopenshell.util.unit.calculate_unit_scale. If not provided, it
        will be automatically calculated for you.
    :return: IfcShapeRepresentation for a door.
    """
    if lining_properties is not None and not isinstance(
        lining_properties, DoorLiningProperties
    ):
        lining_properties = DoorLiningProperties(**lining_properties)
    if panel_properties is not None and not isinstance(
        panel_properties, DoorPanelProperties
    ):
        panel_properties = DoorPanelProperties(**panel_properties)

    try:
        operation_index = SUPPORTED_DOOR_TYPES.index(operation_type)
    except ValueError as error:
        raise ValueError(
            f"Unsupported door operation type: {operation_type}"
        ) from error

    values: dict[str, Any] = {
        "context": _capi.instance_handle(context),
        "operation_type": operation_index,
    }
    if overall_height is not None:
        values["overall_height"] = overall_height
    if overall_width is not None:
        values["overall_width"] = overall_width
    if lining_properties is not None:
        properties = dataclasses.asdict(lining_properties)
        values["lining_properties"] = {
            name[0].lower()
            + "".join(
                f"_{char.lower()}" if char.isupper() else char for char in name[1:]
            ): value
            for name, value in properties.items()
            if name in DOOR_LINING_PROPERTY_ORDER and value is not None
        }
    if panel_properties is not None:
        properties = dataclasses.asdict(panel_properties)
        values["panel_properties"] = {
            name[0].lower()
            + "".join(
                f"_{char.lower()}" if char.isupper() else char for char in name[1:]
            ): value
            for name, value in properties.items()
            if name in DOOR_PANEL_PROPERTY_ORDER and value is not None
        }
    if part_of_product is not None:
        values["part_of_product"] = _capi.instance_handle(part_of_product)
    if unit_scale is not None:
        values["unit_scale"] = unit_scale

    return _capi.call_handle(
        file,
        "geometry_add_door_representation",
        _capi.file_handle(file),
        values,
        nullable=True,
    )
