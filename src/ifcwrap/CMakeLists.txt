################################################################################
#                                                                              #
# This file is part of IfcOpenShell.                                           #
#                                                                              #
# IfcOpenShell is free software: you can redistribute it and/or modify         #
# it under the terms of the Lesser GNU General Public License as published by  #
# the Free Software Foundation, either version 3.0 of the License, or          #
# (at your option) any later version.                                          #
#                                                                              #
# IfcOpenShell is distributed in the hope that it will be useful,              #
# but WITHOUT ANY WARRANTY; without even the implied warranty of               #
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the                 #
# Lesser GNU General Public License for more details.                          #
#                                                                              #
# You should have received a copy of the Lesser GNU General Public License     #
# along with this program. If not, see <http://www.gnu.org/licenses/>.         #
#                                                                              #
################################################################################

if(POLICY CMP0148) # 3.27
    cmake_policy(SET CMP0148 OLD)
endif()
if(POLICY CMP0177) # 3.31
    cmake_policy(SET CMP0177 OLD)
endif()

include(GNUInstallDirs)

if(BUILD_IFCCAPI)
    if(WASM_BUILD AND NOT IFCCAPI_REGENERATE_BINDINGS)
        set(IFCCAPI_REGENERATE_BINDINGS ON CACHE BOOL "Regenerate the full IfcOpenShell binding surface during the build." FORCE)
    endif()

    set(IFCPARSE_CAPI_GENERATED_DIR "${CMAKE_CURRENT_SOURCE_DIR}/binding_generator/generated")
    set(IFCOPENSHELL_BINDING_SPEC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/binding_generator/specs")
    set(IFCOPENSHELL_BINDING_GENERATOR "${CMAKE_CURRENT_SOURCE_DIR}/binding_generator/generate.py")
    set(IFCAPI_CAPI_CPP_SPEC_MACROS "${IFCOPENSHELL_BINDING_SPEC_DIR}/cpp/spec_macros.h")
    set(UNIFIED_CAPI_HEADER "${IFCPARSE_CAPI_GENERATED_DIR}/ifcopenshell_api.h")
    set(UNIFIED_CAPI_CPP "${IFCPARSE_CAPI_GENERATED_DIR}/ifcopenshell_api.cpp")
    set(UNIFIED_CAPI_INTERNAL_HEADER "${IFCPARSE_CAPI_GENERATED_DIR}/ifcopenshell_api_internal.hpp")
    set(UNIFIED_CAPI_PYTHON_OUTPUT_DIR "${IFCPARSE_CAPI_GENERATED_DIR}")
    set(UNIFIED_CAPI_PYTHON_EXTENSION "${UNIFIED_CAPI_PYTHON_OUTPUT_DIR}/ifcopenshell_capi_py.cpp")
    set(UNIFIED_CAPI_PYTHON_UTILS "${CMAKE_CURRENT_SOURCE_DIR}/../ifcapi/python/ifcopenshell/_capi_utils.py")

    set(IFCOPENSHELL_WASM_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/wasm")
    set(IFCOPENSHELL_WASM_API_JS "${IFCOPENSHELL_WASM_OUTPUT_DIR}/ifcopenshell_api.js")
    set(IFCOPENSHELL_WASM_API_MJS "${IFCOPENSHELL_WASM_OUTPUT_DIR}/ifcopenshell_api.mjs")
    set(IFCOPENSHELL_WASM_EXPORTS "${IFCOPENSHELL_WASM_OUTPUT_DIR}/ifcopenshell_exports.txt")
    set(IFCOPENSHELL_WASM_D_TS "${IFCOPENSHELL_WASM_OUTPUT_DIR}/ifcopenshell_api.d.ts")
    set(IFCOPENSHELL_WASM_DIRECT_API_TS "${IFCOPENSHELL_WASM_OUTPUT_DIR}/ifcopenshell_api.ts")
    set(IFCOPENSHELL_JS_GENERATED_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../packages/ifcopenshell-js/src/generated")
    set(IFCOPENSHELL_JS_DIRECT_API_TS "${IFCOPENSHELL_JS_GENERATED_DIR}/ifcopenshell_api.ts")
    set(IFCOPENSHELL_JS_GENERATED_D_TS "${IFCOPENSHELL_JS_GENERATED_DIR}/ifcopenshell_api.d.ts")

    set(IFCOPENSHELL_BINDING_SPEC_DEPENDS
        "${IFCOPENSHELL_BINDING_SPEC_DIR}/ifcparse.yml"
        "${IFCOPENSHELL_BINDING_SPEC_DIR}/ifcgeom.yml"
        "${IFCOPENSHELL_BINDING_SPEC_DIR}/cpp/ifcparse.hpp"
        "${IFCOPENSHELL_BINDING_SPEC_DIR}/cpp/ifcapi.hpp"
        "${IFCOPENSHELL_BINDING_SPEC_DIR}/cpp/ifcgeom.hpp"
        "${IFCAPI_CAPI_CPP_SPEC_MACROS}"
    )

    file(
        GLOB IFCAPI_CAPI_BINDING_HEADERS
        CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/../ifcapi/include/ifcapi/bindings/*.h"
    )
    list(APPEND IFCOPENSHELL_BINDING_SPEC_DEPENDS ${IFCAPI_CAPI_BINDING_HEADERS})

    if(IFCCAPI_REGENERATE_BINDINGS)
        if(NOT ${PYTHON_EXECUTABLE} STREQUAL "")
            set(Python_EXECUTABLE "${PYTHON_EXECUTABLE}")
            message(STATUS "Looking for Python interpreter in: ${PYTHON_EXECUTABLE}")
        endif()
        find_package(Python COMPONENTS Interpreter REQUIRED)

        execute_process(
            COMMAND ${Python_EXECUTABLE} -c "import clang.cindex; import yaml"
            RESULT_VARIABLE IFCOPENSHELL_BINDGEN_PYTHON_DEPS_STATUS
            OUTPUT_QUIET
            ERROR_QUIET
        )
        if(NOT IFCOPENSHELL_BINDGEN_PYTHON_DEPS_STATUS EQUAL 0)
            message(
                FATAL_ERROR
                "IFCCAPI_REGENERATE_BINDINGS enabled, but the selected Python interpreter does not provide "
                "the Clang Python bindings and PyYAML. Install the generator dependencies or disable "
                "IFCCAPI_REGENERATE_BINDINGS."
            )
        endif()

        set(IFCPARSE_CAPI_DISCOVERY_INCLUDE_ARGS)
        foreach(include_dir IN ITEMS
            "${CMAKE_CURRENT_SOURCE_DIR}/.."
            "${CMAKE_CURRENT_SOURCE_DIR}/../plugin"
            "${CMAKE_CURRENT_SOURCE_DIR}/../ifcparse"
            "${CMAKE_CURRENT_SOURCE_DIR}/../ifcparse/schemas"
            "${CMAKE_CURRENT_SOURCE_DIR}/../ifcgeom"
            "${CMAKE_CURRENT_SOURCE_DIR}/../ifcgeom/mapping"
            "${CMAKE_CURRENT_SOURCE_DIR}/../serializers"
            "${CMAKE_CURRENT_SOURCE_DIR}/../svgfill/src"
            "${CMAKE_CURRENT_SOURCE_DIR}/../ifcapi/include"
        )
            list(APPEND IFCPARSE_CAPI_DISCOVERY_INCLUDE_ARGS --discovery-include-dir "${include_dir}")
        endforeach()
        if(WITH_OPENCASCADE)
            list(APPEND IFCPARSE_CAPI_DISCOVERY_INCLUDE_ARGS
                --discovery-include-dir "${CMAKE_CURRENT_SOURCE_DIR}/../ifcgeom/kernels/opencascade"
            )
        endif()
        if(WITH_CGAL)
            list(APPEND IFCPARSE_CAPI_DISCOVERY_INCLUDE_ARGS
                --discovery-include-dir "${CMAKE_CURRENT_SOURCE_DIR}/../ifcgeom/kernels/cgal"
            )
        endif()
        if(WITH_MANIFOLD)
            list(APPEND IFCPARSE_CAPI_DISCOVERY_INCLUDE_ARGS
                --discovery-include-dir "${CMAKE_CURRENT_SOURCE_DIR}/../ifcgeom/kernels/manifold"
            )
        endif()
        list(APPEND IFCPARSE_CAPI_DISCOVERY_INCLUDE_ARGS
            --discovery-include-dir "${CMAKE_CURRENT_SOURCE_DIR}/../ifcgeom/kernels/passthrough"
        )
        foreach(include_dir IN LISTS
            Boost_INCLUDE_DIRS
            CGAL_INCLUDE_DIR
            GMP_INCLUDE_DIR
            MPFR_INCLUDE_DIR
            OCC_INCLUDE_DIR
            OpenCASCADE_INCLUDE_DIR
            OpenCASCADE_INCLUDE_DIRS
            EIGEN3_INCLUDE_DIR
            Eigen3_INCLUDE_DIR
            Eigen3_INCLUDE_DIRS
        )
            if(NOT "${include_dir}" STREQUAL "")
                list(APPEND IFCPARSE_CAPI_DISCOVERY_INCLUDE_ARGS --discovery-include-dir "${include_dir}")
            endif()
        endforeach()
        if(OpenCASCADE_DIR)
            get_filename_component(IFCPARSE_CAPI_OPENCASCADE_PREFIX "${OpenCASCADE_DIR}/../../.." ABSOLUTE)
            if(EXISTS "${IFCPARSE_CAPI_OPENCASCADE_PREFIX}/include/opencascade")
                list(APPEND IFCPARSE_CAPI_DISCOVERY_INCLUDE_ARGS
                    --discovery-include-dir "${IFCPARSE_CAPI_OPENCASCADE_PREFIX}/include/opencascade"
                )
            endif()
        endif()
        if(Eigen3_DIR)
            get_filename_component(IFCPARSE_CAPI_EIGEN_PREFIX "${Eigen3_DIR}/../../.." ABSOLUTE)
            if(EXISTS "${IFCPARSE_CAPI_EIGEN_PREFIX}/include/eigen3")
                list(APPEND IFCPARSE_CAPI_DISCOVERY_INCLUDE_ARGS
                    --discovery-include-dir "${IFCPARSE_CAPI_EIGEN_PREFIX}/include/eigen3"
                )
            endif()
        endif()

        set(IFCPARSE_CAPI_DISCOVERY_DEFINE_ARGS)
        foreach(define IN LISTS SCHEMA_DEFINITIONS)
            list(APPEND IFCPARSE_CAPI_DISCOVERY_DEFINE_ARGS --discovery-define "${define}")
        endforeach()
        if(USE_MMAP)
            list(APPEND IFCPARSE_CAPI_DISCOVERY_DEFINE_ARGS --discovery-define USE_MMAP)
        endif()
        if(WITH_CGAL)
            list(APPEND IFCPARSE_CAPI_DISCOVERY_DEFINE_ARGS --discovery-define IFOPSH_WITH_CGAL)
        endif()
        if(WITH_OPENCASCADE)
            list(APPEND IFCPARSE_CAPI_DISCOVERY_DEFINE_ARGS --discovery-define IFOPSH_WITH_OPENCASCADE)
        endif()
        if(WITH_MANIFOLD)
            list(APPEND IFCPARSE_CAPI_DISCOVERY_DEFINE_ARGS --discovery-define IFOPSH_WITH_MANIFOLD)
        endif()
        if(WITH_ROCKSDB)
            list(APPEND IFCPARSE_CAPI_DISCOVERY_DEFINE_ARGS --discovery-define IFOPSH_WITH_ROCKSDB)
        endif()

        file(
            GLOB_RECURSE IFCOPENSHELL_BINDGEN_SOURCES
            CONFIGURE_DEPENDS
            "${CMAKE_CURRENT_SOURCE_DIR}/binding_generator/*.py"
        )
        list(FILTER IFCOPENSHELL_BINDGEN_SOURCES EXCLUDE REGEX "/generated/")
        list(FILTER IFCOPENSHELL_BINDGEN_SOURCES EXCLUDE REGEX "/wrappergen/")

        set(IFCOPENSHELL_BINDINGS_CODEGEN_STAMP "${CMAKE_CURRENT_BINARY_DIR}/ifcopenshell_bindings_codegen.stamp")
        add_custom_command(
            OUTPUT "${IFCOPENSHELL_BINDINGS_CODEGEN_STAMP}"
            BYPRODUCTS
                "${UNIFIED_CAPI_HEADER}"
                "${UNIFIED_CAPI_CPP}"
                "${UNIFIED_CAPI_INTERNAL_HEADER}"
                "${UNIFIED_CAPI_PYTHON_EXTENSION}"
                "${UNIFIED_CAPI_PYTHON_UTILS}"
                "${IFCOPENSHELL_WASM_API_JS}"
                "${IFCOPENSHELL_WASM_API_MJS}"
                "${IFCOPENSHELL_WASM_EXPORTS}"
                "${IFCOPENSHELL_WASM_D_TS}"
                "${IFCOPENSHELL_WASM_DIRECT_API_TS}"
                "${IFCOPENSHELL_JS_DIRECT_API_TS}"
                "${IFCOPENSHELL_JS_GENERATED_D_TS}"
            COMMAND
                ${Python_EXECUTABLE} -m src.ifcwrap.binding_generator.generate
                --spec-dir "${IFCOPENSHELL_BINDING_SPEC_DIR}"
                --c-output-dir "${IFCPARSE_CAPI_GENERATED_DIR}"
                --python-output-dir "${UNIFIED_CAPI_PYTHON_OUTPUT_DIR}"
                --python-utils-out "${UNIFIED_CAPI_PYTHON_UTILS}"
                --wasm-output-dir "${IFCOPENSHELL_WASM_OUTPUT_DIR}"
                --js-generated-output-dir "${IFCOPENSHELL_JS_GENERATED_DIR}"
                ${IFCPARSE_CAPI_DISCOVERY_INCLUDE_ARGS}
                ${IFCPARSE_CAPI_DISCOVERY_DEFINE_ARGS}
            COMMAND ${CMAKE_COMMAND} -E touch "${IFCOPENSHELL_BINDINGS_CODEGEN_STAMP}"
            DEPENDS
                ${IFCOPENSHELL_BINDING_SPEC_DEPENDS}
                ${IFCOPENSHELL_BINDGEN_SOURCES}
            WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../.."
            VERBATIM
        )
        add_custom_target(
            ifcopenshell_bindings_codegen
            DEPENDS "${IFCOPENSHELL_BINDINGS_CODEGEN_STAMP}"
        )
    endif()

    # Unified C API library (includes both ifcparse and ifcgeom; optionally
    # bundles the high-level handwritten layer from src/ifcapi as well).
    if((EXISTS "${UNIFIED_CAPI_HEADER}" AND EXISTS "${UNIFIED_CAPI_CPP}") OR IFCCAPI_REGENERATE_BINDINGS)
        # Build the unified C API as a SHARED library so that consumers (any
        # language: C, Zig, Rust, Python) get a single distribution
        # exposing both the autogen low-level surface and the optional
        # handwritten high-level surface, sharing the same opaque handle types.
        # A separate static archive is also produced (BUILD_IFCCAPI_STATIC)
        # for users who want to embed the autogen-only core into their own
        # executables.
        if(WASM_BUILD)
            add_library(ifcopenshell_capi STATIC "${UNIFIED_CAPI_HEADER}" "${UNIFIED_CAPI_CPP}")
        else()
            add_library(ifcopenshell_capi SHARED "${UNIFIED_CAPI_HEADER}" "${UNIFIED_CAPI_CPP}")
        endif()
        if(TARGET ifcopenshell_bindings_codegen)
            add_dependencies(ifcopenshell_capi ifcopenshell_bindings_codegen)
        endif()
        if(schema_libraries OR kernel_libraries OR mapping_libraries)
            if(WASM_BUILD)
                # Side-module plugins are loaded lazily by the WASM host executable.
            else()
                add_dependencies(ifcopenshell_capi ${schema_libraries} ${kernel_libraries} ${mapping_libraries})
            endif()
        endif()
        target_link_libraries(ifcopenshell_capi PRIVATE IfcParse)
        if(GLTF_SUPPORT)
            target_compile_definitions(ifcopenshell_capi PRIVATE WITH_GLTF)
            target_link_libraries(ifcopenshell_capi PRIVATE nlohmann_json::nlohmann_json)
        endif()
        if(TARGET IfcGeom)
            target_link_libraries(ifcopenshell_capi PRIVATE IfcGeom)
            if(NOT WASM_BUILD)
                target_link_libraries(ifcopenshell_capi PRIVATE ${OpenCASCADE_LIBRARIES})
            endif()
        endif()
        if(TARGET Serializers AND NOT WASM_BUILD)
            target_link_libraries(ifcopenshell_capi PRIVATE Serializers)
        endif()
        if(TARGET geometry_serializer AND NOT WASM_BUILD)
            target_link_libraries(ifcopenshell_capi PRIVATE geometry_serializer)
        endif()
        if(geometry_serializer_libraries AND NOT WASM_BUILD)
            target_link_libraries(ifcopenshell_capi PRIVATE ${geometry_serializer_libraries})
        endif()
        if(document_serializer_libraries AND NOT WASM_BUILD)
            target_link_libraries(ifcopenshell_capi PRIVATE ${document_serializer_libraries})
        endif()
        if(TARGET svgfill AND NOT WASM_BUILD)
            target_link_libraries(ifcopenshell_capi PRIVATE svgfill)
        endif()
        if(TARGET IFCOPENSHELL_CGAL AND NOT WASM_BUILD)
            target_link_libraries(ifcopenshell_capi PRIVATE IFCOPENSHELL_CGAL)
        endif()
        target_include_directories(
            ifcopenshell_capi
            PUBLIC "${IFCPARSE_CAPI_GENERATED_DIR}"
            PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../ifcparse"
            PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../plugin"
            PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../serializers"
            PRIVATE "${OCC_INCLUDE_DIR}"
        )
        if(BUILD_IFCGEOM)
            target_include_directories(ifcopenshell_capi PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../ifcgeom")
        endif()
        set_target_properties(ifcopenshell_capi PROPERTIES
            POSITION_INDEPENDENT_CODE ON
        )

        # Smoke tests - separate executables for each test scope
        if(BUILD_IFCGEOM AND NOT WASM_BUILD)
            add_executable(ifcopenshell_capi_smoke "${CMAKE_CURRENT_SOURCE_DIR}/tests/ifcgeom_c_api_smoke.c")
            target_link_libraries(ifcopenshell_capi_smoke PRIVATE ifcopenshell_capi)
            if(TARGET geometry_serializer)
                target_link_libraries(ifcopenshell_capi_smoke PRIVATE geometry_serializer)
            endif()
            target_include_directories(ifcopenshell_capi_smoke PRIVATE "${IFCPARSE_CAPI_GENERATED_DIR}")
            if(tree_libraries)
                add_dependencies(ifcopenshell_capi_smoke ${tree_libraries})
            endif()
        endif()

        if(NOT WASM_BUILD)
            add_executable(ifcparse_capi_smoke "${CMAKE_CURRENT_SOURCE_DIR}/tests/ifcparse_c_api_smoke.c")
            target_link_libraries(ifcparse_capi_smoke PRIVATE ifcopenshell_capi)
            if(TARGET geometry_serializer)
                target_link_libraries(ifcparse_capi_smoke PRIVATE geometry_serializer)
            endif()
            target_include_directories(ifcparse_capi_smoke PRIVATE "${IFCPARSE_CAPI_GENERATED_DIR}")
            target_compile_definitions(ifcparse_capi_smoke PRIVATE $<$<BOOL:${WITH_ROCKSDB}>:IFOPSH_WITH_ROCKSDB>)
        endif()

        # CPython C extension module (_ifcopenshell_capi)
        if(IFCCAPI_REGENERATE_BINDINGS OR EXISTS "${UNIFIED_CAPI_PYTHON_EXTENSION}")
            find_package(Python COMPONENTS Development.Module QUIET)
            if(Python_Development.Module_FOUND)
                Python_add_library(_ifcopenshell_capi MODULE "${UNIFIED_CAPI_PYTHON_EXTENSION}" WITH_SOABI)
                target_link_libraries(_ifcopenshell_capi PRIVATE ifcopenshell_capi)
                target_include_directories(_ifcopenshell_capi PRIVATE "${IFCPARSE_CAPI_GENERATED_DIR}")
                set_target_properties(_ifcopenshell_capi PROPERTIES
                    OUTPUT_NAME "_ifcopenshell_capi"
                    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../ifcapi/python/ifcopenshell"
                )
                if(TARGET ifcopenshell_bindings_codegen)
                    add_dependencies(_ifcopenshell_capi ifcopenshell_bindings_codegen)
                endif()
            else()
                message(STATUS "Python Development.Module not found - skipping _ifcopenshell_capi extension build")
            endif()
        endif()

        if(WASM_BUILD)
            include(wasm.cmake)
        endif()
    else()
        message(STATUS "Unified C API not built - generated files not found at ${UNIFIED_CAPI_HEADER}")
    endif()

    if(BUILD_TESTING)
        if(TARGET ifcopenshell_capi_smoke)
            add_test(NAME ifcopenshell_capi_smoke COMMAND ifcopenshell_capi_smoke)
        endif()
        if(TARGET ifcparse_capi_smoke)
            add_test(NAME ifcparse_capi_smoke COMMAND ifcparse_capi_smoke)
        endif()
    endif()
endif()

if(BUILD_IFCPYTHON)
find_package(SWIG)
if(NOT SWIG_FOUND)
    message(
        FATAL_ERROR
        "BUILD_IFCPYTHON enabled, but unable to find SWIG. Disable BUILD_IFCPYTHON or fix SWIG paths to proceed. "
        "Likely SWIG_EXECUTABLE is missing (current value - '${SWIG_EXECUTABLE}')."
    )
endif()
include(${SWIG_USE_FILE})

IF(NOT "${PYTHON_INCLUDE_DIR}" STREQUAL "")
    # Hint for FindPython where to find Python_INCLUDE_DIRS.
    set(Python_INCLUDE_DIR "${PYTHON_INCLUDE_DIR}")
    MESSAGE(STATUS "Looking for Python header files in: ${Python_INCLUDE_DIR}")
ENDIF()
IF(NOT "${PYTHON_LIBRARY}" STREQUAL "")
    # Hint for FindPython where to find Python_LIBRARIES.
    set(Python_LIBRARY "${PYTHON_LIBRARY}")
    MESSAGE(STATUS "Looking for Python library file in: ${Python_LIBRARY}")
ENDIF()

# Development.Module = headers on Unix, headers+libraries on Windows.
# Required variables:
# - Windows - Python_INCLUDE_DIR, Python_LIBRARY (MSVC doesn't allow undefined symbols at link time)
# - Unix - Python_INCLUDE_DIR
# Unfortunately all paths must be always provided explicitly, not by prefixing PATH.
# Otherwise FindPython will break on newer Python versions (list of supported versions is hardcoded).
find_package(Python COMPONENTS Development.Module)
IF(NOT Python_Development.Module_FOUND)
	MESSAGE(FATAL_ERROR "BUILD_IFCPYTHON enabled, but unable to find Python lib or header. Disable BUILD_IFCPYTHON or fix Python paths to proceed.")
ENDIF()

# Ensure version is saved here, from wasm libraries,
# not from Python interpreter that might be unrelated to pyodide Python version.
set(_python_libs_version "${Python_VERSION_MAJOR}${Python_VERSION_MINOR}")
INCLUDE_DIRECTORIES(BEFORE ${CMAKE_CURRENT_SOURCE_DIR})

SET(CMAKE_SWIG_FLAGS ${SWIG_DEFINES})

SET_SOURCE_FILES_PROPERTIES(IfcPython.i PROPERTIES CPLUSPLUS ON)
# Rebuild on changes in other .i files.
SET_PROPERTY(
	SOURCE IfcPython.i
	PROPERTY DEPENDS
		IfcGeomWrapper.i
		IfcParseWrapper.i
		utils/type_conversion.i
		utils/typemaps_in.i
		utils/typemaps_out.i
)

# On Windows there is '_d' prefix for debug builds - e.g. `_d.cp311-win_amd64.pyd`.
# In 4.2+ it's done automatically when discovering `Development.Module`.
if(WIN32 AND CMAKE_VERSION VERSION_LESS "4.2")
    set(Python_DEBUG_POSTFIX "_d")
endif()
# Workaround for most likely missing debug Python libraries on Windows.
# Even if user has them, it's unlikely that they want to use them for building IfcPython
# since it makes extension incompatiblew with standard Python
# and requires all other extensions to be built in debug mode as well.
# But debug mode is still might be needed - e.g. Blender's Python is using it, if you're running debug build.
if(NOT USE_DEBUG_PYTHON)
    set(Python_DEBUG_POSTFIX "")
	add_compile_definitions(SWIG_PYTHON_INTERPRETER_NO_DEBUG)
    if(Python_LIBRARY_DEBUG)
        # If debug libraries were found, then override them with release ones.
        get_target_property(IMPORTED_IMPLIB Python::Module IMPORTED_IMPLIB_RELEASE)
        get_target_property(IMPORTED_LOCATION Python::Module IMPORTED_LOCATION_RELEASE)
        set_target_properties(
            Python::Module
            PROPERTIES
            IMPORTED_IMPLIB_DEBUG "${IMPORTED_IMPLIB}"
            IMPORTED_LOCATION_DEBUG "${IMPORTED_LOCATION}"
        )
    endif()
endif()
if (CMAKE_VERSION VERSION_GREATER_EQUAL "4.2")
    # `DEBUG_POSTFIX` argument was added in 4.2.
    swig_add_library(ifcopenshell_wrapper LANGUAGE python SOURCES IfcPython.i DEBUG_POSTFIX "${Python_DEBUG_POSTFIX}")
else()
    # If we won't override -interface name,
    # swig py wrapper will try to import `_ifcopenshell_wrapper_d` module and would fail.
    set(SWIG_MODULE_ifcopenshell_wrapper_EXTRA_FLAGS "-interface" "_ifcopenshell_wrapper")
    swig_add_library(ifcopenshell_wrapper LANGUAGE python SOURCES IfcPython.i)
    set_target_properties(
        ifcopenshell_wrapper PROPERTIES
        DEBUG_POSTFIX "${Python_DEBUG_POSTFIX}"
    )
endif()

target_link_libraries(ifcopenshell_wrapper PRIVATE Python::Module)
if(WITH_CGAL)
    target_sources(ifcopenshell_wrapper PRIVATE ../svgfill/src/linework_processing_facade.cpp)
endif()
if(WITH_ROCKSDB)
    target_link_libraries(ifcopenshell_wrapper PRIVATE document_serializer_rdb)
endif()
SET_PROPERTY(TARGET ifcopenshell_wrapper PROPERTY SWIG_DEPENDS ${IFCOPENSHELL_LIBRARIES})
if (WASM_BUILD)
    # SIDE_MODULE=1 - add to .so all symbols from linked archives (default used by pyodide).
    # Since currently libIfcGeom.a seems to be linked twice it results in duplicated symbols and compilation errors.
    # Possibly in the future we can clean up linked libs and try `=1`.
    # SIDE_MODULE=2 - add to .so only explicitly exported functions (`-s EXPORTED_FUNCTIONS).
    # Have to provide it here to override possible `=1` coming from pyodide.
    if(CMAKE_GENERATOR MATCHES "Makefile" AND CMAKE_VERSION VERSION_LESS "4.2")
        # Accomodate bug in cmake makefile generator (fixed in 4.2+).
        # https://gitlab.kitware.com/cmake/cmake/-/issues/27326
        string(
            REPLACE "SIDE_MODULE=1" "SIDE_MODULE=2"
            CMAKE_MODULE_LINKER_FLAGS
            "${CMAKE_MODULE_LINKER_FLAGS}"
        )
    endif()
    target_link_options(
        ifcopenshell_wrapper
        PRIVATE "SHELL:-s EXPORTED_FUNCTIONS=_PyInit__ifcopenshell_wrapper -s SIDE_MODULE=2" -O1
    )
endif()
target_link_libraries(ifcopenshell_wrapper PRIVATE IfcGeom IfcParse ${Boost_LIBRARIES})
if(schema_libraries OR kernel_libraries OR tree_libraries OR mapping_libraries OR geometry_serializer_libraries OR document_serializer_libraries OR linework_processing_libraries)
    add_dependencies(ifcopenshell_wrapper ${schema_libraries} ${kernel_libraries} ${tree_libraries} ${mapping_libraries} ${geometry_serializer_libraries} ${document_serializer_libraries} ${linework_processing_libraries})
endif()
if(NOT WIN32)
    SET_INSTALL_RPATHS(ifcopenshell_wrapper "${IFCDIRS};${OCC_LIBRARY_DIR}")
endif()

# Try to find the Python interpreter to get the site-packages
# directory in which the wrapper can be installed.
if(NOT ${PYTHON_EXECUTABLE} STREQUAL "")
    set(Python_EXECUTABLE "${PYTHON_EXECUTABLE}")
    MESSAGE(STATUS "Looking for Python interpreter in: ${PYTHON_EXECUTABLE}")
endif()
FIND_PACKAGE(Python COMPONENTS Interpreter)
IF(Python_Interpreter_FOUND OR PYTHON_MODULE_INSTALL_DIR)
    # Python_SOABI example value is 'cp311-win_amd64'.
    if(WASM_BUILD)
        # For WASM we usually don't provide interpreter,
        # so `find_package` couldn't find `Python_SOABI`.
        # So we just hardcode it.
        # .cpython-313-wasm32-emscripten.so
        set(PYTHON_EXTENSION_SUFFIX ".cpython-${_python_libs_version}-wasm32-emscripten.so")
    elseif(WIN32)
        set(PYTHON_EXTENSION_SUFFIX ".${Python_SOABI}.pyd")
    else()
        set(PYTHON_EXTENSION_SUFFIX ".${Python_SOABI}.so")
    endif()
    set_target_properties(
        ifcopenshell_wrapper PROPERTIES
        SUFFIX ${PYTHON_EXTENSION_SUFFIX}
    )
    if (PYTHON_MODULE_INSTALL_DIR)
	set(python_package_dir "${PYTHON_MODULE_INSTALL_DIR}")
    else()
        IF (USERSPACE_PYTHON_PREFIX)
            EXECUTE_PROCESS(
                COMMAND ${PYTHON_EXECUTABLE} -c "import sys; import site; sys.stdout.write(site.USER_SITE)"
                OUTPUT_VARIABLE python_package_dir
            )
        ELSE ()
            set(python_package_dir "${Python_SITEARCH}")
        ENDIF()
        if (BUILD_PACKAGE)
            set(python_package_dir ${CMAKE_INSTALL_LIBDIR}/python${PYTHON_VERSION_MAJOR}/dist-packages/)
        endif()
    endif()
    IF("${python_package_dir}" STREQUAL "")
        MESSAGE(WARNING "Unable to locate Python site-package directory, unable to install the Python wrapper")
    ELSE()
        message(STATUS "Python wrapper will be installed to '${python_package_dir}'.")
        FILE(GLOB_RECURSE sourcefiles
            "${CMAKE_CURRENT_SOURCE_DIR}/../ifcopenshell-python/ifcopenshell/*"
        )
        FOREACH(file ${sourcefiles})
            FILE(RELATIVE_PATH relative "${CMAKE_CURRENT_SOURCE_DIR}/../ifcopenshell-python/ifcopenshell/" "${file}")
            GET_FILENAME_COMPONENT(dir "${relative}" DIRECTORY)
            if(NOT IS_DIRECTORY "${file}")
			    INSTALL(FILES "${file}"
                        DESTINATION "${python_package_dir}/ifcopenshell/${dir}")
            endif()
        ENDFOREACH()
        INSTALL(FILES "${CMAKE_BINARY_DIR}/ifcwrap/ifcopenshell_wrapper.py"
                DESTINATION "${python_package_dir}/ifcopenshell")
        INSTALL(TARGETS ifcopenshell_wrapper
                DESTINATION "${python_package_dir}/ifcopenshell")

        if (CREATE_BUNDLE)
            set(_ifcopenshell_python_runtime_targets
                IfcGeom
                IfcParse
                ${schema_libraries}
                ${kernel_libraries}
                ${tree_libraries}
                ${mapping_libraries}
                ${geometry_serializer_libraries}
                ${document_serializer_libraries}
                ${linework_processing_libraries}
                plugin
            )

            list(REMOVE_DUPLICATES _ifcopenshell_python_runtime_targets)

            if(_ifcopenshell_python_runtime_targets)
                INSTALL(TARGETS ${_ifcopenshell_python_runtime_targets}
                        LIBRARY DESTINATION "${python_package_dir}/ifcopenshell"
                        RUNTIME DESTINATION "${python_package_dir}/ifcopenshell")
            endif()

            if(NOT WIN32 AND NOT WASM_BUILD)
                set_target_properties(ifcopenshell_wrapper PROPERTIES
                    INSTALL_RPATH "$ORIGIN"
                )
            endif()
        endif()
	if (MSVC)
	    INSTALL(FILES $<TARGET_PDB_FILE:ifcopenshell_wrapper> DESTINATION bin OPTIONAL)
	endif()
    ENDIF()
ELSE()
    MESSAGE(WARNING "No Python interpreter found, unable to install the Python wrapper")
ENDIF()
endif()
