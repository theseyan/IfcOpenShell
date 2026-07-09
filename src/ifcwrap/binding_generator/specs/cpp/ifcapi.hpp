// This file was generated with the assistance of an AI coding tool.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef IFCWRAP_BINDING_GENERATOR_IFCAPI_SPEC_HPP
#define IFCWRAP_BINDING_GENERATOR_IFCAPI_SPEC_HPP

#include "spec_macros.h"

#include "ifcapi/bindings/aggregate.h"
#include "ifcapi/bindings/attribute.h"
#include "ifcapi/bindings/boundary.h"
#include "ifcapi/bindings/classification.h"
#include "ifcapi/bindings/cogo.h"
#include "ifcapi/bindings/constraint.h"
#include "ifcapi/bindings/control.h"
#include "ifcapi/bindings/context.h"
#include "ifcapi/bindings/cost.h"
#include "ifcapi/bindings/document.h"
#include "ifcapi/bindings/drawing.h"
#include "ifcapi/bindings/element.h"
#include "ifcapi/bindings/entity.h"
#include "ifcapi/bindings/feature.h"
#include "ifcapi/bindings/georeference.h"
#include "ifcapi/bindings/geometry.h"
#include "ifcapi/bindings/group.h"
#include "ifcapi/bindings/grid.h"
#include "ifcapi/bindings/layer.h"
#include "ifcapi/bindings/library.h"
#include "ifcapi/bindings/material.h"
#include "ifcapi/bindings/nest.h"
#include "ifcapi/bindings/owner.h"
#include "ifcapi/bindings/placement.h"
#include "ifcapi/bindings/profile.h"
#include "ifcapi/bindings/project.h"
#include "ifcapi/bindings/pset.h"
#include "ifcapi/bindings/pset_template.h"
#include "ifcapi/bindings/representation.h"
#include "ifcapi/bindings/resource.h"
#include "ifcapi/bindings/root.h"
#include "ifcapi/bindings/schema.h"
#include "ifcapi/bindings/sequence.h"
#include "ifcapi/bindings/selector.h"
#include "ifcapi/bindings/shape.h"
#include "ifcapi/bindings/shape_builder.h"
#include "ifcapi/bindings/spatial.h"
#include "ifcapi/bindings/style.h"
#include "ifcapi/bindings/structural.h"
#include "ifcapi/bindings/system.h"
#include "ifcapi/bindings/type.h"
#include "ifcapi/bindings/unit.h"
#include "ifcapi/bindings/value.h"

#include <string>
#include <vector>

namespace ifcapi {
std::string guid_new();
std::string guid_compress(const std::string& uuid_hex);
std::string guid_expand(const std::string& guid);
} // namespace ifcapi

IFCAPI_HANDLE(value, ifcopenshell_selector_value_t, function:ifcapi::bindings::value_free)
IFCAPI_HANDLE(pset_template_handle, ifcopenshell_pset_template_t, function:ifcapi::bindings::pset_template_free)

namespace ifcapi::bindings {

IFCAPI_RESULT_STRUCT(ifcapi::bindings::ShapeBuilderMepTransitionShapeResult)
struct ifcopenshell_shape_builder_mep_transition_shape_result_t {
    express::Base representation;
    double start_length;
    double end_length;
    double angle;
    std::vector<double> profile_offset;
    double transition_length;
    double full_transition_length;
};

IFCAPI_RESULT_STRUCT(ifcapi::bindings::ShapeBuilderMepBendShapeResult)
struct ifcopenshell_shape_builder_mep_bend_shape_result_t {
    express::Base representation;
    double start_length;
    double end_length;
    double radius;
    double angle;
    int lateral_axis;
    double lateral_sign;
    int z_axis_sign;
    double main_profile_dimension;
};

IFCAPI_RESULT_STRUCT(ifcapi::bindings::SequenceDuplicateTaskResult)
struct ifcopenshell_sequence_duplicate_task_result_t {
    std::vector<express::Base> current;
    std::vector<express::Base> duplicate;
};

inline IFCAPI_OWNED std::string guid_new() {
    return ::ifcapi::guid_new();
}

inline IFCAPI_OWNED std::string guid_compress(const std::string& uuid_hex) {
    return ::ifcapi::guid_compress(uuid_hex);
}

inline IFCAPI_OWNED std::string guid_expand(const std::string& guid) {
    return ::ifcapi::guid_expand(guid);
}

} // namespace ifcapi::bindings

#endif // IFCWRAP_BINDING_GENERATOR_IFCAPI_SPEC_HPP
