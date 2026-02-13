const std = @import("std");
const ifcparse = @import("ifcparse");
const ifcgeom = @import("ifcgeom");

const sample_ifc_data = @embedFile("data/building_element_configuration_wall.ifc");

test "ifcgeom settings set/get and introspection" {
    const allocator = std.testing.allocator;

    var settings = try ifcgeom.Settings.init();
    defer settings.deinit();

    try settings.setBool("use-world-coords", true);
    try settings.setInt("iterator-output", 0);
    try settings.setDouble("mesher-linear-deflection", 0.25);
    try settings.setIntSet(allocator, "context-ids", &.{ 1, 2, 3 });
    try settings.setStringSet(allocator, "context-types", &.{ "Model", "Plan" });
    try settings.setDoubleVector(allocator, "model-offset", &.{ 1.0, 2.0, 3.0 });

    try std.testing.expect(try settings.getBool(allocator, "use-world-coords"));
    try std.testing.expectEqual(@as(i32, 0), try settings.getInt(allocator, "iterator-output"));
    try std.testing.expectEqual(@as(f64, 0.25), try settings.getDouble(allocator, "mesher-linear-deflection"));

    const iter_output_type = try settings.settingType(allocator, "iterator-output");
    defer allocator.free(iter_output_type);
    try std.testing.expectEqualStrings("IteratorOutputOptions", iter_output_type);

    var names = try settings.settingNames();
    defer names.deinit();
    try std.testing.expect(names.len() > 0);

    var has_iterator_output = false;
    names.reset();
    var it = names.iterator();
    while (it.next()) |name| {
        if (std.mem.eql(u8, name, "iterator-output")) {
            has_iterator_output = true;
            break;
        }
    }
    try std.testing.expect(has_iterator_output);
}

test "ifcgeom iterator triangulated mesh extraction" {
    const allocator = std.testing.allocator;

    var file = try ifcparse.File.openFromMemory(sample_ifc_data);
    defer file.deinit();

    var settings = try ifcgeom.Settings.init();
    defer settings.deinit();
    try settings.setInt("iterator-output", 0);

    var iterator = try ifcgeom.Iterator.init(&file, &settings, allocator, .{ .num_threads = 1 });
    defer iterator.deinit();

    const initialized = try iterator.initialize();
    try std.testing.expect(initialized);

    var saw_mesh = false;
    var previous_progress: i32 = -1;

    while (true) {
        try std.testing.expectEqual(ifcgeom.ElementKind.triangulation, iterator.currentKind());

        var mesh = try iterator.currentMesh(allocator);
        defer mesh.deinit(allocator);

        try std.testing.expect(mesh.id > 0);
        try std.testing.expect(mesh.verts.len > 0);
        try std.testing.expect(mesh.faces.len > 0);
        try std.testing.expectEqual(@as(usize, 16), mesh.transform.len);
        saw_mesh = true;

        const progress = iterator.progress();
        try std.testing.expect(progress >= previous_progress);
        previous_progress = progress;

        if (!(try iterator.next())) break;
    }

    try std.testing.expect(saw_mesh);
    try std.testing.expect(!(iterator.hadError()));
    try std.testing.expect((try iterator.unitName()).len > 0);
    try std.testing.expect((try iterator.unitMagnitude()) > 0.0);
}

test "ifcgeom iterator id filtering and bounds" {
    const allocator = std.testing.allocator;

    var file = try ifcparse.File.openFromMemory(sample_ifc_data);
    defer file.deinit();

    var settings = try ifcgeom.Settings.init();
    defer settings.deinit();
    try settings.setInt("iterator-output", 0);

    var iterator = try ifcgeom.Iterator.initWithIdFilter(
        &file,
        &settings,
        allocator,
        .{ .num_threads = 1 },
        &.{45},
        .include,
    );
    defer iterator.deinit();

    const initialized = try iterator.initialize();
    try std.testing.expect(initialized);

    var count: usize = 0;
    while (true) {
        var mesh = try iterator.currentMesh(allocator);
        defer mesh.deinit(allocator);

        try std.testing.expectEqual(@as(i32, 45), mesh.id);
        count += 1;

        if (!(try iterator.next())) break;
    }

    try std.testing.expect(count >= 1);

    try iterator.computeBounds(false);
    const bounds_min = try iterator.boundsMin();
    const bounds_max = try iterator.boundsMax();

    try std.testing.expect(bounds_min[0] <= bounds_max[0]);
    try std.testing.expect(bounds_min[1] <= bounds_max[1]);
    try std.testing.expect(bounds_min[2] <= bounds_max[2]);
}

test "ifcgeom current mesh before initialize returns error" {
    const allocator = std.testing.allocator;

    var file = try ifcparse.File.openFromMemory(sample_ifc_data);
    defer file.deinit();

    var settings = try ifcgeom.Settings.init();
    defer settings.deinit();

    var iterator = try ifcgeom.Iterator.init(&file, &settings, allocator, .{});
    defer iterator.deinit();

    try std.testing.expectError(ifcgeom.QueryError.QueryFailed, iterator.currentMesh(allocator));
}
