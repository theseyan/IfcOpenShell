const std = @import("std");
const ifcparse = @import("ifcparse");

pub const QueryError = ifcparse.QueryError;

fn emptyEntitySlice(allocator: std.mem.Allocator) QueryError![]ifcparse.EntityRef {
    return allocator.alloc(ifcparse.EntityRef, 0) catch return error.OutOfMemory;
}

fn emptyPrimitiveSlice(allocator: std.mem.Allocator) QueryError![]PrimitiveValue {
    return allocator.alloc(PrimitiveValue, 0) catch return error.OutOfMemory;
}

fn forwardArgumentIndex(
    allocator: std.mem.Allocator,
    entity: ifcparse.EntityRef,
    attribute_name: []const u8,
) QueryError!?usize {
    if ((try entity.attributeCategory(allocator, attribute_name)) != .forward) return null;
    return try entity.argumentIndex(allocator, attribute_name);
}

fn getStringAttrBorrowed(
    allocator: std.mem.Allocator,
    entity: ifcparse.EntityRef,
    attribute_name: []const u8,
) QueryError!?[]const u8 {
    const index = (try forwardArgumentIndex(allocator, entity, attribute_name)) orelse return null;
    const value_type = entity.argumentValueType(index);
    switch (value_type) {
        .string, .enumeration, .binary => {},
        else => return null,
    }
    if (try entity.argumentIsNull(index)) return null;
    return try entity.getString(index);
}

fn getStringAttrOwned(
    allocator: std.mem.Allocator,
    entity: ifcparse.EntityRef,
    attribute_name: []const u8,
) QueryError!?[]u8 {
    const value = (try getStringAttrBorrowed(allocator, entity, attribute_name)) orelse return null;
    return allocator.dupe(u8, value) catch return error.OutOfMemory;
}

fn getEntityAttr(
    allocator: std.mem.Allocator,
    entity: ifcparse.EntityRef,
    attribute_name: []const u8,
) QueryError!?ifcparse.EntityRef {
    const category = try entity.attributeCategory(allocator, attribute_name);
    if (category == .inverse) {
        const values = try getEntityCollectionAttr(allocator, entity, attribute_name);
        defer allocator.free(values);
        if (values.len == 0) return null;
        return values[0];
    }
    if (category != .forward) return null;

    const index = (try forwardArgumentIndex(allocator, entity, attribute_name)) orelse return null;
    const value_type = entity.argumentValueType(index);
    if (value_type != .entity_instance) return null;
    if (try entity.argumentIsNull(index)) return null;
    return try entity.getEntity(index);
}

fn getEntityCollectionAttr(
    allocator: std.mem.Allocator,
    entity: ifcparse.EntityRef,
    attribute_name: []const u8,
) QueryError![]ifcparse.EntityRef {
    const category = try entity.attributeCategory(allocator, attribute_name);
    if (category == .inverse) {
        var inverses = try entity.getInverse(allocator, attribute_name);
        defer inverses.deinit();
        return inverses.toOwnedSlice(allocator);
    }
    if (category != .forward) return emptyEntitySlice(allocator);

    const index = (try forwardArgumentIndex(allocator, entity, attribute_name)) orelse return emptyEntitySlice(allocator);
    if (try entity.argumentIsNull(index)) return emptyEntitySlice(allocator);

    switch (entity.argumentValueType(index)) {
        .entity_instance => {
            const maybe_entity = try entity.getEntity(index);
            if (maybe_entity == null) return emptyEntitySlice(allocator);

            var values = allocator.alloc(ifcparse.EntityRef, 1) catch return error.OutOfMemory;
            values[0] = maybe_entity.?;
            return values;
        },
        .aggregate_of_entity_instance => {
            var entities = try entity.getEntityList(index);
            defer entities.deinit();
            return try entities.toOwnedSlice(allocator);
        },
        else => return emptyEntitySlice(allocator),
    }
}

fn getFirstEntityFromCollectionAttr(
    allocator: std.mem.Allocator,
    entity: ifcparse.EntityRef,
    attribute_name: []const u8,
) QueryError!?ifcparse.EntityRef {
    const values = try getEntityCollectionAttr(allocator, entity, attribute_name);
    defer allocator.free(values);
    if (values.len == 0) return null;
    return values[0];
}

fn appendUniqueEntity(
    allocator: std.mem.Allocator,
    values: *std.ArrayList(ifcparse.EntityRef),
    seen: *std.AutoHashMap(u32, void),
    entity: ifcparse.EntityRef,
) QueryError!void {
    const id = entity.id();
    if (id == 0) return;
    if (seen.contains(id)) return;
    seen.put(id, {}) catch return error.OutOfMemory;
    values.append(allocator, entity) catch return error.OutOfMemory;
}

fn stringEqIgnoreCase(a: []const u8, b: []const u8) bool {
    return std.ascii.eqlIgnoreCase(a, b);
}

pub const PrimitiveValue = union(enum) {
    int: i32,
    bool: bool,
    logical: i32,
    double: f64,
    string: []u8,

    pub fn deinit(self: *PrimitiveValue, allocator: std.mem.Allocator) void {
        switch (self.*) {
            .string => |value| allocator.free(value),
            else => {},
        }
    }
};

fn deinitPrimitiveSlice(allocator: std.mem.Allocator, values: []PrimitiveValue) void {
    for (values) |*value| {
        value.deinit(allocator);
    }
    allocator.free(values);
}

pub const PropertyTableValue = struct {
    defining_values: []PrimitiveValue,
    defined_values: []PrimitiveValue,

    pub fn deinit(self: *PropertyTableValue, allocator: std.mem.Allocator) void {
        deinitPrimitiveSlice(allocator, self.defining_values);
        deinitPrimitiveSlice(allocator, self.defined_values);
        self.* = .{
            .defining_values = &.{},
            .defined_values = &.{},
        };
    }
};

pub const PropertyValue = union(enum) {
    none: void,
    primitive: PrimitiveValue,
    primitive_list: []PrimitiveValue,
    table: PropertyTableValue,
    unsupported: void,

    pub fn deinit(self: *PropertyValue, allocator: std.mem.Allocator) void {
        switch (self.*) {
            .primitive => |*value| value.deinit(allocator),
            .primitive_list => |values| deinitPrimitiveSlice(allocator, values),
            .table => |*table_values| table_values.deinit(allocator),
            else => {},
        }
        self.* = .{ .none = {} };
    }
};

pub const PsetProperty = struct {
    name: []u8,
    value: PropertyValue,
    property_entity: ?ifcparse.EntityRef = null,

    pub fn deinit(self: *PsetProperty, allocator: std.mem.Allocator) void {
        allocator.free(self.name);
        self.value.deinit(allocator);
        self.* = .{
            .name = &.{},
            .value = .{ .none = {} },
            .property_entity = null,
        };
    }
};

pub const PsetData = struct {
    name: []u8,
    definition: ifcparse.EntityRef,
    properties: []PsetProperty,

    pub fn deinit(self: *PsetData, allocator: std.mem.Allocator) void {
        allocator.free(self.name);
        for (self.properties) |*property| {
            property.deinit(allocator);
        }
        allocator.free(self.properties);
        self.* = .{
            .name = &.{},
            .definition = .{ .handle = null },
            .properties = &.{},
        };
    }

    pub fn propertyByName(self: *const PsetData, property_name: []const u8) ?PsetProperty {
        for (self.properties) |property| {
            if (std.mem.eql(u8, property.name, property_name)) return property;
        }
        return null;
    }
};

fn primitiveFromArgument(
    allocator: std.mem.Allocator,
    entity: ifcparse.EntityRef,
    index: usize,
) QueryError!?PrimitiveValue {
    return switch (entity.argumentValueType(index)) {
        .int => .{ .int = try entity.getInt(index) },
        .bool => .{ .bool = try entity.getBool(index) },
        .logical => .{ .logical = try entity.getLogical(index) },
        .double => .{ .double = try entity.getDouble(index) },
        .string, .enumeration, .binary => .{
            .string = allocator.dupe(u8, try entity.getString(index)) catch return error.OutOfMemory,
        },
        else => null,
    };
}

fn primitiveFromWrappedValueEntity(
    allocator: std.mem.Allocator,
    wrapped_value_entity: ifcparse.EntityRef,
) QueryError!?PrimitiveValue {
    if (wrapped_value_entity.argumentCount() == 0) return null;
    return primitiveFromArgument(allocator, wrapped_value_entity, 0);
}

fn primitiveListFromWrappedEntityCollectionAttr(
    allocator: std.mem.Allocator,
    entity: ifcparse.EntityRef,
    attribute_name: []const u8,
) QueryError![]PrimitiveValue {
    const values = try getEntityCollectionAttr(allocator, entity, attribute_name);
    defer allocator.free(values);

    var resolved_values = std.ArrayList(PrimitiveValue).empty;
    defer resolved_values.deinit(allocator);

    for (values) |value_entity| {
        const resolved = (try primitiveFromWrappedValueEntity(allocator, value_entity)) orelse continue;
        resolved_values.append(allocator, resolved) catch return error.OutOfMemory;
    }
    return resolved_values.toOwnedSlice(allocator) catch return error.OutOfMemory;
}

pub const element = struct {
    pub const PsetOptions = struct {
        psets_only: bool = false,
        qtos_only: bool = false,
        should_inherit: bool = true,
    };

    fn isDefinitionAllowed(
        allocator: std.mem.Allocator,
        definition: ifcparse.EntityRef,
        options: PsetOptions,
    ) QueryError!bool {
        if (options.psets_only and !(try definition.isA(allocator, "IfcPropertySet")) and !(try definition.isA(allocator, "IfcPreDefinedPropertySet"))) {
            return false;
        }
        if (options.qtos_only and !(try definition.isA(allocator, "IfcElementQuantity"))) {
            return false;
        }
        return true;
    }

    fn collectLocalPropertyDefinitions(
        allocator: std.mem.Allocator,
        element_entity: ifcparse.EntityRef,
        options: PsetOptions,
        out: *std.ArrayList(ifcparse.EntityRef),
    ) QueryError!void {
        if (try element_entity.isA(allocator, "IfcTypeObject")) {
            const definitions = try getEntityCollectionAttr(allocator, element_entity, "HasPropertySets");
            defer allocator.free(definitions);
            for (definitions) |definition| {
                if (!(try isDefinitionAllowed(allocator, definition, options))) continue;
                out.append(allocator, definition) catch return error.OutOfMemory;
            }
            return;
        }

        if ((try element_entity.isA(allocator, "IfcMaterialDefinition")) or
            (try element_entity.isA(allocator, "IfcProfileDef")) or
            (try element_entity.isA(allocator, "IfcMaterial")))
        {
            const definitions = try getEntityCollectionAttr(allocator, element_entity, "HasProperties");
            defer allocator.free(definitions);
            for (definitions) |definition| {
                if (!(try isDefinitionAllowed(allocator, definition, options))) continue;
                out.append(allocator, definition) catch return error.OutOfMemory;
            }
            return;
        }

        const relationships = try getEntityCollectionAttr(allocator, element_entity, "IsDefinedBy");
        defer allocator.free(relationships);
        for (relationships) |relationship| {
            if (!(try relationship.isA(allocator, "IfcRelDefinesByProperties"))) continue;
            const definition = (try getEntityAttr(allocator, relationship, "RelatingPropertyDefinition")) orelse continue;
            if (!(try isDefinitionAllowed(allocator, definition, options))) continue;
            out.append(allocator, definition) catch return error.OutOfMemory;
        }
    }

    pub fn getType(
        allocator: std.mem.Allocator,
        element_entity: ifcparse.EntityRef,
    ) QueryError!?ifcparse.EntityRef {
        if (try element_entity.isA(allocator, "IfcTypeObject")) return element_entity;

        const typed_by = try getEntityCollectionAttr(allocator, element_entity, "IsTypedBy");
        defer allocator.free(typed_by);
        for (typed_by) |relationship| {
            if (try getEntityAttr(allocator, relationship, "RelatingType")) |related_type| {
                return related_type;
            }
        }

        const defined_by = try getEntityCollectionAttr(allocator, element_entity, "IsDefinedBy");
        defer allocator.free(defined_by);
        for (defined_by) |relationship| {
            if (!(try relationship.isA(allocator, "IfcRelDefinesByType"))) continue;
            if (try getEntityAttr(allocator, relationship, "RelatingType")) |related_type| {
                return related_type;
            }
        }
        return null;
    }

    pub fn getTypes(
        allocator: std.mem.Allocator,
        type_entity: ifcparse.EntityRef,
    ) QueryError![]ifcparse.EntityRef {
        const object_type_of = try getEntityCollectionAttr(allocator, type_entity, "ObjectTypeOf");
        defer allocator.free(object_type_of);
        if (object_type_of.len > 0) {
            return getEntityCollectionAttr(allocator, object_type_of[0], "RelatedObjects");
        }

        const types = try getEntityCollectionAttr(allocator, type_entity, "Types");
        defer allocator.free(types);
        if (types.len > 0) {
            return getEntityCollectionAttr(allocator, types[0], "RelatedObjects");
        }

        return emptyEntitySlice(allocator);
    }

    pub fn getPredefinedType(
        allocator: std.mem.Allocator,
        element_entity: ifcparse.EntityRef,
    ) QueryError!?[]u8 {
        if (try getType(allocator, element_entity)) |element_type| {
            var predefined = try getStringAttrOwned(allocator, element_type, "PredefinedType");
            if (predefined) |value| {
                if (std.mem.eql(u8, value, "USERDEFINED") or value.len == 0) {
                    allocator.free(value);
                    predefined = null;
                } else if (!std.mem.eql(u8, value, "NOTDEFINED")) {
                    return value;
                } else {
                    allocator.free(value);
                    predefined = null;
                }
            }

            if (predefined == null) {
                if (try getStringAttrOwned(allocator, element_type, "ElementType")) |element_type_name| {
                    if (element_type_name.len > 0) return element_type_name;
                    allocator.free(element_type_name);
                }
                if (try getStringAttrOwned(allocator, element_type, "ProcessType")) |process_type_name| {
                    if (process_type_name.len > 0) return process_type_name;
                    allocator.free(process_type_name);
                }
            }
        }

        var predefined = try getStringAttrOwned(allocator, element_entity, "PredefinedType");
        if (predefined) |value| {
            if (std.mem.eql(u8, value, "USERDEFINED") or value.len == 0) {
                allocator.free(value);
                predefined = null;
            } else {
                return value;
            }
        }

        if (try getStringAttrOwned(allocator, element_entity, "ObjectType")) |object_type| {
            if (object_type.len > 0) return object_type;
            allocator.free(object_type);
        }
        return null;
    }

    pub fn isUserdefinedType(
        allocator: std.mem.Allocator,
        element_entity: ifcparse.EntityRef,
    ) QueryError!bool {
        if (try getType(allocator, element_entity)) |element_type| {
            if (try getStringAttrBorrowed(allocator, element_type, "PredefinedType")) |predefined| {
                if (std.mem.eql(u8, predefined, "USERDEFINED")) return true;
                if (predefined.len > 0 and !std.mem.eql(u8, predefined, "NOTDEFINED")) return false;
            }
            if (try getStringAttrBorrowed(allocator, element_type, "ElementType")) |element_type_name| {
                if (element_type_name.len > 0) return true;
            }
            if (try getStringAttrBorrowed(allocator, element_type, "ProcessType")) |process_type_name| {
                if (process_type_name.len > 0) return true;
            }
        }

        if (try getStringAttrBorrowed(allocator, element_entity, "PredefinedType")) |predefined| {
            if (std.mem.eql(u8, predefined, "USERDEFINED")) return true;
            if (predefined.len > 0 and !std.mem.eql(u8, predefined, "NOTDEFINED")) return false;
        }
        if (try getStringAttrBorrowed(allocator, element_entity, "ObjectType")) |object_type| {
            return object_type.len > 0;
        }
        return false;
    }

    pub fn getAggregate(
        allocator: std.mem.Allocator,
        element_entity: ifcparse.EntityRef,
    ) QueryError!?ifcparse.EntityRef {
        const decomposes = try getEntityCollectionAttr(allocator, element_entity, "Decomposes");
        defer allocator.free(decomposes);
        if (decomposes.len == 0) return null;

        const relationship = decomposes[0];
        if (try relationship.isA(allocator, "IfcRelNests")) return null;
        return getEntityAttr(allocator, relationship, "RelatingObject");
    }

    pub fn getNest(
        allocator: std.mem.Allocator,
        element_entity: ifcparse.EntityRef,
    ) QueryError!?ifcparse.EntityRef {
        const nests = try getEntityCollectionAttr(allocator, element_entity, "Nests");
        defer allocator.free(nests);
        if (nests.len > 0) {
            return getEntityAttr(allocator, nests[0], "RelatingObject");
        }

        const decomposes = try getEntityCollectionAttr(allocator, element_entity, "Decomposes");
        defer allocator.free(decomposes);
        if (decomposes.len == 0) return null;
        if (!(try decomposes[0].isA(allocator, "IfcRelNests"))) return null;
        return getEntityAttr(allocator, decomposes[0], "RelatingObject");
    }

    pub fn getFilledVoid(
        allocator: std.mem.Allocator,
        element_entity: ifcparse.EntityRef,
    ) QueryError!?ifcparse.EntityRef {
        const fills_voids = try getEntityCollectionAttr(allocator, element_entity, "FillsVoids");
        defer allocator.free(fills_voids);
        if (fills_voids.len == 0) return null;
        return getEntityAttr(allocator, fills_voids[0], "RelatingOpeningElement");
    }

    pub fn getVoidedElement(
        allocator: std.mem.Allocator,
        element_entity: ifcparse.EntityRef,
    ) QueryError!?ifcparse.EntityRef {
        const voids = try getEntityCollectionAttr(allocator, element_entity, "VoidsElements");
        defer allocator.free(voids);
        if (voids.len == 0) return null;
        return getEntityAttr(allocator, voids[0], "RelatingBuildingElement");
    }

    fn matchesContainerClass(
        allocator: std.mem.Allocator,
        container_entity: ifcparse.EntityRef,
        ifc_class: ?[]const u8,
    ) QueryError!bool {
        if (ifc_class == null) return true;
        return container_entity.isA(allocator, ifc_class.?);
    }

    fn getContainerImpl(
        allocator: std.mem.Allocator,
        element_entity: ifcparse.EntityRef,
        should_get_direct: bool,
        ifc_class: ?[]const u8,
        depth: u32,
    ) QueryError!?ifcparse.EntityRef {
        if (depth > 64) return null;

        const contained = try getEntityCollectionAttr(allocator, element_entity, "ContainedInStructure");
        defer allocator.free(contained);
        if (contained.len > 0) {
            var container = (try getEntityAttr(allocator, contained[0], "RelatingStructure")) orelse return null;
            if (should_get_direct) {
                if (try matchesContainerClass(allocator, container, ifc_class)) return container;
                return null;
            }
            if (ifc_class == null) return container;

            while (true) {
                if (try container.isA(allocator, ifc_class.?)) return container;
                container = (try getAggregate(allocator, container)) orelse break;
            }
            return null;
        }

        if (should_get_direct) return null;

        const parent = (try getParent(allocator, element_entity)) orelse return null;
        return getContainerImpl(allocator, parent, should_get_direct, ifc_class, depth + 1);
    }

    pub fn getContainer(
        allocator: std.mem.Allocator,
        element_entity: ifcparse.EntityRef,
        should_get_direct: bool,
        ifc_class: ?[]const u8,
    ) QueryError!?ifcparse.EntityRef {
        return getContainerImpl(allocator, element_entity, should_get_direct, ifc_class, 0);
    }

    pub fn getParent(
        allocator: std.mem.Allocator,
        element_entity: ifcparse.EntityRef,
    ) QueryError!?ifcparse.EntityRef {
        if (try getContainer(allocator, element_entity, true, null)) |container| return container;
        if (try getAggregate(allocator, element_entity)) |aggregate| return aggregate;
        if (try getNest(allocator, element_entity)) |nest| return nest;
        if (try getFilledVoid(allocator, element_entity)) |filled_void| return filled_void;
        if (try getVoidedElement(allocator, element_entity)) |voided| return voided;
        return null;
    }

    pub fn getMaterial(
        allocator: std.mem.Allocator,
        element_entity: ifcparse.EntityRef,
        should_skip_usage: bool,
        should_inherit: bool,
    ) QueryError!?ifcparse.EntityRef {
        const associations = try getEntityCollectionAttr(allocator, element_entity, "HasAssociations");
        defer allocator.free(associations);
        for (associations) |relationship| {
            if (!(try relationship.isA(allocator, "IfcRelAssociatesMaterial"))) continue;

            var material = (try getEntityAttr(allocator, relationship, "RelatingMaterial")) orelse continue;
            if (should_skip_usage) {
                if (try material.isA(allocator, "IfcMaterialLayerSetUsage")) {
                    material = (try getEntityAttr(allocator, material, "ForLayerSet")) orelse material;
                } else if (try material.isA(allocator, "IfcMaterialProfileSetUsage")) {
                    material = (try getEntityAttr(allocator, material, "ForProfileSet")) orelse material;
                }
            }
            return material;
        }

        if (!should_inherit) return null;

        const relating_type = (try getType(allocator, element_entity)) orelse return null;
        if (relating_type.id() == element_entity.id()) return null;
        return getMaterial(allocator, relating_type, should_skip_usage, false);
    }

    fn upsertPsetDefinition(
        allocator: std.mem.Allocator,
        definitions: *std.ArrayList(ifcparse.EntityRef),
        name_index: *std.StringHashMap(usize),
        definition: ifcparse.EntityRef,
        replace_existing: bool,
    ) QueryError!void {
        const definition_name = (try getStringAttrOwned(allocator, definition, "Name")) orelse return;
        if (name_index.get(definition_name)) |index| {
            if (replace_existing) {
                definitions.items[index] = definition;
            }
            allocator.free(definition_name);
            return;
        }
        const index = definitions.items.len;
        definitions.append(allocator, definition) catch return error.OutOfMemory;
        name_index.put(definition_name, index) catch return error.OutOfMemory;
    }

    pub fn getPsetEntities(
        allocator: std.mem.Allocator,
        element_entity: ifcparse.EntityRef,
        options: PsetOptions,
    ) QueryError![]ifcparse.EntityRef {
        var definitions = std.ArrayList(ifcparse.EntityRef).empty;
        defer definitions.deinit(allocator);

        var name_index = std.StringHashMap(usize).init(allocator);
        defer {
            var keys = name_index.keyIterator();
            while (keys.next()) |name_ptr| allocator.free(name_ptr.*);
            name_index.deinit();
        }

        if (options.should_inherit) {
            if (try getType(allocator, element_entity)) |element_type| {
                if (element_type.id() != element_entity.id()) {
                    const inherited = try getPsetEntities(allocator, element_type, .{
                        .psets_only = options.psets_only,
                        .qtos_only = options.qtos_only,
                        .should_inherit = false,
                    });
                    defer allocator.free(inherited);

                    for (inherited) |definition| {
                        try upsertPsetDefinition(allocator, &definitions, &name_index, definition, false);
                    }
                }
            }
        }

        var local_definitions = std.ArrayList(ifcparse.EntityRef).empty;
        defer local_definitions.deinit(allocator);
        try collectLocalPropertyDefinitions(allocator, element_entity, options, &local_definitions);

        for (local_definitions.items) |definition| {
            try upsertPsetDefinition(allocator, &definitions, &name_index, definition, true);
        }

        return definitions.toOwnedSlice(allocator) catch return error.OutOfMemory;
    }

    pub fn getPsetEntity(
        allocator: std.mem.Allocator,
        element_entity: ifcparse.EntityRef,
        pset_name: []const u8,
        options: PsetOptions,
    ) QueryError!?ifcparse.EntityRef {
        if (pset_name.len == 0) return error.InvalidArgument;

        const definitions = try getPsetEntities(allocator, element_entity, options);
        defer allocator.free(definitions);

        for (definitions) |definition| {
            const name = (try getStringAttrBorrowed(allocator, definition, "Name")) orelse continue;
            if (std.mem.eql(u8, name, pset_name)) return definition;
        }
        return null;
    }

    pub fn getPropertyEntities(
        allocator: std.mem.Allocator,
        definition_entity: ifcparse.EntityRef,
    ) QueryError![]ifcparse.EntityRef {
        if (try definition_entity.isA(allocator, "IfcPropertySet")) {
            return getEntityCollectionAttr(allocator, definition_entity, "HasProperties");
        }
        if (try definition_entity.isA(allocator, "IfcElementQuantity")) {
            return getEntityCollectionAttr(allocator, definition_entity, "Quantities");
        }
        if ((try definition_entity.isA(allocator, "IfcMaterialProperties")) or
            (try definition_entity.isA(allocator, "IfcProfileProperties")))
        {
            return getEntityCollectionAttr(allocator, definition_entity, "Properties");
        }
        if (try definition_entity.isA(allocator, "IfcExtendedMaterialProperties")) {
            return getEntityCollectionAttr(allocator, definition_entity, "ExtendedProperties");
        }
        return emptyEntitySlice(allocator);
    }

    pub fn getPropertyEntity(
        allocator: std.mem.Allocator,
        definition_entity: ifcparse.EntityRef,
        property_name: []const u8,
    ) QueryError!?ifcparse.EntityRef {
        if (property_name.len == 0) return error.InvalidArgument;

        const properties = try getPropertyEntities(allocator, definition_entity);
        defer allocator.free(properties);

        for (properties) |property_entity| {
            const current_name = (try getStringAttrBorrowed(allocator, property_entity, "Name")) orelse continue;
            if (std.mem.eql(u8, current_name, property_name)) return property_entity;
        }
        return null;
    }

    pub fn getPropertyValue(
        allocator: std.mem.Allocator,
        property_entity: ifcparse.EntityRef,
    ) QueryError!PropertyValue {
        if (try property_entity.isA(allocator, "IfcPropertySingleValue")) {
            const nominal_value = (try getEntityAttr(allocator, property_entity, "NominalValue")) orelse return .{ .none = {} };
            const primitive = (try primitiveFromWrappedValueEntity(allocator, nominal_value)) orelse return .{ .none = {} };
            return .{ .primitive = primitive };
        }
        if (try property_entity.isA(allocator, "IfcPhysicalSimpleQuantity")) {
            if (property_entity.argumentCount() <= 3) return .{ .none = {} };
            const primitive = (try primitiveFromArgument(allocator, property_entity, 3)) orelse return .{ .none = {} };
            return .{ .primitive = primitive };
        }
        if (try property_entity.isA(allocator, "IfcPropertyEnumeratedValue")) {
            const values = try primitiveListFromWrappedEntityCollectionAttr(allocator, property_entity, "EnumerationValues");
            if (values.len == 0) {
                allocator.free(values);
                return .{ .none = {} };
            }
            return .{ .primitive_list = values };
        }
        if (try property_entity.isA(allocator, "IfcPropertyListValue")) {
            const values = try primitiveListFromWrappedEntityCollectionAttr(allocator, property_entity, "ListValues");
            if (values.len == 0) {
                allocator.free(values);
                return .{ .none = {} };
            }
            return .{ .primitive_list = values };
        }
        if (try property_entity.isA(allocator, "IfcPropertyBoundedValue")) {
            var values = std.ArrayList(PrimitiveValue).empty;
            defer values.deinit(allocator);

            const bounds = [_][]const u8{
                "UpperBoundValue",
                "LowerBoundValue",
                "SetPointValue",
            };
            for (bounds) |bound_name| {
                const bound_value = (try getEntityAttr(allocator, property_entity, bound_name)) orelse continue;
                const primitive = (try primitiveFromWrappedValueEntity(allocator, bound_value)) orelse continue;
                values.append(allocator, primitive) catch return error.OutOfMemory;
            }

            if (values.items.len == 0) return .{ .none = {} };
            return .{ .primitive_list = values.toOwnedSlice(allocator) catch return error.OutOfMemory };
        }
        if (try property_entity.isA(allocator, "IfcPropertyTableValue")) {
            const defining_values = try primitiveListFromWrappedEntityCollectionAttr(allocator, property_entity, "DefiningValues");
            errdefer allocator.free(defining_values);
            const defined_values = try primitiveListFromWrappedEntityCollectionAttr(allocator, property_entity, "DefinedValues");
            errdefer allocator.free(defined_values);

            if (defining_values.len == 0 and defined_values.len == 0) {
                allocator.free(defining_values);
                allocator.free(defined_values);
                return .{ .none = {} };
            }

            return .{
                .table = .{
                    .defining_values = defining_values,
                    .defined_values = defined_values,
                },
            };
        }
        return .{ .unsupported = {} };
    }

    fn appendDefinitionProperties(
        allocator: std.mem.Allocator,
        definition_entity: ifcparse.EntityRef,
        out: *std.ArrayList(PsetProperty),
    ) QueryError!void {
        if (try definition_entity.isA(allocator, "IfcPreDefinedPropertySet")) {
            const argument_count = definition_entity.argumentCount();
            var argument_index: usize = 4;
            while (argument_index < argument_count) : (argument_index += 1) {
                const name = definition_entity.argumentName(argument_index) orelse continue;
                const primitive = (try primitiveFromArgument(allocator, definition_entity, argument_index)) orelse continue;

                const owned_name = allocator.dupe(u8, name) catch return error.OutOfMemory;
                errdefer allocator.free(owned_name);

                out.append(allocator, .{
                    .name = owned_name,
                    .value = .{ .primitive = primitive },
                    .property_entity = null,
                }) catch return error.OutOfMemory;
            }
            return;
        }

        const property_entities = try getPropertyEntities(allocator, definition_entity);
        defer allocator.free(property_entities);
        for (property_entities) |property_entity| {
            const property_name = (try getStringAttrOwned(allocator, property_entity, "Name")) orelse continue;
            errdefer allocator.free(property_name);

            const property_value = try getPropertyValue(allocator, property_entity);
            errdefer {
                var cleanup_value = property_value;
                cleanup_value.deinit(allocator);
            }

            out.append(allocator, .{
                .name = property_name,
                .value = property_value,
                .property_entity = property_entity,
            }) catch return error.OutOfMemory;
        }
    }

    fn buildPsetData(
        allocator: std.mem.Allocator,
        definition_entity: ifcparse.EntityRef,
    ) QueryError!PsetData {
        const pset_name = (try getStringAttrOwned(allocator, definition_entity, "Name")) orelse return error.QueryFailed;
        errdefer allocator.free(pset_name);

        var properties = std.ArrayList(PsetProperty).empty;
        defer properties.deinit(allocator);
        errdefer {
            for (properties.items) |*property| {
                property.deinit(allocator);
            }
        }
        try appendDefinitionProperties(allocator, definition_entity, &properties);

        return .{
            .name = pset_name,
            .definition = definition_entity,
            .properties = properties.toOwnedSlice(allocator) catch return error.OutOfMemory,
        };
    }

    pub fn getPset(
        allocator: std.mem.Allocator,
        element_entity: ifcparse.EntityRef,
        pset_name: []const u8,
        options: PsetOptions,
    ) QueryError!?PsetData {
        const definition = (try getPsetEntity(allocator, element_entity, pset_name, options)) orelse return null;
        const pset_data = try buildPsetData(allocator, definition);
        return pset_data;
    }

    pub fn getPsets(
        allocator: std.mem.Allocator,
        element_entity: ifcparse.EntityRef,
        options: PsetOptions,
    ) QueryError![]PsetData {
        const definitions = try getPsetEntities(allocator, element_entity, options);
        defer allocator.free(definitions);

        var psets = std.ArrayList(PsetData).empty;
        defer psets.deinit(allocator);
        errdefer {
            for (psets.items) |*pset| {
                pset.deinit(allocator);
            }
        }

        for (definitions) |definition| {
            const pset_data = try buildPsetData(allocator, definition);
            psets.append(allocator, pset_data) catch return error.OutOfMemory;
        }
        return psets.toOwnedSlice(allocator) catch return error.OutOfMemory;
    }
};

pub const classification = struct {
    pub fn getClassification(
        allocator: std.mem.Allocator,
        reference: ifcparse.EntityRef,
    ) QueryError!?ifcparse.EntityRef {
        var current: ?ifcparse.EntityRef = reference;
        var depth: u32 = 0;
        while (current) |ref| {
            if (depth > 64) return null;
            if (try ref.isA(allocator, "IfcClassification")) return ref;
            current = try getEntityAttr(allocator, ref, "ReferencedSource");
            depth += 1;
        }
        return null;
    }

    pub fn getInheritedReferences(
        allocator: std.mem.Allocator,
        reference: ?ifcparse.EntityRef,
    ) QueryError![]ifcparse.EntityRef {
        var values = std.ArrayList(ifcparse.EntityRef).empty;
        defer values.deinit(allocator);

        var current = reference;
        var depth: u32 = 0;
        while (current) |ref| {
            if (depth > 64) break;
            if (try ref.isA(allocator, "IfcClassification")) break;
            values.append(allocator, ref) catch return error.OutOfMemory;
            current = try getEntityAttr(allocator, ref, "ReferencedSource");
            depth += 1;
        }

        return values.toOwnedSlice(allocator) catch return error.OutOfMemory;
    }

    fn getOccurrenceReferences(
        allocator: std.mem.Allocator,
        element_entity: ifcparse.EntityRef,
    ) QueryError![]ifcparse.EntityRef {
        var values = std.ArrayList(ifcparse.EntityRef).empty;
        defer values.deinit(allocator);
        var seen = std.AutoHashMap(u32, void).init(allocator);
        defer seen.deinit();

        const associations = try getEntityCollectionAttr(allocator, element_entity, "HasAssociations");
        defer allocator.free(associations);
        for (associations) |relationship| {
            if (!(try relationship.isA(allocator, "IfcRelAssociatesClassification"))) continue;
            const reference = (try getEntityAttr(allocator, relationship, "RelatingClassification")) orelse continue;
            try appendUniqueEntity(allocator, &values, &seen, reference);
        }
        return values.toOwnedSlice(allocator) catch return error.OutOfMemory;
    }

    pub fn getReferences(
        allocator: std.mem.Allocator,
        element_entity: ifcparse.EntityRef,
        should_inherit: bool,
    ) QueryError![]ifcparse.EntityRef {
        if (!(try element_entity.isA(allocator, "IfcRoot"))) {
            var values = std.ArrayList(ifcparse.EntityRef).empty;
            defer values.deinit(allocator);
            var seen = std.AutoHashMap(u32, void).init(allocator);
            defer seen.deinit();

            var relationships = try getEntityCollectionAttr(allocator, element_entity, "HasExternalReferences");
            if (relationships.len == 0) {
                allocator.free(relationships);
                relationships = try getEntityCollectionAttr(allocator, element_entity, "HasExternalReference");
            }
            defer allocator.free(relationships);

            for (relationships) |relationship| {
                const reference = (try getEntityAttr(allocator, relationship, "RelatingReference")) orelse continue;
                try appendUniqueEntity(allocator, &values, &seen, reference);
            }
            return values.toOwnedSlice(allocator) catch return error.OutOfMemory;
        }

        var type_references = try emptyEntitySlice(allocator);
        defer allocator.free(type_references);
        if (should_inherit and (try element_entity.isA(allocator, "IfcObject"))) {
            if (try element.getType(allocator, element_entity)) |element_type| {
                if (element_type.id() != element_entity.id()) {
                    const inherited_references = try getReferences(allocator, element_type, true);
                    allocator.free(type_references);
                    type_references = inherited_references;
                }
            }
        }

        const occurrence_references = try getOccurrenceReferences(allocator, element_entity);
        defer allocator.free(occurrence_references);

        if (type_references.len == 0) {
            return allocator.dupe(ifcparse.EntityRef, occurrence_references) catch return error.OutOfMemory;
        }

        var per_system = std.AutoHashMap(u32, ifcparse.EntityRef).init(allocator);
        defer per_system.deinit();

        for (type_references) |reference| {
            const system = (try getClassification(allocator, reference)) orelse reference;
            per_system.put(system.id(), reference) catch return error.OutOfMemory;
        }
        for (occurrence_references) |reference| {
            const system = (try getClassification(allocator, reference)) orelse reference;
            per_system.put(system.id(), reference) catch return error.OutOfMemory;
        }

        var values = std.ArrayList(ifcparse.EntityRef).empty;
        defer values.deinit(allocator);
        var iterator = per_system.iterator();
        while (iterator.next()) |entry| {
            values.append(allocator, entry.value_ptr.*) catch return error.OutOfMemory;
        }
        return values.toOwnedSlice(allocator) catch return error.OutOfMemory;
    }
};

pub const unit = struct {
    pub const PropertyTableUnit = struct {
        defining_unit: ?ifcparse.EntityRef = null,
        defined_unit: ?ifcparse.EntityRef = null,
    };

    fn getPrefixMultiplier(prefix: ?[]const u8) f64 {
        if (prefix == null) return 1.0;
        const p = prefix.?;
        if (stringEqIgnoreCase(p, "EXA")) return 1e18;
        if (stringEqIgnoreCase(p, "PETA")) return 1e15;
        if (stringEqIgnoreCase(p, "TERA")) return 1e12;
        if (stringEqIgnoreCase(p, "GIGA")) return 1e9;
        if (stringEqIgnoreCase(p, "MEGA")) return 1e6;
        if (stringEqIgnoreCase(p, "KILO")) return 1e3;
        if (stringEqIgnoreCase(p, "HECTO")) return 1e2;
        if (stringEqIgnoreCase(p, "DECA")) return 1e1;
        if (stringEqIgnoreCase(p, "DECI")) return 1e-1;
        if (stringEqIgnoreCase(p, "CENTI")) return 1e-2;
        if (stringEqIgnoreCase(p, "MILLI")) return 1e-3;
        if (stringEqIgnoreCase(p, "MICRO")) return 1e-6;
        if (stringEqIgnoreCase(p, "NANO")) return 1e-9;
        if (stringEqIgnoreCase(p, "PICO")) return 1e-12;
        if (stringEqIgnoreCase(p, "FEMTO")) return 1e-15;
        if (stringEqIgnoreCase(p, "ATTO")) return 1e-18;
        return 1.0;
    }

    fn siConversion(unit_name: []const u8) ?f64 {
        if (stringEqIgnoreCase(unit_name, "thou")) return 0.0000254;
        if (stringEqIgnoreCase(unit_name, "inch")) return 0.0254;
        if (stringEqIgnoreCase(unit_name, "foot")) return 0.3048;
        if (stringEqIgnoreCase(unit_name, "yard")) return 0.914;
        if (stringEqIgnoreCase(unit_name, "mile")) return 1609.0;
        if (stringEqIgnoreCase(unit_name, "square inch")) return 0.0006452;
        if (stringEqIgnoreCase(unit_name, "square foot")) return 0.09290304;
        if (stringEqIgnoreCase(unit_name, "square yard")) return 0.83612736;
        if (stringEqIgnoreCase(unit_name, "cubic inch")) return 0.00001639;
        if (stringEqIgnoreCase(unit_name, "cubic foot")) return 0.02831684671168849;
        if (stringEqIgnoreCase(unit_name, "cubic yard")) return 0.7636;
        if (stringEqIgnoreCase(unit_name, "litre")) return 0.001;
        if (stringEqIgnoreCase(unit_name, "degree")) return std.math.pi / 180.0;
        if (stringEqIgnoreCase(unit_name, "ounce")) return 0.02835;
        if (stringEqIgnoreCase(unit_name, "pound")) return 0.454;
        if (stringEqIgnoreCase(unit_name, "tonne")) return 1000.0;
        if (stringEqIgnoreCase(unit_name, "psi")) return 6894.7572932;
        if (stringEqIgnoreCase(unit_name, "ksi")) return 6894757.2932;
        if (stringEqIgnoreCase(unit_name, "minute")) return 60.0;
        if (stringEqIgnoreCase(unit_name, "hour")) return 3600.0;
        if (stringEqIgnoreCase(unit_name, "day")) return 86400.0;
        if (stringEqIgnoreCase(unit_name, "btu")) return 1055.056;
        if (stringEqIgnoreCase(unit_name, "fahrenheit")) return 1.8;
        return null;
    }

    fn containsCaseInsensitive(haystack: []const u8, needle: []const u8) bool {
        if (needle.len == 0) return true;
        if (haystack.len < needle.len) return false;
        var i: usize = 0;
        while (i + needle.len <= haystack.len) : (i += 1) {
            if (stringEqIgnoreCase(haystack[i .. i + needle.len], needle)) return true;
        }
        return false;
    }

    fn measureClassToUnitType(allocator: std.mem.Allocator, measure_class: []const u8) QueryError!?[]u8 {
        if (measure_class.len == 0) return null;
        if (std.mem.eql(u8, measure_class, "IfcNumericMeasure")) {
            return allocator.dupe(u8, "USERDEFINED") catch return error.OutOfMemory;
        }

        var name = measure_class;
        if (std.mem.startsWith(u8, name, "Ifc")) name = name[3..];
        if (std.mem.endsWith(u8, name, "Measure")) name = name[0 .. name.len - "Measure".len];
        if (std.mem.startsWith(u8, name, "Non")) name = name[3..];
        if (std.mem.startsWith(u8, name, "Positive")) name = name["Positive".len..];
        if (std.mem.startsWith(u8, name, "Negative")) name = name["Negative".len..];
        if (name.len == 0) return null;

        var value = std.ArrayList(u8).empty;
        defer value.deinit(allocator);
        value.appendSlice(allocator, name) catch return error.OutOfMemory;
        for (value.items) |*c| c.* = std.ascii.toUpper(c.*);
        value.appendSlice(allocator, "UNIT") catch return error.OutOfMemory;
        return value.toOwnedSlice(allocator) catch return error.OutOfMemory;
    }

    fn measureClassFromSimpleQuantity(
        allocator: std.mem.Allocator,
        property_entity: ifcparse.EntityRef,
        file: *ifcparse.File,
    ) QueryError!?[]u8 {
        const schema_name = file.schemaIdentifier() orelse return null;
        const type_name = property_entity.typeName() orelse return null;
        const declaration = (try ifcparse.schemaDeclarationByName(allocator, schema_name, type_name)) orelse return null;
        var attributes = try declaration.attributes(true);
        defer attributes.deinit();
        const attribute = attributes.at(3) orelse return null;
        const parameter_type = (try attribute.parameterType()) orelse return null;
        const named_type = (try parameter_type.namedDeclaredType()) orelse return null;
        const named_type_name = named_type.name() orelse return null;
        return allocator.dupe(u8, named_type_name) catch return error.OutOfMemory;
    }

    fn firstMeasureClassFromCollection(
        allocator: std.mem.Allocator,
        property_entity: ifcparse.EntityRef,
        attribute_name: []const u8,
    ) QueryError!?[]u8 {
        const entity_value = (try getFirstEntityFromCollectionAttr(allocator, property_entity, attribute_name)) orelse return null;
        const type_name = entity_value.typeName() orelse return null;
        return allocator.dupe(u8, type_name) catch return error.OutOfMemory;
    }

    pub fn getProjectUnit(
        allocator: std.mem.Allocator,
        file: *ifcparse.File,
        unit_type: []const u8,
    ) QueryError!?ifcparse.EntityRef {
        return file.unitEntity(allocator, unit_type);
    }

    pub fn getPropertyUnit(
        allocator: std.mem.Allocator,
        property_entity: ifcparse.EntityRef,
        file: *ifcparse.File,
    ) QueryError!?ifcparse.EntityRef {
        if (try getEntityAttr(allocator, property_entity, "Unit")) |unit_entity| {
            return unit_entity;
        }

        var measure_class: ?[]u8 = null;
        defer if (measure_class) |value| allocator.free(value);

        if (try property_entity.isA(allocator, "IfcPhysicalSimpleQuantity")) {
            measure_class = try measureClassFromSimpleQuantity(allocator, property_entity, file);
        } else if (try property_entity.isA(allocator, "IfcPropertySingleValue")) {
            if (try getEntityAttr(allocator, property_entity, "NominalValue")) |nominal_value| {
                if (nominal_value.typeName()) |type_name| {
                    measure_class = allocator.dupe(u8, type_name) catch return error.OutOfMemory;
                }
            }
        } else if (try property_entity.isA(allocator, "IfcPropertyEnumeratedValue")) {
            if (try getEntityAttr(allocator, property_entity, "EnumerationReference")) |enumeration_reference| {
                if (try getEntityAttr(allocator, enumeration_reference, "Unit")) |unit_entity| {
                    return unit_entity;
                }
                measure_class = try firstMeasureClassFromCollection(allocator, enumeration_reference, "EnumerationValues");
            }
            if (measure_class == null) {
                measure_class = try firstMeasureClassFromCollection(allocator, property_entity, "EnumerationValues");
            }
        } else if (try property_entity.isA(allocator, "IfcPropertyListValue")) {
            measure_class = try firstMeasureClassFromCollection(allocator, property_entity, "ListValues");
        } else if (try property_entity.isA(allocator, "IfcPropertyBoundedValue")) {
            if (try getEntityAttr(allocator, property_entity, "UpperBoundValue")) |value| {
                if (value.typeName()) |type_name| measure_class = allocator.dupe(u8, type_name) catch return error.OutOfMemory;
            } else if (try getEntityAttr(allocator, property_entity, "LowerBoundValue")) |value| {
                if (value.typeName()) |type_name| measure_class = allocator.dupe(u8, type_name) catch return error.OutOfMemory;
            } else if (try getEntityAttr(allocator, property_entity, "SetPointValue")) |value| {
                if (value.typeName()) |type_name| measure_class = allocator.dupe(u8, type_name) catch return error.OutOfMemory;
            }
        }

        if (measure_class == null) return null;

        const unit_type = (try measureClassToUnitType(allocator, measure_class.?)) orelse return null;
        defer allocator.free(unit_type);
        return getProjectUnit(allocator, file, unit_type);
    }

    pub fn getPropertyTableUnit(
        allocator: std.mem.Allocator,
        property_entity: ifcparse.EntityRef,
        file: *ifcparse.File,
    ) QueryError!PropertyTableUnit {
        var result = PropertyTableUnit{};

        if (try getEntityAttr(allocator, property_entity, "DefiningUnit")) |unit_entity| {
            result.defining_unit = unit_entity;
        } else {
            if (try firstMeasureClassFromCollection(allocator, property_entity, "DefiningValues")) |measure_class| {
                defer allocator.free(measure_class);
                if (try measureClassToUnitType(allocator, measure_class)) |unit_type| {
                    defer allocator.free(unit_type);
                    result.defining_unit = try getProjectUnit(allocator, file, unit_type);
                }
            }
        }

        if (try getEntityAttr(allocator, property_entity, "DefinedUnit")) |unit_entity| {
            result.defined_unit = unit_entity;
        } else {
            if (try firstMeasureClassFromCollection(allocator, property_entity, "DefinedValues")) |measure_class| {
                defer allocator.free(measure_class);
                if (try measureClassToUnitType(allocator, measure_class)) |unit_type| {
                    defer allocator.free(unit_type);
                    result.defined_unit = try getProjectUnit(allocator, file, unit_type);
                }
            }
        }

        return result;
    }

    pub fn convert(
        value: f64,
        from_prefix: ?[]const u8,
        from_unit: []const u8,
        to_prefix: ?[]const u8,
        to_unit: []const u8,
    ) f64 {
        var converted = value;
        if (siConversion(from_unit)) |factor| {
            converted *= factor;
        } else if (from_prefix != null) {
            converted *= getPrefixMultiplier(from_prefix);
            if (containsCaseInsensitive(from_unit, "SQUARE")) {
                converted *= getPrefixMultiplier(from_prefix);
            } else if (containsCaseInsensitive(from_unit, "CUBIC")) {
                converted *= getPrefixMultiplier(from_prefix);
                converted *= getPrefixMultiplier(from_prefix);
            }
        }

        if (siConversion(to_unit)) |factor| {
            return converted * (1.0 / factor);
        } else if (to_prefix != null) {
            converted *= 1.0 / getPrefixMultiplier(to_prefix);
            if (containsCaseInsensitive(from_unit, "SQUARE")) {
                converted *= 1.0 / getPrefixMultiplier(to_prefix);
            } else if (containsCaseInsensitive(from_unit, "CUBIC")) {
                converted *= 1.0 / getPrefixMultiplier(to_prefix);
                converted *= 1.0 / getPrefixMultiplier(to_prefix);
            }
        }

        return converted;
    }

    pub fn calculateUnitScale(
        allocator: std.mem.Allocator,
        file: *ifcparse.File,
        unit_type: []const u8,
    ) QueryError!f64 {
        return file.unitScale(allocator, unit_type);
    }

    pub fn siTypeName(unit_type: []const u8) ?[]const u8 {
        if (stringEqIgnoreCase(unit_type, "ABSORBEDDOSEUNIT")) return "GRAY";
        if (stringEqIgnoreCase(unit_type, "AMOUNTOFSUBSTANCEUNIT")) return "MOLE";
        if (stringEqIgnoreCase(unit_type, "AREAUNIT")) return "SQUARE_METRE";
        if (stringEqIgnoreCase(unit_type, "DOSEEQUIVALENTUNIT")) return "SIEVERT";
        if (stringEqIgnoreCase(unit_type, "ELECTRICCAPACITANCEUNIT")) return "FARAD";
        if (stringEqIgnoreCase(unit_type, "ELECTRICCHARGEUNIT")) return "COULOMB";
        if (stringEqIgnoreCase(unit_type, "ELECTRICCONDUCTANCEUNIT")) return "SIEMENS";
        if (stringEqIgnoreCase(unit_type, "ELECTRICCURRENTUNIT")) return "AMPERE";
        if (stringEqIgnoreCase(unit_type, "ELECTRICRESISTANCEUNIT")) return "OHM";
        if (stringEqIgnoreCase(unit_type, "ELECTRICVOLTAGEUNIT")) return "VOLT";
        if (stringEqIgnoreCase(unit_type, "ENERGYUNIT")) return "JOULE";
        if (stringEqIgnoreCase(unit_type, "FORCEUNIT")) return "NEWTON";
        if (stringEqIgnoreCase(unit_type, "FREQUENCYUNIT")) return "HERTZ";
        if (stringEqIgnoreCase(unit_type, "ILLUMINANCEUNIT")) return "LUX";
        if (stringEqIgnoreCase(unit_type, "INDUCTANCEUNIT")) return "HENRY";
        if (stringEqIgnoreCase(unit_type, "LENGTHUNIT")) return "METRE";
        if (stringEqIgnoreCase(unit_type, "LUMINOUSFLUXUNIT")) return "LUMEN";
        if (stringEqIgnoreCase(unit_type, "LUMINOUSINTENSITYUNIT")) return "CANDELA";
        if (stringEqIgnoreCase(unit_type, "MAGNETICFLUXDENSITYUNIT")) return "TESLA";
        if (stringEqIgnoreCase(unit_type, "MAGNETICFLUXUNIT")) return "WEBER";
        if (stringEqIgnoreCase(unit_type, "MASSUNIT")) return "GRAM";
        if (stringEqIgnoreCase(unit_type, "PLANEANGLEUNIT")) return "RADIAN";
        if (stringEqIgnoreCase(unit_type, "POWERUNIT")) return "WATT";
        if (stringEqIgnoreCase(unit_type, "PRESSUREUNIT")) return "PASCAL";
        if (stringEqIgnoreCase(unit_type, "RADIOACTIVITYUNIT")) return "BECQUEREL";
        if (stringEqIgnoreCase(unit_type, "SOLIDANGLEUNIT")) return "STERADIAN";
        if (stringEqIgnoreCase(unit_type, "THERMODYNAMICTEMPERATUREUNIT")) return "KELVIN";
        if (stringEqIgnoreCase(unit_type, "TIMEUNIT")) return "SECOND";
        if (stringEqIgnoreCase(unit_type, "VOLUMEUNIT")) return "CUBIC_METRE";
        if (stringEqIgnoreCase(unit_type, "USERDEFINED")) return "METRE";
        return null;
    }
};
