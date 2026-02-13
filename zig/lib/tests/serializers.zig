const std = @import("std");
const ifcparse = @import("ifcparse");
const ifcgeom = @import("ifcgeom");
const serializers = @import("serializers");

const sample_ifc_data = @embedFile("data/building_element_configuration_wall.ifc");

fn expectFileNonEmpty(path: []const u8) !void {
    const stat = try std.fs.cwd().statFile(path);
    try std.testing.expect(stat.size > 0);
}

test "serializers settings set/get and introspection" {
    const allocator = std.testing.allocator;

    var settings = try serializers.Settings.init();
    defer settings.deinit();

    try settings.setBool(allocator, "use-element-names", true);
    try settings.setInt(allocator, "digits", 8);
    try settings.setString(allocator, "base-uri", "https://example.com/base/");

    try std.testing.expect(try settings.getBool(allocator, "use-element-names"));
    try std.testing.expectEqual(@as(i32, 8), try settings.getInt(allocator, "digits"));

    const base_uri = try settings.getString(allocator, "base-uri");
    defer allocator.free(base_uri);
    try std.testing.expectEqualStrings("https://example.com/base/", base_uri);

    const digits_ty = try settings.settingType(allocator, "digits");
    defer allocator.free(digits_ty);
    try std.testing.expectEqualStrings("int", digits_ty);

    var names = try settings.settingNames();
    defer names.deinit();
    try std.testing.expect(names.len() > 0);

    var has_digits = false;
    names.reset();
    var it = names.iterator();
    while (it.next()) |name| {
        if (std.mem.eql(u8, name, "digits")) {
            has_digits = true;
            break;
        }
    }
    try std.testing.expect(has_digits);
}

test "serializers export geometry and schema outputs" {
    const allocator = std.testing.allocator;

    var file = try ifcparse.File.openFromMemory(sample_ifc_data);
    defer file.deinit();

    var geom_settings = try ifcgeom.Settings.init();
    defer geom_settings.deinit();
    try geom_settings.setBool("use-world-coords", true);

    var serializer_settings = try serializers.Settings.init();
    defer serializer_settings.deinit();
    try serializer_settings.setBool(allocator, "use-element-step-ids", true);
    try serializer_settings.setInt(allocator, "digits", 6);

    var tmp = std.testing.tmpDir(.{});
    defer tmp.cleanup();

    const tmp_dir_path = try tmp.dir.realpathAlloc(allocator, ".");
    defer allocator.free(tmp_dir_path);

    const obj_path = try std.fs.path.join(allocator, &.{ tmp_dir_path, "model.obj" });
    defer allocator.free(obj_path);
    const mtl_path = try std.fs.path.join(allocator, &.{ tmp_dir_path, "model.mtl" });
    defer allocator.free(mtl_path);
    const svg_path = try std.fs.path.join(allocator, &.{ tmp_dir_path, "model.svg" });
    defer allocator.free(svg_path);
    const ttl_path = try std.fs.path.join(allocator, &.{ tmp_dir_path, "model.ttl" });
    defer allocator.free(ttl_path);
    const xml_path = try std.fs.path.join(allocator, &.{ tmp_dir_path, "model.xml" });
    defer allocator.free(xml_path);

    try serializers.exportObj(
        &file,
        &geom_settings,
        &serializer_settings,
        allocator,
        obj_path,
        mtl_path,
        .{ .num_threads = 1 },
    );
    try expectFileNonEmpty(obj_path);
    try expectFileNonEmpty(mtl_path);

    try serializers.exportSvg(
        &file,
        &geom_settings,
        &serializer_settings,
        allocator,
        svg_path,
        .{ .num_threads = 1 },
    );
    try expectFileNonEmpty(svg_path);

    // TtlWktSerializer requires POLYHEDRON_WITH_HOLES triangulation output.
    try geom_settings.setInt("triangulation-type", 2);
    try serializers.exportTtl(
        &file,
        &geom_settings,
        &serializer_settings,
        allocator,
        ttl_path,
        .{ .num_threads = 1 },
    );
    try expectFileNonEmpty(ttl_path);

    try serializers.exportXml(&file, allocator, xml_path);
    try expectFileNonEmpty(xml_path);

    // Restore default triangle mesh output for serializers that do not support
    // polyhedron-with-holes output.
    try geom_settings.setInt("triangulation-type", 0);

    if (serializers.hasGltf()) {
        const gltf_path = try std.fs.path.join(allocator, &.{ tmp_dir_path, "model.glb" });
        defer allocator.free(gltf_path);

        try serializers.exportGltf(
            &file,
            &geom_settings,
            &serializer_settings,
            allocator,
            gltf_path,
            .{ .num_threads = 1 },
        );
        try expectFileNonEmpty(gltf_path);
    } else {
        try std.testing.expectError(
            serializers.QueryError.Unsupported,
            serializers.exportGltf(
                &file,
                &geom_settings,
                &serializer_settings,
                allocator,
                "unused.glb",
                .{},
            ),
        );
    }

    if (serializers.hasJson()) {
        const json_path = try std.fs.path.join(allocator, &.{ tmp_dir_path, "model.json" });
        defer allocator.free(json_path);

        try serializers.exportJson(&file, allocator, json_path);
        try expectFileNonEmpty(json_path);
    } else {
        try std.testing.expectError(
            serializers.QueryError.Unsupported,
            serializers.exportJson(&file, allocator, "unused.json"),
        );
    }
}
