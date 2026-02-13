const std = @import("std");
const common = @import("common.zig");
const deps = @import("deps.zig");
const occt = @import("occt.zig");

pub fn addIfcGeomLibrary(
    b: *std.Build,
    target: std.Build.ResolvedTarget,
    optimize: std.builtin.OptimizeMode,
    schemas: []const []const u8,
    schema_seq_macro: []const u8,
    occ_include_dir: ?[]const u8,
    occ_toolkits_arg: []const u8,
    bundle_occ_sources: bool,
    eigen_include_dir: ?[]const u8,
    occ_library_dir: ?[]const u8,
    occ_libs_arg: []const u8,
    link_occ_libraries: bool,
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

    deps.addIfcGeomIncludePaths(
        b,
        lib,
        target,
        optimize,
        occ_include_dir,
        eigen_include_dir,
    );

    var ifcgeom_flags = std.ArrayList([]const u8).empty;
    defer ifcgeom_flags.deinit(b.allocator);
    common.appendCommonCppFlags(b, &ifcgeom_flags);
    ifcgeom_flags.append(b.allocator, "-DIFC_GEOM_EXPORTS") catch @panic("Out of memory building IfcGeom flags");
    ifcgeom_flags.append(b.allocator, "-DIFOPSH_WITH_OPENCASCADE") catch @panic("Out of memory building IfcGeom flags");
    ifcgeom_flags.append(b.allocator, b.fmt("-DSCHEMA_SEQ={s}", .{schema_seq_macro})) catch @panic("Out of memory building IfcGeom flags");
    common.appendSchemaHasFlags(b, &ifcgeom_flags, schemas);

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
        );
    }

    if (bundle_occ_sources) {
        occt.addOcctToolkitSources(
            b,
            lib,
            occ_toolkits_arg,
        );
    }

    if (link_occ_libraries) {
        if (occ_library_dir) |dir| {
            lib.root_module.addLibraryPath(.{ .cwd_relative = dir });
        }
        occt.linkOpenCascadeLibraries(lib, occ_libs_arg);
    }

    return lib;
}

fn addKernelOpenCascadeSources(
    b: *std.Build,
    lib: *std.Build.Step.Compile,
    schemas: []const []const u8,
    schema_seq_macro: []const u8,
) void {
    var flags = std.ArrayList([]const u8).empty;
    defer flags.deinit(b.allocator);
    common.appendCommonCppFlags(b, &flags);
    flags.append(b.allocator, "-DIFC_GEOM_EXPORTS") catch @panic("Out of memory building kernel flags");
    flags.append(b.allocator, "-DIFOPSH_WITH_OPENCASCADE") catch @panic("Out of memory building kernel flags");
    flags.append(b.allocator, b.fmt("-DSCHEMA_SEQ={s}", .{schema_seq_macro})) catch @panic("Out of memory building kernel flags");
    common.appendSchemaHasFlags(b, &flags, schemas);

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
) void {
    var flags = std.ArrayList([]const u8).empty;
    defer flags.deinit(b.allocator);
    common.appendCommonCppFlags(b, &flags);
    flags.append(b.allocator, "-DIFC_GEOM_EXPORTS") catch @panic("Out of memory building mapping flags");
    flags.append(b.allocator, "-DIFOPSH_WITH_OPENCASCADE") catch @panic("Out of memory building mapping flags");
    flags.append(b.allocator, b.fmt("-DSCHEMA_SEQ={s}", .{schema_seq_macro})) catch @panic("Out of memory building mapping flags");
    flags.append(b.allocator, b.fmt("-DIfcSchema=Ifc{s}", .{schema})) catch @panic("Out of memory building mapping flags");
    common.appendSchemaHasFlags(b, &flags, schemas);

    lib.addCSourceFiles(.{
        .files = mapping_sources,
        .flags = flags.items,
    });
}
