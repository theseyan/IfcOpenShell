# This file was generated with the assistance of an AI coding tool.
# SPDX-License-Identifier: LGPL-3.0-or-later

"""Cost utility functions for cost value calculations and serialisation."""

import lark

arithmetic_operator_symbols = {"ADD": "+", "DIVIDE": "/", "MULTIPLY": "*", "SUBTRACT": "-"}
symbol_arithmetic_operators = {"+": "ADD", "/": "DIVIDE", "*": "MULTIPLY", "-": "SUBTRACT"}


def get_primitive_applied_value(applied_value):
    if not applied_value:
        return 0.0
    elif isinstance(applied_value, float):
        return applied_value
    elif hasattr(applied_value, "wrappedValue") and isinstance(applied_value.wrappedValue, float):
        return applied_value.wrappedValue
    elif applied_value.is_a("IfcMeasureWithUnit"):
        return applied_value.ValueComponent
    return 0.0


def get_total_quantity(root_element):
    if root_element.is_a("IfcCostItem"):
        quantities = root_element.CostQuantities
        if not quantities:
            return None
        return sum([q[3] for q in quantities])
    elif root_element.is_a("IfcConstructionResource"):
        quantity = root_element.BaseQuantity
        return quantity[3] if quantity else 1.0


def calculate_applied_value(root_element, cost_value, category_filter=None):
    if cost_value.ArithmeticOperator and cost_value.Components:
        component_values = []
        for component in cost_value.Components:
            component_values.append(calculate_applied_value(root_element, component, category_filter))
        if cost_value.ArithmeticOperator == "ADD":
            return sum(component_values)
        result = component_values.pop(0)
        if cost_value.ArithmeticOperator == "DIVIDE":
            for value in component_values:
                try:
                    result /= value
                except ZeroDivisionError:
                    pass
        elif cost_value.ArithmeticOperator == "MULTIPLY":
            for value in component_values:
                result *= value
        elif cost_value.ArithmeticOperator == "SUBTRACT":
            for value in component_values:
                result -= value
        return result
    if cost_value.Category is None:
        return get_primitive_applied_value(cost_value.AppliedValue)
    elif cost_value.Category == "*":
        if root_element.IsNestedBy:
            return sum_child_root_elements(root_element)
        else:
            return get_primitive_applied_value(cost_value.AppliedValue)
    elif cost_value.Category:
        if root_element.IsNestedBy:
            return sum_child_root_elements(root_element, category_filter=cost_value.Category)
        else:
            return get_primitive_applied_value(cost_value.AppliedValue)
    return 0.0


def sum_child_root_elements(root_element, category_filter=None):
    result = 0.0
    for rel in root_element.IsNestedBy:
        for child_root_element in rel.RelatedObjects:
            if root_element.is_a("IfcCostItem"):
                values = child_root_element.CostValues
            elif root_element.is_a("IfcConstructionResource"):
                values = child_root_element.BaseCosts
            else:
                values = []
            for child_cost_value in values or []:
                if category_filter and child_cost_value.Category != category_filter:
                    continue
                child_applied_value = calculate_applied_value(child_root_element, child_cost_value)
                child_quantity = get_total_quantity(child_root_element)
                child_quantity = 1.0 if child_quantity is None else child_quantity
                if child_cost_value.UnitBasis:
                    value_component = child_cost_value.UnitBasis.ValueComponent.wrappedValue
                    result += child_quantity / value_component * child_applied_value
                else:
                    result += child_quantity * child_applied_value
    return result


def serialise_cost_value(cost_value):
    result = _serialise_cost_value(cost_value)
    if result and result[0] == "(" and result[-1] == ")":
        return result[1:-1]
    return result


def _serialise_cost_value(cost_value):
    value = ""
    if cost_value.ArithmeticOperator and cost_value.Components:
        operator = arithmetic_operator_symbols[cost_value.ArithmeticOperator]
        serialised_components = []
        for component in cost_value.Components:
            serialised_components.append(_serialise_cost_value(component))
        value = operator.join(serialised_components)
    elif cost_value.AppliedValue is not None:
        if cost_value.AppliedValue.is_a("IfcMonetaryMeasure"):
            value = str(cost_value.AppliedValue.wrappedValue)
        else:
            value = "?"

    category = ""
    if cost_value.Category == "*":
        category = "SUM"
    elif cost_value.Category:
        category = cost_value.Category

    if not category and not value:
        value = "0"

    if category:
        return f"{category}({value})"
    elif cost_value.Components:
        return f"({value})"
    return value


def unserialise_cost_value(formula, cost_value):
    unserialiser = CostValueUnserialiser()
    result = unserialiser.parse(formula)

    def map_element_to_result(element, result):
        result["ifc"] = element
        for i, component in enumerate(result.get("Components", [])):
            if element.Components and i < len(element.Components):
                map_element_to_result(element.Components[i], result["Components"][i])

    map_element_to_result(cost_value, result)
    return result


def get_root_cost_items(cost_schedule):
    return [
        related_object
        for rel in cost_schedule.Controls or []
        for related_object in rel.RelatedObjects
        if related_object.is_a("IfcCostItem")
    ]


def get_all_nested_cost_items(cost_item):
    for ci in get_nested_cost_items(cost_item):
        yield ci
        yield from get_all_nested_cost_items(ci)


def get_nested_cost_items(cost_item, is_deep=False):
    if is_deep:
        return list(get_all_nested_cost_items(cost_item))
    result = []
    is_nested_by = getattr(cost_item, "IsNestedBy", None)
    if is_nested_by is None:
        # IFC2X3: use IsDecomposedBy and filter for IfcRelNests
        is_decomposed_by = getattr(cost_item, "IsDecomposedBy", None)
        if is_decomposed_by:
            for rel in is_decomposed_by:
                if rel.is_a("IfcRelNests"):
                    result.extend(rel.RelatedObjects)
    else:
        for rel in is_nested_by or []:
            result.extend(rel.RelatedObjects)
    return result


def get_schedule_cost_items(cost_schedule):
    for cost_item in get_root_cost_items(cost_schedule):
        yield cost_item
        yield from get_all_nested_cost_items(cost_item)


class CostValueUnserialiser:
    def parse(self, formula):
        l = lark.Lark("""start: formula
                    formula: operand (operator operand)*
                    operand: value | category "(" formula ")"
                    value: NUMBER?
                    category: WORD?
                    operator: add | divide | multiply | subtract
                    add: "+"
                    divide: "/"
                    multiply: "*"
                    subtract: "-"

                    DIGIT: "0".."9"
                    INT: DIGIT+
                    SIGNED_INT: ["+"|"-"] INT
                    DECIMAL: INT "." INT? | "." INT
                    _EXP: ("e"|"E") SIGNED_INT
                    FLOAT: INT _EXP | DECIMAL _EXP?
                    NUMBER: FLOAT | INT
                    LCASE_LETTER: "a".."z"
                    UCASE_LETTER: "A".."Z"
                    LETTER: UCASE_LETTER | LCASE_LETTER
                    WORD: LETTER+
                    WS: /[ \\t\\f\\r\\n]/+

                    %ignore WS
                 """)
        start = l.parse(formula)
        return self._get_formula(start.children[0])

    def _get_formula(self, formula):
        if len(formula.children) == 1:
            return self._get_operand(formula.children[0])
        results = {"Components": []}
        for child in formula.children:
            if child.data == "operand":
                results["Components"].append(self._get_operand(child))
            elif child.data == "operator":
                results["ArithmeticOperator"] = self._get_operator(child)
        return results

    def _get_operand(self, operand):
        child = operand.children[0]
        if child.data == "value":
            value = self._get_value(child)
            return {"AppliedValue": float(value) if value else None}
        elif child.data == "category":
            data = {}
            category = self._get_category(child)
            if category:
                if category.lower() == "sum":
                    category = "*"
                data["Category"] = category
            formula = self._get_formula(operand.children[1])
            if formula.get("Components"):
                data["Components"] = formula["Components"]
                data["ArithmeticOperator"] = formula["ArithmeticOperator"]
            else:
                data["AppliedValue"] = formula["AppliedValue"]
            return data

    def _get_value(self, value):
        if value.children:
            return value.children[0].value

    def _get_category(self, category):
        if category.children:
            return category.children[0].value

    def _get_operator(self, operator):
        return operator.children[0].data.upper()
