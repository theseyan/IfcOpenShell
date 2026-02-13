#include "ifcgeom/c_api.h"

#include "ifcgeom/ConversionSettings.h"
#include "ifcgeom/IfcGeomElement.h"
#include "ifcgeom/IfcGeomRepresentation.h"
#include "ifcgeom/Iterator.h"
#include "ifcgeom/hybrid_kernel.h"
#include "ifcparse/IfcFile.h"

#include <boost/variant/get.hpp>

#include <array>
#include <memory>
#include <set>
#include <string>
#include <vector>

struct ifcopenshell_ifcgeom_settings {
    ifcopenshell::geometry::Settings value;
    std::string last_error;
};

struct ifcopenshell_ifcgeom_iterator {
    std::unique_ptr<IfcGeom::Iterator> value;
    std::string last_error;
    std::string text_cache;
};

struct ifcopenshell_ifcgeom_string_list {
    std::vector<std::string> values;
    size_t cursor = 0;
    std::string last_error;
};

struct ifcopenshell_ifcgeom_mesh {
    int id = 0;
    int parent_id = -1;
    std::string name;
    std::string type;
    std::string guid;
    std::string context;
    std::string unique_id;

    std::vector<double> verts;
    std::vector<int> faces;
    std::vector<int> edges;
    std::vector<double> normals;
    std::vector<double> uvs;
    std::vector<int> material_ids;
    std::vector<int> item_ids;
    std::vector<int> edges_item_ids;
    std::vector<double> colors;
    std::vector<double> transform;
};

namespace {
thread_local std::string g_last_error;
thread_local std::string g_string_cache;

void set_global_error(const std::string& message) {
    g_last_error = message;
}

void clear_global_error() {
    g_last_error.clear();
}

void set_settings_error(ifcopenshell_ifcgeom_settings_t* settings, const std::string& message) {
    if (settings != nullptr) {
        settings->last_error = message;
    }
    set_global_error(message);
}

void clear_settings_error(ifcopenshell_ifcgeom_settings_t* settings) {
    if (settings != nullptr) {
        settings->last_error.clear();
    }
    clear_global_error();
}

void set_iterator_error(ifcopenshell_ifcgeom_iterator_t* iterator, const std::string& message) {
    if (iterator != nullptr) {
        iterator->last_error = message;
    }
    set_global_error(message);
}

void clear_iterator_error(ifcopenshell_ifcgeom_iterator_t* iterator) {
    if (iterator != nullptr) {
        iterator->last_error.clear();
    }
    clear_global_error();
}

void set_list_error(ifcopenshell_ifcgeom_string_list_t* list, const std::string& message) {
    if (list != nullptr) {
        list->last_error = message;
    }
    set_global_error(message);
}

void clear_list_error(ifcopenshell_ifcgeom_string_list_t* list) {
    if (list != nullptr) {
        list->last_error.clear();
    }
    clear_global_error();
}

bool is_valid_name(const char* name) {
    return name != nullptr && name[0] != '\0';
}

IfcParse::IfcFile* native_file_from_handle(const ifcopenshell_ifcparse_file_t* file) {
    const void* native = ifcopenshell_ifcparse_file_native_const(file);
    if (native == nullptr) {
        return nullptr;
    }
    return const_cast<IfcParse::IfcFile*>(static_cast<const IfcParse::IfcFile*>(native));
}

std::string parse_error_or_default(const std::string& fallback) {
    const char* parse_error = ifcopenshell_ifcparse_last_error();
    if (parse_error != nullptr && parse_error[0] != '\0') {
        return parse_error;
    }
    return fallback;
}

ifcopenshell::geometry::Settings settings_value_or_default(const ifcopenshell_ifcgeom_settings_t* settings) {
    if (settings == nullptr) {
        return ifcopenshell::geometry::Settings();
    }
    return settings->value;
}

const std::string geometry_library_or_default(const char* geometry_library) {
    if (geometry_library == nullptr || geometry_library[0] == '\0') {
        return "opencascade";
    }
    return geometry_library;
}

int normalize_num_threads(int num_threads) {
    return num_threads > 0 ? num_threads : 1;
}

const IfcGeom::Element* current_element(ifcopenshell_ifcgeom_iterator_t* iterator) {
    if (iterator == nullptr || !iterator->value) {
        set_iterator_error(iterator, "IfcGeom iterator handle is null");
        return nullptr;
    }

    try {
        const IfcGeom::Element* element = iterator->value->get();
        clear_iterator_error(iterator);
        return element;
    } catch (const std::exception& e) {
        set_iterator_error(iterator, e.what());
        return nullptr;
    } catch (...) {
        set_iterator_error(iterator, "Unknown error retrieving current IfcGeom element");
        return nullptr;
    }
}

void copy_point_to_array(const ifcopenshell::geometry::taxonomy::point3& point, double out_xyz[3]) {
    const auto& v = point.ccomponents();
    out_xyz[0] = v(0);
    out_xyz[1] = v(1);
    out_xyz[2] = v(2);
}

ifcopenshell_ifcgeom_mesh_t* make_mesh_snapshot(
    ifcopenshell_ifcgeom_iterator_t* iterator,
    const IfcGeom::Element* element
) {
    const auto* triangulated = dynamic_cast<const IfcGeom::TriangulationElement*>(element);
    if (triangulated == nullptr) {
        set_iterator_error(iterator, "Current IfcGeom element is not triangulated");
        return nullptr;
    }

    std::unique_ptr<ifcopenshell_ifcgeom_mesh_t> mesh(new ifcopenshell_ifcgeom_mesh_t());

    mesh->id = triangulated->id();
    mesh->parent_id = triangulated->parent_id();
    mesh->name = triangulated->name();
    mesh->type = triangulated->type();
    mesh->guid = triangulated->guid();
    mesh->context = triangulated->context();
    mesh->unique_id = triangulated->unique_id();

    const auto* matrix_data = triangulated->transformation().data()->ccomponents().data();
    mesh->transform.assign(matrix_data, matrix_data + 16);

    const auto& geometry = triangulated->geometry();
    mesh->verts.assign(geometry.verts().begin(), geometry.verts().end());
    mesh->faces.assign(geometry.faces().begin(), geometry.faces().end());
    mesh->edges.assign(geometry.edges().begin(), geometry.edges().end());
    mesh->normals.assign(geometry.normals().begin(), geometry.normals().end());
    mesh->uvs.assign(geometry.uvs().begin(), geometry.uvs().end());
    mesh->material_ids.assign(geometry.material_ids().begin(), geometry.material_ids().end());
    mesh->item_ids.assign(geometry.item_ids().begin(), geometry.item_ids().end());
    mesh->edges_item_ids.assign(geometry.edges_item_ids().begin(), geometry.edges_item_ids().end());

    mesh->colors.reserve(geometry.materials().size() * 4);
    for (const auto& style_ptr : geometry.materials()) {
        const auto& color = style_ptr->get_color();
        mesh->colors.push_back(color.ccomponents()[0]);
        mesh->colors.push_back(color.ccomponents()[1]);
        mesh->colors.push_back(color.ccomponents()[2]);
        if (style_ptr->has_transparency()) {
            mesh->colors.push_back(1.0 - style_ptr->transparency);
        } else {
            mesh->colors.push_back(1.0);
        }
    }

    clear_iterator_error(iterator);
    return mesh.release();
}

ifcopenshell_ifcgeom_iterator_t* create_iterator_without_filters(
    const ifcopenshell_ifcparse_file_t* file,
    const ifcopenshell_ifcgeom_settings_t* settings,
    const char* geometry_library,
    int num_threads
) {
    IfcParse::IfcFile* ifc_file = native_file_from_handle(file);
    if (ifc_file == nullptr) {
        set_global_error(parse_error_or_default("Unable to access native IfcFile from C ABI handle"));
        return nullptr;
    }

    std::unique_ptr<ifcopenshell_ifcgeom_iterator_t> handle(new ifcopenshell_ifcgeom_iterator_t());

    try {
        auto settings_value = settings_value_or_default(settings);
        auto kernel = ifcopenshell::geometry::kernels::construct(
            ifc_file,
            geometry_library_or_default(geometry_library),
            settings_value
        );

        handle->value.reset(new IfcGeom::Iterator(
            std::move(kernel),
            settings_value,
            ifc_file,
            normalize_num_threads(num_threads)
        ));

        clear_iterator_error(handle.get());
        return handle.release();
    } catch (const std::exception& e) {
        set_iterator_error(handle.get(), e.what());
        return nullptr;
    } catch (...) {
        set_iterator_error(handle.get(), "Unknown error constructing IfcGeom iterator");
        return nullptr;
    }
}

template <typename FilterBuilder>
ifcopenshell_ifcgeom_iterator_t* create_iterator_with_filters(
    const ifcopenshell_ifcparse_file_t* file,
    const ifcopenshell_ifcgeom_settings_t* settings,
    const char* geometry_library,
    int num_threads,
    FilterBuilder&& filter_builder
) {
    IfcParse::IfcFile* ifc_file = native_file_from_handle(file);
    if (ifc_file == nullptr) {
        set_global_error(parse_error_or_default("Unable to access native IfcFile from C ABI handle"));
        return nullptr;
    }

    std::unique_ptr<ifcopenshell_ifcgeom_iterator_t> handle(new ifcopenshell_ifcgeom_iterator_t());

    try {
        auto settings_value = settings_value_or_default(settings);
        std::vector<IfcGeom::filter_t> filters;
        filter_builder(filters);

        auto kernel = ifcopenshell::geometry::kernels::construct(
            ifc_file,
            geometry_library_or_default(geometry_library),
            settings_value
        );

        handle->value.reset(new IfcGeom::Iterator(
            std::move(kernel),
            settings_value,
            ifc_file,
            filters,
            normalize_num_threads(num_threads)
        ));

        clear_iterator_error(handle.get());
        return handle.release();
    } catch (const std::exception& e) {
        set_iterator_error(handle.get(), e.what());
        return nullptr;
    } catch (...) {
        set_iterator_error(handle.get(), "Unknown error constructing filtered IfcGeom iterator");
        return nullptr;
    }
}

template <typename T>
const T* vector_data_or_null(const std::vector<T>& values) {
    return values.empty() ? nullptr : values.data();
}

} // namespace

extern "C" {

ifcopenshell_ifcgeom_settings_t* ifcopenshell_ifcgeom_settings_create(void) {
    std::unique_ptr<ifcopenshell_ifcgeom_settings_t> settings(new ifcopenshell_ifcgeom_settings_t());
    clear_settings_error(settings.get());
    return settings.release();
}

void ifcopenshell_ifcgeom_settings_destroy(ifcopenshell_ifcgeom_settings_t* settings) {
    delete settings;
}

int ifcopenshell_ifcgeom_settings_set_bool(
    ifcopenshell_ifcgeom_settings_t* settings,
    const char* name,
    int value
) {
    if (settings == nullptr || !is_valid_name(name)) {
        set_settings_error(settings, "Invalid settings handle or setting name");
        return 0;
    }

    try {
        settings->value.set(name, value != 0);
        clear_settings_error(settings);
        return 1;
    } catch (const std::exception& e) {
        set_settings_error(settings, e.what());
        return 0;
    }
}

int ifcopenshell_ifcgeom_settings_set_int(
    ifcopenshell_ifcgeom_settings_t* settings,
    const char* name,
    int value
) {
    if (settings == nullptr || !is_valid_name(name)) {
        set_settings_error(settings, "Invalid settings handle or setting name");
        return 0;
    }

    try {
        settings->value.set(name, value);
        clear_settings_error(settings);
        return 1;
    } catch (const std::exception& e) {
        set_settings_error(settings, e.what());
        return 0;
    }
}

int ifcopenshell_ifcgeom_settings_set_double(
    ifcopenshell_ifcgeom_settings_t* settings,
    const char* name,
    double value
) {
    if (settings == nullptr || !is_valid_name(name)) {
        set_settings_error(settings, "Invalid settings handle or setting name");
        return 0;
    }

    try {
        settings->value.set(name, value);
        clear_settings_error(settings);
        return 1;
    } catch (const std::exception& e) {
        set_settings_error(settings, e.what());
        return 0;
    }
}

int ifcopenshell_ifcgeom_settings_set_string(
    ifcopenshell_ifcgeom_settings_t* settings,
    const char* name,
    const char* value
) {
    if (settings == nullptr || !is_valid_name(name) || value == nullptr) {
        set_settings_error(settings, "Invalid settings handle, setting name, or value");
        return 0;
    }

    try {
        settings->value.set(name, std::string(value));
        clear_settings_error(settings);
        return 1;
    } catch (const std::exception& e) {
        set_settings_error(settings, e.what());
        return 0;
    }
}

int ifcopenshell_ifcgeom_settings_set_int_set(
    ifcopenshell_ifcgeom_settings_t* settings,
    const char* name,
    const int* values,
    size_t value_count
) {
    if (settings == nullptr || !is_valid_name(name) || (value_count > 0 && values == nullptr)) {
        set_settings_error(settings, "Invalid settings handle, setting name, or value list");
        return 0;
    }

    try {
        std::set<int> value_set;
        for (size_t i = 0; i < value_count; ++i) {
            value_set.insert(values[i]);
        }
        settings->value.set(name, value_set);
        clear_settings_error(settings);
        return 1;
    } catch (const std::exception& e) {
        set_settings_error(settings, e.what());
        return 0;
    }
}

int ifcopenshell_ifcgeom_settings_set_string_set(
    ifcopenshell_ifcgeom_settings_t* settings,
    const char* name,
    const char* const* values,
    size_t value_count
) {
    if (settings == nullptr || !is_valid_name(name) || (value_count > 0 && values == nullptr)) {
        set_settings_error(settings, "Invalid settings handle, setting name, or value list");
        return 0;
    }

    try {
        std::set<std::string> value_set;
        for (size_t i = 0; i < value_count; ++i) {
            if (values[i] == nullptr) {
                set_settings_error(settings, "Setting string set contains null entry");
                return 0;
            }
            value_set.insert(values[i]);
        }
        settings->value.set(name, value_set);
        clear_settings_error(settings);
        return 1;
    } catch (const std::exception& e) {
        set_settings_error(settings, e.what());
        return 0;
    }
}

int ifcopenshell_ifcgeom_settings_set_double_vector(
    ifcopenshell_ifcgeom_settings_t* settings,
    const char* name,
    const double* values,
    size_t value_count
) {
    if (settings == nullptr || !is_valid_name(name) || (value_count > 0 && values == nullptr)) {
        set_settings_error(settings, "Invalid settings handle, setting name, or value list");
        return 0;
    }

    try {
        std::vector<double> vec;
        vec.reserve(value_count);
        for (size_t i = 0; i < value_count; ++i) {
            vec.push_back(values[i]);
        }
        settings->value.set(name, vec);
        clear_settings_error(settings);
        return 1;
    } catch (const std::exception& e) {
        set_settings_error(settings, e.what());
        return 0;
    }
}

int ifcopenshell_ifcgeom_settings_get_bool(
    const ifcopenshell_ifcgeom_settings_t* settings,
    const char* name,
    int* out_value
) {
    if (settings == nullptr || !is_valid_name(name) || out_value == nullptr) {
        set_global_error("Invalid settings handle, setting name, or output pointer");
        return 0;
    }

    try {
        auto value = settings->value.get(name);
        if (const auto* bool_value = boost::get<bool>(&value)) {
            *out_value = *bool_value ? 1 : 0;
            clear_global_error();
            return 1;
        }
        set_global_error("Requested setting is not a bool");
        return 0;
    } catch (const std::exception& e) {
        set_global_error(e.what());
        return 0;
    }
}

int ifcopenshell_ifcgeom_settings_get_int(
    const ifcopenshell_ifcgeom_settings_t* settings,
    const char* name,
    int* out_value
) {
    if (settings == nullptr || !is_valid_name(name) || out_value == nullptr) {
        set_global_error("Invalid settings handle, setting name, or output pointer");
        return 0;
    }

    try {
        auto value = settings->value.get(name);
        if (const auto* int_value = boost::get<int>(&value)) {
            *out_value = *int_value;
            clear_global_error();
            return 1;
        }
        if (const auto* enum_value = boost::get<ifcopenshell::geometry::settings::IteratorOutputOptions>(&value)) {
            *out_value = static_cast<int>(*enum_value);
            clear_global_error();
            return 1;
        }
        if (const auto* enum_value = boost::get<ifcopenshell::geometry::settings::FunctionStepMethod>(&value)) {
            *out_value = static_cast<int>(*enum_value);
            clear_global_error();
            return 1;
        }
        if (const auto* enum_value = boost::get<ifcopenshell::geometry::settings::OutputDimensionalityTypes>(&value)) {
            *out_value = static_cast<int>(*enum_value);
            clear_global_error();
            return 1;
        }
        if (const auto* enum_value = boost::get<ifcopenshell::geometry::settings::TriangulationMethod>(&value)) {
            *out_value = static_cast<int>(*enum_value);
            clear_global_error();
            return 1;
        }

        set_global_error("Requested setting is not an int-like value");
        return 0;
    } catch (const std::exception& e) {
        set_global_error(e.what());
        return 0;
    }
}

int ifcopenshell_ifcgeom_settings_get_double(
    const ifcopenshell_ifcgeom_settings_t* settings,
    const char* name,
    double* out_value
) {
    if (settings == nullptr || !is_valid_name(name) || out_value == nullptr) {
        set_global_error("Invalid settings handle, setting name, or output pointer");
        return 0;
    }

    try {
        auto value = settings->value.get(name);
        if (const auto* double_value = boost::get<double>(&value)) {
            *out_value = *double_value;
            clear_global_error();
            return 1;
        }
        set_global_error("Requested setting is not a double");
        return 0;
    } catch (const std::exception& e) {
        set_global_error(e.what());
        return 0;
    }
}

const char* ifcopenshell_ifcgeom_settings_get_string(
    const ifcopenshell_ifcgeom_settings_t* settings,
    const char* name
) {
    if (settings == nullptr || !is_valid_name(name)) {
        set_global_error("Invalid settings handle or setting name");
        return nullptr;
    }

    try {
        auto value = settings->value.get(name);
        if (const auto* string_value = boost::get<std::string>(&value)) {
            g_string_cache = *string_value;
            clear_global_error();
            return g_string_cache.c_str();
        }
        set_global_error("Requested setting is not a string");
        return nullptr;
    } catch (const std::exception& e) {
        set_global_error(e.what());
        return nullptr;
    }
}

const char* ifcopenshell_ifcgeom_settings_get_type(
    const ifcopenshell_ifcgeom_settings_t* settings,
    const char* name
) {
    if (settings == nullptr || !is_valid_name(name)) {
        set_global_error("Invalid settings handle or setting name");
        return nullptr;
    }

    try {
        g_string_cache = const_cast<ifcopenshell::geometry::Settings&>(settings->value).get_type(name);
        clear_global_error();
        return g_string_cache.c_str();
    } catch (const std::exception& e) {
        set_global_error(e.what());
        return nullptr;
    }
}

ifcopenshell_ifcgeom_string_list_t* ifcopenshell_ifcgeom_settings_setting_names(
    const ifcopenshell_ifcgeom_settings_t* settings
) {
    if (settings == nullptr) {
        set_global_error("Settings handle is null");
        return nullptr;
    }

    try {
        std::unique_ptr<ifcopenshell_ifcgeom_string_list_t> list(new ifcopenshell_ifcgeom_string_list_t());
        list->values = settings->value.setting_names();
        list->cursor = 0;
        clear_list_error(list.get());
        return list.release();
    } catch (const std::exception& e) {
        set_global_error(e.what());
        return nullptr;
    }
}

const char* ifcopenshell_ifcgeom_settings_last_error(
    const ifcopenshell_ifcgeom_settings_t* settings
) {
    if (settings == nullptr) {
        return g_last_error.c_str();
    }
    return settings->last_error.c_str();
}

void ifcopenshell_ifcgeom_string_list_destroy(ifcopenshell_ifcgeom_string_list_t* list) {
    delete list;
}

size_t ifcopenshell_ifcgeom_string_list_count(const ifcopenshell_ifcgeom_string_list_t* list) {
    if (list == nullptr) {
        return 0;
    }
    return list->values.size();
}

void ifcopenshell_ifcgeom_string_list_reset(ifcopenshell_ifcgeom_string_list_t* list) {
    if (list == nullptr) {
        return;
    }
    list->cursor = 0;
    clear_list_error(list);
}

const char* ifcopenshell_ifcgeom_string_list_get(
    const ifcopenshell_ifcgeom_string_list_t* list,
    size_t index
) {
    if (list == nullptr) {
        set_global_error("String list handle is null");
        return nullptr;
    }
    if (index >= list->values.size()) {
        set_list_error(const_cast<ifcopenshell_ifcgeom_string_list_t*>(list), "String list index out of range");
        return nullptr;
    }

    clear_list_error(const_cast<ifcopenshell_ifcgeom_string_list_t*>(list));
    return list->values[index].c_str();
}

const char* ifcopenshell_ifcgeom_string_list_next(ifcopenshell_ifcgeom_string_list_t* list) {
    if (list == nullptr) {
        set_global_error("String list handle is null");
        return nullptr;
    }
    if (list->cursor >= list->values.size()) {
        clear_list_error(list);
        return nullptr;
    }
    return ifcopenshell_ifcgeom_string_list_get(list, list->cursor++);
}

ifcopenshell_ifcgeom_iterator_t* ifcopenshell_ifcgeom_iterator_create(
    const ifcopenshell_ifcparse_file_t* file,
    const ifcopenshell_ifcgeom_settings_t* settings,
    const char* geometry_library,
    int num_threads
) {
    return create_iterator_without_filters(file, settings, geometry_library, num_threads);
}

ifcopenshell_ifcgeom_iterator_t* ifcopenshell_ifcgeom_iterator_create_with_type_filter(
    const ifcopenshell_ifcparse_file_t* file,
    const ifcopenshell_ifcgeom_settings_t* settings,
    const char* geometry_library,
    const char* const* values,
    size_t value_count,
    int include,
    int num_threads
) {
    if (value_count > 0 && values == nullptr) {
        set_global_error("Type filter values pointer is null");
        return nullptr;
    }

    return create_iterator_with_filters(
        file,
        settings,
        geometry_library,
        num_threads,
        [&](std::vector<IfcGeom::filter_t>& filters) {
            std::set<std::string> entity_names;
            for (size_t i = 0; i < value_count; ++i) {
                if (values[i] != nullptr && values[i][0] != '\0') {
                    entity_names.insert(values[i]);
                }
            }
            IfcGeom::entity_filter entity_filter(include != 0, false, entity_names);
            filters.push_back(entity_filter);
        }
    );
}

ifcopenshell_ifcgeom_iterator_t* ifcopenshell_ifcgeom_iterator_create_with_globalid_filter(
    const ifcopenshell_ifcparse_file_t* file,
    const ifcopenshell_ifcgeom_settings_t* settings,
    const char* geometry_library,
    const char* const* values,
    size_t value_count,
    int include,
    int num_threads
) {
    if (value_count > 0 && values == nullptr) {
        set_global_error("GlobalId filter values pointer is null");
        return nullptr;
    }

    return create_iterator_with_filters(
        file,
        settings,
        geometry_library,
        num_threads,
        [&](std::vector<IfcGeom::filter_t>& filters) {
            std::set<std::string> ids;
            for (size_t i = 0; i < value_count; ++i) {
                if (values[i] != nullptr && values[i][0] != '\0') {
                    ids.insert(values[i]);
                }
            }

            IfcGeom::attribute_filter global_id_filter;
            global_id_filter.attribute_name = "GlobalId";
            global_id_filter.populate(ids);
            global_id_filter.include = include != 0;
            filters.push_back(global_id_filter);
        }
    );
}

ifcopenshell_ifcgeom_iterator_t* ifcopenshell_ifcgeom_iterator_create_with_id_filter(
    const ifcopenshell_ifcparse_file_t* file,
    const ifcopenshell_ifcgeom_settings_t* settings,
    const char* geometry_library,
    const int* values,
    size_t value_count,
    int include,
    int num_threads
) {
    if (value_count > 0 && values == nullptr) {
        set_global_error("ID filter values pointer is null");
        return nullptr;
    }

    return create_iterator_with_filters(
        file,
        settings,
        geometry_library,
        num_threads,
        [&](std::vector<IfcGeom::filter_t>& filters) {
            std::set<int> ids;
            for (size_t i = 0; i < value_count; ++i) {
                ids.insert(values[i]);
            }

            IfcGeom::instance_id_filter id_filter(include != 0, false, ids);
            filters.push_back(id_filter);
        }
    );
}

void ifcopenshell_ifcgeom_iterator_destroy(ifcopenshell_ifcgeom_iterator_t* iterator) {
    delete iterator;
}

int ifcopenshell_ifcgeom_iterator_initialize(ifcopenshell_ifcgeom_iterator_t* iterator) {
    if (iterator == nullptr || !iterator->value) {
        set_iterator_error(iterator, "IfcGeom iterator handle is null");
        return 0;
    }

    try {
        const bool initialized = iterator->value->initialize();
        clear_iterator_error(iterator);
        return initialized ? 1 : 0;
    } catch (const std::exception& e) {
        set_iterator_error(iterator, e.what());
        return 0;
    } catch (...) {
        set_iterator_error(iterator, "Unknown error initializing IfcGeom iterator");
        return 0;
    }
}

int ifcopenshell_ifcgeom_iterator_next(ifcopenshell_ifcgeom_iterator_t* iterator) {
    if (iterator == nullptr || !iterator->value) {
        set_iterator_error(iterator, "IfcGeom iterator handle is null");
        return 0;
    }

    try {
        const IfcUtil::IfcBaseClass* next = iterator->value->next();
        clear_iterator_error(iterator);
        return next != nullptr ? 1 : 0;
    } catch (const std::exception& e) {
        set_iterator_error(iterator, e.what());
        return 0;
    } catch (...) {
        set_iterator_error(iterator, "Unknown error advancing IfcGeom iterator");
        return 0;
    }
}

int ifcopenshell_ifcgeom_iterator_progress(const ifcopenshell_ifcgeom_iterator_t* iterator) {
    if (iterator == nullptr || !iterator->value) {
        return -1;
    }
    return iterator->value->progress();
}

int ifcopenshell_ifcgeom_iterator_had_error(const ifcopenshell_ifcgeom_iterator_t* iterator) {
    if (iterator == nullptr || !iterator->value) {
        return 0;
    }
    return iterator->value->had_error_processing_elements() ? 1 : 0;
}

const char* ifcopenshell_ifcgeom_iterator_unit_name(ifcopenshell_ifcgeom_iterator_t* iterator) {
    if (iterator == nullptr || !iterator->value) {
        set_iterator_error(iterator, "IfcGeom iterator handle is null");
        return nullptr;
    }

    try {
        iterator->text_cache = iterator->value->unit_name();
        clear_iterator_error(iterator);
        return iterator->text_cache.c_str();
    } catch (const std::exception& e) {
        set_iterator_error(iterator, e.what());
        return nullptr;
    } catch (...) {
        set_iterator_error(iterator, "Unknown error retrieving IfcGeom iterator unit name");
        return nullptr;
    }
}

double ifcopenshell_ifcgeom_iterator_unit_magnitude(ifcopenshell_ifcgeom_iterator_t* iterator) {
    if (iterator == nullptr || !iterator->value) {
        set_iterator_error(iterator, "IfcGeom iterator handle is null");
        return 0.0;
    }

    try {
        const double value = iterator->value->unit_magnitude();
        clear_iterator_error(iterator);
        return value;
    } catch (const std::exception& e) {
        set_iterator_error(iterator, e.what());
        return 0.0;
    } catch (...) {
        set_iterator_error(iterator, "Unknown error retrieving IfcGeom iterator unit magnitude");
        return 0.0;
    }
}

void ifcopenshell_ifcgeom_iterator_compute_bounds(
    ifcopenshell_ifcgeom_iterator_t* iterator,
    int with_geometry
) {
    if (iterator == nullptr || !iterator->value) {
        set_iterator_error(iterator, "IfcGeom iterator handle is null");
        return;
    }

    try {
        iterator->value->compute_bounds(with_geometry != 0);
        clear_iterator_error(iterator);
    } catch (const std::exception& e) {
        set_iterator_error(iterator, e.what());
    } catch (...) {
        set_iterator_error(iterator, "Unknown error computing IfcGeom bounds");
    }
}

int ifcopenshell_ifcgeom_iterator_bounds_min(
    const ifcopenshell_ifcgeom_iterator_t* iterator,
    double out_xyz[3]
) {
    if (iterator == nullptr || !iterator->value || out_xyz == nullptr) {
        set_global_error("Invalid iterator handle or output pointer");
        return 0;
    }

    copy_point_to_array(iterator->value->bounds_min(), out_xyz);
    clear_global_error();
    return 1;
}

int ifcopenshell_ifcgeom_iterator_bounds_max(
    const ifcopenshell_ifcgeom_iterator_t* iterator,
    double out_xyz[3]
) {
    if (iterator == nullptr || !iterator->value || out_xyz == nullptr) {
        set_global_error("Invalid iterator handle or output pointer");
        return 0;
    }

    copy_point_to_array(iterator->value->bounds_max(), out_xyz);
    clear_global_error();
    return 1;
}

int ifcopenshell_ifcgeom_iterator_current_id(
    ifcopenshell_ifcgeom_iterator_t* iterator,
    int* out_value
) {
    if (out_value == nullptr) {
        set_iterator_error(iterator, "Output pointer is null");
        return 0;
    }

    const IfcGeom::Element* element = current_element(iterator);
    if (element == nullptr) {
        return 0;
    }

    *out_value = element->id();
    return 1;
}

int ifcopenshell_ifcgeom_iterator_current_parent_id(
    ifcopenshell_ifcgeom_iterator_t* iterator,
    int* out_value
) {
    if (out_value == nullptr) {
        set_iterator_error(iterator, "Output pointer is null");
        return 0;
    }

    const IfcGeom::Element* element = current_element(iterator);
    if (element == nullptr) {
        return 0;
    }

    *out_value = element->parent_id();
    return 1;
}

const char* ifcopenshell_ifcgeom_iterator_current_type(ifcopenshell_ifcgeom_iterator_t* iterator) {
    const IfcGeom::Element* element = current_element(iterator);
    if (element == nullptr) {
        return nullptr;
    }
    return element->type().c_str();
}

const char* ifcopenshell_ifcgeom_iterator_current_guid(ifcopenshell_ifcgeom_iterator_t* iterator) {
    const IfcGeom::Element* element = current_element(iterator);
    if (element == nullptr) {
        return nullptr;
    }
    return element->guid().c_str();
}

int ifcopenshell_ifcgeom_iterator_current_transform(
    ifcopenshell_ifcgeom_iterator_t* iterator,
    double out_matrix_16[16]
) {
    if (out_matrix_16 == nullptr) {
        set_iterator_error(iterator, "Output pointer is null");
        return 0;
    }

    const IfcGeom::Element* element = current_element(iterator);
    if (element == nullptr) {
        return 0;
    }

    const auto* matrix_data = element->transformation().data()->ccomponents().data();
    for (size_t i = 0; i < 16; ++i) {
        out_matrix_16[i] = matrix_data[i];
    }

    clear_iterator_error(iterator);
    return 1;
}

ifcopenshell_ifcgeom_element_kind_t ifcopenshell_ifcgeom_iterator_current_kind(
    ifcopenshell_ifcgeom_iterator_t* iterator
) {
    const IfcGeom::Element* element = current_element(iterator);
    if (element == nullptr) {
        return IFCOPENSHELL_IFCGEOM_ELEMENT_NONE;
    }

    if (dynamic_cast<const IfcGeom::TriangulationElement*>(element) != nullptr) {
        return IFCOPENSHELL_IFCGEOM_ELEMENT_TRIANGULATION;
    }
    if (dynamic_cast<const IfcGeom::BRepElement*>(element) != nullptr) {
        return IFCOPENSHELL_IFCGEOM_ELEMENT_BREP;
    }
    if (dynamic_cast<const IfcGeom::SerializedElement*>(element) != nullptr) {
        return IFCOPENSHELL_IFCGEOM_ELEMENT_SERIALIZED;
    }
    return IFCOPENSHELL_IFCGEOM_ELEMENT_NONE;
}

ifcopenshell_ifcgeom_mesh_t* ifcopenshell_ifcgeom_iterator_get_mesh(
    ifcopenshell_ifcgeom_iterator_t* iterator
) {
    const IfcGeom::Element* element = current_element(iterator);
    if (element == nullptr) {
        return nullptr;
    }

    return make_mesh_snapshot(iterator, element);
}

const char* ifcopenshell_ifcgeom_iterator_log(ifcopenshell_ifcgeom_iterator_t* iterator) {
    if (iterator == nullptr || !iterator->value) {
        set_iterator_error(iterator, "IfcGeom iterator handle is null");
        return nullptr;
    }

    try {
        iterator->text_cache = iterator->value->getLog();
        clear_iterator_error(iterator);
        return iterator->text_cache.c_str();
    } catch (const std::exception& e) {
        set_iterator_error(iterator, e.what());
        return nullptr;
    } catch (...) {
        set_iterator_error(iterator, "Unknown error retrieving IfcGeom log");
        return nullptr;
    }
}

const char* ifcopenshell_ifcgeom_iterator_last_error(
    const ifcopenshell_ifcgeom_iterator_t* iterator
) {
    if (iterator == nullptr) {
        return g_last_error.c_str();
    }
    return iterator->last_error.c_str();
}

void ifcopenshell_ifcgeom_mesh_destroy(ifcopenshell_ifcgeom_mesh_t* mesh) {
    delete mesh;
}

int ifcopenshell_ifcgeom_mesh_id(const ifcopenshell_ifcgeom_mesh_t* mesh) {
    return mesh != nullptr ? mesh->id : 0;
}

int ifcopenshell_ifcgeom_mesh_parent_id(const ifcopenshell_ifcgeom_mesh_t* mesh) {
    return mesh != nullptr ? mesh->parent_id : -1;
}

const char* ifcopenshell_ifcgeom_mesh_name(const ifcopenshell_ifcgeom_mesh_t* mesh) {
    return mesh != nullptr ? mesh->name.c_str() : nullptr;
}

const char* ifcopenshell_ifcgeom_mesh_type(const ifcopenshell_ifcgeom_mesh_t* mesh) {
    return mesh != nullptr ? mesh->type.c_str() : nullptr;
}

const char* ifcopenshell_ifcgeom_mesh_guid(const ifcopenshell_ifcgeom_mesh_t* mesh) {
    return mesh != nullptr ? mesh->guid.c_str() : nullptr;
}

const char* ifcopenshell_ifcgeom_mesh_context(const ifcopenshell_ifcgeom_mesh_t* mesh) {
    return mesh != nullptr ? mesh->context.c_str() : nullptr;
}

const char* ifcopenshell_ifcgeom_mesh_unique_id(const ifcopenshell_ifcgeom_mesh_t* mesh) {
    return mesh != nullptr ? mesh->unique_id.c_str() : nullptr;
}

size_t ifcopenshell_ifcgeom_mesh_verts_count(const ifcopenshell_ifcgeom_mesh_t* mesh) {
    return mesh != nullptr ? mesh->verts.size() : 0;
}

const double* ifcopenshell_ifcgeom_mesh_verts_data(const ifcopenshell_ifcgeom_mesh_t* mesh) {
    return mesh != nullptr ? vector_data_or_null(mesh->verts) : nullptr;
}

size_t ifcopenshell_ifcgeom_mesh_faces_count(const ifcopenshell_ifcgeom_mesh_t* mesh) {
    return mesh != nullptr ? mesh->faces.size() : 0;
}

const int* ifcopenshell_ifcgeom_mesh_faces_data(const ifcopenshell_ifcgeom_mesh_t* mesh) {
    return mesh != nullptr ? vector_data_or_null(mesh->faces) : nullptr;
}

size_t ifcopenshell_ifcgeom_mesh_edges_count(const ifcopenshell_ifcgeom_mesh_t* mesh) {
    return mesh != nullptr ? mesh->edges.size() : 0;
}

const int* ifcopenshell_ifcgeom_mesh_edges_data(const ifcopenshell_ifcgeom_mesh_t* mesh) {
    return mesh != nullptr ? vector_data_or_null(mesh->edges) : nullptr;
}

size_t ifcopenshell_ifcgeom_mesh_normals_count(const ifcopenshell_ifcgeom_mesh_t* mesh) {
    return mesh != nullptr ? mesh->normals.size() : 0;
}

const double* ifcopenshell_ifcgeom_mesh_normals_data(const ifcopenshell_ifcgeom_mesh_t* mesh) {
    return mesh != nullptr ? vector_data_or_null(mesh->normals) : nullptr;
}

size_t ifcopenshell_ifcgeom_mesh_uvs_count(const ifcopenshell_ifcgeom_mesh_t* mesh) {
    return mesh != nullptr ? mesh->uvs.size() : 0;
}

const double* ifcopenshell_ifcgeom_mesh_uvs_data(const ifcopenshell_ifcgeom_mesh_t* mesh) {
    return mesh != nullptr ? vector_data_or_null(mesh->uvs) : nullptr;
}

size_t ifcopenshell_ifcgeom_mesh_material_ids_count(const ifcopenshell_ifcgeom_mesh_t* mesh) {
    return mesh != nullptr ? mesh->material_ids.size() : 0;
}

const int* ifcopenshell_ifcgeom_mesh_material_ids_data(const ifcopenshell_ifcgeom_mesh_t* mesh) {
    return mesh != nullptr ? vector_data_or_null(mesh->material_ids) : nullptr;
}

size_t ifcopenshell_ifcgeom_mesh_item_ids_count(const ifcopenshell_ifcgeom_mesh_t* mesh) {
    return mesh != nullptr ? mesh->item_ids.size() : 0;
}

const int* ifcopenshell_ifcgeom_mesh_item_ids_data(const ifcopenshell_ifcgeom_mesh_t* mesh) {
    return mesh != nullptr ? vector_data_or_null(mesh->item_ids) : nullptr;
}

size_t ifcopenshell_ifcgeom_mesh_edges_item_ids_count(const ifcopenshell_ifcgeom_mesh_t* mesh) {
    return mesh != nullptr ? mesh->edges_item_ids.size() : 0;
}

const int* ifcopenshell_ifcgeom_mesh_edges_item_ids_data(const ifcopenshell_ifcgeom_mesh_t* mesh) {
    return mesh != nullptr ? vector_data_or_null(mesh->edges_item_ids) : nullptr;
}

size_t ifcopenshell_ifcgeom_mesh_colors_count(const ifcopenshell_ifcgeom_mesh_t* mesh) {
    return mesh != nullptr ? mesh->colors.size() : 0;
}

const double* ifcopenshell_ifcgeom_mesh_colors_data(const ifcopenshell_ifcgeom_mesh_t* mesh) {
    return mesh != nullptr ? vector_data_or_null(mesh->colors) : nullptr;
}

size_t ifcopenshell_ifcgeom_mesh_transform_count(const ifcopenshell_ifcgeom_mesh_t* mesh) {
    return mesh != nullptr ? mesh->transform.size() : 0;
}

const double* ifcopenshell_ifcgeom_mesh_transform_data(const ifcopenshell_ifcgeom_mesh_t* mesh) {
    return mesh != nullptr ? vector_data_or_null(mesh->transform) : nullptr;
}

const char* ifcopenshell_ifcgeom_last_error(void) {
    return g_last_error.c_str();
}

} // extern "C"
