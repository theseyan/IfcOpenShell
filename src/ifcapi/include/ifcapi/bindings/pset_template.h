/* This file was generated with the assistance of an AI coding tool. */

#ifndef IFCAPI_BINDINGS_PSET_TEMPLATE_H
#define IFCAPI_BINDINGS_PSET_TEMPLATE_H

#include "ifcapi/bindings/contract.h"

#include "ifcparse/express.h"

#include <string>
#include <vector>

struct ifcopenshell_pset_template_t;
namespace IfcParse {
class IfcFile;
}

namespace ifcapi {
namespace bindings {

IFCAPI_BINDING void pset_template_set_template_dir(const std::string& dir);
IFCAPI_BINDING ifcopenshell_pset_template_t* pset_template_get_template(const std::string& schema_identifier);
IFCAPI_BINDING ifcopenshell_pset_template_t* pset_template_create_from_files(
    const std::string& schema_identifier,
    const std::vector<const ifcopenshell::file*>& template_files);
IFCAPI_BINDING void pset_template_free(ifcopenshell_pset_template_t* pqt);
IFCAPI_BINDING express::Base pset_template_get_by_name(
    ifcopenshell_pset_template_t* pqt,
    const std::string& name);
IFCAPI_BINDING bool pset_template_is_templated(ifcopenshell_pset_template_t* pqt, const std::string& name);
IFCAPI_BINDING IFCAPI_COPY std::vector<express::Base> pset_template_get_applicable(
    ifcopenshell_pset_template_t* pqt,
    const char* ifc_class,
    const char* predefined_type,
    bool pset_only,
    bool qto_only,
    const char* schema_name);
IFCAPI_BINDING std::vector<std::string> pset_template_get_applicable_names(
    ifcopenshell_pset_template_t* pqt,
    const char* ifc_class,
    const char* predefined_type,
    bool pset_only,
    bool qto_only,
    const char* schema_name);
IFCAPI_BINDING std::string pset_template_pset_type(express::Base* pset_template);
IFCAPI_BINDING express::Base pset_template_add_pset_template(
    ifcopenshell::file* file,
    const std::string& name,
    const std::string& template_type,
    const std::string& applicable_entity);
IFCAPI_BINDING express::Base pset_template_add_prop_template(
    ifcopenshell::file* file,
    express::Base* pset_template,
    const std::string& name,
    const char* description,
    const char* template_type,
    const char* primary_measure_type);
IFCAPI_BINDING void pset_template_remove_pset_template(
    express::Base* pset_template);
IFCAPI_BINDING void pset_template_remove_prop_template(
    ifcopenshell::file* file,
    express::Base* prop_template);

} // namespace bindings
} // namespace ifcapi

#endif /* IFCAPI_BINDINGS_PSET_TEMPLATE_H */
