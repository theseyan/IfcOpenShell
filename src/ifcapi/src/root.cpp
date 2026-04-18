// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ifcapi/ifcapi.h"
#include "guid.h"
#include "ifcopenshell_api_internal.hpp"

#include "ifcparse/IfcFile.h"
#include "ifcparse/IfcSchema.h"
#include "ifcparse/IfcBaseClass.h"
#include "ifcparse/IfcEntityInstanceData.h"
#include "ifcparse/IfcException.h"

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <string>

// Error-reporting helpers route through the autogen layer's named namespace
// (declared in ifcopenshell_api_internal.hpp) so the high-level and low-level
// APIs share a single thread-local error string and the same public
// ifcopenshell_last_error_message() / ifcopenshell_clear_error() entry points.
namespace {
inline void set_error(const char* msg) { ifcopenshell::capi::set_last_error(msg); }
inline void set_error(const std::string& msg) { ifcopenshell::capi::set_last_error(msg); }
}

// Resolve the enumeration_type for an entity attribute at the given index.
// Walks through named_type → type_declaration chains until an enumeration_type is found.
static const IfcParse::enumeration_type* resolve_enum_type(
    const IfcParse::entity* entity_decl, size_t attr_idx)
{
    auto all_attrs = entity_decl->all_attributes();
    if (attr_idx >= all_attrs.size()) return nullptr;

    const IfcParse::parameter_type* pt = all_attrs[attr_idx]->type_of_attribute();
    while (pt) {
        auto* named = pt->as_named_type();
        if (!named) break;
        auto* decl = named->declared_type();
        if (auto* et = decl->as_enumeration_type()) return et;
        if (auto* td = decl->as_type_declaration()) {
            pt = td->declared_type();
        } else {
            break;
        }
    }
    return nullptr;
}

// Try to set an enumeration attribute by string value.
// Returns true if the value was found and set, false otherwise.
static bool try_set_enum(
    IfcUtil::IfcBaseClass* entity,
    const IfcParse::entity* entity_decl,
    size_t attr_idx,
    const std::string& value)
{
    auto* enum_type = resolve_enum_type(entity_decl, attr_idx);
    if (!enum_type) return false;

    const auto& items = enum_type->enumeration_items();
    auto it = std::find(items.begin(), items.end(), value);
    if (it == items.end()) return false;

    size_t enum_idx = static_cast<size_t>(std::distance(items.begin(), it));
    entity->set_attribute_value(attr_idx, EnumerationReference(enum_type, enum_idx));
    return true;
}

extern "C" {

char* ifcopenshell_guid_new(void) {
    try {
        std::string guid = ifcapi::guid_new();
        char* result = static_cast<char*>(std::malloc(guid.size() + 1));
        if (!result) return nullptr;
        std::memcpy(result, guid.c_str(), guid.size() + 1);
        return result;
    } catch (const std::exception& e) {
        set_error(e.what());
        return nullptr;
    }
}

char* ifcopenshell_guid_compress(const char* uuid_hex) {
    if (!uuid_hex) { set_error("uuid_hex is NULL"); return nullptr; }
    try {
        std::string guid = ifcapi::guid_compress(uuid_hex);
        char* result = static_cast<char*>(std::malloc(guid.size() + 1));
        if (!result) return nullptr;
        std::memcpy(result, guid.c_str(), guid.size() + 1);
        return result;
    } catch (const std::exception& e) {
        set_error(e.what());
        return nullptr;
    }
}

char* ifcopenshell_guid_expand(const char* guid) {
    if (!guid) { set_error("guid is NULL"); return nullptr; }
    try {
        std::string uuid = ifcapi::guid_expand(guid);
        char* result = static_cast<char*>(std::malloc(uuid.size() + 1));
        if (!result) return nullptr;
        std::memcpy(result, uuid.c_str(), uuid.size() + 1);
        return result;
    } catch (const std::exception& e) {
        set_error(e.what());
        return nullptr;
    }
}

void ifcopenshell_free_string(char* str) {
    std::free(str);
}

ifcopenshell_ifc_instance_t* ifcopenshell_root_create_entity(
    ifcopenshell_ifc_file_t* file_ptr,
    const char* ifc_class,
    const char* predefined_type,
    const char* name,
    ifcopenshell_ifc_instance_t* owner_history)
{
    ifcopenshell_clear_error();

    if (!file_ptr) { set_error("file is NULL"); return 0; }
    if (!ifc_class) { set_error("ifc_class is NULL"); return 0; }

    try {
        auto* file = file_ptr->ptr;
        const auto* schema = file->schema();
        const std::string schema_name = schema->name();

        const IfcParse::declaration* decl = nullptr;
        try {
            decl = schema->declaration_by_name(ifc_class);
        } catch (...) {
            set_error(std::string("Unknown IFC class: ") + ifc_class);
            return 0;
        }

        const IfcParse::entity* entity_decl = decl->as_entity();
        if (!entity_decl) {
            set_error(std::string("Not an entity type: ") + ifc_class);
            return 0;
        }

        auto* entity = file->create(decl);
        if (!entity) {
            set_error("Failed to create entity");
            return 0;
        }

        // Helper: find attribute index by name, returns -1 if not found
        auto attr_index = [&](const char* attr_name) -> int {
            try {
                return static_cast<int>(entity_decl->attribute_index(attr_name));
            } catch (...) {
                return -1;
            }
        };

        // Helper: check if entity is_a given type name
        auto is_a = [&](const IfcParse::entity* e, const char* type_name) -> bool {
            try {
                const auto* t = schema->declaration_by_name(type_name);
                return e->is(*t);
            } catch (...) {
                return false;
            }
        };

        auto has_attr = [&](const char* n) -> bool { return attr_index(n) >= 0; };

        auto set_string = [&](const char* attr_name, const std::string& value) {
            int idx = attr_index(attr_name);
            if (idx >= 0) entity->set_attribute_value(static_cast<size_t>(idx), value);
        };

        auto set_enum = [&](const char* attr_name, const std::string& value) -> bool {
            int idx = attr_index(attr_name);
            if (idx < 0) return false;
            return try_set_enum(entity, entity_decl, static_cast<size_t>(idx), value);
        };

        auto set_bool = [&](const char* attr_name, bool value) {
            int idx = attr_index(attr_name);
            if (idx >= 0) entity->set_attribute_value(static_cast<size_t>(idx), value);
        };

        // 1. Set GlobalId
        {
            int gi = attr_index("GlobalId");
            if (gi >= 0) {
                std::string guid = ifcapi::guid_new();
                entity->set_attribute_value(static_cast<size_t>(gi), guid);
            }
        }

        // 2. Set OwnerHistory
        if (owner_history && owner_history->ptr) {
            int oh = attr_index("OwnerHistory");
            if (oh >= 0) {
                auto* oh_entity = (owner_history ? owner_history->ptr : nullptr);
                if (oh_entity) {
                    entity->set_attribute_value(static_cast<size_t>(oh), oh_entity);
                }
            }
        }

        // 3. Set Name
        if (name) {
            set_string("Name", name);
        }

        // 4. Handle PredefinedType
        if (predefined_type && predefined_type[0] != '\0') {
            if (has_attr("PredefinedType")) {
                if (!set_enum("PredefinedType", predefined_type)) {
                    set_enum("PredefinedType", "USERDEFINED");
                    if (has_attr("ObjectType")) {
                        set_string("ObjectType", predefined_type);
                    } else if (has_attr("ElementType")) {
                        set_string("ElementType", predefined_type);
                    } else if (has_attr("ProcessType")) {
                        set_string("ProcessType", predefined_type);
                    }
                }
            } else if (has_attr("ObjectType")) {
                set_string("ObjectType", predefined_type);
            }
        }

        // 5. Schema-specific defaults
        bool is_ifc2x3 = (schema_name.find("2x3") != std::string::npos ||
                          schema_name.find("2X3") != std::string::npos);

        if (is_ifc2x3) {
            if (is_a(entity_decl, "IfcElementType") && has_attr("PredefinedType")) {
                int pt = attr_index("PredefinedType");
                if (pt >= 0 && entity->get_attribute_value(static_cast<size_t>(pt)).isNull()) {
                    set_enum("PredefinedType", "NOTDEFINED");
                }
            }

            if (is_a(entity_decl, "IfcSpatialStructureElement")) {
                set_enum("CompositionType", "ELEMENT");
            } else if (is_a(entity_decl, "IfcRoof")) {
                set_enum("ShapeType", "NOTDEFINED");
            } else if (is_a(entity_decl, "IfcFurnitureType")) {
                set_enum("AssemblyPlace", "NOTDEFINED");
            } else if (is_a(entity_decl, "IfcDoorStyle") || is_a(entity_decl, "IfcWindowStyle")) {
                set_enum("OperationType", "NOTDEFINED");
                set_enum("ConstructionType", "NOTDEFINED");
                set_bool("ParameterTakesPrecedence", false);
                set_bool("Sizeable", false);
            }
        } else {
            if (is_a(entity_decl, "IfcElementType") && has_attr("PredefinedType")) {
                int pt = attr_index("PredefinedType");
                if (pt >= 0 && entity->get_attribute_value(static_cast<size_t>(pt)).isNull()) {
                    set_enum("PredefinedType", "NOTDEFINED");
                }
            }

            bool is_ifc4_only = (schema_name == "IFC4");
            if (is_ifc4_only && (is_a(entity_decl, "IfcDoorStyle") || is_a(entity_decl, "IfcWindowStyle"))) {
                set_enum("OperationType", "NOTDEFINED");
                set_enum("ConstructionType", "NOTDEFINED");
                set_bool("ParameterTakesPrecedence", false);
                set_bool("Sizeable", false);
            } else if (is_a(entity_decl, "IfcDoorType")) {
                set_enum("OperationType", "NOTDEFINED");
            } else if (is_a(entity_decl, "IfcWindowType")) {
                set_enum("PartitioningType", "NOTDEFINED");
            } else if (is_a(entity_decl, "IfcFurnitureType")) {
                set_enum("AssemblyPlace", "NOTDEFINED");
            }
        }

        return ifcopenshell::capi::wrap_instance(entity);

    } catch (const IfcParse::IfcException& e) {
        set_error(e.what());
        return 0;
    } catch (const std::exception& e) {
        set_error(e.what());
        return 0;
    } catch (...) {
        set_error("Unknown C++ exception in ifcopenshell_root_create_entity");
        return 0;
    }
}

} // extern "C"
