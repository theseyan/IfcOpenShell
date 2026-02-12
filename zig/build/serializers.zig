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
    ifcgeom_lib: *std.Build.Step.Compile,
    ifcparse_lib: *std.Build.Step.Compile,
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
    common.appendCommonCppFlags(b, &flags);
    flags.append(b.allocator, "-DSERIALIZERS_EXPORTS") catch @panic("Out of memory building Serializers flags");
    flags.append(b.allocator, "-DIFC_GEOM_EXPORTS") catch @panic("Out of memory building Serializers flags");
    flags.append(b.allocator, "-DIFOPSH_WITH_OPENCASCADE") catch @panic("Out of memory building Serializers flags");
    if (enable_with_gltf) {
        flags.append(b.allocator, "-DWITH_GLTF") catch @panic("Out of memory building Serializers flags");
    }
    flags.append(b.allocator, b.fmt("-DSCHEMA_SEQ={s}", .{schema_seq_macro})) catch @panic("Out of memory building Serializers flags");
    common.appendSchemaHasFlags(b, &flags, schemas);

    const excluded_sources_no_gltf_no_json = [_][]const u8{
        "ColladaSerializer.cpp",
        "GltfSerializer.cpp",
        "HdfSerializer.cpp",
        "JsonSerializer.cpp",
        "RocksDbSerializer.cpp",
        "USDSerializer.cpp",
    };
    const excluded_sources_no_gltf_with_json = [_][]const u8{
        "ColladaSerializer.cpp",
        "GltfSerializer.cpp",
        "HdfSerializer.cpp",
        "RocksDbSerializer.cpp",
        "USDSerializer.cpp",
    };
    const excluded_sources_with_gltf_no_json = [_][]const u8{
        "ColladaSerializer.cpp",
        "HdfSerializer.cpp",
        "JsonSerializer.cpp",
        "RocksDbSerializer.cpp",
        "USDSerializer.cpp",
    };
    const excluded_sources_with_gltf_with_json = [_][]const u8{
        "ColladaSerializer.cpp",
        "HdfSerializer.cpp",
        "RocksDbSerializer.cpp",
        "USDSerializer.cpp",
    };

    const excluded_sources = if (enable_gltf_serializer)
        if (enable_json_serializer)
            excluded_sources_with_gltf_with_json[0..]
        else
            excluded_sources_with_gltf_no_json[0..]
    else if (enable_json_serializer)
        excluded_sources_no_gltf_with_json[0..]
    else
        excluded_sources_no_gltf_no_json[0..];

    var sources = common.collectCppFilesInDirectoryExcluding(
        b,
        "src/serializers",
        false,
        excluded_sources,
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
        const serializer_obj = addSchemaSerializerObject(
            b,
            target,
            optimize,
            schemas,
            schema_seq_macro,
            schema,
            schema_sources.items,
            occ_include_dir,
            eigen_include_dir,
            enable_with_gltf,
        );
        lib.addObject(serializer_obj);
    }

    lib.linkLibrary(ifcgeom_lib);
    lib.linkLibrary(ifcparse_lib);

    return lib;
}

fn addSchemaSerializerObject(
    b: *std.Build,
    target: std.Build.ResolvedTarget,
    optimize: std.builtin.OptimizeMode,
    schemas: []const []const u8,
    schema_seq_macro: []const u8,
    schema: []const u8,
    serializer_sources: []const []const u8,
    occ_include_dir: ?[]const u8,
    eigen_include_dir: ?[]const u8,
    enable_with_gltf: bool,
) *std.Build.Step.Compile {
    const root_module = b.createModule(.{
        .target = target,
        .optimize = optimize,
    });

    const obj = b.addObject(.{
        .name = b.fmt("serializers_ifc{s}", .{schema}),
        .root_module = root_module,
    });
    obj.linkLibC();
    obj.linkLibCpp();

    deps.addSerializersIncludePaths(
        b,
        obj,
        target,
        optimize,
        occ_include_dir,
        eigen_include_dir,
        enable_with_gltf,
    );

    var flags = std.ArrayList([]const u8).empty;
    defer flags.deinit(b.allocator);
    common.appendCommonCppFlags(b, &flags);
    flags.append(b.allocator, "-DSERIALIZERS_EXPORTS") catch @panic("Out of memory building schema serializer flags");
    flags.append(b.allocator, "-DIFC_GEOM_EXPORTS") catch @panic("Out of memory building schema serializer flags");
    flags.append(b.allocator, "-DIFOPSH_WITH_OPENCASCADE") catch @panic("Out of memory building schema serializer flags");
    if (enable_with_gltf) {
        flags.append(b.allocator, "-DWITH_GLTF") catch @panic("Out of memory building schema serializer flags");
    }
    flags.append(b.allocator, b.fmt("-DSCHEMA_SEQ={s}", .{schema_seq_macro})) catch @panic("Out of memory building schema serializer flags");
    flags.append(b.allocator, b.fmt("-DIfcSchema=Ifc{s}", .{schema})) catch @panic("Out of memory building schema serializer flags");
    common.appendSchemaHasFlags(b, &flags, schemas);

    obj.addCSourceFiles(.{
        .files = serializer_sources,
        .flags = flags.items,
    });

    return obj;
}
