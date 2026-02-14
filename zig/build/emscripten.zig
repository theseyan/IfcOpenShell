const builtin = @import("builtin");
const std = @import("std");

/// Return the emsdk dependency, or null if it is not available.
fn getEmsdkDep(b: *std.Build) ?*std.Build.Dependency {
    return b.lazyDependency("emsdk", struct {}{});
}

/// Detect whether the resolved target is wasm32-emscripten.
pub fn isEmscriptenTarget(target: std.Build.ResolvedTarget) bool {
    return target.result.cpu.arch == .wasm32 and target.result.os.tag == .emscripten;
}

/// Add Emscripten sysroot system include paths to a compile step.
/// This is required so that C/C++ code can find standard headers
/// (assert.h, mutex, fstream, etc.) when targeting wasm32-emscripten.
pub fn addEmscriptenSysrootIncludePaths(
    b: *std.Build,
    compile: *std.Build.Step.Compile,
) void {
    const emsdk = getEmsdkDep(b) orelse
        @panic("emsdk dependency is required for wasm32-emscripten builds");

    // Use regular (-I) include paths so they take priority over zig's
    // bundled libc++ headers (which are added as -isystem).
    compile.addIncludePath(
        emSdkLazyPath(b, emsdk, &.{ "upstream", "emscripten", "cache", "sysroot", "include", "c++", "v1" }),
    );
    compile.addIncludePath(
        emSdkLazyPath(b, emsdk, &.{ "upstream", "emscripten", "cache", "sysroot", "include", "compat" }),
    );
    compile.addIncludePath(
        emSdkLazyPath(b, emsdk, &.{ "upstream", "emscripten", "cache", "sysroot", "include" }),
    );
}

/// Create an emsdk setup step (install + activate) if the SDK has not been
/// set up yet.  Returns null when the SDK is already activated.
pub fn emSdkSetupStep(b: *std.Build) ?*std.Build.Step.Run {
    const emsdk = getEmsdkDep(b) orelse
        @panic("emsdk dependency is required for wasm32-emscripten builds");

    const dot_emsc_path = emSdkLazyPath(b, emsdk, &.{".emscripten"}).getPath(b);
    const dot_emsc_exists = !std.meta.isError(std.fs.cwd().access(dot_emsc_path, .{}));

    if (!dot_emsc_exists) {
        const emsdk_install = createEmsdkCommand(b, emsdk);
        emsdk_install.addArgs(&.{ "install", "latest" });

        const emsdk_activate = createEmsdkCommand(b, emsdk);
        emsdk_activate.addArgs(&.{ "activate", "latest" });
        emsdk_activate.step.dependOn(&emsdk_install.step);

        return emsdk_activate;
    }

    return null;
}

/// Get the path to the `emcc` front-end inside the cached emsdk.
pub fn emccPath(b: *std.Build) []const u8 {
    const emsdk = getEmsdkDep(b) orelse
        @panic("emsdk dependency is required for wasm32-emscripten builds");
    return emSdkLazyPath(b, emsdk, &.{ "upstream", "emscripten", "emcc" }).getPath(b);
}

/// Return the C++ include path inside the Emscripten sysroot (as a string).
pub fn emSdkSysrootCxxIncludePath(b: *std.Build) []const u8 {
    const emsdk = getEmsdkDep(b) orelse
        @panic("emsdk dependency is required for wasm32-emscripten builds");
    return emSdkLazyPath(b, emsdk, &.{ "upstream", "emscripten", "cache", "sysroot", "include", "c++", "v1" }).getPath(b);
}

/// Return the C include path inside the Emscripten sysroot (as a string).
pub fn emSdkSysrootIncludePath(b: *std.Build) []const u8 {
    const emsdk = getEmsdkDep(b) orelse
        @panic("emsdk dependency is required for wasm32-emscripten builds");
    return emSdkLazyPath(b, emsdk, &.{ "upstream", "emscripten", "cache", "sysroot", "include" }).getPath(b);
}

/// Return the compat include path inside the Emscripten sysroot (as a string).
pub fn emSdkSysrootCompatIncludePath(b: *std.Build) []const u8 {
    const emsdk = getEmsdkDep(b) orelse
        @panic("emsdk dependency is required for wasm32-emscripten builds");
    return emSdkLazyPath(b, emsdk, &.{ "upstream", "emscripten", "cache", "sysroot", "include", "compat" }).getPath(b);
}

// ------------------------------------------------------------------
// Internal helpers
// ------------------------------------------------------------------

fn emSdkLazyPath(b: *std.Build, emsdk: *std.Build.Dependency, sub_paths: []const []const u8) std.Build.LazyPath {
    return emsdk.path(b.pathJoin(sub_paths));
}

fn createEmsdkCommand(b: *std.Build, emsdk: *std.Build.Dependency) *std.Build.Step.Run {
    if (builtin.os.tag == .windows) {
        return b.addSystemCommand(&.{emSdkLazyPath(b, emsdk, &.{"emsdk.bat"}).getPath(b)});
    } else {
        const step = b.addSystemCommand(&.{"bash"});
        step.addArg(emSdkLazyPath(b, emsdk, &.{"emsdk"}).getPath(b));
        return step;
    }
}
