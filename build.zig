const std = @import("std");

const common = @import("zig/build/common.zig");
const emscripten = @import("zig/build/emscripten.zig");
const ifcparse_build = @import("zig/build/ifcparse.zig");
const ifcparse_capi_build = @import("zig/build/ifcparse_capi.zig");
const ifcgeom_build = @import("zig/build/ifcgeom.zig");
const ifcgeom_capi_build = @import("zig/build/ifcgeom_capi.zig");
const occt_build = @import("zig/build/occt.zig");
const serializers_build = @import("zig/build/serializers.zig");
const serializers_capi_build = @import("zig/build/serializers_capi.zig");
const ifcutil_capi_build = @import("zig/build/ifcutil_capi.zig");

const default_occ_libs =
    "TKernel;TKMath;TKBRep;TKGeomBase;TKGeomAlgo;TKG3d;TKG2d;TKShHealing;TKTopAlgo;TKMesh;TKPrim;TKBool;TKBO;TKFillet;TKXSBase;TKOffset;TKHLR;TKBin;TKDESTEP;TKDEIGES";
const default_occ_toolkits =
    "TKernel;TKMath;TKBRep;TKGeomBase;TKGeomAlgo;TKG3d;TKG2d;TKShHealing;TKTopAlgo;TKMesh;TKPrim;TKBool;TKBO;TKFillet;TKXSBase;TKOffset";

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});
    const is_emscripten = emscripten.isEmscriptenTarget(target);

    // For wasm32-emscripten, set up the Emscripten SDK (install + activate on first use).
    const opt_emsdk_setup_step: ?*std.Build.Step.Run = if (is_emscripten)
        emscripten.emSdkSetupStep(b)
    else
        null;

    const schemas_arg = b.option([]const u8, "schemas", "Semicolon-separated IFC schema versions (e.g. 4;2x3;4x3_add2)") orelse "4";
    const use_mmap = b.option(bool, "use_mmap", "Enable USE_MMAP support for IfcParse") orelse false;

    const occ_include_override = b.option([]const u8, "occ_include_dir", "Path to OpenCASCADE headers (directory containing Standard_Version.hxx or opencascade/Standard_Version.hxx)");
    const occ_toolkits_arg = b.option([]const u8, "occ_toolkits", "Semicolon-separated OpenCASCADE toolkits to compile as static archives and link into IfcGeom") orelse default_occ_toolkits;
    const occ_library_dir = b.option([]const u8, "occ_library_dir", "Optional path to OpenCASCADE libraries");
    const occ_libs_arg = b.option([]const u8, "occ_libs", "Semicolon-separated OpenCASCADE library names for link stage") orelse default_occ_libs;
    const link_occ_libraries = b.option(bool, "link_occ_libraries", "Link OpenCASCADE libraries into IfcGeom (off keeps compile-only path)") orelse false;

    const eigen_include_override = b.option([]const u8, "eigen_include_dir", "Path to Eigen headers (directory containing Eigen/Dense)");
    const enable_gltf_serializer = b.option(bool, "serializers_gltf", "Enable GltfSerializer support in Serializers (requires nlohmann_json)") orelse true;
    const enable_json_serializer = b.option(bool, "serializers_json", "Enable JsonSerializer support in Serializers (requires WITH_GLTF + nlohmann_json)") orelse true;
    const enable_svg_serializer = b.option(bool, "serializers_svg", "Enable SvgSerializer support in C ABI/Zig wrapper (requires TKHLR in -Docc_toolkits)") orelse false;
    const enable_ttl_serializer = b.option(bool, "serializers_ttl", "Enable TtlWktSerializer support in C ABI/Zig wrapper") orelse false;
    const enable_step_serializer = b.option(bool, "serializers_step", "Enable StepSerializer support in C ABI/Zig wrapper (requires TKDESTEP in -Docc_toolkits)") orelse false;
    const enable_iges_serializer = b.option(bool, "serializers_iges", "Enable IgesSerializer support in C ABI/Zig wrapper (requires TKDEIGES in -Docc_toolkits)") orelse false;
    const effective_occ_toolkits = augmentOccToolkitsForSerializers(
        b,
        occ_toolkits_arg,
        enable_svg_serializer,
        enable_step_serializer,
        enable_iges_serializer,
    );
    const occt_toolkit_archives: []const *std.Build.Step.Compile = occt_build.addOcctToolkitArchiveLibraries(
        b,
        target,
        optimize,
        effective_occ_toolkits,
    );
    for (occt_toolkit_archives) |toolkit_archive| {
        applyEmscriptenSysroot(b, is_emscripten, opt_emsdk_setup_step, toolkit_archive, optimize);
    }

    var schemas = std.ArrayList([]const u8).empty;
    defer schemas.deinit(b.allocator);
    common.parseSchemas(b, schemas_arg, &schemas);
    const schema_seq_macro = common.makeSchemaSeqMacro(b, schemas.items);

    const ifcparse_lib = ifcparse_build.addIfcParseLibrary(b, target, optimize, schemas.items, use_mmap);
    applyEmscriptenSysroot(b, is_emscripten, opt_emsdk_setup_step, ifcparse_lib, optimize);
    const ifcparse_install = b.addInstallArtifact(ifcparse_lib, .{});
    b.getInstallStep().dependOn(&ifcparse_install.step);

    const ifcparse_step = b.step("ifcparse", "Build the minimal IfcParse static library");
    ifcparse_step.dependOn(&ifcparse_install.step);

    const ifcparse_capi_lib = ifcparse_capi_build.addIfcParseCApiLibrary(
        b,
        target,
        optimize,
    );
    applyEmscriptenSysroot(b, is_emscripten, opt_emsdk_setup_step, ifcparse_capi_lib, optimize);
    const ifcparse_capi_install = b.addInstallArtifact(ifcparse_capi_lib, .{});
    b.getInstallStep().dependOn(&ifcparse_capi_install.step);

    const ifcparse_capi_step = b.step("ifcparse-capi", "Build C ABI shim for IfcParse used by Zig bindings");
    ifcparse_capi_step.dependOn(&ifcparse_capi_install.step);

    const ifcutil_capi_lib = ifcutil_capi_build.addIfcUtilCApiLibrary(
        b,
        target,
        optimize,
        ifcparse_capi_lib,
    );
    applyEmscriptenSysroot(b, is_emscripten, opt_emsdk_setup_step, ifcutil_capi_lib, optimize);

    const ifcgeom_lib = ifcgeom_build.addIfcGeomLibrary(
        b,
        target,
        optimize,
        schemas.items,
        schema_seq_macro,
        occ_include_override,
        eigen_include_override,
        occ_library_dir,
        occ_libs_arg,
        link_occ_libraries,
    );
    applyEmscriptenSysroot(b, is_emscripten, opt_emsdk_setup_step, ifcgeom_lib, optimize);
    const ifcgeom_install = b.addInstallArtifact(ifcgeom_lib, .{});
    b.getInstallStep().dependOn(&ifcgeom_install.step);

    const ifcgeom_step = b.step("ifcgeom", "Build IfcGeom + OpenCASCADE kernel + per-schema mapping object libraries");
    ifcgeom_step.dependOn(&ifcgeom_install.step);
    for (occt_toolkit_archives) |toolkit_archive| {
        ifcgeom_lib.linkLibrary(toolkit_archive);
    }

    const ifcgeom_capi_lib = ifcgeom_capi_build.addIfcGeomCApiLibrary(
        b,
        target,
        optimize,
        occ_include_override,
        eigen_include_override,
    );
    applyEmscriptenSysroot(b, is_emscripten, opt_emsdk_setup_step, ifcgeom_capi_lib, optimize);
    const ifcgeom_capi_install = b.addInstallArtifact(ifcgeom_capi_lib, .{});
    b.getInstallStep().dependOn(&ifcgeom_capi_install.step);

    const ifcgeom_capi_step = b.step("ifcgeom-capi", "Build C ABI shim for IfcGeom used by Zig bindings");
    ifcgeom_capi_step.dependOn(&ifcgeom_capi_install.step);
    for (occt_toolkit_archives) |toolkit_archive| {
        ifcgeom_capi_lib.linkLibrary(toolkit_archive);
    }

    const serializers_lib = serializers_build.addSerializersLibrary(
        b,
        target,
        optimize,
        schemas.items,
        schema_seq_macro,
        occ_include_override,
        eigen_include_override,
        enable_gltf_serializer,
        enable_json_serializer,
        enable_svg_serializer,
        enable_ttl_serializer,
        enable_step_serializer,
        enable_iges_serializer,
    );
    applyEmscriptenSysroot(b, is_emscripten, opt_emsdk_setup_step, serializers_lib, optimize);
    const serializers_install = b.addInstallArtifact(serializers_lib, .{});
    b.getInstallStep().dependOn(&serializers_install.step);

    const serializers_step = b.step("serializers", "Build Serializers (IfcConvert API) static library without CLI/Collada/HDF5/RocksDB");
    serializers_step.dependOn(&serializers_install.step);
    for (occt_toolkit_archives) |toolkit_archive| {
        serializers_lib.linkLibrary(toolkit_archive);
    }

    const serializers_capi_lib = serializers_capi_build.addSerializersCApiLibrary(
        b,
        target,
        optimize,
        occ_include_override,
        eigen_include_override,
        enable_gltf_serializer,
        enable_json_serializer,
        enable_svg_serializer,
        enable_ttl_serializer,
        enable_step_serializer,
        enable_iges_serializer,
    );
    applyEmscriptenSysroot(b, is_emscripten, opt_emsdk_setup_step, serializers_capi_lib, optimize);
    const serializers_capi_install = b.addInstallArtifact(serializers_capi_lib, .{});
    b.getInstallStep().dependOn(&serializers_capi_install.step);

    const serializers_capi_step = b.step("serializers-capi", "Build C ABI shim for Serializers used by Zig bindings");
    serializers_capi_step.dependOn(&serializers_capi_install.step);
    for (occt_toolkit_archives) |toolkit_archive| {
        serializers_capi_lib.linkLibrary(toolkit_archive);
    }

    const ifcgeom_zig_tests = ifcgeom_capi_build.addZigLibTests(
        b,
        target,
        optimize,
        ifcparse_lib,
        ifcgeom_lib,
        serializers_lib,
        ifcparse_capi_lib,
        ifcgeom_capi_lib,
        serializers_capi_lib,
    );
    const ifcparse_test_run = b.addRunArtifact(ifcgeom_zig_tests.ifcparse_tests);
    const ifcgeom_test_run = b.addRunArtifact(ifcgeom_zig_tests.ifcgeom_tests);
    const serializers_test_run = b.addRunArtifact(ifcgeom_zig_tests.serializers_tests);
    for (occt_toolkit_archives) |toolkit_archive| {
        ifcgeom_zig_tests.ifcgeom_tests.linkLibrary(toolkit_archive);
        ifcgeom_zig_tests.serializers_tests.linkLibrary(toolkit_archive);
    }

    const test_ifcparse_step = b.step("test-ifcparse", "Run IfcParse Zig wrapper tests");
    test_ifcparse_step.dependOn(&ifcparse_test_run.step);

    const test_ifcgeom_step = b.step("test-ifcgeom", "Run IfcGeom Zig wrapper tests");
    test_ifcgeom_step.dependOn(&ifcgeom_test_run.step);

    const test_serializers_step = b.step("test-serializers", "Run Serializers Zig wrapper tests");
    test_serializers_step.dependOn(&serializers_test_run.step);

    const test_step = b.step("test", "Run all Zig wrapper tests");
    test_step.dependOn(&ifcparse_test_run.step);
    test_step.dependOn(&ifcgeom_test_run.step);
    test_step.dependOn(&serializers_test_run.step);

    if (is_emscripten) {
        {
            var emcc_settings = emscripten.emccDefaultLibrarySettings(b, optimize);
            emcc_settings.put("EXPORT_NAME", "createIfcParseModule");
            const wasm_parse_step = emscripten.emccLinkStep(
                b,
                &.{ ifcutil_capi_lib, ifcparse_capi_lib, ifcparse_lib },
                &.{
                    "zig/lib/ifcparse/c_api.h",
                    "zig/lib/ifcutil_capi.zig",
                },
                emcc_settings,
                "ifcparse.js",
                .prefix,
            );
            if (opt_emsdk_setup_step) |setup_step| {
                wasm_parse_step.dependOn(&setup_step.step);
            }
            const step = b.step("wasm-parse", "Link IfcParse+IfcUtil WASM module");
            step.dependOn(wasm_parse_step);
        }

        {
            var emcc_settings = emscripten.emccDefaultLibrarySettings(b, optimize);
            emcc_settings.put("EXPORT_NAME", "createIfcOpenShellModule");

            var link_libs = std.ArrayList(*std.Build.Step.Compile).empty;
            defer link_libs.deinit(b.allocator);
            link_libs.append(b.allocator, serializers_capi_lib) catch @panic("OOM");
            link_libs.append(b.allocator, ifcgeom_capi_lib) catch @panic("OOM");
            link_libs.append(b.allocator, ifcparse_capi_lib) catch @panic("OOM");
            link_libs.append(b.allocator, ifcutil_capi_lib) catch @panic("OOM");
            link_libs.append(b.allocator, serializers_lib) catch @panic("OOM");
            link_libs.append(b.allocator, ifcgeom_lib) catch @panic("OOM");
            link_libs.append(b.allocator, ifcparse_lib) catch @panic("OOM");
            for (occt_toolkit_archives) |toolkit_archive| {
                link_libs.append(b.allocator, toolkit_archive) catch @panic("OOM");
            }

            const wasm_full_step = emscripten.emccLinkStep(
                b,
                link_libs.items,
                &.{
                    "zig/lib/ifcparse/c_api.h",
                    "zig/lib/ifcgeom/c_api.h",
                    "zig/lib/serializers/c_api.h",
                    "zig/lib/ifcutil_capi.zig",
                },
                emcc_settings,
                "ifcopenshell.js",
                .prefix,
            );
            if (opt_emsdk_setup_step) |setup_step| {
                wasm_full_step.dependOn(&setup_step.step);
            }
            const step = b.step("wasm-full", "Link full IfcOpenShell WASM module");
            step.dependOn(wasm_full_step);
        }
    }
}

fn augmentOccToolkitsForSerializers(
    b: *std.Build,
    occ_toolkits_arg: []const u8,
    enable_svg_serializer: bool,
    enable_step_serializer: bool,
    enable_iges_serializer: bool,
) []const u8 {
    var toolkits = common.parseSemicolonList(b, occ_toolkits_arg);
    defer toolkits.deinit(b.allocator);

    if (enable_svg_serializer and !common.sliceContainsString(toolkits.items, "TKHLR")) {
        toolkits.append(b.allocator, "TKHLR") catch @panic("Out of memory appending TKHLR toolkit");
    }
    if (enable_step_serializer and !common.sliceContainsString(toolkits.items, "TKDESTEP")) {
        toolkits.append(b.allocator, "TKDESTEP") catch @panic("Out of memory appending TKDESTEP toolkit");
    }
    if (enable_iges_serializer and !common.sliceContainsString(toolkits.items, "TKDEIGES")) {
        toolkits.append(b.allocator, "TKDEIGES") catch @panic("Out of memory appending TKDEIGES toolkit");
    }

    if (toolkits.items.len == 0) {
        return "";
    }

    var joined = std.ArrayList(u8).empty;
    for (toolkits.items, 0..) |toolkit, i| {
        if (i != 0) {
            joined.append(b.allocator, ';') catch @panic("Out of memory joining OCCT toolkit list");
        }
        joined.appendSlice(b.allocator, toolkit) catch @panic("Out of memory joining OCCT toolkit list");
    }
    return joined.toOwnedSlice(b.allocator) catch @panic("Out of memory finalizing OCCT toolkit list");
}

/// When building for wasm32-emscripten, add the Emscripten sysroot include
/// path and make the compile step depend on the one-time emsdk setup.
fn applyEmscriptenSysroot(
    b: *std.Build,
    is_emscripten: bool,
    opt_emsdk_setup_step: ?*std.Build.Step.Run,
    compile: *std.Build.Step.Compile,
    optimize: std.builtin.OptimizeMode,
) void {
    if (!is_emscripten) return;
    emscripten.addEmscriptenSysrootIncludePaths(b, compile);
    if (optimize == .ReleaseFast) {
        compile.lto = .full;
    }
    if (opt_emsdk_setup_step) |setup_step| {
        compile.step.dependOn(&setup_step.step);
    }
}
