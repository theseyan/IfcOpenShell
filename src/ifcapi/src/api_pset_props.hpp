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
    std::vector<std::string> str_list;
    std::vector<double> dbl_list;
    std::vector<int64_t> int_list;
    std::shared_ptr<ifcopenshell_pset_props_t> nested;
};

}  // namespace ifcapi_pset

struct ifcopenshell_pset_props_t {
    std::vector<ifcapi_pset::Entry> entries;
};

#endif
