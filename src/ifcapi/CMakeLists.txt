# SPDX-License-Identifier: LGPL-3.0-or-later
#
# The handwritten high-level IFC authoring layer (root.create_entity, pset
# helpers, geometry placement, etc., ported from the Python ifcopenshell.api
# and ifcopenshell.util packages).
#
# This subdirectory does NOT build its own library. The unified
# `ifcopenshell_capi` library is created by src/ifcwrap before this directory
# is added; this directory appends
# its sources onto that target so that consumers receive a single distribution
# exposing both the autogen low-level `ifcopenshell_*` C API and the
# handwritten high-level `ifcopenshell_*` C API, sharing the same opaque
# handle types declared in `ifcopenshell_api.h`.
#
if(NOT TARGET ifcopenshell_capi)
    message(FATAL_ERROR "BUILD_IFCAPI requires the unified ifcopenshell_capi target")
endif()

set(IFCAPI_SOURCES
    src/guid.cpp
    src/root.cpp
    src/owner.cpp
    src/file.cpp
    src/entity.cpp
    src/api/aggregate/relationship.cpp
    src/api/boundary/boundary.cpp
    src/api/constraint/constraint.cpp
    src/api/control/control.cpp
    src/api/cost/cost.cpp
    src/api/cogo/cogo.cpp
    src/api/context/context.cpp
    src/api/document/document.cpp
    src/api/drawing/drawing.cpp
    src/api/feature/feature.cpp
    src/api/georeference/georeference.cpp
    src/api/group/relationship.cpp
    src/api/grid/grid.cpp
    src/api/layer/layer.cpp
    src/api/library/library.cpp
    src/api/material/material.cpp
    src/api/nest/relationship.cpp
    src/api/owner/owner.cpp
    src/api/type/relationship.cpp
    src/placement.cpp
    src/element.cpp
    src/representation.cpp
    src/geometry.cpp
    src/selector/parser.cpp
    src/selector/get.cpp
    src/selector/filter.cpp
    src/selector/format.cpp
    src/selector/set.cpp
    src/util/attribute.cpp
    src/util/shape.cpp
    src/util/classification.cpp
    src/util/element/styles.cpp
    src/util/element/traversal.cpp
    src/util/pset/template.cpp
    src/util/schema.cpp
    src/util/unit.cpp
    src/api/pset/add.cpp
    src/api/pset/edit.cpp
    src/api/pset/relationship.cpp
    src/api/pset_template/template.cpp
    src/api/profile/profile.cpp
    src/api/project/project.cpp
    src/api/resource/resource.cpp
    src/api/sequence/sequence.cpp
    src/api/style/style.cpp
    src/api/structural/structural.cpp
    src/api/system/system.cpp
    src/api/unit/unit.cpp
    src/api/geometry/placement.cpp
    src/api/geometry/shape_builder.cpp
    src/api/geometry/door.cpp
    src/api/geometry/railing.cpp
    src/api/geometry/window.cpp
    src/express/runtime.cpp
    src/express/compute_derived.cpp
    src/express/generated/IFC2X3_TC1.cpp
    src/express/generated/IFC4.cpp
    src/express/generated/IFC4X1.cpp
    src/express/generated/IFC4X2.cpp
    src/express/generated/IFC4X3_ADD2.cpp
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
    PRIVATE IFCAPI_BUILD IFCOPENSHELL_WITH_IFCGEOM=$<BOOL:${BUILD_IFCGEOM}>
)

# Smoke test executable for the high-level layer.
if(BUILD_TESTING AND NOT WASM_BUILD)
    add_executable(ifcapi_c_smoke_test
        tests/c_smoke_test.c
        tests/test_root.c
        tests/test_selector.c
        tests/test_pset.c
        tests/test_unit.c
    )
    target_link_libraries(ifcapi_c_smoke_test PRIVATE ifcopenshell_capi)
    target_include_directories(ifcapi_c_smoke_test PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/../ifcwrap/binding_generator/generated"
    )
    add_test(NAME ifcapi_c_smoke_test COMMAND ifcapi_c_smoke_test)
endif()
