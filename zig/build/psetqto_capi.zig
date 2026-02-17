const std = @import("std");
const emscripten = @import("emscripten.zig");

pub fn addPsetQtoCApiLibrary(
    b: *std.Build,
    target: std.Build.ResolvedTarget,
    optimize: std.builtin.OptimizeMode,
    ifcparse_lib: *std.Build.Step.Compile,
    schemas: []const []const u8,
) *std.Build.Step.Compile {
    const psetqto_config = b.addOptions();
    psetqto_config.addOption(bool, "has_ifc4", hasSchema(schemas, "4"));
    psetqto_config.addOption(bool, "has_ifc2x3", hasSchema(schemas, "2x3"));
    psetqto_config.addOption(bool, "has_ifc4x3", hasSchema(schemas, "4x3_add2"));

    const ifcparse_mod = b.createModule(.{
        .root_source_file = b.path("zig/lib/ifcparse.zig"),
        .target = target,
        .optimize = optimize,
    });
    ifcparse_mod.addIncludePath(b.path("zig/lib"));

    const psetqto_mod = b.createModule(.{
        .root_source_file = b.path("zig/lib/psetqto.zig"),
        .target = target,
        .optimize = optimize,
    });
    psetqto_mod.addIncludePath(b.path("zig/lib"));
    psetqto_mod.addImport("ifcparse", ifcparse_mod);
    psetqto_mod.addImport("psetqto_config", psetqto_config.createModule());

    const root_module = b.createModule(.{
        .root_source_file = b.path("zig/lib/psetqto_capi.zig"),
        .target = target,
        .optimize = optimize,
    });
    root_module.addIncludePath(b.path("zig/lib"));
    root_module.addImport("ifcparse", ifcparse_mod);
    root_module.addImport("psetqto", psetqto_mod);

    if (emscripten.isEmscriptenTarget(target)) {
        emscripten.addEmscriptenSysrootModuleIncludePaths(b, ifcparse_mod);
    }

    const lib = b.addLibrary(.{
        .name = "PsetQtoC",
        .linkage = .static,
        .root_module = root_module,
    });

    lib.linkLibC();
    lib.linkLibrary(ifcparse_lib);

    return lib;
}

fn hasSchema(schemas: []const []const u8, needle: []const u8) bool {
    for (schemas) |s| {
        if (std.mem.eql(u8, s, needle)) return true;
    }
    return false;
}
