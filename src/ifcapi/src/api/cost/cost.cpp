// This file was generated with the assistance of an AI coding tool.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ifcapi/bindings/cost.h"

#include "ifcapi/bindings/control.h"
#include "ifcapi/bindings/entity.h"
#include "ifcapi/bindings/nest.h"
#include "ifcapi/bindings/root.h"
#include "ifcapi/bindings/sequence.h"
#include "ifcapi/bindings/unit.h"
#include "ifcapi/detail/attribute.h"
#include "ifcapi/detail/copy.h"
#include "ifcapi/detail/relationship.h"
#include "../pset/attribute_props.hpp"

#include "ifcopenshell_api_internal.hpp"

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <cmath>
#include <map>
#include <limits>
#include <set>
#include <stdexcept>
#include <utility>
#include <variant>

namespace {

express::Base create_entity(ifcopenshell::file* file, const std::string& ifc_class) {
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

express::Base create_measure(ifcopenshell::file* file, const std::string& ifc_type, double value) {
    auto result = ifcapi::detail::create_typed_double(file, ifc_type.c_str(), value);
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

std::string lower_string(std::string value) {
    for (auto& ch : value) {
        ch = static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
    }
    return value;
}

express::Base add_cost_value(ifcopenshell::file* file, express::Base parent) {
    auto value = create_entity(file, "IfcCostValue");
    if (!parent) {
        return value;
    }
    if (parent.declaration().is("IfcCostItem")) {
        auto values = ifcapi::detail::read_ref_aggregate(parent, "CostValues");
        values.push_back(value);
        ifcapi::detail::write_ref_aggregate(parent, "CostValues", values);
    } else if (parent.declaration().is("IfcConstructionResource")) {
        auto values = ifcapi::detail::read_ref_aggregate(parent, "BaseCosts");
        values.push_back(value);
        ifcapi::detail::write_ref_aggregate(parent, "BaseCosts", values);
    } else if (parent.declaration().is("IfcCostValue")) {
        auto values = ifcapi::detail::read_ref_aggregate(parent, "Components");
        values.push_back(value);
        ifcapi::detail::write_ref_aggregate(parent, "Components", values);
    }
    return value;
}

void apply_formula_node(ifcopenshell::file* file, express::Base cost_value, const FormulaNode& node) {
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
            auto component = i < existing.size() && existing[i] ? existing[i] : add_cost_value(file, cost_value);
            apply_formula_node(file, component, node.components[i]);
        }
    }
}

bool is_a(express::Base entity, const char* declaration) {
    return entity && entity.declaration().is(declaration);
}

std::vector<express::Base> snapshot_inverse(ifcopenshell::file* file, express::Base entity) {
    std::vector<express::Base> result;
    if (!file || !entity || entity.id() <= 0) {
        return result;
    }
    try {
        for (auto inverse : file->instances_by_reference(static_cast<int>(entity.id()))) {
            if (inverse && inverse.id() > 0) result.push_back(inverse);
        }
    } catch (const std::exception&) {
    }
    return result;
}

void append_ref(express::Base entity, const char* attr, express::Base ref) {
    auto values = ifcapi::detail::read_ref_aggregate(entity, attr);
    values.push_back(ref);
    ifcapi::detail::write_ref_aggregate(entity, attr, values);
}

void remove_ref_or_clear(express::Base entity, const char* attr, express::Base ref) {
    auto values = ifcapi::detail::read_ref_aggregate(entity, attr);
    values.erase(std::remove(values.begin(), values.end(), ref), values.end());
    if (values.empty()) {
        ifcapi::detail::clear_attr(entity, attr);
    } else {
        ifcapi::detail::write_ref_aggregate(entity, attr, values);
    }
}

std::vector<express::Base> mutable_list(const std::vector<express::Base>& values) {
    std::vector<express::Base> result;
    for (auto value : values) if (value) result.push_back(value);
    return result;
}

void write_quantity_value(express::Base quantity, double value) {
    if (!quantity) return;
    quantity.set_attribute_value(3, value);
}

double read_quantity_value(express::Base quantity, double fallback = 0.0) {
    if (!quantity) return fallback;
    try {
        auto value = quantity.get_attribute_value(3);
        if (value.isNull()) return fallback;
        return static_cast<double>(value);
    } catch (...) {
        try {
            return static_cast<int>(quantity.get_attribute_value(3));
        } catch (...) {
            return fallback;
        }
    }
}

express::Base deep_copy_entity(
    ifcopenshell::file* file,
    express::Base element,
    std::map<express::Base, express::Base>& memo)
{
    if (!file || !element) return {};
    auto found = memo.find(element);
    if (found != memo.end()) return found->second;
    auto* declaration = element.declaration().as_entity();
    if (!declaration) return element;
    auto result = file->create(declaration);
    memo[element] = result;
    auto attrs = declaration->all_attributes();
    for (size_t i = 0; i < attrs.size(); ++i) {
        try {
            auto value = element.get_attribute_value(i);
            if (value.isNull()) continue;
            if (attrs[i]->name() == "GlobalId") {
                result.set_attribute_value(i, ifcapi::guid_new());
                continue;
            }
            switch (value.type()) {
                case ifcopenshell::Argument_ENTITY_INSTANCE:
                    result.set_attribute_value(
                        i,
                        deep_copy_entity(file, static_cast<express::Base>(value), memo));
                    break;
                case ifcopenshell::Argument_AGGREGATE_OF_ENTITY_INSTANCE: {
                    auto input = static_cast<std::vector<express::Base>>(value);
                    std::vector<express::Base> copied;
                    for (auto item : input) copied.push_back(deep_copy_entity(file, item, memo));
                    result.set_attribute_value(i, copied);
                    break;
                }
                case ifcopenshell::Argument_AGGREGATE_OF_AGGREGATE_OF_ENTITY_INSTANCE: {
                    auto input = static_cast<std::vector<std::vector<express::Base>>>(value);
                    std::vector<std::vector<express::Base>> copied;
                    for (auto& row : input) {
                        std::vector<express::Base> copied_row;
                        for (auto item : row) copied_row.push_back(deep_copy_entity(file, item, memo));
                        copied.push_back(copied_row);
                    }
                    result.set_attribute_value(i, copied);
                    break;
                }
                default:
                    ifcapi::detail::copy_attribute_value(result, i, value);
                    break;
            }
        } catch (...) {
        }
    }
    return result;
}

express::Base deep_copy_entity(ifcopenshell::file* file, express::Base element) {
    std::map<express::Base, express::Base> memo;
    return deep_copy_entity(file, element, memo);
}

double primitive_applied_value(express::Base applied_value) {
    if (!applied_value) return 0.0;
    if (is_a(applied_value, "IfcMeasureWithUnit")) {
        return primitive_applied_value(ifcapi::detail::read_ref_attr(applied_value, "ValueComponent"));
    }
    try {
        auto value = applied_value.get_attribute_value(0);
        if (value.isNull()) return 0.0;
        return static_cast<double>(value);
    } catch (...) {
        return 0.0;
    }
}

double total_quantity(express::Base root_element) {
    if (is_a(root_element, "IfcCostItem")) {
        auto quantities = ifcapi::detail::read_ref_aggregate(root_element, "CostQuantities");
        if (quantities.empty()) return std::numeric_limits<double>::quiet_NaN();
        double total = 0.0;
        for (auto quantity : quantities) total += read_quantity_value(quantity);
        return total;
    }
    if (is_a(root_element, "IfcConstructionResource")) {
        auto quantity = ifcapi::detail::read_ref_attr(root_element, "BaseQuantity");
        return quantity ? read_quantity_value(quantity) : 1.0;
    }
    return 1.0;
}

express::Base assigned_rate_cost_item(express::Base cost_item) {
    for (auto assignment : ifcapi::detail::read_inverse_aggregate(cost_item, "HasAssignments")) {
        auto control = ifcapi::detail::read_ref_attr(assignment, "RelatingControl");
        if (is_a(control, "IfcCostItem")) return control;
    }
    return {};
}

double calculate_applied_value(
    express::Base root_element,
    express::Base cost_value,
    const std::string* category_filter = nullptr);

double sum_child_root_elements(express::Base root_element, const std::string* category_filter = nullptr) {
    double result = 0.0;
    for (auto rel : ifcapi::detail::read_inverse_aggregate(root_element, "IsNestedBy")) {
        for (auto child_root : ifcapi::detail::read_ref_aggregate(rel, "RelatedObjects")) {
            auto new_child_root = assigned_rate_cost_item(child_root);
            if (!new_child_root) new_child_root = child_root;
            auto values = is_a(root_element, "IfcCostItem")
                ? ifcapi::detail::read_ref_aggregate(new_child_root, "CostValues")
                : ifcapi::detail::read_ref_aggregate(child_root, "BaseCosts");
            for (auto child_cost_value : values) {
                auto category = ifcapi::detail::read_optional_string_attr(child_cost_value, "Category");
                if (category_filter && (!category.has_value || category.value != *category_filter)) continue;
                double applied = calculate_applied_value(new_child_root, child_cost_value);
                double quantity = total_quantity(child_root);
                if (std::isnan(quantity)) quantity = 1.0;
                if (auto unit_basis = ifcapi::detail::read_ref_attr(child_cost_value, "UnitBasis")) {
                    auto value_component = ifcapi::detail::read_ref_attr(unit_basis, "ValueComponent");
                    double component_value = primitive_applied_value(value_component);
                    if (component_value != 0.0) {
                        result += quantity / component_value * applied;
                    }
                } else {
                    result += quantity * applied;
                }
            }
        }
    }
    return result;
}

double calculate_applied_value(
    express::Base root_element,
    express::Base cost_value,
    const std::string* category_filter)
{
    auto components = ifcapi::detail::read_ref_aggregate(cost_value, "Components");
    auto arithmetic = ifcapi::detail::read_optional_string_attr(cost_value, "ArithmeticOperator");
    if (arithmetic.has_value && !components.empty()) {
        std::vector<double> component_values;
        for (auto component : components) {
            component_values.push_back(calculate_applied_value(root_element, component, category_filter));
        }
        if (arithmetic.value == "ADD") {
            double total = 0.0;
            for (double value : component_values) total += value;
            return total;
        }
        double result = component_values.empty() ? 0.0 : component_values.front();
        for (size_t i = 1; i < component_values.size(); ++i) {
            if (arithmetic.value == "DIVIDE") {
                if (component_values[i] != 0.0) result /= component_values[i];
            } else if (arithmetic.value == "MULTIPLY") {
                result *= component_values[i];
            } else if (arithmetic.value == "SUBTRACT") {
                result -= component_values[i];
            }
        }
        return result;
    }
    auto category = ifcapi::detail::read_optional_string_attr(cost_value, "Category");
    if (!category.has_value) {
        return primitive_applied_value(ifcapi::detail::read_ref_attr(cost_value, "AppliedValue"));
    }
    if (category.value == "*") {
        auto nested = ifcapi::detail::read_inverse_aggregate(root_element, "IsNestedBy");
        return nested.empty()
            ? primitive_applied_value(ifcapi::detail::read_ref_attr(cost_value, "AppliedValue"))
            : sum_child_root_elements(root_element);
    }
    if (!category.value.empty()) {
        auto nested = ifcapi::detail::read_inverse_aggregate(root_element, "IsNestedBy");
        return nested.empty()
            ? primitive_applied_value(ifcapi::detail::read_ref_attr(cost_value, "AppliedValue"))
            : sum_child_root_elements(root_element, &category.value);
    }
    return 0.0;
}

std::pair<bool, std::pair<double, std::string>> resource_cost(express::Base resource) {
    auto base_costs = ifcapi::detail::read_ref_aggregate(resource, "BaseCosts");
    if (base_costs.empty()) return {false, {0.0, std::string()}};
    double cost = 0.0;
    std::string unit;
    for (auto value : base_costs) {
        cost += calculate_applied_value(resource, value);
        if (unit.empty()) {
            auto unit_basis = ifcapi::detail::read_ref_attr(value, "UnitBasis");
            auto unit_component = unit_basis ? ifcapi::detail::read_ref_attr(unit_basis, "UnitComponent") : express::Base();
            if (unit_component && is_a(unit_component, "IfcConversionBasedUnit")) {
                unit = ifcapi::detail::read_string_attr(unit_component, "Name");
            }
        }
    }
    return {true, {cost, unit}};
}

double resource_quantity(express::Base resource) {
    auto usage = ifcapi::detail::read_ref_attr(resource, "Usage");
    auto schedule_work = usage ? ifcapi::detail::read_optional_string_attr(usage, "ScheduleWork") : ifcapi::detail::OptionalString{};
    if (schedule_work.has_value) {
        const auto& text = schedule_work.value;
        if (text.size() > 2 && text[0] == 'P') {
            auto pos_t = text.find('T');
            auto pos_h = text.find('H');
            auto pos_d = text.find('D');
            try {
                if (pos_t != std::string::npos && pos_h != std::string::npos && pos_h > pos_t) {
                    return std::stod(text.substr(pos_t + 1, pos_h - pos_t - 1));
                }
                if (pos_d != std::string::npos) {
                    return std::stod(text.substr(1, pos_d - 1)) * 24.0;
                }
            } catch (...) {
            }
        }
    }
    auto quantity = ifcapi::detail::read_ref_attr(resource, "BaseQuantity");
    return quantity ? read_quantity_value(quantity) : 1.0;
}

} // namespace

namespace ifcapi {
namespace bindings {

void cost_edit_cost_value(
    ifcopenshell::file* file,
    express::Base cost_value,
    ifcopenshell_pset_props_t* attributes,
    bool has_unit_basis,
    bool unit_basis_is_null,
    double value_component,
    std::optional<express::Base> unit_component)
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
            auto old_unit_basis = ifcapi::detail::read_ref_attr(cost_value, "UnitBasis");
            express::Base new_unit_basis = {};
            if (!unit_basis_is_null) {
                if (!unit_component) {
                    throw std::runtime_error("UnitBasis requires a UnitComponent");
                }
                auto unit_type = ifcapi::detail::read_string_attr(*unit_component, "UnitType");
                auto measure_class = unit_get_measure_class(unit_type);
                auto measure = create_measure(file, measure_class, value_component);
                new_unit_basis = create_entity(file, "IfcMeasureWithUnit");
                ifcapi::detail::write_ref_attr(new_unit_basis, "ValueComponent", measure);
                ifcapi::detail::write_ref_attr(new_unit_basis, "UnitComponent", *unit_component);
            }
            if (old_unit_basis) {
                entity_remove_deep2(&old_unit_basis);
            }
            ifcapi::detail::write_ref_attr(cost_value, "UnitBasis", new_unit_basis);
        }
    } catch (const std::exception& e) {
        ifcopenshell::capi::set_last_error(e.what());
    }
}

void cost_edit_cost_value_formula(ifcopenshell::file* file, express::Base cost_value, const std::string& formula) {
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

express::Base cost_add_cost_schedule(
    ifcopenshell::file* file,
    const char* name,
    const std::string& predefined_type,
    const std::string& update_date,
    express::Base owner_history)
{
    ifcopenshell_clear_error();
    try {
        RootCreateEntityOptions options;
        options.ifc_class = "IfcCostSchedule";
        if (!predefined_type.empty()) {
            options.predefined_type = predefined_type;
        }
        if (name && name[0] != '\0') {
            options.name = name;
        }
        if (owner_history) {
            options.owner_history = owner_history;
        }
        auto schedule = root_create_entity(file, options);
        auto result = sequence_add_date_time(file, update_date);
        if (std::holds_alternative<express::Base>(result)) {
            ifcapi::detail::write_ref_attr(schedule, "UpdateDate", std::get<express::Base>(result));
        } else {
            ifcapi::detail::write_string_attr(schedule, "UpdateDate", std::get<std::string>(result));
        }
        return schedule;
    } catch (const std::exception& e) {
        ifcopenshell::capi::set_last_error(e.what());
        return {};
    }
}

express::Base cost_add_cost_item(
    ifcopenshell::file* file,
    express::Base cost_schedule,
    express::Base parent_cost_item,
    express::Base owner_history,
    express::Base user,
    express::Base application)
{
    ifcopenshell_clear_error();
    try {
        RootCreateEntityOptions options;
        options.ifc_class = "IfcCostItem";
        if (owner_history) {
            options.owner_history = owner_history;
        }
        auto item = root_create_entity(file, options);
        if (cost_schedule) {
            control_assign_control(file, {
                cost_schedule,
                {item},
                owner_history ? std::optional<express::Base>(owner_history) : std::nullopt,
                user ? std::optional<express::Base>(user) : std::nullopt,
                application ? std::optional<express::Base>(application) : std::nullopt,
            });
        } else if (parent_cost_item) {
            NestAssignObjectOptions nest_opts;
            nest_opts.products = {item};
            nest_opts.relating_object = parent_cost_item;
            if (owner_history) nest_opts.owner_history = owner_history;
            if (user) nest_opts.user = user;
            if (application) nest_opts.application = application;
            nest_assign_object(file, nest_opts);
        }
        return item;
    } catch (const std::exception& e) {
        ifcopenshell::capi::set_last_error(e.what());
        return {};
    }
}

express::Base cost_add_cost_value(ifcopenshell::file* file, express::Base parent) {
    ifcopenshell_clear_error();
    try {
        return add_cost_value(file, parent);
    } catch (const std::exception& e) {
        ifcopenshell::capi::set_last_error(e.what());
        return {};
    }
}

express::Base cost_add_cost_item_quantity(
    ifcopenshell::file* file,
    express::Base cost_item,
    const std::string& ifc_class)
{
    ifcopenshell_clear_error();
    try {
        auto quantity = create_entity(file, ifc_class);
        ifcapi::detail::write_string_attr(quantity, "Name", "Unnamed");
        if (ifc_class == "IfcQuantityCount") {
            double count = 0.0;
            for (auto rel : ifcapi::detail::read_inverse_aggregate(cost_item, "Controls")) {
                count += static_cast<double>(ifcapi::detail::read_ref_aggregate(rel, "RelatedObjects").size());
            }
            write_quantity_value(quantity, count);
        } else {
            write_quantity_value(quantity, 0.0);
        }
        append_ref(cost_item, "CostQuantities", quantity);
        return quantity;
    } catch (const std::exception& e) {
        ifcopenshell::capi::set_last_error(e.what());
        return {};
    }
}

void cost_assign_cost_item_quantity(
    ifcopenshell::file* file,
    express::Base cost_item,
    const std::vector<express::Base>& products,
    const char* prop_name,
    express::Base owner_history,
    express::Base user,
    express::Base application)
{
    ifcopenshell_clear_error();
    try {
        std::string prop = prop_name ? prop_name : "";
        std::set<express::Base> quantities;
        if (!prop.empty()) {
            for (auto q : ifcapi::detail::read_ref_aggregate(cost_item, "CostQuantities")) quantities.insert(q);
        }
        for (auto raw_product : products) {
            auto product = raw_product;
            if (!product || is_a(product, "IfcSpatialElement")) continue;
            control_assign_control(file, {
                cost_item,
                {product},
                owner_history ? std::optional<express::Base>(owner_history) : std::nullopt,
                user ? std::optional<express::Base>(user) : std::nullopt,
                application ? std::optional<express::Base>(application) : std::nullopt,
            });
            if (prop.empty()) continue;
            auto existing = ifcapi::detail::read_ref_aggregate(cost_item, "CostQuantities");
            if (!existing.empty()) {
                auto first_name = ifcapi::detail::read_string_attr(existing.front(), "Name");
                auto lower_first = lower_string(first_name);
                auto lower_prop = lower_string(prop);
                if (lower_first != lower_prop) continue;
            }
            for (auto relationship : ifcapi::detail::read_inverse_aggregate(product, "IsDefinedBy")) {
                if (!is_a(relationship, "IfcRelDefinesByProperties")) continue;
                auto qto = ifcapi::detail::read_ref_attr(relationship, "RelatingPropertyDefinition");
                if (!is_a(qto, "IfcElementQuantity")) continue;
                for (auto quantity : ifcapi::detail::read_ref_aggregate(qto, "Quantities")) {
                    if (!is_a(quantity, "IfcPhysicalSimpleQuantity")) continue;
                    if (lower_string(ifcapi::detail::read_string_attr(quantity, "Name")) == lower_string(prop)) {
                        quantities.insert(quantity);
                    }
                }
            }
        }
        if (!prop.empty()) {
            std::vector<express::Base> values(quantities.begin(), quantities.end());
            ifcapi::detail::write_ref_aggregate(cost_item, "CostQuantities", values);
        } else {
            auto existing = ifcapi::detail::read_ref_aggregate(cost_item, "CostQuantities");
            if (existing.empty()) {
                cost_add_cost_item_quantity(file, cost_item, "IfcQuantityCount");
                existing = ifcapi::detail::read_ref_aggregate(cost_item, "CostQuantities");
            }
            if (existing.size() == 1 && is_a(existing.front(), "IfcQuantityCount")) {
                double count = 0.0;
                for (auto rel : ifcapi::detail::read_inverse_aggregate(cost_item, "Controls")) {
                    for (auto obj : ifcapi::detail::read_ref_aggregate(rel, "RelatedObjects")) {
                        if (!is_a(obj, "IfcConstructionResource")) count += 1.0;
                    }
                }
                write_quantity_value(existing.front(), count);
            }
        }
    } catch (const std::exception& e) {
        ifcopenshell::capi::set_last_error(e.what());
    }
}

void cost_unassign_cost_item_quantity(
    ifcopenshell::file* file,
    express::Base cost_item,
    const std::vector<express::Base>& products,
    express::Base user,
    express::Base application)
{
    ifcopenshell_clear_error();
    try {
        auto product_vec = mutable_list(products);
        std::set<express::Base> product_set(product_vec.begin(), product_vec.end());
        std::set<express::Base> quantities;
        for (auto q : ifcapi::detail::read_ref_aggregate(cost_item, "CostQuantities")) quantities.insert(q);
        for (auto quantity : ifcapi::detail::read_ref_aggregate(cost_item, "CostQuantities")) {
            for (auto inverse : snapshot_inverse(file, quantity)) {
                if (!is_a(inverse, "IfcElementQuantity")) continue;
                for (auto rel : ifcapi::detail::read_inverse_aggregate(inverse, "DefinesOccurrence")) {
                    for (auto related : ifcapi::detail::read_ref_aggregate(rel, "RelatedObjects")) {
                        if (product_set.find(related) != product_set.end()) quantities.erase(quantity);
                    }
                }
            }
        }
        ifcapi::detail::write_ref_aggregate(
            cost_item,
            "CostQuantities",
            std::vector<express::Base>(quantities.begin(), quantities.end()));
        for (auto product : product_vec) {
            control_unassign_control(file, {
                cost_item,
                {product},
                user ? std::optional<express::Base>(user) : std::nullopt,
                application ? std::optional<express::Base>(application) : std::nullopt,
            });
        }
        auto existing = ifcapi::detail::read_ref_aggregate(cost_item, "CostQuantities");
        if (existing.size() == 1 && is_a(existing.front(), "IfcQuantityCount")) {
            double count = 0.0;
            for (auto rel : ifcapi::detail::read_inverse_aggregate(cost_item, "Controls")) {
                count += static_cast<double>(ifcapi::detail::read_ref_aggregate(rel, "RelatedObjects").size());
            }
            if (count) {
                write_quantity_value(existing.front(), count);
            } else {
                file->remove_entity(existing.front());
            }
        }
    } catch (const std::exception& e) {
        ifcopenshell::capi::set_last_error(e.what());
    }
}

void cost_remove_cost_item_quantity(
    ifcopenshell::file* file,
    express::Base cost_item,
    express::Base physical_quantity)
{
    ifcopenshell_clear_error();
    try {
        if (ifcapi::detail::total_inverses(file, physical_quantity) == 1) {
            file->remove_entity(physical_quantity);
            return;
        }
        auto quantities = ifcapi::detail::read_ref_aggregate(cost_item, "CostQuantities");
        quantities.erase(std::remove(quantities.begin(), quantities.end(), physical_quantity), quantities.end());
        ifcapi::detail::write_ref_aggregate(cost_item, "CostQuantities", quantities);
    } catch (const std::exception& e) {
        ifcopenshell::capi::set_last_error(e.what());
    }
}

void cost_edit_cost_item(ifcopenshell::file*, express::Base cost_item, ifcopenshell_pset_props_t* attributes) {
    ifcopenshell_clear_error();
    try { ifcapi::detail::apply_attribute_props(cost_item, attributes); } catch (const std::exception& e) { ifcopenshell::capi::set_last_error(e.what()); }
}

void cost_edit_cost_schedule(ifcopenshell::file*, express::Base cost_schedule, ifcopenshell_pset_props_t* attributes) {
    ifcopenshell_clear_error();
    try { ifcapi::detail::apply_attribute_props(cost_schedule, attributes); } catch (const std::exception& e) { ifcopenshell::capi::set_last_error(e.what()); }
}

void cost_edit_cost_item_quantity(ifcopenshell::file*, express::Base physical_quantity, ifcopenshell_pset_props_t* attributes) {
    ifcopenshell_clear_error();
    try { ifcapi::detail::apply_attribute_props(physical_quantity, attributes); } catch (const std::exception& e) { ifcopenshell::capi::set_last_error(e.what()); }
}

void cost_remove_cost_value(ifcopenshell::file* file, express::Base parent, express::Base cost_value) {
    ifcopenshell_clear_error();
    try {
        if (ifcapi::detail::total_inverses(file, cost_value) == 1) {
            file->remove_entity(cost_value);
        } else if (is_a(parent, "IfcCostItem")) {
            remove_ref_or_clear(parent, "CostValues", cost_value);
        } else if (is_a(parent, "IfcConstructionResource")) {
            remove_ref_or_clear(parent, "BaseCosts", cost_value);
        } else if (is_a(parent, "IfcCostValue")) {
            remove_ref_or_clear(parent, "Components", cost_value);
        }
    } catch (const std::exception& e) {
        ifcopenshell::capi::set_last_error(e.what());
    }
}

void cost_copy_cost_item_values(ifcopenshell::file* file, express::Base source, express::Base destination) {
    ifcopenshell_clear_error();
    try {
        auto destination_values = ifcapi::detail::read_ref_aggregate(destination, "CostValues");
        for (auto value : destination_values) {
            cost_remove_cost_value(file, source, value);
        }
        std::vector<express::Base> copied;
        for (auto value : ifcapi::detail::read_ref_aggregate(source, "CostValues")) {
            copied.push_back(deep_copy_entity(file, value));
        }
        ifcapi::detail::write_ref_aggregate(destination, "CostValues", copied);
    } catch (const std::exception& e) {
        ifcopenshell::capi::set_last_error(e.what());
    }
}

void cost_assign_cost_value(ifcopenshell::file* file, express::Base cost_item, express::Base cost_rate) {
    ifcopenshell_clear_error();
    try {
        auto values = ifcapi::detail::read_ref_aggregate(cost_item, "CostValues");
        for (auto value : values) cost_remove_cost_value(file, cost_item, value);
        ifcapi::detail::write_ref_aggregate(cost_item, "CostValues", ifcapi::detail::read_ref_aggregate(cost_rate, "CostValues"));
    } catch (const std::exception& e) {
        ifcopenshell::capi::set_last_error(e.what());
    }
}

class CostItemCopier {
public:
    explicit CostItemCopier(ifcopenshell::file* file) : file_(file) {}

    std::vector<express::Base> execute(express::Base cost_item) {
        new_cost_items_.clear();
        duplicate_cost_item(cost_item);
        return new_cost_items_;
    }

private:
    express::Base duplicate_cost_item(express::Base cost_item) {
        auto new_cost_item = deep_copy_entity(file_, cost_item);
        new_cost_items_.push_back(new_cost_item);
        copy_indirect_attributes(cost_item, new_cost_item);
        return new_cost_item;
    }

    void copy_indirect_attributes(express::Base from_element, express::Base to_element) {
        for (auto inverse : snapshot_inverse(file_, from_element)) {
            if (is_a(inverse, "IfcRelDefinesByProperties")) {
                auto rel = ifcapi::detail::shallow_copy(file_, inverse);
                ifcapi::detail::write_ref_aggregate(rel, "RelatedObjects", {to_element});
                auto pset = deep_copy_entity(file_, ifcapi::detail::read_ref_attr(inverse, "RelatingPropertyDefinition"));
                ifcapi::detail::write_ref_attr(rel, "RelatingPropertyDefinition", pset);
            } else if (is_a(inverse, "IfcRelNests") && ifcapi::detail::read_ref_attr(inverse, "RelatingObject") == from_element) {
                auto nested = ifcapi::detail::read_ref_aggregate(inverse, "RelatedObjects");
                if (!nested.empty()) {
                    std::vector<express::Base> new_children;
                    for (auto child : nested) new_children.push_back(duplicate_cost_item(child));
                    auto rel = ifcapi::detail::shallow_copy(file_, inverse);
                    ifcapi::detail::write_ref_attr(rel, "RelatingObject", to_element);
                    ifcapi::detail::write_ref_aggregate(rel, "RelatedObjects", new_children);
                    std::vector<express::Base> child_const(new_children.begin(), new_children.end());
                    {
                        NestUnassignObjectOptions unassign_opts;
                        unassign_opts.products = child_const;
                        nest_unassign_object(file_, unassign_opts);
                    }
                    {
                        NestAssignObjectOptions assign_opts;
                        assign_opts.products = child_const;
                        assign_opts.relating_object = to_element;
                        nest_assign_object(file_, assign_opts);
                    }
                }
            } else {
                auto* declaration = inverse.declaration().as_entity();
                if (!declaration) continue;
                auto attrs = declaration->all_attributes();
                for (size_t i = 0; i < attrs.size(); ++i) {
                    try {
                        auto value = inverse.get_attribute_value(i);
                        if (value.isNull()) continue;
                        if (value.type() == ifcopenshell::Argument_ENTITY_INSTANCE &&
                            static_cast<express::Base>(value) == from_element) {
                            auto new_inverse = ifcapi::detail::shallow_copy(file_, inverse);
                            new_inverse.set_attribute_value(i, to_element);
                        } else if (value.type() == ifcopenshell::Argument_AGGREGATE_OF_ENTITY_INSTANCE) {
                            auto aggregate = static_cast<std::vector<express::Base>>(value);
                            bool contains = false;
                            for (auto item : aggregate) {
                                if (item == from_element) {
                                    contains = true;
                                    break;
                                }
                            }
                            if (contains) {
                                std::vector<express::Base> refs;
                                for (auto item : aggregate) refs.push_back(item);
                                refs.push_back(to_element);
                                ifcapi::detail::set_ref_aggregate(inverse, static_cast<int>(i), refs);
                            }
                        }
                    } catch (...) {
                    }
                }
            }
        }
    }

    ifcopenshell::file* file_;
    std::vector<express::Base> new_cost_items_;
};

std::vector<express::Base> cost_copy_cost_item(ifcopenshell::file* file, express::Base cost_item) {
    ifcopenshell_clear_error();
    try {
        return CostItemCopier(file).execute(cost_item);
    } catch (const std::exception& e) {
        ifcopenshell::capi::set_last_error(e.what());
        return {};
    }
}

express::Base cost_copy_cost_schedule(
    ifcopenshell::file* file,
    express::Base cost_schedule,
    express::Base owner_history,
    express::Base user,
    express::Base application)
{
    ifcopenshell_clear_error();
    try {
        auto new_schedule = ifcapi::detail::shallow_copy(file, cost_schedule);
        for (auto rel : ifcapi::detail::read_inverse_aggregate(cost_schedule, "Controls")) {
            for (auto cost_item : ifcapi::detail::read_ref_aggregate(rel, "RelatedObjects")) {
                auto duplicated = cost_copy_cost_item(file, cost_item);
                if (!duplicated.empty()) {
                    control_assign_control(file, {
                        new_schedule,
                        {duplicated.front()},
                        owner_history ? std::optional<express::Base>(owner_history) : std::nullopt,
                        user ? std::optional<express::Base>(user) : std::nullopt,
                        application ? std::optional<express::Base>(application) : std::nullopt,
                    });
                }
            }
        }
        return new_schedule;
    } catch (const std::exception& e) {
        ifcopenshell::capi::set_last_error(e.what());
        return {};
    }
}

void cost_remove_cost_item(ifcopenshell::file* file, express::Base cost_item) {
    ifcopenshell_clear_error();
    try {
        for (auto inverse : snapshot_inverse(file, cost_item)) {
            if (is_a(inverse, "IfcRelNests")) {
                if (ifcapi::detail::read_ref_attr(inverse, "RelatingObject") == cost_item) {
                    for (auto related : ifcapi::detail::read_ref_aggregate(inverse, "RelatedObjects")) {
                        cost_remove_cost_item(file, related);
                    }
                } else {
                    auto related = ifcapi::detail::read_ref_aggregate(inverse, "RelatedObjects");
                    if (related.size() == 1 && related.front() == cost_item) {
                        ifcapi::detail::remove_with_history(file, inverse);
                    }
                }
            } else if (is_a(inverse, "IfcRelAssignsToControl")) {
                if (ifcapi::detail::read_ref_aggregate(inverse, "RelatedObjects").size() >= 2) continue;
                ifcapi::detail::remove_with_history(file, inverse);
            }
        }
        ifcapi::detail::remove_with_history(file, cost_item);
    } catch (const std::exception& e) {
        ifcopenshell::capi::set_last_error(e.what());
    }
}

void cost_remove_cost_schedule(ifcopenshell::file* file, express::Base cost_schedule) {
    ifcopenshell_clear_error();
    try {
        for (auto inverse : snapshot_inverse(file, cost_schedule)) {
            if (!is_a(inverse, "IfcRelAssignsToControl")) continue;
            for (auto related : ifcapi::detail::read_ref_aggregate(inverse, "RelatedObjects")) {
                if (is_a(related, "IfcCostItem")) cost_remove_cost_item(file, related);
            }
        }
        ifcapi::detail::remove_with_history(file, cost_schedule);
    } catch (const std::exception& e) {
        ifcopenshell::capi::set_last_error(e.what());
    }
}

void cost_calculate_cost_item_resource_value(ifcopenshell::file* file, express::Base cost_item) {
    ifcopenshell_clear_error();
    try {
        auto existing = ifcapi::detail::read_ref_aggregate(cost_item, "CostValues");
        for (auto value : existing) cost_remove_cost_value(file, cost_item, value);
        std::vector<express::Base> resources;
        for (auto rel : ifcapi::detail::read_inverse_aggregate(cost_item, "Controls")) {
            for (auto related : ifcapi::detail::read_ref_aggregate(rel, "RelatedObjects")) {
                if (is_a(related, "IfcConstructionResource")) {
                    resources.push_back(related);
                } else if (is_a(related, "IfcTask")) {
                    for (auto rel2 : ifcapi::detail::read_inverse_aggregate(related, "OperatesOn")) {
                        for (auto related2 : ifcapi::detail::read_ref_aggregate(rel2, "RelatedObjects")) {
                            if (is_a(related2, "IfcConstructionResource")) resources.push_back(related2);
                        }
                    }
                }
            }
        }
        for (auto resource : resources) {
            auto cost = resource_cost(resource);
            if (!cost.first) {
                auto nests = ifcapi::detail::read_inverse_aggregate(resource, "Nests");
                if (!nests.empty()) {
                    cost = resource_cost(ifcapi::detail::read_ref_attr(nests.front(), "RelatingObject"));
                }
            }
            if (!cost.first) continue;
            double quantity = resource_quantity(resource);
            if (cost.second.second.find("day") != std::string::npos) quantity /= 8.0;
            auto value = add_cost_value(file, cost_item);
            ifcapi::detail::write_string_attr(value, "Name", ifcapi::detail::read_string_attr(resource, "Name"));
            cost_edit_cost_value_formula(file, value, std::to_string(cost.second.first) + "*" + std::to_string(quantity));
        }
    } catch (const std::exception& e) {
        ifcopenshell::capi::set_last_error(e.what());
    }
}

void cost_edit_cost_value(
    ifcopenshell::file* file,
    express::Base* cost_value,
    ifcopenshell_pset_props_t* attributes,
    const CostEditCostValueOptions& options)
{
    cost_edit_cost_value(
        file,
        detail::deref_or_empty(cost_value),
        attributes,
        options.edit_unit_basis,
        options.clear_unit_basis,
        options.value_component,
        options.unit_component);
}

void cost_edit_cost_value_formula(ifcopenshell::file* file, express::Base* cost_value, const std::string& formula) {
    cost_edit_cost_value_formula(file, detail::deref_or_empty(cost_value), formula);
}

express::Base cost_add_cost_schedule(
    ifcopenshell::file* file,
    const char* name,
    const std::string& predefined_type,
    const std::string& update_date,
    std::optional<express::Base> owner_history)
{
    return cost_add_cost_schedule(file, name, predefined_type, update_date, owner_history.value_or(express::Base{}));
}

express::Base cost_add_cost_item(
    ifcopenshell::file* file,
    const CostAddCostItemOptions& options)
{
    return cost_add_cost_item(
        file,
        options.cost_schedule.value_or(express::Base{}),
        options.cost_item.value_or(express::Base{}),
        options.owner_history.value_or(express::Base{}),
        options.user.value_or(express::Base{}),
        options.application.value_or(express::Base{}));
}

express::Base cost_add_cost_value(ifcopenshell::file* file, express::Base* parent) {
    return cost_add_cost_value(file, detail::deref_or_empty(parent));
}

express::Base cost_add_cost_item_quantity(ifcopenshell::file* file, express::Base* cost_item, const std::string& ifc_class) {
    return cost_add_cost_item_quantity(file, detail::deref_or_empty(cost_item), ifc_class);
}

void cost_assign_cost_item_quantity(
    ifcopenshell::file* file,
    express::Base* cost_item,
    const std::vector<express::Base>& products,
    const char* prop_name,
    const CostAssignCostItemQuantityOptions& options)
{
    cost_assign_cost_item_quantity(
        file,
        detail::deref_or_empty(cost_item),
        products,
        prop_name,
        options.owner_history.value_or(express::Base{}),
        options.user.value_or(express::Base{}),
        options.application.value_or(express::Base{}));
}

void cost_unassign_cost_item_quantity(
    ifcopenshell::file* file,
    express::Base* cost_item,
    const std::vector<express::Base>& products,
    const CostUnassignCostItemQuantityOptions& options)
{
    cost_unassign_cost_item_quantity(
        file,
        detail::deref_or_empty(cost_item),
        products,
        options.user.value_or(express::Base{}),
        options.application.value_or(express::Base{}));
}

void cost_remove_cost_item_quantity(ifcopenshell::file* file, express::Base* cost_item, express::Base* physical_quantity) {
    cost_remove_cost_item_quantity(file, detail::deref_or_empty(cost_item), detail::deref_or_empty(physical_quantity));
}

void cost_edit_cost_item(ifcopenshell::file* file, express::Base* cost_item, ifcopenshell_pset_props_t* attributes) {
    cost_edit_cost_item(file, detail::deref_or_empty(cost_item), attributes);
}

void cost_edit_cost_schedule(ifcopenshell::file* file, express::Base* cost_schedule, ifcopenshell_pset_props_t* attributes) {
    cost_edit_cost_schedule(file, detail::deref_or_empty(cost_schedule), attributes);
}

void cost_edit_cost_item_quantity(ifcopenshell::file* file, express::Base* physical_quantity, ifcopenshell_pset_props_t* attributes) {
    cost_edit_cost_item_quantity(file, detail::deref_or_empty(physical_quantity), attributes);
}

void cost_remove_cost_value(ifcopenshell::file* file, express::Base* parent, express::Base* cost_value) {
    cost_remove_cost_value(file, detail::deref_or_empty(parent), detail::deref_or_empty(cost_value));
}

void cost_copy_cost_item_values(ifcopenshell::file* file, express::Base* source, express::Base* destination) {
    cost_copy_cost_item_values(file, detail::deref_or_empty(source), detail::deref_or_empty(destination));
}

void cost_assign_cost_value(ifcopenshell::file* file, express::Base* cost_item, express::Base* cost_rate) {
    cost_assign_cost_value(file, detail::deref_or_empty(cost_item), detail::deref_or_empty(cost_rate));
}

std::vector<express::Base> cost_copy_cost_item(ifcopenshell::file* file, express::Base* cost_item) {
    return cost_copy_cost_item(file, detail::deref_or_empty(cost_item));
}

express::Base cost_copy_cost_schedule(
    ifcopenshell::file* file,
    express::Base* cost_schedule,
    const CostCopyCostScheduleOptions& options)
{
    return cost_copy_cost_schedule(
        file,
        detail::deref_or_empty(cost_schedule),
        options.owner_history.value_or(express::Base{}),
        options.user.value_or(express::Base{}),
        options.application.value_or(express::Base{}));
}

void cost_remove_cost_item(ifcopenshell::file* file, express::Base* cost_item) {
    cost_remove_cost_item(file, detail::deref_or_empty(cost_item));
}

void cost_remove_cost_schedule(ifcopenshell::file* file, express::Base* cost_schedule) {
    cost_remove_cost_schedule(file, detail::deref_or_empty(cost_schedule));
}

void cost_calculate_cost_item_resource_value(ifcopenshell::file* file, express::Base* cost_item) {
    cost_calculate_cost_item_resource_value(file, detail::deref_or_empty(cost_item));
}

} // namespace bindings
} // namespace ifcapi
