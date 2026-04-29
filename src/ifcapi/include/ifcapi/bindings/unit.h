/* This file was generated with the assistance of an AI coding tool. */

#ifndef IFCAPI_BINDINGS_UNIT_H
#define IFCAPI_BINDINGS_UNIT_H

#include "ifcapi/bindings/contract.h"

#include <string>

namespace ifcapi {
namespace bindings {

IFCAPI_BINDING std::string unit_get_prefix(const std::string& text);
IFCAPI_BINDING double unit_get_prefix_multiplier(const std::string& text);
IFCAPI_BINDING std::string unit_get_unit_name(const std::string& text);
IFCAPI_BINDING std::string unit_get_unit_name_universal(const std::string& text);
IFCAPI_BINDING std::string unit_get_measure_class(const std::string& unit_type);
IFCAPI_BINDING std::string unit_get_measure_unit_type(const std::string& measure_class);
IFCAPI_BINDING std::string unit_get_symbol_measure_class(const std::string& symbol);
IFCAPI_BINDING std::string unit_get_symbol_quantity_class(const std::string& symbol);

} // namespace bindings
} // namespace ifcapi

#endif /* IFCAPI_BINDINGS_UNIT_H */
