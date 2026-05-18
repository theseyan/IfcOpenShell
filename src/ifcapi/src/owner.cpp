// This file was generated with the assistance of an AI coding tool.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ifcapi/bindings/owner.h"
#include "guid.h"
#include "ifcopenshell_api_internal.hpp"

#include "ifcparse/IfcBaseClass.h"
#include "ifcparse/IfcEntityInstanceData.h"
#include "ifcparse/IfcFile.h"
#include "ifcparse/IfcSchema.h"

#include <algorithm>
#include <ctime>
#include <string>

namespace {

inline void set_error(const char* msg) { ifcopenshell::capi::set_last_error(msg); }
inline void set_error(const std::string& msg) { ifcopenshell::capi::set_last_error(msg); }

bool is_ifc2x3(IfcParse::IfcFile* file) {
    const std::string schema = file && file->schema() ? file->schema()->name() : std::string();
    return schema == "IFC2X3";
}

int attr_index(IfcUtil::IfcBaseClass* entity, const char* name) {
    if (!entity) return -1;
    auto* decl = entity->declaration().as_entity();
    if (!decl) return -1;
    try {
        return decl->attribute_index(name);
    } catch (...) {
        return -1;
    }
}

void set_ref(IfcUtil::IfcBaseClass* entity, const char* name, IfcUtil::IfcBaseClass* value) {
    const int idx = attr_index(entity, name);
    if (idx >= 0) {
        if (value) {
            entity->set_attribute_value(static_cast<size_t>(idx), value);
        } else {
            entity->set_attribute_value(static_cast<size_t>(idx), Blank{});
        }
    }
}

void set_int(IfcUtil::IfcBaseClass* entity, const char* name, int value) {
    const int idx = attr_index(entity, name);
    if (idx >= 0) {
        entity->set_attribute_value(static_cast<size_t>(idx), value);
    }
}

const IfcParse::enumeration_type* resolve_enum_type(IfcUtil::IfcBaseClass* entity, const char* name) {
    if (!entity) return nullptr;
    auto* entity_decl = entity->declaration().as_entity();
    if (!entity_decl) return nullptr;
    const int idx = attr_index(entity, name);
    if (idx < 0) return nullptr;
    auto attrs = entity_decl->all_attributes();
    if (static_cast<size_t>(idx) >= attrs.size()) return nullptr;
    const IfcParse::parameter_type* pt = attrs[static_cast<size_t>(idx)]->type_of_attribute();
    while (pt) {
        auto* named = pt->as_named_type();
        if (!named) break;
        auto* decl = named->declared_type();
        if (auto* enum_type = decl->as_enumeration_type()) return enum_type;
        if (auto* type_decl = decl->as_type_declaration()) {
            pt = type_decl->declared_type();
        } else {
            break;
        }
    }
    return nullptr;
}

void set_enum(IfcUtil::IfcBaseClass* entity, const char* name, const std::string& value) {
    const int idx = attr_index(entity, name);
    if (idx < 0) return;
    auto* enum_type = resolve_enum_type(entity, name);
    if (!enum_type) {
        entity->set_attribute_value(static_cast<size_t>(idx), value);
        return;
    }
    const auto& items = enum_type->enumeration_items();
    auto it = std::find(items.begin(), items.end(), value);
    if (it != items.end()) {
        entity->set_attribute_value(
            static_cast<size_t>(idx),
            EnumerationReference(enum_type, static_cast<size_t>(std::distance(items.begin(), it))));
    }
}

IfcUtil::IfcBaseClass* get_ref_attr(IfcUtil::IfcBaseClass* entity, const char* name) {
    const int idx = attr_index(entity, name);
    if (idx < 0) return nullptr;
    try {
        auto value = entity->get_attribute_value(static_cast<size_t>(idx));
        if (value.isNull()) return nullptr;
        return static_cast<IfcUtil::IfcBaseClass*>(value);
    } catch (...) {
        return nullptr;
    }
}

bool is_a(IfcParse::IfcFile* file, IfcUtil::IfcBaseClass* entity, const char* type_name) {
    if (!file || !entity) return false;
    try {
        const auto* decl = file->schema()->declaration_by_name(type_name);
        return entity->declaration().is(*decl);
    } catch (...) {
        return false;
    }
}

size_t total_inverses(IfcParse::IfcFile* file, IfcUtil::IfcBaseClass* entity) {
    if (!file || !entity) return 0;
    try {
        return file->getInverse(entity->id(), nullptr, -1)->size();
    } catch (...) {
        return 0;
    }
}

IfcUtil::IfcBaseClass* shallow_copy(IfcParse::IfcFile* file, IfcUtil::IfcBaseClass* source) {
    if (!file || !source) return nullptr;
    const auto& declaration = source->declaration();
    auto* result = file->create(&declaration);
    auto* entity_decl = declaration.as_entity();
    if (!result || !entity_decl) return result;

    const auto attrs = entity_decl->all_attributes();
    for (size_t i = 0; i < attrs.size(); ++i) {
        try {
            auto value = source->get_attribute_value(i);
            if (value.isNull()) continue;
            switch (value.type()) {
                case IfcUtil::Argument_INT:
                    result->set_attribute_value(i, static_cast<int>(value));
                    break;
                case IfcUtil::Argument_STRING:
                case IfcUtil::Argument_ENUMERATION:
                    result->set_attribute_value(i, static_cast<std::string>(value));
                    break;
                case IfcUtil::Argument_ENTITY_INSTANCE:
                    result->set_attribute_value(i, static_cast<IfcUtil::IfcBaseClass*>(value));
                    break;
                default:
                    break;
            }
        } catch (...) {
        }
    }
    return result;
}

int now_seconds() {
    return static_cast<int>(std::time(nullptr));
}

} // namespace

namespace ifcapi {
namespace bindings {

IfcUtil::IfcBaseClass* owner_create_owner_history(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* user,
    IfcUtil::IfcBaseClass* application)
{
    ifcopenshell_clear_error();
    if (!file) {
        set_error("file is NULL");
        return nullptr;
    }
    if (!user || !application) {
        if (is_ifc2x3(file)) {
            set_error("IFC2X3 owner history requires an owning user and application");
        }
        return nullptr;
    }

    try {
        const auto* decl = file->schema()->declaration_by_name("IfcOwnerHistory");
        auto* owner_history = file->create(decl);
        if (!owner_history) {
            set_error("Failed to create IfcOwnerHistory");
            return nullptr;
        }
        const int now = now_seconds();
        set_ref(owner_history, "OwningUser", user);
        set_ref(owner_history, "OwningApplication", application);
        set_enum(owner_history, "State", "READWRITE");
        set_enum(owner_history, "ChangeAction", "ADDED");
        set_int(owner_history, "LastModifiedDate", now);
        set_ref(owner_history, "LastModifyingUser", user);
        set_ref(owner_history, "LastModifyingApplication", application);
        set_int(owner_history, "CreationDate", now);
        return owner_history;
    } catch (const std::exception& ex) {
        set_error(ex.what());
        return nullptr;
    } catch (...) {
        set_error("Unknown C++ exception in owner_create_owner_history");
        return nullptr;
    }
}

IfcUtil::IfcBaseClass* owner_update_owner_history(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* element,
    IfcUtil::IfcBaseClass* user,
    IfcUtil::IfcBaseClass* application)
{
    ifcopenshell_clear_error();
    if (!file) {
        set_error("file is NULL");
        return nullptr;
    }
    if (!element || !is_a(file, element, "IfcRoot") || !user || !application) {
        return nullptr;
    }

    try {
        auto* owner_history = get_ref_attr(element, "OwnerHistory");
        if (!owner_history) {
            owner_history = owner_create_owner_history(file, user, application);
            if (owner_history) {
                set_ref(element, "OwnerHistory", owner_history);
            }
            return owner_history;
        }

        if (total_inverses(file, owner_history) > 1) {
            owner_history = shallow_copy(file, owner_history);
            if (owner_history) {
                set_ref(element, "OwnerHistory", owner_history);
            }
        }

        if (!owner_history) return nullptr;
        set_enum(owner_history, "ChangeAction", "MODIFIED");
        set_int(owner_history, "LastModifiedDate", now_seconds());
        set_ref(owner_history, "LastModifyingUser", user);
        set_ref(owner_history, "LastModifyingApplication", application);
        return owner_history;
    } catch (const std::exception& ex) {
        set_error(ex.what());
        return nullptr;
    } catch (...) {
        set_error("Unknown C++ exception in owner_update_owner_history");
        return nullptr;
    }
}

} // namespace bindings
} // namespace ifcapi
