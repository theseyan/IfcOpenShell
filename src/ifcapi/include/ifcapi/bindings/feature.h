/* This file was generated with the assistance of an AI coding tool. */

#ifndef IFCAPI_BINDINGS_FEATURE_H
#define IFCAPI_BINDINGS_FEATURE_H

#include "ifcapi/bindings/contract.h"

#include "ifcparse/express.h"
#include "ifcparse/file.h"

#include <optional>

namespace ifcapi {
namespace bindings {

/**
 * Options for adding a feature element to a host element.
 */
struct FeatureAddFeatureOptions {
    /// The feature element (IfcOpeningElement, IfcProjectionElement, IfcSurfaceFeature, etc.).
    express::Base feature;
    /// The host element that the feature is attached to.
    express::Base element;
    /// Optional user for owner history creation.
    std::optional<express::Base> user;
    /// Optional application for owner history creation.
    std::optional<express::Base> application;
};

/**
 * Options for removing a feature element and its relationship.
 */
struct FeatureRemoveFeatureOptions {
    /// The feature element to remove.
    express::Base feature;
    /// Optional user for owner history updates on modified relationships.
    std::optional<express::Base> user;
    /// Optional application for owner history updates on modified relationships.
    std::optional<express::Base> application;
};

/**
 * Associate a feature element with a host element.
 *
 * For IfcFeatureElementSubtraction (e.g. IfcOpeningElement), creates
 * IfcRelVoidsElement. For IfcFeatureElementAddition (e.g. IfcProjectionElement),
 * creates IfcRelProjectsElement. For IfcSurfaceFeature, creates
 * IfcRelAdheresToElement (IFC4X3) or aggregates via IfcRelAggregates (IFC4).
 * If the feature is already associated with the same host, returns the
 * existing relationship. If associated with a different host, the old
 * relationship is removed first.
 */
IFCAPI_BINDING express::Base feature_add_feature(
    ifcopenshell::file* file,
    const FeatureAddFeatureOptions& options);

/**
 * Fill an opening with an element via IfcRelFillsElement.
 *
 * If the element already fills the same opening, returns the existing
 * relationship. If it fills a different opening, the old relationship is
 * removed first.
 */
IFCAPI_BINDING express::Base feature_add_filling(
    ifcopenshell::file* file,
    express::Base* opening,
    express::Base* element);

/**
 * Remove a feature element and its relationships.
 *
 * For IfcFeatureElementSubtraction subclasses, removes the IfcRelVoidsElement.
 * For IfcFeatureElementAddition subclasses, removes the IfcRelProjectsElement.
 * For IfcSurfaceFeature in IFC4, unassigns from the aggregate parent. In
 * other schemas, no feature-specific relationship is removed before the
 * element itself is removed.
 * IfcOpeningElement fillings are also removed. Nested elements, property sets,
 * representations, and other inverse relationships are cleaned up as part of
 * removing the feature.
 */
IFCAPI_BINDING void feature_remove_feature(
    ifcopenshell::file* file,
    const FeatureRemoveFeatureOptions& options);

/**
 * Remove a filling relationship for an element.
 *
 * Finds and deletes the IfcRelFillsElement where the element is the
 * RelatedBuildingElement.
 */
IFCAPI_BINDING void feature_remove_filling(
    ifcopenshell::file* file,
    express::Base* element);

} // namespace bindings
} // namespace ifcapi

#endif /* IFCAPI_BINDINGS_FEATURE_H */
