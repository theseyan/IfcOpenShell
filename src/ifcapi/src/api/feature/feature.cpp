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

bool is_instance(IfcUtil::IfcBaseClass* entity, const char* ifc_class) {
    return entity && entity->declaration().is(ifc_class);
}

IfcUtil::IfcBaseClass* relationship_host(IfcUtil::IfcBaseClass* rel) {
    if (auto* element = ifcapi::detail::read_ref_attr(rel, "RelatingBuildingElement")) return element;
    return ifcapi::detail::read_ref_attr(rel, "RelatingElement");
}

IfcUtil::IfcBaseClass* first_rel(const std::vector<IfcUtil::IfcBaseClass*>& rels) {
    return rels.empty() ? nullptr : rels.front();
}
}

namespace ifcapi {
namespace bindings {

IfcUtil::IfcBaseClass* feature_add_feature(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* feature,
    IfcUtil::IfcBaseClass* element,
    IfcUtil::IfcBaseClass* user,
    IfcUtil::IfcBaseClass* application)
{
    ifcopenshell_clear_error();
    try {
        std::vector<IfcUtil::IfcBaseClass*> rels;
        const char* ifc_class = nullptr;
        const char* relating_attr = nullptr;
        const char* related_attr = nullptr;

        if (is_instance(feature, "IfcFeatureElementSubtraction")) {
            rels = ifcapi::detail::read_inverse_aggregate(feature, "VoidsElements");
            ifc_class = "IfcRelVoidsElement";
            relating_attr = "RelatingBuildingElement";
            related_attr = "RelatedOpeningElement";
        } else if (is_instance(feature, "IfcFeatureElementAddition")) {
            rels = ifcapi::detail::read_inverse_aggregate(feature, "ProjectsElements");
            ifc_class = "IfcRelProjectsElement";
            relating_attr = "RelatingElement";
            related_attr = "RelatedFeatureElement";
        } else if (is_instance(feature, "IfcSurfaceFeature")) {
            if (file && file->schema()->name() == "IFC4") {
                return aggregate_assign_object(file, {feature}, element, nullptr, user, application);
            }
            rels = ifcapi::detail::read_inverse_aggregate(feature, "AdheresToElement");
            ifc_class = "IfcRelAdheresToElement";
            relating_attr = "RelatingElement";
            related_attr = "RelatedSurfaceFeatures";
        }

        if (!ifc_class) {
            set_error("Unsupported feature type");
            return nullptr;
        }

        if (auto* rel = first_rel(rels)) {
            if (relationship_host(rel) == element) {
                return rel;
            }
            if (std::string(ifc_class) == "IfcRelAdheresToElement" &&
                ifcapi::detail::read_ref_aggregate(rel, "RelatedSurfaceFeatures").size() != 1) {
                auto related = ifcapi::detail::read_ref_aggregate(rel, "RelatedSurfaceFeatures");
                related.erase(std::remove(related.begin(), related.end(), feature), related.end());
                ifcapi::detail::write_ref_aggregate(rel, "RelatedSurfaceFeatures", related);
            } else {
                ifcapi::detail::remove_with_history(file, rel);
            }
        }

        auto* rel = file->create(file->schema()->declaration_by_name(ifc_class));
        ifcapi::detail::write_string_attr(rel, "GlobalId", ifcapi::guid_new());
        ifcapi::detail::write_ref_attr(
            rel,
            "OwnerHistory",
            ifcapi::detail::ensure_owner_history(file, nullptr, user, application));
        ifcapi::detail::write_ref_attr(rel, relating_attr, element);
        if (std::string(ifc_class) == "IfcRelVoidsElement") {
            ifcapi::detail::write_ref_attr(rel, related_attr, feature);
        } else if (std::string(ifc_class) == "IfcRelProjectsElement") {
            ifcapi::detail::write_ref_attr(rel, related_attr, feature);
        } else {
            ifcapi::detail::write_ref_aggregate(rel, related_attr, {feature});
        }

        auto* placement = ifcapi::detail::read_ref_attr(feature, "ObjectPlacement");
        if (is_instance(placement, "IfcLocalPlacement")) {
            geometry_edit_object_placement(file, feature, placement_get_local_placement(placement), false, false);
        }
        return rel;
    } catch (const std::exception& e) {
        set_error(e.what());
        return nullptr;
    }
}

IfcUtil::IfcBaseClass* feature_add_filling(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* opening,
    IfcUtil::IfcBaseClass* element)
{
    ifcopenshell_clear_error();
    try {
        auto fills_voids = ifcapi::detail::read_inverse_aggregate(element, "FillsVoids");
        if (auto* rel = first_rel(fills_voids)) {
            if (ifcapi::detail::read_ref_attr(rel, "RelatingOpeningElement") == opening) {
                return rel;
            }
            ifcapi::detail::remove_with_history(file, rel);
        }
        auto* rel = file->create(file->schema()->declaration_by_name("IfcRelFillsElement"));
        ifcapi::detail::write_string_attr(rel, "GlobalId", ifcapi::guid_new());
        ifcapi::detail::write_ref_attr(rel, "RelatingOpeningElement", opening);
        ifcapi::detail::write_ref_attr(rel, "RelatedBuildingElement", element);
        return rel;
    } catch (const std::exception& e) {
        set_error(e.what());
        return nullptr;
    }
}

void feature_remove_feature(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* feature,
    IfcUtil::IfcBaseClass* user,
    IfcUtil::IfcBaseClass* application)
{
    ifcopenshell_clear_error();
    try {
        std::vector<IfcUtil::IfcBaseClass*> rels;
        if (is_instance(feature, "IfcFeatureElementSubtraction")) {
            rels = ifcapi::detail::read_inverse_aggregate(feature, "VoidsElements");
        } else if (is_instance(feature, "IfcFeatureElementAddition")) {
            rels = ifcapi::detail::read_inverse_aggregate(feature, "ProjectsElements");
        } else if (is_instance(feature, "IfcSurfaceFeature")) {
            if (file && file->schema()->name() == "IFC4") {
                aggregate_unassign_object(file, {feature}, user, application);
            } else {
                // Upstream queries the absent ProjectsElements inverse here; preserve its empty-rel behavior explicitly.
                rels = {};
            }
        }
        for (auto* rel : rels) {
            ifcapi::detail::remove_with_history(file, rel);
        }
        if (is_instance(feature, "IfcOpeningElement")) {
            for (auto* rel : ifcapi::detail::read_inverse_aggregate(feature, "HasFillings")) {
                ifcapi::detail::remove_with_history(file, rel);
            }
        }
        root_remove_product(file, feature, user, application);
    } catch (const std::exception& e) {
        set_error(e.what());
    }
}

void feature_remove_filling(IfcParse::IfcFile* file, IfcUtil::IfcBaseClass* element) {
    ifcopenshell_clear_error();
    try {
        for (auto* rel : ifcapi::detail::instances_by_type(file, "IfcRelFillsElement")) {
            if (ifcapi::detail::read_ref_attr(rel, "RelatedBuildingElement") == element) {
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
