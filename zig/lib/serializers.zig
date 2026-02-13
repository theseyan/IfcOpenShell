const std = @import("std");
const ifcparse = @import("ifcparse");
const ifcgeom = @import("ifcgeom");

const c = @cImport({
    @cInclude("serializers/c_api.h");
});

pub const QueryError = error{
    InvalidArgument,
    OutOfMemory,
    QueryFailed,
    Unsupported,
};

pub const ExportOptions = struct {
    geometry_library: []const u8 = "opencascade",
    num_threads: i32 = 1,
};

pub fn lastError() []const u8 {
    const raw = c.ifcopenshell_ifcserializers_last_error();
    if (raw == null) return "";
    return std.mem.span(raw);
}

fn mapBoolResult(ok: c_int) QueryError!void {
    if (ok != 0) return;
    const err_msg = lastError();
    if (std.mem.indexOf(u8, err_msg, "not enabled") != null) {
        return error.Unsupported;
    }
    return error.QueryFailed;
}

fn toZ(allocator: std.mem.Allocator, value: []const u8) QueryError![:0]u8 {
    if (value.len == 0) return error.InvalidArgument;
    return allocator.dupeZ(u8, value) catch error.OutOfMemory;
}

fn fileHandle(file: *ifcparse.File) QueryError!*const c.ifcopenshell_ifcparse_file_t {
    if (file.handle == null) return error.InvalidArgument;
    return @ptrCast(file.handle.?);
}

fn geomSettingsHandle(settings: ?*const ifcgeom.Settings) QueryError!?*const c.ifcopenshell_ifcgeom_settings_t {
    if (settings == null) return null;
    if (settings.?.handle == null) return error.InvalidArgument;
    return @ptrCast(settings.?.handle.?);
}

fn serializerSettingsHandle(settings: ?*const Settings) QueryError!?*const c.ifcopenshell_ifcserializers_settings_t {
    if (settings == null) return null;
    if (settings.?.handle == null) return error.InvalidArgument;
    return settings.?.handle.?;
}

pub fn hasGltf() bool {
    return c.ifcopenshell_ifcserializers_has_gltf() != 0;
}

pub fn hasJson() bool {
    return c.ifcopenshell_ifcserializers_has_json() != 0;
}

pub const StringList = struct {
    handle: ?*c.ifcopenshell_ifcserializers_string_list_t,

    pub fn deinit(self: *StringList) void {
        if (self.handle) |h| {
            c.ifcopenshell_ifcserializers_string_list_destroy(h);
            self.handle = null;
        }
    }

    pub fn len(self: StringList) usize {
        if (self.handle == null) return 0;
        return c.ifcopenshell_ifcserializers_string_list_count(self.handle.?);
    }

    pub fn reset(self: *StringList) void {
        if (self.handle == null) return;
        c.ifcopenshell_ifcserializers_string_list_reset(self.handle.?);
    }

    pub fn at(self: StringList, index: usize) ?[]const u8 {
        if (self.handle == null) return null;
        const raw = c.ifcopenshell_ifcserializers_string_list_get(self.handle.?, index);
        if (raw == null) return null;
        return std.mem.span(raw);
    }

    pub fn next(self: *StringList) ?[]const u8 {
        if (self.handle == null) return null;
        const raw = c.ifcopenshell_ifcserializers_string_list_next(self.handle.?);
        if (raw == null) return null;
        return std.mem.span(raw);
    }

    pub fn iterator(self: *StringList) Iterator {
        return .{ .list = self };
    }

    pub const Iterator = struct {
        list: *StringList,

        pub fn next(self: *Iterator) ?[]const u8 {
            return self.list.next();
        }
    };
};

pub const Settings = struct {
    handle: ?*c.ifcopenshell_ifcserializers_settings_t,

    pub fn init() QueryError!Settings {
        const raw = c.ifcopenshell_ifcserializers_settings_create();
        if (raw == null) return error.QueryFailed;
        return .{ .handle = raw };
    }

    pub fn deinit(self: *Settings) void {
        if (self.handle) |h| {
            c.ifcopenshell_ifcserializers_settings_destroy(h);
            self.handle = null;
        }
    }

    pub fn setBool(self: *Settings, allocator: std.mem.Allocator, name: []const u8, value: bool) QueryError!void {
        if (self.handle == null) return error.InvalidArgument;
        const name_z = try toZ(allocator, name);
        defer allocator.free(name_z);
        try mapBoolResult(c.ifcopenshell_ifcserializers_settings_set_bool(self.handle.?, name_z.ptr, if (value) 1 else 0));
    }

    pub fn setInt(self: *Settings, allocator: std.mem.Allocator, name: []const u8, value: i32) QueryError!void {
        if (self.handle == null) return error.InvalidArgument;
        const name_z = try toZ(allocator, name);
        defer allocator.free(name_z);
        try mapBoolResult(c.ifcopenshell_ifcserializers_settings_set_int(self.handle.?, name_z.ptr, @intCast(value)));
    }

    pub fn setDouble(self: *Settings, allocator: std.mem.Allocator, name: []const u8, value: f64) QueryError!void {
        if (self.handle == null) return error.InvalidArgument;
        const name_z = try toZ(allocator, name);
        defer allocator.free(name_z);
        try mapBoolResult(c.ifcopenshell_ifcserializers_settings_set_double(self.handle.?, name_z.ptr, value));
    }

    pub fn setString(
        self: *Settings,
        allocator: std.mem.Allocator,
        name: []const u8,
        value: []const u8,
    ) QueryError!void {
        if (self.handle == null) return error.InvalidArgument;
        const name_z = try toZ(allocator, name);
        defer allocator.free(name_z);
        const value_z = try toZ(allocator, value);
        defer allocator.free(value_z);

        try mapBoolResult(c.ifcopenshell_ifcserializers_settings_set_string(
            self.handle.?,
            name_z.ptr,
            value_z.ptr,
        ));
    }

    pub fn getBool(self: *Settings, allocator: std.mem.Allocator, name: []const u8) QueryError!bool {
        if (self.handle == null) return error.InvalidArgument;
        const name_z = try toZ(allocator, name);
        defer allocator.free(name_z);

        var value: c_int = 0;
        try mapBoolResult(c.ifcopenshell_ifcserializers_settings_get_bool(self.handle.?, name_z.ptr, &value));
        return value != 0;
    }

    pub fn getInt(self: *Settings, allocator: std.mem.Allocator, name: []const u8) QueryError!i32 {
        if (self.handle == null) return error.InvalidArgument;
        const name_z = try toZ(allocator, name);
        defer allocator.free(name_z);

        var value: c_int = 0;
        try mapBoolResult(c.ifcopenshell_ifcserializers_settings_get_int(self.handle.?, name_z.ptr, &value));
        return @intCast(value);
    }

    pub fn getDouble(self: *Settings, allocator: std.mem.Allocator, name: []const u8) QueryError!f64 {
        if (self.handle == null) return error.InvalidArgument;
        const name_z = try toZ(allocator, name);
        defer allocator.free(name_z);

        var value: f64 = 0.0;
        try mapBoolResult(c.ifcopenshell_ifcserializers_settings_get_double(self.handle.?, name_z.ptr, &value));
        return value;
    }

    pub fn getString(self: *Settings, allocator: std.mem.Allocator, name: []const u8) QueryError![]u8 {
        if (self.handle == null) return error.InvalidArgument;
        const name_z = try toZ(allocator, name);
        defer allocator.free(name_z);

        const raw = c.ifcopenshell_ifcserializers_settings_get_string(self.handle.?, name_z.ptr);
        if (raw == null) return error.QueryFailed;
        return allocator.dupe(u8, std.mem.span(raw.?)) catch error.OutOfMemory;
    }

    pub fn settingType(self: *Settings, allocator: std.mem.Allocator, name: []const u8) QueryError![]u8 {
        if (self.handle == null) return error.InvalidArgument;
        const name_z = try toZ(allocator, name);
        defer allocator.free(name_z);

        const raw = c.ifcopenshell_ifcserializers_settings_get_type(self.handle.?, name_z.ptr);
        if (raw == null) return error.QueryFailed;
        return allocator.dupe(u8, std.mem.span(raw.?)) catch error.OutOfMemory;
    }

    pub fn settingNames(self: *Settings) QueryError!StringList {
        if (self.handle == null) return error.InvalidArgument;
        const raw = c.ifcopenshell_ifcserializers_settings_setting_names(self.handle.?);
        if (raw == null) return error.QueryFailed;
        return .{ .handle = raw };
    }

    pub fn lastError(self: *const Settings) []const u8 {
        if (self.handle == null) {
            const raw_global = c.ifcopenshell_ifcserializers_last_error();
            if (raw_global == null) return "";
            return std.mem.span(raw_global);
        }
        const raw = c.ifcopenshell_ifcserializers_settings_last_error(self.handle.?);
        if (raw == null) return "";
        return std.mem.span(raw);
    }
};

pub fn exportObj(
    file: *ifcparse.File,
    geometry_settings: ?*const ifcgeom.Settings,
    serializer_settings: ?*const Settings,
    allocator: std.mem.Allocator,
    obj_path: []const u8,
    mtl_path: []const u8,
    options: ExportOptions,
) QueryError!void {
    const file_h = try fileHandle(file);
    const geom_h = try geomSettingsHandle(geometry_settings);
    const ser_h = try serializerSettingsHandle(serializer_settings);

    const obj_z = try toZ(allocator, obj_path);
    defer allocator.free(obj_z);
    const mtl_z = try toZ(allocator, mtl_path);
    defer allocator.free(mtl_z);
    const lib_z = try toZ(allocator, options.geometry_library);
    defer allocator.free(lib_z);

    try mapBoolResult(c.ifcopenshell_ifcserializers_export_obj(
        file_h,
        geom_h,
        ser_h,
        obj_z.ptr,
        mtl_z.ptr,
        lib_z.ptr,
        @intCast(options.num_threads),
    ));
}

pub fn exportSvg(
    file: *ifcparse.File,
    geometry_settings: ?*const ifcgeom.Settings,
    serializer_settings: ?*const Settings,
    allocator: std.mem.Allocator,
    svg_path: []const u8,
    options: ExportOptions,
) QueryError!void {
    const file_h = try fileHandle(file);
    const geom_h = try geomSettingsHandle(geometry_settings);
    const ser_h = try serializerSettingsHandle(serializer_settings);

    const svg_z = try toZ(allocator, svg_path);
    defer allocator.free(svg_z);
    const lib_z = try toZ(allocator, options.geometry_library);
    defer allocator.free(lib_z);

    try mapBoolResult(c.ifcopenshell_ifcserializers_export_svg(
        file_h,
        geom_h,
        ser_h,
        svg_z.ptr,
        lib_z.ptr,
        @intCast(options.num_threads),
    ));
}

pub fn exportTtl(
    file: *ifcparse.File,
    geometry_settings: ?*const ifcgeom.Settings,
    serializer_settings: ?*const Settings,
    allocator: std.mem.Allocator,
    ttl_path: []const u8,
    options: ExportOptions,
) QueryError!void {
    const file_h = try fileHandle(file);
    const geom_h = try geomSettingsHandle(geometry_settings);
    const ser_h = try serializerSettingsHandle(serializer_settings);

    const ttl_z = try toZ(allocator, ttl_path);
    defer allocator.free(ttl_z);
    const lib_z = try toZ(allocator, options.geometry_library);
    defer allocator.free(lib_z);

    try mapBoolResult(c.ifcopenshell_ifcserializers_export_ttl(
        file_h,
        geom_h,
        ser_h,
        ttl_z.ptr,
        lib_z.ptr,
        @intCast(options.num_threads),
    ));
}

pub fn exportGltf(
    file: *ifcparse.File,
    geometry_settings: ?*const ifcgeom.Settings,
    serializer_settings: ?*const Settings,
    allocator: std.mem.Allocator,
    gltf_path: []const u8,
    options: ExportOptions,
) QueryError!void {
    if (!hasGltf()) return error.Unsupported;

    const file_h = try fileHandle(file);
    const geom_h = try geomSettingsHandle(geometry_settings);
    const ser_h = try serializerSettingsHandle(serializer_settings);

    const gltf_z = try toZ(allocator, gltf_path);
    defer allocator.free(gltf_z);
    const lib_z = try toZ(allocator, options.geometry_library);
    defer allocator.free(lib_z);

    try mapBoolResult(c.ifcopenshell_ifcserializers_export_gltf(
        file_h,
        geom_h,
        ser_h,
        gltf_z.ptr,
        lib_z.ptr,
        @intCast(options.num_threads),
    ));
}

pub fn exportXml(
    file: *ifcparse.File,
    allocator: std.mem.Allocator,
    xml_path: []const u8,
) QueryError!void {
    const file_h = try fileHandle(file);
    const xml_z = try toZ(allocator, xml_path);
    defer allocator.free(xml_z);

    try mapBoolResult(c.ifcopenshell_ifcserializers_export_xml(
        file_h,
        xml_z.ptr,
    ));
}

pub fn exportJson(
    file: *ifcparse.File,
    allocator: std.mem.Allocator,
    json_path: []const u8,
) QueryError!void {
    if (!hasJson()) return error.Unsupported;

    const file_h = try fileHandle(file);
    const json_z = try toZ(allocator, json_path);
    defer allocator.free(json_z);

    try mapBoolResult(c.ifcopenshell_ifcserializers_export_json(
        file_h,
        json_z.ptr,
    ));
}
