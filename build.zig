const std = @import("std");

const default_occ_libs =
    "TKernel;TKMath;TKBRep;TKGeomBase;TKGeomAlgo;TKG3d;TKG2d;TKShHealing;TKTopAlgo;TKMesh;TKPrim;TKBool;TKBO;TKFillet;TKXSBase;TKOffset;TKHLR;TKBin;TKDESTEP;TKDEIGES";
const default_occ_toolkits =
    "TKernel;TKMath;TKBRep;TKGeomBase;TKGeomAlgo;TKG3d;TKG2d;TKShHealing;TKTopAlgo;TKMesh;TKPrim;TKBool;TKBO;TKFillet;TKXSBase;TKOffset;TKHLR";

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    const schemas_arg = b.option([]const u8, "schemas", "Semicolon-separated IFC schema versions (e.g. 4;2x3;4x3_add2)") orelse "4";
    const use_mmap = b.option(bool, "use_mmap", "Enable USE_MMAP support for IfcParse") orelse false;

    const occ_include_override = b.option([]const u8, "occ_include_dir", "Path to OpenCASCADE headers (directory containing Standard_Version.hxx or opencascade/Standard_Version.hxx)");
    const occ_toolkits_arg = b.option([]const u8, "occ_toolkits", "Semicolon-separated OpenCASCADE toolkits to compile and bundle into IfcGeom") orelse default_occ_toolkits;
    const bundle_occ_sources = b.option(bool, "bundle_occ_sources", "Compile OCCT toolkit sources and bundle into IfcGeom static archive") orelse true;
    const occ_library_dir = b.option([]const u8, "occ_library_dir", "Optional path to OpenCASCADE libraries");
    const occ_libs_arg = b.option([]const u8, "occ_libs", "Semicolon-separated OpenCASCADE library names for link stage") orelse default_occ_libs;
    const link_occ_libraries = b.option(bool, "link_occ_libraries", "Link OpenCASCADE libraries into IfcGeom (off keeps compile-only path)") orelse false;

    const eigen_include_override = b.option([]const u8, "eigen_include_dir", "Path to Eigen headers (directory containing Eigen/Dense)");

    var schemas = std.ArrayList([]const u8).empty;
    defer schemas.deinit(b.allocator);

    parseSchemas(b, schemas_arg, &schemas);
    const schema_seq_macro = makeSchemaSeqMacro(b, schemas.items);

    const ifcparse_lib = addIfcParseLibrary(b, target, optimize, schemas.items, use_mmap);
    const ifcparse_install = b.addInstallArtifact(ifcparse_lib, .{});
    b.getInstallStep().dependOn(&ifcparse_install.step);

    const ifcparse_step = b.step("ifcparse", "Build the minimal IfcParse static library");
    ifcparse_step.dependOn(&ifcparse_install.step);

    const ifcgeom_step = b.step("ifcgeom", "Build IfcGeom + OpenCASCADE kernel + per-schema mapping object libraries");
    const ifcgeom_lib = addIfcGeomLibrary(
        b,
        target,
        optimize,
        schemas.items,
        schema_seq_macro,
        occ_include_override,
        occ_toolkits_arg,
        bundle_occ_sources,
        eigen_include_override,
        occ_library_dir,
        occ_libs_arg,
        link_occ_libraries,
        ifcparse_lib,
    );
    const ifcgeom_install = b.addInstallArtifact(ifcgeom_lib, .{});
    b.getInstallStep().dependOn(&ifcgeom_install.step);
    ifcgeom_step.dependOn(&ifcgeom_install.step);
}

fn addIfcParseLibrary(
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
    addBoostIncludesFromDependency(b, lib, target, optimize);

    var cpp_flags = std.ArrayList([]const u8).empty;
    defer cpp_flags.deinit(b.allocator);

    appendCommonCppFlags(b, &cpp_flags);
    if (use_mmap) {
        cpp_flags.append(b.allocator, "-DUSE_MMAP") catch @panic("Out of memory building IfcParse flags");
    }
    appendSchemaHasFlags(b, &cpp_flags, schemas);

    var sources = collectCppFilesInDirectory(b, "src/ifcparse", true);
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

fn addIfcGeomLibrary(
    b: *std.Build,
    target: std.Build.ResolvedTarget,
    optimize: std.builtin.OptimizeMode,
    schemas: []const []const u8,
    schema_seq_macro: []const u8,
    occ_include_dir: ?[]const u8,
    occ_toolkits_arg: []const u8,
    bundle_occ_sources: bool,
    eigen_include_dir: ?[]const u8,
    occ_library_dir: ?[]const u8,
    occ_libs_arg: []const u8,
    link_occ_libraries: bool,
    ifcparse_lib: *std.Build.Step.Compile,
) *std.Build.Step.Compile {
    const root_module = b.createModule(.{
        .target = target,
        .optimize = optimize,
    });

    const lib = b.addLibrary(.{
        .name = "IfcGeom",
        .linkage = .static,
        .root_module = root_module,
    });

    lib.linkLibC();
    lib.linkLibCpp();

    addIfcGeomIncludePaths(
        b,
        lib,
        target,
        optimize,
        occ_include_dir,
        eigen_include_dir,
    );

    var ifcgeom_flags = std.ArrayList([]const u8).empty;
    defer ifcgeom_flags.deinit(b.allocator);
    appendCommonCppFlags(b, &ifcgeom_flags);
    ifcgeom_flags.append(b.allocator, "-DIFC_GEOM_EXPORTS") catch @panic("Out of memory building IfcGeom flags");
    ifcgeom_flags.append(b.allocator, "-DIFOPSH_WITH_OPENCASCADE") catch @panic("Out of memory building IfcGeom flags");
    ifcgeom_flags.append(b.allocator, b.fmt("-DSCHEMA_SEQ={s}", .{schema_seq_macro})) catch @panic("Out of memory building IfcGeom flags");
    appendSchemaHasFlags(b, &ifcgeom_flags, schemas);

    var ifcgeom_sources = collectCppFilesInDirectory(b, "src/ifcgeom", false);
    defer ifcgeom_sources.deinit(b.allocator);
    lib.addCSourceFiles(.{
        .files = ifcgeom_sources.items,
        .flags = ifcgeom_flags.items,
    });

    const kernel_occt_obj = addKernelOpenCascadeObject(
        b,
        target,
        optimize,
        schemas,
        schema_seq_macro,
        occ_include_dir,
        eigen_include_dir,
    );
    lib.addObject(kernel_occt_obj);

    var mapping_sources = collectCppFilesInDirectory(b, "src/ifcgeom/mapping", false);
    defer mapping_sources.deinit(b.allocator);
    for (schemas) |schema| {
        const mapping_obj = addSchemaMappingObject(
            b,
            target,
            optimize,
            schemas,
            schema_seq_macro,
            schema,
            mapping_sources.items,
            occ_include_dir,
            eigen_include_dir,
        );
        lib.addObject(mapping_obj);
    }

    if (bundle_occ_sources) {
        addOcctToolkitObjects(
            b,
            lib,
            target,
            optimize,
            occ_toolkits_arg,
        );
    }

    lib.linkLibrary(ifcparse_lib);

    if (link_occ_libraries) {
        if (occ_library_dir) |dir| {
            lib.root_module.addLibraryPath(.{ .cwd_relative = dir });
        }
        linkOpenCascadeLibraries(lib, occ_libs_arg);
    }

    return lib;
}

fn addKernelOpenCascadeObject(
    b: *std.Build,
    target: std.Build.ResolvedTarget,
    optimize: std.builtin.OptimizeMode,
    schemas: []const []const u8,
    schema_seq_macro: []const u8,
    occ_include_dir: ?[]const u8,
    eigen_include_dir: ?[]const u8,
) *std.Build.Step.Compile {
    const root_module = b.createModule(.{
        .target = target,
        .optimize = optimize,
    });

    const obj = b.addObject(.{
        .name = "geometry_kernel_opencascade",
        .root_module = root_module,
    });
    obj.linkLibC();
    obj.linkLibCpp();

    addIfcGeomIncludePaths(
        b,
        obj,
        target,
        optimize,
        occ_include_dir,
        eigen_include_dir,
    );

    var flags = std.ArrayList([]const u8).empty;
    defer flags.deinit(b.allocator);
    appendCommonCppFlags(b, &flags);
    flags.append(b.allocator, "-DIFC_GEOM_EXPORTS") catch @panic("Out of memory building kernel flags");
    flags.append(b.allocator, "-DIFOPSH_WITH_OPENCASCADE") catch @panic("Out of memory building kernel flags");
    flags.append(b.allocator, b.fmt("-DSCHEMA_SEQ={s}", .{schema_seq_macro})) catch @panic("Out of memory building kernel flags");
    appendSchemaHasFlags(b, &flags, schemas);

    var sources = collectCppFilesInDirectory(b, "src/ifcgeom/kernels/opencascade", false);
    defer sources.deinit(b.allocator);
    obj.addCSourceFiles(.{
        .files = sources.items,
        .flags = flags.items,
    });

    return obj;
}

fn addSchemaMappingObject(
    b: *std.Build,
    target: std.Build.ResolvedTarget,
    optimize: std.builtin.OptimizeMode,
    schemas: []const []const u8,
    schema_seq_macro: []const u8,
    schema: []const u8,
    mapping_sources: []const []const u8,
    occ_include_dir: ?[]const u8,
    eigen_include_dir: ?[]const u8,
) *std.Build.Step.Compile {
    const root_module = b.createModule(.{
        .target = target,
        .optimize = optimize,
    });

    const obj = b.addObject(.{
        .name = b.fmt("geometry_mapping_ifc{s}", .{schema}),
        .root_module = root_module,
    });
    obj.linkLibC();
    obj.linkLibCpp();

    addIfcGeomIncludePaths(
        b,
        obj,
        target,
        optimize,
        occ_include_dir,
        eigen_include_dir,
    );

    var flags = std.ArrayList([]const u8).empty;
    defer flags.deinit(b.allocator);
    appendCommonCppFlags(b, &flags);
    flags.append(b.allocator, "-DIFC_GEOM_EXPORTS") catch @panic("Out of memory building mapping flags");
    flags.append(b.allocator, "-DIFOPSH_WITH_OPENCASCADE") catch @panic("Out of memory building mapping flags");
    flags.append(b.allocator, b.fmt("-DSCHEMA_SEQ={s}", .{schema_seq_macro})) catch @panic("Out of memory building mapping flags");
    flags.append(b.allocator, b.fmt("-DIfcSchema=Ifc{s}", .{schema})) catch @panic("Out of memory building mapping flags");
    appendSchemaHasFlags(b, &flags, schemas);

    obj.addCSourceFiles(.{
        .files = mapping_sources,
        .flags = flags.items,
    });

    return obj;
}

fn addIfcGeomIncludePaths(
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
        addOcctIncludePathsFromDependency(b, compile);
    }
    if (eigen_include_dir) |dir| {
        compile.addIncludePath(.{ .cwd_relative = dir });
    } else {
        const eigen_dep = b.dependency("eigen", .{});
        compile.addIncludePath(eigen_dep.path(""));
    }

    addBoostIncludesFromDependency(b, compile, target, optimize);
}

fn addOcctIncludePathsFromDependency(
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

fn addOcctToolkitObjects(
    b: *std.Build,
    lib: *std.Build.Step.Compile,
    target: std.Build.ResolvedTarget,
    optimize: std.builtin.OptimizeMode,
    toolkits_arg: []const u8,
) void {
    const occt_dep = b.dependency("occt", .{});
    const occt_src_root = occt_dep.path("src");
    const occt_src = occt_src_root.getPath(b);

    var requested = parseSemicolonList(b, toolkits_arg);
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
        const toolkit_obj = addOcctToolkitObject(
            b,
            target,
            optimize,
            toolkit,
            occt_src_root,
            occt_src,
        );
        lib.addObject(toolkit_obj);
    }
}

fn addOcctToolkitObject(
    b: *std.Build,
    target: std.Build.ResolvedTarget,
    optimize: std.builtin.OptimizeMode,
    toolkit: []const u8,
    occt_src_root: std.Build.LazyPath,
    occt_src: []const u8,
) *std.Build.Step.Compile {
    const root_module = b.createModule(.{
        .target = target,
        .optimize = optimize,
    });

    const obj = b.addObject(.{
        .name = b.fmt("occt_toolkit_{s}", .{toolkit}),
        .root_module = root_module,
    });
    obj.linkLibC();
    obj.linkLibCpp();

    addOcctIncludePathsFromDependency(b, obj);

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
    cpp_flags.append(b.allocator, "-std=c++17") catch @panic("Out of memory building OCCT flags");
    cpp_flags.append(b.allocator, "-Wno-error") catch @panic("Out of memory building OCCT flags");
    cpp_flags.append(b.allocator, "-DOCCT_NO_PLUGINS") catch @panic("Out of memory building OCCT flags");

    if (cpp_sources.items.len > 0) {
        obj.addCSourceFiles(.{
            .root = occt_src_root,
            .files = cpp_sources.items,
            .flags = cpp_flags.items,
        });
    }

    var c_flags = std.ArrayList([]const u8).empty;
    defer c_flags.deinit(b.allocator);
    c_flags.append(b.allocator, "-Wno-error") catch @panic("Out of memory building OCCT C flags");
    c_flags.append(b.allocator, "-DOCCT_NO_PLUGINS") catch @panic("Out of memory building OCCT C flags");

    if (c_sources.items.len > 0) {
        obj.addCSourceFiles(.{
            .root = occt_src_root,
            .files = c_sources.items,
            .flags = c_flags.items,
        });
    }

    return obj;
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
        appendUniqueString(b, &seen, &queue, root);
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
            appendUniqueString(b, &seen, &queue, dep);
        }
    }

    return ordered;
}

fn parseSemicolonList(
    b: *std.Build,
    value: []const u8,
) std.ArrayList([]const u8) {
    var list = std.ArrayList([]const u8).empty;
    var it = std.mem.splitScalar(u8, value, ';');
    while (it.next()) |raw| {
        const item = std.mem.trim(u8, raw, " \t\r\n");
        if (item.len == 0) continue;
        list.append(b.allocator, item) catch @panic("Out of memory parsing semicolon-separated list");
    }
    return list;
}

fn appendUniqueString(
    b: *std.Build,
    seen: *std.StringHashMap(void),
    list: *std.ArrayList([]const u8),
    value: []const u8,
) void {
    const owned = b.allocator.dupe(u8, value) catch @panic("Out of memory collecting OCCT toolkit names");
    const gop = seen.getOrPut(owned) catch @panic("Out of memory collecting OCCT toolkit names");
    if (gop.found_existing) {
        b.allocator.free(owned);
        return;
    }
    list.append(b.allocator, owned) catch @panic("Out of memory collecting OCCT toolkit names");
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
    // IfcParse / IfcGeom require <boost/dynamic_bitset.hpp>.
    const boost_dynamic_bitset_dep = b.dependency("boost_dynamic_bitset", .{});
    lib.addIncludePath(boost_dynamic_bitset_dep.path("include"));

    // boost-libraries-zig currently does not export scope_exit;
    // IfcParse requires <boost/scope_exit.hpp>.
    const boost_scope_exit_dep = b.dependency("boost_scope_exit", .{});
    lib.addIncludePath(boost_scope_exit_dep.path("include"));

    // boost-libraries-zig currently does not export program_options;
    // IfcGeom requires <boost/program_options.hpp>.
    const boost_program_options_dep = b.dependency("boost_program_options", .{});
    lib.addIncludePath(boost_program_options_dep.path("include"));

    // boost-libraries-zig currently does not export foreach;
    // IfcGeom requires <boost/foreach.hpp>.
    const boost_foreach_dep = b.dependency("boost_foreach", .{});
    lib.addIncludePath(boost_foreach_dep.path("include"));
}

fn collectCppFilesInDirectory(
    b: *std.Build,
    dir_path: []const u8,
    exclude_if_name_has_digit: bool,
) std.ArrayList([]const u8) {
    var files = std.ArrayList([]const u8).empty;

    var dir = std.fs.cwd().openDir(dir_path, .{ .iterate = true }) catch @panic("Failed to open source directory");
    defer dir.close();

    var it = dir.iterate();
    while (it.next() catch @panic("Failed to iterate source directory")) |entry| {
        if (entry.kind != .file) continue;
        if (!std.mem.endsWith(u8, entry.name, ".cpp")) continue;

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

fn appendCommonCppFlags(
    b: *std.Build,
    flags: *std.ArrayList([]const u8),
) void {
    flags.append(b.allocator, "-std=c++17") catch @panic("Out of memory building C++ flags");
    flags.append(b.allocator, "-D_DISABLE_CONSTEXPR_MUTEX_CONSTRUCTOR") catch @panic("Out of memory building C++ flags");
    flags.append(b.allocator, "-DBOOST_ALL_NO_LIB") catch @panic("Out of memory building C++ flags");
    // Keep bring-up aligned with CMake behavior where warnings are not fatal.
    flags.append(b.allocator, "-Wno-error") catch @panic("Out of memory building C++ flags");
}

fn appendSchemaHasFlags(
    b: *std.Build,
    flags: *std.ArrayList([]const u8),
    schemas: []const []const u8,
) void {
    for (schemas) |schema| {
        flags.append(b.allocator, b.fmt("-DHAS_SCHEMA_{s}", .{schema})) catch
            @panic("Out of memory building schema flags");
    }
}

fn parseSchemas(
    b: *std.Build,
    schemas_arg: []const u8,
    out_schemas: *std.ArrayList([]const u8),
) void {
    var schema_it = std.mem.splitScalar(u8, schemas_arg, ';');
    while (schema_it.next()) |raw| {
        const schema = std.mem.trim(u8, raw, " \t\r\n");
        if (schema.len == 0) continue;
        out_schemas.append(b.allocator, schema) catch @panic("Out of memory parsing -Dschemas");
    }

    if (out_schemas.items.len == 0) {
        @panic("No schema versions provided. Pass -Dschemas=4 or similar.");
    }
}

fn makeSchemaSeqMacro(
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

fn linkOpenCascadeLibraries(
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
