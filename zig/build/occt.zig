const std = @import("std");
const common = @import("common.zig");
const emscripten = @import("emscripten.zig");

pub fn addOcctIncludePathsFromDependency(
    b: *std.Build,
    compile: *std.Build.Step.Compile,
) void {
    const occt_dep = b.dependency("occt", .{});
    const occt_root_lp = occt_dep.path("");
    const occt_root = occt_root_lp.getPath(b);
    const occt_src = b.fmt("{s}/src", .{occt_root});

    // OCCT source tree does not ship generated Standard_Version.hxx.
    // Create a local equivalent from version.cmake so source-tree includes work.
    const version = parseOcctVersionFromCMake(
        b,
        b.fmt("{s}/adm/cmake/version.cmake", .{occt_root}),
    );
    const wf = b.addWriteFiles();
    const standard_version_hxx = wf.add("Standard_Version.hxx", b.fmt(
        \\#ifndef _Standard_Version_HeaderFile
        \\#define _Standard_Version_HeaderFile
        \\#define OCC_VERSION_MAJOR {d}
        \\#define OCC_VERSION_MINOR {d}
        \\#define OCC_VERSION_MAINTENANCE {d}
        \\#define OCC_VERSION {d}.{d}
        \\#define OCC_VERSION_STRING "{d}.{d}"
        \\#define OCC_VERSION_COMPLETE "{d}.{d}.{d}"
        \\#define OCC_VERSION_STRING_EXT OCC_VERSION_COMPLETE
        \\#define OCC_VERSION_HEX (OCC_VERSION_MAJOR << 16 | OCC_VERSION_MINOR << 8 | OCC_VERSION_MAINTENANCE)
        \\#endif
    , .{
        version.major,
        version.minor,
        version.maintenance,
        version.major,
        version.minor,
        version.major,
        version.minor,
        version.major,
        version.minor,
        version.maintenance,
    }));
    compile.addIncludePath(standard_version_hxx.dirname());

    var include_dirs = collectHeaderDirsRecursively(b, occt_src);
    defer include_dirs.deinit(b.allocator);
    for (include_dirs.items) |dir| {
        compile.addIncludePath(.{ .cwd_relative = dir });
    }
}

pub fn addOcctToolkitArchiveLibraries(
    b: *std.Build,
    target: std.Build.ResolvedTarget,
    optimize: std.builtin.OptimizeMode,
    toolkits_arg: []const u8,
) []const *std.Build.Step.Compile {
    const occt_dep = b.dependency("occt", .{});
    const occt_src_root = occt_dep.path("src");
    const occt_src = occt_src_root.getPath(b);

    var requested = common.parseSemicolonList(b, toolkits_arg);
    defer requested.deinit(b.allocator);
    if (requested.items.len == 0) {
        @panic("No OCCT toolkits selected. Provide -Docc_toolkits.");
    }

    var toolkits = resolveOcctToolkitClosure(b, occt_src, requested.items);
    defer toolkits.deinit(b.allocator);

    var archives = std.ArrayList(*std.Build.Step.Compile).empty;
    for (toolkits.items) |toolkit| {
        const root_module = b.createModule(.{
            .target = target,
            .optimize = optimize,
        });
        const toolkit_archive = b.addLibrary(.{
            .name = b.fmt("occt_{s}", .{toolkit}),
            .linkage = .static,
            .root_module = root_module,
        });
        toolkit_archive.linkLibC();
        toolkit_archive.linkLibCpp();
        addOcctIncludePathsFromDependency(b, toolkit_archive);

        appendOcctToolkitSourcesToLibrary(
            b,
            toolkit_archive,
            toolkit,
            occt_src_root,
            occt_src,
            target,
        );

        archives.append(b.allocator, toolkit_archive) catch
            @panic("Out of memory collecting OCCT toolkit archive steps");
    }

    return archives.toOwnedSlice(b.allocator) catch
        @panic("Out of memory finalizing OCCT toolkit archive list");
}

pub fn addOcctToolkitSources(
    b: *std.Build,
    lib: *std.Build.Step.Compile,
    toolkits_arg: []const u8,
    target: std.Build.ResolvedTarget,
) void {
    const occt_dep = b.dependency("occt", .{});
    const occt_src_root = occt_dep.path("src");
    const occt_src = occt_src_root.getPath(b);

    var requested = common.parseSemicolonList(b, toolkits_arg);
    defer requested.deinit(b.allocator);
    if (requested.items.len == 0) {
        @panic("No OCCT toolkits selected. Provide -Docc_toolkits.");
    }

    var toolkits = resolveOcctToolkitClosure(b, occt_src, requested.items);
    defer toolkits.deinit(b.allocator);

    std.mem.sort([]const u8, toolkits.items, {}, struct {
        fn lessThan(_: void, a: []const u8, b_: []const u8) bool {
            return std.mem.order(u8, a, b_) == .lt;
        }
    }.lessThan);

    for (toolkits.items) |toolkit| {
        appendOcctToolkitSourcesToLibrary(
            b,
            lib,
            toolkit,
            occt_src_root,
            occt_src,
            target,
        );
    }
}

pub fn linkOpenCascadeLibraries(
    lib: *std.Build.Step.Compile,
    occ_libs_arg: []const u8,
) void {
    var it = std.mem.splitScalar(u8, occ_libs_arg, ';');
    while (it.next()) |raw| {
        const name = std.mem.trim(u8, raw, " \t\r\n");
        if (name.len == 0) continue;
        lib.root_module.linkSystemLibrary(name, .{});
    }
}

fn appendOcctToolkitSourcesToLibrary(
    b: *std.Build,
    lib: *std.Build.Step.Compile,
    toolkit: []const u8,
    occt_src_root: std.Build.LazyPath,
    occt_src: []const u8,
    target: std.Build.ResolvedTarget,
) void {
    var cpp_sources = std.ArrayList([]const u8).empty;
    defer cpp_sources.deinit(b.allocator);
    var c_sources = std.ArrayList([]const u8).empty;
    defer c_sources.deinit(b.allocator);

    collectOcctToolkitSources(
        b,
        occt_src,
        toolkit,
        &cpp_sources,
        &c_sources,
    );

    if (cpp_sources.items.len == 0 and c_sources.items.len == 0) {
        @panic("No OCCT source files found for toolkit");
    }

    var cpp_flags = std.ArrayList([]const u8).empty;
    defer cpp_flags.deinit(b.allocator);
    const math_compat_header = b.path("zig/build/include/occt_math_compat.h").getPath(b);
    cpp_flags.append(b.allocator, "-std=c++17") catch @panic("Out of memory building OCCT flags");
    cpp_flags.append(b.allocator, "-Wno-error") catch @panic("Out of memory building OCCT flags");
    cpp_flags.append(b.allocator, "-DOCCT_NO_PLUGINS") catch @panic("Out of memory building OCCT flags");
    cpp_flags.append(b.allocator, "-include") catch @panic("Out of memory building OCCT flags");
    cpp_flags.append(b.allocator, math_compat_header) catch @panic("Out of memory building OCCT flags");
    common.appendEmscriptenSysrootFlags(b, &cpp_flags, target);

    if (cpp_sources.items.len > 0) {
        lib.addCSourceFiles(.{
            .root = occt_src_root,
            .files = cpp_sources.items,
            .flags = cpp_flags.items,
        });
    }

    var c_flags = std.ArrayList([]const u8).empty;
    defer c_flags.deinit(b.allocator);
    c_flags.append(b.allocator, "-Wno-error") catch @panic("Out of memory building OCCT C flags");
    c_flags.append(b.allocator, "-DOCCT_NO_PLUGINS") catch @panic("Out of memory building OCCT C flags");
    c_flags.append(b.allocator, "-include") catch @panic("Out of memory building OCCT C flags");
    c_flags.append(b.allocator, math_compat_header) catch @panic("Out of memory building OCCT C flags");
    common.appendEmscriptenSysrootFlags(b, &c_flags, target);

    if (c_sources.items.len > 0) {
        lib.addCSourceFiles(.{
            .root = occt_src_root,
            .files = c_sources.items,
            .flags = c_flags.items,
        });
    }
}

fn collectOcctToolkitSources(
    b: *std.Build,
    occt_src: []const u8,
    toolkit: []const u8,
    cpp_sources: *std.ArrayList([]const u8),
    c_sources: *std.ArrayList([]const u8),
) void {
    const packages_path = b.fmt("{s}/{s}/PACKAGES", .{ occt_src, toolkit });
    const packages_data = std.fs.cwd().readFileAlloc(b.allocator, packages_path, 128 * 1024) catch
        @panic("Unable to read OCCT toolkit PACKAGES");
    defer b.allocator.free(packages_data);

    var seen_sources = std.StringHashMap(void).init(b.allocator);
    defer seen_sources.deinit();

    var has_package = false;
    var line_it = std.mem.splitScalar(u8, packages_data, '\n');
    while (line_it.next()) |line_raw| {
        const package_name = sanitizeOcctListLine(line_raw);
        if (package_name.len == 0) continue;
        has_package = true;
        appendOcctPackageSources(
            b,
            occt_src,
            package_name,
            &seen_sources,
            cpp_sources,
            c_sources,
        );
    }

    if (!has_package) {
        appendOcctPackageSources(
            b,
            occt_src,
            toolkit,
            &seen_sources,
            cpp_sources,
            c_sources,
        );
    }
}

fn appendOcctPackageSources(
    b: *std.Build,
    occt_src: []const u8,
    package_name: []const u8,
    seen_sources: *std.StringHashMap(void),
    cpp_sources: *std.ArrayList([]const u8),
    c_sources: *std.ArrayList([]const u8),
) void {
    const files_path = b.fmt("{s}/{s}/FILES", .{ occt_src, package_name });
    const files_data = std.fs.cwd().readFileAlloc(b.allocator, files_path, 2 * 1024 * 1024) catch return;
    defer b.allocator.free(files_data);

    var line_it = std.mem.splitScalar(u8, files_data, '\n');
    while (line_it.next()) |line_raw| {
        const file_name = sanitizeOcctListLine(line_raw);
        if (file_name.len == 0) continue;
        if (!isOcctCppSource(file_name) and !isOcctCSource(file_name)) continue;

        const rel_path = b.fmt("{s}/{s}", .{ package_name, file_name });
        const abs_path = b.fmt("{s}/{s}", .{ occt_src, rel_path });
        std.fs.cwd().access(abs_path, .{}) catch continue;

        const gop = seen_sources.getOrPut(rel_path) catch @panic("Out of memory collecting OCCT sources");
        if (gop.found_existing) continue;

        if (isOcctCppSource(file_name)) {
            cpp_sources.append(b.allocator, rel_path) catch @panic("Out of memory collecting OCCT C++ sources");
        } else {
            c_sources.append(b.allocator, rel_path) catch @panic("Out of memory collecting OCCT C sources");
        }
    }
}

fn resolveOcctToolkitClosure(
    b: *std.Build,
    occt_src: []const u8,
    roots: []const []const u8,
) std.ArrayList([]const u8) {
    var queue = std.ArrayList([]const u8).empty;
    var ordered = std.ArrayList([]const u8).empty;
    var seen = std.StringHashMap(void).init(b.allocator);
    defer seen.deinit();

    for (roots) |root| {
        if (!occtToolkitExists(b, occt_src, root)) {
            @panic("Requested OCCT toolkit is missing in dependency source tree.");
        }
        common.appendUniqueString(b, &seen, &queue, root);
    }

    var index: usize = 0;
    while (index < queue.items.len) : (index += 1) {
        const toolkit = queue.items[index];
        ordered.append(b.allocator, toolkit) catch @panic("Out of memory collecting OCCT toolkits");

        const externlib_path = b.fmt("{s}/{s}/EXTERNLIB", .{ occt_src, toolkit });
        const externlib_data = std.fs.cwd().readFileAlloc(b.allocator, externlib_path, 128 * 1024) catch continue;
        defer b.allocator.free(externlib_data);

        var line_it = std.mem.splitScalar(u8, externlib_data, '\n');
        while (line_it.next()) |line_raw| {
            const dep = sanitizeOcctListLine(line_raw);
            if (dep.len == 0) continue;
            if (!std.mem.startsWith(u8, dep, "TK")) continue;
            if (!occtToolkitExists(b, occt_src, dep)) continue;
            common.appendUniqueString(b, &seen, &queue, dep);
        }
    }

    return ordered;
}

fn occtToolkitExists(
    b: *std.Build,
    occt_src: []const u8,
    toolkit: []const u8,
) bool {
    const packages_path = b.fmt("{s}/{s}/PACKAGES", .{ occt_src, toolkit });
    std.fs.cwd().access(packages_path, .{}) catch return false;
    return true;
}

fn sanitizeOcctListLine(line_raw: []const u8) []const u8 {
    var line = std.mem.trim(u8, line_raw, " \t\r");
    if (line.len == 0) return "";
    if (line[0] == '#') return "";

    if (std.mem.indexOfScalar(u8, line, '#')) |comment_idx| {
        line = std.mem.trim(u8, line[0..comment_idx], " \t\r");
    }
    return line;
}

fn isOcctCppSource(file_name: []const u8) bool {
    return std.mem.endsWith(u8, file_name, ".cxx") or
        std.mem.endsWith(u8, file_name, ".cpp") or
        std.mem.endsWith(u8, file_name, ".cc") or
        std.mem.endsWith(u8, file_name, ".mm");
}

fn isOcctCSource(file_name: []const u8) bool {
    return std.mem.endsWith(u8, file_name, ".c");
}

const OcctVersion = struct {
    major: u32,
    minor: u32,
    maintenance: u32,
};

fn parseOcctVersionFromCMake(
    b: *std.Build,
    version_cmake_path: []const u8,
) OcctVersion {
    const data = std.fs.cwd().readFileAlloc(b.allocator, version_cmake_path, 1024 * 1024) catch
        @panic("Unable to read OCCT adm/cmake/version.cmake");
    defer b.allocator.free(data);

    const major = parseOcctVersionValue(data, "OCC_VERSION_MAJOR");
    const minor = parseOcctVersionValue(data, "OCC_VERSION_MINOR");
    const maintenance = parseOcctVersionValue(data, "OCC_VERSION_MAINTENANCE");

    return .{
        .major = major,
        .minor = minor,
        .maintenance = maintenance,
    };
}

fn parseOcctVersionValue(
    file_data: []const u8,
    key: []const u8,
) u32 {
    var line_it = std.mem.splitScalar(u8, file_data, '\n');
    while (line_it.next()) |line_raw| {
        const line = std.mem.trim(u8, line_raw, " \t\r");
        if (!std.mem.startsWith(u8, line, "set")) continue;
        if (std.mem.indexOf(u8, line, key) == null) continue;

        var open_paren = std.mem.indexOfScalar(u8, line, '(') orelse continue;
        open_paren += 1;
        const close_paren = std.mem.lastIndexOfScalar(u8, line, ')') orelse line.len;
        if (close_paren <= open_paren) continue;

        const inside = line[open_paren..close_paren];
        var tok_it = std.mem.tokenizeAny(u8, inside, " \t");
        const first = tok_it.next() orelse continue;
        if (!std.mem.eql(u8, first, key)) continue;
        const value_tok = tok_it.next() orelse continue;
        return std.fmt.parseInt(u32, value_tok, 10) catch continue;
    }

    @panic("Failed to parse OCCT version from version.cmake");
}

fn collectHeaderDirsRecursively(
    b: *std.Build,
    root_dir: []const u8,
) std.ArrayList([]const u8) {
    var dirs = std.ArrayList([]const u8).empty;
    var seen = std.StringHashMap(void).init(b.allocator);
    defer seen.deinit();

    var dir = std.fs.cwd().openDir(root_dir, .{ .iterate = true }) catch
        @panic("Failed to open OCCT source dir");
    defer dir.close();

    var walker = dir.walk(b.allocator) catch
        @panic("Failed to walk OCCT source dir");
    defer walker.deinit();

    while (walker.next() catch @panic("Failed to iterate OCCT source dir")) |entry| {
        if (entry.kind != .file) continue;
        if (!std.mem.endsWith(u8, entry.path, ".hxx") and
            !std.mem.endsWith(u8, entry.path, ".h") and
            !std.mem.endsWith(u8, entry.path, ".lxx") and
            !std.mem.endsWith(u8, entry.path, ".gxx") and
            !std.mem.endsWith(u8, entry.path, ".hpp") and
            !std.mem.endsWith(u8, entry.path, ".pxx") and
            !std.mem.endsWith(u8, entry.path, ".g"))
        {
            continue;
        }

        const rel_dir = std.fs.path.dirname(entry.path) orelse continue;
        const abs_dir = if (rel_dir.len == 0)
            b.fmt("{s}", .{root_dir})
        else
            b.fmt("{s}/{s}", .{ root_dir, rel_dir });

        const gop = seen.getOrPut(abs_dir) catch @panic("Out of memory collecting OCCT include dirs");
        if (gop.found_existing) continue;
        dirs.append(b.allocator, abs_dir) catch @panic("Out of memory collecting OCCT include dirs");
    }

    std.mem.sort([]const u8, dirs.items, {}, struct {
        fn lessThan(_: void, a: []const u8, b_: []const u8) bool {
            return std.mem.order(u8, a, b_) == .lt;
        }
    }.lessThan);

    return dirs;
}
