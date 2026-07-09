// This file was generated with the assistance of an AI coding tool.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ifcapi/bindings/aggregate.h"
#include "ifcapi/bindings/feature.h"
#include "ifcapi/bindings/geometry.h"
#include "ifcapi/bindings/placement.h"
#include "ifcapi/bindings/root.h"
#include "ifcapi/detail/attribute.h"
#include "ifcapi/detail/relationship.h"
#include "guid.h"
#include "ifcopenshell_api_internal.hpp"

#include "ifcparse/schema.h"

#include <algorithm>
#include <string>

namespace {
inline void set_error(const std::string& msg) { ifcopenshell::capi::set_last_error(msg); }

bool is_instance(express::Base entity, const char* ifc_class) {
    return entity && entity.declaration().is(ifc_class);
}

express::Base relationship_host(express::Base rel) {
    if (auto element = ifcapi::detail::read_ref_attr(rel, "RelatingBuildingElement")) return element;
    return ifcapi::detail::read_ref_attr(rel, "RelatingElement");
}

express::Base first_rel(const std::vector<express::Base>& rels) {
    return rels.empty() ? express::Base() : rels.front();
}
}

namespace ifcapi {
namespace bindings {

express::Base feature_add_feature(
    ifcopenshell::file* file,
    const FeatureAddFeatureOptions& options)
{
    ifcopenshell_clear_error();
    try {
        auto feature_value = options.feature;
        auto element_value = options.element;
        auto user_value = options.user.value_or(express::Base());
        auto application_value = options.application.value_or(express::Base());
        std::vector<express::Base> rels;
        const char* ifc_class = nullptr;
        const char* relating_attr = nullptr;
        const char* related_attr = nullptr;

        if (is_instance(feature_value, "IfcFeatureElementSubtraction")) {
            rels = ifcapi::detail::read_inverse_aggregate(feature_value, "VoidsElements");
            ifc_class = "IfcRelVoidsElement";
            relating_attr = "RelatingBuildingElement";
            related_attr = "RelatedOpeningElement";
        } else if (is_instance(feature_value, "IfcFeatureElementAddition")) {
            rels = ifcapi::detail::read_inverse_aggregate(feature_value, "ProjectsElements");
            ifc_class = "IfcRelProjectsElement";
            relating_attr = "RelatingElement";
            related_attr = "RelatedFeatureElement";
        } else if (is_instance(feature_value, "IfcSurfaceFeature")) {
            if (file && file->schema()->name() == "IFC4") {
                AggregateAssignObjectOptions agg_options;
                agg_options.products = {feature_value};
                agg_options.relating_object = element_value;
                if (user_value) agg_options.user = user_value;
                if (application_value) agg_options.application = application_value;
                return aggregate_assign_object(file, agg_options);
            }
            rels = ifcapi::detail::read_inverse_aggregate(feature_value, "AdheresToElement");
            ifc_class = "IfcRelAdheresToElement";
            relating_attr = "RelatingElement";
            related_attr = "RelatedSurfaceFeatures";
        }

        if (!ifc_class) {
            set_error("Unsupported feature type");
            return {};
        }

        if (auto rel = first_rel(rels)) {
            if (relationship_host(rel) == element_value) {
                return rel;
            }
            if (std::string(ifc_class) == "IfcRelAdheresToElement" &&
                ifcapi::detail::read_ref_aggregate(rel, "RelatedSurfaceFeatures").size() != 1) {
                auto related = ifcapi::detail::read_ref_aggregate(rel, "RelatedSurfaceFeatures");
                related.erase(std::remove(related.begin(), related.end(), feature_value), related.end());
                ifcapi::detail::write_ref_aggregate(rel, "RelatedSurfaceFeatures", related);
            } else {
                ifcapi::detail::remove_with_history(file, rel);
            }
        }

        auto rel = file->create(file->schema()->declaration_by_name(ifc_class));
        ifcapi::detail::write_string_attr(rel, "GlobalId", ifcapi::guid_new());
        ifcapi::detail::write_ref_attr(
            rel,
            "OwnerHistory",
            ifcapi::detail::ensure_owner_history(file, {}, user_value, application_value));
        ifcapi::detail::write_ref_attr(rel, relating_attr, element_value);
        if (std::string(ifc_class) == "IfcRelVoidsElement") {
            ifcapi::detail::write_ref_attr(rel, related_attr, feature_value);
        } else if (std::string(ifc_class) == "IfcRelProjectsElement") {
            ifcapi::detail::write_ref_attr(rel, related_attr, feature_value);
        } else {
            ifcapi::detail::write_ref_aggregate(rel, related_attr, {feature_value});
        }

        auto placement = ifcapi::detail::read_ref_attr(feature_value, "ObjectPlacement");
        if (is_instance(placement, "IfcLocalPlacement")) {
            geometry_edit_object_placement(
                file,
                GeometryEditObjectPlacementOptions{feature_value, placement_get_local_placement(placement), false, false});
        }
        return rel;
    } catch (const std::exception& e) {
        set_error(e.what());
        return {};
    }
}

express::Base feature_add_filling(
    ifcopenshell::file* file,
    express::Base* opening,
    express::Base* element)
{
    ifcopenshell_clear_error();
    try {
        auto opening_value = ifcapi::detail::deref_or_empty(opening);
        auto element_value = ifcapi::detail::deref_or_empty(element);
        auto fills_voids = ifcapi::detail::read_inverse_aggregate(element_value, "FillsVoids");
        if (auto rel = first_rel(fills_voids)) {
            if (ifcapi::detail::read_ref_attr(rel, "RelatingOpeningElement") == opening_value) {
                return rel;
            }
            ifcapi::detail::remove_with_history(file, rel);
        }
        auto rel = file->create(file->schema()->declaration_by_name("IfcRelFillsElement"));
        ifcapi::detail::write_string_attr(rel, "GlobalId", ifcapi::guid_new());
        ifcapi::detail::write_ref_attr(rel, "RelatingOpeningElement", opening_value);
        ifcapi::detail::write_ref_attr(rel, "RelatedBuildingElement", element_value);
        return rel;
    } catch (const std::exception& e) {
        set_error(e.what());
        return {};
    }
}

void feature_remove_feature(
    ifcopenshell::file* file,
    const FeatureRemoveFeatureOptions& options)
{
    ifcopenshell_clear_error();
    try {
        auto feature_value = options.feature;
        auto user_value = options.user.value_or(express::Base());
        auto application_value = options.application.value_or(express::Base());
        std::vector<express::Base> rels;
        if (is_instance(feature_value, "IfcFeatureElementSubtraction")) {
            rels = ifcapi::detail::read_inverse_aggregate(feature_value, "VoidsElements");
        } else if (is_instance(feature_value, "IfcFeatureElementAddition")) {
            rels = ifcapi::detail::read_inverse_aggregate(feature_value, "ProjectsElements");
        } else if (is_instance(feature_value, "IfcSurfaceFeature")) {
            if (file && file->schema()->name() == "IFC4") {
                AggregateUnassignObjectOptions agg_options;
                agg_options.products = {feature_value};
                if (user_value) agg_options.user = user_value;
                if (application_value) agg_options.application = application_value;
                aggregate_unassign_object(file, agg_options);
            } else {
                // Upstream queries the absent ProjectsElements inverse here; preserve its empty-rel behavior explicitly.
                rels = {};
            }
        }
        for (auto rel : rels) {
            ifcapi::detail::remove_with_history(file, rel);
        }
        if (is_instance(feature_value, "IfcOpeningElement")) {
            for (auto rel : ifcapi::detail::read_inverse_aggregate(feature_value, "HasFillings")) {
                ifcapi::detail::remove_with_history(file, rel);
            }
        }
        RootRemoveProductOptions remove_options;
        if (user_value) remove_options.user = user_value;
        if (application_value) remove_options.application = application_value;
        root_remove_product(file, &feature_value, remove_options);
    } catch (const std::exception& e) {
        set_error(e.what());
    }
}

void feature_remove_filling(ifcopenshell::file* file, express::Base* element) {
    ifcopenshell_clear_error();
    try {
        auto element_value = ifcapi::detail::deref_or_empty(element);
        for (auto rel : ifcapi::detail::instances_by_type(file, "IfcRelFillsElement")) {
            if (ifcapi::detail::read_ref_attr(rel, "RelatedBuildingElement") == element_value) {
                ifcapi::detail::remove_with_history(file, rel);
                break;
            }
        }
    } catch (const std::exception& e) {
        set_error(e.what());
    }
}

} // namespace bindings
} // namespace ifcapi
