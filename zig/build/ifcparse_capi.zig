const std = @import("std");
const common = @import("common.zig");
const deps = @import("deps.zig");

pub fn addIfcParseCApiLibrary(
    b: *std.Build,
    target: std.Build.ResolvedTarget,
    optimize: std.builtin.OptimizeMode,
) *std.Build.Step.Compile {
    const root_module = b.createModule(.{
        .target = target,
        .optimize = optimize,
    });

    const lib = b.addLibrary(.{
        .name = "IfcParseC",
        .linkage = .static,
        .root_module = root_module,
    });

    lib.linkLibC();
    lib.linkLibCpp();

    lib.addIncludePath(b.path("src"));
    lib.addIncludePath(b.path("src/ifcparse"));
    lib.addIncludePath(b.path("zig/lib"));
    deps.addBoostIncludesFromDependency(b, lib, target, optimize);

    var cpp_flags = std.ArrayList([]const u8).empty;
    defer cpp_flags.deinit(b.allocator);
    common.appendCommonCppFlags(b, &cpp_flags);

    lib.addCSourceFiles(.{
        .files = &.{"zig/lib/ifcparse/c_api.cpp"},
        .flags = cpp_flags.items,
    });

    return lib;
}
