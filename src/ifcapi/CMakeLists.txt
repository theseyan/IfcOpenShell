# SPDX-License-Identifier: LGPL-3.0-or-later
#
# The handwritten high-level IFC authoring layer (root.create_entity, pset
# helpers, geometry placement, etc., ported from the Python ifcopenshell.api
# and ifcopenshell.util packages).
#
# This subdirectory does NOT build its own library. Instead, when the unified
# `ifcopenshell_capi` SHARED library exists (built by src/ifcwrap), it appends
# its sources onto that target so that consumers receive a single distribution
# exposing both the autogen low-level `ifcopenshell_*` C API and the
# handwritten high-level `ifcopenshell_*` C API, sharing the same opaque
# handle types declared in `ifcopenshell_api.h`.
#
# When the autogen unified library is unavailable (BUILD_IFCCAPI=OFF), this
# subdirectory is a no-op.

option(IFCOPENSHELL_BUILD_HIGHLEVEL_API
    "Bundle the handwritten high-level C API (src/ifcapi) into ifcopenshell_capi."
    ON)

if(NOT IFCOPENSHELL_BUILD_HIGHLEVEL_API)
    message(STATUS "ifcapi: high-level layer disabled (IFCOPENSHELL_BUILD_HIGHLEVEL_API=OFF).")
    return()
endif()

if(NOT TARGET ifcopenshell_capi)
    message(WARNING
        "ifcapi: ifcopenshell_capi target not present; high-level layer will not be built. "
        "Enable BUILD_IFCCAPI to produce the unified library.")
    return()
endif()

set(IFCAPI_SOURCES
    src/guid.cpp
    src/root.cpp
    src/file.cpp
    src/entity.cpp
    src/aggregate_spatial.cpp
    src/type.cpp
    src/nest.cpp
    src/group.cpp
    src/placement.cpp
    src/element.cpp
    src/representation.cpp
    src/geometry.cpp
    src/schema.cpp
    src/selector_parser.cpp
    src/selector_get.cpp
    src/selector_filter.cpp
    src/selector_format.cpp
    src/selector_set.cpp
    src/util_attribute.cpp
    src/util_shape.cpp
    src/util_classification.cpp
    src/util_element_styles.cpp
    src/util_element_traversal.cpp
    src/util_pset_template.cpp
    src/util_schema.cpp
    src/util_unit.cpp
    src/api_pset_add.cpp
    src/api_pset_edit.cpp
    src/api_geometry_placement.cpp
)
list(TRANSFORM IFCAPI_SOURCES PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/")

target_sources(ifcopenshell_capi PRIVATE ${IFCAPI_SOURCES})
target_include_directories(ifcopenshell_capi
    PUBLIC  "${CMAKE_CURRENT_SOURCE_DIR}/include"
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src"
            # The high-level sources include schema/parser headers using the
            # `ifcparse/...` prefix; expose the parent src/ directory so those
            # paths resolve.
            "${CMAKE_CURRENT_SOURCE_DIR}/.."
)
target_compile_definitions(ifcopenshell_capi
    PUBLIC  IFCOPENSHELL_HAS_HIGHLEVEL_API=1
    PRIVATE IFCAPI_BUILD
)

# Smoke test executable for the high-level layer.
if(BUILD_TESTING)
    add_executable(ifcapi_test_root tests/test_root.c)
    target_link_libraries(ifcapi_test_root PRIVATE ifcopenshell_capi)
    target_include_directories(ifcapi_test_root PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/../ifcwrap/binding_generator/generated"
    )
    add_test(NAME ifcapi_test_root COMMAND ifcapi_test_root)
endif()
