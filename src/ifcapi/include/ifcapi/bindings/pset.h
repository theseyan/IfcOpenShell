/* This file was generated with the assistance of an AI coding tool. */

#ifndef IFCAPI_BINDINGS_PSET_H
#define IFCAPI_BINDINGS_PSET_H

#include "ifcapi/bindings/contract.h"

#include "ifcparse/express.h"
#include "ifcparse/file.h"

#include <cstdint>
#include <string>
#include <vector>

struct ifcopenshell_pset_props_t;

namespace ifcapi {
namespace bindings {

IFCAPI_BINDING ifcopenshell_pset_props_t* pset_props_new();
IFCAPI_BINDING void pset_props_free(ifcopenshell_pset_props_t* props);
IFCAPI_BINDING void pset_props_set_null(ifcopenshell_pset_props_t* props, const std::string& key);
IFCAPI_BINDING void pset_props_set_bool(ifcopenshell_pset_props_t* props, const std::string& key, bool value);
IFCAPI_BINDING void pset_props_set_int(ifcopenshell_pset_props_t* props, const std::string& key, int64_t value);
IFCAPI_BINDING void pset_props_set_double(ifcopenshell_pset_props_t* props, const std::string& key, double value);
IFCAPI_BINDING void pset_props_set_string(
    ifcopenshell_pset_props_t* props, const std::string& key, const std::string& value);
IFCAPI_BINDING void pset_props_set_instance(
    ifcopenshell_pset_props_t* props, const std::string& key, IFCAPI_NULLABLE express::Base* value);
IFCAPI_BINDING void pset_props_set_typed_string(
    ifcopenshell_pset_props_t* props, const std::string& key, const std::string& value, const std::string& ifc_type);
IFCAPI_BINDING void pset_props_set_typed_double(
    ifcopenshell_pset_props_t* props, const std::string& key, double value, const std::string& ifc_type);
IFCAPI_BINDING void pset_props_set_typed_int(
    ifcopenshell_pset_props_t* props, const std::string& key, int64_t value, const std::string& ifc_type);
IFCAPI_BINDING void pset_props_set_typed_bool(
    ifcopenshell_pset_props_t* props, const std::string& key, bool value, const std::string& ifc_type);
IFCAPI_BINDING void pset_props_set_string_list(
    ifcopenshell_pset_props_t* props, const std::string& key, const std::vector<std::string>& values);
IFCAPI_BINDING void pset_props_set_double_list(
    ifcopenshell_pset_props_t* props, const std::string& key, const std::vector<double>& values);
IFCAPI_BINDING void pset_props_set_int_list(
    ifcopenshell_pset_props_t* props, const std::string& key, const std::vector<int64_t>& values);
IFCAPI_BINDING void pset_props_set_instance_list(
    ifcopenshell_pset_props_t* props, const std::string& key, const std::vector<express::Base>& values);
IFCAPI_BINDING void pset_props_set_date(
    ifcopenshell_pset_props_t* props, const std::string& key, int year, int month, int day);
IFCAPI_BINDING void pset_props_set_datetime(
    ifcopenshell_pset_props_t* props, const std::string& key, int year, int month, int day, int hour, int minute, int second,
    int microsecond, bool has_timezone, int timezone_offset_minutes);
IFCAPI_BINDING void pset_props_set_duration(
    ifcopenshell_pset_props_t* props, const std::string& key, bool negative, int years, int months, int days, int hours,
    int minutes, int seconds, int microseconds);
IFCAPI_BINDING void pset_props_set_dict(
    ifcopenshell_pset_props_t* outer, const std::string& key, ifcopenshell_pset_props_t* inner);
IFCAPI_BINDING void pset_props_set_unit_for_last(
    ifcopenshell_pset_props_t* props, IFCAPI_NULLABLE express::Base* unit);

IFCAPI_BINDING express::Base pset_add_pset(
    ifcopenshell::file* file,
    express::Base* product,
    const std::string& name,
    IFCAPI_NULLABLE express::Base* owner_history,
    IFCAPI_NULLABLE express::Base* user,
    IFCAPI_NULLABLE express::Base* application,
    const char* ifc2x3_subclass);
IFCAPI_BINDING express::Base pset_add_qto(
    ifcopenshell::file* file,
    express::Base* product,
    const std::string& name,
    IFCAPI_NULLABLE express::Base* owner_history,
    IFCAPI_NULLABLE express::Base* user,
    IFCAPI_NULLABLE express::Base* application);
IFCAPI_BINDING bool pset_edit_pset(
    ifcopenshell::file* file,
    express::Base* pset,
    const char* name,
    ifcopenshell_pset_props_t* properties,
    IFCAPI_NULLABLE express::Base* pset_template,
    bool should_purge);
IFCAPI_BINDING bool pset_edit_qto(
    ifcopenshell::file* file,
    express::Base* qto,
    const char* name,
    ifcopenshell_pset_props_t* properties,
    IFCAPI_NULLABLE express::Base* qto_template);
IFCAPI_BINDING express::Base pset_assign_pset(
    ifcopenshell::file* file,
    const std::vector<express::Base>& products,
    express::Base* pset,
    IFCAPI_NULLABLE express::Base* owner_history,
    IFCAPI_NULLABLE express::Base* user,
    IFCAPI_NULLABLE express::Base* application);
IFCAPI_BINDING void pset_unassign_pset(
    ifcopenshell::file* file,
    const std::vector<express::Base>& products,
    express::Base* pset);
IFCAPI_BINDING void pset_remove_pset(
    ifcopenshell::file* file,
    express::Base* product,
    express::Base* pset);
IFCAPI_BINDING std::vector<express::Base> pset_unshare_pset(
    ifcopenshell::file* file,
    const std::vector<express::Base>& products,
    express::Base* pset,
    IFCAPI_NULLABLE express::Base* owner_history,
    IFCAPI_NULLABLE express::Base* user,
    IFCAPI_NULLABLE express::Base* application);

} // namespace bindings
} // namespace ifcapi

#endif /* IFCAPI_BINDINGS_PSET_H */
