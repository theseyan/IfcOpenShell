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

    const boost_dep = b.lazyDependency("boost", .{
        .target = target,
        .optimize = optimize,
        .regex = !is_wasm,
    });

    if (boost_dep) |dep| {
        const boost_artifact = dep.artifact("boost");
        for (boost_artifact.root_module.include_dirs.items) |include_dir| {
            compile.root_module.include_dirs.append(b.allocator, include_dir) catch
                @panic("Out of memory adding boost include dirs");
        }
    }

    const boost_dynamic_bitset_dep = b.dependency("boost_dynamic_bitset", .{});
    compile.addIncludePath(boost_dynamic_bitset_dep.path("include"));
    const boost_scope_exit_dep = b.dependency("boost_scope_exit", .{});
    compile.addIncludePath(boost_scope_exit_dep.path("include"));
    const boost_program_options_dep = b.dependency("boost_program_options", .{});
    compile.addIncludePath(boost_program_options_dep.path("include"));
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
    const boost_dep = b.lazyDependency("boost", .{
        .target = target,
        .optimize = optimize,
        .regex = !is_wasm,
    });

    if (boost_dep) |dep| {
        const boost_artifact = dep.artifact("boost");
        if (is_wasm) {
            emscripten.addEmscriptenSysrootIncludePaths(b, boost_artifact);
        }
        compile.linkLibrary(boost_artifact);
    }
}

pub fn addIfcGeomIncludePaths(
    b: *std.Build,
    compile: *std.Build.Step.Compile,
    target: std.Build.ResolvedTarget,
    optimize: std.builtin.OptimizeMode,
) void {
    compile.addIncludePath(b.path("src"));
    compile.addIncludePath(b.path("src/ifcparse"));
    compile.addIncludePath(b.path("src/ifcgeom"));
    compile.addIncludePath(b.path("src/ifcgeom/mapping"));
    compile.addIncludePath(b.path("src/ifcgeom/kernels"));
    compile.addIncludePath(b.path("src/ifcgeom/kernels/opencascade"));

    occt.addOcctIncludePathsFromDependency(b, compile);

    const eigen_dep = b.dependency("eigen", .{});
    compile.addIncludePath(eigen_dep.path(""));

    addBoostIncludesFromDependency(b, compile, target, optimize);
}

pub fn addSerializersIncludePaths(
    b: *std.Build,
    compile: *std.Build.Step.Compile,
    target: std.Build.ResolvedTarget,
    optimize: std.builtin.OptimizeMode,
    enable_with_gltf: bool,
) void {
    addIfcGeomIncludePaths(
        b,
        compile,
        target,
        optimize,
    );
    compile.addIncludePath(b.path("src/serializers"));
    compile.addIncludePath(b.path("src/serializers/schema_dependent"));

    if (enable_with_gltf) {
        const nlohmann_json_dep = b.dependency("nlohmann_json", .{});
        compile.addIncludePath(nlohmann_json_dep.path("include"));
        compile.addIncludePath(nlohmann_json_dep.path("single_include"));
    }
}
