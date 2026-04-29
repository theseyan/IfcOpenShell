// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ifcapi/ifcapi.h"
#include "ifcapi/bindings/unit.h"
#include "entity_introspection.hpp"
#include "ifcopenshell_api_internal.hpp"

#include "ifcparse/IfcFile.h"
#include "ifcparse/IfcSchema.h"
#include "ifcparse/IfcBaseClass.h"

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

const std::vector<std::pair<std::string, std::string>>& imperial_types_table() {
    static const std::vector<std::pair<std::string, std::string>> t = {
        {"thou", "LENGTHUNIT"}, {"inch", "LENGTHUNIT"}, {"foot", "LENGTHUNIT"},
        {"yard", "LENGTHUNIT"}, {"mile", "LENGTHUNIT"},
        {"square thou", "AREAUNIT"}, {"square inch", "AREAUNIT"},
        {"square foot", "AREAUNIT"}, {"square yard", "AREAUNIT"},
        {"acre", "AREAUNIT"}, {"square mile", "AREAUNIT"},
        {"cubic thou", "VOLUMEUNIT"}, {"cubic inch", "VOLUMEUNIT"},
        {"cubic foot", "VOLUMEUNIT"}, {"cubic yard", "VOLUMEUNIT"},
        {"cubic mile", "VOLUMEUNIT"}, {"litre", "VOLUMEUNIT"},
        {"fluid ounce UK", "VOLUMEUNIT"}, {"fluid ounce US", "VOLUMEUNIT"},
        {"pint UK", "VOLUMEUNIT"}, {"pint US", "VOLUMEUNIT"},
        {"gallon UK", "VOLUMEUNIT"}, {"gallon US", "VOLUMEUNIT"},
        {"degree", "PLANEANGLEUNIT"},
        {"ounce", "MASSUNIT"}, {"pound", "MASSUNIT"},
        {"ton UK", "MASSUNIT"}, {"ton US", "MASSUNIT"}, {"tonne", "MASSUNIT"},
        {"lbf", "FORCEUNIT"}, {"kip", "FORCEUNIT"},
        {"psi", "PRESSUREUNIT"}, {"ksi", "PRESSUREUNIT"},
        {"minute", "TIMEUNIT"}, {"hour", "TIMEUNIT"}, {"day", "TIMEUNIT"},
        {"btu", "ENERGYUNIT"},
        {"fahrenheit", "THERMODYNAMICTEMPERATUREUNIT"},
    };
    return t;
}

const std::unordered_map<std::string, double>& si_conversions_table() {
    static const std::unordered_map<std::string, double> t = {
        {"thou", 0.0000254}, {"inch", 0.0254}, {"foot", 0.3048},
        {"yard", 0.914}, {"mile", 1609},
        {"square thou", 6.4516e-10}, {"square inch", 0.0006452},
        {"square foot", 0.09290304}, {"square yard", 0.83612736},
        {"acre", 4046.86}, {"square mile", 2588881},
        {"cubic thou", 1.6387064e-14}, {"cubic inch", 0.00001639},
        {"cubic foot", 0.02831684671168849}, {"cubic yard", 0.7636},
        {"cubic mile", 4165509529.0}, {"litre", 0.001},
        {"fluid ounce UK", 0.0000284130625}, {"fluid ounce US", 0.00002957353},
        {"pint UK", 0.000568}, {"pint US", 0.000473},
        {"gallon UK", 0.004546}, {"gallon US", 0.003785},
        {"degree", M_PI / 180.0},
        {"ounce", 0.02835}, {"pound", 0.454},
        {"ton UK", 1016.0469088}, {"ton US", 907.18474}, {"tonne", 1000.0},
        {"lbf", 4.4482216153}, {"kip", 4448.2216153},
        {"psi", 6894.7572932}, {"ksi", 6894757.2932},
        {"minute", 60}, {"hour", 3600}, {"day", 86400},
        {"btu", 1055.056}, {"fahrenheit", 1.8},
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

const std::unordered_map<std::string, std::array<int, 7>>& named_dimensions_table() {
    static const std::unordered_map<std::string, std::array<int, 7>> t = {
        {"ABSORBEDDOSEUNIT", {2, 0, -2, 0, 0, 0, 0}},
        {"AMOUNTOFSUBSTANCEUNIT", {0, 0, 0, 0, 0, 1, 0}},
        {"AREAUNIT", {2, 0, 0, 0, 0, 0, 0}},
        {"DOSEEQUIVALENTUNIT", {2, 0, -2, 0, 0, 0, 0}},
        {"ELECTRICCAPACITANCEUNIT", {-2, -1, 4, 2, 0, 0, 0}},
        {"ELECTRICCHARGEUNIT", {0, 0, 1, 1, 0, 0, 0}},
        {"ELECTRICCONDUCTANCEUNIT", {-2, -1, 3, 2, 0, 0, 0}},
        {"ELECTRICCURRENTUNIT", {0, 0, 0, 1, 0, 0, 0}},
        {"ELECTRICRESISTANCEUNIT", {2, 1, -3, -2, 0, 0, 0}},
        {"ELECTRICVOLTAGEUNIT", {2, 1, -3, -1, 0, 0, 0}},
        {"ENERGYUNIT", {2, 1, -2, 0, 0, 0, 0}},
        {"FORCEUNIT", {1, 1, -2, 0, 0, 0, 0}},
        {"FREQUENCYUNIT", {0, 0, -1, 0, 0, 0, 0}},
        {"ILLUMINANCEUNIT", {-2, 0, 0, 0, 0, 1, 1}},
        {"INDUCTANCEUNIT", {2, 1, -2, -2, 0, 0, 0}},
        {"LENGTHUNIT", {1, 0, 0, 0, 0, 0, 0}},
        {"LUMINOUSFLUXUNIT", {0, 0, 0, 0, 0, 1, 1}},
        {"LUMINOUSINTENSITYUNIT", {0, 0, 0, 0, 0, 0, 1}},
        {"MAGNETICFLUXDENSITYUNIT", {0, 1, -2, -1, 0, 0, 0}},
        {"MAGNETICFLUXUNIT", {2, 1, -2, -1, 0, 0, 0}},
        {"MASSUNIT", {0, 1, 0, 0, 0, 0, 0}},
        {"PLANEANGLEUNIT", {0, 0, 0, 0, 0, 0, 0}},
        {"POWERUNIT", {2, 1, -3, 0, 0, 0, 0}},
        {"PRESSUREUNIT", {-1, 1, -2, 0, 0, 0, 0}},
        {"RADIOACTIVITYUNIT", {0, 0, -1, 0, 0, 0, 0}},
        {"SOLIDANGLEUNIT", {0, 0, 0, 0, 0, 0, 0}},
        {"THERMODYNAMICTEMPERATUREUNIT", {0, 0, 0, 0, 1, 0, 0}},
        {"TIMEUNIT", {0, 0, 1, 0, 0, 0, 0}},
        {"VOLUMEUNIT", {3, 0, 0, 0, 0, 0, 0}},
        {"USERDEFINED", {0, 0, 0, 0, 0, 0, 0}},
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
    for (const auto& kv : imperial_types_table()) {
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
double convert_value_for_unit(double value, IfcUtil::IfcBaseClass* unit, bool to_si) {
    if (!unit) return value;
    while (unit && unit->declaration().is("IfcConversionBasedUnit")) {
        auto* cf = ifcapi::get_entity_ref(unit, "ConversionFactor");
        if (!cf) break;
        if (auto* vc = ifcapi::get_entity_ref(cf, "ValueComponent")) {
            try {
                auto v = vc->get_attribute_value(0);
                if (!v.isNull()) {
                    double f = (double)v;
                    if (to_si) value *= f;
                    else value /= f;
                }
            } catch (...) {}
        }
        unit = ifcapi::get_entity_ref(cf, "UnitComponent");
    }
    if (unit && unit->declaration().is("IfcSIUnit")) {
        std::string prefix = ifcapi::get_string_attr(unit, "Prefix");
        if (!prefix.empty()) {
            double m = prefix_multiplier(prefix);
            if (to_si) value *= m;
            else value /= m;
        }
    }
    return value;
}

// Resolve a measure class name from a wrapped value entity (e.g. IfcLengthMeasure).
std::string entity_type_name(IfcUtil::IfcBaseClass* e) {
    if (!e) return {};
    return e->declaration().name();
}

// For IfcPhysicalSimpleQuantity, attribute index 3 holds the typed value (e.g. LengthValue).
// Resolve its declared type's name (e.g. "IfcLengthMeasure").
std::string simple_quantity_measure_class(IfcUtil::IfcBaseClass* e) {
    auto* be = dynamic_cast<IfcUtil::IfcBaseEntity*>(e);
    if (!be) return {};
    auto* decl = be->declaration().as_entity();
    if (!decl) return {};
    auto attrs = decl->all_attributes();
    if (attrs.size() <= 3) return {};
    const IfcParse::parameter_type* pt = attrs[3]->type_of_attribute();
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

IfcUtil::IfcBaseClass* unit_assignment_for(IfcParse::IfcFile* file) {
    if (!file) return nullptr;
    try {
        const auto* proj_decl = file->schema()->declaration_by_name("IfcProject");
        auto projects = file->instances_by_type(proj_decl);
        if (!projects || projects->size() == 0) return nullptr;
        return ifcapi::get_entity_ref((*projects)[0], "UnitsInContext");
    } catch (...) {
        return nullptr;
    }
}

IfcUtil::IfcBaseClass* project_unit_for(IfcParse::IfcFile* file, const std::string& unit_type) {
    auto* uia = unit_assignment_for(file);
    if (!uia) return nullptr;
    for (auto* unit : ifcapi::get_entity_list(uia, "Units")) {
        if (ifcapi::get_string_attr(unit, "UnitType") == unit_type) return unit;
    }
    return nullptr;
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

} // namespace bindings
} // namespace ifcapi

extern "C" {

// ---------------------------------------------------------------------------
// Pure-string helpers.
// ---------------------------------------------------------------------------

IFCAPI_EXPORT void ifcopenshell_util_unit_get_si_dimensions(const char* name, int* out7) {
    if (!out7) return;
    const auto& tbl = si_dimensions_table();
    auto it = tbl.find(name ? name : "");
    if (it == tbl.end()) it = tbl.find("OTHERWISE");
    for (int i = 0; i < 7; ++i) out7[i] = it->second[i];
}

IFCAPI_EXPORT void ifcopenshell_util_unit_get_named_dimensions(const char* name, int* out7) {
    if (!out7) return;
    const auto& tbl = named_dimensions_table();
    auto it = tbl.find(name ? name : "");
    if (it == tbl.end()) {
        for (int i = 0; i < 7; ++i) out7[i] = 0;
        return;
    }
    for (int i = 0; i < 7; ++i) out7[i] = it->second[i];
}

IFCAPI_EXPORT double ifcopenshell_util_unit_convert(
    double value,
    const char* from_prefix, const char* from_unit,
    const char* to_prefix, const char* to_unit)
{
    if (!from_unit || !to_unit) return value;
    std::string fu = from_unit, tu = to_unit;
    std::string fu_low = lower_str(fu);
    std::string tu_low = lower_str(tu);
    const auto& sic = si_conversions_table();
    auto fit = sic.find(fu_low);
    if (fit != sic.end()) {
        value *= fit->second;
    } else if (from_prefix && *from_prefix) {
        double m = prefix_multiplier(from_prefix);
        value *= m;
        if (fu.find("SQUARE") != std::string::npos) value *= m;
        else if (fu.find("CUBIC") != std::string::npos) { value *= m; value *= m; }
    }
    auto tit = sic.find(tu_low);
    if (tit != sic.end()) {
        return value * (1.0 / tit->second);
    } else if (to_prefix && *to_prefix) {
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

IFCAPI_EXPORT char* ifcopenshell_util_unit_format_length(
    double value, double precision, int decimal_places,
    int suppress_zero_inches, const char* unit_system,
    const char* input_unit, const char* output_unit)
{
    std::string sys = unit_system ? unit_system : "imperial";
    std::string in_u = input_unit ? input_unit : "foot";
    std::string out_u = output_unit ? output_unit : "foot";

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
        return dup_cstr(fmt_imperial(feet, nearest, den, suppress_zero_inches, out_u));
    } else if (sys == "metric") {
        double rounded = std::round(value / precision) * precision;
        std::ostringstream os;
        os.precision(decimal_places);
        os << std::fixed << rounded;
        return dup_cstr(os.str());
    }
    return nullptr;
}

// ---------------------------------------------------------------------------
// Entity-based ABI.
// ---------------------------------------------------------------------------

IFCAPI_EXPORT ifcopenshell_ifc_instance_t* ifcopenshell_util_unit_get_unit_assignment(
    ifcopenshell_ifc_file_t* file_h)
{
    if (!file_h || !file_h->ptr) return nullptr;
    return ifcopenshell::capi::wrap_instance(unit_assignment_for(file_h->ptr));
}

IFCAPI_EXPORT ifcopenshell_ifc_instance_t* ifcopenshell_util_unit_get_project_unit(
    ifcopenshell_ifc_file_t* file_h, const char* unit_type)
{
    if (!file_h || !file_h->ptr || !unit_type) return nullptr;
    return ifcopenshell::capi::wrap_instance(project_unit_for(file_h->ptr, unit_type));
}

IFCAPI_EXPORT char* ifcopenshell_util_unit_get_full_unit_name(
    ifcopenshell_ifc_instance_t* unit_h)
{
    auto* unit = ifcopenshell::capi::unwrap_instance(unit_h);
    if (!unit) return nullptr;
    std::string prefix = ifcapi::get_string_attr(unit, "Prefix");
    std::string name = upper_str(ifcapi::get_string_attr(unit, "Name"));
    return dup_cstr(prefix + name);
}

IFCAPI_EXPORT char* ifcopenshell_util_unit_get_unit_symbol(
    ifcopenshell_ifc_instance_t* unit_h)
{
    auto* unit = ifcopenshell::capi::unwrap_instance(unit_h);
    if (!unit) return nullptr;
    std::string symbol;
    if (unit->declaration().is("IfcSIUnit")) {
        std::string prefix = ifcapi::get_string_attr(unit, "Prefix");
        const auto& ps = prefix_symbols_table();
        auto pit = ps.find(prefix);
        if (pit != ps.end()) symbol += pit->second;
    }
    std::string name = ifcapi::get_string_attr(unit, "Name");
    name = replace_all(name, "METER", "METRE");
    const auto& us = unit_symbols_table();
    auto uit = us.find(name);
    if (uit != us.end()) symbol += uit->second;
    else symbol += "?";
    if (unit->declaration().is("IfcContextDependentUnit")) {
        std::string ut = ifcapi::get_string_attr(unit, "UnitType");
        if (ut == "USERDEFINED") {
            symbol = ifcapi::get_string_attr(unit, "Name");
        }
    }
    return dup_cstr(symbol);
}

IFCAPI_EXPORT double ifcopenshell_util_unit_convert_unit(
    double value,
    ifcopenshell_ifc_instance_t* from_unit_h,
    ifcopenshell_ifc_instance_t* to_unit_h)
{
    auto* from_u = ifcopenshell::capi::unwrap_instance(from_unit_h);
    auto* to_u = ifcopenshell::capi::unwrap_instance(to_unit_h);
    if (!from_u || !to_u) return value;
    std::string from_prefix = ifcapi::get_string_attr(from_u, "Prefix");
    std::string from_name = ifcapi::get_string_attr(from_u, "Name");
    std::string to_prefix = ifcapi::get_string_attr(to_u, "Prefix");
    std::string to_name = ifcapi::get_string_attr(to_u, "Name");
    return ifcopenshell_util_unit_convert(
        value,
        from_prefix.c_str(), from_name.c_str(),
        to_prefix.c_str(), to_name.c_str());
}

namespace {

// Pull the first element of an aggregate attribute (or return nullptr).
IfcUtil::IfcBaseClass* first_in_list(IfcUtil::IfcBaseClass* e, const char* attr) {
    auto v = ifcapi::get_entity_list(e, attr);
    return v.empty() ? nullptr : v.front();
}

// Inspect a property/quantity and return either a directly-attached Unit or
// the measure class name to look up against project defaults. This split lets
// the Python wrapper route the project-default fallback through the cache-aware
// get_project_unit helper.
void resolve_property_unit_or_class(IfcUtil::IfcBaseClass* prop,
                                    IfcUtil::IfcBaseClass** out_unit,
                                    std::string* out_measure_class) {
    *out_unit = nullptr;
    out_measure_class->clear();
    if (!prop) return;

    if (auto* u = ifcapi::get_entity_ref(prop, "Unit")) { *out_unit = u; return; }

    auto& d = prop->declaration();
    if (d.is("IfcPhysicalSimpleQuantity")) {
        *out_measure_class = simple_quantity_measure_class(prop);
    } else if (d.is("IfcPropertySingleValue")) {
        if (auto* nv = ifcapi::get_entity_ref(prop, "NominalValue")) {
            *out_measure_class = entity_type_name(nv);
        }
    } else if (d.is("IfcPropertyEnumeratedValue")) {
        if (auto* ref = ifcapi::get_entity_ref(prop, "EnumerationReference")) {
            if (auto* u = ifcapi::get_entity_ref(ref, "Unit")) { *out_unit = u; return; }
            if (auto* v = first_in_list(ref, "EnumerationValues")) *out_measure_class = entity_type_name(v);
        }
        if (auto* v = first_in_list(prop, "EnumerationValues")) *out_measure_class = entity_type_name(v);
    } else if (d.is("IfcPropertyListValue")) {
        if (auto* v = first_in_list(prop, "ListValues")) *out_measure_class = entity_type_name(v);
    } else if (d.is("IfcPropertyBoundedValue")) {
        IfcUtil::IfcBaseClass* v = ifcapi::get_entity_ref(prop, "UpperBoundValue");
        if (!v) v = ifcapi::get_entity_ref(prop, "LowerBoundValue");
        if (!v) v = ifcapi::get_entity_ref(prop, "SetPointValue");
        if (v) *out_measure_class = entity_type_name(v);
    }
}

void resolve_table_side(IfcUtil::IfcBaseClass* prop, const char* unit_attr, const char* values_attr,
                        IfcUtil::IfcBaseClass** out_unit, std::string* out_measure_class) {
    *out_unit = ifcapi::get_entity_ref(prop, unit_attr);
    out_measure_class->clear();
    if (*out_unit) return;
    if (auto* v = first_in_list(prop, values_attr)) {
        *out_measure_class = entity_type_name(v);
    }
}

}  // namespace

IFCAPI_EXPORT void ifcopenshell_util_unit_resolve_property(
    ifcopenshell_ifc_instance_t* prop_h,
    ifcopenshell_ifc_instance_t** out_unit,
    char** out_measure_class)
{
    if (out_unit) *out_unit = nullptr;
    if (out_measure_class) *out_measure_class = nullptr;
    auto* prop = ifcopenshell::capi::unwrap_instance(prop_h);
    IfcUtil::IfcBaseClass* unit = nullptr;
    std::string mc;
    resolve_property_unit_or_class(prop, &unit, &mc);
    if (out_unit) *out_unit = ifcopenshell::capi::wrap_instance(unit);
    if (out_measure_class && !mc.empty()) *out_measure_class = dup_cstr(mc);
}

IFCAPI_EXPORT void ifcopenshell_util_unit_resolve_property_table(
    ifcopenshell_ifc_instance_t* prop_h,
    ifcopenshell_ifc_instance_t** out_defining_unit,
    char** out_defining_measure_class,
    ifcopenshell_ifc_instance_t** out_defined_unit,
    char** out_defined_measure_class)
{
    if (out_defining_unit) *out_defining_unit = nullptr;
    if (out_defining_measure_class) *out_defining_measure_class = nullptr;
    if (out_defined_unit) *out_defined_unit = nullptr;
    if (out_defined_measure_class) *out_defined_measure_class = nullptr;
    auto* prop = ifcopenshell::capi::unwrap_instance(prop_h);
    if (!prop) return;
    IfcUtil::IfcBaseClass* u = nullptr;
    std::string mc;
    resolve_table_side(prop, "DefiningUnit", "DefiningValues", &u, &mc);
    if (out_defining_unit) *out_defining_unit = ifcopenshell::capi::wrap_instance(u);
    if (out_defining_measure_class && !mc.empty()) *out_defining_measure_class = dup_cstr(mc);
    resolve_table_side(prop, "DefinedUnit", "DefinedValues", &u, &mc);
    if (out_defined_unit) *out_defined_unit = ifcopenshell::capi::wrap_instance(u);
    if (out_defined_measure_class && !mc.empty()) *out_defined_measure_class = dup_cstr(mc);
}

IFCAPI_EXPORT double ifcopenshell_util_unit_calculate_unit_scale(
    ifcopenshell_ifc_file_t* file_h, const char* unit_type)
{
    if (!file_h || !file_h->ptr) return 1.0;
    auto* file = file_h->ptr;
    std::string want = (unit_type && *unit_type) ? unit_type : "LENGTHUNIT";
    auto* unit = project_unit_for(file, want);
    if (!unit) return 1.0;
    return convert_value_for_unit(1.0, unit, /*to_si=*/true);
}

}  // extern "C"
