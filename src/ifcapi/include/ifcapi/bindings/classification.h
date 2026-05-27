/* This file was generated with the assistance of an AI coding tool. */

#ifndef IFCAPI_BINDINGS_CLASSIFICATION_H
#define IFCAPI_BINDINGS_CLASSIFICATION_H

#include "ifcapi/bindings/contract.h"

#include "ifcparse/express.h"
#include "ifcparse/file.h"
#include <vector>

#include <string>
#include <vector>

namespace ifcapi {
namespace bindings {

IFCAPI_BINDING std::vector<express::Base> classification_get_references(
    express::Base* element,
    bool should_inherit);
IFCAPI_BINDING express::Base classification_add_classification(
    ifcopenshell::file* file,
    const std::string& name);
IFCAPI_BINDING express::Base classification_add_reference(
    ifcopenshell::file* file,
    const std::vector<express::Base>& products,
    IFCAPI_NULLABLE express::Base* reference,
    const std::string& identification,
    bool has_identification,
    const std::string& name,
    bool has_name,
    IFCAPI_NULLABLE express::Base* classification,
    IFCAPI_NULLABLE express::Base* owner_history,
    IFCAPI_NULLABLE express::Base* user,
    IFCAPI_NULLABLE express::Base* application);
IFCAPI_BINDING void classification_remove_reference(
    ifcopenshell::file* file,
    express::Base* reference,
    const std::vector<express::Base>& products,
    IFCAPI_NULLABLE express::Base* user,
    IFCAPI_NULLABLE express::Base* application);
IFCAPI_BINDING void classification_remove_classification(
    ifcopenshell::file* file,
    express::Base* classification);

} // namespace bindings
} // namespace ifcapi

#endif /* IFCAPI_BINDINGS_CLASSIFICATION_H */
