/* This file was generated with the assistance of an AI coding tool. */

#ifndef IFCAPI_BINDINGS_FEATURE_H
#define IFCAPI_BINDINGS_FEATURE_H

#include "ifcapi/bindings/contract.h"

#include "ifcparse/express.h"
#include "ifcparse/file.h"

namespace ifcapi {
namespace bindings {

IFCAPI_BINDING express::Base feature_add_feature(
    ifcopenshell::file* file,
    express::Base* feature,
    express::Base* element,
    IFCAPI_NULLABLE express::Base* user,
    IFCAPI_NULLABLE express::Base* application);
IFCAPI_BINDING express::Base feature_add_filling(
    ifcopenshell::file* file,
    express::Base* opening,
    express::Base* element);
IFCAPI_BINDING void feature_remove_feature(
    ifcopenshell::file* file,
    express::Base* feature,
    IFCAPI_NULLABLE express::Base* user,
    IFCAPI_NULLABLE express::Base* application);
IFCAPI_BINDING void feature_remove_filling(
    ifcopenshell::file* file,
    express::Base* element);

} // namespace bindings
} // namespace ifcapi

#endif /* IFCAPI_BINDINGS_FEATURE_H */
