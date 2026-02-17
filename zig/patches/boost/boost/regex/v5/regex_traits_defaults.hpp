#ifndef IFCOPENSHELL_BOOST_REGEX_TRAITS_DEFAULTS_V5_PATCH_HPP
#define IFCOPENSHELL_BOOST_REGEX_TRAITS_DEFAULTS_V5_PATCH_HPP

#include_next <boost/regex/v5/regex_traits_defaults.hpp>
#include <cstddef>
#include <string>

namespace boost {
namespace BOOST_REGEX_DETAIL_NS {

inline bool ifcopenshell_regex_eq_v5(
    const char* p1,
    const char* p2,
    const char* lit
) {
    const std::size_t n = static_cast<std::size_t>(p2 - p1);
    return std::char_traits<char>::length(lit) == n &&
        std::char_traits<char>::compare(p1, lit, n) == 0;
}

template <>
inline int get_default_class_id<char>(const char* p1, const char* p2) {
    if (ifcopenshell_regex_eq_v5(p1, p2, "alnum")) return 0;
    if (ifcopenshell_regex_eq_v5(p1, p2, "alpha")) return 1;
    if (ifcopenshell_regex_eq_v5(p1, p2, "blank")) return 2;
    if (ifcopenshell_regex_eq_v5(p1, p2, "cntrl")) return 3;
    if (ifcopenshell_regex_eq_v5(p1, p2, "d")) return 4;
    if (ifcopenshell_regex_eq_v5(p1, p2, "digit")) return 5;
    if (ifcopenshell_regex_eq_v5(p1, p2, "graph")) return 6;
    if (ifcopenshell_regex_eq_v5(p1, p2, "h")) return 7;
    if (ifcopenshell_regex_eq_v5(p1, p2, "l")) return 8;
    if (ifcopenshell_regex_eq_v5(p1, p2, "lower")) return 9;
    if (ifcopenshell_regex_eq_v5(p1, p2, "print")) return 10;
    if (ifcopenshell_regex_eq_v5(p1, p2, "punct")) return 11;
    if (ifcopenshell_regex_eq_v5(p1, p2, "s")) return 12;
    if (ifcopenshell_regex_eq_v5(p1, p2, "space")) return 13;
    if (ifcopenshell_regex_eq_v5(p1, p2, "u")) return 14;
    if (ifcopenshell_regex_eq_v5(p1, p2, "unicode")) return 15;
    if (ifcopenshell_regex_eq_v5(p1, p2, "upper")) return 16;
    if (ifcopenshell_regex_eq_v5(p1, p2, "v")) return 17;
    if (ifcopenshell_regex_eq_v5(p1, p2, "w")) return 18;
    if (ifcopenshell_regex_eq_v5(p1, p2, "word")) return 19;
    if (ifcopenshell_regex_eq_v5(p1, p2, "xdigit")) return 20;
    return -1;
}

} // namespace BOOST_REGEX_DETAIL_NS
} // namespace boost

#endif // IFCOPENSHELL_BOOST_REGEX_TRAITS_DEFAULTS_V5_PATCH_HPP
