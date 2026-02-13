const std = @import("std");
const ifcparse = @import("ifcparse");

const c = @cImport({
    @cInclude("ifcgeom/c_api.h");
});

pub const QueryError = error{
    InvalidArgument,
    OutOfMemory,
    QueryFailed,
};

pub const ElementKind = enum(c.ifcopenshell_ifcgeom_element_kind_t) {
    none = c.IFCOPENSHELL_IFCGEOM_ELEMENT_NONE,
    triangulation = c.IFCOPENSHELL_IFCGEOM_ELEMENT_TRIANGULATION,
    brep = c.IFCOPENSHELL_IFCGEOM_ELEMENT_BREP,
    serialized = c.IFCOPENSHELL_IFCGEOM_ELEMENT_SERIALIZED,
};

pub const FilterMode = enum {
    include,
    exclude,
};

pub const IteratorOptions = struct {
    geometry_library: []const u8 = "opencascade",
    num_threads: i32 = 1,
};

pub fn lastError() []const u8 {
    const raw = c.ifcopenshell_ifcgeom_last_error();
    if (raw == null) return "";
    return std.mem.span(raw);
}

fn mapBoolResult(ok: c_int) QueryError!void {
    if (ok != 0) return;
    if (lastError().len > 0) return error.QueryFailed;
    return error.QueryFailed;
}

fn toCStringArray(
    allocator: std.mem.Allocator,
    values: []const []const u8,
) QueryError!struct { z_strings: []const [:0]u8, ptrs: []const [*:0]const u8 } {
    var z_strings = allocator.alloc([:0]u8, values.len) catch return error.OutOfMemory;
    errdefer allocator.free(z_strings);

    for (values, 0..) |value, i| {
        z_strings[i] = allocator.dupeZ(u8, value) catch return error.OutOfMemory;
    }
    errdefer {
        for (z_strings) |value| allocator.free(value);
    }

    var ptrs = allocator.alloc([*:0]const u8, values.len) catch return error.OutOfMemory;
    errdefer allocator.free(ptrs);

    for (z_strings, 0..) |value, i| {
        ptrs[i] = value.ptr;
    }

    return .{ .z_strings = z_strings, .ptrs = ptrs };
}

pub const StringList = struct {
    handle: ?*c.ifcopenshell_ifcgeom_string_list_t,

    pub fn deinit(self: *StringList) void {
        if (self.handle) |h| {
            c.ifcopenshell_ifcgeom_string_list_destroy(h);
            self.handle = null;
        }
    }

    pub fn len(self: StringList) usize {
        if (self.handle == null) return 0;
        return c.ifcopenshell_ifcgeom_string_list_count(self.handle.?);
    }

    pub fn reset(self: *StringList) void {
        if (self.handle == null) return;
        c.ifcopenshell_ifcgeom_string_list_reset(self.handle.?);
    }

    pub fn at(self: StringList, index: usize) ?[]const u8 {
        if (self.handle == null) return null;
        const raw = c.ifcopenshell_ifcgeom_string_list_get(self.handle.?, index);
        if (raw == null) return null;
        return std.mem.span(raw);
    }

    pub fn next(self: *StringList) ?[]const u8 {
        if (self.handle == null) return null;
        const raw = c.ifcopenshell_ifcgeom_string_list_next(self.handle.?);
        if (raw == null) return null;
        return std.mem.span(raw);
    }

    pub fn iterator(self: *StringList) StringIterator {
        return .{ .list = self };
    }

    pub const StringIterator = struct {
        list: *StringList,

        pub fn next(self: *StringIterator) ?[]const u8 {
            return self.list.next();
        }
    };
};

pub const Settings = struct {
    handle: ?*c.ifcopenshell_ifcgeom_settings_t,

    pub fn init() QueryError!Settings {
        const raw = c.ifcopenshell_ifcgeom_settings_create();
        if (raw == null) return error.QueryFailed;
        return .{ .handle = raw };
    }

    pub fn deinit(self: *Settings) void {
        if (self.handle) |h| {
            c.ifcopenshell_ifcgeom_settings_destroy(h);
            self.handle = null;
        }
    }

    pub fn setBool(self: *Settings, name: []const u8, value: bool) QueryError!void {
        if (self.handle == null or name.len == 0) return error.InvalidArgument;

        const name_z = std.heap.page_allocator.dupeZ(u8, name) catch return error.OutOfMemory;
        defer std.heap.page_allocator.free(name_z);

        try mapBoolResult(c.ifcopenshell_ifcgeom_settings_set_bool(self.handle.?, name_z.ptr, if (value) 1 else 0));
    }

    pub fn setInt(self: *Settings, name: []const u8, value: i32) QueryError!void {
        if (self.handle == null or name.len == 0) return error.InvalidArgument;

        const name_z = std.heap.page_allocator.dupeZ(u8, name) catch return error.OutOfMemory;
        defer std.heap.page_allocator.free(name_z);

        try mapBoolResult(c.ifcopenshell_ifcgeom_settings_set_int(self.handle.?, name_z.ptr, @intCast(value)));
    }

    pub fn setDouble(self: *Settings, name: []const u8, value: f64) QueryError!void {
        if (self.handle == null or name.len == 0) return error.InvalidArgument;

        const name_z = std.heap.page_allocator.dupeZ(u8, name) catch return error.OutOfMemory;
        defer std.heap.page_allocator.free(name_z);

        try mapBoolResult(c.ifcopenshell_ifcgeom_settings_set_double(self.handle.?, name_z.ptr, value));
    }

    pub fn setString(
        self: *Settings,
        allocator: std.mem.Allocator,
        name: []const u8,
        value: []const u8,
    ) QueryError!void {
        if (self.handle == null or name.len == 0) return error.InvalidArgument;

        const name_z = allocator.dupeZ(u8, name) catch return error.OutOfMemory;
        defer allocator.free(name_z);

        const value_z = allocator.dupeZ(u8, value) catch return error.OutOfMemory;
        defer allocator.free(value_z);

        try mapBoolResult(c.ifcopenshell_ifcgeom_settings_set_string(self.handle.?, name_z.ptr, value_z.ptr));
    }

    pub fn setIntSet(self: *Settings, allocator: std.mem.Allocator, name: []const u8, values: []const i32) QueryError!void {
        if (self.handle == null or name.len == 0) return error.InvalidArgument;

        const name_z = allocator.dupeZ(u8, name) catch return error.OutOfMemory;
        defer allocator.free(name_z);

        const ptr: ?[*]const c_int = if (values.len == 0) null else @ptrCast(values.ptr);
        try mapBoolResult(c.ifcopenshell_ifcgeom_settings_set_int_set(self.handle.?, name_z.ptr, ptr, values.len));
    }

    pub fn setStringSet(
        self: *Settings,
        allocator: std.mem.Allocator,
        name: []const u8,
        values: []const []const u8,
    ) QueryError!void {
        if (self.handle == null or name.len == 0) return error.InvalidArgument;

        const name_z = allocator.dupeZ(u8, name) catch return error.OutOfMemory;
        defer allocator.free(name_z);

        const c_values = try toCStringArray(allocator, values);
        defer {
            allocator.free(c_values.ptrs);
            for (c_values.z_strings) |value| allocator.free(value);
            allocator.free(c_values.z_strings);
        }

        const ptr: ?[*]const [*:0]const u8 = if (c_values.ptrs.len == 0) null else c_values.ptrs.ptr;
        try mapBoolResult(c.ifcopenshell_ifcgeom_settings_set_string_set(self.handle.?, name_z.ptr, @ptrCast(ptr), values.len));
    }

    pub fn setDoubleVector(self: *Settings, allocator: std.mem.Allocator, name: []const u8, values: []const f64) QueryError!void {
        if (self.handle == null or name.len == 0) return error.InvalidArgument;

        const name_z = allocator.dupeZ(u8, name) catch return error.OutOfMemory;
        defer allocator.free(name_z);

        const ptr: ?[*]const f64 = if (values.len == 0) null else values.ptr;
        try mapBoolResult(c.ifcopenshell_ifcgeom_settings_set_double_vector(self.handle.?, name_z.ptr, ptr, values.len));
    }

    pub fn getBool(self: *Settings, allocator: std.mem.Allocator, name: []const u8) QueryError!bool {
        if (self.handle == null or name.len == 0) return error.InvalidArgument;
        const name_z = allocator.dupeZ(u8, name) catch return error.OutOfMemory;
        defer allocator.free(name_z);

        var out: c_int = 0;
        try mapBoolResult(c.ifcopenshell_ifcgeom_settings_get_bool(self.handle.?, name_z.ptr, &out));
        return out != 0;
    }

    pub fn getInt(self: *Settings, allocator: std.mem.Allocator, name: []const u8) QueryError!i32 {
        if (self.handle == null or name.len == 0) return error.InvalidArgument;
        const name_z = allocator.dupeZ(u8, name) catch return error.OutOfMemory;
        defer allocator.free(name_z);

        var out: c_int = 0;
        try mapBoolResult(c.ifcopenshell_ifcgeom_settings_get_int(self.handle.?, name_z.ptr, &out));
        return @intCast(out);
    }

    pub fn getDouble(self: *Settings, allocator: std.mem.Allocator, name: []const u8) QueryError!f64 {
        if (self.handle == null or name.len == 0) return error.InvalidArgument;
        const name_z = allocator.dupeZ(u8, name) catch return error.OutOfMemory;
        defer allocator.free(name_z);

        var out: f64 = 0.0;
        try mapBoolResult(c.ifcopenshell_ifcgeom_settings_get_double(self.handle.?, name_z.ptr, &out));
        return out;
    }

    pub fn getString(self: *Settings, allocator: std.mem.Allocator, name: []const u8) QueryError![]u8 {
        if (self.handle == null or name.len == 0) return error.InvalidArgument;
        const name_z = allocator.dupeZ(u8, name) catch return error.OutOfMemory;
        defer allocator.free(name_z);

        const raw = c.ifcopenshell_ifcgeom_settings_get_string(self.handle.?, name_z.ptr);
        if (raw == null) return error.QueryFailed;
        return allocator.dupe(u8, std.mem.span(raw)) catch error.OutOfMemory;
    }

    pub fn settingType(self: *Settings, allocator: std.mem.Allocator, name: []const u8) QueryError![]u8 {
        if (self.handle == null or name.len == 0) return error.InvalidArgument;
        const name_z = allocator.dupeZ(u8, name) catch return error.OutOfMemory;
        defer allocator.free(name_z);

        const raw = c.ifcopenshell_ifcgeom_settings_get_type(self.handle.?, name_z.ptr);
        if (raw == null) return error.QueryFailed;
        return allocator.dupe(u8, std.mem.span(raw)) catch error.OutOfMemory;
    }

    pub fn settingNames(self: *Settings) QueryError!StringList {
        if (self.handle == null) return error.InvalidArgument;
        const raw = c.ifcopenshell_ifcgeom_settings_setting_names(self.handle.?);
        if (raw == null) return error.QueryFailed;
        return .{ .handle = raw };
    }

    pub fn lastError(self: *Settings) []const u8 {
        if (self.handle == null) return "";
        const raw = c.ifcopenshell_ifcgeom_settings_last_error(self.handle.?);
        if (raw == null) return "";
        return std.mem.span(raw);
    }
};

pub const Iterator = struct {
    handle: ?*c.ifcopenshell_ifcgeom_iterator_t,

    pub fn init(file: *ifcparse.File, settings: *const Settings, allocator: std.mem.Allocator, options: IteratorOptions) QueryError!Iterator {
        if (file.handle == null or settings.handle == null) return error.InvalidArgument;
        const lib_z = allocator.dupeZ(u8, options.geometry_library) catch return error.OutOfMemory;
        defer allocator.free(lib_z);
        const file_handle: *const c.ifcopenshell_ifcparse_file_t = @ptrCast(file.handle.?);

        const raw = c.ifcopenshell_ifcgeom_iterator_create(
            file_handle,
            settings.handle.?,
            lib_z.ptr,
            @intCast(options.num_threads),
        );
        if (raw == null) return error.QueryFailed;
        return .{ .handle = raw };
    }

    pub fn initWithTypeFilter(
        file: *ifcparse.File,
        settings: *const Settings,
        allocator: std.mem.Allocator,
        options: IteratorOptions,
        values: []const []const u8,
        mode: FilterMode,
    ) QueryError!Iterator {
        if (file.handle == null or settings.handle == null) return error.InvalidArgument;

        const lib_z = allocator.dupeZ(u8, options.geometry_library) catch return error.OutOfMemory;
        defer allocator.free(lib_z);
        const file_handle: *const c.ifcopenshell_ifcparse_file_t = @ptrCast(file.handle.?);

        const c_values = try toCStringArray(allocator, values);
        defer {
            allocator.free(c_values.ptrs);
            for (c_values.z_strings) |value| allocator.free(value);
            allocator.free(c_values.z_strings);
        }

        const ptr: ?[*]const [*:0]const u8 = if (c_values.ptrs.len == 0) null else c_values.ptrs.ptr;
        const raw = c.ifcopenshell_ifcgeom_iterator_create_with_type_filter(
            file_handle,
            settings.handle.?,
            lib_z.ptr,
            @ptrCast(ptr),
            values.len,
            if (mode == .include) 1 else 0,
            @intCast(options.num_threads),
        );
        if (raw == null) return error.QueryFailed;
        return .{ .handle = raw };
    }

    pub fn initWithGlobalIdFilter(
        file: *ifcparse.File,
        settings: *const Settings,
        allocator: std.mem.Allocator,
        options: IteratorOptions,
        values: []const []const u8,
        mode: FilterMode,
    ) QueryError!Iterator {
        if (file.handle == null or settings.handle == null) return error.InvalidArgument;

        const lib_z = allocator.dupeZ(u8, options.geometry_library) catch return error.OutOfMemory;
        defer allocator.free(lib_z);
        const file_handle: *const c.ifcopenshell_ifcparse_file_t = @ptrCast(file.handle.?);

        const c_values = try toCStringArray(allocator, values);
        defer {
            allocator.free(c_values.ptrs);
            for (c_values.z_strings) |value| allocator.free(value);
            allocator.free(c_values.z_strings);
        }

        const ptr: ?[*]const [*:0]const u8 = if (c_values.ptrs.len == 0) null else c_values.ptrs.ptr;
        const raw = c.ifcopenshell_ifcgeom_iterator_create_with_globalid_filter(
            file_handle,
            settings.handle.?,
            lib_z.ptr,
            @ptrCast(ptr),
            values.len,
            if (mode == .include) 1 else 0,
            @intCast(options.num_threads),
        );
        if (raw == null) return error.QueryFailed;
        return .{ .handle = raw };
    }

    pub fn initWithIdFilter(
        file: *ifcparse.File,
        settings: *const Settings,
        allocator: std.mem.Allocator,
        options: IteratorOptions,
        ids: []const i32,
        mode: FilterMode,
    ) QueryError!Iterator {
        if (file.handle == null or settings.handle == null) return error.InvalidArgument;

        const lib_z = allocator.dupeZ(u8, options.geometry_library) catch return error.OutOfMemory;
        defer allocator.free(lib_z);
        const file_handle: *const c.ifcopenshell_ifcparse_file_t = @ptrCast(file.handle.?);

        const ptr: ?[*]const c_int = if (ids.len == 0) null else @ptrCast(ids.ptr);
        const raw = c.ifcopenshell_ifcgeom_iterator_create_with_id_filter(
            file_handle,
            settings.handle.?,
            lib_z.ptr,
            ptr,
            ids.len,
            if (mode == .include) 1 else 0,
            @intCast(options.num_threads),
        );
        if (raw == null) return error.QueryFailed;
        return .{ .handle = raw };
    }

    pub fn deinit(self: *Iterator) void {
        if (self.handle) |h| {
            c.ifcopenshell_ifcgeom_iterator_destroy(h);
            self.handle = null;
        }
    }

    pub fn initialize(self: *Iterator) QueryError!bool {
        if (self.handle == null) return error.InvalidArgument;
        const ok = c.ifcopenshell_ifcgeom_iterator_initialize(self.handle.?);
        if (ok == 0 and self.lastError().len > 0) return error.QueryFailed;
        return ok != 0;
    }

    pub fn next(self: *Iterator) QueryError!bool {
        if (self.handle == null) return error.InvalidArgument;
        const ok = c.ifcopenshell_ifcgeom_iterator_next(self.handle.?);
        if (ok == 0 and self.lastError().len > 0) return error.QueryFailed;
        return ok != 0;
    }

    pub fn progress(self: *const Iterator) i32 {
        if (self.handle == null) return -1;
        return c.ifcopenshell_ifcgeom_iterator_progress(self.handle.?);
    }

    pub fn hadError(self: *const Iterator) bool {
        if (self.handle == null) return false;
        return c.ifcopenshell_ifcgeom_iterator_had_error(self.handle.?) != 0;
    }

    pub fn unitName(self: *Iterator) QueryError![]const u8 {
        if (self.handle == null) return error.InvalidArgument;
        const raw = c.ifcopenshell_ifcgeom_iterator_unit_name(self.handle.?);
        if (raw == null) return error.QueryFailed;
        return std.mem.span(raw);
    }

    pub fn unitMagnitude(self: *Iterator) QueryError!f64 {
        if (self.handle == null) return error.InvalidArgument;
        const value = c.ifcopenshell_ifcgeom_iterator_unit_magnitude(self.handle.?);
        if (value == 0.0 and self.lastError().len > 0) return error.QueryFailed;
        return value;
    }

    pub fn computeBounds(self: *Iterator, with_geometry: bool) QueryError!void {
        if (self.handle == null) return error.InvalidArgument;
        c.ifcopenshell_ifcgeom_iterator_compute_bounds(self.handle.?, if (with_geometry) 1 else 0);
        if (self.lastError().len > 0) return error.QueryFailed;
    }

    pub fn boundsMin(self: *const Iterator) QueryError![3]f64 {
        if (self.handle == null) return error.InvalidArgument;
        var out: [3]f64 = .{ 0.0, 0.0, 0.0 };
        if (c.ifcopenshell_ifcgeom_iterator_bounds_min(self.handle.?, &out) == 0) return error.QueryFailed;
        return out;
    }

    pub fn boundsMax(self: *const Iterator) QueryError![3]f64 {
        if (self.handle == null) return error.InvalidArgument;
        var out: [3]f64 = .{ 0.0, 0.0, 0.0 };
        if (c.ifcopenshell_ifcgeom_iterator_bounds_max(self.handle.?, &out) == 0) return error.QueryFailed;
        return out;
    }

    pub fn currentId(self: *Iterator) QueryError!i32 {
        if (self.handle == null) return error.InvalidArgument;
        var out: c_int = 0;
        if (c.ifcopenshell_ifcgeom_iterator_current_id(self.handle.?, &out) == 0) return error.QueryFailed;
        return @intCast(out);
    }

    pub fn currentParentId(self: *Iterator) QueryError!i32 {
        if (self.handle == null) return error.InvalidArgument;
        var out: c_int = -1;
        if (c.ifcopenshell_ifcgeom_iterator_current_parent_id(self.handle.?, &out) == 0) return error.QueryFailed;
        return @intCast(out);
    }

    pub fn currentType(self: *Iterator) QueryError![]const u8 {
        if (self.handle == null) return error.InvalidArgument;
        const raw = c.ifcopenshell_ifcgeom_iterator_current_type(self.handle.?);
        if (raw == null) return error.QueryFailed;
        return std.mem.span(raw);
    }

    pub fn currentGuid(self: *Iterator) QueryError![]const u8 {
        if (self.handle == null) return error.InvalidArgument;
        const raw = c.ifcopenshell_ifcgeom_iterator_current_guid(self.handle.?);
        if (raw == null) return error.QueryFailed;
        return std.mem.span(raw);
    }

    pub fn currentTransform(self: *Iterator) QueryError![16]f64 {
        if (self.handle == null) return error.InvalidArgument;
        var out: [16]f64 = .{0.0} ** 16;
        if (c.ifcopenshell_ifcgeom_iterator_current_transform(self.handle.?, &out) == 0) return error.QueryFailed;
        return out;
    }

    pub fn currentKind(self: *Iterator) ElementKind {
        if (self.handle == null) return .none;
        return @enumFromInt(c.ifcopenshell_ifcgeom_iterator_current_kind(self.handle.?));
    }

    pub fn currentMesh(self: *Iterator, allocator: std.mem.Allocator) QueryError!Mesh {
        if (self.handle == null) return error.InvalidArgument;

        const raw_mesh = c.ifcopenshell_ifcgeom_iterator_get_mesh(self.handle.?);
        if (raw_mesh == null) return error.QueryFailed;
        defer c.ifcopenshell_ifcgeom_mesh_destroy(raw_mesh.?);

        return Mesh.fromRaw(allocator, raw_mesh.?);
    }

    pub fn log(self: *Iterator) QueryError![]const u8 {
        if (self.handle == null) return error.InvalidArgument;
        const raw = c.ifcopenshell_ifcgeom_iterator_log(self.handle.?);
        if (raw == null) return error.QueryFailed;
        return std.mem.span(raw);
    }

    pub fn lastError(self: *const Iterator) []const u8 {
        if (self.handle == null) return "";
        const raw = c.ifcopenshell_ifcgeom_iterator_last_error(self.handle.?);
        if (raw == null) return "";
        return std.mem.span(raw);
    }
};

fn copyFromRawSlice(
    comptime T: type,
    allocator: std.mem.Allocator,
    ptr: ?[*]const T,
    count: usize,
) QueryError![]T {
    if (count == 0) return allocator.alloc(T, 0) catch error.OutOfMemory;
    if (ptr == null) return error.QueryFailed;

    const src = ptr.?[0..count];
    const out = allocator.alloc(T, count) catch return error.OutOfMemory;
    @memcpy(out, src);
    return out;
}

fn copyIntFromRawSlice(
    allocator: std.mem.Allocator,
    ptr: ?[*]const c_int,
    count: usize,
) QueryError![]i32 {
    if (count == 0) return allocator.alloc(i32, 0) catch error.OutOfMemory;
    if (ptr == null) return error.QueryFailed;

    const out = allocator.alloc(i32, count) catch return error.OutOfMemory;
    for (0..count) |i| {
        out[i] = @intCast(ptr.?[i]);
    }
    return out;
}

fn dupRawOrEmpty(allocator: std.mem.Allocator, raw: ?[*:0]const u8) QueryError![]u8 {
    if (raw == null) return allocator.alloc(u8, 0) catch error.OutOfMemory;
    return allocator.dupe(u8, std.mem.span(raw.?)) catch error.OutOfMemory;
}

pub const Mesh = struct {
    id: i32,
    parent_id: i32,
    name: []u8,
    type_name: []u8,
    guid: []u8,
    context: []u8,
    unique_id: []u8,

    verts: []f64,
    faces: []i32,
    edges: []i32,
    normals: []f64,
    uvs: []f64,
    material_ids: []i32,
    item_ids: []i32,
    edges_item_ids: []i32,
    colors: []f64,
    transform: []f64,

    fn fromRaw(allocator: std.mem.Allocator, raw: *const c.ifcopenshell_ifcgeom_mesh_t) QueryError!Mesh {
        return .{
            .id = @intCast(c.ifcopenshell_ifcgeom_mesh_id(raw)),
            .parent_id = @intCast(c.ifcopenshell_ifcgeom_mesh_parent_id(raw)),
            .name = try dupRawOrEmpty(allocator, c.ifcopenshell_ifcgeom_mesh_name(raw)),
            .type_name = try dupRawOrEmpty(allocator, c.ifcopenshell_ifcgeom_mesh_type(raw)),
            .guid = try dupRawOrEmpty(allocator, c.ifcopenshell_ifcgeom_mesh_guid(raw)),
            .context = try dupRawOrEmpty(allocator, c.ifcopenshell_ifcgeom_mesh_context(raw)),
            .unique_id = try dupRawOrEmpty(allocator, c.ifcopenshell_ifcgeom_mesh_unique_id(raw)),
            .verts = try copyFromRawSlice(f64, allocator, c.ifcopenshell_ifcgeom_mesh_verts_data(raw), c.ifcopenshell_ifcgeom_mesh_verts_count(raw)),
            .faces = try copyIntFromRawSlice(allocator, c.ifcopenshell_ifcgeom_mesh_faces_data(raw), c.ifcopenshell_ifcgeom_mesh_faces_count(raw)),
            .edges = try copyIntFromRawSlice(allocator, c.ifcopenshell_ifcgeom_mesh_edges_data(raw), c.ifcopenshell_ifcgeom_mesh_edges_count(raw)),
            .normals = try copyFromRawSlice(f64, allocator, c.ifcopenshell_ifcgeom_mesh_normals_data(raw), c.ifcopenshell_ifcgeom_mesh_normals_count(raw)),
            .uvs = try copyFromRawSlice(f64, allocator, c.ifcopenshell_ifcgeom_mesh_uvs_data(raw), c.ifcopenshell_ifcgeom_mesh_uvs_count(raw)),
            .material_ids = try copyIntFromRawSlice(allocator, c.ifcopenshell_ifcgeom_mesh_material_ids_data(raw), c.ifcopenshell_ifcgeom_mesh_material_ids_count(raw)),
            .item_ids = try copyIntFromRawSlice(allocator, c.ifcopenshell_ifcgeom_mesh_item_ids_data(raw), c.ifcopenshell_ifcgeom_mesh_item_ids_count(raw)),
            .edges_item_ids = try copyIntFromRawSlice(allocator, c.ifcopenshell_ifcgeom_mesh_edges_item_ids_data(raw), c.ifcopenshell_ifcgeom_mesh_edges_item_ids_count(raw)),
            .colors = try copyFromRawSlice(f64, allocator, c.ifcopenshell_ifcgeom_mesh_colors_data(raw), c.ifcopenshell_ifcgeom_mesh_colors_count(raw)),
            .transform = try copyFromRawSlice(f64, allocator, c.ifcopenshell_ifcgeom_mesh_transform_data(raw), c.ifcopenshell_ifcgeom_mesh_transform_count(raw)),
        };
    }

    pub fn deinit(self: *Mesh, allocator: std.mem.Allocator) void {
        allocator.free(self.name);
        allocator.free(self.type_name);
        allocator.free(self.guid);
        allocator.free(self.context);
        allocator.free(self.unique_id);

        allocator.free(self.verts);
        allocator.free(self.faces);
        allocator.free(self.edges);
        allocator.free(self.normals);
        allocator.free(self.uvs);
        allocator.free(self.material_ids);
        allocator.free(self.item_ids);
        allocator.free(self.edges_item_ids);
        allocator.free(self.colors);
        allocator.free(self.transform);

        self.* = undefined;
    }
};
