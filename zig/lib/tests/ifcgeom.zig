const std = @import("std");
const ifcparse = @import("ifcparse");
const ifcgeom = @import("ifcgeom");

const sample_ifc_data = @embedFile("data/building_element_configuration_wall.ifc");

test "ifcgeom globalid filter initializes and yields current guid" {
    const allocator = std.testing.allocator;

    var file = try ifcparse.File.openFromMemory(sample_ifc_data);
    defer file.deinit();

    var settings = try ifcgeom.Settings.init();
    defer settings.deinit();
    try settings.setInt("iterator-output", 0);

    var guid_iter = try ifcgeom.Iterator.initWithGlobalIdFilter(
        &file,
        &settings,
        allocator,
        .{ .num_threads = 1 },
        &.{ "3ZYW59sxj8lei475l7EhLU" },
        .include,
    );
    defer guid_iter.deinit();
    try std.testing.expect(try guid_iter.initialize());
    try std.testing.expect((try guid_iter.currentGuid()).len > 0);
}

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

test "ifcgeom create mesh for id and entity" {
    const allocator = std.testing.allocator;

    var file = try ifcparse.File.openFromMemory(sample_ifc_data);
    defer file.deinit();

    var settings = try ifcgeom.Settings.init();
    defer settings.deinit();
    try settings.setInt("iterator-output", 0);

    var walls = try file.instancesByType(allocator, "IfcWall", true);
    defer walls.deinit();

    try std.testing.expect(walls.len() > 0);
    const wall = walls.at(0).?;
    const wall_id_i32: i32 = @intCast(wall.id());

    var mesh_by_id = try ifcgeom.createMeshById(
        &file,
        &settings,
        allocator,
        wall_id_i32,
        .{ .num_threads = 1 },
    );
    defer mesh_by_id.deinit(allocator);

    try std.testing.expectEqual(wall_id_i32, mesh_by_id.id);
    try std.testing.expect(mesh_by_id.verts.len > 0);
    try std.testing.expect(mesh_by_id.faces.len > 0);

    var mesh_by_entity = try ifcgeom.createMeshForEntity(
        &file,
        &settings,
        allocator,
        wall,
        .{ .num_threads = 1 },
    );
    defer mesh_by_entity.deinit(allocator);

    try std.testing.expectEqual(mesh_by_id.id, mesh_by_entity.id);
    try std.testing.expectEqual(mesh_by_id.verts.len, mesh_by_entity.verts.len);
    try std.testing.expectEqual(mesh_by_id.faces.len, mesh_by_entity.faces.len);
}

test "ifcgeom create mesh for missing id returns error" {
    const allocator = std.testing.allocator;

    var file = try ifcparse.File.openFromMemory(sample_ifc_data);
    defer file.deinit();

    var settings = try ifcgeom.Settings.init();
    defer settings.deinit();
    try settings.setInt("iterator-output", 0);

    try std.testing.expectError(
        ifcgeom.QueryError.QueryFailed,
        ifcgeom.createMeshById(&file, &settings, allocator, 999999999, .{}),
    );
}

test "ifcgeom create shape by id triangulated" {
    const allocator = std.testing.allocator;

    var file = try ifcparse.File.openFromMemory(sample_ifc_data);
    defer file.deinit();

    var settings = try ifcgeom.Settings.init();
    defer settings.deinit();
    try settings.setInt("iterator-output", 0);

    var walls = try file.instancesByType(allocator, "IfcWall", true);
    defer walls.deinit();
    try std.testing.expect(walls.len() > 0);
    const wall = walls.at(0).?;

    var shape = try ifcgeom.createShapeForEntity(&file, &settings, allocator, wall, .{ .num_threads = 1 });
    defer shape.deinit(allocator);

    switch (shape) {
        .triangulation => |mesh| {
            try std.testing.expect(mesh.id > 0);
            try std.testing.expect(mesh.verts.len > 0);
            try std.testing.expect(mesh.faces.len > 0);
        },
        else => return error.TestUnexpectedResult,
    }
}

test "ifcgeom create shape by id serialized" {
    const allocator = std.testing.allocator;

    var file = try ifcparse.File.openFromMemory(sample_ifc_data);
    defer file.deinit();

    var settings = try ifcgeom.Settings.init();
    defer settings.deinit();
    try settings.setInt("iterator-output", 2);

    var walls = try file.instancesByType(allocator, "IfcWall", true);
    defer walls.deinit();
    try std.testing.expect(walls.len() > 0);
    const wall = walls.at(0).?;

    var shape = try ifcgeom.createShapeForEntity(&file, &settings, allocator, wall, .{ .num_threads = 1 });
    defer shape.deinit(allocator);

    switch (shape) {
        .serialized => |serialized| {
            try std.testing.expect(serialized.id > 0);
            try std.testing.expect(serialized.brep_data.len > 0);
            try std.testing.expectEqual(@as(usize, 16), serialized.transform.len);
        },
        else => return error.TestUnexpectedResult,
    }
}

test "ifcgeom create shape by id native brep fallback serialization" {
    const allocator = std.testing.allocator;

    var file = try ifcparse.File.openFromMemory(sample_ifc_data);
    defer file.deinit();

    var settings = try ifcgeom.Settings.init();
    defer settings.deinit();
    try settings.setInt("iterator-output", 1);

    var walls = try file.instancesByType(allocator, "IfcWall", true);
    defer walls.deinit();
    try std.testing.expect(walls.len() > 0);
    const wall = walls.at(0).?;

    var shape = try ifcgeom.createShapeForEntity(&file, &settings, allocator, wall, .{ .num_threads = 1 });
    defer shape.deinit(allocator);

    switch (shape) {
        .brep => |serialized| {
            try std.testing.expect(serialized.id > 0);
            try std.testing.expect(serialized.brep_data.len > 0);
            try std.testing.expectEqual(@as(usize, 16), serialized.transform.len);
        },
        else => return error.TestUnexpectedResult,
    }
}

test "ifcgeom create_shape parity for placement transform" {
    const allocator = std.testing.allocator;

    var file = try ifcparse.File.openFromMemory(sample_ifc_data);
    defer file.deinit();

    var settings = try ifcgeom.Settings.init();
    defer settings.deinit();

    var placements = try file.instancesByType(allocator, "IfcLocalPlacement", true);
    defer placements.deinit();
    try std.testing.expect(placements.len() > 0);
    const placement = placements.at(0).?;

    var shape = try ifcgeom.createShapeAnyForEntity(&file, &settings, allocator, placement, .{ .num_threads = 1 });
    defer shape.deinit(allocator);

    switch (shape) {
        .transform => |m| {
            try std.testing.expectEqual(@as(usize, 16), m.len);
            try std.testing.expectEqual(@as(f64, 1.0), m[15]);
        },
        else => return error.TestUnexpectedResult,
    }
}

test "ifcgeom create_shape parity for representation item and map_shape repr" {
    const allocator = std.testing.allocator;

    var file = try ifcparse.File.openFromMemory(sample_ifc_data);
    defer file.deinit();

    var settings_tri = try ifcgeom.Settings.init();
    defer settings_tri.deinit();
    try settings_tri.setInt("iterator-output", 0);

    var settings_ser = try ifcgeom.Settings.init();
    defer settings_ser.deinit();
    try settings_ser.setInt("iterator-output", 2);

    var reps = try file.instancesByType(allocator, "IfcExtrudedAreaSolid", true);
    defer reps.deinit();
    try std.testing.expect(reps.len() > 0);
    const rep = reps.at(0).?;

    var tri_shape = try ifcgeom.createShapeAnyForEntity(&file, &settings_tri, allocator, rep, .{});
    defer tri_shape.deinit(allocator);
    switch (tri_shape) {
        .triangulation => |mesh| {
            try std.testing.expect(mesh.verts.len > 0);
            try std.testing.expect(mesh.faces.len > 0);
        },
        else => return error.TestUnexpectedResult,
    }

    var ser_shape = try ifcgeom.createShapeAnyForEntity(&file, &settings_ser, allocator, rep, .{});
    defer ser_shape.deinit(allocator);
    switch (ser_shape) {
        .serialized => |serialized| {
            try std.testing.expect(serialized.brep_data.len > 0);
            try std.testing.expectEqual(@as(usize, 16), serialized.transform.len);
        },
        else => return error.TestUnexpectedResult,
    }

    const mapped = try ifcgeom.mapShapeReprForEntity(&file, &settings_tri, allocator, rep);
    defer allocator.free(mapped);
    try std.testing.expect(mapped.len > 0);
}

test "ifcgeom iterator metadata and filtered constructors" {
    const allocator = std.testing.allocator;

    var file = try ifcparse.File.openFromMemory(sample_ifc_data);
    defer file.deinit();

    var settings = try ifcgeom.Settings.init();
    defer settings.deinit();
    try settings.setInt("iterator-output", 0);

    var iter = try ifcgeom.Iterator.init(&file, &settings, allocator, .{ .num_threads = 1 });
    defer iter.deinit();
    try std.testing.expect(try iter.initialize());

    const first_id = try iter.currentId();
    try std.testing.expect(first_id > 0);
    try std.testing.expect((try iter.currentParentId()) >= -1);
    try std.testing.expect((try iter.currentType()).len > 0);
    _ = try iter.currentGuid();
    try std.testing.expectEqual(@as(usize, 16), (try iter.currentTransform()).len);
    _ = try iter.log();
    try std.testing.expectEqual(@as(usize, 0), iter.lastError().len);

    var chosen_guid: ?[]u8 = null;
    while (true) {
        const guid = try iter.currentGuid();
        if (guid.len > 0) {
            chosen_guid = try allocator.dupe(u8, guid);
            break;
        }
        if (!(try iter.next())) break;
    }
    defer if (chosen_guid) |guid| allocator.free(guid);
    try std.testing.expect(chosen_guid != null);

    var type_iter = try ifcgeom.Iterator.initWithTypeFilter(
        &file,
        &settings,
        allocator,
        .{ .num_threads = 1 },
        &.{ "IfcWall" },
        .include,
    );
    defer type_iter.deinit();
    try std.testing.expect(try type_iter.initialize());
    try std.testing.expect(std.mem.indexOf(u8, try type_iter.currentType(), "IfcWall") != null);

    var guid_iter = try ifcgeom.Iterator.initWithGlobalIdFilter(
        &file,
        &settings,
        allocator,
        .{ .num_threads = 1 },
        &.{chosen_guid.?},
        .include,
    );
    defer guid_iter.deinit();
    try std.testing.expect(try guid_iter.initialize());
    try std.testing.expectEqualStrings(chosen_guid.?, try guid_iter.currentGuid());
    while (try guid_iter.next()) {
        try std.testing.expectEqualStrings(chosen_guid.?, try guid_iter.currentGuid());
    }
}

test "ifcgeom by-id shape helpers and serialized iterator payload" {
    const allocator = std.testing.allocator;

    var file = try ifcparse.File.openFromMemory(sample_ifc_data);
    defer file.deinit();

    var settings_tri = try ifcgeom.Settings.init();
    defer settings_tri.deinit();
    try settings_tri.setInt("iterator-output", 0);

    var walls = try file.instancesByType(allocator, "IfcWall", true);
    defer walls.deinit();
    try std.testing.expect(walls.len() > 0);
    const wall_id: i32 = @intCast(walls.at(0).?.id());

    var by_id = try ifcgeom.createShapeById(&file, &settings_tri, allocator, wall_id, .{});
    defer by_id.deinit(allocator);
    switch (by_id) {
        .triangulation => |mesh| try std.testing.expect(mesh.verts.len > 0),
        else => return error.TestUnexpectedResult,
    }

    var placements = try file.instancesByType(allocator, "IfcLocalPlacement", true);
    defer placements.deinit();
    try std.testing.expect(placements.len() > 0);
    const placement_id: i32 = @intCast(placements.at(0).?.id());
    var any_shape = try ifcgeom.createShapeAnyById(&file, &settings_tri, allocator, placement_id, .{});
    defer any_shape.deinit(allocator);
    switch (any_shape) {
        .transform => |m| try std.testing.expectEqual(@as(f64, 1.0), m[15]),
        else => return error.TestUnexpectedResult,
    }

    var reps = try file.instancesByType(allocator, "IfcExtrudedAreaSolid", true);
    defer reps.deinit();
    try std.testing.expect(reps.len() > 0);
    const rep_id: i32 = @intCast(reps.at(0).?.id());
    const mapped = try ifcgeom.mapShapeReprById(&file, &settings_tri, allocator, rep_id);
    defer allocator.free(mapped);
    try std.testing.expect(mapped.len > 0);

    var settings_ser = try ifcgeom.Settings.init();
    defer settings_ser.deinit();
    try settings_ser.setInt("iterator-output", 2);
    var ser_iter = try ifcgeom.Iterator.init(&file, &settings_ser, allocator, .{ .num_threads = 1 });
    defer ser_iter.deinit();
    try std.testing.expect(try ser_iter.initialize());
    try std.testing.expectEqual(ifcgeom.ElementKind.serialized, ser_iter.currentKind());
    var serialized = try ser_iter.currentSerialized(allocator);
    defer serialized.deinit(allocator);
    try std.testing.expect(serialized.id > 0);
    try std.testing.expect(serialized.brep_data.len > 0);
}

test "ifcgeom string setting APIs report invalid setting errors" {
    const allocator = std.testing.allocator;

    var settings = try ifcgeom.Settings.init();
    defer settings.deinit();

    try std.testing.expectError(
        ifcgeom.QueryError.QueryFailed,
        settings.setString(allocator, "non-existent-setting", "value"),
    );
    try std.testing.expectError(
        ifcgeom.QueryError.QueryFailed,
        settings.getString(allocator, "non-existent-setting"),
    );
}

test "ifcgeom tree add file and selection queries" {
    const allocator = std.testing.allocator;

    var file = try ifcparse.File.openFromMemory(sample_ifc_data);
    defer file.deinit();

    var settings = try ifcgeom.Settings.init();
    defer settings.deinit();
    try settings.setBool("use-world-coords", true);

    var tree = try ifcgeom.Tree.init();
    defer tree.deinit();
    try tree.addFile(&file, &settings);

    var walls = try file.instancesByType(allocator, "IfcWall", true);
    defer walls.deinit();
    try std.testing.expect(walls.len() > 0);
    const wall = walls.at(0).?;
    const wall_id: i32 = @intCast(wall.id());

    var selected_by_entity = try tree.selectByEntity(&file, wall, false, 0.0);
    defer selected_by_entity.deinit();
    try std.testing.expect(selected_by_entity.len() > 0);

    var has_wall_id = false;
    var sel_it = selected_by_entity.iterator();
    while (sel_it.next()) |id| {
        if (id == wall_id) {
            has_wall_id = true;
            break;
        }
    }
    try std.testing.expect(has_wall_id);

    var selected_by_id = try tree.selectById(&file, wall_id, false, 0.0);
    defer selected_by_id.deinit();
    try std.testing.expect(selected_by_id.len() > 0);

    const big_min = [3]f64{ -1.0e6, -1.0e6, -1.0e6 };
    const big_max = [3]f64{ 1.0e6, 1.0e6, 1.0e6 };
    var selected_big_box = try tree.selectBox(big_min, big_max, false);
    defer selected_big_box.deinit();
    try std.testing.expect(selected_big_box.len() >= selected_by_id.len());

    var wall_mesh = try ifcgeom.createMeshById(&file, &settings, allocator, wall_id, .{ .num_threads = 1 });
    defer wall_mesh.deinit(allocator);
    try std.testing.expect(wall_mesh.verts.len >= 3);

    const point = [3]f64{ wall_mesh.verts[0], wall_mesh.verts[1], wall_mesh.verts[2] };
    var selected_by_point = try tree.selectPoint(point, 0.01);
    defer selected_by_point.deinit();
    try std.testing.expect(selected_by_point.len() > 0);
}

test "ifcgeom tree clash queries" {
    const allocator = std.testing.allocator;

    var file = try ifcparse.File.openFromMemory(sample_ifc_data);
    defer file.deinit();

    var settings = try ifcgeom.Settings.init();
    defer settings.deinit();
    try settings.setBool("use-world-coords", true);

    var tree = try ifcgeom.Tree.init();
    defer tree.deinit();
    try tree.addFile(&file, &settings);

    const big_min = [3]f64{ -1.0e6, -1.0e6, -1.0e6 };
    const big_max = [3]f64{ 1.0e6, 1.0e6, 1.0e6 };
    var all_ids_list = try tree.selectBox(big_min, big_max, false);
    defer all_ids_list.deinit();

    const all_ids = try all_ids_list.toOwnedSlice(allocator);
    defer allocator.free(all_ids);
    try std.testing.expect(all_ids.len > 0);

    const sample_count: usize = @min(all_ids.len, 8);
    const ids = all_ids[0..sample_count];

    var intersection = try tree.clashIntersectionMany(&file, ids, ids, 0.002, true);
    defer intersection.deinit();

    var collision = try tree.clashCollisionMany(&file, ids, ids, false);
    defer collision.deinit();

    var clearance = try tree.clashClearanceMany(&file, ids, ids, 0.05, false);
    defer clearance.deinit();

    if (intersection.len() > 0) {
        const first = intersection.at(0).?;
        try std.testing.expect(first.a_id > 0);
        try std.testing.expect(first.b_id > 0);
    } else {
        try std.testing.expect(intersection.at(0) == null);
    }

    if (collision.len() > 0) {
        const first = collision.at(0).?;
        try std.testing.expect(first.a_id > 0);
        try std.testing.expect(first.b_id > 0);
    } else {
        try std.testing.expect(collision.at(0) == null);
    }

    if (clearance.len() > 0) {
        const first = clearance.at(0).?;
        try std.testing.expect(first.a_id > 0);
        try std.testing.expect(first.b_id > 0);
    } else {
        try std.testing.expect(clearance.at(0) == null);
    }
}
