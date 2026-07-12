#ifndef IFCOPENSHELL_API_H
#define IFCOPENSHELL_API_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Common types - guarded to allow multiple API headers to be included */
#ifndef IFCOPENSHELL_COMMON_TYPES_DEFINED
#define IFCOPENSHELL_COMMON_TYPES_DEFINED

typedef struct ifcopenshell_string_t {
    char* data;
    size_t size;
    bool owned;
} ifcopenshell_string_t;

typedef enum ifcopenshell_logical_t {
    IFCOPENSHELL_LOGICAL_UNKNOWN = -1,
    IFCOPENSHELL_LOGICAL_FALSE = 0,
    IFCOPENSHELL_LOGICAL_TRUE = 1
} ifcopenshell_logical_t;

typedef struct ifcopenshell_double_list_t {
    double* items;
    size_t size;
} ifcopenshell_double_list_t;

typedef struct ifcopenshell_string_list_t {
    ifcopenshell_string_t* items;
    size_t size;
} ifcopenshell_string_list_t;

typedef struct ifcopenshell_double_list_list_t {
    ifcopenshell_double_list_t* items;
    size_t size;
} ifcopenshell_double_list_list_t;

typedef struct ifcopenshell_int64_list_t {
    int64_t* items;
    size_t size;
} ifcopenshell_int64_list_t;

typedef struct ifcopenshell_int32_list_t {
    int32_t* items;
    size_t size;
} ifcopenshell_int32_list_t;

typedef struct ifcopenshell_int32_list_list_t {
    ifcopenshell_int32_list_t* items;
    size_t size;
} ifcopenshell_int32_list_list_t;

typedef struct ifcopenshell_int32_list_list_list_t {
    ifcopenshell_int32_list_list_t* items;
    size_t size;
} ifcopenshell_int32_list_list_list_t;

typedef struct ifcopenshell_bool_list_t {
    bool* items;
    size_t size;
} ifcopenshell_bool_list_t;

typedef struct ifcopenshell_uint32_list_t {
    uint32_t* items;
    size_t size;
} ifcopenshell_uint32_list_t;

typedef struct ifcopenshell_uint8_list_t {
    uint8_t* items;
    size_t size;
} ifcopenshell_uint8_list_t;

typedef struct ifcopenshell_double_list_list_list_t {
    ifcopenshell_double_list_list_t* items;
    size_t size;
} ifcopenshell_double_list_list_list_t;

typedef struct ifcopenshell_int32_list_list_list_list_t {
    ifcopenshell_int32_list_list_list_t* items;
    size_t size;
} ifcopenshell_int32_list_list_list_list_t;

void ifcopenshell_string_destroy(ifcopenshell_string_t* value);

void ifcopenshell_double_list_destroy(ifcopenshell_double_list_t* value);

void ifcopenshell_string_list_destroy(ifcopenshell_string_list_t* value);

void ifcopenshell_double_list_list_destroy(ifcopenshell_double_list_list_t* value);

void ifcopenshell_int64_list_destroy(ifcopenshell_int64_list_t* value);

void ifcopenshell_int32_list_destroy(ifcopenshell_int32_list_t* value);

void ifcopenshell_int32_list_list_destroy(ifcopenshell_int32_list_list_t* value);

void ifcopenshell_int32_list_list_list_destroy(ifcopenshell_int32_list_list_list_t* value);

void ifcopenshell_bool_list_destroy(ifcopenshell_bool_list_t* value);

void ifcopenshell_uint32_list_destroy(ifcopenshell_uint32_list_t* value);

void ifcopenshell_uint8_list_destroy(ifcopenshell_uint8_list_t* value);

void ifcopenshell_double_list_list_list_destroy(ifcopenshell_double_list_list_list_t* value);

void ifcopenshell_int32_list_list_list_list_destroy(ifcopenshell_int32_list_list_list_list_t* value);

#endif /* IFCOPENSHELL_COMMON_TYPES_DEFINED */

typedef struct ifcopenshell_file_t ifcopenshell_file_t;
typedef struct ifcopenshell_instance_streamer_t ifcopenshell_instance_streamer_t;
typedef struct ifcopenshell_instance_t ifcopenshell_instance_t;
typedef struct ifcopenshell_header_t ifcopenshell_header_t;
typedef struct ifcopenshell_file_description_t ifcopenshell_file_description_t;
typedef struct ifcopenshell_file_name_t ifcopenshell_file_name_t;
typedef struct ifcopenshell_file_schema_t ifcopenshell_file_schema_t;
typedef struct ifcopenshell_declaration_t ifcopenshell_declaration_t;
typedef struct ifcopenshell_type_declaration_t ifcopenshell_type_declaration_t;
typedef struct ifcopenshell_select_type_t ifcopenshell_select_type_t;
typedef struct ifcopenshell_schema_t ifcopenshell_schema_t;
typedef struct ifcopenshell_enumeration_t ifcopenshell_enumeration_t;
typedef struct ifcopenshell_parameter_type_t ifcopenshell_parameter_type_t;
typedef struct ifcopenshell_named_type_t ifcopenshell_named_type_t;
typedef struct ifcopenshell_simple_type_t ifcopenshell_simple_type_t;
typedef struct ifcopenshell_aggregation_type_t ifcopenshell_aggregation_type_t;
typedef struct ifcopenshell_entity_t ifcopenshell_entity_t;
typedef struct ifcopenshell_attribute_t ifcopenshell_attribute_t;
typedef struct ifcopenshell_inverse_attribute_t ifcopenshell_inverse_attribute_t;
typedef struct ifcopenshell_parse_attribute_value_t ifcopenshell_parse_attribute_value_t;
typedef struct ifcopenshell_parse_instance_list_t ifcopenshell_parse_instance_list_t;
typedef struct ifcopenshell_value_t ifcopenshell_value_t;
typedef struct ifcopenshell_pset_template_handle_t ifcopenshell_pset_template_handle_t;
typedef struct ifcopenshell_geom_iterator_t ifcopenshell_geom_iterator_t;
typedef struct ifcopenshell_geom_settings_t ifcopenshell_geom_settings_t;
typedef struct ifcopenshell_geom_serializer_settings_t ifcopenshell_geom_serializer_settings_t;
typedef struct ifcopenshell_geom_geometry_serializer_t ifcopenshell_geom_geometry_serializer_t;
typedef struct ifcopenshell_geom_serializer_t ifcopenshell_geom_serializer_t;
typedef struct ifcopenshell_geom_buffer_t ifcopenshell_geom_buffer_t;
typedef struct ifcopenshell_geom_tree_t ifcopenshell_geom_tree_t;
typedef struct ifcopenshell_geom_tree_clash_list_t ifcopenshell_geom_tree_clash_list_t;
typedef struct ifcopenshell_geom_tree_clash_t ifcopenshell_geom_tree_clash_t;
typedef struct ifcopenshell_geom_tree_ray_intersection_list_t ifcopenshell_geom_tree_ray_intersection_list_t;
typedef struct ifcopenshell_geom_tree_ray_intersection_t ifcopenshell_geom_tree_ray_intersection_t;
typedef struct ifcopenshell_geom_transformation_t ifcopenshell_geom_transformation_t;
typedef struct ifcopenshell_geom_element_t ifcopenshell_geom_element_t;
typedef struct ifcopenshell_geom_brep_element_t ifcopenshell_geom_brep_element_t;
typedef struct ifcopenshell_geom_triangulation_element_t ifcopenshell_geom_triangulation_element_t;
typedef struct ifcopenshell_geom_serialized_element_t ifcopenshell_geom_serialized_element_t;
typedef struct ifcopenshell_geom_triangulation_t ifcopenshell_geom_triangulation_t;
typedef struct ifcopenshell_geom_brep_representation_t ifcopenshell_geom_brep_representation_t;
typedef struct ifcopenshell_geom_serialization_t ifcopenshell_geom_serialization_t;
typedef struct ifcopenshell_geom_conversion_result_shape_t ifcopenshell_geom_conversion_result_shape_t;
typedef struct ifcopenshell_geom_opaque_number_t ifcopenshell_geom_opaque_number_t;
typedef struct ifcopenshell_geom_svgfill_polygon_t ifcopenshell_geom_svgfill_polygon_t;
typedef struct ifcopenshell_geom_function_item_evaluator_t ifcopenshell_geom_function_item_evaluator_t;
typedef struct ifcopenshell_geom_taxonomy_item_t ifcopenshell_geom_taxonomy_item_t;
typedef struct ifcopenshell_geom_taxonomy_matrix4_t ifcopenshell_geom_taxonomy_matrix4_t;
typedef struct ifcopenshell_geom_taxonomy_point3_t ifcopenshell_geom_taxonomy_point3_t;
typedef struct ifcopenshell_geom_taxonomy_direction3_t ifcopenshell_geom_taxonomy_direction3_t;
typedef struct ifcopenshell_geom_taxonomy_style_t ifcopenshell_geom_taxonomy_style_t;
typedef struct ifcopenshell_geom_taxonomy_colour_t ifcopenshell_geom_taxonomy_colour_t;
typedef struct ifcopenshell_geom_taxonomy_line_t ifcopenshell_geom_taxonomy_line_t;
typedef struct ifcopenshell_geom_taxonomy_circle_t ifcopenshell_geom_taxonomy_circle_t;
typedef struct ifcopenshell_geom_taxonomy_ellipse_t ifcopenshell_geom_taxonomy_ellipse_t;
typedef struct ifcopenshell_geom_taxonomy_bspline_curve_t ifcopenshell_geom_taxonomy_bspline_curve_t;
typedef struct ifcopenshell_geom_taxonomy_offset_curve_t ifcopenshell_geom_taxonomy_offset_curve_t;
typedef struct ifcopenshell_geom_taxonomy_edge_t ifcopenshell_geom_taxonomy_edge_t;
typedef struct ifcopenshell_geom_taxonomy_loop_t ifcopenshell_geom_taxonomy_loop_t;
typedef struct ifcopenshell_geom_taxonomy_face_t ifcopenshell_geom_taxonomy_face_t;
typedef struct ifcopenshell_geom_taxonomy_shell_t ifcopenshell_geom_taxonomy_shell_t;
typedef struct ifcopenshell_geom_taxonomy_solid_t ifcopenshell_geom_taxonomy_solid_t;
typedef struct ifcopenshell_geom_taxonomy_plane_t ifcopenshell_geom_taxonomy_plane_t;
typedef struct ifcopenshell_geom_taxonomy_cylinder_t ifcopenshell_geom_taxonomy_cylinder_t;
typedef struct ifcopenshell_geom_taxonomy_sphere_t ifcopenshell_geom_taxonomy_sphere_t;
typedef struct ifcopenshell_geom_taxonomy_torus_t ifcopenshell_geom_taxonomy_torus_t;
typedef struct ifcopenshell_geom_taxonomy_bspline_surface_t ifcopenshell_geom_taxonomy_bspline_surface_t;
typedef struct ifcopenshell_geom_taxonomy_collection_t ifcopenshell_geom_taxonomy_collection_t;
typedef struct ifcopenshell_geom_taxonomy_loft_t ifcopenshell_geom_taxonomy_loft_t;
typedef struct ifcopenshell_geom_taxonomy_extrusion_t ifcopenshell_geom_taxonomy_extrusion_t;
typedef struct ifcopenshell_geom_taxonomy_revolve_t ifcopenshell_geom_taxonomy_revolve_t;
typedef struct ifcopenshell_geom_taxonomy_sweep_along_curve_t ifcopenshell_geom_taxonomy_sweep_along_curve_t;
typedef struct ifcopenshell_geom_taxonomy_node_t ifcopenshell_geom_taxonomy_node_t;
typedef struct ifcopenshell_geom_taxonomy_boolean_result_t ifcopenshell_geom_taxonomy_boolean_result_t;

typedef struct ifcopenshell_instance_list_t {
    ifcopenshell_instance_t** items;
    size_t size;
} ifcopenshell_instance_list_t;
typedef struct ifcopenshell_file_list_t {
    ifcopenshell_file_t** items;
    size_t size;
} ifcopenshell_file_list_t;
typedef struct ifcopenshell_geom_svgfill_polygon_list_t {
    ifcopenshell_geom_svgfill_polygon_t** items;
    size_t size;
} ifcopenshell_geom_svgfill_polygon_list_t;
typedef struct ifcopenshell_geom_conversion_result_shape_list_t {
    ifcopenshell_geom_conversion_result_shape_t** items;
    size_t size;
} ifcopenshell_geom_conversion_result_shape_list_t;
typedef struct ifcopenshell_declaration_list_t {
    ifcopenshell_declaration_t** items;
    size_t size;
} ifcopenshell_declaration_list_t;
typedef struct ifcopenshell_entity_list_t {
    ifcopenshell_entity_t** items;
    size_t size;
} ifcopenshell_entity_list_t;
typedef struct ifcopenshell_enumeration_list_t {
    ifcopenshell_enumeration_t** items;
    size_t size;
} ifcopenshell_enumeration_list_t;
typedef struct ifcopenshell_select_type_list_t {
    ifcopenshell_select_type_t** items;
    size_t size;
} ifcopenshell_select_type_list_t;
typedef struct ifcopenshell_type_declaration_list_t {
    ifcopenshell_type_declaration_t** items;
    size_t size;
} ifcopenshell_type_declaration_list_t;
typedef struct ifcopenshell_attribute_list_t {
    ifcopenshell_attribute_t** items;
    size_t size;
} ifcopenshell_attribute_list_t;
typedef struct ifcopenshell_inverse_attribute_list_t {
    ifcopenshell_inverse_attribute_t** items;
    size_t size;
} ifcopenshell_inverse_attribute_list_t;
typedef struct ifcopenshell_geom_taxonomy_style_list_t {
    ifcopenshell_geom_taxonomy_style_t** items;
    size_t size;
} ifcopenshell_geom_taxonomy_style_list_t;
typedef struct ifcopenshell_geom_taxonomy_item_list_t {
    ifcopenshell_geom_taxonomy_item_t** items;
    size_t size;
} ifcopenshell_geom_taxonomy_item_list_t;
typedef struct ifcopenshell_geom_element_list_t {
    ifcopenshell_geom_element_t** items;
    size_t size;
} ifcopenshell_geom_element_list_t;
typedef struct ifcopenshell_instance_list_list_t {
    ifcopenshell_instance_list_t* items;
    size_t size;
} ifcopenshell_instance_list_list_t;
typedef struct ifcopenshell_file_list_list_t {
    ifcopenshell_file_list_t* items;
    size_t size;
} ifcopenshell_file_list_list_t;
typedef struct ifcopenshell_geom_svgfill_polygon_list_list_t {
    ifcopenshell_geom_svgfill_polygon_list_t* items;
    size_t size;
} ifcopenshell_geom_svgfill_polygon_list_list_t;
typedef struct ifcopenshell_geom_conversion_result_shape_list_list_t {
    ifcopenshell_geom_conversion_result_shape_list_t* items;
    size_t size;
} ifcopenshell_geom_conversion_result_shape_list_list_t;
typedef struct ifcopenshell_declaration_list_list_t {
    ifcopenshell_declaration_list_t* items;
    size_t size;
} ifcopenshell_declaration_list_list_t;
typedef struct ifcopenshell_entity_list_list_t {
    ifcopenshell_entity_list_t* items;
    size_t size;
} ifcopenshell_entity_list_list_t;
typedef struct ifcopenshell_enumeration_list_list_t {
    ifcopenshell_enumeration_list_t* items;
    size_t size;
} ifcopenshell_enumeration_list_list_t;
typedef struct ifcopenshell_select_type_list_list_t {
    ifcopenshell_select_type_list_t* items;
    size_t size;
} ifcopenshell_select_type_list_list_t;
typedef struct ifcopenshell_type_declaration_list_list_t {
    ifcopenshell_type_declaration_list_t* items;
    size_t size;
} ifcopenshell_type_declaration_list_list_t;
typedef struct ifcopenshell_attribute_list_list_t {
    ifcopenshell_attribute_list_t* items;
    size_t size;
} ifcopenshell_attribute_list_list_t;
typedef struct ifcopenshell_inverse_attribute_list_list_t {
    ifcopenshell_inverse_attribute_list_t* items;
    size_t size;
} ifcopenshell_inverse_attribute_list_list_t;
typedef struct ifcopenshell_geom_taxonomy_style_list_list_t {
    ifcopenshell_geom_taxonomy_style_list_t* items;
    size_t size;
} ifcopenshell_geom_taxonomy_style_list_list_t;
typedef struct ifcopenshell_geom_taxonomy_item_list_list_t {
    ifcopenshell_geom_taxonomy_item_list_t* items;
    size_t size;
} ifcopenshell_geom_taxonomy_item_list_list_t;
typedef struct ifcopenshell_geom_element_list_list_t {
    ifcopenshell_geom_element_list_t* items;
    size_t size;
} ifcopenshell_geom_element_list_list_t;

typedef struct ifcopenshell_aggregate_assign_object_options_t {
    ifcopenshell_parse_instance_list_t* products;
    ifcopenshell_instance_t* relating_object;
    ifcopenshell_instance_t* owner_history;
    bool has_owner_history;
    ifcopenshell_instance_t* user;
    bool has_user;
    ifcopenshell_instance_t* application;
    bool has_application;
} ifcopenshell_aggregate_assign_object_options_t;

typedef struct ifcopenshell_aggregate_unassign_object_options_t {
    ifcopenshell_parse_instance_list_t* products;
    ifcopenshell_instance_t* user;
    bool has_user;
    ifcopenshell_instance_t* application;
    bool has_application;
} ifcopenshell_aggregate_unassign_object_options_t;

typedef struct ifcopenshell_attribute_edit_attributes_options_t {
    ifcopenshell_instance_t* product;
    void* attributes;
    bool sync_predefined_type;
    bool update_owner_history;
    ifcopenshell_instance_t* user;
    bool has_user;
    ifcopenshell_instance_t* application;
    bool has_application;
} ifcopenshell_attribute_edit_attributes_options_t;

typedef struct ifcopenshell_boundary_assign_connection_geometry_options_t {
    const ifcopenshell_double_list_list_t* outer_boundary;
    const ifcopenshell_double_list_t* location;
    const ifcopenshell_double_list_t* axis;
    const ifcopenshell_double_list_t* ref_direction;
    const ifcopenshell_double_list_list_list_t* inner_boundaries;
    double unit_scale;
} ifcopenshell_boundary_assign_connection_geometry_options_t;

typedef struct ifcopenshell_boundary_edit_attributes_options_t {
    ifcopenshell_instance_t* relating_space;
    ifcopenshell_instance_t* related_building_element;
    ifcopenshell_instance_t* parent_boundary;
    bool has_parent_boundary;
    ifcopenshell_instance_t* corresponding_boundary;
    bool has_corresponding_boundary;
    const char* physical_or_virtual;
    const char* internal_or_external;
} ifcopenshell_boundary_edit_attributes_options_t;

typedef struct ifcopenshell_classification_add_reference_options_t {
    ifcopenshell_parse_instance_list_t* products;
    ifcopenshell_instance_t* reference;
    bool has_reference;
    const char* identification;
    bool has_identification;
    const char* name;
    bool has_name;
    ifcopenshell_instance_t* classification;
    bool has_classification;
    ifcopenshell_instance_t* owner_history;
    bool has_owner_history;
    ifcopenshell_instance_t* user;
    bool has_user;
    ifcopenshell_instance_t* application;
    bool has_application;
} ifcopenshell_classification_add_reference_options_t;

typedef struct ifcopenshell_classification_remove_reference_options_t {
    ifcopenshell_instance_t* reference;
    ifcopenshell_parse_instance_list_t* products;
    ifcopenshell_instance_t* user;
    bool has_user;
    ifcopenshell_instance_t* application;
    bool has_application;
} ifcopenshell_classification_remove_reference_options_t;

typedef struct ifcopenshell_cogo_add_survey_point_options_t {
    ifcopenshell_instance_t* survey_point;
    ifcopenshell_instance_t* site;
    bool has_site;
    ifcopenshell_instance_t* owner_history;
    bool has_owner_history;
    ifcopenshell_instance_t* user;
    bool has_user;
    ifcopenshell_instance_t* application;
    bool has_application;
} ifcopenshell_cogo_add_survey_point_options_t;

typedef struct ifcopenshell_constraint_assign_constraint_options_t {
    ifcopenshell_parse_instance_list_t* products;
    ifcopenshell_instance_t* constraint;
    ifcopenshell_instance_t* owner_history;
    bool has_owner_history;
    ifcopenshell_instance_t* user;
    bool has_user;
    ifcopenshell_instance_t* application;
    bool has_application;
} ifcopenshell_constraint_assign_constraint_options_t;

typedef struct ifcopenshell_constraint_unassign_constraint_options_t {
    ifcopenshell_parse_instance_list_t* products;
    ifcopenshell_instance_t* constraint;
    ifcopenshell_instance_t* user;
    bool has_user;
    ifcopenshell_instance_t* application;
    bool has_application;
} ifcopenshell_constraint_unassign_constraint_options_t;

typedef struct ifcopenshell_context_add_context_options_t {
    const char* context_type;
    const char* context_identifier;
    const char* target_view;
    double target_scale;
    bool has_target_scale;
    ifcopenshell_instance_t* parent;
    bool has_parent;
} ifcopenshell_context_add_context_options_t;

typedef struct ifcopenshell_control_assign_control_options_t {
    ifcopenshell_instance_t* relating_control;
    ifcopenshell_parse_instance_list_t* related_objects;
    ifcopenshell_instance_t* owner_history;
    bool has_owner_history;
    ifcopenshell_instance_t* user;
    bool has_user;
    ifcopenshell_instance_t* application;
    bool has_application;
} ifcopenshell_control_assign_control_options_t;

typedef struct ifcopenshell_control_unassign_control_options_t {
    ifcopenshell_instance_t* relating_control;
    ifcopenshell_parse_instance_list_t* related_objects;
    ifcopenshell_instance_t* user;
    bool has_user;
    ifcopenshell_instance_t* application;
    bool has_application;
} ifcopenshell_control_unassign_control_options_t;

typedef struct ifcopenshell_cost_add_cost_item_options_t {
    ifcopenshell_instance_t* cost_schedule;
    bool has_cost_schedule;
    ifcopenshell_instance_t* cost_item;
    bool has_cost_item;
    ifcopenshell_instance_t* owner_history;
    bool has_owner_history;
    ifcopenshell_instance_t* user;
    bool has_user;
    ifcopenshell_instance_t* application;
    bool has_application;
} ifcopenshell_cost_add_cost_item_options_t;

typedef struct ifcopenshell_cost_assign_cost_item_quantity_options_t {
    ifcopenshell_instance_t* owner_history;
    bool has_owner_history;
    ifcopenshell_instance_t* user;
    bool has_user;
    ifcopenshell_instance_t* application;
    bool has_application;
} ifcopenshell_cost_assign_cost_item_quantity_options_t;

typedef struct ifcopenshell_cost_copy_cost_schedule_options_t {
    ifcopenshell_instance_t* owner_history;
    bool has_owner_history;
    ifcopenshell_instance_t* user;
    bool has_user;
    ifcopenshell_instance_t* application;
    bool has_application;
} ifcopenshell_cost_copy_cost_schedule_options_t;

typedef struct ifcopenshell_cost_edit_cost_value_options_t {
    bool edit_unit_basis;
    bool clear_unit_basis;
    double value_component;
    ifcopenshell_instance_t* unit_component;
    bool has_unit_component;
} ifcopenshell_cost_edit_cost_value_options_t;

typedef struct ifcopenshell_cost_unassign_cost_item_quantity_options_t {
    ifcopenshell_instance_t* user;
    bool has_user;
    ifcopenshell_instance_t* application;
    bool has_application;
} ifcopenshell_cost_unassign_cost_item_quantity_options_t;

typedef struct ifcopenshell_document_add_information_options_t {
    ifcopenshell_instance_t* parent;
    bool has_parent;
    ifcopenshell_instance_t* owner_history;
    bool has_owner_history;
    ifcopenshell_instance_t* user;
    bool has_user;
    ifcopenshell_instance_t* application;
    bool has_application;
} ifcopenshell_document_add_information_options_t;

typedef struct ifcopenshell_document_assign_document_options_t {
    ifcopenshell_parse_instance_list_t* products;
    ifcopenshell_instance_t* document;
    ifcopenshell_instance_t* owner_history;
    bool has_owner_history;
    ifcopenshell_instance_t* user;
    bool has_user;
    ifcopenshell_instance_t* application;
    bool has_application;
} ifcopenshell_document_assign_document_options_t;

typedef struct ifcopenshell_document_unassign_document_options_t {
    ifcopenshell_parse_instance_list_t* products;
    ifcopenshell_instance_t* document;
    ifcopenshell_instance_t* user;
    bool has_user;
    ifcopenshell_instance_t* application;
    bool has_application;
} ifcopenshell_document_unassign_document_options_t;

typedef struct ifcopenshell_drawing_assign_product_options_t {
    ifcopenshell_instance_t* relating_product;
    ifcopenshell_instance_t* related_object;
    ifcopenshell_instance_t* owner_history;
    bool has_owner_history;
    ifcopenshell_instance_t* user;
    bool has_user;
    ifcopenshell_instance_t* application;
    bool has_application;
} ifcopenshell_drawing_assign_product_options_t;

typedef struct ifcopenshell_drawing_unassign_product_options_t {
    ifcopenshell_instance_t* relating_product;
    ifcopenshell_instance_t* related_object;
    ifcopenshell_instance_t* user;
    bool has_user;
    ifcopenshell_instance_t* application;
    bool has_application;
} ifcopenshell_drawing_unassign_product_options_t;

typedef struct ifcopenshell_element_get_container_options_t {
    bool direct_only;
    bool has_direct_only;
    const char* ifc_class;
    bool has_ifc_class;
} ifcopenshell_element_get_container_options_t;

typedef struct ifcopenshell_element_get_decomposition_options_t {
    bool is_recursive;
    bool has_is_recursive;
} ifcopenshell_element_get_decomposition_options_t;

typedef struct ifcopenshell_element_get_material_options_t {
    bool should_skip_usage;
    bool has_should_skip_usage;
    bool should_inherit;
    bool has_should_inherit;
} ifcopenshell_element_get_material_options_t;

typedef struct ifcopenshell_element_get_pset_ids_options_t {
    bool psets_only;
    bool has_psets_only;
    bool qtos_only;
    bool has_qtos_only;
    bool should_inherit;
    bool has_should_inherit;
} ifcopenshell_element_get_pset_ids_options_t;

typedef struct ifcopenshell_element_get_shape_aspects_options_t {
    bool should_inherit;
    bool has_should_inherit;
} ifcopenshell_element_get_shape_aspects_options_t;

typedef struct ifcopenshell_entity_remove_deep_options_t {
    ifcopenshell_parse_instance_list_t* also_consider;
    ifcopenshell_parse_instance_list_t* do_not_delete;
} ifcopenshell_entity_remove_deep_options_t;

typedef struct ifcopenshell_feature_add_feature_options_t {
    ifcopenshell_instance_t* feature;
    ifcopenshell_instance_t* element;
    ifcopenshell_instance_t* user;
    bool has_user;
    ifcopenshell_instance_t* application;
    bool has_application;
} ifcopenshell_feature_add_feature_options_t;

typedef struct ifcopenshell_feature_remove_feature_options_t {
    ifcopenshell_instance_t* feature;
    ifcopenshell_instance_t* user;
    bool has_user;
    ifcopenshell_instance_t* application;
    bool has_application;
} ifcopenshell_feature_remove_feature_options_t;

typedef struct ifcopenshell_geometry_add_door_representation_options_t {
    ifcopenshell_instance_t* context;
    double overall_height;
    double overall_width;
    const char* operation_type;
    const ifcopenshell_double_list_t* lining_properties;
    const ifcopenshell_double_list_t* panel_properties;
    ifcopenshell_instance_t* part_of_product;
    bool has_part_of_product;
    double unit_scale;
} ifcopenshell_geometry_add_door_representation_options_t;

typedef struct ifcopenshell_geometry_add_mesh_representation_options_t {
    const ifcopenshell_double_list_list_list_t* vertices;
    const ifcopenshell_int32_list_list_list_list_t* faces;
    bool force_faceted_brep;
    bool has_force_faceted_brep;
} ifcopenshell_geometry_add_mesh_representation_options_t;

typedef struct ifcopenshell_geometry_add_railing_representation_options_t {
    ifcopenshell_instance_t* context;
    const ifcopenshell_double_list_list_t* railing_path;
    bool use_manual_supports;
    double support_spacing;
    double railing_diameter;
    double clear_width;
    const char* terminal_type;
    double height;
    bool looped_path;
    double unit_scale;
} ifcopenshell_geometry_add_railing_representation_options_t;

typedef struct ifcopenshell_geometry_add_shape_aspect_options_t {
    const char* name;
    ifcopenshell_parse_instance_list_t* items;
    ifcopenshell_instance_t* representation;
    ifcopenshell_instance_t* part_of_product;
    const char* description;
    bool has_description;
} ifcopenshell_geometry_add_shape_aspect_options_t;

typedef struct ifcopenshell_geometry_add_slab_representation_options_t {
    ifcopenshell_instance_t* context;
    double depth;
    const char* direction_sense;
    double offset;
    double x_angle;
    const ifcopenshell_int32_list_t* clipping_kinds;
    const ifcopenshell_double_list_list_t* clipping_locations;
    const ifcopenshell_double_list_list_t* clipping_normals;
    ifcopenshell_parse_instance_list_t* clipping_entities;
    const ifcopenshell_double_list_list_t* polyline;
    bool has_polyline;
} ifcopenshell_geometry_add_slab_representation_options_t;

typedef struct ifcopenshell_geometry_add_topology_representation_options_t {
    ifcopenshell_instance_t* context;
    ifcopenshell_instance_t* item;
    const char* representation_identifier;
    bool has_representation_identifier;
    const char* representation_type;
    bool has_representation_type;
} ifcopenshell_geometry_add_topology_representation_options_t;

typedef struct ifcopenshell_geometry_add_wall_representation_options_t {
    ifcopenshell_instance_t* context;
    double length;
    double height;
    const char* direction_sense;
    double offset;
    double thickness;
    double x_angle;
    const ifcopenshell_int32_list_t* clipping_kinds;
    const ifcopenshell_double_list_list_t* clipping_locations;
    const ifcopenshell_double_list_list_t* clipping_normals;
    ifcopenshell_parse_instance_list_t* clipping_entities;
    ifcopenshell_parse_instance_list_t* booleans;
} ifcopenshell_geometry_add_wall_representation_options_t;

typedef struct ifcopenshell_geometry_add_window_representation_options_t {
    ifcopenshell_instance_t* context;
    double overall_height;
    double overall_width;
    const ifcopenshell_int32_list_list_t* panel_schema;
    const ifcopenshell_double_list_t* lining_properties;
    const ifcopenshell_double_list_list_t* panel_properties;
    ifcopenshell_instance_t* part_of_product;
    bool has_part_of_product;
    double glass_thickness;
} ifcopenshell_geometry_add_window_representation_options_t;

typedef struct ifcopenshell_geometry_clip_solid_options_t {
    ifcopenshell_instance_t* item;
    const ifcopenshell_double_list_t* location;
    const ifcopenshell_double_list_t* normal;
    ifcopenshell_instance_t* element;
    bool has_element;
    ifcopenshell_instance_t* owner_history;
    bool has_owner_history;
    ifcopenshell_instance_t* user;
    bool has_user;
    ifcopenshell_instance_t* application;
    bool has_application;
} ifcopenshell_geometry_clip_solid_options_t;

typedef struct ifcopenshell_geometry_clip_solid_bounded_options_t {
    ifcopenshell_instance_t* item;
    const ifcopenshell_double_list_t* location;
    const ifcopenshell_double_list_t* normal;
    const ifcopenshell_double_list_list_t* boundary_points;
    const ifcopenshell_double_list_t* boundary_position;
    ifcopenshell_instance_t* element;
    bool has_element;
    ifcopenshell_instance_t* owner_history;
    bool has_owner_history;
    ifcopenshell_instance_t* user;
    bool has_user;
    ifcopenshell_instance_t* application;
    bool has_application;
} ifcopenshell_geometry_clip_solid_bounded_options_t;

typedef struct ifcopenshell_geometry_connect_element_options_t {
    ifcopenshell_instance_t* relating_element;
    ifcopenshell_instance_t* related_element;
    const char* description;
    bool has_description;
    ifcopenshell_instance_t* owner_history;
    bool has_owner_history;
    ifcopenshell_instance_t* user;
    bool has_user;
    ifcopenshell_instance_t* application;
    bool has_application;
} ifcopenshell_geometry_connect_element_options_t;

typedef struct ifcopenshell_geometry_connect_path_options_t {
    ifcopenshell_instance_t* relating_element;
    ifcopenshell_instance_t* related_element;
    const char* relating_connection;
    const char* related_connection;
    const char* description;
    bool has_description;
    ifcopenshell_instance_t* connection_geometry;
    bool has_connection_geometry;
    ifcopenshell_instance_t* owner_history;
    bool has_owner_history;
    ifcopenshell_instance_t* user;
    bool has_user;
    ifcopenshell_instance_t* application;
    bool has_application;
} ifcopenshell_geometry_connect_path_options_t;

typedef struct ifcopenshell_geometry_connect_wall_options_t {
    ifcopenshell_instance_t* first_wall;
    ifcopenshell_instance_t* second_wall;
    bool is_atpath;
    ifcopenshell_instance_t* owner_history;
    bool has_owner_history;
    ifcopenshell_instance_t* user;
    bool has_user;
    ifcopenshell_instance_t* application;
    bool has_application;
} ifcopenshell_geometry_connect_wall_options_t;

typedef struct ifcopenshell_geometry_copy_representation_options_t {
    ifcopenshell_instance_t* source;
    ifcopenshell_instance_t* target;
    const char* context_identifier;
    bool has_context_identifier;
} ifcopenshell_geometry_copy_representation_options_t;

typedef struct ifcopenshell_geometry_create2_pt_wall_options_t {
    ifcopenshell_instance_t* element;
    ifcopenshell_instance_t* context;
    const ifcopenshell_double_list_t* start;
    const ifcopenshell_double_list_t* end;
    double elevation;
    double height;
    double thickness;
    bool is_si;
} ifcopenshell_geometry_create2_pt_wall_options_t;

typedef struct ifcopenshell_geometry_disconnect_path_options_t {
    ifcopenshell_instance_t* element;
    bool has_element;
    const char* connection_type;
    bool has_connection_type;
    ifcopenshell_instance_t* relating_element;
    bool has_relating_element;
    ifcopenshell_instance_t* related_element;
    bool has_related_element;
} ifcopenshell_geometry_disconnect_path_options_t;

typedef struct ifcopenshell_geometry_edit_object_placement_options_t {
    ifcopenshell_instance_t* product;
    const ifcopenshell_double_list_t* matrix;
    bool is_si;
    bool should_transform_children;
} ifcopenshell_geometry_edit_object_placement_options_t;

typedef struct ifcopenshell_geometry_regenerate_wall_representation_options_t {
    ifcopenshell_instance_t* wall;
    double length;
    double height;
    double angle;
    bool has_angle;
} ifcopenshell_geometry_regenerate_wall_representation_options_t;

typedef struct ifcopenshell_geometry_remove_representation_options_t {
    bool should_keep_named_profiles;
    bool has_should_keep_named_profiles;
} ifcopenshell_geometry_remove_representation_options_t;

typedef struct ifcopenshell_geometry_validate_type_options_t {
    ifcopenshell_instance_t* preferred_item;
    bool has_preferred_item;
} ifcopenshell_geometry_validate_type_options_t;

typedef struct ifcopenshell_georeference_add_georeferencing_options_t {
    const char* ifc_class;
    const char* name;
    ifcopenshell_instance_t* owner_history;
    bool has_owner_history;
    ifcopenshell_instance_t* user;
    bool has_user;
    ifcopenshell_instance_t* application;
    bool has_application;
} ifcopenshell_georeference_add_georeferencing_options_t;

typedef struct ifcopenshell_georeference_edit_georeferencing_options_t {
    void* coordinate_operation;
    bool has_coordinate_operation;
    void* projected_crs;
    bool has_projected_crs;
} ifcopenshell_georeference_edit_georeferencing_options_t;

typedef struct ifcopenshell_georeference_edit_true_north_options_t {
    const ifcopenshell_double_list_t* true_north;
    bool has_true_north;
} ifcopenshell_georeference_edit_true_north_options_t;

typedef struct ifcopenshell_georeference_edit_wcs_options_t {
    double x;
    bool has_x;
    double y;
    bool has_y;
    double z;
    bool has_z;
    double rotation;
    bool has_rotation;
    bool is_si;
    bool has_is_si;
} ifcopenshell_georeference_edit_wcs_options_t;

typedef struct ifcopenshell_group_add_group_options_t {
    const char* name;
    const char* description;
    bool has_description;
    ifcopenshell_instance_t* owner_history;
    bool has_owner_history;
    ifcopenshell_instance_t* user;
    bool has_user;
    ifcopenshell_instance_t* application;
    bool has_application;
} ifcopenshell_group_add_group_options_t;

typedef struct ifcopenshell_group_assign_group_options_t {
    ifcopenshell_parse_instance_list_t* products;
    ifcopenshell_instance_t* group;
    ifcopenshell_instance_t* owner_history;
    bool has_owner_history;
    ifcopenshell_instance_t* user;
    bool has_user;
    ifcopenshell_instance_t* application;
    bool has_application;
} ifcopenshell_group_assign_group_options_t;

typedef struct ifcopenshell_group_unassign_group_options_t {
    ifcopenshell_parse_instance_list_t* products;
    ifcopenshell_instance_t* group;
    ifcopenshell_instance_t* user;
    bool has_user;
    ifcopenshell_instance_t* application;
    bool has_application;
} ifcopenshell_group_unassign_group_options_t;

typedef struct ifcopenshell_group_update_group_products_options_t {
    ifcopenshell_instance_t* group;
    ifcopenshell_parse_instance_list_t* products;
    ifcopenshell_instance_t* owner_history;
    bool has_owner_history;
    ifcopenshell_instance_t* user;
    bool has_user;
    ifcopenshell_instance_t* application;
    bool has_application;
} ifcopenshell_group_update_group_products_options_t;

typedef struct ifcopenshell_layer_add_layer_with_style_options_t {
    bool on;
    bool has_on;
    bool frozen;
    bool has_frozen;
    bool blocked;
    bool has_blocked;
    ifcopenshell_parse_instance_list_t* styles;
} ifcopenshell_layer_add_layer_with_style_options_t;

typedef struct ifcopenshell_library_assign_reference_options_t {
    ifcopenshell_parse_instance_list_t* products;
    ifcopenshell_instance_t* reference;
    ifcopenshell_instance_t* owner_history;
    bool has_owner_history;
    ifcopenshell_instance_t* user;
    bool has_user;
    ifcopenshell_instance_t* application;
    bool has_application;
} ifcopenshell_library_assign_reference_options_t;

typedef struct ifcopenshell_library_unassign_reference_options_t {
    ifcopenshell_instance_t* reference;
    ifcopenshell_parse_instance_list_t* products;
    ifcopenshell_instance_t* user;
    bool has_user;
    ifcopenshell_instance_t* application;
    bool has_application;
} ifcopenshell_library_unassign_reference_options_t;

typedef struct ifcopenshell_material_add_constituent_options_t {
    ifcopenshell_instance_t* material;
    const char* name;
    bool has_name;
} ifcopenshell_material_add_constituent_options_t;

typedef struct ifcopenshell_material_add_layer_options_t {
    ifcopenshell_instance_t* material;
    const char* name;
    bool has_name;
} ifcopenshell_material_add_layer_options_t;

typedef struct ifcopenshell_material_add_material_options_t {
    const char* name;
    bool has_name;
    const char* category;
    bool has_category;
    const char* description;
    bool has_description;
} ifcopenshell_material_add_material_options_t;

typedef struct ifcopenshell_material_add_material_set_options_t {
    const char* name;
    bool has_name;
    const char* set_type;
    bool has_set_type;
} ifcopenshell_material_add_material_set_options_t;

typedef struct ifcopenshell_material_add_profile_options_t {
    ifcopenshell_instance_t* material;
    bool has_material;
    ifcopenshell_instance_t* profile;
    bool has_profile;
    const char* name;
    bool has_name;
} ifcopenshell_material_add_profile_options_t;

typedef struct ifcopenshell_material_assign_material_options_t {
    const char* type;
    bool has_type;
    ifcopenshell_instance_t* material;
    bool has_material;
    ifcopenshell_instance_t* owner_history;
    bool has_owner_history;
    ifcopenshell_instance_t* user;
    bool has_user;
    ifcopenshell_instance_t* application;
    bool has_application;
} ifcopenshell_material_assign_material_options_t;

typedef struct ifcopenshell_material_edit_profile_usage_options_t {
    void* attributes;
    double profile_width;
    bool has_profile_width;
    double profile_height;
    bool has_profile_height;
} ifcopenshell_material_edit_profile_usage_options_t;

typedef struct ifcopenshell_material_remove_item_options_t {
    bool should_remove_material;
    bool has_should_remove_material;
} ifcopenshell_material_remove_item_options_t;

typedef struct ifcopenshell_material_remove_list_item_options_t {
    int32_t material_index;
    bool has_material_index;
} ifcopenshell_material_remove_list_item_options_t;

typedef struct ifcopenshell_material_remove_profile_options_t {
    bool should_remove_profile_def;
    bool has_should_remove_profile_def;
    bool should_remove_material;
    bool has_should_remove_material;
} ifcopenshell_material_remove_profile_options_t;

typedef struct ifcopenshell_material_reorder_set_item_options_t {
    int32_t old_index;
    bool has_old_index;
    int32_t new_index;
    bool has_new_index;
} ifcopenshell_material_reorder_set_item_options_t;

typedef struct ifcopenshell_material_unassign_material_options_t {
    ifcopenshell_instance_t* user;
    bool has_user;
    ifcopenshell_instance_t* application;
    bool has_application;
} ifcopenshell_material_unassign_material_options_t;

typedef struct ifcopenshell_nest_assign_object_options_t {
    ifcopenshell_parse_instance_list_t* products;
    ifcopenshell_instance_t* relating_object;
    ifcopenshell_instance_t* owner_history;
    bool has_owner_history;
    ifcopenshell_instance_t* user;
    bool has_user;
    ifcopenshell_instance_t* application;
    bool has_application;
} ifcopenshell_nest_assign_object_options_t;

typedef struct ifcopenshell_nest_unassign_object_options_t {
    ifcopenshell_parse_instance_list_t* products;
    ifcopenshell_instance_t* user;
    bool has_user;
    ifcopenshell_instance_t* application;
    bool has_application;
} ifcopenshell_nest_unassign_object_options_t;

typedef struct ifcopenshell_owner_add_actor_options_t {
    ifcopenshell_instance_t* actor;
    const char* ifc_class;
    ifcopenshell_instance_t* owner_history;
    bool has_owner_history;
    ifcopenshell_instance_t* user;
    bool has_user;
    ifcopenshell_instance_t* application;
    bool has_application;
} ifcopenshell_owner_add_actor_options_t;

typedef struct ifcopenshell_owner_add_application_options_t {
    ifcopenshell_instance_t* application_developer;
    bool has_application_developer;
    const char* version;
    const char* application_full_name;
    const char* application_identifier;
    ifcopenshell_instance_t* owner_history;
    bool has_owner_history;
    ifcopenshell_instance_t* user;
    bool has_user;
    ifcopenshell_instance_t* application;
    bool has_application;
} ifcopenshell_owner_add_application_options_t;

typedef struct ifcopenshell_owner_assign_actor_options_t {
    ifcopenshell_instance_t* relating_actor;
    ifcopenshell_instance_t* related_object;
    ifcopenshell_instance_t* owner_history;
    bool has_owner_history;
    ifcopenshell_instance_t* user;
    bool has_user;
    ifcopenshell_instance_t* application;
    bool has_application;
} ifcopenshell_owner_assign_actor_options_t;

typedef struct ifcopenshell_owner_create_owner_history_options_t {
    ifcopenshell_instance_t* user;
    bool has_user;
    ifcopenshell_instance_t* application;
    bool has_application;
} ifcopenshell_owner_create_owner_history_options_t;

typedef struct ifcopenshell_owner_unassign_actor_options_t {
    ifcopenshell_instance_t* relating_actor;
    ifcopenshell_instance_t* related_object;
    ifcopenshell_instance_t* user;
    bool has_user;
    ifcopenshell_instance_t* application;
    bool has_application;
} ifcopenshell_owner_unassign_actor_options_t;

typedef struct ifcopenshell_owner_update_owner_history_options_t {
    ifcopenshell_instance_t* element;
    bool has_element;
    ifcopenshell_instance_t* user;
    bool has_user;
    ifcopenshell_instance_t* application;
    bool has_application;
} ifcopenshell_owner_update_owner_history_options_t;

typedef struct ifcopenshell_profile_add_arbitrary_profile_options_t {
    const ifcopenshell_double_list_list_t* profile;
    const char* name;
    bool has_name;
} ifcopenshell_profile_add_arbitrary_profile_options_t;

typedef struct ifcopenshell_profile_add_arbitrary_profile_with_voids_options_t {
    const ifcopenshell_double_list_list_t* outer_profile;
    const ifcopenshell_double_list_list_list_t* inner_profiles;
    const char* name;
    bool has_name;
} ifcopenshell_profile_add_arbitrary_profile_with_voids_options_t;

typedef struct ifcopenshell_project_assign_declaration_options_t {
    ifcopenshell_parse_instance_list_t* definitions;
    ifcopenshell_instance_t* relating_context;
    ifcopenshell_instance_t* owner_history;
    bool has_owner_history;
    ifcopenshell_instance_t* user;
    bool has_user;
    ifcopenshell_instance_t* application;
    bool has_application;
} ifcopenshell_project_assign_declaration_options_t;

typedef struct ifcopenshell_project_unassign_declaration_options_t {
    ifcopenshell_parse_instance_list_t* definitions;
    ifcopenshell_instance_t* relating_context;
    ifcopenshell_instance_t* user;
    bool has_user;
    ifcopenshell_instance_t* application;
    bool has_application;
} ifcopenshell_project_unassign_declaration_options_t;

typedef struct ifcopenshell_pset_add_pset_options_t {
    ifcopenshell_instance_t* product;
    const char* name;
    ifcopenshell_instance_t* owner_history;
    bool has_owner_history;
    ifcopenshell_instance_t* user;
    bool has_user;
    ifcopenshell_instance_t* application;
    bool has_application;
    const char* ifc2x3_subclass;
    bool has_ifc2x3_subclass;
} ifcopenshell_pset_add_pset_options_t;

typedef struct ifcopenshell_pset_add_qto_options_t {
    ifcopenshell_instance_t* product;
    const char* name;
    ifcopenshell_instance_t* owner_history;
    bool has_owner_history;
    ifcopenshell_instance_t* user;
    bool has_user;
    ifcopenshell_instance_t* application;
    bool has_application;
} ifcopenshell_pset_add_qto_options_t;

typedef struct ifcopenshell_pset_assign_pset_options_t {
    ifcopenshell_parse_instance_list_t* products;
    ifcopenshell_instance_t* pset;
    ifcopenshell_instance_t* owner_history;
    bool has_owner_history;
    ifcopenshell_instance_t* user;
    bool has_user;
    ifcopenshell_instance_t* application;
    bool has_application;
} ifcopenshell_pset_assign_pset_options_t;

typedef struct ifcopenshell_pset_edit_pset_options_t {
    ifcopenshell_instance_t* pset;
    const char* name;
    bool has_name;
    void* properties;
    ifcopenshell_instance_t* pset_template;
    bool has_pset_template;
    bool should_purge;
} ifcopenshell_pset_edit_pset_options_t;

typedef struct ifcopenshell_pset_edit_qto_options_t {
    ifcopenshell_instance_t* qto;
    const char* name;
    bool has_name;
    void* properties;
    ifcopenshell_instance_t* qto_template;
    bool has_qto_template;
} ifcopenshell_pset_edit_qto_options_t;

typedef struct ifcopenshell_pset_unshare_pset_options_t {
    ifcopenshell_parse_instance_list_t* products;
    ifcopenshell_instance_t* pset;
    ifcopenshell_instance_t* owner_history;
    bool has_owner_history;
    ifcopenshell_instance_t* user;
    bool has_user;
    ifcopenshell_instance_t* application;
    bool has_application;
} ifcopenshell_pset_unshare_pset_options_t;

typedef struct ifcopenshell_representation_get_product_representation_options_t {
    ifcopenshell_instance_t* context;
    bool has_context;
    const char* context_type;
    bool has_context_type;
    const char* subcontext;
    bool has_subcontext;
    const char* target_view;
    bool has_target_view;
} ifcopenshell_representation_get_product_representation_options_t;

typedef struct ifcopenshell_root_create_entity_options_t {
    const char* ifc_class;
    const char* predefined_type;
    bool has_predefined_type;
    const char* name;
    bool has_name;
    ifcopenshell_instance_t* owner_history;
    bool has_owner_history;
} ifcopenshell_root_create_entity_options_t;

typedef struct ifcopenshell_root_remove_product_options_t {
    ifcopenshell_instance_t* user;
    bool has_user;
    ifcopenshell_instance_t* application;
    bool has_application;
} ifcopenshell_root_remove_product_options_t;

typedef struct ifcopenshell_sequence_add_task_options_t {
    ifcopenshell_instance_t* work_schedule;
    bool has_work_schedule;
    ifcopenshell_instance_t* parent_task;
    bool has_parent_task;
    const char* name;
    bool has_name;
    const char* description;
    bool has_description;
    const char* identification;
    bool has_identification;
    const char* predefined_type;
    bool has_predefined_type;
    ifcopenshell_instance_t* owner_history;
    bool has_owner_history;
    ifcopenshell_instance_t* user;
    bool has_user;
    ifcopenshell_instance_t* application;
    bool has_application;
} ifcopenshell_sequence_add_task_options_t;

typedef struct ifcopenshell_sequence_add_task_time_options_t {
    bool is_recurring;
    bool has_is_recurring;
} ifcopenshell_sequence_add_task_time_options_t;

typedef struct ifcopenshell_sequence_add_time_period_options_t {
    const char* start_time;
    bool has_start_time;
    const char* end_time;
    bool has_end_time;
} ifcopenshell_sequence_add_time_period_options_t;

typedef struct ifcopenshell_sequence_add_work_calendar_options_t {
    const char* name;
    bool has_name;
    const char* predefined_type;
    bool has_predefined_type;
    ifcopenshell_instance_t* owner_history;
    bool has_owner_history;
    ifcopenshell_instance_t* user;
    bool has_user;
    ifcopenshell_instance_t* application;
    bool has_application;
} ifcopenshell_sequence_add_work_calendar_options_t;

typedef struct ifcopenshell_sequence_add_work_plan_options_t {
    const char* name;
    bool has_name;
    const char* predefined_type;
    bool has_predefined_type;
    const char* creation_date;
    bool has_creation_date;
    const char* start_time;
    bool has_start_time;
    ifcopenshell_instance_t* creator_person;
    bool has_creator_person;
    ifcopenshell_instance_t* owner_history;
    bool has_owner_history;
    ifcopenshell_instance_t* user;
    bool has_user;
    ifcopenshell_instance_t* application;
    bool has_application;
} ifcopenshell_sequence_add_work_plan_options_t;

typedef struct ifcopenshell_sequence_add_work_schedule_options_t {
    const char* name;
    bool has_name;
    const char* predefined_type;
    bool has_predefined_type;
    const char* object_type;
    bool has_object_type;
    const char* creation_date;
    bool has_creation_date;
    const char* start_time;
    bool has_start_time;
    ifcopenshell_instance_t* work_plan;
    bool has_work_plan;
    ifcopenshell_instance_t* creator_person;
    bool has_creator_person;
    ifcopenshell_instance_t* owner_history;
    bool has_owner_history;
    ifcopenshell_instance_t* user;
    bool has_user;
    ifcopenshell_instance_t* application;
    bool has_application;
} ifcopenshell_sequence_add_work_schedule_options_t;

typedef struct ifcopenshell_sequence_assign_lag_time_options_t {
    const char* duration_type;
    bool has_duration_type;
} ifcopenshell_sequence_assign_lag_time_options_t;

typedef struct ifcopenshell_sequence_assign_process_options_t {
    ifcopenshell_instance_t* owner_history;
    bool has_owner_history;
    ifcopenshell_instance_t* user;
    bool has_user;
    ifcopenshell_instance_t* application;
    bool has_application;
} ifcopenshell_sequence_assign_process_options_t;

typedef struct ifcopenshell_sequence_assign_product_options_t {
    ifcopenshell_instance_t* owner_history;
    bool has_owner_history;
    ifcopenshell_instance_t* user;
    bool has_user;
    ifcopenshell_instance_t* application;
    bool has_application;
} ifcopenshell_sequence_assign_product_options_t;

typedef struct ifcopenshell_sequence_assign_sequence_options_t {
    const char* sequence_type;
    bool has_sequence_type;
    ifcopenshell_instance_t* owner_history;
    bool has_owner_history;
    ifcopenshell_instance_t* user;
    bool has_user;
    ifcopenshell_instance_t* application;
    bool has_application;
} ifcopenshell_sequence_assign_sequence_options_t;

typedef struct ifcopenshell_sequence_assign_work_plan_options_t {
    ifcopenshell_instance_t* owner_history;
    bool has_owner_history;
    ifcopenshell_instance_t* user;
    bool has_user;
    ifcopenshell_instance_t* application;
    bool has_application;
} ifcopenshell_sequence_assign_work_plan_options_t;

typedef struct ifcopenshell_sequence_copy_work_schedule_options_t {
    ifcopenshell_instance_t* owner_history;
    bool has_owner_history;
    ifcopenshell_instance_t* user;
    bool has_user;
    ifcopenshell_instance_t* application;
    bool has_application;
} ifcopenshell_sequence_copy_work_schedule_options_t;

typedef struct ifcopenshell_sequence_create_baseline_options_t {
    const char* name;
    bool has_name;
    ifcopenshell_instance_t* owner_history;
    bool has_owner_history;
    ifcopenshell_instance_t* user;
    bool has_user;
    ifcopenshell_instance_t* application;
    bool has_application;
} ifcopenshell_sequence_create_baseline_options_t;

typedef struct ifcopenshell_sequence_duplicate_task_options_t {
    ifcopenshell_instance_t* owner_history;
    bool has_owner_history;
    ifcopenshell_instance_t* user;
    bool has_user;
    ifcopenshell_instance_t* application;
    bool has_application;
} ifcopenshell_sequence_duplicate_task_options_t;

typedef struct ifcopenshell_sequence_remove_options_t {
    ifcopenshell_instance_t* user;
    bool has_user;
    ifcopenshell_instance_t* application;
    bool has_application;
} ifcopenshell_sequence_remove_options_t;

typedef struct ifcopenshell_shape_builder_axis2_placement2d_options_t {
    const ifcopenshell_double_list_t* position;
    const ifcopenshell_double_list_t* x_direction;
    bool has_x_direction;
} ifcopenshell_shape_builder_axis2_placement2d_options_t;

typedef struct ifcopenshell_shape_builder_axis2_placement3d_options_t {
    const ifcopenshell_double_list_t* position;
    const ifcopenshell_double_list_t* z_axis;
    const ifcopenshell_double_list_t* x_axis;
} ifcopenshell_shape_builder_axis2_placement3d_options_t;

typedef struct ifcopenshell_shape_builder_block_options_t {
    const ifcopenshell_double_list_t* position;
    double x_length;
    double y_length;
    double z_length;
} ifcopenshell_shape_builder_block_options_t;

typedef struct ifcopenshell_shape_builder_ellipse_curve_options_t {
    double x_axis_radius;
    double y_axis_radius;
    const ifcopenshell_double_list_t* position;
    const ifcopenshell_double_list_list_t* trim_points;
    const ifcopenshell_double_list_t* ref_x_direction;
    bool has_ref_x_direction;
    const ifcopenshell_int32_list_t* trim_points_mask;
} ifcopenshell_shape_builder_ellipse_curve_options_t;

typedef struct ifcopenshell_shape_builder_extrude_options_t {
    ifcopenshell_instance_t* profile_or_curve;
    double magnitude;
    const ifcopenshell_double_list_t* position;
    const ifcopenshell_double_list_t* extrusion_vector;
    const ifcopenshell_double_list_t* position_z_axis;
    const ifcopenshell_double_list_t* position_x_axis;
    const ifcopenshell_double_list_t* position_y_axis;
    bool has_position_y_axis;
} ifcopenshell_shape_builder_extrude_options_t;

typedef struct ifcopenshell_shape_builder_half_space_solid_options_t {
    ifcopenshell_instance_t* plane;
    bool agreement_flag;
} ifcopenshell_shape_builder_half_space_solid_options_t;

typedef struct ifcopenshell_shape_builder_mep_bend_shape_options_t {
    ifcopenshell_instance_t* segment;
    double start_length;
    double end_length;
    double angle;
    double radius;
    const ifcopenshell_double_list_t* bend_vector;
    bool flip_z_axis;
} ifcopenshell_shape_builder_mep_bend_shape_options_t;

typedef struct ifcopenshell_shape_builder_mep_transition_calculate_options_t {
    const ifcopenshell_double_list_t* start_half_dim;
    const ifcopenshell_double_list_t* end_half_dim;
    const ifcopenshell_double_list_t* offset;
    const ifcopenshell_double_list_t* diff;
    bool has_diff;
    bool end_profile;
    double length;
    bool has_length;
    double angle;
    bool has_angle;
} ifcopenshell_shape_builder_mep_transition_calculate_options_t;

typedef struct ifcopenshell_shape_builder_mep_transition_length_options_t {
    const ifcopenshell_double_list_t* start_half_dim;
    const ifcopenshell_double_list_t* end_half_dim;
    double angle;
    const ifcopenshell_double_list_t* profile_offset;
} ifcopenshell_shape_builder_mep_transition_length_options_t;

typedef struct ifcopenshell_shape_builder_mep_transition_shape_options_t {
    ifcopenshell_instance_t* start_segment;
    ifcopenshell_instance_t* end_segment;
    double start_length;
    double end_length;
    double angle;
    const ifcopenshell_double_list_t* profile_offset;
} ifcopenshell_shape_builder_mep_transition_shape_options_t;

typedef struct ifcopenshell_shape_builder_mirror_options_t {
    ifcopenshell_instance_t* item;
    const ifcopenshell_double_list_t* mirror_axes;
    const ifcopenshell_double_list_t* mirror_point;
    bool create_copy;
    const ifcopenshell_double_list_t* placement_matrix;
} ifcopenshell_shape_builder_mirror_options_t;

typedef struct ifcopenshell_shape_builder_polyline_options_t {
    const ifcopenshell_double_list_list_t* points;
    bool closed;
    bool has_closed;
    const ifcopenshell_double_list_t* position_offset;
    bool has_position_offset;
    const ifcopenshell_int32_list_t* arc_points;
} ifcopenshell_shape_builder_polyline_options_t;

typedef struct ifcopenshell_shape_builder_profile_options_t {
    ifcopenshell_instance_t* outer_curve;
    const char* name;
    bool has_name;
    ifcopenshell_parse_instance_list_t* inner_curves;
    const char* profile_type;
    bool has_profile_type;
} ifcopenshell_shape_builder_profile_options_t;

typedef struct ifcopenshell_shape_builder_representation_options_t {
    ifcopenshell_instance_t* context;
    ifcopenshell_parse_instance_list_t* items;
    const char* representation_type;
    bool has_representation_type;
} ifcopenshell_shape_builder_representation_options_t;

typedef struct ifcopenshell_shape_builder_rotate_options_t {
    ifcopenshell_instance_t* item;
    double angle;
    const ifcopenshell_double_list_t* pivot_point;
    bool counter_clockwise;
    bool create_copy;
} ifcopenshell_shape_builder_rotate_options_t;

typedef struct ifcopenshell_shape_builder_sphere_options_t {
    double radius;
    const ifcopenshell_double_list_t* center;
} ifcopenshell_shape_builder_sphere_options_t;

typedef struct ifcopenshell_shape_builder_translate_options_t {
    ifcopenshell_instance_t* item;
    const ifcopenshell_double_list_t* translation;
    bool create_copy;
} ifcopenshell_shape_builder_translate_options_t;

typedef struct ifcopenshell_spatial_assign_container_options_t {
    ifcopenshell_parse_instance_list_t* products;
    ifcopenshell_instance_t* relating_structure;
    ifcopenshell_instance_t* owner_history;
    bool has_owner_history;
    ifcopenshell_instance_t* user;
    bool has_user;
    ifcopenshell_instance_t* application;
    bool has_application;
} ifcopenshell_spatial_assign_container_options_t;

typedef struct ifcopenshell_spatial_dereference_structure_options_t {
    ifcopenshell_parse_instance_list_t* products;
    ifcopenshell_instance_t* relating_structure;
    ifcopenshell_instance_t* user;
    bool has_user;
    ifcopenshell_instance_t* application;
    bool has_application;
} ifcopenshell_spatial_dereference_structure_options_t;

typedef struct ifcopenshell_spatial_reference_structure_options_t {
    ifcopenshell_parse_instance_list_t* products;
    ifcopenshell_instance_t* relating_structure;
    ifcopenshell_instance_t* owner_history;
    bool has_owner_history;
    ifcopenshell_instance_t* user;
    bool has_user;
    ifcopenshell_instance_t* application;
    bool has_application;
} ifcopenshell_spatial_reference_structure_options_t;

typedef struct ifcopenshell_spatial_unassign_container_options_t {
    ifcopenshell_parse_instance_list_t* products;
    ifcopenshell_instance_t* user;
    bool has_user;
    ifcopenshell_instance_t* application;
    bool has_application;
} ifcopenshell_spatial_unassign_container_options_t;

typedef struct ifcopenshell_structural_add_structural_activity_options_t {
    ifcopenshell_instance_t* activity_owner_history;
    bool has_activity_owner_history;
    ifcopenshell_instance_t* relationship_owner_history;
    bool has_relationship_owner_history;
} ifcopenshell_structural_add_structural_activity_options_t;

typedef struct ifcopenshell_structural_add_structural_boundary_condition_options_t {
    const char* name;
    bool has_name;
    ifcopenshell_instance_t* connection;
    bool has_connection;
} ifcopenshell_structural_add_structural_boundary_condition_options_t;

typedef struct ifcopenshell_structural_assign_structural_analysis_model_options_t {
    ifcopenshell_instance_t* owner_history;
    bool has_owner_history;
    ifcopenshell_instance_t* user;
    bool has_user;
    ifcopenshell_instance_t* application;
    bool has_application;
} ifcopenshell_structural_assign_structural_analysis_model_options_t;

typedef struct ifcopenshell_structural_remove_structural_boundary_condition_options_t {
    ifcopenshell_instance_t* connection;
    bool has_connection;
    ifcopenshell_instance_t* boundary_condition;
    bool has_boundary_condition;
} ifcopenshell_structural_remove_structural_boundary_condition_options_t;

typedef struct ifcopenshell_structural_unassign_structural_analysis_model_options_t {
    ifcopenshell_instance_t* user;
    bool has_user;
    ifcopenshell_instance_t* application;
    bool has_application;
} ifcopenshell_structural_unassign_structural_analysis_model_options_t;

typedef struct ifcopenshell_style_assign_item_style_options_t {
    ifcopenshell_instance_t* item;
    ifcopenshell_instance_t* style;
    bool has_style;
    bool should_use_presentation_style_assignment;
} ifcopenshell_style_assign_item_style_options_t;

typedef struct ifcopenshell_system_add_port_options_t {
    ifcopenshell_instance_t* element;
    bool has_element;
    ifcopenshell_instance_t* owner_history;
    bool has_owner_history;
    ifcopenshell_instance_t* user;
    bool has_user;
    ifcopenshell_instance_t* application;
    bool has_application;
} ifcopenshell_system_add_port_options_t;

typedef struct ifcopenshell_system_add_system_options_t {
    const char* ifc_class;
    ifcopenshell_instance_t* owner_history;
    bool has_owner_history;
} ifcopenshell_system_add_system_options_t;

typedef struct ifcopenshell_system_assign_flow_control_options_t {
    ifcopenshell_instance_t* relating_flow_element;
    ifcopenshell_instance_t* related_flow_control;
    ifcopenshell_instance_t* owner_history;
    bool has_owner_history;
    ifcopenshell_instance_t* user;
    bool has_user;
    ifcopenshell_instance_t* application;
    bool has_application;
} ifcopenshell_system_assign_flow_control_options_t;

typedef struct ifcopenshell_system_assign_port_options_t {
    ifcopenshell_instance_t* element;
    ifcopenshell_instance_t* port;
    ifcopenshell_instance_t* owner_history;
    bool has_owner_history;
    ifcopenshell_instance_t* user;
    bool has_user;
    ifcopenshell_instance_t* application;
    bool has_application;
} ifcopenshell_system_assign_port_options_t;

typedef struct ifcopenshell_system_assign_system_options_t {
    ifcopenshell_parse_instance_list_t* products;
    ifcopenshell_instance_t* system;
    ifcopenshell_instance_t* owner_history;
    bool has_owner_history;
    ifcopenshell_instance_t* user;
    bool has_user;
    ifcopenshell_instance_t* application;
    bool has_application;
} ifcopenshell_system_assign_system_options_t;

typedef struct ifcopenshell_system_connect_port_options_t {
    ifcopenshell_instance_t* port1;
    ifcopenshell_instance_t* port2;
    const char* direction;
    ifcopenshell_instance_t* element;
    bool has_element;
    ifcopenshell_instance_t* owner_history;
    bool has_owner_history;
    ifcopenshell_instance_t* user;
    bool has_user;
    ifcopenshell_instance_t* application;
    bool has_application;
} ifcopenshell_system_connect_port_options_t;

typedef struct ifcopenshell_system_unassign_flow_control_options_t {
    ifcopenshell_instance_t* relating_flow_element;
    ifcopenshell_instance_t* related_flow_control;
    ifcopenshell_instance_t* user;
    bool has_user;
    ifcopenshell_instance_t* application;
    bool has_application;
} ifcopenshell_system_unassign_flow_control_options_t;

typedef struct ifcopenshell_system_unassign_port_options_t {
    ifcopenshell_instance_t* element;
    ifcopenshell_instance_t* port;
    ifcopenshell_instance_t* user;
    bool has_user;
    ifcopenshell_instance_t* application;
    bool has_application;
} ifcopenshell_system_unassign_port_options_t;

typedef struct ifcopenshell_system_unassign_system_options_t {
    ifcopenshell_parse_instance_list_t* products;
    ifcopenshell_instance_t* system;
    ifcopenshell_instance_t* user;
    bool has_user;
    ifcopenshell_instance_t* application;
    bool has_application;
} ifcopenshell_system_unassign_system_options_t;

typedef struct ifcopenshell_type_assign_type_options_t {
    ifcopenshell_parse_instance_list_t* objects;
    ifcopenshell_instance_t* relating_type;
    bool should_map_representations;
    bool has_should_map_representations;
    ifcopenshell_instance_t* owner_history;
    bool has_owner_history;
    ifcopenshell_instance_t* user;
    bool has_user;
    ifcopenshell_instance_t* application;
    bool has_application;
} ifcopenshell_type_assign_type_options_t;

typedef struct ifcopenshell_type_unassign_type_options_t {
    ifcopenshell_parse_instance_list_t* objects;
    ifcopenshell_instance_t* user;
    bool has_user;
    ifcopenshell_instance_t* application;
    bool has_application;
} ifcopenshell_type_unassign_type_options_t;

typedef struct ifcopenshell_shape_builder_mep_transition_shape_result_t {
    ifcopenshell_instance_t* representation;
    double start_length;
    double end_length;
    double angle;
    ifcopenshell_double_list_t profile_offset;
    double transition_length;
    double full_transition_length;
} ifcopenshell_shape_builder_mep_transition_shape_result_t;

typedef struct ifcopenshell_shape_builder_mep_bend_shape_result_t {
    ifcopenshell_instance_t* representation;
    double start_length;
    double end_length;
    double radius;
    double angle;
    int32_t lateral_axis;
    double lateral_sign;
    int32_t z_axis_sign;
    double main_profile_dimension;
} ifcopenshell_shape_builder_mep_bend_shape_result_t;

typedef struct ifcopenshell_sequence_duplicate_task_result_t {
    ifcopenshell_parse_instance_list_t* current;
    ifcopenshell_parse_instance_list_t* duplicate;
} ifcopenshell_sequence_duplicate_task_result_t;

typedef struct ifcopenshell_optional_shape_builder_mep_transition_shape_result_t {
    bool has_value;
    ifcopenshell_shape_builder_mep_transition_shape_result_t value;
} ifcopenshell_optional_shape_builder_mep_transition_shape_result_t;

typedef struct ifcopenshell_instance_string_variant_t {
    int32_t kind;
    ifcopenshell_instance_t* value_0;
    ifcopenshell_string_t value_1;
} ifcopenshell_instance_string_variant_t;

typedef enum {
    IFCOPENSHELL_ERROR_NONE = 0,
    IFCOPENSHELL_ERROR_RUNTIME = 1,
    IFCOPENSHELL_ERROR_VALUE = 2,
    IFCOPENSHELL_ERROR_TYPE = 3,
    IFCOPENSHELL_ERROR_NOT_IMPLEMENTED = 4,
    IFCOPENSHELL_ERROR_KEY = 5
} ifcopenshell_error_kind_t;

void ifcopenshell_clear_error(void);
const char* ifcopenshell_last_error_message(void);
int ifcopenshell_last_error_kind(void);

void ifcopenshell_file_destroy(ifcopenshell_file_t* handle);
void ifcopenshell_instance_streamer_destroy(ifcopenshell_instance_streamer_t* handle);
void ifcopenshell_instance_destroy(ifcopenshell_instance_t* handle);
void ifcopenshell_header_destroy(ifcopenshell_header_t* handle);
void ifcopenshell_file_description_destroy(ifcopenshell_file_description_t* handle);
void ifcopenshell_file_name_destroy(ifcopenshell_file_name_t* handle);
void ifcopenshell_file_schema_destroy(ifcopenshell_file_schema_t* handle);
void ifcopenshell_declaration_destroy(ifcopenshell_declaration_t* handle);
void ifcopenshell_type_declaration_destroy(ifcopenshell_type_declaration_t* handle);
void ifcopenshell_select_type_destroy(ifcopenshell_select_type_t* handle);
void ifcopenshell_schema_destroy(ifcopenshell_schema_t* handle);
void ifcopenshell_enumeration_destroy(ifcopenshell_enumeration_t* handle);
void ifcopenshell_parameter_type_destroy(ifcopenshell_parameter_type_t* handle);
void ifcopenshell_named_type_destroy(ifcopenshell_named_type_t* handle);
void ifcopenshell_simple_type_destroy(ifcopenshell_simple_type_t* handle);
void ifcopenshell_aggregation_type_destroy(ifcopenshell_aggregation_type_t* handle);
void ifcopenshell_entity_destroy(ifcopenshell_entity_t* handle);
void ifcopenshell_attribute_destroy(ifcopenshell_attribute_t* handle);
void ifcopenshell_inverse_attribute_destroy(ifcopenshell_inverse_attribute_t* handle);
void ifcopenshell_parse_attribute_value_destroy(ifcopenshell_parse_attribute_value_t* handle);
void ifcopenshell_parse_instance_list_destroy(ifcopenshell_parse_instance_list_t* handle);
void ifcopenshell_value_destroy(ifcopenshell_value_t* handle);
void ifcopenshell_pset_template_handle_destroy(ifcopenshell_pset_template_handle_t* handle);
void ifcopenshell_geom_iterator_destroy(ifcopenshell_geom_iterator_t* handle);
void ifcopenshell_geom_settings_destroy(ifcopenshell_geom_settings_t* handle);
void ifcopenshell_geom_serializer_settings_destroy(ifcopenshell_geom_serializer_settings_t* handle);
void ifcopenshell_geom_geometry_serializer_destroy(ifcopenshell_geom_geometry_serializer_t* handle);
void ifcopenshell_geom_serializer_destroy(ifcopenshell_geom_serializer_t* handle);
void ifcopenshell_geom_buffer_destroy(ifcopenshell_geom_buffer_t* handle);
void ifcopenshell_geom_tree_destroy(ifcopenshell_geom_tree_t* handle);
void ifcopenshell_geom_tree_clash_list_destroy(ifcopenshell_geom_tree_clash_list_t* handle);
void ifcopenshell_geom_tree_clash_destroy(ifcopenshell_geom_tree_clash_t* handle);
void ifcopenshell_geom_tree_ray_intersection_list_destroy(ifcopenshell_geom_tree_ray_intersection_list_t* handle);
void ifcopenshell_geom_tree_ray_intersection_destroy(ifcopenshell_geom_tree_ray_intersection_t* handle);
void ifcopenshell_geom_transformation_destroy(ifcopenshell_geom_transformation_t* handle);
void ifcopenshell_geom_element_destroy(ifcopenshell_geom_element_t* handle);
void ifcopenshell_geom_brep_element_destroy(ifcopenshell_geom_brep_element_t* handle);
void ifcopenshell_geom_triangulation_element_destroy(ifcopenshell_geom_triangulation_element_t* handle);
void ifcopenshell_geom_serialized_element_destroy(ifcopenshell_geom_serialized_element_t* handle);
void ifcopenshell_geom_triangulation_destroy(ifcopenshell_geom_triangulation_t* handle);
void ifcopenshell_geom_brep_representation_destroy(ifcopenshell_geom_brep_representation_t* handle);
void ifcopenshell_geom_serialization_destroy(ifcopenshell_geom_serialization_t* handle);
void ifcopenshell_geom_conversion_result_shape_destroy(ifcopenshell_geom_conversion_result_shape_t* handle);
void ifcopenshell_geom_opaque_number_destroy(ifcopenshell_geom_opaque_number_t* handle);
void ifcopenshell_geom_svgfill_polygon_destroy(ifcopenshell_geom_svgfill_polygon_t* handle);
void ifcopenshell_geom_function_item_evaluator_destroy(ifcopenshell_geom_function_item_evaluator_t* handle);
void ifcopenshell_geom_taxonomy_item_destroy(ifcopenshell_geom_taxonomy_item_t* handle);
void ifcopenshell_geom_taxonomy_matrix4_destroy(ifcopenshell_geom_taxonomy_matrix4_t* handle);
void ifcopenshell_geom_taxonomy_point3_destroy(ifcopenshell_geom_taxonomy_point3_t* handle);
void ifcopenshell_geom_taxonomy_direction3_destroy(ifcopenshell_geom_taxonomy_direction3_t* handle);
void ifcopenshell_geom_taxonomy_style_destroy(ifcopenshell_geom_taxonomy_style_t* handle);
void ifcopenshell_geom_taxonomy_colour_destroy(ifcopenshell_geom_taxonomy_colour_t* handle);
void ifcopenshell_geom_taxonomy_line_destroy(ifcopenshell_geom_taxonomy_line_t* handle);
void ifcopenshell_geom_taxonomy_circle_destroy(ifcopenshell_geom_taxonomy_circle_t* handle);
void ifcopenshell_geom_taxonomy_ellipse_destroy(ifcopenshell_geom_taxonomy_ellipse_t* handle);
void ifcopenshell_geom_taxonomy_bspline_curve_destroy(ifcopenshell_geom_taxonomy_bspline_curve_t* handle);
void ifcopenshell_geom_taxonomy_offset_curve_destroy(ifcopenshell_geom_taxonomy_offset_curve_t* handle);
void ifcopenshell_geom_taxonomy_edge_destroy(ifcopenshell_geom_taxonomy_edge_t* handle);
void ifcopenshell_geom_taxonomy_loop_destroy(ifcopenshell_geom_taxonomy_loop_t* handle);
void ifcopenshell_geom_taxonomy_face_destroy(ifcopenshell_geom_taxonomy_face_t* handle);
void ifcopenshell_geom_taxonomy_shell_destroy(ifcopenshell_geom_taxonomy_shell_t* handle);
void ifcopenshell_geom_taxonomy_solid_destroy(ifcopenshell_geom_taxonomy_solid_t* handle);
void ifcopenshell_geom_taxonomy_plane_destroy(ifcopenshell_geom_taxonomy_plane_t* handle);
void ifcopenshell_geom_taxonomy_cylinder_destroy(ifcopenshell_geom_taxonomy_cylinder_t* handle);
void ifcopenshell_geom_taxonomy_sphere_destroy(ifcopenshell_geom_taxonomy_sphere_t* handle);
void ifcopenshell_geom_taxonomy_torus_destroy(ifcopenshell_geom_taxonomy_torus_t* handle);
void ifcopenshell_geom_taxonomy_bspline_surface_destroy(ifcopenshell_geom_taxonomy_bspline_surface_t* handle);
void ifcopenshell_geom_taxonomy_collection_destroy(ifcopenshell_geom_taxonomy_collection_t* handle);
void ifcopenshell_geom_taxonomy_loft_destroy(ifcopenshell_geom_taxonomy_loft_t* handle);
void ifcopenshell_geom_taxonomy_extrusion_destroy(ifcopenshell_geom_taxonomy_extrusion_t* handle);
void ifcopenshell_geom_taxonomy_revolve_destroy(ifcopenshell_geom_taxonomy_revolve_t* handle);
void ifcopenshell_geom_taxonomy_sweep_along_curve_destroy(ifcopenshell_geom_taxonomy_sweep_along_curve_t* handle);
void ifcopenshell_geom_taxonomy_node_destroy(ifcopenshell_geom_taxonomy_node_t* handle);
void ifcopenshell_geom_taxonomy_boolean_result_destroy(ifcopenshell_geom_taxonomy_boolean_result_t* handle);
void ifcopenshell_instance_list_destroy(ifcopenshell_instance_list_t* value);
void ifcopenshell_file_list_destroy(ifcopenshell_file_list_t* value);
void ifcopenshell_geom_svgfill_polygon_list_destroy(ifcopenshell_geom_svgfill_polygon_list_t* value);
void ifcopenshell_geom_conversion_result_shape_list_destroy(ifcopenshell_geom_conversion_result_shape_list_t* value);
void ifcopenshell_declaration_list_destroy(ifcopenshell_declaration_list_t* value);
void ifcopenshell_entity_list_destroy(ifcopenshell_entity_list_t* value);
void ifcopenshell_enumeration_list_destroy(ifcopenshell_enumeration_list_t* value);
void ifcopenshell_select_type_list_destroy(ifcopenshell_select_type_list_t* value);
void ifcopenshell_type_declaration_list_destroy(ifcopenshell_type_declaration_list_t* value);
void ifcopenshell_attribute_list_destroy(ifcopenshell_attribute_list_t* value);
void ifcopenshell_inverse_attribute_list_destroy(ifcopenshell_inverse_attribute_list_t* value);
void ifcopenshell_geom_taxonomy_style_list_destroy(ifcopenshell_geom_taxonomy_style_list_t* value);
void ifcopenshell_geom_taxonomy_item_list_destroy(ifcopenshell_geom_taxonomy_item_list_t* value);
void ifcopenshell_geom_element_list_destroy(ifcopenshell_geom_element_list_t* value);
void ifcopenshell_instance_list_list_destroy(ifcopenshell_instance_list_list_t* value);
void ifcopenshell_file_list_list_destroy(ifcopenshell_file_list_list_t* value);
void ifcopenshell_geom_svgfill_polygon_list_list_destroy(ifcopenshell_geom_svgfill_polygon_list_list_t* value);
void ifcopenshell_geom_conversion_result_shape_list_list_destroy(ifcopenshell_geom_conversion_result_shape_list_list_t* value);
void ifcopenshell_declaration_list_list_destroy(ifcopenshell_declaration_list_list_t* value);
void ifcopenshell_entity_list_list_destroy(ifcopenshell_entity_list_list_t* value);
void ifcopenshell_enumeration_list_list_destroy(ifcopenshell_enumeration_list_list_t* value);
void ifcopenshell_select_type_list_list_destroy(ifcopenshell_select_type_list_list_t* value);
void ifcopenshell_type_declaration_list_list_destroy(ifcopenshell_type_declaration_list_list_t* value);
void ifcopenshell_attribute_list_list_destroy(ifcopenshell_attribute_list_list_t* value);
void ifcopenshell_inverse_attribute_list_list_destroy(ifcopenshell_inverse_attribute_list_list_t* value);
void ifcopenshell_geom_taxonomy_style_list_list_destroy(ifcopenshell_geom_taxonomy_style_list_list_t* value);
void ifcopenshell_geom_taxonomy_item_list_list_destroy(ifcopenshell_geom_taxonomy_item_list_list_t* value);
void ifcopenshell_geom_element_list_list_destroy(ifcopenshell_geom_element_list_list_t* value);
void ifcopenshell_instance_string_variant_destroy(ifcopenshell_instance_string_variant_t* value);

bool ifcopenshell_geom_create_xml_serializer(ifcopenshell_file_t* file, const char* filename, ifcopenshell_geom_serializer_t** out_result);
bool ifcopenshell_geom_create_tree(ifcopenshell_geom_tree_t** out_result);
bool ifcopenshell_geom_create_tree_from_file(ifcopenshell_file_t* file, ifcopenshell_geom_tree_t** out_result);
bool ifcopenshell_geom_create_tree_from_file_with_settings(ifcopenshell_file_t* file, ifcopenshell_geom_settings_t* settings, ifcopenshell_geom_tree_t** out_result);
bool ifcopenshell_geom_create_tree_from_iterator(ifcopenshell_geom_iterator_t* iterator, ifcopenshell_geom_tree_t** out_result);
bool ifcopenshell_geom_create_json_serializer(ifcopenshell_file_t* file, const char* filename, ifcopenshell_geom_serializer_t** out_result);
bool ifcopenshell_geom_create_rocksdb_serializer_streaming(const char* input_filename, const char* rocksdb_filename, ifcopenshell_geom_serializer_t** out_result);
bool ifcopenshell_geom_create_buffer(ifcopenshell_geom_buffer_t** out_result);
bool ifcopenshell_geom_create_buffer_from_filename(const char* filename, ifcopenshell_geom_buffer_t** out_result);
bool ifcopenshell_geom_create_settings(ifcopenshell_geom_settings_t** out_result);
bool ifcopenshell_geom_create_serializer_settings(ifcopenshell_geom_serializer_settings_t** out_result);
bool ifcopenshell_geom_helmert_curve_point(double A0, double A1, double A2, double s, ifcopenshell_double_list_t* out_result);
bool ifcopenshell_parse_argument_type_to_string(int32_t type, ifcopenshell_string_t* out_result);
bool ifcopenshell_parse_clear_plugin_search_paths(void);
bool ifcopenshell_parse_clear_schemas(void);
bool ifcopenshell_parse_escape_xml(const char* text);
bool ifcopenshell_parse_from_parameter_type(ifcopenshell_parameter_type_t* parameter_type, int32_t* out_result);
bool ifcopenshell_parse_general_token_ptr(size_t start, const char* token, int32_t* out_result);
bool ifcopenshell_parse_get_feature(const char* name, bool* out_result);
bool ifcopenshell_parse_get_info_cpp(ifcopenshell_instance_t* instance, bool include_identifier, ifcopenshell_string_t* out_result);
bool ifcopenshell_parse_get_log(ifcopenshell_string_t* out_result);
bool ifcopenshell_parse_get_plugin_search_paths(ifcopenshell_string_list_t* out_result);
bool ifcopenshell_parse_get_si_equivalent(ifcopenshell_instance_t* named_unit, double* out_result);
bool ifcopenshell_parse_guess_file_type(const char* path, int32_t* out_result);
bool ifcopenshell_parse_instance_list_create_from_handles(const ifcopenshell_instance_list_t* instances, ifcopenshell_parse_instance_list_t** out_result);
bool ifcopenshell_parse_make_aggregate(int32_t element_type, int32_t* out_result);
bool ifcopenshell_parse_new_file(const char* schema_identifier, int32_t file_type, const char* path, ifcopenshell_file_t** out_result);
bool ifcopenshell_parse_open(const char* path, bool readonly, ifcopenshell_file_t** out_result);
bool ifcopenshell_parse_open_bypass(const char* path, const ifcopenshell_string_list_t* type_names, ifcopenshell_file_t** out_result);
bool ifcopenshell_parse_operator_token_ptr(size_t start, const char* data, int32_t* out_result);
bool ifcopenshell_parse_read_memory(void* data, int32_t length, ifcopenshell_file_t** out_result);
bool ifcopenshell_parse_register_schema(ifcopenshell_schema_t* schema);
bool ifcopenshell_parse_sanitate_material_name(const char* material_name);
bool ifcopenshell_parse_schema_by_name(const char* schema_name, ifcopenshell_schema_t** out_result);
bool ifcopenshell_parse_schema_names(ifcopenshell_string_list_t* out_result);
bool ifcopenshell_parse_schema_plugin_registration_symbol(ifcopenshell_string_t* out_result);
bool ifcopenshell_parse_set_feature(const char* name, bool value);
bool ifcopenshell_parse_set_log_format_json(void);
bool ifcopenshell_parse_set_log_format_text(void);
bool ifcopenshell_parse_set_plugin_search_paths(const ifcopenshell_string_list_t* paths);
bool ifcopenshell_parse_si_prefix_to_value(const char* prefix, double* out_result);
bool ifcopenshell_parse_stream(ifcopenshell_instance_streamer_t** out_result);
bool ifcopenshell_parse_stream_from_path(const char* path, bool mmap, ifcopenshell_instance_streamer_t** out_result);
bool ifcopenshell_parse_stream_from_string(const char* data, ifcopenshell_instance_streamer_t** out_result);
bool ifcopenshell_parse_traverse(ifcopenshell_instance_t* instance, int32_t max_depth, ifcopenshell_parse_instance_list_t** out_result);
bool ifcopenshell_parse_traverse_breadth_first(ifcopenshell_instance_t* instance, int32_t max_depth, ifcopenshell_parse_instance_list_t** out_result);
bool ifcopenshell_parse_turn_off_detailed_logging(void);
bool ifcopenshell_parse_turn_on_detailed_logging(void);
bool ifcopenshell_parse_unescape_xml(const char* text);
bool ifcopenshell_parse_valid_binary_string(const char* binary_string, bool* out_result);
bool ifcopenshell_parse_version(ifcopenshell_string_t* out_result);
/**
 * Assign products as parts of an aggregate object via IfcRelAggregates.
 *
 * Products already aggregated under a different parent are moved. Products
 * previously contained in a spatial structure (IfcRelContainedInSpatialStructure)
 * are removed from that containment. If an existing IfcRelAggregates
 * relationship already relates to the same parent, products are merged into it.
 */
bool ifcopenshell_aggregate_assign_object(ifcopenshell_file_t* file, const ifcopenshell_aggregate_assign_object_options_t* options, ifcopenshell_instance_t** out_result);
/**
 * Remove products from their IfcRelAggregates relationships.
 *
 * If no related objects remain after removal, the IfcRelAggregates
 * relationship itself is deleted.
 */
bool ifcopenshell_aggregate_unassign_object(ifcopenshell_file_t* file, const ifcopenshell_aggregate_unassign_object_options_t* options);
/**
 * Edit arbitrary attributes on a product.
 *
 * Applies the attribute values from the property bag to the product.
 * Optionally synchronizes PredefinedType when ElementType or ObjectType
 * changes, and optionally updates OwnerHistory.
 *
 * @param file The IFC file containing the product.
 * @param options Attribute editing options.
 */
bool ifcopenshell_attribute_edit_attributes(ifcopenshell_file_t* file, const ifcopenshell_attribute_edit_attributes_options_t* options);
/** Assign a planar connection geometry to a space boundary relationship. */
bool ifcopenshell_boundary_assign_connection_geometry(ifcopenshell_file_t* file, ifcopenshell_instance_t* rel_space_boundary, const ifcopenshell_boundary_assign_connection_geometry_options_t* options);
/**
 * Shallow-copy a space boundary relationship, deep-copying its connection geometry.
 *
 * @param file File that receives the copied entities.
 * @param boundary IfcRelSpaceBoundary entity to copy.
 * @return Newly created copy, or a null handle on failure.
 */
bool ifcopenshell_boundary_copy_boundary(ifcopenshell_file_t* file, ifcopenshell_instance_t* boundary, ifcopenshell_instance_t** out_result);
/**
 * Edit attributes of a space boundary relationship.
 *
 * Updates the relating space, related building element, and boundary
 * classification. ParentBoundary and CorrespondingBoundary are set only when
 * the schema supports them (IFC4+); nullopt clears those attributes.
 *
 * @param entity IfcRelSpaceBoundary entity to modify.
 * @param options Attribute values to set.
 */
bool ifcopenshell_boundary_edit_attributes(ifcopenshell_instance_t* entity, const ifcopenshell_boundary_edit_attributes_options_t* options);
/**
 * Remove a space boundary relationship and its connection geometry.
 *
 * Removes the ConnectionGeometry attribute first (deep-removing its entities),
 * then removes the boundary entity itself with history cleanup.
 *
 * @param file IFC file containing the boundary.
 * @param boundary IfcRelSpaceBoundary entity to remove.
 */
bool ifcopenshell_boundary_remove_boundary(ifcopenshell_file_t* file, ifcopenshell_instance_t* boundary);
/**
 * Create a new IfcClassification and associate it with the project.
 *
 * The classification is linked to the first IfcProject via
 * IfcRelAssociatesClassification so that it persists in the file.
 */
bool ifcopenshell_classification_add_classification(ifcopenshell_file_t* file, const char* name, ifcopenshell_instance_t** out_result);
/**
 * Add a classification reference and associate it with products.
 *
 * If an existing reference handle is provided, it is used directly.
 * Otherwise, a new IfcClassificationReference is created using the
 * optional identification, name, and classification fields.
 */
bool ifcopenshell_classification_add_reference(ifcopenshell_file_t* file, const ifcopenshell_classification_add_reference_options_t* options, ifcopenshell_instance_t** out_result);
/**
 * Return all classification references associated with an element.
 *
 * For IfcRoot-derived elements, returns references from IfcRelAssociatesClassification.
 * For non-IfcRoot elements (e.g. IfcMaterial), returns references via
 * IfcExternalReferenceRelationship. When should_inherit is true, references
 * from the element's type are merged, with occurrence-level references
 * overriding type-level ones within the same classification system.
 */
bool ifcopenshell_classification_get_references(ifcopenshell_instance_t* element, bool should_inherit, ifcopenshell_parse_instance_list_t** out_result);
/**
 * Remove an IfcClassification and all its references.
 *
 * For IFC4+, recursively deletes every IfcClassificationReference in the
 * classification hierarchy via HasReferences. For IFC2X3, deletes only
 * references whose ReferencedSource is the classification itself; nested
 * references are not traversed and may be orphaned. Any
 * IfcRelAssociatesClassification that becomes orphaned is also deleted.
 */
bool ifcopenshell_classification_remove_classification(ifcopenshell_file_t* file, ifcopenshell_instance_t* classification);
/**
 * Remove a classification reference association from products.
 *
 * If the reference is no longer associated with any products after
 * removal, the reference entity itself is also deleted.
 */
bool ifcopenshell_classification_remove_reference(ifcopenshell_file_t* file, const ifcopenshell_classification_remove_reference_options_t* options);
/**
 * Add a single survey point as an IfcAnnotation.
 *
 * Creates an IfcAnnotation with PredefinedType SURVEY, wraps the given
 * IfcPoint in an IfcShapeRepresentation (Annotation/Point), and places
 * the annotation in the given or first IfcSite. The annotation's
 * ObjectPlacement is set to the world coordinate system of the
 * Model/Annotation/MODEL_VIEW representation context.
 *
 * @param file File that receives the new entities.
 * @param options Survey point geometry and placement options.
 * @return The newly created IfcAnnotation, or a null handle on error.
 */
bool ifcopenshell_cogo_add_survey_point(ifcopenshell_file_t* file, const ifcopenshell_cogo_add_survey_point_options_t* options, ifcopenshell_instance_t** out_result);
/**
 * Replace the survey point geometry inside an existing annotation.
 *
 * Replaces the first item in the annotation's IfcShapeRepresentation with
 * the given IfcPoint. The annotation must already have a shape representation.
 *
 * @param annotation IfcAnnotation whose survey point to replace.
 * @param survey_point IfcPoint to assign as the new geometry.
 */
bool ifcopenshell_cogo_assign_survey_point(ifcopenshell_instance_t* annotation, ifcopenshell_instance_t* survey_point);
/**
 * Update the coordinates of the survey point inside an existing annotation.
 *
 * Reads the first item from the annotation's IfcShapeRepresentation and
 * overwrites its Coordinates attribute. If the point currently has two
 * coordinates, only x and y are written; otherwise all three are used.
 *
 * @param annotation IfcAnnotation containing the survey point.
 * @param x Easting or X coordinate in model units.
 * @param y Northing or Y coordinate in model units.
 * @param z Elevation or Z coordinate in model units.
 */
bool ifcopenshell_cogo_edit_survey_point(ifcopenshell_instance_t* annotation, double x, double y, double z);
/**
 * Compute a derived attribute value for an instance.
 *
 * Evaluates derived attributes defined in the IFC schema (e.g.
 * IfcDirection.DirectionRatios) for the given instance and attribute.
 *
 * @param instance The entity instance.
 * @param attribute_name The name of the derived attribute.
 * @return The computed value, or empty on error. Free with value_free.
 */
bool ifcopenshell_compute_derived(ifcopenshell_instance_t* instance, const char* attribute_name, ifcopenshell_value_t** out_result);
/**
 * Add a new IfcMetric constraint. If an objective is provided, the metric
 * is appended to the objective's benchmark values.
 */
bool ifcopenshell_constraint_add_metric(ifcopenshell_file_t* file, ifcopenshell_instance_t* objective, ifcopenshell_instance_t** out_result);
/**
 * Create a chain of IfcReference entities from a dot-separated path and
 * assign it to a metric's ReferencePath attribute.
 *
 * For example, a path "Pset_WallCommon.FireRating" produces two IfcReference
 * entities linked by InnerReference, with AttributeIdentifier set to
 * "Pset_WallCommon" and "FireRating" respectively.
 *
 * @return The created IfcReference chain in order from outermost to innermost.
 */
bool ifcopenshell_constraint_add_metric_reference(ifcopenshell_file_t* file, ifcopenshell_instance_t* metric, const char* reference_path, ifcopenshell_parse_instance_list_t** out_result);
/**
 * Create a new IfcObjective constraint with default attributes.
 *
 * Sets Name to "Unnamed", ConstraintGrade and ObjectiveQualifier to "NOTDEFINED".
 */
bool ifcopenshell_constraint_add_objective(ifcopenshell_file_t* file, ifcopenshell_instance_t** out_result);
/**
 * Assign a constraint to a list of products via IfcRelAssociatesConstraint.
 *
 * If the constraint is already associated with some of the products, only
 * new products are added to the existing relationship.
 */
bool ifcopenshell_constraint_assign_constraint(ifcopenshell_file_t* file, const ifcopenshell_constraint_assign_constraint_options_t* options, ifcopenshell_instance_t** out_result);
/**
 * Remove a constraint entity and clean up orphaned IfcRelAssociatesConstraint
 * relationships that reference it.
 */
bool ifcopenshell_constraint_remove_constraint(ifcopenshell_file_t* file, ifcopenshell_instance_t* constraint);
/**
 * Remove an IfcMetric and its reference path chain.
 *
 * Deletes the metric's IfcReference chain (via InnerReference), then removes
 * the metric entity. Orphaned IfcRelAssociatesConstraint and
 * IfcResourceConstraintRelationship entities are also deleted.
 */
bool ifcopenshell_constraint_remove_metric(ifcopenshell_file_t* file, ifcopenshell_instance_t* metric);
/**
 * Remove constraint associations from products.
 *
 * Products are removed from existing IfcRelAssociatesConstraint
 * relationships. If no products remain, the relationship is removed.
 */
bool ifcopenshell_constraint_unassign_constraint(ifcopenshell_file_t* file, const ifcopenshell_constraint_unassign_constraint_options_t* options);
/**
 * Create a geometric representation context or subcontext.
 *
 * When parent is omitted, creates an IfcGeometricRepresentationContext with
 * a world coordinate system and attaches it to the IfcProject's
 * RepresentationContexts. A context_type of "Plan" creates a 2D context
 * (coordinate space dimension 2); all other values create a 3D context.
 *
 * When parent is provided, creates an IfcGeometricRepresentationSubContext
 * as a child of the given parent context.
 */
bool ifcopenshell_context_add_context(ifcopenshell_file_t* file, const ifcopenshell_context_add_context_options_t* options, ifcopenshell_instance_t** out_result);
/**
 * Edit attributes of an existing geometric representation context.
 *
 * Applies attribute key-value pairs from the props builder to the context entity.
 */
bool ifcopenshell_context_edit_context(ifcopenshell_file_t* file, ifcopenshell_instance_t* context, void* attributes);
/**
 * Remove a geometric representation context and its subcontexts recursively.
 *
 * For subcontexts, references from IfcCoordinateOperation entities are
 * deep-removed; other referencing entities are redirected to the parent
 * context. For top-level contexts, representations using the context are
 * unassigned from their elements and removed.
 */
bool ifcopenshell_context_remove_context(ifcopenshell_file_t* file, ifcopenshell_instance_t* context);
/**
 * Assign a planning control or constraint to objects via IfcRelAssignsToControl.
 *
 * Objects already assigned to the control are skipped. If an existing
 * IfcRelAssignsToControl relationship exists for the control, new objects
 * are merged into it.
 */
bool ifcopenshell_control_assign_control(ifcopenshell_file_t* file, const ifcopenshell_control_assign_control_options_t* options, ifcopenshell_instance_t** out_result);
/**
 * Remove objects from an IfcRelAssignsToControl relationship.
 *
 * If no related objects remain after removal, the relationship is deleted.
 */
bool ifcopenshell_control_unassign_control(ifcopenshell_file_t* file, const ifcopenshell_control_unassign_control_options_t* options);
/**
 * Create an IfcCostItem entity.
 *
 * When both cost_schedule and cost_item are provided, cost_schedule takes
 * precedence and the item is assigned to it via IfcRelAssignsToControl.
 * When only cost_item is provided, the item is nested under it via
 * IfcRelNests.
 *
 * @param file File that receives the new entity.
 * @param options Schedule/parent placement and ownership options.
 * @return Newly created IfcCostItem.
 */
bool ifcopenshell_cost_add_cost_item(ifcopenshell_file_t* file, const ifcopenshell_cost_add_cost_item_options_t* options, ifcopenshell_instance_t** out_result);
/**
 * Create an IfcPhysicalQuantity and add it to a cost item.
 *
 * Creates a quantity of the given IFC class (e.g. "IfcQuantityLength",
 * "IfcQuantityCount") with Name "Unnamed" and appends it to the cost item's
 * CostQuantities aggregate. For IfcQuantityCount, the initial value is set
 * to the number of controlled objects.
 *
 * @param file File that receives the new entity.
 * @param cost_item IfcCostItem to receive the quantity.
 * @param ifc_class IFC quantity class name.
 * @return Newly created IfcPhysicalQuantity.
 */
bool ifcopenshell_cost_add_cost_item_quantity(ifcopenshell_file_t* file, ifcopenshell_instance_t* cost_item, const char* ifc_class, ifcopenshell_instance_t** out_result);
/**
 * Create an IfcCostSchedule entity.
 *
 * Creates the schedule with the given name, predefined type, and update date.
 * The update_date is stored as an IfcDateAndTime on IFC2X3 or as a string
 * on IFC4+.
 *
 * @param file File that receives the new entity.
 * @param name Schedule name. May be null or empty for no name.
 * @param predefined_type IFC predefined type enum value (e.g. "BUDGET", "COSTPLAN").
 * @param update_date ISO 8601 date-time string for the UpdateDate attribute.
 * @param owner_history Owner history for the new entity. May be std::nullopt.
 * @return Newly created IfcCostSchedule.
 */
bool ifcopenshell_cost_add_cost_schedule(ifcopenshell_file_t* file, const char* name, const char* predefined_type, const char* update_date, ifcopenshell_instance_t* owner_history, ifcopenshell_instance_t** out_result);
/**
 * Create an IfcCostValue and attach it to a parent entity.
 *
 * Appends the new IfcCostValue to the parent's CostValues (IfcCostItem),
 * BaseCosts (IfcConstructionResource), or Components (IfcCostValue)
 * aggregate, depending on the parent's type.
 *
 * @param file File that receives the new entity.
 * @param parent IfcCostItem, IfcConstructionResource, or IfcCostValue to receive the value.
 * @return Newly created IfcCostValue.
 */
bool ifcopenshell_cost_add_cost_value(ifcopenshell_file_t* file, ifcopenshell_instance_t* parent, ifcopenshell_instance_t** out_result);
/**
 * Assign product quantities to a cost item.
 *
 * For each product, creates an IfcRelAssignsToControl linking the cost item
 * to the product. If prop_name is provided, matching quantities from the
 * products' IfcElementQuantity property sets are collected into the cost
 * item's CostQuantities. If prop_name is null/empty and the cost item has a
 * single IfcQuantityCount, its value is updated to the count of assigned
 * non-resource objects. IfcSpatialElement products are skipped.
 *
 * @param file File containing the cost item and products.
 * @param cost_item IfcCostItem to assign quantities to.
 * @param products Products whose quantities to collect.
 * @param prop_name Quantity property name to match. May be null.
 * @param options Ownership options for the assignment relationship.
 */
bool ifcopenshell_cost_assign_cost_item_quantity(ifcopenshell_file_t* file, ifcopenshell_instance_t* cost_item, const ifcopenshell_instance_list_t* products, const char* prop_name, const ifcopenshell_cost_assign_cost_item_quantity_options_t* options);
/**
 * Assign a cost rate's values to a cost item.
 *
 * Removes the cost item's existing CostValues, then assigns the cost rate's
 * CostValues aggregate directly (sharing, not copying).
 *
 * @param file File containing both entities.
 * @param cost_item IfcCostItem to receive the values.
 * @param cost_rate IfcCostItem whose CostValues to assign.
 */
bool ifcopenshell_cost_assign_cost_value(ifcopenshell_file_t* file, ifcopenshell_instance_t* cost_item, ifcopenshell_instance_t* cost_rate);
/**
 * Calculate and populate cost values from assigned resources.
 *
 * Removes existing CostValues from the cost item, then for each
 * IfcConstructionResource assigned (directly or via an IfcTask), creates
 * an IfcCostValue with the resource's base cost rate multiplied by its
 * quantity. Resources with day-based units are converted using an 8-hour
 * workday.
 *
 * @param file File containing the cost item.
 * @param cost_item IfcCostItem to populate.
 */
bool ifcopenshell_cost_calculate_cost_item_resource_value(ifcopenshell_file_t* file, ifcopenshell_instance_t* cost_item);
/**
 * Deep-copy an IfcCostItem and its nested children.
 *
 * Creates a deep copy of the cost item including nested child items,
 * property sets, and IfcRelDefinesByProperties relationships. Returns
 * the list of all newly created cost items (root first, then descendants).
 *
 * @param file File that receives the copied entities.
 * @param cost_item IfcCostItem to copy.
 * @return Vector of newly created IfcCostItem entities (owned, caller must not free).
 */
bool ifcopenshell_cost_copy_cost_item(ifcopenshell_file_t* file, ifcopenshell_instance_t* cost_item, ifcopenshell_parse_instance_list_t** out_result);
/**
 * Deep-copy cost values from one cost item to another.
 *
 * Removes existing CostValues from the destination, then deep-copies each
 * IfcCostValue (and its component tree) from the source.
 *
 * @param file File containing both cost items.
 * @param source IfcCostItem to copy values from.
 * @param destination IfcCostItem to copy values to.
 */
bool ifcopenshell_cost_copy_cost_item_values(ifcopenshell_file_t* file, ifcopenshell_instance_t* source, ifcopenshell_instance_t* destination);
/**
 * Deep-copy an IfcCostSchedule and all its controlled cost items.
 *
 * Shallow-copies the schedule, then deep-copies each controlled IfcCostItem
 * and assigns the copies to the new schedule via IfcRelAssignsToControl.
 *
 * @param file File that receives the copied entities.
 * @param cost_schedule IfcCostSchedule to copy.
 * @param options Ownership options for the new schedule and its items.
 * @return Newly created IfcCostSchedule.
 */
bool ifcopenshell_cost_copy_cost_schedule(ifcopenshell_file_t* file, ifcopenshell_instance_t* cost_schedule, const ifcopenshell_cost_copy_cost_schedule_options_t* options, ifcopenshell_instance_t** out_result);
/**
 * Edit attributes of an IfcCostItem.
 *
 * Applies attribute changes from the property bag to the cost item.
 *
 * @param file File containing the cost item.
 * @param cost_item IfcCostItem entity to edit.
 * @param attributes Property bag of attribute name/value pairs.
 */
bool ifcopenshell_cost_edit_cost_item(ifcopenshell_file_t* file, ifcopenshell_instance_t* cost_item, void* attributes);
/**
 * Edit attributes of an IfcPhysicalQuantity.
 *
 * Applies attribute changes from the property bag to the physical quantity.
 *
 * @param file File containing the quantity.
 * @param physical_quantity IfcPhysicalQuantity entity to edit.
 * @param attributes Property bag of attribute name/value pairs.
 */
bool ifcopenshell_cost_edit_cost_item_quantity(ifcopenshell_file_t* file, ifcopenshell_instance_t* physical_quantity, void* attributes);
/**
 * Edit attributes of an IfcCostSchedule.
 *
 * Applies attribute changes from the property bag to the cost schedule.
 *
 * @param file File containing the cost schedule.
 * @param cost_schedule IfcCostSchedule entity to edit.
 * @param attributes Property bag of attribute name/value pairs.
 */
bool ifcopenshell_cost_edit_cost_schedule(ifcopenshell_file_t* file, ifcopenshell_instance_t* cost_schedule, void* attributes);
/**
 * Edit attributes of an IfcCostValue.
 *
 * Applies attribute changes from the property bag. The AppliedValue attribute
 * is wrapped as IfcMonetaryMeasure. When edit_unit_basis is true, the
 * UnitBasis (IfcMeasureWithUnit) is replaced or cleared.
 *
 * @param file File containing the cost value.
 * @param cost_value IfcCostValue entity to edit.
 * @param attributes Property bag of attribute name/value pairs.
 * @param options Unit basis replacement options.
 */
bool ifcopenshell_cost_edit_cost_value(ifcopenshell_file_t* file, ifcopenshell_instance_t* cost_value, void* attributes, const ifcopenshell_cost_edit_cost_value_options_t* options);
/**
 * Set an IfcCostValue's attributes from a formula expression.
 *
 * Parses a text formula (e.g. "100*5", "sum(10+20)") and applies the
 * resulting value tree to the cost value's AppliedValue, Category,
 * ArithmeticOperator, and Component cost values.
 *
 * @param file File containing the cost value.
 * @param cost_value IfcCostValue entity to update.
 * @param formula Formula string to parse.
 */
bool ifcopenshell_cost_edit_cost_value_formula(ifcopenshell_file_t* file, ifcopenshell_instance_t* cost_value, const char* formula);
/**
 * Remove an IfcCostItem and its nested children from the file.
 *
 * Recursively removes nested child cost items, then removes the item's
 * IfcRelNests and IfcRelAssignsToControl relationships (when the item is
 * the sole RelatedObject), and finally removes the item itself.
 *
 * @param file File containing the cost item.
 * @param cost_item IfcCostItem to remove.
 */
bool ifcopenshell_cost_remove_cost_item(ifcopenshell_file_t* file, ifcopenshell_instance_t* cost_item);
/**
 * Remove a physical quantity from a cost item.
 *
 * If the quantity is only referenced by this cost item, it is deleted from
 * the file. Otherwise it is removed from the CostQuantities aggregate only.
 *
 * @param file File containing the cost item.
 * @param cost_item IfcCostItem that owns the quantity.
 * @param physical_quantity IfcPhysicalQuantity to remove.
 */
bool ifcopenshell_cost_remove_cost_item_quantity(ifcopenshell_file_t* file, ifcopenshell_instance_t* cost_item, ifcopenshell_instance_t* physical_quantity);
/**
 * Remove an IfcCostSchedule and all its controlled cost items from the file.
 *
 * Removes each controlled IfcCostItem (recursively), then removes the
 * schedule's IfcRelAssignsToControl relationships and the schedule itself.
 *
 * @param file File containing the cost schedule.
 * @param cost_schedule IfcCostSchedule to remove.
 */
bool ifcopenshell_cost_remove_cost_schedule(ifcopenshell_file_t* file, ifcopenshell_instance_t* cost_schedule);
/**
 * Remove an IfcCostValue from its parent and the file.
 *
 * If the cost value is only referenced by this parent, it is deleted from
 * the file. Otherwise it is removed from the parent's CostValues, BaseCosts,
 * or Components aggregate, depending on the parent's type.
 *
 * @param file File containing the cost value.
 * @param parent IfcCostItem, IfcConstructionResource, or IfcCostValue that owns the value.
 * @param cost_value IfcCostValue to remove.
 */
bool ifcopenshell_cost_remove_cost_value(ifcopenshell_file_t* file, ifcopenshell_instance_t* parent, ifcopenshell_instance_t* cost_value);
/**
 * Remove product quantity assignments from a cost item.
 *
 * Removes the cost item's control relationship over the given products and
 * strips any quantities from CostQuantities that are referenced by those
 * products' IfcElementQuantity property sets. If a lone IfcQuantityCount
 * remains, its value is updated to the remaining object count or removed
 * if zero.
 *
 * @param file File containing the cost item and products.
 * @param cost_item IfcCostItem to unassign from.
 * @param products Products to unassign.
 * @param options Ownership options for updating the relationship.
 */
bool ifcopenshell_cost_unassign_cost_item_quantity(ifcopenshell_file_t* file, ifcopenshell_instance_t* cost_item, const ifcopenshell_instance_list_t* products, const ifcopenshell_cost_unassign_cost_item_quantity_options_t* options);
/**
 * Create a new IfcDocumentInformation and link it to a parent.
 *
 * If the parent is an IfcProject or IfcContext, the document is linked via
 * IfcRelAssociatesDocument. If the parent is another IfcDocumentInformation,
 * the new document is linked as a child via IfcDocumentInformationRelationship.
 * If parent is omitted, the first IfcProject is used.
 */
bool ifcopenshell_document_add_information(ifcopenshell_file_t* file, const ifcopenshell_document_add_information_options_t* options, ifcopenshell_instance_t** out_result);
/**
 * Create a new IfcDocumentReference.
 *
 * If an IfcDocumentInformation is provided, the reference is linked to it
 * (via ReferencedDocument in IFC4+ or DocumentReferences in IFC2X3).
 */
bool ifcopenshell_document_add_reference(ifcopenshell_file_t* file, ifcopenshell_instance_t* information, ifcopenshell_instance_t** out_result);
/**
 * Assign a document (IfcDocumentInformation or IfcDocumentReference) to products.
 *
 * Creates or updates an IfcRelAssociatesDocument relationship. Products
 * already associated with the document are skipped.
 */
bool ifcopenshell_document_assign_document(ifcopenshell_file_t* file, const ifcopenshell_document_assign_document_options_t* options, ifcopenshell_instance_t** out_result);
/**
 * Remove an IfcDocumentInformation and its entire document tree.
 *
 * Recursively deletes all child IfcDocumentReference entities, child
 * IfcDocumentInformation entities (via IsPointer relationships), and all
 * associated IfcRelAssociatesDocument relationships.
 */
bool ifcopenshell_document_remove_information(ifcopenshell_file_t* file, ifcopenshell_instance_t* information);
/**
 * Remove an IfcDocumentReference and its association relationships.
 *
 * Deletes the reference entity and all IfcRelAssociatesDocument relationships
 * that reference it.
 */
bool ifcopenshell_document_remove_reference(ifcopenshell_file_t* file, ifcopenshell_instance_t* reference);
/**
 * Remove products from a document association.
 *
 * Products are removed from IfcRelAssociatesDocument relationships. If no
 * products remain in a relationship, the relationship is deleted.
 */
bool ifcopenshell_document_unassign_document(ifcopenshell_file_t* file, const ifcopenshell_document_unassign_document_options_t* options);
/**
 * Assign a product to an annotation object via IfcRelAssignsToProduct.
 *
 * When the relating product is an IfcGridAxis, the assignment is created
 * against the owning IfcGrid with the axis tag as the relationship name;
 * a new IfcRelAssignsToProduct is always created for grid axes even if one
 * already exists for the same axis tag. For non-grid products, if an
 * existing IfcRelAssignsToProduct already references the relating product,
 * the related object is appended to its RelatedObjects aggregate instead
 * of creating a new relationship. Returns a null handle on exact duplicate.
 *
 * @param file IFC file to modify.
 * @param options Assignment parameters.
 * @return IfcRelAssignsToProduct relationship, or null handle on duplicate or failure.
 */
bool ifcopenshell_drawing_assign_product(ifcopenshell_file_t* file, const ifcopenshell_drawing_assign_product_options_t* options, ifcopenshell_instance_t** out_result);
/**
 * Remove a product assignment from an annotation object.
 *
 * Finds and removes the IfcRelAssignsToProduct relating the product to the
 * object. If the relationship contains only this one related object, the
 * entire relationship is removed; otherwise only the object is removed from
 * the RelatedObjects aggregate.
 *
 * @param file IFC file to modify.
 * @param options Unassignment parameters.
 */
bool ifcopenshell_drawing_unassign_product(ifcopenshell_file_t* file, const ifcopenshell_drawing_unassign_product_options_t* options);
/**
 * Return the aggregate parent of an element.
 *
 * Follows the Decomposes inverse to find the RelatingObject via
 * IfcRelAggregates. In IFC2X3, returns empty if the relationship
 * is IfcRelNests rather than IfcRelAggregates.
 *
 * @param instance The element to query.
 * @return The aggregate parent, or empty if not aggregated.
 */
bool ifcopenshell_element_get_aggregate(ifcopenshell_instance_t* instance, ifcopenshell_instance_t** out_result);
/**
 * Return elements directly contained in a spatial element.
 *
 * Follows ContainsElements to find RelatedElements.
 *
 * @param element The spatial element (e.g. IfcBuildingStorey).
 * @return List of contained elements.
 */
bool ifcopenshell_element_get_contained(ifcopenshell_instance_t* element, ifcopenshell_parse_instance_list_t** out_result);
/**
 * Return the spatial container of an element.
 *
 * By default walks up the spatial hierarchy to find an indirect container
 * (e.g. a building storey for an element inside an aggregate). When
 * direct_only is true, only a direct ContainedInStructure relationship
 * is considered.
 *
 * @param instance The element to query.
 * @param options Container lookup options.
 * @return The spatial container, or empty if not contained.
 */
bool ifcopenshell_element_get_container(ifcopenshell_instance_t* instance, const ifcopenshell_element_get_container_options_t* options, ifcopenshell_instance_t** out_result);
/**
 * Return the controls assigned to an element.
 *
 * Follows HasAssignments to find IfcRelAssignsToControl relationships.
 *
 * @param element The element to query.
 * @return List of IfcControl entities.
 */
bool ifcopenshell_element_get_controls(ifcopenshell_instance_t* element, ifcopenshell_parse_instance_list_t** out_result);
/**
 * Return the full spatial decomposition of an element.
 *
 * Collects all subelements by traversing ContainsElements,
 * IsDecomposedBy, HasOpenings, HasFillings, and IsNestedBy
 * relationships. When is_recursive is true (default), the traversal
 * is breadth-first through the entire hierarchy.
 *
 * @param element The root element.
 * @param options Decomposition traversal options.
 * @return Ordered list of decomposed subelements (excluding the root).
 */
bool ifcopenshell_element_get_decomposition(ifcopenshell_instance_t* element, const ifcopenshell_element_get_decomposition_options_t* options, ifcopenshell_parse_instance_list_t** out_result);
/**
 * Return elements assigned to a presentation layer.
 *
 * Follows AssignedItems on the IfcPresentationLayerAssignment to find
 * all elements whose geometry is on the layer.
 *
 * @param layer The IfcPresentationLayerAssignment entity.
 * @return List of elements on the layer.
 */
bool ifcopenshell_element_get_elements_by_layer(ifcopenshell_instance_t* layer, ifcopenshell_parse_instance_list_t** out_result);
/**
 * Return elements that use a material, directly or via a material set.
 *
 * Traverses inverse relationships from the material to find all elements
 * associated through IfcRelAssociatesMaterial, as well as elements using
 * the material as part of a layer, profile, constituent, or material list.
 *
 * @param material The IfcMaterial or material set entity.
 * @return List of elements using the material.
 */
bool ifcopenshell_element_get_elements_by_material(ifcopenshell_instance_t* material, ifcopenshell_parse_instance_list_t** out_result);
/**
 * Return elements that use a profile definition in their representation.
 *
 * Traverses from the IfcProfileDef through representation items to find
 * all elements whose geometry references the profile.
 *
 * @param profile The IfcProfileDef entity.
 * @return List of elements using the profile.
 */
bool ifcopenshell_element_get_elements_by_profile(ifcopenshell_instance_t* profile, ifcopenshell_parse_instance_list_t** out_result);
/**
 * Return elements that use a geometric representation.
 *
 * Follows OfProductRepresentation and RepresentationMap to find all
 * IfcProduct and IfcTypeProduct entities sharing the representation.
 *
 * @param representation The IfcShapeRepresentation entity.
 * @return List of elements using the representation.
 */
bool ifcopenshell_element_get_elements_by_representation(ifcopenshell_instance_t* representation, ifcopenshell_parse_instance_list_t** out_result);
/**
 * Return elements whose geometric representation uses a style.
 *
 * Traverses from IfcSurfaceStyle through IfcStyledItem and
 * IfcShapeRepresentation to find all elements using the style.
 *
 * @param style The IfcPresentationStyle entity.
 * @return List of elements using the style.
 */
bool ifcopenshell_element_get_elements_by_style(ifcopenshell_instance_t* style, ifcopenshell_parse_instance_list_t** out_result);
/**
 * Return the opening element that an element fills.
 *
 * Follows FillsVoids to find the RelatingOpeningElement.
 * Typically applies to windows and doors.
 *
 * @param element The filling element (e.g. IfcWindow).
 * @return The IfcOpeningElement being filled, or empty if none.
 */
bool ifcopenshell_element_get_filled_void(ifcopenshell_instance_t* element, ifcopenshell_instance_t** out_result);
/**
 * Return the groups that an element is assigned to.
 *
 * Follows HasAssignments to find IfcRelAssignsToGroup relationships.
 *
 * @param element The element to query.
 * @return List of IfcGroup entities.
 */
bool ifcopenshell_element_get_groups(ifcopenshell_instance_t* element, ifcopenshell_parse_instance_list_t** out_result);
/**
 * Return the presentation layers that an element is part of.
 *
 * Traverses the element's representation to find IfcPresentationLayerAssignment
 * entities.
 *
 * @param element The element to query.
 * @return List of IfcPresentationLayerAssignment entities.
 */
bool ifcopenshell_element_get_layers(ifcopenshell_instance_t* element, ifcopenshell_parse_instance_list_t** out_result);
/**
 * Return the material associated with an element.
 *
 * Searches HasAssociations for IfcRelAssociatesMaterial. When
 * should_inherit is true (default), falls back to the element's type.
 * When should_skip_usage is true, unwraps material set usages
 * (IfcMaterialLayerSetUsage, IfcMaterialProfileSetUsage) to their
 * underlying set.
 *
 * @param instance The element to query.
 * @param options Material lookup options.
 * @return The material entity, or empty if none is associated.
 */
bool ifcopenshell_element_get_material(ifcopenshell_instance_t* instance, const ifcopenshell_element_get_material_options_t* options, ifcopenshell_instance_t** out_result);
/**
 * Return the nest parent of an element.
 *
 * Follows the Nests inverse (IFC4+) or Decomposes/IfcRelNests (IFC2X3)
 * to find the RelatingObject.
 *
 * @param instance The element to query.
 * @return The nesting parent, or empty if not nested.
 */
bool ifcopenshell_element_get_nest(ifcopenshell_instance_t* instance, ifcopenshell_instance_t** out_result);
/**
 * Return opening elements associated with an element.
 *
 * Follows HasOpenings to find RelatedOpeningElement. Also traverses
 * aggregate parents to collect inherited openings.
 *
 * @param element The building element (e.g. IfcWall).
 * @return List of IfcOpeningElement entities.
 */
bool ifcopenshell_element_get_openings(ifcopenshell_instance_t* element, ifcopenshell_parse_instance_list_t** out_result);
/**
 * Return the parent of an element in the spatial hierarchy.
 *
 * Checks spatial containment, aggregation, nesting, filling, and voiding
 * relationships in that order, returning the first parent found.
 *
 * @param instance The element to query.
 * @return The parent element, or empty if at the top of the hierarchy.
 */
bool ifcopenshell_element_get_parent(ifcopenshell_instance_t* instance, ifcopenshell_instance_t** out_result);
/**
 * Return the direct aggregation parts of an element.
 *
 * Follows IsDecomposedBy to find RelatedObjects via IfcRelAggregates.
 *
 * @param element The element to query.
 * @return List of aggregated parts.
 */
bool ifcopenshell_element_get_parts(ifcopenshell_instance_t* element, ifcopenshell_parse_instance_list_t** out_result);
/**
 * Return property set and quantity identifiers of an element.
 *
 * Collects IfcPropertySet, IfcElementQuantity, and related property
 * definition entities. For IfcTypeObject, reads HasPropertySets.
 * For other objects, reads IsDefinedBy/IfcRelDefinesByProperties.
 * When should_inherit is true (default), also includes property sets
 * from the element's type.
 *
 * @param element The element to query.
 * @param options Property set filtering options.
 * @return List of property definition entities.
 */
bool ifcopenshell_element_get_pset_ids(ifcopenshell_instance_t* element, const ifcopenshell_element_get_pset_ids_options_t* options, ifcopenshell_parse_instance_list_t** out_result);
/**
 * Return elements that have an external reference assigned.
 *
 * For IfcExternalReference subtypes, follows ExternalReferenceForResources.
 * For classification/document/library references, follows the appropriate
 * inverse attribute.
 *
 * @param reference The IfcExternalReference or IfcExternalInformation entity.
 * @return List of elements using the reference.
 */
bool ifcopenshell_element_get_referenced_elements(ifcopenshell_instance_t* reference, ifcopenshell_parse_instance_list_t** out_result);
/**
 * Return spatial elements that reference an element.
 *
 * Follows ReferencedInStructures to find RelatingStructure.
 * Useful for multi-storey elements or elements spanning multiple spaces.
 *
 * @param element The element to query.
 * @return List of referenced IfcSpatialElement entities.
 */
bool ifcopenshell_element_get_referenced_structures(ifcopenshell_instance_t* element, ifcopenshell_parse_instance_list_t** out_result);
/**
 * Return the shape aspects of an element.
 *
 * For an IfcProduct, reads HasShapeAspects from the Representation.
 * For an IfcTypeProduct, reads from RepresentationMaps. When
 * should_inherit is true (default), also includes shape aspects from
 * the element's type.
 *
 * @param element The IfcProduct or IfcTypeProduct.
 * @param options Shape aspect lookup options.
 * @return List of IfcShapeAspect entities.
 */
bool ifcopenshell_element_get_shape_aspects(ifcopenshell_instance_t* element, const ifcopenshell_element_get_shape_aspects_options_t* options, ifcopenshell_parse_instance_list_t** out_result);
/**
 * Return elements referenced by a spatial structure.
 *
 * Follows ReferencesElements to find RelatedElements.
 *
 * @param structure The spatial element (e.g. IfcBuildingStorey).
 * @return List of referenced elements.
 */
bool ifcopenshell_element_get_structure_referenced_elements(ifcopenshell_instance_t* structure, ifcopenshell_parse_instance_list_t** out_result);
/**
 * Return the surface styles used in an element's representation.
 *
 * Collects styles from the element's materials and body representation,
 * including styles on mapped items and boolean operands.
 *
 * @param element The element to query.
 * @return List of IfcSurfaceStyle entities.
 */
bool ifcopenshell_element_get_styles(ifcopenshell_instance_t* element, ifcopenshell_parse_instance_list_t** out_result);
/**
 * Return the type element associated with an element occurrence.
 *
 * For an IfcTypeObject, returns the element itself. For an IfcObject,
 * follows IsTypedBy (IFC4+) or IsDefinedBy/IfcRelDefinesByType (IFC2X3).
 *
 * @param instance The element to query.
 * @return The related type element, or empty if none.
 */
bool ifcopenshell_element_get_type(ifcopenshell_instance_t* instance, ifcopenshell_instance_t** out_result);
/**
 * Return all occurrences of a type element.
 *
 * Follows Types (IFC4+) or ObjectTypeOf (IFC2X3) to find the
 * RelatedObjects.
 *
 * @param type_element The type element (e.g. IfcWallType).
 * @return List of element occurrences of that type.
 */
bool ifcopenshell_element_get_types(ifcopenshell_instance_t* type_element, ifcopenshell_parse_instance_list_t** out_result);
/**
 * Return the building element voided by an opening.
 *
 * Follows VoidsElements to find the RelatingBuildingElement.
 *
 * @param element The IfcOpeningElement.
 * @return The building element being voided, or empty if none.
 */
bool ifcopenshell_element_get_voided_element(ifcopenshell_instance_t* element, ifcopenshell_instance_t** out_result);
/**
 * Check whether an element's predefined type is user-defined.
 *
 * Returns true if the element's PredefinedType is USERDEFINED, or if
 * the type's PredefinedType is USERDEFINED or not set and an
 * ElementType or ObjectType is present.
 *
 * @param element The element to check.
 * @return True if the predefined type is user-defined.
 */
bool ifcopenshell_element_is_userdefined_type(ifcopenshell_instance_t* element, bool* out_result);
/**
 * Recursively remove an element and its owned subgraph.
 *
 * Traverses forward through the element's subgraph. Each subelement is
 * deleted only if it has no inverses outside the subgraph. Protected
 * elements and elements with external references are preserved.
 *
 * @param element The root element to remove.
 */
bool ifcopenshell_element_remove_deep(ifcopenshell_instance_t* element);
/**
 * Replace all references to an element with another element.
 *
 * Traverses all inverse relationships of old_element and substitutes
 * references to old_element with new_element.
 *
 * @param old_element The element to be replaced.
 * @param new_element The replacement element.
 */
bool ifcopenshell_element_replace_element(ifcopenshell_instance_t* old_element, ifcopenshell_instance_t* new_element);
/**
 * Recursively remove an entity and its owned subgraph.
 *
 * Equivalent to entity_remove_deep_with_options with empty options.
 * The start element must have no inverses outside the subgraph.
 *
 * @param instance The root entity to remove.
 */
bool ifcopenshell_entity_remove_deep(ifcopenshell_instance_t* instance);
/**
 * Recursively remove an entity and its owned subgraph with fine-grained control.
 *
 * Traverses forward through the entity's subgraph. Each subelement is
 * deleted only if it has fewer than two inverse references, or all of
 * its inverses are within the subgraph. The also_consider list extends
 * the subgraph for inverse checking. The do_not_delete list protects
 * specific entities from deletion.
 *
 * @param instance The root entity to remove.
 * @param options Additional control over the removal process.
 */
bool ifcopenshell_entity_remove_deep_with_options(ifcopenshell_instance_t* instance, const ifcopenshell_entity_remove_deep_options_t* options);
/**
 * Associate a feature element with a host element.
 *
 * For IfcFeatureElementSubtraction (e.g. IfcOpeningElement), creates
 * IfcRelVoidsElement. For IfcFeatureElementAddition (e.g. IfcProjectionElement),
 * creates IfcRelProjectsElement. For IfcSurfaceFeature, creates
 * IfcRelAdheresToElement (IFC4X3) or aggregates via IfcRelAggregates (IFC4).
 * If the feature is already associated with the same host, returns the
 * existing relationship. If associated with a different host, the old
 * relationship is removed first.
 */
bool ifcopenshell_feature_add_feature(ifcopenshell_file_t* file, const ifcopenshell_feature_add_feature_options_t* options, ifcopenshell_instance_t** out_result);
/**
 * Fill an opening with an element via IfcRelFillsElement.
 *
 * If the element already fills the same opening, returns the existing
 * relationship. If it fills a different opening, the old relationship is
 * removed first.
 */
bool ifcopenshell_feature_add_filling(ifcopenshell_file_t* file, ifcopenshell_instance_t* opening, ifcopenshell_instance_t* element, ifcopenshell_instance_t** out_result);
/**
 * Remove a feature element and its relationships.
 *
 * For IfcFeatureElementSubtraction subclasses, removes the IfcRelVoidsElement.
 * For IfcFeatureElementAddition subclasses, removes the IfcRelProjectsElement.
 * For IfcSurfaceFeature in IFC4, unassigns from the aggregate parent. In
 * other schemas, no feature-specific relationship is removed before the
 * element itself is removed via root_remove_product.
 * IfcOpeningElement fillings are also removed. root_remove_product cleans
 * up nested elements, property sets, representations, and other inverse
 * relationships.
 */
bool ifcopenshell_feature_remove_feature(ifcopenshell_file_t* file, const ifcopenshell_feature_remove_feature_options_t* options);
/**
 * Remove a filling relationship for an element.
 *
 * Finds and deletes the IfcRelFillsElement where the element is the
 * RelatedBuildingElement.
 */
bool ifcopenshell_feature_remove_filling(ifcopenshell_file_t* file, ifcopenshell_instance_t* element);
/**
 * Create an axis representation (Curve2D or Curve3D) from a polyline.
 *
 * Points are converted from SI metres to project units.
 *
 * @param file IFC file that receives the representation.
 * @param context IfcGeometricRepresentationContext.
 * @param axis Ordered XY or XYZ points defining the axis curve.
 * @return IfcShapeRepresentation entity, or a null handle on failure.
 */
bool ifcopenshell_geometry_add_axis_representation(ifcopenshell_file_t* file, ifcopenshell_instance_t* context, const ifcopenshell_double_list_list_t* axis, ifcopenshell_instance_t** out_result);
/**
 * Add boolean operands to a solid representation item.
 *
 * Creates IfcBooleanResult (or IfcBooleanClippingResult for DIFFERENCE with
 * half-space solids) chaining each second_item to the first. The first item
 * walks up any existing boolean chain to find the top-level operand. Returns
 * the created boolean result entities in order.
 *
 * @param file IFC file that receives the boolean entities.
 * @param first_item Base solid operand.
 * @param second_items Additional operands to apply.
 * @param operator_type Boolean operator: "DIFFERENCE", "UNION", or "INTERSECTION".
 * @return Created IfcBooleanResult entities, or empty on failure.
 */
bool ifcopenshell_geometry_add_boolean(ifcopenshell_file_t* file, ifcopenshell_instance_t* first_item, const ifcopenshell_instance_list_t* second_items, const char* operator_type, ifcopenshell_parse_instance_list_t** out_result);
/**
 * Create a door representation with lining and panel geometry.
 *
 * @param file IFC file that receives the representation.
 * @param options Door dimensions, operation type, and lining/panel properties.
 * @return IfcShapeRepresentation entity, or a null handle on failure.
 */
bool ifcopenshell_geometry_add_door_representation(ifcopenshell_file_t* file, const ifcopenshell_geometry_add_door_representation_options_t* options, ifcopenshell_instance_t** out_result);
/**
 * Create a footprint representation (GeometricCurveSet) from curves.
 *
 * @param file IFC file that receives the representation.
 * @param context IfcGeometricRepresentationContext.
 * @param curves IfcCurve entities to include in the footprint.
 * @return IfcShapeRepresentation entity, or a null handle on failure.
 */
bool ifcopenshell_geometry_add_footprint_representation(ifcopenshell_file_t* file, ifcopenshell_instance_t* context, const ifcopenshell_instance_list_t* curves, ifcopenshell_instance_t** out_result);
/**
 * Create a mesh representation from vertices and face indices.
 *
 * Uses IfcPolygonalFaceSet (IFC4+) or IfcFacetedBrep (IFC2X3). IFC2X3
 * always uses faceted BRep regardless of force_faceted_brep.
 *
 * @param file IFC file that receives the representation.
 * @param context IfcGeometricRepresentationContext.
 * @param options Vertices, faces, and optional faceted BRep override.
 * @return IfcShapeRepresentation entity, or a null handle on failure.
 */
bool ifcopenshell_geometry_add_mesh_representation(ifcopenshell_file_t* file, ifcopenshell_instance_t* context, const ifcopenshell_geometry_add_mesh_representation_options_t* options, ifcopenshell_instance_t** out_result);
/**
 * Create a railing representation along a path.
 *
 * @param file IFC file that receives the representation.
 * @param options Railing path, support spacing, dimensions, and terminal type.
 * @return IfcShapeRepresentation entity, or a null handle on failure.
 */
bool ifcopenshell_geometry_add_railing_representation(ifcopenshell_file_t* file, const ifcopenshell_geometry_add_railing_representation_options_t* options, ifcopenshell_instance_t** out_result);
/**
 * Create or update an IfcShapeAspect within a product definition shape.
 *
 * If an aspect with the same name already exists, its items are merged.
 * Items are removed from other aspects in the same product definition shape.
 *
 * @param file IFC file that receives the aspect.
 * @param options Aspect name, items, representation, and owning product.
 * @return IfcShapeAspect entity, or a null handle on failure.
 */
bool ifcopenshell_geometry_add_shape_aspect(ifcopenshell_file_t* file, const ifcopenshell_geometry_add_shape_aspect_options_t* options, ifcopenshell_instance_t** out_result);
/**
 * Create a slab-style swept solid representation (IfcExtrudedAreaSolid).
 *
 * @param file IFC file that receives the representation.
 * @param options Slab dimensions, direction, clippings, and boundary polyline.
 * @return IfcShapeRepresentation entity, or a null handle on failure.
 */
bool ifcopenshell_geometry_add_slab_representation(ifcopenshell_file_t* file, const ifcopenshell_geometry_add_slab_representation_options_t* options, ifcopenshell_instance_t** out_result);
/**
 * Create an IfcTopologyRepresentation for a single topology item.
 *
 * @param file IFC file that receives the representation.
 * @param options Context, topology item, and optional identifier/type.
 * @return IfcTopologyRepresentation entity, or a null handle on failure.
 */
bool ifcopenshell_geometry_add_topology_representation(ifcopenshell_file_t* file, const ifcopenshell_geometry_add_topology_representation_options_t* options, ifcopenshell_instance_t** out_result);
/**
 * Create a wall-style swept solid representation (IfcExtrudedAreaSolid).
 *
 * @param file IFC file that receives the representation.
 * @param options Wall dimensions, direction, clippings, and booleans.
 * @return IfcShapeRepresentation entity, or a null handle on failure.
 */
bool ifcopenshell_geometry_add_wall_representation(ifcopenshell_file_t* file, const ifcopenshell_geometry_add_wall_representation_options_t* options, ifcopenshell_instance_t** out_result);
/**
 * Create a window representation with lining and panel geometry.
 *
 * @param file IFC file that receives the representation.
 * @param options Window dimensions, panel schema, lining/panel properties.
 * @return IfcShapeRepresentation entity, or a null handle on failure.
 */
bool ifcopenshell_geometry_add_window_representation(ifcopenshell_file_t* file, const ifcopenshell_geometry_add_window_representation_options_t* options, ifcopenshell_instance_t** out_result);
/**
 * Assign a representation to a product or type product.
 *
 * For IfcProduct, the representation is appended to the product's
 * IfcProductDefinitionShape. For IfcTypeProduct, an IfcRepresentationMap is
 * created and mapped representations are assigned to all related occurrences.
 * If the product has a type with existing maps and the representation is not
 * already mapped, the assignment is redirected to the type product.
 *
 * @param file IFC file to modify.
 * @param product IfcProduct or IfcTypeProduct entity.
 * @param representation IfcShapeRepresentation entity.
 * @return The product (possibly re-routed to its type), or null handle on failure.
 */
bool ifcopenshell_geometry_assign_representation(ifcopenshell_file_t* file, ifcopenshell_instance_t* product, ifcopenshell_instance_t* representation, ifcopenshell_instance_t** out_result);
/**
 * Clip a solid with an infinite half-space plane (IfcBooleanClippingResult).
 *
 * Creates a DIFFERENCE boolean between the item and an IfcHalfSpaceSolid.
 * If an owning element is provided, the clipping is registered in its
 * BBIM_Boolean property set.
 *
 * @param file IFC file that receives the clipping.
 * @param options Solid, plane point, normal, and optional element/history.
 * @return IfcBooleanClippingResult entity, or a null handle on failure.
 */
bool ifcopenshell_geometry_clip_solid(ifcopenshell_file_t* file, const ifcopenshell_geometry_clip_solid_options_t* options, ifcopenshell_instance_t** out_result);
/**
 * Clip a solid with a polygonal-bounded half-space (IfcPolygonalBoundedHalfSpace).
 *
 * Creates a DIFFERENCE boolean between the item and an
 * IfcPolygonalBoundedHalfSpace. Locations are in model units.
 *
 * @param file IFC file that receives the clipping.
 * @param options Solid, plane, boundary polygon, and optional element/history.
 * @return IfcBooleanClippingResult entity, or a null handle on failure.
 */
bool ifcopenshell_geometry_clip_solid_bounded(ifcopenshell_file_t* file, const ifcopenshell_geometry_clip_solid_bounded_options_t* options, ifcopenshell_instance_t** out_result);
/**
 * Create an IfcRelConnectsElements between two elements.
 *
 * Removes any existing incompatible element connections between the two
 * elements before creating the new relationship.
 *
 * @param file IFC file that receives the relationship.
 * @param options Relating element, related element, and optional description/history.
 * @return IfcRelConnectsElements entity, or null handle on failure.
 */
bool ifcopenshell_geometry_connect_element(ifcopenshell_file_t* file, const ifcopenshell_geometry_connect_element_options_t* options, ifcopenshell_instance_t** out_result);
/**
 * Create an IfcRelConnectsPathElements between two path-based elements.
 *
 * Removes incompatible existing path connections (same terminal end or same
 * pair) before creating the new relationship. Connection types are "ATSTART",
 * "ATEND", or "ATPATH".
 *
 * @param file IFC file that receives the relationship.
 * @param options Elements, connection types, and optional description/geometry/history.
 * @return IfcRelConnectsPathElements entity, or null handle on failure.
 */
bool ifcopenshell_geometry_connect_path(ifcopenshell_file_t* file, const ifcopenshell_geometry_connect_path_options_t* options, ifcopenshell_instance_t** out_result);
/**
 * Connect two walls at their nearest endpoint.
 *
 * Computes the intersection of the second wall's axis with the first wall's
 * axis to determine ATSTART or ATEND. When is_atpath is true, the connection
 * type is ATPATH. Creates an IfcRelConnectsPathElements.
 *
 * @param file IFC file that receives the connection.
 * @param options Walls, connection mode, and optional owner history.
 * @return IfcRelConnectsPathElements entity, or a null handle on failure.
 */
bool ifcopenshell_geometry_connect_wall(ifcopenshell_file_t* file, const ifcopenshell_geometry_connect_wall_options_t* options, ifcopenshell_instance_t** out_result);
/**
 * Deep-copy a representation from one product to another.
 *
 * Copies the "Body" (or specified context) representation from the source
 * product, replaces any existing representation of the same context on the
 * target, and assigns the copy.
 *
 * @param file IFC file to modify.
 * @param options Source product, target product, and optional context identifier.
 * @return Newly created IfcShapeRepresentation, or no value when the source
 * has no representation for the requested context identifier.
 */
bool ifcopenshell_geometry_copy_representation(ifcopenshell_file_t* file, const ifcopenshell_geometry_copy_representation_options_t* options, ifcopenshell_instance_t** out_result);
/**
 * Create a wall from two XY endpoints with body representation and placement.
 *
 * Computes the wall length from the endpoint distance, creates a wall
 * representation, and sets the object placement with rotation to align the
 * wall along the baseline.
 *
 * @param file IFC file that receives the wall geometry.
 * @param options Element, context, endpoints, elevation, height, thickness, and unit flag.
 * @return IfcShapeRepresentation entity, or a null handle on failure.
 */
bool ifcopenshell_geometry_create_2pt_wall(ifcopenshell_file_t* file, const ifcopenshell_geometry_create2_pt_wall_options_t* options, ifcopenshell_instance_t** out_result);
/**
 * Remove all IfcRelConnectsElements relationships between two elements.
 *
 * Searches both ConnectedTo and ConnectedFrom inverse references on both
 * elements and removes matching relationships.
 *
 * @param file IFC file to modify.
 * @param relating_element First element.
 * @param related_element Second element.
 */
bool ifcopenshell_geometry_disconnect_element(ifcopenshell_file_t* file, ifcopenshell_instance_t* relating_element, ifcopenshell_instance_t* related_element);
/**
 * Remove path-based element connections.
 *
 * When element and connection_type are provided, removes all path connections
 * matching that type on the element. When relating_element and
 * related_element are provided, removes the specific connection between them.
 *
 * @param file IFC file to modify.
 * @param options Element, connection type, or relating/related element pair.
 */
bool ifcopenshell_geometry_disconnect_path(ifcopenshell_file_t* file, const ifcopenshell_geometry_disconnect_path_options_t* options);
/**
 * Set the ObjectPlacement of a product from a 4x4 transformation matrix.
 *
 * Creates a new IfcLocalPlacement relative to the product's parent
 * (determined from aggregation, nesting, containment, or void relationships).
 * The old placement is removed if unreferenced. When
 * should_transform_children is false, child placements are re-applied in their
 * previous world-space positions.
 *
 * @param file IFC file to modify.
 * @param options Product, matrix, SI flag, and child transform flag.
 * @return Newly created IfcLocalPlacement, or null handle on failure.
 */
bool ifcopenshell_geometry_edit_object_placement(ifcopenshell_file_t* file, const ifcopenshell_geometry_edit_object_placement_options_t* options, ifcopenshell_instance_t** out_result);
/**
 * Create a mapped representation (IfcRepresentationMap + IfcMappedItem).
 *
 * Wraps the given representation in an IfcRepresentationMap and returns a new
 * IfcShapeRepresentation of type "MappedRepresentation" containing one
 * IfcMappedItem.
 *
 * @param file IFC file that receives the mapped entities.
 * @param representation IfcShapeRepresentation to map.
 * @return New IfcShapeRepresentation with MappedRepresentation type.
 */
bool ifcopenshell_geometry_map_representation(ifcopenshell_file_t* file, ifcopenshell_instance_t* representation, ifcopenshell_instance_t** out_result);
/**
 * Return the axis-aligned 2D bounding box extents of a profile.
 *
 * Computes the X and Y extents from the profile's parameterized attributes
 * (e.g. OverallWidth/OverallDepth for I-shaped profiles). Falls back to
 * geometry evaluation via OpenCASCADE when available. Returns an empty vector
 * on failure.
 *
 * @param file IFC file containing the profile.
 * @param profile IfcProfileDef entity.
 * @return Two-element vector {x_extent, y_extent} in model units, or empty.
 */
bool ifcopenshell_geometry_profile_extents(ifcopenshell_file_t* file, ifcopenshell_instance_t* profile, ifcopenshell_double_list_t* out_result);
/**
 * Regenerate a wall's body and axis representations from its material layers.
 *
 * Walks connected walls to compute join geometry, rebuilds the profile from
 * layer axes, and replaces the existing body and axis representations.
 *
 * @param file IFC file containing the wall.
 * @param options Wall entity, length, height, and optional angle.
 * @return New IfcShapeRepresentation for the body, or a null handle on failure.
 */
bool ifcopenshell_geometry_regenerate_wall_representation(ifcopenshell_file_t* file, const ifcopenshell_geometry_regenerate_wall_representation_options_t* options, ifcopenshell_instance_t** out_result);
/**
 * Remove boolean operands from a solid representation.
 *
 * Walks the IfcBooleanResult chain for the given item, replaces references
 * to the item with its FirstOperand in parent entities, and moves the
 * SecondOperand into the owning representation's Items.
 *
 * @param file IFC file to modify.
 * @param item Solid operand whose boolean chain to remove.
 */
bool ifcopenshell_geometry_remove_boolean(ifcopenshell_file_t* file, ifcopenshell_instance_t* item);
/**
 * Remove a representation and deep-delete its unreferenced sub-entities.
 *
 * Cleans up styled items, presentation layer assignments, textures, and
 * colours. Geometric representation contexts are never deleted. Named
 * profiles are preserved when should_keep_named_profiles is true.
 *
 * @param file IFC file to modify.
 * @param representation IfcShapeRepresentation to remove.
 * @param options Whether to preserve named profiles.
 */
bool ifcopenshell_geometry_remove_representation(ifcopenshell_file_t* file, ifcopenshell_instance_t* representation, const ifcopenshell_geometry_remove_representation_options_t* options);
/**
 * Unassign a representation from a product or type product.
 *
 * For IfcProduct, removes the representation from the
 * IfcProductDefinitionShape (and cleans up the shape if empty). For
 * IfcTypeProduct, removes the matching IfcRepresentationMap and unmaps
 * occurrences. Shape aspects referencing the representation are also removed.
 *
 * @param file IFC file to modify.
 * @param product IfcProduct or IfcTypeProduct entity.
 * @param representation IfcShapeRepresentation to unassign.
 */
bool ifcopenshell_geometry_unassign_representation(ifcopenshell_file_t* file, ifcopenshell_instance_t* product, ifcopenshell_instance_t* representation);
/**
 * Validate and correct a representation's RepresentationType.
 *
 * When the representation contains multiple boolean operands, they are
 * consolidated into a single boolean chain via UNION. Returns true if the
 * final representation type is "CSG" or was successfully auto-detected.
 *
 * @param file IFC file containing the representation.
 * @param representation IfcShapeRepresentation to validate.
 * @param options Optional preferred operand for boolean consolidation.
 * @return True if the representation type is valid or was corrected.
 */
bool ifcopenshell_geometry_validate_type(ifcopenshell_file_t* file, ifcopenshell_instance_t* representation, const ifcopenshell_geometry_validate_type_options_t* options, bool* out_result);
/**
 * Add georeferencing entities to a model.
 *
 * Creates an IfcProjectedCRS and a coordinate operation (IfcMapConversion,
 * IfcMapConversionScaled, or IfcRigidOperation) linking the Model context
 * to the projected CRS. For IFC2X3, creates ePSet_MapConversion and
 * ePSet_ProjectedCRS property sets on the IfcProject instead. Does nothing
 * if georeferencing entities already exist.
 *
 * @param file File that receives the new entities.
 * @param options Coordinate operation class, CRS name, and ownership options.
 */
bool ifcopenshell_georeference_add_georeferencing(ifcopenshell_file_t* file, const ifcopenshell_georeference_add_georeferencing_options_t* options);
/**
 * Edit map conversion and projected CRS attributes.
 *
 * For IFC4+, applies attribute changes from the property bags to the first
 * IfcCoordinateOperation and/or IfcProjectedCRS in the file. For IFC2X3,
 * applies changes to the ePSet_MapConversion and ePSet_ProjectedCRS property
 * sets on the IfcProject. Entities must already exist (call
 * georeference_add_georeferencing first if needed).
 *
 * @param file File containing the georeferencing entities.
 * @param options Property bags for coordinate operation and projected CRS.
 */
bool ifcopenshell_georeference_edit_georeferencing(ifcopenshell_file_t* file, const ifcopenshell_georeference_edit_georeferencing_options_t* options);
/**
 * Set or remove the true north direction on all geometric representation contexts.
 *
 * When true_north is std::nullopt, any existing TrueNorth reference is removed
 * from every IfcGeometricRepresentationContext and the orphaned IfcDirection is
 * deleted if unreferenced. When present, the first two elements of the vector
 * are used as (X, Y) direction ratios; missing entries default to 0.0. The
 * vector is not normalized.
 *
 * @param file File whose contexts to update.
 * @param options True north direction vector or std::nullopt to remove.
 */
bool ifcopenshell_georeference_edit_true_north(ifcopenshell_file_t* file, const ifcopenshell_georeference_edit_true_north_options_t* options);
/**
 * Edit the world coordinate system (WCS) on all geometric representation contexts.
 *
 * Replaces the WorldCoordinateSystem placement on every
 * IfcGeometricRepresentationContext. The rotation parameter specifies the
 * X-axis rotation in degrees. When is_si is true (default), coordinates are
 * interpreted as SI metres and scaled to project length units. The old WCS
 * placement is deleted if unreferenced.
 *
 * @param file File whose contexts to update.
 * @param options WCS offset, rotation, and unit-system options.
 */
bool ifcopenshell_georeference_edit_wcs(ifcopenshell_file_t* file, const ifcopenshell_georeference_edit_wcs_options_t* options);
/**
 * Remove all georeferencing entities from a model.
 *
 * For IFC4+, deletes all IfcProjectedCRS and IfcCoordinateOperation entities
 * (including orphaned MapUnit). For IFC2X3, removes the ePSet_MapConversion
 * and ePSet_ProjectedCRS property sets from the IfcProject.
 *
 * @param file File from which to remove georeferencing.
 */
bool ifcopenshell_georeference_remove_georeferencing(ifcopenshell_file_t* file);
/**
 * Create or replace the AxisCurve of a grid axis as a 2D IfcPolyline.
 *
 * Points are given in world coordinates; when is_si is true they are divided
 * by the file's LENGTHUNIT scale. The points are transformed into the grid's
 * local coordinate system using the grid's ObjectPlacement. If the axis
 * already has an AxisCurve, it is deep-removed after replacement.
 *
 * @param file IFC file that receives the polyline.
 * @param p1 First endpoint (at least three coordinates; X and Y are used).
 * @param p2 Second endpoint (at least three coordinates; X and Y are used).
 * @param grid_axis IfcGridAxis whose AxisCurve to set.
 * @param is_si True if p1/p2 are in SI metres; false if already in model units.
 */
bool ifcopenshell_grid_create_axis_curve(ifcopenshell_file_t* file, const ifcopenshell_double_list_t* p1, const ifcopenshell_double_list_t* p2, ifcopenshell_instance_t* grid_axis, bool is_si);
/**
 * Create an IfcGridAxis and append it to the specified grid axis aggregate.
 *
 * @param file IFC file that receives the new axis.
 * @param grid IfcGrid entity owning the axis.
 * @param axis_tag Label for the axis (e.g. "A", "1").
 * @param same_sense True if the axis direction agrees with the curve direction.
 * @param uvw_axes Name of the grid aggregate to append to: "UAxes", "VAxes", or "WAxes".
 * @return Newly created IfcGridAxis, or a null handle on failure.
 */
bool ifcopenshell_grid_create_grid_axis(ifcopenshell_file_t* file, ifcopenshell_instance_t* grid, const char* axis_tag, bool same_sense, const char* uvw_axes, ifcopenshell_instance_t** out_result);
/**
 * Remove an IfcGridAxis and its associated AxisCurve.
 *
 * The axis entity is removed from the file and its AxisCurve (if any) is
 * deep-removed.
 *
 * @param file IFC file to modify.
 * @param axis IfcGridAxis entity to remove.
 */
bool ifcopenshell_grid_remove_grid_axis(ifcopenshell_file_t* file, ifcopenshell_instance_t* axis);
/** Create a new IfcGroup with generated GlobalId and OwnerHistory. */
bool ifcopenshell_group_add_group(ifcopenshell_file_t* file, const ifcopenshell_group_add_group_options_t* options, ifcopenshell_instance_t** out_result);
/**
 * Assign products to a group via IfcRelAssignsToGroup.
 *
 * If an existing relationship is present, new products are merged into it.
 * Products already in the group are skipped.
 */
bool ifcopenshell_group_assign_group(ifcopenshell_file_t* file, const ifcopenshell_group_assign_group_options_t* options, ifcopenshell_instance_t** out_result);
/**
 * Remove a group and its relationships.
 *
 * Removes associated IfcRelDefinesByProperties (property sets) and
 * IfcRelAssignsToGroup relationships, then deletes the group entity.
 */
bool ifcopenshell_group_remove_group(ifcopenshell_file_t* file, ifcopenshell_instance_t* group);
/**
 * Remove products from an IfcRelAssignsToGroup relationship.
 *
 * If no products remain in the relationship after removal, the relationship
 * is deleted.
 */
bool ifcopenshell_group_unassign_group(ifcopenshell_file_t* file, const ifcopenshell_group_unassign_group_options_t* options);
/**
 * Replace the product list of an existing IfcRelAssignsToGroup relationship.
 *
 * If no relationship exists, a new one is created. Child IfcGroup entities
 * from the existing relationship are preserved and merged with the new list.
 * Multiple existing relationships are consolidated into one.
 */
bool ifcopenshell_group_update_group_products(ifcopenshell_file_t* file, const ifcopenshell_group_update_group_products_options_t* options, ifcopenshell_instance_t** out_result);
bool ifcopenshell_guid_compress(const char* uuid_hex, ifcopenshell_string_t* out_result);
bool ifcopenshell_guid_expand(const char* guid, ifcopenshell_string_t* out_result);
bool ifcopenshell_guid_generate(ifcopenshell_string_t* out_result);
/**
 * Create an IfcPresentationLayerAssignment with the given name.
 *
 * @param file IFC file that receives the layer.
 * @param name Layer name.
 * @return Newly created IfcPresentationLayerAssignment.
 */
bool ifcopenshell_layer_add_layer(ifcopenshell_file_t* file, const char* name, ifcopenshell_instance_t** out_result);
/**
 * Create an IfcPresentationLayerWithStyle with visibility flags and styles.
 *
 * @param file IFC file that receives the layer.
 * @param name Layer name.
 * @param options Visibility flags and styles.
 * @return Newly created IfcPresentationLayerWithStyle.
 */
bool ifcopenshell_layer_add_layer_with_style(ifcopenshell_file_t* file, const char* name, const ifcopenshell_layer_add_layer_with_style_options_t* options, ifcopenshell_instance_t** out_result);
/**
 * Assign representation items to a presentation layer.
 *
 * Items already present in the layer's AssignedItems are not duplicated.
 *
 * @param file IFC file (unused, reserved for consistency).
 * @param items Representation items to assign.
 * @param layer IfcPresentationLayerAssignment or IfcPresentationLayerWithStyle.
 */
bool ifcopenshell_layer_assign_layer(ifcopenshell_file_t* file, const ifcopenshell_instance_list_t* items, ifcopenshell_instance_t* layer);
/**
 * Remove a presentation layer entity from the file.
 *
 * @param file IFC file to modify.
 * @param layer IfcPresentationLayerAssignment or IfcPresentationLayerWithStyle to remove.
 */
bool ifcopenshell_layer_remove_layer(ifcopenshell_file_t* file, ifcopenshell_instance_t* layer);
/**
 * Remove representation items from a presentation layer.
 *
 * If all assigned items are removed, the layer entity itself is deleted from
 * the file. Does nothing silently when any specified item is not currently
 * assigned to the layer.
 *
 * @param file IFC file to modify.
 * @param items Representation items to unassign.
 * @param layer IfcPresentationLayerAssignment or IfcPresentationLayerWithStyle.
 */
bool ifcopenshell_layer_unassign_layer(ifcopenshell_file_t* file, const ifcopenshell_instance_list_t* items, ifcopenshell_instance_t* layer);
/** Create a new IfcLibraryInformation with the given name. */
bool ifcopenshell_library_add_library(ifcopenshell_file_t* file, const char* name, ifcopenshell_instance_t** out_result);
/**
 * Create a new IfcLibraryReference linked to a library.
 *
 * In IFC4+, the reference's ReferencedLibrary attribute is set. In IFC2X3,
 * the reference is appended to the library's LibraryReference aggregate.
 */
bool ifcopenshell_library_add_reference(ifcopenshell_file_t* file, ifcopenshell_instance_t* library, ifcopenshell_instance_t** out_result);
/**
 * Assign a library reference to products via IfcRelAssociatesLibrary.
 *
 * Products already associated with the reference are skipped. If an
 * existing relationship is present, new products are merged into it.
 */
bool ifcopenshell_library_assign_reference(ifcopenshell_file_t* file, const ifcopenshell_library_assign_reference_options_t* options, ifcopenshell_instance_t** out_result);
/**
 * Remove an IfcLibraryInformation and all its references.
 *
 * Deletes all child IfcLibraryReference entities, the library entity itself,
 * and all associated IfcRelAssociatesLibrary relationships.
 */
bool ifcopenshell_library_remove_library(ifcopenshell_file_t* file, ifcopenshell_instance_t* library);
/**
 * Remove an IfcLibraryReference and its association relationships.
 *
 * Deletes the reference entity and all IfcRelAssociatesLibrary relationships
 * that reference it.
 */
bool ifcopenshell_library_remove_reference(ifcopenshell_file_t* file, ifcopenshell_instance_t* reference);
/**
 * Remove products from an IfcRelAssociatesLibrary relationship.
 *
 * If no products remain after removal, the relationship is deleted.
 */
bool ifcopenshell_library_unassign_reference(ifcopenshell_file_t* file, const ifcopenshell_library_unassign_reference_options_t* options);
/** Add an IfcMaterialConstituent to an IfcMaterialConstituentSet. */
bool ifcopenshell_material_add_constituent(ifcopenshell_file_t* file, ifcopenshell_instance_t* constituent_set, const ifcopenshell_material_add_constituent_options_t* options, ifcopenshell_instance_t** out_result);
/**
 * Add an IfcMaterialLayer to an IfcMaterialLayerSet.
 *
 * The layer thickness is initialized to 0.1 metres, converted to the file's
 * project units (e.g. 100 for a millimetre project).
 */
bool ifcopenshell_material_add_layer(ifcopenshell_file_t* file, ifcopenshell_instance_t* layer_set, const ifcopenshell_material_add_layer_options_t* options, ifcopenshell_instance_t** out_result);
/** Append an IfcMaterial to an IfcMaterialList. */
bool ifcopenshell_material_add_list_item(ifcopenshell_file_t* file, ifcopenshell_instance_t* material_list, ifcopenshell_instance_t* material);
/** Create a new IfcMaterial. */
bool ifcopenshell_material_add_material(ifcopenshell_file_t* file, const ifcopenshell_material_add_material_options_t* options, ifcopenshell_instance_t** out_result);
/**
 * Create a new material set of the requested type.
 *
 * Defaults to IfcMaterialConstituentSet if set_type is omitted.
 */
bool ifcopenshell_material_add_material_set(ifcopenshell_file_t* file, const ifcopenshell_material_add_material_set_options_t* options, ifcopenshell_instance_t** out_result);
/** Add an IfcMaterialProfile to an IfcMaterialProfileSet. */
bool ifcopenshell_material_add_profile(ifcopenshell_file_t* file, ifcopenshell_instance_t* profile_set, const ifcopenshell_material_add_profile_options_t* options, ifcopenshell_instance_t** out_result);
/**
 * Assign a material to products via IfcRelAssociatesMaterial.
 *
 * Any existing material assignment on the products is removed first.
 * For "IfcMaterialLayerSetUsage" and "IfcMaterialProfileSetUsage", the
 * products are grouped by material set and direction; each group gets its
 * own usage entity and relationship. Returns the created relationship(s).
 */
bool ifcopenshell_material_assign_material(ifcopenshell_file_t* file, const ifcopenshell_instance_list_t* products, const ifcopenshell_material_assign_material_options_t* options, ifcopenshell_parse_instance_list_t** out_result);
/**
 * Replace the profile on an IfcMaterialProfile and update representations.
 *
 * Sets the Profile attribute on the material profile. Updates the
 * SweptArea of any IfcSweptAreaSolid in representations of elements
 * using the parent IfcMaterialProfileSetUsage. The old profile is
 * deleted if it has no remaining references.
 */
bool ifcopenshell_material_assign_profile(ifcopenshell_file_t* file, ifcopenshell_instance_t* material_profile, ifcopenshell_instance_t* profile);
/**
 * Edit attributes of an IfcMaterialProfileSetUsage.
 *
 * Applies attribute key-value pairs from the props builder. If CardinalPoint
 * changes and both profile_width and profile_height are provided, the
 * ObjectPlacement of associated elements is updated to reflect the new
 * cardinal point position.
 */
bool ifcopenshell_material_edit_profile_usage(ifcopenshell_file_t* file, ifcopenshell_instance_t* usage, const ifcopenshell_material_edit_profile_usage_options_t* options);
/**
 * Remove an IfcMaterialConstituent from its set.
 *
 * If should_remove_material is true, the associated IfcMaterial is also
 * deleted.
 */
bool ifcopenshell_material_remove_constituent(ifcopenshell_file_t* file, ifcopenshell_instance_t* constituent, const ifcopenshell_material_remove_item_options_t* options);
/**
 * Remove an IfcMaterialLayer from its set.
 *
 * If should_remove_material is true, the associated IfcMaterial is also
 * deleted.
 */
bool ifcopenshell_material_remove_layer(ifcopenshell_file_t* file, ifcopenshell_instance_t* layer, const ifcopenshell_material_remove_item_options_t* options);
/** Remove an item from an IfcMaterialList by index. */
bool ifcopenshell_material_remove_list_item(ifcopenshell_file_t* file, ifcopenshell_instance_t* material_list, const ifcopenshell_material_remove_list_item_options_t* options);
/**
 * Remove an IfcMaterial and its container constituents/layers/profiles.
 *
 * Deletes the material entity. Constituent, layer, or profile entities
 * that reference it are also removed. Associated IfcRelAssociatesMaterial,
 * IfcMaterialProperties, and IfcMaterialDefinitionRepresentation entities
 * are cleaned up.
 */
bool ifcopenshell_material_remove_material(ifcopenshell_file_t* file, ifcopenshell_instance_t* material);
/**
 * Remove a material set and its items.
 *
 * Deletes all items (layers, profiles, constituents) in the set, then
 * removes the set entity. For IfcMaterialLayerSet and IfcMaterialProfileSet,
 * associated usage entities and their relationships are also deleted.
 * Associated IfcRelAssociatesMaterial and IfcMaterialProperties are
 * cleaned up.
 */
bool ifcopenshell_material_remove_material_set(ifcopenshell_file_t* file, ifcopenshell_instance_t* material);
/**
 * Remove an IfcMaterialProfile from its set.
 *
 * Optionally deletes the associated IfcMaterial and/or IfcProfileDef
 * if the corresponding flags are set.
 */
bool ifcopenshell_material_remove_profile(ifcopenshell_file_t* file, ifcopenshell_instance_t* profile, const ifcopenshell_material_remove_profile_options_t* options);
/**
 * Reorder an item within a material set.
 *
 * Moves the item from old_index to new_index within the set's aggregate
 * (MaterialConstituents, MaterialLayers, MaterialProfiles, or Materials).
 * Supports IfcMaterialConstituentSet, IfcMaterialLayerSet,
 * IfcMaterialProfileSet, and IfcMaterialList.
 */
bool ifcopenshell_material_reorder_set_item(ifcopenshell_file_t* file, ifcopenshell_instance_t* material_set, const ifcopenshell_material_reorder_set_item_options_t* options);
/**
 * Remove material assignments from products.
 *
 * Deletes IfcRelAssociatesMaterial relationships for the products. If the
 * material is an IfcMaterialLayerSetUsage or IfcMaterialProfileSetUsage
 * with no remaining products, the usage entity is also deleted.
 */
bool ifcopenshell_material_unassign_material(ifcopenshell_file_t* file, const ifcopenshell_instance_list_t* products, const ifcopenshell_material_unassign_material_options_t* options);
/**
 * Assign objects as nested children of a parent host via IfcRelNests.
 *
 * Objects previously nested under a different parent are moved. Objects
 * previously contained in a spatial structure or aggregated under a
 * different parent are removed from those relationships. If an existing
 * IfcRelNests relationship already relates to the same parent, objects
 * are merged into it while preserving insertion order.
 */
bool ifcopenshell_nest_assign_object(ifcopenshell_file_t* file, const ifcopenshell_nest_assign_object_options_t* options, ifcopenshell_instance_t** out_result);
/**
 * Remove objects from their IfcRelNests relationships.
 *
 * If no related objects remain after removal, the IfcRelNests
 * relationship itself is deleted.
 */
bool ifcopenshell_nest_unassign_object(ifcopenshell_file_t* file, const ifcopenshell_nest_unassign_object_options_t* options);
/**
 * Create an IfcActor (or subclass) entity.
 *
 * @param file File that receives the new entity.
 * @param options Actor identity and class options.
 * @return Newly created IfcActor or subclass.
 */
bool ifcopenshell_owner_add_actor(ifcopenshell_file_t* file, const ifcopenshell_owner_add_actor_options_t* options, ifcopenshell_instance_t** out_result);
/**
 * Add an IfcAddress subclass to an IfcOrganization or IfcPerson.
 *
 * Creates an entity of the given IFC class (e.g. "IfcTelecomAddress",
 * "IfcPostalAddress") with Purpose set to OFFICE and appends it to the
 * assigned object's Addresses aggregate.
 *
 * @param file File that receives the new address.
 * @param assigned_object IfcOrganization or IfcPerson to receive the address.
 * @param ifc_class IFC class name of the address to create.
 * @return Newly created IfcAddress subclass.
 */
bool ifcopenshell_owner_add_address(ifcopenshell_file_t* file, ifcopenshell_instance_t* assigned_object, const char* ifc_class, ifcopenshell_instance_t** out_result);
/**
 * Create an IfcApplication entity.
 *
 * If application_developer is omitted, a default IfcOpenShell organisation
 * with a web-page address is created automatically.
 *
 * @param file File that receives the new entity.
 * @param options Application identity options.
 * @return Newly created IfcApplication.
 */
bool ifcopenshell_owner_add_application(ifcopenshell_file_t* file, const ifcopenshell_owner_add_application_options_t* options, ifcopenshell_instance_t** out_result);
/**
 * Create an IfcOrganization entity.
 *
 * Uses the Identification attribute on IFC4+ and Id on IFC2X3.
 *
 * @param file File that receives the new entity.
 * @param identification Organisation identifier.
 * @param name Organisation name.
 * @return Newly created IfcOrganization.
 */
bool ifcopenshell_owner_add_organisation(ifcopenshell_file_t* file, const char* identification, const char* name, ifcopenshell_instance_t** out_result);
/**
 * Create an IfcPerson entity.
 *
 * Uses the Identification attribute on IFC4+ and Id on IFC2X3.
 *
 * @param file File that receives the new entity.
 * @param identification Person identifier.
 * @param family_name Family (sur)name.
 * @param given_name Given (first) name.
 * @return Newly created IfcPerson.
 */
bool ifcopenshell_owner_add_person(ifcopenshell_file_t* file, const char* identification, const char* family_name, const char* given_name, ifcopenshell_instance_t** out_result);
/**
 * Create an IfcPersonAndOrganization entity linking a person and an organisation.
 *
 * @param file File that receives the new entity.
 * @param person IfcPerson to link.
 * @param organisation IfcOrganization to link.
 * @return Newly created IfcPersonAndOrganization.
 */
bool ifcopenshell_owner_add_person_and_organisation(ifcopenshell_file_t* file, ifcopenshell_instance_t* person, ifcopenshell_instance_t* organisation, ifcopenshell_instance_t** out_result);
/**
 * Add an IfcActorRole to an IfcOrganization, IfcPerson, or IfcPersonAndOrganization.
 *
 * If the role string matches a valid IFC enum value (e.g. "ARCHITECT"), it is
 * set directly. Otherwise, Role is set to USERDEFINED and UserDefinedRole
 * receives the string. The role is appended to the assigned object's Roles aggregate.
 *
 * @param file File that receives the new role.
 * @param assigned_object Entity to receive the role.
 * @param role Role name or user-defined string.
 * @return Newly created IfcActorRole.
 */
bool ifcopenshell_owner_add_role(ifcopenshell_file_t* file, ifcopenshell_instance_t* assigned_object, const char* role, ifcopenshell_instance_t** out_result);
/**
 * Assign an actor to an object via IfcRelAssignsToActor.
 *
 * If an IfcRelAssignsToActor already exists for this actor and object, the
 * existing relationship is returned. If the actor already has a relationship,
 * the object is appended to its RelatedObjects. Otherwise a new relationship
 * is created.
 *
 * @param file File that receives the relationship.
 * @param options Actor, object, and ownership options.
 * @return The IfcRelAssignsToActor relationship.
 */
bool ifcopenshell_owner_assign_actor(ifcopenshell_file_t* file, const ifcopenshell_owner_assign_actor_options_t* options, ifcopenshell_instance_t** out_result);
/**
 * Create a new IfcOwnerHistory.
 *
 * Sets CreationDate and LastModifiedDate to the current time, State to
 * READWRITE, and ChangeAction to ADDED. Both user and application are
 * required; if either is omitted, returns a null handle.
 *
 * @param file File that receives the new entity.
 * @param options User and application for the owner history.
 * @return Newly created IfcOwnerHistory, or a null handle on error.
 */
bool ifcopenshell_owner_create_owner_history(ifcopenshell_file_t* file, const ifcopenshell_owner_create_owner_history_options_t* options, ifcopenshell_instance_t** out_result);
/**
 * Remove an IfcActor (or subclass) from the file.
 *
 * Removes the entity and its owner history.
 *
 * @param file File containing the actor.
 * @param actor IfcActor entity to remove.
 */
bool ifcopenshell_owner_remove_actor(ifcopenshell_file_t* file, ifcopenshell_instance_t* actor);
/**
 * Remove an IfcAddress subclass from the file.
 *
 * If the address is the only entry in its parent's Addresses aggregate,
 * that aggregate is cleared before the entity is removed.
 *
 * @param file File containing the address.
 * @param address IfcAddress entity to remove.
 */
bool ifcopenshell_owner_remove_address(ifcopenshell_file_t* file, ifcopenshell_instance_t* address);
/**
 * Remove an IfcApplication from the file.
 *
 * @param file File containing the application.
 * @param application IfcApplication entity to remove.
 */
bool ifcopenshell_owner_remove_application(ifcopenshell_file_t* file, ifcopenshell_instance_t* application);
/**
 * Remove an IfcOrganization from the file.
 *
 * Cascades removal to roles, addresses, IfcPersonAndOrganization entities,
 * IfcActor entities, IfcOrganizationRelationship entities, and IfcApplication
 * entities that reference this organisation as their sole member.
 *
 * @param file File containing the organisation.
 * @param organisation IfcOrganization entity to remove.
 */
bool ifcopenshell_owner_remove_organisation(ifcopenshell_file_t* file, ifcopenshell_instance_t* organisation);
/**
 * Remove an IfcPerson from the file.
 *
 * Cascades removal to roles, addresses, IfcPersonAndOrganization entities,
 * IfcActor entities, and IfcResourceLevelRelationship entities that reference
 * this person as their sole member.
 *
 * @param file File containing the person.
 * @param person IfcPerson entity to remove.
 */
bool ifcopenshell_owner_remove_person(ifcopenshell_file_t* file, ifcopenshell_instance_t* person);
/**
 * Remove an IfcPersonAndOrganization from the file.
 *
 * Cascades removal to IfcOwnerHistory, IfcActor, and
 * IfcResourceLevelRelationship entities that reference this entity as their
 * sole member.
 *
 * @param file File containing the person-and-organisation entity.
 * @param person_and_organisation IfcPersonAndOrganization entity to remove.
 */
bool ifcopenshell_owner_remove_person_and_organisation(ifcopenshell_file_t* file, ifcopenshell_instance_t* person_and_organisation);
/**
 * Remove an IfcActorRole from the file.
 *
 * If the role is the sole entry in its parent's Roles aggregate, that
 * aggregate is cleared before the entity is removed.
 *
 * @param file File containing the role.
 * @param role IfcActorRole entity to remove.
 */
bool ifcopenshell_owner_remove_role(ifcopenshell_file_t* file, ifcopenshell_instance_t* role);
/**
 * Remove an object assignment from an actor via IfcRelAssignsToActor.
 *
 * If the relationship has only one RelatedObjects entry, the entire
 * relationship is removed. Otherwise the object is removed from the
 * RelatedObjects aggregate and the OwnerHistory is updated.
 *
 * @param file File containing the relationship.
 * @param options Actor and object to unassign.
 */
bool ifcopenshell_owner_unassign_actor(ifcopenshell_file_t* file, const ifcopenshell_owner_unassign_actor_options_t* options);
/**
 * Update or create the OwnerHistory on an IfcRoot element.
 *
 * If the element already has an IfcOwnerHistory, its LastModifiedDate,
 * LastModifyingUser, and LastModifyingApplication are updated. If not,
 * a new IfcOwnerHistory is created and assigned. Requires both user and
 * application to be provided.
 *
 * @param file File containing the element.
 * @param options Element, user, and application.
 * @return The updated or newly created IfcOwnerHistory, or a null handle if the element is not an IfcRoot.
 */
bool ifcopenshell_owner_update_owner_history(ifcopenshell_file_t* file, const ifcopenshell_owner_update_owner_history_options_t* options, ifcopenshell_instance_t** out_result);
/**
 * Extract a 4x4 row-major matrix from an IfcAxis2Placement entity.
 *
 * Supports IfcAxis2Placement2D, IfcAxis2Placement3D, and IfcAxis1Placement.
 * Returns an identity matrix if the instance is null or unsupported.
 *
 * @param instance IfcAxis2Placement entity.
 * @return 16-element row-major 4x4 matrix.
 */
bool ifcopenshell_placement_get_axis2_placement(ifcopenshell_instance_t* instance, ifcopenshell_double_list_t* out_result);
/**
 * Extract a 4x4 row-major matrix from an IfcCartesianTransformationOperator3D.
 *
 * Handles uniform and non-uniform scaling. Returns an identity matrix if the
 * instance is null or not a cartesian transformation operator.
 *
 * @param instance IfcCartesianTransformationOperator3D entity.
 * @return 16-element row-major 4x4 matrix.
 */
bool ifcopenshell_placement_get_cartesian_xform_3d(ifcopenshell_instance_t* instance, ifcopenshell_double_list_t* out_result);
/**
 * Compute the cumulative 4x4 row-major world matrix of an IfcLocalPlacement.
 *
 * Walks the PlacementRelTo chain to compute the full transformation.
 * Returns an identity matrix if the instance is nullopt.
 *
 * @param instance IfcLocalPlacement entity, or nullopt for identity.
 * @return 16-element row-major 4x4 matrix.
 */
bool ifcopenshell_placement_get_local_placement(ifcopenshell_instance_t* instance, ifcopenshell_double_list_t* out_result);
/**
 * Compute the combined 4x4 row-major matrix for an IfcMappedItem.
 *
 * Multiplies the MappingTarget transformation by the MappingOrigin placement.
 * Returns an identity matrix if the instance is null or not an IfcMappedItem.
 *
 * @param instance IfcMappedItem entity.
 * @return 16-element row-major 4x4 matrix.
 */
bool ifcopenshell_placement_get_mappeditem_xform(ifcopenshell_instance_t* instance, ifcopenshell_double_list_t* out_result);
/**
 * Return the elevation of a building storey in model units.
 *
 * Uses the Z-translation of the storey's ObjectPlacement when available,
 * falling back to the Elevation attribute. Returns 0.0 if the instance is
 * null or has no placement.
 *
 * @param instance IfcBuildingStorey entity.
 * @return Elevation in model units.
 */
bool ifcopenshell_placement_get_storey_elevation(ifcopenshell_instance_t* instance, double* out_result);
/**
 * Compute a 4x4 row-major transformation matrix from origin and axes.
 *
 * @param origin XYZ origin of the placement.
 * @param z_axis Direction ratios for the Z axis.
 * @param x_axis Direction ratios for the X axis (Y is derived).
 * @return 16-element row-major 4x4 matrix.
 */
bool ifcopenshell_placement_matrix_from_axes(const ifcopenshell_double_list_t* origin, const ifcopenshell_double_list_t* z_axis, const ifcopenshell_double_list_t* x_axis, ifcopenshell_double_list_t* out_result);
/**
 * Build a 4x4 row-major rotation matrix about a principal axis.
 *
 * @param angle_rad Rotation angle in radians.
 * @param axis Rotation axis: "X", "Y", or "Z" (case-insensitive).
 * @return 16-element row-major 4x4 rotation matrix.
 */
bool ifcopenshell_placement_rotation(double angle_rad, const char* axis, ifcopenshell_double_list_t* out_result);
/**
 * Create an IfcArbitraryClosedProfileDef from an ordered polyline.
 *
 * Points are converted from SI metres to project units before creating the
 * outer curve. The curve representation is chosen automatically (IfcPolyline
 * for IFC2X3, IfcIndexedPolyCurve otherwise).
 *
 * @param file IFC file that receives the profile.
 * @param options Outer curve points and optional name.
 * @return Newly created IfcArbitraryClosedProfileDef.
 */
bool ifcopenshell_profile_add_arbitrary_profile(ifcopenshell_file_t* file, const ifcopenshell_profile_add_arbitrary_profile_options_t* options, ifcopenshell_instance_t** out_result);
/**
 * Create an IfcArbitraryProfileDefWithVoids from an outer curve and inner voids.
 *
 * Points are converted from SI metres to project units. The outer curve uses
 * IfcCartesianPointList3D for IFC4+ schemas and IfcPolyline for IFC2X3.
 *
 * @param file IFC file that receives the profile.
 * @param options Outer curve, inner void curves, and optional name.
 * @return Newly created IfcArbitraryProfileDefWithVoids.
 */
bool ifcopenshell_profile_add_arbitrary_profile_with_voids(ifcopenshell_file_t* file, const ifcopenshell_profile_add_arbitrary_profile_with_voids_options_t* options, ifcopenshell_instance_t** out_result);
/**
 * Create a parameterized IFC profile definition.
 *
 * @param file IFC file that receives the profile.
 * @param ifc_class IFC entity class name (e.g. "IfcRectangleProfileDef").
 * @param profile_type Profile type string, typically "AREA".
 * @return Newly created IfcProfileDef entity.
 */
bool ifcopenshell_profile_add_parameterized_profile(ifcopenshell_file_t* file, const char* ifc_class, const char* profile_type, ifcopenshell_instance_t** out_result);
/**
 * Deep-copy a profile and its associated IfcProfileProperties.
 *
 * @param file IFC file that receives the copied profile.
 * @param profile IfcProfileDef entity to copy.
 * @return Newly created deep copy of the profile.
 */
bool ifcopenshell_profile_copy_profile(ifcopenshell_file_t* file, ifcopenshell_instance_t* profile, ifcopenshell_instance_t** out_result);
/**
 * Edit attributes of an existing profile definition.
 *
 * @param profile IfcProfileDef entity to modify.
 * @param attributes Property container with attribute name-value pairs.
 */
bool ifcopenshell_profile_edit_profile(ifcopenshell_instance_t* profile, void* attributes);
/**
 * Remove a profile definition and its directly referenced sub-entities.
 *
 * Removes associated IfcProfileProperties first, then removes the profile
 * entity and deep-removes all entities reachable through its direct
 * attributes (e.g. curves, placement entities).
 *
 * @param file IFC file to modify.
 * @param profile IfcProfileDef entity to remove.
 */
bool ifcopenshell_profile_remove_profile(ifcopenshell_file_t* file, ifcopenshell_instance_t* profile);
/**
 * Declare objects to a project or project library context.
 *
 * Creates or updates an IfcRelDeclares relationship linking the given
 * definitions to the relating context. If the definitions are already
 * declared to a different context, they are moved.
 */
bool ifcopenshell_project_assign_declaration(ifcopenshell_file_t* file, const ifcopenshell_project_assign_declaration_options_t* options, ifcopenshell_instance_t** out_result);
/**
 * Remove declarations of objects from a project or project library context.
 *
 * Removes the definitions from any existing IfcRelDeclares relationship.
 * If no definitions remain, the relationship itself is removed.
 */
bool ifcopenshell_project_unassign_declaration(ifcopenshell_file_t* file, const ifcopenshell_project_unassign_declaration_options_t* options);
/**
 * Add or return an existing property set attached to an object, context, type,
 * material, or profile definition.
 *
 * For IfcObject/IfcContext, creates IfcRelDefinesByProperties. For
 * IfcTypeObject, appends to HasPropertySets. For IfcMaterial, creates
 * IfcMaterialProperties (or IfcExtendedMaterialProperties in IFC2X3).
 * For IfcProfileDef, creates IfcProfileProperties (or
 * IfcGeneralProfileProperties in IFC2X3). If a property set with the
 * same name already exists on the product, it is returned without
 * creating a new one.
 */
bool ifcopenshell_pset_add_pset(ifcopenshell_file_t* file, const ifcopenshell_pset_add_pset_options_t* options, ifcopenshell_instance_t** out_result);
/**
 * Add or return an existing quantity set (IfcElementQuantity) on an object.
 *
 * If the name ends with "BaseQuantities", MethodOfMeasurement is set to
 * "BaseQuantities". If a quantity set with the same name already exists,
 * it is returned without creating a new one.
 */
bool ifcopenshell_pset_add_qto(ifcopenshell_file_t* file, const ifcopenshell_pset_add_qto_options_t* options, ifcopenshell_instance_t** out_result);
/**
 * Assign a property set to products.
 *
 * For IfcTypeProduct instances, the pset is appended to HasPropertySets.
 * For other products, creates or updates an IfcRelDefinesByProperties
 * relationship.
 */
bool ifcopenshell_pset_assign_pset(ifcopenshell_file_t* file, const ifcopenshell_pset_assign_pset_options_t* options, ifcopenshell_instance_t** out_result);
/**
 * Edit properties on an existing property set.
 *
 * Updates existing properties in-place (when not shared with other psets),
 * adds new properties for keys not yet present, and optionally removes
 * null-valued properties. Uses the pset template for type inference when
 * available. Returns true on success, false on error.
 */
bool ifcopenshell_pset_edit_pset(ifcopenshell_file_t* file, const ifcopenshell_pset_edit_pset_options_t* options, bool* out_result);
/**
 * Edit quantities on an existing quantity set.
 *
 * Updates existing quantities in-place, adds new quantities for keys not
 * yet present. Quantity types (Length, Area, Volume, Count, Weight, Time)
 * are inferred from the value type and name. Returns true on success,
 * false on error.
 */
bool ifcopenshell_pset_edit_qto(ifcopenshell_file_t* file, const ifcopenshell_pset_edit_qto_options_t* options, bool* out_result);
/** Free a property builder allocated by pset_props_new. */
bool ifcopenshell_pset_props_free(void* props);
/** Allocate a new property builder. Free with pset_props_free when done. */
bool ifcopenshell_pset_props_new(void** out_result);
/** Set a boolean property value. */
bool ifcopenshell_pset_props_set_bool(void* props, const char* key, bool value);
/** Set a date property value (IfcCalendarDate / IfcDate). */
bool ifcopenshell_pset_props_set_date(void* props, const char* key, int32_t year, int32_t month, int32_t day);
/** Set a date-time property value (IfcLocalTime / IfcDateTime). */
bool ifcopenshell_pset_props_set_datetime(void* props, const char* key, int32_t year, int32_t month, int32_t day, int32_t hour, int32_t minute, int32_t second, int32_t microsecond, bool has_timezone, int32_t timezone_offset_minutes);
/**
 * Set a nested dict property. Ownership of inner is transferred to outer;
 * do not free inner separately. Used for IfcPhysicalComplexQuantity in qtos.
 */
bool ifcopenshell_pset_props_set_dict(void* outer, const char* key, void* inner);
/** Set a double property value. */
bool ifcopenshell_pset_props_set_double(void* props, const char* key, double value);
/** Set a list-of-doubles property value (creates IfcPropertyListValue). */
bool ifcopenshell_pset_props_set_double_list(void* props, const char* key, const ifcopenshell_double_list_t* values);
/** Set a duration property value (IfcDuration). */
bool ifcopenshell_pset_props_set_duration(void* props, const char* key, bool negative, int32_t years, int32_t months, int32_t days, int32_t hours, int32_t minutes, int32_t seconds, int32_t microseconds);
/**
 * Set a property to an existing entity instance (e.g. a typed value or
 * an IfcProperty). Pass std::nullopt to clear.
 */
bool ifcopenshell_pset_props_set_instance(void* props, const char* key, ifcopenshell_instance_t* value);
/** Set a list-of-instances property value (creates IfcPropertyListValue). */
bool ifcopenshell_pset_props_set_instance_list(void* props, const char* key, const ifcopenshell_instance_list_t* values);
/** Set an integer property value. */
bool ifcopenshell_pset_props_set_int(void* props, const char* key, int64_t value);
/** Set a list-of-integers property value (creates IfcPropertyListValue). */
bool ifcopenshell_pset_props_set_int_list(void* props, const char* key, const ifcopenshell_int64_list_t* values);
/**
 * Set a property to null (blank). When editing, the property is removed if
 * should_purge is true; otherwise its NominalValue is set to blank.
 */
bool ifcopenshell_pset_props_set_null(void* props, const char* key);
/** Set a string property value. The IFC type is inferred as IfcLabel. */
bool ifcopenshell_pset_props_set_string(void* props, const char* key, const char* value);
/** Set a list-of-strings property value (creates IfcPropertyListValue). */
bool ifcopenshell_pset_props_set_string_list(void* props, const char* key, const ifcopenshell_string_list_t* values);
/** Set a boolean property with an explicit IFC type (e.g. "IfcBoolean"). */
bool ifcopenshell_pset_props_set_typed_bool(void* props, const char* key, bool value, const char* ifc_type);
/** Set a double property with an explicit IFC type (e.g. "IfcLengthMeasure"). */
bool ifcopenshell_pset_props_set_typed_double(void* props, const char* key, double value, const char* ifc_type);
/** Set an integer property with an explicit IFC type (e.g. "IfcInteger"). */
bool ifcopenshell_pset_props_set_typed_int(void* props, const char* key, int64_t value, const char* ifc_type);
/** Set a string property with an explicit IFC type (e.g. "IfcURIReference"). */
bool ifcopenshell_pset_props_set_typed_string(void* props, const char* key, const char* value, const char* ifc_type);
/**
 * Attach a unit to the most recently added property entry.
 *
 * Sets the Unit attribute on the resulting IfcPropertySingleValue. Pass
 * std::nullopt to clear.
 */
bool ifcopenshell_pset_props_set_unit_for_last(void* props, ifcopenshell_instance_t* unit);
/**
 * Remove a property set from a specific product.
 *
 * Removes the IfcRelDefinesByProperties relationship between the product
 * and pset. If the pset has no remaining associations, the pset and its
 * properties are deleted.
 */
bool ifcopenshell_pset_remove_pset(ifcopenshell_file_t* file, ifcopenshell_instance_t* product, ifcopenshell_instance_t* pset);
/**
 * Create a new IfcSimplePropertyTemplate and add it to a property set template.
 *
 * Property templates are kept sorted by name within the parent template.
 * If template_type is omitted, defaults to "P_SINGLEVALUE" for psets or
 * "Q_LENGTH" for qtos. If primary_measure_type is omitted, defaults to
 * "IfcLabel" for psets.
 */
bool ifcopenshell_pset_template_add_prop_template(ifcopenshell_file_t* file, ifcopenshell_instance_t* pset_template, const char* name, const char* description, const char* template_type, const char* primary_measure_type, ifcopenshell_instance_t** out_result);
/**
 * Create a new IfcPropertySetTemplate in the file.
 *
 * @param file The IFC file.
 * @param name Template name (e.g. "Pset_WallCommon").
 * @param template_type Template type string (e.g. "PSET_TYPEDRIVENOVERRIDE", "QTO").
 * @param applicable_entity Applicable IFC entity name (e.g. "IfcWall").
 */
bool ifcopenshell_pset_template_add_pset_template(ifcopenshell_file_t* file, const char* name, const char* template_type, const char* applicable_entity, ifcopenshell_instance_t** out_result);
/**
 * Create a template handle from custom IFC template files.
 *
 * Loads IfcPropertySetTemplate and IfcSimplePropertyTemplate entities from
 * the provided files. The caller owns the returned handle and must free it
 * with pset_template_free.
 */
bool ifcopenshell_pset_template_create_from_files(const char* schema_identifier, const ifcopenshell_file_list_t* template_files, ifcopenshell_pset_template_handle_t** out_result);
/**
 * Return property set templates applicable to an IFC class and predefined type.
 *
 * Filters by pset_only (PSET templates) or qto_only (QTO templates).
 * If neither flag is set, returns both types. Pass nullptr for
 * predefined_type or schema_name to use defaults.
 */
bool ifcopenshell_pset_template_get_applicable(ifcopenshell_pset_template_handle_t* pqt, const char* ifc_class, const char* predefined_type, bool pset_only, bool qto_only, const char* schema_name, ifcopenshell_parse_instance_list_t** out_result);
/**
 * Return names of property set templates applicable to an IFC class.
 *
 * Same filtering as pset_template_get_applicable but returns only the
 * Name strings.
 */
bool ifcopenshell_pset_template_get_applicable_names(ifcopenshell_pset_template_handle_t* pqt, const char* ifc_class, const char* predefined_type, bool pset_only, bool qto_only, const char* schema_name, ifcopenshell_string_list_t* out_result);
/**
 * Look up a property set template by name.
 *
 * Returns the IfcPropertySetTemplate entity with the given name, or a
 * null handle if not found.
 */
bool ifcopenshell_pset_template_get_by_name(ifcopenshell_pset_template_handle_t* pqt, const char* name, ifcopenshell_instance_t** out_result);
/**
 * Return a cached template handle for the given schema (e.g. "IFC4", "IFC2X3").
 *
 * Loads and caches the built-in templates on first call. Returns nullptr
 * if the schema is unknown or templates are not available.
 */
bool ifcopenshell_pset_template_get_template(const char* schema_identifier, ifcopenshell_pset_template_handle_t** out_result);
/** Return true if a property set template with the given name exists. */
bool ifcopenshell_pset_template_is_templated(ifcopenshell_pset_template_handle_t* pqt, const char* name, bool* out_result);
/**
 * Return the template type category for a property set template.
 *
 * Returns "PSET" for property set templates, "QTO" for quantity templates,
 * or an empty string if unknown.
 */
bool ifcopenshell_pset_template_pset_type(ifcopenshell_instance_t* pset_template, ifcopenshell_string_t* out_result);
/**
 * Remove a property template from its parent set template.
 *
 * Removes the IfcSimplePropertyTemplate from its parent's
 * HasPropertyTemplates aggregate, then deletes the template entity.
 */
bool ifcopenshell_pset_template_remove_prop_template(ifcopenshell_file_t* file, ifcopenshell_instance_t* prop_template);
/**
 * Remove a property set template via deep removal.
 *
 * Deletes the IfcPropertySetTemplate and all its child
 * IfcSimplePropertyTemplate entities.
 */
bool ifcopenshell_pset_template_remove_pset_template(ifcopenshell_instance_t* pset_template);
/**
 * Set the directory used to locate built-in pset/qto template files.
 *
 * Must be called before pset_template_get_template to override the default
 * search path for IFC property set template files.
 */
bool ifcopenshell_pset_template_set_template_dir(const char* dir);
/**
 * Remove products from a property set's associations.
 *
 * For IfcTypeProduct instances, removes the pset from HasPropertySets.
 * For other products, removes them from IfcRelDefinesByProperties
 * relationships. Orphaned relationships are deleted. The pset itself is
 * not deleted.
 */
bool ifcopenshell_pset_unassign_pset(ifcopenshell_file_t* file, const ifcopenshell_instance_list_t* products, ifcopenshell_instance_t* pset);
/**
 * Unshare a property set by creating independent copies for specified products.
 *
 * When the selected products are the complete set of products the pset is
 * assigned to, the first product keeps the original and the rest receive
 * copies. When the selection is a subset, every selected product receives a
 * copy and the original remains assigned to the unselected products.
 * Returns the list of newly created pset copies.
 */
bool ifcopenshell_pset_unshare_pset(ifcopenshell_file_t* file, const ifcopenshell_pset_unshare_pset_options_t* options, ifcopenshell_parse_instance_list_t** out_result);
/**
 * Register a scratch file for a given schema.
 *
 * Registers a temporary IFC file for the specified schema name,
 * used internally for schema-aware operations.
 *
 * @param schema_name The IFC schema identifier (e.g. "IFC4").
 * @param file The IFC file to register.
 * @return True if registration succeeded.
 */
bool ifcopenshell_register_scratch_file(const char* schema_name, ifcopenshell_file_t* file, bool* out_result);
/**
 * Return a geometric representation context matching the given criteria.
 *
 * Searches for IfcGeometricRepresentationSubContext when subcontext or
 * target_view is provided, otherwise searches for
 * IfcGeometricRepresentationContext (excluding subcontexts).
 *
 * @param file The IFC file to search.
 * @param context_type Context type filter (e.g. "Model", "Plan").
 * @param subcontext Context identifier filter (e.g. "Body", "Axis").
 * @param target_view Target view filter (e.g. "MODEL_VIEW", "GRAPH_VIEW").
 * @return The first matching context, or empty if none found.
 */
bool ifcopenshell_representation_get_context(ifcopenshell_file_t* file, const char* context_type, const char* subcontext, const char* target_view, ifcopenshell_instance_t** out_result);
/**
 * Return all geometric representation contexts sorted by priority.
 *
 * Sorts by ContextType (Model > Plan > Annotation), then by
 * ContextIdentifier (Body > Body-FallBack > ...), then by
 * TargetView (MODEL_VIEW > PLAN_VIEW > ...), then by TargetScale.
 *
 * @param file The IFC file to search.
 * @return Ordered list of IfcGeometricRepresentationContext entities.
 */
bool ifcopenshell_representation_get_prioritised_contexts(ifcopenshell_file_t* file, ifcopenshell_parse_instance_list_t** out_result);
/**
 * Return a product's representation filtered by context.
 *
 * For an IfcProduct, reads Representation/Representations. For an
 * IfcTypeProduct, reads RepresentationMaps/MappedRepresentation.
 * Returns the first representation matching the context filters.
 *
 * @param element The IfcProduct or IfcTypeProduct.
 * @param options Context filtering options.
 * @return The matching IfcShapeRepresentation, or empty if none found.
 */
bool ifcopenshell_representation_get_product_representation(ifcopenshell_instance_t* element, const ifcopenshell_representation_get_product_representation_options_t* options, ifcopenshell_instance_t** out_result);
/**
 * Resolve a representation by unwrapping single mapped items.
 *
 * If a representation contains a single IfcMappedItem whose
 * MappingSource points to another representation, this function
 * follows the chain and returns the innermost representation.
 * This handles Tekla-style representation indirection.
 *
 * @param representation The IfcShapeRepresentation to resolve.
 * @return The resolved representation, or the original if no unwrapping was needed.
 */
bool ifcopenshell_representation_resolve(ifcopenshell_instance_t* representation, ifcopenshell_instance_t** out_result);
/**
 * Return the base items of a representation, unwrapping mapped items and boolean operands.
 *
 * Recursively follows IfcMappedItem sources and IfcBooleanResult
 * operands to collect leaf-level representation items. Guards against
 * infinite recursion (depth limit of 64, iteration limit of 100000).
 *
 * @param representation The IfcShapeRepresentation to resolve.
 * @return List of leaf-level IfcRepresentationItem entities.
 */
bool ifcopenshell_representation_resolve_base_items(ifcopenshell_instance_t* representation, ifcopenshell_parse_instance_list_t** out_result);
/**
 * Edit attributes of an IfcResourceTime entity.
 *
 * Applies attribute changes from the property bag to the resource time.
 * Attributes that are hard-constrained (ConstraintGrade HARD and Benchmark
 * EQUALTO) on the owning IfcResource are skipped. If ScheduleUsage is
 * changed and a ScheduleWork constraint exists, the associated IfcTask
 * duration is recalculated. Duration values use ISO 8601 format
 * (e.g. "PT8H", "P5D"). Date-time values use ISO 8601 extended format
 * (e.g. "2024-01-15T09:00:00").
 *
 * @param file File containing the resource time.
 * @param resource_time IfcResourceTime entity to edit.
 * @param attributes Property bag of attribute name/value pairs.
 */
bool ifcopenshell_resource_edit_resource_time(ifcopenshell_file_t* file, ifcopenshell_instance_t* resource_time, void* attributes);
/**
 * Create an IFC entity with generated identity, optional name, and optional
 * predefined type.
 *
 * Sets GlobalId (for IfcRoot-derived entities). OwnerHistory is assigned only
 * when the owner_history option contains a handle; it is not created
 * automatically. Schema-specific defaults are applied for spatial elements,
 * element types, and door/window styles. If the predefined type is not a valid
 * enum value, it is stored as USERDEFINED with the value in ObjectType
 * (or ElementType/ProcessType where applicable).
 */
bool ifcopenshell_root_create_entity(ifcopenshell_file_t* file, const ifcopenshell_root_create_entity_options_t* options, ifcopenshell_instance_t** out_result);
/**
 * Remove a product and all its relationships.
 *
 * Performs a deep removal that cleans up: representations, object placements,
 * opening elements, property sets, material assignments, type definitions,
 * space boundaries, nesting relationships, aggregate relationships, spatial
 * containment, element connections, port connections, group memberships,
 * and grid axes. The product entity itself is deleted last.
 */
bool ifcopenshell_root_remove_product(ifcopenshell_file_t* file, ifcopenshell_instance_t* product, const ifcopenshell_root_remove_product_options_t* options);
/**
 * Change the IFC class of an entity.
 *
 * Removes the element and recreates it as the new class with the same
 * STEP id. Compatible attributes are preserved; incompatible attributes
 * are dropped. Inverse references to the old element are updated to
 * point to the new element.
 *
 * If the element is already of the requested class, returns it unchanged.
 *
 * @param file The IFC file. If empty, uses the element's file.
 * @param element The entity to reassign.
 * @param new_class The target IFC class name (e.g. "IfcWall").
 * @return The new entity of the requested class, or empty on failure.
 */
bool ifcopenshell_schema_reassign_class(ifcopenshell_file_t* file, ifcopenshell_instance_t* element, const char* new_class, ifcopenshell_instance_t** out_result);
/**
 * Filter all elements in a file using a selector query.
 *
 * Parses the query as a filter expression and returns all matching
 * elements as a list value.
 *
 * @param file The IFC file to search.
 * @param query The filter query string.
 * @return List value of matching elements, or empty on error. Free with value_free.
 */
bool ifcopenshell_selector_filter_all(ifcopenshell_file_t* file, const char* query, ifcopenshell_value_t** out_result);
/**
 * Filter a list of elements using a selector query.
 *
 * Parses the query as a filter expression and returns the subset
 * of elements that match.
 *
 * @param file The IFC file context.
 * @param query The filter query string.
 * @param elements The elements to filter.
 * @return List value of matching elements, or empty on error. Free with value_free.
 */
bool ifcopenshell_selector_filter_elements(ifcopenshell_file_t* file, const char* query, const ifcopenshell_instance_list_t* elements, ifcopenshell_value_t** out_result);
/**
 * Format a selector query against an element.
 *
 * Evaluates a format expression (e.g. attribute names, arithmetic,
 * string functions) against an element and returns the formatted string.
 *
 * @param file Optional IFC file context.
 * @param instance The element to format against.
 * @param query The format expression string.
 * @return The formatted result, or empty on error.
 */
bool ifcopenshell_selector_format(ifcopenshell_file_t* file, ifcopenshell_instance_t* instance, const char* query, ifcopenshell_string_t* out_result);
/**
 * Get a value from an element using a selector query.
 *
 * Parses the query as a get-element key path and extracts the
 * corresponding value from the element's attributes.
 *
 * @param file Optional IFC file context.
 * @param element The element to query.
 * @param query The selector key path (e.g. "Name", "Pset_WallCommon.FireRating").
 * @return The extracted value, or empty on error. Free with value_free.
 */
bool ifcopenshell_selector_get_element_value(ifcopenshell_file_t* file, ifcopenshell_instance_t* element, const char* query, ifcopenshell_value_t** out_result);
/**
 * Return the number of keys in a parsed key list.
 *
 * @param keys Opaque key list handle from selector_parse_keys.
 * @return Number of keys, or 0 if keys is null.
 */
bool ifcopenshell_selector_keys_count(void* keys, size_t* out_result);
/**
 * Free a parsed key list.
 *
 * @param keys Opaque key list handle from selector_parse_keys.
 */
bool ifcopenshell_selector_keys_free(void* keys);
/**
 * Return the text of a key at the given index.
 *
 * @param keys Opaque key list handle.
 * @param index Zero-based key index.
 * @return Key text, or empty string if out of range.
 */
bool ifcopenshell_selector_keys_get(void* keys, size_t index, ifcopenshell_string_t* out_result);
/**
 * Check whether a key at the given index is a regular expression.
 *
 * @param keys Opaque key list handle.
 * @param index Zero-based key index.
 * @return True if the key is a regex pattern.
 */
bool ifcopenshell_selector_keys_is_regex(void* keys, size_t index, bool* out_result);
/**
 * Return a child node at the given index.
 *
 * @param node The parent selector node.
 * @param index Zero-based child index.
 * @return The child node, or null if out of range.
 */
bool ifcopenshell_selector_node_child(void* node, size_t index, void** out_result);
/**
 * Return the number of child nodes.
 *
 * @param node The selector node.
 * @return Number of children, or 0 if node is null.
 */
bool ifcopenshell_selector_node_child_count(void* node, size_t* out_result);
/**
 * Free a selector AST and all its children.
 *
 * @param root Root node of the AST from selector_parse_filter,
 * selector_parse_get_element, or selector_parse_format.
 */
bool ifcopenshell_selector_node_free(void* root);
/**
 * Return the kind of a selector node.
 *
 * Values below IFCSEL_TOKEN_FIRST (100) are grammar rule nodes;
 * values at or above IFCSEL_TOKEN_FIRST are token nodes.
 *
 * @param node The selector node.
 * @return Node kind as an ifcsel_node_kind value, or 0 if node is null.
 */
bool ifcopenshell_selector_node_kind(void* node, int32_t* out_result);
/**
 * Return the text content of a token node.
 *
 * For token nodes (kind >= IFCSEL_TOKEN_FIRST), returns the matched
 * text. For rule nodes, returns empty.
 *
 * @param node The selector node.
 * @return Node text, or empty string if not a token.
 */
bool ifcopenshell_selector_node_text(void* node, ifcopenshell_string_t* out_result);
/**
 * Parse a filter query into an AST.
 *
 * Parses a filter expression (e.g. "IfcWall, Pset_WallCommon.FireRating=*2h*")
 * into a tree of selector nodes.
 *
 * @param query The filter query string.
 * @return Root node of the AST. Free with selector_node_free.
 */
bool ifcopenshell_selector_parse_filter(const char* query, void** out_result);
/**
 * Parse a format query into an AST.
 *
 * Parses a format expression (e.g. "Name + ' - ' + GlobalId") into
 * a tree of selector nodes.
 *
 * @param query The format query string.
 * @return Root node of the AST. Free with selector_node_free.
 */
bool ifcopenshell_selector_parse_format(const char* query, void** out_result);
/**
 * Parse a get-element query into an AST.
 *
 * Parses a key path expression (e.g. "IfcWall/Name") into a tree of
 * selector nodes.
 *
 * @param query The get-element query string.
 * @return Root node of the AST. Free with selector_node_free.
 */
bool ifcopenshell_selector_parse_get_element(const char* query, void** out_result);
/**
 * Parse a selector query into a list of key entries.
 *
 * Extracts the key path from a get-element query (e.g.
 * "IfcWall/Name" yields keys ["IfcWall", "Name"]).
 *
 * @param query The selector query string.
 * @return Opaque key list handle. Free with selector_keys_free.
 */
bool ifcopenshell_selector_parse_keys(const char* query, void** out_result);
/**
 * Set a value on an element using a selector query.
 *
 * Parses the query to identify the target attribute (e.g. a property
 * set property or direct attribute) and sets it to the provided value.
 *
 * @param file The IFC file context.
 * @param element The element to modify.
 * @param query The selector key path identifying the target.
 * @param value The value to set. If empty, unsets the target.
 * @param concat If non-null and non-empty, concatenated with the value as a prefix.
 */
bool ifcopenshell_selector_set_element_value(ifcopenshell_file_t* file, ifcopenshell_instance_t* element, const char* query, ifcopenshell_value_t* value, const char* concat);
/**
 * Parse an ISO 8601 date-time string and return a schema-appropriate representation.
 *
 * For IFC4+, returns the string as-is. For IFC2X3, returns an IfcDateAndTime
 * entity constructed from the parsed components.
 *
 * @param file File to determine the schema version.
 * @param date_time ISO 8601 date-time string (e.g. "2024-01-15T09:00:00").
 * @return An IfcDateAndTime entity (IFC2X3) or the original string (IFC4+).
 */
bool ifcopenshell_sequence_add_date_time(ifcopenshell_file_t* file, const char* date_time, ifcopenshell_instance_string_variant_t* out_result);
/**
 * Create an IfcTask entity.
 *
 * When both work_schedule and parent_task are provided, work_schedule takes
 * precedence and the task is assigned to it via IfcRelAssignsToControl.
 * When only parent_task is provided, the task is nested under it via
 * IfcRelNests and its Identification is auto-generated as "parent.N" on IFC4+.
 *
 * @param file File that receives the new entity.
 * @param options Task placement and attribute options.
 * @return Newly created IfcTask.
 */
bool ifcopenshell_sequence_add_task(ifcopenshell_file_t* file, const ifcopenshell_sequence_add_task_options_t* options, ifcopenshell_instance_t** out_result);
/**
 * Create an IfcTaskTime (or IfcTaskTimeRecurring) and assign it to a task.
 *
 * Replaces any existing TaskTime on the task.
 *
 * @param file File that receives the new entity.
 * @param task IfcTask to receive the task time.
 * @param options Whether to create a recurring task time.
 * @return Newly created IfcTaskTime or IfcTaskTimeRecurring.
 */
bool ifcopenshell_sequence_add_task_time(ifcopenshell_file_t* file, ifcopenshell_instance_t* task, const ifcopenshell_sequence_add_task_time_options_t* options, ifcopenshell_instance_t** out_result);
/**
 * Create an IfcTimePeriod and add it to a recurrence pattern.
 *
 * @param file File that receives the new entity.
 * @param recurrence_pattern IfcRecurrencePattern to receive the time period.
 * @param options Start and end time strings.
 * @return Newly created IfcTimePeriod.
 */
bool ifcopenshell_sequence_add_time_period(ifcopenshell_file_t* file, ifcopenshell_instance_t* recurrence_pattern, const ifcopenshell_sequence_add_time_period_options_t* options, ifcopenshell_instance_t** out_result);
/**
 * Create an IfcWorkCalendar and declare it on the project.
 *
 * @param file File that receives the new entity.
 * @param options Calendar name, predefined type, and ownership options.
 * @return Newly created IfcWorkCalendar.
 */
bool ifcopenshell_sequence_add_work_calendar(ifcopenshell_file_t* file, const ifcopenshell_sequence_add_work_calendar_options_t* options, ifcopenshell_instance_t** out_result);
/**
 * Create an IfcWorkPlan and declare it on the project.
 *
 * @param file File that receives the new entity.
 * @param options Work plan name, dates, creator, and ownership options.
 * @return Newly created IfcWorkPlan.
 */
bool ifcopenshell_sequence_add_work_plan(ifcopenshell_file_t* file, const ifcopenshell_sequence_add_work_plan_options_t* options, ifcopenshell_instance_t** out_result);
/**
 * Create an IfcWorkSchedule.
 *
 * When work_plan is provided, the schedule is nested under it via
 * IfcRelAggregates. Otherwise it is declared directly on the project
 * context (IFC4+).
 *
 * @param file File that receives the new entity.
 * @param options Schedule name, type, dates, work plan, and ownership options.
 * @return Newly created IfcWorkSchedule.
 */
bool ifcopenshell_sequence_add_work_schedule(ifcopenshell_file_t* file, const ifcopenshell_sequence_add_work_schedule_options_t* options, ifcopenshell_instance_t** out_result);
/**
 * Create an IfcWorkTime and add it to a work calendar.
 *
 * @param file File that receives the new entity.
 * @param work_calendar IfcWorkCalendar to receive the work time.
 * @param time_type "WorkingTimes" or "ExceptionTimes".
 * @return Newly created IfcWorkTime.
 */
bool ifcopenshell_sequence_add_work_time(ifcopenshell_file_t* file, ifcopenshell_instance_t* work_calendar, const char* time_type, ifcopenshell_instance_t** out_result);
/**
 * Create an IfcLagTime and assign it to an IfcRelSequence.
 *
 * Replaces any existing TimeLag on the sequence (removing the old one if
 * it is sole-referenced).
 *
 * @param file File that receives the new entity.
 * @param rel_sequence IfcRelSequence to receive the lag time.
 * @param lag_value ISO 8601 duration string (e.g. "P2D", "PT8H").
 * @param options Duration type option. Defaults to "WORKTIME".
 * @return Newly created IfcLagTime.
 */
bool ifcopenshell_sequence_assign_lag_time(ifcopenshell_file_t* file, ifcopenshell_instance_t* rel_sequence, const char* lag_value, const ifcopenshell_sequence_assign_lag_time_options_t* options, ifcopenshell_instance_t** out_result);
/**
 * Assign a process (IfcTask) to an object via IfcRelAssignsToProcess.
 *
 * If a relationship already exists for this process and object, it is
 * returned. Otherwise a new relationship is created or the object is
 * appended to an existing relationship.
 *
 * @param file File that receives the relationship.
 * @param relating_process IfcTask (process) to assign.
 * @param related_object Object to assign to the process.
 * @param options Ownership options.
 * @return The IfcRelAssignsToProcess relationship.
 */
bool ifcopenshell_sequence_assign_process(ifcopenshell_file_t* file, ifcopenshell_instance_t* relating_process, ifcopenshell_instance_t* related_object, const ifcopenshell_sequence_assign_process_options_t* options, ifcopenshell_instance_t** out_result);
/**
 * Assign a product to an object via IfcRelAssignsToProduct.
 *
 * @param file File that receives the relationship.
 * @param relating_product IfcProduct to assign.
 * @param related_object Object to assign to the product.
 * @param options Ownership options.
 * @return The IfcRelAssignsToProduct relationship.
 */
bool ifcopenshell_sequence_assign_product(ifcopenshell_file_t* file, ifcopenshell_instance_t* relating_product, ifcopenshell_instance_t* related_object, const ifcopenshell_sequence_assign_product_options_t* options, ifcopenshell_instance_t** out_result);
/**
 * Create an IfcRecurrencePattern and assign it to a parent entity.
 *
 * Assigns the pattern to an IfcWorkTime's RecurrencePattern or an
 * IfcTaskTimeRecurring's Recurrence attribute. Replaces any existing
 * pattern on the parent (removing the old one if sole-referenced).
 *
 * @param file File that receives the new entity.
 * @param parent IfcWorkTime or IfcTaskTimeRecurring to receive the pattern.
 * @param recurrence_type RecurrenceType enum value (e.g. "WEEKLY").
 * @return Newly created IfcRecurrencePattern.
 */
bool ifcopenshell_sequence_assign_recurrence_pattern(ifcopenshell_file_t* file, ifcopenshell_instance_t* parent, const char* recurrence_type, ifcopenshell_instance_t** out_result);
/**
 * Create an IfcRelSequence linking two tasks.
 *
 * If a sequence relationship already exists between the two tasks, the
 * existing relationship is returned. After creating the relationship,
 * cascades schedule changes from the relating (predecessor) task.
 *
 * @param file File that receives the relationship.
 * @param relating_process Predecessor IfcTask.
 * @param related_process Successor IfcTask.
 * @param options Sequence type and ownership options.
 * @return The IfcRelSequence relationship.
 */
bool ifcopenshell_sequence_assign_sequence(ifcopenshell_file_t* file, ifcopenshell_instance_t* relating_process, ifcopenshell_instance_t* related_process, const ifcopenshell_sequence_assign_sequence_options_t* options, ifcopenshell_instance_t** out_result);
/**
 * Assign an IfcWorkSchedule to an IfcWorkPlan via IfcRelAggregates.
 *
 * First unassigns the schedule from its current project declaration, then
 * nests it under the work plan.
 *
 * @param file File containing both entities.
 * @param work_schedule IfcWorkSchedule to assign.
 * @param work_plan IfcWorkPlan to assign to.
 * @param options Ownership options.
 * @return The IfcRelAggregates relationship.
 */
bool ifcopenshell_sequence_assign_work_plan(ifcopenshell_file_t* file, ifcopenshell_instance_t* work_schedule, ifcopenshell_instance_t* work_plan, const ifcopenshell_sequence_assign_work_plan_options_t* options, ifcopenshell_instance_t** out_result);
/**
 * Recalculate a task's ScheduleDuration from its assigned resources.
 *
 * Computes the maximum resource usage duration across all IfcConstructionResource
 * entities assigned to the task (directly or via IfcRelAssignsToProcess),
 * accounting for ScheduleWork and ScheduleUsage. The result is written to the
 * task's IfcTaskTime.ScheduleDuration. Creates an IfcTaskTime if absent.
 *
 * @param file File containing the task.
 * @param task IfcTask entity to recalculate.
 */
bool ifcopenshell_sequence_calculate_task_duration(ifcopenshell_file_t* file, ifcopenshell_instance_t* task);
/**
 * Cascade schedule dates from a task to all its successors.
 *
 * Propagates ScheduleStart and ScheduleFinish dates forward through the
 * task network, respecting sequence types (FS, SS, FF, SF), lag times,
 * duration types, and work calendars.
 *
 * @param file File containing the task network.
 * @param task IfcTask from which to cascade.
 */
bool ifcopenshell_sequence_cascade_schedule(ifcopenshell_file_t* file, ifcopenshell_instance_t* task);
/**
 * Deep-copy an IfcWorkSchedule and all its controlled tasks.
 *
 * Shallow-copies the schedule, then deep-copies each controlled IfcTask
 * (with its subtasks and relationships) and assigns the copies to the new
 * schedule.
 *
 * @param file File that receives the copied entities.
 * @param work_schedule IfcWorkSchedule to copy.
 * @param options Ownership options for the copied entities.
 * @return Newly created IfcWorkSchedule.
 */
bool ifcopenshell_sequence_copy_work_schedule(ifcopenshell_file_t* file, ifcopenshell_instance_t* work_schedule, const ifcopenshell_sequence_copy_work_schedule_options_t* options, ifcopenshell_instance_t** out_result);
/**
 * Create a baseline snapshot of an IfcWorkSchedule.
 *
 * Only operates on schedules with PredefinedType PLANNED. Creates a new
 * IfcWorkSchedule with PredefinedType BASELINE, deep-copies all controlled
 * tasks into it, and creates IfcRelDefinesByObject references linking
 * original tasks to their baseline copies.
 *
 * @param file File that receives the baseline entities.
 * @param work_schedule IfcWorkSchedule to baseline.
 * @param options Name override and ownership options.
 */
bool ifcopenshell_sequence_create_baseline(ifcopenshell_file_t* file, ifcopenshell_instance_t* work_schedule, const ifcopenshell_sequence_create_baseline_options_t* options);
/**
 * Deep-copy a task and its subtasks, property sets, and sequence relationships.
 *
 * Creates duplicates of the task, its nested child tasks, property sets, and
 * IfcRelSequence relationships between duplicated tasks. Returns parallel
 * vectors of original and duplicated tasks in depth-first order.
 *
 * @param file File that receives the duplicated entities.
 * @param task IfcTask to duplicate.
 * @param options Ownership options for duplicated entities.
 * @return Parallel vectors of original and duplicated tasks.
 */
bool ifcopenshell_sequence_duplicate_task(ifcopenshell_file_t* file, ifcopenshell_instance_t* task, const ifcopenshell_sequence_duplicate_task_options_t* options, ifcopenshell_sequence_duplicate_task_result_t* out_result);
/**
 * Edit attributes of an IfcLagTime entity.
 *
 * @param lag_time IfcLagTime entity to edit.
 * @param attributes Property bag of attribute name/value pairs.
 */
bool ifcopenshell_sequence_edit_lag_time(ifcopenshell_instance_t* lag_time, void* attributes);
/**
 * Edit attributes of an IfcRecurrencePattern entity.
 *
 * @param recurrence_pattern IfcRecurrencePattern entity to edit.
 * @param attributes Property bag of attribute name/value pairs.
 */
bool ifcopenshell_sequence_edit_recurrence_pattern(ifcopenshell_instance_t* recurrence_pattern, void* attributes);
/**
 * Edit attributes of an IfcRelSequence entity.
 *
 * @param rel_sequence IfcRelSequence entity to edit.
 * @param attributes Property bag of attribute name/value pairs.
 */
bool ifcopenshell_sequence_edit_sequence(ifcopenshell_instance_t* rel_sequence, void* attributes);
/**
 * Edit attributes of an IfcTask entity.
 *
 * @param task IfcTask entity to edit.
 * @param attributes Property bag of attribute name/value pairs.
 */
bool ifcopenshell_sequence_edit_task(ifcopenshell_instance_t* task, void* attributes);
/**
 * Edit attributes of an IfcTaskTime entity.
 *
 * Applies attribute changes from the property bag. Automatically derives
 * ScheduleFinish from ScheduleStart + ScheduleDuration, or ScheduleDuration
 * from ScheduleStart and ScheduleFinish, as appropriate. Dates are normalised
 * to the soonest working day based on the task's calendar. After editing,
 * cascades schedule changes to successor tasks.
 *
 * @param file File containing the task time.
 * @param task_time IfcTaskTime entity to edit.
 * @param attributes Property bag of attribute name/value pairs.
 */
bool ifcopenshell_sequence_edit_task_time(ifcopenshell_file_t* file, ifcopenshell_instance_t* task_time, void* attributes);
/**
 * Edit attributes of an IfcWorkCalendar entity.
 *
 * @param work_calendar IfcWorkCalendar entity to edit.
 * @param attributes Property bag of attribute name/value pairs.
 */
bool ifcopenshell_sequence_edit_work_calendar(ifcopenshell_instance_t* work_calendar, void* attributes);
/**
 * Edit attributes of an IfcWorkPlan entity.
 *
 * @param work_plan IfcWorkPlan entity to edit.
 * @param attributes Property bag of attribute name/value pairs.
 */
bool ifcopenshell_sequence_edit_work_plan(ifcopenshell_instance_t* work_plan, void* attributes);
/**
 * Edit attributes of an IfcWorkSchedule entity.
 *
 * @param work_schedule IfcWorkSchedule entity to edit.
 * @param attributes Property bag of attribute name/value pairs.
 */
bool ifcopenshell_sequence_edit_work_schedule(ifcopenshell_instance_t* work_schedule, void* attributes);
/**
 * Edit attributes of an IfcWorkTime entity.
 *
 * Date attributes "Start"/"StartDate" and "Finish"/"FinishDate" are
 * automatically mapped to the correct attribute name for the schema version.
 *
 * @param work_time IfcWorkTime entity to edit.
 * @param attributes Property bag of attribute name/value pairs.
 */
bool ifcopenshell_sequence_edit_work_time(ifcopenshell_instance_t* work_time, void* attributes);
/**
 * Recalculate an entire work schedule using the critical path method.
 *
 * Performs forward and backward passes through the task network to compute
 * early/late start/finish dates, total float, free float, and critical
 * path flags on each IfcTaskTime.
 *
 * @param file File containing the work schedule.
 * @param work_schedule IfcWorkSchedule to recalculate.
 */
bool ifcopenshell_sequence_recalculate_schedule(ifcopenshell_file_t* file, ifcopenshell_instance_t* work_schedule);
/**
 * Remove an IfcTask and its subtasks from the file.
 *
 * Recursively removes nested child tasks, their IfcTaskTime entities,
 * sequence relationships, control assignments, process assignments,
 * product assignments, property sets, and project declarations.
 *
 * @param file File containing the task.
 * @param task IfcTask to remove.
 * @param options Ownership options for relationship cleanup.
 */
bool ifcopenshell_sequence_remove_task(ifcopenshell_file_t* file, ifcopenshell_instance_t* task, const ifcopenshell_sequence_remove_options_t* options);
/**
 * Remove an IfcTimePeriod from the file.
 *
 * @param file File containing the time period.
 * @param time_period IfcTimePeriod to remove.
 */
bool ifcopenshell_sequence_remove_time_period(ifcopenshell_file_t* file, ifcopenshell_instance_t* time_period);
/**
 * Remove an IfcWorkCalendar from the file.
 *
 * Unassigns the calendar from all controlled tasks, removes all working
 * and exception times, and removes the project declaration.
 *
 * @param file File containing the calendar.
 * @param work_calendar IfcWorkCalendar to remove.
 * @param options Ownership options for relationship cleanup.
 */
bool ifcopenshell_sequence_remove_work_calendar(ifcopenshell_file_t* file, ifcopenshell_instance_t* work_calendar, const ifcopenshell_sequence_remove_options_t* options);
/**
 * Remove an IfcWorkPlan from the file.
 *
 * Unassigns the plan from its project declaration and removes the aggregate
 * relationships to child work schedules (without removing the schedules).
 *
 * @param file File containing the work plan.
 * @param work_plan IfcWorkPlan to remove.
 * @param options Ownership options for relationship cleanup.
 */
bool ifcopenshell_sequence_remove_work_plan(ifcopenshell_file_t* file, ifcopenshell_instance_t* work_plan, const ifcopenshell_sequence_remove_options_t* options);
/**
 * Remove an IfcWorkSchedule and its controlled tasks from the file.
 *
 * Recursively removes nested work schedules, all controlled IfcTask
 * entities (with their subtasks and relationships), aggregate and
 * declaration relationships, and the schedule itself.
 *
 * @param file File containing the work schedule.
 * @param work_schedule IfcWorkSchedule to remove.
 * @param options Ownership options for relationship cleanup.
 */
bool ifcopenshell_sequence_remove_work_schedule(ifcopenshell_file_t* file, ifcopenshell_instance_t* work_schedule, const ifcopenshell_sequence_remove_options_t* options);
/**
 * Remove an IfcWorkTime and its associated recurrence pattern from the file.
 *
 * If the work time has a RecurrencePattern, all its TimePeriods and the
 * pattern itself are deleted first.
 *
 * @param file File containing the work time.
 * @param work_time IfcWorkTime to remove.
 */
bool ifcopenshell_sequence_remove_work_time(ifcopenshell_file_t* file, ifcopenshell_instance_t* work_time);
/**
 * Remove the TimeLag from an IfcRelSequence.
 *
 * If the IfcLagTime is only referenced by this sequence, it is deleted.
 * Otherwise the TimeLag attribute is cleared.
 *
 * @param file File containing the sequence.
 * @param rel_sequence IfcRelSequence to remove the lag from.
 */
bool ifcopenshell_sequence_unassign_lag_time(ifcopenshell_file_t* file, ifcopenshell_instance_t* rel_sequence);
/**
 * Remove a process assignment via IfcRelAssignsToProcess.
 *
 * If the relationship has only one RelatedObjects entry, the entire
 * relationship is removed. Otherwise the object is removed from the
 * aggregate.
 *
 * @param file File containing the relationship.
 * @param relating_process IfcTask to unassign from.
 * @param related_object Object to unassign.
 * @param options Ownership options.
 * @return The modified relationship, or a null handle if removed.
 */
bool ifcopenshell_sequence_unassign_process(ifcopenshell_file_t* file, ifcopenshell_instance_t* relating_process, ifcopenshell_instance_t* related_object, const ifcopenshell_sequence_remove_options_t* options, ifcopenshell_instance_t** out_result);
/**
 * Remove a product assignment via IfcRelAssignsToProduct.
 *
 * If the relationship has only one RelatedObjects entry, the entire
 * relationship is removed. Otherwise the object is removed from the
 * aggregate.
 *
 * @param file File containing the relationship.
 * @param relating_product IfcProduct to unassign from.
 * @param related_object Object to unassign.
 * @param options Ownership options.
 * @return The modified relationship, or a null handle if removed.
 */
bool ifcopenshell_sequence_unassign_product(ifcopenshell_file_t* file, ifcopenshell_instance_t* relating_product, ifcopenshell_instance_t* related_object, const ifcopenshell_sequence_remove_options_t* options, ifcopenshell_instance_t** out_result);
/**
 * Remove an IfcRecurrencePattern and its time periods from the file.
 *
 * Deletes all IfcTimePeriod entities in the pattern's TimePeriods aggregate,
 * then deletes the pattern itself.
 *
 * @param file File containing the recurrence pattern.
 * @param recurrence_pattern IfcRecurrencePattern to remove.
 */
bool ifcopenshell_sequence_unassign_recurrence_pattern(ifcopenshell_file_t* file, ifcopenshell_instance_t* recurrence_pattern);
/**
 * Remove an IfcRelSequence between two tasks.
 *
 * Removes the sequence relationship where relating_process is the
 * predecessor and related_process is the successor, then cascades
 * schedule changes from the successor.
 *
 * @param file File containing the tasks.
 * @param relating_process Predecessor IfcTask.
 * @param related_process Successor IfcTask.
 */
bool ifcopenshell_sequence_unassign_sequence(ifcopenshell_file_t* file, ifcopenshell_instance_t* relating_process, ifcopenshell_instance_t* related_process);
/**
 * Create an IfcAxis2Placement2D from position and optional reference direction.
 *
 * @param file IFC file that receives the placement.
 * @param options Position and optional X direction.
 * @return IfcAxis2Placement2D entity.
 */
bool ifcopenshell_shape_builder_axis2_placement_2d(ifcopenshell_file_t* file, const ifcopenshell_shape_builder_axis2_placement2d_options_t* options, ifcopenshell_instance_t** out_result);
/**
 * Create an IfcAxis2Placement3D from position and axes.
 *
 * @param file IFC file that receives the placement.
 * @param options Position, Z axis, and X axis direction ratios.
 * @return IfcAxis2Placement3D entity.
 */
bool ifcopenshell_shape_builder_axis2_placement_3d(ifcopenshell_file_t* file, const ifcopenshell_shape_builder_axis2_placement3d_options_t* options, ifcopenshell_instance_t** out_result);
/**
 * Create an IfcBlock (rectangular parallelepiped).
 *
 * @param file IFC file that receives the geometry.
 * @param options Position and XYZ dimensions.
 * @return IfcBlock entity.
 */
bool ifcopenshell_shape_builder_block(ifcopenshell_file_t* file, const ifcopenshell_shape_builder_block_options_t* options, ifcopenshell_instance_t** out_result);
/**
 * Create an IfcCircle centred at the given position.
 *
 * @param file IFC file that receives the geometry.
 * @param center XY coordinates of the circle centre.
 * @param radius Circle radius in model units.
 * @return IfcCircle entity.
 */
bool ifcopenshell_shape_builder_circle(ifcopenshell_file_t* file, const ifcopenshell_double_list_t* center, double radius, ifcopenshell_instance_t** out_result);
/**
 * Create a 2D IfcIndexedPolyCurve arc between two points.
 *
 * Inserts a midpoint slightly offset along the dominant axis to form a
 * three-point arc segment.
 *
 * @param file IFC file that receives the geometry.
 * @param points Two XY endpoints.
 * @return IfcIndexedPolyCurve entity with one arc segment.
 */
bool ifcopenshell_shape_builder_curve_between_two_points(ifcopenshell_file_t* file, const ifcopenshell_double_list_list_t* points, ifcopenshell_instance_t** out_result);
/**
 * Deep-copy an IFC entity and all entities it references.
 *
 * New GlobalId attributes are generated for the copied entities.
 *
 * @param file IFC file that receives the copy.
 * @param element Entity to deep-copy.
 * @return Root entity of the copied subgraph.
 */
bool ifcopenshell_shape_builder_deep_copy(ifcopenshell_file_t* file, ifcopenshell_instance_t* element, ifcopenshell_instance_t** out_result);
/**
 * Create an IfcEdge between two vertex positions.
 *
 * @param file IFC file that receives the geometry.
 * @param start XYZ coordinates of the edge start.
 * @param end XYZ coordinates of the edge end.
 * @return IfcEdge entity.
 */
bool ifcopenshell_shape_builder_edge(ifcopenshell_file_t* file, const ifcopenshell_double_list_t* start, const ifcopenshell_double_list_t* end, ifcopenshell_instance_t** out_result);
/**
 * Create an IfcEllipse, optionally trimmed to an IfcTrimmedCurve.
 *
 * @param file IFC file that receives the geometry.
 * @param options Radii, position, trim points, and reference direction.
 * @return IfcEllipse or IfcTrimmedCurve entity.
 */
bool ifcopenshell_shape_builder_ellipse_curve(ifcopenshell_file_t* file, const ifcopenshell_shape_builder_ellipse_curve_options_t* options, ifcopenshell_instance_t** out_result);
/**
 * Create an IfcExtrudedAreaSolid by extruding a profile along a direction.
 *
 * If the profile_or_curve is a bare IfcCurve (not an IfcProfileDef), it is
 * wrapped in an IfcArbitraryClosedProfileDef automatically.
 *
 * @param file IFC file that receives the geometry.
 * @param options Profile, depth, position, and extrusion direction.
 * @return IfcExtrudedAreaSolid entity.
 */
bool ifcopenshell_shape_builder_extrude(ifcopenshell_file_t* file, const ifcopenshell_shape_builder_extrude_options_t* options, ifcopenshell_instance_t** out_result);
/**
 * Create an IfcFace from an ordered list of polygon vertices.
 *
 * @param file IFC file that receives the geometry.
 * @param points XYZ coordinates defining the face outer boundary.
 * @return IfcFace entity with an IfcFaceOuterBound.
 */
bool ifcopenshell_shape_builder_face(ifcopenshell_file_t* file, const ifcopenshell_double_list_list_t* points, ifcopenshell_instance_t** out_result);
/**
 * Create an IfcFacetedBrep from vertices and face index lists.
 *
 * @param file IFC file that receives the geometry.
 * @param points XYZ vertex coordinates.
 * @param faces Face index lists (zero-based). Each face is a single outer loop.
 * @return IfcFacetedBrep entity with an IfcClosedShell.
 */
bool ifcopenshell_shape_builder_faceted_brep(ifcopenshell_file_t* file, const ifcopenshell_double_list_list_t* points, const ifcopenshell_int32_list_list_t* faces, ifcopenshell_instance_t** out_result);
/**
 * Read the coordinate list from an IfcPolyline or IfcIndexedPolyCurve.
 *
 * @param polyline IfcPolyline or IfcIndexedPolyCurve entity.
 * @return Ordered XY or XYZ coordinate vectors.
 */
bool ifcopenshell_shape_builder_get_polyline_coords(ifcopenshell_instance_t* polyline, ifcopenshell_double_list_list_t* out_result);
/**
 * Create an IfcHalfSpaceSolid bounded by a plane.
 *
 * @param file IFC file that receives the geometry.
 * @param options Bounding plane and agreement flag.
 * @return IfcHalfSpaceSolid entity.
 */
bool ifcopenshell_shape_builder_half_space_solid(ifcopenshell_file_t* file, const ifcopenshell_shape_builder_half_space_solid_options_t* options, ifcopenshell_instance_t** out_result);
/**
 * Create a 2D IfcIndexedPolyCurve from explicit points and segment indices.
 *
 * Segments with two indices are line segments; segments with three indices
 * are arc segments.
 *
 * @param file IFC file that receives the geometry.
 * @param points XY coordinates for the point list.
 * @param segments Segment index arrays (1-based).
 * @return IfcIndexedPolyCurve entity.
 */
bool ifcopenshell_shape_builder_indexed_polycurve_2d(ifcopenshell_file_t* file, const ifcopenshell_double_list_list_t* points, const ifcopenshell_int32_list_list_t* segments, ifcopenshell_instance_t** out_result);
/**
 * Build MEP bend geometry for a duct segment.
 *
 * Generates a swept disk solid (for circular profiles) or an extruded bend
 * shape (for rectangular profiles), plus optional start and end straight
 * extensions.
 *
 * @param file IFC file that receives the geometry.
 * @param options Segment, lengths, angle, radius, bend vector, and Z flip.
 * @return Bend result with representation and computed parameters.
 */
bool ifcopenshell_shape_builder_mep_bend_shape(ifcopenshell_file_t* file, const ifcopenshell_shape_builder_mep_bend_shape_options_t* options, ifcopenshell_shape_builder_mep_bend_shape_result_t* out_result);
/**
 * Compute the transition angle or length between two MEP duct profiles.
 *
 * Provide either length or angle (not both) to solve for the other.
 * Returns NaN when no valid solution exists.
 *
 * @param options Profile half-dimensions, offset, and one of length or angle.
 * @return The solved angle (degrees) or length (model units), or NaN.
 */
bool ifcopenshell_shape_builder_mep_transition_calculate(const ifcopenshell_shape_builder_mep_transition_calculate_options_t* options, double* out_result);
/**
 * Compute the minimum transition length between two MEP duct profiles at a given angle.
 *
 * @param options Profile half-dimensions, angle in degrees, and profile offset.
 * @return Required transition length in model units, or NaN if no valid solution.
 */
bool ifcopenshell_shape_builder_mep_transition_length(const ifcopenshell_shape_builder_mep_transition_length_options_t* options, double* out_result);
/**
 * Build MEP transition geometry between two duct segments.
 *
 * Generates start/end extrusions and a connecting transition mesh.
 * Returns nullopt when the segments lack material profiles or the
 * transition cannot be computed.
 *
 * @param file IFC file that receives the geometry.
 * @param options Start/end segments, lengths, angle, and profile offset.
 * @return Transition result with representation and dimensions, or nullopt.
 */
bool ifcopenshell_shape_builder_mep_transition_shape(ifcopenshell_file_t* file, const ifcopenshell_shape_builder_mep_transition_shape_options_t* options, ifcopenshell_optional_shape_builder_mep_transition_shape_result_t* out_result);
/**
 * Create a mesh representation using IfcPolygonalFaceSet (IFC4+) or IfcFacetedBrep (IFC2X3).
 *
 * @param file IFC file that receives the geometry.
 * @param points XYZ vertex coordinates.
 * @param faces Face index lists (zero-based).
 * @return IfcPolygonalFaceSet or IfcFacetedBrep entity.
 */
bool ifcopenshell_shape_builder_mesh(ifcopenshell_file_t* file, const ifcopenshell_double_list_list_t* points, const ifcopenshell_int32_list_list_t* faces, ifcopenshell_instance_t** out_result);
/**
 * Mirror a geometry item across 2D axes.
 *
 * Supports IfcIndexedPolyCurve, IfcPolyline, IfcCircle, IfcEllipse,
 * IfcTrimmedCurve, and IfcExtrudedAreaSolid. A non-zero value in
 * mirror_axes at index 0 or 1 flips the corresponding coordinate.
 *
 * @param file IFC file containing the item.
 * @param options Item, axes, point, copy flag, and optional placement matrix.
 * @return The mirrored item (same entity or a deep copy).
 */
bool ifcopenshell_shape_builder_mirror(ifcopenshell_file_t* file, const ifcopenshell_shape_builder_mirror_options_t* options, ifcopenshell_instance_t** out_result);
/**
 * Create an IfcPlane from a point and normal direction.
 *
 * The reference direction is computed automatically from the normal.
 *
 * @param file IFC file that receives the geometry.
 * @param location XYZ point on the plane.
 * @param normal Direction ratios of the plane normal.
 * @return IfcPlane entity.
 */
bool ifcopenshell_shape_builder_plane(ifcopenshell_file_t* file, const ifcopenshell_double_list_t* location, const ifcopenshell_double_list_t* normal, ifcopenshell_instance_t** out_result);
/**
 * Create an IfcPolygonalFaceSet from vertices and polygonal face loops.
 *
 * Each face may contain one outer loop and optional inner loops (voids).
 * Faces with multiple loops create IfcIndexedPolygonalFaceWithVoids entities.
 *
 * @param file IFC file that receives the geometry.
 * @param points XYZ vertex coordinates.
 * @param faces Per-face loop index lists (zero-based).
 * @return IfcPolygonalFaceSet entity.
 */
bool ifcopenshell_shape_builder_polygonal_face_set(ifcopenshell_file_t* file, const ifcopenshell_double_list_list_t* points, const ifcopenshell_int32_list_list_list_t* faces, ifcopenshell_instance_t** out_result);
/**
 * Create a 2D or 3D polyline (IfcPolyline for IFC2X3, IfcIndexedPolyCurve otherwise).
 *
 * @param file IFC file that receives the geometry.
 * @param options Points, closure, offset, and arc segment indices.
 * @return IfcPolyline or IfcIndexedPolyCurve entity.
 */
bool ifcopenshell_shape_builder_polyline(ifcopenshell_file_t* file, const ifcopenshell_shape_builder_polyline_options_t* options, ifcopenshell_instance_t** out_result);
/**
 * Create an IfcArbitraryClosedProfileDef or IfcArbitraryProfileDefWithVoids.
 *
 * @param file IFC file that receives the profile.
 * @param options Outer curve, optional inner curves, name, and profile type.
 * @return IfcProfileDef entity.
 */
bool ifcopenshell_shape_builder_profile(ifcopenshell_file_t* file, const ifcopenshell_shape_builder_profile_options_t* options, ifcopenshell_instance_t** out_result);
/**
 * Create an IfcShapeRepresentation or IfcTopologyRepresentation.
 *
 * The representation type is auto-detected from the items when omitted.
 * Topology types (Vertex, Edge, Path, Face, Shell) produce IfcTopologyRepresentation;
 * all others produce IfcShapeRepresentation.
 *
 * @param file IFC file that receives the representation.
 * @param options Context, items, and optional representation type.
 * @return IfcShapeRepresentation or IfcTopologyRepresentation entity.
 */
bool ifcopenshell_shape_builder_representation(ifcopenshell_file_t* file, const ifcopenshell_shape_builder_representation_options_t* options, ifcopenshell_instance_t** out_result);
/**
 * Rotate a geometry item around a 2D pivot point.
 *
 * Supports IfcIndexedPolyCurve, IfcPolyline, IfcCircle, and
 * IfcExtrudedAreaSolid. The angle is in degrees.
 *
 * @param file IFC file containing the item.
 * @param options Item, angle, pivot, direction, and copy flag.
 * @return The rotated item (same entity or a deep copy).
 */
bool ifcopenshell_shape_builder_rotate(ifcopenshell_file_t* file, const ifcopenshell_shape_builder_rotate_options_t* options, ifcopenshell_instance_t** out_result);
/**
 * Replace the coordinate list of an IfcPolyline or IfcIndexedPolyCurve.
 *
 * For IfcPolyline, the new coordinate count must match the existing point count.
 *
 * @param file IFC file containing the polyline.
 * @param polyline IfcPolyline or IfcIndexedPolyCurve entity.
 * @param coords Replacement coordinates.
 * @return The modified polyline entity.
 */
bool ifcopenshell_shape_builder_set_polyline_coords(ifcopenshell_file_t* file, ifcopenshell_instance_t* polyline, const ifcopenshell_double_list_list_t* coords, ifcopenshell_instance_t** out_result);
/**
 * Create an IfcSphere.
 *
 * @param file IFC file that receives the geometry.
 * @param options Radius and centre position.
 * @return IfcSphere entity.
 */
bool ifcopenshell_shape_builder_sphere(ifcopenshell_file_t* file, const ifcopenshell_shape_builder_sphere_options_t* options, ifcopenshell_instance_t** out_result);
/**
 * Create an IfcSweptDiskSolid along a directrix curve.
 *
 * @param file IFC file that receives the geometry.
 * @param path_curve IfcCurve entity defining the sweep path.
 * @param radius Disk radius in model units.
 * @return IfcSweptDiskSolid entity.
 */
bool ifcopenshell_shape_builder_swept_disk_solid(ifcopenshell_file_t* file, ifcopenshell_instance_t* path_curve, double radius, ifcopenshell_instance_t** out_result);
/**
 * Translate a geometry item by a vector.
 *
 * Supports IfcIndexedPolyCurve, IfcPolyline, IfcCircle, IfcEllipse,
 * IfcExtrudedAreaSolid, IfcTessellatedFaceSet, IfcShapeRepresentation,
 * and IfcTrimmedCurve.
 *
 * @param file IFC file containing the item.
 * @param options Item, translation vector, and copy flag.
 * @return The translated item (same entity or a deep copy).
 */
bool ifcopenshell_shape_builder_translate(ifcopenshell_file_t* file, const ifcopenshell_shape_builder_translate_options_t* options, ifcopenshell_instance_t** out_result);
/**
 * Create an IfcTriangulatedFaceSet from vertices and triangle index lists.
 *
 * @param file IFC file that receives the geometry.
 * @param points XYZ vertex coordinates.
 * @param faces Triangle index lists (zero-based, truncated to 3 vertices each).
 * @return IfcTriangulatedFaceSet entity.
 */
bool ifcopenshell_shape_builder_triangulated_face_set(ifcopenshell_file_t* file, const ifcopenshell_double_list_list_t* points, const ifcopenshell_int32_list_list_t* faces, ifcopenshell_instance_t** out_result);
/**
 * Create an IfcVertexPoint at the given position.
 *
 * @param file IFC file that receives the geometry.
 * @param position XYZ coordinates of the vertex.
 * @return IfcVertexPoint entity.
 */
bool ifcopenshell_shape_builder_vertex(ifcopenshell_file_t* file, const ifcopenshell_double_list_t* position, ifcopenshell_instance_t** out_result);
/**
 * Check whether two floating-point values are approximately equal.
 *
 * Uses strict less-than comparison. When tolerance is 0.0, it defaults
 * to 1e-6.
 *
 * @param value First value.
 * @param x Second value.
 * @param tolerance Maximum absolute difference (exclusive). Defaults to 1e-6 when 0.
 * @return True if |value - x| < tolerance.
 */
bool ifcopenshell_shape_is_almost_equal(double value, double x, double tolerance, bool* out_result);
/**
 * Contain products in a spatial structure via IfcRelContainedInSpatialStructure.
 *
 * Products previously contained in a different structure are moved. Products
 * previously aggregated under a different parent (IfcRelAggregates) are
 * removed from that aggregation. If an existing containment relationship
 * already relates to the same structure, products are merged into it.
 */
bool ifcopenshell_spatial_assign_container(ifcopenshell_file_t* file, const ifcopenshell_spatial_assign_container_options_t* options, ifcopenshell_instance_t** out_result);
/**
 * Remove products from a spatial structure reference relationship.
 *
 * Removes products from IfcRelReferencedInSpatialStructure relationships
 * for the given structure. If no products remain, the relationship is deleted.
 */
bool ifcopenshell_spatial_dereference_structure(ifcopenshell_file_t* file, const ifcopenshell_spatial_dereference_structure_options_t* options);
/**
 * Reference products in a spatial structure via IfcRelReferencedInSpatialStructure.
 *
 * Unlike containment, referencing does not establish ownership. Products
 * already referenced by the same structure are skipped.
 */
bool ifcopenshell_spatial_reference_structure(ifcopenshell_file_t* file, const ifcopenshell_spatial_reference_structure_options_t* options, ifcopenshell_instance_t** out_result);
/**
 * Remove products from their IfcRelContainedInSpatialStructure relationships.
 *
 * If no related elements remain after removal, the relationship is deleted.
 */
bool ifcopenshell_spatial_unassign_container(ifcopenshell_file_t* file, const ifcopenshell_spatial_unassign_container_options_t* options);
/**
 * Create an IfcStructuralActivity and connect it to a structural member.
 *
 * Creates the activity entity of the given class and predefined type, sets
 * its AppliedLoad and GlobalOrLocal attributes, and creates an
 * IfcRelConnectsStructuralActivity linking the activity to the structural
 * member.
 *
 * @param file File that receives the new entities.
 * @param applied_load IfcStructuralLoad to apply.
 * @param structural_member Structural member to connect the activity to.
 * @param ifc_class IFC class name (e.g. "IfcStructuralPlanarAction").
 * @param predefined_type Predefined type enum value.
 * @param global_or_local "GLOBAL_COORDS" or "LOCAL_COORDS".
 * @param options Ownership options for the activity and relationship.
 * @return Newly created IfcStructuralActivity.
 */
bool ifcopenshell_structural_add_structural_activity(ifcopenshell_file_t* file, ifcopenshell_instance_t* applied_load, ifcopenshell_instance_t* structural_member, const char* ifc_class, const char* predefined_type, const char* global_or_local, const ifcopenshell_structural_add_structural_activity_options_t* options, ifcopenshell_instance_t** out_result);
/**
 * Create an IfcStructuralAnalysisModel with PredefinedType LOADING_3D.
 *
 * @param file File that receives the new entity.
 * @param owner_history Owner history for the new entity. May be std::nullopt.
 * @return Newly created IfcStructuralAnalysisModel.
 */
bool ifcopenshell_structural_add_structural_analysis_model(ifcopenshell_file_t* file, ifcopenshell_instance_t* owner_history, ifcopenshell_instance_t** out_result);
/**
 * Create an IfcBoundaryCondition subclass entity.
 *
 * When a connection is provided, the boundary class is inferred:
 * IfcStructuralPointConnection yields IfcBoundaryNodeCondition,
 * IfcStructuralCurveConnection yields IfcBoundaryEdgeCondition,
 * IfcStructuralSurfaceConnection yields IfcBoundaryFaceCondition.
 * The condition is applied to the connection's AppliedCondition attribute.
 *
 * @param file File that receives the new entity.
 * @param ifc_class IFC boundary condition class name. Overridden when connection is provided.
 * @param options Name and connection options.
 * @return Newly created IfcBoundaryCondition subclass.
 */
bool ifcopenshell_structural_add_structural_boundary_condition(ifcopenshell_file_t* file, const char* ifc_class, const ifcopenshell_structural_add_structural_boundary_condition_options_t* options, ifcopenshell_instance_t** out_result);
/**
 * Create an IfcStructuralLoad subclass entity.
 *
 * @param file File that receives the new entity.
 * @param ifc_class IFC class name (e.g. "IfcStructuralLoadConfiguration").
 * @param name Optional name for the load entity.
 * @return Newly created IfcStructuralLoad subclass.
 */
bool ifcopenshell_structural_add_structural_load(ifcopenshell_file_t* file, const char* ifc_class, const char* name, ifcopenshell_instance_t** out_result);
/**
 * Create an IfcStructuralLoadCase entity.
 *
 * Creates the load case with PredefinedType LOAD_CASE and the given
 * ActionType and ActionSource enum values.
 *
 * @param file File that receives the new entity.
 * @param name Name for the load case.
 * @param action_type ActionType enum value (e.g. "PERMANENT_G", "VARIABLE_Q").
 * @param action_source ActionSource enum value (e.g. "WIND", "IMPOSED").
 * @param owner_history Owner history for the new entity. May be std::nullopt.
 * @return Newly created IfcStructuralLoadCase.
 */
bool ifcopenshell_structural_add_structural_load_case(ifcopenshell_file_t* file, const char* name, const char* action_type, const char* action_source, ifcopenshell_instance_t* owner_history, ifcopenshell_instance_t** out_result);
/**
 * Create an IfcStructuralLoadGroup entity.
 *
 * Creates the load group with PredefinedType LOAD_GROUP and the given
 * ActionType and ActionSource enum values.
 *
 * @param file File that receives the new entity.
 * @param name Name for the load group.
 * @param action_type ActionType enum value.
 * @param action_source ActionSource enum value.
 * @param owner_history Owner history for the new entity. May be std::nullopt.
 * @return Newly created IfcStructuralLoadGroup.
 */
bool ifcopenshell_structural_add_structural_load_group(ifcopenshell_file_t* file, const char* name, const char* action_type, const char* action_source, ifcopenshell_instance_t* owner_history, ifcopenshell_instance_t** out_result);
/**
 * Connect a structural member to a structural connection via IfcRelConnectsStructuralMember.
 *
 * If a connection already exists between the two, the existing relationship
 * is returned.
 *
 * @param file File containing both entities.
 * @param relating_structural_member IfcStructuralMember to connect.
 * @param related_structural_connection IfcStructuralConnection to connect to.
 * @param owner_history Owner history for the new relationship. May be std::nullopt.
 * @return The IfcRelConnectsStructuralMember relationship.
 */
bool ifcopenshell_structural_add_structural_member_connection(ifcopenshell_file_t* file, ifcopenshell_instance_t* relating_structural_member, ifcopenshell_instance_t* related_structural_connection, ifcopenshell_instance_t* owner_history, ifcopenshell_instance_t** out_result);
/**
 * Assign a structural member or connection to a product via IfcRelAssignsToProduct.
 *
 * If a relationship already exists between the two, the object is appended
 * to the existing relationship's RelatedObjects. Otherwise a new
 * IfcRelAssignsToProduct is created.
 *
 * @param file File containing both entities.
 * @param relating_product IfcProduct that the structural item references.
 * @param related_object Structural item to assign.
 * @param owner_history Owner history for new relationships. May be std::nullopt.
 * @return The IfcRelAssignsToProduct relationship.
 */
bool ifcopenshell_structural_assign_product(ifcopenshell_file_t* file, ifcopenshell_instance_t* relating_product, ifcopenshell_instance_t* related_object, ifcopenshell_instance_t* owner_history, ifcopenshell_instance_t** out_result);
/**
 * Assign products to an IfcStructuralAnalysisModel via IfcRelAssignsToGroup.
 *
 * @param file File containing the products and model.
 * @param products Products to assign to the model.
 * @param structural_analysis_model IfcStructuralAnalysisModel to assign to.
 * @param options Ownership options for the relationship.
 * @return The IfcRelAssignsToGroup relationship.
 */
bool ifcopenshell_structural_assign_structural_analysis_model(ifcopenshell_file_t* file, const ifcopenshell_instance_list_t* products, ifcopenshell_instance_t* structural_analysis_model, const ifcopenshell_structural_assign_structural_analysis_model_options_t* options, ifcopenshell_instance_t** out_result);
/**
 * Assign an IfcStructuralAnalysisModel to a building via IfcRelServicesBuildings.
 *
 * If a relationship already exists for this model, the building is appended
 * to its RelatedBuildings aggregate.
 *
 * @param file File containing both entities.
 * @param structural_analysis_model IfcStructuralAnalysisModel to assign.
 * @param building IfcBuilding to assign to.
 * @param owner_history Owner history for the new relationship. May be std::nullopt.
 * @return The IfcRelServicesBuildings relationship.
 */
bool ifcopenshell_structural_assign_to_building(ifcopenshell_file_t* file, ifcopenshell_instance_t* structural_analysis_model, ifcopenshell_instance_t* building, ifcopenshell_instance_t* owner_history, ifcopenshell_instance_t** out_result);
/**
 * Edit attributes of an IfcBoundaryCondition subclass.
 *
 * Each entry in the attributes bag must be a dictionary with "type" and
 * "value" sub-entries. The type specifies the IFC typed value class (e.g.
 * "IfcBoolean", "IfcForceMeasure") or "string"/"null" for direct values.
 *
 * @param file File containing the boundary condition.
 * @param condition IfcBoundaryCondition entity to edit.
 * @param attributes Property bag of typed attribute entries.
 */
bool ifcopenshell_structural_edit_structural_boundary_condition(ifcopenshell_file_t* file, ifcopenshell_instance_t* condition, void* attributes);
/**
 * Edit the ConditionCoordinateSystem of a structural item.
 *
 * Replaces the Axis and RefDirection directions on the item's
 * ConditionCoordinateSystem (IfcAxis2Placement3D). Creates the placement
 * if absent. Old direction entities are deleted if unreferenced.
 *
 * @param file File containing the structural item.
 * @param structural_item Structural item (e.g. IfcStructuralPointConnection).
 * @param axis 3-element direction vector for the Axis attribute.
 * @param ref_direction 3-element direction vector for the RefDirection attribute.
 */
bool ifcopenshell_structural_edit_structural_connection_cs(ifcopenshell_file_t* file, ifcopenshell_instance_t* structural_item, const ifcopenshell_double_list_t* axis, const ifcopenshell_double_list_t* ref_direction);
/**
 * Edit the Axis direction of a structural item.
 *
 * Replaces the Axis direction on the structural item (e.g.
 * IfcStructuralCurveMember). The old direction entity is deleted if
 * unreferenced.
 *
 * @param file File containing the structural item.
 * @param structural_item Structural item with an Axis attribute.
 * @param axis 3-element direction vector.
 */
bool ifcopenshell_structural_edit_structural_item_axis(ifcopenshell_file_t* file, ifcopenshell_instance_t* structural_item, const ifcopenshell_double_list_t* axis);
/**
 * Remove an IfcStructuralAnalysisModel from the file.
 *
 * Removes the model's IfcRelAssignsToGroup relationships and then the
 * model entity itself.
 *
 * @param file File containing the model.
 * @param structural_analysis_model IfcStructuralAnalysisModel to remove.
 */
bool ifcopenshell_structural_remove_structural_analysis_model(ifcopenshell_file_t* file, ifcopenshell_instance_t* structural_analysis_model);
/**
 * Remove a boundary condition from a structural connection.
 *
 * When connection is provided, clears and deletes its AppliedCondition.
 * When boundary_condition is provided instead, clears AppliedCondition on
 * all referencing entities and deletes the condition. At least one of
 * connection or boundary_condition must be provided.
 *
 * @param file File containing the entities.
 * @param options Connection or boundary condition to remove.
 */
bool ifcopenshell_structural_remove_structural_boundary_condition(ifcopenshell_file_t* file, const ifcopenshell_structural_remove_structural_boundary_condition_options_t* options);
/**
 * Remove an IfcRelConnectsStructuralMember and its AppliedCondition.
 *
 * Removes the boundary condition from the related structural connection
 * (if any), then removes the relationship and its OwnerHistory.
 *
 * @param file File containing the relationship.
 * @param relation IfcRelConnectsStructuralMember to remove.
 */
bool ifcopenshell_structural_remove_structural_connection_condition(ifcopenshell_file_t* file, ifcopenshell_instance_t* relation);
/**
 * Remove an IfcStructuralLoad subclass from the file.
 *
 * @param file File containing the load.
 * @param structural_load IfcStructuralLoad entity to remove.
 */
bool ifcopenshell_structural_remove_structural_load(ifcopenshell_file_t* file, ifcopenshell_instance_t* structural_load);
/**
 * Remove an IfcStructuralLoadCase from the file.
 *
 * Removes the load case's IfcRelAssignsToGroup relationships and then the
 * entity itself.
 *
 * @param file File containing the load case.
 * @param structural_load_case IfcStructuralLoadCase to remove.
 */
bool ifcopenshell_structural_remove_structural_load_case(ifcopenshell_file_t* file, ifcopenshell_instance_t* structural_load_case);
/**
 * Remove an IfcStructuralLoadGroup from the file.
 *
 * Removes singleton IfcRelAssignsToGroup relationships and then the entity
 * itself.
 *
 * @param file File containing the load group.
 * @param structural_load_group IfcStructuralLoadGroup to remove.
 */
bool ifcopenshell_structural_remove_structural_load_group(ifcopenshell_file_t* file, ifcopenshell_instance_t* structural_load_group);
/**
 * Remove products from an IfcStructuralAnalysisModel's group.
 *
 * @param file File containing the products and model.
 * @param products Products to unassign from the model.
 * @param structural_analysis_model IfcStructuralAnalysisModel to unassign from.
 * @param options Ownership options for updating the relationship.
 */
bool ifcopenshell_structural_unassign_structural_analysis_model(ifcopenshell_file_t* file, const ifcopenshell_instance_list_t* products, ifcopenshell_instance_t* structural_analysis_model, const ifcopenshell_structural_unassign_structural_analysis_model_options_t* options);
/**
 * Create a new presentation style entity.
 *
 * For IfcSurfaceStyle, the Side attribute defaults to "BOTH".
 *
 * @param file IFC file that receives the style.
 * @param name Style name (may be null for unnamed styles).
 * @param ifc_class IFC entity class (e.g. "IfcSurfaceStyle", "IfcFillAreaStyle").
 * @return Newly created style entity.
 */
bool ifcopenshell_style_add_style(ifcopenshell_file_t* file, const char* name, const char* ifc_class, ifcopenshell_instance_t** out_result);
/**
 * Assign or replace a style on a single representation item.
 *
 * Creates an IfcStyledItem (and optionally an IfcPresentationStyleAssignment
 * for IFC2X3) linking the item to the given style. If the item already has a
 * styled item, the existing style is replaced. Passing an empty style removes
 * the styled item from the representation item.
 *
 * @param file IFC file to modify.
 * @param options Item, style, and IFC2X3 compat flag.
 * @return The IfcStyledItem, or null handle if style was removed.
 */
bool ifcopenshell_style_assign_item_style(ifcopenshell_file_t* file, const ifcopenshell_style_assign_item_style_options_t* options, ifcopenshell_instance_t** out_result);
/**
 * Assign a style to a material via IfcMaterialDefinitionRepresentation.
 *
 * Creates or extends a styled representation under the material's definition.
 * Also propagates the style to shape aspects whose names match material
 * constituent names.
 *
 * @param file IFC file to modify.
 * @param material IfcMaterial entity.
 * @param style Presentation style entity to assign.
 * @param context IfcGeometricRepresentationContext for the styled representation.
 * @param should_use_presentation_style_assignment Wrap style in IfcPresentationStyleAssignment.
 */
bool ifcopenshell_style_assign_material_style(ifcopenshell_file_t* file, ifcopenshell_instance_t* material, ifcopenshell_instance_t* style, ifcopenshell_instance_t* context, bool should_use_presentation_style_assignment);
/**
 * Assign styles to the geometric items within a shape representation.
 *
 * Traverses the representation and assigns each style to sequential
 * representation items. When replace_previous_same_type_style is true, styles
 * of the same IFC class are replaced rather than appended.
 *
 * @param file IFC file to modify.
 * @param shape_representation IfcShapeRepresentation to assign styles to.
 * @param styles Presentation style entities to assign.
 * @param should_use_presentation_style_assignment Wrap styles in IfcPresentationStyleAssignment.
 * @param replace_previous_same_type_style Replace existing styles of the same type.
 * @return Vector of newly created IfcStyledItem entities.
 */
bool ifcopenshell_style_assign_representation_styles(ifcopenshell_file_t* file, ifcopenshell_instance_t* shape_representation, const ifcopenshell_instance_list_t* styles, bool should_use_presentation_style_assignment, bool replace_previous_same_type_style, ifcopenshell_parse_instance_list_t** out_result);
/**
 * Edit attributes of an IfcSurfaceStyle or its nested surface styles.
 *
 * Handles colour attributes (IfcColourRgb), specular highlights, and
 * colour-or-factor properties specially; other attributes are set directly.
 *
 * @param file IFC file containing the style.
 * @param style IfcSurfaceStyle entity to modify.
 * @param attributes Property container with attribute name-value pairs.
 */
bool ifcopenshell_style_edit_surface_style(ifcopenshell_file_t* file, ifcopenshell_instance_t* style, void* attributes);
/**
 * Remove a presentation style and clean up all references to it.
 *
 * Removes inverse IfcStyledItem references, nested surface styles (for
 * IfcSurfaceStyle), fill area style hatching, and orphaned
 * IfcPresentationStyleAssignment entities.
 *
 * @param file IFC file to modify.
 * @param style Presentation style entity to remove.
 */
bool ifcopenshell_style_remove_style(ifcopenshell_file_t* file, ifcopenshell_instance_t* style);
/**
 * Remove an IfcStyledRepresentation and its parent IfcMaterialDefinitionRepresentation.
 *
 * @param file IFC file to modify.
 * @param representation IfcStyledRepresentation entity to remove.
 */
bool ifcopenshell_style_remove_styled_representation(ifcopenshell_file_t* file, ifcopenshell_instance_t* representation);
/**
 * Remove an IfcSurfaceStyleWithTextures or IfcSurfaceStyleRendering and its nested entities.
 *
 * Deep-removes texture coordinates, textures, and colour entities owned by the
 * surface style.
 *
 * @param file IFC file to modify.
 * @param style Surface style sub-entity to remove.
 */
bool ifcopenshell_style_remove_surface_style(ifcopenshell_file_t* file, ifcopenshell_instance_t* style);
/**
 * Remove a style from a material's styled representation.
 *
 * Cleans up empty IfcStyledItem, IfcStyledRepresentation, and
 * IfcMaterialDefinitionRepresentation entities. Also propagates removal to
 * matching shape aspects.
 *
 * @param file IFC file to modify.
 * @param material IfcMaterial entity.
 * @param style Presentation style entity to remove.
 * @param context IfcGeometricRepresentationContext identifying the styled representation.
 */
bool ifcopenshell_style_unassign_material_style(ifcopenshell_file_t* file, ifcopenshell_instance_t* material, ifcopenshell_instance_t* style, ifcopenshell_instance_t* context);
/**
 * Remove styles from the geometric items within a shape representation.
 *
 * Traverses the representation and removes matching styles from IfcStyledItem
 * and IfcPresentationStyleAssignment entities.
 *
 * @param file IFC file to modify.
 * @param shape_representation IfcShapeRepresentation to unassign styles from.
 * @param styles Presentation style entities to remove.
 * @param should_use_presentation_style_assignment Also search inside IfcPresentationStyleAssignment.
 */
bool ifcopenshell_style_unassign_representation_styles(ifcopenshell_file_t* file, ifcopenshell_instance_t* shape_representation, const ifcopenshell_instance_list_t* styles, bool should_use_presentation_style_assignment);
/**
 * Create a new IfcDistributionPort and optionally assign it to an element.
 *
 * If an element is provided, the port is assigned to it via IfcRelNests
 * (IFC4+) or IfcRelConnectsPortToElement (IFC2X3).
 */
bool ifcopenshell_system_add_port(ifcopenshell_file_t* file, const ifcopenshell_system_add_port_options_t* options, ifcopenshell_instance_t** out_result);
/**
 * Create a new system with the given IFC class.
 *
 * In IFC2X3, "IfcDistributionSystem" is automatically resolved to "IfcSystem".
 * The system is created with Name set to "Unnamed".
 */
bool ifcopenshell_system_add_system(ifcopenshell_file_t* file, const ifcopenshell_system_add_system_options_t* options, ifcopenshell_instance_t** out_result);
/**
 * Assign a flow control element to a flow element via IfcRelFlowControlElements.
 *
 * If the flow control is already assigned to a different element, no change
 * is made and an empty handle is returned.
 */
bool ifcopenshell_system_assign_flow_control(ifcopenshell_file_t* file, const ifcopenshell_system_assign_flow_control_options_t* options, ifcopenshell_instance_t** out_result);
/**
 * Assign a port to an element.
 *
 * In IFC2X3, uses IfcRelConnectsPortToElement. In IFC4+, uses IfcRelNests
 * to nest the port under the element. Updates the port's ObjectPlacement
 * if it has an IfcLocalPlacement.
 */
bool ifcopenshell_system_assign_port(ifcopenshell_file_t* file, const ifcopenshell_system_assign_port_options_t* options, ifcopenshell_instance_t** out_result);
/**
 * Assign products to a system via IfcRelAssignsToGroup (delegated to group_assign_group).
 *
 * Validates that each product is compatible with the system type (e.g.
 * only IfcDistributionElement for IfcDistributionSystem). Throws if a
 * product is not assignable.
 */
bool ifcopenshell_system_assign_system(ifcopenshell_file_t* file, const ifcopenshell_system_assign_system_options_t* options, ifcopenshell_instance_t** out_result);
/**
 * Connect two ports together with a given flow direction.
 *
 * Creates IfcRelConnectsPorts relationships and sets FlowDirection on both
 * ports. Existing connections to other ports are removed first. The
 * direction parameter controls which port is SOURCE, SINK, or both.
 */
bool ifcopenshell_system_connect_port(ifcopenshell_file_t* file, const ifcopenshell_system_connect_port_options_t* options);
/**
 * Disconnect a port from all connected ports.
 *
 * Removes all IfcRelConnectsPorts relationships involving the port and
 * clears FlowDirection on both sides.
 */
bool ifcopenshell_system_disconnect_port(ifcopenshell_file_t* file, ifcopenshell_instance_t* port);
/**
 * Remove a system and its relationships.
 *
 * Removes associated IfcRelDefinesByProperties (property sets) and
 * IfcRelAssignsToGroup relationships, then deletes the system entity.
 */
bool ifcopenshell_system_remove_system(ifcopenshell_file_t* file, ifcopenshell_instance_t* system);
/**
 * Remove a flow control element from a flow element's IfcRelFlowControlElements.
 *
 * If the control element is the only one in the relationship, the
 * relationship is deleted. Otherwise, the control element is removed from
 * the RelatedControlElements aggregate.
 */
bool ifcopenshell_system_unassign_flow_control(ifcopenshell_file_t* file, const ifcopenshell_system_unassign_flow_control_options_t* options);
/**
 * Remove a port from an element.
 *
 * In IFC2X3, removes the IfcRelConnectsPortToElement. In IFC4+, removes
 * the port from the IfcRelNests relationship (or deletes the relationship
 * if it was the only nested object).
 */
bool ifcopenshell_system_unassign_port(ifcopenshell_file_t* file, const ifcopenshell_system_unassign_port_options_t* options);
/** Remove products from a system (delegated to group_unassign_group). */
bool ifcopenshell_system_unassign_system(ifcopenshell_file_t* file, const ifcopenshell_system_unassign_system_options_t* options);
/**
 * Assign a type to element occurrences via IfcRelDefinesByType.
 *
 * Objects previously typed by a different type are moved. If an existing
 * IfcRelDefinesByType relationship already relates to the same type,
 * objects are merged into it. When should_map_representations is true
 * (default), IfcRepresentationMaps from the type are propagated onto
 * each occurrence and PredefinedType/ObjectType are cleared to avoid
 * double-typing.
 */
bool ifcopenshell_type_assign_type(ifcopenshell_file_t* file, const ifcopenshell_type_assign_type_options_t* options, ifcopenshell_instance_t** out_result);
/**
 * Map IfcRepresentationMaps from a type onto a related object.
 *
 * Clones mapped representations from the type's RepresentationMaps and
 * assigns them to the object. Returns true if any representations were
 * mapped.
 */
bool ifcopenshell_type_map_type_representations(ifcopenshell_file_t* file, ifcopenshell_instance_t* related_object, ifcopenshell_instance_t* relating_type, bool* out_result);
/**
 * Remove type assignments from element occurrences.
 *
 * Removes objects from IfcRelDefinesByType relationships. If no objects
 * remain in a relationship, the relationship is deleted.
 */
bool ifcopenshell_type_unassign_type(ifcopenshell_file_t* file, const ifcopenshell_type_unassign_type_options_t* options);
/**
 * Create an IfcContextDependentUnit entity.
 *
 * Creates a unit with custom dimensional exponents, unit type, and name.
 *
 * @param file File that receives the new entity.
 * @param unit_type IFC unit type enum value (e.g. "LENGTHUNIT").
 * @param name Display name for the unit (e.g. "bag", "each").
 * @param dimensions 7-element vector of dimensional exponents.
 * @return Newly created IfcContextDependentUnit.
 */
bool ifcopenshell_unit_add_context_dependent_unit(ifcopenshell_file_t* file, const char* unit_type, const char* name, const ifcopenshell_int64_list_t* dimensions, ifcopenshell_instance_t** out_result);
/**
 * Create an IfcDerivedUnit entity.
 *
 * Constructs a derived unit from a list of component units and their
 * exponents (e.g. m/s from ["METRE", "SECOND"] with exponents [1, -1]).
 *
 * @param file File that receives the new entity.
 * @param unit_type IFC unit type enum value (e.g. "VELOCITYUNIT").
 * @param userdefinedtype UserDefinedType string, or null to leave blank.
 * @param units Component IfcUnit entities.
 * @param exponents Exponent for each component unit (must match units in length).
 * @return Newly created IfcDerivedUnit.
 */
bool ifcopenshell_unit_add_derived_unit(ifcopenshell_file_t* file, const char* unit_type, const char* userdefinedtype, const ifcopenshell_instance_list_t* units, const ifcopenshell_int64_list_t* exponents, ifcopenshell_instance_t** out_result);
/**
 * Create an IfcMonetaryUnit entity.
 *
 * @param file File that receives the new entity.
 * @param currency ISO 4217 currency code (e.g. "USD", "EUR").
 * @return Newly created IfcMonetaryUnit.
 */
bool ifcopenshell_unit_add_monetary_unit(ifcopenshell_file_t* file, const char* currency, ifcopenshell_instance_t** out_result);
/**
 * Create an IfcSIUnit entity.
 *
 * Sets the UnitType, Name (derived from the unit type), and optional
 * Prefix. The Prefix attribute is left blank when prefix is null.
 *
 * @param file File that receives the new entity.
 * @param unit_type IFC unit type enum value (e.g. "LENGTHUNIT").
 * @param prefix SI prefix (e.g. "KILO", "MILLI") or null for base unit.
 * @return Newly created IfcSIUnit.
 */
bool ifcopenshell_unit_add_si_unit(ifcopenshell_file_t* file, const char* unit_type, const char* prefix, ifcopenshell_instance_t** out_result);
/**
 * Calculate the scale factor from SI for a project unit type.
 *
 * Returns the multiplier needed to convert from the project's unit to SI
 * base units. For example, if the project length unit is millimetres,
 * returns 0.001. Defaults to 1.0 when no unit is found.
 *
 * @param file File to query for the project unit.
 * @param unit_type IFC unit type (e.g. "LENGTHUNIT"). Defaults to "LENGTHUNIT" when empty.
 * @return Scale factor to SI.
 */
bool ifcopenshell_unit_calculate_unit_scale(ifcopenshell_file_t* file, const char* unit_type, double* out_result);
/**
 * Convert a numeric value between units specified by prefix and name strings.
 *
 * Supports SI conversions and prefix-aware scaling. For example,
 * converting from ("KILO", "METRE") to ("", "METRE") multiplies by 1000.
 *
 * @param value Numeric value to convert.
 * @param from_prefix Source SI prefix (e.g. "KILO"). Empty for base unit.
 * @param from_unit Source unit name (e.g. "METRE", "GRAM").
 * @param to_prefix Target SI prefix.
 * @param to_unit Target unit name.
 * @return The converted value.
 */
bool ifcopenshell_unit_convert(double value, const char* from_prefix, const char* from_unit, const char* to_prefix, const char* to_unit, double* out_result);
/**
 * Convert a numeric value between two IFC unit entities.
 *
 * Reads the Prefix and Name attributes from each unit to perform the
 * conversion.
 *
 * @param value Numeric value to convert.
 * @param from_unit Source IfcUnit entity.
 * @param to_unit Target IfcUnit entity.
 * @return The converted value.
 */
bool ifcopenshell_unit_convert_unit(double value, ifcopenshell_instance_t* from_unit, ifcopenshell_instance_t* to_unit, double* out_result);
/**
 * Format a length value as an imperial or metric string.
 *
 * For imperial output: returns feet-inches notation with fractional
 * precision (e.g. "5' - 3 1/4\""). For metric: returns a rounded
 * decimal value.
 *
 * @param value Length value to format.
 * @param precision Fractional precision denominator for imperial (e.g. 16 for 1/16").
 * @param decimal_places Number of decimal places for metric output.
 * @param suppress_zero_inches When true, omits zero inches in imperial output.
 * @param unit_system "imperial" or "metric".
 * @param input_unit Source unit: "foot" or "inch". Defaults to "foot" when empty.
 * @param output_unit Target unit: "foot" or "inch". Defaults to "foot" when empty.
 * @return Formatted length string.
 */
bool ifcopenshell_unit_format_length(double value, double precision, int32_t decimal_places, bool suppress_zero_inches, const char* unit_system, const char* input_unit, const char* output_unit, ifcopenshell_string_t* out_result);
/**
 * Return the full name of an IfcSIUnit (prefix + name).
 *
 * For example, a unit with Prefix "KILO" and Name "METRE" returns
 * "KILOMETRE".
 *
 * @param unit IfcSIUnit entity.
 * @return Concatenated prefix and name string.
 */
bool ifcopenshell_unit_get_full_unit_name(ifcopenshell_instance_t* unit, ifcopenshell_string_t* out_result);
/**
 * Derive the IFC measure class name from a unit type string.
 *
 * For example, "LENGTHUNIT" returns "IfcLengthMeasure",
 * "USERDEFINED" returns "IfcNumericMeasure". Returns an empty string
 * for unrecognised input.
 *
 * @param unit_type IFC unit type enum value (e.g. "LENGTHUNIT").
 * @return The corresponding IFC measure class name.
 */
bool ifcopenshell_unit_get_measure_class(const char* unit_type, ifcopenshell_string_t* out_result);
/**
 * Derive the IFC unit type string from a measure class name.
 *
 * The inverse of unit_get_measure_class. For example,
 * "IfcLengthMeasure" returns "LENGTHUNIT".
 *
 * @param measure_class IFC measure class name.
 * @return The corresponding IFC unit type, or empty string if unrecognised.
 */
bool ifcopenshell_unit_get_measure_unit_type(const char* measure_class, ifcopenshell_string_t* out_result);
/**
 * Return the named dimensional exponents for a given unit type name.
 *
 * Like unit_get_si_dimensions but returns all zeros for unknown types
 * instead of falling back.
 *
 * @param name Unit type name.
 * @return 7-element vector of dimensional exponents.
 */
bool ifcopenshell_unit_get_named_dimensions(const char* name, ifcopenshell_int32_list_t* out_result);
/**
 * Extract the SI prefix from a combined unit text string.
 *
 * For example, "KILOMETRE" returns "KILO", "MILLIGRAM" returns "MILLI".
 * Returns an empty string if no prefix is found.
 *
 * @param text Combined prefix and unit name string.
 * @return The prefix portion, or empty string if none.
 */
bool ifcopenshell_unit_get_prefix(const char* text, ifcopenshell_string_t* out_result);
/**
 * Return the numeric multiplier for an SI prefix.
 *
 * For example, "KILO" returns 1000.0, "MILLI" returns 0.001.
 * Returns 1.0 if no prefix is found.
 *
 * @param text SI prefix string (e.g. "KILO", "MILLI").
 * @return The multiplier value.
 */
bool ifcopenshell_unit_get_prefix_multiplier(const char* text, double* out_result);
/**
 * Return the project unit for a given unit type.
 *
 * Searches the project's IfcUnitAssignment for a unit whose UnitType
 * matches the given string.
 *
 * @param file File to query.
 * @param unit_type IFC unit type enum value (e.g. "LENGTHUNIT").
 * @return The matching unit entity, or a null handle if not found.
 */
bool ifcopenshell_unit_get_project_unit(ifcopenshell_file_t* file, const char* unit_type, ifcopenshell_instance_t** out_result);
/**
 * Return the SI dimensional exponents for a given unit type name.
 *
 * Returns a 7-element vector of integers corresponding to the
 * IfcDimensionalExponents attributes: Length, Mass, Time,
 * ElectricCurrent, ThermodynamicTemperature, AmountOfSubstance,
 * LuminousIntensity. Falls back to the "OTHERWISE" entry for
 * unknown types.
 *
 * @param name Unit type name (e.g. "LENGTHUNIT", "MASSUNIT").
 * @return 7-element vector of dimensional exponents.
 */
bool ifcopenshell_unit_get_si_dimensions(const char* name, ifcopenshell_int32_list_t* out_result);
/**
 * Infer the IFC measure class from a unit symbol string.
 *
 * For example, "m" returns "IfcLengthMeasure", "m2" returns "IfcAreaMeasure",
 * "kg" returns "IfcMassMeasure". Returns "IfcNumericMeasure" for unknown symbols.
 *
 * @param symbol Unit symbol (e.g. "m", "ft", "kg", "hr").
 * @return The corresponding IFC measure class name.
 */
bool ifcopenshell_unit_get_symbol_measure_class(const char* symbol, ifcopenshell_string_t* out_result);
/**
 * Infer the IFC quantity class from a unit symbol string.
 *
 * For example, "m" returns "IfcQuantityLength", "m2" returns "IfcQuantityArea",
 * "kg" returns "IfcQuantityWeight". Returns "IfcQuantityCount" for unknown symbols.
 *
 * @param symbol Unit symbol (e.g. "m", "ft", "kg", "hr").
 * @return The corresponding IFC quantity class name.
 */
bool ifcopenshell_unit_get_symbol_quantity_class(const char* symbol, ifcopenshell_string_t* out_result);
/**
 * Return the IfcUnitAssignment entity for the project.
 *
 * @param file File to query.
 * @return The IfcUnitAssignment entity, or a null handle if not found.
 */
bool ifcopenshell_unit_get_unit_assignment(ifcopenshell_file_t* file, ifcopenshell_instance_t** out_result);
/**
 * Extract the base unit name from a combined unit text string.
 *
 * Normalises "METER" to "METRE" before matching. For example,
 * "KILOMETRE" returns "METRE", "SQUARE_METRE" returns "SQUARE_METRE".
 * Returns an empty string if no unit name is found.
 *
 * @param text Combined prefix and unit name string.
 * @return The base unit name, or empty string if not recognised.
 */
bool ifcopenshell_unit_get_unit_name(const char* text, ifcopenshell_string_t* out_result);
/**
 * Extract the base unit name, accepting both metric and imperial names.
 *
 * Like unit_get_unit_name but also recognises imperial unit names
 * (e.g. "FOOT", "INCH"). Returns an empty string if not recognised.
 *
 * @param text Unit name string.
 * @return The base unit name, or empty string if not recognised.
 */
bool ifcopenshell_unit_get_unit_name_universal(const char* text, ifcopenshell_string_t* out_result);
/**
 * Return the display symbol for a unit entity.
 *
 * For IfcSIUnit: prefix symbol + unit symbol (e.g. "km").
 * For IfcContextDependentUnit with USERDEFINED UnitType: the Name attribute.
 * Returns "?" for unrecognised units.
 *
 * @param unit IfcUnit entity (IfcSIUnit, IfcContextDependentUnit, etc.).
 * @return Symbol string.
 */
bool ifcopenshell_unit_get_unit_symbol(ifcopenshell_instance_t* unit, ifcopenshell_string_t* out_result);
/**
 * Remove a unit entity from the file.
 *
 * Removes the unit from the project's IfcUnitAssignment (if present) and
 * then deletes the unit entity (and its dimensional exponents, if owned).
 *
 * @param file File containing the unit.
 * @param unit IfcUnit entity to remove.
 */
bool ifcopenshell_unit_remove_unit(ifcopenshell_file_t* file, ifcopenshell_instance_t* unit);
/**
 * Resolve the IFC measure class for a property or quantity.
 *
 * Returns the measure class name (e.g. "IfcLengthMeasure") derived from
 * the property's NominalValue or quantity type. Returns an empty string
 * if a unit entity is directly attached (use unit_resolve_property_unit
 * instead).
 *
 * @param prop IfcProperty or IfcPhysicalQuantity entity.
 * @return Measure class name, or empty string if a unit entity is attached.
 */
bool ifcopenshell_unit_resolve_property_measure_class(ifcopenshell_instance_t* prop, ifcopenshell_string_t* out_result);
/**
 * Resolve the defined measure class of an IfcPropertyTableValue.
 *
 * Returns the measure class name derived from the DefinedValues.
 * Returns an empty string if a DefinedUnit entity is directly attached.
 *
 * @param prop IfcPropertyTableValue entity.
 * @return Measure class name, or empty string if a unit entity is attached.
 */
bool ifcopenshell_unit_resolve_property_table_defined_measure_class(ifcopenshell_instance_t* prop, ifcopenshell_string_t* out_result);
/**
 * Resolve the defined unit of an IfcPropertyTableValue.
 *
 * Returns the DefinedUnit attribute, or a null handle if the unit
 * must be inferred from the DefinedValues measure class.
 *
 * @param prop IfcPropertyTableValue entity.
 * @return The DefinedUnit entity, or a null handle.
 */
bool ifcopenshell_unit_resolve_property_table_defined_unit(ifcopenshell_instance_t* prop, ifcopenshell_instance_t** out_result);
/**
 * Resolve the defining measure class of an IfcPropertyTableValue.
 *
 * Returns the measure class name derived from the DefiningValues.
 * Returns an empty string if a DefiningUnit entity is directly attached.
 *
 * @param prop IfcPropertyTableValue entity.
 * @return Measure class name, or empty string if a unit entity is attached.
 */
bool ifcopenshell_unit_resolve_property_table_defining_measure_class(ifcopenshell_instance_t* prop, ifcopenshell_string_t* out_result);
/**
 * Resolve the defining unit of an IfcPropertyTableValue.
 *
 * Returns the DefiningUnit attribute, or a null handle if the unit
 * must be inferred from the DefiningValues measure class.
 *
 * @param prop IfcPropertyTableValue entity.
 * @return The DefiningUnit entity, or a null handle.
 */
bool ifcopenshell_unit_resolve_property_table_defining_unit(ifcopenshell_instance_t* prop, ifcopenshell_instance_t** out_result);
/**
 * Resolve the unit entity attached to a property or quantity.
 *
 * Returns the Unit attribute directly attached to the property (for
 * IfcPropertySingleValue, IfcPhysicalSimpleQuantity, etc.), or a null
 * handle if the unit must be inferred from the measure class.
 *
 * @param prop IfcProperty or IfcPhysicalQuantity entity.
 * @return The attached IfcUnit, or a null handle if none.
 */
bool ifcopenshell_unit_resolve_property_unit(ifcopenshell_instance_t* prop, ifcopenshell_instance_t** out_result);
/**
 * Remove units from the project's IfcUnitAssignment.
 *
 * Removes the given units from the IfcUnitAssignment's Units aggregate.
 * If the aggregate becomes empty, the IfcUnitAssignment entity itself is
 * deleted. The unit entities are not deleted.
 *
 * @param file File containing the unit assignment.
 * @param units Units to unassign.
 */
bool ifcopenshell_unit_unassign_unit(ifcopenshell_file_t* file, const ifcopenshell_instance_list_t* units);
/**
 * Extract a boolean from a selector value.
 *
 * @param value The value handle.
 * @return The boolean value, or false if null or not a boolean.
 */
bool ifcopenshell_value_as_bool(ifcopenshell_value_t* value, bool* out_result);
/**
 * Extract a double from a selector value.
 *
 * @param value The value handle.
 * @return The double value, or 0.0 if null or not a double.
 */
bool ifcopenshell_value_as_double(ifcopenshell_value_t* value, double* out_result);
/**
 * Extract an entity instance from a selector value.
 *
 * @param value The value handle.
 * @return The entity instance, or empty if null or not an instance.
 */
bool ifcopenshell_value_as_instance(ifcopenshell_value_t* value, ifcopenshell_instance_t** out_result);
/**
 * Extract a 64-bit integer from a selector value.
 *
 * @param value The value handle.
 * @return The integer value, or 0 if null or not an integer.
 */
bool ifcopenshell_value_as_int64(ifcopenshell_value_t* value, int64_t* out_result);
/**
 * Extract a string from a selector value.
 *
 * @param value The value handle.
 * @return The string value, or empty string if null or not a string.
 */
bool ifcopenshell_value_as_string(ifcopenshell_value_t* value, ifcopenshell_string_t* out_result);
/**
 * Return the key at the given index in a dictionary value.
 *
 * @param value The dict value handle.
 * @param index Zero-based entry index.
 * @return The key string, or empty if out of range or not a dict.
 */
bool ifcopenshell_value_dict_key_at(ifcopenshell_value_t* value, size_t index, ifcopenshell_string_t* out_result);
/**
 * Set a key-value pair in a dictionary value.
 *
 * @param dict The dict value handle.
 * @param key The string key.
 * @param value The value to associate with the key. If empty, sets a None value.
 * @return True if the key-value pair was set.
 */
bool ifcopenshell_value_dict_set(ifcopenshell_value_t* dict, const char* key, ifcopenshell_value_t* value, bool* out_result);
/**
 * Return the number of entries in a dictionary value.
 *
 * @param value The value handle.
 * @return Number of entries, or 0 if null or not a dict.
 */
bool ifcopenshell_value_dict_size(ifcopenshell_value_t* value, size_t* out_result);
/**
 * Return the value at the given index in a dictionary value.
 *
 * @param value The dict value handle.
 * @param index Zero-based entry index.
 * @return The value at the index, or null if out of range or not a dict.
 */
bool ifcopenshell_value_dict_value_at(ifcopenshell_value_t* value, size_t index, ifcopenshell_value_t** out_result);
/**
 * Return the kind of a selector value.
 *
 * @param value The value handle.
 * @return One of the ifcopenshell_selector_value_kind_t values, or IFCSEL_VALUE_NONE if null.
 */
bool ifcopenshell_value_kind(ifcopenshell_value_t* value, int32_t* out_result);
/**
 * Append an item to a list value.
 *
 * @param list The list value handle.
 * @param item The item to append. If empty, appends a None value.
 * @return True if the item was appended.
 */
bool ifcopenshell_value_list_append(ifcopenshell_value_t* list, ifcopenshell_value_t* item, bool* out_result);
/**
 * Return an item from a list value at the given index.
 *
 * @param value The list value handle.
 * @param index Zero-based item index.
 * @return The item at the index, or null if out of range or not a list.
 */
bool ifcopenshell_value_list_at(ifcopenshell_value_t* value, size_t index, ifcopenshell_value_t** out_result);
/**
 * Return the number of items in a list value.
 *
 * @param value The value handle.
 * @return Number of items, or 0 if null or not a list.
 */
bool ifcopenshell_value_list_size(ifcopenshell_value_t* value, size_t* out_result);
/**
 * Create a boolean value.
 *
 * @param value The boolean value.
 * @return New boolean value handle. Free with value_free.
 */
bool ifcopenshell_value_new_bool(bool value, ifcopenshell_value_t** out_result);
/**
 * Create an empty dictionary value.
 *
 * @return New dict value handle. Free with value_free.
 */
bool ifcopenshell_value_new_dict(ifcopenshell_value_t** out_result);
/**
 * Create a double-precision floating-point value.
 *
 * @param value The double value.
 * @return New double value handle. Free with value_free.
 */
bool ifcopenshell_value_new_double(double value, ifcopenshell_value_t** out_result);
/**
 * Create an instance (entity reference) value.
 *
 * If value is empty, creates a None value.
 *
 * @param value The IFC entity instance.
 * @return New instance value handle. Free with value_free.
 */
bool ifcopenshell_value_new_instance(ifcopenshell_instance_t* value, ifcopenshell_value_t** out_result);
/**
 * Create a 64-bit integer value.
 *
 * @param value The integer value.
 * @return New integer value handle. Free with value_free.
 */
bool ifcopenshell_value_new_int(int64_t value, ifcopenshell_value_t** out_result);
/**
 * Create an empty list value.
 *
 * @return New list value handle. Free with value_free.
 */
bool ifcopenshell_value_new_list(ifcopenshell_value_t** out_result);
/**
 * Create a None value.
 *
 * @return New None value handle. Free with value_free.
 */
bool ifcopenshell_value_new_none(ifcopenshell_value_t** out_result);
/**
 * Create a string value.
 *
 * @param value The string value.
 * @return New string value handle. Free with value_free.
 */
bool ifcopenshell_value_new_string(const char* value, ifcopenshell_value_t** out_result);
bool ifcopenshell_geom_arrange_polygons(const ifcopenshell_geom_svgfill_polygon_list_t* polygons_cpp, ifcopenshell_geom_svgfill_polygon_list_t* out_result);
bool ifcopenshell_geom_convert_loop_to_function_item(ifcopenshell_geom_taxonomy_item_t* loop_item_cpp, ifcopenshell_geom_taxonomy_item_t** out_result);
bool ifcopenshell_geom_create_epeck_from_double(double value, ifcopenshell_geom_opaque_number_t** out_result);
bool ifcopenshell_geom_create_epeck_from_int(int32_t value, ifcopenshell_geom_opaque_number_t** out_result);
bool ifcopenshell_geom_create_epeck_from_string(const char* value_cpp, ifcopenshell_geom_opaque_number_t** out_result);
bool ifcopenshell_geom_create_function_item_evaluator(ifcopenshell_geom_settings_t* settings_cpp, ifcopenshell_geom_taxonomy_item_t* fn_item_cpp, ifcopenshell_geom_function_item_evaluator_t** out_result);
bool ifcopenshell_geom_create_geometry_serializer_by_path(const char* format, const char* output_filename, const char* output_temp_filename, ifcopenshell_geom_settings_t* geometry_settings, ifcopenshell_geom_serializer_settings_t* serializer_settings, ifcopenshell_geom_geometry_serializer_t** out_result);
bool ifcopenshell_geom_create_geometry_serializer_by_stream(const char* format, ifcopenshell_geom_buffer_t* output, ifcopenshell_geom_buffer_t* output_temp, ifcopenshell_geom_settings_t* geometry_settings, ifcopenshell_geom_serializer_settings_t* serializer_settings, ifcopenshell_geom_geometry_serializer_t** out_result);
bool ifcopenshell_geom_create_iterator(const char* geometry_library_cpp, ifcopenshell_geom_settings_t* settings_cpp, ifcopenshell_file_t* file_cpp, int32_t num_threads, ifcopenshell_geom_iterator_t** out_result);
bool ifcopenshell_geom_create_iterator_with_include_exclude(const char* geometry_library_cpp, ifcopenshell_geom_settings_t* settings_cpp, ifcopenshell_file_t* file_cpp, const ifcopenshell_string_list_t* elems_cpp, bool include, int32_t num_threads, ifcopenshell_geom_iterator_t** out_result);
bool ifcopenshell_geom_create_iterator_with_include_exclude_globalid(const char* geometry_library_cpp, ifcopenshell_geom_settings_t* settings_cpp, ifcopenshell_file_t* file_cpp, const ifcopenshell_string_list_t* elems_cpp, bool include, int32_t num_threads, ifcopenshell_geom_iterator_t** out_result);
bool ifcopenshell_geom_create_iterator_with_include_exclude_id(const char* geometry_library_cpp, ifcopenshell_geom_settings_t* settings_cpp, ifcopenshell_file_t* file_cpp, const ifcopenshell_int32_list_t* elems_cpp, bool include, int32_t num_threads, ifcopenshell_geom_iterator_t** out_result);
bool ifcopenshell_geom_create_shape(ifcopenshell_geom_settings_t* settings_cpp, ifcopenshell_instance_t* instance_cpp, ifcopenshell_instance_t* representation, const char* geometry_library, ifcopenshell_geom_element_t** out_result);
bool ifcopenshell_geom_line_segments_to_polygons(int32_t solver, double eps, const char* segments_json_cpp, ifcopenshell_geom_svgfill_polygon_list_t* out_result);
bool ifcopenshell_geom_map_shape(ifcopenshell_geom_settings_t* settings_cpp, ifcopenshell_instance_t* instance_cpp, ifcopenshell_geom_taxonomy_item_t** out_result);
bool ifcopenshell_geom_nary_union(const ifcopenshell_geom_conversion_result_shape_list_t* shapes_cpp, ifcopenshell_geom_conversion_result_shape_t** out_result);
bool ifcopenshell_geom_plugin_is_loaded(const char* kind, const char* id, bool* out_result);
bool ifcopenshell_geom_plugin_load(const char* kind, const char* id, bool* out_result);
bool ifcopenshell_geom_svg_to_line_segments(const char* svg_data_cpp, const char* class_name, ifcopenshell_string_t* out_result);
bool ifcopenshell_geom_svg_to_polygons(const char* svg_data_cpp, const char* class_name, ifcopenshell_geom_svgfill_polygon_list_t* out_result);
bool ifcopenshell_geom_taxonomy_create_boolean_result(int32_t operation, ifcopenshell_geom_taxonomy_boolean_result_t** out_result);
bool ifcopenshell_geom_taxonomy_create_box(double dx, double dy, double dz, ifcopenshell_geom_taxonomy_solid_t** out_result);
bool ifcopenshell_geom_taxonomy_create_bspline_curve(int32_t degree, ifcopenshell_geom_taxonomy_bspline_curve_t** out_result);
bool ifcopenshell_geom_taxonomy_create_bspline_surface(int32_t degree_u, int32_t degree_v, ifcopenshell_geom_taxonomy_bspline_surface_t** out_result);
bool ifcopenshell_geom_taxonomy_create_circle(double origin_x, double origin_y, double origin_z, double dir_x, double dir_y, double dir_z, double radius, ifcopenshell_geom_taxonomy_circle_t** out_result);
bool ifcopenshell_geom_taxonomy_create_collection(ifcopenshell_geom_taxonomy_collection_t** out_result);
bool ifcopenshell_geom_taxonomy_create_cylinder(double origin_x, double origin_y, double origin_z, double dir_x, double dir_y, double dir_z, double radius, ifcopenshell_geom_taxonomy_cylinder_t** out_result);
bool ifcopenshell_geom_taxonomy_create_direction3(double x, double y, double z, ifcopenshell_geom_taxonomy_direction3_t** out_result);
bool ifcopenshell_geom_taxonomy_create_ellipse(double origin_x, double origin_y, double origin_z, double dir_x, double dir_y, double dir_z, double radius1, double radius2, ifcopenshell_geom_taxonomy_ellipse_t** out_result);
bool ifcopenshell_geom_taxonomy_create_extrusion(ifcopenshell_geom_taxonomy_item_t* basis_cpp, ifcopenshell_geom_taxonomy_direction3_t* direction_cpp, double depth, ifcopenshell_geom_taxonomy_extrusion_t** out_result);
bool ifcopenshell_geom_taxonomy_create_line(double origin_x, double origin_y, double origin_z, double dir_x, double dir_y, double dir_z, ifcopenshell_geom_taxonomy_line_t** out_result);
bool ifcopenshell_geom_taxonomy_create_loft(ifcopenshell_geom_taxonomy_loft_t** out_result);
bool ifcopenshell_geom_taxonomy_create_node(ifcopenshell_geom_taxonomy_node_t** out_result);
bool ifcopenshell_geom_taxonomy_create_offset_curve(ifcopenshell_geom_taxonomy_item_t* basis, ifcopenshell_geom_taxonomy_direction3_t* reference, double offset, ifcopenshell_geom_taxonomy_offset_curve_t** out_result);
bool ifcopenshell_geom_taxonomy_create_plane(double origin_x, double origin_y, double origin_z, double dir_x, double dir_y, double dir_z, ifcopenshell_geom_taxonomy_plane_t** out_result);
bool ifcopenshell_geom_taxonomy_create_point3(double x, double y, double z, ifcopenshell_geom_taxonomy_point3_t** out_result);
bool ifcopenshell_geom_taxonomy_create_revolve(ifcopenshell_geom_taxonomy_item_t* basis_cpp, ifcopenshell_geom_taxonomy_point3_t* axis_origin_cpp, ifcopenshell_geom_taxonomy_direction3_t* direction_cpp, double angle, ifcopenshell_geom_taxonomy_revolve_t** out_result);
bool ifcopenshell_geom_taxonomy_create_sphere(double origin_x, double origin_y, double origin_z, double dir_x, double dir_y, double dir_z, double radius, ifcopenshell_geom_taxonomy_sphere_t** out_result);
bool ifcopenshell_geom_taxonomy_create_sweep_along_curve(ifcopenshell_geom_taxonomy_face_t* basis_face_cpp, ifcopenshell_geom_taxonomy_item_t* directrix_cpp, ifcopenshell_geom_taxonomy_direction3_t* reference_direction_cpp, ifcopenshell_geom_taxonomy_sweep_along_curve_t** out_result);
bool ifcopenshell_geom_taxonomy_create_torus(double origin_x, double origin_y, double origin_z, double dir_x, double dir_y, double dir_z, double radius1, double radius2, ifcopenshell_geom_taxonomy_torus_t** out_result);
bool ifcopenshell_geom_taxonomy_function_item_end(ifcopenshell_geom_taxonomy_item_t* item_cpp, double* out_result);
bool ifcopenshell_geom_taxonomy_function_item_start(ifcopenshell_geom_taxonomy_item_t* item_cpp, double* out_result);
bool ifcopenshell_file_create(ifcopenshell_file_t* self, ifcopenshell_declaration_t* declaration, int32_t instance_id, ifcopenshell_instance_t** out_result);
bool ifcopenshell_file_get_inverses_by_declaration(ifcopenshell_file_t* self, int32_t instance_id, ifcopenshell_declaration_t* declaration, int32_t attribute_index, ifcopenshell_instance_list_t* out_result);
bool ifcopenshell_file_by_type(ifcopenshell_file_t* self, const char* type_name, ifcopenshell_parse_instance_list_t** out_result);
bool ifcopenshell_file_by_type_excl_subtypes(ifcopenshell_file_t* self, const char* type_name, ifcopenshell_parse_instance_list_t** out_result);
bool ifcopenshell_file_add(ifcopenshell_file_t* self, ifcopenshell_instance_t* entity, int32_t instance_id, ifcopenshell_instance_t** out_result);
bool ifcopenshell_file_add_type_ref(ifcopenshell_file_t* self, ifcopenshell_instance_t* new_entity);
bool ifcopenshell_file_batch(ifcopenshell_file_t* self);
bool ifcopenshell_file_build_inverses(ifcopenshell_file_t* self);
bool ifcopenshell_file_build_inverses_(ifcopenshell_file_t* self, ifcopenshell_instance_t* entity);
bool ifcopenshell_file_bypass_type(ifcopenshell_file_t* self, const char* type_name);
bool ifcopenshell_file_create_timestamp(ifcopenshell_file_t* self, ifcopenshell_string_t* out_result);
bool ifcopenshell_file_fresh_id(ifcopenshell_file_t* self, uint32_t* out_result);
bool ifcopenshell_file_get_inverse_indices_by_id(ifcopenshell_file_t* self, int32_t instance_id, ifcopenshell_int32_list_t* out_result);
bool ifcopenshell_file_get_max_id(ifcopenshell_file_t* self, uint32_t* out_result);
bool ifcopenshell_file_get_total_inverses_by_id(ifcopenshell_file_t* self, int32_t instance_id, size_t* out_result);
bool ifcopenshell_file_ifcroot_type(ifcopenshell_file_t* self, ifcopenshell_declaration_t** out_result);
bool ifcopenshell_file_initialize(ifcopenshell_file_t* self, const char* path, int32_t type, bool read_only, bool* out_result);
bool ifcopenshell_file_by_guid(ifcopenshell_file_t* self, const char* global_id, ifcopenshell_instance_t** out_result);
bool ifcopenshell_file_by_id(ifcopenshell_file_t* self, int32_t instance_id, ifcopenshell_instance_t** out_result);
bool ifcopenshell_file_instances_by_reference(ifcopenshell_file_t* self, int32_t reference_id, ifcopenshell_parse_instance_list_t** out_result);
bool ifcopenshell_file_process_deletion_inverse(ifcopenshell_file_t* self, ifcopenshell_instance_t* entity);
bool ifcopenshell_file_recalculate_id_counter(ifcopenshell_file_t* self);
bool ifcopenshell_file_remove(ifcopenshell_file_t* self, ifcopenshell_instance_t* entity);
bool ifcopenshell_file_remove_type_ref(ifcopenshell_file_t* self, ifcopenshell_instance_t* new_entity);
bool ifcopenshell_file_reset_identity_cache(ifcopenshell_file_t* self);
bool ifcopenshell_file_schema(ifcopenshell_file_t* self, ifcopenshell_schema_t** out_result);
bool ifcopenshell_file_traverse(ifcopenshell_file_t* self, ifcopenshell_instance_t* instance, int32_t max_depth, ifcopenshell_parse_instance_list_t** out_result);
bool ifcopenshell_file_traverse_breadth_first(ifcopenshell_file_t* self, ifcopenshell_instance_t* instance, int32_t max_depth, ifcopenshell_parse_instance_list_t** out_result);
bool ifcopenshell_file_unbatch(ifcopenshell_file_t* self);
bool ifcopenshell_instance_declaration(ifcopenshell_instance_t* self, ifcopenshell_declaration_t** out_result);
bool ifcopenshell_instance_file(ifcopenshell_instance_t* self, ifcopenshell_file_t** out_result);
bool ifcopenshell_instance_get_argument(ifcopenshell_instance_t* self, size_t attribute_index, ifcopenshell_parse_attribute_value_t** out_result);
bool ifcopenshell_instance_id(ifcopenshell_instance_t* self, uint32_t* out_result);
bool ifcopenshell_instance_identity(ifcopenshell_instance_t* self, uint32_t* out_result);
bool ifcopenshell_schema_declaration_by_name(ifcopenshell_schema_t* self, const char* name, ifcopenshell_declaration_t** out_result);
bool ifcopenshell_schema_declaration_by_index(ifcopenshell_schema_t* self, size_t declaration_index, ifcopenshell_declaration_t** out_result);
bool ifcopenshell_schema_declarations(ifcopenshell_schema_t* self, ifcopenshell_declaration_list_t* out_result);
bool ifcopenshell_schema_entities(ifcopenshell_schema_t* self, ifcopenshell_entity_list_t* out_result);
bool ifcopenshell_schema_enumeration_types(ifcopenshell_schema_t* self, ifcopenshell_enumeration_list_t* out_result);
bool ifcopenshell_schema_name(ifcopenshell_schema_t* self, ifcopenshell_string_t* out_result);
bool ifcopenshell_schema_select_types(ifcopenshell_schema_t* self, ifcopenshell_select_type_list_t* out_result);
bool ifcopenshell_schema_type_declarations(ifcopenshell_schema_t* self, ifcopenshell_type_declaration_list_t* out_result);
bool ifcopenshell_declaration_is_a(ifcopenshell_declaration_t* self, const char* name, bool* out_result);
bool ifcopenshell_declaration_as_entity(ifcopenshell_declaration_t* self, ifcopenshell_entity_t** out_result);
bool ifcopenshell_declaration_as_enumeration_type(ifcopenshell_declaration_t* self, ifcopenshell_enumeration_t** out_result);
bool ifcopenshell_declaration_as_select_type(ifcopenshell_declaration_t* self, ifcopenshell_select_type_t** out_result);
bool ifcopenshell_declaration_as_type_declaration(ifcopenshell_declaration_t* self, ifcopenshell_type_declaration_t** out_result);
bool ifcopenshell_declaration_index_in_schema(ifcopenshell_declaration_t* self, int32_t* out_result);
bool ifcopenshell_declaration_name(ifcopenshell_declaration_t* self, ifcopenshell_string_t* out_result);
bool ifcopenshell_declaration_name_uc(ifcopenshell_declaration_t* self, ifcopenshell_string_t* out_result);
bool ifcopenshell_declaration_schema(ifcopenshell_declaration_t* self, ifcopenshell_schema_t** out_result);
bool ifcopenshell_declaration_type(ifcopenshell_declaration_t* self, int32_t* out_result);
bool ifcopenshell_type_declaration_as_type_declaration(ifcopenshell_type_declaration_t* self, ifcopenshell_type_declaration_t** out_result);
bool ifcopenshell_type_declaration_declared_type(ifcopenshell_type_declaration_t* self, ifcopenshell_parameter_type_t** out_result);
bool ifcopenshell_select_type_as_select_type(ifcopenshell_select_type_t* self, ifcopenshell_select_type_t** out_result);
bool ifcopenshell_select_type_select_list(ifcopenshell_select_type_t* self, ifcopenshell_declaration_list_t* out_result);
bool ifcopenshell_enumeration_as_enumeration_type(ifcopenshell_enumeration_t* self, ifcopenshell_enumeration_t** out_result);
bool ifcopenshell_enumeration_enumeration_items(ifcopenshell_enumeration_t* self, ifcopenshell_string_list_t* out_result);
bool ifcopenshell_enumeration_lookup_enum_offset(ifcopenshell_enumeration_t* self, const char* value_name, size_t* out_result);
bool ifcopenshell_enumeration_lookup_enum_value(ifcopenshell_enumeration_t* self, size_t i, ifcopenshell_string_t* out_result);
bool ifcopenshell_parameter_type_as_aggregation_type(ifcopenshell_parameter_type_t* self, ifcopenshell_aggregation_type_t** out_result);
bool ifcopenshell_parameter_type_as_named_type(ifcopenshell_parameter_type_t* self, ifcopenshell_named_type_t** out_result);
bool ifcopenshell_parameter_type_as_simple_type(ifcopenshell_parameter_type_t* self, ifcopenshell_simple_type_t** out_result);
bool ifcopenshell_named_type_is_a(ifcopenshell_named_type_t* self, const char* name, bool* out_result);
bool ifcopenshell_named_type_as_named_type(ifcopenshell_named_type_t* self, ifcopenshell_named_type_t** out_result);
bool ifcopenshell_named_type_declared_type(ifcopenshell_named_type_t* self, ifcopenshell_declaration_t** out_result);
bool ifcopenshell_simple_type_as_simple_type(ifcopenshell_simple_type_t* self, ifcopenshell_simple_type_t** out_result);
bool ifcopenshell_simple_type_declared_type(ifcopenshell_simple_type_t* self, int32_t* out_result);
bool ifcopenshell_aggregation_type_as_aggregation_type(ifcopenshell_aggregation_type_t* self, ifcopenshell_aggregation_type_t** out_result);
bool ifcopenshell_aggregation_type_bound1(ifcopenshell_aggregation_type_t* self, int32_t* out_result);
bool ifcopenshell_aggregation_type_bound2(ifcopenshell_aggregation_type_t* self, int32_t* out_result);
bool ifcopenshell_aggregation_type_type_of_element(ifcopenshell_aggregation_type_t* self, ifcopenshell_parameter_type_t** out_result);
bool ifcopenshell_header_file(ifcopenshell_header_t* self, ifcopenshell_file_t** out_result);
bool ifcopenshell_header_file_description(ifcopenshell_header_t* self, ifcopenshell_file_description_t** out_result);
bool ifcopenshell_header_file_name(ifcopenshell_header_t* self, ifcopenshell_file_name_t** out_result);
bool ifcopenshell_header_file_schema(ifcopenshell_header_t* self, ifcopenshell_file_schema_t** out_result);
bool ifcopenshell_file_description_class(ifcopenshell_file_description_t* self, ifcopenshell_entity_t** out_result);
bool ifcopenshell_file_description_description(ifcopenshell_file_description_t* self, ifcopenshell_string_list_t* out_result);
bool ifcopenshell_file_description_implementation_level(ifcopenshell_file_description_t* self, ifcopenshell_string_t* out_result);
bool ifcopenshell_file_description_initialize(ifcopenshell_file_description_t* self, const ifcopenshell_string_list_t* v1_description, const char* v2_implementation_level, ifcopenshell_file_description_t** out_result);
bool ifcopenshell_file_description_setdescription(ifcopenshell_file_description_t* self, const ifcopenshell_string_list_t* v);
bool ifcopenshell_file_description_setimplementation_level(ifcopenshell_file_description_t* self, const char* v);
bool ifcopenshell_file_name_class(ifcopenshell_file_name_t* self, ifcopenshell_entity_t** out_result);
bool ifcopenshell_file_name_author(ifcopenshell_file_name_t* self, ifcopenshell_string_list_t* out_result);
bool ifcopenshell_file_name_authorization(ifcopenshell_file_name_t* self, ifcopenshell_string_t* out_result);
bool ifcopenshell_file_name_initialize(ifcopenshell_file_name_t* self, const char* v1_name, const char* v2_time_stamp, const ifcopenshell_string_list_t* v3_author, const ifcopenshell_string_list_t* v4_organization, const char* v5_preprocessor_version, const char* v6_originating_system, const char* v7_authorization, ifcopenshell_file_name_t** out_result);
bool ifcopenshell_file_name_name(ifcopenshell_file_name_t* self, ifcopenshell_string_t* out_result);
bool ifcopenshell_file_name_organization(ifcopenshell_file_name_t* self, ifcopenshell_string_list_t* out_result);
bool ifcopenshell_file_name_originating_system(ifcopenshell_file_name_t* self, ifcopenshell_string_t* out_result);
bool ifcopenshell_file_name_preprocessor_version(ifcopenshell_file_name_t* self, ifcopenshell_string_t* out_result);
bool ifcopenshell_file_name_setauthor(ifcopenshell_file_name_t* self, const ifcopenshell_string_list_t* v);
bool ifcopenshell_file_name_setauthorization(ifcopenshell_file_name_t* self, const char* v);
bool ifcopenshell_file_name_setname(ifcopenshell_file_name_t* self, const char* v);
bool ifcopenshell_file_name_setorganization(ifcopenshell_file_name_t* self, const ifcopenshell_string_list_t* v);
bool ifcopenshell_file_name_setoriginating_system(ifcopenshell_file_name_t* self, const char* v);
bool ifcopenshell_file_name_setpreprocessor_version(ifcopenshell_file_name_t* self, const char* v);
bool ifcopenshell_file_name_settime_stamp(ifcopenshell_file_name_t* self, const char* v);
bool ifcopenshell_file_name_time_stamp(ifcopenshell_file_name_t* self, ifcopenshell_string_t* out_result);
bool ifcopenshell_file_schema_class(ifcopenshell_file_schema_t* self, ifcopenshell_entity_t** out_result);
bool ifcopenshell_file_schema_initialize(ifcopenshell_file_schema_t* self, const ifcopenshell_string_list_t* v1_schema_identifiers, ifcopenshell_file_schema_t** out_result);
bool ifcopenshell_file_schema_schema_identifiers(ifcopenshell_file_schema_t* self, ifcopenshell_string_list_t* out_result);
bool ifcopenshell_file_schema_setschema_identifiers(ifcopenshell_file_schema_t* self, const ifcopenshell_string_list_t* v);
bool ifcopenshell_entity_attribute_index(ifcopenshell_entity_t* self, const char* attr_name, int32_t* out_result);
bool ifcopenshell_entity_all_attributes(ifcopenshell_entity_t* self, ifcopenshell_attribute_list_t* out_result);
bool ifcopenshell_entity_all_inverse_attributes(ifcopenshell_entity_t* self, ifcopenshell_inverse_attribute_list_t* out_result);
bool ifcopenshell_entity_as_entity(ifcopenshell_entity_t* self, ifcopenshell_entity_t** out_result);
bool ifcopenshell_entity_attribute_by_index(ifcopenshell_entity_t* self, size_t index, ifcopenshell_attribute_t** out_result);
bool ifcopenshell_entity_attribute_count(ifcopenshell_entity_t* self, size_t* out_result);
bool ifcopenshell_entity_attributes(ifcopenshell_entity_t* self, ifcopenshell_attribute_list_t* out_result);
bool ifcopenshell_entity_derived(ifcopenshell_entity_t* self, ifcopenshell_bool_list_t* out_result);
bool ifcopenshell_entity_inverse_attributes(ifcopenshell_entity_t* self, ifcopenshell_inverse_attribute_list_t* out_result);
bool ifcopenshell_entity_is_abstract(ifcopenshell_entity_t* self, bool* out_result);
bool ifcopenshell_entity_set_attributes(ifcopenshell_entity_t* self, const ifcopenshell_attribute_list_t* attributes, const ifcopenshell_bool_list_t* derived);
bool ifcopenshell_entity_set_inverse_attributes(ifcopenshell_entity_t* self, const ifcopenshell_inverse_attribute_list_t* inverse_attributes);
bool ifcopenshell_entity_set_subtypes(ifcopenshell_entity_t* self, const ifcopenshell_entity_list_t* subtypes);
bool ifcopenshell_entity_subtypes(ifcopenshell_entity_t* self, ifcopenshell_entity_list_t* out_result);
bool ifcopenshell_entity_supertype(ifcopenshell_entity_t* self, ifcopenshell_entity_t** out_result);
bool ifcopenshell_attribute_name(ifcopenshell_attribute_t* self, ifcopenshell_string_t* out_result);
bool ifcopenshell_attribute_optional(ifcopenshell_attribute_t* self, bool* out_result);
bool ifcopenshell_attribute_type_of_attribute(ifcopenshell_attribute_t* self, ifcopenshell_parameter_type_t** out_result);
bool ifcopenshell_inverse_attribute_attribute_reference(ifcopenshell_inverse_attribute_t* self, ifcopenshell_attribute_t** out_result);
bool ifcopenshell_inverse_attribute_bound1(ifcopenshell_inverse_attribute_t* self, int32_t* out_result);
bool ifcopenshell_inverse_attribute_bound2(ifcopenshell_inverse_attribute_t* self, int32_t* out_result);
bool ifcopenshell_inverse_attribute_entity_reference(ifcopenshell_inverse_attribute_t* self, ifcopenshell_entity_t** out_result);
bool ifcopenshell_inverse_attribute_name(ifcopenshell_inverse_attribute_t* self, ifcopenshell_string_t* out_result);
bool ifcopenshell_instance_streamer_bypassed_instances(ifcopenshell_instance_streamer_t* self, ifcopenshell_uint32_list_t* out_result);
bool ifcopenshell_instance_streamer_has_semicolon(ifcopenshell_instance_streamer_t* self, bool* out_result);
bool ifcopenshell_instance_streamer_push_page(ifcopenshell_instance_streamer_t* self, const char* page_data);
bool ifcopenshell_instance_streamer_semicolon_count(ifcopenshell_instance_streamer_t* self, size_t* out_result);
bool ifcopenshell_geom_triangulation_edges(ifcopenshell_geom_triangulation_t* self, ifcopenshell_int32_list_t* out_result);
bool ifcopenshell_geom_triangulation_edges_item_ids(ifcopenshell_geom_triangulation_t* self, ifcopenshell_int32_list_t* out_result);
bool ifcopenshell_geom_triangulation_faces(ifcopenshell_geom_triangulation_t* self, ifcopenshell_int32_list_t* out_result);
bool ifcopenshell_geom_triangulation_item_ids(ifcopenshell_geom_triangulation_t* self, ifcopenshell_int32_list_t* out_result);
bool ifcopenshell_geom_triangulation_material_ids(ifcopenshell_geom_triangulation_t* self, ifcopenshell_int32_list_t* out_result);
bool ifcopenshell_geom_triangulation_materials(ifcopenshell_geom_triangulation_t* self, ifcopenshell_geom_taxonomy_style_list_t* out_result);
bool ifcopenshell_geom_triangulation_normals(ifcopenshell_geom_triangulation_t* self, ifcopenshell_double_list_t* out_result);
bool ifcopenshell_geom_triangulation_polyhedral_faces_with_holes(ifcopenshell_geom_triangulation_t* self, ifcopenshell_int32_list_list_list_t* out_result);
bool ifcopenshell_geom_triangulation_polyhedral_faces_without_holes(ifcopenshell_geom_triangulation_t* self, ifcopenshell_int32_list_list_t* out_result);
bool ifcopenshell_geom_triangulation_uvs(ifcopenshell_geom_triangulation_t* self, ifcopenshell_double_list_t* out_result);
bool ifcopenshell_geom_triangulation_verts(ifcopenshell_geom_triangulation_t* self, ifcopenshell_double_list_t* out_result);
bool ifcopenshell_geom_triangulation_verts_buffer_size(ifcopenshell_geom_triangulation_t* self, size_t* out_result);
bool ifcopenshell_geom_triangulation_faces_buffer_size(ifcopenshell_geom_triangulation_t* self, size_t* out_result);
bool ifcopenshell_geom_triangulation_normals_buffer_size(ifcopenshell_geom_triangulation_t* self, size_t* out_result);
bool ifcopenshell_geom_triangulation_edges_buffer_size(ifcopenshell_geom_triangulation_t* self, size_t* out_result);
bool ifcopenshell_geom_triangulation_material_ids_buffer_size(ifcopenshell_geom_triangulation_t* self, size_t* out_result);
bool ifcopenshell_geom_triangulation_item_ids_buffer_size(ifcopenshell_geom_triangulation_t* self, size_t* out_result);
bool ifcopenshell_geom_triangulation_edges_item_ids_buffer_size(ifcopenshell_geom_triangulation_t* self, size_t* out_result);
bool ifcopenshell_geom_triangulation_uvs_buffer_size(ifcopenshell_geom_triangulation_t* self, size_t* out_result);
bool ifcopenshell_geom_triangulation_material_count(ifcopenshell_geom_triangulation_t* self, size_t* out_result);
bool ifcopenshell_geom_triangulation_material_at(ifcopenshell_geom_triangulation_t* self, size_t index, ifcopenshell_geom_taxonomy_style_t** out_result);
bool ifcopenshell_geom_iterator_bounds_max(ifcopenshell_geom_iterator_t* self, ifcopenshell_geom_taxonomy_point3_t** out_result);
bool ifcopenshell_geom_iterator_bounds_min(ifcopenshell_geom_iterator_t* self, ifcopenshell_geom_taxonomy_point3_t** out_result);
bool ifcopenshell_geom_iterator_compute_bounds(ifcopenshell_geom_iterator_t* self, bool with_geometry);
bool ifcopenshell_geom_iterator_create(ifcopenshell_geom_iterator_t* self, ifcopenshell_instance_t** out_result);
bool ifcopenshell_geom_iterator_file(ifcopenshell_geom_iterator_t* self, ifcopenshell_file_t** out_result);
bool ifcopenshell_geom_iterator_get(ifcopenshell_geom_iterator_t* self, ifcopenshell_geom_element_t** out_result);
bool ifcopenshell_geom_iterator_get_log(ifcopenshell_geom_iterator_t* self, ifcopenshell_string_t* out_result);
bool ifcopenshell_geom_iterator_get_native(ifcopenshell_geom_iterator_t* self, ifcopenshell_geom_brep_element_t** out_result);
bool ifcopenshell_geom_iterator_get_object(ifcopenshell_geom_iterator_t* self, int32_t id, ifcopenshell_geom_element_t** out_result);
bool ifcopenshell_geom_iterator_get_task_items(ifcopenshell_geom_iterator_t* self, ifcopenshell_geom_taxonomy_item_list_t* out_result);
bool ifcopenshell_geom_iterator_get_task_products(ifcopenshell_geom_iterator_t* self, ifcopenshell_instance_list_list_t* out_result);
bool ifcopenshell_geom_iterator_had_error_processing_elements(ifcopenshell_geom_iterator_t* self, bool* out_result);
bool ifcopenshell_geom_iterator_initialize(ifcopenshell_geom_iterator_t* self, bool* out_result);
bool ifcopenshell_geom_iterator_progress(ifcopenshell_geom_iterator_t* self, int32_t* out_result);
bool ifcopenshell_geom_iterator_unit_magnitude(ifcopenshell_geom_iterator_t* self, double* out_result);
bool ifcopenshell_geom_iterator_unit_name(ifcopenshell_geom_iterator_t* self, ifcopenshell_string_t* out_result);
bool ifcopenshell_geom_brep_representation_calculate_projected_surface_area(ifcopenshell_geom_brep_representation_t* self, ifcopenshell_geom_taxonomy_matrix4_t* ax, double along_x, double along_y, double along_z, bool* out_result);
bool ifcopenshell_geom_brep_representation_calculate_surface_area(ifcopenshell_geom_brep_representation_t* self, double* out_result);
bool ifcopenshell_geom_brep_representation_calculate_volume(ifcopenshell_geom_brep_representation_t* self, double* out_result);
bool ifcopenshell_geom_brep_representation_entity(ifcopenshell_geom_brep_representation_t* self, ifcopenshell_string_t* out_result);
bool ifcopenshell_geom_brep_representation_id(ifcopenshell_geom_brep_representation_t* self, ifcopenshell_string_t* out_result);
bool ifcopenshell_geom_brep_representation_item(ifcopenshell_geom_brep_representation_t* self, int32_t i, ifcopenshell_geom_conversion_result_shape_t** out_result);
bool ifcopenshell_geom_brep_representation_item_id(ifcopenshell_geom_brep_representation_t* self, int32_t i, int32_t* out_result);
bool ifcopenshell_geom_brep_representation_settings(ifcopenshell_geom_brep_representation_t* self, ifcopenshell_geom_settings_t** out_result);
bool ifcopenshell_geom_brep_representation_size(ifcopenshell_geom_brep_representation_t* self, int32_t* out_result);
bool ifcopenshell_geom_element_context(ifcopenshell_geom_element_t* self, ifcopenshell_string_t* out_result);
bool ifcopenshell_geom_element_guid(ifcopenshell_geom_element_t* self, ifcopenshell_string_t* out_result);
bool ifcopenshell_geom_element_id(ifcopenshell_geom_element_t* self, int32_t* out_result);
bool ifcopenshell_geom_element_name(ifcopenshell_geom_element_t* self, ifcopenshell_string_t* out_result);
bool ifcopenshell_geom_element_parent_id(ifcopenshell_geom_element_t* self, int32_t* out_result);
bool ifcopenshell_geom_element_parents(ifcopenshell_geom_element_t* self, ifcopenshell_geom_element_list_t* out_result);
bool ifcopenshell_geom_element_product(ifcopenshell_geom_element_t* self, ifcopenshell_instance_t** out_result);
bool ifcopenshell_geom_element_transformation(ifcopenshell_geom_element_t* self, ifcopenshell_geom_transformation_t** out_result);
bool ifcopenshell_geom_element_type(ifcopenshell_geom_element_t* self, ifcopenshell_string_t* out_result);
bool ifcopenshell_geom_element_unique_id(ifcopenshell_geom_element_t* self, ifcopenshell_string_t* out_result);
bool ifcopenshell_geom_brep_element_calculate_projected_surface_area(ifcopenshell_geom_brep_element_t* self, double along_x, double along_y, double along_z, bool* out_result);
bool ifcopenshell_geom_brep_element_geometry(ifcopenshell_geom_brep_element_t* self, ifcopenshell_geom_brep_representation_t** out_result);
bool ifcopenshell_geom_triangulation_element_geometry(ifcopenshell_geom_triangulation_element_t* self, ifcopenshell_geom_triangulation_t** out_result);
bool ifcopenshell_geom_serialized_element_geometry(ifcopenshell_geom_serialized_element_t* self, ifcopenshell_geom_serialization_t** out_result);
bool ifcopenshell_geom_conversion_result_shape_solid_mt(ifcopenshell_geom_conversion_result_shape_t* self, ifcopenshell_geom_conversion_result_shape_t** out_result);
bool ifcopenshell_geom_conversion_result_shape_add(ifcopenshell_geom_conversion_result_shape_t* self, ifcopenshell_geom_conversion_result_shape_t* arg_0, ifcopenshell_geom_conversion_result_shape_t** out_result);
bool ifcopenshell_geom_conversion_result_shape_box(ifcopenshell_geom_conversion_result_shape_t* self, ifcopenshell_geom_conversion_result_shape_t** out_result);
bool ifcopenshell_geom_conversion_result_shape_concat(ifcopenshell_geom_conversion_result_shape_t* self, ifcopenshell_geom_conversion_result_shape_t* arg_0, ifcopenshell_geom_conversion_result_shape_t** out_result);
bool ifcopenshell_geom_conversion_result_shape_edges(ifcopenshell_geom_conversion_result_shape_t* self, ifcopenshell_geom_conversion_result_shape_list_t* out_result);
bool ifcopenshell_geom_conversion_result_shape_facets(ifcopenshell_geom_conversion_result_shape_t* self, ifcopenshell_geom_conversion_result_shape_list_t* out_result);
bool ifcopenshell_geom_conversion_result_shape_halfspaces(ifcopenshell_geom_conversion_result_shape_t* self, ifcopenshell_geom_conversion_result_shape_t** out_result);
bool ifcopenshell_geom_conversion_result_shape_intersect(ifcopenshell_geom_conversion_result_shape_t* self, ifcopenshell_geom_conversion_result_shape_t* arg_0, ifcopenshell_geom_conversion_result_shape_t** out_result);
bool ifcopenshell_geom_conversion_result_shape_is_manifold(ifcopenshell_geom_conversion_result_shape_t* self, bool* out_result);
bool ifcopenshell_geom_conversion_result_shape_moved(ifcopenshell_geom_conversion_result_shape_t* self, ifcopenshell_geom_taxonomy_matrix4_t* arg_0, ifcopenshell_geom_conversion_result_shape_t** out_result);
bool ifcopenshell_geom_conversion_result_shape_num_edges(ifcopenshell_geom_conversion_result_shape_t* self, int32_t* out_result);
bool ifcopenshell_geom_conversion_result_shape_num_faces(ifcopenshell_geom_conversion_result_shape_t* self, int32_t* out_result);
bool ifcopenshell_geom_conversion_result_shape_num_vertices(ifcopenshell_geom_conversion_result_shape_t* self, int32_t* out_result);
bool ifcopenshell_geom_conversion_result_shape_solid(ifcopenshell_geom_conversion_result_shape_t* self, ifcopenshell_geom_conversion_result_shape_t** out_result);
bool ifcopenshell_geom_conversion_result_shape_subtract(ifcopenshell_geom_conversion_result_shape_t* self, ifcopenshell_geom_conversion_result_shape_t* arg_0, ifcopenshell_geom_conversion_result_shape_t** out_result);
bool ifcopenshell_geom_conversion_result_shape_surface_area_along_direction(ifcopenshell_geom_conversion_result_shape_t* self, double tol, ifcopenshell_geom_taxonomy_matrix4_t* arg_1, double along_x, double along_y, double along_z, bool* out_result);
bool ifcopenshell_geom_conversion_result_shape_surface_genus(ifcopenshell_geom_conversion_result_shape_t* self, int32_t* out_result);
bool ifcopenshell_geom_conversion_result_shape_vertices(ifcopenshell_geom_conversion_result_shape_t* self, ifcopenshell_geom_conversion_result_shape_list_t* out_result);
bool ifcopenshell_geom_conversion_result_shape_wrap_in_compound(ifcopenshell_geom_conversion_result_shape_t* self, ifcopenshell_geom_conversion_result_shape_t** out_result);
bool ifcopenshell_geom_function_item_evaluator_evaluation_points(ifcopenshell_geom_function_item_evaluator_t* self, ifcopenshell_double_list_t* out_result);
bool ifcopenshell_geom_function_item_evaluator_evaluation_points_range(ifcopenshell_geom_function_item_evaluator_t* self, double ustart, double uend, int32_t nsteps, ifcopenshell_double_list_t* out_result);
bool ifcopenshell_geom_function_item_evaluator_evaluate(ifcopenshell_geom_function_item_evaluator_t* self, ifcopenshell_geom_taxonomy_item_t** out_result);
bool ifcopenshell_geom_function_item_evaluator_evaluate_range(ifcopenshell_geom_function_item_evaluator_t* self, double ustart, double uend, int32_t nsteps, ifcopenshell_geom_taxonomy_item_t** out_result);
bool ifcopenshell_geom_serialization_brep_data(ifcopenshell_geom_serialization_t* self, ifcopenshell_string_t* out_result);
bool ifcopenshell_geom_serialization_surface_style_ids(ifcopenshell_geom_serialization_t* self, ifcopenshell_int32_list_t* out_result);
bool ifcopenshell_geom_serialization_surface_styles(ifcopenshell_geom_serialization_t* self, ifcopenshell_double_list_t* out_result);
bool ifcopenshell_geom_geometry_serializer_geometry_settings(ifcopenshell_geom_geometry_serializer_t* self, ifcopenshell_geom_settings_t** out_result);
bool ifcopenshell_geom_geometry_serializer_settings(ifcopenshell_geom_geometry_serializer_t* self, ifcopenshell_geom_serializer_settings_t** out_result);
bool ifcopenshell_geom_geometry_serializer_write_triangulation_element(ifcopenshell_geom_geometry_serializer_t* self, ifcopenshell_geom_triangulation_element_t* o);
bool ifcopenshell_geom_geometry_serializer_write_brep_element(ifcopenshell_geom_geometry_serializer_t* self, ifcopenshell_geom_brep_element_t* o);
bool ifcopenshell_geom_geometry_serializer_finalize(ifcopenshell_geom_geometry_serializer_t* self);
bool ifcopenshell_geom_geometry_serializer_is_tesselated(ifcopenshell_geom_geometry_serializer_t* self, bool* out_result);
bool ifcopenshell_geom_geometry_serializer_is_streaming(ifcopenshell_geom_geometry_serializer_t* self, bool* out_result);
bool ifcopenshell_geom_geometry_serializer_read(ifcopenshell_geom_geometry_serializer_t* self, ifcopenshell_file_t* f, const char* guid, const char* representation_id, int32_t rt, ifcopenshell_geom_element_t** out_result);
bool ifcopenshell_geom_geometry_serializer_ready(ifcopenshell_geom_geometry_serializer_t* self, bool* out_result);
bool ifcopenshell_geom_geometry_serializer_set_file(ifcopenshell_geom_geometry_serializer_t* self, ifcopenshell_file_t* arg_0);
bool ifcopenshell_geom_geometry_serializer_set_unit_name_and_magnitude(ifcopenshell_geom_geometry_serializer_t* self, const char* name, double magnitude);
bool ifcopenshell_geom_geometry_serializer_write_header(ifcopenshell_geom_geometry_serializer_t* self);
bool ifcopenshell_geom_taxonomy_circle_matrix(ifcopenshell_geom_taxonomy_circle_t* self, ifcopenshell_geom_taxonomy_matrix4_t** out_result);
bool ifcopenshell_geom_taxonomy_circle_radius(ifcopenshell_geom_taxonomy_circle_t* self, double* out_result);
bool ifcopenshell_geom_taxonomy_line_matrix(ifcopenshell_geom_taxonomy_line_t* self, ifcopenshell_geom_taxonomy_matrix4_t** out_result);
bool ifcopenshell_geom_taxonomy_line_as_item(ifcopenshell_geom_taxonomy_line_t* self, ifcopenshell_geom_taxonomy_item_t** out_result);
bool ifcopenshell_geom_taxonomy_plane_matrix(ifcopenshell_geom_taxonomy_plane_t* self, ifcopenshell_geom_taxonomy_matrix4_t** out_result);
bool ifcopenshell_geom_taxonomy_ellipse_matrix(ifcopenshell_geom_taxonomy_ellipse_t* self, ifcopenshell_geom_taxonomy_matrix4_t** out_result);
bool ifcopenshell_geom_taxonomy_ellipse_radius1(ifcopenshell_geom_taxonomy_ellipse_t* self, double* out_result);
bool ifcopenshell_geom_taxonomy_ellipse_radius2(ifcopenshell_geom_taxonomy_ellipse_t* self, double* out_result);
bool ifcopenshell_geom_taxonomy_style_has_specularity(ifcopenshell_geom_taxonomy_style_t* self, bool* out_result);
bool ifcopenshell_geom_taxonomy_style_has_transparency(ifcopenshell_geom_taxonomy_style_t* self, bool* out_result);
bool ifcopenshell_geom_taxonomy_style_diffuse(ifcopenshell_geom_taxonomy_style_t* self, ifcopenshell_geom_taxonomy_colour_t** out_result);
bool ifcopenshell_geom_taxonomy_style_name(ifcopenshell_geom_taxonomy_style_t* self, ifcopenshell_string_t* out_result);
bool ifcopenshell_geom_taxonomy_style_specular(ifcopenshell_geom_taxonomy_style_t* self, ifcopenshell_geom_taxonomy_colour_t** out_result);
bool ifcopenshell_geom_taxonomy_style_specularity(ifcopenshell_geom_taxonomy_style_t* self, double* out_result);
bool ifcopenshell_geom_taxonomy_style_surface(ifcopenshell_geom_taxonomy_style_t* self, ifcopenshell_geom_taxonomy_colour_t** out_result);
bool ifcopenshell_geom_taxonomy_style_transparency(ifcopenshell_geom_taxonomy_style_t* self, double* out_result);
bool ifcopenshell_geom_taxonomy_style_use_surface_color(ifcopenshell_geom_taxonomy_style_t* self, bool* out_result);
bool ifcopenshell_geom_taxonomy_sphere_matrix(ifcopenshell_geom_taxonomy_sphere_t* self, ifcopenshell_geom_taxonomy_matrix4_t** out_result);
bool ifcopenshell_geom_taxonomy_sphere_radius(ifcopenshell_geom_taxonomy_sphere_t* self, double* out_result);
bool ifcopenshell_geom_taxonomy_torus_matrix(ifcopenshell_geom_taxonomy_torus_t* self, ifcopenshell_geom_taxonomy_matrix4_t** out_result);
bool ifcopenshell_geom_taxonomy_torus_radius1(ifcopenshell_geom_taxonomy_torus_t* self, double* out_result);
bool ifcopenshell_geom_taxonomy_torus_radius2(ifcopenshell_geom_taxonomy_torus_t* self, double* out_result);
bool ifcopenshell_geom_taxonomy_cylinder_matrix(ifcopenshell_geom_taxonomy_cylinder_t* self, ifcopenshell_geom_taxonomy_matrix4_t** out_result);
bool ifcopenshell_geom_taxonomy_cylinder_radius(ifcopenshell_geom_taxonomy_cylinder_t* self, double* out_result);
bool ifcopenshell_geom_taxonomy_extrusion_basis(ifcopenshell_geom_taxonomy_extrusion_t* self, ifcopenshell_geom_taxonomy_item_t** out_result);
bool ifcopenshell_geom_taxonomy_extrusion_depth(ifcopenshell_geom_taxonomy_extrusion_t* self, double* out_result);
bool ifcopenshell_geom_taxonomy_extrusion_direction(ifcopenshell_geom_taxonomy_extrusion_t* self, ifcopenshell_geom_taxonomy_direction3_t** out_result);
bool ifcopenshell_geom_taxonomy_extrusion_matrix(ifcopenshell_geom_taxonomy_extrusion_t* self, ifcopenshell_geom_taxonomy_matrix4_t** out_result);
bool ifcopenshell_geom_taxonomy_offset_curve_basis(ifcopenshell_geom_taxonomy_offset_curve_t* self, ifcopenshell_geom_taxonomy_item_t** out_result);
bool ifcopenshell_geom_taxonomy_offset_curve_offset(ifcopenshell_geom_taxonomy_offset_curve_t* self, double* out_result);
bool ifcopenshell_geom_taxonomy_offset_curve_reference(ifcopenshell_geom_taxonomy_offset_curve_t* self, ifcopenshell_geom_taxonomy_direction3_t** out_result);
bool ifcopenshell_geom_taxonomy_offset_curve_as_item(ifcopenshell_geom_taxonomy_offset_curve_t* self, ifcopenshell_geom_taxonomy_item_t** out_result);
bool ifcopenshell_geom_taxonomy_revolve_has_angle(ifcopenshell_geom_taxonomy_revolve_t* self, bool* out_result);
bool ifcopenshell_geom_taxonomy_revolve_angle(ifcopenshell_geom_taxonomy_revolve_t* self, double* out_result);
bool ifcopenshell_geom_taxonomy_revolve_axis_origin(ifcopenshell_geom_taxonomy_revolve_t* self, ifcopenshell_geom_taxonomy_point3_t** out_result);
bool ifcopenshell_geom_taxonomy_revolve_basis(ifcopenshell_geom_taxonomy_revolve_t* self, ifcopenshell_geom_taxonomy_item_t** out_result);
bool ifcopenshell_geom_taxonomy_revolve_direction(ifcopenshell_geom_taxonomy_revolve_t* self, ifcopenshell_geom_taxonomy_direction3_t** out_result);
bool ifcopenshell_geom_taxonomy_revolve_matrix(ifcopenshell_geom_taxonomy_revolve_t* self, ifcopenshell_geom_taxonomy_matrix4_t** out_result);
bool ifcopenshell_geom_taxonomy_bspline_curve_degree(ifcopenshell_geom_taxonomy_bspline_curve_t* self, int32_t* out_result);
bool ifcopenshell_geom_taxonomy_bspline_curve_knots(ifcopenshell_geom_taxonomy_bspline_curve_t* self, ifcopenshell_double_list_t* out_result);
bool ifcopenshell_geom_taxonomy_bspline_curve_multiplicities(ifcopenshell_geom_taxonomy_bspline_curve_t* self, ifcopenshell_int32_list_t* out_result);
bool ifcopenshell_geom_taxonomy_bspline_curve_has_weights(ifcopenshell_geom_taxonomy_bspline_curve_t* self, bool* out_result);
bool ifcopenshell_geom_taxonomy_bspline_curve_weights(ifcopenshell_geom_taxonomy_bspline_curve_t* self, ifcopenshell_double_list_t* out_result);
bool ifcopenshell_geom_taxonomy_bspline_curve_control_point_count(ifcopenshell_geom_taxonomy_bspline_curve_t* self, size_t* out_result);
bool ifcopenshell_geom_taxonomy_bspline_curve_control_point_at(ifcopenshell_geom_taxonomy_bspline_curve_t* self, size_t index, ifcopenshell_geom_taxonomy_point3_t** out_result);
bool ifcopenshell_geom_taxonomy_bspline_curve_as_item(ifcopenshell_geom_taxonomy_bspline_curve_t* self, ifcopenshell_geom_taxonomy_item_t** out_result);
bool ifcopenshell_geom_taxonomy_sweep_along_curve_basis(ifcopenshell_geom_taxonomy_sweep_along_curve_t* self, ifcopenshell_geom_taxonomy_item_t** out_result);
bool ifcopenshell_geom_taxonomy_sweep_along_curve_has_basis(ifcopenshell_geom_taxonomy_sweep_along_curve_t* self, bool* out_result);
bool ifcopenshell_geom_taxonomy_sweep_along_curve_curve(ifcopenshell_geom_taxonomy_sweep_along_curve_t* self, ifcopenshell_geom_taxonomy_item_t** out_result);
bool ifcopenshell_geom_taxonomy_sweep_along_curve_has_curve(ifcopenshell_geom_taxonomy_sweep_along_curve_t* self, bool* out_result);
bool ifcopenshell_geom_taxonomy_sweep_along_curve_direction(ifcopenshell_geom_taxonomy_sweep_along_curve_t* self, ifcopenshell_geom_taxonomy_direction3_t** out_result);
bool ifcopenshell_geom_taxonomy_sweep_along_curve_has_direction(ifcopenshell_geom_taxonomy_sweep_along_curve_t* self, bool* out_result);
bool ifcopenshell_geom_taxonomy_sweep_along_curve_matrix(ifcopenshell_geom_taxonomy_sweep_along_curve_t* self, ifcopenshell_geom_taxonomy_matrix4_t** out_result);
bool ifcopenshell_geom_taxonomy_sweep_along_curve_has_matrix(ifcopenshell_geom_taxonomy_sweep_along_curve_t* self, bool* out_result);
bool ifcopenshell_geom_taxonomy_sweep_along_curve_surface(ifcopenshell_geom_taxonomy_sweep_along_curve_t* self, ifcopenshell_geom_taxonomy_item_t** out_result);
bool ifcopenshell_geom_taxonomy_sweep_along_curve_has_surface(ifcopenshell_geom_taxonomy_sweep_along_curve_t* self, bool* out_result);
bool ifcopenshell_geom_taxonomy_face_basis(ifcopenshell_geom_taxonomy_face_t* self, ifcopenshell_geom_taxonomy_item_t** out_result);
bool ifcopenshell_geom_taxonomy_face_loop_count(ifcopenshell_geom_taxonomy_face_t* self, size_t* out_result);
bool ifcopenshell_geom_taxonomy_face_loop_at(ifcopenshell_geom_taxonomy_face_t* self, size_t index, ifcopenshell_geom_taxonomy_loop_t** out_result);
bool ifcopenshell_geom_taxonomy_face_matrix(ifcopenshell_geom_taxonomy_face_t* self, ifcopenshell_geom_taxonomy_matrix4_t** out_result);
bool ifcopenshell_geom_taxonomy_face_as_item(ifcopenshell_geom_taxonomy_face_t* self, ifcopenshell_geom_taxonomy_item_t** out_result);
bool ifcopenshell_geom_taxonomy_loft_axis(ifcopenshell_geom_taxonomy_loft_t* self, ifcopenshell_geom_taxonomy_item_t** out_result);
bool ifcopenshell_geom_taxonomy_loft_has_axis(ifcopenshell_geom_taxonomy_loft_t* self, bool* out_result);
bool ifcopenshell_geom_taxonomy_loft_item_count(ifcopenshell_geom_taxonomy_loft_t* self, size_t* out_result);
bool ifcopenshell_geom_taxonomy_loft_item_at(ifcopenshell_geom_taxonomy_loft_t* self, size_t index, ifcopenshell_geom_taxonomy_item_t** out_result);
bool ifcopenshell_geom_taxonomy_loft_add_item(ifcopenshell_geom_taxonomy_loft_t* self, ifcopenshell_geom_taxonomy_item_t* item);
bool ifcopenshell_geom_taxonomy_loft_set_axis(ifcopenshell_geom_taxonomy_loft_t* self, ifcopenshell_geom_taxonomy_item_t* value);
bool ifcopenshell_geom_taxonomy_loop_edge_count(ifcopenshell_geom_taxonomy_loop_t* self, size_t* out_result);
bool ifcopenshell_geom_taxonomy_loop_edge_at(ifcopenshell_geom_taxonomy_loop_t* self, size_t index, ifcopenshell_geom_taxonomy_edge_t** out_result);
bool ifcopenshell_geom_taxonomy_shell_face_count(ifcopenshell_geom_taxonomy_shell_t* self, size_t* out_result);
bool ifcopenshell_geom_taxonomy_shell_face_at(ifcopenshell_geom_taxonomy_shell_t* self, size_t index, ifcopenshell_geom_taxonomy_face_t** out_result);
bool ifcopenshell_geom_taxonomy_solid_shell_count(ifcopenshell_geom_taxonomy_solid_t* self, size_t* out_result);
bool ifcopenshell_geom_taxonomy_solid_shell_at(ifcopenshell_geom_taxonomy_solid_t* self, size_t index, ifcopenshell_geom_taxonomy_shell_t** out_result);
bool ifcopenshell_geom_taxonomy_solid_matrix(ifcopenshell_geom_taxonomy_solid_t* self, ifcopenshell_geom_taxonomy_matrix4_t** out_result);
bool ifcopenshell_geom_taxonomy_collection_item_count(ifcopenshell_geom_taxonomy_collection_t* self, size_t* out_result);
bool ifcopenshell_geom_taxonomy_collection_item_at(ifcopenshell_geom_taxonomy_collection_t* self, size_t index, ifcopenshell_geom_taxonomy_item_t** out_result);
bool ifcopenshell_geom_taxonomy_collection_add_item(ifcopenshell_geom_taxonomy_collection_t* self, ifcopenshell_geom_taxonomy_item_t* item);
bool ifcopenshell_geom_taxonomy_boolean_result_operation(ifcopenshell_geom_taxonomy_boolean_result_t* self, int32_t* out_result);
bool ifcopenshell_geom_taxonomy_boolean_result_item_count(ifcopenshell_geom_taxonomy_boolean_result_t* self, size_t* out_result);
bool ifcopenshell_geom_taxonomy_boolean_result_item_at(ifcopenshell_geom_taxonomy_boolean_result_t* self, size_t index, ifcopenshell_geom_taxonomy_item_t** out_result);
bool ifcopenshell_geom_taxonomy_boolean_result_add_item(ifcopenshell_geom_taxonomy_boolean_result_t* self, ifcopenshell_geom_taxonomy_item_t* item);
bool ifcopenshell_geom_taxonomy_bspline_surface_degree_u(ifcopenshell_geom_taxonomy_bspline_surface_t* self, int32_t* out_result);
bool ifcopenshell_geom_taxonomy_bspline_surface_degree_v(ifcopenshell_geom_taxonomy_bspline_surface_t* self, int32_t* out_result);
bool ifcopenshell_geom_taxonomy_bspline_surface_multiplicities_u(ifcopenshell_geom_taxonomy_bspline_surface_t* self, ifcopenshell_int32_list_t* out_result);
bool ifcopenshell_geom_taxonomy_bspline_surface_multiplicities_v(ifcopenshell_geom_taxonomy_bspline_surface_t* self, ifcopenshell_int32_list_t* out_result);
bool ifcopenshell_geom_taxonomy_bspline_surface_knots_u(ifcopenshell_geom_taxonomy_bspline_surface_t* self, ifcopenshell_double_list_t* out_result);
bool ifcopenshell_geom_taxonomy_bspline_surface_knots_v(ifcopenshell_geom_taxonomy_bspline_surface_t* self, ifcopenshell_double_list_t* out_result);
bool ifcopenshell_geom_taxonomy_bspline_surface_as_item(ifcopenshell_geom_taxonomy_bspline_surface_t* self, ifcopenshell_geom_taxonomy_item_t** out_result);
bool ifcopenshell_geom_opaque_number_clone(ifcopenshell_geom_opaque_number_t* self, ifcopenshell_geom_opaque_number_t** out_result);
bool ifcopenshell_geom_opaque_number_to_double(ifcopenshell_geom_opaque_number_t* self, double* out_result);
bool ifcopenshell_geom_opaque_number_to_string(ifcopenshell_geom_opaque_number_t* self, ifcopenshell_string_t* out_result);
bool ifcopenshell_geom_serializer_finalize(ifcopenshell_geom_serializer_t* self);
bool ifcopenshell_geom_serializer_is_streaming(ifcopenshell_geom_serializer_t* self, bool* out_result);
bool ifcopenshell_geom_serializer_ready(ifcopenshell_geom_serializer_t* self, bool* out_result);
bool ifcopenshell_geom_serializer_set_file(ifcopenshell_geom_serializer_t* self, ifcopenshell_file_t* arg_0);
bool ifcopenshell_geom_serializer_write_header(ifcopenshell_geom_serializer_t* self);
bool ifcopenshell_geom_settings_get_type(ifcopenshell_geom_settings_t* self, const char* name, ifcopenshell_string_t* out_result);
bool ifcopenshell_geom_settings_setting_names(ifcopenshell_geom_settings_t* self, ifcopenshell_string_list_t* out_result);
bool ifcopenshell_geom_settings_get_bool(ifcopenshell_geom_settings_t* self, const char* name, bool* out_result);
bool ifcopenshell_geom_settings_set_bool(ifcopenshell_geom_settings_t* self, const char* name, bool value);
bool ifcopenshell_geom_settings_get_int(ifcopenshell_geom_settings_t* self, const char* name, int32_t* out_result);
bool ifcopenshell_geom_settings_set_int(ifcopenshell_geom_settings_t* self, const char* name, int32_t value);
bool ifcopenshell_geom_settings_get_double(ifcopenshell_geom_settings_t* self, const char* name, double* out_result);
bool ifcopenshell_geom_settings_set_double(ifcopenshell_geom_settings_t* self, const char* name, double value);
bool ifcopenshell_geom_settings_get_string(ifcopenshell_geom_settings_t* self, const char* name, ifcopenshell_string_t* out_result);
bool ifcopenshell_geom_settings_set_string(ifcopenshell_geom_settings_t* self, const char* name, const char* value);
bool ifcopenshell_geom_settings_get_int_set(ifcopenshell_geom_settings_t* self, const char* name, ifcopenshell_int32_list_t* out_result);
bool ifcopenshell_geom_settings_set_int_set(ifcopenshell_geom_settings_t* self, const char* name, const ifcopenshell_int32_list_t* value);
bool ifcopenshell_geom_settings_get_string_set(ifcopenshell_geom_settings_t* self, const char* name, ifcopenshell_string_list_t* out_result);
bool ifcopenshell_geom_settings_set_string_set(ifcopenshell_geom_settings_t* self, const char* name, const ifcopenshell_string_list_t* value);
bool ifcopenshell_geom_settings_get_double_list(ifcopenshell_geom_settings_t* self, const char* name, ifcopenshell_double_list_t* out_result);
bool ifcopenshell_geom_settings_set_double_list(ifcopenshell_geom_settings_t* self, const char* name, const ifcopenshell_double_list_t* value);
bool ifcopenshell_geom_serializer_settings_get_type(ifcopenshell_geom_serializer_settings_t* self, const char* name, ifcopenshell_string_t* out_result);
bool ifcopenshell_geom_serializer_settings_setting_names(ifcopenshell_geom_serializer_settings_t* self, ifcopenshell_string_list_t* out_result);
bool ifcopenshell_geom_serializer_settings_get_bool(ifcopenshell_geom_serializer_settings_t* self, const char* name, bool* out_result);
bool ifcopenshell_geom_serializer_settings_set_bool(ifcopenshell_geom_serializer_settings_t* self, const char* name, bool value);
bool ifcopenshell_geom_serializer_settings_get_int(ifcopenshell_geom_serializer_settings_t* self, const char* name, int32_t* out_result);
bool ifcopenshell_geom_serializer_settings_set_int(ifcopenshell_geom_serializer_settings_t* self, const char* name, int32_t value);
bool ifcopenshell_geom_serializer_settings_get_double(ifcopenshell_geom_serializer_settings_t* self, const char* name, double* out_result);
bool ifcopenshell_geom_serializer_settings_set_double(ifcopenshell_geom_serializer_settings_t* self, const char* name, double value);
bool ifcopenshell_geom_serializer_settings_get_string(ifcopenshell_geom_serializer_settings_t* self, const char* name, ifcopenshell_string_t* out_result);
bool ifcopenshell_geom_serializer_settings_set_string(ifcopenshell_geom_serializer_settings_t* self, const char* name, const char* value);
bool ifcopenshell_geom_serializer_settings_get_int_set(ifcopenshell_geom_serializer_settings_t* self, const char* name, ifcopenshell_int32_list_t* out_result);
bool ifcopenshell_geom_serializer_settings_set_int_set(ifcopenshell_geom_serializer_settings_t* self, const char* name, const ifcopenshell_int32_list_t* value);
bool ifcopenshell_geom_buffer_get_value(ifcopenshell_geom_buffer_t* self, ifcopenshell_string_t* out_result);
bool ifcopenshell_geom_buffer_is_ready(ifcopenshell_geom_buffer_t* self, bool* out_result);
bool ifcopenshell_geom_taxonomy_item_hash(ifcopenshell_geom_taxonomy_item_t* self, size_t* out_result);
bool ifcopenshell_geom_taxonomy_item_identity(ifcopenshell_geom_taxonomy_item_t* self, uint32_t* out_result);
bool ifcopenshell_geom_taxonomy_item_kind(ifcopenshell_geom_taxonomy_item_t* self, int32_t* out_result);
bool ifcopenshell_geom_tree_enable_face_styles(ifcopenshell_geom_tree_t* self, bool* out_result);
bool ifcopenshell_geom_tree_set_enable_face_styles(ifcopenshell_geom_tree_t* self, bool enable);
bool ifcopenshell_geom_tree_add_file(ifcopenshell_geom_tree_t* self, ifcopenshell_file_t* file, ifcopenshell_geom_settings_t* settings);
bool ifcopenshell_geom_tree_add_iterator(ifcopenshell_geom_tree_t* self, ifcopenshell_geom_iterator_t* iterator);
bool ifcopenshell_geom_tree_clash_clearance_many(ifcopenshell_geom_tree_t* self, const ifcopenshell_instance_list_t* set_a, const ifcopenshell_instance_list_t* set_b, double clearance, bool check_all, ifcopenshell_geom_tree_clash_list_t** out_result);
bool ifcopenshell_geom_tree_clash_collision_many(ifcopenshell_geom_tree_t* self, const ifcopenshell_instance_list_t* set_a, const ifcopenshell_instance_list_t* set_b, bool allow_touching, ifcopenshell_geom_tree_clash_list_t** out_result);
bool ifcopenshell_geom_tree_clash_intersection_many(ifcopenshell_geom_tree_t* self, const ifcopenshell_instance_list_t* set_a, const ifcopenshell_instance_list_t* set_b, double tolerance, bool check_all, ifcopenshell_geom_tree_clash_list_t** out_result);
bool ifcopenshell_geom_tree_distances(ifcopenshell_geom_tree_t* self, ifcopenshell_double_list_t* out_result);
bool ifcopenshell_geom_tree_is_manifold(ifcopenshell_geom_tree_t* self, const ifcopenshell_int32_list_t* faces, bool* out_result);
bool ifcopenshell_geom_tree_protrusion_distances(ifcopenshell_geom_tree_t* self, ifcopenshell_double_list_t* out_result);
bool ifcopenshell_geom_tree_styles(ifcopenshell_geom_tree_t* self, ifcopenshell_geom_taxonomy_style_list_t* out_result);
bool ifcopenshell_geom_tree_uint8_to_b64(ifcopenshell_geom_tree_t* self, const ifcopenshell_uint8_list_t* uuids_array, ifcopenshell_string_t* out_result);
bool ifcopenshell_geom_tree_style_count(ifcopenshell_geom_tree_t* self, size_t* out_result);
bool ifcopenshell_geom_tree_style_at(ifcopenshell_geom_tree_t* self, size_t index, ifcopenshell_geom_taxonomy_style_t** out_result);
bool ifcopenshell_geom_tree_clash_a(ifcopenshell_geom_tree_clash_t* self, ifcopenshell_instance_t** out_result);
bool ifcopenshell_geom_tree_clash_b(ifcopenshell_geom_tree_clash_t* self, ifcopenshell_instance_t** out_result);
bool ifcopenshell_geom_tree_clash_type(ifcopenshell_geom_tree_clash_t* self, int32_t* out_result);
bool ifcopenshell_geom_tree_clash_distance(ifcopenshell_geom_tree_clash_t* self, double* out_result);
bool ifcopenshell_geom_tree_clash_p1(ifcopenshell_geom_tree_clash_t* self, ifcopenshell_double_list_t* out_result);
bool ifcopenshell_geom_tree_clash_p2(ifcopenshell_geom_tree_clash_t* self, ifcopenshell_double_list_t* out_result);
bool ifcopenshell_geom_tree_ray_intersection_distance(ifcopenshell_geom_tree_ray_intersection_t* self, double* out_result);
bool ifcopenshell_geom_tree_ray_intersection_dot_product(ifcopenshell_geom_tree_ray_intersection_t* self, double* out_result);
bool ifcopenshell_geom_tree_ray_intersection_normal(ifcopenshell_geom_tree_ray_intersection_t* self, ifcopenshell_double_list_t* out_result);
bool ifcopenshell_geom_tree_ray_intersection_position(ifcopenshell_geom_tree_ray_intersection_t* self, ifcopenshell_double_list_t* out_result);
bool ifcopenshell_geom_tree_ray_intersection_ray_distance(ifcopenshell_geom_tree_ray_intersection_t* self, double* out_result);
bool ifcopenshell_geom_tree_ray_intersection_style_index(ifcopenshell_geom_tree_ray_intersection_t* self, int32_t* out_result);
bool ifcopenshell_geom_taxonomy_point3_get_data(ifcopenshell_geom_taxonomy_point3_t* self, ifcopenshell_double_list_t* out_result);
bool ifcopenshell_geom_taxonomy_direction3_get_data(ifcopenshell_geom_taxonomy_direction3_t* self, ifcopenshell_double_list_t* out_result);
bool ifcopenshell_geom_taxonomy_matrix4_get_data(ifcopenshell_geom_taxonomy_matrix4_t* self, ifcopenshell_double_list_t* out_result);
bool ifcopenshell_geom_taxonomy_colour_get_data(ifcopenshell_geom_taxonomy_colour_t* self, ifcopenshell_double_list_t* out_result);
bool ifcopenshell_geom_transformation_matrix(ifcopenshell_geom_transformation_t* self, ifcopenshell_double_list_t* out_result);
bool ifcopenshell_geom_tree_clash_count(ifcopenshell_geom_tree_t* self, ifcopenshell_geom_tree_clash_list_t* clashes, size_t* out_result);
bool ifcopenshell_geom_tree_clash_at(ifcopenshell_geom_tree_t* self, ifcopenshell_geom_tree_clash_list_t* clashes, size_t index, ifcopenshell_geom_tree_clash_t** out_result);
bool ifcopenshell_geom_tree_ray_intersection_count(ifcopenshell_geom_tree_t* self, ifcopenshell_geom_tree_ray_intersection_list_t* intersections, size_t* out_result);
bool ifcopenshell_geom_tree_ray_intersection_at(ifcopenshell_geom_tree_t* self, ifcopenshell_geom_tree_ray_intersection_list_t* intersections, size_t index, ifcopenshell_geom_tree_ray_intersection_t** out_result);
bool ifcopenshell_file_add_entity(ifcopenshell_file_t* self, ifcopenshell_instance_t* instance, uint32_t id, ifcopenshell_instance_t** out_result);
bool ifcopenshell_type_declaration_argument_types(ifcopenshell_type_declaration_t* self, ifcopenshell_string_list_t* out_result);
bool ifcopenshell_enumeration_argument_types(ifcopenshell_enumeration_t* self, ifcopenshell_string_list_t* out_result);
bool ifcopenshell_entity_argument_types(ifcopenshell_entity_t* self, ifcopenshell_string_list_t* out_result);
bool ifcopenshell_parse_attribute_value_as_bool(ifcopenshell_parse_attribute_value_t* self, bool* out_result);
bool ifcopenshell_parse_attribute_value_as_double(ifcopenshell_parse_attribute_value_t* self, double* out_result);
bool ifcopenshell_parse_attribute_value_as_double_list(ifcopenshell_parse_attribute_value_t* self, ifcopenshell_double_list_t* out_result);
bool ifcopenshell_parse_attribute_value_as_double_list_list(ifcopenshell_parse_attribute_value_t* self, ifcopenshell_double_list_list_t* out_result);
bool ifcopenshell_parse_attribute_value_as_enumeration_index(ifcopenshell_parse_attribute_value_t* self, size_t* out_result);
bool ifcopenshell_parse_attribute_value_as_enumeration_type(ifcopenshell_parse_attribute_value_t* self, ifcopenshell_enumeration_t** out_result);
bool ifcopenshell_parse_attribute_value_as_enumeration_value(ifcopenshell_parse_attribute_value_t* self, ifcopenshell_string_t* out_result);
bool ifcopenshell_parse_attribute_value_as_instance(ifcopenshell_parse_attribute_value_t* self, ifcopenshell_instance_t** out_result);
bool ifcopenshell_parse_attribute_value_as_instance_id_list_list(ifcopenshell_parse_attribute_value_t* self, ifcopenshell_int32_list_list_t* out_result);
bool ifcopenshell_parse_attribute_value_as_instance_list(ifcopenshell_parse_attribute_value_t* self, ifcopenshell_parse_instance_list_t** out_result);
bool ifcopenshell_parse_attribute_value_as_int32(ifcopenshell_parse_attribute_value_t* self, int32_t* out_result);
bool ifcopenshell_parse_attribute_value_as_int32_list(ifcopenshell_parse_attribute_value_t* self, ifcopenshell_int32_list_t* out_result);
bool ifcopenshell_parse_attribute_value_as_int32_list_list(ifcopenshell_parse_attribute_value_t* self, ifcopenshell_int32_list_list_t* out_result);
bool ifcopenshell_parse_attribute_value_as_string(ifcopenshell_parse_attribute_value_t* self, ifcopenshell_string_t* out_result);
bool ifcopenshell_parse_attribute_value_as_string_list(ifcopenshell_parse_attribute_value_t* self, ifcopenshell_string_list_t* out_result);
bool ifcopenshell_instance_class_name(ifcopenshell_instance_t* self, bool with_schema, ifcopenshell_string_t* out_result);
bool ifcopenshell_file_create_entity_by_name(ifcopenshell_file_t* self, const char* type_name, ifcopenshell_instance_t** out_result);
bool ifcopenshell_file_create_entity_by_name_with_id(ifcopenshell_file_t* self, const char* type_name, uint32_t id, ifcopenshell_instance_t** out_result);
bool ifcopenshell_file_entity_names(ifcopenshell_file_t* self, ifcopenshell_uint32_list_t* out_result);
bool ifcopenshell_file_file_pointer(ifcopenshell_file_t* self, size_t* out_result);
bool ifcopenshell_instance_file_pointer(ifcopenshell_instance_t* self, size_t* out_result);
bool ifcopenshell_parse_instance_list_get(ifcopenshell_parse_instance_list_t* self, size_t index, ifcopenshell_instance_t** out_result);
bool ifcopenshell_instance_get_argument_by_name(ifcopenshell_instance_t* self, const char* name, ifcopenshell_parse_attribute_value_t** out_result);
bool ifcopenshell_instance_get_argument_index(ifcopenshell_instance_t* self, const char* name, uint32_t* out_result);
bool ifcopenshell_instance_get_argument_name(ifcopenshell_instance_t* self, uint32_t index, ifcopenshell_string_t* out_result);
bool ifcopenshell_instance_get_argument_type(ifcopenshell_instance_t* self, uint32_t index, ifcopenshell_string_t* out_result);
bool ifcopenshell_instance_get_attribute_category(ifcopenshell_instance_t* self, const char* name, int32_t* out_result);
bool ifcopenshell_instance_get_attribute_names(ifcopenshell_instance_t* self, ifcopenshell_string_list_t* out_result);
bool ifcopenshell_instance_get_attribute_value(ifcopenshell_instance_t* self, size_t index, ifcopenshell_parse_attribute_value_t** out_result);
bool ifcopenshell_file_get_inverse(ifcopenshell_file_t* self, ifcopenshell_instance_t* instance, ifcopenshell_parse_instance_list_t** out_result);
bool ifcopenshell_instance_get_inverse(ifcopenshell_instance_t* self, const char* name, ifcopenshell_parse_instance_list_t** out_result);
bool ifcopenshell_instance_get_inverse_attribute_by_name(ifcopenshell_instance_t* self, const char* name, ifcopenshell_parse_instance_list_t** out_result);
bool ifcopenshell_instance_get_inverse_attribute_names(ifcopenshell_instance_t* self, ifcopenshell_string_list_t* out_result);
bool ifcopenshell_file_get_inverse_indices(ifcopenshell_file_t* self, ifcopenshell_instance_t* instance, ifcopenshell_int32_list_t* out_result);
bool ifcopenshell_file_get_total_inverses(ifcopenshell_file_t* self, ifcopenshell_instance_t* instance, int32_t* out_result);
bool ifcopenshell_file_get_unit(ifcopenshell_file_t* self, const char* unit_type, double* out_result);
bool ifcopenshell_file_good(ifcopenshell_file_t* self, int32_t* out_result);
bool ifcopenshell_file_header(ifcopenshell_file_t* self, ifcopenshell_header_t** out_result);
bool ifcopenshell_file_header_file_description(ifcopenshell_file_t* self, ifcopenshell_instance_t** out_result);
bool ifcopenshell_file_header_file_name(ifcopenshell_file_t* self, ifcopenshell_instance_t** out_result);
bool ifcopenshell_file_header_file_schema(ifcopenshell_file_t* self, ifcopenshell_instance_t** out_result);
bool ifcopenshell_instance_streamer_inverses(ifcopenshell_instance_streamer_t* self, ifcopenshell_string_t* out_result);
bool ifcopenshell_instance_is_a(ifcopenshell_instance_t* self, const char* declaration_name, bool* out_result);
bool ifcopenshell_parse_attribute_value_is_null(ifcopenshell_parse_attribute_value_t* self, bool* out_result);
bool ifcopenshell_file_key_value_store_iter(ifcopenshell_file_t* self, const char* prefix, ifcopenshell_string_list_t* out_result);
bool ifcopenshell_file_key_value_store_query(ifcopenshell_file_t* self, const char* key, ifcopenshell_uint8_list_t* out_result);
bool ifcopenshell_parameter_type_kind(ifcopenshell_parameter_type_t* self, ifcopenshell_string_t* out_result);
bool ifcopenshell_simple_type_kind(ifcopenshell_simple_type_t* self, ifcopenshell_string_t* out_result);
bool ifcopenshell_aggregation_type_kind(ifcopenshell_aggregation_type_t* self, ifcopenshell_string_t* out_result);
bool ifcopenshell_instance_streamer_read_instance_py(ifcopenshell_instance_streamer_t* self, bool type_as_declaration_instance, ifcopenshell_string_t* out_result);
bool ifcopenshell_instance_streamer_references(ifcopenshell_instance_streamer_t* self, ifcopenshell_string_t* out_result);
bool ifcopenshell_file_schema_name(ifcopenshell_file_t* self, ifcopenshell_string_t* out_result);
bool ifcopenshell_select_type_select_list_names(ifcopenshell_select_type_t* self, ifcopenshell_string_list_t* out_result);
bool ifcopenshell_instance_set_argument_as_aggregate_of_aggregate_of_entity_instance(ifcopenshell_instance_t* self, size_t index, const ifcopenshell_int32_list_list_t* value);
bool ifcopenshell_instance_set_argument_bool(ifcopenshell_instance_t* self, size_t index, bool value);
bool ifcopenshell_instance_set_argument_double(ifcopenshell_instance_t* self, size_t index, double value);
bool ifcopenshell_instance_set_argument_double_list(ifcopenshell_instance_t* self, size_t index, const ifcopenshell_double_list_t* value);
bool ifcopenshell_instance_set_argument_double_list_list(ifcopenshell_instance_t* self, size_t index, const ifcopenshell_double_list_list_t* value);
bool ifcopenshell_instance_set_argument_enumeration(ifcopenshell_instance_t* self, size_t index, ifcopenshell_enumeration_t* enumeration, size_t enumeration_index);
bool ifcopenshell_instance_set_argument_enumeration_by_name(ifcopenshell_instance_t* self, size_t index, const char* value, bool* out_result);
bool ifcopenshell_instance_set_argument_instance(ifcopenshell_instance_t* self, size_t index, ifcopenshell_instance_t* value);
bool ifcopenshell_instance_set_argument_instance_list(ifcopenshell_instance_t* self, size_t index, ifcopenshell_parse_instance_list_t* value);
bool ifcopenshell_instance_set_argument_int32(ifcopenshell_instance_t* self, size_t index, int32_t value);
bool ifcopenshell_instance_set_argument_int32_list(ifcopenshell_instance_t* self, size_t index, const ifcopenshell_int32_list_t* value);
bool ifcopenshell_instance_set_argument_int32_list_list(ifcopenshell_instance_t* self, size_t index, const ifcopenshell_int32_list_list_t* value);
bool ifcopenshell_instance_set_argument_logical(ifcopenshell_instance_t* self, size_t index, int32_t value);
bool ifcopenshell_instance_set_argument_string(ifcopenshell_instance_t* self, size_t index, const char* value);
bool ifcopenshell_instance_set_argument_string_list(ifcopenshell_instance_t* self, size_t index, const ifcopenshell_string_list_t* value);
bool ifcopenshell_instance_set_attribute_value(ifcopenshell_instance_t* self, const char* name, ifcopenshell_parse_attribute_value_t* value);
bool ifcopenshell_parse_attribute_value_size(ifcopenshell_parse_attribute_value_t* self, size_t* out_result);
bool ifcopenshell_parse_instance_list_size(ifcopenshell_parse_instance_list_t* self, size_t* out_result);
bool ifcopenshell_instance_streamer_status(ifcopenshell_instance_streamer_t* self, int32_t* out_result);
bool ifcopenshell_file_storage_mode(ifcopenshell_file_t* self, int32_t* out_result);
bool ifcopenshell_file_to_string(ifcopenshell_file_t* self, ifcopenshell_string_t* out_result);
bool ifcopenshell_instance_to_string(ifcopenshell_instance_t* self, bool valid_spf, ifcopenshell_string_t* out_result);
bool ifcopenshell_parse_attribute_value_type(ifcopenshell_parse_attribute_value_t* self, ifcopenshell_string_t* out_result);
bool ifcopenshell_aggregation_type_type_of_aggregation(ifcopenshell_aggregation_type_t* self, int32_t* out_result);
bool ifcopenshell_inverse_attribute_type_of_aggregation(ifcopenshell_inverse_attribute_t* self, int32_t* out_result);
bool ifcopenshell_aggregation_type_type_of_aggregation_string(ifcopenshell_aggregation_type_t* self, ifcopenshell_string_t* out_result);
bool ifcopenshell_inverse_attribute_type_of_aggregation_string(ifcopenshell_inverse_attribute_t* self, ifcopenshell_string_t* out_result);
bool ifcopenshell_file_types(ifcopenshell_file_t* self, ifcopenshell_string_list_t* out_result);
bool ifcopenshell_instance_unset_argument(ifcopenshell_instance_t* self, size_t index);
bool ifcopenshell_instance_unset_attribute_value(ifcopenshell_instance_t* self, const char* name);
bool ifcopenshell_file_write(ifcopenshell_file_t* self, const char* path);
bool ifcopenshell_header_write(ifcopenshell_header_t* self, ifcopenshell_string_t* out_result);
bool ifcopenshell_geom_opaque_number_add(ifcopenshell_geom_opaque_number_t* self, ifcopenshell_geom_opaque_number_t* other, ifcopenshell_geom_opaque_number_t** out_result);
bool ifcopenshell_geom_conversion_result_shape_area(ifcopenshell_geom_conversion_result_shape_t* self, double* out_result);
bool ifcopenshell_geom_brep_representation_as_compound(ifcopenshell_geom_brep_representation_t* self, bool force_meters, ifcopenshell_geom_conversion_result_shape_t** out_result);
bool ifcopenshell_geom_svgfill_polygon_boundary_point(ifcopenshell_geom_svgfill_polygon_t* self, size_t index, ifcopenshell_double_list_t* out_result);
bool ifcopenshell_geom_svgfill_polygon_boundary_size(ifcopenshell_geom_svgfill_polygon_t* self, size_t* out_result);
bool ifcopenshell_geom_brep_element_calc_surface_area(ifcopenshell_geom_brep_element_t* self, double* out_result);
bool ifcopenshell_geom_brep_element_calc_volume(ifcopenshell_geom_brep_element_t* self, double* out_result);
bool ifcopenshell_geom_triangulation_colors_buffer(ifcopenshell_geom_triangulation_t* self, ifcopenshell_double_list_t* out_result);
bool ifcopenshell_geom_triangulation_colors_buffer_size(ifcopenshell_geom_triangulation_t* self, size_t* out_result);
bool ifcopenshell_geom_taxonomy_bspline_surface_control_point_at(ifcopenshell_geom_taxonomy_bspline_surface_t* self, size_t row, size_t col, ifcopenshell_geom_taxonomy_point3_t** out_result);
bool ifcopenshell_geom_taxonomy_bspline_surface_control_point_col_count_at(ifcopenshell_geom_taxonomy_bspline_surface_t* self, size_t row, size_t* out_result);
bool ifcopenshell_geom_taxonomy_bspline_surface_control_point_row_count(ifcopenshell_geom_taxonomy_bspline_surface_t* self, size_t* out_result);
bool ifcopenshell_geom_conversion_result_shape_convex_tag(ifcopenshell_geom_conversion_result_shape_t* self, bool value);
bool ifcopenshell_geom_opaque_number_divide(ifcopenshell_geom_opaque_number_t* self, ifcopenshell_geom_opaque_number_t* other, ifcopenshell_geom_opaque_number_t** out_result);
bool ifcopenshell_geom_triangulation_edges_buffer(ifcopenshell_geom_triangulation_t* self, const int32_t** out_result);
bool ifcopenshell_geom_triangulation_edges_item_ids_buffer(ifcopenshell_geom_triangulation_t* self, const int32_t** out_result);
bool ifcopenshell_geom_opaque_number_equals(ifcopenshell_geom_opaque_number_t* self, ifcopenshell_geom_opaque_number_t* other, bool* out_result);
bool ifcopenshell_geom_function_item_evaluator_evaluate_at(ifcopenshell_geom_function_item_evaluator_t* self, double u, ifcopenshell_double_list_t* out_result);
bool ifcopenshell_geom_triangulation_faces_buffer(ifcopenshell_geom_triangulation_t* self, const int32_t** out_result);
bool ifcopenshell_geom_iterator_get_as_brep_element(ifcopenshell_geom_iterator_t* self, ifcopenshell_geom_brep_element_t** out_result);
bool ifcopenshell_geom_iterator_get_as_serialized_element(ifcopenshell_geom_iterator_t* self, ifcopenshell_geom_serialized_element_t** out_result);
bool ifcopenshell_geom_iterator_get_as_triangulation_element(ifcopenshell_geom_iterator_t* self, ifcopenshell_geom_triangulation_element_t** out_result);
bool ifcopenshell_geom_taxonomy_bspline_surface_has_weights(ifcopenshell_geom_taxonomy_bspline_surface_t* self, bool* out_result);
bool ifcopenshell_geom_svgfill_polygon_inner_boundary_count(ifcopenshell_geom_svgfill_polygon_t* self, size_t* out_result);
bool ifcopenshell_geom_svgfill_polygon_inner_boundary_point(ifcopenshell_geom_svgfill_polygon_t* self, size_t boundary_index, size_t point_index, ifcopenshell_double_list_t* out_result);
bool ifcopenshell_geom_svgfill_polygon_inner_boundary_size(ifcopenshell_geom_svgfill_polygon_t* self, size_t boundary_index, size_t* out_result);
bool ifcopenshell_geom_tree_ray_intersection_instance(ifcopenshell_geom_tree_ray_intersection_t* self, ifcopenshell_instance_t** out_result);
bool ifcopenshell_geom_taxonomy_style_instance_id(ifcopenshell_geom_taxonomy_style_t* self, size_t* out_result);
bool ifcopenshell_geom_triangulation_item_ids_buffer(ifcopenshell_geom_triangulation_t* self, const int32_t** out_result);
bool ifcopenshell_geom_conversion_result_shape_length(ifcopenshell_geom_conversion_result_shape_t* self, double* out_result);
bool ifcopenshell_geom_opaque_number_less_than(ifcopenshell_geom_opaque_number_t* self, ifcopenshell_geom_opaque_number_t* other, bool* out_result);
bool ifcopenshell_geom_triangulation_material_ids_buffer(ifcopenshell_geom_triangulation_t* self, const int32_t** out_result);
bool ifcopenshell_geom_opaque_number_multiply(ifcopenshell_geom_opaque_number_t* self, ifcopenshell_geom_opaque_number_t* other, ifcopenshell_geom_opaque_number_t** out_result);
bool ifcopenshell_geom_opaque_number_negate(ifcopenshell_geom_opaque_number_t* self, ifcopenshell_geom_opaque_number_t** out_result);
bool ifcopenshell_geom_iterator_next(ifcopenshell_geom_iterator_t* self, bool* out_result);
bool ifcopenshell_geom_triangulation_normals_buffer(ifcopenshell_geom_triangulation_t* self, const double** out_result);
bool ifcopenshell_geom_svgfill_polygon_point_inside(ifcopenshell_geom_svgfill_polygon_t* self, ifcopenshell_double_list_t* out_result);
bool ifcopenshell_geom_tree_select_box_bounds(ifcopenshell_geom_tree_t* self, double xmin, double ymin, double zmin, double xmax, double ymax, double zmax, bool completely_within, ifcopenshell_parse_instance_list_t** out_result);
bool ifcopenshell_geom_tree_select_box_element(ifcopenshell_geom_tree_t* self, ifcopenshell_instance_t* instance, bool completely_within, double extend, ifcopenshell_parse_instance_list_t** out_result);
bool ifcopenshell_geom_tree_select_box_point(ifcopenshell_geom_tree_t* self, double x, double y, double z, double extend, ifcopenshell_parse_instance_list_t** out_result);
bool ifcopenshell_geom_tree_select_brep_element(ifcopenshell_geom_tree_t* self, ifcopenshell_geom_brep_element_t* element, bool completely_within, double extend, ifcopenshell_parse_instance_list_t** out_result);
bool ifcopenshell_geom_tree_select_element(ifcopenshell_geom_tree_t* self, ifcopenshell_instance_t* instance, bool completely_within, double extend, ifcopenshell_parse_instance_list_t** out_result);
bool ifcopenshell_geom_tree_select_point(ifcopenshell_geom_tree_t* self, double x, double y, double z, double extend, ifcopenshell_parse_instance_list_t** out_result);
bool ifcopenshell_geom_tree_select_ray(ifcopenshell_geom_tree_t* self, double origin_x, double origin_y, double origin_z, double dir_x, double dir_y, double dir_z, double length, ifcopenshell_geom_tree_ray_intersection_list_t** out_result);
bool ifcopenshell_geom_conversion_result_shape_serialize(ifcopenshell_geom_conversion_result_shape_t* self, ifcopenshell_string_t* out_result);
bool ifcopenshell_geom_conversion_result_shape_serialize_obj(ifcopenshell_geom_conversion_result_shape_t* self, ifcopenshell_string_t* out_result);
bool ifcopenshell_geom_opaque_number_subtract(ifcopenshell_geom_opaque_number_t* self, ifcopenshell_geom_opaque_number_t* other, ifcopenshell_geom_opaque_number_t** out_result);
bool ifcopenshell_geom_element_transformation_buffer(ifcopenshell_geom_element_t* self, const double** out_result);
bool ifcopenshell_geom_element_transformation_buffer_size(ifcopenshell_geom_element_t* self, size_t* out_result);
bool ifcopenshell_geom_triangulation_uvs_buffer(ifcopenshell_geom_triangulation_t* self, const double** out_result);
bool ifcopenshell_geom_triangulation_verts_buffer(ifcopenshell_geom_triangulation_t* self, const double** out_result);
bool ifcopenshell_geom_conversion_result_shape_volume(ifcopenshell_geom_conversion_result_shape_t* self, double* out_result);
bool ifcopenshell_geom_taxonomy_bspline_surface_weight_at(ifcopenshell_geom_taxonomy_bspline_surface_t* self, size_t row, size_t col, double* out_result);
bool ifcopenshell_geom_taxonomy_bspline_surface_weight_col_count_at(ifcopenshell_geom_taxonomy_bspline_surface_t* self, size_t row, size_t* out_result);
bool ifcopenshell_geom_taxonomy_bspline_surface_weight_row_count(ifcopenshell_geom_taxonomy_bspline_surface_t* self, size_t* out_result);

#ifdef __cplusplus
}
#endif

#endif
