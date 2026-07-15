// This file was generated with the assistance of an AI coding tool.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef IFCAPI_BINDINGS_COGO_H
#define IFCAPI_BINDINGS_COGO_H

#include "ifcapi/bindings/contract.h"
#include "ifcparse/file.h"

#include <optional>
#include <string>

namespace ifcapi {
namespace bindings {

/**
 * Convert a quadrant bearing to decimal degrees.
 *
 * Accepts N/S, degrees, optional minutes and decimal seconds, and E/W,
 * separated by arbitrary whitespace. Invalid input is a value error with the
 * stable invalid-quadrant-bearing code; its message is diagnostic only.
 */
IFCAPI_BINDING double cogo_bearing2dd(const std::string& bearing);

/**
 * Options for adding a survey point to the model.
 */
struct CogoAddSurveyPointOptions {
    /// IfcPoint (IfcCartesianPoint or IfcPointOnCurve) representing the survey location.
    express::Base survey_point;
    /// IfcSite to contain the annotation. When omitted, the first IfcSite in the file is used.
    std::optional<express::Base> site;
    /// Owner history applied to created root entities. When omitted, one is created from user/application.
    std::optional<express::Base> owner_history;
    /// IfcPersonAndOrganization used to create an IfcOwnerHistory when owner_history is omitted.
    std::optional<express::Base> user;
    /// IfcApplication used to create an IfcOwnerHistory when owner_history is omitted.
    std::optional<express::Base> application;
};

/**
 * Add a single survey point as an IfcAnnotation.
 *
 * Creates an IfcAnnotation with PredefinedType SURVEY, wraps the given
 * IfcPoint in an IfcShapeRepresentation (Annotation/Point), and places
 * the annotation in the given or first IfcSite. The annotation's
 * ObjectPlacement is set to the world coordinate system of the
 * Model/Annotation/MODEL_VIEW representation context.
 *
 * @param file File that receives the new entities.
 * @param options Survey point geometry and placement options.
 * @return The newly created IfcAnnotation, or no result if creation fails.
 */
IFCAPI_BINDING express::Base cogo_add_survey_point(
    ifcopenshell::file* file,
    const CogoAddSurveyPointOptions& options);

/**
 * Replace the survey point geometry of an existing annotation.
 *
 * Replaces the annotation's existing survey point with the given IfcPoint.
 * The annotation must already have a shape representation.
 *
 * @param annotation IfcAnnotation whose survey point to replace.
 * @param survey_point IfcPoint to assign as the new geometry.
 */
IFCAPI_BINDING void cogo_assign_survey_point(
    express::Base* annotation,
    express::Base* survey_point);

/**
 * Update the coordinates of the survey point inside an existing annotation.
 *
 * Updates the coordinates of the annotation's survey point. If the existing
 * point is two-dimensional, only x and y are written; otherwise all three
 * coordinates are used.
 *
 * @param annotation IfcAnnotation containing the survey point.
 * @param x Easting or X coordinate in model units.
 * @param y Northing or Y coordinate in model units.
 * @param z Elevation or Z coordinate in model units.
 */
IFCAPI_BINDING void cogo_edit_survey_point(express::Base* annotation, double x, double y, double z);

} // namespace bindings
} // namespace ifcapi

#endif /* IFCAPI_BINDINGS_COGO_H */
