const std = @import("std");

const common = @import("zig/build/common.zig");
const emscripten = @import("zig/build/emscripten.zig");
const ifcparse_build = @import("zig/build/ifcparse.zig");
const ifcgeom_build = @import("zig/build/ifcgeom.zig");
const occt_build = @import("zig/build/occt.zig");
const serializers_build = @import("zig/build/serializers.zig");
const ifcutil_capi_build = @import("zig/build/ifcutil_capi.zig");
const psetqto_capi_build = @import("zig/build/psetqto_capi.zig");

const default_occ_toolkits =
    "TKernel;TKMath;TKBRep;TKGeomBase;TKGeomAlgo;TKG3d;TKG2d;TKShHealing;TKTopAlgo;TKMesh;TKPrim;TKBool;TKBO;TKFillet;TKXSBase;TKOffset";

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});
    const is_emscripten = emscripten.isEmscriptenTarget(target);
    const opt_emsdk_setup_step: ?*std.Build.Step.Run = if (is_emscripten)
        emscripten.emSdkSetupStep(b)
    else
        null;

    const schemas_arg = b.option([]const u8, "schemas", "Comma-separated IFC schema versions (4,2x3,4x3_add2)") orelse "4";
    const serializers_arg = b.option([]const u8, "serializers", "Comma-separated serializers to enable (gltf,json,svg,ttl,step,iges,all,none)") orelse "gltf,json";
    const use_mmap = b.option(bool, "use_mmap", "Enable MMAP support for IfcParse") orelse false;
    const enable_psetqto = b.option(bool, "with_psetqto", "Include PsetQto property set template lookup") orelse false;
    const package_wasm_dir = "packages/ifczero/wasm";

    var selected_serializers = common.parseCommaList(b, serializers_arg);
    defer selected_serializers.deinit(b.allocator);
    const enabled_serializers = serializers_build.EnabledSerializers.fromList(
        b,
        selected_serializers.items,
    );

    const effective_occ_toolkits = augmentOccToolkitsForSerializers(
        b,
        default_occ_toolkits,
        enabled_serializers,
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
    const ifcparse_step = b.step("ifcparse", "Build IfcParse static library");
    ifcparse_step.dependOn(&ifcparse_install.step);

    const ifcutil_capi_lib = ifcutil_capi_build.addIfcUtilCApiLibrary(
        b,
        target,
        optimize,
        ifcparse_lib,
    );
    applyEmscriptenSysroot(b, is_emscripten, opt_emsdk_setup_step, ifcutil_capi_lib, optimize);

    const opt_psetqto_capi_lib = if (enable_psetqto)
        psetqto_capi_build.addPsetQtoCApiLibrary(b, target, optimize, ifcparse_lib, schemas.items)
    else
        null;
    if (opt_psetqto_capi_lib) |lib| {
        applyEmscriptenSysroot(b, is_emscripten, opt_emsdk_setup_step, lib, optimize);
    }
    const psetqto_lib_for_tests = opt_psetqto_capi_lib orelse
        psetqto_capi_build.addPsetQtoCApiLibrary(b, target, optimize, ifcparse_lib, schemas.items);
    if (!enable_psetqto) {
        applyEmscriptenSysroot(b, is_emscripten, opt_emsdk_setup_step, psetqto_lib_for_tests, optimize);
    }

    const ifcgeom_lib = ifcgeom_build.addIfcGeomLibrary(
        b,
        target,
        optimize,
        schemas.items,
        schema_seq_macro,
    );
    for (occt_toolkit_archives) |toolkit_archive| {
        ifcgeom_lib.linkLibrary(toolkit_archive);
    }
    applyEmscriptenSysroot(b, is_emscripten, opt_emsdk_setup_step, ifcgeom_lib, optimize);
    const ifcgeom_install = b.addInstallArtifact(ifcgeom_lib, .{});
    b.getInstallStep().dependOn(&ifcgeom_install.step);
    const ifcgeom_step = b.step("ifcgeom", "Build IfcGeom static library");
    ifcgeom_step.dependOn(&ifcgeom_install.step);

    const serializers_lib = serializers_build.addSerializersLibrary(
        b,
        target,
        optimize,
        schemas.items,
        schema_seq_macro,
        enabled_serializers,
    );
    for (occt_toolkit_archives) |toolkit_archive| {
        serializers_lib.linkLibrary(toolkit_archive);
    }
    applyEmscriptenSysroot(b, is_emscripten, opt_emsdk_setup_step, serializers_lib, optimize);
    const serializers_install = b.addInstallArtifact(serializers_lib, .{});
    b.getInstallStep().dependOn(&serializers_install.step);
    const serializers_step = b.step("serializers", "Build Serializers static library");
    serializers_step.dependOn(&serializers_install.step);

    const zig_tests = ifcgeom_build.addZigLibTests(
        b,
        target,
        optimize,
        ifcparse_lib,
        ifcgeom_lib,
        serializers_lib,
    );
    const ifcparse_test_run = b.addRunArtifact(zig_tests.ifcparse_tests);
    const ifcutil_test_run = b.addRunArtifact(zig_tests.ifcutil_tests);
    const ifcgeom_test_run = b.addRunArtifact(zig_tests.ifcgeom_tests);
    const serializers_test_run = b.addRunArtifact(zig_tests.serializers_tests);
    const all_tests_run = b.addRunArtifact(zig_tests.all_tests);
    for (occt_toolkit_archives) |toolkit_archive| {
        zig_tests.ifcgeom_tests.linkLibrary(toolkit_archive);
        zig_tests.serializers_tests.linkLibrary(toolkit_archive);
        zig_tests.all_tests.linkLibrary(toolkit_archive);
    }

    const test_ifcparse_step = b.step("test-ifcparse", "Run IfcParse Zig wrapper tests");
    test_ifcparse_step.dependOn(&ifcparse_test_run.step);

    const test_ifcutil_step = b.step("test-ifcutil", "Run IfcUtil Zig wrapper tests");
    test_ifcutil_step.dependOn(&ifcutil_test_run.step);

    const test_ifcgeom_step = b.step("test-ifcgeom", "Run IfcGeom Zig wrapper tests");
    test_ifcgeom_step.dependOn(&ifcgeom_test_run.step);

    const test_serializers_step = b.step("test-serializers", "Run Serializers Zig wrapper tests");
    test_serializers_step.dependOn(&serializers_test_run.step);

    const test_step = b.step("test", "Run all Zig wrapper tests");
    test_step.dependOn(&all_tests_run.step);

    const psetqto_test_config = b.addOptions();
    psetqto_test_config.addOption(bool, "has_ifc4", common.sliceContainsString(schemas.items, "4"));
    psetqto_test_config.addOption(bool, "has_ifc2x3", common.sliceContainsString(schemas.items, "2x3"));
    psetqto_test_config.addOption(bool, "has_ifc4x3", common.sliceContainsString(schemas.items, "4x3_add2"));

    const ifcparse_mod_for_tests = b.createModule(.{
        .root_source_file = b.path("zig/lib/ifcparse.zig"),
        .target = target,
        .optimize = optimize,
    });
    ifcparse_mod_for_tests.addIncludePath(b.path("zig/lib"));

    const psetqto_mod_for_tests = b.createModule(.{
        .root_source_file = b.path("zig/lib/psetqto.zig"),
        .target = target,
        .optimize = optimize,
    });
    psetqto_mod_for_tests.addIncludePath(b.path("zig/lib"));
    psetqto_mod_for_tests.addImport("ifcparse", ifcparse_mod_for_tests);
    psetqto_mod_for_tests.addImport("psetqto_config", psetqto_test_config.createModule());

    const psetqto_test_module = b.createModule(.{
        .root_source_file = b.path("zig/lib/tests/psetqto.zig"),
        .target = target,
        .optimize = optimize,
    });
    psetqto_test_module.addImport("ifcparse", ifcparse_mod_for_tests);
    psetqto_test_module.addImport("psetqto", psetqto_mod_for_tests);

    const psetqto_tests = b.addTest(.{
        .name = "ifcopenshell_zig_psetqto_tests",
        .root_module = psetqto_test_module,
    });
    psetqto_tests.linkLibC();
    psetqto_tests.linkLibCpp();
    psetqto_tests.addIncludePath(b.path("zig/lib"));
    psetqto_tests.linkLibrary(ifcparse_lib);
    psetqto_tests.linkLibrary(psetqto_lib_for_tests);

    const psetqto_test_run = b.addRunArtifact(psetqto_tests);
    const test_psetqto_step = b.step("test-psetqto", "Run PsetQto Zig wrapper tests");
    test_psetqto_step.dependOn(&psetqto_test_run.step);
    test_step.dependOn(&psetqto_test_run.step);

    if (is_emscripten) {
        {
            var emcc_settings = emscripten.emccDefaultLibrarySettings(b, optimize);
            emcc_settings.put("EXPORT_NAME", "createIfcParseModule");

            var parse_link_libs = std.ArrayList(*std.Build.Step.Compile).empty;
            defer parse_link_libs.deinit(b.allocator);
            if (opt_psetqto_capi_lib) |lib| parse_link_libs.append(b.allocator, lib) catch @panic("OOM");
            parse_link_libs.append(b.allocator, ifcutil_capi_lib) catch @panic("OOM");
            parse_link_libs.append(b.allocator, ifcparse_lib) catch @panic("OOM");

            var parse_export_sources = std.ArrayList([]const u8).empty;
            defer parse_export_sources.deinit(b.allocator);
            parse_export_sources.appendSlice(b.allocator, &.{
                "zig/lib/ifcparse/c_api.h",
                "zig/lib/ifcutil_capi.zig",
            }) catch @panic("OOM");
            if (enable_psetqto) parse_export_sources.append(b.allocator, "zig/lib/psetqto_capi.zig") catch @panic("OOM");

            const wasm_parse_link = emscripten.emccLinkStep(
                b,
                parse_link_libs.items,
                parse_export_sources.items,
                emcc_settings,
                "ifcparse.js",
                .prefix,
            );
            if (opt_emsdk_setup_step) |setup_step| {
                wasm_parse_link.step.dependOn(&setup_step.step);
            }

            const sync_parse_outputs = b.addUpdateSourceFiles();
            sync_parse_outputs.addCopyFileToSource(
                wasm_parse_link.js,
                b.pathJoin(&.{ package_wasm_dir, "ifcparse.js" }),
            );
            sync_parse_outputs.addCopyFileToSource(
                wasm_parse_link.wasm,
                b.pathJoin(&.{ package_wasm_dir, "ifcparse.wasm" }),
            );
            sync_parse_outputs.addCopyFileToSource(
                b.path("zig/lib/psetqto/Pset_IFC2X3.ifc"),
                b.pathJoin(&.{ package_wasm_dir, "psetqto", "Pset_IFC2X3.ifc" }),
            );
            sync_parse_outputs.addCopyFileToSource(
                b.path("zig/lib/psetqto/Pset_IFC4_ADD2.ifc"),
                b.pathJoin(&.{ package_wasm_dir, "psetqto", "Pset_IFC4_ADD2.ifc" }),
            );
            sync_parse_outputs.addCopyFileToSource(
                b.path("zig/lib/psetqto/Pset_IFC4X3.ifc"),
                b.pathJoin(&.{ package_wasm_dir, "psetqto", "Pset_IFC4X3.ifc" }),
            );

            const step = b.step("wasm-parse", "Link IfcParse+IfcUtil WASM module");
            step.dependOn(&sync_parse_outputs.step);
        }

        {
            var emcc_settings = emscripten.emccDefaultLibrarySettings(b, optimize);
            emcc_settings.put("EXPORT_NAME", "createIfcOpenShellModule");

            var link_libs = std.ArrayList(*std.Build.Step.Compile).empty;
            defer link_libs.deinit(b.allocator);
            link_libs.append(b.allocator, serializers_lib) catch @panic("OOM");
            link_libs.append(b.allocator, ifcgeom_lib) catch @panic("OOM");
            link_libs.append(b.allocator, ifcutil_capi_lib) catch @panic("OOM");
            if (opt_psetqto_capi_lib) |lib| link_libs.append(b.allocator, lib) catch @panic("OOM");
            link_libs.append(b.allocator, ifcparse_lib) catch @panic("OOM");
            for (occt_toolkit_archives) |toolkit_archive| {
                link_libs.append(b.allocator, toolkit_archive) catch @panic("OOM");
            }

            var full_export_sources = std.ArrayList([]const u8).empty;
            defer full_export_sources.deinit(b.allocator);
            full_export_sources.appendSlice(b.allocator, &.{
                "zig/lib/ifcparse/c_api.h",
                "zig/lib/ifcgeom/c_api.h",
                "zig/lib/serializers/c_api.h",
                "zig/lib/ifcutil_capi.zig",
            }) catch @panic("OOM");
            if (enable_psetqto) full_export_sources.append(b.allocator, "zig/lib/psetqto_capi.zig") catch @panic("OOM");

            const wasm_full_link = emscripten.emccLinkStep(
                b,
                link_libs.items,
                full_export_sources.items,
                emcc_settings,
                "ifcopenshell.js",
                .prefix,
            );
            if (opt_emsdk_setup_step) |setup_step| {
                wasm_full_link.step.dependOn(&setup_step.step);
            }

            const sync_full_outputs = b.addUpdateSourceFiles();
            sync_full_outputs.addCopyFileToSource(
                wasm_full_link.js,
                b.pathJoin(&.{ package_wasm_dir, "ifcopenshell.js" }),
            );
            sync_full_outputs.addCopyFileToSource(
                wasm_full_link.wasm,
                b.pathJoin(&.{ package_wasm_dir, "ifcopenshell.wasm" }),
            );
            sync_full_outputs.addCopyFileToSource(
                b.path("zig/lib/psetqto/Pset_IFC2X3.ifc"),
                b.pathJoin(&.{ package_wasm_dir, "psetqto", "Pset_IFC2X3.ifc" }),
            );
            sync_full_outputs.addCopyFileToSource(
                b.path("zig/lib/psetqto/Pset_IFC4_ADD2.ifc"),
                b.pathJoin(&.{ package_wasm_dir, "psetqto", "Pset_IFC4_ADD2.ifc" }),
            );
            sync_full_outputs.addCopyFileToSource(
                b.path("zig/lib/psetqto/Pset_IFC4X3.ifc"),
                b.pathJoin(&.{ package_wasm_dir, "psetqto", "Pset_IFC4X3.ifc" }),
            );

            const step = b.step("wasm-full", "Link full IfcOpenShell WASM module");
            step.dependOn(&sync_full_outputs.step);
        }
    }
}

fn augmentOccToolkitsForSerializers(
    b: *std.Build,
    occ_toolkits_arg: []const u8,
    enabled_serializers: serializers_build.EnabledSerializers,
) []const u8 {
    var toolkits = common.parseSemicolonList(b, occ_toolkits_arg);
    defer toolkits.deinit(b.allocator);

    if (enabled_serializers.svg and !common.sliceContainsString(toolkits.items, "TKHLR")) {
        toolkits.append(b.allocator, "TKHLR") catch @panic("Out of memory appending TKHLR toolkit");
    }
    if (enabled_serializers.step and !common.sliceContainsString(toolkits.items, "TKDESTEP")) {
        toolkits.append(b.allocator, "TKDESTEP") catch @panic("Out of memory appending TKDESTEP toolkit");
    }
    if (enabled_serializers.iges and !common.sliceContainsString(toolkits.items, "TKDEIGES")) {
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

fn applyEmscriptenSysroot(
    b: *std.Build,
    is_emscripten: bool,
    opt_emsdk_setup_step: ?*std.Build.Step.Run,
    compile: *std.Build.Step.Compile,
    optimize: std.builtin.OptimizeMode,
) void {
    if (!is_emscripten) return;
    emscripten.addEmscriptenSysrootIncludePaths(b, compile);
    if (optimize != .Debug) {
        compile.lto = .full;
    }
    if (opt_emsdk_setup_step) |setup_step| {
        compile.step.dependOn(&setup_step.step);
    }
}
