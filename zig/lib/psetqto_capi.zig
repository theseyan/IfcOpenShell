const std = @import("std");
const ifcparse = @import("ifcparse");
const psetqto = @import("psetqto");

const c = ifcparse.c;
const allocator = std.heap.c_allocator;
threadlocal var g_borrowed_string_buf: std.ArrayList(u8) = .empty;

// Opaque handles

const TemplateListHandle = struct {
    items: []psetqto.PropertySetTemplate,
};

const PropertyListHandle = struct {
    items: []ifcparse.EntityRef,
};

const StringListHandle = struct {
    items: [][]const u8,
};

// Template list operations

export fn ifcopenshell_psetqto_get_applicable(
    schema_name: ?[*:0]const u8,
    ifc_class: ?[*:0]const u8,
    predefined_type: ?[*:0]const u8,
    pset_only: c_int,
    qto_only: c_int,
) ?*const TemplateListHandle {
    const schema_str = sliceFromC(schema_name);
    const class_str = sliceFromC(ifc_class);
    if (schema_str.len == 0 or class_str.len == 0) return null;

    const ptype: ?[]const u8 = if (predefined_type) |pt| blk: {
        const s = std.mem.sliceTo(pt, 0);
        break :blk if (s.len == 0) null else s;
    } else null;

    const items = psetqto.getApplicable(
        allocator,
        schema_str,
        class_str,
        ptype,
        pset_only != 0,
        qto_only != 0,
    ) catch return null;

    const handle = allocator.create(TemplateListHandle) catch {
        allocator.free(items);
        return null;
    };
    handle.* = .{ .items = items };
    return handle;
}

export fn ifcopenshell_psetqto_get_applicable_names(
    schema_name: ?[*:0]const u8,
    ifc_class: ?[*:0]const u8,
    predefined_type: ?[*:0]const u8,
    pset_only: c_int,
    qto_only: c_int,
) ?*const StringListHandle {
    const schema_str = sliceFromC(schema_name);
    const class_str = sliceFromC(ifc_class);
    if (schema_str.len == 0 or class_str.len == 0) return null;

    const ptype: ?[]const u8 = if (predefined_type) |pt| blk: {
        const s = std.mem.sliceTo(pt, 0);
        break :blk if (s.len == 0) null else s;
    } else null;

    const items = psetqto.getApplicableNames(
        allocator,
        schema_str,
        class_str,
        ptype,
        pset_only != 0,
        qto_only != 0,
    ) catch return null;

    const handle = allocator.create(StringListHandle) catch {
        for (items) |item| allocator.free(item);
        allocator.free(items);
        return null;
    };
    handle.* = .{ .items = items };
    return handle;
}

export fn ifcopenshell_psetqto_get_by_name(
    schema_name: ?[*:0]const u8,
    name: ?[*:0]const u8,
) ?*const c.ifcopenshell_ifcparse_entity_ref_t {
    const schema_str = sliceFromC(schema_name);
    const name_str = sliceFromC(name);
    if (schema_str.len == 0 or name_str.len == 0) return null;

    const tmpl = psetqto.getByName(allocator, schema_str, name_str) catch return null;
    if (tmpl) |t| {
        return t.entity.handle;
    }
    return null;
}

export fn ifcopenshell_psetqto_is_templated(
    schema_name: ?[*:0]const u8,
    name: ?[*:0]const u8,
) c_int {
    const schema_str = sliceFromC(schema_name);
    const name_str = sliceFromC(name);
    if (schema_str.len == 0 or name_str.len == 0) return 0;

    const result = psetqto.isTemplated(allocator, schema_str, name_str) catch return 0;
    return if (result) 1 else 0;
}

export fn ifcopenshell_psetqto_all_templates(
    schema_name: ?[*:0]const u8,
) ?*const TemplateListHandle {
    const schema_str = sliceFromC(schema_name);
    if (schema_str.len == 0) return null;

    const items = psetqto.allTemplates(allocator, schema_str) catch return null;
    const handle = allocator.create(TemplateListHandle) catch {
        allocator.free(items);
        return null;
    };
    handle.* = .{ .items = items };
    return handle;
}

// Template list accessors

export fn ifcopenshell_psetqto_template_list_count(
    handle: ?*const TemplateListHandle,
) c_int {
    const h = handle orelse return 0;
    return @intCast(h.items.len);
}

export fn ifcopenshell_psetqto_template_list_get(
    handle: ?*const TemplateListHandle,
    index: c_int,
) ?*const c.ifcopenshell_ifcparse_entity_ref_t {
    const h = handle orelse return null;
    if (index < 0) return null;
    const i: usize = @intCast(index);
    if (i >= h.items.len) return null;
    return h.items[i].entity.handle;
}

export fn ifcopenshell_psetqto_template_list_close(
    handle: ?*const TemplateListHandle,
) void {
    const h = handle orelse return;
    allocator.free(h.items);
    allocator.destroy(h);
}

// Template entity accessors (operate on IfcPropertySetTemplate entity)

export fn ifcopenshell_psetqto_template_name_borrowed(
    entity: ?*const c.ifcopenshell_ifcparse_entity_ref_t,
) ?[*:0]const u8 {
    const value = getEntityString(entity, 2) orelse return null;
    return borrowString(value);
}

export fn ifcopenshell_psetqto_template_name_copy(
    entity: ?*const c.ifcopenshell_ifcparse_entity_ref_t,
) ?[*:0]u8 {
    const value = getEntityString(entity, 2) orelse return null;
    return copyString(value);
}

export fn ifcopenshell_psetqto_template_description_borrowed(
    entity: ?*const c.ifcopenshell_ifcparse_entity_ref_t,
) ?[*:0]const u8 {
    const value = getEntityString(entity, 3) orelse return null;
    return borrowString(value);
}

export fn ifcopenshell_psetqto_template_description_copy(
    entity: ?*const c.ifcopenshell_ifcparse_entity_ref_t,
) ?[*:0]u8 {
    const value = getEntityString(entity, 3) orelse return null;
    return copyString(value);
}

export fn ifcopenshell_psetqto_template_type_borrowed(
    entity: ?*const c.ifcopenshell_ifcparse_entity_ref_t,
) ?[*:0]const u8 {
    const value = getEntityString(entity, 4) orelse return null;
    return borrowString(value);
}

export fn ifcopenshell_psetqto_template_type_copy(
    entity: ?*const c.ifcopenshell_ifcparse_entity_ref_t,
) ?[*:0]u8 {
    const value = getEntityString(entity, 4) orelse return null;
    return copyString(value);
}

export fn ifcopenshell_psetqto_template_applicable_entity_borrowed(
    entity: ?*const c.ifcopenshell_ifcparse_entity_ref_t,
) ?[*:0]const u8 {
    const value = getEntityString(entity, 5) orelse return null;
    return borrowString(value);
}

export fn ifcopenshell_psetqto_template_applicable_entity_copy(
    entity: ?*const c.ifcopenshell_ifcparse_entity_ref_t,
) ?[*:0]u8 {
    const value = getEntityString(entity, 5) orelse return null;
    return copyString(value);
}

export fn ifcopenshell_psetqto_template_properties(
    entity: ?*const c.ifcopenshell_ifcparse_entity_ref_t,
) ?*const PropertyListHandle {
    const e = ifcparse.EntityRef{ .handle = entity };
    var list = e.getEntityList(6) catch return null;
    defer list.deinit();

    var items = std.ArrayList(ifcparse.EntityRef).empty;
    defer items.deinit(allocator);
    while (list.next()) |item| {
        items.append(allocator, item) catch return null;
    }
    const slice = items.toOwnedSlice(allocator) catch return null;
    const handle = allocator.create(PropertyListHandle) catch {
        allocator.free(slice);
        return null;
    };
    handle.* = .{ .items = slice };
    return handle;
}

// Property template accessors (operate on IfcSimplePropertyTemplate)

export fn ifcopenshell_psetqto_property_name_borrowed(
    entity: ?*const c.ifcopenshell_ifcparse_entity_ref_t,
) ?[*:0]const u8 {
    const value = getEntityString(entity, 2) orelse return null;
    return borrowString(value);
}

export fn ifcopenshell_psetqto_property_name_copy(
    entity: ?*const c.ifcopenshell_ifcparse_entity_ref_t,
) ?[*:0]u8 {
    const value = getEntityString(entity, 2) orelse return null;
    return copyString(value);
}

export fn ifcopenshell_psetqto_property_description_borrowed(
    entity: ?*const c.ifcopenshell_ifcparse_entity_ref_t,
) ?[*:0]const u8 {
    const value = getEntityString(entity, 3) orelse return null;
    return borrowString(value);
}

export fn ifcopenshell_psetqto_property_description_copy(
    entity: ?*const c.ifcopenshell_ifcparse_entity_ref_t,
) ?[*:0]u8 {
    const value = getEntityString(entity, 3) orelse return null;
    return copyString(value);
}

export fn ifcopenshell_psetqto_property_template_type_borrowed(
    entity: ?*const c.ifcopenshell_ifcparse_entity_ref_t,
) ?[*:0]const u8 {
    const value = getEntityString(entity, 4) orelse return null;
    return borrowString(value);
}

export fn ifcopenshell_psetqto_property_template_type_copy(
    entity: ?*const c.ifcopenshell_ifcparse_entity_ref_t,
) ?[*:0]u8 {
    const value = getEntityString(entity, 4) orelse return null;
    return copyString(value);
}

export fn ifcopenshell_psetqto_property_primary_measure_type_borrowed(
    entity: ?*const c.ifcopenshell_ifcparse_entity_ref_t,
) ?[*:0]const u8 {
    const value = getEntityString(entity, 5) orelse return null;
    return borrowString(value);
}

export fn ifcopenshell_psetqto_property_primary_measure_type_copy(
    entity: ?*const c.ifcopenshell_ifcparse_entity_ref_t,
) ?[*:0]u8 {
    const value = getEntityString(entity, 5) orelse return null;
    return copyString(value);
}

// Property list accessors

export fn ifcopenshell_psetqto_property_list_count(
    handle: ?*const PropertyListHandle,
) c_int {
    const h = handle orelse return 0;
    return @intCast(h.items.len);
}

export fn ifcopenshell_psetqto_property_list_get(
    handle: ?*const PropertyListHandle,
    index: c_int,
) ?*const c.ifcopenshell_ifcparse_entity_ref_t {
    const h = handle orelse return null;
    if (index < 0) return null;
    const i: usize = @intCast(index);
    if (i >= h.items.len) return null;
    return h.items[i].handle;
}

export fn ifcopenshell_psetqto_property_list_close(
    handle: ?*const PropertyListHandle,
) void {
    const h = handle orelse return;
    allocator.free(h.items);
    allocator.destroy(h);
}

// String list accessors (for getApplicableNames)

export fn ifcopenshell_psetqto_string_list_count(
    handle: ?*const StringListHandle,
) c_int {
    const h = handle orelse return 0;
    return @intCast(h.items.len);
}

export fn ifcopenshell_psetqto_string_list_get(
    handle: ?*const StringListHandle,
    index: c_int,
) ?[*:0]const u8 {
    const h = handle orelse return null;
    if (index < 0) return null;
    const i: usize = @intCast(index);
    if (i >= h.items.len) return null;
    return @ptrCast(h.items[i].ptr);
}

export fn ifcopenshell_psetqto_string_list_close(
    handle: ?*const StringListHandle,
) void {
    const h = handle orelse return;
    for (h.items) |item| allocator.free(item);
    allocator.free(h.items);
    allocator.destroy(h);
}

// Cleanup

export fn ifcopenshell_psetqto_deinit() void {
    psetqto.deinit();
}

// Helpers

fn getEntityString(
    entity: ?*const c.ifcopenshell_ifcparse_entity_ref_t,
    index: usize,
) ?[]const u8 {
    const e = ifcparse.EntityRef{ .handle = entity };
    return e.getString(index) catch null;
}

fn borrowString(value: []const u8) ?[*:0]const u8 {
    g_borrowed_string_buf.clearRetainingCapacity();
    g_borrowed_string_buf.appendSlice(allocator, value) catch return null;
    g_borrowed_string_buf.append(allocator, 0) catch {
        g_borrowed_string_buf.clearRetainingCapacity();
        return null;
    };
    return @ptrCast(g_borrowed_string_buf.items.ptr);
}

fn copyString(value: []const u8) ?[*:0]u8 {
    return c.ifcopenshell_string_copy_n(@ptrCast(value.ptr), value.len);
}

fn sliceFromC(ptr: ?[*:0]const u8) []const u8 {
    const p = ptr orelse return "";
    return std.mem.sliceTo(p, 0);
}
