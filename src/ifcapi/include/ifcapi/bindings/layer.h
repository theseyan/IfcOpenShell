// This file was generated with the assistance of an AI coding tool.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef IFCAPI_BINDINGS_LAYER_H
#define IFCAPI_BINDINGS_LAYER_H

#include "ifcapi/bindings/contract.h"

#include "ifcparse/IfcBaseClass.h"
#include "ifcparse/IfcFile.h"

#include <boost/logic/tribool.hpp>

#include <string>
#include <vector>

namespace ifcapi {
namespace bindings {

IFCAPI_BINDING IfcUtil::IfcBaseClass* layer_add_layer(
    IfcParse::IfcFile* file,
    const std::string& name);
IFCAPI_BINDING IfcUtil::IfcBaseClass* layer_add_layer_with_style(
    IfcParse::IfcFile* file,
    const std::string& name,
    boost::logic::tribool on,
    boost::logic::tribool frozen,
    boost::logic::tribool blocked,
    const std::vector<const IfcUtil::IfcBaseClass*>& styles);
IFCAPI_BINDING void layer_assign_layer(
    IfcParse::IfcFile* file,
    const std::vector<const IfcUtil::IfcBaseClass*>& items,
    IfcUtil::IfcBaseClass* layer);
IFCAPI_BINDING void layer_unassign_layer(
    IfcParse::IfcFile* file,
    const std::vector<const IfcUtil::IfcBaseClass*>& items,
    IfcUtil::IfcBaseClass* layer);
IFCAPI_BINDING void layer_remove_layer(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* layer);

} // namespace bindings
} // namespace ifcapi

#endif /* IFCAPI_BINDINGS_LAYER_H */
