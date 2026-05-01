/* This file was generated with the assistance of an AI coding tool. */

#ifndef IFCAPI_BINDINGS_ELEMENT_H
#define IFCAPI_BINDINGS_ELEMENT_H

#include "ifcapi/bindings/contract.h"

#include "ifcparse/IfcBaseClass.h"
#include "ifcparse/aggregate_of_instance.h"

namespace ifcapi {
namespace bindings {

IFCAPI_BINDING IfcUtil::IfcBaseClass* element_get_type(IfcUtil::IfcBaseClass* instance);
IFCAPI_BINDING IfcUtil::IfcBaseClass* element_get_aggregate(IfcUtil::IfcBaseClass* instance);
IFCAPI_BINDING IfcUtil::IfcBaseClass* element_get_nest(IfcUtil::IfcBaseClass* instance);
IFCAPI_BINDING IfcUtil::IfcBaseClass* element_get_container(
    IfcUtil::IfcBaseClass* instance,
    bool direct_only,
    const char* ifc_class);
IFCAPI_BINDING IfcUtil::IfcBaseClass* element_get_parent(IfcUtil::IfcBaseClass* instance);
IFCAPI_BINDING IfcUtil::IfcBaseClass* element_get_material(
    IfcUtil::IfcBaseClass* instance,
    bool should_skip_usage,
    bool should_inherit);
IFCAPI_BINDING aggregate_of_instance::ptr element_get_types(IfcUtil::IfcBaseClass* type_element);
IFCAPI_BINDING aggregate_of_instance::ptr element_get_shape_aspects(
    IfcUtil::IfcBaseClass* element,
    bool should_inherit);
IFCAPI_BINDING aggregate_of_instance::ptr element_get_groups(IfcUtil::IfcBaseClass* element);
IFCAPI_BINDING aggregate_of_instance::ptr element_get_controls(IfcUtil::IfcBaseClass* element);
IFCAPI_BINDING aggregate_of_instance::ptr element_get_parts(IfcUtil::IfcBaseClass* element);
IFCAPI_BINDING aggregate_of_instance::ptr element_get_contained(IfcUtil::IfcBaseClass* element);
IFCAPI_BINDING aggregate_of_instance::ptr element_get_referenced_structures(IfcUtil::IfcBaseClass* element);
IFCAPI_BINDING aggregate_of_instance::ptr element_get_structure_referenced_elements(IfcUtil::IfcBaseClass* structure);
IFCAPI_BINDING aggregate_of_instance::ptr element_get_openings(IfcUtil::IfcBaseClass* element);
IFCAPI_BINDING IfcUtil::IfcBaseClass* element_get_filled_void(IfcUtil::IfcBaseClass* element);
IFCAPI_BINDING IfcUtil::IfcBaseClass* element_get_voided_element(IfcUtil::IfcBaseClass* element);
IFCAPI_BINDING bool element_is_userdefined_type(IfcUtil::IfcBaseClass* element);
IFCAPI_BINDING aggregate_of_instance::ptr element_get_referenced_elements(IfcUtil::IfcBaseClass* reference);
IFCAPI_BINDING aggregate_of_instance::ptr element_get_elements_by_material(IfcUtil::IfcBaseClass* material);
IFCAPI_BINDING aggregate_of_instance::ptr element_get_elements_by_style(IfcUtil::IfcBaseClass* style);
IFCAPI_BINDING aggregate_of_instance::ptr element_get_elements_by_representation(IfcUtil::IfcBaseClass* representation);
IFCAPI_BINDING aggregate_of_instance::ptr element_get_elements_by_profile(IfcUtil::IfcBaseClass* profile);
IFCAPI_BINDING aggregate_of_instance::ptr element_get_elements_by_layer(IfcUtil::IfcBaseClass* layer);
IFCAPI_BINDING aggregate_of_instance::ptr element_get_layers(IfcUtil::IfcBaseClass* element);
IFCAPI_BINDING aggregate_of_instance::ptr element_get_styles(IfcUtil::IfcBaseClass* element);
IFCAPI_BINDING void element_replace_element(
    IfcUtil::IfcBaseClass* old_element,
    IfcUtil::IfcBaseClass* new_element);
IFCAPI_BINDING void element_remove_deep(IfcUtil::IfcBaseClass* element);
IFCAPI_BINDING aggregate_of_instance::ptr element_get_decomposition(
    IfcUtil::IfcBaseClass* element,
    bool is_recursive);
IFCAPI_BINDING aggregate_of_instance::ptr element_get_pset_ids(
    IfcUtil::IfcBaseClass* element,
    bool psets_only,
    bool qtos_only,
    bool should_inherit);

} // namespace bindings
} // namespace ifcapi

#endif /* IFCAPI_BINDINGS_ELEMENT_H */
