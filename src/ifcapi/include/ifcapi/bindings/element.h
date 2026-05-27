/* This file was generated with the assistance of an AI coding tool. */

#ifndef IFCAPI_BINDINGS_ELEMENT_H
#define IFCAPI_BINDINGS_ELEMENT_H

#include "ifcapi/bindings/contract.h"

#include "ifcparse/express.h"
#include <vector>

namespace ifcapi {
namespace bindings {

IFCAPI_BINDING express::Base element_get_type(express::Base* instance);
IFCAPI_BINDING express::Base element_get_aggregate(express::Base* instance);
IFCAPI_BINDING express::Base element_get_nest(express::Base* instance);
IFCAPI_BINDING express::Base element_get_container(
    express::Base* instance,
    bool direct_only,
    const char* ifc_class);
IFCAPI_BINDING express::Base element_get_parent(express::Base* instance);
IFCAPI_BINDING express::Base element_get_material(
    express::Base* instance,
    bool should_skip_usage,
    bool should_inherit);
IFCAPI_BINDING std::vector<express::Base> element_get_types(express::Base* type_element);
IFCAPI_BINDING std::vector<express::Base> element_get_shape_aspects(
    express::Base* element,
    bool should_inherit);
IFCAPI_BINDING std::vector<express::Base> element_get_groups(express::Base* element);
IFCAPI_BINDING std::vector<express::Base> element_get_controls(express::Base* element);
IFCAPI_BINDING std::vector<express::Base> element_get_parts(express::Base* element);
IFCAPI_BINDING std::vector<express::Base> element_get_contained(express::Base* element);
IFCAPI_BINDING std::vector<express::Base> element_get_referenced_structures(express::Base* element);
IFCAPI_BINDING std::vector<express::Base> element_get_structure_referenced_elements(express::Base* structure);
IFCAPI_BINDING std::vector<express::Base> element_get_openings(express::Base* element);
IFCAPI_BINDING express::Base element_get_filled_void(express::Base* element);
IFCAPI_BINDING express::Base element_get_voided_element(express::Base* element);
IFCAPI_BINDING bool element_is_userdefined_type(express::Base* element);
IFCAPI_BINDING std::vector<express::Base> element_get_referenced_elements(express::Base* reference);
IFCAPI_BINDING std::vector<express::Base> element_get_elements_by_material(express::Base* material);
IFCAPI_BINDING std::vector<express::Base> element_get_elements_by_style(express::Base* style);
IFCAPI_BINDING std::vector<express::Base> element_get_elements_by_representation(express::Base* representation);
IFCAPI_BINDING std::vector<express::Base> element_get_elements_by_profile(express::Base* profile);
IFCAPI_BINDING std::vector<express::Base> element_get_elements_by_layer(express::Base* layer);
IFCAPI_BINDING std::vector<express::Base> element_get_layers(express::Base* element);
IFCAPI_BINDING std::vector<express::Base> element_get_styles(express::Base* element);
IFCAPI_BINDING void element_replace_element(
    express::Base* old_element,
    express::Base* new_element);
IFCAPI_BINDING void element_remove_deep(express::Base* element);
IFCAPI_BINDING std::vector<express::Base> element_get_decomposition(
    express::Base* element,
    bool is_recursive);
IFCAPI_BINDING std::vector<express::Base> element_get_pset_ids(
    express::Base* element,
    bool psets_only,
    bool qtos_only,
    bool should_inherit);

} // namespace bindings
} // namespace ifcapi

#endif /* IFCAPI_BINDINGS_ELEMENT_H */
