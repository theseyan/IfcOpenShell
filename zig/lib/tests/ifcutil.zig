const std = @import("std");
const ifcparse = @import("ifcparse");
const ifcutil = @import("ifcutil");

const sample_ifc_data = @embedFile("data/building_element_configuration_wall.ifc");
const classification_ifc_data = @embedFile("data/classification.ifc");
const property_value_ifc_data = @embedFile("data/property_value_variants.ifc");

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

fn openEmbeddedClassificationFile(allocator: std.mem.Allocator) !ifcparse.File {
    var tmp = std.testing.tmpDir(.{});
    defer tmp.cleanup();

    try tmp.dir.writeFile(.{
        .sub_path = "classification.ifc",
        .data = classification_ifc_data,
    });

    const tmp_dir_path = try tmp.dir.realpathAlloc(allocator, ".");
    defer allocator.free(tmp_dir_path);

    const tmp_file_path = try std.fs.path.join(allocator, &.{ tmp_dir_path, "classification.ifc" });
    defer allocator.free(tmp_file_path);

    return ifcparse.File.open(allocator, tmp_file_path, .{});
}

test "ifcutil element helpers and pset extraction" {
    const allocator = std.testing.allocator;

    var file = try openEmbeddedSampleFile(allocator);
    defer file.deinit();

    const wall = file.instanceById(45) orelse return error.TestUnexpectedNull;
    if (try ifcutil.element.getType(allocator, wall)) |wall_type| {
        try std.testing.expect(try wall_type.isA(allocator, "IfcTypeObject"));
    }

    if (try ifcutil.element.getPredefinedType(allocator, wall)) |predefined_type| {
        defer allocator.free(predefined_type);
        try std.testing.expect(predefined_type.len > 0);
    }

    const material_usage = (try ifcutil.element.getMaterial(allocator, wall, false, true)) orelse return error.TestUnexpectedNull;
    try std.testing.expect(try material_usage.isA(allocator, "IfcMaterialLayerSetUsage"));

    const material_set = (try ifcutil.element.getMaterial(allocator, wall, true, true)) orelse return error.TestUnexpectedNull;
    try std.testing.expect(try material_set.isA(allocator, "IfcMaterialLayerSet"));

    const container = (try ifcutil.element.getContainer(allocator, wall, true, null)) orelse return error.TestUnexpectedNull;
    try std.testing.expect(try container.isA(allocator, "IfcSpatialStructureElement"));
    const parent = (try ifcutil.element.getParent(allocator, wall)) orelse return error.TestUnexpectedNull;
    try std.testing.expectEqual(container.id(), parent.id());

    const aggregate = try ifcutil.element.getAggregate(allocator, wall);
    try std.testing.expect(aggregate == null);
    const nest = try ifcutil.element.getNest(allocator, wall);
    try std.testing.expect(nest == null);

    const pset = (try ifcutil.element.getPset(allocator, wall, "Pset_WallCommon", .{})) orelse return error.TestUnexpectedNull;
    defer {
        var owned = pset;
        owned.deinit(allocator);
    }
    const is_external = pset.propertyByName("IsExternal") orelse return error.TestUnexpectedNull;
    switch (is_external.value) {
        .primitive => |primitive| switch (primitive) {
            .bool => |value| try std.testing.expect(value),
            else => return error.TestUnexpectedResult,
        },
        else => return error.TestUnexpectedResult,
    }

    const pset_entity = (try ifcutil.element.getPsetEntity(allocator, wall, "Pset_WallCommon", .{})) orelse return error.TestUnexpectedNull;
    const thermal_property = (try ifcutil.element.getPropertyEntity(allocator, pset_entity, "ThermalTransmittance")) orelse return error.TestUnexpectedNull;
    var thermal_value = try ifcutil.element.getPropertyValue(allocator, thermal_property);
    defer thermal_value.deinit(allocator);
    switch (thermal_value) {
        .primitive => |primitive| switch (primitive) {
            .double => |value| try std.testing.expectApproxEqRel(@as(f64, 0.24), value, 1e-6),
            else => return error.TestUnexpectedResult,
        },
        else => return error.TestUnexpectedResult,
    }

    const all_psets = try ifcutil.element.getPsets(allocator, wall, .{});
    defer {
        for (all_psets) |*entry| {
            entry.deinit(allocator);
        }
        allocator.free(all_psets);
    }
    try std.testing.expect(all_psets.len >= 1);
}

test "ifcutil property value extraction variants" {
    const allocator = std.testing.allocator;

    var file = try ifcparse.File.openFromMemory(property_value_ifc_data);
    defer file.deinit();

    const list_property = file.instanceById(1) orelse return error.TestUnexpectedNull;
    var list_value = try ifcutil.element.getPropertyValue(allocator, list_property);
    defer list_value.deinit(allocator);
    switch (list_value) {
        .primitive_list => |values| {
            try std.testing.expectEqual(@as(usize, 2), values.len);
            var found_a = false;
            var found_b = false;
            for (values) |value| {
                switch (value) {
                    .string => |v| {
                        if (std.mem.eql(u8, v, "A")) found_a = true;
                        if (std.mem.eql(u8, v, "B")) found_b = true;
                    },
                    else => return error.TestUnexpectedResult,
                }
            }
            try std.testing.expect(found_a);
            try std.testing.expect(found_b);
        },
        else => return error.TestUnexpectedResult,
    }

    const enum_property = file.instanceById(2) orelse return error.TestUnexpectedNull;
    var enum_value = try ifcutil.element.getPropertyValue(allocator, enum_property);
    defer enum_value.deinit(allocator);
    switch (enum_value) {
        .primitive_list => |values| try std.testing.expectEqual(@as(usize, 2), values.len),
        else => return error.TestUnexpectedResult,
    }

    const bounded_property = file.instanceById(3) orelse return error.TestUnexpectedNull;
    var bounded_value = try ifcutil.element.getPropertyValue(allocator, bounded_property);
    defer bounded_value.deinit(allocator);
    switch (bounded_value) {
        .primitive_list => |values| try std.testing.expect(values.len >= 2),
        else => return error.TestUnexpectedResult,
    }

    const table_property = file.instanceById(4) orelse return error.TestUnexpectedNull;
    var table_value = try ifcutil.element.getPropertyValue(allocator, table_property);
    defer table_value.deinit(allocator);
    switch (table_value) {
        .table => |table| {
            try std.testing.expectEqual(@as(usize, 2), table.defining_values.len);
            try std.testing.expectEqual(@as(usize, 2), table.defined_values.len);
        },
        else => return error.TestUnexpectedResult,
    }
}

test "ifcutil classification helpers and unit helpers" {
    const allocator = std.testing.allocator;

    var classification_file = try openEmbeddedClassificationFile(allocator);
    defer classification_file.deinit();

    const library = classification_file.instanceById(1) orelse return error.TestUnexpectedNull;
    const direct_references = try ifcutil.classification.getReferences(allocator, library, true);
    defer allocator.free(direct_references);
    try std.testing.expect(direct_references.len >= 1);
    try std.testing.expect(try direct_references[0].isA(allocator, "IfcClassification"));

    const classification_reference = classification_file.instanceById(9) orelse return error.TestUnexpectedNull;
    const resolved_classification = (try ifcutil.classification.getClassification(allocator, classification_reference)) orelse return error.TestUnexpectedNull;
    try std.testing.expectEqual(@as(u32, 2), resolved_classification.id());

    const inherited_references = try ifcutil.classification.getInheritedReferences(allocator, classification_reference);
    defer allocator.free(inherited_references);
    try std.testing.expectEqual(@as(usize, 3), inherited_references.len);
    try std.testing.expectEqual(@as(u32, 9), inherited_references[0].id());
    try std.testing.expectEqual(@as(u32, 8), inherited_references[1].id());
    try std.testing.expectEqual(@as(u32, 7), inherited_references[2].id());

    var sample_file = try openEmbeddedSampleFile(allocator);
    defer sample_file.deinit();
    const wall = sample_file.instanceById(45) orelse return error.TestUnexpectedNull;
    const pset = (try ifcutil.element.getPsetEntity(allocator, wall, "Pset_WallCommon", .{})) orelse return error.TestUnexpectedNull;
    const property = (try ifcutil.element.getPropertyEntity(allocator, pset, "ThermalTransmittance")) orelse return error.TestUnexpectedNull;
    if (try ifcutil.unit.getPropertyUnit(allocator, property, &sample_file)) |property_unit| {
        try std.testing.expect(property_unit.id() > 0);
    }

    try std.testing.expectEqualStrings("METRE", ifcutil.unit.siTypeName("LENGTHUNIT") orelse return error.TestUnexpectedNull);
    try std.testing.expect(ifcutil.unit.siTypeName("THIS_IS_NOT_A_UNIT") == null);
    try std.testing.expectApproxEqRel(@as(f64, 0.0254), ifcutil.unit.convert(1.0, null, "inch", null, "METRE"), 1e-9);

    const unit_scale = try ifcutil.unit.calculateUnitScale(allocator, &sample_file, "LENGTHUNIT");
    try std.testing.expect(unit_scale > 0.0);
}
