const std = @import("std");
const common = @import("common.zig");
const deps = @import("deps.zig");

pub fn addSerializersCApiLibrary(
    b: *std.Build,
    target: std.Build.ResolvedTarget,
    optimize: std.builtin.OptimizeMode,
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
        .name = "IfcSerializersC",
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
        occ_include_dir,
        eigen_include_dir,
        enable_with_gltf,
    );

    var cpp_flags = std.ArrayList([]const u8).empty;
    defer cpp_flags.deinit(b.allocator);
    common.appendCommonCppFlags(b, &cpp_flags, target);
    cpp_flags.append(b.allocator, "-DIFOPSH_WITH_OPENCASCADE") catch
        @panic("Out of memory building C++ flags");
    cpp_flags.append(b.allocator, "-DSERIALIZERS_EXPORTS") catch
        @panic("Out of memory building C++ flags");
    cpp_flags.append(b.allocator, "-DIFC_GEOM_EXPORTS") catch
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
    if (enable_with_gltf) {
        cpp_flags.append(b.allocator, "-DWITH_GLTF") catch
            @panic("Out of memory building C++ flags");
    }
    if (enable_gltf_serializer) {
        cpp_flags.append(b.allocator, "-DIFCOPENSHELL_SERIALIZERS_HAS_GLTF=1") catch
            @panic("Out of memory building C++ flags");
    }
    if (enable_json_serializer) {
        cpp_flags.append(b.allocator, "-DIFCOPENSHELL_SERIALIZERS_HAS_JSON=1") catch
            @panic("Out of memory building C++ flags");
    }
    if (enable_svg_serializer) {
        cpp_flags.append(b.allocator, "-DIFCOPENSHELL_SERIALIZERS_HAS_SVG=1") catch
            @panic("Out of memory building C++ flags");
    }
    if (enable_ttl_serializer) {
        cpp_flags.append(b.allocator, "-DIFCOPENSHELL_SERIALIZERS_HAS_TTL=1") catch
            @panic("Out of memory building C++ flags");
    }
    if (enable_step_serializer) {
        cpp_flags.append(b.allocator, "-DIFCOPENSHELL_SERIALIZERS_HAS_STEP=1") catch
            @panic("Out of memory building C++ flags");
    }
    if (enable_iges_serializer) {
        cpp_flags.append(b.allocator, "-DIFCOPENSHELL_SERIALIZERS_HAS_IGES=1") catch
            @panic("Out of memory building C++ flags");
    }

    lib.addCSourceFiles(.{
        .files = &.{"zig/lib/serializers/c_api.cpp"},
        .flags = cpp_flags.items,
    });

    return lib;
}
