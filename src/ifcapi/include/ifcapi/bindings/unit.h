/* This file was generated with the assistance of an AI coding tool. */

#ifndef IFCAPI_BINDINGS_UNIT_H
#define IFCAPI_BINDINGS_UNIT_H

#include "ifcapi/bindings/contract.h"

#include "ifcparse/IfcBaseClass.h"
#include "ifcparse/IfcFile.h"

#include <string>
#include <vector>

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
IFCAPI_BINDING std::vector<int> unit_get_si_dimensions(const std::string& name);
IFCAPI_BINDING std::vector<int> unit_get_named_dimensions(const std::string& name);
IFCAPI_BINDING double unit_convert(
    double value,
    const std::string& from_prefix,
    const std::string& from_unit,
    const std::string& to_prefix,
    const std::string& to_unit);
IFCAPI_BINDING std::string unit_format_length(
    double value,
    double precision,
    int decimal_places,
    bool suppress_zero_inches,
    const std::string& unit_system,
    const std::string& input_unit,
    const std::string& output_unit);
IFCAPI_BINDING IfcUtil::IfcBaseClass* unit_get_unit_assignment(IfcParse::IfcFile* file);
IFCAPI_BINDING IfcUtil::IfcBaseClass* unit_get_project_unit(IfcParse::IfcFile* file, const std::string& unit_type);
IFCAPI_BINDING std::string unit_get_full_unit_name(IfcUtil::IfcBaseClass* unit);
IFCAPI_BINDING std::string unit_get_unit_symbol(IfcUtil::IfcBaseClass* unit);
IFCAPI_BINDING double unit_convert_unit(
    double value,
    IfcUtil::IfcBaseClass* from_unit,
    IfcUtil::IfcBaseClass* to_unit);
IFCAPI_BINDING IfcUtil::IfcBaseClass* unit_resolve_property_unit(IfcUtil::IfcBaseClass* prop);
IFCAPI_BINDING std::string unit_resolve_property_measure_class(IfcUtil::IfcBaseClass* prop);
IFCAPI_BINDING IfcUtil::IfcBaseClass* unit_resolve_property_table_defining_unit(IfcUtil::IfcBaseClass* prop);
IFCAPI_BINDING std::string unit_resolve_property_table_defining_measure_class(IfcUtil::IfcBaseClass* prop);
IFCAPI_BINDING IfcUtil::IfcBaseClass* unit_resolve_property_table_defined_unit(IfcUtil::IfcBaseClass* prop);
IFCAPI_BINDING std::string unit_resolve_property_table_defined_measure_class(IfcUtil::IfcBaseClass* prop);
IFCAPI_BINDING double unit_calculate_unit_scale(IfcParse::IfcFile* file, const std::string& unit_type);

} // namespace bindings
} // namespace ifcapi

#endif /* IFCAPI_BINDINGS_UNIT_H */
