const std = @import("std");
const common = @import("common.zig");
const deps = @import("deps.zig");

pub const EnabledSerializers = struct {
    gltf: bool = false,
    json: bool = false,
    svg: bool = false,
    ttl: bool = false,
    step: bool = false,
    iges: bool = false,

    pub fn fromList(
        b: *std.Build,
        values: []const []const u8,
    ) EnabledSerializers {
        var enabled = EnabledSerializers{};

        for (values) |value| {
            if (std.ascii.eqlIgnoreCase(value, "all")) {
                enabled = .{
                    .gltf = true,
                    .json = true,
                    .svg = true,
                    .ttl = true,
                    .step = true,
                    .iges = true,
                };
                continue;
            }
            if (std.ascii.eqlIgnoreCase(value, "none")) {
                enabled = .{};
                continue;
            }
            if (std.ascii.eqlIgnoreCase(value, "gltf")) {
                enabled.gltf = true;
                continue;
            }
            if (std.ascii.eqlIgnoreCase(value, "json")) {
                enabled.json = true;
                continue;
            }
            if (std.ascii.eqlIgnoreCase(value, "svg")) {
                enabled.svg = true;
                continue;
            }
            if (std.ascii.eqlIgnoreCase(value, "ttl")) {
                enabled.ttl = true;
                continue;
            }
            if (std.ascii.eqlIgnoreCase(value, "step")) {
                enabled.step = true;
                continue;
            }
            if (std.ascii.eqlIgnoreCase(value, "iges")) {
                enabled.iges = true;
                continue;
            }
            @panic(b.fmt(
                "Unknown serializer '{s}'. Supported values: gltf,json,svg,ttl,step,iges,all,none.",
                .{value},
            ));
        }

        return enabled;
    }

    pub fn withGltf(self: EnabledSerializers) bool {
        return self.gltf or self.json;
    }
};

pub fn addSerializersLibrary(
    b: *std.Build,
    target: std.Build.ResolvedTarget,
    optimize: std.builtin.OptimizeMode,
    schemas: []const []const u8,
    schema_seq_macro: []const u8,
    enabled_serializers: EnabledSerializers,
) *std.Build.Step.Compile {
    const enable_with_gltf = enabled_serializers.withGltf();

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

    lib.addIncludePath(b.path("zig/lib"));
    deps.addSerializersIncludePaths(
        b,
        lib,
        target,
        optimize,
        enable_with_gltf,
    );

    var flags = std.ArrayList([]const u8).empty;
    defer flags.deinit(b.allocator);
    appendSerializersFlags(
        b,
        &flags,
        target,
        schemas,
        schema_seq_macro,
        enable_with_gltf,
    );

    var excluded_sources = std.ArrayList([]const u8).empty;
    defer excluded_sources.deinit(b.allocator);
    excluded_sources.appendSlice(b.allocator, &.{
        "ColladaSerializer.cpp",
        "HdfSerializer.cpp",
        "RocksDbSerializer.cpp",
        "USDSerializer.cpp",
    }) catch @panic("Out of memory collecting Serializers excluded sources");

    if (!enabled_serializers.gltf) {
        excluded_sources.append(b.allocator, "GltfSerializer.cpp") catch
            @panic("Out of memory collecting Serializers excluded sources");
    }
    if (!enabled_serializers.json) {
        excluded_sources.append(b.allocator, "JsonSerializer.cpp") catch
            @panic("Out of memory collecting Serializers excluded sources");
    }
    if (!enabled_serializers.svg) {
        excluded_sources.append(b.allocator, "SvgSerializer.cpp") catch
            @panic("Out of memory collecting Serializers excluded sources");
    }
    if (!enabled_serializers.ttl) {
        excluded_sources.append(b.allocator, "TtlWktSerializer.cpp") catch
            @panic("Out of memory collecting Serializers excluded sources");
    }
    if (!enabled_serializers.step and !enabled_serializers.iges) {
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

    const excluded_schema_sources: []const []const u8 = if (enabled_serializers.json)
        &.{}
    else
        &.{"JsonSerializer.cpp"};
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

    var capi_flags = std.ArrayList([]const u8).empty;
    defer capi_flags.deinit(b.allocator);
    appendSerializersCapiFlags(
        b,
        &capi_flags,
        target,
        enabled_serializers,
    );
    lib.addCSourceFiles(.{
        .files = &.{"zig/lib/serializers/c_api.cpp"},
        .flags = capi_flags.items,
    });

    return lib;
}

fn appendSerializersFlags(
    b: *std.Build,
    flags: *std.ArrayList([]const u8),
    target: std.Build.ResolvedTarget,
    schemas: []const []const u8,
    schema_seq_macro: []const u8,
    enable_with_gltf: bool,
) void {
    common.appendCommonCppFlags(b, flags, target);
    flags.append(b.allocator, "-DSERIALIZERS_EXPORTS") catch @panic("Out of memory building Serializers flags");
    flags.append(b.allocator, "-DIFC_GEOM_EXPORTS") catch @panic("Out of memory building Serializers flags");
    flags.append(b.allocator, "-DIFOPSH_WITH_OPENCASCADE") catch @panic("Out of memory building Serializers flags");
    if (enable_with_gltf) {
        flags.append(b.allocator, "-DWITH_GLTF") catch @panic("Out of memory building Serializers flags");
    }
    flags.append(b.allocator, b.fmt("-DSCHEMA_SEQ={s}", .{schema_seq_macro})) catch @panic("Out of memory building Serializers flags");
    common.appendSchemaHasFlags(b, flags, schemas);
}

fn appendSerializersCapiFlags(
    b: *std.Build,
    flags: *std.ArrayList([]const u8),
    target: std.Build.ResolvedTarget,
    enabled_serializers: EnabledSerializers,
) void {
    const enable_with_gltf = enabled_serializers.withGltf();

    common.appendCommonCppFlags(b, flags, target);
    flags.append(b.allocator, "-DIFOPSH_WITH_OPENCASCADE") catch
        @panic("Out of memory building Serializers C API flags");
    flags.append(b.allocator, "-DSERIALIZERS_EXPORTS") catch
        @panic("Out of memory building Serializers C API flags");
    flags.append(b.allocator, "-DIFC_GEOM_EXPORTS") catch
        @panic("Out of memory building Serializers C API flags");
    flags.append(b.allocator, "-Wno-inconsistent-missing-override") catch
        @panic("Out of memory building Serializers C API flags");
    flags.append(b.allocator, "-Wno-deprecated-declarations") catch
        @panic("Out of memory building Serializers C API flags");
    flags.append(b.allocator, "-Wno-error=deprecated-declarations") catch
        @panic("Out of memory building Serializers C API flags");
    flags.append(b.allocator, "-Wno-error=unused-value") catch
        @panic("Out of memory building Serializers C API flags");
    flags.append(b.allocator, "-Wno-unused-value") catch
        @panic("Out of memory building Serializers C API flags");

    if (enable_with_gltf) {
        flags.append(b.allocator, "-DWITH_GLTF") catch
            @panic("Out of memory building Serializers C API flags");
    }
    if (enabled_serializers.gltf) {
        flags.append(b.allocator, "-DIFCOPENSHELL_SERIALIZERS_HAS_GLTF=1") catch
            @panic("Out of memory building Serializers C API flags");
    }
    if (enabled_serializers.json) {
        flags.append(b.allocator, "-DIFCOPENSHELL_SERIALIZERS_HAS_JSON=1") catch
            @panic("Out of memory building Serializers C API flags");
    }
    if (enabled_serializers.svg) {
        flags.append(b.allocator, "-DIFCOPENSHELL_SERIALIZERS_HAS_SVG=1") catch
            @panic("Out of memory building Serializers C API flags");
    }
    if (enabled_serializers.ttl) {
        flags.append(b.allocator, "-DIFCOPENSHELL_SERIALIZERS_HAS_TTL=1") catch
            @panic("Out of memory building Serializers C API flags");
    }
    if (enabled_serializers.step) {
        flags.append(b.allocator, "-DIFCOPENSHELL_SERIALIZERS_HAS_STEP=1") catch
            @panic("Out of memory building Serializers C API flags");
    }
    if (enabled_serializers.iges) {
        flags.append(b.allocator, "-DIFCOPENSHELL_SERIALIZERS_HAS_IGES=1") catch
            @panic("Out of memory building Serializers C API flags");
    }
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
    appendSerializersFlags(b, &flags, target, schemas, schema_seq_macro, enable_with_gltf);
    flags.append(b.allocator, b.fmt("-DIfcSchema=Ifc{s}", .{schema})) catch @panic("Out of memory building schema serializer flags");

    lib.addCSourceFiles(.{
        .files = serializer_sources,
        .flags = flags.items,
    });
}
