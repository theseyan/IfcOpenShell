const std = @import("std");
const common = @import("common.zig");
const deps = @import("deps.zig");

pub fn addIfcGeomCApiLibrary(
    b: *std.Build,
    target: std.Build.ResolvedTarget,
    optimize: std.builtin.OptimizeMode,
    occ_include_dir: ?[]const u8,
    eigen_include_dir: ?[]const u8,
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
    cpp_flags.append(b.allocator, "-Wno-deprecated-declarations") catch
        @panic("Out of memory building C++ flags");
    cpp_flags.append(b.allocator, "-Wno-error=deprecated-declarations") catch
        @panic("Out of memory building C++ flags");
    cpp_flags.append(b.allocator, "-Wno-error=unused-value") catch
        @panic("Out of memory building C++ flags");
    cpp_flags.append(b.allocator, "-Wno-unused-value") catch
        @panic("Out of memory building C++ flags");

    lib.addCSourceFiles(.{
        .files = &.{"zig/lib/ifcgeom/c_api.cpp"},
        .flags = cpp_flags.items,
    });

    return lib;
}

pub fn addZigLibTests(
    b: *std.Build,
    target: std.Build.ResolvedTarget,
    optimize: std.builtin.OptimizeMode,
    ifcparse_lib: *std.Build.Step.Compile,
    ifcgeom_lib: *std.Build.Step.Compile,
    serializers_lib: *std.Build.Step.Compile,
    ifcparse_capi_lib: *std.Build.Step.Compile,
    ifcgeom_capi_lib: *std.Build.Step.Compile,
    ifcserializers_capi_lib: *std.Build.Step.Compile,
) struct {
    ifcparse_tests: *std.Build.Step.Compile,
    ifcgeom_tests: *std.Build.Step.Compile,
    serializers_tests: *std.Build.Step.Compile,
} {
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

    const serializers_module = b.createModule(.{
        .root_source_file = b.path("zig/lib/serializers.zig"),
        .target = target,
        .optimize = optimize,
    });
    serializers_module.addIncludePath(b.path("zig/lib"));
    serializers_module.addImport("ifcparse", ifcparse_module);
    serializers_module.addImport("ifcgeom", ifcgeom_module);

    const ifcparse_root_module = b.createModule(.{
        .root_source_file = b.path("zig/lib/tests/ifcparse.zig"),
        .target = target,
        .optimize = optimize,
    });
    ifcparse_root_module.addImport("ifcparse", ifcparse_module);

    const ifcgeom_root_module = b.createModule(.{
        .root_source_file = b.path("zig/lib/tests/ifcgeom.zig"),
        .target = target,
        .optimize = optimize,
    });
    ifcgeom_root_module.addImport("ifcparse", ifcparse_module);
    ifcgeom_root_module.addImport("ifcgeom", ifcgeom_module);

    const serializers_root_module = b.createModule(.{
        .root_source_file = b.path("zig/lib/tests/serializers.zig"),
        .target = target,
        .optimize = optimize,
    });
    serializers_root_module.addImport("ifcparse", ifcparse_module);
    serializers_root_module.addImport("ifcgeom", ifcgeom_module);
    serializers_root_module.addImport("serializers", serializers_module);

    const ifcparse_tests = b.addTest(.{
        .name = "ifcopenshell_zig_ifcparse_tests",
        .root_module = ifcparse_root_module,
    });

    ifcparse_tests.linkLibC();
    ifcparse_tests.linkLibCpp();
    ifcparse_tests.addIncludePath(b.path("zig/lib"));
    ifcparse_tests.linkLibrary(ifcparse_lib);
    ifcparse_tests.linkLibrary(ifcparse_capi_lib);

    const ifcgeom_tests = b.addTest(.{
        .name = "ifcopenshell_zig_ifcgeom_tests",
        .root_module = ifcgeom_root_module,
    });
    ifcgeom_tests.linkLibC();
    ifcgeom_tests.linkLibCpp();
    ifcgeom_tests.addIncludePath(b.path("zig/lib"));
    ifcgeom_tests.linkLibrary(ifcparse_lib);
    ifcgeom_tests.linkLibrary(ifcgeom_lib);
    ifcgeom_tests.linkLibrary(ifcparse_capi_lib);
    ifcgeom_tests.linkLibrary(ifcgeom_capi_lib);

    const serializers_tests = b.addTest(.{
        .name = "ifcopenshell_zig_serializers_tests",
        .root_module = serializers_root_module,
    });
    serializers_tests.linkLibC();
    serializers_tests.linkLibCpp();
    serializers_tests.addIncludePath(b.path("zig/lib"));
    serializers_tests.linkLibrary(ifcparse_lib);
    serializers_tests.linkLibrary(ifcgeom_lib);
    serializers_tests.linkLibrary(serializers_lib);
    serializers_tests.linkLibrary(ifcparse_capi_lib);
    serializers_tests.linkLibrary(ifcgeom_capi_lib);
    serializers_tests.linkLibrary(ifcserializers_capi_lib);

    return .{
        .ifcparse_tests = ifcparse_tests,
        .ifcgeom_tests = ifcgeom_tests,
        .serializers_tests = serializers_tests,
    };
}
