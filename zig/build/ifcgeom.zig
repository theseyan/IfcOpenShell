const std = @import("std");
const common = @import("common.zig");
const deps = @import("deps.zig");

pub fn addIfcGeomLibrary(
    b: *std.Build,
    target: std.Build.ResolvedTarget,
    optimize: std.builtin.OptimizeMode,
    schemas: []const []const u8,
    schema_seq_macro: []const u8,
) *std.Build.Step.Compile {
    const root_module = b.createModule(.{
        .target = target,
        .optimize = optimize,
    });

    const lib = b.addLibrary(.{
        .name = "IfcGeom",
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
    );

    var ifcgeom_flags = std.ArrayList([]const u8).empty;
    defer ifcgeom_flags.deinit(b.allocator);
    appendIfcGeomFlags(b, &ifcgeom_flags, target, schemas, schema_seq_macro);

    var ifcgeom_sources = common.collectCppFilesInDirectory(b, "src/ifcgeom", false);
    defer ifcgeom_sources.deinit(b.allocator);
    lib.addCSourceFiles(.{
        .files = ifcgeom_sources.items,
        .flags = ifcgeom_flags.items,
    });

    addKernelOpenCascadeSources(
        b,
        lib,
        schemas,
        schema_seq_macro,
        target,
    );

    var mapping_sources = common.collectCppFilesInDirectory(b, "src/ifcgeom/mapping", false);
    defer mapping_sources.deinit(b.allocator);
    for (schemas) |schema| {
        addSchemaMappingSources(
            b,
            lib,
            schemas,
            schema_seq_macro,
            schema,
            mapping_sources.items,
            target,
        );
    }

    var capi_flags = std.ArrayList([]const u8).empty;
    defer capi_flags.deinit(b.allocator);
    appendIfcGeomCapiFlags(b, &capi_flags, target);
    lib.addCSourceFiles(.{
        .files = &.{"zig/lib/ifcgeom/c_api.cpp"},
        .flags = capi_flags.items,
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
) struct {
    ifcparse_tests: *std.Build.Step.Compile,
    ifcutil_tests: *std.Build.Step.Compile,
    ifcgeom_tests: *std.Build.Step.Compile,
    serializers_tests: *std.Build.Step.Compile,
    all_tests: *std.Build.Step.Compile,
} {
    const ifcparse_module = b.createModule(.{
        .root_source_file = b.path("zig/lib/ifcparse.zig"),
        .target = target,
        .optimize = optimize,
    });
    ifcparse_module.addIncludePath(b.path("zig/lib"));

    const ifcutil_module = b.createModule(.{
        .root_source_file = b.path("zig/lib/ifcutil.zig"),
        .target = target,
        .optimize = optimize,
    });
    ifcutil_module.addIncludePath(b.path("zig/lib"));
    ifcutil_module.addImport("ifcparse", ifcparse_module);

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

    const ifcutil_root_module = b.createModule(.{
        .root_source_file = b.path("zig/lib/tests/ifcutil.zig"),
        .target = target,
        .optimize = optimize,
    });
    ifcutil_root_module.addImport("ifcparse", ifcparse_module);
    ifcutil_root_module.addImport("ifcutil", ifcutil_module);

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

    const all_root_module = b.createModule(.{
        .root_source_file = b.path("zig/lib/tests/main.zig"),
        .target = target,
        .optimize = optimize,
    });
    all_root_module.addImport("ifcparse", ifcparse_module);
    all_root_module.addImport("ifcutil", ifcutil_module);
    all_root_module.addImport("ifcgeom", ifcgeom_module);
    all_root_module.addImport("serializers", serializers_module);

    const ifcparse_tests = b.addTest(.{
        .name = "ifcopenshell_zig_ifcparse_tests",
        .root_module = ifcparse_root_module,
    });
    ifcparse_tests.linkLibC();
    ifcparse_tests.linkLibCpp();
    ifcparse_tests.addIncludePath(b.path("zig/lib"));
    ifcparse_tests.linkLibrary(ifcparse_lib);

    const ifcutil_tests = b.addTest(.{
        .name = "ifcopenshell_zig_ifcutil_tests",
        .root_module = ifcutil_root_module,
    });
    ifcutil_tests.linkLibC();
    ifcutil_tests.linkLibCpp();
    ifcutil_tests.addIncludePath(b.path("zig/lib"));
    ifcutil_tests.linkLibrary(ifcparse_lib);

    const ifcgeom_tests = b.addTest(.{
        .name = "ifcopenshell_zig_ifcgeom_tests",
        .root_module = ifcgeom_root_module,
    });
    ifcgeom_tests.linkLibC();
    ifcgeom_tests.linkLibCpp();
    ifcgeom_tests.addIncludePath(b.path("zig/lib"));
    ifcgeom_tests.linkLibrary(ifcparse_lib);
    ifcgeom_tests.linkLibrary(ifcgeom_lib);

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

    const all_tests = b.addTest(.{
        .name = "ifcopenshell_zig_all_tests",
        .root_module = all_root_module,
    });
    all_tests.linkLibC();
    all_tests.linkLibCpp();
    all_tests.addIncludePath(b.path("zig/lib"));
    all_tests.linkLibrary(ifcparse_lib);
    all_tests.linkLibrary(ifcgeom_lib);
    all_tests.linkLibrary(serializers_lib);

    return .{
        .ifcparse_tests = ifcparse_tests,
        .ifcutil_tests = ifcutil_tests,
        .ifcgeom_tests = ifcgeom_tests,
        .serializers_tests = serializers_tests,
        .all_tests = all_tests,
    };
}

fn appendIfcGeomFlags(
    b: *std.Build,
    flags: *std.ArrayList([]const u8),
    target: std.Build.ResolvedTarget,
    schemas: []const []const u8,
    schema_seq_macro: []const u8,
) void {
    common.appendCommonCppFlags(b, flags, target);
    flags.append(b.allocator, "-DIFC_GEOM_EXPORTS") catch @panic("Out of memory building IfcGeom flags");
    flags.append(b.allocator, "-DIFOPSH_WITH_OPENCASCADE") catch @panic("Out of memory building IfcGeom flags");
    flags.append(b.allocator, b.fmt("-DSCHEMA_SEQ={s}", .{schema_seq_macro})) catch @panic("Out of memory building IfcGeom flags");
    common.appendSchemaHasFlags(b, flags, schemas);
}

fn appendIfcGeomCapiFlags(
    b: *std.Build,
    flags: *std.ArrayList([]const u8),
    target: std.Build.ResolvedTarget,
) void {
    common.appendCommonCppFlags(b, flags, target);
    flags.append(b.allocator, "-DIFOPSH_WITH_OPENCASCADE") catch
        @panic("Out of memory building IfcGeom C API flags");
    flags.append(b.allocator, "-Wno-inconsistent-missing-override") catch
        @panic("Out of memory building IfcGeom C API flags");
    flags.append(b.allocator, "-Wno-deprecated-declarations") catch
        @panic("Out of memory building IfcGeom C API flags");
    flags.append(b.allocator, "-Wno-error=deprecated-declarations") catch
        @panic("Out of memory building IfcGeom C API flags");
    flags.append(b.allocator, "-Wno-error=unused-value") catch
        @panic("Out of memory building IfcGeom C API flags");
    flags.append(b.allocator, "-Wno-unused-value") catch
        @panic("Out of memory building IfcGeom C API flags");
}

fn addKernelOpenCascadeSources(
    b: *std.Build,
    lib: *std.Build.Step.Compile,
    schemas: []const []const u8,
    schema_seq_macro: []const u8,
    target: std.Build.ResolvedTarget,
) void {
    var flags = std.ArrayList([]const u8).empty;
    defer flags.deinit(b.allocator);
    appendIfcGeomFlags(b, &flags, target, schemas, schema_seq_macro);

    var sources = common.collectCppFilesInDirectory(b, "src/ifcgeom/kernels/opencascade", false);
    defer sources.deinit(b.allocator);
    lib.addCSourceFiles(.{
        .files = sources.items,
        .flags = flags.items,
    });
}

fn addSchemaMappingSources(
    b: *std.Build,
    lib: *std.Build.Step.Compile,
    schemas: []const []const u8,
    schema_seq_macro: []const u8,
    schema: []const u8,
    mapping_sources: []const []const u8,
    target: std.Build.ResolvedTarget,
) void {
    var flags = std.ArrayList([]const u8).empty;
    defer flags.deinit(b.allocator);
    appendIfcGeomFlags(b, &flags, target, schemas, schema_seq_macro);
    flags.append(b.allocator, b.fmt("-DIfcSchema=Ifc{s}", .{schema})) catch @panic("Out of memory building mapping flags");

    lib.addCSourceFiles(.{
        .files = mapping_sources,
        .flags = flags.items,
    });
}
