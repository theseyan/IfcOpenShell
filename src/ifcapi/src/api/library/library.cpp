// This file was generated with the assistance of an AI coding tool.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ifcapi/bindings/library.h"
#include "ifcapi/detail/attribute.h"
#include "ifcapi/detail/relationship.h"
#include "guid.h"

#include "ifcparse/file.h"
#include "ifcparse/schema.h"

#include <algorithm>
#include <regex>
#include <stdexcept>
#include <string>
#include <unordered_set>
#include <vector>

namespace {

bool is_ifc2x3(ifcopenshell::file* file) {
    return file && file->schema() && file->schema()->name() == "IFC2X3";
}

struct IsoDate {
    int year;
    int month;
    int day;
};

bool leap_year(int year) {
    return year % 4 == 0 && (year % 100 != 0 || year % 400 == 0);
}

IsoDate parse_iso_date_time(const std::string& value) {
    static const std::regex pattern(
        R"(^([0-9]{4})-([0-9]{2})-([0-9]{2})T([0-9]{2}):([0-9]{2}):([0-9]{2})(\.[0-9]{1,6})?(Z|([+-])([0-9]{2}):([0-9]{2})(:([0-9]{2})(\.[0-9]{1,6})?)?)?$)");
    std::smatch match;
    if (!std::regex_match(value, match, pattern)) {
        throw std::invalid_argument("Invalid ISO date-time");
    }
    const int year = std::stoi(match[1].str());
    const int month = std::stoi(match[2].str());
    const int day = std::stoi(match[3].str());
    const int hour = std::stoi(match[4].str());
    const int minute = std::stoi(match[5].str());
    const int second = std::stoi(match[6].str());
    static const int month_days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    const int max_day = month >= 1 && month <= 12
        ? month_days[month - 1] + (month == 2 && leap_year(year) ? 1 : 0)
        : 0;
    if (year == 0 || day < 1 || day > max_day || hour >= 24 || minute >= 60 || second >= 60) {
        throw std::invalid_argument("Invalid ISO date-time");
    }
    if (match[9].matched) {
        const int offset_hour = std::stoi(match[10].str());
        const int offset_minute = std::stoi(match[11].str());
        const int offset_second = match[13].matched ? std::stoi(match[13].str()) : 0;
        if (offset_hour >= 24 || offset_minute >= 60 || offset_second >= 60) {
            throw std::invalid_argument("Invalid ISO date-time");
        }
    }
    return {year, month, day};
}

std::vector<express::Base> mutable_entities(
    const std::vector<express::Base>& entities)
{
    std::vector<express::Base> result;
    result.reserve(entities.size());
    for (auto entity : entities) if (entity) result.push_back(entity);
    return result;
}

std::vector<express::Base> inverse_entities(express::Base entity, const char* attribute) {
    return ifcapi::detail::read_inverse_aggregate(entity, attribute);
}

std::vector<express::Base> library_association_rels(
    ifcopenshell::file* file,
    express::Base reference)
{
    std::vector<express::Base> result;
    if (!file || !reference) return result;
    if (is_ifc2x3(file)) {
        auto rels = ifcapi::detail::instances_by_type(file, "IfcRelAssociatesLibrary");
        for (auto rel : rels) {
            if (ifcapi::detail::read_ref_attr(rel, "RelatingLibrary") == reference) {
                result.push_back(rel);
            }
        }
        return result;
    }
    return inverse_entities(reference, "LibraryRefForObjects");
}

std::vector<express::Base> referenced_elements(
    ifcopenshell::file* file,
    express::Base reference)
{
    std::vector<express::Base> result;
    std::unordered_set<express::Base> seen;
    for (auto rel : library_association_rels(file, reference)) {
        for (auto object : ifcapi::detail::read_ref_aggregate(rel, "RelatedObjects")) {
            if (object && seen.insert(object).second) result.push_back(object);
        }
    }
    return result;
}

std::vector<express::Base> products_not_already_referenced(
    const std::vector<express::Base>& products,
    const std::vector<express::Base>& referenced)
{
    std::unordered_set<express::Base> referenced_set(referenced.begin(), referenced.end());
    std::unordered_set<express::Base> seen;
    std::vector<express::Base> result;
    for (auto product : products) {
        if (product && referenced_set.find(product) == referenced_set.end() && seen.insert(product).second) {
            result.push_back(product);
        }
    }
    return result;
}

express::Base create_rel_associates_library(
    ifcopenshell::file* file,
    const std::vector<express::Base>& products,
    express::Base reference,
    express::Base owner_history,
    express::Base user,
    express::Base application)
{
    const auto* declaration = file->schema()->declaration_by_name("IfcRelAssociatesLibrary");
    auto rel = file->create(declaration);
    auto* entity = declaration->as_entity();
    int guid_idx = ifcapi::detail::find_attr_index(entity, "GlobalId");
    if (guid_idx >= 0) rel.set_attribute_value(static_cast<size_t>(guid_idx), ifcapi::guid_new());
    ifcapi::detail::set_ref(rel, ifcapi::detail::find_attr_index(entity, "OwnerHistory"),
        ifcapi::detail::ensure_owner_history(file, owner_history, user, application));
    ifcapi::detail::set_ref_aggregate(rel, ifcapi::detail::find_attr_index(entity, "RelatedObjects"), products);
    ifcapi::detail::set_ref(rel, ifcapi::detail::find_attr_index(entity, "RelatingLibrary"), reference);
    return rel;
}

void append_reference(express::Base library, express::Base reference) {
    auto references = ifcapi::detail::read_ref_aggregate(library, "LibraryReference");
    references.push_back(reference);
    ifcapi::detail::write_ref_aggregate(library, "LibraryReference", references);
}

void remove_library_reference_rels(ifcopenshell::file* file, express::Base reference) {
    auto rels = library_association_rels(file, reference);
    for (auto rel : rels) {
        ifcapi::detail::remove_with_history(file, rel);
    }
}

} // namespace

namespace ifcapi {
namespace bindings {

express::Base library_add_library(
    ifcopenshell::file* file,
    const std::string& name)
{
    const auto* declaration = file->schema()->declaration_by_name("IfcLibraryInformation");
    auto library = file->create(declaration);
    detail::write_string_attr(library, "Name", name);
    return library;
}

void library_edit_version_date(
    ifcopenshell::file* file,
    express::Base* library_ptr,
    const std::string& iso_date_time)
{
    auto library = detail::deref_or_empty(library_ptr);
    if (!file || !library || !library.declaration().is("IfcLibraryInformation")
        || !detail::exists_in_file(file, library)) {
        throw std::invalid_argument("Invalid IfcLibraryInformation");
    }
    const auto date = parse_iso_date_time(iso_date_time);
    if (!is_ifc2x3(file)) {
        detail::write_string_attr(library, "VersionDate", iso_date_time);
        return;
    }

    const auto* declaration = detail::declaration_by_name(file, "IfcCalendarDate");
    if (!declaration) throw std::invalid_argument("IfcCalendarDate is not supported by this schema");
    auto calendar_date = file->create(declaration);
    detail::write_int_attr(calendar_date, "DayComponent", date.day);
    detail::write_int_attr(calendar_date, "MonthComponent", date.month);
    detail::write_int_attr(calendar_date, "YearComponent", date.year);
    detail::write_ref_attr(library, "VersionDate", calendar_date);
}

express::Base library_add_reference(
    ifcopenshell::file* file,
    express::Base* library)
{
    auto library_value = detail::deref_or_empty(library);
    const auto* declaration = file->schema()->declaration_by_name("IfcLibraryReference");
    auto reference = file->create(declaration);
    if (is_ifc2x3(file)) {
        append_reference(library_value, reference);
    } else {
        detail::write_ref_attr(reference, "ReferencedLibrary", library_value);
    }
    return reference;
}

express::Base library_assign_reference(
    ifcopenshell::file* file,
    const LibraryAssignReferenceOptions& options)
{
    auto owner_history = options.owner_history.value_or(express::Base());
    auto user = options.user.value_or(express::Base());
    auto application = options.application.value_or(express::Base());
    auto reference = options.reference;

    auto product_vec = mutable_entities(options.products);
    auto products_to_add = products_not_already_referenced(product_vec, referenced_elements(file, reference));
    if (products_to_add.empty()) return {};

    auto rels = library_association_rels(file, reference);
    express::Base rel = rels.empty() ? express::Base() : rels.front();
    if (!rel) {
        return create_rel_associates_library(
            file, products_to_add, reference, owner_history, user, application);
    }

    auto related = detail::read_ref_aggregate(rel, "RelatedObjects");
    std::unordered_set<express::Base> seen(related.begin(), related.end());
    for (auto product : products_to_add) {
        if (seen.insert(product).second) related.push_back(product);
    }
    detail::write_ref_aggregate(rel, "RelatedObjects", related);
    detail::update_owner_history(file, rel, user, application);
    return rel;
}

void library_unassign_reference(
    ifcopenshell::file* file,
    const LibraryUnassignReferenceOptions& options)
{
    auto user = options.user.value_or(express::Base());
    auto application = options.application.value_or(express::Base());
    auto reference = options.reference;

    auto product_vec = mutable_entities(options.products);
    std::unordered_set<express::Base> products_set(product_vec.begin(), product_vec.end());
    std::vector<express::Base> rels;
    std::unordered_set<express::Base> seen_rels;
    for (auto product : product_vec) {
        for (auto rel : inverse_entities(product, "HasAssociations")) {
            if (rel && rel.declaration().is("IfcRelAssociatesLibrary")
                && detail::read_ref_attr(rel, "RelatingLibrary") == reference
                && seen_rels.insert(rel).second) {
                rels.push_back(rel);
            }
        }
    }

    for (auto rel : rels) {
        std::vector<express::Base> remaining;
        for (auto object : detail::read_ref_aggregate(rel, "RelatedObjects")) {
            if (products_set.find(object) == products_set.end()) remaining.push_back(object);
        }
        if (remaining.empty()) {
            detail::remove_with_history(file, rel);
        } else {
            detail::write_ref_aggregate(rel, "RelatedObjects", remaining);
            detail::update_owner_history(file, rel, user, application);
        }
    }
}

void library_remove_reference(
    ifcopenshell::file* file,
    express::Base* reference)
{
    auto reference_value = detail::deref_or_empty(reference);
    remove_library_reference_rels(file, reference_value);
    file->remove_entity(reference_value);
}

void library_remove_library(
    ifcopenshell::file* file,
    express::Base* library)
{
    auto library_value = detail::deref_or_empty(library);
    std::vector<express::Base> references;
    std::vector<express::Base> rels;
    if (is_ifc2x3(file)) {
        references = detail::read_ref_aggregate(library_value, "LibraryReference");
        rels = library_association_rels(file, library_value);
        for (auto reference : references) {
            auto reference_rels = library_association_rels(file, reference);
            rels.insert(rels.end(), reference_rels.begin(), reference_rels.end());
        }
    } else {
        references = inverse_entities(library_value, "HasLibraryReferences");
        for (auto reference : references) {
            auto reference_rels = inverse_entities(reference, "LibraryRefForObjects");
            rels.insert(rels.end(), reference_rels.begin(), reference_rels.end());
        }
        auto library_rels = inverse_entities(library_value, "LibraryInfoForObjects");
        rels.insert(rels.end(), library_rels.begin(), library_rels.end());
    }

    for (auto reference : references) {
        file->remove_entity(reference);
    }
    file->remove_entity(library_value);

    std::unordered_set<express::Base> seen;
    for (auto rel : rels) {
        if (rel && seen.insert(rel).second) {
            detail::remove_with_history(file, rel);
        }
    }
}

} // namespace bindings
} // namespace ifcapi
