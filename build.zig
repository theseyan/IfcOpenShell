const std = @import("std");

const common = @import("zig/build/common.zig");
const ifcparse_build = @import("zig/build/ifcparse.zig");
const ifcparse_capi_build = @import("zig/build/ifcparse_capi.zig");
const ifcgeom_build = @import("zig/build/ifcgeom.zig");
const serializers_build = @import("zig/build/serializers.zig");

const default_occ_libs =
    "TKernel;TKMath;TKBRep;TKGeomBase;TKGeomAlgo;TKG3d;TKG2d;TKShHealing;TKTopAlgo;TKMesh;TKPrim;TKBool;TKBO;TKFillet;TKXSBase;TKOffset;TKHLR;TKBin;TKDESTEP;TKDEIGES";
const default_occ_toolkits =
    "TKernel;TKMath;TKBRep;TKGeomBase;TKGeomAlgo;TKG3d;TKG2d;TKShHealing;TKTopAlgo;TKMesh;TKPrim;TKBool;TKBO;TKFillet;TKXSBase;TKOffset;TKHLR";

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    const schemas_arg = b.option([]const u8, "schemas", "Semicolon-separated IFC schema versions (e.g. 4;2x3;4x3_add2)") orelse "4";
    const use_mmap = b.option(bool, "use_mmap", "Enable USE_MMAP support for IfcParse") orelse false;

    const occ_include_override = b.option([]const u8, "occ_include_dir", "Path to OpenCASCADE headers (directory containing Standard_Version.hxx or opencascade/Standard_Version.hxx)");
    const occ_toolkits_arg = b.option([]const u8, "occ_toolkits", "Semicolon-separated OpenCASCADE toolkits to compile and bundle into IfcGeom") orelse default_occ_toolkits;
    const bundle_occ_sources = b.option(bool, "bundle_occ_sources", "Compile OCCT toolkit sources and bundle into IfcGeom static archive") orelse true;
    const occ_library_dir = b.option([]const u8, "occ_library_dir", "Optional path to OpenCASCADE libraries");
    const occ_libs_arg = b.option([]const u8, "occ_libs", "Semicolon-separated OpenCASCADE library names for link stage") orelse default_occ_libs;
    const link_occ_libraries = b.option(bool, "link_occ_libraries", "Link OpenCASCADE libraries into IfcGeom (off keeps compile-only path)") orelse false;

    const eigen_include_override = b.option([]const u8, "eigen_include_dir", "Path to Eigen headers (directory containing Eigen/Dense)");
    const enable_gltf_serializer = b.option(bool, "serializers_gltf", "Enable GltfSerializer support in Serializers (requires nlohmann_json)") orelse true;
    const enable_json_serializer = b.option(bool, "serializers_json", "Enable JsonSerializer support in Serializers (requires WITH_GLTF + nlohmann_json)") orelse true;

    var schemas = std.ArrayList([]const u8).empty;
    defer schemas.deinit(b.allocator);
    common.parseSchemas(b, schemas_arg, &schemas);
    const schema_seq_macro = common.makeSchemaSeqMacro(b, schemas.items);

    const ifcparse_lib = ifcparse_build.addIfcParseLibrary(b, target, optimize, schemas.items, use_mmap);
    const ifcparse_install = b.addInstallArtifact(ifcparse_lib, .{});
    b.getInstallStep().dependOn(&ifcparse_install.step);

    const ifcparse_step = b.step("ifcparse", "Build the minimal IfcParse static library");
    ifcparse_step.dependOn(&ifcparse_install.step);

    const ifcparse_capi_lib = ifcparse_capi_build.addIfcParseCApiLibrary(
        b,
        target,
        optimize,
        ifcparse_lib,
    );
    const ifcparse_capi_install = b.addInstallArtifact(ifcparse_capi_lib, .{});
    b.getInstallStep().dependOn(&ifcparse_capi_install.step);

    const ifcparse_capi_step = b.step("ifcparse-capi", "Build C ABI shim for IfcParse used by Zig bindings");
    ifcparse_capi_step.dependOn(&ifcparse_capi_install.step);

    const zig_lib_tests = ifcparse_capi_build.addZigLibTests(
        b,
        target,
        optimize,
        ifcparse_capi_lib,
    );
    const zig_lib_test_run = b.addRunArtifact(zig_lib_tests);
    const test_step = b.step("test", "Run all Zig wrapper tests");
    test_step.dependOn(&zig_lib_test_run.step);

    const ifcgeom_lib = ifcgeom_build.addIfcGeomLibrary(
        b,
        target,
        optimize,
        schemas.items,
        schema_seq_macro,
        occ_include_override,
        occ_toolkits_arg,
        bundle_occ_sources,
        eigen_include_override,
        occ_library_dir,
        occ_libs_arg,
        link_occ_libraries,
        ifcparse_lib,
    );
    const ifcgeom_install = b.addInstallArtifact(ifcgeom_lib, .{});
    b.getInstallStep().dependOn(&ifcgeom_install.step);

    const ifcgeom_step = b.step("ifcgeom", "Build IfcGeom + OpenCASCADE kernel + per-schema mapping object libraries");
    ifcgeom_step.dependOn(&ifcgeom_install.step);

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
        ifcgeom_lib,
        ifcparse_lib,
    );
    const serializers_install = b.addInstallArtifact(serializers_lib, .{});
    b.getInstallStep().dependOn(&serializers_install.step);

    const serializers_step = b.step("serializers", "Build Serializers (IfcConvert API) static library without CLI/Collada/HDF5/RocksDB");
    serializers_step.dependOn(&serializers_install.step);
}
