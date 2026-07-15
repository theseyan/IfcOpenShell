// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ifcopenshell_api.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

static int tests_run, tests_failed;

#define ASSERT(c, m)                                                                                                   \
    do {                                                                                                               \
        tests_run++;                                                                                                   \
        if (!(c)) {                                                                                                    \
            fprintf(stderr, "FAIL [%s:%d]: %s (%s)\n", __FILE__, __LINE__, m, ifcopenshell_last_error_message()); \
            tests_failed++;                                                                                            \
        }                                                                                                              \
    } while (0)

static ifcopenshell_instance_t* create_entity(ifcopenshell_file_t* file, const char* name) {
    ifcopenshell_instance_t* result = NULL;
    ASSERT(ifcopenshell_file_create_entity_by_name(file, name, &result), "entity creation succeeds");
    return result;
}

static void assert_string(ifcopenshell_instance_t* entity, const char* name, const char* expected) {
    ifcopenshell_parse_attribute_value_t* value = NULL;
    ifcopenshell_string_t text = {0};
    ASSERT(ifcopenshell_instance_get_argument_by_name(entity, name, &value), "attribute read succeeds");
    ASSERT(value != NULL && ifcopenshell_parse_attribute_value_as_string(value, &text), "attribute is a string");
    ASSERT(text.data != NULL && strcmp(text.data, expected) == 0, "attribute value matches");
    ifcopenshell_string_destroy(&text);
    if (value != NULL) {
        ifcopenshell_parse_attribute_value_destroy(value);
    }
}

static bool has_instance_attribute(ifcopenshell_instance_t* entity, const char* name) {
    ifcopenshell_parse_attribute_value_t* value = NULL;
    ifcopenshell_instance_t* referenced = NULL;
    bool result = ifcopenshell_instance_get_argument_by_name(entity, name, &value) &&
        ifcopenshell_parse_attribute_value_as_instance(value, &referenced) && referenced != NULL;
    ifcopenshell_parse_attribute_value_destroy(value);
    return result;
}

static void test_named_editors(void) {
    ifcopenshell_file_t* file = NULL;
    void* attributes = NULL;
    ASSERT(ifcopenshell_parse_new_file("IFC4", 0, "", &file), "new IFC4 file succeeds");
    ASSERT(ifcopenshell_pset_props_new(&attributes), "empty property map succeeds");

    ifcopenshell_instance_t* classification = create_entity(file, "IfcClassification");
    ifcopenshell_instance_t* reference = create_entity(file, "IfcClassificationReference");
    ifcopenshell_instance_t* metric = create_entity(file, "IfcMetric");
    ifcopenshell_instance_t* objective = create_entity(file, "IfcObjective");
    ifcopenshell_instance_t* information = create_entity(file, "IfcDocumentInformation");
    ifcopenshell_instance_t* document_reference = create_entity(file, "IfcDocumentReference");
    ifcopenshell_instance_t* text_literal = create_entity(file, "IfcTextLiteral");
    ifcopenshell_instance_t* group = create_entity(file, "IfcGroup");
    ifcopenshell_instance_t* layer = create_entity(file, "IfcPresentationLayerAssignment");
    ifcopenshell_instance_t* library = create_entity(file, "IfcLibraryInformation");
    ifcopenshell_instance_t* library_reference = create_entity(file, "IfcLibraryReference");
    ifcopenshell_instance_t* material = create_entity(file, "IfcMaterial");
    ifcopenshell_instance_t* constituent = create_entity(file, "IfcMaterialConstituent");
    ifcopenshell_instance_t* material_layer = create_entity(file, "IfcMaterialLayer");
    ifcopenshell_instance_t* layer_usage = create_entity(file, "IfcMaterialLayerSetUsage");
    ifcopenshell_instance_t* material_profile = create_entity(file, "IfcMaterialProfile");
    ifcopenshell_instance_t* wall = create_entity(file, "IfcWall");
    ifcopenshell_instance_t* actor = create_entity(file, "IfcActor");
    ifcopenshell_instance_t* address = create_entity(file, "IfcAddress");
    ifcopenshell_instance_t* application = create_entity(file, "IfcApplication");
    ifcopenshell_instance_t* organisation = create_entity(file, "IfcOrganization");
    ifcopenshell_instance_t* person = create_entity(file, "IfcPerson");
    ifcopenshell_instance_t* role = create_entity(file, "IfcActorRole");
    ifcopenshell_instance_t* pset_template = create_entity(file, "IfcPropertySetTemplate");
    ifcopenshell_instance_t* resource = create_entity(file, "IfcLaborResource");
    ifcopenshell_instance_t* quantity = create_entity(file, "IfcQuantityCount");
    ifcopenshell_instance_t* analysis_model = create_entity(file, "IfcStructuralAnalysisModel");
    ifcopenshell_instance_t* load = create_entity(file, "IfcStructuralLoad");
    ifcopenshell_instance_t* load_case = create_entity(file, "IfcStructuralLoadCase");
    ifcopenshell_instance_t* presentation_style = create_entity(file, "IfcSurfaceStyle");
    ifcopenshell_instance_t* system = create_entity(file, "IfcDistributionSystem");
    ifcopenshell_instance_t* derived_unit = create_entity(file, "IfcDerivedUnit");
    ifcopenshell_instance_t* monetary_unit = create_entity(file, "IfcMonetaryUnit");

    ASSERT(ifcopenshell_classification_edit_classification(file, classification, attributes), "classification edit succeeds");
    ASSERT(ifcopenshell_classification_edit_reference(file, reference, attributes), "classification reference edit succeeds");
    ASSERT(ifcopenshell_constraint_edit_metric(file, metric, attributes), "metric edit succeeds");
    ASSERT(ifcopenshell_constraint_edit_objective(file, objective, attributes), "objective edit succeeds");
    ASSERT(ifcopenshell_document_edit_information(file, information, attributes), "document information edit succeeds");
    ASSERT(ifcopenshell_document_edit_reference(file, document_reference, attributes), "document reference edit succeeds");
    ASSERT(ifcopenshell_drawing_edit_text_literal(file, text_literal, attributes), "text literal edit succeeds");
    ASSERT(ifcopenshell_group_edit_group(file, group, attributes), "group edit succeeds");
    ASSERT(ifcopenshell_layer_edit_layer(file, layer, attributes), "layer edit succeeds");
    ASSERT(ifcopenshell_library_edit_reference(file, library_reference, attributes), "library reference edit succeeds");
    ASSERT(ifcopenshell_material_edit_material(file, material, attributes), "material edit succeeds");
    ASSERT(ifcopenshell_material_edit_constituent(file, constituent, attributes, material), "constituent edit succeeds");
    ASSERT(ifcopenshell_material_edit_layer(file, material_layer, attributes, material), "material layer edit succeeds");
    ASSERT(ifcopenshell_material_edit_layer_usage(file, layer_usage, attributes), "layer usage edit succeeds");
    ASSERT(ifcopenshell_material_edit_assigned_material(file, wall, attributes), "assigned material edit succeeds");
    ASSERT(ifcopenshell_material_edit_profile(file, material_profile, attributes, NULL, material), "material profile edit succeeds");
    ASSERT(ifcopenshell_owner_edit_actor(file, actor, attributes), "actor edit succeeds");
    ASSERT(ifcopenshell_owner_edit_address(file, address, attributes), "address edit succeeds");
    ASSERT(ifcopenshell_owner_edit_application(file, application, attributes), "application edit succeeds");
    ASSERT(ifcopenshell_owner_edit_organisation(file, organisation, attributes), "organisation edit succeeds");
    ASSERT(ifcopenshell_owner_edit_person(file, person, attributes), "person edit succeeds");
    ASSERT(ifcopenshell_owner_edit_role(file, role, attributes), "role edit succeeds");
    ASSERT(ifcopenshell_profile_edit_profile(NULL, attributes) == false, "profile rejects null target");
    ASSERT(ifcopenshell_pset_template_edit_pset_template(file, pset_template, attributes), "pset template edit succeeds");
    ASSERT(ifcopenshell_resource_edit_resource(file, resource, attributes), "resource edit succeeds");
    ASSERT(ifcopenshell_resource_edit_resource_quantity(file, quantity, attributes), "resource quantity edit succeeds");
    ASSERT(ifcopenshell_structural_edit_structural_analysis_model(file, analysis_model, attributes), "analysis model edit succeeds");
    ASSERT(ifcopenshell_structural_edit_structural_load(file, load, attributes), "structural load edit succeeds");
    ASSERT(ifcopenshell_structural_edit_structural_load_case(file, load_case, attributes), "load case edit succeeds");
    ASSERT(ifcopenshell_style_edit_presentation_style(file, presentation_style, attributes), "presentation style edit succeeds");
    ASSERT(ifcopenshell_system_edit_system(file, system, attributes), "system edit succeeds");
    ASSERT(ifcopenshell_unit_edit_derived_unit(file, derived_unit, attributes), "derived unit edit succeeds");
    ASSERT(ifcopenshell_unit_edit_monetary_unit(file, monetary_unit, attributes), "monetary unit edit succeeds");

    ASSERT(ifcopenshell_pset_props_set_string(attributes, "Name", "Renamed group"), "set representative property succeeds");
    ASSERT(ifcopenshell_group_edit_group(file, group, attributes), "group property edit succeeds");
    assert_string(group, "Name", "Renamed group");
    ASSERT(!ifcopenshell_group_edit_group(file, group, NULL), "null property map reports an error");

    ifcopenshell_pset_props_free(attributes);
    ifcopenshell_instance_destroy(classification);
    ifcopenshell_instance_destroy(reference);
    ifcopenshell_instance_destroy(metric);
    ifcopenshell_instance_destroy(objective);
    ifcopenshell_instance_destroy(information);
    ifcopenshell_instance_destroy(document_reference);
    ifcopenshell_instance_destroy(text_literal);
    ifcopenshell_instance_destroy(group);
    ifcopenshell_instance_destroy(layer);
    ifcopenshell_instance_destroy(library);
    ifcopenshell_instance_destroy(library_reference);
    ifcopenshell_instance_destroy(material);
    ifcopenshell_instance_destroy(constituent);
    ifcopenshell_instance_destroy(material_layer);
    ifcopenshell_instance_destroy(layer_usage);
    ifcopenshell_instance_destroy(material_profile);
    ifcopenshell_instance_destroy(wall);
    ifcopenshell_instance_destroy(actor);
    ifcopenshell_instance_destroy(address);
    ifcopenshell_instance_destroy(application);
    ifcopenshell_instance_destroy(organisation);
    ifcopenshell_instance_destroy(person);
    ifcopenshell_instance_destroy(role);
    ifcopenshell_instance_destroy(pset_template);
    ifcopenshell_instance_destroy(resource);
    ifcopenshell_instance_destroy(quantity);
    ifcopenshell_instance_destroy(analysis_model);
    ifcopenshell_instance_destroy(load);
    ifcopenshell_instance_destroy(load_case);
    ifcopenshell_instance_destroy(presentation_style);
    ifcopenshell_instance_destroy(system);
    ifcopenshell_instance_destroy(derived_unit);
    ifcopenshell_instance_destroy(monetary_unit);
    ifcopenshell_file_destroy(file);
}

static void test_library_version_date(void) {
    ifcopenshell_file_t* ifc4 = NULL;
    ifcopenshell_file_t* ifc2x3 = NULL;
    ifcopenshell_instance_t* library4 = NULL;
    ifcopenshell_instance_t* library2x3 = NULL;
    void* attributes = NULL;
    ASSERT(ifcopenshell_parse_new_file("IFC4", 0, "", &ifc4), "new IFC4 library file succeeds");
    ASSERT(ifcopenshell_parse_new_file("IFC2X3", 0, "", &ifc2x3), "new IFC2X3 library file succeeds");
    ASSERT(ifcopenshell_file_create_entity_by_name(ifc4, "IfcLibraryInformation", &library4), "IFC4 library creation succeeds");
    ASSERT(ifcopenshell_file_create_entity_by_name(ifc2x3, "IfcLibraryInformation", &library2x3), "IFC2X3 library creation succeeds");
    ASSERT(ifcopenshell_pset_props_new(&attributes), "date property map succeeds");
    ASSERT(ifcopenshell_pset_props_set_datetime(attributes, "VersionDate", 2024, 1, 2, 3, 4, 5, 600000, true, 330), "set date-time succeeds");
    ASSERT(ifcopenshell_library_edit_library(ifc4, library4, attributes), "IFC4 library date edit succeeds");
    ASSERT(ifcopenshell_library_edit_library(ifc2x3, library2x3, attributes), "IFC2X3 library date edit succeeds");
    assert_string(library4, "VersionDate", "2024-01-02T03:04:05.600000+05:30");
    ASSERT(has_instance_attribute(library2x3, "VersionDate"), "IFC2X3 library date is a calendar entity");
    ifcopenshell_pset_props_free(attributes);
    ifcopenshell_instance_destroy(library4);
    ifcopenshell_instance_destroy(library2x3);
    ifcopenshell_file_destroy(ifc4);
    ifcopenshell_file_destroy(ifc2x3);
}

int ifcapi_run_edit_wrapper_smoke_tests(void) {
    printf("=== Named edit wrapper C ABI tests ===\n");
    tests_run = 0;
    tests_failed = 0;
    test_named_editors();
    test_library_version_date();
    printf("Named edit wrapper C ABI: %d assertions, %d failures\n", tests_run, tests_failed);
    return tests_failed;
}
