// This file was generated with the assistance of an AI coding tool.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef IFCAPI_BINDINGS_GEOREFERENCE_H
#define IFCAPI_BINDINGS_GEOREFERENCE_H

#include "ifcapi/bindings/contract.h"

#include "ifcparse/IfcFile.h"

namespace ifcapi {
namespace bindings {

IFCAPI_BINDING void georeference_edit_true_north(IfcParse::IfcFile* file, bool has_true_north, double x, double y);

IFCAPI_BINDING void georeference_edit_wcs(
    IfcParse::IfcFile* file,
    double x,
    double y,
    double z,
    double rotation,
    bool is_si);

} // namespace bindings
} // namespace ifcapi

#endif /* IFCAPI_BINDINGS_GEOREFERENCE_H */
