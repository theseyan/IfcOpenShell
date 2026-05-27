// This file was generated with the assistance of an AI coding tool.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef IFCAPI_BINDINGS_LAYER_H
#define IFCAPI_BINDINGS_LAYER_H

#include "ifcapi/bindings/contract.h"

#include "ifcparse/express.h"
#include "ifcparse/file.h"

#include <boost/logic/tribool.hpp>

#include <string>
#include <vector>

namespace ifcapi {
namespace bindings {

IFCAPI_BINDING express::Base layer_add_layer(
    ifcopenshell::file* file,
    const std::string& name);
IFCAPI_BINDING express::Base layer_add_layer_with_style(
    ifcopenshell::file* file,
    const std::string& name,
    boost::logic::tribool on,
    boost::logic::tribool frozen,
    boost::logic::tribool blocked,
    const std::vector<express::Base>& styles);
IFCAPI_BINDING void layer_assign_layer(
    ifcopenshell::file* file,
    const std::vector<express::Base>& items,
    express::Base* layer);
IFCAPI_BINDING void layer_unassign_layer(
    ifcopenshell::file* file,
    const std::vector<express::Base>& items,
    express::Base* layer);
IFCAPI_BINDING void layer_remove_layer(
    ifcopenshell::file* file,
    express::Base* layer);

} // namespace bindings
} // namespace ifcapi

#endif /* IFCAPI_BINDINGS_LAYER_H */
