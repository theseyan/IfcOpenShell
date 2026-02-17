const std = @import("std");
const emscripten = @import("emscripten.zig");

pub fn parseSchemas(
    b: *std.Build,
    schemas_arg: []const u8,
    out_schemas: *std.ArrayList([]const u8),
) void {
    appendUniqueTrimmedBySeparator(b, schemas_arg, ',', out_schemas, "Out of memory parsing -Dschemas");

    if (out_schemas.items.len == 0) {
        @panic("No schema versions provided. Pass -Dschemas=4 or similar.");
    }
}

pub fn makeSchemaSeqMacro(
    b: *std.Build,
    schemas: []const []const u8,
) []const u8 {
    var buffer = std.ArrayList(u8).empty;
    for (schemas) |schema| {
        buffer.append(b.allocator, '(') catch @panic("Out of memory building SCHEMA_SEQ");
        buffer.appendSlice(b.allocator, schema) catch @panic("Out of memory building SCHEMA_SEQ");
        buffer.append(b.allocator, ')') catch @panic("Out of memory building SCHEMA_SEQ");
    }
    return buffer.toOwnedSlice(b.allocator) catch @panic("Out of memory finalizing SCHEMA_SEQ");
}

pub fn appendCommonCppFlags(
    b: *std.Build,
    flags: *std.ArrayList([]const u8),
    target: std.Build.ResolvedTarget,
) void {
    flags.append(b.allocator, "-std=c++17") catch @panic("Out of memory building C++ flags");
    flags.append(b.allocator, "-D_DISABLE_CONSTEXPR_MUTEX_CONSTRUCTOR") catch @panic("Out of memory building C++ flags");
    flags.append(b.allocator, "-DBOOST_ALL_NO_LIB") catch @panic("Out of memory building C++ flags");
    flags.append(b.allocator, "-Wno-error") catch @panic("Out of memory building C++ flags");

    const math_compat_header = b.path("zig/build/include/occt_math_compat.h").getPath(b);
    flags.append(b.allocator, "-include") catch @panic("Out of memory building C++ flags");
    flags.append(b.allocator, math_compat_header) catch @panic("Out of memory building C++ flags");

    appendEmscriptenSysrootFlags(b, flags, target);
}

pub fn appendEmscriptenSysrootFlags(
    b: *std.Build,
    flags: *std.ArrayList([]const u8),
    target: std.Build.ResolvedTarget,
) void {
    if (!emscripten.isEmscriptenTarget(target)) return;
    flags.append(b.allocator, "-Wno-macro-redefined") catch @panic("OOM");
    flags.append(b.allocator, "-fno-sanitize=all") catch @panic("OOM");
    // Emscripten JS-based exception handling (pairs with
    // -sDISABLE_EXCEPTION_CATCHING=0 on the linker side in emscripten.zig).
    flags.append(b.allocator, "-mllvm") catch @panic("OOM");
    flags.append(b.allocator, "-enable-emscripten-cxx-exceptions") catch @panic("OOM");
    flags.append(b.allocator, "-I") catch @panic("OOM");
    flags.append(b.allocator, emscripten.emSdkSysrootCxxIncludePath(b)) catch @panic("OOM");
    flags.append(b.allocator, "-I") catch @panic("OOM");
    flags.append(b.allocator, emscripten.emSdkSysrootCompatIncludePath(b)) catch @panic("OOM");
    flags.append(b.allocator, "-I") catch @panic("OOM");
    flags.append(b.allocator, emscripten.emSdkSysrootIncludePath(b)) catch @panic("OOM");
}

pub fn appendSchemaHasFlags(
    b: *std.Build,
    flags: *std.ArrayList([]const u8),
    schemas: []const []const u8,
) void {
    for (schemas) |schema| {
        flags.append(b.allocator, b.fmt("-DHAS_SCHEMA_{s}", .{schema})) catch
            @panic("Out of memory building schema flags");
    }
}

pub fn collectCppFilesInDirectory(
    b: *std.Build,
    dir_path: []const u8,
    exclude_if_name_has_digit: bool,
) std.ArrayList([]const u8) {
    return collectCppFilesInDirectoryExcluding(
        b,
        dir_path,
        exclude_if_name_has_digit,
        &.{},
    );
}

pub fn collectCppFilesInDirectoryExcluding(
    b: *std.Build,
    dir_path: []const u8,
    exclude_if_name_has_digit: bool,
    excluded_file_names: []const []const u8,
) std.ArrayList([]const u8) {
    var files = std.ArrayList([]const u8).empty;

    var dir = std.fs.cwd().openDir(dir_path, .{ .iterate = true }) catch @panic("Failed to open source directory");
    defer dir.close();

    var it = dir.iterate();
    while (it.next() catch @panic("Failed to iterate source directory")) |entry| {
        if (entry.kind != .file) continue;
        if (!std.mem.endsWith(u8, entry.name, ".cpp")) continue;
        if (sliceContainsString(excluded_file_names, entry.name)) continue;

        if (exclude_if_name_has_digit) {
            var has_digit = false;
            for (entry.name) |c| {
                if (std.ascii.isDigit(c)) {
                    has_digit = true;
                    break;
                }
            }
            if (has_digit) continue;
        }

        files.append(b.allocator, b.fmt("{s}/{s}", .{ dir_path, entry.name })) catch
            @panic("Out of memory collecting C++ sources");
    }

    std.mem.sort([]const u8, files.items, {}, struct {
        fn lessThan(_: void, a: []const u8, b_: []const u8) bool {
            return std.mem.order(u8, a, b_) == .lt;
        }
    }.lessThan);

    return files;
}

pub fn parseSemicolonList(
    b: *std.Build,
    value: []const u8,
) std.ArrayList([]const u8) {
    var list = std.ArrayList([]const u8).empty;
    appendUniqueTrimmedBySeparator(b, value, ';', &list, "Out of memory parsing semicolon-separated list");
    return list;
}

pub fn parseCommaList(
    b: *std.Build,
    value: []const u8,
) std.ArrayList([]const u8) {
    var list = std.ArrayList([]const u8).empty;
    appendUniqueTrimmedBySeparator(
        b,
        value,
        ',',
        &list,
        "Out of memory parsing comma-separated list",
    );
    return list;
}

fn appendUniqueTrimmedBySeparator(
    b: *std.Build,
    value: []const u8,
    separator: u8,
    out: *std.ArrayList([]const u8),
    oom_message: []const u8,
) void {
    var it = std.mem.splitScalar(u8, value, separator);
    while (it.next()) |raw| {
        const item = std.mem.trim(u8, raw, " \t\r\n");
        if (item.len == 0) continue;
        if (sliceContainsString(out.items, item)) continue;
        out.append(b.allocator, item) catch @panic(oom_message);
    }
}

pub fn appendUniqueString(
    b: *std.Build,
    seen: *std.StringHashMap(void),
    list: *std.ArrayList([]const u8),
    value: []const u8,
) void {
    const owned = b.allocator.dupe(u8, value) catch @panic("Out of memory collecting strings");
    const gop = seen.getOrPut(owned) catch @panic("Out of memory collecting strings");
    if (gop.found_existing) {
        b.allocator.free(owned);
        return;
    }
    list.append(b.allocator, owned) catch @panic("Out of memory collecting strings");
}

pub fn sliceContainsString(
    values: []const []const u8,
    value: []const u8,
) bool {
    for (values) |it| {
        if (std.mem.eql(u8, it, value)) return true;
    }
    return false;
}
