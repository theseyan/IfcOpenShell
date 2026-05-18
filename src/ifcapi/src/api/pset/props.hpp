// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef IFCAPI_API_PSET_PROPS_HPP
#define IFCAPI_API_PSET_PROPS_HPP

#include "ifcparse/IfcBaseClass.h"

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

struct ifcopenshell_pset_props_t;

namespace ifcapi_pset {

enum class Kind {
    NONE,
    BOOL,
    INT,
    DOUBLE,
    STRING,
    INSTANCE,
    TYPED_STRING,
    TYPED_DOUBLE,
    TYPED_INT,
    TYPED_BOOL,
    STRING_LIST,
    DOUBLE_LIST,
    INT_LIST,
    INSTANCE_LIST,
    DATE,
    DATETIME,
    DURATION,
    DICT,
};

struct Entry {
    std::string key;
    Kind kind = Kind::NONE;
    bool b_val = false;
    int64_t i_val = 0;
    double d_val = 0.0;
    std::string s_val;
    std::string ifc_type;
    IfcUtil::IfcBaseClass* inst = nullptr;
    IfcUtil::IfcBaseClass* unit = nullptr;
    std::vector<std::string> str_list;
    std::vector<double> dbl_list;
    std::vector<int64_t> int_list;
    std::vector<IfcUtil::IfcBaseClass*> inst_list;
    int year = 0;
    int month = 0;
    int day = 0;
    int hour = 0;
    int minute = 0;
    int second = 0;
    int microsecond = 0;
    bool has_timezone = false;
    int timezone_offset_minutes = 0;
    bool duration_negative = false;
    int duration_years = 0;
    int duration_months = 0;
    int duration_days = 0;
    int duration_hours = 0;
    int duration_minutes = 0;
    int duration_seconds = 0;
    int duration_microseconds = 0;
    std::shared_ptr<ifcopenshell_pset_props_t> nested;
};

}  // namespace ifcapi_pset

struct ifcopenshell_pset_props_t {
    std::vector<ifcapi_pset::Entry> entries;
};

#endif
