#include "ifcgeom/c_api.h"

#include "ifcgeom/ConversionSettings.h"
#include "ifcgeom/Converter.h"
#include "ifcgeom/IfcGeomElement.h"
#include "ifcgeom/IfcGeomRepresentation.h"
#include "ifcgeom/Iterator.h"
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#pragma clang diagnostic ignored "-Wunused-value"
#endif
#include "ifcgeom/kernels/opencascade/IfcGeomTree.h"
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
#include "ifcgeom/taxonomy.h"
#include "ifcgeom/hybrid_kernel.h"
#include "ifcparse/IfcFile.h"

#include <boost/variant/get.hpp>

#include <array>
#include <sstream>
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

struct ifcopenshell_ifcgeom_serialized {
    int id = 0;
    int parent_id = -1;
    std::string name;
    std::string type;
    std::string guid;
    std::string context;
    std::string unique_id;

    std::string brep_data;
    std::vector<double> surface_styles;
    std::vector<int> surface_style_ids;
    std::vector<double> transform;
};

struct ifcopenshell_ifcgeom_created_shape {
    ifcopenshell_ifcgeom_created_shape_kind_t kind = IFCOPENSHELL_IFCGEOM_CREATED_SHAPE_NONE;
    std::unique_ptr<ifcopenshell_ifcgeom_mesh_t> mesh;
    std::unique_ptr<ifcopenshell_ifcgeom_serialized_t> serialized;
    std::array<double, 16> transform{{0.0}};
};

struct ifcopenshell_ifcgeom_tree {
    std::unique_ptr<IfcGeom::tree> value;
    std::string last_error;
};

struct ifcopenshell_ifcgeom_id_list {
    std::vector<int> values;
    size_t cursor = 0;
    std::string last_error;
};

struct ifcopenshell_ifcgeom_clash_list {
    std::vector<ifcopenshell_ifcgeom_clash_t> values;
    size_t cursor = 0;
    std::string last_error;
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

void set_tree_error(ifcopenshell_ifcgeom_tree_t* tree, const std::string& message) {
    if (tree != nullptr) {
        tree->last_error = message;
    }
    set_global_error(message);
}

void clear_tree_error(ifcopenshell_ifcgeom_tree_t* tree) {
    if (tree != nullptr) {
        tree->last_error.clear();
    }
    clear_global_error();
}

void set_id_list_error(ifcopenshell_ifcgeom_id_list_t* list, const std::string& message) {
    if (list != nullptr) {
        list->last_error = message;
    }
    set_global_error(message);
}

void clear_id_list_error(ifcopenshell_ifcgeom_id_list_t* list) {
    if (list != nullptr) {
        list->last_error.clear();
    }
    clear_global_error();
}

void set_clash_list_error(ifcopenshell_ifcgeom_clash_list_t* list, const std::string& message) {
    if (list != nullptr) {
        list->last_error = message;
    }
    set_global_error(message);
}

void clear_clash_list_error(ifcopenshell_ifcgeom_clash_list_t* list) {
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

IfcUtil::IfcBaseEntity* native_product_entity_from_file_and_id(
    const ifcopenshell_ifcparse_file_t* file,
    int id,
    std::string& out_error
) {
    IfcParse::IfcFile* ifc_file = native_file_from_handle(file);
    if (ifc_file == nullptr) {
        out_error = parse_error_or_default("Unable to access native IfcFile from C ABI handle");
        return nullptr;
    }

    IfcUtil::IfcBaseClass* instance = ifc_file->instance_by_id(id);
    if (instance == nullptr) {
        out_error = "Ifc entity id does not exist in file";
        return nullptr;
    }

    if (!instance->declaration().is("IfcProduct")) {
        out_error = "Ifc entity is not an IfcProduct";
        return nullptr;
    }

    IfcUtil::IfcBaseEntity* entity = instance->as<IfcUtil::IfcBaseEntity>();
    if (entity == nullptr) {
        out_error = "Ifc entity is not a concrete IfcBaseEntity";
        return nullptr;
    }

    return entity;
}

IfcUtil::IfcBaseClass* native_instance_from_file_and_id(
    const ifcopenshell_ifcparse_file_t* file,
    int id,
    std::string& out_error
) {
    IfcParse::IfcFile* ifc_file = native_file_from_handle(file);
    if (ifc_file == nullptr) {
        out_error = parse_error_or_default("Unable to access native IfcFile from C ABI handle");
        return nullptr;
    }

    IfcUtil::IfcBaseClass* instance = ifc_file->instance_by_id(id);
    if (instance == nullptr) {
        out_error = "Ifc entity id does not exist in file";
        return nullptr;
    }

    return instance;
}

bool entity_exists_in_tree(
    ifcopenshell_ifcgeom_tree_t* tree,
    const IfcUtil::IfcBaseEntity* entity
) {
    if (tree == nullptr || !tree->value || entity == nullptr) {
        return false;
    }

    try {
        const auto selected = tree->value->select_box(entity, false, 0.0);
        return !selected.empty();
    } catch (...) {
        return false;
    }
}

ifcopenshell_ifcgeom_id_list_t* make_id_list_from_entities(
    const std::vector<const IfcUtil::IfcBaseEntity*>& entities
) {
    std::unique_ptr<ifcopenshell_ifcgeom_id_list_t> list(new ifcopenshell_ifcgeom_id_list_t());
    list->values.reserve(entities.size());
    for (const auto* entity : entities) {
        if (entity != nullptr) {
            list->values.push_back(static_cast<int>(entity->id()));
        }
    }
    clear_id_list_error(list.get());
    return list.release();
}

ifcopenshell_ifcgeom_clash_list_t* make_clash_list(
    const std::vector<IfcGeom::clash>& clashes
) {
    std::unique_ptr<ifcopenshell_ifcgeom_clash_list_t> list(new ifcopenshell_ifcgeom_clash_list_t());
    list->values.reserve(clashes.size());
    for (const auto& clash : clashes) {
        ifcopenshell_ifcgeom_clash_t row{};
        row.clash_type = clash.clash_type;
        row.a_id = clash.a != nullptr ? static_cast<int>(clash.a->id()) : 0;
        row.b_id = clash.b != nullptr ? static_cast<int>(clash.b->id()) : 0;
        row.distance = clash.distance;
        row.p1[0] = clash.p1[0];
        row.p1[1] = clash.p1[1];
        row.p1[2] = clash.p1[2];
        row.p2[0] = clash.p2[0];
        row.p2[1] = clash.p2[1];
        row.p2[2] = clash.p2[2];
        list->values.push_back(row);
    }
    clear_clash_list_error(list.get());
    return list.release();
}

template <typename Fn>
ifcopenshell_ifcgeom_clash_list_t* tree_run_clash_query(
    ifcopenshell_ifcgeom_tree_t* tree,
    const ifcopenshell_ifcparse_file_t* file,
    const int* set_a_ids,
    size_t set_a_count,
    const int* set_b_ids,
    size_t set_b_count,
    Fn&& fn
) {
    if (tree == nullptr || !tree->value) {
        set_tree_error(tree, "IfcGeom tree handle is null");
        return nullptr;
    }
    if (set_a_count > 0 && set_a_ids == nullptr) {
        set_tree_error(tree, "Set A ids pointer is null");
        return nullptr;
    }
    if (set_b_count > 0 && set_b_ids == nullptr) {
        set_tree_error(tree, "Set B ids pointer is null");
        return nullptr;
    }

    std::vector<const IfcUtil::IfcBaseEntity*> set_a;
    set_a.reserve(set_a_count);
    std::vector<const IfcUtil::IfcBaseEntity*> set_b;
    set_b.reserve(set_b_count);

    std::string error_message;
    for (size_t i = 0; i < set_a_count; ++i) {
        const auto* entity = native_product_entity_from_file_and_id(file, set_a_ids[i], error_message);
        if (entity == nullptr) {
            set_tree_error(tree, error_message);
            return nullptr;
        }
        if (entity_exists_in_tree(tree, entity)) {
            set_a.push_back(entity);
        }
    }
    for (size_t i = 0; i < set_b_count; ++i) {
        const auto* entity = native_product_entity_from_file_and_id(file, set_b_ids[i], error_message);
        if (entity == nullptr) {
            set_tree_error(tree, error_message);
            return nullptr;
        }
        if (entity_exists_in_tree(tree, entity)) {
            set_b.push_back(entity);
        }
    }

    try {
        auto clashes = fn(set_a, set_b);
        clear_tree_error(tree);
        return make_clash_list(clashes);
    } catch (const std::exception& e) {
        set_tree_error(tree, e.what());
        return nullptr;
    } catch (...) {
        set_tree_error(tree, "Unknown error querying tree clashes");
        return nullptr;
    }
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

ifcopenshell_ifcgeom_serialized_t* make_serialized_snapshot(
    ifcopenshell_ifcgeom_iterator_t* iterator,
    const IfcGeom::Element* element
) {
    const auto* serialized = dynamic_cast<const IfcGeom::SerializedElement*>(element);
    const auto* brep = dynamic_cast<const IfcGeom::BRepElement*>(element);
    if (serialized == nullptr && brep == nullptr) {
        set_iterator_error(iterator, "Current IfcGeom element is neither serialized nor native BRep");
        return nullptr;
    }

    std::unique_ptr<ifcopenshell_ifcgeom_serialized_t> result(new ifcopenshell_ifcgeom_serialized_t());

    result->id = element->id();
    result->parent_id = element->parent_id();
    result->name = element->name();
    result->type = element->type();
    result->guid = element->guid();
    result->context = element->context();
    result->unique_id = element->unique_id();

    const auto* matrix_data = element->transformation().data()->ccomponents().data();
    result->transform.assign(matrix_data, matrix_data + 16);

    if (serialized != nullptr) {
        const auto& geometry = serialized->geometry();
        result->brep_data = geometry.brep_data();
        result->surface_styles.assign(geometry.surface_styles().begin(), geometry.surface_styles().end());
        result->surface_style_ids.assign(geometry.surface_style_ids().begin(), geometry.surface_style_ids().end());
    } else {
        IfcGeom::Representation::Serialization geometry(brep->geometry());
        result->brep_data = geometry.brep_data();
        result->surface_styles.assign(geometry.surface_styles().begin(), geometry.surface_styles().end());
        result->surface_style_ids.assign(geometry.surface_style_ids().begin(), geometry.surface_style_ids().end());
    }

    clear_iterator_error(iterator);
    return result.release();
}

void set_identity_transform(std::vector<double>& out_matrix) {
    out_matrix.assign(16, 0.0);
    out_matrix[0] = 1.0;
    out_matrix[5] = 1.0;
    out_matrix[10] = 1.0;
    out_matrix[15] = 1.0;
}

int instance_id_or_zero(const IfcUtil::IfcBaseClass* instance) {
    const auto* entity = instance != nullptr ? instance->as<IfcUtil::IfcBaseEntity>() : nullptr;
    return entity != nullptr ? static_cast<int>(entity->id()) : 0;
}

ifcopenshell_ifcgeom_mesh_t* make_mesh_snapshot_from_representation(
    const IfcGeom::Representation::Triangulation& geometry,
    const IfcUtil::IfcBaseClass* instance
) {
    std::unique_ptr<ifcopenshell_ifcgeom_mesh_t> mesh(new ifcopenshell_ifcgeom_mesh_t());
    mesh->id = instance_id_or_zero(instance);
    mesh->parent_id = -1;
    mesh->name.clear();
    mesh->type = instance != nullptr ? instance->declaration().name() : "";
    mesh->guid.clear();
    mesh->context.clear();
    mesh->unique_id.clear();
    set_identity_transform(mesh->transform);

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

    return mesh.release();
}

ifcopenshell_ifcgeom_serialized_t* make_serialized_snapshot_from_representation(
    const IfcGeom::Representation::Serialization& geometry,
    const IfcUtil::IfcBaseClass* instance
) {
    std::unique_ptr<ifcopenshell_ifcgeom_serialized_t> serialized(new ifcopenshell_ifcgeom_serialized_t());
    serialized->id = instance_id_or_zero(instance);
    serialized->parent_id = -1;
    serialized->name.clear();
    serialized->type = instance != nullptr ? instance->declaration().name() : "";
    serialized->guid.clear();
    serialized->context.clear();
    serialized->unique_id.clear();
    serialized->brep_data = geometry.brep_data();
    serialized->surface_styles.assign(geometry.surface_styles().begin(), geometry.surface_styles().end());
    serialized->surface_style_ids.assign(geometry.surface_style_ids().begin(), geometry.surface_style_ids().end());
    set_identity_transform(serialized->transform);
    return serialized.release();
}

template <typename FilterBuilder>
ifcopenshell_ifcgeom_iterator_t* create_iterator_with_filters(
    const ifcopenshell_ifcparse_file_t* file,
    const ifcopenshell_ifcgeom_settings_t* settings,
    const char* geometry_library,
    int num_threads,
    FilterBuilder&& filter_builder
);

ifcopenshell_ifcgeom_created_shape_t* make_created_shape_from_product_id(
    const ifcopenshell_ifcparse_file_t* file,
    const ifcopenshell_ifcgeom_settings_t* settings,
    const char* geometry_library,
    int id
) {
    std::unique_ptr<ifcopenshell_ifcgeom_iterator_t> iterator(
        create_iterator_with_filters(
            file,
            settings,
            geometry_library,
            1,
            [&](std::vector<IfcGeom::filter_t>& filters) {
                std::set<int> set_ids;
                set_ids.insert(id);
                IfcGeom::instance_id_filter id_filter(true, false, set_ids);
                filters.push_back(id_filter);
            }
        )
    );

    if (!iterator) {
        return nullptr;
    }

    if (ifcopenshell_ifcgeom_iterator_initialize(iterator.get()) == 0) {
        if (iterator->last_error.empty()) {
            set_iterator_error(iterator.get(), "No geometry generated for requested entity id");
        }
        return nullptr;
    }

    const IfcGeom::Element* element = current_element(iterator.get());
    if (element == nullptr) {
        return nullptr;
    }

    std::unique_ptr<ifcopenshell_ifcgeom_created_shape_t> shape(new ifcopenshell_ifcgeom_created_shape_t());
    const auto kind = ifcopenshell_ifcgeom_iterator_current_kind(iterator.get());
    if (kind == IFCOPENSHELL_IFCGEOM_ELEMENT_TRIANGULATION) {
        shape->kind = IFCOPENSHELL_IFCGEOM_CREATED_SHAPE_TRIANGULATION;
        shape->mesh.reset(make_mesh_snapshot(iterator.get(), element));
    } else if (kind == IFCOPENSHELL_IFCGEOM_ELEMENT_SERIALIZED) {
        shape->kind = IFCOPENSHELL_IFCGEOM_CREATED_SHAPE_SERIALIZED;
        shape->serialized.reset(make_serialized_snapshot(iterator.get(), element));
    } else if (kind == IFCOPENSHELL_IFCGEOM_ELEMENT_BREP) {
        shape->kind = IFCOPENSHELL_IFCGEOM_CREATED_SHAPE_BREP;
        shape->serialized.reset(make_serialized_snapshot(iterator.get(), element));
    } else {
        set_iterator_error(iterator.get(), "Unsupported shape kind for product");
        return nullptr;
    }

    if ((shape->mesh == nullptr && shape->kind == IFCOPENSHELL_IFCGEOM_CREATED_SHAPE_TRIANGULATION) ||
        (shape->serialized == nullptr && (shape->kind == IFCOPENSHELL_IFCGEOM_CREATED_SHAPE_SERIALIZED ||
                                          shape->kind == IFCOPENSHELL_IFCGEOM_CREATED_SHAPE_BREP))) {
        return nullptr;
    }
    return shape.release();
}

ifcopenshell_ifcgeom_created_shape_t* make_created_shape_from_transform_instance(
    IfcParse::IfcFile* ifc_file,
    ifcopenshell::geometry::Settings settings_value,
    const char* geometry_library,
    IfcUtil::IfcBaseClass* instance
) {
    std::unique_ptr<ifcopenshell_ifcgeom_created_shape_t> shape(new ifcopenshell_ifcgeom_created_shape_t());

    ifcopenshell::geometry::Converter kernel(
        ifcopenshell::geometry::kernels::construct(ifc_file, geometry_library_or_default(geometry_library), settings_value),
        ifc_file,
        settings_value
    );

    auto item = ifcopenshell::geometry::taxonomy::cast<ifcopenshell::geometry::taxonomy::matrix4>(
        kernel.mapping()->map(instance)
    );
    if (item == nullptr) {
        set_global_error("Failed to convert placement into transformation matrix");
        return nullptr;
    }

    if (settings_value.get<ifcopenshell::geometry::settings::ConvertBackUnits>().get()) {
        item = ifcopenshell::geometry::taxonomy::matrix4::ptr(item->clone_());
        item->components().col(3).head<3>() /= kernel.settings().get<ifcopenshell::geometry::settings::LengthUnit>().get();
    }

    shape->kind = IFCOPENSHELL_IFCGEOM_CREATED_SHAPE_TRANSFORM;
    const auto* matrix_data = item->ccomponents().data();
    for (size_t i = 0; i < 16; ++i) {
        shape->transform[i] = matrix_data[i];
    }
    return shape.release();
}

ifcopenshell_ifcgeom_created_shape_t* make_created_shape_from_representation_instance(
    IfcParse::IfcFile* ifc_file,
    ifcopenshell::geometry::Settings settings_value,
    const char* geometry_library,
    IfcUtil::IfcBaseClass* instance
) {
    ifcopenshell::geometry::Converter kernel(
        ifcopenshell::geometry::kernels::construct(ifc_file, geometry_library_or_default(geometry_library), settings_value),
        ifc_file,
        settings_value
    );

    IfcGeom::ConversionResults shapes;
    try {
        shapes = kernel.convert(instance);
    } catch (...) {
        set_global_error("Failed to process shape for representation instance");
        return nullptr;
    }

    const std::string rep_id = std::to_string(instance_id_or_zero(instance));
    IfcGeom::Representation::BRep brep(kernel.settings(), instance->declaration().name(), rep_id, shapes);

    std::unique_ptr<ifcopenshell_ifcgeom_created_shape_t> shape(new ifcopenshell_ifcgeom_created_shape_t());
    const auto output = settings_value.get<ifcopenshell::geometry::settings::IteratorOutput>().get();
    if (output == ifcopenshell::geometry::settings::TRIANGULATED) {
        IfcGeom::Representation::Triangulation triangulation(brep);
        shape->kind = IFCOPENSHELL_IFCGEOM_CREATED_SHAPE_TRIANGULATION;
        shape->mesh.reset(make_mesh_snapshot_from_representation(triangulation, instance));
    } else if (output == ifcopenshell::geometry::settings::SERIALIZED) {
        IfcGeom::Representation::Serialization serialized(brep);
        shape->kind = IFCOPENSHELL_IFCGEOM_CREATED_SHAPE_SERIALIZED;
        shape->serialized.reset(make_serialized_snapshot_from_representation(serialized, instance));
    } else {
        IfcGeom::Representation::Serialization serialized(brep);
        shape->kind = IFCOPENSHELL_IFCGEOM_CREATED_SHAPE_BREP;
        shape->serialized.reset(make_serialized_snapshot_from_representation(serialized, instance));
    }

    return shape.release();
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

void* ifcopenshell_ifcgeom_settings_native(ifcopenshell_ifcgeom_settings_t* settings) {
    if (settings == nullptr) {
        return nullptr;
    }
    return static_cast<void*>(&settings->value);
}

const void* ifcopenshell_ifcgeom_settings_native_const(const ifcopenshell_ifcgeom_settings_t* settings) {
    if (settings == nullptr) {
        return nullptr;
    }
    return static_cast<const void*>(&settings->value);
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

ifcopenshell_ifcgeom_tree_t* ifcopenshell_ifcgeom_tree_create(void) {
    std::unique_ptr<ifcopenshell_ifcgeom_tree_t> tree(new ifcopenshell_ifcgeom_tree_t());
    tree->value.reset(new IfcGeom::tree());
    clear_tree_error(tree.get());
    return tree.release();
}

void ifcopenshell_ifcgeom_tree_destroy(ifcopenshell_ifcgeom_tree_t* tree) {
    delete tree;
}

int ifcopenshell_ifcgeom_tree_add_file(
    ifcopenshell_ifcgeom_tree_t* tree,
    const ifcopenshell_ifcparse_file_t* file,
    const ifcopenshell_ifcgeom_settings_t* settings
) {
    if (tree == nullptr || !tree->value) {
        set_tree_error(tree, "IfcGeom tree handle is null");
        return 0;
    }

    IfcParse::IfcFile* ifc_file = native_file_from_handle(file);
    if (ifc_file == nullptr) {
        set_tree_error(tree, parse_error_or_default("Unable to access native IfcFile from C ABI handle"));
        return 0;
    }

    try {
        auto settings_value = settings_value_or_default(settings);
        tree->value->add_file(*ifc_file, settings_value);
        clear_tree_error(tree);
        return 1;
    } catch (const std::exception& e) {
        set_tree_error(tree, e.what());
        return 0;
    } catch (...) {
        set_tree_error(tree, "Unknown error adding file to IfcGeom tree");
        return 0;
    }
}

ifcopenshell_ifcgeom_id_list_t* ifcopenshell_ifcgeom_tree_select_by_id(
    ifcopenshell_ifcgeom_tree_t* tree,
    const ifcopenshell_ifcparse_file_t* file,
    int id,
    int completely_within,
    double extend
) {
    if (tree == nullptr || !tree->value) {
        set_tree_error(tree, "IfcGeom tree handle is null");
        return nullptr;
    }

    std::string error_message;
    IfcUtil::IfcBaseEntity* entity = native_product_entity_from_file_and_id(file, id, error_message);
    if (entity == nullptr) {
        set_tree_error(tree, error_message);
        return nullptr;
    }

    try {
        auto selected = tree->value->select(entity, completely_within != 0, extend);
        clear_tree_error(tree);
        return make_id_list_from_entities(selected);
    } catch (const std::exception& e) {
        set_tree_error(tree, e.what());
        return nullptr;
    } catch (...) {
        set_tree_error(tree, "Unknown error selecting IfcGeom tree by id");
        return nullptr;
    }
}

ifcopenshell_ifcgeom_id_list_t* ifcopenshell_ifcgeom_tree_select_box(
    ifcopenshell_ifcgeom_tree_t* tree,
    const double min_xyz[3],
    const double max_xyz[3],
    int completely_within
) {
    if (tree == nullptr || !tree->value) {
        set_tree_error(tree, "IfcGeom tree handle is null");
        return nullptr;
    }
    if (min_xyz == nullptr || max_xyz == nullptr) {
        set_tree_error(tree, "Bounding box pointers must not be null");
        return nullptr;
    }

    try {
        Bnd_Box box;
        box.Add(gp_Pnt(min_xyz[0], min_xyz[1], min_xyz[2]));
        box.Add(gp_Pnt(max_xyz[0], max_xyz[1], max_xyz[2]));
        auto selected = tree->value->select_box(box, completely_within != 0);
        clear_tree_error(tree);
        return make_id_list_from_entities(selected);
    } catch (const std::exception& e) {
        set_tree_error(tree, e.what());
        return nullptr;
    } catch (...) {
        set_tree_error(tree, "Unknown error selecting IfcGeom tree by bounding box");
        return nullptr;
    }
}

ifcopenshell_ifcgeom_id_list_t* ifcopenshell_ifcgeom_tree_select_point(
    ifcopenshell_ifcgeom_tree_t* tree,
    const double xyz[3],
    double extend
) {
    if (tree == nullptr || !tree->value) {
        set_tree_error(tree, "IfcGeom tree handle is null");
        return nullptr;
    }
    if (xyz == nullptr) {
        set_tree_error(tree, "Point pointer must not be null");
        return nullptr;
    }

    try {
        gp_Pnt point(xyz[0], xyz[1], xyz[2]);
        auto selected = tree->value->select(point, extend);
        clear_tree_error(tree);
        return make_id_list_from_entities(selected);
    } catch (const std::exception& e) {
        set_tree_error(tree, e.what());
        return nullptr;
    } catch (...) {
        set_tree_error(tree, "Unknown error selecting IfcGeom tree by point");
        return nullptr;
    }
}

ifcopenshell_ifcgeom_clash_list_t* ifcopenshell_ifcgeom_tree_clash_intersection_many(
    ifcopenshell_ifcgeom_tree_t* tree,
    const ifcopenshell_ifcparse_file_t* file,
    const int* set_a_ids,
    size_t set_a_count,
    const int* set_b_ids,
    size_t set_b_count,
    double tolerance,
    int check_all
) {
    return tree_run_clash_query(
        tree,
        file,
        set_a_ids,
        set_a_count,
        set_b_ids,
        set_b_count,
        [&](const std::vector<const IfcUtil::IfcBaseEntity*>& set_a, const std::vector<const IfcUtil::IfcBaseEntity*>& set_b) {
            return tree->value->clash_intersection_many(set_a, set_b, tolerance, check_all != 0);
        }
    );
}

ifcopenshell_ifcgeom_clash_list_t* ifcopenshell_ifcgeom_tree_clash_collision_many(
    ifcopenshell_ifcgeom_tree_t* tree,
    const ifcopenshell_ifcparse_file_t* file,
    const int* set_a_ids,
    size_t set_a_count,
    const int* set_b_ids,
    size_t set_b_count,
    int allow_touching
) {
    return tree_run_clash_query(
        tree,
        file,
        set_a_ids,
        set_a_count,
        set_b_ids,
        set_b_count,
        [&](const std::vector<const IfcUtil::IfcBaseEntity*>& set_a, const std::vector<const IfcUtil::IfcBaseEntity*>& set_b) {
            return tree->value->clash_collision_many(set_a, set_b, allow_touching != 0);
        }
    );
}

ifcopenshell_ifcgeom_clash_list_t* ifcopenshell_ifcgeom_tree_clash_clearance_many(
    ifcopenshell_ifcgeom_tree_t* tree,
    const ifcopenshell_ifcparse_file_t* file,
    const int* set_a_ids,
    size_t set_a_count,
    const int* set_b_ids,
    size_t set_b_count,
    double clearance,
    int check_all
) {
    return tree_run_clash_query(
        tree,
        file,
        set_a_ids,
        set_a_count,
        set_b_ids,
        set_b_count,
        [&](const std::vector<const IfcUtil::IfcBaseEntity*>& set_a, const std::vector<const IfcUtil::IfcBaseEntity*>& set_b) {
            return tree->value->clash_clearance_many(set_a, set_b, clearance, check_all != 0);
        }
    );
}

const char* ifcopenshell_ifcgeom_tree_last_error(const ifcopenshell_ifcgeom_tree_t* tree) {
    if (tree == nullptr) {
        return g_last_error.c_str();
    }
    return tree->last_error.c_str();
}

void ifcopenshell_ifcgeom_id_list_destroy(ifcopenshell_ifcgeom_id_list_t* list) {
    delete list;
}

size_t ifcopenshell_ifcgeom_id_list_count(const ifcopenshell_ifcgeom_id_list_t* list) {
    return list != nullptr ? list->values.size() : 0;
}

void ifcopenshell_ifcgeom_id_list_reset(ifcopenshell_ifcgeom_id_list_t* list) {
    if (list == nullptr) {
        return;
    }
    list->cursor = 0;
    clear_id_list_error(list);
}

int ifcopenshell_ifcgeom_id_list_get(
    const ifcopenshell_ifcgeom_id_list_t* list,
    size_t index,
    int* out_value
) {
    if (list == nullptr || out_value == nullptr) {
        set_global_error("ID list handle or output pointer is null");
        return 0;
    }
    if (index >= list->values.size()) {
        return 0;
    }
    *out_value = list->values[index];
    clear_global_error();
    return 1;
}

int ifcopenshell_ifcgeom_id_list_next(
    ifcopenshell_ifcgeom_id_list_t* list,
    int* out_value
) {
    if (list == nullptr || out_value == nullptr) {
        set_id_list_error(list, "ID list handle or output pointer is null");
        return 0;
    }
    if (list->cursor >= list->values.size()) {
        return 0;
    }
    *out_value = list->values[list->cursor++];
    clear_id_list_error(list);
    return 1;
}

void ifcopenshell_ifcgeom_clash_list_destroy(ifcopenshell_ifcgeom_clash_list_t* list) {
    delete list;
}

size_t ifcopenshell_ifcgeom_clash_list_count(const ifcopenshell_ifcgeom_clash_list_t* list) {
    return list != nullptr ? list->values.size() : 0;
}

void ifcopenshell_ifcgeom_clash_list_reset(ifcopenshell_ifcgeom_clash_list_t* list) {
    if (list == nullptr) {
        return;
    }
    list->cursor = 0;
    clear_clash_list_error(list);
}

const ifcopenshell_ifcgeom_clash_t* ifcopenshell_ifcgeom_clash_list_get(
    const ifcopenshell_ifcgeom_clash_list_t* list,
    size_t index
) {
    if (list == nullptr) {
        set_global_error("Clash list handle is null");
        return nullptr;
    }
    if (index >= list->values.size()) {
        return nullptr;
    }
    clear_global_error();
    return &list->values[index];
}

const ifcopenshell_ifcgeom_clash_t* ifcopenshell_ifcgeom_clash_list_next(
    ifcopenshell_ifcgeom_clash_list_t* list
) {
    if (list == nullptr) {
        set_clash_list_error(list, "Clash list handle is null");
        return nullptr;
    }
    if (list->cursor >= list->values.size()) {
        return nullptr;
    }
    clear_clash_list_error(list);
    return &list->values[list->cursor++];
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

ifcopenshell_ifcgeom_serialized_t* ifcopenshell_ifcgeom_iterator_get_serialized(
    ifcopenshell_ifcgeom_iterator_t* iterator
) {
    const IfcGeom::Element* element = current_element(iterator);
    if (element == nullptr) {
        return nullptr;
    }

    return make_serialized_snapshot(iterator, element);
}

ifcopenshell_ifcgeom_mesh_t* ifcopenshell_ifcgeom_create_mesh_for_id(
    const ifcopenshell_ifcparse_file_t* file,
    const ifcopenshell_ifcgeom_settings_t* settings,
    const char* geometry_library,
    int id
) {
    if (id <= 0) {
        set_global_error("Entity id must be a positive integer");
        return nullptr;
    }

    const int ids[1] = {id};
    std::unique_ptr<ifcopenshell_ifcgeom_iterator_t> iterator(
        ifcopenshell_ifcgeom_iterator_create_with_id_filter(
            file,
            settings,
            geometry_library,
            ids,
            1,
            1,
            1
        )
    );

    if (!iterator) {
        return nullptr;
    }

    if (ifcopenshell_ifcgeom_iterator_initialize(iterator.get()) == 0) {
        if (iterator->last_error.empty()) {
            set_iterator_error(iterator.get(), "No geometry generated for requested entity id");
        }
        return nullptr;
    }

    const IfcGeom::Element* element = current_element(iterator.get());
    if (element == nullptr) {
        return nullptr;
    }

    return make_mesh_snapshot(iterator.get(), element);
}

ifcopenshell_ifcgeom_created_shape_t* ifcopenshell_ifcgeom_create_shape_for_id(
    const ifcopenshell_ifcparse_file_t* file,
    const ifcopenshell_ifcgeom_settings_t* settings,
    const char* geometry_library,
    int id
) {
    if (id <= 0) {
        set_global_error("Entity id must be a positive integer");
        return nullptr;
    }

    std::string error_message;
    IfcUtil::IfcBaseClass* instance = native_instance_from_file_and_id(file, id, error_message);
    if (instance == nullptr) {
        set_global_error(error_message);
        return nullptr;
    }

    IfcParse::IfcFile* ifc_file = native_file_from_handle(file);
    if (ifc_file == nullptr) {
        set_global_error(parse_error_or_default("Unable to access native IfcFile from C ABI handle"));
        return nullptr;
    }

    try {
        if (instance->declaration().is("IfcProduct")) {
            auto* shape = make_created_shape_from_product_id(file, settings, geometry_library, id);
            if (shape != nullptr) {
                clear_global_error();
            }
            return shape;
        }

        auto settings_value = settings_value_or_default(settings);
        if (instance->declaration().is("IfcPlacement") || instance->declaration().is("IfcObjectPlacement")) {
            auto* shape = make_created_shape_from_transform_instance(ifc_file, settings_value, geometry_library, instance);
            if (shape != nullptr) {
                clear_global_error();
            }
            return shape;
        }

        if (instance->declaration().is("IfcRepresentationItem") ||
            instance->declaration().is("IfcRepresentation") ||
            instance->declaration().is("IfcProfileDef")) {
            auto* shape = make_created_shape_from_representation_instance(ifc_file, settings_value, geometry_library, instance);
            if (shape != nullptr) {
                clear_global_error();
            }
            return shape;
        }

        set_global_error("Entity type is not supported by create_shape");
        return nullptr;
    } catch (const std::exception& e) {
        set_global_error(e.what());
        return nullptr;
    } catch (...) {
        set_global_error("Unknown error creating shape for entity");
        return nullptr;
    }
}

const char* ifcopenshell_ifcgeom_map_shape_repr_for_id(
    const ifcopenshell_ifcparse_file_t* file,
    const ifcopenshell_ifcgeom_settings_t* settings,
    int id
) {
    if (id <= 0) {
        set_global_error("Entity id must be a positive integer");
        return nullptr;
    }

    std::string error_message;
    IfcUtil::IfcBaseClass* instance = native_instance_from_file_and_id(file, id, error_message);
    if (instance == nullptr) {
        set_global_error(error_message);
        return nullptr;
    }

    IfcParse::IfcFile* ifc_file = native_file_from_handle(file);
    if (ifc_file == nullptr) {
        set_global_error(parse_error_or_default("Unable to access native IfcFile from C ABI handle"));
        return nullptr;
    }

    try {
        auto settings_value = settings_value_or_default(settings);
        std::unique_ptr<ifcopenshell::geometry::abstract_mapping> mapping(
            ifcopenshell::geometry::impl::mapping_implementations().construct(ifc_file, settings_value)
        );
        auto item = mapping->map(instance);
        if (item == nullptr) {
            set_global_error("map_shape returned null taxonomy item");
            return nullptr;
        }

        std::ostringstream oss;
        item->print(oss);
        g_string_cache = oss.str();
        if (!g_string_cache.empty() && g_string_cache.back() == '\n') {
            g_string_cache.pop_back();
        }
        clear_global_error();
        return g_string_cache.c_str();
    } catch (const std::exception& e) {
        set_global_error(e.what());
        return nullptr;
    } catch (...) {
        set_global_error("Unknown error mapping shape to taxonomy");
        return nullptr;
    }
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

void ifcopenshell_ifcgeom_serialized_destroy(ifcopenshell_ifcgeom_serialized_t* serialized) {
    delete serialized;
}

int ifcopenshell_ifcgeom_serialized_id(const ifcopenshell_ifcgeom_serialized_t* serialized) {
    return serialized != nullptr ? serialized->id : 0;
}

int ifcopenshell_ifcgeom_serialized_parent_id(const ifcopenshell_ifcgeom_serialized_t* serialized) {
    return serialized != nullptr ? serialized->parent_id : -1;
}

const char* ifcopenshell_ifcgeom_serialized_name(const ifcopenshell_ifcgeom_serialized_t* serialized) {
    return serialized != nullptr ? serialized->name.c_str() : nullptr;
}

const char* ifcopenshell_ifcgeom_serialized_type(const ifcopenshell_ifcgeom_serialized_t* serialized) {
    return serialized != nullptr ? serialized->type.c_str() : nullptr;
}

const char* ifcopenshell_ifcgeom_serialized_guid(const ifcopenshell_ifcgeom_serialized_t* serialized) {
    return serialized != nullptr ? serialized->guid.c_str() : nullptr;
}

const char* ifcopenshell_ifcgeom_serialized_context(const ifcopenshell_ifcgeom_serialized_t* serialized) {
    return serialized != nullptr ? serialized->context.c_str() : nullptr;
}

const char* ifcopenshell_ifcgeom_serialized_unique_id(const ifcopenshell_ifcgeom_serialized_t* serialized) {
    return serialized != nullptr ? serialized->unique_id.c_str() : nullptr;
}

const char* ifcopenshell_ifcgeom_serialized_brep_data(const ifcopenshell_ifcgeom_serialized_t* serialized) {
    return serialized != nullptr ? serialized->brep_data.c_str() : nullptr;
}

size_t ifcopenshell_ifcgeom_serialized_surface_styles_count(const ifcopenshell_ifcgeom_serialized_t* serialized) {
    return serialized != nullptr ? serialized->surface_styles.size() : 0;
}

const double* ifcopenshell_ifcgeom_serialized_surface_styles_data(const ifcopenshell_ifcgeom_serialized_t* serialized) {
    return serialized != nullptr ? vector_data_or_null(serialized->surface_styles) : nullptr;
}

size_t ifcopenshell_ifcgeom_serialized_surface_style_ids_count(const ifcopenshell_ifcgeom_serialized_t* serialized) {
    return serialized != nullptr ? serialized->surface_style_ids.size() : 0;
}

const int* ifcopenshell_ifcgeom_serialized_surface_style_ids_data(const ifcopenshell_ifcgeom_serialized_t* serialized) {
    return serialized != nullptr ? vector_data_or_null(serialized->surface_style_ids) : nullptr;
}

size_t ifcopenshell_ifcgeom_serialized_transform_count(const ifcopenshell_ifcgeom_serialized_t* serialized) {
    return serialized != nullptr ? serialized->transform.size() : 0;
}

const double* ifcopenshell_ifcgeom_serialized_transform_data(const ifcopenshell_ifcgeom_serialized_t* serialized) {
    return serialized != nullptr ? vector_data_or_null(serialized->transform) : nullptr;
}

void ifcopenshell_ifcgeom_created_shape_destroy(ifcopenshell_ifcgeom_created_shape_t* shape) {
    delete shape;
}

ifcopenshell_ifcgeom_created_shape_kind_t ifcopenshell_ifcgeom_created_shape_kind(
    const ifcopenshell_ifcgeom_created_shape_t* shape
) {
    return shape != nullptr ? shape->kind : IFCOPENSHELL_IFCGEOM_CREATED_SHAPE_NONE;
}

const ifcopenshell_ifcgeom_mesh_t* ifcopenshell_ifcgeom_created_shape_mesh(
    const ifcopenshell_ifcgeom_created_shape_t* shape
) {
    if (shape == nullptr || !shape->mesh) {
        return nullptr;
    }
    return shape->mesh.get();
}

const ifcopenshell_ifcgeom_serialized_t* ifcopenshell_ifcgeom_created_shape_serialized(
    const ifcopenshell_ifcgeom_created_shape_t* shape
) {
    if (shape == nullptr || !shape->serialized) {
        return nullptr;
    }
    return shape->serialized.get();
}

int ifcopenshell_ifcgeom_created_shape_transform(
    const ifcopenshell_ifcgeom_created_shape_t* shape,
    double out_matrix_16[16]
) {
    if (shape == nullptr || out_matrix_16 == nullptr) {
        set_global_error("Created shape handle or output pointer is null");
        return 0;
    }
    if (shape->kind != IFCOPENSHELL_IFCGEOM_CREATED_SHAPE_TRANSFORM) {
        set_global_error("Created shape is not a transform");
        return 0;
    }
    for (size_t i = 0; i < 16; ++i) {
        out_matrix_16[i] = shape->transform[i];
    }
    clear_global_error();
    return 1;
}

const char* ifcopenshell_ifcgeom_last_error(void) {
    return g_last_error.c_str();
}

} // extern "C"
