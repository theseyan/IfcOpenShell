const std = @import("std");
const ifcparse = @import("ifcparse");

const sample_ifc_data = @embedFile("data/building_element_configuration_wall.ifc");

fn openEmbeddedSampleFile(allocator: std.mem.Allocator) !ifcparse.File {
    var tmp = std.testing.tmpDir(.{});
    defer tmp.cleanup();

    try tmp.dir.writeFile(.{
        .sub_path = "sample.ifc",
        .data = sample_ifc_data,
    });

    const tmp_dir_path = try tmp.dir.realpathAlloc(allocator, ".");
    defer allocator.free(tmp_dir_path);

    const tmp_file_path = try std.fs.path.join(allocator, &.{ tmp_dir_path, "sample.ifc" });
    defer allocator.free(tmp_file_path);

    return ifcparse.File.open(allocator, tmp_file_path, .{});
}

test "ifcparse version is non-empty" {
    try std.testing.expect(ifcparse.version().len > 0);
}

test "ifcparse schema names and timestamp helpers" {
    try std.testing.expect((try ifcparse.createTimestamp()).len > 0);

    var schema_names = try ifcparse.schemaNames();
    defer schema_names.deinit();

    try std.testing.expect(schema_names.len() > 0);

    var found_ifc4_family = false;
    schema_names.reset();
    var iter = schema_names.iterator();
    while (iter.next()) |schema_name| {
        if (std.mem.startsWith(u8, schema_name, "IFC4")) {
            found_ifc4_family = true;
            break;
        }
    }
    try std.testing.expect(found_ifc4_family);
}

test "ifcparse open from memory and enumerate entities" {
    var file = try ifcparse.File.openFromMemory(sample_ifc_data);
    defer file.deinit();

    const schema_name = file.schemaName() orelse return error.TestUnexpectedNull;
    try std.testing.expectEqualStrings("IFC4", schema_name);

    var all_entities = try file.entities();
    defer all_entities.deinit();

    try std.testing.expect(all_entities.len() > 0);
    try std.testing.expectEqual(file.entityCount(), all_entities.len());
}

test "ifcparse create empty serialize and write spf" {
    const allocator = std.testing.allocator;

    var empty_file = try ifcparse.File.createEmpty(allocator, "IFC4");
    defer empty_file.deinit();

    const schema_name = empty_file.schemaName() orelse return error.TestUnexpectedNull;
    try std.testing.expectEqualStrings("IFC4", schema_name);

    const empty_spf = try empty_file.toSpf();
    try std.testing.expect(std.mem.indexOf(u8, empty_spf, "ISO-10303-21") != null);
    try std.testing.expect(std.mem.indexOf(u8, empty_spf, "END-ISO-10303-21") != null);

    var source_file = try openEmbeddedSampleFile(allocator);
    defer source_file.deinit();

    var tmp = std.testing.tmpDir(.{});
    defer tmp.cleanup();

    const tmp_dir_path = try tmp.dir.realpathAlloc(allocator, ".");
    defer allocator.free(tmp_dir_path);

    const output_ifc_path = try std.fs.path.join(allocator, &.{ tmp_dir_path, "roundtrip.ifc" });
    defer allocator.free(output_ifc_path);

    const guessed_type = try ifcparse.guessFileType(allocator, output_ifc_path);
    try std.testing.expectEqual(ifcparse.FileType.ifcspf, guessed_type);

    try source_file.writeSpf(allocator, output_ifc_path);

    var reopened = try ifcparse.File.open(allocator, output_ifc_path, .{});
    defer reopened.deinit();

    try std.testing.expectEqual(source_file.entityCount(), reopened.entityCount());
}

test "ifcparse unit queries" {
    const allocator = std.testing.allocator;

    var file = try openEmbeddedSampleFile(allocator);
    defer file.deinit();

    const length_scale = try file.unitScale(allocator, "LENGTHUNIT");
    try std.testing.expect(length_scale > 0.0);

    const length_unit_entity = (try file.unitEntity(allocator, "LENGTHUNIT")) orelse return error.TestUnexpectedNull;
    try std.testing.expect(length_unit_entity.id() > 0);
}

test "ifcparse entity argument metadata and string editing" {
    const allocator = std.testing.allocator;

    var file = try openEmbeddedSampleFile(allocator);
    defer file.deinit();

    const project = file.instanceById(1) orelse return error.TestUnexpectedNull;

    try std.testing.expect(project.argumentCount() > 0);

    const global_id_index = try project.argumentIndex(allocator, "GlobalId");
    try std.testing.expectEqual(@as(usize, 0), global_id_index);
    try std.testing.expectEqual(ifcparse.ArgumentType.string, project.argumentDeclaredType(global_id_index));

    const global_id = try project.getString(global_id_index);
    try std.testing.expect(global_id.len > 0);

    const name_index = try project.argumentIndex(allocator, "Name");
    try project.setString(allocator, name_index, "IfcOpenShell Zig");
    const updated_name = try project.getString(name_index);
    try std.testing.expectEqualStrings("IfcOpenShell Zig", updated_name);

    try project.setNull(name_index);
    try std.testing.expect(try project.argumentIsNull(name_index));

    const project_spf = try project.toSpf(true);
    try std.testing.expect(std.mem.indexOf(u8, project_spf, "IFCPROJECT") != null);
}

test "ifcparse entity argument aggregates and references editing" {
    const allocator = std.testing.allocator;

    var file = try ifcparse.File.createEmpty(allocator, "IFC4");
    defer file.deinit();

    const point_a = try file.createEntityByType(allocator, "IfcCartesianPoint");
    const point_b = try file.createEntityByType(allocator, "IfcCartesianPoint");

    const coordinates_index = try point_a.argumentIndex(allocator, "Coordinates");
    try std.testing.expectEqual(ifcparse.ArgumentType.aggregate_of_double, point_a.argumentDeclaredType(coordinates_index));
    try point_a.setDoubleList(coordinates_index, &.{ 1.0, 2.0, 3.0 });
    try point_b.setDoubleList(coordinates_index, &.{ 4.0, 5.0, 6.0 });

    var coordinates = try point_a.getDoubleList(coordinates_index);
    defer coordinates.deinit();
    const coords_slice = try coordinates.toOwnedSlice(allocator);
    defer allocator.free(coords_slice);
    try std.testing.expectEqual(@as(usize, 3), coords_slice.len);
    try std.testing.expectEqual(@as(f64, 1.0), coords_slice[0]);

    const axis = try file.createEntityByType(allocator, "IfcAxis2Placement3D");
    const location_index = try axis.argumentIndex(allocator, "Location");
    try axis.setEntity(location_index, point_a);

    const resolved_location = (try axis.getEntity(location_index)) orelse return error.TestUnexpectedNull;
    try std.testing.expectEqual(point_a.id(), resolved_location.id());

    const polyline = try file.createEntityByType(allocator, "IfcPolyline");
    const points_index = try polyline.argumentIndex(allocator, "Points");
    try polyline.setEntityList(allocator, points_index, &.{ point_a, point_b });

    var points = try polyline.getEntityList(points_index);
    defer points.deinit();
    try std.testing.expectEqual(@as(usize, 2), points.len());
}

test "ifcparse file add_entity and explicit id creation" {
    const allocator = std.testing.allocator;

    var source = try openEmbeddedSampleFile(allocator);
    defer source.deinit();

    var target = try ifcparse.File.createEmpty(allocator, "IFC4");
    defer target.deinit();

    const source_project = source.instanceById(1) orelse return error.TestUnexpectedNull;
    const added_project = try target.addEntity(source_project, null);

    const added_type_name = added_project.typeName() orelse return error.TestUnexpectedNull;
    try std.testing.expectEqualStrings("IfcProject", added_type_name);
    try std.testing.expect(target.entityCount() >= 1);

    const explicit_id_entity = try target.createEntityByTypeWithId(allocator, "IfcBuildingElementProxy", 9001);
    try std.testing.expectEqual(@as(u32, 9001), explicit_id_entity.id());
}

test "ifcparse entity queries via id and type" {
    const allocator = std.testing.allocator;

    var file = try openEmbeddedSampleFile(allocator);
    defer file.deinit();

    const schema_name = file.schemaName() orelse return error.TestUnexpectedNull;
    try std.testing.expectEqualStrings("IFC4", schema_name);
    try std.testing.expect(file.entityCount() > 0);

    const project = file.instanceById(1) orelse return error.TestUnexpectedNull;
    try std.testing.expectEqual(@as(u32, 1), project.id());
    const project_type_name = project.typeName() orelse return error.TestUnexpectedNull;
    try std.testing.expectEqualStrings("IfcProject", project_type_name);
    try std.testing.expect(try project.isA(allocator, "IfcContext"));
    try std.testing.expect(try project.isA(allocator, "IFCPROJECT"));
    try std.testing.expect(!(try project.isA(allocator, "IfcWall")));

    try std.testing.expect(file.instanceById(999_999) == null);

    var projects = try file.instancesByType(allocator, "IfcProject", true);
    defer projects.deinit();
    try std.testing.expectEqual(@as(usize, 1), projects.len());
    const first_project = projects.at(0) orelse return error.TestUnexpectedNull;
    try std.testing.expectEqual(@as(u32, 1), first_project.id());

    projects.reset();
    var projects_iter = projects.iterator();
    const iter_project = projects_iter.next() orelse return error.TestUnexpectedNull;
    try std.testing.expectEqual(@as(u32, 1), iter_project.id());
    try std.testing.expect(projects_iter.next() == null);

    var building_elements = try file.instancesByType(allocator, "IfcBuildingElement", true);
    defer building_elements.deinit();
    try std.testing.expect(building_elements.len() >= 1);

    const elements_slice = try building_elements.toOwnedSlice(allocator);
    defer allocator.free(elements_slice);
    try std.testing.expectEqual(building_elements.len(), elements_slice.len);
}

test "ifcparse instances_by_type exact matching and guid lookup" {
    const allocator = std.testing.allocator;

    var file = try openEmbeddedSampleFile(allocator);
    defer file.deinit();

    var walls_exact = try file.instancesByType(allocator, "IfcWallStandardCase", false);
    defer walls_exact.deinit();
    try std.testing.expectEqual(@as(usize, 1), walls_exact.len());
    const wall = walls_exact.at(0) orelse return error.TestUnexpectedNull;
    try std.testing.expectEqual(@as(u32, 45), wall.id());

    const by_guid = (try file.instanceByGuid(allocator, "28hypXUBvBefc20SI8kfA$")) orelse return error.TestUnexpectedNull;
    try std.testing.expectEqual(@as(u32, 1), by_guid.id());

    var walls_with_subtypes = try file.instancesByType(allocator, "IfcWall", true);
    defer walls_with_subtypes.deinit();
    try std.testing.expect(walls_with_subtypes.len() >= 1);
}

test "ifcparse inverse queries and traversal" {
    const allocator = std.testing.allocator;

    var file = try openEmbeddedSampleFile(allocator);
    defer file.deinit();

    var refs = try file.instancesByReference(45);
    defer refs.deinit();
    try std.testing.expect(refs.len() >= 1);

    var inverse_all = try file.getInverse(allocator, 45, null, -1);
    defer inverse_all.deinit();
    try std.testing.expectEqual(refs.len(), inverse_all.len());

    var inverse_filtered = try file.getInverse(allocator, 45, "IfcRelContainedInSpatialStructure", -1);
    defer inverse_filtered.deinit();
    try std.testing.expect(inverse_filtered.len() >= 1);

    const total_inverses = file.getTotalInverses(45);
    try std.testing.expect(total_inverses >= inverse_all.len());

    var inverse_indices = try file.getInverseIndices(45);
    defer inverse_indices.deinit();
    try std.testing.expectEqual(inverse_all.len(), inverse_indices.len());

    const inverse_idx_values = try inverse_indices.toOwnedSlice(allocator);
    defer allocator.free(inverse_idx_values);
    try std.testing.expectEqual(inverse_indices.len(), inverse_idx_values.len);

    var traverse_depth_first = try file.traverseById(45, -1, false);
    defer traverse_depth_first.deinit();
    try std.testing.expect(traverse_depth_first.len() >= 1);
    const first_depth = traverse_depth_first.at(0) orelse return error.TestUnexpectedNull;
    try std.testing.expectEqual(@as(u32, 45), first_depth.id());

    var traverse_breadth_first = try file.traverseById(45, -1, true);
    defer traverse_breadth_first.deinit();
    try std.testing.expect(traverse_breadth_first.len() >= 1);
    const first_breadth = traverse_breadth_first.at(0) orelse return error.TestUnexpectedNull;
    try std.testing.expectEqual(@as(u32, 45), first_breadth.id());
}

test "ifcparse type enumeration" {
    const allocator = std.testing.allocator;

    var file = try openEmbeddedSampleFile(allocator);
    defer file.deinit();

    var types = try file.types();
    defer types.deinit();

    try std.testing.expect(types.len() >= 1);

    var found_project = false;
    var found_wall = false;

    types.reset();
    var iter = types.iterator();
    while (iter.next()) |type_ref| {
        const name = type_ref.name() orelse continue;
        if (std.mem.eql(u8, name, "IfcProject")) found_project = true;
        if (std.mem.eql(u8, name, "IfcWallStandardCase")) found_wall = true;
    }

    try std.testing.expect(found_project);
    try std.testing.expect(found_wall);
}

test "ifcparse mutation APIs create remove and batching" {
    const allocator = std.testing.allocator;

    var file = try openEmbeddedSampleFile(allocator);
    defer file.deinit();

    const initial_count = file.entityCount();
    const initial_max_id = file.maxId();
    try std.testing.expect(initial_count > 0);
    try std.testing.expect(initial_max_id > 0);

    const created = try file.createEntityByType(allocator, "IfcBuildingElementProxy");
    const created_id: i32 = @intCast(created.id());
    try std.testing.expect(created.id() > initial_max_id);
    try std.testing.expectEqual(initial_count + 1, file.entityCount());

    const created_lookup = file.instanceById(created_id) orelse return error.TestUnexpectedNull;
    const created_type_name = created_lookup.typeName() orelse return error.TestUnexpectedNull;
    try std.testing.expectEqualStrings("IfcBuildingElementProxy", created_type_name);

    try file.removeEntityById(created_id);
    try std.testing.expectEqual(initial_count, file.entityCount());
    try std.testing.expect(file.instanceById(created_id) == null);

    try std.testing.expectError(ifcparse.QueryError.QueryFailed, file.removeEntityById(created_id));

    const batch_e1 = try file.createEntityByType(allocator, "IfcBuildingElementProxy");
    const batch_e2 = try file.createEntityByType(allocator, "IfcBuildingElementProxy");
    const batch_id1: i32 = @intCast(batch_e1.id());
    const batch_id2: i32 = @intCast(batch_e2.id());
    try std.testing.expectEqual(initial_count + 2, file.entityCount());

    try file.batchBegin();
    try file.removeEntityById(batch_id1);
    try file.removeEntityById(batch_id2);

    try std.testing.expect(file.instanceById(batch_id1) != null);
    try std.testing.expect(file.instanceById(batch_id2) != null);

    try file.batchEnd();
    try std.testing.expect(file.instanceById(batch_id1) == null);
    try std.testing.expect(file.instanceById(batch_id2) == null);
    try std.testing.expectEqual(initial_count, file.entityCount());

    try file.recalculateIdCounter();
    try std.testing.expect(file.maxId() > 0);
}
