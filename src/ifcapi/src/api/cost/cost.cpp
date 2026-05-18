// This file was generated with the assistance of an AI coding tool.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ifcapi/bindings/cost.h"

#include "ifcapi/bindings/entity.h"
#include "ifcapi/bindings/unit.h"
#include "ifcapi/detail/attribute.h"
#include "../pset/attribute_props.hpp"

#include "ifcopenshell_api_internal.hpp"

#include <cctype>
#include <cstdlib>
#include <stdexcept>
#include <utility>

namespace {

IfcUtil::IfcBaseClass* create_entity(IfcParse::IfcFile* file, const std::string& ifc_class) {
    return file->create(file->schema()->declaration_by_name(ifc_class));
}

double numeric_value(const ifcapi_pset::Entry& entry) {
    switch (entry.kind) {
        case ifcapi_pset::Kind::DOUBLE:
        case ifcapi_pset::Kind::TYPED_DOUBLE:
            return entry.d_val;
        case ifcapi_pset::Kind::INT:
        case ifcapi_pset::Kind::TYPED_INT:
            return static_cast<double>(entry.i_val);
        case ifcapi_pset::Kind::BOOL:
        case ifcapi_pset::Kind::TYPED_BOOL:
            return entry.b_val ? 1.0 : 0.0;
        default:
            throw std::runtime_error("Expected a numeric cost value");
    }
}

IfcUtil::IfcBaseClass* create_measure(IfcParse::IfcFile* file, const std::string& ifc_type, double value) {
    auto* result = ifcapi::detail::create_typed_double(file, ifc_type.c_str(), value);
    if (!result) {
        throw std::runtime_error("Unable to create " + ifc_type);
    }
    return result;
}

struct FormulaNode {
    bool has_applied_value = false;
    double applied_value = 0.0;
    bool applied_value_is_null = false;
    bool has_category = false;
    std::string category;
    bool has_operator = false;
    std::string arithmetic_operator;
    std::vector<FormulaNode> components;
};

class FormulaParser {
public:
    explicit FormulaParser(std::string text) : text_(std::move(text)) {}

    bool parse(FormulaNode& out) {
        skip_ws();
        if (pos_ == text_.size()) {
            return false;
        }
        if (!parse_formula(out)) {
            return false;
        }
        skip_ws();
        return pos_ == text_.size();
    }

private:
    bool parse_formula(FormulaNode& out) {
        FormulaNode first;
        if (!parse_operand(first)) {
            return false;
        }
        std::vector<FormulaNode> components;
        components.push_back(std::move(first));
        std::string last_operator;
        while (true) {
            skip_ws();
            std::string op;
            if (!parse_operator(op)) {
                break;
            }
            FormulaNode operand;
            if (!parse_operand(operand)) {
                return false;
            }
            last_operator = op;
            components.push_back(std::move(operand));
        }
        if (components.size() == 1) {
            out = std::move(components.front());
        } else {
            out.components = std::move(components);
            out.has_operator = true;
            out.arithmetic_operator = last_operator;
        }
        return true;
    }

    bool parse_operand(FormulaNode& out) {
        skip_ws();
        const size_t mark = pos_;
        std::string category;
        parse_word(category);
        skip_ws();
        if (pos_ < text_.size() && text_[pos_] == '(') {
            ++pos_;
            FormulaNode formula;
            if (!parse_formula(formula)) {
                return false;
            }
            skip_ws();
            if (pos_ >= text_.size() || text_[pos_] != ')') {
                return false;
            }
            ++pos_;
            if (!category.empty()) {
                out.has_category = true;
                out.category = lower(category) == "sum" ? "*" : category;
            }
            if (!formula.components.empty()) {
                out.components = std::move(formula.components);
                out.has_operator = formula.has_operator;
                out.arithmetic_operator = formula.arithmetic_operator;
            } else {
                out.has_applied_value = true;
                out.applied_value_is_null = formula.applied_value_is_null;
                out.applied_value = formula.applied_value;
            }
            return true;
        }
        pos_ = mark;
        return parse_value(out);
    }

    bool parse_value(FormulaNode& out) {
        skip_ws();
        const size_t start = pos_;
        bool saw_digit = false;
        while (pos_ < text_.size() && std::isdigit(static_cast<unsigned char>(text_[pos_]))) {
            saw_digit = true;
            ++pos_;
        }
        if (pos_ < text_.size() && text_[pos_] == '.') {
            ++pos_;
            while (pos_ < text_.size() && std::isdigit(static_cast<unsigned char>(text_[pos_]))) {
                saw_digit = true;
                ++pos_;
            }
        }
        if (saw_digit && pos_ < text_.size() && (text_[pos_] == 'e' || text_[pos_] == 'E')) {
            const size_t exp = pos_++;
            if (pos_ < text_.size() && (text_[pos_] == '+' || text_[pos_] == '-')) {
                ++pos_;
            }
            bool exp_digit = false;
            while (pos_ < text_.size() && std::isdigit(static_cast<unsigned char>(text_[pos_]))) {
                exp_digit = true;
                ++pos_;
            }
            if (!exp_digit) {
                pos_ = exp;
            }
        }
        out.has_applied_value = true;
        if (!saw_digit) {
            out.applied_value_is_null = true;
            return true;
        }
        out.applied_value = std::strtod(text_.substr(start, pos_ - start).c_str(), nullptr);
        return true;
    }

    bool parse_operator(std::string& op) {
        if (pos_ >= text_.size()) {
            return false;
        }
        switch (text_[pos_]) {
            case '+': op = "ADD"; break;
            case '/': op = "DIVIDE"; break;
            case '*': op = "MULTIPLY"; break;
            case '-': op = "SUBTRACT"; break;
            default: return false;
        }
        ++pos_;
        return true;
    }

    void parse_word(std::string& word) {
        while (pos_ < text_.size() && std::isalpha(static_cast<unsigned char>(text_[pos_]))) {
            word.push_back(text_[pos_++]);
        }
    }

    void skip_ws() {
        while (pos_ < text_.size() && std::isspace(static_cast<unsigned char>(text_[pos_]))) {
            ++pos_;
        }
    }

    static std::string lower(std::string value) {
        for (auto& ch : value) {
            ch = static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
        }
        return value;
    }

    std::string text_;
    size_t pos_ = 0;
};

IfcUtil::IfcBaseClass* add_cost_value(IfcParse::IfcFile* file, IfcUtil::IfcBaseClass* parent) {
    auto* value = create_entity(file, "IfcCostValue");
    if (!parent) {
        return value;
    }
    if (parent->declaration().is("IfcCostItem")) {
        auto values = ifcapi::detail::read_ref_aggregate(parent, "CostValues");
        values.push_back(value);
        ifcapi::detail::write_ref_aggregate(parent, "CostValues", values);
    } else if (parent->declaration().is("IfcConstructionResource")) {
        auto values = ifcapi::detail::read_ref_aggregate(parent, "BaseCosts");
        values.push_back(value);
        ifcapi::detail::write_ref_aggregate(parent, "BaseCosts", values);
    } else if (parent->declaration().is("IfcCostValue")) {
        auto values = ifcapi::detail::read_ref_aggregate(parent, "Components");
        values.push_back(value);
        ifcapi::detail::write_ref_aggregate(parent, "Components", values);
    }
    return value;
}

void apply_formula_node(IfcParse::IfcFile* file, IfcUtil::IfcBaseClass* cost_value, const FormulaNode& node) {
    if (node.has_applied_value) {
        if (node.applied_value_is_null) {
            ifcapi::detail::clear_attr(cost_value, "AppliedValue");
        } else {
            ifcapi::detail::write_ref_attr(cost_value, "AppliedValue", create_measure(file, "IfcMonetaryMeasure", node.applied_value));
        }
    }
    if (node.has_category) {
        ifcapi::detail::write_string_attr(cost_value, "Category", node.category);
    } else {
        ifcapi::detail::clear_attr(cost_value, "Category");
    }
    if (node.has_operator) {
        ifcapi::detail::write_string_attr(cost_value, "ArithmeticOperator", node.arithmetic_operator);
    } else {
        ifcapi::detail::clear_attr(cost_value, "ArithmeticOperator");
    }
    if (!node.components.empty()) {
        auto existing = ifcapi::detail::read_ref_aggregate(cost_value, "Components");
        for (size_t i = 0; i < node.components.size(); ++i) {
            auto* component = i < existing.size() && existing[i] ? existing[i] : add_cost_value(file, cost_value);
            apply_formula_node(file, component, node.components[i]);
        }
    }
}

} // namespace

namespace ifcapi {
namespace bindings {

void cost_edit_cost_value(
    IfcParse::IfcFile* file,
    IfcUtil::IfcBaseClass* cost_value,
    ifcopenshell_pset_props_t* attributes,
    bool has_unit_basis,
    bool unit_basis_is_null,
    double value_component,
    IfcUtil::IfcBaseClass* unit_component)
{
    ifcopenshell_clear_error();
    try {
        if (!file || !cost_value) {
            throw std::runtime_error("cost_edit_cost_value requires a file and cost value");
        }
        if (attributes) {
            for (const auto& entry : attributes->entries) {
                if (entry.key == "AppliedValue" && entry.kind != ifcapi_pset::Kind::NONE) {
                    ifcapi::detail::write_ref_attr(
                        cost_value,
                        "AppliedValue",
                        create_measure(file, "IfcMonetaryMeasure", numeric_value(entry)));
                } else {
                    ifcapi::detail::apply_attribute_prop(cost_value, entry);
                }
            }
        }
        if (has_unit_basis) {
            auto* old_unit_basis = ifcapi::detail::read_ref_attr(cost_value, "UnitBasis");
            IfcUtil::IfcBaseClass* new_unit_basis = nullptr;
            if (!unit_basis_is_null) {
                if (!unit_component) {
                    throw std::runtime_error("UnitBasis requires a UnitComponent");
                }
                auto unit_type = ifcapi::detail::read_string_attr(unit_component, "UnitType");
                auto measure_class = unit_get_measure_class(unit_type);
                auto* measure = create_measure(file, measure_class, value_component);
                new_unit_basis = create_entity(file, "IfcMeasureWithUnit");
                ifcapi::detail::write_ref_attr(new_unit_basis, "ValueComponent", measure);
                ifcapi::detail::write_ref_attr(new_unit_basis, "UnitComponent", unit_component);
            }
            if (old_unit_basis) {
                entity_remove_deep2(old_unit_basis);
            }
            ifcapi::detail::write_ref_attr(cost_value, "UnitBasis", new_unit_basis);
        }
    } catch (const std::exception& e) {
        ifcopenshell::capi::set_last_error(e.what());
    }
}

void cost_edit_cost_value_formula(IfcParse::IfcFile* file, IfcUtil::IfcBaseClass* cost_value, const std::string& formula) {
    ifcopenshell_clear_error();
    try {
        if (!file || !cost_value) {
            throw std::runtime_error("cost_edit_cost_value_formula requires a file and cost value");
        }
        FormulaNode root;
        if (!FormulaParser(formula).parse(root)) {
            return;
        }
        apply_formula_node(file, cost_value, root);
    } catch (const std::exception& e) {
        ifcopenshell::capi::set_last_error(e.what());
    }
}

} // namespace bindings
} // namespace ifcapi
