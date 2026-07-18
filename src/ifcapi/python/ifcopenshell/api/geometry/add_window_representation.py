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
from itertools import chain
from typing import Any, Literal, Optional, Union, get_args

import ifcopenshell.util.unit

from . import _capi

# SCHEMAS describe panels setup
# where:
# - schema rows represent window X axis
# - schema columns represent window Y axis
# - order of rows is from top of the window to bottom

WINDOW_TYPE = Literal[
    "SINGLE_PANEL",
    "DOUBLE_PANEL_HORIZONTAL",
    "DOUBLE_PANEL_VERTICAL",
    "TRIPLE_PANEL_BOTTOM",
    "TRIPLE_PANEL_HORIZONTAL",
    "TRIPLE_PANEL_LEFT",
    "TRIPLE_PANEL_RIGHT",
    "TRIPLE_PANEL_TOP",
    "TRIPLE_PANEL_VERTICAL",
]
SUPPORTED_WINDOW_TYPES = get_args(WINDOW_TYPE)

DEFAULT_PANEL_SCHEMAS = {
    "SINGLE_PANEL": [[0]],
    "DOUBLE_PANEL_HORIZONTAL": [[0], [1]],
    "DOUBLE_PANEL_VERTICAL": [[0, 1]],
    "TRIPLE_PANEL_BOTTOM": [[0, 1], [2, 2]],
    "TRIPLE_PANEL_TOP": [[0, 0], [1, 2]],
    "TRIPLE_PANEL_LEFT": [[0, 1], [0, 2]],
    "TRIPLE_PANEL_RIGHT": [[0, 1], [2, 1]],
    "TRIPLE_PANEL_HORIZONTAL": [[0], [1], [2]],
    "TRIPLE_PANEL_VERTICAL": [[0, 1, 2]],
}

WINDOW_LINING_PROPERTY_ORDER = (
    "LiningDepth",
    "LiningThickness",
    "LiningOffset",
    "LiningToPanelOffsetX",
    "LiningToPanelOffsetY",
    "MullionThickness",
    "FirstMullionOffset",
    "SecondMullionOffset",
    "TransomThickness",
    "FirstTransomOffset",
    "SecondTransomOffset",
)

WINDOW_PANEL_PROPERTY_ORDER = ("FrameDepth", "FrameThickness")


def mm(x: float) -> float:
    """mm to meters shortcut for readability"""
    return x / 1000




# we use dataclass as we need default values for arguments
# it's okay to use slots since we don't need dynamic attributes
@dataclasses.dataclass(slots=True)
class WindowLiningProperties:
    LiningDepth: Optional[float] = None
    """Optional, defaults to 50mm."""

    LiningThickness: Optional[float] = None
    """Optional, defaults to 50mm."""

    LiningOffset: Optional[float] = None
    """Offset to the wall. Optional, defaults to 50mm."""

    LiningToPanelOffsetX: Optional[float] = None
    """Offset from the wall. Optional, defaults to 25mm."""

    # that way it allows you to define overall_depth constant between all panels
    # and still have panels with different size:
    # overall_depth = lining_depth + offset_y
    # full offset from X axis = overall_depth - frame_depth.
    LiningToPanelOffsetY: Optional[float] = None
    """Offset from the lining. Optional, defaults to 25mm."""

    MullionThickness: Optional[float] = None
    """Mullion thickness (horizontal distance between panels).

    Applies to windows of types: DoublePanelVertical, TriplePanelBottom, TriplePanelTop,
    TriplePanelLeft, TriplePanelRight.

    Optional, defaults to 50mm."""

    FirstMullionOffset: Optional[float] = None
    """Distance from the first lining to the mullion center. Optional, defaults to 300mm."""

    SecondMullionOffset: Optional[float] = None
    """Distance from the first lining to the second mullion center. 

    Applies to windows of type: TriplePanelVertical.

    Optional, defaults to 450mm."""

    TransomThickness: Optional[float] = None
    """Transom thickness (vertical distance between panels), works similar way to mullions.

    Applies to windows of types:DoublePanelHorizontal, TriplePanelBottom, TriplePanelTop,
    TriplePanelLeft, TriplePanelRight.

    Optional, defaults to 50mm."""

    FirstTransomOffset: Optional[float] = None
    """Optional, defaults to 300mm."""

    SecondTransomOffset: Optional[float] = None
    """
    Applies to windows of type: TriplePanelHorizontal.
    Optional, defaults to 600mm."""

    ShapeAspectStyle: None = None
    """Optional. Deprecated argument."""

    def initialize_properties(self, unit_scale: float) -> None:
        # in meters
        # fmt: off
        default_values: dict[str, float] = dict(
            LiningDepth          = mm(50),
            LiningThickness      = mm(50),
            LiningOffset         = mm(50),
            LiningToPanelOffsetX = mm(25),
            LiningToPanelOffsetY = mm(25),
            MullionThickness     = mm(50),
            FirstMullionOffset   = mm(300),
            SecondMullionOffset  = mm(450),
            TransomThickness     = mm(50),
            FirstTransomOffset   = mm(300),
            SecondTransomOffset  = mm(600),
        )
        # fmt: on

        si_conversion = 1 / unit_scale
        for attr, default_value in default_values.items():
            if getattr(self, attr) is not None:
                continue
            setattr(self, attr, default_value * si_conversion)


@dataclasses.dataclass(slots=True)
class WindowPanelProperties:
    FrameDepth: Optional[float] = None
    """Frame thickness by Y axis. Optional, defaults to 35 mm."""

    FrameThickness: Optional[float] = None
    """Frame thickness by X axis. Optional, defaults to 35 mm."""

    PanelPosition: None = None
    """Optional, value is never used"""

    PanelOperation: None = None
    """Optional, value is never used.
    Defines the basic ways to describe how window panels operate."""

    ShapeAspectStyle: None = None
    """Optional. Deprecated argument."""

    def initialize_properties(self, unit_scale: float) -> None:
        # in meters
        # fmt: off
        default_values: dict[str, float] = dict(
            FrameDepth     = mm(35),
            FrameThickness = mm(35),
        )
        # fmt: on

        si_conversion = 1 / unit_scale
        for attr, default_value in default_values.items():
            if getattr(self, attr) is not None:
                continue
            setattr(self, attr, default_value * si_conversion)


def add_window_representation(
    file: ifcopenshell.file,
    *,  # keywords only as this API implementation is probably not final
    context: ifcopenshell.entity_instance,
    overall_height: Optional[float] = None,
    overall_width: Optional[float] = None,
    partition_type: WINDOW_TYPE = "SINGLE_PANEL",
    lining_properties: Optional[Union[WindowLiningProperties, dict[str, Any]]] = None,
    panel_properties: Optional[
        list[Union[WindowPanelProperties, dict[str, Any]]]
    ] = None,
    part_of_product: Optional[ifcopenshell.entity_instance] = None,
    unit_scale: Optional[float] = None,
) -> ifcopenshell.entity_instance:
    """units in usecase_settings expected to be in ifc project units

    :param context: IfcGeometricRepresentationContext for the representation.
    :param overall_height: Overall window height. Defaults to 0.9m.
    :param overall_width: Overall window width. Defaults to 0.6m.
    :param partition_type: Type of the window. Defaults to SINGLE_PANEL.
    :param lining_properties: WindowLiningProperties or a dictionary to create one.
        See WindowLiningProperties description for details.
    :param panel_properties: A list of WindowPanelProperties or dictionaries to create one.
        See WindowPanelProperties description for details.
    :param unit_scale: The unit scale as calculated by
        ifcopenshell.util.unit.calculate_unit_scale. If not provided, it
        will be automatically calculated for you.
    :return: IfcShapeRepresentation for a window.
    """
    # http://ifc43-docs.standards.buildingsmart.org/IFC/RELEASE/IFC4x3/HTML/lexical/IfcWindow.htm
    # http://ifc43-docs.standards.buildingsmart.org/IFC/RELEASE/IFC4x3/HTML/lexical/IfcWindowTypePartitioningEnum.htm
    # http://ifc43-docs.standards.buildingsmart.org/IFC/RELEASE/IFC4x3/HTML/lexical/IfcWindowLiningProperties.htm
    # http://ifc43-docs.standards.buildingsmart.org/IFC/RELEASE/IFC4x3/HTML/lexical/IfcWindowPanelProperties.htm
    # define unit_scale first as it's going to be used setting default arguments
    resolved_unit_scale = (
        ifcopenshell.util.unit.calculate_unit_scale(file)
        if unit_scale is None
        else unit_scale
    )
    lining_properties_supplied = lining_properties is not None
    panel_properties_supplied = panel_properties is not None

    if lining_properties is None:
        lining_properties = WindowLiningProperties()
    elif not isinstance(lining_properties, WindowLiningProperties):
        lining_properties = WindowLiningProperties(**lining_properties)
    lining_properties.initialize_properties(resolved_unit_scale)
    lining_properties = dataclasses.asdict(lining_properties)

    if panel_properties is None:
        panel_properties = [WindowPanelProperties()]

    for i in range(len(panel_properties)):
        properties = panel_properties[i]
        if not isinstance(properties, WindowPanelProperties):
            properties = WindowPanelProperties(**properties)
        properties.initialize_properties(resolved_unit_scale)
        panel_properties[i] = dataclasses.asdict(properties)

    panel_schema = DEFAULT_PANEL_SCHEMAS[partition_type]
    max_panel_index = max(chain.from_iterable(panel_schema))
    panel_properties[max_panel_index]

    values: dict[str, Any] = {
        "context": _capi.instance_handle(context),
        "partition_type": SUPPORTED_WINDOW_TYPES.index(partition_type),
    }
    if overall_height is not None:
        values["overall_height"] = overall_height
    if overall_width is not None:
        values["overall_width"] = overall_width
    if lining_properties_supplied:
        values["lining_properties"] = {
            name[0].lower()
            + "".join(
                f"_{char.lower()}" if char.isupper() else char for char in name[1:]
            ): value
            for name, value in lining_properties.items()
            if name in WINDOW_LINING_PROPERTY_ORDER and value is not None
        }
    if panel_properties_supplied:
        values["panel_properties"] = [
            {
                name[0].lower()
                + "".join(
                    f"_{char.lower()}" if char.isupper() else char for char in name[1:]
                ): value
                for name, value in panel.items()
                if name in WINDOW_PANEL_PROPERTY_ORDER and value is not None
            }
            for panel in panel_properties
        ]
    if part_of_product is not None:
        values["part_of_product"] = _capi.instance_handle(part_of_product)
    if unit_scale is not None:
        values["unit_scale"] = unit_scale
    return _capi.call_handle(
        file,
        "geometry_add_window_representation",
        _capi.file_handle(file),
        values,
    )
