const std = @import("std");
const ifcparse = @import("ifcparse");
const config = @import("psetqto_config");

// Compile-time embedded template data

const ifc4_data: ?[]const u8 = if (config.has_ifc4) @embedFile("psetqto/Pset_IFC4_ADD2.ifc") else null;
const ifc2x3_data: ?[]const u8 = if (config.has_ifc2x3) @embedFile("psetqto/Pset_IFC2X3.ifc") else null;
const ifc4x3_data: ?[]const u8 = if (config.has_ifc4x3) @embedFile("psetqto/Pset_IFC4X3.ifc") else null;

// Schema index

pub const SchemaIndex = enum {
    ifc4,
    ifc2x3,
    ifc4x3,

    pub fn fromName(name: []const u8) ?SchemaIndex {
        if (std.ascii.eqlIgnoreCase(name, "IFC4")) return .ifc4;
        if (std.ascii.eqlIgnoreCase(name, "IFC2X3")) return .ifc2x3;
        if (std.ascii.eqlIgnoreCase(name, "IFC4X3")) return .ifc4x3;
        if (std.ascii.eqlIgnoreCase(name, "IFC4X3_ADD2")) return .ifc4x3;
        return null;
    }

    /// The IFC schema name used by the template file itself (for parsing).
    /// IFC2X3 and IFC4X3 templates are written in IFC4X3_ADD2 format.
    pub fn fileSchemaName(self: SchemaIndex) []const u8 {
        return switch (self) {
            .ifc4 => "IFC4",
            .ifc2x3 => "IFC4X3_ADD2",
            .ifc4x3 => "IFC4X3_ADD2",
        };
    }

    /// The user-facing schema name used for type resolution.
    pub fn querySchemaName(self: SchemaIndex) []const u8 {
        return switch (self) {
            .ifc4 => "IFC4",
            .ifc2x3 => "IFC2X3",
            .ifc4x3 => "IFC4X3_ADD2",
        };
    }

    pub fn isAvailable(self: SchemaIndex) bool {
        return switch (self) {
            .ifc4 => config.has_ifc4,
            .ifc2x3 => config.has_ifc2x3,
            .ifc4x3 => config.has_ifc4x3,
        };
    }

    pub fn templateData(self: SchemaIndex) ?[]const u8 {
        return switch (self) {
            .ifc4 => ifc4_data,
            .ifc2x3 => ifc2x3_data,
            .ifc4x3 => ifc4x3_data,
        };
    }
};

// Lazy per-schema state

const SchemaState = struct {
    file: ?ifcparse.File = null,
    initialized: bool = false,
};

var schema_states: [3]SchemaState = .{ .{}, .{}, .{} };

fn ensureLoaded(idx: SchemaIndex) !*ifcparse.File {
    const state = &schema_states[@intFromEnum(idx)];
    if (state.initialized) {
        if (state.file) |*f| return f;
        return error.QueryFailed;
    }
    state.initialized = true;

    const data = idx.templateData() orelse return error.QueryFailed;
    state.file = ifcparse.File.openFromMemory(data) catch return error.QueryFailed;
    return &state.file.?;
}

// Public types

pub const PropertySetTemplate = struct {
    entity: ifcparse.EntityRef,
};

// Public query API

pub fn getApplicable(
    allocator: std.mem.Allocator,
    schema_name: []const u8,
    ifc_class: []const u8,
    predefined_type: ?[]const u8,
    pset_only: bool,
    qto_only: bool,
) ![]PropertySetTemplate {
    const idx = SchemaIndex.fromName(schema_name) orelse return error.QueryFailed;
    if (!idx.isAvailable()) return error.QueryFailed;

    const file = try ensureLoaded(idx);
    const query_schema = idx.querySchemaName();
    const any_class = ifc_class.len == 0;

    var templates = file.instancesByType(allocator, "IfcPropertySetTemplate", false) catch return error.QueryFailed;
    defer templates.deinit();

    var result = std.ArrayList(PropertySetTemplate).empty;
    errdefer result.deinit(allocator);

    while (templates.next()) |entity| {
        const raw_template_type = entity.getString(4) catch "";
        if (pset_only and std.mem.startsWith(u8, raw_template_type, "QTO_")) continue;
        if (qto_only and std.mem.startsWith(u8, raw_template_type, "PSET_")) continue;

        if (any_class) {
            result.append(allocator, .{ .entity = entity }) catch return error.QueryFailed;
            continue;
        }

        const has_type_keyword = std.mem.indexOf(u8, raw_template_type, "TYPE") != null;
        const applicable_entity = entity.getString(5) catch continue;
        if (isApplicable(allocator, query_schema, applicable_entity, ifc_class, predefined_type, has_type_keyword)) {
            result.append(allocator, .{ .entity = entity }) catch return error.QueryFailed;
        }
    }

    return result.toOwnedSlice(allocator) catch return error.QueryFailed;
}

pub fn getApplicableNames(
    allocator: std.mem.Allocator,
    schema_name: []const u8,
    ifc_class: []const u8,
    predefined_type: ?[]const u8,
    pset_only: bool,
    qto_only: bool,
) ![][]const u8 {
    const templates = try getApplicable(allocator, schema_name, ifc_class, predefined_type, pset_only, qto_only);
    defer allocator.free(templates);

    var names = std.ArrayList([]const u8).empty;
    errdefer {
        for (names.items) |n| allocator.free(n);
        names.deinit(allocator);
    }

    for (templates) |tmpl| {
        const name = tmpl.entity.getString(2) catch continue;
        const duped = allocator.dupe(u8, name) catch return error.QueryFailed;
        names.append(allocator, duped) catch {
            allocator.free(duped);
            return error.QueryFailed;
        };
    }

    return names.toOwnedSlice(allocator) catch return error.QueryFailed;
}

pub fn getByName(
    allocator: std.mem.Allocator,
    schema_name: []const u8,
    name: []const u8,
) !?PropertySetTemplate {
    const idx = SchemaIndex.fromName(schema_name) orelse return error.QueryFailed;
    if (!idx.isAvailable()) return error.QueryFailed;

    const file = try ensureLoaded(idx);

    var templates = file.instancesByType(allocator, "IfcPropertySetTemplate", false) catch return error.QueryFailed;
    defer templates.deinit();

    while (templates.next()) |entity| {
        const tmpl_name = entity.getString(2) catch continue;
        if (std.mem.eql(u8, tmpl_name, name)) {
            return .{ .entity = entity };
        }
    }

    return null;
}

pub fn isTemplated(
    allocator: std.mem.Allocator,
    schema_name: []const u8,
    name: []const u8,
) !bool {
    const result = try getByName(allocator, schema_name, name);
    return result != null;
}

pub fn allTemplates(
    allocator: std.mem.Allocator,
    schema_name: []const u8,
) ![]PropertySetTemplate {
    const idx = SchemaIndex.fromName(schema_name) orelse return error.QueryFailed;
    if (!idx.isAvailable()) return error.QueryFailed;

    const file = try ensureLoaded(idx);

    var templates = file.instancesByType(allocator, "IfcPropertySetTemplate", false) catch return error.QueryFailed;
    defer templates.deinit();

    var result = std.ArrayList(PropertySetTemplate).empty;
    errdefer result.deinit(allocator);

    while (templates.next()) |entity| {
        result.append(allocator, .{ .entity = entity }) catch return error.QueryFailed;
    }

    return result.toOwnedSlice(allocator) catch return error.QueryFailed;
}

pub fn deinit() void {
    for (&schema_states) |*state| {
        if (state.file) |*f| {
            f.deinit();
        }
        state.* = .{};
    }
}

// Applicability matching

///
fn isApplicable(
    allocator: std.mem.Allocator,
    schema_name: []const u8,
    applicable_entity: []const u8,
    ifc_class: []const u8,
    predefined_type: ?[]const u8,
    type_driven: bool,
) bool {
    var iter = std.mem.splitSequence(u8, applicable_entity, ",");
    while (iter.next()) |raw_entry| {
        const entry = std.mem.trim(u8, raw_entry, " \t\r\n");
        if (entry.len == 0) continue;

        if (matchEntry(allocator, schema_name, entry, ifc_class, predefined_type, type_driven))
            return true;
    }
    return false;
}

fn matchEntry(
    allocator: std.mem.Allocator,
    schema_name: []const u8,
    entry: []const u8,
    ifc_class: []const u8,
    predefined_type: ?[]const u8,
    type_driven: bool,
) bool {
    // Strip [PerformanceHistory] suffix if present
    var class_part = entry;
    if (std.mem.indexOf(u8, entry, "[")) |bracket_pos| {
        class_part = entry[0..bracket_pos];
    }

    // Split on '/' for predefined type
    var entry_class: []const u8 = undefined;
    var entry_ptype: ?[]const u8 = null;
    if (std.mem.indexOf(u8, class_part, "/")) |slash_pos| {
        entry_class = class_part[0..slash_pos];
        entry_ptype = class_part[slash_pos + 1 ..];
    } else {
        entry_class = class_part;
    }

    // Check class inheritance: is ifc_class a subtype of entry_class?
    const type_ref = ifcparse.schemaDeclarationByName(allocator, schema_name, ifc_class) catch return false;
    const decl = type_ref orelse return false;

    const direct_match = std.ascii.eqlIgnoreCase(ifc_class, entry_class) or
        (decl.isA(allocator, entry_class) catch false);

    if (direct_match) {
        // Check predefined type if required
        if (entry_ptype) |required_ptype| {
            const actual_ptype = predefined_type orelse return false;
            if (!std.ascii.eqlIgnoreCase(actual_ptype, required_ptype)) return false;
        }
        return true;
    }

    // Implementer agreement: if template type contains "TYPE" and the queried
    // entity is an IfcTypeObject, the template should also apply to the
    // corresponding type class even if not explicitly listed.
    // https://github.com/buildingSMART/IFC4.3.x-development/issues/22
    if (type_driven) {
        const is_type_obj = decl.isA(allocator, "IfcTypeObject") catch false;
        if (is_type_obj) {
            // Try entry_class + "Type" (e.g. IfcBoiler -> IfcBoilerType)
            var buf: [256]u8 = undefined;
            if (entry_class.len + 4 <= buf.len) {
                const type_name = std.fmt.bufPrint(&buf, "{s}Type", .{entry_class}) catch unreachable;
                if (tryTypeMatch(allocator, schema_name, ifc_class, type_name)) {
                    if (checkPredefinedType(entry_ptype, predefined_type)) return true;
                }
            }
            // Try "IfcType" + entry_class[3..] (e.g. IfcBoiler -> IfcTypeBoiler)
            if (entry_class.len > 3 and std.mem.startsWith(u8, entry_class, "Ifc")) {
                const suffix = entry_class[3..];
                if (7 + suffix.len <= buf.len) {
                    const type_name = std.fmt.bufPrint(&buf, "IfcType{s}", .{suffix}) catch unreachable;
                    if (tryTypeMatch(allocator, schema_name, ifc_class, type_name)) {
                        if (checkPredefinedType(entry_ptype, predefined_type)) return true;
                    }
                }
            }
        }
    }

    return false;
}

fn tryTypeMatch(
    allocator: std.mem.Allocator,
    schema_name: []const u8,
    ifc_class: []const u8,
    type_class: []const u8,
) bool {
    if ((ifcparse.schemaDeclarationByName(allocator, schema_name, type_class) catch return false) == null) {
        return false;
    }
    if (std.ascii.eqlIgnoreCase(ifc_class, type_class)) return true;
    const entity_decl = ifcparse.schemaDeclarationByName(allocator, schema_name, ifc_class) catch return false;
    const decl = entity_decl orelse return false;
    return decl.isA(allocator, type_class) catch false;
}

fn checkPredefinedType(entry_ptype: ?[]const u8, predefined_type: ?[]const u8) bool {
    if (entry_ptype) |required_ptype| {
        const actual_ptype = predefined_type orelse return false;
        return std.ascii.eqlIgnoreCase(actual_ptype, required_ptype);
    }
    return true;
}
