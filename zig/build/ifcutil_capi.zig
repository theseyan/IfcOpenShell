const std = @import("std");
const emscripten = @import("emscripten.zig");

pub fn addIfcUtilCApiLibrary(
    b: *std.Build,
    target: std.Build.ResolvedTarget,
    optimize: std.builtin.OptimizeMode,
    ifcparse_capi_lib: *std.Build.Step.Compile,
) *std.Build.Step.Compile {
    // ifcparse Zig module (wraps the C API via @cImport)
    const ifcparse_mod = b.createModule(.{
        .root_source_file = b.path("zig/lib/ifcparse.zig"),
        .target = target,
        .optimize = optimize,
    });
    ifcparse_mod.addIncludePath(b.path("zig/lib"));

    // ifcutil Zig module (high-level convenience functions)
    const ifcutil_mod = b.createModule(.{
        .root_source_file = b.path("zig/lib/ifcutil.zig"),
        .target = target,
        .optimize = optimize,
    });
    ifcutil_mod.addIncludePath(b.path("zig/lib"));
    ifcutil_mod.addImport("ifcparse", ifcparse_mod);

    // Root module: the C ABI wrapper that exports functions
    const root_module = b.createModule(.{
        .root_source_file = b.path("zig/lib/ifcutil_capi.zig"),
        .target = target,
        .optimize = optimize,
    });
    root_module.addIncludePath(b.path("zig/lib"));
    root_module.addImport("ifcparse", ifcparse_mod);
    root_module.addImport("ifcutil", ifcutil_mod);

    // When targeting wasm32-emscripten, the @cImport of c_api.h needs
    // emscripten sysroot headers (for <emscripten.h>).
    if (emscripten.isEmscriptenTarget(target)) {
        emscripten.addEmscriptenSysrootModuleIncludePaths(b, ifcparse_mod);
    }

    const lib = b.addLibrary(.{
        .name = "IfcUtilC",
        .linkage = .static,
        .root_module = root_module,
    });

    lib.linkLibC();
    lib.linkLibrary(ifcparse_capi_lib);

    return lib;
}
