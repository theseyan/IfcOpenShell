const std = @import("std");
const ifcparse = @import("ifcparse");
const ifcutil = @import("ifcutil");

// Reuse the C typedefs from the ifcparse module's @cImport to avoid
// creating duplicate opaque types.
const c = ifcparse.c;

// Allocator

const allocator = std.heap.c_allocator;

// Internal helpers

fn entityToHandle(entity: ?ifcparse.EntityRef) ?*const c.ifcopenshell_ifcparse_entity_ref_t {
    const e = entity orelse return null;
    return e.handle;
}

fn handleToEntity(handle: ?*const c.ifcopenshell_ifcparse_entity_ref_t) ifcparse.EntityRef {
    return .{ .handle = handle };
}

fn dupeToC(str: []const u8) ?[*:0]u8 {
    return c.ifcopenshell_string_copy_n(@ptrCast(str.ptr), str.len);
}

fn sliceFromC(ptr: ?[*:0]const u8) []const u8 {
    const p = ptr orelse return "";
    return std.mem.sliceTo(p, 0);
}

// Opaque handles for complex return types

const EntityListHandle = struct {
    items: []ifcparse.EntityRef,
};

const PsetHandle = struct {
    data: ifcutil.PsetData,
};

const PsetListHandle = struct {
    items: []PsetHandle,
};

const PropertyValueHandle = struct {
    value: ifcutil.PropertyValue,
};

const PropertyTableUnitHandle = struct {
    data: ifcutil.unit.PropertyTableUnit,
};

// element.* convenience functions

export fn ifcopenshell_ifcutil_element_get_type(
    entity: ?*const c.ifcopenshell_ifcparse_entity_ref_t,
) ?*const c.ifcopenshell_ifcparse_entity_ref_t {
    const result = ifcutil.element.getType(allocator, handleToEntity(entity)) catch return null;
    return entityToHandle(result);
}

export fn ifcopenshell_ifcutil_element_get_types(
    entity: ?*const c.ifcopenshell_ifcparse_entity_ref_t,
) ?*EntityListHandle {
    const items = ifcutil.element.getTypes(allocator, handleToEntity(entity)) catch return null;
    const handle = allocator.create(EntityListHandle) catch {
        allocator.free(items);
        return null;
    };
    handle.* = .{ .items = items };
    return handle;
}

export fn ifcopenshell_ifcutil_element_get_predefined_type_copy(
    entity: ?*const c.ifcopenshell_ifcparse_entity_ref_t,
) ?[*:0]u8 {
    const result = ifcutil.element.getPredefinedType(allocator, handleToEntity(entity)) catch return null;
    const value = result orelse return null;
    defer allocator.free(value);
    return dupeToC(value);
}

export fn ifcopenshell_ifcutil_element_is_userdefined_type(
    entity: ?*const c.ifcopenshell_ifcparse_entity_ref_t,
) c_int {
    const result = ifcutil.element.isUserdefinedType(allocator, handleToEntity(entity)) catch return 0;
    return if (result) 1 else 0;
}

export fn ifcopenshell_ifcutil_element_get_aggregate(
    entity: ?*const c.ifcopenshell_ifcparse_entity_ref_t,
) ?*const c.ifcopenshell_ifcparse_entity_ref_t {
    const result = ifcutil.element.getAggregate(allocator, handleToEntity(entity)) catch return null;
    return entityToHandle(result);
}

export fn ifcopenshell_ifcutil_element_get_nest(
    entity: ?*const c.ifcopenshell_ifcparse_entity_ref_t,
) ?*const c.ifcopenshell_ifcparse_entity_ref_t {
    const result = ifcutil.element.getNest(allocator, handleToEntity(entity)) catch return null;
    return entityToHandle(result);
}

export fn ifcopenshell_ifcutil_element_get_filled_void(
    entity: ?*const c.ifcopenshell_ifcparse_entity_ref_t,
) ?*const c.ifcopenshell_ifcparse_entity_ref_t {
    const result = ifcutil.element.getFilledVoid(allocator, handleToEntity(entity)) catch return null;
    return entityToHandle(result);
}

export fn ifcopenshell_ifcutil_element_get_voided_element(
    entity: ?*const c.ifcopenshell_ifcparse_entity_ref_t,
) ?*const c.ifcopenshell_ifcparse_entity_ref_t {
    const result = ifcutil.element.getVoidedElement(allocator, handleToEntity(entity)) catch return null;
    return entityToHandle(result);
}

export fn ifcopenshell_ifcutil_element_get_container(
    entity: ?*const c.ifcopenshell_ifcparse_entity_ref_t,
    should_get_direct: c_int,
    ifc_class: ?[*:0]const u8,
) ?*const c.ifcopenshell_ifcparse_entity_ref_t {
    const cls: ?[]const u8 = if (ifc_class) |p| sliceFromC(p) else null;
    const result = ifcutil.element.getContainer(
        allocator,
        handleToEntity(entity),
        should_get_direct != 0,
        cls,
    ) catch return null;
    return entityToHandle(result);
}

export fn ifcopenshell_ifcutil_element_get_parent(
    entity: ?*const c.ifcopenshell_ifcparse_entity_ref_t,
) ?*const c.ifcopenshell_ifcparse_entity_ref_t {
    const result = ifcutil.element.getParent(allocator, handleToEntity(entity)) catch return null;
    return entityToHandle(result);
}

export fn ifcopenshell_ifcutil_element_get_material(
    entity: ?*const c.ifcopenshell_ifcparse_entity_ref_t,
    should_skip_usage: c_int,
    should_inherit: c_int,
) ?*const c.ifcopenshell_ifcparse_entity_ref_t {
    const result = ifcutil.element.getMaterial(
        allocator,
        handleToEntity(entity),
        should_skip_usage != 0,
        should_inherit != 0,
    ) catch return null;
    return entityToHandle(result);
}

// Property set (Pset) access

export fn ifcopenshell_ifcutil_element_get_pset_entities(
    entity: ?*const c.ifcopenshell_ifcparse_entity_ref_t,
    psets_only: c_int,
    qtos_only: c_int,
    should_inherit: c_int,
) ?*EntityListHandle {
    const items = ifcutil.element.getPsetEntities(allocator, handleToEntity(entity), .{
        .psets_only = psets_only != 0,
        .qtos_only = qtos_only != 0,
        .should_inherit = should_inherit != 0,
    }) catch return null;
    const handle = allocator.create(EntityListHandle) catch {
        allocator.free(items);
        return null;
    };
    handle.* = .{ .items = items };
    return handle;
}

export fn ifcopenshell_ifcutil_element_get_pset_entity(
    entity: ?*const c.ifcopenshell_ifcparse_entity_ref_t,
    pset_name: ?[*:0]const u8,
    psets_only: c_int,
    qtos_only: c_int,
    should_inherit: c_int,
) ?*const c.ifcopenshell_ifcparse_entity_ref_t {
    const name = sliceFromC(pset_name);
    const result = ifcutil.element.getPsetEntity(allocator, handleToEntity(entity), name, .{
        .psets_only = psets_only != 0,
        .qtos_only = qtos_only != 0,
        .should_inherit = should_inherit != 0,
    }) catch return null;
    return entityToHandle(result);
}

export fn ifcopenshell_ifcutil_element_get_property_entities(
    definition: ?*const c.ifcopenshell_ifcparse_entity_ref_t,
) ?*EntityListHandle {
    const items = ifcutil.element.getPropertyEntities(allocator, handleToEntity(definition)) catch return null;
    const handle = allocator.create(EntityListHandle) catch {
        allocator.free(items);
        return null;
    };
    handle.* = .{ .items = items };
    return handle;
}

export fn ifcopenshell_ifcutil_element_get_property_entity(
    definition: ?*const c.ifcopenshell_ifcparse_entity_ref_t,
    property_name: ?[*:0]const u8,
) ?*const c.ifcopenshell_ifcparse_entity_ref_t {
    const name = sliceFromC(property_name);
    const result = ifcutil.element.getPropertyEntity(allocator, handleToEntity(definition), name) catch return null;
    return entityToHandle(result);
}

export fn ifcopenshell_ifcutil_element_get_property_value(
    property_entity: ?*const c.ifcopenshell_ifcparse_entity_ref_t,
) ?*PropertyValueHandle {
    const value = ifcutil.element.getPropertyValue(allocator, handleToEntity(property_entity)) catch return null;
    const handle = allocator.create(PropertyValueHandle) catch {
        var owned = value;
        owned.deinit(allocator);
        return null;
    };
    handle.* = .{ .value = value };
    return handle;
}

export fn ifcopenshell_ifcutil_element_get_pset(
    entity: ?*const c.ifcopenshell_ifcparse_entity_ref_t,
    pset_name: ?[*:0]const u8,
    psets_only: c_int,
    qtos_only: c_int,
    should_inherit: c_int,
) ?*PsetHandle {
    const name = sliceFromC(pset_name);
    const result = ifcutil.element.getPset(allocator, handleToEntity(entity), name, .{
        .psets_only = psets_only != 0,
        .qtos_only = qtos_only != 0,
        .should_inherit = should_inherit != 0,
    }) catch return null;
    const data = result orelse return null;
    const handle = allocator.create(PsetHandle) catch {
        var owned = data;
        owned.deinit(allocator);
        return null;
    };
    handle.* = .{ .data = data };
    return handle;
}

export fn ifcopenshell_ifcutil_element_get_psets(
    entity: ?*const c.ifcopenshell_ifcparse_entity_ref_t,
    psets_only: c_int,
    qtos_only: c_int,
    should_inherit: c_int,
) ?*PsetListHandle {
    const raw_items = ifcutil.element.getPsets(allocator, handleToEntity(entity), .{
        .psets_only = psets_only != 0,
        .qtos_only = qtos_only != 0,
        .should_inherit = should_inherit != 0,
    }) catch return null;

    const items = allocator.alloc(PsetHandle, raw_items.len) catch {
        for (raw_items) |*item| {
            var owned = item.*;
            owned.deinit(allocator);
        }
        allocator.free(raw_items);
        return null;
    };
    for (raw_items, 0..) |item, i| {
        items[i] = .{ .data = item };
    }
    allocator.free(raw_items);

    const handle = allocator.create(PsetListHandle) catch {
        for (items) |*item| {
            item.data.deinit(allocator);
        }
        allocator.free(items);
        return null;
    };
    handle.* = .{ .items = items };
    return handle;
}

// Pset/Property handle accessors & destructors

export fn ifcopenshell_ifcutil_pset_name_copy(handle: ?*const PsetHandle) ?[*:0]u8 {
    const h = handle orelse return null;
    return dupeToC(h.data.name);
}

export fn ifcopenshell_ifcutil_pset_definition(handle: ?*const PsetHandle) ?*const c.ifcopenshell_ifcparse_entity_ref_t {
    const h = handle orelse return null;
    return entityToHandle(h.data.definition);
}

export fn ifcopenshell_ifcutil_pset_property_count(handle: ?*const PsetHandle) usize {
    const h = handle orelse return 0;
    return h.data.properties.len;
}

export fn ifcopenshell_ifcutil_pset_property_name_copy(handle: ?*const PsetHandle, index: usize) ?[*:0]u8 {
    const h = handle orelse return null;
    if (index >= h.data.properties.len) return null;
    return dupeToC(h.data.properties[index].name);
}

export fn ifcopenshell_ifcutil_pset_property_entity(
    handle: ?*const PsetHandle,
    index: usize,
) ?*const c.ifcopenshell_ifcparse_entity_ref_t {
    const h = handle orelse return null;
    if (index >= h.data.properties.len) return null;
    return entityToHandle(h.data.properties[index].property_entity);
}

export fn ifcopenshell_ifcutil_pset_property_value_kind(
    handle: ?*const PsetHandle,
    index: usize,
) c_int {
    const h = handle orelse return 0;
    if (index >= h.data.properties.len) return 0;
    return propertyValueKind(h.data.properties[index].value);
}

fn propertyValueKind(value: ifcutil.PropertyValue) c_int {
    return switch (value) {
        .none => 0,
        .primitive => |p| switch (p) {
            .int => 1,
            .bool => 2,
            .logical => 1,
            .double => 3,
            .string => 4,
        },
        .primitive_list => 5,
        .table => 6,
        .unsupported => 7,
    };
}

export fn ifcopenshell_ifcutil_pset_property_value_int(handle: ?*const PsetHandle, index: usize) c_int {
    const h = handle orelse return 0;
    if (index >= h.data.properties.len) return 0;
    return switch (h.data.properties[index].value) {
        .primitive => |p| switch (p) {
            .int, .logical => |v| v,
            .bool => |v| if (v) @as(c_int, 1) else 0,
            else => 0,
        },
        else => 0,
    };
}

export fn ifcopenshell_ifcutil_pset_property_value_double(handle: ?*const PsetHandle, index: usize) f64 {
    const h = handle orelse return 0.0;
    if (index >= h.data.properties.len) return 0.0;
    return switch (h.data.properties[index].value) {
        .primitive => |p| switch (p) {
            .double => |v| v,
            else => 0.0,
        },
        else => 0.0,
    };
}

export fn ifcopenshell_ifcutil_pset_property_value_string_copy(handle: ?*const PsetHandle, index: usize) ?[*:0]u8 {
    const h = handle orelse return null;
    if (index >= h.data.properties.len) return null;
    return switch (h.data.properties[index].value) {
        .primitive => |p| switch (p) {
            .string => |v| dupeToC(v),
            else => null,
        },
        else => null,
    };
}

export fn ifcopenshell_ifcutil_pset_property_value_list_count(handle: ?*const PsetHandle, index: usize) usize {
    const h = handle orelse return 0;
    if (index >= h.data.properties.len) return 0;
    return switch (h.data.properties[index].value) {
        .primitive_list => |list| list.len,
        else => 0,
    };
}

export fn ifcopenshell_ifcutil_pset_property_value_list_item_kind(
    handle: ?*const PsetHandle,
    prop_index: usize,
    list_index: usize,
) c_int {
    const h = handle orelse return 0;
    if (prop_index >= h.data.properties.len) return 0;
    const list = switch (h.data.properties[prop_index].value) {
        .primitive_list => |l| l,
        else => return 0,
    };
    if (list_index >= list.len) return 0;
    return switch (list[list_index]) {
        .int, .logical => 1,
        .bool => 2,
        .double => 3,
        .string => 4,
    };
}

export fn ifcopenshell_ifcutil_pset_property_value_list_item_int(
    handle: ?*const PsetHandle,
    prop_index: usize,
    list_index: usize,
) c_int {
    const h = handle orelse return 0;
    if (prop_index >= h.data.properties.len) return 0;
    const list = switch (h.data.properties[prop_index].value) {
        .primitive_list => |l| l,
        else => return 0,
    };
    if (list_index >= list.len) return 0;
    return switch (list[list_index]) {
        .int, .logical => |v| v,
        .bool => |v| if (v) @as(c_int, 1) else 0,
        else => 0,
    };
}

export fn ifcopenshell_ifcutil_pset_property_value_list_item_double(
    handle: ?*const PsetHandle,
    prop_index: usize,
    list_index: usize,
) f64 {
    const h = handle orelse return 0.0;
    if (prop_index >= h.data.properties.len) return 0.0;
    const list = switch (h.data.properties[prop_index].value) {
        .primitive_list => |l| l,
        else => return 0.0,
    };
    if (list_index >= list.len) return 0.0;
    return switch (list[list_index]) {
        .double => |v| v,
        else => 0.0,
    };
}

export fn ifcopenshell_ifcutil_pset_property_value_list_item_string_copy(
    handle: ?*const PsetHandle,
    prop_index: usize,
    list_index: usize,
) ?[*:0]u8 {
    const h = handle orelse return null;
    if (prop_index >= h.data.properties.len) return null;
    const list = switch (h.data.properties[prop_index].value) {
        .primitive_list => |l| l,
        else => return null,
    };
    if (list_index >= list.len) return null;
    return switch (list[list_index]) {
        .string => |v| dupeToC(v),
        else => null,
    };
}

export fn ifcopenshell_ifcutil_pset_destroy(handle: ?*PsetHandle) void {
    const h = handle orelse return;
    h.data.deinit(allocator);
    allocator.destroy(h);
}

// Pset list handle

export fn ifcopenshell_ifcutil_pset_list_count(handle: ?*const PsetListHandle) usize {
    const h = handle orelse return 0;
    return h.items.len;
}

export fn ifcopenshell_ifcutil_pset_list_get(handle: ?*PsetListHandle, index: usize) ?*PsetHandle {
    const h = handle orelse return null;
    if (index >= h.items.len) return null;
    return &h.items[index];
}

export fn ifcopenshell_ifcutil_pset_list_destroy(handle: ?*PsetListHandle) void {
    const h = handle orelse return;
    for (h.items) |*item| {
        item.data.deinit(allocator);
    }
    allocator.free(h.items);
    allocator.destroy(h);
}

// PropertyValue handle (standalone)

export fn ifcopenshell_ifcutil_property_value_kind(handle: ?*const PropertyValueHandle) c_int {
    const h = handle orelse return 0;
    return propertyValueKind(h.value);
}

export fn ifcopenshell_ifcutil_property_value_int(handle: ?*const PropertyValueHandle) c_int {
    const h = handle orelse return 0;
    return switch (h.value) {
        .primitive => |p| switch (p) {
            .int, .logical => |v| v,
            .bool => |v| if (v) @as(c_int, 1) else 0,
            else => 0,
        },
        else => 0,
    };
}

export fn ifcopenshell_ifcutil_property_value_double(handle: ?*const PropertyValueHandle) f64 {
    const h = handle orelse return 0.0;
    return switch (h.value) {
        .primitive => |p| switch (p) {
            .double => |v| v,
            else => 0.0,
        },
        else => 0.0,
    };
}

export fn ifcopenshell_ifcutil_property_value_string_copy(handle: ?*const PropertyValueHandle) ?[*:0]u8 {
    const h = handle orelse return null;
    return switch (h.value) {
        .primitive => |p| switch (p) {
            .string => |v| dupeToC(v),
            else => null,
        },
        else => null,
    };
}

export fn ifcopenshell_ifcutil_property_value_list_count(handle: ?*const PropertyValueHandle) usize {
    const h = handle orelse return 0;
    return switch (h.value) {
        .primitive_list => |list| list.len,
        else => 0,
    };
}

export fn ifcopenshell_ifcutil_property_value_destroy(handle: ?*PropertyValueHandle) void {
    const h = handle orelse return;
    h.value.deinit(allocator);
    allocator.destroy(h);
}

// Entity list handle

export fn ifcopenshell_ifcutil_entity_list_count(handle: ?*const EntityListHandle) usize {
    const h = handle orelse return 0;
    return h.items.len;
}

export fn ifcopenshell_ifcutil_entity_list_get(
    handle: ?*const EntityListHandle,
    index: usize,
) ?*const c.ifcopenshell_ifcparse_entity_ref_t {
    const h = handle orelse return null;
    if (index >= h.items.len) return null;
    return entityToHandle(h.items[index]);
}

export fn ifcopenshell_ifcutil_entity_list_destroy(handle: ?*EntityListHandle) void {
    const h = handle orelse return;
    allocator.free(h.items);
    allocator.destroy(h);
}

// classification.*

export fn ifcopenshell_ifcutil_classification_get_classification(
    reference: ?*const c.ifcopenshell_ifcparse_entity_ref_t,
) ?*const c.ifcopenshell_ifcparse_entity_ref_t {
    const result = ifcutil.classification.getClassification(allocator, handleToEntity(reference)) catch return null;
    return entityToHandle(result);
}

export fn ifcopenshell_ifcutil_classification_get_inherited_references(
    reference: ?*const c.ifcopenshell_ifcparse_entity_ref_t,
) ?*EntityListHandle {
    const items = ifcutil.classification.getInheritedReferences(allocator, handleToEntity(reference)) catch return null;
    const handle = allocator.create(EntityListHandle) catch {
        allocator.free(items);
        return null;
    };
    handle.* = .{ .items = items };
    return handle;
}

export fn ifcopenshell_ifcutil_classification_get_references(
    entity: ?*const c.ifcopenshell_ifcparse_entity_ref_t,
    should_inherit: c_int,
) ?*EntityListHandle {
    const items = ifcutil.classification.getReferences(
        allocator,
        handleToEntity(entity),
        should_inherit != 0,
    ) catch return null;
    const handle = allocator.create(EntityListHandle) catch {
        allocator.free(items);
        return null;
    };
    handle.* = .{ .items = items };
    return handle;
}

// unit.*

export fn ifcopenshell_ifcutil_unit_get_project_unit(
    file: ?*c.ifcopenshell_ifcparse_file_t,
    unit_type: ?[*:0]const u8,
) ?*const c.ifcopenshell_ifcparse_entity_ref_t {
    var f = ifcparse.File{ .handle = file };
    const result = ifcutil.unit.getProjectUnit(allocator, &f, sliceFromC(unit_type)) catch return null;
    return entityToHandle(result);
}

export fn ifcopenshell_ifcutil_unit_get_property_unit(
    property_entity: ?*const c.ifcopenshell_ifcparse_entity_ref_t,
    file: ?*c.ifcopenshell_ifcparse_file_t,
) ?*const c.ifcopenshell_ifcparse_entity_ref_t {
    var f = ifcparse.File{ .handle = file };
    const result = ifcutil.unit.getPropertyUnit(allocator, handleToEntity(property_entity), &f) catch return null;
    return entityToHandle(result);
}

export fn ifcopenshell_ifcutil_unit_get_property_table_unit(
    property_entity: ?*const c.ifcopenshell_ifcparse_entity_ref_t,
    file: ?*c.ifcopenshell_ifcparse_file_t,
) ?*PropertyTableUnitHandle {
    var f = ifcparse.File{ .handle = file };
    const result = ifcutil.unit.getPropertyTableUnit(allocator, handleToEntity(property_entity), &f) catch return null;
    const handle = allocator.create(PropertyTableUnitHandle) catch return null;
    handle.* = .{ .data = result };
    return handle;
}

export fn ifcopenshell_ifcutil_property_table_unit_defining(
    handle: ?*const PropertyTableUnitHandle,
) ?*const c.ifcopenshell_ifcparse_entity_ref_t {
    const h = handle orelse return null;
    return entityToHandle(h.data.defining_unit);
}

export fn ifcopenshell_ifcutil_property_table_unit_defined(
    handle: ?*const PropertyTableUnitHandle,
) ?*const c.ifcopenshell_ifcparse_entity_ref_t {
    const h = handle orelse return null;
    return entityToHandle(h.data.defined_unit);
}

export fn ifcopenshell_ifcutil_property_table_unit_destroy(handle: ?*PropertyTableUnitHandle) void {
    const h = handle orelse return;
    allocator.destroy(h);
}

export fn ifcopenshell_ifcutil_unit_convert(
    value: f64,
    from_prefix: ?[*:0]const u8,
    from_unit: ?[*:0]const u8,
    to_prefix: ?[*:0]const u8,
    to_unit: ?[*:0]const u8,
) f64 {
    const fp: ?[]const u8 = if (from_prefix) |p| sliceFromC(p) else null;
    const tp: ?[]const u8 = if (to_prefix) |p| sliceFromC(p) else null;
    return ifcutil.unit.convert(
        value,
        fp,
        sliceFromC(from_unit),
        tp,
        sliceFromC(to_unit),
    );
}

export fn ifcopenshell_ifcutil_unit_calculate_unit_scale(
    file: ?*c.ifcopenshell_ifcparse_file_t,
    unit_type: ?[*:0]const u8,
) f64 {
    var f = ifcparse.File{ .handle = file };
    return ifcutil.unit.calculateUnitScale(allocator, &f, sliceFromC(unit_type)) catch return 1.0;
}

export fn ifcopenshell_ifcutil_unit_si_type_name_copy(
    unit_type: ?[*:0]const u8,
) ?[*:0]u8 {
    const result = ifcutil.unit.siTypeName(sliceFromC(unit_type)) orelse return null;
    return dupeToC(result);
}

// Free helper — free a C string returned by any of the above functions.

export fn ifcopenshell_ifcutil_free_string(ptr: ?[*:0]u8) void {
    c.ifcopenshell_string_free(ptr);
}
