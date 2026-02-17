const std = @import("std");
const psetqto = @import("psetqto");

// ---------------------------------------------------------------------------
// Schema availability
// ---------------------------------------------------------------------------

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

test "psetqto IFC4X3_ADD2 maps to ifc4x3" {
    const idx = psetqto.SchemaIndex.fromName("IFC4X3_ADD2").?;
    try std.testing.expectEqual(psetqto.SchemaIndex.ifc4x3, idx);
}

test "psetqto schema query names" {
    try std.testing.expectEqualStrings("IFC4", psetqto.SchemaIndex.ifc4.querySchemaName());
    try std.testing.expectEqualStrings("IFC2X3", psetqto.SchemaIndex.ifc2x3.querySchemaName());
    try std.testing.expectEqualStrings("IFC4X3_ADD2", psetqto.SchemaIndex.ifc4x3.querySchemaName());
}

test "psetqto schema file names" {
    try std.testing.expectEqualStrings("IFC4", psetqto.SchemaIndex.ifc4.fileSchemaName());
    try std.testing.expectEqualStrings("IFC4X3_ADD2", psetqto.SchemaIndex.ifc2x3.fileSchemaName());
    try std.testing.expectEqualStrings("IFC4X3_ADD2", psetqto.SchemaIndex.ifc4x3.fileSchemaName());
}

test "psetqto IFC4 availability" {
    const idx = psetqto.SchemaIndex.fromName("IFC4").?;
    try std.testing.expect(idx.isAvailable());
    try std.testing.expect(idx.templateData() != null);
}

test "psetqto IFC2X3 and IFC4X3 not available with default schemas" {
    const ifc2x3 = psetqto.SchemaIndex.fromName("IFC2X3").?;
    try std.testing.expect(!ifc2x3.isAvailable());
    try std.testing.expect(ifc2x3.templateData() == null);

    const ifc4x3 = psetqto.SchemaIndex.fromName("IFC4X3").?;
    try std.testing.expect(!ifc4x3.isAvailable());
    try std.testing.expect(ifc4x3.templateData() == null);
}

// ---------------------------------------------------------------------------
// allTemplates
// ---------------------------------------------------------------------------

test "psetqto allTemplates returns non-empty list" {
    const allocator = std.testing.allocator;
    const templates = try psetqto.allTemplates(allocator, "IFC4");
    defer allocator.free(templates);

    try std.testing.expect(templates.len > 0);
}

test "psetqto allTemplates with invalid schema returns error" {
    const allocator = std.testing.allocator;
    const result = psetqto.allTemplates(allocator, "IFC99");
    try std.testing.expectError(error.QueryFailed, result);
}

// ---------------------------------------------------------------------------
// getByName
// ---------------------------------------------------------------------------

test "psetqto getByName finds known template" {
    const allocator = std.testing.allocator;

    const tmpl = try psetqto.getByName(allocator, "IFC4", "Pset_WallCommon");
    try std.testing.expect(tmpl != null);

    const entity = tmpl.?.entity;
    const name = try entity.getString(2);
    try std.testing.expectEqualStrings("Pset_WallCommon", name);
}

test "psetqto getByName returns null for unknown name" {
    const allocator = std.testing.allocator;

    const tmpl = try psetqto.getByName(allocator, "IFC4", "Pset_NonExistent_12345");
    try std.testing.expect(tmpl == null);
}

test "psetqto getByName with invalid schema returns error" {
    const allocator = std.testing.allocator;
    const result = psetqto.getByName(allocator, "IFC99", "Pset_WallCommon");
    try std.testing.expectError(error.QueryFailed, result);
}

// ---------------------------------------------------------------------------
// isTemplated
// ---------------------------------------------------------------------------

test "psetqto isTemplated returns true for known template" {
    const allocator = std.testing.allocator;
    const result = try psetqto.isTemplated(allocator, "IFC4", "Pset_WallCommon");
    try std.testing.expect(result);
}

test "psetqto isTemplated returns false for unknown name" {
    const allocator = std.testing.allocator;
    const result = try psetqto.isTemplated(allocator, "IFC4", "Pset_NonExistent_12345");
    try std.testing.expect(!result);
}

// ---------------------------------------------------------------------------
// getApplicable
// ---------------------------------------------------------------------------

test "psetqto getApplicable for IfcWall returns results" {
    const allocator = std.testing.allocator;

    const templates = try psetqto.getApplicable(
        allocator,
        "IFC4",
        "IfcWall",
        null,
        false,
        false,
    );
    defer allocator.free(templates);

    try std.testing.expect(templates.len > 0);

    // Pset_WallCommon should be among the results
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

test "psetqto getApplicable with pset_only excludes Qto" {
    const allocator = std.testing.allocator;

    const templates = try psetqto.getApplicable(
        allocator,
        "IFC4",
        "IfcWall",
        null,
        true,
        false,
    );
    defer allocator.free(templates);

    for (templates) |tmpl| {
        const name = tmpl.entity.getString(2) catch continue;
        try std.testing.expect(!std.mem.startsWith(u8, name, "Qto_"));
    }
}

test "psetqto getApplicable with qto_only excludes Pset" {
    const allocator = std.testing.allocator;

    const templates = try psetqto.getApplicable(
        allocator,
        "IFC4",
        "IfcWall",
        null,
        false,
        true,
    );
    defer allocator.free(templates);

    for (templates) |tmpl| {
        const name = tmpl.entity.getString(2) catch continue;
        try std.testing.expect(!std.mem.startsWith(u8, name, "Pset_"));
    }
}

test "psetqto getApplicable for non-existent class returns empty" {
    const allocator = std.testing.allocator;

    const templates = try psetqto.getApplicable(
        allocator,
        "IFC4",
        "IfcNonExistentEntity_12345",
        null,
        false,
        false,
    );
    defer allocator.free(templates);

    try std.testing.expectEqual(@as(usize, 0), templates.len);
}

test "psetqto getApplicable with qto_only returns Qto templates" {
    const allocator = std.testing.allocator;

    // IfcSpace should have Qto templates
    const templates = try psetqto.getApplicable(
        allocator,
        "IFC4",
        "IfcSpace",
        null,
        false,
        true,
    );
    defer allocator.free(templates);

    try std.testing.expect(templates.len > 0);
    // All results should start with Qto_
    for (templates) |tmpl| {
        const name = tmpl.entity.getString(2) catch continue;
        try std.testing.expect(std.mem.startsWith(u8, name, "Qto_"));
    }
}

test "psetqto getApplicable with predefined type" {
    const allocator = std.testing.allocator;

    // Query without predefined type — should include more general templates
    const general_templates = try psetqto.getApplicable(
        allocator,
        "IFC4",
        "IfcBoilerType",
        null,
        false,
        false,
    );
    defer allocator.free(general_templates);

    // Query with specific predefined type
    const typed_templates = try psetqto.getApplicable(
        allocator,
        "IFC4",
        "IfcBoilerType",
        "STEAM",
        false,
        false,
    );
    defer allocator.free(typed_templates);

    // With a predefined type, we should get at least as many results
    // (templates matching the general class + those requiring the predefined type)
    try std.testing.expect(typed_templates.len >= general_templates.len);
}

// ---------------------------------------------------------------------------
// getApplicableNames
// ---------------------------------------------------------------------------

test "psetqto getApplicableNames returns string list" {
    const allocator = std.testing.allocator;

    const names = try psetqto.getApplicableNames(
        allocator,
        "IFC4",
        "IfcWall",
        null,
        false,
        false,
    );
    defer {
        for (names) |n| allocator.free(n);
        allocator.free(names);
    }

    try std.testing.expect(names.len > 0);

    var found_wall_common = false;
    for (names) |name| {
        if (std.mem.eql(u8, name, "Pset_WallCommon")) {
            found_wall_common = true;
            break;
        }
    }
    try std.testing.expect(found_wall_common);
}

test "psetqto getApplicableNames matches getApplicable count" {
    const allocator = std.testing.allocator;

    const templates = try psetqto.getApplicable(
        allocator,
        "IFC4",
        "IfcDoor",
        null,
        false,
        false,
    );
    defer allocator.free(templates);

    const names = try psetqto.getApplicableNames(
        allocator,
        "IFC4",
        "IfcDoor",
        null,
        false,
        false,
    );
    defer {
        for (names) |n| allocator.free(n);
        allocator.free(names);
    }

    try std.testing.expectEqual(templates.len, names.len);
}

// ---------------------------------------------------------------------------
// Template entity accessors
// ---------------------------------------------------------------------------

test "psetqto template entity has valid attributes" {
    const allocator = std.testing.allocator;

    const tmpl = (try psetqto.getByName(allocator, "IFC4", "Pset_WallCommon")).?;

    // Name (index 2)
    const name = try tmpl.entity.getString(2);
    try std.testing.expectEqualStrings("Pset_WallCommon", name);

    // ApplicableEntity (index 5) should contain IfcWall
    const applicable = try tmpl.entity.getString(5);
    try std.testing.expect(applicable.len > 0);
    try std.testing.expect(std.mem.indexOf(u8, applicable, "IfcWall") != null);
}

// ---------------------------------------------------------------------------
// Inheritance-based matching
// ---------------------------------------------------------------------------

test "psetqto getApplicable matches subtypes" {
    const allocator = std.testing.allocator;

    // IfcWallStandardCase is a subtype of IfcWall in IFC4.
    // Templates applicable to IfcWall should also match IfcWallStandardCase.
    const wall_templates = try psetqto.getApplicable(
        allocator,
        "IFC4",
        "IfcWall",
        null,
        false,
        false,
    );
    defer allocator.free(wall_templates);

    const std_case_templates = try psetqto.getApplicable(
        allocator,
        "IFC4",
        "IfcWallStandardCase",
        null,
        false,
        false,
    );
    defer allocator.free(std_case_templates);

    // IfcWallStandardCase should have at least as many applicable templates
    // as IfcWall, since it inherits from IfcWall
    try std.testing.expect(std_case_templates.len >= wall_templates.len);
}

// ---------------------------------------------------------------------------
// deinit
// ---------------------------------------------------------------------------

test "psetqto deinit releases resources" {
    const allocator = std.testing.allocator;

    // Load templates
    const templates = try psetqto.allTemplates(allocator, "IFC4");
    allocator.free(templates);

    // Deinit — should not crash
    psetqto.deinit();

    // Reload — should work fine (lazy re-initialization)
    const templates2 = try psetqto.allTemplates(allocator, "IFC4");
    defer allocator.free(templates2);

    try std.testing.expect(templates2.len > 0);
}

// ---------------------------------------------------------------------------
// Implementer agreement: TYPE-based templates apply to IfcTypeObject subclasses
// ---------------------------------------------------------------------------

test "getApplicable with TYPE implementer agreement matches IfcBeamType via IfcBeam template" {
    const allocator = std.testing.allocator;

    // Pset_BeamCommon has TemplateType=PSET_TYPEDRIVENOVERRIDE and ApplicableEntity=IfcBeam.
    // Per the implementer agreement, TYPE-based templates should also apply to
    // the corresponding IfcTypeObject subclass (IfcBeamType).
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
