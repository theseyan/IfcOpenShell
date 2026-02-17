const std = @import("std");
const ifcparse = @import("ifcparse");
const config = @import("psetqto_config");

pub const LoadError = error{
    InvalidArgument,
    UnsupportedSchema,
    OutOfMemory,
    OpenFailed,
};

pub const QueryError = error{
    InvalidArgument,
    UnsupportedSchema,
    TemplateNotLoaded,
    OutOfMemory,
    QueryFailed,
};

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

    pub fn fileSchemaName(self: SchemaIndex) []const u8 {
        return switch (self) {
            .ifc4 => "IFC4",
            .ifc2x3 => "IFC4X3_ADD2",
            .ifc4x3 => "IFC4X3_ADD2",
        };
    }

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
};

const SchemaState = struct {
    file: ?ifcparse.File = null,

    fn clear(self: *SchemaState) void {
        if (self.file) |*f| f.deinit();
        self.* = .{};
    }
};

var schema_states: [3]SchemaState = .{ .{}, .{}, .{} };

fn resolveSchema(schema_name: []const u8) ?SchemaIndex {
    const idx = SchemaIndex.fromName(schema_name) orelse return null;
    if (!idx.isAvailable()) return null;
    return idx;
}

fn ensureLoaded(idx: SchemaIndex) QueryError!*ifcparse.File {
    const state = &schema_states[@intFromEnum(idx)];
    if (state.file) |*f| return f;
    return error.TemplateNotLoaded;
}

pub fn loadTemplateFromMemory(
    schema_name: []const u8,
    data: []const u8,
) LoadError!void {
    if (schema_name.len == 0 or data.len == 0) return error.InvalidArgument;

    const idx = resolveSchema(schema_name) orelse return error.UnsupportedSchema;
    var file = ifcparse.File.openFromMemory(data) catch return error.OpenFailed;
    errdefer file.deinit();

    const state = &schema_states[@intFromEnum(idx)];
    state.clear();
    state.file = file;
}

pub fn loadTemplateFromFile(
    allocator: std.mem.Allocator,
    schema_name: []const u8,
    path: []const u8,
) LoadError!void {
    if (schema_name.len == 0 or path.len == 0) return error.InvalidArgument;

    const idx = resolveSchema(schema_name) orelse return error.UnsupportedSchema;
    var file = ifcparse.File.open(allocator, path, .{}) catch return error.OpenFailed;
    errdefer file.deinit();

    const state = &schema_states[@intFromEnum(idx)];
    state.clear();
    state.file = file;
}

pub fn unloadTemplate(schema_name: []const u8) LoadError!void {
    if (schema_name.len == 0) return error.InvalidArgument;
    const idx = resolveSchema(schema_name) orelse return error.UnsupportedSchema;
    schema_states[@intFromEnum(idx)].clear();
}

pub fn isTemplateLoaded(schema_name: []const u8) bool {
    const idx = resolveSchema(schema_name) orelse return false;
    return schema_states[@intFromEnum(idx)].file != null;
}

pub const PropertySetTemplate = struct {
    entity: ifcparse.EntityRef,
};

pub fn getApplicable(
    allocator: std.mem.Allocator,
    schema_name: []const u8,
    ifc_class: []const u8,
    predefined_type: ?[]const u8,
    pset_only: bool,
    qto_only: bool,
) QueryError![]PropertySetTemplate {
    if (schema_name.len == 0) return error.InvalidArgument;
    const idx = resolveSchema(schema_name) orelse return error.UnsupportedSchema;

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
            result.append(allocator, .{ .entity = entity }) catch return error.OutOfMemory;
            continue;
        }

        const has_type_keyword = std.mem.indexOf(u8, raw_template_type, "TYPE") != null;
        const applicable_entity = entity.getString(5) catch continue;
        if (isApplicable(allocator, query_schema, applicable_entity, ifc_class, predefined_type, has_type_keyword)) {
            result.append(allocator, .{ .entity = entity }) catch return error.OutOfMemory;
        }
    }

    return result.toOwnedSlice(allocator) catch return error.OutOfMemory;
}

pub fn getApplicableNames(
    allocator: std.mem.Allocator,
    schema_name: []const u8,
    ifc_class: []const u8,
    predefined_type: ?[]const u8,
    pset_only: bool,
    qto_only: bool,
) QueryError![][:0]u8 {
    const templates = try getApplicable(allocator, schema_name, ifc_class, predefined_type, pset_only, qto_only);
    defer allocator.free(templates);

    var names = std.ArrayList([:0]u8).empty;
    errdefer {
        for (names.items) |n| allocator.free(n);
        names.deinit(allocator);
    }

    for (templates) |tmpl| {
        const name = tmpl.entity.getString(2) catch continue;
        const duped = allocator.dupeZ(u8, name) catch return error.OutOfMemory;
        names.append(allocator, duped) catch {
            allocator.free(duped);
            return error.OutOfMemory;
        };
    }

    return names.toOwnedSlice(allocator) catch return error.OutOfMemory;
}

pub fn getByName(
    allocator: std.mem.Allocator,
    schema_name: []const u8,
    name: []const u8,
) QueryError!?PropertySetTemplate {
    if (schema_name.len == 0 or name.len == 0) return error.InvalidArgument;
    const idx = resolveSchema(schema_name) orelse return error.UnsupportedSchema;

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
) QueryError!bool {
    const result = try getByName(allocator, schema_name, name);
    return result != null;
}

pub fn allTemplates(
    allocator: std.mem.Allocator,
    schema_name: []const u8,
) QueryError![]PropertySetTemplate {
    if (schema_name.len == 0) return error.InvalidArgument;
    const idx = resolveSchema(schema_name) orelse return error.UnsupportedSchema;

    const file = try ensureLoaded(idx);

    var templates = file.instancesByType(allocator, "IfcPropertySetTemplate", false) catch return error.QueryFailed;
    defer templates.deinit();

    var result = std.ArrayList(PropertySetTemplate).empty;
    errdefer result.deinit(allocator);

    while (templates.next()) |entity| {
        result.append(allocator, .{ .entity = entity }) catch return error.OutOfMemory;
    }

    return result.toOwnedSlice(allocator) catch return error.OutOfMemory;
}

pub fn deinit() void {
    for (&schema_states) |*state| {
        state.clear();
    }
}

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
    var class_part = entry;
    if (std.mem.indexOf(u8, entry, "[")) |bracket_pos| {
        class_part = entry[0..bracket_pos];
    }

    var entry_class: []const u8 = undefined;
    var entry_ptype: ?[]const u8 = null;
    if (std.mem.indexOf(u8, class_part, "/")) |slash_pos| {
        entry_class = class_part[0..slash_pos];
        entry_ptype = class_part[slash_pos + 1 ..];
    } else {
        entry_class = class_part;
    }

    const type_ref = ifcparse.schemaDeclarationByName(allocator, schema_name, ifc_class) catch return false;
    const decl = type_ref orelse return false;

    const direct_match = std.ascii.eqlIgnoreCase(ifc_class, entry_class) or
        (decl.isA(allocator, entry_class) catch false);

    if (direct_match) {
        if (entry_ptype) |required_ptype| {
            const actual_ptype = predefined_type orelse return false;
            if (!std.ascii.eqlIgnoreCase(actual_ptype, required_ptype)) return false;
        }
        return true;
    }

    if (type_driven) {
        const is_type_obj = decl.isA(allocator, "IfcTypeObject") catch false;
        if (is_type_obj) {
            var buf: [256]u8 = undefined;
            if (entry_class.len + 4 <= buf.len) {
                const type_name = std.fmt.bufPrint(&buf, "{s}Type", .{entry_class}) catch unreachable;
                if (tryTypeMatch(allocator, schema_name, ifc_class, type_name)) {
                    if (checkPredefinedType(entry_ptype, predefined_type)) return true;
                }
            }
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
