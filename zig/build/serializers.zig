const std = @import("std");
const common = @import("common.zig");
const deps = @import("deps.zig");

pub fn addSerializersLibrary(
    b: *std.Build,
    target: std.Build.ResolvedTarget,
    optimize: std.builtin.OptimizeMode,
    schemas: []const []const u8,
    schema_seq_macro: []const u8,
    occ_include_dir: ?[]const u8,
    eigen_include_dir: ?[]const u8,
    enable_gltf_serializer: bool,
    enable_json_serializer: bool,
    enable_svg_serializer: bool,
    enable_ttl_serializer: bool,
    enable_step_serializer: bool,
    enable_iges_serializer: bool,
) *std.Build.Step.Compile {
    const enable_with_gltf = enable_gltf_serializer or enable_json_serializer;

    const root_module = b.createModule(.{
        .target = target,
        .optimize = optimize,
    });

    const lib = b.addLibrary(.{
        .name = "Serializers",
        .linkage = .static,
        .root_module = root_module,
    });

    lib.linkLibC();
    lib.linkLibCpp();

    deps.addSerializersIncludePaths(
        b,
        lib,
        target,
        optimize,
        occ_include_dir,
        eigen_include_dir,
        enable_with_gltf,
    );

    var flags = std.ArrayList([]const u8).empty;
    defer flags.deinit(b.allocator);
    common.appendCommonCppFlags(b, &flags, target);
    flags.append(b.allocator, "-DSERIALIZERS_EXPORTS") catch @panic("Out of memory building Serializers flags");
    flags.append(b.allocator, "-DIFC_GEOM_EXPORTS") catch @panic("Out of memory building Serializers flags");
    flags.append(b.allocator, "-DIFOPSH_WITH_OPENCASCADE") catch @panic("Out of memory building Serializers flags");
    if (enable_with_gltf) {
        flags.append(b.allocator, "-DWITH_GLTF") catch @panic("Out of memory building Serializers flags");
    }
    flags.append(b.allocator, b.fmt("-DSCHEMA_SEQ={s}", .{schema_seq_macro})) catch @panic("Out of memory building Serializers flags");
    common.appendSchemaHasFlags(b, &flags, schemas);

    var excluded_sources = std.ArrayList([]const u8).empty;
    defer excluded_sources.deinit(b.allocator);
    excluded_sources.appendSlice(b.allocator, &.{
        "ColladaSerializer.cpp",
        "HdfSerializer.cpp",
        "RocksDbSerializer.cpp",
        "USDSerializer.cpp",
    }) catch @panic("Out of memory collecting Serializers excluded sources");

    if (!enable_gltf_serializer) {
        excluded_sources.append(b.allocator, "GltfSerializer.cpp") catch
            @panic("Out of memory collecting Serializers excluded sources");
    }
    if (!enable_json_serializer) {
        excluded_sources.append(b.allocator, "JsonSerializer.cpp") catch
            @panic("Out of memory collecting Serializers excluded sources");
    }
    if (!enable_svg_serializer) {
        excluded_sources.append(b.allocator, "SvgSerializer.cpp") catch
            @panic("Out of memory collecting Serializers excluded sources");
    }
    if (!enable_ttl_serializer) {
        excluded_sources.append(b.allocator, "TtlWktSerializer.cpp") catch
            @panic("Out of memory collecting Serializers excluded sources");
    }
    if (!enable_step_serializer and !enable_iges_serializer) {
        excluded_sources.append(b.allocator, "OpenCascadeBasedSerializer.cpp") catch
            @panic("Out of memory collecting Serializers excluded sources");
    }

    var sources = common.collectCppFilesInDirectoryExcluding(
        b,
        "src/serializers",
        false,
        excluded_sources.items,
    );
    defer sources.deinit(b.allocator);

    lib.addCSourceFiles(.{
        .files = sources.items,
        .flags = flags.items,
    });

    const excluded_schema_sources_no_json = [_][]const u8{};
    const excluded_schema_sources_with_json = [_][]const u8{
        "JsonSerializer.cpp",
    };
    const excluded_schema_sources = if (enable_json_serializer)
        excluded_schema_sources_no_json[0..]
    else
        excluded_schema_sources_with_json[0..];

    var schema_sources = common.collectCppFilesInDirectoryExcluding(
        b,
        "src/serializers/schema_dependent",
        false,
        excluded_schema_sources,
    );
    defer schema_sources.deinit(b.allocator);

    for (schemas) |schema| {
        addSchemaSerializerSources(
            b,
            lib,
            schemas,
            schema_seq_macro,
            schema,
            schema_sources.items,
            enable_with_gltf,
            target,
        );
    }

    return lib;
}

fn addSchemaSerializerSources(
    b: *std.Build,
    lib: *std.Build.Step.Compile,
    schemas: []const []const u8,
    schema_seq_macro: []const u8,
    schema: []const u8,
    serializer_sources: []const []const u8,
    enable_with_gltf: bool,
    target: std.Build.ResolvedTarget,
) void {
    var flags = std.ArrayList([]const u8).empty;
    defer flags.deinit(b.allocator);
    common.appendCommonCppFlags(b, &flags, target);
    flags.append(b.allocator, "-DSERIALIZERS_EXPORTS") catch @panic("Out of memory building schema serializer flags");
    flags.append(b.allocator, "-DIFC_GEOM_EXPORTS") catch @panic("Out of memory building schema serializer flags");
    flags.append(b.allocator, "-DIFOPSH_WITH_OPENCASCADE") catch @panic("Out of memory building schema serializer flags");
    if (enable_with_gltf) {
        flags.append(b.allocator, "-DWITH_GLTF") catch @panic("Out of memory building schema serializer flags");
    }
    flags.append(b.allocator, b.fmt("-DSCHEMA_SEQ={s}", .{schema_seq_macro})) catch @panic("Out of memory building schema serializer flags");
    flags.append(b.allocator, b.fmt("-DIfcSchema=Ifc{s}", .{schema})) catch @panic("Out of memory building schema serializer flags");
    common.appendSchemaHasFlags(b, &flags, schemas);

    lib.addCSourceFiles(.{
        .files = serializer_sources,
        .flags = flags.items,
    });
}
