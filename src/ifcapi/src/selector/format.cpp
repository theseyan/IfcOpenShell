// SPDX-License-Identifier: LGPL-3.0-or-later

#include "selector/internal.h"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <numeric>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

/* ====================================================================
 *  String / numeric helpers
 * ==================================================================== */

inline std::string strip_ws(const std::string& s) {
    size_t a = 0, b = s.size();
    while (a < b && std::isspace((unsigned char)s[a])) ++a;
    while (b > a && std::isspace((unsigned char)s[b - 1])) --b;
    return s.substr(a, b - a);
}

inline std::string unescape_quoted_token(const std::string& raw) {
    if (raw.size() < 2) return "";
    std::string inner = raw.substr(1, raw.size() - 2);
    std::string out;
    out.reserve(inner.size());
    for (char c : inner) {
        if (c == '\\') continue;
        out += c;
    }
    return out;
}

inline bool parse_double(const std::string& s, double& out) {
    if (s.empty()) return false;
    char* end = nullptr;
    out = std::strtod(s.c_str(), &end);
    return end != s.c_str();
}

inline std::string format_double_python(double d) {
    if (std::isnan(d)) return "nan";
    if (std::isinf(d)) return d < 0 ? "-inf" : "inf";
    char buf[64];
    std::snprintf(buf, sizeof(buf), "%g", d);
    std::string s = buf;
    bool has_dot_or_exp = false;
    for (char c : s) if (c == '.' || c == 'e' || c == 'E') { has_dot_or_exp = true; break; }
    if (!has_dot_or_exp) s += ".0";
    return s;
}

/* Stringify the result of an arithmetic op:
   - integer-valued double → "<int>"
   - otherwise            → format_double_python */
inline std::string format_arith_result(double d) {
    if (std::isfinite(d) && std::floor(d) == d &&
        d >= -1e18 && d <= 1e18) {
        std::ostringstream os;
        os << static_cast<long long>(d);
        return os.str();
    }
    return format_double_python(d);
}

/* Convert a Val to a string for use as input to a string-taking function
   (upper, concat, etc.). */
std::string val_to_string(const Val* v);

inline std::string list_join(const Val* lst, const std::string& sep) {
    std::string out;
    bool first = true;
    for (auto* item : lst->list_val) {
        if (!first) out += sep;
        first = false;
        out += val_to_string(item);
    }
    return out;
}

std::string val_to_string(const Val* v) {
    if (!v) return "None";
    switch (v->kind) {
        case IFCSEL_VALUE_NONE:   return "None";
        case IFCSEL_VALUE_BOOL:   return v->b_val ? "True" : "False";
        case IFCSEL_VALUE_INT: {
            std::ostringstream os; os << v->i_val; return os.str();
        }
        case IFCSEL_VALUE_DOUBLE: return format_double_python(v->d_val);
        case IFCSEL_VALUE_STRING: return v->s_val;
        case IFCSEL_VALUE_INSTANCE: {
            if (v->inst_val) {
                std::ostringstream os;
                os << "#" << v->inst_val->id() << "=" << v->inst_val->declaration().name();
                return os.str();
            }
            return "None";
        }
        case IFCSEL_VALUE_LIST:   return list_join(v, ", ");
        case IFCSEL_VALUE_DICT: {
            std::string out = "{";
            bool first = true;
            for (auto& kv : v->dict_val) {
                if (!first) out += ", ";
                first = false;
                out += kv.first + ": " + val_to_string(kv.second);
            }
            out += "}";
            return out;
        }
    }
    return "";
}

/* Try to coerce a Val to a number for arithmetic. None / empty → 0. */
inline double val_to_number(const Val* v, double zero_if_none = 0.0) {
    if (!v || v->kind == IFCSEL_VALUE_NONE) return zero_if_none;
    switch (v->kind) {
        case IFCSEL_VALUE_BOOL:   return v->b_val ? 1.0 : 0.0;
        case IFCSEL_VALUE_INT:    return static_cast<double>(v->i_val);
        case IFCSEL_VALUE_DOUBLE: return v->d_val;
        case IFCSEL_VALUE_STRING: {
            if (v->s_val.empty() || v->s_val == "None") return zero_if_none;
            double d;
            if (parse_double(v->s_val, d)) return d;
            return zero_if_none;
        }
        default: return zero_if_none;
    }
}

inline std::string token_text(const ifcopenshell_selector_node_t* n) {
    const char* t = ifcopenshell_selector_node_text(n);
    return t ? std::string(t) : std::string();
}

/* ====================================================================
 *  format_length port (metric & imperial)
 * ==================================================================== */

inline std::string format_metric(double value, double precision, int decimal_places) {
    double rounded = std::round(value / precision) * precision;
    char buf[64];
    if (decimal_places < 0) decimal_places = 0;
    std::snprintf(buf, sizeof(buf), "%.*f", decimal_places, rounded);
    return std::string(buf);
}

inline std::string format_imperial(double value, int precision,
                                   const std::string& input_unit,
                                   const std::string& output_unit,
                                   bool suppress_zero_inches) {
    long long feet = 0;
    double inches = 0.0;
    if (input_unit == "inch") {
        inches = std::fmod(value, 12.0);
        feet   = static_cast<long long>(std::llround((value - inches) / 12.0));
    } else {
        feet   = static_cast<long long>(value); // truncate toward zero
        inches = (value - static_cast<double>(feet)) * 12.0;
    }

    long long nearest = static_cast<long long>(std::llround(inches * static_cast<double>(precision)));

    long long num = nearest;
    long long den = precision;
    if (den < 0) { num = -num; den = -den; }
    long long g = std::gcd(num < 0 ? -num : num, den);
    if (g > 0) { num /= g; den /= g; }

    auto fmt = [&](const std::string& core) {
        return core;
    };

    if (den == 1) {
        if (suppress_zero_inches && num == 0) {
            std::ostringstream os;
            if (output_unit == "foot") os << feet << "'";
            else                       os << (feet * 12) << "\"";
            return os.str();
        }
        std::ostringstream os;
        if (output_unit == "foot") os << feet << "' - " << num << "\"";
        else                       os << ((feet * 12) + num) << "\"";
        return os.str();
    }

    if (num > den) {
        long long remainder = num % den;
        long long whole = (num - remainder) / den;
        std::ostringstream os;
        if (output_unit == "foot")
            os << feet << "' - " << whole << " " << remainder << "/" << den << "\"";
        else
            os << ((feet * 12) + whole) << " " << remainder << "/" << den << "\"";
        return os.str();
    }

    std::ostringstream os;
    if (output_unit == "foot")
        os << feet << "' - 0 " << num << "/" << den << "\"";
    else
        os << (feet * 12) << " " << num << "/" << den << "\"";
    (void)fmt;
    return os.str();
}

/* ====================================================================
 *  number() formatter
 * ==================================================================== */

inline std::string with_thousands(const std::string& integral_part) {
    /* Inserts ',' every 3 digits (Python's "{:,}"). */
    std::string sign;
    std::string digits = integral_part;
    if (!digits.empty() && (digits[0] == '-' || digits[0] == '+')) {
        sign = digits.substr(0, 1);
        digits = digits.substr(1);
    }
    std::string out;
    int count = 0;
    for (auto it = digits.rbegin(); it != digits.rend(); ++it) {
        if (count && count % 3 == 0) out.push_back(',');
        out.push_back(*it);
        ++count;
    }
    std::reverse(out.begin(), out.end());
    return sign + out;
}

inline std::string format_number_default(double d, bool is_int_input) {
    /* Mirrors Python's "{:,}".format(value).
       - int: thousand-separated integer
       - float: thousand-separated integer part, default %g-ish fraction. */
    if (is_int_input || (std::isfinite(d) && std::floor(d) == d)) {
        long long iv = static_cast<long long>(d);
        std::ostringstream os; os << iv;
        return with_thousands(os.str());
    }
    /* Match {:,} formatting: 6 significant fraction digits, trim trailing zeros.
       For test parity: 1234.56 → "1,234.56" */
    char buf[64];
    std::snprintf(buf, sizeof(buf), "%.12g", d);
    std::string s = buf;
    auto dot = s.find('.');
    if (dot == std::string::npos) {
        return with_thousands(s);
    }
    std::string ipart = s.substr(0, dot);
    std::string fpart = s.substr(dot + 1);
    return with_thousands(ipart) + "." + fpart;
}

inline std::string apply_decimal_thousand(const std::string& s,
                                          const std::string& decimal_sep,
                                          const std::string& thousand_sep) {
    /* Python: "{:,}".format(x).replace(".", "*").replace(",", thousand).replace("*", decimal) */
    std::string r;
    r.reserve(s.size());
    for (char c : s) {
        if (c == '.') r += "*";
        else r += c;
    }
    std::string r2;
    r2.reserve(r.size());
    for (char c : r) {
        if (c == ',') r2 += thousand_sep;
        else r2 += c;
    }
    std::string out;
    out.reserve(r2.size());
    for (size_t i = 0; i < r2.size(); ++i) {
        if (r2[i] == '*') out += decimal_sep;
        else out += r2[i];
    }
    return out;
}

/* ====================================================================
 *  Format AST evaluation
 * ==================================================================== */

class FormatEvaluator {
public:
    FormatEvaluator(IfcParse::IfcFile* file, IfcUtil::IfcBaseClass* element)
        : file_(file), element_(element) {}

    Val* eval(const ifcopenshell_selector_node_t* node) {
        if (!node) return make_none();
        ifcsel_node_kind k = ifcopenshell_selector_node_kind(node);

        switch (k) {
            case IFCSEL_NODE_START: {
                if (ifcopenshell_selector_node_child_count(node) == 0) return make_none();
                return eval(ifcopenshell_selector_node_child(node, 0));
            }

            case IFCSEL_NODE_FUNCTION: {
                if (ifcopenshell_selector_node_child_count(node) == 0) return make_none();
                return eval(ifcopenshell_selector_node_child(node, 0));
            }

            case IFCSEL_NODE_ADD:
            case IFCSEL_NODE_SUBTRACT:
            case IFCSEL_NODE_MULTIPLY:
            case IFCSEL_NODE_DIVIDE:
                return eval_binop(node, k);

            case IFCSEL_NODE_VARIABLE:
                return eval_variable(node);

            case IFCSEL_NODE_ROUND:           return eval_round(node);
            case IFCSEL_NODE_NUMBER_FN:       return eval_number(node);
            case IFCSEL_NODE_INT_FN:          return eval_int(node);
            case IFCSEL_NODE_FORMAT_LENGTH: {
                if (ifcopenshell_selector_node_child_count(node) == 0) return make_none();
                return eval(ifcopenshell_selector_node_child(node, 0));
            }
            case IFCSEL_NODE_METRIC_LENGTH:   return eval_metric_length(node);
            case IFCSEL_NODE_IMPERIAL_LENGTH: return eval_imperial_length(node);
            case IFCSEL_NODE_LOWER:           return eval_unary_str(node, [](std::string s){
                std::transform(s.begin(), s.end(), s.begin(),
                               [](unsigned char c){ return std::tolower(c); });
                return s;
            });
            case IFCSEL_NODE_UPPER:           return eval_unary_str(node, [](std::string s){
                std::transform(s.begin(), s.end(), s.begin(),
                               [](unsigned char c){ return std::toupper(c); });
                return s;
            });
            case IFCSEL_NODE_TITLE:           return eval_title(node);
            case IFCSEL_NODE_CONCAT:          return eval_concat(node);
            case IFCSEL_NODE_SUBSTR:          return eval_substr(node);
            case IFCSEL_NODE_SORT:            return eval_sort(node);
            case IFCSEL_NODE_REVERSE:         return eval_reverse(node);
            case IFCSEL_NODE_JOIN:            return eval_join(node);
            case IFCSEL_NODE_BOOLEAN:         return eval_boolean(node);

            case IFCSEL_TOKEN_ESCAPED_STRING:
                return make_string(unescape_quoted_token(token_text(node)));

            case IFCSEL_TOKEN_SIGNED_NUMBER:
            case IFCSEL_TOKEN_NUMBER: {
                std::string txt = token_text(node);
                if (txt.find('.') == std::string::npos &&
                    txt.find('e') == std::string::npos &&
                    txt.find('E') == std::string::npos) {
                    try { return make_string(txt); }
                    catch (...) { return make_string(txt); }
                }
                return make_string(txt);
            }
            case IFCSEL_TOKEN_SIGNED_INT:
                return make_string(token_text(node));

            case IFCSEL_TOKEN_TRUE:  return make_bool(true);
            case IFCSEL_TOKEN_FALSE: return make_bool(false);

            default:
                return make_none();
        }
    }

    /* Top-level conversion to output string. Returns empty optional for None. */
    bool to_output(const ifcopenshell_selector_node_t* root, std::string& out) {
        Val* v = eval(root);
        if (!v) { return false; }
        if (v->kind == IFCSEL_VALUE_NONE) { delete v; return false; }
        if (v->kind == IFCSEL_VALUE_LIST) {
            out = list_join(v, ", ");
        } else {
            out = val_to_string(v);
        }
        delete v;
        return true;
    }

private:
    IfcParse::IfcFile* file_ = nullptr;
    IfcUtil::IfcBaseClass* element_ = nullptr;

    /* ---- helpers ---- */

    Val* eval_binop(const ifcopenshell_selector_node_t* node, ifcsel_node_kind k) {
        if (ifcopenshell_selector_node_child_count(node) < 2) return make_none();
        Val* L = eval(ifcopenshell_selector_node_child(node, 0));
        Val* R = eval(ifcopenshell_selector_node_child(node, 1));

        if (k == IFCSEL_NODE_ADD) {
            /* Python's add catches conversion errors and falls back to
               str(left)+str(right). Implement that fallback. */
            bool ok = true;
            double l = 0.0, r = 0.0;
            try {
                l = val_to_number(L);
                r = val_to_number(R);
            } catch (...) { ok = false; }
            if (!ok) {
                std::string s = val_to_string(L) + val_to_string(R);
                delete L; delete R;
                return make_string(s);
            }
            delete L; delete R;
            return make_string(format_arith_result(l + r));
        }

        double l = val_to_number(L);
        double r = val_to_number(R);
        delete L; delete R;
        double result = 0.0;
        switch (k) {
            case IFCSEL_NODE_SUBTRACT: result = l - r; break;
            case IFCSEL_NODE_MULTIPLY: result = l * r; break;
            case IFCSEL_NODE_DIVIDE:
                if (r == 0.0) return make_string("inf");
                result = l / r;
                break;
            default: break;
        }
        return make_string(format_arith_result(result));
    }

    Val* eval_variable(const ifcopenshell_selector_node_t* node) {
        /* VARIABLE → QUERY_PATH → ANON token containing inner query text. */
        if (!element_) return make_none();
        if (ifcopenshell_selector_node_child_count(node) == 0) return make_none();
        auto* qp = ifcopenshell_selector_node_child(node, 0);
        if (!qp || ifcopenshell_selector_node_child_count(qp) == 0) return make_none();
        auto* tok = ifcopenshell_selector_node_child(qp, 0);
        std::string query = strip_ws(token_text(tok));
        if (query.empty()) return make_none();

        ifcopenshell_selector_node_t* ast = ifcopenshell_selector_parse_get_element(query.c_str());
        if (!ast) return make_none();
        std::vector<KeyEntry> keys = extract_keys(ast);
        ifcopenshell_selector_node_free(ast);

        try {
            return get_element_value_impl(file_, element_, keys);
        } catch (...) {
            return make_none();
        }
    }

    template<typename Fn>
    Val* eval_unary_str(const ifcopenshell_selector_node_t* node, Fn fn) {
        if (ifcopenshell_selector_node_child_count(node) == 0) return make_string(fn(std::string()));
        Val* v = eval(ifcopenshell_selector_node_child(node, 0));
        std::string s = val_to_string(v);
        delete v;
        return make_string(fn(std::move(s)));
    }

    Val* eval_title(const ifcopenshell_selector_node_t* node) {
        return eval_unary_str(node, [](std::string s){
            /* Python's str.title(): every word boundary capitalised. */
            bool prev_alpha = false;
            for (auto& c : s) {
                unsigned char uc = static_cast<unsigned char>(c);
                if (std::isalpha(uc)) {
                    c = prev_alpha ? static_cast<char>(std::tolower(uc))
                                   : static_cast<char>(std::toupper(uc));
                    prev_alpha = true;
                } else {
                    prev_alpha = false;
                }
            }
            return s;
        });
    }

    Val* eval_concat(const ifcopenshell_selector_node_t* node) {
        std::string out;
        size_t n = ifcopenshell_selector_node_child_count(node);
        for (size_t i = 0; i < n; ++i) {
            Val* v = eval(ifcopenshell_selector_node_child(node, i));
            out += val_to_string(v);
            delete v;
        }
        return make_string(out);
    }

    Val* eval_substr(const ifcopenshell_selector_node_t* node) {
        size_t nc = ifcopenshell_selector_node_child_count(node);
        if (nc < 2) return make_none();
        Val* sv = eval(ifcopenshell_selector_node_child(node, 0));
        std::string s = val_to_string(sv);
        delete sv;

        long long start_i = std::strtoll(token_text(ifcopenshell_selector_node_child(node, 1)).c_str(), nullptr, 10);

        long long len = static_cast<long long>(s.size());
        long long si = start_i;
        if (si < 0) si = std::max<long long>(0, len + si);
        if (si > len) si = len;

        if (nc >= 3) {
            long long end_i = std::strtoll(token_text(ifcopenshell_selector_node_child(node, 2)).c_str(), nullptr, 10);
            long long ei = end_i;
            if (ei < 0) ei = std::max<long long>(0, len + ei);
            if (ei > len) ei = len;
            if (ei < si) ei = si;
            return make_string(s.substr(static_cast<size_t>(si),
                                        static_cast<size_t>(ei - si)));
        }
        return make_string(s.substr(static_cast<size_t>(si)));
    }

    Val* eval_sort(const ifcopenshell_selector_node_t* node) {
        if (ifcopenshell_selector_node_child_count(node) == 0) return make_list();
        Val* v = eval(ifcopenshell_selector_node_child(node, 0));
        if (!v || v->kind != IFCSEL_VALUE_LIST) {
            return v ? v : make_none();
        }
        std::sort(v->list_val.begin(), v->list_val.end(),
                  [](const Val* a, const Val* b){
                      return val_to_string(a) < val_to_string(b);
                  });
        return v;
    }

    Val* eval_reverse(const ifcopenshell_selector_node_t* node) {
        if (ifcopenshell_selector_node_child_count(node) == 0) return make_list();
        Val* v = eval(ifcopenshell_selector_node_child(node, 0));
        if (!v || v->kind != IFCSEL_VALUE_LIST) {
            return v ? v : make_none();
        }
        std::reverse(v->list_val.begin(), v->list_val.end());
        return v;
    }

    Val* eval_join(const ifcopenshell_selector_node_t* node) {
        size_t nc = ifcopenshell_selector_node_child_count(node);
        if (nc < 2) return make_string("");
        std::string sep = unescape_quoted_token(token_text(ifcopenshell_selector_node_child(node, 0)));
        Val* v = eval(ifcopenshell_selector_node_child(node, 1));
        std::string out;
        if (v && v->kind == IFCSEL_VALUE_LIST) {
            out = list_join(v, sep);
        } else {
            out = val_to_string(v);
        }
        delete v;
        return make_string(out);
    }

    Val* eval_boolean(const ifcopenshell_selector_node_t* node) {
        if (ifcopenshell_selector_node_child_count(node) == 0) return make_bool(true);
        auto* tok = ifcopenshell_selector_node_child(node, 0);
        if (ifcopenshell_selector_node_kind(tok) == IFCSEL_TOKEN_TRUE)  return make_bool(true);
        if (ifcopenshell_selector_node_kind(tok) == IFCSEL_TOKEN_FALSE) return make_bool(false);
        std::string txt = token_text(tok);
        std::string low; low.reserve(txt.size());
        for (char c : txt) low.push_back(static_cast<char>(std::tolower((unsigned char)c)));
        return make_bool(low == "true" || low == "1" || low == "yes");
    }

    Val* eval_round(const ifcopenshell_selector_node_t* node) {
        if (ifcopenshell_selector_node_child_count(node) < 2) return make_none();
        Val* val_v = eval(ifcopenshell_selector_node_child(node, 0));
        double value = val_to_number(val_v);
        delete val_v;

        std::string nearest_txt = token_text(ifcopenshell_selector_node_child(node, 1));
        double nearest = 0.0;
        if (!parse_double(nearest_txt, nearest) || nearest == 0.0) return make_string("0");

        double result = std::round(value / nearest) * nearest;

        bool nearest_is_int = (nearest_txt.find('.') == std::string::npos &&
                               nearest_txt.find('e') == std::string::npos &&
                               nearest_txt.find('E') == std::string::npos);
        if (nearest_is_int) {
            std::ostringstream os; os << static_cast<long long>(std::llround(result));
            return make_string(os.str());
        }
        return make_string(format_double_python(result));
    }

    Val* eval_number(const ifcopenshell_selector_node_t* node) {
        size_t nc = ifcopenshell_selector_node_child_count(node);
        if (nc == 0) return make_string("0");
        Val* val_v = eval(ifcopenshell_selector_node_child(node, 0));

        bool is_int_input = false;
        if (val_v && val_v->kind == IFCSEL_VALUE_INT) is_int_input = true;
        else if (val_v && val_v->kind == IFCSEL_VALUE_STRING) {
            const std::string& s = val_v->s_val;
            is_int_input = !s.empty() && s.find('.') == std::string::npos &&
                            s.find('e') == std::string::npos && s.find('E') == std::string::npos;
        }
        double d = val_to_number(val_v);
        delete val_v;

        std::string base = format_number_default(d, is_int_input);

        std::string decimal_sep, thousand_sep;
        if (nc >= 2) decimal_sep  = unescape_quoted_token(token_text(ifcopenshell_selector_node_child(node, 1)));
        if (nc >= 3) thousand_sep = unescape_quoted_token(token_text(ifcopenshell_selector_node_child(node, 2)));

        if (!thousand_sep.empty()) {
            return make_string(apply_decimal_thousand(base, decimal_sep, thousand_sep));
        }
        if (!decimal_sep.empty()) {
            /* Python: replace "." with decimal sep; thousand stays "," from {:,}.
               But the original Python branch when only decimal sep is provided
               uses "{}".format(arg_val).replace(".", decimal) — no thousands. */
            std::ostringstream os;
            if (is_int_input || (std::isfinite(d) && std::floor(d) == d)) {
                os << static_cast<long long>(d);
            } else {
                os << format_double_python(d);
            }
            std::string s = os.str();
            std::string out;
            for (char c : s) {
                if (c == '.') out += decimal_sep;
                else out.push_back(c);
            }
            return make_string(out);
        }
        return make_string(base);
    }

    Val* eval_int(const ifcopenshell_selector_node_t* node) {
        if (ifcopenshell_selector_node_child_count(node) == 0) return make_string("0");
        Val* v = eval(ifcopenshell_selector_node_child(node, 0));
        double d = val_to_number(v);
        delete v;
        std::ostringstream os;
        os << static_cast<long long>(d); /* truncate toward zero */
        return make_string(os.str());
    }

    Val* eval_metric_length(const ifcopenshell_selector_node_t* node) {
        if (ifcopenshell_selector_node_child_count(node) < 3) return make_none();
        Val* v = eval(ifcopenshell_selector_node_child(node, 0));
        double value = val_to_number(v);
        delete v;
        double precision = 0.0;
        parse_double(token_text(ifcopenshell_selector_node_child(node, 1)), precision);
        if (precision == 0.0) precision = 1.0;
        int decimal_places = std::atoi(token_text(ifcopenshell_selector_node_child(node, 2)).c_str());
        return make_string(format_metric(value, precision, decimal_places));
    }

    Val* eval_imperial_length(const ifcopenshell_selector_node_t* node) {
        size_t nc = ifcopenshell_selector_node_child_count(node);
        if (nc < 2) return make_none();
        Val* v = eval(ifcopenshell_selector_node_child(node, 0));
        double value = val_to_number(v);
        delete v;
        int precision = std::atoi(token_text(ifcopenshell_selector_node_child(node, 1)).c_str());
        if (precision == 0) precision = 1;

        std::string input_unit = "foot";
        std::string output_unit = "foot";
        bool suppress_zero_inches = true;

        size_t idx = 2;
        if (nc > idx && ifcopenshell_selector_node_kind(ifcopenshell_selector_node_child(node, idx))
                        == IFCSEL_TOKEN_ESCAPED_STRING) {
            std::string in_u = unescape_quoted_token(token_text(ifcopenshell_selector_node_child(node, idx)));
            ++idx;
            std::string out_u;
            if (nc > idx && ifcopenshell_selector_node_kind(ifcopenshell_selector_node_child(node, idx))
                            == IFCSEL_TOKEN_ESCAPED_STRING) {
                out_u = unescape_quoted_token(token_text(ifcopenshell_selector_node_child(node, idx)));
                ++idx;
            }
            input_unit  = (in_u == "inch") ? "inch" : "foot";
            output_unit = (out_u == "inch") ? "inch" : "foot";
        }
        if (nc > idx) {
            auto* maybe_bool = ifcopenshell_selector_node_child(node, idx);
            if (ifcopenshell_selector_node_kind(maybe_bool) == IFCSEL_NODE_BOOLEAN) {
                Val* bv = eval(maybe_bool);
                suppress_zero_inches = bv && bv->kind == IFCSEL_VALUE_BOOL ? bv->b_val : true;
                delete bv;
            }
        }
        return make_string(format_imperial(value, precision, input_unit, output_unit, suppress_zero_inches));
    }
};

/* ====================================================================
 *  Key extraction (public ABI)
 * ==================================================================== */

struct KeysHandle {
    std::vector<KeyEntry> entries;
};

} /* anonymous namespace */

/* ====================================================================
 *  C ABI
 * ==================================================================== */

struct ifcopenshell_selector_keys_t : public KeysHandle {};

extern "C" {

char* ifcopenshell_selector_format(
    ifcopenshell_ifc_file_t* file,
    ifcopenshell_ifc_instance_t* instance,
    const char* query)
{
    if (!query) {
        ifcopenshell::capi::set_last_error("format: null query");
        return nullptr;
    }
    ifcopenshell_selector_node_t* ast = ifcopenshell_selector_parse_format(query);
    if (!ast) return nullptr;

    IfcParse::IfcFile* ifc_file = file ? file->ptr : nullptr;
    IfcUtil::IfcBaseClass* element = (instance && instance->ptr) ? instance->ptr : nullptr;

    std::string out;
    bool has_value = false;
    try {
        FormatEvaluator ev(ifc_file, element);
        has_value = ev.to_output(ast, out);
    } catch (const std::exception& ex) {
        ifcopenshell_selector_node_free(ast);
        ifcopenshell::capi::set_last_error(ex.what());
        return nullptr;
    } catch (...) {
        ifcopenshell_selector_node_free(ast);
        ifcopenshell::capi::set_last_error("format: unknown exception");
        return nullptr;
    }
    ifcopenshell_selector_node_free(ast);

    if (!has_value) {
        return nullptr;
    }
    char* buf = static_cast<char*>(std::malloc(out.size() + 1));
    if (!buf) {
        ifcopenshell::capi::set_last_error("format: out of memory");
        return nullptr;
    }
    std::memcpy(buf, out.data(), out.size());
    buf[out.size()] = '\0';
    return buf;
}

ifcopenshell_selector_keys_t* ifcopenshell_selector_parse_keys(const char* query) {
    if (!query) {
        ifcopenshell::capi::set_last_error("parse_keys: null query");
        return nullptr;
    }
    ifcopenshell_selector_node_t* ast = ifcopenshell_selector_parse_get_element(query);
    if (!ast) return nullptr;
    auto* h = new ifcopenshell_selector_keys_t();
    h->entries = extract_keys(ast);
    ifcopenshell_selector_node_free(ast);
    return h;
}

uint32_t ifcopenshell_selector_keys_count(ifcopenshell_selector_keys_t* h) {
    return h ? static_cast<uint32_t>(h->entries.size()) : 0;
}

const char* ifcopenshell_selector_keys_get(ifcopenshell_selector_keys_t* h, uint32_t i) {
    if (!h || i >= h->entries.size()) return nullptr;
    return h->entries[i].text.c_str();
}

bool ifcopenshell_selector_keys_is_regex(ifcopenshell_selector_keys_t* h, uint32_t i) {
    if (!h || i >= h->entries.size()) return false;
    return h->entries[i].is_regex;
}

void ifcopenshell_selector_keys_free(ifcopenshell_selector_keys_t* h) {
    delete h;
}

} /* extern "C" */
