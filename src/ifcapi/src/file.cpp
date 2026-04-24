// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ifcapi/ifcapi.h"

#include "ifcparse/IfcFile.h"
#include "ifcparse/IfcSchema.h"
#include "ifcparse/IfcSpfHeader.h"
#include "ifcparse/IfcBaseClass.h"
#include "ifcparse/IfcException.h"

#include <cstdlib>
#include <cstring>
#include <fstream>
#include <sstream>
#include <string>

// Defined in root.cpp
#include "ifcopenshell_api_internal.hpp"

// Route error reporting through the autogen layer's shared error string
// so that ifcopenshell_last_error_message() returns errors raised by the
// high-level layer too.
namespace {
inline void set_error(const char* msg) { ifcopenshell::capi::set_last_error(msg); }
inline void set_error(const std::string& msg) { ifcopenshell::capi::set_last_error(msg); }
}

static char* alloc_cstr(const std::string& s) {
    char* result = static_cast<char*>(std::malloc(s.size() + 1));
    if (result) std::memcpy(result, s.c_str(), s.size() + 1);
    return result;
}

extern "C" {

ifcopenshell_ifc_file_t* ifcopenshell_file_create(const char* schema_name) {
    ifcopenshell_clear_error();
    if (!schema_name) { set_error("schema_name is NULL"); return nullptr; }
    try {
        const auto* schema = IfcParse::schema_by_name(schema_name);
        auto* file = new IfcParse::IfcFile(schema);
        return ifcopenshell::capi::wrap_file(file, /*owned=*/true);
    } catch (const std::exception& e) {
        set_error(e.what());
        return nullptr;
    }
}

ifcopenshell_ifc_file_t* ifcopenshell_file_open(const char* path) {
    ifcopenshell_clear_error();
    if (!path) { set_error("path is NULL"); return nullptr; }
    {
        std::ifstream probe(path, std::ios::binary);
        if (!probe.good()) {
            std::ostringstream oss;
            oss << "File does not exist or is not readable: " << path;
            set_error(oss.str());
            return nullptr;
        }
    }
    try {
        auto* file = new IfcParse::IfcFile(path);
        if (!file->good()) {
            std::ostringstream oss;
            oss << "Failed to open IFC file: " << path;
            set_error(oss.str());
            delete file;
            return nullptr;
        }
        return ifcopenshell::capi::wrap_file(file, /*owned=*/true);
    } catch (const std::exception& e) {
        set_error(e.what());
        return nullptr;
    }
}

ifcopenshell_ifc_file_t* ifcopenshell_file_from_string(const char* data, int length) {
    ifcopenshell_clear_error();
    if (!data) { set_error("data is NULL"); return nullptr; }
    if (length < 0) { set_error("length is negative"); return nullptr; }
    try {
        auto* file = new IfcParse::IfcFile(const_cast<char*>(data), length);
        if (!file->good()) {
            set_error("Failed to parse IFC data from string");
            delete file;
            return nullptr;
        }
        return ifcopenshell::capi::wrap_file(file, /*owned=*/true);
    } catch (const std::exception& e) {
        set_error(e.what());
        return nullptr;
    }
}

void ifcopenshell_file_free(ifcopenshell_ifc_file_t* file) {
    if (!file) return;
    if (file->owned) delete file->ptr;
    delete file;
}

const char* ifcopenshell_file_schema(const ifcopenshell_ifc_file_t* file) {
    if (!file) return "";
    auto* f = file->ptr;
    auto* schema = f->schema();
    return schema ? schema->name().c_str() : "";
}

ifcopenshell_ifc_instance_t* ifcopenshell_file_header_file_description(ifcopenshell_ifc_file_t* file) {
    if (!file || !file->ptr) return nullptr;
    try {
        auto* h = file->ptr->header().file_description();
        if (!h) return nullptr;
        return ifcopenshell::capi::wrap_instance(const_cast<IfcUtil::IfcBaseClass*>(static_cast<const IfcUtil::IfcBaseClass*>(h)));
    } catch (...) { return nullptr; }
}

ifcopenshell_ifc_instance_t* ifcopenshell_file_header_file_name(ifcopenshell_ifc_file_t* file) {
    if (!file || !file->ptr) return nullptr;
    try {
        auto* h = file->ptr->header().file_name();
        if (!h) return nullptr;
        return ifcopenshell::capi::wrap_instance(const_cast<IfcUtil::IfcBaseClass*>(static_cast<const IfcUtil::IfcBaseClass*>(h)));
    } catch (...) { return nullptr; }
}

ifcopenshell_ifc_instance_t* ifcopenshell_file_header_file_schema(ifcopenshell_ifc_file_t* file) {
    if (!file || !file->ptr) return nullptr;
    try {
        auto* h = file->ptr->header().file_schema();
        if (!h) return nullptr;
        return ifcopenshell::capi::wrap_instance(const_cast<IfcUtil::IfcBaseClass*>(static_cast<const IfcUtil::IfcBaseClass*>(h)));
    } catch (...) { return nullptr; }
}

ifcopenshell_ifc_instance_t* ifcopenshell_file_create_entity(ifcopenshell_ifc_file_t* file, const char* type_name) {
    ifcopenshell_clear_error();
    if (!file) { set_error("file is NULL"); return nullptr; }
    if (!type_name) { set_error("type_name is NULL"); return nullptr; }
    try {
        auto* f = file->ptr;
        const auto* schema = f->schema();
        const auto* decl = schema->declaration_by_name(type_name);
        // Reject type (non-entity) declarations — callers should use the
        // typed-value path for IfcLabel/IfcReal/etc. This keeps the Python
        // wrapper's create_entity() fallback for inline simple types intact.
        if (!decl || !decl->as_entity()) {
            set_error("Type declaration is not an entity");
            return nullptr;
        }
        auto* entity = f->create(decl);
        if (!entity) {
            set_error("Failed to create entity");
            return nullptr;
        }
        return ifcopenshell::capi::wrap_instance(entity);
    } catch (const IfcParse::IfcException& e) {
        set_error(e.what());
        return nullptr;
    } catch (const std::exception& e) {
        set_error(e.what());
        return nullptr;
    }
}

ifcopenshell_ifc_instance_t* ifcopenshell_file_create_entity_with_id(
    ifcopenshell_ifc_file_t* file, const char* type_name, uint32_t id) {
    ifcopenshell_clear_error();
    if (!file) { set_error("file is NULL"); return nullptr; }
    if (!type_name) { set_error("type_name is NULL"); return nullptr; }
    try {
        auto* f = file->ptr;
        const auto* schema = f->schema();
        const auto* decl = schema->declaration_by_name(type_name);
        if (!decl || !decl->as_entity()) {
            set_error("Type declaration is not an entity");
            return nullptr;
        }
        // Bypass IfcFile::create() (which auto-assigns IDs sequentially).
        // Instantiate the entity directly, then register it via addEntity()
        // with the explicit ID. addEntity() throws if the ID is taken.
        auto* inst = schema->instantiate(
            decl, in_memory_attribute_storage(decl->as_entity()->attribute_count()));
        // file_ must be nullptr so addEntity() runs its registration path
        // (mirrors what in_memory_file_storage::create() does).
        inst->file_ = nullptr;
        auto* entity = f->addEntity(inst, static_cast<int>(id));
        if (!entity) {
            set_error("Failed to create entity with id");
            return nullptr;
        }
        return ifcopenshell::capi::wrap_instance(entity);
    } catch (const IfcParse::IfcException& e) {
        set_error(e.what());
        return nullptr;
    } catch (const std::exception& e) {
        set_error(e.what());
        return nullptr;
    }
}

int32_t ifcopenshell_file_by_type_count(const ifcopenshell_ifc_file_t* file, const char* type_name) {
    if (!file || !type_name) return 0;
    try {
        auto* f = file->ptr;
        auto entities = f->instances_by_type(type_name);
        return entities ? static_cast<int32_t>(entities->size()) : 0;
    } catch (...) {
        return 0;
    }
}

int32_t ifcopenshell_file_by_type(const ifcopenshell_ifc_file_t* file, const char* type_name, ifcopenshell_ifc_instance_t** ids_out) {
    if (!file || !type_name || !ids_out) return 0;
    try {
        auto* f = file->ptr;
        auto entities = f->instances_by_type(type_name);
        if (!entities) return 0;
        int32_t count = 0;
        for (auto it = entities->begin(); it != entities->end(); ++it) {
            ids_out[count++] = ifcopenshell::capi::wrap_instance(*it);
        }
        return count;
    } catch (...) {
        return 0;
    }
}

void ifcopenshell_free_instance_array(ifcopenshell_ifc_instance_t** arr, uint32_t count) {
    if (!arr) return;
    for (uint32_t i = 0; i < count; ++i) {
        if (arr[i]) ifcopenshell_ifc_instance_destroy(arr[i]);
    }
    std::free(arr);
}

void ifcopenshell_free_instance_array_only(ifcopenshell_ifc_instance_t** arr) {
    if (arr) std::free(arr);
}

void ifcopenshell_file_remove(ifcopenshell_ifc_instance_t* instance) {
    ifcopenshell_clear_error();
    if (!instance || !instance->ptr) { set_error("instance is NULL"); return; }
    try {
        auto* entity = instance->ptr;
        auto* f = entity->file_;
        if (!f) { set_error("instance has no file"); return; }
        f->removeEntity(entity);
    } catch (const std::exception& e) {
        set_error(e.what());
    }
}

ifcopenshell_ifc_instance_t* ifcopenshell_file_by_guid(const ifcopenshell_ifc_file_t* file, const char* guid) {
    if (!file || !guid) return nullptr;
    try {
        auto* f = file->ptr;
        auto* entity = f->instance_by_guid(guid);
        return entity ? ifcopenshell::capi::wrap_instance(entity) : nullptr;
    } catch (...) {
        return nullptr;
    }
}

uint32_t ifcopenshell_file_entity_count(const ifcopenshell_ifc_file_t* file) {
    if (!file) return 0;
    auto* f = file->ptr;
    uint32_t count = 0;
    for (auto it = f->begin(); it != f->end(); ++it) {
        ++count;
    }
    return count;
}

uint32_t ifcopenshell_file_get_max_id(const ifcopenshell_ifc_file_t* file) {
    if (!file) return 0;
    return file->ptr->getMaxId();
}

ifcopenshell_ifc_instance_t* ifcopenshell_file_add_entity(
    ifcopenshell_ifc_file_t* file, ifcopenshell_ifc_instance_t* instance, uint32_t id) {
    ifcopenshell_clear_error();
    if (!file) { set_error("file is NULL"); return nullptr; }
    if (!instance) { set_error("instance is NULL"); return nullptr; }
    try {
        auto* added = file->ptr->addEntity(
            instance->ptr, id == 0 ? -1 : static_cast<int>(id));
        if (!added) {
            set_error("Failed to add entity");
            return nullptr;
        }
        return ifcopenshell::capi::wrap_instance(added);
    } catch (const IfcParse::IfcException& e) {
        set_error(e.what());
        return nullptr;
    } catch (const std::exception& e) {
        set_error(e.what());
        return nullptr;
    }
}

ifcopenshell_ifc_instance_t** ifcopenshell_file_entity_ids(const ifcopenshell_ifc_file_t* file, uint32_t* count) {
    if (!file || !count) return nullptr;
    auto* f = file->ptr;
    uint32_t n = 0;
    for (auto it = f->begin(); it != f->end(); ++it) ++n;
    if (n == 0) { *count = 0; return nullptr; }
    auto* arr = static_cast<ifcopenshell_ifc_instance_t**>(std::malloc(n * sizeof(ifcopenshell_ifc_instance_t*)));
    if (!arr) { *count = 0; return nullptr; }
    uint32_t i = 0;
    for (auto it = f->begin(); it != f->end(); ++it) {
        arr[i++] = ifcopenshell::capi::wrap_instance(it->second);
    }
    *count = n;
    return arr;
}

ifcopenshell_ifc_instance_t** ifcopenshell_file_get_inverse(ifcopenshell_ifc_instance_t* instance, uint32_t* count) {
    if (!instance || !instance->ptr || !count) return nullptr;
    *count = 0;
    try {
        auto* entity = instance->ptr;
        auto* f = entity->file_;
        if (!f) return nullptr;
        auto result = f->getInverse(static_cast<int>(entity->id()), nullptr, -1);
        if (!result || result->size() == 0) return nullptr;
        auto* arr = static_cast<ifcopenshell_ifc_instance_t**>(std::malloc(result->size() * sizeof(ifcopenshell_ifc_instance_t*)));
        if (!arr) return nullptr;
        uint32_t i = 0;
        for (auto it = result->begin(); it != result->end(); ++it) {
            arr[i++] = ifcopenshell::capi::wrap_instance(*it);
        }
        *count = i;
        return arr;
    } catch (...) {
        return nullptr;
    }
}

int32_t* ifcopenshell_file_get_inverse_indices(ifcopenshell_ifc_instance_t* instance, uint32_t* count) {
    if (!instance || !instance->ptr || !count) return nullptr;
    *count = 0;
    try {
        auto* entity = instance->ptr;
        auto* f = entity->file_;
        if (!f) return nullptr;
        auto idxs = f->get_inverse_indices(static_cast<int>(entity->id()));
        if (idxs.empty()) return nullptr;
        auto* arr = static_cast<int32_t*>(std::malloc(idxs.size() * sizeof(int32_t)));
        if (!arr) return nullptr;
        for (size_t i = 0; i < idxs.size(); ++i) arr[i] = static_cast<int32_t>(idxs[i]);
        *count = static_cast<uint32_t>(idxs.size());
        return arr;
    } catch (...) {
        return nullptr;
    }
}

void ifcopenshell_free_int32_array(int32_t* arr) {
    if (arr) std::free(arr);
}

ifcopenshell_ifc_instance_t** ifcopenshell_file_traverse(ifcopenshell_ifc_instance_t* instance, int max_levels, uint32_t* count) {
    if (!instance || !instance->ptr || !count) return nullptr;
    *count = 0;
    try {
        auto* entity = instance->ptr;
        auto* f = entity->file_;
        if (!f) return nullptr;
        auto result = f->traverse(entity, max_levels > 0 ? max_levels : -1);
        if (!result || result->size() == 0) return nullptr;
        auto* arr = static_cast<ifcopenshell_ifc_instance_t**>(std::malloc(result->size() * sizeof(ifcopenshell_ifc_instance_t*)));
        if (!arr) return nullptr;
        uint32_t i = 0;
        for (auto it = result->begin(); it != result->end(); ++it) {
            arr[i++] = ifcopenshell::capi::wrap_instance(*it);
        }
        *count = static_cast<uint32_t>(result->size());
        return arr;
    } catch (...) {
        return nullptr;
    }
}

bool ifcopenshell_file_write(ifcopenshell_ifc_file_t* file, const char* path) {
    ifcopenshell_clear_error();
    if (!file || !path) { set_error("file or path is NULL"); return false; }
    try {
        auto* f = file->ptr;
        std::ofstream out(path);
        if (!out.is_open()) {
            set_error("Cannot open file for writing");
            return false;
        }
        out << *f;
        return true;
    } catch (const std::exception& e) {
        set_error(e.what());
        return false;
    }
}

char* ifcopenshell_file_to_string(ifcopenshell_ifc_file_t* file) {
    ifcopenshell_clear_error();
    if (!file) { set_error("file is NULL"); return nullptr; }
    try {
        auto* f = file->ptr;
        std::ostringstream oss;
        oss << *f;
        return alloc_cstr(oss.str());
    } catch (const std::exception& e) {
        set_error(e.what());
        return nullptr;
    }
}

} // extern "C"
