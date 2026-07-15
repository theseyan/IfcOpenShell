// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef IFCAPI_DETAIL_UNIT_H
#define IFCAPI_DETAIL_UNIT_H

#include <array>
#include <cmath>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace ifcapi {
namespace detail {
namespace unit {

inline const std::vector<std::pair<std::string, std::string>>& imperial_types() {
    static const std::vector<std::pair<std::string, std::string>> values = {
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
    return values;
}

inline const std::unordered_map<std::string, double>& si_conversions() {
    static const std::unordered_map<std::string, double> values = {
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
        {"degree", std::acos(-1.0) / 180.0},
        {"ounce", 0.02835}, {"pound", 0.454},
        {"ton UK", 1016.0469088}, {"ton US", 907.18474}, {"tonne", 1000.0},
        {"lbf", 4.4482216153}, {"kip", 4448.2216153},
        {"psi", 6894.7572932}, {"ksi", 6894757.2932},
        {"minute", 60}, {"hour", 3600}, {"day", 86400},
        {"btu", 1055.056}, {"fahrenheit", 1.8},
    };
    return values;
}

inline const std::unordered_map<std::string, double>& si_offsets() {
    static const std::unordered_map<std::string, double> values = {{"fahrenheit", -459.67}};
    return values;
}

inline const std::unordered_map<std::string, std::array<int, 7>>& named_dimensions() {
    static const std::unordered_map<std::string, std::array<int, 7>> values = {
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
    return values;
}

inline const std::unordered_map<std::string, std::string>& si_type_names() {
    static const std::unordered_map<std::string, std::string> values = {
        {"ABSORBEDDOSEUNIT", "GRAY"}, {"AMOUNTOFSUBSTANCEUNIT", "MOLE"},
        {"AREAUNIT", "SQUARE_METRE"}, {"DOSEEQUIVALENTUNIT", "SIEVERT"},
        {"ELECTRICCAPACITANCEUNIT", "FARAD"}, {"ELECTRICCHARGEUNIT", "COULOMB"},
        {"ELECTRICCONDUCTANCEUNIT", "SIEMENS"}, {"ELECTRICCURRENTUNIT", "AMPERE"},
        {"ELECTRICRESISTANCEUNIT", "OHM"}, {"ELECTRICVOLTAGEUNIT", "VOLT"},
        {"ENERGYUNIT", "JOULE"}, {"FORCEUNIT", "NEWTON"}, {"FREQUENCYUNIT", "HERTZ"},
        {"ILLUMINANCEUNIT", "LUX"}, {"INDUCTANCEUNIT", "HENRY"}, {"LENGTHUNIT", "METRE"},
        {"LUMINOUSFLUXUNIT", "LUMEN"}, {"LUMINOUSINTENSITYUNIT", "CANDELA"},
        {"MAGNETICFLUXDENSITYUNIT", "TESLA"}, {"MAGNETICFLUXUNIT", "WEBER"},
        {"MASSUNIT", "GRAM"}, {"PLANEANGLEUNIT", "RADIAN"}, {"POWERUNIT", "WATT"},
        {"PRESSUREUNIT", "PASCAL"}, {"RADIOACTIVITYUNIT", "BECQUEREL"},
        {"SOLIDANGLEUNIT", "STERADIAN"}, {"THERMODYNAMICTEMPERATUREUNIT", "KELVIN"},
        {"TIMEUNIT", "SECOND"}, {"VOLUMEUNIT", "CUBIC_METRE"}, {"USERDEFINED", "METRE"},
    };
    return values;
}

} // namespace unit
} // namespace detail
} // namespace ifcapi

#endif /* IFCAPI_DETAIL_UNIT_H */
