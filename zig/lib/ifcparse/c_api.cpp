#include "ifcparse/c_api.h"

#include "ifcparse/IfcFile.h"
#include "ifcparse/IfcEntityInstanceData.h"
#include "ifcparse/IfcParse.h"

#include <boost/logic/tribool.hpp>

#include <fstream>
#include <climits>
#include <limits>
#include <memory>
#include <new>
#include <sstream>
#include <string>
#include <vector>

struct ifcopenshell_ifcparse_file {
    std::shared_ptr<IfcParse::IfcFile> file;
    std::string last_error;
};

struct ifcopenshell_ifcparse_entity_list {
    std::shared_ptr<IfcParse::IfcFile> owner_file;
    aggregate_of_instance::ptr entities;
    size_t cursor = 0;
    std::string last_error;
};

struct ifcopenshell_ifcparse_int_list {
    std::shared_ptr<IfcParse::IfcFile> owner_file;
    std::vector<int> values;
    size_t cursor = 0;
    std::string last_error;
};

struct ifcopenshell_ifcparse_double_list {
    std::shared_ptr<IfcParse::IfcFile> owner_file;
    std::vector<double> values;
    size_t cursor = 0;
    std::string last_error;
};

struct ifcopenshell_ifcparse_int_matrix {
    std::vector<std::vector<int>> values;
    std::string last_error;
};

struct ifcopenshell_ifcparse_double_matrix {
    std::vector<std::vector<double>> values;
    std::string last_error;
};

struct ifcopenshell_ifcparse_entity_matrix {
    std::vector<std::vector<IfcUtil::IfcBaseClass*>> values;
    std::string last_error;
};

struct ifcopenshell_ifcparse_type_list {
    std::shared_ptr<IfcParse::IfcFile> owner_file;
    std::vector<const IfcParse::declaration*> types;
    size_t cursor = 0;
    std::string last_error;
};

struct ifcopenshell_ifcparse_string_list {
    std::vector<std::string> values;
    size_t cursor = 0;
    std::string last_error;
};

namespace {
thread_local std::string g_last_error;
thread_local std::string g_spf_buffer;
thread_local std::string g_timestamp_buffer;
thread_local std::string g_entity_spf_buffer;
thread_local std::string g_argument_string_buffer;
thread_local std::string g_entity_type_name_with_schema_buffer;

const ifcopenshell_ifcparse_entity_ref_t* to_entity_ref(const IfcUtil::IfcBaseClass* entity) {
    return reinterpret_cast<const ifcopenshell_ifcparse_entity_ref_t*>(entity);
}

const IfcUtil::IfcBaseClass* from_entity_ref(const ifcopenshell_ifcparse_entity_ref_t* entity) {
    return reinterpret_cast<const IfcUtil::IfcBaseClass*>(entity);
}

IfcUtil::IfcBaseClass* from_entity_ref_mut(const ifcopenshell_ifcparse_entity_ref_t* entity) {
    return const_cast<IfcUtil::IfcBaseClass*>(reinterpret_cast<const IfcUtil::IfcBaseClass*>(entity));
}

const ifcopenshell_ifcparse_type_ref_t* to_type_ref(const IfcParse::declaration* decl) {
    return reinterpret_cast<const ifcopenshell_ifcparse_type_ref_t*>(decl);
}

const IfcParse::declaration* from_type_ref(const ifcopenshell_ifcparse_type_ref_t* type_ref) {
    return reinterpret_cast<const IfcParse::declaration*>(type_ref);
}

void set_global_error(const std::string& message) {
    g_last_error = message;
}

void clear_global_error() {
    g_last_error.clear();
}

void set_file_error(ifcopenshell_ifcparse_file_t* file, const std::string& message) {
    if (file != nullptr) {
        file->last_error = message;
    }
    set_global_error(message);
}

void clear_file_error(ifcopenshell_ifcparse_file_t* file) {
    if (file != nullptr) {
        file->last_error.clear();
    }
    clear_global_error();
}

void set_list_error(ifcopenshell_ifcparse_entity_list_t* list, const std::string& message) {
    if (list != nullptr) {
        list->last_error = message;
    }
    set_global_error(message);
}

void clear_list_error(ifcopenshell_ifcparse_entity_list_t* list) {
    if (list != nullptr) {
        list->last_error.clear();
    }
    clear_global_error();
}

void set_int_list_error(ifcopenshell_ifcparse_int_list_t* list, const std::string& message) {
    if (list != nullptr) {
        list->last_error = message;
    }
    set_global_error(message);
}

void clear_int_list_error(ifcopenshell_ifcparse_int_list_t* list) {
    if (list != nullptr) {
        list->last_error.clear();
    }
    clear_global_error();
}

void set_double_list_error(ifcopenshell_ifcparse_double_list_t* list, const std::string& message) {
    if (list != nullptr) {
        list->last_error = message;
    }
    set_global_error(message);
}

void clear_double_list_error(ifcopenshell_ifcparse_double_list_t* list) {
    if (list != nullptr) {
        list->last_error.clear();
    }
    clear_global_error();
}

void set_type_list_error(ifcopenshell_ifcparse_type_list_t* list, const std::string& message) {
    if (list != nullptr) {
        list->last_error = message;
    }
    set_global_error(message);
}

void clear_type_list_error(ifcopenshell_ifcparse_type_list_t* list) {
    if (list != nullptr) {
        list->last_error.clear();
    }
    clear_global_error();
}

void set_int_matrix_error(ifcopenshell_ifcparse_int_matrix_t* matrix, const std::string& message) {
    if (matrix != nullptr) {
        matrix->last_error = message;
    }
    set_global_error(message);
}

void clear_int_matrix_error(ifcopenshell_ifcparse_int_matrix_t* matrix) {
    if (matrix != nullptr) {
        matrix->last_error.clear();
    }
    clear_global_error();
}

void set_double_matrix_error(ifcopenshell_ifcparse_double_matrix_t* matrix, const std::string& message) {
    if (matrix != nullptr) {
        matrix->last_error = message;
    }
    set_global_error(message);
}

void clear_double_matrix_error(ifcopenshell_ifcparse_double_matrix_t* matrix) {
    if (matrix != nullptr) {
        matrix->last_error.clear();
    }
    clear_global_error();
}

void set_entity_matrix_error(ifcopenshell_ifcparse_entity_matrix_t* matrix, const std::string& message) {
    if (matrix != nullptr) {
        matrix->last_error = message;
    }
    set_global_error(message);
}

void clear_entity_matrix_error(ifcopenshell_ifcparse_entity_matrix_t* matrix) {
    if (matrix != nullptr) {
        matrix->last_error.clear();
    }
    clear_global_error();
}

void set_string_list_error(ifcopenshell_ifcparse_string_list_t* list, const std::string& message) {
    if (list != nullptr) {
        list->last_error = message;
    }
    set_global_error(message);
}

void clear_string_list_error(ifcopenshell_ifcparse_string_list_t* list) {
    if (list != nullptr) {
        list->last_error.clear();
    }
    clear_global_error();
}

ifcopenshell_ifcparse_entity_list_t* make_entity_list(
    const std::shared_ptr<IfcParse::IfcFile>& owner_file,
    const aggregate_of_instance::ptr& entities
) {
    std::unique_ptr<ifcopenshell_ifcparse_entity_list_t> list(new ifcopenshell_ifcparse_entity_list_t());
    list->owner_file = owner_file;
    list->entities = entities ? entities : aggregate_of_instance::ptr(new aggregate_of_instance());
    list->cursor = 0;
    clear_list_error(list.get());
    return list.release();
}

IfcParse::filetype to_ifcparse_filetype(ifcopenshell_ifcparse_filetype_t filetype) {
    switch (filetype) {
        case IFCOPENSHELL_IFCPARSE_FT_IFCSPF:
            return IfcParse::FT_IFCSPF;
        case IFCOPENSHELL_IFCPARSE_FT_IFCXML:
            return IfcParse::FT_IFCXML;
        case IFCOPENSHELL_IFCPARSE_FT_IFCZIP:
            return IfcParse::FT_IFCZIP;
        case IFCOPENSHELL_IFCPARSE_FT_ROCKSDB:
            return IfcParse::FT_ROCKSDB;
        case IFCOPENSHELL_IFCPARSE_FT_UNKNOWN:
            return IfcParse::FT_UNKNOWN;
        case IFCOPENSHELL_IFCPARSE_FT_AUTODETECT:
            return IfcParse::FT_AUTODETECT;
        default:
            return IfcParse::FT_AUTODETECT;
    }
}

ifcopenshell_ifcparse_filetype_t to_c_filetype(IfcParse::filetype filetype) {
    switch (filetype) {
        case IfcParse::FT_IFCSPF:
            return IFCOPENSHELL_IFCPARSE_FT_IFCSPF;
        case IfcParse::FT_IFCXML:
            return IFCOPENSHELL_IFCPARSE_FT_IFCXML;
        case IfcParse::FT_IFCZIP:
            return IFCOPENSHELL_IFCPARSE_FT_IFCZIP;
        case IfcParse::FT_ROCKSDB:
            return IFCOPENSHELL_IFCPARSE_FT_ROCKSDB;
        case IfcParse::FT_UNKNOWN:
            return IFCOPENSHELL_IFCPARSE_FT_UNKNOWN;
        case IfcParse::FT_AUTODETECT:
            return IFCOPENSHELL_IFCPARSE_FT_AUTODETECT;
        default:
            return IFCOPENSHELL_IFCPARSE_FT_UNKNOWN;
    }
}

ifcopenshell_ifcparse_argument_type_t to_c_argument_type(IfcUtil::ArgumentType type) {
    switch (type) {
        case IfcUtil::Argument_NULL:
            return IFCOPENSHELL_IFCPARSE_ARG_NULL;
        case IfcUtil::Argument_DERIVED:
            return IFCOPENSHELL_IFCPARSE_ARG_DERIVED;
        case IfcUtil::Argument_INT:
            return IFCOPENSHELL_IFCPARSE_ARG_INT;
        case IfcUtil::Argument_BOOL:
            return IFCOPENSHELL_IFCPARSE_ARG_BOOL;
        case IfcUtil::Argument_LOGICAL:
            return IFCOPENSHELL_IFCPARSE_ARG_LOGICAL;
        case IfcUtil::Argument_DOUBLE:
            return IFCOPENSHELL_IFCPARSE_ARG_DOUBLE;
        case IfcUtil::Argument_STRING:
            return IFCOPENSHELL_IFCPARSE_ARG_STRING;
        case IfcUtil::Argument_BINARY:
            return IFCOPENSHELL_IFCPARSE_ARG_BINARY;
        case IfcUtil::Argument_ENUMERATION:
            return IFCOPENSHELL_IFCPARSE_ARG_ENUMERATION;
        case IfcUtil::Argument_ENTITY_INSTANCE:
            return IFCOPENSHELL_IFCPARSE_ARG_ENTITY_INSTANCE;
        case IfcUtil::Argument_EMPTY_AGGREGATE:
            return IFCOPENSHELL_IFCPARSE_ARG_EMPTY_AGGREGATE;
        case IfcUtil::Argument_AGGREGATE_OF_INT:
            return IFCOPENSHELL_IFCPARSE_ARG_AGGREGATE_OF_INT;
        case IfcUtil::Argument_AGGREGATE_OF_DOUBLE:
            return IFCOPENSHELL_IFCPARSE_ARG_AGGREGATE_OF_DOUBLE;
        case IfcUtil::Argument_AGGREGATE_OF_STRING:
            return IFCOPENSHELL_IFCPARSE_ARG_AGGREGATE_OF_STRING;
        case IfcUtil::Argument_AGGREGATE_OF_BINARY:
            return IFCOPENSHELL_IFCPARSE_ARG_AGGREGATE_OF_BINARY;
        case IfcUtil::Argument_AGGREGATE_OF_ENTITY_INSTANCE:
            return IFCOPENSHELL_IFCPARSE_ARG_AGGREGATE_OF_ENTITY_INSTANCE;
        case IfcUtil::Argument_AGGREGATE_OF_EMPTY_AGGREGATE:
            return IFCOPENSHELL_IFCPARSE_ARG_AGGREGATE_OF_EMPTY_AGGREGATE;
        case IfcUtil::Argument_AGGREGATE_OF_AGGREGATE_OF_INT:
            return IFCOPENSHELL_IFCPARSE_ARG_AGGREGATE_OF_AGGREGATE_OF_INT;
        case IfcUtil::Argument_AGGREGATE_OF_AGGREGATE_OF_DOUBLE:
            return IFCOPENSHELL_IFCPARSE_ARG_AGGREGATE_OF_AGGREGATE_OF_DOUBLE;
        case IfcUtil::Argument_AGGREGATE_OF_AGGREGATE_OF_ENTITY_INSTANCE:
            return IFCOPENSHELL_IFCPARSE_ARG_AGGREGATE_OF_AGGREGATE_OF_ENTITY_INSTANCE;
        case IfcUtil::Argument_UNKNOWN:
            return IFCOPENSHELL_IFCPARSE_ARG_UNKNOWN;
        default:
            return IFCOPENSHELL_IFCPARSE_ARG_UNKNOWN;
    }
}

IfcUtil::ArgumentType declared_argument_type(const IfcUtil::IfcBaseClass* inst, size_t index) {
    if (inst == nullptr) {
        return IfcUtil::Argument_UNKNOWN;
    }

    const IfcParse::declaration* decl = &inst->declaration();
    const IfcParse::parameter_type* parameter = nullptr;

    if (decl->as_entity() != nullptr) {
        if (index >= static_cast<size_t>(decl->as_entity()->attribute_count())) {
            return IfcUtil::Argument_UNKNOWN;
        }
        parameter = decl->as_entity()->attribute_by_index(index)->type_of_attribute();
        if (decl->as_entity()->derived()[index]) {
            return IfcUtil::Argument_DERIVED;
        }
    } else if ((decl->as_type_declaration() != nullptr) && index == 0) {
        parameter = decl->as_type_declaration()->declared_type();
    } else if ((decl->as_enumeration_type() != nullptr) && index == 0) {
        return IfcUtil::Argument_ENUMERATION;
    }

    if (parameter == nullptr) {
        return IfcUtil::Argument_UNKNOWN;
    }
    return IfcUtil::from_parameter_type(parameter);
}

const IfcParse::enumeration_type* resolve_enumeration_type(
    const IfcUtil::IfcBaseClass* inst,
    size_t index
) {
    if (inst == nullptr) {
        return nullptr;
    }
    const IfcParse::declaration* decl = &inst->declaration();
    if (decl->as_entity() == nullptr) {
        return nullptr;
    }
    if (index >= static_cast<size_t>(decl->as_entity()->attribute_count())) {
        return nullptr;
    }

    const IfcParse::parameter_type* parameter = decl->as_entity()->attribute_by_index(index)->type_of_attribute();
    while (parameter != nullptr) {
        if (const auto* named = parameter->as_named_type()) {
            const auto* declared = named->declared_type();
            if (const auto* enumeration = declared->as_enumeration_type()) {
                return enumeration;
            }
            if (const auto* type_decl = declared->as_type_declaration()) {
                parameter = type_decl->declared_type();
                continue;
            }
        }
        break;
    }
    return nullptr;
}

ifcopenshell_ifcparse_status_t to_c_status(IfcParse::file_open_status status) {
    switch (status.value()) {
        case IfcParse::file_open_status::SUCCESS:
            return IFCOPENSHELL_IFCPARSE_STATUS_SUCCESS;
        case IfcParse::file_open_status::READ_ERROR:
            return IFCOPENSHELL_IFCPARSE_STATUS_READ_ERROR;
        case IfcParse::file_open_status::NO_HEADER:
            return IFCOPENSHELL_IFCPARSE_STATUS_NO_HEADER;
        case IfcParse::file_open_status::UNSUPPORTED_SCHEMA:
            return IFCOPENSHELL_IFCPARSE_STATUS_UNSUPPORTED_SCHEMA;
        case IfcParse::file_open_status::INVALID_SYNTAX:
            return IFCOPENSHELL_IFCPARSE_STATUS_INVALID_SYNTAX;
        case IfcParse::file_open_status::UNKNOWN:
            return IFCOPENSHELL_IFCPARSE_STATUS_UNKNOWN;
        default:
            return IFCOPENSHELL_IFCPARSE_STATUS_UNKNOWN;
    }
}

} // namespace

const char* ifcopenshell_ifcparse_version(void) {
    return IFCOPENSHELL_VERSION;
}

ifcopenshell_ifcparse_filetype_t ifcopenshell_ifcparse_guess_file_type(const char* path) {
    if (path == nullptr || path[0] == '\0') {
        set_global_error("Invalid path");
        return IFCOPENSHELL_IFCPARSE_FT_UNKNOWN;
    }

    try {
        clear_global_error();
        return to_c_filetype(IfcParse::guess_file_type(std::string(path)));
    } catch (const std::exception& e) {
        set_global_error(e.what());
        return IFCOPENSHELL_IFCPARSE_FT_UNKNOWN;
    } catch (...) {
        set_global_error("Unknown native exception while guessing file type");
        return IFCOPENSHELL_IFCPARSE_FT_UNKNOWN;
    }
}

const char* ifcopenshell_ifcparse_create_timestamp(void) {
    try {
        g_timestamp_buffer = IfcParse::IfcFile::createTimestamp();
        clear_global_error();
        return g_timestamp_buffer.c_str();
    } catch (const std::exception& e) {
        set_global_error(e.what());
        return nullptr;
    } catch (...) {
        set_global_error("Unknown native exception while creating timestamp");
        return nullptr;
    }
}

ifcopenshell_ifcparse_file_t* ifcopenshell_ifcparse_file_create_empty(
    const char* schema_name
) {
    if (schema_name == nullptr || schema_name[0] == '\0') {
        set_global_error("Invalid schema name");
        return nullptr;
    }

    try {
        const IfcParse::schema_definition* schema = IfcParse::schema_by_name(std::string(schema_name));
        std::unique_ptr<ifcopenshell_ifcparse_file_t> handle(new ifcopenshell_ifcparse_file_t());
        handle->file = std::make_shared<IfcParse::IfcFile>(schema, IfcParse::FT_IFCSPF, std::string{});
        handle->last_error.clear();
        clear_global_error();
        return handle.release();
    } catch (const std::exception& e) {
        set_global_error(e.what());
        return nullptr;
    } catch (...) {
        set_global_error("Unknown native exception while creating empty IFC file");
        return nullptr;
    }
}

ifcopenshell_ifcparse_file_t* ifcopenshell_ifcparse_file_open_from_memory(
    const char* data,
    size_t length
) {
    if (data == nullptr || length == 0) {
        set_global_error("Invalid in-memory IFC buffer");
        return nullptr;
    }
    if (length > static_cast<size_t>(INT_MAX)) {
        set_global_error("In-memory IFC buffer exceeds supported size limits");
        return nullptr;
    }

    try {
        std::unique_ptr<ifcopenshell_ifcparse_file_t> handle(new ifcopenshell_ifcparse_file_t());
        handle->file = std::make_shared<IfcParse::IfcFile>(const_cast<char*>(data), static_cast<int>(length));

        const ifcopenshell_ifcparse_status_t status = to_c_status(handle->file->good());
        if (status != IFCOPENSHELL_IFCPARSE_STATUS_SUCCESS) {
            handle->last_error = std::string(ifcopenshell_ifcparse_file_status_message(status));
            set_global_error(handle->last_error);
        } else {
            handle->last_error.clear();
            clear_global_error();
        }
        return handle.release();
    } catch (const std::bad_alloc&) {
        set_global_error("Out of memory while opening IFC buffer");
        return nullptr;
    } catch (const std::exception& e) {
        set_global_error(e.what());
        return nullptr;
    } catch (...) {
        set_global_error("Unknown native exception while opening IFC buffer");
        return nullptr;
    }
}

ifcopenshell_ifcparse_file_t* ifcopenshell_ifcparse_file_open(
    const char* path,
    ifcopenshell_ifcparse_filetype_t filetype,
    int readonly
) {
    if (path == nullptr || path[0] == '\0') {
        set_global_error("Invalid path");
        return nullptr;
    }

    try {
        std::unique_ptr<ifcopenshell_ifcparse_file_t> handle(new ifcopenshell_ifcparse_file_t());
        handle->file = std::make_shared<IfcParse::IfcFile>(std::string(path), to_ifcparse_filetype(filetype), readonly != 0);

        const ifcopenshell_ifcparse_status_t status = to_c_status(handle->file->good());
        if (status != IFCOPENSHELL_IFCPARSE_STATUS_SUCCESS) {
            handle->last_error = std::string(ifcopenshell_ifcparse_file_status_message(status));
            set_global_error(handle->last_error);
        } else {
            handle->last_error.clear();
            clear_global_error();
        }
        return handle.release();
    } catch (const std::bad_alloc&) {
        set_global_error("Out of memory while opening IFC file");
        return nullptr;
    } catch (const std::exception& e) {
        set_global_error(e.what());
        return nullptr;
    } catch (...) {
        set_global_error("Unknown native exception while opening IFC file");
        return nullptr;
    }
}

void ifcopenshell_ifcparse_file_close(ifcopenshell_ifcparse_file_t* file) {
    delete file;
}

ifcopenshell_ifcparse_status_t ifcopenshell_ifcparse_file_status(const ifcopenshell_ifcparse_file_t* file) {
    if (file == nullptr || !file->file) {
        return IFCOPENSHELL_IFCPARSE_STATUS_UNKNOWN;
    }
    return to_c_status(file->file->good());
}

const char* ifcopenshell_ifcparse_file_status_message(ifcopenshell_ifcparse_status_t status) {
    switch (status) {
        case IFCOPENSHELL_IFCPARSE_STATUS_SUCCESS:
            return "SUCCESS";
        case IFCOPENSHELL_IFCPARSE_STATUS_READ_ERROR:
            return "READ_ERROR";
        case IFCOPENSHELL_IFCPARSE_STATUS_NO_HEADER:
            return "NO_HEADER";
        case IFCOPENSHELL_IFCPARSE_STATUS_UNSUPPORTED_SCHEMA:
            return "UNSUPPORTED_SCHEMA";
        case IFCOPENSHELL_IFCPARSE_STATUS_INVALID_SYNTAX:
            return "INVALID_SYNTAX";
        case IFCOPENSHELL_IFCPARSE_STATUS_UNKNOWN:
            return "UNKNOWN";
        default:
            return "UNKNOWN";
    }
}

const char* ifcopenshell_ifcparse_file_schema_name(ifcopenshell_ifcparse_file_t* file) {
    if (file == nullptr || !file->file) {
        set_global_error("IfcParse handle is null");
        return nullptr;
    }

    try {
        clear_file_error(file);
        return file->file->schema()->name().c_str();
    } catch (const std::exception& e) {
        set_file_error(file, e.what());
        return nullptr;
    } catch (...) {
        set_file_error(file, "Unknown native exception while reading schema name");
        return nullptr;
    }
}

const ifcopenshell_ifcparse_entity_ref_t* ifcopenshell_ifcparse_file_header_file_description(
    ifcopenshell_ifcparse_file_t* file
) {
    if (file == nullptr || !file->file) {
        set_global_error("IfcParse handle is null");
        return nullptr;
    }

    try {
        clear_file_error(file);
        return to_entity_ref(file->file->header().file_description());
    } catch (const std::exception& e) {
        set_file_error(file, e.what());
        return nullptr;
    } catch (...) {
        set_file_error(file, "Unknown native exception while querying FILE_DESCRIPTION");
        return nullptr;
    }
}

const ifcopenshell_ifcparse_entity_ref_t* ifcopenshell_ifcparse_file_header_file_name(
    ifcopenshell_ifcparse_file_t* file
) {
    if (file == nullptr || !file->file) {
        set_global_error("IfcParse handle is null");
        return nullptr;
    }

    try {
        clear_file_error(file);
        return to_entity_ref(file->file->header().file_name());
    } catch (const std::exception& e) {
        set_file_error(file, e.what());
        return nullptr;
    } catch (...) {
        set_file_error(file, "Unknown native exception while querying FILE_NAME");
        return nullptr;
    }
}

const ifcopenshell_ifcparse_entity_ref_t* ifcopenshell_ifcparse_file_header_file_schema(
    ifcopenshell_ifcparse_file_t* file
) {
    if (file == nullptr || !file->file) {
        set_global_error("IfcParse handle is null");
        return nullptr;
    }

    try {
        clear_file_error(file);
        return to_entity_ref(file->file->header().file_schema());
    } catch (const std::exception& e) {
        set_file_error(file, e.what());
        return nullptr;
    } catch (...) {
        set_file_error(file, "Unknown native exception while querying FILE_SCHEMA");
        return nullptr;
    }
}

size_t ifcopenshell_ifcparse_file_entity_count(const ifcopenshell_ifcparse_file_t* file) {
    if (file == nullptr || !file->file) {
        return 0;
    }

    size_t count = 0;
    for (auto it = file->file->begin(); it != file->file->end(); ++it) {
        ++count;
    }
    return count;
}

ifcopenshell_ifcparse_entity_list_t* ifcopenshell_ifcparse_file_entities(
    ifcopenshell_ifcparse_file_t* file
) {
    if (file == nullptr || !file->file) {
        set_global_error("IfcParse handle is null");
        return nullptr;
    }

    try {
        aggregate_of_instance::ptr entities(new aggregate_of_instance());
        for (auto it = file->file->begin(); it != file->file->end(); ++it) {
            entities->push(it->second);
        }
        clear_file_error(file);
        return make_entity_list(file->file, entities);
    } catch (const std::exception& e) {
        set_file_error(file, e.what());
        return nullptr;
    } catch (...) {
        set_file_error(file, "Unknown native exception while collecting file entities");
        return nullptr;
    }
}

ifcopenshell_ifcparse_int_list_t* ifcopenshell_ifcparse_file_entity_ids(
    ifcopenshell_ifcparse_file_t* file
) {
    if (file == nullptr || !file->file) {
        set_global_error("IfcParse handle is null");
        return nullptr;
    }

    try {
        std::unique_ptr<ifcopenshell_ifcparse_int_list_t> list(new ifcopenshell_ifcparse_int_list_t());
        list->owner_file = file->file;
        list->cursor = 0;
        for (auto it = file->file->begin(); it != file->file->end(); ++it) {
            list->values.push_back(static_cast<int>(it->first));
        }
        clear_file_error(file);
        clear_int_list_error(list.get());
        return list.release();
    } catch (const std::exception& e) {
        set_file_error(file, e.what());
        return nullptr;
    } catch (...) {
        set_file_error(file, "Unknown native exception while collecting entity ids");
        return nullptr;
    }
}

const ifcopenshell_ifcparse_entity_ref_t* ifcopenshell_ifcparse_file_instance_by_id(
    ifcopenshell_ifcparse_file_t* file,
    int id
) {
    if (file == nullptr || !file->file) {
        set_global_error("IfcParse handle is null");
        return nullptr;
    }

    try {
        clear_file_error(file);
        return to_entity_ref(file->file->instance_by_id(id));
    } catch (const std::exception& e) {
        set_file_error(file, e.what());
        return nullptr;
    } catch (...) {
        set_file_error(file, "Unknown native exception while querying instance by id");
        return nullptr;
    }
}

const ifcopenshell_ifcparse_entity_ref_t* ifcopenshell_ifcparse_file_instance_by_guid(
    ifcopenshell_ifcparse_file_t* file,
    const char* guid
) {
    if (file == nullptr || !file->file) {
        set_global_error("IfcParse handle is null");
        return nullptr;
    }
    if (guid == nullptr || guid[0] == '\0') {
        set_file_error(file, "Invalid GlobalId");
        return nullptr;
    }

    try {
        clear_file_error(file);
        return to_entity_ref(file->file->instance_by_guid(std::string(guid)));
    } catch (const std::exception& e) {
        set_file_error(file, e.what());
        return nullptr;
    } catch (...) {
        set_file_error(file, "Unknown native exception while querying instance by GlobalId");
        return nullptr;
    }
}

ifcopenshell_ifcparse_entity_list_t* ifcopenshell_ifcparse_file_instances_by_type(
    ifcopenshell_ifcparse_file_t* file,
    const char* type_name,
    int include_subtypes
) {
    if (file == nullptr || !file->file) {
        set_global_error("IfcParse handle is null");
        return nullptr;
    }
    if (type_name == nullptr || type_name[0] == '\0') {
        set_file_error(file, "Invalid type name");
        return nullptr;
    }

    try {
        aggregate_of_instance::ptr entities = include_subtypes != 0
            ? file->file->instances_by_type(std::string(type_name))
            : file->file->instances_by_type_excl_subtypes(std::string(type_name));
        clear_file_error(file);
        return make_entity_list(file->file, entities);
    } catch (const std::exception& e) {
        set_file_error(file, e.what());
        return nullptr;
    } catch (...) {
        set_file_error(file, "Unknown native exception while querying instances by type");
        return nullptr;
    }
}

ifcopenshell_ifcparse_entity_list_t* ifcopenshell_ifcparse_file_instances_by_reference(
    ifcopenshell_ifcparse_file_t* file,
    int id
) {
    if (file == nullptr || !file->file) {
        set_global_error("IfcParse handle is null");
        return nullptr;
    }

    try {
        clear_file_error(file);
        return make_entity_list(file->file, file->file->instances_by_reference(id));
    } catch (const std::exception& e) {
        set_file_error(file, e.what());
        return nullptr;
    } catch (...) {
        set_file_error(file, "Unknown native exception while querying instances by reference");
        return nullptr;
    }
}

ifcopenshell_ifcparse_entity_list_t* ifcopenshell_ifcparse_file_get_inverse(
    ifcopenshell_ifcparse_file_t* file,
    int instance_id,
    const char* type_name,
    int attribute_index
) {
    if (file == nullptr || !file->file) {
        set_global_error("IfcParse handle is null");
        return nullptr;
    }
    if (type_name == nullptr && attribute_index != -1) {
        set_file_error(file, "type_name must be provided when attribute_index is specified");
        return nullptr;
    }

    try {
        const IfcParse::declaration* type_decl = nullptr;
        if (type_name != nullptr && type_name[0] != '\0') {
            type_decl = file->file->schema()->declaration_by_name(std::string(type_name));
        }
        clear_file_error(file);
        return make_entity_list(file->file, file->file->getInverse(instance_id, type_decl, attribute_index));
    } catch (const std::exception& e) {
        set_file_error(file, e.what());
        return nullptr;
    } catch (...) {
        set_file_error(file, "Unknown native exception while querying inverse references");
        return nullptr;
    }
}

size_t ifcopenshell_ifcparse_file_get_total_inverses(
    ifcopenshell_ifcparse_file_t* file,
    int instance_id
) {
    if (file == nullptr || !file->file) {
        return 0;
    }

    try {
        clear_file_error(file);
        return file->file->getTotalInverses(instance_id);
    } catch (const std::exception& e) {
        set_file_error(file, e.what());
        return 0;
    } catch (...) {
        set_file_error(file, "Unknown native exception while counting inverse references");
        return 0;
    }
}

ifcopenshell_ifcparse_int_list_t* ifcopenshell_ifcparse_file_get_inverse_indices(
    ifcopenshell_ifcparse_file_t* file,
    int instance_id
) {
    if (file == nullptr || !file->file) {
        set_global_error("IfcParse handle is null");
        return nullptr;
    }

    try {
        std::unique_ptr<ifcopenshell_ifcparse_int_list_t> list(new ifcopenshell_ifcparse_int_list_t());
        list->owner_file = file->file;
        list->values = file->file->get_inverse_indices(instance_id);
        list->cursor = 0;
        clear_file_error(file);
        clear_int_list_error(list.get());
        return list.release();
    } catch (const std::exception& e) {
        set_file_error(file, e.what());
        return nullptr;
    } catch (...) {
        set_file_error(file, "Unknown native exception while querying inverse indices");
        return nullptr;
    }
}

ifcopenshell_ifcparse_entity_list_t* ifcopenshell_ifcparse_file_traverse_by_id(
    ifcopenshell_ifcparse_file_t* file,
    int instance_id,
    int max_level,
    int breadth_first
) {
    if (file == nullptr || !file->file) {
        set_global_error("IfcParse handle is null");
        return nullptr;
    }

    try {
        auto* root = file->file->instance_by_id(instance_id);
        aggregate_of_instance::ptr entities = breadth_first != 0
            ? IfcParse::traverse_breadth_first(root, max_level)
            : IfcParse::traverse(root, max_level);
        clear_file_error(file);
        return make_entity_list(file->file, entities);
    } catch (const std::exception& e) {
        set_file_error(file, e.what());
        return nullptr;
    } catch (...) {
        set_file_error(file, "Unknown native exception while traversing entity graph");
        return nullptr;
    }
}

ifcopenshell_ifcparse_entity_list_t* ifcopenshell_ifcparse_file_traverse(
    ifcopenshell_ifcparse_file_t* file,
    const ifcopenshell_ifcparse_entity_ref_t* entity,
    int max_level,
    int breadth_first
) {
    if (file == nullptr || !file->file) {
        set_global_error("IfcParse handle is null");
        return nullptr;
    }
    if (entity == nullptr) {
        set_file_error(file, "Entity handle is null");
        return nullptr;
    }

    try {
        auto* root = from_entity_ref_mut(entity);
        aggregate_of_instance::ptr entities = breadth_first != 0
            ? IfcParse::traverse_breadth_first(root, max_level)
            : IfcParse::traverse(root, max_level);
        clear_file_error(file);
        return make_entity_list(file->file, entities);
    } catch (const std::exception& e) {
        set_file_error(file, e.what());
        return nullptr;
    } catch (...) {
        set_file_error(file, "Unknown native exception while traversing entity graph");
        return nullptr;
    }
}

const ifcopenshell_ifcparse_entity_ref_t* ifcopenshell_ifcparse_file_create_entity_by_type(
    ifcopenshell_ifcparse_file_t* file,
    const char* type_name
) {
    if (file == nullptr || !file->file) {
        set_global_error("IfcParse handle is null");
        return nullptr;
    }
    if (type_name == nullptr || type_name[0] == '\0') {
        set_file_error(file, "Invalid type name");
        return nullptr;
    }

    try {
        const IfcParse::declaration* decl = file->file->schema()->declaration_by_name(std::string(type_name));
        if (decl->as_entity() == nullptr) {
            set_file_error(file, "Requested type is not an entity declaration");
            return nullptr;
        }

        auto* entity = file->file->create(decl);
        if (entity == nullptr) {
            set_file_error(file, "Entity creation returned null");
            return nullptr;
        }

        clear_file_error(file);
        return to_entity_ref(entity);
    } catch (const std::exception& e) {
        set_file_error(file, e.what());
        return nullptr;
    } catch (...) {
        set_file_error(file, "Unknown native exception while creating entity");
        return nullptr;
    }
}

const ifcopenshell_ifcparse_entity_ref_t* ifcopenshell_ifcparse_file_create_entity_by_type_with_id(
    ifcopenshell_ifcparse_file_t* file,
    const char* type_name,
    int id
) {
    if (file == nullptr || !file->file) {
        set_global_error("IfcParse handle is null");
        return nullptr;
    }
    if (type_name == nullptr || type_name[0] == '\0') {
        set_file_error(file, "Invalid type name");
        return nullptr;
    }

    try {
        const IfcParse::declaration* decl = file->file->schema()->declaration_by_name(std::string(type_name));
        if (decl->as_entity() == nullptr) {
            set_file_error(file, "Requested type is not an entity declaration");
            return nullptr;
        }

        IfcEntityInstanceData data(in_memory_attribute_storage(decl->as_entity()->attribute_count()));
        auto* entity = file->file->schema()->instantiate(decl, std::move(data));
        if (entity == nullptr) {
            set_file_error(file, "Entity instantiation returned null");
            return nullptr;
        }

        auto* added = file->file->addEntity(entity, id);
        if (added == nullptr) {
            set_file_error(file, "Entity insertion returned null");
            return nullptr;
        }

        clear_file_error(file);
        return to_entity_ref(added);
    } catch (const std::exception& e) {
        set_file_error(file, e.what());
        return nullptr;
    } catch (...) {
        set_file_error(file, "Unknown native exception while creating entity with explicit id");
        return nullptr;
    }
}

const ifcopenshell_ifcparse_entity_ref_t* ifcopenshell_ifcparse_file_add_entity(
    ifcopenshell_ifcparse_file_t* file,
    const ifcopenshell_ifcparse_entity_ref_t* entity,
    int id
) {
    if (file == nullptr || !file->file) {
        set_global_error("IfcParse handle is null");
        return nullptr;
    }
    if (entity == nullptr) {
        set_file_error(file, "Entity handle is null");
        return nullptr;
    }

    try {
        auto* added = file->file->addEntity(from_entity_ref_mut(entity), id);
        if (added == nullptr) {
            set_file_error(file, "Entity insertion returned null");
            return nullptr;
        }
        clear_file_error(file);
        return to_entity_ref(added);
    } catch (const std::exception& e) {
        set_file_error(file, e.what());
        return nullptr;
    } catch (...) {
        set_file_error(file, "Unknown native exception while adding entity");
        return nullptr;
    }
}

size_t ifcopenshell_ifcparse_file_add_entities(
    ifcopenshell_ifcparse_file_t* file,
    const ifcopenshell_ifcparse_entity_ref_t* const* entities,
    size_t entity_count
) {
    if (file == nullptr || !file->file) {
        set_global_error("IfcParse handle is null");
        return 0;
    }
    if (entities == nullptr && entity_count > 0) {
        set_file_error(file, "Entity array is null");
        return 0;
    }

    try {
        aggregate_of_instance::ptr aggregate(new aggregate_of_instance());
        for (size_t i = 0; i < entity_count; ++i) {
            auto* entity = from_entity_ref_mut(entities[i]);
            if (entity == nullptr) {
                set_file_error(file, "Entity array contains null handle");
                return 0;
            }
            aggregate->push(entity);
        }
        file->file->addEntities(aggregate);
        clear_file_error(file);
        return entity_count;
    } catch (const std::exception& e) {
        set_file_error(file, e.what());
        return 0;
    } catch (...) {
        set_file_error(file, "Unknown native exception while adding entities");
        return 0;
    }
}

int ifcopenshell_ifcparse_file_remove_entity_by_id(
    ifcopenshell_ifcparse_file_t* file,
    int id
) {
    if (file == nullptr || !file->file) {
        set_global_error("IfcParse handle is null");
        return 0;
    }

    try {
        auto* entity = file->file->instance_by_id(id);
        file->file->removeEntity(entity);
        clear_file_error(file);
        return 1;
    } catch (const std::exception& e) {
        set_file_error(file, e.what());
        return 0;
    } catch (...) {
        set_file_error(file, "Unknown native exception while removing entity");
        return 0;
    }
}

int ifcopenshell_ifcparse_file_remove_entity(
    ifcopenshell_ifcparse_file_t* file,
    const ifcopenshell_ifcparse_entity_ref_t* entity
) {
    if (file == nullptr || !file->file) {
        set_global_error("IfcParse handle is null");
        return 0;
    }
    if (entity == nullptr) {
        set_file_error(file, "Entity handle is null");
        return 0;
    }

    try {
        file->file->removeEntity(from_entity_ref_mut(entity));
        clear_file_error(file);
        return 1;
    } catch (const std::exception& e) {
        set_file_error(file, e.what());
        return 0;
    } catch (...) {
        set_file_error(file, "Unknown native exception while removing entity");
        return 0;
    }
}

void ifcopenshell_ifcparse_file_batch_begin(ifcopenshell_ifcparse_file_t* file) {
    if (file == nullptr || !file->file) {
        set_global_error("IfcParse handle is null");
        return;
    }
    try {
        file->file->batch();
        clear_file_error(file);
    } catch (const std::exception& e) {
        set_file_error(file, e.what());
    } catch (...) {
        set_file_error(file, "Unknown native exception while beginning batch mode");
    }
}

void ifcopenshell_ifcparse_file_batch_end(ifcopenshell_ifcparse_file_t* file) {
    if (file == nullptr || !file->file) {
        set_global_error("IfcParse handle is null");
        return;
    }
    try {
        file->file->unbatch();
        clear_file_error(file);
    } catch (const std::exception& e) {
        set_file_error(file, e.what());
    } catch (...) {
        set_file_error(file, "Unknown native exception while finalizing batch mode");
    }
}

unsigned int ifcopenshell_ifcparse_file_max_id(const ifcopenshell_ifcparse_file_t* file) {
    if (file == nullptr || !file->file) {
        return 0;
    }
    return file->file->getMaxId();
}

void ifcopenshell_ifcparse_file_recalculate_id_counter(ifcopenshell_ifcparse_file_t* file) {
    if (file == nullptr || !file->file) {
        set_global_error("IfcParse handle is null");
        return;
    }
    try {
        file->file->recalculate_id_counter();
        clear_file_error(file);
    } catch (const std::exception& e) {
        set_file_error(file, e.what());
    } catch (...) {
        set_file_error(file, "Unknown native exception while recalculating id counter");
    }
}

int ifcopenshell_ifcparse_file_write_spf(
    ifcopenshell_ifcparse_file_t* file,
    const char* path
) {
    if (file == nullptr || !file->file) {
        set_global_error("IfcParse handle is null");
        return 0;
    }
    if (path == nullptr || path[0] == '\0') {
        set_file_error(file, "Invalid output path");
        return 0;
    }

    try {
        std::ofstream out(path, std::ios::out | std::ios::trunc);
        if (!out.is_open()) {
            set_file_error(file, "Unable to open output path for writing");
            return 0;
        }
        out << *file->file;
        if (!out.good()) {
            set_file_error(file, "Failed while writing IFC SPF output");
            return 0;
        }
        clear_file_error(file);
        return 1;
    } catch (const std::exception& e) {
        set_file_error(file, e.what());
        return 0;
    } catch (...) {
        set_file_error(file, "Unknown native exception while writing IFC SPF output");
        return 0;
    }
}

const char* ifcopenshell_ifcparse_file_to_spf(ifcopenshell_ifcparse_file_t* file) {
    if (file == nullptr || !file->file) {
        set_global_error("IfcParse handle is null");
        return nullptr;
    }

    try {
        std::ostringstream out;
        out << *file->file;
        g_spf_buffer = out.str();
        clear_file_error(file);
        return g_spf_buffer.c_str();
    } catch (const std::exception& e) {
        set_file_error(file, e.what());
        return nullptr;
    } catch (...) {
        set_file_error(file, "Unknown native exception while serializing IFC SPF output");
        return nullptr;
    }
}

double ifcopenshell_ifcparse_file_unit_scale(
    ifcopenshell_ifcparse_file_t* file,
    const char* unit_type
) {
    if (file == nullptr || !file->file) {
        set_global_error("IfcParse handle is null");
        return 0.0;
    }
    if (unit_type == nullptr || unit_type[0] == '\0') {
        set_file_error(file, "Invalid unit type");
        return 0.0;
    }

    try {
        const auto unit_info = file->file->getUnit(std::string(unit_type));
        clear_file_error(file);
        return unit_info.second;
    } catch (const std::exception& e) {
        set_file_error(file, e.what());
        return 0.0;
    } catch (...) {
        set_file_error(file, "Unknown native exception while querying unit scale");
        return 0.0;
    }
}

const ifcopenshell_ifcparse_entity_ref_t* ifcopenshell_ifcparse_file_unit_entity(
    ifcopenshell_ifcparse_file_t* file,
    const char* unit_type
) {
    if (file == nullptr || !file->file) {
        set_global_error("IfcParse handle is null");
        return nullptr;
    }
    if (unit_type == nullptr || unit_type[0] == '\0') {
        set_file_error(file, "Invalid unit type");
        return nullptr;
    }

    try {
        const auto unit_info = file->file->getUnit(std::string(unit_type));
        clear_file_error(file);
        return to_entity_ref(unit_info.first);
    } catch (const std::exception& e) {
        set_file_error(file, e.what());
        return nullptr;
    } catch (...) {
        set_file_error(file, "Unknown native exception while querying unit entity");
        return nullptr;
    }
}

void ifcopenshell_ifcparse_entity_list_close(ifcopenshell_ifcparse_entity_list_t* list) {
    delete list;
}

size_t ifcopenshell_ifcparse_entity_list_count(const ifcopenshell_ifcparse_entity_list_t* list) {
    if (list == nullptr || !list->entities) {
        return 0;
    }
    return list->entities->size();
}

void ifcopenshell_ifcparse_entity_list_reset(ifcopenshell_ifcparse_entity_list_t* list) {
    if (list == nullptr) {
        return;
    }
    list->cursor = 0;
    clear_list_error(list);
}

const ifcopenshell_ifcparse_entity_ref_t* ifcopenshell_ifcparse_entity_list_get(
    const ifcopenshell_ifcparse_entity_list_t* list,
    size_t index
) {
    if (list == nullptr || !list->entities) {
        set_global_error("Entity list handle is null");
        return nullptr;
    }
    if (index >= list->entities->size()) {
        set_list_error(const_cast<ifcopenshell_ifcparse_entity_list_t*>(list), "Entity list index out of range");
        return nullptr;
    }
    if (index > static_cast<size_t>((std::numeric_limits<int>::max)())) {
        set_list_error(const_cast<ifcopenshell_ifcparse_entity_list_t*>(list), "Entity list index exceeds integer limits");
        return nullptr;
    }

    clear_list_error(const_cast<ifcopenshell_ifcparse_entity_list_t*>(list));
    return to_entity_ref((*list->entities)[static_cast<int>(index)]);
}

const ifcopenshell_ifcparse_entity_ref_t* ifcopenshell_ifcparse_entity_list_next(
    ifcopenshell_ifcparse_entity_list_t* list
) {
    if (list == nullptr || !list->entities) {
        set_global_error("Entity list handle is null");
        return nullptr;
    }
    if (list->cursor >= list->entities->size()) {
        clear_list_error(list);
        return nullptr;
    }
    const ifcopenshell_ifcparse_entity_ref_t* next = ifcopenshell_ifcparse_entity_list_get(list, list->cursor);
    if (next != nullptr) {
        ++list->cursor;
    }
    return next;
}

int ifcopenshell_ifcparse_entity_id(const ifcopenshell_ifcparse_entity_ref_t* entity) {
    const IfcUtil::IfcBaseClass* inst = from_entity_ref(entity);
    if (inst == nullptr) {
        return 0;
    }
    return static_cast<int>(inst->id());
}

const char* ifcopenshell_ifcparse_entity_type_name(const ifcopenshell_ifcparse_entity_ref_t* entity) {
    const IfcUtil::IfcBaseClass* inst = from_entity_ref(entity);
    if (inst == nullptr) {
        set_global_error("Entity handle is null");
        return nullptr;
    }

    try {
        clear_global_error();
        return inst->declaration().name().c_str();
    } catch (const std::exception& e) {
        set_global_error(e.what());
        return nullptr;
    } catch (...) {
        set_global_error("Unknown native exception while reading entity type");
        return nullptr;
    }
}

const char* ifcopenshell_ifcparse_entity_type_name_with_schema(
    const ifcopenshell_ifcparse_entity_ref_t* entity
) {
    const IfcUtil::IfcBaseClass* inst = from_entity_ref(entity);
    if (inst == nullptr) {
        set_global_error("Entity handle is null");
        return nullptr;
    }

    try {
        const auto* schema = inst->declaration().schema();
        if (schema == nullptr) {
            set_global_error("Entity schema is null");
            return nullptr;
        }
        g_entity_type_name_with_schema_buffer = schema->name() + "." + inst->declaration().name();
        clear_global_error();
        return g_entity_type_name_with_schema_buffer.c_str();
    } catch (const std::exception& e) {
        set_global_error(e.what());
        return nullptr;
    } catch (...) {
        set_global_error("Unknown native exception while reading schema-qualified type name");
        return nullptr;
    }
}

int ifcopenshell_ifcparse_entity_is_a(
    const ifcopenshell_ifcparse_entity_ref_t* entity,
    const char* type_name
) {
    const IfcUtil::IfcBaseClass* inst = from_entity_ref(entity);
    if (inst == nullptr) {
        set_global_error("Entity handle is null");
        return 0;
    }
    if (type_name == nullptr || type_name[0] == '\0') {
        set_global_error("Invalid type name");
        return 0;
    }

    try {
        clear_global_error();
        return inst->declaration().is(std::string(type_name)) ? 1 : 0;
    } catch (const std::exception& e) {
        set_global_error(e.what());
        return 0;
    } catch (...) {
        set_global_error("Unknown native exception while checking entity type");
        return 0;
    }
}

int ifcopenshell_ifcparse_entity_attribute_category(
    const ifcopenshell_ifcparse_entity_ref_t* entity,
    const char* attribute_name
) {
    const IfcUtil::IfcBaseClass* inst = from_entity_ref(entity);
    if (inst == nullptr) {
        set_global_error("Entity handle is null");
        return 0;
    }
    if (attribute_name == nullptr || attribute_name[0] == '\0') {
        set_global_error("Attribute name is empty");
        return 0;
    }

    try {
        const auto* entity_decl = inst->declaration().as_entity();
        const std::string requested(attribute_name);
        if (entity_decl == nullptr) {
            clear_global_error();
            return requested == "wrappedValue" ? 1 : 0;
        }

        const auto attrs = entity_decl->all_attributes();
        for (const auto* attr : attrs) {
            if (attr != nullptr && attr->name() == requested) {
                clear_global_error();
                return 1;
            }
        }

        const auto inverse_attrs = entity_decl->all_inverse_attributes();
        for (const auto* inverse_attr : inverse_attrs) {
            if (inverse_attr != nullptr && inverse_attr->name() == requested) {
                clear_global_error();
                return 2;
            }
        }

        clear_global_error();
        return 0;
    } catch (const std::exception& e) {
        set_global_error(e.what());
        return 0;
    } catch (...) {
        set_global_error("Unknown native exception while querying attribute category");
        return 0;
    }
}

ifcopenshell_ifcparse_string_list_t* ifcopenshell_ifcparse_entity_attribute_names(
    const ifcopenshell_ifcparse_entity_ref_t* entity
) {
    const IfcUtil::IfcBaseClass* inst = from_entity_ref(entity);
    if (inst == nullptr) {
        set_global_error("Entity handle is null");
        return nullptr;
    }

    try {
        std::unique_ptr<ifcopenshell_ifcparse_string_list_t> list(new ifcopenshell_ifcparse_string_list_t());
        const auto* entity_decl = inst->declaration().as_entity();
        if (entity_decl == nullptr) {
            list->values.push_back("wrappedValue");
        } else {
            const auto attrs = entity_decl->all_attributes();
            list->values.reserve(attrs.size());
            for (const auto* attr : attrs) {
                if (attr != nullptr) {
                    list->values.push_back(attr->name());
                }
            }
        }
        list->cursor = 0;
        clear_string_list_error(list.get());
        return list.release();
    } catch (const std::exception& e) {
        set_global_error(e.what());
        return nullptr;
    } catch (...) {
        set_global_error("Unknown native exception while listing attribute names");
        return nullptr;
    }
}

ifcopenshell_ifcparse_string_list_t* ifcopenshell_ifcparse_entity_inverse_attribute_names(
    const ifcopenshell_ifcparse_entity_ref_t* entity
) {
    const IfcUtil::IfcBaseClass* inst = from_entity_ref(entity);
    if (inst == nullptr) {
        set_global_error("Entity handle is null");
        return nullptr;
    }

    try {
        std::unique_ptr<ifcopenshell_ifcparse_string_list_t> list(new ifcopenshell_ifcparse_string_list_t());
        const auto* entity_decl = inst->declaration().as_entity();
        if (entity_decl != nullptr) {
            const auto attrs = entity_decl->all_inverse_attributes();
            list->values.reserve(attrs.size());
            for (const auto* attr : attrs) {
                if (attr != nullptr) {
                    list->values.push_back(attr->name());
                }
            }
        }
        list->cursor = 0;
        clear_string_list_error(list.get());
        return list.release();
    } catch (const std::exception& e) {
        set_global_error(e.what());
        return nullptr;
    } catch (...) {
        set_global_error("Unknown native exception while listing inverse attribute names");
        return nullptr;
    }
}

size_t ifcopenshell_ifcparse_entity_argument_count(
    const ifcopenshell_ifcparse_entity_ref_t* entity
) {
    const IfcUtil::IfcBaseClass* inst = from_entity_ref(entity);
    if (inst == nullptr) {
        return 0;
    }
    if (inst->declaration().as_entity() != nullptr) {
        return static_cast<size_t>(inst->declaration().as_entity()->attribute_count());
    }
    return 1;
}

int ifcopenshell_ifcparse_entity_argument_index(
    const ifcopenshell_ifcparse_entity_ref_t* entity,
    const char* argument_name
) {
    const IfcUtil::IfcBaseClass* inst = from_entity_ref(entity);
    if (inst == nullptr) {
        set_global_error("Entity handle is null");
        return -1;
    }
    if (argument_name == nullptr || argument_name[0] == '\0') {
        set_global_error("Invalid argument name");
        return -1;
    }

    try {
        if (inst->declaration().as_entity() != nullptr) {
            clear_global_error();
            return inst->declaration().as_entity()->attribute_index(std::string(argument_name));
        }
        if (std::string(argument_name) == "wrappedValue") {
            clear_global_error();
            return 0;
        }
        set_global_error("Argument name not found");
        return -1;
    } catch (const std::exception& e) {
        set_global_error(e.what());
        return -1;
    } catch (...) {
        set_global_error("Unknown native exception while looking up argument index");
        return -1;
    }
}

const char* ifcopenshell_ifcparse_entity_argument_name(
    const ifcopenshell_ifcparse_entity_ref_t* entity,
    size_t index
) {
    const IfcUtil::IfcBaseClass* inst = from_entity_ref(entity);
    if (inst == nullptr) {
        set_global_error("Entity handle is null");
        return nullptr;
    }

    try {
        if (inst->declaration().as_entity() != nullptr) {
            clear_global_error();
            return inst->declaration().as_entity()->attribute_by_index(index)->name().c_str();
        }
        if (index == 0) {
            static std::string wrapped_name = "wrappedValue";
            clear_global_error();
            return wrapped_name.c_str();
        }
        set_global_error("Argument index out of range");
        return nullptr;
    } catch (const std::exception& e) {
        set_global_error(e.what());
        return nullptr;
    } catch (...) {
        set_global_error("Unknown native exception while reading argument name");
        return nullptr;
    }
}

ifcopenshell_ifcparse_argument_type_t ifcopenshell_ifcparse_entity_argument_declared_type(
    const ifcopenshell_ifcparse_entity_ref_t* entity,
    size_t index
) {
    const IfcUtil::IfcBaseClass* inst = from_entity_ref(entity);
    if (inst == nullptr) {
        set_global_error("Entity handle is null");
        return IFCOPENSHELL_IFCPARSE_ARG_UNKNOWN;
    }

    try {
        clear_global_error();
        return to_c_argument_type(declared_argument_type(inst, index));
    } catch (const std::exception& e) {
        set_global_error(e.what());
        return IFCOPENSHELL_IFCPARSE_ARG_UNKNOWN;
    } catch (...) {
        set_global_error("Unknown native exception while reading declared argument type");
        return IFCOPENSHELL_IFCPARSE_ARG_UNKNOWN;
    }
}

ifcopenshell_ifcparse_argument_type_t ifcopenshell_ifcparse_entity_argument_value_type(
    const ifcopenshell_ifcparse_entity_ref_t* entity,
    size_t index
) {
    const IfcUtil::IfcBaseClass* inst = from_entity_ref(entity);
    if (inst == nullptr) {
        set_global_error("Entity handle is null");
        return IFCOPENSHELL_IFCPARSE_ARG_UNKNOWN;
    }

    try {
        clear_global_error();
        const auto argument = inst->get_attribute_value(index);
        return to_c_argument_type(argument.type());
    } catch (const std::exception& e) {
        set_global_error(e.what());
        return IFCOPENSHELL_IFCPARSE_ARG_UNKNOWN;
    } catch (...) {
        set_global_error("Unknown native exception while reading argument value type");
        return IFCOPENSHELL_IFCPARSE_ARG_UNKNOWN;
    }
}

int ifcopenshell_ifcparse_entity_argument_is_null(
    const ifcopenshell_ifcparse_entity_ref_t* entity,
    size_t index
) {
    const IfcUtil::IfcBaseClass* inst = from_entity_ref(entity);
    if (inst == nullptr) {
        set_global_error("Entity handle is null");
        return 0;
    }

    try {
        clear_global_error();
        return inst->get_attribute_value(index).isNull() ? 1 : 0;
    } catch (const std::exception& e) {
        set_global_error(e.what());
        return 0;
    } catch (...) {
        set_global_error("Unknown native exception while checking null argument value");
        return 0;
    }
}

const char* ifcopenshell_ifcparse_entity_to_string(
    const ifcopenshell_ifcparse_entity_ref_t* entity,
    int valid_spf
) {
    const IfcUtil::IfcBaseClass* inst = from_entity_ref(entity);
    if (inst == nullptr) {
        set_global_error("Entity handle is null");
        return nullptr;
    }

    try {
        std::ostringstream out;
        inst->toString(out, valid_spf != 0);
        g_entity_spf_buffer = out.str();
        clear_global_error();
        return g_entity_spf_buffer.c_str();
    } catch (const std::exception& e) {
        set_global_error(e.what());
        return nullptr;
    } catch (...) {
        set_global_error("Unknown native exception while serializing entity");
        return nullptr;
    }
}

int ifcopenshell_ifcparse_entity_get_argument_as_int(
    const ifcopenshell_ifcparse_entity_ref_t* entity,
    size_t index,
    int* out_value
) {
    const IfcUtil::IfcBaseClass* inst = from_entity_ref(entity);
    if (inst == nullptr || out_value == nullptr) {
        set_global_error("Invalid argument handle");
        return 0;
    }
    try {
        const auto argument = inst->get_attribute_value(index);
        if (argument.type() != IfcUtil::Argument_INT) {
            set_global_error("Argument is not an integer");
            return 0;
        }
        *out_value = static_cast<int>(argument);
        clear_global_error();
        return 1;
    } catch (const std::exception& e) {
        set_global_error(e.what());
        return 0;
    } catch (...) {
        set_global_error("Unknown native exception while reading integer argument");
        return 0;
    }
}

int ifcopenshell_ifcparse_entity_get_argument_as_bool(
    const ifcopenshell_ifcparse_entity_ref_t* entity,
    size_t index,
    int* out_value
) {
    const IfcUtil::IfcBaseClass* inst = from_entity_ref(entity);
    if (inst == nullptr || out_value == nullptr) {
        set_global_error("Invalid argument handle");
        return 0;
    }
    try {
        const auto argument = inst->get_attribute_value(index);
        if (argument.type() != IfcUtil::Argument_BOOL) {
            set_global_error("Argument is not a boolean");
            return 0;
        }
        *out_value = static_cast<bool>(argument) ? 1 : 0;
        clear_global_error();
        return 1;
    } catch (const std::exception& e) {
        set_global_error(e.what());
        return 0;
    } catch (...) {
        set_global_error("Unknown native exception while reading boolean argument");
        return 0;
    }
}

int ifcopenshell_ifcparse_entity_get_argument_as_logical(
    const ifcopenshell_ifcparse_entity_ref_t* entity,
    size_t index,
    int* out_value
) {
    const IfcUtil::IfcBaseClass* inst = from_entity_ref(entity);
    if (inst == nullptr || out_value == nullptr) {
        set_global_error("Invalid argument handle");
        return 0;
    }
    try {
        const auto argument = inst->get_attribute_value(index);
        if (argument.type() != IfcUtil::Argument_LOGICAL) {
            set_global_error("Argument is not a logical");
            return 0;
        }
        boost::logic::tribool logical = static_cast<boost::logic::tribool>(argument);
        *out_value = boost::logic::indeterminate(logical) ? -1 : (logical ? 1 : 0);
        clear_global_error();
        return 1;
    } catch (const std::exception& e) {
        set_global_error(e.what());
        return 0;
    } catch (...) {
        set_global_error("Unknown native exception while reading logical argument");
        return 0;
    }
}

int ifcopenshell_ifcparse_entity_get_argument_as_double(
    const ifcopenshell_ifcparse_entity_ref_t* entity,
    size_t index,
    double* out_value
) {
    const IfcUtil::IfcBaseClass* inst = from_entity_ref(entity);
    if (inst == nullptr || out_value == nullptr) {
        set_global_error("Invalid argument handle");
        return 0;
    }
    try {
        const auto argument = inst->get_attribute_value(index);
        if (argument.type() != IfcUtil::Argument_DOUBLE) {
            set_global_error("Argument is not a double");
            return 0;
        }
        *out_value = static_cast<double>(argument);
        clear_global_error();
        return 1;
    } catch (const std::exception& e) {
        set_global_error(e.what());
        return 0;
    } catch (...) {
        set_global_error("Unknown native exception while reading double argument");
        return 0;
    }
}

const char* ifcopenshell_ifcparse_entity_get_argument_as_string(
    const ifcopenshell_ifcparse_entity_ref_t* entity,
    size_t index
) {
    const IfcUtil::IfcBaseClass* inst = from_entity_ref(entity);
    if (inst == nullptr) {
        set_global_error("Entity handle is null");
        return nullptr;
    }
    try {
        g_argument_string_buffer = static_cast<std::string>(inst->get_attribute_value(index));
        clear_global_error();
        return g_argument_string_buffer.c_str();
    } catch (const std::exception& e) {
        set_global_error(e.what());
        return nullptr;
    } catch (...) {
        set_global_error("Unknown native exception while reading string argument");
        return nullptr;
    }
}

const ifcopenshell_ifcparse_entity_ref_t* ifcopenshell_ifcparse_entity_get_argument_as_entity(
    const ifcopenshell_ifcparse_entity_ref_t* entity,
    size_t index
) {
    const IfcUtil::IfcBaseClass* inst = from_entity_ref(entity);
    if (inst == nullptr) {
        set_global_error("Entity handle is null");
        return nullptr;
    }
    try {
        const auto argument = inst->get_attribute_value(index);
        if (argument.type() != IfcUtil::Argument_ENTITY_INSTANCE) {
            set_global_error("Argument is not an entity instance");
            return nullptr;
        }
        clear_global_error();
        return to_entity_ref(static_cast<IfcUtil::IfcBaseClass*>(argument));
    } catch (const std::exception& e) {
        set_global_error(e.what());
        return nullptr;
    } catch (...) {
        set_global_error("Unknown native exception while reading entity argument");
        return nullptr;
    }
}

ifcopenshell_ifcparse_int_list_t* ifcopenshell_ifcparse_entity_get_argument_as_int_list(
    const ifcopenshell_ifcparse_entity_ref_t* entity,
    size_t index
) {
    const IfcUtil::IfcBaseClass* inst = from_entity_ref(entity);
    if (inst == nullptr) {
        set_global_error("Entity handle is null");
        return nullptr;
    }
    try {
        const auto argument = inst->get_attribute_value(index);
        if (argument.type() != IfcUtil::Argument_AGGREGATE_OF_INT) {
            set_global_error("Argument is not an integer aggregate");
            return nullptr;
        }
        std::unique_ptr<ifcopenshell_ifcparse_int_list_t> list(new ifcopenshell_ifcparse_int_list_t());
        list->values = static_cast<std::vector<int>>(argument);
        list->cursor = 0;
        clear_int_list_error(list.get());
        return list.release();
    } catch (const std::exception& e) {
        set_global_error(e.what());
        return nullptr;
    } catch (...) {
        set_global_error("Unknown native exception while reading integer aggregate argument");
        return nullptr;
    }
}

ifcopenshell_ifcparse_double_list_t* ifcopenshell_ifcparse_entity_get_argument_as_double_list(
    const ifcopenshell_ifcparse_entity_ref_t* entity,
    size_t index
) {
    const IfcUtil::IfcBaseClass* inst = from_entity_ref(entity);
    if (inst == nullptr) {
        set_global_error("Entity handle is null");
        return nullptr;
    }
    try {
        const auto argument = inst->get_attribute_value(index);
        if (argument.type() != IfcUtil::Argument_AGGREGATE_OF_DOUBLE) {
            set_global_error("Argument is not a double aggregate");
            return nullptr;
        }
        std::unique_ptr<ifcopenshell_ifcparse_double_list_t> list(new ifcopenshell_ifcparse_double_list_t());
        list->values = static_cast<std::vector<double>>(argument);
        list->cursor = 0;
        clear_double_list_error(list.get());
        return list.release();
    } catch (const std::exception& e) {
        set_global_error(e.what());
        return nullptr;
    } catch (...) {
        set_global_error("Unknown native exception while reading double aggregate argument");
        return nullptr;
    }
}

ifcopenshell_ifcparse_string_list_t* ifcopenshell_ifcparse_entity_get_argument_as_string_list(
    const ifcopenshell_ifcparse_entity_ref_t* entity,
    size_t index
) {
    const IfcUtil::IfcBaseClass* inst = from_entity_ref(entity);
    if (inst == nullptr) {
        set_global_error("Entity handle is null");
        return nullptr;
    }
    try {
        const auto argument = inst->get_attribute_value(index);
        std::unique_ptr<ifcopenshell_ifcparse_string_list_t> list(new ifcopenshell_ifcparse_string_list_t());
        if (argument.type() == IfcUtil::Argument_AGGREGATE_OF_STRING) {
            list->values = static_cast<std::vector<std::string>>(argument);
        } else if (argument.type() == IfcUtil::Argument_AGGREGATE_OF_BINARY) {
            std::vector<boost::dynamic_bitset<>> bits = static_cast<std::vector<boost::dynamic_bitset<>>>(argument);
            list->values.reserve(bits.size());
            for (const auto& item : bits) {
                std::string value;
                boost::to_string(item, value);
                list->values.push_back(value);
            }
        } else {
            set_global_error("Argument is not a string aggregate");
            return nullptr;
        }
        list->cursor = 0;
        clear_string_list_error(list.get());
        return list.release();
    } catch (const std::exception& e) {
        set_global_error(e.what());
        return nullptr;
    } catch (...) {
        set_global_error("Unknown native exception while reading string aggregate argument");
        return nullptr;
    }
}

ifcopenshell_ifcparse_entity_list_t* ifcopenshell_ifcparse_entity_get_argument_as_entity_list(
    const ifcopenshell_ifcparse_entity_ref_t* entity,
    size_t index
) {
    const IfcUtil::IfcBaseClass* inst = from_entity_ref(entity);
    if (inst == nullptr) {
        set_global_error("Entity handle is null");
        return nullptr;
    }
    try {
        const auto argument = inst->get_attribute_value(index);
        if (argument.type() != IfcUtil::Argument_AGGREGATE_OF_ENTITY_INSTANCE) {
            set_global_error("Argument is not an entity aggregate");
            return nullptr;
        }
        aggregate_of_instance::ptr values = static_cast<aggregate_of_instance::ptr>(argument);
        clear_global_error();
        return make_entity_list(std::shared_ptr<IfcParse::IfcFile>(), values);
    } catch (const std::exception& e) {
        set_global_error(e.what());
        return nullptr;
    } catch (...) {
        set_global_error("Unknown native exception while reading entity aggregate argument");
        return nullptr;
    }
}

ifcopenshell_ifcparse_entity_list_t* ifcopenshell_ifcparse_entity_get_inverse(
    const ifcopenshell_ifcparse_entity_ref_t* entity,
    const char* inverse_name
) {
    const IfcUtil::IfcBaseClass* inst = from_entity_ref(entity);
    if (inst == nullptr) {
        set_global_error("Entity handle is null");
        return nullptr;
    }
    if (inverse_name == nullptr || inverse_name[0] == '\0') {
        set_global_error("Inverse attribute name is empty");
        return nullptr;
    }

    try {
        const auto* base_entity = inst->as<IfcUtil::IfcBaseEntity>();
        if (base_entity == nullptr) {
            set_global_error("Only entity instances support inverse attribute lookups");
            return nullptr;
        }
        aggregate_of_instance::ptr values = base_entity->get_inverse(std::string(inverse_name));
        clear_global_error();
        return make_entity_list(std::shared_ptr<IfcParse::IfcFile>(), values);
    } catch (const std::exception& e) {
        set_global_error(e.what());
        return nullptr;
    } catch (...) {
        set_global_error("Unknown native exception while reading inverse attribute values");
        return nullptr;
    }
}

ifcopenshell_ifcparse_int_matrix_t* ifcopenshell_ifcparse_entity_get_argument_as_int_matrix(
    const ifcopenshell_ifcparse_entity_ref_t* entity,
    size_t index
) {
    const IfcUtil::IfcBaseClass* inst = from_entity_ref(entity);
    if (inst == nullptr) {
        set_global_error("Entity handle is null");
        return nullptr;
    }
    try {
        const auto argument = inst->get_attribute_value(index);
        if (argument.type() != IfcUtil::Argument_AGGREGATE_OF_AGGREGATE_OF_INT) {
            set_global_error("Argument is not an integer matrix");
            return nullptr;
        }
        std::unique_ptr<ifcopenshell_ifcparse_int_matrix_t> matrix(new ifcopenshell_ifcparse_int_matrix_t());
        matrix->values = static_cast<std::vector<std::vector<int>>>(argument);
        clear_int_matrix_error(matrix.get());
        return matrix.release();
    } catch (const std::exception& e) {
        set_global_error(e.what());
        return nullptr;
    } catch (...) {
        set_global_error("Unknown native exception while reading integer matrix argument");
        return nullptr;
    }
}

ifcopenshell_ifcparse_double_matrix_t* ifcopenshell_ifcparse_entity_get_argument_as_double_matrix(
    const ifcopenshell_ifcparse_entity_ref_t* entity,
    size_t index
) {
    const IfcUtil::IfcBaseClass* inst = from_entity_ref(entity);
    if (inst == nullptr) {
        set_global_error("Entity handle is null");
        return nullptr;
    }
    try {
        const auto argument = inst->get_attribute_value(index);
        if (argument.type() != IfcUtil::Argument_AGGREGATE_OF_AGGREGATE_OF_DOUBLE) {
            set_global_error("Argument is not a double matrix");
            return nullptr;
        }
        std::unique_ptr<ifcopenshell_ifcparse_double_matrix_t> matrix(new ifcopenshell_ifcparse_double_matrix_t());
        matrix->values = static_cast<std::vector<std::vector<double>>>(argument);
        clear_double_matrix_error(matrix.get());
        return matrix.release();
    } catch (const std::exception& e) {
        set_global_error(e.what());
        return nullptr;
    } catch (...) {
        set_global_error("Unknown native exception while reading double matrix argument");
        return nullptr;
    }
}

ifcopenshell_ifcparse_entity_matrix_t* ifcopenshell_ifcparse_entity_get_argument_as_entity_matrix(
    const ifcopenshell_ifcparse_entity_ref_t* entity,
    size_t index
) {
    const IfcUtil::IfcBaseClass* inst = from_entity_ref(entity);
    if (inst == nullptr) {
        set_global_error("Entity handle is null");
        return nullptr;
    }
    try {
        const auto argument = inst->get_attribute_value(index);
        if (argument.type() != IfcUtil::Argument_AGGREGATE_OF_AGGREGATE_OF_ENTITY_INSTANCE) {
            set_global_error("Argument is not an entity matrix");
            return nullptr;
        }
        aggregate_of_aggregate_of_instance::ptr values = static_cast<aggregate_of_aggregate_of_instance::ptr>(argument);
        std::unique_ptr<ifcopenshell_ifcparse_entity_matrix_t> matrix(new ifcopenshell_ifcparse_entity_matrix_t());
        if (values != nullptr) {
            matrix->values.reserve(values->size());
            for (auto outer_it = values->begin(); outer_it != values->end(); ++outer_it) {
                matrix->values.push_back(*outer_it);
            }
        }
        clear_entity_matrix_error(matrix.get());
        return matrix.release();
    } catch (const std::exception& e) {
        set_global_error(e.what());
        return nullptr;
    } catch (...) {
        set_global_error("Unknown native exception while reading entity matrix argument");
        return nullptr;
    }
}

int ifcopenshell_ifcparse_entity_set_argument_null(
    const ifcopenshell_ifcparse_entity_ref_t* entity,
    size_t index
) {
    auto* inst = from_entity_ref_mut(entity);
    if (inst == nullptr) {
        set_global_error("Entity handle is null");
        return 0;
    }
    if (inst->declaration().as_entity() == nullptr) {
        set_global_error("Null assignment is only supported for entity declarations");
        return 0;
    }

    try {
        if (!inst->declaration().as_entity()->attribute_by_index(index)->optional()) {
            set_global_error("Attribute not set");
            return 0;
        }
        inst->set_attribute_value(index, Blank{});
        clear_global_error();
        return 1;
    } catch (const std::exception& e) {
        set_global_error(e.what());
        return 0;
    } catch (...) {
        set_global_error("Unknown native exception while setting null argument");
        return 0;
    }
}

int ifcopenshell_ifcparse_entity_unset_argument(
    const ifcopenshell_ifcparse_entity_ref_t* entity,
    size_t index
) {
    auto* inst = from_entity_ref_mut(entity);
    if (inst == nullptr) {
        set_global_error("Entity handle is null");
        return 0;
    }
    try {
        inst->unset_attribute_value(index);
        clear_global_error();
        return 1;
    } catch (const std::exception& e) {
        set_global_error(e.what());
        return 0;
    } catch (...) {
        set_global_error("Unknown native exception while unsetting argument");
        return 0;
    }
}

int ifcopenshell_ifcparse_entity_set_argument_int(
    const ifcopenshell_ifcparse_entity_ref_t* entity,
    size_t index,
    int value
) {
    auto* inst = from_entity_ref_mut(entity);
    if (inst == nullptr) {
        set_global_error("Entity handle is null");
        return 0;
    }
    try {
        const auto expected = declared_argument_type(inst, index);
        if (expected == IfcUtil::Argument_INT) {
            inst->set_attribute_value(index, value);
            clear_global_error();
            return 1;
        }
        if (expected == IfcUtil::Argument_BOOL && (value == 0 || value == 1)) {
            inst->set_attribute_value(index, value != 0);
            clear_global_error();
            return 1;
        }
        set_global_error("Attribute not set");
        return 0;
    } catch (const std::exception& e) {
        set_global_error(e.what());
        return 0;
    } catch (...) {
        set_global_error("Unknown native exception while setting integer argument");
        return 0;
    }
}

int ifcopenshell_ifcparse_entity_set_argument_bool(
    const ifcopenshell_ifcparse_entity_ref_t* entity,
    size_t index,
    int value
) {
    auto* inst = from_entity_ref_mut(entity);
    if (inst == nullptr) {
        set_global_error("Entity handle is null");
        return 0;
    }
    try {
        if (declared_argument_type(inst, index) != IfcUtil::Argument_BOOL) {
            set_global_error("Attribute not set");
            return 0;
        }
        inst->set_attribute_value(index, value != 0);
        clear_global_error();
        return 1;
    } catch (const std::exception& e) {
        set_global_error(e.what());
        return 0;
    } catch (...) {
        set_global_error("Unknown native exception while setting boolean argument");
        return 0;
    }
}

int ifcopenshell_ifcparse_entity_set_argument_logical(
    const ifcopenshell_ifcparse_entity_ref_t* entity,
    size_t index,
    int value
) {
    auto* inst = from_entity_ref_mut(entity);
    if (inst == nullptr) {
        set_global_error("Entity handle is null");
        return 0;
    }
    try {
        if (declared_argument_type(inst, index) != IfcUtil::Argument_LOGICAL) {
            set_global_error("Attribute not set");
            return 0;
        }
        if (value != -1 && value != 0 && value != 1) {
            set_global_error("Logical value must be -1, 0, or 1");
            return 0;
        }
        boost::logic::tribool logical_value = value == -1 ? boost::logic::tribool(boost::logic::indeterminate) : boost::logic::tribool(value != 0);
        inst->set_attribute_value(index, logical_value);
        clear_global_error();
        return 1;
    } catch (const std::exception& e) {
        set_global_error(e.what());
        return 0;
    } catch (...) {
        set_global_error("Unknown native exception while setting logical argument");
        return 0;
    }
}

int ifcopenshell_ifcparse_entity_set_argument_double(
    const ifcopenshell_ifcparse_entity_ref_t* entity,
    size_t index,
    double value
) {
    auto* inst = from_entity_ref_mut(entity);
    if (inst == nullptr) {
        set_global_error("Entity handle is null");
        return 0;
    }
    try {
        if (declared_argument_type(inst, index) != IfcUtil::Argument_DOUBLE) {
            set_global_error("Attribute not set");
            return 0;
        }
        inst->set_attribute_value(index, value);
        clear_global_error();
        return 1;
    } catch (const std::exception& e) {
        set_global_error(e.what());
        return 0;
    } catch (...) {
        set_global_error("Unknown native exception while setting double argument");
        return 0;
    }
}

int ifcopenshell_ifcparse_entity_set_argument_string(
    const ifcopenshell_ifcparse_entity_ref_t* entity,
    size_t index,
    const char* value
) {
    auto* inst = from_entity_ref_mut(entity);
    if (inst == nullptr) {
        set_global_error("Entity handle is null");
        return 0;
    }
    if (value == nullptr) {
        set_global_error("String value is null");
        return 0;
    }
    try {
        const auto expected = declared_argument_type(inst, index);
        if (expected == IfcUtil::Argument_STRING) {
            inst->set_attribute_value(index, std::string(value));
            clear_global_error();
            return 1;
        }
        if (expected == IfcUtil::Argument_ENUMERATION) {
            const auto* enum_type = resolve_enumeration_type(inst, index);
            if (enum_type == nullptr) {
                set_global_error("Unable to resolve enumeration type");
                return 0;
            }
            inst->set_attribute_value(index, EnumerationReference(enum_type, enum_type->lookup_enum_offset(std::string(value))));
            clear_global_error();
            return 1;
        }
        if (expected == IfcUtil::Argument_BINARY) {
            std::string as_string(value);
            if (!IfcUtil::valid_binary_string(as_string)) {
                set_global_error("String not a valid binary representation");
                return 0;
            }
            boost::dynamic_bitset<> bits(as_string);
            inst->set_attribute_value(index, bits);
            clear_global_error();
            return 1;
        }
        set_global_error("Attribute not set");
        return 0;
    } catch (const std::exception& e) {
        set_global_error(e.what());
        return 0;
    } catch (...) {
        set_global_error("Unknown native exception while setting string argument");
        return 0;
    }
}

int ifcopenshell_ifcparse_entity_set_argument_entity(
    const ifcopenshell_ifcparse_entity_ref_t* entity,
    size_t index,
    const ifcopenshell_ifcparse_entity_ref_t* value
) {
    auto* inst = from_entity_ref_mut(entity);
    if (inst == nullptr) {
        set_global_error("Entity handle is null");
        return 0;
    }
    try {
        if (declared_argument_type(inst, index) != IfcUtil::Argument_ENTITY_INSTANCE) {
            set_global_error("Attribute not set");
            return 0;
        }
        inst->set_attribute_value(index, from_entity_ref_mut(value));
        clear_global_error();
        return 1;
    } catch (const std::exception& e) {
        set_global_error(e.what());
        return 0;
    } catch (...) {
        set_global_error("Unknown native exception while setting entity argument");
        return 0;
    }
}

int ifcopenshell_ifcparse_entity_set_argument_int_list(
    const ifcopenshell_ifcparse_entity_ref_t* entity,
    size_t index,
    const int* values,
    size_t value_count
) {
    auto* inst = from_entity_ref_mut(entity);
    if (inst == nullptr || (values == nullptr && value_count > 0)) {
        set_global_error("Invalid integer aggregate input");
        return 0;
    }
    try {
        if (declared_argument_type(inst, index) != IfcUtil::Argument_AGGREGATE_OF_INT) {
            set_global_error("Attribute not set");
            return 0;
        }
        std::vector<int> vector_values(values, values + value_count);
        inst->set_attribute_value(index, vector_values);
        clear_global_error();
        return 1;
    } catch (const std::exception& e) {
        set_global_error(e.what());
        return 0;
    } catch (...) {
        set_global_error("Unknown native exception while setting integer aggregate argument");
        return 0;
    }
}

int ifcopenshell_ifcparse_entity_set_argument_double_list(
    const ifcopenshell_ifcparse_entity_ref_t* entity,
    size_t index,
    const double* values,
    size_t value_count
) {
    auto* inst = from_entity_ref_mut(entity);
    if (inst == nullptr || (values == nullptr && value_count > 0)) {
        set_global_error("Invalid double aggregate input");
        return 0;
    }
    try {
        if (declared_argument_type(inst, index) != IfcUtil::Argument_AGGREGATE_OF_DOUBLE) {
            set_global_error("Attribute not set");
            return 0;
        }
        std::vector<double> vector_values(values, values + value_count);
        inst->set_attribute_value(index, vector_values);
        clear_global_error();
        return 1;
    } catch (const std::exception& e) {
        set_global_error(e.what());
        return 0;
    } catch (...) {
        set_global_error("Unknown native exception while setting double aggregate argument");
        return 0;
    }
}

int ifcopenshell_ifcparse_entity_set_argument_string_list(
    const ifcopenshell_ifcparse_entity_ref_t* entity,
    size_t index,
    const char* const* values,
    size_t value_count
) {
    auto* inst = from_entity_ref_mut(entity);
    if (inst == nullptr || (values == nullptr && value_count > 0)) {
        set_global_error("Invalid string aggregate input");
        return 0;
    }
    try {
        const auto expected = declared_argument_type(inst, index);
        if (expected == IfcUtil::Argument_AGGREGATE_OF_STRING) {
            std::vector<std::string> vector_values;
            vector_values.reserve(value_count);
            for (size_t i = 0; i < value_count; ++i) {
                if (values[i] == nullptr) {
                    set_global_error("Null item in string aggregate");
                    return 0;
                }
                vector_values.emplace_back(values[i]);
            }
            inst->set_attribute_value(index, vector_values);
            clear_global_error();
            return 1;
        }
        if (expected == IfcUtil::Argument_AGGREGATE_OF_BINARY) {
            std::vector<boost::dynamic_bitset<>> bits;
            bits.reserve(value_count);
            for (size_t i = 0; i < value_count; ++i) {
                if (values[i] == nullptr) {
                    set_global_error("Null item in binary aggregate");
                    return 0;
                }
                std::string binary(values[i]);
                if (!IfcUtil::valid_binary_string(binary)) {
                    set_global_error("String not a valid binary representation");
                    return 0;
                }
                bits.emplace_back(binary);
            }
            inst->set_attribute_value(index, bits);
            clear_global_error();
            return 1;
        }
        set_global_error("Attribute not set");
        return 0;
    } catch (const std::exception& e) {
        set_global_error(e.what());
        return 0;
    } catch (...) {
        set_global_error("Unknown native exception while setting string aggregate argument");
        return 0;
    }
}

int ifcopenshell_ifcparse_entity_set_argument_entity_list(
    const ifcopenshell_ifcparse_entity_ref_t* entity,
    size_t index,
    const ifcopenshell_ifcparse_entity_ref_t* const* values,
    size_t value_count
) {
    auto* inst = from_entity_ref_mut(entity);
    if (inst == nullptr || (values == nullptr && value_count > 0)) {
        set_global_error("Invalid entity aggregate input");
        return 0;
    }
    try {
        if (declared_argument_type(inst, index) != IfcUtil::Argument_AGGREGATE_OF_ENTITY_INSTANCE) {
            set_global_error("Attribute not set");
            return 0;
        }
        aggregate_of_instance::ptr entities(new aggregate_of_instance());
        for (size_t i = 0; i < value_count; ++i) {
            entities->push(from_entity_ref_mut(values[i]));
        }
        inst->set_attribute_value(index, entities);
        clear_global_error();
        return 1;
    } catch (const std::exception& e) {
        set_global_error(e.what());
        return 0;
    } catch (...) {
        set_global_error("Unknown native exception while setting entity aggregate argument");
        return 0;
    }
}

int ifcopenshell_ifcparse_entity_set_argument_int_matrix(
    const ifcopenshell_ifcparse_entity_ref_t* entity,
    size_t index,
    const int* values,
    size_t value_count,
    const size_t* row_offsets,
    size_t row_count
) {
    auto* inst = from_entity_ref_mut(entity);
    if (inst == nullptr) {
        set_global_error("Entity handle is null");
        return 0;
    }
    if ((values == nullptr && value_count > 0) || row_offsets == nullptr) {
        set_global_error("Invalid integer matrix input");
        return 0;
    }

    try {
        if (declared_argument_type(inst, index) != IfcUtil::Argument_AGGREGATE_OF_AGGREGATE_OF_INT) {
            set_global_error("Attribute not set");
            return 0;
        }
        if (row_offsets[0] != 0 || row_offsets[row_count] != value_count) {
            set_global_error("Invalid row offsets for integer matrix");
            return 0;
        }
        std::vector<std::vector<int>> matrix;
        matrix.reserve(row_count);
        for (size_t row = 0; row < row_count; ++row) {
            const size_t begin = row_offsets[row];
            const size_t end = row_offsets[row + 1];
            if (begin > end || end > value_count) {
                set_global_error("Invalid row offsets for integer matrix");
                return 0;
            }
            std::vector<int> current;
            current.reserve(end - begin);
            for (size_t i = begin; i < end; ++i) {
                current.push_back(values[i]);
            }
            matrix.push_back(std::move(current));
        }
        inst->set_attribute_value(index, matrix);
        clear_global_error();
        return 1;
    } catch (const std::exception& e) {
        set_global_error(e.what());
        return 0;
    } catch (...) {
        set_global_error("Unknown native exception while setting integer matrix argument");
        return 0;
    }
}

int ifcopenshell_ifcparse_entity_set_argument_double_matrix(
    const ifcopenshell_ifcparse_entity_ref_t* entity,
    size_t index,
    const double* values,
    size_t value_count,
    const size_t* row_offsets,
    size_t row_count
) {
    auto* inst = from_entity_ref_mut(entity);
    if (inst == nullptr) {
        set_global_error("Entity handle is null");
        return 0;
    }
    if ((values == nullptr && value_count > 0) || row_offsets == nullptr) {
        set_global_error("Invalid double matrix input");
        return 0;
    }

    try {
        if (declared_argument_type(inst, index) != IfcUtil::Argument_AGGREGATE_OF_AGGREGATE_OF_DOUBLE) {
            set_global_error("Attribute not set");
            return 0;
        }
        if (row_offsets[0] != 0 || row_offsets[row_count] != value_count) {
            set_global_error("Invalid row offsets for double matrix");
            return 0;
        }
        std::vector<std::vector<double>> matrix;
        matrix.reserve(row_count);
        for (size_t row = 0; row < row_count; ++row) {
            const size_t begin = row_offsets[row];
            const size_t end = row_offsets[row + 1];
            if (begin > end || end > value_count) {
                set_global_error("Invalid row offsets for double matrix");
                return 0;
            }
            std::vector<double> current;
            current.reserve(end - begin);
            for (size_t i = begin; i < end; ++i) {
                current.push_back(values[i]);
            }
            matrix.push_back(std::move(current));
        }
        inst->set_attribute_value(index, matrix);
        clear_global_error();
        return 1;
    } catch (const std::exception& e) {
        set_global_error(e.what());
        return 0;
    } catch (...) {
        set_global_error("Unknown native exception while setting double matrix argument");
        return 0;
    }
}

int ifcopenshell_ifcparse_entity_set_argument_entity_matrix(
    const ifcopenshell_ifcparse_entity_ref_t* entity,
    size_t index,
    const ifcopenshell_ifcparse_entity_ref_t* const* values,
    size_t value_count,
    const size_t* row_offsets,
    size_t row_count
) {
    auto* inst = from_entity_ref_mut(entity);
    if (inst == nullptr) {
        set_global_error("Entity handle is null");
        return 0;
    }
    if ((values == nullptr && value_count > 0) || row_offsets == nullptr) {
        set_global_error("Invalid entity matrix input");
        return 0;
    }

    try {
        if (declared_argument_type(inst, index) != IfcUtil::Argument_AGGREGATE_OF_AGGREGATE_OF_ENTITY_INSTANCE) {
            set_global_error("Attribute not set");
            return 0;
        }
        if (row_offsets[0] != 0 || row_offsets[row_count] != value_count) {
            set_global_error("Invalid row offsets for entity matrix");
            return 0;
        }
        aggregate_of_aggregate_of_instance::ptr matrix(new aggregate_of_aggregate_of_instance());
        for (size_t row = 0; row < row_count; ++row) {
            const size_t begin = row_offsets[row];
            const size_t end = row_offsets[row + 1];
            if (begin > end || end > value_count) {
                set_global_error("Invalid row offsets for entity matrix");
                return 0;
            }
            std::vector<IfcUtil::IfcBaseClass*> current;
            current.reserve(end - begin);
            for (size_t i = begin; i < end; ++i) {
                auto* value = from_entity_ref_mut(values[i]);
                if (value == nullptr) {
                    set_global_error("Entity matrix contains null entry");
                    return 0;
                }
                current.push_back(value);
            }
            matrix->push(current);
        }
        inst->set_attribute_value(index, matrix);
        clear_global_error();
        return 1;
    } catch (const std::exception& e) {
        set_global_error(e.what());
        return 0;
    } catch (...) {
        set_global_error("Unknown native exception while setting entity matrix argument");
        return 0;
    }
}

void ifcopenshell_ifcparse_int_list_close(ifcopenshell_ifcparse_int_list_t* list) {
    delete list;
}

size_t ifcopenshell_ifcparse_int_list_count(const ifcopenshell_ifcparse_int_list_t* list) {
    if (list == nullptr) {
        return 0;
    }
    return list->values.size();
}

void ifcopenshell_ifcparse_int_list_reset(ifcopenshell_ifcparse_int_list_t* list) {
    if (list == nullptr) {
        return;
    }
    list->cursor = 0;
    clear_int_list_error(list);
}

int ifcopenshell_ifcparse_int_list_get(
    const ifcopenshell_ifcparse_int_list_t* list,
    size_t index
) {
    if (list == nullptr) {
        set_global_error("Integer list handle is null");
        return 0;
    }
    if (index >= list->values.size()) {
        set_int_list_error(const_cast<ifcopenshell_ifcparse_int_list_t*>(list), "Integer list index out of range");
        return 0;
    }
    clear_int_list_error(const_cast<ifcopenshell_ifcparse_int_list_t*>(list));
    return list->values[index];
}

int ifcopenshell_ifcparse_int_list_next(ifcopenshell_ifcparse_int_list_t* list) {
    if (list == nullptr) {
        set_global_error("Integer list handle is null");
        return 0;
    }
    if (list->cursor >= list->values.size()) {
        set_int_list_error(list, "End of integer list");
        return 0;
    }
    return ifcopenshell_ifcparse_int_list_get(list, list->cursor++);
}

void ifcopenshell_ifcparse_double_list_close(ifcopenshell_ifcparse_double_list_t* list) {
    delete list;
}

size_t ifcopenshell_ifcparse_double_list_count(const ifcopenshell_ifcparse_double_list_t* list) {
    if (list == nullptr) {
        return 0;
    }
    return list->values.size();
}

void ifcopenshell_ifcparse_double_list_reset(ifcopenshell_ifcparse_double_list_t* list) {
    if (list == nullptr) {
        return;
    }
    list->cursor = 0;
    clear_double_list_error(list);
}

double ifcopenshell_ifcparse_double_list_get(
    const ifcopenshell_ifcparse_double_list_t* list,
    size_t index
) {
    if (list == nullptr) {
        set_global_error("Double list handle is null");
        return 0.0;
    }
    if (index >= list->values.size()) {
        set_double_list_error(const_cast<ifcopenshell_ifcparse_double_list_t*>(list), "Double list index out of range");
        return 0.0;
    }
    clear_double_list_error(const_cast<ifcopenshell_ifcparse_double_list_t*>(list));
    return list->values[index];
}

double ifcopenshell_ifcparse_double_list_next(ifcopenshell_ifcparse_double_list_t* list) {
    if (list == nullptr) {
        set_global_error("Double list handle is null");
        return 0.0;
    }
    if (list->cursor >= list->values.size()) {
        set_double_list_error(list, "End of double list");
        return 0.0;
    }
    return ifcopenshell_ifcparse_double_list_get(list, list->cursor++);
}

void ifcopenshell_ifcparse_int_matrix_close(ifcopenshell_ifcparse_int_matrix_t* matrix) {
    delete matrix;
}

size_t ifcopenshell_ifcparse_int_matrix_row_count(const ifcopenshell_ifcparse_int_matrix_t* matrix) {
    if (matrix == nullptr) {
        return 0;
    }
    return matrix->values.size();
}

size_t ifcopenshell_ifcparse_int_matrix_col_count(
    const ifcopenshell_ifcparse_int_matrix_t* matrix,
    size_t row
) {
    if (matrix == nullptr) {
        set_global_error("Integer matrix handle is null");
        return 0;
    }
    if (row >= matrix->values.size()) {
        set_int_matrix_error(const_cast<ifcopenshell_ifcparse_int_matrix_t*>(matrix), "Integer matrix row out of range");
        return 0;
    }
    clear_int_matrix_error(const_cast<ifcopenshell_ifcparse_int_matrix_t*>(matrix));
    return matrix->values[row].size();
}

int ifcopenshell_ifcparse_int_matrix_get(
    const ifcopenshell_ifcparse_int_matrix_t* matrix,
    size_t row,
    size_t col
) {
    if (matrix == nullptr) {
        set_global_error("Integer matrix handle is null");
        return 0;
    }
    if (row >= matrix->values.size() || col >= matrix->values[row].size()) {
        set_int_matrix_error(const_cast<ifcopenshell_ifcparse_int_matrix_t*>(matrix), "Integer matrix index out of range");
        return 0;
    }
    clear_int_matrix_error(const_cast<ifcopenshell_ifcparse_int_matrix_t*>(matrix));
    return matrix->values[row][col];
}

void ifcopenshell_ifcparse_double_matrix_close(ifcopenshell_ifcparse_double_matrix_t* matrix) {
    delete matrix;
}

size_t ifcopenshell_ifcparse_double_matrix_row_count(const ifcopenshell_ifcparse_double_matrix_t* matrix) {
    if (matrix == nullptr) {
        return 0;
    }
    return matrix->values.size();
}

size_t ifcopenshell_ifcparse_double_matrix_col_count(
    const ifcopenshell_ifcparse_double_matrix_t* matrix,
    size_t row
) {
    if (matrix == nullptr) {
        set_global_error("Double matrix handle is null");
        return 0;
    }
    if (row >= matrix->values.size()) {
        set_double_matrix_error(const_cast<ifcopenshell_ifcparse_double_matrix_t*>(matrix), "Double matrix row out of range");
        return 0;
    }
    clear_double_matrix_error(const_cast<ifcopenshell_ifcparse_double_matrix_t*>(matrix));
    return matrix->values[row].size();
}

double ifcopenshell_ifcparse_double_matrix_get(
    const ifcopenshell_ifcparse_double_matrix_t* matrix,
    size_t row,
    size_t col
) {
    if (matrix == nullptr) {
        set_global_error("Double matrix handle is null");
        return 0.0;
    }
    if (row >= matrix->values.size() || col >= matrix->values[row].size()) {
        set_double_matrix_error(const_cast<ifcopenshell_ifcparse_double_matrix_t*>(matrix), "Double matrix index out of range");
        return 0.0;
    }
    clear_double_matrix_error(const_cast<ifcopenshell_ifcparse_double_matrix_t*>(matrix));
    return matrix->values[row][col];
}

void ifcopenshell_ifcparse_entity_matrix_close(ifcopenshell_ifcparse_entity_matrix_t* matrix) {
    delete matrix;
}

size_t ifcopenshell_ifcparse_entity_matrix_row_count(const ifcopenshell_ifcparse_entity_matrix_t* matrix) {
    if (matrix == nullptr) {
        return 0;
    }
    return matrix->values.size();
}

size_t ifcopenshell_ifcparse_entity_matrix_col_count(
    const ifcopenshell_ifcparse_entity_matrix_t* matrix,
    size_t row
) {
    if (matrix == nullptr) {
        set_global_error("Entity matrix handle is null");
        return 0;
    }
    if (row >= matrix->values.size()) {
        set_entity_matrix_error(const_cast<ifcopenshell_ifcparse_entity_matrix_t*>(matrix), "Entity matrix row out of range");
        return 0;
    }
    clear_entity_matrix_error(const_cast<ifcopenshell_ifcparse_entity_matrix_t*>(matrix));
    return matrix->values[row].size();
}

const ifcopenshell_ifcparse_entity_ref_t* ifcopenshell_ifcparse_entity_matrix_get(
    const ifcopenshell_ifcparse_entity_matrix_t* matrix,
    size_t row,
    size_t col
) {
    if (matrix == nullptr) {
        set_global_error("Entity matrix handle is null");
        return nullptr;
    }
    if (row >= matrix->values.size() || col >= matrix->values[row].size()) {
        set_entity_matrix_error(const_cast<ifcopenshell_ifcparse_entity_matrix_t*>(matrix), "Entity matrix index out of range");
        return nullptr;
    }
    clear_entity_matrix_error(const_cast<ifcopenshell_ifcparse_entity_matrix_t*>(matrix));
    return to_entity_ref(matrix->values[row][col]);
}

ifcopenshell_ifcparse_type_list_t* ifcopenshell_ifcparse_file_types(ifcopenshell_ifcparse_file_t* file) {
    if (file == nullptr || !file->file) {
        set_global_error("IfcParse handle is null");
        return nullptr;
    }

    try {
        std::unique_ptr<ifcopenshell_ifcparse_type_list_t> list(new ifcopenshell_ifcparse_type_list_t());
        list->owner_file = file->file;
        list->cursor = 0;
        for (auto it = file->file->types_begin(); it != file->file->types_end(); ++it) {
            list->types.push_back(*it);
        }
        clear_file_error(file);
        clear_type_list_error(list.get());
        return list.release();
    } catch (const std::exception& e) {
        set_file_error(file, e.what());
        return nullptr;
    } catch (...) {
        set_file_error(file, "Unknown native exception while querying type list");
        return nullptr;
    }
}

void ifcopenshell_ifcparse_type_list_close(ifcopenshell_ifcparse_type_list_t* list) {
    delete list;
}

size_t ifcopenshell_ifcparse_type_list_count(const ifcopenshell_ifcparse_type_list_t* list) {
    if (list == nullptr) {
        return 0;
    }
    return list->types.size();
}

void ifcopenshell_ifcparse_type_list_reset(ifcopenshell_ifcparse_type_list_t* list) {
    if (list == nullptr) {
        return;
    }
    list->cursor = 0;
    clear_type_list_error(list);
}

const ifcopenshell_ifcparse_type_ref_t* ifcopenshell_ifcparse_type_list_get(
    const ifcopenshell_ifcparse_type_list_t* list,
    size_t index
) {
    if (list == nullptr) {
        set_global_error("Type list handle is null");
        return nullptr;
    }
    if (index >= list->types.size()) {
        set_type_list_error(const_cast<ifcopenshell_ifcparse_type_list_t*>(list), "Type list index out of range");
        return nullptr;
    }
    clear_type_list_error(const_cast<ifcopenshell_ifcparse_type_list_t*>(list));
    return to_type_ref(list->types[index]);
}

const ifcopenshell_ifcparse_type_ref_t* ifcopenshell_ifcparse_type_list_next(
    ifcopenshell_ifcparse_type_list_t* list
) {
    if (list == nullptr) {
        set_global_error("Type list handle is null");
        return nullptr;
    }
    if (list->cursor >= list->types.size()) {
        clear_type_list_error(list);
        return nullptr;
    }
    return ifcopenshell_ifcparse_type_list_get(list, list->cursor++);
}

const char* ifcopenshell_ifcparse_type_name(const ifcopenshell_ifcparse_type_ref_t* type_ref) {
    const IfcParse::declaration* decl = from_type_ref(type_ref);
    if (decl == nullptr) {
        set_global_error("Type reference is null");
        return nullptr;
    }
    clear_global_error();
    return decl->name().c_str();
}

int ifcopenshell_ifcparse_type_index(const ifcopenshell_ifcparse_type_ref_t* type_ref) {
    const IfcParse::declaration* decl = from_type_ref(type_ref);
    if (decl == nullptr) {
        set_global_error("Type reference is null");
        return -1;
    }
    clear_global_error();
    return decl->index_in_schema();
}

ifcopenshell_ifcparse_string_list_t* ifcopenshell_ifcparse_schema_names(void) {
    try {
        std::unique_ptr<ifcopenshell_ifcparse_string_list_t> list(new ifcopenshell_ifcparse_string_list_t());
        list->values = IfcParse::schema_names();
        list->cursor = 0;
        clear_string_list_error(list.get());
        return list.release();
    } catch (const std::exception& e) {
        set_global_error(e.what());
        return nullptr;
    } catch (...) {
        set_global_error("Unknown native exception while listing schema names");
        return nullptr;
    }
}

void ifcopenshell_ifcparse_string_list_close(ifcopenshell_ifcparse_string_list_t* list) {
    delete list;
}

size_t ifcopenshell_ifcparse_string_list_count(const ifcopenshell_ifcparse_string_list_t* list) {
    if (list == nullptr) {
        return 0;
    }
    return list->values.size();
}

void ifcopenshell_ifcparse_string_list_reset(ifcopenshell_ifcparse_string_list_t* list) {
    if (list == nullptr) {
        return;
    }
    list->cursor = 0;
    clear_string_list_error(list);
}

const char* ifcopenshell_ifcparse_string_list_get(
    const ifcopenshell_ifcparse_string_list_t* list,
    size_t index
) {
    if (list == nullptr) {
        set_global_error("String list handle is null");
        return nullptr;
    }
    if (index >= list->values.size()) {
        set_string_list_error(const_cast<ifcopenshell_ifcparse_string_list_t*>(list), "String list index out of range");
        return nullptr;
    }
    clear_string_list_error(const_cast<ifcopenshell_ifcparse_string_list_t*>(list));
    return list->values[index].c_str();
}

const char* ifcopenshell_ifcparse_string_list_next(ifcopenshell_ifcparse_string_list_t* list) {
    if (list == nullptr) {
        set_global_error("String list handle is null");
        return nullptr;
    }
    if (list->cursor >= list->values.size()) {
        clear_string_list_error(list);
        return nullptr;
    }
    return ifcopenshell_ifcparse_string_list_get(list, list->cursor++);
}

const char* ifcopenshell_ifcparse_file_last_error(const ifcopenshell_ifcparse_file_t* file) {
    if (file == nullptr) {
        return g_last_error.c_str();
    }
    return file->last_error.c_str();
}

const char* ifcopenshell_ifcparse_last_error(void) {
    return g_last_error.c_str();
}
