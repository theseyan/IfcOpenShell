const std = @import("std");
const emscripten = @import("emscripten.zig");
const occt = @import("occt.zig");

pub fn addBoostIncludesFromDependency(
    b: *std.Build,
    compile: *std.Build.Step.Compile,
    target: std.Build.ResolvedTarget,
    optimize: std.builtin.OptimizeMode,
) void {
    const is_wasm = emscripten.isEmscriptenTarget(target);

    // Overlay for Boost.Regex trait lookup that avoids a Zig C++ local-static
    // initialization bug while keeping Boost.Regex itself in use.
    compile.addIncludePath(b.path("zig/patches/boost"));

    const boost_dep = b.dependency("boost", .{
        .target = target,
        .optimize = optimize,
        // Boost.Regex requires threading; disable for wasm (single-threaded).
        .regex = !is_wasm,
    });
    const boost_artifact = boost_dep.artifact("boost");

    for (boost_artifact.root_module.include_dirs.items) |include_dir| {
        compile.root_module.include_dirs.append(b.allocator, include_dir) catch
            @panic("Out of memory adding boost include dirs");
    }

    // boost-libraries-zig currently does not export dynamic_bitset;
    // IfcParse / IfcGeom require <boost/dynamic_bitset.hpp>.
    const boost_dynamic_bitset_dep = b.dependency("boost_dynamic_bitset", .{});
    compile.addIncludePath(boost_dynamic_bitset_dep.path("include"));

    // boost-libraries-zig currently does not export scope_exit;
    // IfcParse requires <boost/scope_exit.hpp>.
    const boost_scope_exit_dep = b.dependency("boost_scope_exit", .{});
    compile.addIncludePath(boost_scope_exit_dep.path("include"));

    // boost-libraries-zig currently does not export program_options;
    // IfcGeom requires <boost/program_options.hpp>.
    const boost_program_options_dep = b.dependency("boost_program_options", .{});
    compile.addIncludePath(boost_program_options_dep.path("include"));

    // boost-libraries-zig currently does not export foreach;
    // IfcGeom requires <boost/foreach.hpp>.
    const boost_foreach_dep = b.dependency("boost_foreach", .{});
    compile.addIncludePath(boost_foreach_dep.path("include"));
}

pub fn linkBoostLibraryFromDependency(
    b: *std.Build,
    compile: *std.Build.Step.Compile,
    target: std.Build.ResolvedTarget,
    optimize: std.builtin.OptimizeMode,
) void {
    const is_wasm = emscripten.isEmscriptenTarget(target);
    const boost_dep = b.dependency("boost", .{
        .target = target,
        .optimize = optimize,
        .regex = !is_wasm,
    });
    const boost_artifact = boost_dep.artifact("boost");

    // When targeting wasm32-emscripten the boost artifact needs the
    // Emscripten sysroot so it can find standard C/C++ headers.
    if (is_wasm) {
        emscripten.addEmscriptenSysrootIncludePaths(b, boost_artifact);
    }

    compile.linkLibrary(boost_artifact);
}

pub fn addIfcGeomIncludePaths(
    b: *std.Build,
    compile: *std.Build.Step.Compile,
    target: std.Build.ResolvedTarget,
    optimize: std.builtin.OptimizeMode,
    occ_include_dir: ?[]const u8,
    eigen_include_dir: ?[]const u8,
) void {
    compile.addIncludePath(b.path("src"));
    compile.addIncludePath(b.path("src/ifcparse"));
    compile.addIncludePath(b.path("src/ifcgeom"));
    compile.addIncludePath(b.path("src/ifcgeom/mapping"));
    compile.addIncludePath(b.path("src/ifcgeom/kernels"));
    compile.addIncludePath(b.path("src/ifcgeom/kernels/opencascade"));

    if (occ_include_dir) |dir| {
        compile.addIncludePath(.{ .cwd_relative = dir });
    } else {
        occt.addOcctIncludePathsFromDependency(b, compile);
    }

    if (eigen_include_dir) |dir| {
        compile.addIncludePath(.{ .cwd_relative = dir });
    } else {
        const eigen_dep = b.dependency("eigen", .{});
        compile.addIncludePath(eigen_dep.path(""));
    }

    addBoostIncludesFromDependency(b, compile, target, optimize);
}

pub fn addSerializersIncludePaths(
    b: *std.Build,
    compile: *std.Build.Step.Compile,
    target: std.Build.ResolvedTarget,
    optimize: std.builtin.OptimizeMode,
    occ_include_dir: ?[]const u8,
    eigen_include_dir: ?[]const u8,
    enable_with_gltf: bool,
) void {
    addIfcGeomIncludePaths(
        b,
        compile,
        target,
        optimize,
        occ_include_dir,
        eigen_include_dir,
    );
    compile.addIncludePath(b.path("src/serializers"));
    compile.addIncludePath(b.path("src/serializers/schema_dependent"));

    if (enable_with_gltf) {
        const nlohmann_json_dep = b.dependency("nlohmann_json", .{});
        compile.addIncludePath(nlohmann_json_dep.path("include"));
        compile.addIncludePath(nlohmann_json_dep.path("single_include"));
    }
}
