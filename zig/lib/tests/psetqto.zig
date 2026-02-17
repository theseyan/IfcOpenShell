const std = @import("std");
const psetqto = @import("psetqto");

const ifc4_template_path = "zig/lib/psetqto/Pset_IFC4_ADD2.ifc";

extern fn ifcopenshell_psetqto_load_template_from_file(
    schema_name: ?[*:0]const u8,
    path: ?[*:0]const u8,
) c_int;
extern fn ifcopenshell_psetqto_unload_template(schema_name: ?[*:0]const u8) c_int;
extern fn ifcopenshell_psetqto_is_template_loaded(schema_name: ?[*:0]const u8) c_int;

fn loadIfc4TemplateFromFile(allocator: std.mem.Allocator) !void {
    psetqto.deinit();
    try psetqto.loadTemplateFromFile(allocator, "IFC4", ifc4_template_path);
}

fn loadIfc4TemplateFromMemory(allocator: std.mem.Allocator) !void {
    psetqto.deinit();
    const bytes = try std.fs.cwd().readFileAlloc(allocator, ifc4_template_path, 16 * 1024 * 1024);
    defer allocator.free(bytes);
    try psetqto.loadTemplateFromMemory("IFC4", bytes);
}

test "psetqto schema index from valid names" {
    try std.testing.expect(psetqto.SchemaIndex.fromName("IFC4") != null);
    try std.testing.expect(psetqto.SchemaIndex.fromName("IFC2X3") != null);
    try std.testing.expect(psetqto.SchemaIndex.fromName("IFC4X3") != null);
    try std.testing.expect(psetqto.SchemaIndex.fromName("IFC4X3_ADD2") != null);
}

test "psetqto schema index from invalid name returns null" {
    try std.testing.expect(psetqto.SchemaIndex.fromName("IFC99") == null);
    try std.testing.expect(psetqto.SchemaIndex.fromName("") == null);
}

test "psetqto schema index case insensitive" {
    try std.testing.expect(psetqto.SchemaIndex.fromName("ifc4") != null);
    try std.testing.expect(psetqto.SchemaIndex.fromName("Ifc2x3") != null);
    try std.testing.expect(psetqto.SchemaIndex.fromName("ifc4x3_add2") != null);
}

test "psetqto schema query and file names" {
    try std.testing.expectEqualStrings("IFC4", psetqto.SchemaIndex.ifc4.querySchemaName());
    try std.testing.expectEqualStrings("IFC2X3", psetqto.SchemaIndex.ifc2x3.querySchemaName());
    try std.testing.expectEqualStrings("IFC4X3_ADD2", psetqto.SchemaIndex.ifc4x3.querySchemaName());

    try std.testing.expectEqualStrings("IFC4", psetqto.SchemaIndex.ifc4.fileSchemaName());
    try std.testing.expectEqualStrings("IFC4X3_ADD2", psetqto.SchemaIndex.ifc2x3.fileSchemaName());
    try std.testing.expectEqualStrings("IFC4X3_ADD2", psetqto.SchemaIndex.ifc4x3.fileSchemaName());
}

test "psetqto load from file marks template as loaded" {
    const allocator = std.testing.allocator;
    try loadIfc4TemplateFromFile(allocator);
    defer psetqto.deinit();
    try std.testing.expect(psetqto.isTemplateLoaded("IFC4"));
}

test "psetqto load from memory marks template as loaded" {
    const allocator = std.testing.allocator;
    try loadIfc4TemplateFromMemory(allocator);
    defer psetqto.deinit();
    try std.testing.expect(psetqto.isTemplateLoaded("IFC4"));
}

test "psetqto C API template load lifecycle works" {
    psetqto.deinit();
    try std.testing.expectEqual(@as(c_int, 1), ifcopenshell_psetqto_load_template_from_file("IFC4", ifc4_template_path));
    try std.testing.expectEqual(@as(c_int, 1), ifcopenshell_psetqto_is_template_loaded("IFC4"));
    try std.testing.expectEqual(@as(c_int, 1), ifcopenshell_psetqto_unload_template("IFC4"));
    try std.testing.expectEqual(@as(c_int, 0), ifcopenshell_psetqto_is_template_loaded("IFC4"));
}

test "psetqto queries fail before template load" {
    const allocator = std.testing.allocator;
    psetqto.deinit();
    try std.testing.expectError(error.TemplateNotLoaded, psetqto.allTemplates(allocator, "IFC4"));
}

test "psetqto unsupported schema returns explicit error" {
    const allocator = std.testing.allocator;
    try std.testing.expectError(error.UnsupportedSchema, psetqto.allTemplates(allocator, "IFC99"));
    try std.testing.expectError(error.UnsupportedSchema, psetqto.loadTemplateFromFile(allocator, "IFC99", ifc4_template_path));
}

test "psetqto allTemplates returns non-empty list" {
    const allocator = std.testing.allocator;
    try loadIfc4TemplateFromFile(allocator);
    defer psetqto.deinit();

    const templates = try psetqto.allTemplates(allocator, "IFC4");
    defer allocator.free(templates);
    try std.testing.expect(templates.len > 0);
}

test "psetqto getByName finds known template" {
    const allocator = std.testing.allocator;
    try loadIfc4TemplateFromFile(allocator);
    defer psetqto.deinit();

    const tmpl = try psetqto.getByName(allocator, "IFC4", "Pset_WallCommon");
    try std.testing.expect(tmpl != null);
    const name = try tmpl.?.entity.getString(2);
    try std.testing.expectEqualStrings("Pset_WallCommon", name);
}

test "psetqto isTemplated returns true for known template and false for unknown" {
    const allocator = std.testing.allocator;
    try loadIfc4TemplateFromFile(allocator);
    defer psetqto.deinit();

    try std.testing.expect(try psetqto.isTemplated(allocator, "IFC4", "Pset_WallCommon"));
    try std.testing.expect(!(try psetqto.isTemplated(allocator, "IFC4", "Pset_NonExistent_12345")));
}

test "psetqto getApplicable for IfcWall includes Pset_WallCommon" {
    const allocator = std.testing.allocator;
    try loadIfc4TemplateFromFile(allocator);
    defer psetqto.deinit();

    const templates = try psetqto.getApplicable(allocator, "IFC4", "IfcWall", null, false, false);
    defer allocator.free(templates);

    var found_wall_common = false;
    for (templates) |tmpl| {
        const name = tmpl.entity.getString(2) catch continue;
        if (std.mem.eql(u8, name, "Pset_WallCommon")) {
            found_wall_common = true;
            break;
        }
    }
    try std.testing.expect(found_wall_common);
}

test "psetqto getApplicable filters pset_only and qto_only" {
    const allocator = std.testing.allocator;
    try loadIfc4TemplateFromFile(allocator);
    defer psetqto.deinit();

    const psets = try psetqto.getApplicable(allocator, "IFC4", "IfcWall", null, true, false);
    defer allocator.free(psets);
    for (psets) |tmpl| {
        const name = tmpl.entity.getString(2) catch continue;
        try std.testing.expect(!std.mem.startsWith(u8, name, "Qto_"));
    }

    const qtos = try psetqto.getApplicable(allocator, "IFC4", "IfcSpace", null, false, true);
    defer allocator.free(qtos);
    for (qtos) |tmpl| {
        const name = tmpl.entity.getString(2) catch continue;
        try std.testing.expect(std.mem.startsWith(u8, name, "Qto_"));
    }
}

test "psetqto getApplicable handles subtype matching" {
    const allocator = std.testing.allocator;
    try loadIfc4TemplateFromFile(allocator);
    defer psetqto.deinit();

    const wall_templates = try psetqto.getApplicable(allocator, "IFC4", "IfcWall", null, false, false);
    defer allocator.free(wall_templates);

    const std_case_templates = try psetqto.getApplicable(allocator, "IFC4", "IfcWallStandardCase", null, false, false);
    defer allocator.free(std_case_templates);

    try std.testing.expect(std_case_templates.len >= wall_templates.len);
}

test "psetqto getApplicable names count matches templates" {
    const allocator = std.testing.allocator;
    try loadIfc4TemplateFromFile(allocator);
    defer psetqto.deinit();

    const templates = try psetqto.getApplicable(allocator, "IFC4", "IfcDoor", null, false, false);
    defer allocator.free(templates);

    const names = try psetqto.getApplicableNames(allocator, "IFC4", "IfcDoor", null, false, false);
    defer {
        for (names) |n| allocator.free(n);
        allocator.free(names);
    }

    try std.testing.expectEqual(templates.len, names.len);
}

test "psetqto TYPE implementer agreement matches IfcBeamType via IfcBeam template" {
    const allocator = std.testing.allocator;
    try loadIfc4TemplateFromFile(allocator);
    defer psetqto.deinit();

    const templates = try psetqto.getApplicable(allocator, "IFC4", "IfcBeamType", null, false, false);
    defer allocator.free(templates);

    var found = false;
    for (templates) |tmpl| {
        const name = tmpl.entity.getString(2) catch continue;
        if (std.mem.eql(u8, name, "Pset_BeamCommon")) {
            found = true;
            break;
        }
    }
    try std.testing.expect(found);
}

test "psetqto deinit unloads templates" {
    const allocator = std.testing.allocator;
    try loadIfc4TemplateFromFile(allocator);
    try std.testing.expect(psetqto.isTemplateLoaded("IFC4"));

    psetqto.deinit();
    try std.testing.expect(!psetqto.isTemplateLoaded("IFC4"));
    try std.testing.expectError(error.TemplateNotLoaded, psetqto.allTemplates(allocator, "IFC4"));
}
