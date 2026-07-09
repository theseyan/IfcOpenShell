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
    express::Base feature;
    express::Base element;
    std::optional<express::Base> user;
    std::optional<express::Base> application;
};

/**
 * Options for removing a feature element and its relationship.
 */
struct FeatureRemoveFeatureOptions {
    express::Base feature;
    std::optional<express::Base> user;
    std::optional<express::Base> application;
};

IFCAPI_BINDING express::Base feature_add_feature(
    ifcopenshell::file* file,
    const FeatureAddFeatureOptions& options);
IFCAPI_BINDING express::Base feature_add_filling(
    ifcopenshell::file* file,
    express::Base* opening,
    express::Base* element);
IFCAPI_BINDING void feature_remove_feature(
    ifcopenshell::file* file,
    const FeatureRemoveFeatureOptions& options);
IFCAPI_BINDING void feature_remove_filling(
    ifcopenshell::file* file,
    express::Base* element);

} // namespace bindings
} // namespace ifcapi

#endif /* IFCAPI_BINDINGS_FEATURE_H */
