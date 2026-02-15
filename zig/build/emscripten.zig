const builtin = @import("builtin");
const std = @import("std");

fn getEmsdkDep(b: *std.Build) ?*std.Build.Dependency {
    return b.lazyDependency("emsdk", struct {}{});
}

pub fn isEmscriptenTarget(target: std.Build.ResolvedTarget) bool {
    return target.result.cpu.arch == .wasm32 and target.result.os.tag == .emscripten;
}

pub fn addEmscriptenSysrootIncludePaths(
    b: *std.Build,
    compile: *std.Build.Step.Compile,
) void {
    addEmscriptenSysrootModuleIncludePaths(b, compile.root_module);
}

pub fn addEmscriptenSysrootModuleIncludePaths(
    b: *std.Build,
    module: *std.Build.Module,
) void {
    const emsdk = getEmsdkDep(b) orelse
        @panic("emsdk dependency is required for wasm32-emscripten builds");

    module.addIncludePath(
        emSdkLazyPath(b, emsdk, &.{ "upstream", "emscripten", "cache", "sysroot", "include", "c++", "v1" }),
    );
    module.addIncludePath(
        emSdkLazyPath(b, emsdk, &.{ "upstream", "emscripten", "cache", "sysroot", "include", "compat" }),
    );
    module.addIncludePath(
        emSdkLazyPath(b, emsdk, &.{ "upstream", "emscripten", "cache", "sysroot", "include" }),
    );
}

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

pub fn emccPath(b: *std.Build) []const u8 {
    const emsdk = getEmsdkDep(b) orelse
        @panic("emsdk dependency is required for wasm32-emscripten builds");
    return emSdkLazyPath(b, emsdk, &.{ "upstream", "emscripten", "emcc" }).getPath(b);
}

pub fn emSdkSysrootCxxIncludePath(b: *std.Build) []const u8 {
    const emsdk = getEmsdkDep(b) orelse
        @panic("emsdk dependency is required for wasm32-emscripten builds");
    return emSdkLazyPath(b, emsdk, &.{ "upstream", "emscripten", "cache", "sysroot", "include", "c++", "v1" }).getPath(b);
}

pub fn emSdkSysrootIncludePath(b: *std.Build) []const u8 {
    const emsdk = getEmsdkDep(b) orelse
        @panic("emsdk dependency is required for wasm32-emscripten builds");
    return emSdkLazyPath(b, emsdk, &.{ "upstream", "emscripten", "cache", "sysroot", "include" }).getPath(b);
}

pub fn emSdkSysrootCompatIncludePath(b: *std.Build) []const u8 {
    const emsdk = getEmsdkDep(b) orelse
        @panic("emsdk dependency is required for wasm32-emscripten builds");
    return emSdkLazyPath(b, emsdk, &.{ "upstream", "emscripten", "cache", "sysroot", "include", "compat" }).getPath(b);
}

pub const EmccSettings = struct {
    settings: std.StringHashMap([]const u8),
    flags: std.ArrayList([]const u8),

    pub fn init(allocator: std.mem.Allocator) EmccSettings {
        return .{
            .settings = std.StringHashMap([]const u8).init(allocator),
            .flags = std.ArrayList([]const u8).empty,
        };
    }

    pub fn put(self: *EmccSettings, key: []const u8, value: []const u8) void {
        self.settings.put(key, value) catch @panic("OOM");
    }

    pub fn addFlag(self: *EmccSettings, allocator: std.mem.Allocator, flag: []const u8) void {
        self.flags.append(allocator, flag) catch @panic("OOM");
    }
};

pub fn emccDefaultLibrarySettings(
    b: *std.Build,
    optimize: std.builtin.OptimizeMode,
) EmccSettings {
    var s = EmccSettings.init(b.allocator);

    switch (optimize) {
        .Debug => {
            s.addFlag(b.allocator, "-O0");
            s.addFlag(b.allocator, "-g");
        },
        .ReleaseSafe => s.addFlag(b.allocator, "-O2"),
        .ReleaseFast => {
            s.addFlag(b.allocator, "-O3");
            s.addFlag(b.allocator, "--lto");
        },
        .ReleaseSmall => s.addFlag(b.allocator, "-Oz"),
    }

    s.put("MODULARIZE", "1");
    s.put("EXPORT_ES6", "1");
    s.put("EXPORT_NAME", "createModule");
    s.put("ALLOW_MEMORY_GROWTH", "1");
    s.put("INITIAL_MEMORY", "33554432");
    s.put("EXPORTED_RUNTIME_METHODS", "['cwrap','ccall','getValue','setValue','UTF8ToString','stringToUTF8','lengthBytesUTF8','stackAlloc','stackSave','stackRestore','HEAPU8','FS']");
    // Emscripten JS-based exception handling (pairs with -mllvm
    // -enable-emscripten-cxx-exceptions on the compiler side in common.zig).
    s.put("DISABLE_EXCEPTION_CATCHING", "0");
    s.addFlag(b.allocator, "--no-entry");
    s.put("ENVIRONMENT", "web,node,worker");

    return s;
}

/// Link Zig-compiled static libraries into a .wasm + .js pair via emcc.
/// Extracts `ifcopenshell_*` symbols from `c_api_headers` for EXPORTED_FUNCTIONS.
pub fn emccLinkStep(
    b: *std.Build,
    compile_steps: []const *std.Build.Step.Compile,
    c_api_headers: []const []const u8,
    settings: EmccSettings,
    out_file_name: []const u8,
    install_dir: std.Build.InstallDir,
) *std.Build.Step {
    const emcc = b.addSystemCommand(&.{emccPath(b)});

    for (settings.flags.items) |flag| {
        emcc.addArg(flag);
    }

    var iter = settings.settings.iterator();
    while (iter.next()) |entry| {
        emcc.addArg(b.fmt("-s{s}={s}", .{ entry.key_ptr.*, entry.value_ptr.* }));
    }

    {
        const alloc = b.allocator;
        var exported: std.ArrayList(u8) = .empty;
        exported.appendSlice(alloc, "['_malloc','_free'") catch @panic("OOM");

        for (c_api_headers) |header_path| {
            const content = std.fs.cwd().readFileAlloc(alloc, header_path, 1024 * 1024) catch
                @panic("failed to read C API header for export generation");

            const is_zig = std.mem.endsWith(u8, header_path, ".zig");

            var line_iter = std.mem.splitScalar(u8, content, '\n');
            while (line_iter.next()) |line| {
                const trimmed = std.mem.trim(u8, line, " \t\r");
                const needle = "ifcopenshell_";

                if (is_zig) {
                    if (!std.mem.startsWith(u8, trimmed, "export fn ")) continue;
                    const after_fn = trimmed["export fn ".len..];
                    const fn_paren = std.mem.indexOf(u8, after_fn, "(") orelse continue;
                    const fn_name = after_fn[0..fn_paren];
                    if (!std.mem.startsWith(u8, fn_name, needle)) continue;
                    exported.appendSlice(alloc, ",'_") catch @panic("OOM");
                    exported.appendSlice(alloc, fn_name) catch @panic("OOM");
                    exported.append(alloc, '\'') catch @panic("OOM");
                } else {
                    const paren = std.mem.indexOf(u8, trimmed, "(") orelse continue;
                    const prefix_pos = std.mem.indexOf(u8, trimmed, needle) orelse continue;
                    if (prefix_pos >= paren) continue;

                    var end = paren;
                    while (end > prefix_pos and trimmed[end - 1] == ' ') end -= 1;
                    const func_name = trimmed[prefix_pos..end];

                    var valid = true;
                    for (func_name) |ch| {
                        if (!std.ascii.isAlphanumeric(ch) and ch != '_') {
                            valid = false;
                            break;
                        }
                    }
                    if (!valid or func_name.len == 0) continue;

                    exported.appendSlice(alloc, ",'_") catch @panic("OOM");
                    exported.appendSlice(alloc, func_name) catch @panic("OOM");
                    exported.append(alloc, '\'') catch @panic("OOM");
                }
            }
        }
        exported.append(alloc, ']') catch @panic("OOM");

        emcc.addArg(b.fmt("-sEXPORTED_FUNCTIONS={s}", .{exported.items}));
    }

    for (compile_steps) |compile_step| {
        emcc.addArtifactArg(compile_step);
        for (compile_step.root_module.getGraph().modules) |module| {
            for (module.link_objects.items) |link_object| {
                switch (link_object) {
                    .other_step => |linked| {
                        switch (linked.kind) {
                            .lib => emcc.addArtifactArg(linked),
                            else => {},
                        }
                    },
                    else => {},
                }
            }
        }
    }

    emcc.addArg("-o");
    const out_file = emcc.addOutputFileArg(out_file_name);

    const install_step = b.addInstallDirectory(.{
        .source_dir = out_file.dirname(),
        .install_dir = install_dir,
        .install_subdir = "",
    });
    install_step.step.dependOn(&emcc.step);

    return &install_step.step;
}

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
