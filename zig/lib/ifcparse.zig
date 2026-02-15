const std = @import("std");

pub const c = @cImport({
    @cInclude("ifcparse/c_api.h");
});

pub const FileType = enum(c.ifcopenshell_ifcparse_filetype_t) {
    ifcspf = c.IFCOPENSHELL_IFCPARSE_FT_IFCSPF,
    ifcxml = c.IFCOPENSHELL_IFCPARSE_FT_IFCXML,
    ifczip = c.IFCOPENSHELL_IFCPARSE_FT_IFCZIP,
    rocksdb = c.IFCOPENSHELL_IFCPARSE_FT_ROCKSDB,
    unknown = c.IFCOPENSHELL_IFCPARSE_FT_UNKNOWN,
    auto_detect = c.IFCOPENSHELL_IFCPARSE_FT_AUTODETECT,
};

pub const ArgumentType = enum(c.ifcopenshell_ifcparse_argument_type_t) {
    null = c.IFCOPENSHELL_IFCPARSE_ARG_NULL,
    derived = c.IFCOPENSHELL_IFCPARSE_ARG_DERIVED,
    int = c.IFCOPENSHELL_IFCPARSE_ARG_INT,
    bool = c.IFCOPENSHELL_IFCPARSE_ARG_BOOL,
    logical = c.IFCOPENSHELL_IFCPARSE_ARG_LOGICAL,
    double = c.IFCOPENSHELL_IFCPARSE_ARG_DOUBLE,
    string = c.IFCOPENSHELL_IFCPARSE_ARG_STRING,
    binary = c.IFCOPENSHELL_IFCPARSE_ARG_BINARY,
    enumeration = c.IFCOPENSHELL_IFCPARSE_ARG_ENUMERATION,
    entity_instance = c.IFCOPENSHELL_IFCPARSE_ARG_ENTITY_INSTANCE,
    empty_aggregate = c.IFCOPENSHELL_IFCPARSE_ARG_EMPTY_AGGREGATE,
    aggregate_of_int = c.IFCOPENSHELL_IFCPARSE_ARG_AGGREGATE_OF_INT,
    aggregate_of_double = c.IFCOPENSHELL_IFCPARSE_ARG_AGGREGATE_OF_DOUBLE,
    aggregate_of_string = c.IFCOPENSHELL_IFCPARSE_ARG_AGGREGATE_OF_STRING,
    aggregate_of_binary = c.IFCOPENSHELL_IFCPARSE_ARG_AGGREGATE_OF_BINARY,
    aggregate_of_entity_instance = c.IFCOPENSHELL_IFCPARSE_ARG_AGGREGATE_OF_ENTITY_INSTANCE,
    aggregate_of_empty_aggregate = c.IFCOPENSHELL_IFCPARSE_ARG_AGGREGATE_OF_EMPTY_AGGREGATE,
    aggregate_of_aggregate_of_int = c.IFCOPENSHELL_IFCPARSE_ARG_AGGREGATE_OF_AGGREGATE_OF_INT,
    aggregate_of_aggregate_of_double = c.IFCOPENSHELL_IFCPARSE_ARG_AGGREGATE_OF_AGGREGATE_OF_DOUBLE,
    aggregate_of_aggregate_of_entity_instance = c.IFCOPENSHELL_IFCPARSE_ARG_AGGREGATE_OF_AGGREGATE_OF_ENTITY_INSTANCE,
    unknown = c.IFCOPENSHELL_IFCPARSE_ARG_UNKNOWN,
    _,
};

pub const DeclarationKind = enum(c.ifcopenshell_ifcparse_declaration_kind_t) {
    unknown = c.IFCOPENSHELL_IFCPARSE_DECL_UNKNOWN,
    type_declaration = c.IFCOPENSHELL_IFCPARSE_DECL_TYPE_DECLARATION,
    select_type = c.IFCOPENSHELL_IFCPARSE_DECL_SELECT_TYPE,
    enumeration_type = c.IFCOPENSHELL_IFCPARSE_DECL_ENUMERATION_TYPE,
    entity = c.IFCOPENSHELL_IFCPARSE_DECL_ENTITY,
    _,
};

pub const ParameterTypeKind = enum(c.ifcopenshell_ifcparse_parameter_type_kind_t) {
    unknown = c.IFCOPENSHELL_IFCPARSE_PARAM_UNKNOWN,
    named = c.IFCOPENSHELL_IFCPARSE_PARAM_NAMED,
    simple = c.IFCOPENSHELL_IFCPARSE_PARAM_SIMPLE,
    aggregation = c.IFCOPENSHELL_IFCPARSE_PARAM_AGGREGATION,
    _,
};

pub const SimpleType = enum(c.ifcopenshell_ifcparse_simple_type_t) {
    binary = c.IFCOPENSHELL_IFCPARSE_SIMPLE_BINARY,
    boolean = c.IFCOPENSHELL_IFCPARSE_SIMPLE_BOOLEAN,
    integer = c.IFCOPENSHELL_IFCPARSE_SIMPLE_INTEGER,
    logical = c.IFCOPENSHELL_IFCPARSE_SIMPLE_LOGICAL,
    number = c.IFCOPENSHELL_IFCPARSE_SIMPLE_NUMBER,
    real = c.IFCOPENSHELL_IFCPARSE_SIMPLE_REAL,
    string = c.IFCOPENSHELL_IFCPARSE_SIMPLE_STRING,
    invalid = c.IFCOPENSHELL_IFCPARSE_SIMPLE_INVALID,
    _,
};

pub const AggregationType = enum(c.ifcopenshell_ifcparse_aggregation_type_t) {
    array = c.IFCOPENSHELL_IFCPARSE_AGGR_ARRAY,
    bag = c.IFCOPENSHELL_IFCPARSE_AGGR_BAG,
    list = c.IFCOPENSHELL_IFCPARSE_AGGR_LIST,
    set = c.IFCOPENSHELL_IFCPARSE_AGGR_SET,
    invalid = c.IFCOPENSHELL_IFCPARSE_AGGR_INVALID,
    _,
};

pub const InverseAggregationType = enum(c.ifcopenshell_ifcparse_inverse_aggregation_type_t) {
    bag = c.IFCOPENSHELL_IFCPARSE_INV_AGGR_BAG,
    set = c.IFCOPENSHELL_IFCPARSE_INV_AGGR_SET,
    unspecified = c.IFCOPENSHELL_IFCPARSE_INV_AGGR_UNSPECIFIED,
    invalid = c.IFCOPENSHELL_IFCPARSE_INV_AGGR_INVALID,
    _,
};

pub const AttributeCategory = enum(c_int) {
    invalid = 0,
    forward = 1,
    inverse = 2,
    _,
};

pub const OpenStatus = enum(c.ifcopenshell_ifcparse_status_t) {
    success = c.IFCOPENSHELL_IFCPARSE_STATUS_SUCCESS,
    read_error = c.IFCOPENSHELL_IFCPARSE_STATUS_READ_ERROR,
    no_header = c.IFCOPENSHELL_IFCPARSE_STATUS_NO_HEADER,
    unsupported_schema = c.IFCOPENSHELL_IFCPARSE_STATUS_UNSUPPORTED_SCHEMA,
    invalid_syntax = c.IFCOPENSHELL_IFCPARSE_STATUS_INVALID_SYNTAX,
    unknown = c.IFCOPENSHELL_IFCPARSE_STATUS_UNKNOWN,
    _,
};

pub const OpenOptions = struct {
    file_type: FileType = .auto_detect,
    readonly: bool = false,
};

pub const OpenError = error{
    InvalidArgument,
    ReadError,
    NoHeader,
    UnsupportedSchema,
    InvalidSyntax,
    UnknownStatus,
    OpenFailed,
};

pub const QueryError = error{
    InvalidArgument,
    OutOfMemory,
    QueryFailed,
};

pub const EntityRef = struct {
    handle: ?*const c.ifcopenshell_ifcparse_entity_ref_t,

    pub fn id(self: EntityRef) u32 {
        if (self.handle == null) return 0;
        return @intCast(c.ifcopenshell_ifcparse_entity_id(self.handle.?));
    }

    pub fn typeName(self: EntityRef) ?[]const u8 {
        if (self.handle == null) return null;
        const raw_name = c.ifcopenshell_ifcparse_entity_type_name(self.handle.?);
        if (raw_name == null) return null;
        return std.mem.span(raw_name);
    }

    pub fn typeNameWithSchema(self: EntityRef) ?[]const u8 {
        if (self.handle == null) return null;
        const raw_name = c.ifcopenshell_ifcparse_entity_type_name_with_schema(self.handle.?);
        if (raw_name == null) return null;
        return std.mem.span(raw_name);
    }

    pub fn isA(
        self: EntityRef,
        allocator: std.mem.Allocator,
        type_name: []const u8,
    ) QueryError!bool {
        if (self.handle == null) return error.InvalidArgument;
        if (type_name.len == 0) return error.InvalidArgument;

        const type_name_z = allocator.dupeZ(u8, type_name) catch return error.OutOfMemory;
        defer allocator.free(type_name_z);

        const result = c.ifcopenshell_ifcparse_entity_is_a(self.handle.?, type_name_z.ptr);
        if (result == 0 and lastError().len > 0) return error.QueryFailed;
        return result != 0;
    }

    pub fn attributeCategory(
        self: EntityRef,
        allocator: std.mem.Allocator,
        attribute_name: []const u8,
    ) QueryError!AttributeCategory {
        if (self.handle == null) return error.InvalidArgument;
        if (attribute_name.len == 0) return error.InvalidArgument;

        const attribute_name_z = allocator.dupeZ(u8, attribute_name) catch return error.OutOfMemory;
        defer allocator.free(attribute_name_z);

        const raw_category = c.ifcopenshell_ifcparse_entity_attribute_category(self.handle.?, attribute_name_z.ptr);
        if (raw_category == 0 and lastError().len > 0) return error.QueryFailed;
        return @enumFromInt(raw_category);
    }

    pub fn attributeNames(self: EntityRef) QueryError!StringList {
        if (self.handle == null) return error.InvalidArgument;
        const raw_list = c.ifcopenshell_ifcparse_entity_attribute_names(self.handle.?);
        if (raw_list == null) return error.QueryFailed;
        return StringList{ .handle = raw_list };
    }

    pub fn inverseAttributeNames(self: EntityRef) QueryError!StringList {
        if (self.handle == null) return error.InvalidArgument;
        const raw_list = c.ifcopenshell_ifcparse_entity_inverse_attribute_names(self.handle.?);
        if (raw_list == null) return error.QueryFailed;
        return StringList{ .handle = raw_list };
    }

    pub fn argumentCount(self: EntityRef) usize {
        if (self.handle == null) return 0;
        return c.ifcopenshell_ifcparse_entity_argument_count(self.handle.?);
    }

    pub fn argumentIndex(
        self: EntityRef,
        allocator: std.mem.Allocator,
        argument_name: []const u8,
    ) QueryError!usize {
        if (self.handle == null) return error.InvalidArgument;
        if (argument_name.len == 0) return error.InvalidArgument;

        const argument_name_z = allocator.dupeZ(u8, argument_name) catch return error.OutOfMemory;
        defer allocator.free(argument_name_z);

        const index = c.ifcopenshell_ifcparse_entity_argument_index(self.handle.?, argument_name_z.ptr);
        if (index < 0) return error.QueryFailed;
        return @intCast(index);
    }

    pub fn argumentName(self: EntityRef, index: usize) ?[]const u8 {
        if (self.handle == null) return null;
        const raw_name = c.ifcopenshell_ifcparse_entity_argument_name(self.handle.?, index);
        if (raw_name == null) return null;
        return std.mem.span(raw_name);
    }

    pub fn argumentDeclaredType(self: EntityRef, index: usize) ArgumentType {
        if (self.handle == null) return .unknown;
        return @enumFromInt(c.ifcopenshell_ifcparse_entity_argument_declared_type(self.handle.?, index));
    }

    pub fn argumentValueType(self: EntityRef, index: usize) ArgumentType {
        if (self.handle == null) return .unknown;
        return @enumFromInt(c.ifcopenshell_ifcparse_entity_argument_value_type(self.handle.?, index));
    }

    pub fn argumentIsNull(self: EntityRef, index: usize) QueryError!bool {
        if (self.handle == null) return error.InvalidArgument;
        const result = c.ifcopenshell_ifcparse_entity_argument_is_null(self.handle.?, index);
        if (result == 0 and lastError().len > 0) return error.QueryFailed;
        return result != 0;
    }

    pub fn toSpf(self: EntityRef, valid_spf: bool) QueryError![]const u8 {
        if (self.handle == null) return error.InvalidArgument;
        const raw_value = c.ifcopenshell_ifcparse_entity_to_string(self.handle.?, if (valid_spf) 1 else 0);
        if (raw_value == null) return error.QueryFailed;
        return std.mem.span(raw_value);
    }

    pub fn getInt(self: EntityRef, index: usize) QueryError!i32 {
        if (self.handle == null) return error.InvalidArgument;
        var value: c_int = 0;
        if (c.ifcopenshell_ifcparse_entity_get_argument_as_int(self.handle.?, index, &value) == 0) {
            return error.QueryFailed;
        }
        return @intCast(value);
    }

    pub fn getBool(self: EntityRef, index: usize) QueryError!bool {
        if (self.handle == null) return error.InvalidArgument;
        var value: c_int = 0;
        if (c.ifcopenshell_ifcparse_entity_get_argument_as_bool(self.handle.?, index, &value) == 0) {
            return error.QueryFailed;
        }
        return value != 0;
    }

    pub fn getLogical(self: EntityRef, index: usize) QueryError!i32 {
        if (self.handle == null) return error.InvalidArgument;
        var value: c_int = 0;
        if (c.ifcopenshell_ifcparse_entity_get_argument_as_logical(self.handle.?, index, &value) == 0) {
            return error.QueryFailed;
        }
        return value;
    }

    pub fn getDouble(self: EntityRef, index: usize) QueryError!f64 {
        if (self.handle == null) return error.InvalidArgument;
        var value: f64 = 0.0;
        if (c.ifcopenshell_ifcparse_entity_get_argument_as_double(self.handle.?, index, &value) == 0) {
            return error.QueryFailed;
        }
        return value;
    }

    pub fn getString(self: EntityRef, index: usize) QueryError![]const u8 {
        if (self.handle == null) return error.InvalidArgument;
        const raw_value = c.ifcopenshell_ifcparse_entity_get_argument_as_string(self.handle.?, index);
        if (raw_value == null) return error.QueryFailed;
        return std.mem.span(raw_value);
    }

    pub fn getEntity(self: EntityRef, index: usize) QueryError!?EntityRef {
        if (self.handle == null) return error.InvalidArgument;
        const raw_entity = c.ifcopenshell_ifcparse_entity_get_argument_as_entity(self.handle.?, index);
        if (raw_entity == null) {
            if (lastError().len > 0) return error.QueryFailed;
            return null;
        }
        return EntityRef{ .handle = raw_entity };
    }

    pub fn getIntList(self: EntityRef, index: usize) QueryError!IntList {
        if (self.handle == null) return error.InvalidArgument;
        const raw_list = c.ifcopenshell_ifcparse_entity_get_argument_as_int_list(self.handle.?, index);
        if (raw_list == null) return error.QueryFailed;
        return IntList{ .handle = raw_list };
    }

    pub fn getDoubleList(self: EntityRef, index: usize) QueryError!DoubleList {
        if (self.handle == null) return error.InvalidArgument;
        const raw_list = c.ifcopenshell_ifcparse_entity_get_argument_as_double_list(self.handle.?, index);
        if (raw_list == null) return error.QueryFailed;
        return DoubleList{ .handle = raw_list };
    }

    pub fn getStringList(self: EntityRef, index: usize) QueryError!StringList {
        if (self.handle == null) return error.InvalidArgument;
        const raw_list = c.ifcopenshell_ifcparse_entity_get_argument_as_string_list(self.handle.?, index);
        if (raw_list == null) return error.QueryFailed;
        return StringList{ .handle = raw_list };
    }

    pub fn getEntityList(self: EntityRef, index: usize) QueryError!EntityList {
        if (self.handle == null) return error.InvalidArgument;
        const raw_list = c.ifcopenshell_ifcparse_entity_get_argument_as_entity_list(self.handle.?, index);
        if (raw_list == null) return error.QueryFailed;
        return EntityList{ .handle = raw_list };
    }

    pub fn getInverse(
        self: EntityRef,
        allocator: std.mem.Allocator,
        inverse_name: []const u8,
    ) QueryError!EntityList {
        if (self.handle == null) return error.InvalidArgument;
        if (inverse_name.len == 0) return error.InvalidArgument;

        const inverse_name_z = allocator.dupeZ(u8, inverse_name) catch return error.OutOfMemory;
        defer allocator.free(inverse_name_z);

        const raw_list = c.ifcopenshell_ifcparse_entity_get_inverse(self.handle.?, inverse_name_z.ptr);
        if (raw_list == null) return error.QueryFailed;
        return EntityList{ .handle = raw_list };
    }

    pub fn getIntMatrix(self: EntityRef, index: usize) QueryError!IntMatrix {
        if (self.handle == null) return error.InvalidArgument;
        const raw_matrix = c.ifcopenshell_ifcparse_entity_get_argument_as_int_matrix(self.handle.?, index);
        if (raw_matrix == null) return error.QueryFailed;
        return IntMatrix{ .handle = raw_matrix };
    }

    pub fn getDoubleMatrix(self: EntityRef, index: usize) QueryError!DoubleMatrix {
        if (self.handle == null) return error.InvalidArgument;
        const raw_matrix = c.ifcopenshell_ifcparse_entity_get_argument_as_double_matrix(self.handle.?, index);
        if (raw_matrix == null) return error.QueryFailed;
        return DoubleMatrix{ .handle = raw_matrix };
    }

    pub fn getEntityMatrix(self: EntityRef, index: usize) QueryError!EntityMatrix {
        if (self.handle == null) return error.InvalidArgument;
        const raw_matrix = c.ifcopenshell_ifcparse_entity_get_argument_as_entity_matrix(self.handle.?, index);
        if (raw_matrix == null) return error.QueryFailed;
        return EntityMatrix{ .handle = raw_matrix };
    }

    pub fn setNull(self: EntityRef, index: usize) QueryError!void {
        if (self.handle == null) return error.InvalidArgument;
        if (c.ifcopenshell_ifcparse_entity_set_argument_null(self.handle.?, index) == 0) {
            return error.QueryFailed;
        }
    }

    pub fn unset(self: EntityRef, index: usize) QueryError!void {
        if (self.handle == null) return error.InvalidArgument;
        if (c.ifcopenshell_ifcparse_entity_unset_argument(self.handle.?, index) == 0) {
            return error.QueryFailed;
        }
    }

    pub fn setInt(self: EntityRef, index: usize, value: i32) QueryError!void {
        if (self.handle == null) return error.InvalidArgument;
        if (c.ifcopenshell_ifcparse_entity_set_argument_int(self.handle.?, index, @intCast(value)) == 0) {
            return error.QueryFailed;
        }
    }

    pub fn setBool(self: EntityRef, index: usize, value: bool) QueryError!void {
        if (self.handle == null) return error.InvalidArgument;
        if (c.ifcopenshell_ifcparse_entity_set_argument_bool(self.handle.?, index, if (value) 1 else 0) == 0) {
            return error.QueryFailed;
        }
    }

    pub fn setLogical(self: EntityRef, index: usize, value: i32) QueryError!void {
        if (self.handle == null) return error.InvalidArgument;
        if (c.ifcopenshell_ifcparse_entity_set_argument_logical(self.handle.?, index, @intCast(value)) == 0) {
            return error.QueryFailed;
        }
    }

    pub fn setDouble(self: EntityRef, index: usize, value: f64) QueryError!void {
        if (self.handle == null) return error.InvalidArgument;
        if (c.ifcopenshell_ifcparse_entity_set_argument_double(self.handle.?, index, value) == 0) {
            return error.QueryFailed;
        }
    }

    pub fn setString(
        self: EntityRef,
        allocator: std.mem.Allocator,
        index: usize,
        value: []const u8,
    ) QueryError!void {
        if (self.handle == null) return error.InvalidArgument;
        const value_z = allocator.dupeZ(u8, value) catch return error.OutOfMemory;
        defer allocator.free(value_z);
        if (c.ifcopenshell_ifcparse_entity_set_argument_string(self.handle.?, index, value_z.ptr) == 0) {
            return error.QueryFailed;
        }
    }

    pub fn setEntity(self: EntityRef, index: usize, value: EntityRef) QueryError!void {
        if (self.handle == null or value.handle == null) return error.InvalidArgument;
        if (c.ifcopenshell_ifcparse_entity_set_argument_entity(self.handle.?, index, value.handle.?) == 0) {
            return error.QueryFailed;
        }
    }

    pub fn setIntList(self: EntityRef, index: usize, values: []const i32) QueryError!void {
        if (self.handle == null) return error.InvalidArgument;
        const ptr: ?[*]const c_int = if (values.len == 0) null else @ptrCast(values.ptr);
        if (c.ifcopenshell_ifcparse_entity_set_argument_int_list(self.handle.?, index, ptr, values.len) == 0) {
            return error.QueryFailed;
        }
    }

    pub fn setDoubleList(self: EntityRef, index: usize, values: []const f64) QueryError!void {
        if (self.handle == null) return error.InvalidArgument;
        const ptr: ?[*]const f64 = if (values.len == 0) null else values.ptr;
        if (c.ifcopenshell_ifcparse_entity_set_argument_double_list(self.handle.?, index, ptr, values.len) == 0) {
            return error.QueryFailed;
        }
    }

    pub fn setStringList(
        self: EntityRef,
        allocator: std.mem.Allocator,
        index: usize,
        values: []const []const u8,
    ) QueryError!void {
        if (self.handle == null) return error.InvalidArgument;

        const c_values = allocator.alloc([:0]u8, values.len) catch return error.OutOfMemory;
        defer allocator.free(c_values);
        for (values, 0..) |value, i| {
            c_values[i] = allocator.dupeZ(u8, value) catch return error.OutOfMemory;
        }
        defer for (c_values) |value_z| allocator.free(value_z);

        const ptr_values = allocator.alloc([*:0]const u8, values.len) catch return error.OutOfMemory;
        defer allocator.free(ptr_values);
        for (c_values, 0..) |value_z, i| {
            ptr_values[i] = value_z.ptr;
        }

        const ptr: ?[*]const [*:0]const u8 = if (ptr_values.len == 0) null else ptr_values.ptr;
        if (c.ifcopenshell_ifcparse_entity_set_argument_string_list(self.handle.?, index, @ptrCast(ptr), values.len) == 0) {
            return error.QueryFailed;
        }
    }

    pub fn setEntityList(
        self: EntityRef,
        allocator: std.mem.Allocator,
        index: usize,
        values: []const EntityRef,
    ) QueryError!void {
        if (self.handle == null) return error.InvalidArgument;

        const ptr_values = allocator.alloc(*const c.ifcopenshell_ifcparse_entity_ref_t, values.len) catch return error.OutOfMemory;
        defer allocator.free(ptr_values);

        for (values, 0..) |value, i| {
            ptr_values[i] = value.handle orelse return error.InvalidArgument;
        }

        const ptr: ?[*]const *const c.ifcopenshell_ifcparse_entity_ref_t = if (ptr_values.len == 0) null else ptr_values.ptr;
        if (c.ifcopenshell_ifcparse_entity_set_argument_entity_list(self.handle.?, index, @ptrCast(ptr), values.len) == 0) {
            return error.QueryFailed;
        }
    }

    pub fn setIntMatrix(
        self: EntityRef,
        allocator: std.mem.Allocator,
        index: usize,
        values: []const []const i32,
    ) QueryError!void {
        if (self.handle == null) return error.InvalidArgument;

        var total_count: usize = 0;
        for (values) |row| {
            total_count += row.len;
        }

        const flat_values = allocator.alloc(i32, total_count) catch return error.OutOfMemory;
        defer allocator.free(flat_values);

        const row_offsets = allocator.alloc(usize, values.len + 1) catch return error.OutOfMemory;
        defer allocator.free(row_offsets);

        row_offsets[0] = 0;
        var cursor: usize = 0;
        for (values, 0..) |row, row_index| {
            for (row) |value| {
                flat_values[cursor] = value;
                cursor += 1;
            }
            row_offsets[row_index + 1] = cursor;
        }

        const values_ptr: ?[*]const c_int = if (flat_values.len == 0) null else @ptrCast(flat_values.ptr);
        if (c.ifcopenshell_ifcparse_entity_set_argument_int_matrix(
            self.handle.?,
            index,
            values_ptr,
            flat_values.len,
            row_offsets.ptr,
            values.len,
        ) == 0) {
            return error.QueryFailed;
        }
    }

    pub fn setDoubleMatrix(
        self: EntityRef,
        allocator: std.mem.Allocator,
        index: usize,
        values: []const []const f64,
    ) QueryError!void {
        if (self.handle == null) return error.InvalidArgument;

        var total_count: usize = 0;
        for (values) |row| {
            total_count += row.len;
        }

        const flat_values = allocator.alloc(f64, total_count) catch return error.OutOfMemory;
        defer allocator.free(flat_values);

        const row_offsets = allocator.alloc(usize, values.len + 1) catch return error.OutOfMemory;
        defer allocator.free(row_offsets);

        row_offsets[0] = 0;
        var cursor: usize = 0;
        for (values, 0..) |row, row_index| {
            for (row) |value| {
                flat_values[cursor] = value;
                cursor += 1;
            }
            row_offsets[row_index + 1] = cursor;
        }

        const values_ptr: ?[*]const f64 = if (flat_values.len == 0) null else flat_values.ptr;
        if (c.ifcopenshell_ifcparse_entity_set_argument_double_matrix(
            self.handle.?,
            index,
            values_ptr,
            flat_values.len,
            row_offsets.ptr,
            values.len,
        ) == 0) {
            return error.QueryFailed;
        }
    }

    pub fn setEntityMatrix(
        self: EntityRef,
        allocator: std.mem.Allocator,
        index: usize,
        values: []const []const EntityRef,
    ) QueryError!void {
        if (self.handle == null) return error.InvalidArgument;

        var total_count: usize = 0;
        for (values) |row| {
            total_count += row.len;
        }

        const flat_values = allocator.alloc(*const c.ifcopenshell_ifcparse_entity_ref_t, total_count) catch return error.OutOfMemory;
        defer allocator.free(flat_values);

        const row_offsets = allocator.alloc(usize, values.len + 1) catch return error.OutOfMemory;
        defer allocator.free(row_offsets);

        row_offsets[0] = 0;
        var cursor: usize = 0;
        for (values, 0..) |row, row_index| {
            for (row) |value| {
                flat_values[cursor] = value.handle orelse return error.InvalidArgument;
                cursor += 1;
            }
            row_offsets[row_index + 1] = cursor;
        }

        const values_ptr: ?[*]const *const c.ifcopenshell_ifcparse_entity_ref_t = if (flat_values.len == 0) null else flat_values.ptr;
        if (c.ifcopenshell_ifcparse_entity_set_argument_entity_matrix(
            self.handle.?,
            index,
            @ptrCast(values_ptr),
            flat_values.len,
            row_offsets.ptr,
            values.len,
        ) == 0) {
            return error.QueryFailed;
        }
    }

    pub fn getArgument(self: EntityRef, index: usize) QueryError!ArgumentValue {
        if (self.handle == null) return error.InvalidArgument;

        return switch (self.argumentValueType(index)) {
            .null => .{ .null = {} },
            .derived => .{ .derived = {} },
            .int => .{ .int = try self.getInt(index) },
            .bool => .{ .bool = try self.getBool(index) },
            .logical => .{ .logical = try self.getLogical(index) },
            .double => .{ .double = try self.getDouble(index) },
            .string => .{ .string = try self.getString(index) },
            .binary => .{ .binary = try self.getString(index) },
            .enumeration => .{ .enumeration = try self.getString(index) },
            .entity_instance => .{ .entity_instance = try self.getEntity(index) },
            .empty_aggregate => .{ .empty_aggregate = {} },
            .aggregate_of_int => .{ .aggregate_of_int = try self.getIntList(index) },
            .aggregate_of_double => .{ .aggregate_of_double = try self.getDoubleList(index) },
            .aggregate_of_string => .{ .aggregate_of_string = try self.getStringList(index) },
            .aggregate_of_binary => .{ .aggregate_of_binary = try self.getStringList(index) },
            .aggregate_of_entity_instance => .{ .aggregate_of_entity_instance = try self.getEntityList(index) },
            .aggregate_of_empty_aggregate => .{ .aggregate_of_empty_aggregate = {} },
            .aggregate_of_aggregate_of_int => .{ .aggregate_of_aggregate_of_int = try self.getIntMatrix(index) },
            .aggregate_of_aggregate_of_double => .{ .aggregate_of_aggregate_of_double = try self.getDoubleMatrix(index) },
            .aggregate_of_aggregate_of_entity_instance => .{ .aggregate_of_aggregate_of_entity_instance = try self.getEntityMatrix(index) },
            .unknown, _ => .{ .unknown = {} },
        };
    }

    pub fn getArgumentByName(
        self: EntityRef,
        allocator: std.mem.Allocator,
        argument_name: []const u8,
    ) QueryError!ArgumentValue {
        return self.getArgument(try self.argumentIndex(allocator, argument_name));
    }
};

pub const EntityList = struct {
    handle: ?*c.ifcopenshell_ifcparse_entity_list_t,

    pub fn deinit(self: *EntityList) void {
        if (self.handle) |h| {
            c.ifcopenshell_ifcparse_entity_list_close(h);
            self.handle = null;
        }
    }

    pub fn len(self: EntityList) usize {
        if (self.handle == null) return 0;
        return c.ifcopenshell_ifcparse_entity_list_count(self.handle.?);
    }

    pub fn reset(self: *EntityList) void {
        if (self.handle == null) return;
        c.ifcopenshell_ifcparse_entity_list_reset(self.handle.?);
    }

    pub fn at(self: EntityList, index: usize) ?EntityRef {
        if (self.handle == null) return null;
        const raw_entity = c.ifcopenshell_ifcparse_entity_list_get(self.handle.?, index);
        if (raw_entity == null) return null;
        return EntityRef{ .handle = raw_entity };
    }

    pub fn next(self: *EntityList) ?EntityRef {
        if (self.handle == null) return null;
        const raw_entity = c.ifcopenshell_ifcparse_entity_list_next(self.handle.?);
        if (raw_entity == null) return null;
        return EntityRef{ .handle = raw_entity };
    }

    pub fn iterator(self: *EntityList) Iterator {
        return Iterator{ .list = self };
    }

    pub fn toOwnedSlice(self: EntityList, allocator: std.mem.Allocator) QueryError![]EntityRef {
        const count = self.len();
        var entities = allocator.alloc(EntityRef, count) catch return error.OutOfMemory;
        errdefer allocator.free(entities);

        var index: usize = 0;
        while (index < count) : (index += 1) {
            entities[index] = self.at(index) orelse return error.QueryFailed;
        }

        return entities;
    }

    pub const Iterator = struct {
        list: *EntityList,

        pub fn next(self: *Iterator) ?EntityRef {
            return self.list.next();
        }
    };
};

pub const IntList = struct {
    handle: ?*c.ifcopenshell_ifcparse_int_list_t,

    pub fn deinit(self: *IntList) void {
        if (self.handle) |h| {
            c.ifcopenshell_ifcparse_int_list_close(h);
            self.handle = null;
        }
    }

    pub fn len(self: IntList) usize {
        if (self.handle == null) return 0;
        return c.ifcopenshell_ifcparse_int_list_count(self.handle.?);
    }

    pub fn reset(self: *IntList) void {
        if (self.handle == null) return;
        c.ifcopenshell_ifcparse_int_list_reset(self.handle.?);
    }

    pub fn at(self: IntList, index: usize) ?i32 {
        if (self.handle == null) return null;
        if (index >= self.len()) return null;
        return c.ifcopenshell_ifcparse_int_list_get(self.handle.?, index);
    }

    pub fn next(self: *IntList) ?i32 {
        if (self.handle == null) return null;
        const value = c.ifcopenshell_ifcparse_int_list_next(self.handle.?);
        if (value == 0 and lastError().len > 0) return null;
        return value;
    }

    pub fn iterator(self: *IntList) Iterator {
        return Iterator{
            .list = self,
            .index = 0,
            .count = self.len(),
        };
    }

    pub fn toOwnedSlice(self: IntList, allocator: std.mem.Allocator) QueryError![]i32 {
        const count = self.len();
        var values = allocator.alloc(i32, count) catch return error.OutOfMemory;
        errdefer allocator.free(values);

        var index: usize = 0;
        while (index < count) : (index += 1) {
            values[index] = self.at(index) orelse return error.QueryFailed;
        }

        return values;
    }

    pub const Iterator = struct {
        list: *IntList,
        index: usize,
        count: usize,

        pub fn next(self: *Iterator) ?i32 {
            if (self.index >= self.count) return null;
            defer self.index += 1;
            return self.list.at(self.index);
        }
    };
};

pub const DoubleList = struct {
    handle: ?*c.ifcopenshell_ifcparse_double_list_t,

    pub fn deinit(self: *DoubleList) void {
        if (self.handle) |h| {
            c.ifcopenshell_ifcparse_double_list_close(h);
            self.handle = null;
        }
    }

    pub fn len(self: DoubleList) usize {
        if (self.handle == null) return 0;
        return c.ifcopenshell_ifcparse_double_list_count(self.handle.?);
    }

    pub fn reset(self: *DoubleList) void {
        if (self.handle == null) return;
        c.ifcopenshell_ifcparse_double_list_reset(self.handle.?);
    }

    pub fn at(self: DoubleList, index: usize) ?f64 {
        if (self.handle == null) return null;
        if (index >= self.len()) return null;
        return c.ifcopenshell_ifcparse_double_list_get(self.handle.?, index);
    }

    pub fn next(self: *DoubleList) ?f64 {
        if (self.handle == null) return null;
        const value = c.ifcopenshell_ifcparse_double_list_next(self.handle.?);
        if (value == 0.0 and lastError().len > 0) return null;
        return value;
    }

    pub fn iterator(self: *DoubleList) Iterator {
        return Iterator{
            .list = self,
            .index = 0,
            .count = self.len(),
        };
    }

    pub fn toOwnedSlice(self: DoubleList, allocator: std.mem.Allocator) QueryError![]f64 {
        const count = self.len();
        var values = allocator.alloc(f64, count) catch return error.OutOfMemory;
        errdefer allocator.free(values);

        var index: usize = 0;
        while (index < count) : (index += 1) {
            values[index] = self.at(index) orelse return error.QueryFailed;
        }

        return values;
    }

    pub const Iterator = struct {
        list: *DoubleList,
        index: usize,
        count: usize,

        pub fn next(self: *Iterator) ?f64 {
            if (self.index >= self.count) return null;
            defer self.index += 1;
            return self.list.at(self.index);
        }
    };
};

pub const IntMatrix = struct {
    handle: ?*c.ifcopenshell_ifcparse_int_matrix_t,

    pub fn deinit(self: *IntMatrix) void {
        if (self.handle) |h| {
            c.ifcopenshell_ifcparse_int_matrix_close(h);
            self.handle = null;
        }
    }

    pub fn rowCount(self: IntMatrix) usize {
        if (self.handle == null) return 0;
        return c.ifcopenshell_ifcparse_int_matrix_row_count(self.handle.?);
    }

    pub fn colCount(self: IntMatrix, row: usize) ?usize {
        if (self.handle == null) return null;
        const count = c.ifcopenshell_ifcparse_int_matrix_col_count(self.handle.?, row);
        if (count == 0 and lastError().len > 0) return null;
        return count;
    }

    pub fn at(self: IntMatrix, row: usize, col: usize) ?i32 {
        if (self.handle == null) return null;
        const value = c.ifcopenshell_ifcparse_int_matrix_get(self.handle.?, row, col);
        if (value == 0 and lastError().len > 0) return null;
        return value;
    }
};

pub const DoubleMatrix = struct {
    handle: ?*c.ifcopenshell_ifcparse_double_matrix_t,

    pub fn deinit(self: *DoubleMatrix) void {
        if (self.handle) |h| {
            c.ifcopenshell_ifcparse_double_matrix_close(h);
            self.handle = null;
        }
    }

    pub fn rowCount(self: DoubleMatrix) usize {
        if (self.handle == null) return 0;
        return c.ifcopenshell_ifcparse_double_matrix_row_count(self.handle.?);
    }

    pub fn colCount(self: DoubleMatrix, row: usize) ?usize {
        if (self.handle == null) return null;
        const count = c.ifcopenshell_ifcparse_double_matrix_col_count(self.handle.?, row);
        if (count == 0 and lastError().len > 0) return null;
        return count;
    }

    pub fn at(self: DoubleMatrix, row: usize, col: usize) ?f64 {
        if (self.handle == null) return null;
        const value = c.ifcopenshell_ifcparse_double_matrix_get(self.handle.?, row, col);
        if (value == 0.0 and lastError().len > 0) return null;
        return value;
    }
};

pub const EntityMatrix = struct {
    handle: ?*c.ifcopenshell_ifcparse_entity_matrix_t,

    pub fn deinit(self: *EntityMatrix) void {
        if (self.handle) |h| {
            c.ifcopenshell_ifcparse_entity_matrix_close(h);
            self.handle = null;
        }
    }

    pub fn rowCount(self: EntityMatrix) usize {
        if (self.handle == null) return 0;
        return c.ifcopenshell_ifcparse_entity_matrix_row_count(self.handle.?);
    }

    pub fn colCount(self: EntityMatrix, row: usize) ?usize {
        if (self.handle == null) return null;
        const count = c.ifcopenshell_ifcparse_entity_matrix_col_count(self.handle.?, row);
        if (count == 0 and lastError().len > 0) return null;
        return count;
    }

    pub fn at(self: EntityMatrix, row: usize, col: usize) ?EntityRef {
        if (self.handle == null) return null;
        const raw_entity = c.ifcopenshell_ifcparse_entity_matrix_get(self.handle.?, row, col);
        if (raw_entity == null) return null;
        return EntityRef{ .handle = raw_entity };
    }
};

pub const ArgumentValue = union(ArgumentType) {
    null: void,
    derived: void,
    int: i32,
    bool: bool,
    logical: i32,
    double: f64,
    string: []const u8,
    binary: []const u8,
    enumeration: []const u8,
    entity_instance: ?EntityRef,
    empty_aggregate: void,
    aggregate_of_int: IntList,
    aggregate_of_double: DoubleList,
    aggregate_of_string: StringList,
    aggregate_of_binary: StringList,
    aggregate_of_entity_instance: EntityList,
    aggregate_of_empty_aggregate: void,
    aggregate_of_aggregate_of_int: IntMatrix,
    aggregate_of_aggregate_of_double: DoubleMatrix,
    aggregate_of_aggregate_of_entity_instance: EntityMatrix,
    unknown: void,

    pub fn deinit(self: *ArgumentValue) void {
        switch (self.*) {
            .aggregate_of_int => |*list| list.deinit(),
            .aggregate_of_double => |*list| list.deinit(),
            .aggregate_of_string => |*list| list.deinit(),
            .aggregate_of_binary => |*list| list.deinit(),
            .aggregate_of_entity_instance => |*list| list.deinit(),
            .aggregate_of_aggregate_of_int => |*matrix| matrix.deinit(),
            .aggregate_of_aggregate_of_double => |*matrix| matrix.deinit(),
            .aggregate_of_aggregate_of_entity_instance => |*matrix| matrix.deinit(),
            else => {},
        }
    }
};

pub const TypeRef = struct {
    handle: ?*const c.ifcopenshell_ifcparse_type_ref_t,

    pub fn name(self: TypeRef) ?[]const u8 {
        if (self.handle == null) return null;
        const raw_name = c.ifcopenshell_ifcparse_type_name(self.handle.?);
        if (raw_name == null) return null;
        return std.mem.span(raw_name);
    }

    pub fn indexInSchema(self: TypeRef) i32 {
        if (self.handle == null) return -1;
        return c.ifcopenshell_ifcparse_type_index(self.handle.?);
    }

    pub fn schemaName(self: TypeRef) ?[]const u8 {
        if (self.handle == null) return null;
        const raw_name = c.ifcopenshell_ifcparse_type_schema_name(self.handle.?);
        if (raw_name == null) return null;
        return std.mem.span(raw_name);
    }

    pub fn kind(self: TypeRef) DeclarationKind {
        if (self.handle == null) return .unknown;
        return @enumFromInt(c.ifcopenshell_ifcparse_type_kind(self.handle.?));
    }

    pub fn isA(
        self: TypeRef,
        allocator: std.mem.Allocator,
        type_name: []const u8,
    ) QueryError!bool {
        if (self.handle == null) return error.InvalidArgument;
        if (type_name.len == 0) return error.InvalidArgument;

        const type_name_z = allocator.dupeZ(u8, type_name) catch return error.OutOfMemory;
        defer allocator.free(type_name_z);

        const result = c.ifcopenshell_ifcparse_type_is_a(self.handle.?, type_name_z.ptr);
        if (result == 0 and lastError().len > 0) return error.QueryFailed;
        return result != 0;
    }

    pub fn isAbstract(self: TypeRef) QueryError!bool {
        if (self.handle == null) return error.InvalidArgument;
        const result = c.ifcopenshell_ifcparse_type_is_abstract(self.handle.?);
        if (result == 0 and lastError().len > 0) return error.QueryFailed;
        return result != 0;
    }

    pub fn supertype(self: TypeRef) QueryError!?TypeRef {
        if (self.handle == null) return error.InvalidArgument;
        const raw_type = c.ifcopenshell_ifcparse_type_supertype(self.handle.?);
        if (raw_type == null) {
            if (lastError().len > 0) return error.QueryFailed;
            return null;
        }
        return TypeRef{ .handle = raw_type };
    }

    pub fn subtypes(self: TypeRef) QueryError!TypeList {
        if (self.handle == null) return error.InvalidArgument;
        const raw_list = c.ifcopenshell_ifcparse_type_subtypes(self.handle.?);
        if (raw_list == null) return error.QueryFailed;
        return TypeList{ .handle = raw_list };
    }

    pub fn declaredType(self: TypeRef) QueryError!?TypeRef {
        if (self.handle == null) return error.InvalidArgument;
        const raw_type = c.ifcopenshell_ifcparse_type_declared_type(self.handle.?);
        if (raw_type == null) {
            if (lastError().len > 0) return error.QueryFailed;
            return null;
        }
        return TypeRef{ .handle = raw_type };
    }

    pub fn selectList(self: TypeRef) QueryError!TypeList {
        if (self.handle == null) return error.InvalidArgument;
        const raw_list = c.ifcopenshell_ifcparse_type_select_list(self.handle.?);
        if (raw_list == null) return error.QueryFailed;
        return TypeList{ .handle = raw_list };
    }

    pub fn enumerationItems(self: TypeRef) QueryError!StringList {
        if (self.handle == null) return error.InvalidArgument;
        const raw_list = c.ifcopenshell_ifcparse_type_enumeration_items(self.handle.?);
        if (raw_list == null) return error.QueryFailed;
        return StringList{ .handle = raw_list };
    }

    pub fn attributeIndex(
        self: TypeRef,
        allocator: std.mem.Allocator,
        attribute_name: []const u8,
    ) QueryError!usize {
        if (self.handle == null) return error.InvalidArgument;
        if (attribute_name.len == 0) return error.InvalidArgument;

        const attribute_name_z = allocator.dupeZ(u8, attribute_name) catch return error.OutOfMemory;
        defer allocator.free(attribute_name_z);

        const index = c.ifcopenshell_ifcparse_type_attribute_index(self.handle.?, attribute_name_z.ptr);
        if (index < 0) return error.QueryFailed;
        return @intCast(index);
    }

    pub fn attributes(self: TypeRef, include_inherited: bool) QueryError!AttributeList {
        if (self.handle == null) return error.InvalidArgument;
        const raw_list = c.ifcopenshell_ifcparse_type_attributes(self.handle.?, if (include_inherited) 1 else 0);
        if (raw_list == null) return error.QueryFailed;
        return AttributeList{ .handle = raw_list };
    }

    pub fn inverseAttributes(self: TypeRef, include_inherited: bool) QueryError!InverseAttributeList {
        if (self.handle == null) return error.InvalidArgument;
        const raw_list = c.ifcopenshell_ifcparse_type_inverse_attributes(self.handle.?, if (include_inherited) 1 else 0);
        if (raw_list == null) return error.QueryFailed;
        return InverseAttributeList{ .handle = raw_list };
    }
};

pub const ParameterTypeRef = struct {
    handle: ?*const c.ifcopenshell_ifcparse_parameter_type_ref_t,

    pub fn kind(self: ParameterTypeRef) ParameterTypeKind {
        if (self.handle == null) return .unknown;
        return @enumFromInt(c.ifcopenshell_ifcparse_parameter_type_kind(self.handle.?));
    }

    pub fn namedDeclaredType(self: ParameterTypeRef) QueryError!?TypeRef {
        if (self.handle == null) return error.InvalidArgument;
        const raw_type = c.ifcopenshell_ifcparse_parameter_type_named_declared_type(self.handle.?);
        if (raw_type == null) {
            if (lastError().len > 0) return error.QueryFailed;
            return null;
        }
        return TypeRef{ .handle = raw_type };
    }

    pub fn simpleDeclaredType(self: ParameterTypeRef) QueryError!SimpleType {
        if (self.handle == null) return error.InvalidArgument;
        const value = c.ifcopenshell_ifcparse_parameter_type_simple_declared_type(self.handle.?);
        const result: SimpleType = @enumFromInt(value);
        if (result == .invalid and lastError().len > 0) return error.QueryFailed;
        return result;
    }

    pub fn aggregationType(self: ParameterTypeRef) QueryError!AggregationType {
        if (self.handle == null) return error.InvalidArgument;
        const value = c.ifcopenshell_ifcparse_parameter_type_aggregation_type(self.handle.?);
        const result: AggregationType = @enumFromInt(value);
        if (result == .invalid and lastError().len > 0) return error.QueryFailed;
        return result;
    }

    pub fn aggregationBound1(self: ParameterTypeRef) QueryError!i32 {
        if (self.handle == null) return error.InvalidArgument;
        const value = c.ifcopenshell_ifcparse_parameter_type_aggregation_bound1(self.handle.?);
        if (value == 0 and lastError().len > 0) return error.QueryFailed;
        return value;
    }

    pub fn aggregationBound2(self: ParameterTypeRef) QueryError!i32 {
        if (self.handle == null) return error.InvalidArgument;
        const value = c.ifcopenshell_ifcparse_parameter_type_aggregation_bound2(self.handle.?);
        if (value == 0 and lastError().len > 0) return error.QueryFailed;
        return value;
    }

    pub fn aggregationElementType(self: ParameterTypeRef) QueryError!?ParameterTypeRef {
        if (self.handle == null) return error.InvalidArgument;
        const raw_type = c.ifcopenshell_ifcparse_parameter_type_aggregation_element_type(self.handle.?);
        if (raw_type == null) {
            if (lastError().len > 0) return error.QueryFailed;
            return null;
        }
        return ParameterTypeRef{ .handle = raw_type };
    }
};

pub const AttributeRef = struct {
    handle: ?*const c.ifcopenshell_ifcparse_attribute_ref_t,

    pub fn name(self: AttributeRef) ?[]const u8 {
        if (self.handle == null) return null;
        const raw_name = c.ifcopenshell_ifcparse_attribute_name(self.handle.?);
        if (raw_name == null) return null;
        return std.mem.span(raw_name);
    }

    pub fn optional(self: AttributeRef) QueryError!bool {
        if (self.handle == null) return error.InvalidArgument;
        const value = c.ifcopenshell_ifcparse_attribute_optional(self.handle.?);
        if (value == 0 and lastError().len > 0) return error.QueryFailed;
        return value != 0;
    }

    pub fn parameterType(self: AttributeRef) QueryError!?ParameterTypeRef {
        if (self.handle == null) return error.InvalidArgument;
        const raw_type = c.ifcopenshell_ifcparse_attribute_parameter_type(self.handle.?);
        if (raw_type == null) {
            if (lastError().len > 0) return error.QueryFailed;
            return null;
        }
        return ParameterTypeRef{ .handle = raw_type };
    }
};

pub const InverseAttributeRef = struct {
    handle: ?*const c.ifcopenshell_ifcparse_inverse_attribute_ref_t,

    pub fn name(self: InverseAttributeRef) ?[]const u8 {
        if (self.handle == null) return null;
        const raw_name = c.ifcopenshell_ifcparse_inverse_attribute_name(self.handle.?);
        if (raw_name == null) return null;
        return std.mem.span(raw_name);
    }

    pub fn aggregationType(self: InverseAttributeRef) QueryError!InverseAggregationType {
        if (self.handle == null) return error.InvalidArgument;
        const value = c.ifcopenshell_ifcparse_inverse_attribute_aggregation_type(self.handle.?);
        const result: InverseAggregationType = @enumFromInt(value);
        if (result == .invalid and lastError().len > 0) return error.QueryFailed;
        return result;
    }

    pub fn bound1(self: InverseAttributeRef) QueryError!i32 {
        if (self.handle == null) return error.InvalidArgument;
        const value = c.ifcopenshell_ifcparse_inverse_attribute_bound1(self.handle.?);
        if (value == 0 and lastError().len > 0) return error.QueryFailed;
        return value;
    }

    pub fn bound2(self: InverseAttributeRef) QueryError!i32 {
        if (self.handle == null) return error.InvalidArgument;
        const value = c.ifcopenshell_ifcparse_inverse_attribute_bound2(self.handle.?);
        if (value == 0 and lastError().len > 0) return error.QueryFailed;
        return value;
    }

    pub fn entityReference(self: InverseAttributeRef) QueryError!?TypeRef {
        if (self.handle == null) return error.InvalidArgument;
        const raw_type = c.ifcopenshell_ifcparse_inverse_attribute_entity_reference(self.handle.?);
        if (raw_type == null) {
            if (lastError().len > 0) return error.QueryFailed;
            return null;
        }
        return TypeRef{ .handle = raw_type };
    }

    pub fn attributeReference(self: InverseAttributeRef) QueryError!?AttributeRef {
        if (self.handle == null) return error.InvalidArgument;
        const raw_attribute = c.ifcopenshell_ifcparse_inverse_attribute_attribute_reference(self.handle.?);
        if (raw_attribute == null) {
            if (lastError().len > 0) return error.QueryFailed;
            return null;
        }
        return AttributeRef{ .handle = raw_attribute };
    }
};

pub const AttributeList = struct {
    handle: ?*c.ifcopenshell_ifcparse_attribute_list_t,

    pub fn deinit(self: *AttributeList) void {
        if (self.handle) |h| {
            c.ifcopenshell_ifcparse_attribute_list_close(h);
            self.handle = null;
        }
    }

    pub fn len(self: AttributeList) usize {
        if (self.handle == null) return 0;
        return c.ifcopenshell_ifcparse_attribute_list_count(self.handle.?);
    }

    pub fn reset(self: *AttributeList) void {
        if (self.handle == null) return;
        c.ifcopenshell_ifcparse_attribute_list_reset(self.handle.?);
    }

    pub fn at(self: AttributeList, index: usize) ?AttributeRef {
        if (self.handle == null) return null;
        const raw_attribute = c.ifcopenshell_ifcparse_attribute_list_get(self.handle.?, index);
        if (raw_attribute == null) return null;
        return AttributeRef{ .handle = raw_attribute };
    }

    pub fn next(self: *AttributeList) ?AttributeRef {
        if (self.handle == null) return null;
        const raw_attribute = c.ifcopenshell_ifcparse_attribute_list_next(self.handle.?);
        if (raw_attribute == null) return null;
        return AttributeRef{ .handle = raw_attribute };
    }

    pub fn iterator(self: *AttributeList) Iterator {
        return Iterator{ .list = self };
    }

    pub const Iterator = struct {
        list: *AttributeList,

        pub fn next(self: *Iterator) ?AttributeRef {
            return self.list.next();
        }
    };
};

pub const InverseAttributeList = struct {
    handle: ?*c.ifcopenshell_ifcparse_inverse_attribute_list_t,

    pub fn deinit(self: *InverseAttributeList) void {
        if (self.handle) |h| {
            c.ifcopenshell_ifcparse_inverse_attribute_list_close(h);
            self.handle = null;
        }
    }

    pub fn len(self: InverseAttributeList) usize {
        if (self.handle == null) return 0;
        return c.ifcopenshell_ifcparse_inverse_attribute_list_count(self.handle.?);
    }

    pub fn reset(self: *InverseAttributeList) void {
        if (self.handle == null) return;
        c.ifcopenshell_ifcparse_inverse_attribute_list_reset(self.handle.?);
    }

    pub fn at(self: InverseAttributeList, index: usize) ?InverseAttributeRef {
        if (self.handle == null) return null;
        const raw_attribute = c.ifcopenshell_ifcparse_inverse_attribute_list_get(self.handle.?, index);
        if (raw_attribute == null) return null;
        return InverseAttributeRef{ .handle = raw_attribute };
    }

    pub fn next(self: *InverseAttributeList) ?InverseAttributeRef {
        if (self.handle == null) return null;
        const raw_attribute = c.ifcopenshell_ifcparse_inverse_attribute_list_next(self.handle.?);
        if (raw_attribute == null) return null;
        return InverseAttributeRef{ .handle = raw_attribute };
    }

    pub fn iterator(self: *InverseAttributeList) Iterator {
        return Iterator{ .list = self };
    }

    pub const Iterator = struct {
        list: *InverseAttributeList,

        pub fn next(self: *Iterator) ?InverseAttributeRef {
            return self.list.next();
        }
    };
};

pub const TypeList = struct {
    handle: ?*c.ifcopenshell_ifcparse_type_list_t,

    pub fn deinit(self: *TypeList) void {
        if (self.handle) |h| {
            c.ifcopenshell_ifcparse_type_list_close(h);
            self.handle = null;
        }
    }

    pub fn len(self: TypeList) usize {
        if (self.handle == null) return 0;
        return c.ifcopenshell_ifcparse_type_list_count(self.handle.?);
    }

    pub fn reset(self: *TypeList) void {
        if (self.handle == null) return;
        c.ifcopenshell_ifcparse_type_list_reset(self.handle.?);
    }

    pub fn at(self: TypeList, index: usize) ?TypeRef {
        if (self.handle == null) return null;
        const raw_type = c.ifcopenshell_ifcparse_type_list_get(self.handle.?, index);
        if (raw_type == null) return null;
        return TypeRef{ .handle = raw_type };
    }

    pub fn next(self: *TypeList) ?TypeRef {
        if (self.handle == null) return null;
        const raw_type = c.ifcopenshell_ifcparse_type_list_next(self.handle.?);
        if (raw_type == null) return null;
        return TypeRef{ .handle = raw_type };
    }

    pub fn iterator(self: *TypeList) Iterator {
        return Iterator{ .list = self };
    }

    pub const Iterator = struct {
        list: *TypeList,

        pub fn next(self: *Iterator) ?TypeRef {
            return self.list.next();
        }
    };
};

pub const StringList = struct {
    handle: ?*c.ifcopenshell_ifcparse_string_list_t,

    pub fn deinit(self: *StringList) void {
        if (self.handle) |h| {
            c.ifcopenshell_ifcparse_string_list_close(h);
            self.handle = null;
        }
    }

    pub fn len(self: StringList) usize {
        if (self.handle == null) return 0;
        return c.ifcopenshell_ifcparse_string_list_count(self.handle.?);
    }

    pub fn reset(self: *StringList) void {
        if (self.handle == null) return;
        c.ifcopenshell_ifcparse_string_list_reset(self.handle.?);
    }

    pub fn at(self: StringList, index: usize) ?[]const u8 {
        if (self.handle == null) return null;
        const raw_value = c.ifcopenshell_ifcparse_string_list_get(self.handle.?, index);
        if (raw_value == null) return null;
        return std.mem.span(raw_value);
    }

    pub fn next(self: *StringList) ?[]const u8 {
        if (self.handle == null) return null;
        const raw_value = c.ifcopenshell_ifcparse_string_list_next(self.handle.?);
        if (raw_value == null) return null;
        return std.mem.span(raw_value);
    }

    pub fn iterator(self: *StringList) Iterator {
        return Iterator{ .list = self };
    }

    pub const Iterator = struct {
        list: *StringList,

        pub fn next(self: *Iterator) ?[]const u8 {
            return self.list.next();
        }
    };
};

pub const File = struct {
    handle: ?*c.ifcopenshell_ifcparse_file_t,

    pub fn createEmpty(
        allocator: std.mem.Allocator,
        schema_name: []const u8,
    ) QueryError!File {
        if (schema_name.len == 0) return error.InvalidArgument;

        const schema_name_z = allocator.dupeZ(u8, schema_name) catch return error.OutOfMemory;
        defer allocator.free(schema_name_z);

        const raw_handle = c.ifcopenshell_ifcparse_file_create_empty(schema_name_z.ptr);
        if (raw_handle == null) return error.QueryFailed;
        return File{ .handle = raw_handle };
    }

    pub fn open(
        allocator: std.mem.Allocator,
        path: []const u8,
        options: OpenOptions,
    ) OpenError!File {
        if (path.len == 0) return error.InvalidArgument;

        const path_z = allocator.dupeZ(u8, path) catch return error.OpenFailed;
        defer allocator.free(path_z);

        const raw_file_type: c.ifcopenshell_ifcparse_filetype_t = @intFromEnum(options.file_type);
        const raw_handle = c.ifcopenshell_ifcparse_file_open(
            path_z.ptr,
            raw_file_type,
            if (options.readonly) 1 else 0,
        );
        if (raw_handle == null) return error.OpenFailed;

        var file = File{ .handle = raw_handle };
        const open_status = file.status();
        if (open_status != .success) {
            defer file.deinit();
            return mapStatusToError(open_status);
        }

        return file;
    }

    pub fn openFromMemory(data: []const u8) OpenError!File {
        if (data.len == 0) return error.InvalidArgument;

        const raw_handle = c.ifcopenshell_ifcparse_file_open_from_memory(
            @ptrCast(data.ptr),
            data.len,
        );
        if (raw_handle == null) return error.OpenFailed;

        var file = File{ .handle = raw_handle };
        const open_status = file.status();
        if (open_status != .success) {
            defer file.deinit();
            return mapStatusToError(open_status);
        }

        return file;
    }

    pub fn deinit(self: *File) void {
        if (self.handle) |h| {
            c.ifcopenshell_ifcparse_file_close(h);
            self.handle = null;
        }
    }

    pub fn status(self: File) OpenStatus {
        if (self.handle == null) return .unknown;
        const raw_status = c.ifcopenshell_ifcparse_file_status(self.handle.?);
        return @enumFromInt(raw_status);
    }

    pub fn statusMessage(self: File) []const u8 {
        const raw_status: c.ifcopenshell_ifcparse_status_t = @intFromEnum(self.status());
        return std.mem.span(c.ifcopenshell_ifcparse_file_status_message(raw_status));
    }

    pub fn schemaName(self: *File) ?[]const u8 {
        if (self.handle == null) return null;
        const raw_name = c.ifcopenshell_ifcparse_file_schema_name(self.handle.?);
        if (raw_name == null) return null;
        return std.mem.span(raw_name);
    }

    pub fn schemaIdentifier(self: *File) ?[]const u8 {
        return self.schemaName();
    }

    pub fn headerFileDescription(self: *File) QueryError!?EntityRef {
        if (self.handle == null) return error.QueryFailed;
        const raw_entity = c.ifcopenshell_ifcparse_file_header_file_description(self.handle.?);
        if (raw_entity == null) {
            if (self.lastError().len > 0) return error.QueryFailed;
            return null;
        }
        return EntityRef{ .handle = raw_entity };
    }

    pub fn headerFileName(self: *File) QueryError!?EntityRef {
        if (self.handle == null) return error.QueryFailed;
        const raw_entity = c.ifcopenshell_ifcparse_file_header_file_name(self.handle.?);
        if (raw_entity == null) {
            if (self.lastError().len > 0) return error.QueryFailed;
            return null;
        }
        return EntityRef{ .handle = raw_entity };
    }

    pub fn headerFileSchema(self: *File) QueryError!?EntityRef {
        if (self.handle == null) return error.QueryFailed;
        const raw_entity = c.ifcopenshell_ifcparse_file_header_file_schema(self.handle.?);
        if (raw_entity == null) {
            if (self.lastError().len > 0) return error.QueryFailed;
            return null;
        }
        return EntityRef{ .handle = raw_entity };
    }

    pub fn entityCount(self: File) usize {
        if (self.handle == null) return 0;
        return c.ifcopenshell_ifcparse_file_entity_count(self.handle.?);
    }

    pub fn entities(self: *File) QueryError!EntityList {
        if (self.handle == null) return error.QueryFailed;
        const raw_list = c.ifcopenshell_ifcparse_file_entities(self.handle.?);
        if (raw_list == null) return error.QueryFailed;
        return EntityList{ .handle = raw_list };
    }

    pub fn entityIds(self: *File) QueryError!IntList {
        if (self.handle == null) return error.QueryFailed;
        const raw_list = c.ifcopenshell_ifcparse_file_entity_ids(self.handle.?);
        if (raw_list == null) return error.QueryFailed;
        return IntList{ .handle = raw_list };
    }

    pub fn instanceById(self: *File, id: i32) ?EntityRef {
        if (self.handle == null) return null;
        const raw_entity = c.ifcopenshell_ifcparse_file_instance_by_id(self.handle.?, @intCast(id));
        if (raw_entity == null) return null;
        return EntityRef{ .handle = raw_entity };
    }

    pub fn instanceByGuid(
        self: *File,
        allocator: std.mem.Allocator,
        guid: []const u8,
    ) QueryError!?EntityRef {
        if (self.handle == null) return error.QueryFailed;
        if (guid.len == 0) return error.InvalidArgument;

        const guid_z = allocator.dupeZ(u8, guid) catch return error.OutOfMemory;
        defer allocator.free(guid_z);

        const raw_entity = c.ifcopenshell_ifcparse_file_instance_by_guid(self.handle.?, guid_z.ptr);
        if (raw_entity == null) {
            if (self.lastError().len > 0) return error.QueryFailed;
            return null;
        }
        return EntityRef{ .handle = raw_entity };
    }

    pub fn instancesByType(
        self: *File,
        allocator: std.mem.Allocator,
        type_name: []const u8,
        include_subtypes: bool,
    ) QueryError!EntityList {
        if (self.handle == null) return error.QueryFailed;
        if (type_name.len == 0) return error.InvalidArgument;

        const type_name_z = allocator.dupeZ(u8, type_name) catch return error.OutOfMemory;
        defer allocator.free(type_name_z);

        const raw_list = c.ifcopenshell_ifcparse_file_instances_by_type(
            self.handle.?,
            type_name_z.ptr,
            if (include_subtypes) 1 else 0,
        );
        if (raw_list == null) return error.QueryFailed;

        return EntityList{ .handle = raw_list };
    }

    pub fn instancesByReference(self: *File, id: i32) QueryError!EntityList {
        if (self.handle == null) return error.QueryFailed;
        const raw_list = c.ifcopenshell_ifcparse_file_instances_by_reference(self.handle.?, @intCast(id));
        if (raw_list == null) return error.QueryFailed;
        return EntityList{ .handle = raw_list };
    }

    pub fn getInverse(
        self: *File,
        allocator: std.mem.Allocator,
        instance_id: i32,
        type_name: ?[]const u8,
        attribute_index: i32,
    ) QueryError!EntityList {
        if (self.handle == null) return error.QueryFailed;
        if (type_name == null and attribute_index != -1) return error.InvalidArgument;

        var type_name_z: ?[:0]u8 = null;
        defer if (type_name_z) |buffer| allocator.free(buffer);

        const raw_type_name: ?[*:0]const u8 = if (type_name) |name| blk: {
            if (name.len == 0) return error.InvalidArgument;
            type_name_z = allocator.dupeZ(u8, name) catch return error.OutOfMemory;
            break :blk type_name_z.?.ptr;
        } else null;

        const raw_list = c.ifcopenshell_ifcparse_file_get_inverse(
            self.handle.?,
            @intCast(instance_id),
            raw_type_name,
            @intCast(attribute_index),
        );
        if (raw_list == null) return error.QueryFailed;
        return EntityList{ .handle = raw_list };
    }

    pub fn getTotalInverses(self: *File, instance_id: i32) usize {
        if (self.handle == null) return 0;
        return c.ifcopenshell_ifcparse_file_get_total_inverses(self.handle.?, @intCast(instance_id));
    }

    pub fn getInverseIndices(self: *File, instance_id: i32) QueryError!IntList {
        if (self.handle == null) return error.QueryFailed;
        const raw_list = c.ifcopenshell_ifcparse_file_get_inverse_indices(self.handle.?, @intCast(instance_id));
        if (raw_list == null) return error.QueryFailed;
        return IntList{ .handle = raw_list };
    }

    pub fn traverseById(
        self: *File,
        instance_id: i32,
        max_level: i32,
        breadth_first: bool,
    ) QueryError!EntityList {
        if (self.handle == null) return error.QueryFailed;
        const raw_list = c.ifcopenshell_ifcparse_file_traverse_by_id(
            self.handle.?,
            @intCast(instance_id),
            @intCast(max_level),
            if (breadth_first) 1 else 0,
        );
        if (raw_list == null) return error.QueryFailed;
        return EntityList{ .handle = raw_list };
    }

    pub fn traverse(
        self: *File,
        entity: EntityRef,
        max_level: i32,
        breadth_first: bool,
    ) QueryError!EntityList {
        if (self.handle == null or entity.handle == null) return error.InvalidArgument;
        const raw_list = c.ifcopenshell_ifcparse_file_traverse(
            self.handle.?,
            entity.handle.?,
            @intCast(max_level),
            if (breadth_first) 1 else 0,
        );
        if (raw_list == null) return error.QueryFailed;
        return EntityList{ .handle = raw_list };
    }

    pub fn types(self: *File) QueryError!TypeList {
        if (self.handle == null) return error.QueryFailed;
        const raw_list = c.ifcopenshell_ifcparse_file_types(self.handle.?);
        if (raw_list == null) return error.QueryFailed;
        return TypeList{ .handle = raw_list };
    }

    pub fn createEntityByType(
        self: *File,
        allocator: std.mem.Allocator,
        type_name: []const u8,
    ) QueryError!EntityRef {
        if (self.handle == null) return error.QueryFailed;
        if (type_name.len == 0) return error.InvalidArgument;

        const type_name_z = allocator.dupeZ(u8, type_name) catch return error.OutOfMemory;
        defer allocator.free(type_name_z);

        const raw_entity = c.ifcopenshell_ifcparse_file_create_entity_by_type(self.handle.?, type_name_z.ptr);
        if (raw_entity == null) return error.QueryFailed;
        return EntityRef{ .handle = raw_entity };
    }

    pub fn createEntityByTypeWithId(
        self: *File,
        allocator: std.mem.Allocator,
        type_name: []const u8,
        id: i32,
    ) QueryError!EntityRef {
        if (self.handle == null) return error.QueryFailed;
        if (type_name.len == 0) return error.InvalidArgument;

        const type_name_z = allocator.dupeZ(u8, type_name) catch return error.OutOfMemory;
        defer allocator.free(type_name_z);

        const raw_entity = c.ifcopenshell_ifcparse_file_create_entity_by_type_with_id(self.handle.?, type_name_z.ptr, @intCast(id));
        if (raw_entity == null) return error.QueryFailed;
        return EntityRef{ .handle = raw_entity };
    }

    pub fn addEntity(self: *File, entity: EntityRef, id: ?i32) QueryError!EntityRef {
        if (self.handle == null or entity.handle == null) return error.InvalidArgument;
        const raw_entity = c.ifcopenshell_ifcparse_file_add_entity(
            self.handle.?,
            entity.handle.?,
            if (id) |resolved_id| @intCast(resolved_id) else -1,
        );
        if (raw_entity == null) return error.QueryFailed;
        return EntityRef{ .handle = raw_entity };
    }

    pub fn addEntities(
        self: *File,
        allocator: std.mem.Allocator,
        entity_values: []const EntityRef,
    ) QueryError!void {
        if (self.handle == null) return error.InvalidArgument;

        const ptr_values = allocator.alloc(*const c.ifcopenshell_ifcparse_entity_ref_t, entity_values.len) catch return error.OutOfMemory;
        defer allocator.free(ptr_values);

        for (entity_values, 0..) |entity, i| {
            ptr_values[i] = entity.handle orelse return error.InvalidArgument;
        }

        const ptr: ?[*]const *const c.ifcopenshell_ifcparse_entity_ref_t = if (ptr_values.len == 0) null else ptr_values.ptr;
        const added_count = c.ifcopenshell_ifcparse_file_add_entities(
            self.handle.?,
            @ptrCast(ptr),
            entity_values.len,
        );
        if (added_count != entity_values.len) return error.QueryFailed;
    }

    pub fn removeEntityById(self: *File, id: i32) QueryError!void {
        if (self.handle == null) return error.QueryFailed;
        const removed = c.ifcopenshell_ifcparse_file_remove_entity_by_id(self.handle.?, @intCast(id));
        if (removed == 0) return error.QueryFailed;
    }

    pub fn removeEntity(self: *File, entity: EntityRef) QueryError!void {
        if (self.handle == null or entity.handle == null) return error.InvalidArgument;
        const removed = c.ifcopenshell_ifcparse_file_remove_entity(self.handle.?, entity.handle.?);
        if (removed == 0) return error.QueryFailed;
    }

    pub fn batchBegin(self: *File) QueryError!void {
        if (self.handle == null) return error.QueryFailed;
        c.ifcopenshell_ifcparse_file_batch_begin(self.handle.?);
        if (self.lastError().len > 0) return error.QueryFailed;
    }

    pub fn batchEnd(self: *File) QueryError!void {
        if (self.handle == null) return error.QueryFailed;
        c.ifcopenshell_ifcparse_file_batch_end(self.handle.?);
        if (self.lastError().len > 0) return error.QueryFailed;
    }

    pub fn maxId(self: File) u32 {
        if (self.handle == null) return 0;
        return c.ifcopenshell_ifcparse_file_max_id(self.handle.?);
    }

    pub fn recalculateIdCounter(self: *File) QueryError!void {
        if (self.handle == null) return error.QueryFailed;
        c.ifcopenshell_ifcparse_file_recalculate_id_counter(self.handle.?);
        if (self.lastError().len > 0) return error.QueryFailed;
    }

    pub fn writeSpf(
        self: *File,
        allocator: std.mem.Allocator,
        path: []const u8,
    ) QueryError!void {
        if (self.handle == null) return error.QueryFailed;
        if (path.len == 0) return error.InvalidArgument;

        const path_z = allocator.dupeZ(u8, path) catch return error.OutOfMemory;
        defer allocator.free(path_z);

        const ok = c.ifcopenshell_ifcparse_file_write_spf(self.handle.?, path_z.ptr);
        if (ok == 0) return error.QueryFailed;
    }

    pub fn toSpf(self: *File) QueryError![]const u8 {
        if (self.handle == null) return error.QueryFailed;
        const raw_spf = c.ifcopenshell_ifcparse_file_to_spf(self.handle.?);
        if (raw_spf == null) return error.QueryFailed;
        return std.mem.span(raw_spf);
    }

    pub fn unitScale(
        self: *File,
        allocator: std.mem.Allocator,
        unit_type: []const u8,
    ) QueryError!f64 {
        if (self.handle == null) return error.QueryFailed;
        if (unit_type.len == 0) return error.InvalidArgument;

        const unit_type_z = allocator.dupeZ(u8, unit_type) catch return error.OutOfMemory;
        defer allocator.free(unit_type_z);

        const unit_scale = c.ifcopenshell_ifcparse_file_unit_scale(self.handle.?, unit_type_z.ptr);
        if (self.lastError().len > 0) return error.QueryFailed;
        return unit_scale;
    }

    pub fn unitEntity(
        self: *File,
        allocator: std.mem.Allocator,
        unit_type: []const u8,
    ) QueryError!?EntityRef {
        if (self.handle == null) return error.QueryFailed;
        if (unit_type.len == 0) return error.InvalidArgument;

        const unit_type_z = allocator.dupeZ(u8, unit_type) catch return error.OutOfMemory;
        defer allocator.free(unit_type_z);

        const raw_entity = c.ifcopenshell_ifcparse_file_unit_entity(self.handle.?, unit_type_z.ptr);
        if (raw_entity == null) return null;
        return EntityRef{ .handle = raw_entity };
    }

    pub fn lastError(self: File) []const u8 {
        if (self.handle == null) return std.mem.span(c.ifcopenshell_ifcparse_last_error());
        return std.mem.span(c.ifcopenshell_ifcparse_file_last_error(self.handle.?));
    }
};

pub fn version() []const u8 {
    return std.mem.span(c.ifcopenshell_ifcparse_version());
}

pub fn guessFileType(
    allocator: std.mem.Allocator,
    path: []const u8,
) QueryError!FileType {
    if (path.len == 0) return error.InvalidArgument;

    const path_z = allocator.dupeZ(u8, path) catch return error.OutOfMemory;
    defer allocator.free(path_z);

    const raw_type = c.ifcopenshell_ifcparse_guess_file_type(path_z.ptr);
    if (lastError().len > 0) return error.QueryFailed;
    return @enumFromInt(raw_type);
}

pub fn createTimestamp() QueryError![]const u8 {
    const raw_value = c.ifcopenshell_ifcparse_create_timestamp();
    if (raw_value == null) return error.QueryFailed;
    return std.mem.span(raw_value);
}

pub fn lastError() []const u8 {
    return std.mem.span(c.ifcopenshell_ifcparse_last_error());
}

pub fn schemaNames() QueryError!StringList {
    const raw_list = c.ifcopenshell_ifcparse_schema_names();
    if (raw_list == null) return error.QueryFailed;
    return StringList{ .handle = raw_list };
}

pub fn schemaDeclarationByName(
    allocator: std.mem.Allocator,
    schema_name: []const u8,
    declaration_name: []const u8,
) QueryError!?TypeRef {
    if (schema_name.len == 0 or declaration_name.len == 0) return error.InvalidArgument;

    const schema_name_z = allocator.dupeZ(u8, schema_name) catch return error.OutOfMemory;
    defer allocator.free(schema_name_z);
    const declaration_name_z = allocator.dupeZ(u8, declaration_name) catch return error.OutOfMemory;
    defer allocator.free(declaration_name_z);

    const raw_type = c.ifcopenshell_ifcparse_schema_declaration_by_name(schema_name_z.ptr, declaration_name_z.ptr);
    if (raw_type == null) {
        if (lastError().len > 0) return error.QueryFailed;
        return null;
    }
    return TypeRef{ .handle = raw_type };
}

pub fn schemaEntities(
    allocator: std.mem.Allocator,
    schema_name: []const u8,
) QueryError!TypeList {
    if (schema_name.len == 0) return error.InvalidArgument;

    const schema_name_z = allocator.dupeZ(u8, schema_name) catch return error.OutOfMemory;
    defer allocator.free(schema_name_z);

    const raw_list = c.ifcopenshell_ifcparse_schema_entities(schema_name_z.ptr);
    if (raw_list == null) return error.QueryFailed;
    return TypeList{ .handle = raw_list };
}

fn mapStatusToError(status: OpenStatus) OpenError {
    return switch (status) {
        .read_error => error.ReadError,
        .no_header => error.NoHeader,
        .unsupported_schema => error.UnsupportedSchema,
        .invalid_syntax => error.InvalidSyntax,
        .success => unreachable,
        .unknown, _ => error.UnknownStatus,
    };
}
