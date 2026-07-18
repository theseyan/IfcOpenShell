// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ifcapi/ifcapi.h"
#include "ifcapi/bindings/unit.h"
#include "ifcapi/detail/attribute.h"
#include "ifcapi/detail/unit.h"
#include "entity_introspection.hpp"
#include "ifcopenshell_api_internal.hpp"

#include "ifcparse/file.h"
#include "ifcparse/schema.h"
#include "ifcparse/express.h"

#include <algorithm>
#include <array>
#include <cctype>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

namespace {

const std::vector<std::pair<std::string, double>>& prefixes_table() {
    static const std::vector<std::pair<std::string, double>> t = {
        {"EXA", 1e18}, {"PETA", 1e15}, {"TERA", 1e12}, {"GIGA", 1e9},
        {"MEGA", 1e6}, {"KILO", 1e3}, {"HECTO", 1e2}, {"DECA", 1e1},
        {"DECI", 1e-1}, {"CENTI", 1e-2}, {"MILLI", 1e-3}, {"MICRO", 1e-6},
        {"NANO", 1e-9}, {"PICO", 1e-12}, {"FEMTO", 1e-15}, {"ATTO", 1e-18},
    };
    return t;
}

const std::vector<std::string>& unit_names_table() {
    static const std::vector<std::string> t = {
        "AMPERE", "BECQUEREL", "CANDELA", "COULOMB", "CUBIC_METRE",
        "DEGREE_CELSIUS", "FARAD", "GRAM", "GRAY", "HENRY", "HERTZ",
        "JOULE", "KELVIN", "LUMEN", "LUX", "MOLE", "NEWTON", "OHM",
        "PASCAL", "RADIAN", "SECOND", "SIEMENS", "SIEVERT",
        "SQUARE_METRE", "METRE", "STERADIAN", "TESLA", "VOLT", "WATT", "WEBER",
    };
    return t;
}

const std::unordered_map<std::string, std::array<int, 7>>& si_dimensions_table() {
    static const std::unordered_map<std::string, std::array<int, 7>> t = {
        {"METRE", {1, 0, 0, 0, 0, 0, 0}},
        {"SQUARE_METRE", {2, 0, 0, 0, 0, 0, 0}},
        {"CUBIC_METRE", {3, 0, 0, 0, 0, 0, 0}},
        {"GRAM", {0, 1, 0, 0, 0, 0, 0}},
        {"SECOND", {0, 0, 1, 0, 0, 0, 0}},
        {"AMPERE", {0, 0, 0, 1, 0, 0, 0}},
        {"KELVIN", {0, 0, 0, 0, 1, 0, 0}},
        {"MOLE", {0, 0, 0, 0, 0, 1, 0}},
        {"CANDELA", {0, 0, 0, 0, 0, 0, 1}},
        {"RADIAN", {0, 0, 0, 0, 0, 0, 0}},
        {"STERADIAN", {0, 0, 0, 0, 0, 0, 0}},
        {"HERTZ", {0, 0, -1, 0, 0, 0, 0}},
        {"NEWTON", {1, 1, -2, 0, 0, 0, 0}},
        {"PASCAL", {-1, 1, -2, 0, 0, 0, 0}},
        {"JOULE", {2, 1, -2, 0, 0, 0, 0}},
        {"WATT", {2, 1, -3, 0, 0, 0, 0}},
        {"COULOMB", {0, 0, 1, 1, 0, 0, 0}},
        {"VOLT", {2, 1, -3, -1, 0, 0, 0}},
        {"FARAD", {-2, -1, 4, 2, 0, 0, 0}},
        {"OHM", {2, 1, -3, -2, 0, 0, 0}},
        {"SIEMENS", {-2, -1, 3, 2, 0, 0, 0}},
        {"WEBER", {2, 1, -2, -1, 0, 0, 0}},
        {"TESLA", {0, 1, -2, -1, 0, 0, 0}},
        {"HENRY", {2, 1, -2, -2, 0, 0, 0}},
        {"DEGREE_CELSIUS", {0, 0, 0, 0, 1, 0, 0}},
        {"LUMEN", {0, 0, 0, 0, 0, 0, 1}},
        {"LUX", {-2, 0, 0, 0, 0, 0, 1}},
        {"BECQUEREL", {0, 0, -1, 0, 0, 0, 0}},
        {"GRAY", {2, 0, -2, 0, 0, 0, 0}},
        {"SIEVERT", {2, 0, -2, 0, 0, 0, 0}},
        {"OTHERWISE", {0, 0, 0, 0, 0, 0, 0}},
    };
    return t;
}

const std::unordered_map<std::string, std::string>& prefix_symbols_table() {
    static const std::unordered_map<std::string, std::string> t = {
        {"EXA", "E"}, {"PETA", "P"}, {"TERA", "T"}, {"GIGA", "G"},
        {"MEGA", "M"}, {"KILO", "k"}, {"HECTO", "h"}, {"DECA", "da"},
        {"DECI", "d"}, {"CENTI", "c"}, {"MILLI", "m"}, {"MICRO", "\xCE\xBC"},
        {"NANO", "n"}, {"PICO", "p"}, {"FEMTO", "f"}, {"ATTO", "a"},
    };
    return t;
}

const std::unordered_map<std::string, std::string>& unit_symbols_table() {
    static const std::unordered_map<std::string, std::string> t = {
        {"CUBIC_METRE", "m3"}, {"GRAM", "g"}, {"SECOND", "s"},
        {"SQUARE_METRE", "m2"}, {"METRE", "m"}, {"NEWTON", "N"}, {"PASCAL", "Pa"},
        {"pound-force", "lbf"}, {"pound-force per square inch", "psi"},
        {"thou", "th"}, {"inch", "in"}, {"foot", "ft"}, {"yard", "yd"}, {"mile", "mi"},
        {"square thou", "th2"}, {"square inch", "in2"}, {"square foot", "ft2"},
        {"square yard", "yd2"}, {"acre", "ac"}, {"square mile", "mi2"},
        {"cubic thou", "th3"}, {"cubic inch", "in3"}, {"cubic foot", "ft3"},
        {"cubic yard", "yd3"}, {"cubic mile", "mi3"}, {"litre", "L"},
        {"fluid ounce UK", "fl oz"}, {"fluid ounce US", "fl oz"},
        {"pint UK", "pt"}, {"pint US", "pt"},
        {"gallon UK", "gal"}, {"gallon US", "gal"},
        {"degree", "\xC2\xB0"},
        {"ounce", "oz"}, {"pound", "lb"},
        {"ton UK", "ton"}, {"ton US", "ton"}, {"tonne", "t"},
        {"lbf", "lbf"}, {"kip", "kip"}, {"psi", "psi"}, {"ksi", "ksi"},
        {"minute", "min"}, {"hour", "hr"}, {"day", "day"},
        {"btu", "btu"}, {"fahrenheit", "\xC2\xB0""F"},
    };
    return t;
}

std::string upper_str(const std::string& s) {
    std::string r = s;
    std::transform(r.begin(), r.end(), r.begin(), [](unsigned char c){ return std::toupper(c); });
    return r;
}

std::string lower_str(const std::string& s) {
    std::string r = s;
    std::transform(r.begin(), r.end(), r.begin(), [](unsigned char c){ return std::tolower(c); });
    return r;
}

std::string replace_all(std::string s, const std::string& from, const std::string& to) {
    if (from.empty()) return s;
    size_t pos = 0;
    while ((pos = s.find(from, pos)) != std::string::npos) {
        s.replace(pos, from.size(), to);
        pos += to.size();
    }
    return s;
}

const char* find_prefix_in(const std::string& upper) {
    for (const auto& kv : prefixes_table()) {
        if (upper.find(kv.first) != std::string::npos) return kv.first.c_str();
    }
    return nullptr;
}

double prefix_multiplier(const std::string& prefix) {
    for (const auto& kv : prefixes_table()) {
        if (kv.first == prefix) return kv.second;
    }
    return 1.0;
}

const char* find_unit_name_in(const std::string& upper) {
    for (const auto& name : unit_names_table()) {
        std::string spaced = replace_all(name, "_", " ");
        if (upper.find(spaced) != std::string::npos) return name.c_str();
    }
    return nullptr;
}

const char* find_imperial_in(const std::string& upper) {
    for (const auto& kv : ifcapi::detail::unit::imperial_types()) {
        if (upper.find(upper_str(kv.first)) != std::string::npos) return kv.first.c_str();
    }
    return nullptr;
}

char* dup_cstr(const std::string& s) {
    char* r = (char*)std::malloc(s.size() + 1);
    if (!r) return nullptr;
    std::memcpy(r, s.c_str(), s.size() + 1);
    return r;
}

// IfcConversionBasedUnit chain: keep unwrapping.
double convert_value_for_unit(double value, express::Base unit, bool to_si) {
    if (!unit) return value;
    while (unit && unit.declaration().is("IfcConversionBasedUnit")) {
        auto cf = ifcapi::detail::read_ref_attr(unit, "ConversionFactor");
        if (!cf) break;
        if (auto vc = ifcapi::detail::read_ref_attr(cf, "ValueComponent")) {
            try {
                auto v = vc.get_attribute_value(0);
                if (!v.isNull()) {
                    double f = (double)v;
                    if (to_si) value *= f;
                    else value /= f;
                }
            } catch (...) {}
        }
        unit = ifcapi::detail::read_ref_attr(cf, "UnitComponent");
    }
    if (unit && unit.declaration().is("IfcSIUnit")) {
        std::string prefix = ifcapi::detail::read_string_attr(unit, "Prefix");
        if (!prefix.empty()) {
            double m = prefix_multiplier(prefix);
            if (to_si) value *= m;
            else value /= m;
        }
    }
    return value;
}

// Resolve a measure class name from a wrapped value entity (e.g. IfcLengthMeasure).
std::string entity_type_name(express::Base e) {
    if (!e) return {};
    return e.declaration().name();
}

// For IfcPhysicalSimpleQuantity, attribute index 3 holds the typed value (e.g. LengthValue).
// Resolve its declared type's name (e.g. "IfcLengthMeasure").
std::string simple_quantity_measure_class(express::Base e) {
    if (!e) return {};
    auto* decl = e.declaration().as_entity();
    if (!decl) return {};
    auto attrs = decl->all_attributes();
    if (attrs.size() <= 3) return {};
    const ifcopenshell::parameter_type* pt = attrs[3]->type_of_attribute();
    while (pt) {
        auto* nt = pt->as_named_type();
        if (!nt) break;
        auto* d = nt->declared_type();
        if (auto* td = d->as_type_declaration()) {
            return td->name();
        }
        if (d) return d->name();
        break;
    }
    return {};
}

std::string measure_to_unit_type(const std::string& measure_class) {
    if (measure_class == "IfcNumericMeasure") return "USERDEFINED";
    std::string s = measure_class;
    for (const char* tok : {"Ifc", "Measure", "Non", "Positive", "Negative"}) {
        size_t pos;
        while ((pos = s.find(tok)) != std::string::npos) {
            s.erase(pos, std::strlen(tok));
        }
    }
    return upper_str(s) + "UNIT";
}

express::Base unit_assignment_for(ifcopenshell::file* file) {
    if (!file) return {};
    try {
        const auto* proj_decl = file->schema()->declaration_by_name("IfcProject");
        auto projects = file->instances_by_type(proj_decl);
        if (projects.empty()) return {};
        return ifcapi::detail::read_ref_attr(projects[0], "UnitsInContext");
    } catch (...) {
        return {};
    }
}

express::Base project_unit_for(ifcopenshell::file* file, const std::string& unit_type) {
    auto uia = unit_assignment_for(file);
    if (!uia) return {};
    for (auto unit : ifcapi::detail::read_ref_aggregate(uia, "Units")) {
        if (ifcapi::detail::read_string_attr(unit, "UnitType") == unit_type) return unit;
    }
    return {};
}

}  // namespace

namespace ifcapi {
namespace bindings {

std::string unit_get_prefix(const std::string& text) {
    if (text.empty()) return std::string();
    const char* result = find_prefix_in(upper_str(text));
    return result ? std::string(result) : std::string();
}

double unit_get_prefix_multiplier(const std::string& text) {
    if (text.empty()) return 1.0;
    const char* p = find_prefix_in(upper_str(text));
    if (!p) return 1.0;
    return prefix_multiplier(p);
}

std::string unit_get_unit_name(const std::string& text) {
    std::string up = replace_all(upper_str(text), "METER", "METRE");
    const char* result = find_unit_name_in(up);
    return result ? std::string(result) : std::string();
}

std::string unit_get_unit_name_universal(const std::string& text) {
    std::string up = replace_all(upper_str(text), "METER", "METRE");
    if (const char* n = find_unit_name_in(up)) return std::string(n);
    const char* result = find_imperial_in(up);
    return result ? std::string(result) : std::string();
}

std::string unit_get_measure_class(const std::string& unit_type) {
    if (unit_type.empty()) return std::string();
    if (unit_type == "USERDEFINED") return "IfcNumericMeasure";
    if (unit_type.size() < 4) return std::string();
    std::string body = unit_type.substr(0, unit_type.size() - 4);
    body = lower_str(body);
    if (!body.empty()) body[0] = std::toupper((unsigned char)body[0]);
    return "Ifc" + body + "Measure";
}

std::string unit_get_measure_unit_type(const std::string& measure_class) {
    if (measure_class.empty()) return std::string();
    return measure_to_unit_type(measure_class);
}

std::string unit_get_symbol_measure_class(const std::string& symbol) {
    if (symbol.empty()) return "IfcNumericMeasure";
    std::string s = lower_str(symbol);
    static const std::vector<std::string> length = {"km", "m", "cm", "mm", "ly", "lf", "lin", "yd", "ft", "in"};
    static const std::vector<std::string> area = {"km2", "m2", "cm2", "mm2", "sqy", "sqft", "sqin"};
    static const std::vector<std::string> vol = {"km3", "m3", "cm3", "mm3", "cy", "cft", "cin"};
    static const std::vector<std::string> mass = {"kg", "g", "mt", "kt", "t"};
    static const std::vector<std::string> tim = {"day", "d", "hour", "hr", "h", "minute", "min", "m", "second", "sec", "s"};
    auto in = [&](const std::vector<std::string>& v){ return std::find(v.begin(), v.end(), s) != v.end(); };
    if (in(length)) return "IfcLengthMeasure";
    if (in(area)) return "IfcAreaMeasure";
    if (in(vol)) return "IfcVolumeMeasure";
    if (in(mass)) return "IfcMassMeasure";
    if (in(tim)) return "IfcTimeMeasure";
    return "IfcNumericMeasure";
}

std::string unit_get_symbol_quantity_class(const std::string& symbol) {
    if (symbol.empty()) return "IfcQuantityCount";
    std::string s = lower_str(symbol);
    static const std::vector<std::string> length = {"km", "m", "cm", "mm", "ly", "lf", "lin", "yd", "ft", "in"};
    static const std::vector<std::string> area = {"km2", "m2", "cm2", "mm2", "sqy", "sqft", "sqin"};
    static const std::vector<std::string> vol = {"km3", "m3", "cm3", "mm3", "cy", "cft", "cin"};
    static const std::vector<std::string> mass = {"kg", "g", "mt", "kt", "t"};
    static const std::vector<std::string> tim = {"day", "d", "hour", "hr", "h", "minute", "min", "m", "second", "sec", "s"};
    auto in = [&](const std::vector<std::string>& v){ return std::find(v.begin(), v.end(), s) != v.end(); };
    if (in(length)) return "IfcQuantityLength";
    if (in(area)) return "IfcQuantityArea";
    if (in(vol)) return "IfcQuantityVolume";
    if (in(mass)) return "IfcQuantityWeight";
    if (in(tim)) return "IfcQuantityTime";
    return "IfcQuantityCount";
}

Dimensions7 unit_get_si_dimensions(const std::string& name) {
    Dimensions7 result{};
    const auto& tbl = si_dimensions_table();
    auto it = tbl.find(name);
    if (it == tbl.end()) it = tbl.find("OTHERWISE");
    for (int i = 0; i < 7; ++i) result[i] = it->second[i];
    return result;
}

Dimensions7 unit_get_named_dimensions(const std::string& name) {
    Dimensions7 result{};
    const auto& tbl = ifcapi::detail::unit::named_dimensions();
    auto it = tbl.find(name);
    if (it == tbl.end()) return result;
    for (int i = 0; i < 7; ++i) result[i] = it->second[i];
    return result;
}

double unit_convert(
    double value,
    const std::string& from_prefix, const std::string& from_unit,
    const std::string& to_prefix, const std::string& to_unit)
{
    if (from_unit.empty() || to_unit.empty()) return value;
    std::string fu = from_unit, tu = to_unit;
    std::string fu_low = lower_str(fu);
    std::string tu_low = lower_str(tu);
    const auto& sic = ifcapi::detail::unit::si_conversions();
    auto fit = sic.find(fu_low);
    if (fit != sic.end()) {
        value *= fit->second;
    } else if (!from_prefix.empty()) {
        double m = prefix_multiplier(from_prefix);
        value *= m;
        if (fu.find("SQUARE") != std::string::npos) value *= m;
        else if (fu.find("CUBIC") != std::string::npos) { value *= m; value *= m; }
    }
    auto tit = sic.find(tu_low);
    if (tit != sic.end()) {
        return value * (1.0 / tit->second);
    } else if (!to_prefix.empty()) {
        double m = prefix_multiplier(to_prefix);
        value *= 1.0 / m;
        if (fu.find("SQUARE") != std::string::npos) value *= 1.0 / m;
        else if (fu.find("CUBIC") != std::string::npos) { value *= 1.0 / m; value *= 1.0 / m; }
    }
    return value;
}

// ---------------------------------------------------------------------------
// format_length: returns malloc'd C string (NULL on bad input). Caller frees
// with ifcopenshell_free_string.
// ---------------------------------------------------------------------------

namespace {

long long gcd_ll(long long a, long long b) {
    a = std::llabs(a); b = std::llabs(b);
    while (b) { long long t = a % b; a = b; b = t; }
    return a ? a : 1;
}

std::string fmt_imperial(int feet, long long num, long long den, int suppress_zero, const std::string& output_unit) {
    long long g = gcd_ll(num, den);
    long long n = num / g;
    long long d = den / g;
    std::ostringstream os;
    if (d == 1) {
        if (suppress_zero && n == 0) {
            if (output_unit == "foot") os << feet << "'";
            else os << (feet * 12) << "\"";
            return os.str();
        }
        if (output_unit == "foot") os << feet << "' - " << n << "\"";
        else os << ((feet * 12) + n) << "\"";
        return os.str();
    }
    if (n > d) {
        long long rem = n % d;
        long long whole = (n - rem) / d;
        if (output_unit == "foot") os << feet << "' - " << whole << " " << rem << "/" << d << "\"";
        else os << ((feet * 12) + whole) << " " << rem << "/" << d << "\"";
        return os.str();
    }
    if (output_unit == "foot") os << feet << "' - 0 " << n << "/" << d << "\"";
    else os << (feet * 12) << " " << n << "/" << d << "\"";
    return os.str();
}

}  // namespace

std::string unit_format_length(
    double value, double precision, int decimal_places,
    bool suppress_zero_inches, const std::string& unit_system,
    const std::string& input_unit, const std::string& output_unit)
{
    std::string sys = unit_system.empty() ? "imperial" : unit_system;
    std::string in_u = input_unit.empty() ? "foot" : input_unit;
    std::string out_u = output_unit.empty() ? "foot" : output_unit;

    if (sys == "imperial") {
        int feet;
        double inches;
        if (in_u == "foot") {
            feet = (int)value;
            inches = (value - feet) * 12.0;
        } else {
            inches = std::fmod(value, 12.0);
            feet = (int)std::round((value - inches) / 12.0);
        }
        long long nearest = (long long)std::llround(inches * precision);
        long long den = (long long)std::llround(precision);
        if (den <= 0) den = 1;
        return fmt_imperial(feet, nearest, den, suppress_zero_inches, out_u);
    } else if (sys == "metric") {
        double rounded = std::round(value / precision) * precision;
        std::ostringstream os;
        os.precision(decimal_places);
        os << std::fixed << rounded;
        return os.str();
    }
    return std::string();
}

express::Base unit_get_unit_assignment(ifcopenshell::file* file) {
    if (!file) return {};
    return unit_assignment_for(file);
}

express::Base unit_get_project_unit(ifcopenshell::file* file, const std::string& unit_type) {
    if (!file || unit_type.empty()) return {};
    return project_unit_for(file, unit_type);
}

std::string unit_get_full_unit_name(express::Base* unit_ptr) {
    auto unit = ifcapi::detail::deref_or_empty(unit_ptr);
    if (!unit) return std::string();
    std::string prefix = ifcapi::detail::read_string_attr(unit, "Prefix");
    std::string name = upper_str(ifcapi::detail::read_string_attr(unit, "Name"));
    return prefix + name;
}

std::string unit_get_unit_symbol(express::Base* unit_ptr) {
    auto unit = ifcapi::detail::deref_or_empty(unit_ptr);
    if (!unit) return std::string();
    std::string symbol;
    if (unit.declaration().is("IfcSIUnit")) {
        std::string prefix = ifcapi::detail::read_string_attr(unit, "Prefix");
        const auto& ps = prefix_symbols_table();
        auto pit = ps.find(prefix);
        if (pit != ps.end()) symbol += pit->second;
    }
    std::string name = ifcapi::detail::read_string_attr(unit, "Name");
    name = replace_all(name, "METER", "METRE");
    const auto& us = unit_symbols_table();
    auto uit = us.find(name);
    if (uit != us.end()) symbol += uit->second;
    else symbol += "?";
    if (unit.declaration().is("IfcContextDependentUnit")) {
        std::string ut = ifcapi::detail::read_string_attr(unit, "UnitType");
        if (ut == "USERDEFINED") {
            symbol = ifcapi::detail::read_string_attr(unit, "Name");
        }
    }
    return symbol;
}

double unit_convert_unit(double value, express::Base* from_unit_ptr, express::Base* to_unit_ptr) {
    auto from_unit = ifcapi::detail::deref_or_empty(from_unit_ptr);
    auto to_unit = ifcapi::detail::deref_or_empty(to_unit_ptr);
    if (!from_unit || !to_unit) return value;
    std::string from_prefix = ifcapi::detail::read_string_attr(from_unit, "Prefix");
    std::string from_name = ifcapi::detail::read_string_attr(from_unit, "Name");
    std::string to_prefix = ifcapi::detail::read_string_attr(to_unit, "Prefix");
    std::string to_name = ifcapi::detail::read_string_attr(to_unit, "Name");
    return ifcapi::bindings::unit_convert(value, from_prefix, from_name, to_prefix, to_name);
}

} // namespace bindings
} // namespace ifcapi

// ---------------------------------------------------------------------------
// Entity-based ABI.
// ---------------------------------------------------------------------------

namespace {

// Pull the first element of an aggregate attribute (or return nullptr).
express::Base first_in_list(express::Base e, const char* attr) {
    auto v = ifcapi::detail::read_ref_aggregate(e, attr);
    return v.empty() ? express::Base() : v.front();
}

// Inspect a property/quantity and return either a directly-attached Unit or
// the measure class name to look up against project defaults. This split lets
// the Python wrapper route the project-default fallback through the cache-aware
// get_project_unit helper.
void resolve_property_unit_or_class(express::Base prop,
                                    express::Base* out_unit,
                                    std::string* out_measure_class) {
    *out_unit = {};
    out_measure_class->clear();
    if (!prop) return;

    if (auto u = ifcapi::detail::read_ref_attr(prop, "Unit")) { *out_unit = u; return; }

    auto& d = prop.declaration();
    if (d.is("IfcPhysicalSimpleQuantity")) {
        *out_measure_class = simple_quantity_measure_class(prop);
    } else if (d.is("IfcPropertySingleValue")) {
        if (auto nv = ifcapi::detail::read_ref_attr(prop, "NominalValue")) {
            *out_measure_class = entity_type_name(nv);
        }
    } else if (d.is("IfcPropertyEnumeratedValue")) {
        if (auto ref = ifcapi::detail::read_ref_attr(prop, "EnumerationReference")) {
            if (auto u = ifcapi::detail::read_ref_attr(ref, "Unit")) { *out_unit = u; return; }
            if (auto v = first_in_list(ref, "EnumerationValues")) *out_measure_class = entity_type_name(v);
        }
        if (auto v = first_in_list(prop, "EnumerationValues")) *out_measure_class = entity_type_name(v);
    } else if (d.is("IfcPropertyListValue")) {
        if (auto v = first_in_list(prop, "ListValues")) *out_measure_class = entity_type_name(v);
    } else if (d.is("IfcPropertyBoundedValue")) {
        auto v = ifcapi::detail::read_ref_attr(prop, "UpperBoundValue");
        if (!v) v = ifcapi::detail::read_ref_attr(prop, "LowerBoundValue");
        if (!v) v = ifcapi::detail::read_ref_attr(prop, "SetPointValue");
        if (v) *out_measure_class = entity_type_name(v);
    }
}

void resolve_table_side(express::Base prop, const char* unit_attr, const char* values_attr,
                        express::Base* out_unit, std::string* out_measure_class) {
    *out_unit = ifcapi::detail::read_ref_attr(prop, unit_attr);
    out_measure_class->clear();
    if (*out_unit) return;
    if (auto v = first_in_list(prop, values_attr)) {
        *out_measure_class = entity_type_name(v);
    }
}

}  // namespace

namespace ifcapi {
namespace bindings {

express::Base unit_resolve_property_unit(express::Base* prop_ptr) {
    auto prop = ifcapi::detail::deref_or_empty(prop_ptr);
    express::Base unit;
    std::string mc;
    resolve_property_unit_or_class(prop, &unit, &mc);
    return unit;
}

std::string unit_resolve_property_measure_class(express::Base* prop_ptr) {
    auto prop = ifcapi::detail::deref_or_empty(prop_ptr);
    express::Base unit;
    std::string mc;
    resolve_property_unit_or_class(prop, &unit, &mc);
    return unit ? std::string() : mc;
}

express::Base unit_resolve_property_table_defining_unit(express::Base* prop_ptr) {
    auto prop = ifcapi::detail::deref_or_empty(prop_ptr);
    express::Base u;
    std::string mc;
    resolve_table_side(prop, "DefiningUnit", "DefiningValues", &u, &mc);
    return u;
}

std::string unit_resolve_property_table_defining_measure_class(express::Base* prop_ptr) {
    auto prop = ifcapi::detail::deref_or_empty(prop_ptr);
    express::Base u;
    std::string mc;
    resolve_table_side(prop, "DefiningUnit", "DefiningValues", &u, &mc);
    return u ? std::string() : mc;
}

express::Base unit_resolve_property_table_defined_unit(express::Base* prop_ptr) {
    auto prop = ifcapi::detail::deref_or_empty(prop_ptr);
    express::Base u;
    std::string mc;
    resolve_table_side(prop, "DefinedUnit", "DefinedValues", &u, &mc);
    return u;
}

std::string unit_resolve_property_table_defined_measure_class(express::Base* prop_ptr) {
    auto prop = ifcapi::detail::deref_or_empty(prop_ptr);
    express::Base u;
    std::string mc;
    resolve_table_side(prop, "DefinedUnit", "DefinedValues", &u, &mc);
    return u ? std::string() : mc;
}

double unit_calculate_unit_scale(ifcopenshell::file* file, const std::string& unit_type) {
    if (!file) return 1.0;
    std::string want = unit_type.empty() ? "LENGTHUNIT" : unit_type;
    auto unit = project_unit_for(file, want);
    if (!unit) return 1.0;
    return convert_value_for_unit(1.0, unit, /*to_si=*/true);
}

} // namespace bindings
} // namespace ifcapi
