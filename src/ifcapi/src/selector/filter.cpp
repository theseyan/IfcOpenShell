// SPDX-License-Identifier: LGPL-3.0-or-later

#include "selector/internal.h"

#include <algorithm>
#include <unordered_set>

/* ====================================================================
 *  Internal types for filter evaluation
 * ==================================================================== */

namespace {

/* --- Comparison infrastructure --- */

struct ParsedComparison {
    bool is_negated = false;
    enum BaseOp { EQ, GTE, LTE, GT, LT, CONTAINS } base_op = EQ;
};

struct FilterValue {
    enum Kind { STRING, REGEX, NULL_VAL, BOOL_TRUE, BOOL_FALSE } kind = NULL_VAL;
    std::string str;
    std::regex regex_pat;
};

using ElemSet = std::unordered_set<express::Base>;

/* --- AST node shortcuts --- */

static ifcsel_node_kind nkind(const ifcopenshell_selector_node_t* n) {
    return ifcopenshell_selector_node_kind(n);
}
static size_t ncount(const ifcopenshell_selector_node_t* n) {
    return ifcopenshell_selector_node_child_count(n);
}
static const ifcopenshell_selector_node_t* nchild(const ifcopenshell_selector_node_t* n, size_t i) {
    return ifcopenshell_selector_node_child(n, i);
}
static const char* ntext(const ifcopenshell_selector_node_t* n) {
    return ifcopenshell_selector_node_text(n);
}

/* Find first child with a given kind, or nullptr. */
static const ifcopenshell_selector_node_t* find_child(
    const ifcopenshell_selector_node_t* parent, ifcsel_node_kind kind)
{
    for (size_t i = 0; i < ncount(parent); ++i) {
        auto c = nchild(parent, i);
        if (c && nkind(c) == kind) return c;
    }
    return {};
}

/* Get the text of the first token child (for class names, GlobalIds, etc.) */
static std::string first_token_text(const ifcopenshell_selector_node_t* node) {
    if (!node || ncount(node) == 0) return "";
    auto tok = nchild(node, 0);
    const char* txt = ntext(tok);
    return txt ? txt : "";
}

/* --- Extract comparison from IFCSEL_NODE_COMPARISON node --- */

static ParsedComparison extract_comparison(const ifcopenshell_selector_node_t* comp_node) {
    ParsedComparison result;
    if (!comp_node || ncount(comp_node) == 0) return result;

    auto first = nchild(comp_node, 0);
    if (nkind(first) == IFCSEL_NODE_NOT) {
        result.is_negated = true;
        if (ncount(comp_node) < 2) return result;
        first = nchild(comp_node, 1);
    }

    switch (nkind(first)) {
        case IFCSEL_NODE_EQUALS:          result.base_op = ParsedComparison::EQ;       break;
        case IFCSEL_NODE_MORETHANEQUALTO: result.base_op = ParsedComparison::GTE;      break;
        case IFCSEL_NODE_LESSTHANEQUALTO: result.base_op = ParsedComparison::LTE;      break;
        case IFCSEL_NODE_MORETHAN:        result.base_op = ParsedComparison::GT;       break;
        case IFCSEL_NODE_LESSTHAN:        result.base_op = ParsedComparison::LT;       break;
        case IFCSEL_NODE_CONTAINS:        result.base_op = ParsedComparison::CONTAINS; break;
        default: break;
    }
    return result;
}

/* --- Extract FilterValue from value/pset/prop node --- */

static std::string unescape_filter_quoted(const std::string& raw) {
    /* raw includes outer quotes; strip them and unescape \" */
    if (raw.size() < 2) return "";
    std::string out;
    bool prev_backslash = false;
    for (size_t i = 1; i + 1 < raw.size(); ++i) {
        if (prev_backslash) {
            out += raw[i];
            prev_backslash = false;
        } else if (raw[i] == '\\') {
            prev_backslash = true;
        } else {
            out += raw[i];
        }
    }
    return out;
}

static FilterValue extract_filter_value(const ifcopenshell_selector_node_t* val_node) {
    FilterValue fv;
    if (!val_node || ncount(val_node) == 0) return fv;

    const auto child = nchild(val_node, 0);
    if (!child) return fv;

    switch (nkind(child)) {
        case IFCSEL_NODE_UNQUOTED_STRING: {
            fv.kind = FilterValue::STRING;
            fv.str = first_token_text(child);
            break;
        }
        case IFCSEL_NODE_QUOTED_STRING: {
            fv.kind = FilterValue::STRING;
            fv.str = unescape_filter_quoted(first_token_text(child));
            break;
        }
        case IFCSEL_NODE_REGEX_STRING: {
            fv.kind = FilterValue::REGEX;
            fv.str = first_token_text(child);
            try { fv.regex_pat = std::regex(fv.str); }
            catch (...) { fv.kind = FilterValue::STRING; }
            break;
        }
        case IFCSEL_NODE_SPECIAL: {
            if (ncount(child) > 0) {
                switch (nkind(nchild(child, 0))) {
                    case IFCSEL_NODE_NULL:  fv.kind = FilterValue::NULL_VAL;   break;
                    case IFCSEL_NODE_TRUE:  fv.kind = FilterValue::BOOL_TRUE;  break;
                    case IFCSEL_NODE_FALSE: fv.kind = FilterValue::BOOL_FALSE; break;
                    default: break;
                }
            }
            break;
        }
        default:
            break;
    }
    return fv;
}

/* --- Comparison logic ---
 *
 *  compare_base: apply the base operator, ignoring is_negated.
 *  compare_full: apply base then negate if is_negated.
 *
 *  Both recurse into LIST values.
 */

static bool compare_base(const Val* elem_val,
                         ParsedComparison::BaseOp base_op,
                         const FilterValue& fv);

static bool compare_full(const Val* elem_val,
                         const ParsedComparison& cmp,
                         const FilterValue& fv);

static bool compare_base(const Val* elem_val,
                         ParsedComparison::BaseOp base_op,
                         const FilterValue& fv)
{
    /* LIST: recurse, any match */
    if (elem_val && elem_val->kind == IFCSEL_VALUE_LIST) {
        for (auto item : elem_val->list_val) {
            /* Build a ParsedComparison with is_negated=false for recursion */
            ParsedComparison sub; sub.base_op = base_op; sub.is_negated = false;
            if (compare_full(item, sub, fv)) return true;
        }
        return false;
    }

    /* None element value */
    if (!elem_val || elem_val->kind == IFCSEL_VALUE_NONE) {
        switch (fv.kind) {
            case FilterValue::NULL_VAL:  return true;  /* None is None */
            case FilterValue::REGEX:     return false; /* can't match None */
            default:                     return false; /* None != str/bool */
        }
    }

    /* Regex filter value */
    if (fv.kind == FilterValue::REGEX) {
        if (elem_val->kind == IFCSEL_VALUE_STRING)
            return regex_match_start(elem_val->s_val, fv.regex_pat);
        return false;
    }

    /* NULL filter value */
    if (fv.kind == FilterValue::NULL_VAL)
        return false; /* elem_val is not None */

    /* Bool filter value */
    if (fv.kind == FilterValue::BOOL_TRUE || fv.kind == FilterValue::BOOL_FALSE) {
        if (elem_val->kind != IFCSEL_VALUE_BOOL) return false;
        bool expected = (fv.kind == FilterValue::BOOL_TRUE);
        return elem_val->b_val == expected;
    }

    /* String filter value — try numeric coercion first (Python semantics) */
    if (elem_val->kind == IFCSEL_VALUE_INT) {
        try {
            int64_t iv = std::stoll(fv.str);
            double ev = static_cast<double>(elem_val->i_val);
            double cv = static_cast<double>(iv);
            switch (base_op) {
                case ParsedComparison::GTE: return ev >= cv;
                case ParsedComparison::LTE: return ev <= cv;
                case ParsedComparison::GT:  return ev > cv;
                case ParsedComparison::LT:  return ev < cv;
                default:                    return ev == cv;
            }
        } catch (...) { return false; }
    }

    if (elem_val->kind == IFCSEL_VALUE_DOUBLE) {
        try {
            double cv = std::stod(fv.str);
            double ev = elem_val->d_val;
            switch (base_op) {
                case ParsedComparison::GTE: return ev >= cv;
                case ParsedComparison::LTE: return ev <= cv;
                case ParsedComparison::GT:  return ev > cv;
                case ParsedComparison::LT:  return ev < cv;
                default:                    return ev == cv;
            }
        } catch (...) { return false; }
    }

    if (elem_val->kind == IFCSEL_VALUE_STRING) {
        if (base_op == ParsedComparison::CONTAINS)
            return elem_val->s_val.find(fv.str) != std::string::npos;
        return elem_val->s_val == fv.str;
    }

    /* Bool element with string filter: Python returns False */
    return false;
}

static bool compare_full(const Val* elem_val,
                         const ParsedComparison& cmp,
                         const FilterValue& fv)
{
    if (elem_val && elem_val->kind == IFCSEL_VALUE_LIST) {
        for (auto item : elem_val->list_val)
            if (compare_full(item, cmp, fv)) return true;
        return false;
    }
    bool r = compare_base(elem_val, cmp.base_op, fv);
    return cmp.is_negated ? !r : r;
}

/* Convenience: build a transient string Val (stack-allocated safe to pass as ptr). */
struct StringVal : Val {
    explicit StringVal(const std::string& s) { kind = IFCSEL_VALUE_STRING; s_val = s; }
};
struct NoneVal : Val {
    NoneVal() { kind = IFCSEL_VALUE_NONE; }
};

/* ====================================================================
 *  Element set helpers
 * ==================================================================== */

/* Populate elements with all default elements (IfcProduct + IfcTypeProduct),
   or with base_elements if provided.  Only does so once. */
static void ensure_default_elements(
    ifcopenshell::file* file,
    const ElemSet* base,
    ElemSet& elements,
    bool& has_additive)
{
    if (has_additive) return;
    has_additive = true;
    if (base) {
        elements.insert(base->begin(), base->end());
    } else {
        try {
            auto prods = file->instances_by_type("IfcProduct");
            for (auto e : prods) if (e) elements.insert(e);
        } catch (...) {}
        try {
            auto types = file->instances_by_type("IfcTypeProduct");
            for (auto e : types) if (e) elements.insert(e);
        } catch (...) {}
    }
}

/* ====================================================================
 *  Per-facet filter helpers
 * ==================================================================== */

/* ENTITY facet: adds or removes elements by IFC class */
static void apply_entity_facet(
    ifcopenshell::file* file,
    const ElemSet* base,
    const ifcopenshell_selector_node_t* entity_node,
    ElemSet& elements,
    bool& has_additive)
{
    has_additive = true;

    bool is_not = (ncount(entity_node) > 0 &&
                   nkind(nchild(entity_node, 0)) == IFCSEL_NODE_NOT);
    const auto class_node = is_not ? nchild(entity_node, 1) : nchild(entity_node, 0);
    if (!class_node) return;

    /* class_node is IFCSEL_NODE_IFC_CLASS; its first child is the text token */
    std::string class_name = first_token_text(class_node);
    if (class_name.empty()) return;

    if (base) {
        if (is_not) {
            for (auto it = elements.begin(); it != elements.end(); ) {
                if ((*it).declaration().is(class_name.c_str())) it = elements.erase(it);
                else ++it;
            }
        } else {
            for (auto e : *base)
                if (e && e.declaration().is(class_name.c_str()))
                    elements.insert(e);
        }
    } else {
        try {
            auto entities = file->instances_by_type(class_name);
            if (entities.empty()) return;
            if (is_not) {
                for (auto e : entities) if (e) elements.erase(e);
            } else {
                for (auto e : entities) if (e) elements.insert(e);
            }
        } catch (...) {}
    }
}

/* INSTANCE facet: adds or removes element by GlobalId */
static void apply_instance_facet(
    ifcopenshell::file* file,
    ifcopenshell_file_t* file_h,
    const ElemSet* base,
    const ifcopenshell_selector_node_t* inst_node,
    ElemSet& elements,
    bool& has_additive)
{
    has_additive = true;

    bool is_not = (ncount(inst_node) > 0 &&
                   nkind(nchild(inst_node, 0)) == IFCSEL_NODE_NOT);
    const auto guid_node = is_not ? nchild(inst_node, 1) : nchild(inst_node, 0);
    if (!guid_node) return;

    std::string guid = first_token_text(guid_node);
    if (guid.empty()) return;

    if (base) {
        if (is_not) {
            for (auto it = elements.begin(); it != elements.end(); ) {
                std::string g = get_string_attr(*it, "GlobalId");
                if (g == guid) it = elements.erase(it);
                else ++it;
            }
        } else {
            for (auto e : *base) {
                if (e && get_string_attr(e, "GlobalId") == guid)
                    elements.insert(e);
            }
        }
    } else {
        auto e = file ? file->instance_by_guid(guid) : express::Base();
        if (!e) return;
        if (is_not) elements.erase(e);
        else elements.insert(e);
    }
}

/* ATTRIBUTE facet: filters by a named attribute value */
static void apply_attribute_facet(
    const ifcopenshell_selector_node_t* attr_node,
    ElemSet& elements)
{
    const auto name_node  = find_child(attr_node, IFCSEL_NODE_ATTRIBUTE_NAME);
    const auto comp_node  = find_child(attr_node, IFCSEL_NODE_COMPARISON);
    const auto value_node = find_child(attr_node, IFCSEL_NODE_VALUE);
    if (!name_node || !comp_node || !value_node) return;

    std::string attr_name = first_token_text(name_node);
    ParsedComparison cmp = extract_comparison(comp_node);
    FilterValue fv = extract_filter_value(value_node);

    ElemSet result;
    for (auto e : elements) {
        Val* elem_val = {};
        if (attr_name == "PredefinedType") {
            elem_val = resolve_predefined_type(e);
        } else {
            elem_val = read_attr(e, attr_name);
            if (!elem_val) elem_val = make_none();
        }
        bool match = compare_full(elem_val, cmp, fv);
        delete elem_val;
        if (match) result.insert(e);
    }
    elements = std::move(result);
}

/* TYPE facet: filters by element type's Name or GlobalId */
static void apply_type_facet(
    const ifcopenshell_selector_node_t* type_node,
    ElemSet& elements)
{
    const auto comp_node  = find_child(type_node, IFCSEL_NODE_COMPARISON);
    const auto value_node = find_child(type_node, IFCSEL_NODE_VALUE);
    if (!comp_node || !value_node) return;

    ParsedComparison cmp = extract_comparison(comp_node);
    FilterValue fv = extract_filter_value(value_node);

    ElemSet result;
    for (auto e : elements) {
        auto type_e = ifcapi::bindings::element_get_type(&e);

        bool match;
        if (!type_e) {
            match = compare_full(nullptr, cmp, fv);
        } else {
            std::string type_name = get_string_attr(*type_e, "Name");
            std::string type_guid = get_string_attr(*type_e, "GlobalId");
            StringVal nv(type_name), gv(type_guid);
            const Val* nv_ptr = type_name.empty() ? static_cast<const Val*>(nullptr)
                                                    : static_cast<const Val*>(&nv);
            const Val* gv_ptr = type_guid.empty() ? static_cast<const Val*>(nullptr)
                                                    : static_cast<const Val*>(&gv);
            match = compare_full(nv_ptr, cmp, fv) || compare_full(gv_ptr, cmp, fv);
        }
        if (match) result.insert(e);
    }
    elements = std::move(result);
}

/* MATERIAL facet: filters by material name or category */
static std::vector<express::Base> expand_to_materials(express::Base mat) {
    std::vector<express::Base> result;
    if (!mat) return result;
    if (entity_is_a(mat, "IfcMaterial")) {
        result.push_back(mat);
    } else if (entity_is_a(mat, "IfcMaterialLayerSet")) {
        for (auto lay : get_entity_list(mat, "MaterialLayers")) {
            auto m = get_entity_ref(lay, "Material");
            if (m) result.push_back(m);
        }
    } else if (entity_is_a(mat, "IfcMaterialProfileSet")) {
        for (auto pr : get_entity_list(mat, "MaterialProfiles")) {
            auto m = get_entity_ref(pr, "Material");
            if (m) result.push_back(m);
        }
    } else if (entity_is_a(mat, "IfcMaterialConstituentSet")) {
        for (auto co : get_entity_list(mat, "MaterialConstituents")) {
            auto m = get_entity_ref(co, "Material");
            if (m) result.push_back(m);
        }
    } else if (entity_is_a(mat, "IfcMaterialList")) {
        for (auto m : get_entity_list(mat, "Materials"))
            result.push_back(m);
    }
    return result;
}

static void apply_material_facet(
    const ifcopenshell_selector_node_t* mat_node,
    ElemSet& elements)
{
    const auto comp_node  = find_child(mat_node, IFCSEL_NODE_COMPARISON);
    const auto value_node = find_child(mat_node, IFCSEL_NODE_VALUE);
    if (!comp_node || !value_node) return;

    ParsedComparison cmp = extract_comparison(comp_node);
    FilterValue fv = extract_filter_value(value_node);

    ElemSet result;
    for (auto e : elements) {
        auto mat = ifcapi::bindings::element_get_material(&e, {true, true});
        auto materials = mat ? expand_to_materials(*mat) : std::vector<express::Base>{};

        bool filter_result;
        if (materials.empty()) {
            /* No materials: compare against None */
            filter_result = compare_full(nullptr, cmp, fv);
        } else {
            /* For each material, OR together Name and Category matches.
             * For non-equals operators the final result is inverted. */
            bool any_triggered = false;
            for (auto m : materials) {
                std::string name = get_string_attr(m, "Name");
                StringVal nv(name);
                if (compare_full(name.empty() ? nullptr : static_cast<Val*>(&nv), cmp, fv))
                    any_triggered = true;

                std::string cat = get_string_attr(m, "Category");
                StringVal cv(cat);
                const Val* cat_ptr = cat.empty() ? nullptr : static_cast<Val*>(&cv);
                if (compare_full(cat_ptr, cmp, fv))
                    any_triggered = true;
            }
            filter_result = cmp.is_negated ? !any_triggered : any_triggered;
        }
        if (filter_result) result.insert(e);
    }
    elements = std::move(result);
}

/* PROPERTY facet: filters by pset.prop */
static void apply_property_facet(
    const ifcopenshell_selector_node_t* prop_facet_node,
    ElemSet& elements)
{
    const auto pset_node  = find_child(prop_facet_node, IFCSEL_NODE_PSET);
    const auto prop_node  = find_child(prop_facet_node, IFCSEL_NODE_PROP);
    const auto comp_node  = find_child(prop_facet_node, IFCSEL_NODE_COMPARISON);
    const auto value_node = find_child(prop_facet_node, IFCSEL_NODE_VALUE);
    if (!pset_node || !prop_node || !comp_node || !value_node) return;

    FilterValue pset_fv = extract_filter_value(pset_node);
    FilterValue prop_fv = extract_filter_value(prop_node);
    ParsedComparison cmp = extract_comparison(comp_node);
    FilterValue fv = extract_filter_value(value_node);

    bool pset_is_regex = (pset_fv.kind == FilterValue::REGEX);
    bool prop_is_regex = (prop_fv.kind == FilterValue::REGEX);

    ElemSet result;
    for (auto e : elements) {
        auto psets = get_all_psets(e);

        /* prop_found: whether we reached a prop match (and should use comparison_result).
         * When false, Python falls back to compare(None, comparison, value). */
        bool prop_found = false;
        bool comparison_result = false;

        /* Helper to free remaining psets */
        auto free_remaining = [&](size_t from_idx) {
            for (size_t ri = from_idx; ri < psets.size(); ++ri) delete psets[ri].second;
        };

        if (!pset_is_regex && !prop_is_regex) {
            /* str.str: Python's get_pset(elem, pset, prop) — returns value or None */
            for (size_t pi = 0; pi < psets.size(); ++pi) {
                auto& kv = psets[pi];
                if (kv.first != pset_fv.str) { delete kv.second; continue; }
                /* Pset found: look up prop (nullptr → None) */
                prop_found = true;
                Val* prop_val = {};
                for (auto pv : kv.second->dict_val)
                    if (pv.first == prop_fv.str) { prop_val = pv.second; break; }
                comparison_result = compare_full(prop_val, cmp, fv);
                delete kv.second;
                free_remaining(pi + 1);
                break;
            }
        } else if (!pset_is_regex && prop_is_regex) {
            /* str.regex: return compare(first matching prop) or compare(None) */
            for (size_t pi = 0; pi < psets.size(); ++pi) {
                auto& kv = psets[pi];
                if (kv.first != pset_fv.str) { delete kv.second; continue; }
                for (auto pv : kv.second->dict_val) {
                    if (regex_match_start(pv.first, prop_fv.regex_pat)) {
                        prop_found = true;
                        comparison_result = compare_full(pv.second, cmp, fv);
                        break;
                    }
                }
                delete kv.second;
                free_remaining(pi + 1);
                break;
            }
        } else if (pset_is_regex && !prop_is_regex) {
            /* regex.str */
            for (size_t pi = 0; pi < psets.size(); ++pi) {
                auto& kv = psets[pi];
                if (!regex_match_start(kv.first, pset_fv.regex_pat)) { delete kv.second; continue; }
                Val* prop_val = {};
                for (auto pv : kv.second->dict_val)
                    if (pv.first == prop_fv.str) { prop_val = pv.second; break; }
                if (prop_val) {
                    prop_found = true;
                    comparison_result = compare_full(prop_val, cmp, fv);
                    delete kv.second;
                    free_remaining(pi + 1);
                    break;
                }
                delete kv.second;
            }
        } else {
            /* regex.regex */
            for (size_t pi = 0; pi < psets.size(); ++pi) {
                auto& kv = psets[pi];
                if (!regex_match_start(kv.first, pset_fv.regex_pat)) { delete kv.second; continue; }
                for (auto pv : kv.second->dict_val) {
                    if (regex_match_start(pv.first, prop_fv.regex_pat)) {
                        prop_found = true;
                        comparison_result = compare_full(pv.second, cmp, fv);
                        break;
                    }
                }
                delete kv.second;
                if (prop_found) { free_remaining(pi + 1); break; }
            }
        }

        bool filter_result = prop_found ? comparison_result
                                        : compare_full(nullptr, cmp, fv);
        if (filter_result) result.insert(e);
    }
    elements = std::move(result);
}

/* CLASSIFICATION facet */
static void apply_classification_facet(
    const ifcopenshell_selector_node_t* cls_node,
    ElemSet& elements)
{
    const auto comp_node  = find_child(cls_node, IFCSEL_NODE_COMPARISON);
    const auto value_node = find_child(cls_node, IFCSEL_NODE_VALUE);
    if (!comp_node || !value_node) return;

    ParsedComparison cmp = extract_comparison(comp_node);
    FilterValue fv = extract_filter_value(value_node);

    ElemSet result;
    for (auto e : elements) {
        std::vector<express::Base> refs;
        for (auto rel : get_inverse_list(e, "HasAssociations")) {
            if (!entity_is_a(rel, "IfcRelAssociatesClassification")) continue;
            auto ref = get_entity_ref(rel, "RelatingClassification");
            if (ref) refs.push_back(ref);
        }

        bool filter_result;
        if (refs.empty()) {
            filter_result = compare_full(nullptr, cmp, fv);
        } else {
            bool any_triggered = false;
            for (auto ref : refs) {
                std::string ref_name = get_string_attr(ref, "Name");
                StringVal nv(ref_name);
                if (compare_full(ref_name.empty() ? nullptr : static_cast<Val*>(&nv), cmp, fv))
                    any_triggered = true;

                /* IFC4: Identification; IFC2X3: ItemReference */
                std::string ident = get_string_attr(ref, "Identification");
                if (ident.empty()) ident = get_string_attr(ref, "ItemReference");
                StringVal iv(ident);
                const Val* id_ptr = ident.empty() ? nullptr : static_cast<Val*>(&iv);
                if (compare_full(id_ptr, cmp, fv)) any_triggered = true;
            }
            filter_result = cmp.is_negated ? !any_triggered : any_triggered;
        }
        if (filter_result) result.insert(e);
    }
    elements = std::move(result);
}

/* LOCATION facet: checks the spatial container tree */
static std::vector<express::Base> get_container_tree(express::Base container) {
    std::vector<express::Base> tree;
    while (container) {
        if (entity_is_a(container, "IfcProject")) break;
        tree.push_back(container);
        auto aggregate = ifcapi::bindings::element_get_aggregate(&container);
        container = aggregate.value_or(express::Base());
    }
    return tree;
}

static void apply_location_facet(
    const ifcopenshell_selector_node_t* loc_node,
    ElemSet& elements)
{
    const auto comp_node  = find_child(loc_node, IFCSEL_NODE_COMPARISON);
    const auto value_node = find_child(loc_node, IFCSEL_NODE_VALUE);
    if (!comp_node || !value_node) return;

    ParsedComparison cmp = extract_comparison(comp_node);
    FilterValue fv = extract_filter_value(value_node);

    /* Location uses equality check on Name/GlobalId, then applies outer negation */
    ParsedComparison inner_cmp; /* is_negated=false, base_op=EQ */

    ElemSet result;
    for (auto e : elements) {
        /* Get direct spatial container */
        auto container = ifcapi::bindings::element_get_container(&e, {false, {}});

        /* Fall back to aggregate parent if no spatial container */
        if (!container) {
            container = ifcapi::bindings::element_get_aggregate(&e);
        }

        auto containers = container ? get_container_tree(*container) : std::vector<express::Base>{};

        bool filter_result;
        if (containers.empty()) {
            filter_result = compare_full(nullptr, cmp, fv);
        } else {
            bool any_match = false;
            for (auto c : containers) {
                std::string cname = get_string_attr(c, "Name");
                std::string cguid = get_string_attr(c, "GlobalId");
                StringVal nv(cname), gv(cguid);
                if (compare_full(cname.empty() ? nullptr : static_cast<Val*>(&nv), inner_cmp, fv))
                    any_match = true;
                if (compare_full(cguid.empty() ? nullptr : static_cast<Val*>(&gv), inner_cmp, fv))
                    any_match = true;
            }
            filter_result = cmp.is_negated ? !any_match : any_match;
        }
        if (filter_result) result.insert(e);
    }
    elements = std::move(result);
}

/* GROUP facet */
static void apply_group_facet(
    const ifcopenshell_selector_node_t* grp_node,
    ElemSet& elements)
{
    const auto comp_node  = find_child(grp_node, IFCSEL_NODE_COMPARISON);
    const auto value_node = find_child(grp_node, IFCSEL_NODE_VALUE);
    if (!comp_node || !value_node) return;

    ParsedComparison cmp = extract_comparison(comp_node);
    FilterValue fv = extract_filter_value(value_node);

    ParsedComparison inner_cmp; /* is_negated=false, EQ */

    ElemSet result;
    for (auto e : elements) {
        bool any_match = false;
        for (auto rel : get_inverse_list(e, "HasAssignments")) {
            if (!entity_is_a(rel, "IfcRelAssignsToGroup")) continue;
            auto grp = get_entity_ref(rel, "RelatingGroup");
            if (!grp) continue;

            std::string gname = get_string_attr(grp, "Name");
            std::string gguid = get_string_attr(grp, "GlobalId");
            StringVal nv(gname), gv(gguid);
            if (compare_full(gname.empty() ? nullptr : static_cast<Val*>(&nv), inner_cmp, fv))
                any_match = true;
            if (compare_full(gguid.empty() ? nullptr : static_cast<Val*>(&gv), inner_cmp, fv))
                any_match = true;
        }
        bool filter_result = cmp.is_negated ? !any_match : any_match;
        if (filter_result) result.insert(e);
    }
    elements = std::move(result);
}

/* PARENT facet */
static void apply_parent_facet(
    ifcopenshell::file* file,
    const ifcopenshell_selector_node_t* parent_node,
    ElemSet& elements)
{
    const auto comp_node  = find_child(parent_node, IFCSEL_NODE_COMPARISON);
    const auto value_node = find_child(parent_node, IFCSEL_NODE_VALUE);
    if (!comp_node || !value_node) return;

    ParsedComparison cmp = extract_comparison(comp_node);
    FilterValue fv = extract_filter_value(value_node);

    /* Collect matching parents */
    ElemSet parents;

    auto check_parent = [&](express::Base parent) {
        if (!parent) return;
        std::string pname = get_string_attr(parent, "Name");
        std::string pguid = get_string_attr(parent, "GlobalId");
        StringVal nv(pname), gv(pguid);
        if (compare_full(pname.empty() ? nullptr : static_cast<Val*>(&nv), cmp, fv) ||
            compare_full(pguid.empty() ? nullptr : static_cast<Val*>(&gv), cmp, fv))
            parents.insert(parent);
    };

    try {
        auto rels = file->instances_by_type("IfcRelAggregates");
        for (auto rel : rels) {
            auto p = get_entity_ref(rel, "RelatingObject");
            check_parent(p);
        }
    } catch (...) {}

    try {
        auto rels = file->instances_by_type("IfcRelContainedInSpatialStructure");
        for (auto rel : rels) {
            auto p = get_entity_ref(rel, "RelatingStructure");
            check_parent(p);
        }
    } catch (...) {}

    try {
        auto rels = file->instances_by_type("IfcRelNests");
        for (auto rel : rels) {
            auto p = get_entity_ref(rel, "RelatingObject");
            check_parent(p);
        }
    } catch (...) {}

    try {
        auto rels = file->instances_by_type("IfcRelVoidsElement");
        for (auto rel : rels) {
            auto p = get_entity_ref(rel, "RelatingBuildingElement");
            check_parent(p);
        }
    } catch (...) {}

    try {
        auto rels = file->instances_by_type("IfcRelFillsElement");
        for (auto rel : rels) {
            auto p = get_entity_ref(rel, "RelatingOpeningElement");
            check_parent(p);
        }
    } catch (...) {}

    /* Get all children of matched parents */
    ElemSet children;
    for (auto parent : parents) {
        auto decomposed = ifcapi::bindings::element_get_decomposition(&parent, {true});
        if (!decomposed.empty()) {
            for (auto child : decomposed)
                if (child) children.insert(child);
        }
    }

    ElemSet related = parents;
    related.insert(children.begin(), children.end());

    ElemSet result;
    if (cmp.is_negated) {
        for (auto e : elements)
            if (related.find(e) == related.end()) result.insert(e);
    } else {
        for (auto e : elements)
            if (related.find(e) != related.end()) result.insert(e);
    }
    elements = std::move(result);
}

/* QUERY facet: evaluates a key-path query and compares the result */
static void apply_query_facet(
    ifcopenshell::file* file,
    const ifcopenshell_selector_node_t* qfacet_node,
    ElemSet& elements)
{
    const auto keys_node  = find_child(qfacet_node, IFCSEL_NODE_KEYS);
    const auto comp_node  = find_child(qfacet_node, IFCSEL_NODE_COMPARISON);
    const auto value_node = find_child(qfacet_node, IFCSEL_NODE_VALUE);
    if (!keys_node || !comp_node || !value_node) return;

    /* The keys node holds a simple value (string path like "type.Name") */
    FilterValue keys_fv = extract_filter_value(keys_node);
    ParsedComparison cmp = extract_comparison(comp_node);
    FilterValue fv = extract_filter_value(value_node);

    if (keys_fv.str.empty()) return;

    /* Parse the key-path as a get_element query */
    ifcopenshell_selector_node_t* ast =
        ifcopenshell_selector_parse_get_element(keys_fv.str.c_str());
    if (!ast) return;

    std::vector<KeyEntry> keys = extract_keys(ast);
    ifcopenshell_selector_node_free(ast);

    ElemSet result;
    for (auto e : elements) {
        Val* elem_val = get_element_value_impl(file, e, keys);
        bool match = compare_full(elem_val, cmp, fv);
        delete elem_val;
        if (match) result.insert(e);
    }
    elements = std::move(result);
}

/* ====================================================================
 *  Facet list evaluation
 * ==================================================================== */

static void process_facet(
    ifcopenshell::file* file,
    ifcopenshell_file_t* file_h,
    const ElemSet* base,
    const ifcopenshell_selector_node_t* facet_child,
    ElemSet& elements,
    bool& has_additive)
{
    switch (nkind(facet_child)) {
        case IFCSEL_NODE_ENTITY:
            apply_entity_facet(file, base, facet_child, elements, has_additive);
            break;
        case IFCSEL_NODE_INSTANCE:
            apply_instance_facet(file, file_h, base, facet_child, elements, has_additive);
            break;
        case IFCSEL_NODE_ATTRIBUTE:
            ensure_default_elements(file, base, elements, has_additive);
            apply_attribute_facet(facet_child, elements);
            break;
        case IFCSEL_NODE_TYPE_FACET:
            ensure_default_elements(file, base, elements, has_additive);
            apply_type_facet(facet_child, elements);
            break;
        case IFCSEL_NODE_MATERIAL_FACET:
            ensure_default_elements(file, base, elements, has_additive);
            apply_material_facet(facet_child, elements);
            break;
        case IFCSEL_NODE_PROPERTY:
            ensure_default_elements(file, base, elements, has_additive);
            apply_property_facet(facet_child, elements);
            break;
        case IFCSEL_NODE_CLASSIFICATION:
            ensure_default_elements(file, base, elements, has_additive);
            apply_classification_facet(facet_child, elements);
            break;
        case IFCSEL_NODE_LOCATION:
            ensure_default_elements(file, base, elements, has_additive);
            apply_location_facet(facet_child, elements);
            break;
        case IFCSEL_NODE_GROUP:
            ensure_default_elements(file, base, elements, has_additive);
            apply_group_facet(facet_child, elements);
            break;
        case IFCSEL_NODE_PARENT:
            ensure_default_elements(file, base, elements, has_additive);
            apply_parent_facet(file, facet_child, elements);
            break;
        case IFCSEL_NODE_QUERY_FACET:
            ensure_default_elements(file, base, elements, has_additive);
            apply_query_facet(file, facet_child, elements);
            break;
        default:
            break;
    }
}

static ElemSet process_facet_list(
    ifcopenshell::file* file,
    ifcopenshell_file_t* file_h,
    const ElemSet* base,
    const ifcopenshell_selector_node_t* fl_node)
{
    ElemSet elements;
    bool has_additive = false;

    for (size_t i = 0; i < ncount(fl_node); ++i) {
        const auto facet_node = nchild(fl_node, i);
        if (!facet_node || nkind(facet_node) != IFCSEL_NODE_FACET) continue;

        /* A FACET node has one child: the actual facet type node */
        if (ncount(facet_node) == 0) continue;
        const auto facet_child = nchild(facet_node, 0);
        if (!facet_child) continue;

        process_facet(file, file_h, base, facet_child, elements, has_additive);
    }
    return elements;
}

/* Walk start → filter_group → facet_list; return union of all facet-list results. */
static ElemSet filter_elements_impl(
    ifcopenshell::file* file,
    ifcopenshell_file_t* file_h,
    const ElemSet* base,
    const ifcopenshell_selector_node_t* ast)
{
    ElemSet total;

    /* Recurse into any node looking for FILTER_GROUP → FACET_LIST */
    std::function<void(const ifcopenshell_selector_node_t*)> walk =
        [&](const ifcopenshell_selector_node_t* node) {
            if (!node) return;
            auto k = nkind(node);
            if (k == IFCSEL_NODE_FACET_LIST) {
                ElemSet fl = process_facet_list(file, file_h, base, node);
                if (!fl.empty()) total.insert(fl.begin(), fl.end());
                return;
            }
            for (size_t i = 0; i < ncount(node); ++i)
                walk(nchild(node, i));
        };

    walk(ast);
    return total;
}

} /* anonymous namespace */

namespace ifcapi {
namespace bindings {

std::optional<ifcopenshell_selector_value_t*> selector_filter_all(ifcopenshell::file* file, const std::string& query)
{
    return selector_filter_elements(file, query, {});
}

std::optional<ifcopenshell_selector_value_t*> selector_filter_elements(
    ifcopenshell::file* file,
    const std::string& query,
    const std::vector<express::Base>& elements)
{
    if (!file) {
        ifcopenshell::capi::set_last_error("filter_elements: null argument");
        return {};
    }
    if (query.empty()) {
        /* Empty query: return provided elements or empty list */
        auto result = make_list();
        for (auto element : elements)
            if (element)
                result->list_val.push_back(make_instance(element));
        return result;
    }

    ifcopenshell_selector_node_t* ast = selector_parse_filter(query);
    if (!ast) return {};

    /* Build base set from supplied elements array (or nullptr for whole-file) */
    ElemSet base_set;
    const ElemSet* base_ptr = {};
    if (!elements.empty()) {
        for (auto element : elements)
            if (element)
                base_set.insert(element);
        base_ptr = &base_set;
    }

    ElemSet result_set;
    ifcopenshell_file_t file_handle{file, false};
    try {
        result_set = filter_elements_impl(file, &file_handle, base_ptr, ast);
    } catch (const std::exception& ex) {
        selector_node_free(ast);
        ifcopenshell::capi::set_last_error(ex.what());
        return {};
    } catch (...) {
        selector_node_free(ast);
        ifcopenshell::capi::set_last_error("filter_elements: unknown exception");
        return {};
    }

    selector_node_free(ast);

    auto out = make_list();
    for (auto e : result_set)
        out->list_val.push_back(make_instance(e));
    return out;
}

} // namespace bindings
} // namespace ifcapi
