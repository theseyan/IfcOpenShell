#include "serializers/c_api.h"

#include "ifcgeom/ConversionSettings.h"
#include "ifcgeom/GeometrySerializer.h"
#include "ifcgeom/IfcGeomElement.h"
#include "ifcgeom/Iterator.h"
#include "ifcgeom/hybrid_kernel.h"
#include "ifcparse/IfcFile.h"
#include <Standard_Handle.hxx>

#ifdef IFOPSH_WITH_OPENCASCADE
#ifndef Handle
#define Handle(Class) opencascade::handle<Class>
#endif
#endif

#if defined(IFCOPENSHELL_SERIALIZERS_HAS_SVG)
#include <HLRBRep_Algo.hxx>
#include <HLRBRep_HLRToShape.hxx>
#include <HLRBRep_PolyAlgo.hxx>
#include <HLRAlgo_Projector.hxx>
#endif

#if defined(IFCOPENSHELL_SERIALIZERS_HAS_GLTF)
#include "serializers/GltfSerializer.h"
#endif
#if defined(IFCOPENSHELL_SERIALIZERS_HAS_IGES)
#include "serializers/IgesSerializer.h"
#include <IGESControl_Controller.hxx>
#endif
#if defined(IFCOPENSHELL_SERIALIZERS_HAS_JSON)
#include "serializers/JsonSerializer.h"
#endif
#if defined(IFCOPENSHELL_SERIALIZERS_HAS_STEP)
#include "serializers/StepSerializer.h"
#endif
#if defined(IFCOPENSHELL_SERIALIZERS_HAS_SVG)
#include "serializers/SvgSerializer.h"
#endif
#if defined(IFCOPENSHELL_SERIALIZERS_HAS_TTL)
#include "serializers/TtlWktSerializer.h"
#endif
#include "serializers/WavefrontObjSerializer.h"
#include "serializers/XmlSerializer.h"

#include <boost/variant/get.hpp>

#include <memory>
#include <string>
#include <utility>
#include <vector>

struct ifcopenshell_ifcserializers_settings {
    ifcopenshell::geometry::SerializerSettings value;
    std::string last_error;
};

struct ifcopenshell_ifcserializers_string_list {
    std::vector<std::string> values;
    size_t cursor = 0;
    std::string last_error;
};

enum class serializer_kind {
    obj,
    svg,
    ttl,
};

struct ifcopenshell_ifcserializers_serializer {
    serializer_kind kind;
    ifcopenshell::geometry::Settings geometry_settings;
    ifcopenshell::geometry::SerializerSettings serializer_settings;
    std::unique_ptr<stream_or_filename> primary_stream;
    std::unique_ptr<stream_or_filename> secondary_stream;
    std::unique_ptr<GeometrySerializer> value;
    bool primary_is_buffer = false;
    bool secondary_is_buffer = false;
    bool has_run = false;
    std::string primary_cache;
    std::string secondary_cache;
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

void set_settings_error(ifcopenshell_ifcserializers_settings_t* settings, const std::string& message) {
    if (settings != nullptr) {
        settings->last_error = message;
    }
    set_global_error(message);
}

void clear_settings_error(ifcopenshell_ifcserializers_settings_t* settings) {
    if (settings != nullptr) {
        settings->last_error.clear();
    }
    clear_global_error();
}

void set_list_error(ifcopenshell_ifcserializers_string_list_t* list, const std::string& message) {
    if (list != nullptr) {
        list->last_error = message;
    }
    set_global_error(message);
}

void clear_list_error(ifcopenshell_ifcserializers_string_list_t* list) {
    if (list != nullptr) {
        list->last_error.clear();
    }
    clear_global_error();
}

void set_serializer_error(ifcopenshell_ifcserializers_serializer_t* serializer, const std::string& message) {
    if (serializer != nullptr) {
        serializer->last_error = message;
    }
    set_global_error(message);
}

void clear_serializer_error(ifcopenshell_ifcserializers_serializer_t* serializer) {
    if (serializer != nullptr) {
        serializer->last_error.clear();
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

ifcopenshell::geometry::Settings geometry_settings_or_default(const ifcopenshell_ifcgeom_settings_t* settings) {
    if (settings == nullptr) {
        return ifcopenshell::geometry::Settings();
    }

    const void* native = ifcopenshell_ifcgeom_settings_native_const(settings);
    if (native == nullptr) {
        return ifcopenshell::geometry::Settings();
    }

    return *static_cast<const ifcopenshell::geometry::Settings*>(native);
}

ifcopenshell::geometry::SerializerSettings serializer_settings_or_default(
    const ifcopenshell_ifcserializers_settings_t* settings
) {
    if (settings == nullptr) {
        return ifcopenshell::geometry::SerializerSettings();
    }
    return settings->value;
}

int normalize_num_threads(int num_threads) {
    return num_threads > 0 ? num_threads : 1;
}

const std::string geometry_library_or_default(const char* geometry_library) {
    if (geometry_library == nullptr || geometry_library[0] == '\0') {
        return "opencascade";
    }
    return geometry_library;
}

template <typename Factory>
int export_geometry(
    const ifcopenshell_ifcparse_file_t* file,
    const ifcopenshell_ifcgeom_settings_t* geometry_settings,
    const ifcopenshell_ifcserializers_settings_t* serializer_settings,
    const char* geometry_library,
    int num_threads,
    Factory&& factory
) {
    IfcParse::IfcFile* ifc_file = native_file_from_handle(file);
    if (ifc_file == nullptr) {
        set_global_error("Unable to access native IfcFile from C ABI handle");
        return 0;
    }

    try {
        auto geometry = geometry_settings_or_default(geometry_settings);
        auto serializer_opts = serializer_settings_or_default(serializer_settings);

        std::unique_ptr<GeometrySerializer> serializer = factory(geometry, serializer_opts);
        if (!serializer) {
            set_global_error("Unable to construct serializer");
            return 0;
        }

        if (!serializer->isTesselated()) {
            geometry.get<ifcopenshell::geometry::settings::IteratorOutput>().value =
                ifcopenshell::geometry::settings::NATIVE;
        }

        if (!serializer->ready()) {
            set_global_error("Serializer output is not ready");
            return 0;
        }

        auto kernel = ifcopenshell::geometry::kernels::construct(
            ifc_file,
            geometry_library_or_default(geometry_library),
            geometry
        );

        IfcGeom::Iterator iterator(
            std::move(kernel),
            geometry,
            ifc_file,
            normalize_num_threads(num_threads)
        );

        serializer->setFile(ifc_file);

        const bool initialized = iterator.initialize();
        if (initialized) {
            serializer->setUnitNameAndMagnitude(
                iterator.unit_name(),
                static_cast<float>(iterator.unit_magnitude())
            );
        } else {
            serializer->setUnitNameAndMagnitude("METER", 1.0f);
        }

        serializer->writeHeader();

        if (initialized) {
            const bool tesselated = serializer->isTesselated();
            do {
                const IfcGeom::Element* element = iterator.get();
                if (element == nullptr) {
                    set_global_error("Iterator yielded null geometry element");
                    return 0;
                }

                if (tesselated) {
                    const auto* tri = dynamic_cast<const IfcGeom::TriangulationElement*>(element);
                    if (tri == nullptr) {
                        set_global_error("Serializer expects triangulated elements but iterator yielded non-triangulated data");
                        return 0;
                    }
                    serializer->write(tri);
                } else {
                    const auto* brep = dynamic_cast<const IfcGeom::BRepElement*>(element);
                    if (brep == nullptr) {
                        set_global_error("Serializer expects native BRep elements but iterator yielded non-BRep data");
                        return 0;
                    }
                    serializer->write(brep);
                }
            } while (iterator.next());
        }

        serializer->finalize();
        clear_global_error();
        return 1;
    } catch (const std::exception& e) {
        set_global_error(e.what());
        return 0;
    } catch (...) {
        set_global_error("Unknown error during serialization");
        return 0;
    }
}

std::unique_ptr<ifcopenshell_ifcserializers_serializer_t> make_serializer_base(
    serializer_kind kind,
    const ifcopenshell_ifcgeom_settings_t* geometry_settings,
    const ifcopenshell_ifcserializers_settings_t* serializer_settings
) {
    std::unique_ptr<ifcopenshell_ifcserializers_serializer_t> handle(new ifcopenshell_ifcserializers_serializer_t());
    handle->kind = kind;
    handle->geometry_settings = geometry_settings_or_default(geometry_settings);
    handle->serializer_settings = serializer_settings_or_default(serializer_settings);
    clear_serializer_error(handle.get());
    return handle;
}

std::unique_ptr<ifcopenshell_ifcserializers_serializer_t> make_obj_serializer(
    const ifcopenshell_ifcgeom_settings_t* geometry_settings,
    const ifcopenshell_ifcserializers_settings_t* serializer_settings,
    const char* obj_filename,
    const char* mtl_filename,
    bool buffer_output
) {
    if (!buffer_output && (!is_valid_name(obj_filename) || !is_valid_name(mtl_filename))) {
        set_global_error("Invalid OBJ or MTL output filename");
        return nullptr;
    }

    try {
        std::unique_ptr<ifcopenshell_ifcserializers_serializer_t> handle = make_serializer_base(
            serializer_kind::obj,
            geometry_settings,
            serializer_settings
        );
        handle->primary_is_buffer = buffer_output;
        handle->secondary_is_buffer = buffer_output;

        if (buffer_output) {
            handle->primary_stream.reset(new stream_or_filename());
            handle->secondary_stream.reset(new stream_or_filename());
        } else {
            handle->primary_stream.reset(new stream_or_filename(std::string(obj_filename)));
            handle->secondary_stream.reset(new stream_or_filename(std::string(mtl_filename)));
        }

        handle->value.reset(
            new WaveFrontOBJSerializer(
                *handle->primary_stream,
                *handle->secondary_stream,
                handle->geometry_settings,
                handle->serializer_settings
            )
        );
        if (!handle->value || !handle->value->ready()) {
            set_global_error("Serializer output is not ready");
            return nullptr;
        }
        clear_serializer_error(handle.get());
        return handle;
    } catch (const std::exception& e) {
        set_global_error(e.what());
        return nullptr;
    } catch (...) {
        set_global_error("Unknown error constructing OBJ serializer");
        return nullptr;
    }
}

std::unique_ptr<ifcopenshell_ifcserializers_serializer_t> make_svg_serializer(
    const ifcopenshell_ifcgeom_settings_t* geometry_settings,
    const ifcopenshell_ifcserializers_settings_t* serializer_settings,
    const char* svg_filename,
    bool buffer_output
) {
#if defined(IFCOPENSHELL_SERIALIZERS_HAS_SVG)
    if (!buffer_output && !is_valid_name(svg_filename)) {
        set_global_error("Invalid SVG output filename");
        return nullptr;
    }

    try {
        std::unique_ptr<ifcopenshell_ifcserializers_serializer_t> handle = make_serializer_base(
            serializer_kind::svg,
            geometry_settings,
            serializer_settings
        );
        handle->primary_is_buffer = buffer_output;
        handle->secondary_is_buffer = false;

        if (buffer_output) {
            handle->primary_stream.reset(new stream_or_filename());
        } else {
            handle->primary_stream.reset(new stream_or_filename(std::string(svg_filename)));
        }

        handle->value.reset(
            new SvgSerializer(
                *handle->primary_stream,
                handle->geometry_settings,
                handle->serializer_settings
            )
        );
        if (!handle->value || !handle->value->ready()) {
            set_global_error("Serializer output is not ready");
            return nullptr;
        }
        clear_serializer_error(handle.get());
        return handle;
    } catch (const std::exception& e) {
        set_global_error(e.what());
        return nullptr;
    } catch (...) {
        set_global_error("Unknown error constructing SVG serializer");
        return nullptr;
    }
#else
    (void)geometry_settings;
    (void)serializer_settings;
    (void)svg_filename;
    (void)buffer_output;
    set_global_error("SVG serializer is not enabled in this build");
    return nullptr;
#endif
}

std::unique_ptr<ifcopenshell_ifcserializers_serializer_t> make_ttl_serializer(
    const ifcopenshell_ifcgeom_settings_t* geometry_settings,
    const ifcopenshell_ifcserializers_settings_t* serializer_settings,
    const char* ttl_filename,
    bool buffer_output
) {
#if defined(IFCOPENSHELL_SERIALIZERS_HAS_TTL)
    if (!buffer_output && !is_valid_name(ttl_filename)) {
        set_global_error("Invalid TTL output filename");
        return nullptr;
    }

    try {
        std::unique_ptr<ifcopenshell_ifcserializers_serializer_t> handle = make_serializer_base(
            serializer_kind::ttl,
            geometry_settings,
            serializer_settings
        );
        handle->primary_is_buffer = buffer_output;
        handle->secondary_is_buffer = false;

        if (buffer_output) {
            handle->primary_stream.reset(new stream_or_filename());
        } else {
            handle->primary_stream.reset(new stream_or_filename(std::string(ttl_filename)));
        }

        handle->value.reset(
            new TtlWktSerializer(
                *handle->primary_stream,
                handle->geometry_settings,
                handle->serializer_settings
            )
        );
        if (!handle->value || !handle->value->ready()) {
            set_global_error("Serializer output is not ready");
            return nullptr;
        }
        clear_serializer_error(handle.get());
        return handle;
    } catch (const std::exception& e) {
        set_global_error(e.what());
        return nullptr;
    } catch (...) {
        set_global_error("Unknown error constructing TTL serializer");
        return nullptr;
    }
#else
    (void)geometry_settings;
    (void)serializer_settings;
    (void)ttl_filename;
    (void)buffer_output;
    set_global_error("TTL serializer is not enabled in this build");
    return nullptr;
#endif
}

int run_serializer(
    ifcopenshell_ifcserializers_serializer_t* serializer_handle,
    const ifcopenshell_ifcparse_file_t* file,
    const char* geometry_library,
    int num_threads
) {
    if (serializer_handle == nullptr || serializer_handle->value == nullptr) {
        set_serializer_error(serializer_handle, "Invalid serializer handle");
        return 0;
    }
    if (serializer_handle->has_run) {
        set_serializer_error(serializer_handle, "Serializer handle has already been executed");
        return 0;
    }

    IfcParse::IfcFile* ifc_file = native_file_from_handle(file);
    if (ifc_file == nullptr) {
        set_serializer_error(serializer_handle, "Unable to access native IfcFile from C ABI handle");
        return 0;
    }

    try {
        auto geometry = serializer_handle->geometry_settings;

        if (!serializer_handle->value->isTesselated()) {
            geometry.get<ifcopenshell::geometry::settings::IteratorOutput>().value =
                ifcopenshell::geometry::settings::NATIVE;
        }

        if (!serializer_handle->value->ready()) {
            set_serializer_error(serializer_handle, "Serializer output is not ready");
            return 0;
        }

        auto kernel = ifcopenshell::geometry::kernels::construct(
            ifc_file,
            geometry_library_or_default(geometry_library),
            geometry
        );

        IfcGeom::Iterator iterator(
            std::move(kernel),
            geometry,
            ifc_file,
            normalize_num_threads(num_threads)
        );

        serializer_handle->value->setFile(ifc_file);

        const bool initialized = iterator.initialize();
        if (initialized) {
            serializer_handle->value->setUnitNameAndMagnitude(
                iterator.unit_name(),
                static_cast<float>(iterator.unit_magnitude())
            );
        } else {
            serializer_handle->value->setUnitNameAndMagnitude("METER", 1.0f);
        }

        serializer_handle->value->writeHeader();

        if (initialized) {
            const bool tesselated = serializer_handle->value->isTesselated();
            do {
                const IfcGeom::Element* element = iterator.get();
                if (element == nullptr) {
                    set_serializer_error(serializer_handle, "Iterator yielded null geometry element");
                    return 0;
                }

                if (tesselated) {
                    const auto* tri = dynamic_cast<const IfcGeom::TriangulationElement*>(element);
                    if (tri == nullptr) {
                        set_serializer_error(
                            serializer_handle,
                            "Serializer expects triangulated elements but iterator yielded non-triangulated data"
                        );
                        return 0;
                    }
                    serializer_handle->value->write(tri);
                } else {
                    const auto* brep = dynamic_cast<const IfcGeom::BRepElement*>(element);
                    if (brep == nullptr) {
                        set_serializer_error(
                            serializer_handle,
                            "Serializer expects native BRep elements but iterator yielded non-BRep data"
                        );
                        return 0;
                    }
                    serializer_handle->value->write(brep);
                }
            } while (iterator.next());
        }

        serializer_handle->value->finalize();
        if (serializer_handle->primary_stream) {
            serializer_handle->primary_stream->stream.flush();
        }
        if (serializer_handle->secondary_stream) {
            serializer_handle->secondary_stream->stream.flush();
        }
        serializer_handle->has_run = true;
        if (serializer_handle->primary_is_buffer && serializer_handle->primary_stream) {
            serializer_handle->primary_cache = serializer_handle->primary_stream->get_value();
        }
        if (serializer_handle->secondary_is_buffer && serializer_handle->secondary_stream) {
            serializer_handle->secondary_cache = serializer_handle->secondary_stream->get_value();
        }
        clear_serializer_error(serializer_handle);
        return 1;
    } catch (const std::exception& e) {
        set_serializer_error(serializer_handle, e.what());
        return 0;
    } catch (...) {
        set_serializer_error(serializer_handle, "Unknown error during serialization");
        return 0;
    }
}

} // namespace

extern "C" {

ifcopenshell_ifcserializers_settings_t* ifcopenshell_ifcserializers_settings_create(void) {
    std::unique_ptr<ifcopenshell_ifcserializers_settings_t> settings(new ifcopenshell_ifcserializers_settings_t());
    clear_settings_error(settings.get());
    return settings.release();
}

void ifcopenshell_ifcserializers_settings_destroy(ifcopenshell_ifcserializers_settings_t* settings) {
    delete settings;
}

int ifcopenshell_ifcserializers_settings_set_bool(
    ifcopenshell_ifcserializers_settings_t* settings,
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

int ifcopenshell_ifcserializers_settings_set_int(
    ifcopenshell_ifcserializers_settings_t* settings,
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

int ifcopenshell_ifcserializers_settings_set_double(
    ifcopenshell_ifcserializers_settings_t* settings,
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

int ifcopenshell_ifcserializers_settings_set_string(
    ifcopenshell_ifcserializers_settings_t* settings,
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

int ifcopenshell_ifcserializers_settings_get_bool(
    const ifcopenshell_ifcserializers_settings_t* settings,
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

int ifcopenshell_ifcserializers_settings_get_int(
    const ifcopenshell_ifcserializers_settings_t* settings,
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
        set_global_error("Requested setting is not an int");
        return 0;
    } catch (const std::exception& e) {
        set_global_error(e.what());
        return 0;
    }
}

int ifcopenshell_ifcserializers_settings_get_double(
    const ifcopenshell_ifcserializers_settings_t* settings,
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

const char* ifcopenshell_ifcserializers_settings_get_string(
    const ifcopenshell_ifcserializers_settings_t* settings,
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

const char* ifcopenshell_ifcserializers_settings_get_type(
    const ifcopenshell_ifcserializers_settings_t* settings,
    const char* name
) {
    if (settings == nullptr || !is_valid_name(name)) {
        set_global_error("Invalid settings handle or setting name");
        return nullptr;
    }

    try {
        g_string_cache = const_cast<ifcopenshell::geometry::SerializerSettings&>(settings->value).get_type(name);
        clear_global_error();
        return g_string_cache.c_str();
    } catch (const std::exception& e) {
        set_global_error(e.what());
        return nullptr;
    }
}

ifcopenshell_ifcserializers_string_list_t* ifcopenshell_ifcserializers_settings_setting_names(
    const ifcopenshell_ifcserializers_settings_t* settings
) {
    if (settings == nullptr) {
        set_global_error("Settings handle is null");
        return nullptr;
    }

    try {
        std::unique_ptr<ifcopenshell_ifcserializers_string_list_t> list(new ifcopenshell_ifcserializers_string_list_t());
        list->values = settings->value.setting_names();
        list->cursor = 0;
        clear_list_error(list.get());
        return list.release();
    } catch (const std::exception& e) {
        set_global_error(e.what());
        return nullptr;
    }
}

const char* ifcopenshell_ifcserializers_settings_last_error(
    const ifcopenshell_ifcserializers_settings_t* settings
) {
    if (settings == nullptr) {
        return g_last_error.c_str();
    }
    return settings->last_error.c_str();
}

void ifcopenshell_ifcserializers_string_list_destroy(ifcopenshell_ifcserializers_string_list_t* list) {
    delete list;
}

size_t ifcopenshell_ifcserializers_string_list_count(const ifcopenshell_ifcserializers_string_list_t* list) {
    if (list == nullptr) {
        return 0;
    }
    return list->values.size();
}

void ifcopenshell_ifcserializers_string_list_reset(ifcopenshell_ifcserializers_string_list_t* list) {
    if (list == nullptr) {
        return;
    }
    list->cursor = 0;
    clear_list_error(list);
}

const char* ifcopenshell_ifcserializers_string_list_get(
    const ifcopenshell_ifcserializers_string_list_t* list,
    size_t index
) {
    if (list == nullptr) {
        set_global_error("String list handle is null");
        return nullptr;
    }
    if (index >= list->values.size()) {
        set_global_error("String list index out of range");
        return nullptr;
    }
    clear_global_error();
    return list->values[index].c_str();
}

const char* ifcopenshell_ifcserializers_string_list_next(ifcopenshell_ifcserializers_string_list_t* list) {
    if (list == nullptr) {
        set_global_error("String list handle is null");
        return nullptr;
    }
    if (list->cursor >= list->values.size()) {
        return nullptr;
    }
    clear_global_error();
    return list->values[list->cursor++].c_str();
}

ifcopenshell_ifcserializers_serializer_t* ifcopenshell_ifcserializers_serializer_obj_create_file(
    const ifcopenshell_ifcgeom_settings_t* geometry_settings,
    const ifcopenshell_ifcserializers_settings_t* serializer_settings,
    const char* obj_filename,
    const char* mtl_filename
) {
    std::unique_ptr<ifcopenshell_ifcserializers_serializer_t> serializer = make_obj_serializer(
        geometry_settings,
        serializer_settings,
        obj_filename,
        mtl_filename,
        false
    );
    return serializer.release();
}

ifcopenshell_ifcserializers_serializer_t* ifcopenshell_ifcserializers_serializer_obj_create_buffer(
    const ifcopenshell_ifcgeom_settings_t* geometry_settings,
    const ifcopenshell_ifcserializers_settings_t* serializer_settings
) {
    std::unique_ptr<ifcopenshell_ifcserializers_serializer_t> serializer = make_obj_serializer(
        geometry_settings,
        serializer_settings,
        nullptr,
        nullptr,
        true
    );
    return serializer.release();
}

ifcopenshell_ifcserializers_serializer_t* ifcopenshell_ifcserializers_serializer_svg_create_file(
    const ifcopenshell_ifcgeom_settings_t* geometry_settings,
    const ifcopenshell_ifcserializers_settings_t* serializer_settings,
    const char* svg_filename
) {
    std::unique_ptr<ifcopenshell_ifcserializers_serializer_t> serializer = make_svg_serializer(
        geometry_settings,
        serializer_settings,
        svg_filename,
        false
    );
    return serializer.release();
}

ifcopenshell_ifcserializers_serializer_t* ifcopenshell_ifcserializers_serializer_svg_create_buffer(
    const ifcopenshell_ifcgeom_settings_t* geometry_settings,
    const ifcopenshell_ifcserializers_settings_t* serializer_settings
) {
    std::unique_ptr<ifcopenshell_ifcserializers_serializer_t> serializer = make_svg_serializer(
        geometry_settings,
        serializer_settings,
        nullptr,
        true
    );
    return serializer.release();
}

ifcopenshell_ifcserializers_serializer_t* ifcopenshell_ifcserializers_serializer_ttl_create_file(
    const ifcopenshell_ifcgeom_settings_t* geometry_settings,
    const ifcopenshell_ifcserializers_settings_t* serializer_settings,
    const char* ttl_filename
) {
    std::unique_ptr<ifcopenshell_ifcserializers_serializer_t> serializer = make_ttl_serializer(
        geometry_settings,
        serializer_settings,
        ttl_filename,
        false
    );
    return serializer.release();
}

ifcopenshell_ifcserializers_serializer_t* ifcopenshell_ifcserializers_serializer_ttl_create_buffer(
    const ifcopenshell_ifcgeom_settings_t* geometry_settings,
    const ifcopenshell_ifcserializers_settings_t* serializer_settings
) {
    std::unique_ptr<ifcopenshell_ifcserializers_serializer_t> serializer = make_ttl_serializer(
        geometry_settings,
        serializer_settings,
        nullptr,
        true
    );
    return serializer.release();
}

void ifcopenshell_ifcserializers_serializer_destroy(ifcopenshell_ifcserializers_serializer_t* serializer) {
    delete serializer;
}

int ifcopenshell_ifcserializers_serializer_ready(const ifcopenshell_ifcserializers_serializer_t* serializer) {
    if (serializer == nullptr || serializer->value == nullptr) {
        set_global_error("Invalid serializer handle");
        return 0;
    }
    try {
        const int is_ready = serializer->value->ready() ? 1 : 0;
        clear_global_error();
        return is_ready;
    } catch (const std::exception& e) {
        set_global_error(e.what());
        return 0;
    } catch (...) {
        set_global_error("Unknown error checking serializer readiness");
        return 0;
    }
}

int ifcopenshell_ifcserializers_serializer_run(
    ifcopenshell_ifcserializers_serializer_t* serializer,
    const ifcopenshell_ifcparse_file_t* file,
    const char* geometry_library,
    int num_threads
) {
    return run_serializer(serializer, file, geometry_library, num_threads);
}

const char* ifcopenshell_ifcserializers_serializer_buffer_primary(
    ifcopenshell_ifcserializers_serializer_t* serializer
) {
    if (serializer == nullptr) {
        set_global_error("Invalid serializer handle");
        return nullptr;
    }
    if (!serializer->primary_is_buffer || serializer->primary_stream == nullptr) {
        set_serializer_error(serializer, "Primary output is not buffer-backed");
        return nullptr;
    }
    try {
        serializer->primary_cache = serializer->primary_stream->get_value();
        clear_serializer_error(serializer);
        return serializer->primary_cache.c_str();
    } catch (const std::exception& e) {
        set_serializer_error(serializer, e.what());
        return nullptr;
    } catch (...) {
        set_serializer_error(serializer, "Unknown error reading serializer primary buffer");
        return nullptr;
    }
}

const char* ifcopenshell_ifcserializers_serializer_buffer_secondary(
    ifcopenshell_ifcserializers_serializer_t* serializer
) {
    if (serializer == nullptr) {
        set_global_error("Invalid serializer handle");
        return nullptr;
    }
    if (!serializer->secondary_is_buffer || serializer->secondary_stream == nullptr) {
        set_serializer_error(serializer, "Secondary output is not buffer-backed");
        return nullptr;
    }
    try {
        serializer->secondary_cache = serializer->secondary_stream->get_value();
        clear_serializer_error(serializer);
        return serializer->secondary_cache.c_str();
    } catch (const std::exception& e) {
        set_serializer_error(serializer, e.what());
        return nullptr;
    } catch (...) {
        set_serializer_error(serializer, "Unknown error reading serializer secondary buffer");
        return nullptr;
    }
}

const char* ifcopenshell_ifcserializers_serializer_last_error(
    const ifcopenshell_ifcserializers_serializer_t* serializer
) {
    if (serializer == nullptr) {
        return g_last_error.c_str();
    }
    return serializer->last_error.c_str();
}

int ifcopenshell_ifcserializers_export_obj(
    const ifcopenshell_ifcparse_file_t* file,
    const ifcopenshell_ifcgeom_settings_t* geometry_settings,
    const ifcopenshell_ifcserializers_settings_t* serializer_settings,
    const char* obj_filename,
    const char* mtl_filename,
    const char* geometry_library,
    int num_threads
) {
    if (!is_valid_name(obj_filename) || !is_valid_name(mtl_filename)) {
        set_global_error("Invalid OBJ or MTL output filename");
        return 0;
    }

    return export_geometry(
        file,
        geometry_settings,
        serializer_settings,
        geometry_library,
        num_threads,
        [&](const ifcopenshell::geometry::Settings& geometry, const ifcopenshell::geometry::SerializerSettings& serializer) {
            return std::unique_ptr<GeometrySerializer>(
                new WaveFrontOBJSerializer(
                    stream_or_filename(std::string(obj_filename)),
                    stream_or_filename(std::string(mtl_filename)),
                    geometry,
                    serializer
                )
            );
        }
    );
}

int ifcopenshell_ifcserializers_export_svg(
    const ifcopenshell_ifcparse_file_t* file,
    const ifcopenshell_ifcgeom_settings_t* geometry_settings,
    const ifcopenshell_ifcserializers_settings_t* serializer_settings,
    const char* svg_filename,
    const char* geometry_library,
    int num_threads
) {
#if defined(IFCOPENSHELL_SERIALIZERS_HAS_SVG)
    if (!is_valid_name(svg_filename)) {
        set_global_error("Invalid SVG output filename");
        return 0;
    }

    return export_geometry(
        file,
        geometry_settings,
        serializer_settings,
        geometry_library,
        num_threads,
        [&](const ifcopenshell::geometry::Settings& geometry, const ifcopenshell::geometry::SerializerSettings& serializer) {
            return std::unique_ptr<GeometrySerializer>(
                new SvgSerializer(
                    stream_or_filename(std::string(svg_filename)),
                    geometry,
                    serializer
                )
            );
        }
    );
#else
    (void)file;
    (void)geometry_settings;
    (void)serializer_settings;
    (void)svg_filename;
    (void)geometry_library;
    (void)num_threads;
    set_global_error("SVG serializer is not enabled in this build");
    return 0;
#endif
}

int ifcopenshell_ifcserializers_export_step(
    const ifcopenshell_ifcparse_file_t* file,
    const ifcopenshell_ifcgeom_settings_t* geometry_settings,
    const ifcopenshell_ifcserializers_settings_t* serializer_settings,
    const char* step_filename,
    const char* geometry_library,
    int num_threads
) {
#if defined(IFCOPENSHELL_SERIALIZERS_HAS_STEP)
    if (!is_valid_name(step_filename)) {
        set_global_error("Invalid STEP output filename");
        return 0;
    }

    return export_geometry(
        file,
        geometry_settings,
        serializer_settings,
        geometry_library,
        num_threads,
        [&](const ifcopenshell::geometry::Settings& geometry, const ifcopenshell::geometry::SerializerSettings& serializer) {
            return std::unique_ptr<GeometrySerializer>(
                new StepSerializer(
                    std::string(step_filename),
                    geometry,
                    serializer
                )
            );
        }
    );
#else
    (void)file;
    (void)geometry_settings;
    (void)serializer_settings;
    (void)step_filename;
    (void)geometry_library;
    (void)num_threads;
    set_global_error("STEP serializer is not enabled in this build");
    return 0;
#endif
}

int ifcopenshell_ifcserializers_export_iges(
    const ifcopenshell_ifcparse_file_t* file,
    const ifcopenshell_ifcgeom_settings_t* geometry_settings,
    const ifcopenshell_ifcserializers_settings_t* serializer_settings,
    const char* iges_filename,
    const char* geometry_library,
    int num_threads
) {
#if defined(IFCOPENSHELL_SERIALIZERS_HAS_IGES)
    if (!is_valid_name(iges_filename)) {
        set_global_error("Invalid IGES output filename");
        return 0;
    }

    IGESControl_Controller::Init();

    return export_geometry(
        file,
        geometry_settings,
        serializer_settings,
        geometry_library,
        num_threads,
        [&](const ifcopenshell::geometry::Settings& geometry, const ifcopenshell::geometry::SerializerSettings& serializer) {
            return std::unique_ptr<GeometrySerializer>(
                new IgesSerializer(
                    std::string(iges_filename),
                    geometry,
                    serializer
                )
            );
        }
    );
#else
    (void)file;
    (void)geometry_settings;
    (void)serializer_settings;
    (void)iges_filename;
    (void)geometry_library;
    (void)num_threads;
    set_global_error("IGES serializer is not enabled in this build");
    return 0;
#endif
}

int ifcopenshell_ifcserializers_export_ttl(
    const ifcopenshell_ifcparse_file_t* file,
    const ifcopenshell_ifcgeom_settings_t* geometry_settings,
    const ifcopenshell_ifcserializers_settings_t* serializer_settings,
    const char* ttl_filename,
    const char* geometry_library,
    int num_threads
) {
#if defined(IFCOPENSHELL_SERIALIZERS_HAS_TTL)
    if (!is_valid_name(ttl_filename)) {
        set_global_error("Invalid TTL output filename");
        return 0;
    }

    return export_geometry(
        file,
        geometry_settings,
        serializer_settings,
        geometry_library,
        num_threads,
        [&](const ifcopenshell::geometry::Settings& geometry, const ifcopenshell::geometry::SerializerSettings& serializer) {
            return std::unique_ptr<GeometrySerializer>(
                new TtlWktSerializer(
                    stream_or_filename(std::string(ttl_filename)),
                    geometry,
                    serializer
                )
            );
        }
    );
#else
    (void)file;
    (void)geometry_settings;
    (void)serializer_settings;
    (void)ttl_filename;
    (void)geometry_library;
    (void)num_threads;
    set_global_error("TTL serializer is not enabled in this build");
    return 0;
#endif
}

int ifcopenshell_ifcserializers_export_gltf(
    const ifcopenshell_ifcparse_file_t* file,
    const ifcopenshell_ifcgeom_settings_t* geometry_settings,
    const ifcopenshell_ifcserializers_settings_t* serializer_settings,
    const char* gltf_filename,
    const char* geometry_library,
    int num_threads
) {
#if defined(IFCOPENSHELL_SERIALIZERS_HAS_GLTF) && defined(WITH_GLTF)
    if (!is_valid_name(gltf_filename)) {
        set_global_error("Invalid glTF output filename");
        return 0;
    }

    return export_geometry(
        file,
        geometry_settings,
        serializer_settings,
        geometry_library,
        num_threads,
        [&](const ifcopenshell::geometry::Settings& geometry, const ifcopenshell::geometry::SerializerSettings& serializer) {
            return std::unique_ptr<GeometrySerializer>(
                new GltfSerializer(gltf_filename, geometry, serializer)
            );
        }
    );
#else
    (void)file;
    (void)geometry_settings;
    (void)serializer_settings;
    (void)gltf_filename;
    (void)geometry_library;
    (void)num_threads;
    set_global_error("glTF serializer is not enabled in this build");
    return 0;
#endif
}

int ifcopenshell_ifcserializers_export_xml(
    const ifcopenshell_ifcparse_file_t* file,
    const char* xml_filename
) {
    if (!is_valid_name(xml_filename)) {
        set_global_error("Invalid XML output filename");
        return 0;
    }

    IfcParse::IfcFile* ifc_file = native_file_from_handle(file);
    if (ifc_file == nullptr) {
        set_global_error("Unable to access native IfcFile from C ABI handle");
        return 0;
    }

    try {
        XmlSerializer serializer(ifc_file, std::string(xml_filename));
        if (!serializer.ready()) {
            set_global_error("XML serializer output is not ready");
            return 0;
        }
        serializer.finalize();
        clear_global_error();
        return 1;
    } catch (const std::exception& e) {
        set_global_error(e.what());
        return 0;
    } catch (...) {
        set_global_error("Unknown error during XML serialization");
        return 0;
    }
}

int ifcopenshell_ifcserializers_export_json(
    const ifcopenshell_ifcparse_file_t* file,
    const char* json_filename
) {
#if defined(IFCOPENSHELL_SERIALIZERS_HAS_JSON) && defined(WITH_GLTF)
    if (!is_valid_name(json_filename)) {
        set_global_error("Invalid JSON output filename");
        return 0;
    }

    IfcParse::IfcFile* ifc_file = native_file_from_handle(file);
    if (ifc_file == nullptr) {
        set_global_error("Unable to access native IfcFile from C ABI handle");
        return 0;
    }

    try {
        JsonSerializer serializer(
            ifc_file,
            std::string(json_filename),
            JsonSerializer::JSON_DIALECT_CREOOX
        );
        if (!serializer.ready()) {
            set_global_error("JSON serializer output is not ready");
            return 0;
        }
        serializer.finalize();
        clear_global_error();
        return 1;
    } catch (const std::exception& e) {
        set_global_error(e.what());
        return 0;
    } catch (...) {
        set_global_error("Unknown error during JSON serialization");
        return 0;
    }
#else
    (void)file;
    (void)json_filename;
    set_global_error("JSON serializer is not enabled in this build");
    return 0;
#endif
}

int ifcopenshell_ifcserializers_has_gltf(void) {
#if defined(IFCOPENSHELL_SERIALIZERS_HAS_GLTF)
    return 1;
#else
    return 0;
#endif
}

int ifcopenshell_ifcserializers_has_json(void) {
#if defined(IFCOPENSHELL_SERIALIZERS_HAS_JSON)
    return 1;
#else
    return 0;
#endif
}

int ifcopenshell_ifcserializers_has_svg(void) {
#if defined(IFCOPENSHELL_SERIALIZERS_HAS_SVG)
    return 1;
#else
    return 0;
#endif
}

int ifcopenshell_ifcserializers_has_ttl(void) {
#if defined(IFCOPENSHELL_SERIALIZERS_HAS_TTL)
    return 1;
#else
    return 0;
#endif
}

int ifcopenshell_ifcserializers_has_step(void) {
#if defined(IFCOPENSHELL_SERIALIZERS_HAS_STEP)
    return 1;
#else
    return 0;
#endif
}

int ifcopenshell_ifcserializers_has_iges(void) {
#if defined(IFCOPENSHELL_SERIALIZERS_HAS_IGES)
    return 1;
#else
    return 0;
#endif
}

const char* ifcopenshell_ifcserializers_last_error(void) {
    return g_last_error.c_str();
}

} // extern "C"
