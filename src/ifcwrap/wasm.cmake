if(NOT WASM_BUILD)
    return()
endif()

# This file was generated with the assistance of an AI coding tool.

set(IFCOPENSHELL_WASM_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/wasm")
set(IFCOPENSHELL_WASM_PLUGINS_DIR "${IFCOPENSHELL_WASM_OUTPUT_DIR}/plugins")
set(IFCOPENSHELL_WASM_API_JS "${IFCOPENSHELL_WASM_OUTPUT_DIR}/ifcopenshell_api.js")
set(IFCOPENSHELL_WASM_API_MJS "${IFCOPENSHELL_WASM_OUTPUT_DIR}/ifcopenshell_api.mjs")
set(IFCOPENSHELL_WASM_EXPORTS "${IFCOPENSHELL_WASM_OUTPUT_DIR}/ifcopenshell_exports.txt")
set(IFCOPENSHELL_WASM_D_TS "${IFCOPENSHELL_WASM_OUTPUT_DIR}/ifcopenshell_api.d.ts")
set(IFCOPENSHELL_WASM_PLUGINS_JSON "${IFCOPENSHELL_WASM_OUTPUT_DIR}/ifcopenshell_plugins.json")
set(IFCOPENSHELL_WASM_MATH_IMPORTS_JS "${CMAKE_CURRENT_SOURCE_DIR}/wasm_math_imports.js")

add_custom_command(
    OUTPUT
        "${IFCOPENSHELL_WASM_API_JS}"
        "${IFCOPENSHELL_WASM_API_MJS}"
        "${IFCOPENSHELL_WASM_EXPORTS}"
        "${IFCOPENSHELL_WASM_D_TS}"
    COMMAND
        ${Python_EXECUTABLE} "${IFCPARSE_TARGET_GENERATOR}"
        --target wasm
        --spec-dir "${CMAKE_CURRENT_SOURCE_DIR}/binding_generator/specs"
        --api-header "${UNIFIED_CAPI_HEADER}"
        --output-dir "${IFCOPENSHELL_WASM_OUTPUT_DIR}"
        ${IFCOPENSHELL_TARGET_GENERATOR_SPEC_ARGS}
        ${IFCPARSE_CAPI_DISCOVERY_INCLUDE_ARGS}
        ${IFCPARSE_CAPI_DISCOVERY_DEFINE_ARGS}
        --module ifcopenshell
        --c-prefix ifcopenshell
    COMMAND
        ${CMAKE_COMMAND} -E copy_if_different
        "${IFCOPENSHELL_WASM_API_JS}"
        "${IFCOPENSHELL_WASM_API_MJS}"
    DEPENDS
        "${UNIFIED_CAPI_HEADER}"
        "${IFCPARSE_TARGET_GENERATOR}"
        ${IFCOPENSHELL_TARGET_SPEC_DEPENDS}
        "${IFCAPI_CAPI_CPP_SPEC_MACROS}"
        "${CMAKE_CURRENT_SOURCE_DIR}/binding_generator/targets/wasm/backend.py"
        "${CMAKE_CURRENT_SOURCE_DIR}/binding_generator/targets/wasm/js_glue.py"
        "${CMAKE_CURRENT_SOURCE_DIR}/binding_generator/targets/wasm/typescript.py"
    VERBATIM
)
add_custom_target(
    ifcopenshell_wasm_bindings
    DEPENDS
        "${IFCOPENSHELL_WASM_API_JS}"
        "${IFCOPENSHELL_WASM_API_MJS}"
        "${IFCOPENSHELL_WASM_EXPORTS}"
        "${IFCOPENSHELL_WASM_D_TS}"
)

set(IFCOPENSHELL_WASM_ENTRYPOINT "${CMAKE_CURRENT_BINARY_DIR}/ifcopenshell_wasm_entrypoint.cpp")
file(WRITE "${IFCOPENSHELL_WASM_ENTRYPOINT}"
    "// WASM main module entrypoint. Schema and geometry plugins load lazily at runtime.\n"
    "#include <setjmp.h>\n"
    "#include <emscripten.h>\n"
    "extern \"C\" void ifcopenshell_wasm_entrypoint(void) {}\n"
    "extern \"C\" EMSCRIPTEN_KEEPALIVE void ifcopenshell_wasm_keep_setjmp(void) { jmp_buf env; if (setjmp(env)) {} }\n"
)

add_executable(ifcopenshell_wasm "${IFCOPENSHELL_WASM_ENTRYPOINT}")
ifcopenshell_wasm_main_module_link(ifcopenshell_wasm)
add_dependencies(ifcopenshell_wasm ifcopenshell_capi ifcopenshell_wasm_bindings)
# Keep IfcParse symbols in the main module for SIDE_MODULE plugins that import from it.
target_link_libraries(
    ifcopenshell_wasm
    PRIVATE
        ifcopenshell_capi
        "-Wl,--whole-archive"
        IfcParse
        "-Wl,--no-whole-archive"
        plugin
)
if(TARGET IfcGeom)
    target_link_libraries(ifcopenshell_wasm PRIVATE IfcGeom)
endif()

# Collect plugin targets and manifest entries. Each plugin target copies its
# output to the plugins directory via an aggregate custom target so that the
# manifest generator only needs to write JSON (no filesystem probing).
set(ifcopenshell_wasm_plugin_entries)
set(ifcopenshell_wasm_plugin_copy_commands)
foreach(plugin_target IN LISTS schema_libraries kernel_libraries tree_libraries mapping_libraries geometry_serializer_libraries document_serializer_libraries)
    if(NOT TARGET ${plugin_target})
        continue()
    endif()
    add_dependencies(ifcopenshell_wasm ${plugin_target})

    ifcopenshell_wasm_plugin_manifest_entry(${plugin_target} plugin_entry)
    if(plugin_entry)
        list(APPEND ifcopenshell_wasm_plugin_entries ${plugin_entry})

        get_target_property(output_name ${plugin_target} OUTPUT_NAME)
        if(NOT output_name)
            set(output_name ${plugin_target})
        endif()

        list(APPEND ifcopenshell_wasm_plugin_copy_commands
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "$<TARGET_FILE:${plugin_target}>"
                "${IFCOPENSHELL_WASM_PLUGINS_DIR}/${output_name}.wasm"
        )
    endif()
endforeach()
string(REPLACE ";" "@" ifcopenshell_wasm_plugin_entries_arg "${ifcopenshell_wasm_plugin_entries}")

add_custom_target(ifcopenshell_wasm_plugins_bundle
    COMMAND ${CMAKE_COMMAND} -E make_directory "${IFCOPENSHELL_WASM_PLUGINS_DIR}"
    ${ifcopenshell_wasm_plugin_copy_commands}
    DEPENDS
        ${schema_libraries}
        ${kernel_libraries}
        ${tree_libraries}
        ${mapping_libraries}
        ${geometry_serializer_libraries}
        ${document_serializer_libraries}
    VERBATIM
)
add_dependencies(ifcopenshell_wasm ifcopenshell_wasm_plugins_bundle)

target_compile_options(ifcopenshell_wasm PRIVATE -fwasm-exceptions)
set_property(TARGET ifcopenshell_wasm APPEND PROPERTY LINK_DEPENDS "${IFCOPENSHELL_WASM_MATH_IMPORTS_JS}")

target_link_options(
    ifcopenshell_wasm
    PRIVATE
        "SHELL:--no-entry"
        "SHELL:-fwasm-exceptions"
        "SHELL:-s WASM=1"
        "SHELL:-s MAIN_MODULE=1"
        "SHELL:-s MODULARIZE=1"
        "SHELL:-s EXPORT_ES6=1"
        "SHELL:-s EXPORT_NAME=initIfcOpenShellWasmModule"
        "SHELL:-s ENVIRONMENT=web,node"
        "SHELL:--js-library=${IFCOPENSHELL_WASM_MATH_IMPORTS_JS}"
        "SHELL:-s EXPORTED_FUNCTIONS=@${IFCOPENSHELL_WASM_EXPORTS}"
        "SHELL:-s EXPORTED_RUNTIME_METHODS=[\"stringToUTF8\",\"UTF8ToString\",\"lengthBytesUTF8\",\"getValue\",\"setValue\",\"HEAP32\",\"HEAPU32\",\"ccall\",\"cwrap\",\"loadDynamicLibrary\",\"FS\"]"
        "SHELL:-s ALLOW_MEMORY_GROWTH=1"
        "SHELL:-s ALLOW_TABLE_GROWTH=1"
        "SHELL:-s AUTOLOAD_DYLIBS=0"
        "SHELL:-s ERROR_ON_UNDEFINED_SYMBOLS=0"
        "SHELL:-s WASM_BIGINT=1"
        "SHELL:-Wl,--export=__c_longjmp"
        "SHELL:-Wl,--export=__wasm_longjmp"
        "SHELL:-Wl,--export=__wasm_setjmp"
        "SHELL:-sSUPPORT_LONGJMP=wasm"
        -Oz
)
set_target_properties(ifcopenshell_wasm PROPERTIES
    OUTPUT_NAME "ifcopenshell_wasm"
    SUFFIX ".mjs"
    RUNTIME_OUTPUT_DIRECTORY "${IFCOPENSHELL_WASM_OUTPUT_DIR}"
)

# Generate the plugin manifest JSON after all plugins have been copied.
add_custom_target(ifcopenshell_wasm_plugin_manifest
    COMMAND ${CMAKE_COMMAND}
        -DIFCOPENSHELL_WASM_PLUGINS_JSON=${IFCOPENSHELL_WASM_PLUGINS_JSON}
        -DIFCOPENSHELL_WASM_PLUGIN_ENTRIES=${ifcopenshell_wasm_plugin_entries_arg}
        -P "${CMAKE_CURRENT_SOURCE_DIR}/wasm_plugins.cmake"
    DEPENDS ifcopenshell_wasm_plugins_bundle
    VERBATIM
)
add_dependencies(ifcopenshell_wasm ifcopenshell_wasm_plugin_manifest)
