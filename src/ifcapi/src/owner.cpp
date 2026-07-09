// This file was generated with the assistance of an AI coding tool.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ifcapi/bindings/owner.h"
#include "ifcapi/detail/attribute.h"
#include "ifcopenshell_api_internal.hpp"

#include <ctime>
#include <string>

namespace {

inline void set_error(const char* msg) { ifcopenshell::capi::set_last_error(msg); }
inline void set_error(const std::string& msg) { ifcopenshell::capi::set_last_error(msg); }

bool is_ifc2x3(ifcopenshell::file* file) {
    const std::string schema = file && file->schema() ? file->schema()->name() : std::string();
    return schema == "IFC2X3";
}

int now_seconds() {
    return static_cast<int>(std::time(nullptr));
}

express::Base shallow_copy(ifcopenshell::file* file, const express::Base& source) {
    if (!file || !source) {
        return {};
    }
    const auto& declaration = source.declaration();
    auto result = file->create(&declaration);
    auto* entity_decl = declaration.as_entity();
    if (!result || !entity_decl) {
        return result;
    }

    const auto attrs = entity_decl->all_attributes();
    for (size_t i = 0; i < attrs.size(); ++i) {
        auto value = source.get_attribute_value(i);
        if (value.isNull()) {
            continue;
        }
        switch (value.type()) {
            case ifcopenshell::Argument_INT:
                result.set_attribute_value(i, static_cast<int>(value));
                break;
            case ifcopenshell::Argument_STRING:
            case ifcopenshell::Argument_ENUMERATION:
                result.set_attribute_value(i, static_cast<std::string>(value));
                break;
            case ifcopenshell::Argument_ENTITY_INSTANCE:
                result.set_attribute_value(i, static_cast<express::Base>(value));
                break;
            default:
                break;
        }
    }
    return result;
}

} // namespace

namespace ifcapi {
namespace bindings {

express::Base owner_create_owner_history(
    ifcopenshell::file* file,
    const OwnerCreateOwnerHistoryOptions& options)
{
    ifcopenshell_clear_error();
    if (!file) {
        set_error("file is NULL");
        return {};
    }
    auto user = options.user.value_or(express::Base());
    auto application = options.application.value_or(express::Base());
    if (!user || !application) {
        if (is_ifc2x3(file)) {
            set_error("IFC2X3 owner history requires an owning user and application");
        }
        return {};
    }

    try {
        const auto* decl = file->schema()->declaration_by_name("IfcOwnerHistory");
        auto owner_history = file->create(decl);
        if (!owner_history) {
            set_error("Failed to create IfcOwnerHistory");
            return {};
        }
        const int now = now_seconds();
        ifcapi::detail::write_ref_attr(owner_history, "OwningUser", user);
        ifcapi::detail::write_ref_attr(owner_history, "OwningApplication", application);
        ifcapi::detail::write_enum_attr(owner_history, "State", "READWRITE");
        ifcapi::detail::write_enum_attr(owner_history, "ChangeAction", "ADDED");
        ifcapi::detail::write_int_attr(owner_history, "LastModifiedDate", now);
        ifcapi::detail::write_ref_attr(owner_history, "LastModifyingUser", user);
        ifcapi::detail::write_ref_attr(owner_history, "LastModifyingApplication", application);
        ifcapi::detail::write_int_attr(owner_history, "CreationDate", now);
        return owner_history;
    } catch (const std::exception& ex) {
        set_error(ex.what());
        return {};
    } catch (...) {
        set_error("Unknown C++ exception in owner_create_owner_history");
        return {};
    }
}

express::Base owner_update_owner_history(
    ifcopenshell::file* file,
    const OwnerUpdateOwnerHistoryOptions& options)
{
    ifcopenshell_clear_error();
    if (!file) {
        set_error("file is NULL");
        return {};
    }
    auto element = options.element.value_or(express::Base());
    auto user = options.user.value_or(express::Base());
    auto application = options.application.value_or(express::Base());
    if (!element || !element.declaration().is("IfcRoot") || !user || !application) {
        return {};
    }

    try {
        auto owner_history = ifcapi::detail::read_ref_attr(element, "OwnerHistory");
        if (!owner_history) {
            owner_history = owner_create_owner_history(file, OwnerCreateOwnerHistoryOptions{user, application});
            if (owner_history) {
                ifcapi::detail::write_ref_attr(element, "OwnerHistory", owner_history);
            }
            return owner_history;
        }

        if (ifcapi::detail::total_inverses(file, owner_history) > 1) {
            owner_history = shallow_copy(file, owner_history);
            if (owner_history) {
                ifcapi::detail::write_ref_attr(element, "OwnerHistory", owner_history);
            }
        }

        if (!owner_history) {
            return {};
        }
        ifcapi::detail::write_enum_attr(owner_history, "ChangeAction", "MODIFIED");
        ifcapi::detail::write_int_attr(owner_history, "LastModifiedDate", now_seconds());
        ifcapi::detail::write_ref_attr(owner_history, "LastModifyingUser", user);
        ifcapi::detail::write_ref_attr(owner_history, "LastModifyingApplication", application);
        return owner_history;
    } catch (const std::exception& ex) {
        set_error(ex.what());
        return {};
    } catch (...) {
        set_error("Unknown C++ exception in owner_update_owner_history");
        return {};
    }
}

} // namespace bindings
} // namespace ifcapi
