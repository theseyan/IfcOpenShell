const std = @import("std");
const common = @import("common.zig");
const deps = @import("deps.zig");

pub fn addIfcGeomCApiLibrary(
    b: *std.Build,
    target: std.Build.ResolvedTarget,
    optimize: std.builtin.OptimizeMode,
    occ_include_dir: ?[]const u8,
    eigen_include_dir: ?[]const u8,
    ifcgeom_lib: *std.Build.Step.Compile,
    ifcparse_capi_lib: *std.Build.Step.Compile,
) *std.Build.Step.Compile {
    const root_module = b.createModule(.{
        .target = target,
        .optimize = optimize,
    });

    const lib = b.addLibrary(.{
        .name = "IfcGeomC",
        .linkage = .static,
        .root_module = root_module,
    });

    lib.linkLibC();
    lib.linkLibCpp();

    lib.addIncludePath(b.path("zig/lib"));
    deps.addIfcGeomIncludePaths(
        b,
        lib,
        target,
        optimize,
        occ_include_dir,
        eigen_include_dir,
    );

    var cpp_flags = std.ArrayList([]const u8).empty;
    defer cpp_flags.deinit(b.allocator);
    common.appendCommonCppFlags(b, &cpp_flags);
    cpp_flags.append(b.allocator, "-DIFOPSH_WITH_OPENCASCADE") catch
        @panic("Out of memory building C++ flags");
    cpp_flags.append(b.allocator, "-Wno-inconsistent-missing-override") catch
        @panic("Out of memory building C++ flags");

    lib.addCSourceFiles(.{
        .files = &.{"zig/lib/ifcgeom/c_api.cpp"},
        .flags = cpp_flags.items,
    });

    lib.linkLibrary(ifcgeom_lib);
    lib.linkLibrary(ifcparse_capi_lib);

    return lib;
}

pub fn addZigLibTests(
    b: *std.Build,
    target: std.Build.ResolvedTarget,
    optimize: std.builtin.OptimizeMode,
    ifcparse_capi_lib: *std.Build.Step.Compile,
    ifcgeom_capi_lib: *std.Build.Step.Compile,
) *std.Build.Step.Compile {
    _ = ifcparse_capi_lib;

    const ifcparse_module = b.createModule(.{
        .root_source_file = b.path("zig/lib/ifcparse.zig"),
        .target = target,
        .optimize = optimize,
    });
    ifcparse_module.addIncludePath(b.path("zig/lib"));

    const ifcgeom_module = b.createModule(.{
        .root_source_file = b.path("zig/lib/ifcgeom.zig"),
        .target = target,
        .optimize = optimize,
    });
    ifcgeom_module.addIncludePath(b.path("zig/lib"));
    ifcgeom_module.addImport("ifcparse", ifcparse_module);

    const root_module = b.createModule(.{
        .root_source_file = b.path("zig/lib/tests/main.zig"),
        .target = target,
        .optimize = optimize,
    });
    root_module.addImport("ifcparse", ifcparse_module);
    root_module.addImport("ifcgeom", ifcgeom_module);

    const tests = b.addTest(.{
        .name = "ifcopenshell_zig_tests",
        .root_module = root_module,
    });

    tests.linkLibC();
    tests.linkLibCpp();
    tests.addIncludePath(b.path("zig/lib"));
    tests.linkLibrary(ifcgeom_capi_lib);

    return tests;
}
