const std = @import("std");

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    const schemas_arg = b.option([]const u8, "schemas", "Semicolon-separated IFC schema versions (e.g. 4;2x3;4x3_add2)") orelse "4";
    const use_mmap = b.option(bool, "use_mmap", "Enable USE_MMAP support") orelse false;

    var schemas = std.ArrayList([]const u8).empty;
    defer schemas.deinit(b.allocator);

    var schema_it = std.mem.splitScalar(u8, schemas_arg, ';');
    while (schema_it.next()) |raw| {
        const schema = std.mem.trim(u8, raw, " \t\r\n");
        if (schema.len == 0) continue;
        schemas.append(b.allocator, schema) catch @panic("Out of memory parsing --schemas");
    }
    if (schemas.items.len == 0) {
        @panic("No schema versions provided. Pass -Dschemas=4 or similar.");
    }

    const root_module = b.createModule(.{
        .target = target,
        .optimize = optimize,
    });

    const lib = b.addLibrary(.{
        .name = "IfcParse",
        .linkage = .static,
        .root_module = root_module,
    });

    lib.linkLibC();
    lib.linkLibCpp();

    lib.addIncludePath(b.path("src/ifcparse"));
    addBoostIncludesFromDependency(b, lib, target, optimize);

    var cpp_flags = std.ArrayList([]const u8).empty;
    defer cpp_flags.deinit(b.allocator);

    cpp_flags.append(b.allocator, "-std=c++17") catch @panic("Out of memory building C++ flags");
    cpp_flags.append(b.allocator, "-D_DISABLE_CONSTEXPR_MUTEX_CONSTRUCTOR") catch @panic("Out of memory building C++ flags");
    cpp_flags.append(b.allocator, "-DBOOST_ALL_NO_LIB") catch @panic("Out of memory building C++ flags");

    if (use_mmap) {
        cpp_flags.append(b.allocator, "-DUSE_MMAP") catch @panic("Out of memory building C++ flags");
    }

    for (schemas.items) |schema| {
        cpp_flags.append(b.allocator, b.fmt("-DHAS_SCHEMA_{s}", .{schema})) catch @panic("Out of memory building schema flags");
    }

    var sources = std.ArrayList([]const u8).empty;
    defer sources.deinit(b.allocator);

    var dir = std.fs.cwd().openDir("src/ifcparse", .{ .iterate = true }) catch @panic("Failed to open src/ifcparse");
    defer dir.close();

    var iter = dir.iterate();
    while (iter.next() catch @panic("Failed to iterate src/ifcparse")) |entry| {
        if (entry.kind != .file) continue;
        if (!std.mem.endsWith(u8, entry.name, ".cpp")) continue;

        // Mirrors CMake logic: include only non-schema generated core .cpp files.
        var has_digit = false;
        for (entry.name) |c| {
            if (std.ascii.isDigit(c)) {
                has_digit = true;
                break;
            }
        }
        if (has_digit) continue;

        sources.append(b.allocator, b.fmt("src/ifcparse/{s}", .{entry.name})) catch @panic("Out of memory collecting sources");
    }

    for (schemas.items) |schema| {
        const schema_cpp = b.fmt("src/ifcparse/Ifc{s}.cpp", .{schema});
        const schema_schema_cpp = b.fmt("src/ifcparse/Ifc{s}-schema.cpp", .{schema});

        std.fs.cwd().access(schema_cpp, .{}) catch @panic("Missing schema source file. Check -Dschemas values.");
        std.fs.cwd().access(schema_schema_cpp, .{}) catch @panic("Missing schema source file. Check -Dschemas values.");

        sources.append(b.allocator, schema_cpp) catch @panic("Out of memory collecting schema sources");
        sources.append(b.allocator, schema_schema_cpp) catch @panic("Out of memory collecting schema sources");
    }

    std.mem.sort([]const u8, sources.items, {}, struct {
        fn lessThan(_: void, a: []const u8, b_: []const u8) bool {
            return std.mem.order(u8, a, b_) == .lt;
        }
    }.lessThan);

    lib.addCSourceFiles(.{
        .files = sources.items,
        .flags = cpp_flags.items,
    });

    b.installArtifact(lib);

    const build_step = b.step("ifcparse", "Build the minimal IfcParse static library");
    build_step.dependOn(&lib.step);
}

fn addBoostIncludesFromDependency(
    b: *std.Build,
    lib: *std.Build.Step.Compile,
    target: std.Build.ResolvedTarget,
    optimize: std.builtin.OptimizeMode,
) void {
    const boost_dep = b.dependency("boost", .{
        .target = target,
        .optimize = optimize,
    });
    const boost_artifact = boost_dep.artifact("boost");

    for (boost_artifact.root_module.include_dirs.items) |include_dir| {
        lib.root_module.include_dirs.append(b.allocator, include_dir) catch
            @panic("Out of memory adding boost include dirs");
    }

    // boost-libraries-zig currently does not export dynamic_bitset;
    // IfcParse requires <boost/dynamic_bitset.hpp>.
    const boost_dynamic_bitset_dep = b.dependency("boost_dynamic_bitset", .{});
    lib.addIncludePath(boost_dynamic_bitset_dep.path("include"));

    // boost-libraries-zig currently does not export scope_exit;
    // IfcParse requires <boost/scope_exit.hpp>.
    const boost_scope_exit_dep = b.dependency("boost_scope_exit", .{});
    lib.addIncludePath(boost_scope_exit_dep.path("include"));
}
