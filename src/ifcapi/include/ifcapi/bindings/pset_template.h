/* This file was generated with the assistance of an AI coding tool. */

#ifndef IFCAPI_BINDINGS_PSET_TEMPLATE_H
#define IFCAPI_BINDINGS_PSET_TEMPLATE_H

#include "ifcapi/bindings/contract.h"

#include "ifcparse/express.h"
#include "ifcparse/file.h"

#include <string>
#include <vector>

struct ifcopenshell_pset_template_t;

namespace ifcapi {
namespace bindings {

/**
 * Set the directory used to locate built-in pset/qto template files.
 *
 * Must be called before pset_template_get_template to override the default
 * search path for IFC property set template files.
 */
IFCAPI_BINDING void pset_template_set_template_dir(const std::string& dir);

/**
 * Return the cached property template collection for the given schema (e.g. "IFC4", "IFC2X3").
 *
 * Loads and caches the built-in templates on first call. Returns no result
 * if the schema is unknown or templates are not available.
 */
IFCAPI_BINDING ifcopenshell_pset_template_t* pset_template_get_template(const std::string& schema_identifier);

/**
 * Create a property template collection from custom IFC template files.
 *
 * Loads IfcPropertySetTemplate and IfcSimplePropertyTemplate entities from
 * the provided files. The returned collection remains valid until it is
 * released with pset_template_free.
 */
IFCAPI_BINDING IFCAPI_OWNED ifcopenshell_pset_template_t* pset_template_create_from_files(
    const std::string& schema_identifier,
    const std::vector<const ifcopenshell::file*>& template_files);

/**
 * Release a template collection created by pset_template_create_from_files.
 */
void pset_template_free(ifcopenshell_pset_template_t* pqt);

/**
 * Look up a property set template by name.
 *
 * Returns the IfcPropertySetTemplate entity with the given name, or a
 * no result if the template is not found.
 */
IFCAPI_BINDING express::Base pset_template_get_by_name(
    ifcopenshell_pset_template_t* pqt,
    const std::string& name);

/**
 * Return true if a property set template with the given name exists.
 */
IFCAPI_BINDING bool pset_template_is_templated(ifcopenshell_pset_template_t* pqt, const std::string& name);

/**
 * Return property set templates applicable to an IFC class and predefined type.
 *
 * Filters by pset_only (PSET templates) or qto_only (QTO templates).
 * If neither flag is set, returns both types. When predefined_type or
 * schema_name is omitted, the default is used.
 */
IFCAPI_BINDING IFCAPI_COPY std::vector<express::Base> pset_template_get_applicable(
    ifcopenshell_pset_template_t* pqt,
    const char* ifc_class,
    const char* predefined_type,
    bool pset_only,
    bool qto_only,
    const char* schema_name);

/**
 * Return names of property set templates applicable to an IFC class.
 *
 * Same filtering as pset_template_get_applicable but returns only the
 * Name strings.
 */
IFCAPI_BINDING std::vector<std::string> pset_template_get_applicable_names(
    ifcopenshell_pset_template_t* pqt,
    const char* ifc_class,
    const char* predefined_type,
    bool pset_only,
    bool qto_only,
    const char* schema_name);

/**
 * Return the template type category for a property set template.
 *
 * Returns "PSET" for property set templates, "QTO" for quantity templates,
 * or an empty string if unknown.
 */
IFCAPI_BINDING std::string pset_template_pset_type(express::Base* pset_template);

/**
 * Create a new IfcPropertySetTemplate in the file.
 *
 * @param file The IFC file.
 * @param name Template name (e.g. "Pset_WallCommon").
 * @param template_type Template type string (e.g. "PSET_TYPEDRIVENOVERRIDE", "QTO").
 * @param applicable_entity Applicable IFC entity name (e.g. "IfcWall").
 */
IFCAPI_BINDING express::Base pset_template_add_pset_template(
    ifcopenshell::file* file,
    const std::string& name,
    const std::string& template_type,
    const std::string& applicable_entity);

/**
 * Create a new IfcSimplePropertyTemplate and add it to a property set template.
 *
 * Property templates are kept sorted by name within the parent template.
 * If template_type is omitted, defaults to "P_SINGLEVALUE" for psets or
 * "Q_LENGTH" for qtos. If primary_measure_type is omitted, defaults to
 * "IfcLabel" for psets.
 */
IFCAPI_BINDING express::Base pset_template_add_prop_template(
    ifcopenshell::file* file,
    express::Base* pset_template,
    const std::string& name,
    const char* description,
    const char* template_type,
    const char* primary_measure_type);

/**
 * Remove a property set template and its child property templates.
 *
 * Deletes the IfcPropertySetTemplate and all its child
 * IfcSimplePropertyTemplate entities.
 */
IFCAPI_BINDING void pset_template_remove_pset_template(
    express::Base* pset_template);

/**
 * Remove a property template from its parent set template.
 *
 * Removes the IfcSimplePropertyTemplate from its parent and deletes the
 * template entity.
 */
IFCAPI_BINDING void pset_template_remove_prop_template(
    ifcopenshell::file* file,
    express::Base* prop_template);

} // namespace bindings
} // namespace ifcapi

#endif /* IFCAPI_BINDINGS_PSET_TEMPLATE_H */
