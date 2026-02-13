const std = @import("std");
const common = @import("common.zig");
const deps = @import("deps.zig");

pub fn addIfcParseLibrary(
    b: *std.Build,
    target: std.Build.ResolvedTarget,
    optimize: std.builtin.OptimizeMode,
    schemas: []const []const u8,
    use_mmap: bool,
) *std.Build.Step.Compile {
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
    deps.addBoostIncludesFromDependency(b, lib, target, optimize);
    deps.linkBoostLibraryFromDependency(b, lib, target, optimize);

    var cpp_flags = std.ArrayList([]const u8).empty;
    defer cpp_flags.deinit(b.allocator);

    common.appendCommonCppFlags(b, &cpp_flags);
    if (use_mmap) {
        cpp_flags.append(b.allocator, "-DUSE_MMAP") catch @panic("Out of memory building IfcParse flags");
    }
    common.appendSchemaHasFlags(b, &cpp_flags, schemas);

    var sources = common.collectCppFilesInDirectory(b, "src/ifcparse", true);
    defer sources.deinit(b.allocator);

    for (schemas) |schema| {
        const schema_cpp = b.fmt("src/ifcparse/Ifc{s}.cpp", .{schema});
        const schema_schema_cpp = b.fmt("src/ifcparse/Ifc{s}-schema.cpp", .{schema});

        std.fs.cwd().access(schema_cpp, .{}) catch @panic("Missing schema source file. Check -Dschemas values.");
        std.fs.cwd().access(schema_schema_cpp, .{}) catch @panic("Missing schema source file. Check -Dschemas values.");

        sources.append(b.allocator, schema_cpp) catch @panic("Out of memory collecting IfcParse schema sources");
        sources.append(b.allocator, schema_schema_cpp) catch @panic("Out of memory collecting IfcParse schema sources");
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

    return lib;
}
