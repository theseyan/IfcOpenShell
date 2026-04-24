// SPDX-License-Identifier: LGPL-3.0-or-later
//
// Hand-written recursive-descent parser for the three selector query grammars
// (filter_elements, get_element, format). Produces an opaque C AST consumed
// through the C ABI declared in ifcapi/selector_ast.h.

#include "ifcapi/selector/ast.h"
#include "ifcopenshell_api_internal.hpp"

#include <cctype>
#include <cstring>
#include <memory>
#include <string>
#include <vector>

namespace {

// ============================================================
//  Node implementation
// ============================================================

struct Node {
    ifcsel_node_kind                kind;
    std::string                     text;       // non-empty for token nodes
    std::vector<Node*>              children;

    bool is_token() const { return kind >= IFCSEL_TOKEN_FIRST; }
};

Node* make_node(ifcsel_node_kind kind) {
    auto* n  = new Node;
    n->kind  = kind;
    return n;
}

Node* make_token(ifcsel_node_kind kind, const std::string& text) {
    auto* n  = new Node;
    n->kind  = kind;
    n->text  = text;
    return n;
}

void node_add(Node* parent, Node* child) {
    if (child) parent->children.push_back(child);
}

void node_free_impl(Node* n) {
    if (!n) return;
    for (auto* c : n->children) node_free_impl(c);
    delete n;
}

// ============================================================
//  Parser state
// ============================================================

struct PS {
    const char* input;
    size_t      pos;
    size_t      len;
    std::string error;

    explicit PS(const char* s) : input(s), pos(0), len(s ? std::strlen(s) : 0) {}

    bool at_end()            const { return pos >= len; }
    char cur()               const { return pos < len ? input[pos] : '\0'; }
    char peek(size_t off = 0)const { return (pos + off) < len ? input[pos + off] : '\0'; }
    void advance(size_t n=1)       { pos = (pos + n <= len) ? pos + n : len; }

    void skip_ws() {
        while (pos < len && (input[pos] == ' '  || input[pos] == '\t' ||
                             input[pos] == '\r'  || input[pos] == '\n' ||
                             input[pos] == '\f'))
            ++pos;
    }

    bool at_str(const char* s) const {
        size_t sl = std::strlen(s);
        if (pos + sl > len) return false;
        return std::memcmp(input + pos, s, sl) == 0;
    }

    bool try_consume(const char* s) {
        if (at_str(s)) { advance(std::strlen(s)); return true; }
        return false;
    }

    bool try_consume_char(char c) {
        if (pos < len && input[pos] == c) { ++pos; return true; }
        return false;
    }

    void fail(const std::string& msg) {
        error = "selector: " + msg + " at offset " + std::to_string(pos);
    }

    bool ok() const { return error.empty(); }
};

// ============================================================
//  Character classification helpers
// ============================================================

static bool is_word(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
           (c >= '0' && c <= '9') || c == '_';
}

static bool is_globalid_body(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
           (c >= '0' && c <= '9') || c == '_' || c == '$';
}

// Returns true if the 22-char slice starting at ps.input+off is a valid globalid.
static bool looks_like_globalid(const PS& ps, size_t off) {
    if (off + 22 > ps.len) return false;
    char first = ps.input[off];
    if (first < '0' || first > '3') return false;
    for (size_t i = 1; i < 22; ++i) {
        if (!is_globalid_body(ps.input[off + i])) return false;
    }
    // Must end here (not be part of a longer word)
    if (off + 22 < ps.len && is_globalid_body(ps.input[off + 22])) return false;
    return true;
}

// Scan a word character sequence from ps.pos without consuming.
static std::string scan_word(const PS& ps) {
    size_t i = ps.pos;
    while (i < ps.len && is_word(ps.input[i])) ++i;
    return std::string(ps.input + ps.pos, i - ps.pos);
}

// Consume an unquoted_string for filter grammar: [^,.=><*!\s]+
static Node* parse_filter_unquoted(PS& ps) {
    size_t start = ps.pos;
    while (ps.pos < ps.len) {
        char c = ps.input[ps.pos];
        if (c == ',' || c == '.' || c == '=' || c == '>' || c == '<' ||
            c == '*' || c == '!' || std::isspace((unsigned char)c)) break;
        ++ps.pos;
    }
    if (ps.pos == start) { ps.fail("expected unquoted string"); return nullptr; }
    auto* n = make_node(IFCSEL_NODE_UNQUOTED_STRING);
    node_add(n, make_token(IFCSEL_TOKEN_ANON,
                           std::string(ps.input + start, ps.pos - start)));
    return n;
}

// Consume an unquoted_string for get_element grammar: [^.=\/\s]+
static Node* parse_get_unquoted(PS& ps) {
    size_t start = ps.pos;
    while (ps.pos < ps.len) {
        char c = ps.input[ps.pos];
        if (c == '.' || c == '=' || c == '/' || std::isspace((unsigned char)c)) break;
        ++ps.pos;
    }
    if (ps.pos == start) { ps.fail("expected unquoted string"); return nullptr; }
    auto* n = make_node(IFCSEL_NODE_UNQUOTED_STRING);
    node_add(n, make_token(IFCSEL_TOKEN_ANON,
                           std::string(ps.input + start, ps.pos - start)));
    return n;
}

// Parse ESCAPED_STRING: "..." with backslash escapes.
static Node* parse_quoted_string(PS& ps) {
    if (ps.cur() != '"') { ps.fail("expected '\"'"); return nullptr; }
    size_t start = ps.pos;
    ++ps.pos;
    bool prev_backslash = false;
    while (ps.pos < ps.len) {
        char c = ps.input[ps.pos];
        if (c == '"' && !prev_backslash) { ++ps.pos; break; }
        prev_backslash = (c == '\\' && !prev_backslash);
        ++ps.pos;
    }
    std::string raw(ps.input + start, ps.pos - start);
    auto* n = make_node(IFCSEL_NODE_QUOTED_STRING);
    node_add(n, make_token(IFCSEL_TOKEN_ESCAPED_STRING, raw));
    return n;
}

// Parse regex_string: /[^\/]+/
static Node* parse_regex_string(PS& ps) {
    if (ps.cur() != '/') { ps.fail("expected '/'"); return nullptr; }
    ++ps.pos;
    size_t start = ps.pos;
    while (ps.pos < ps.len && ps.input[ps.pos] != '/') ++ps.pos;
    if (ps.pos >= ps.len) { ps.fail("unterminated regex string"); return nullptr; }
    std::string inner(ps.input + start, ps.pos - start);
    ++ps.pos;  // consume closing '/'
    auto* n = make_node(IFCSEL_NODE_REGEX_STRING);
    node_add(n, make_token(IFCSEL_TOKEN_ANON, inner));
    return n;
}

// ============================================================
//  Filter grammar
// ============================================================

// Forward declarations
static Node* parse_filter_value(PS& ps);
static Node* parse_filter_comparison(PS& ps);
static Node* parse_filter_pset_or_prop(PS& ps);

static Node* parse_filter_value(PS& ps) {
    ps.skip_ws();
    char c = ps.cur();
    auto* vn = make_node(IFCSEL_NODE_VALUE);

    if (c == '"') {
        auto* qs = parse_quoted_string(ps);
        if (!qs) { delete vn; return nullptr; }
        node_add(vn, qs);
        return vn;
    }
    if (c == '/') {
        auto* rs = parse_regex_string(ps);
        if (!rs) { delete vn; return nullptr; }
        node_add(vn, rs);
        return vn;
    }
    // Try special: NULL, TRUE, FALSE
    if (ps.at_str("NULL") && (ps.pos + 4 >= ps.len || !is_word(ps.input[ps.pos + 4]))) {
        ps.advance(4);
        auto* sp = make_node(IFCSEL_NODE_SPECIAL);
        node_add(sp, make_node(IFCSEL_NODE_NULL));
        node_add(vn, sp);
        return vn;
    }
    if (ps.at_str("TRUE") && (ps.pos + 4 >= ps.len || !is_word(ps.input[ps.pos + 4]))) {
        ps.advance(4);
        auto* sp = make_node(IFCSEL_NODE_SPECIAL);
        node_add(sp, make_node(IFCSEL_NODE_TRUE));
        node_add(vn, sp);
        return vn;
    }
    if (ps.at_str("FALSE") && (ps.pos + 5 >= ps.len || !is_word(ps.input[ps.pos + 5]))) {
        ps.advance(5);
        auto* sp = make_node(IFCSEL_NODE_SPECIAL);
        node_add(sp, make_node(IFCSEL_NODE_FALSE));
        node_add(vn, sp);
        return vn;
    }
    // Fallback: unquoted_string
    auto* us = parse_filter_unquoted(ps);
    if (!us) { delete vn; return nullptr; }
    node_add(vn, us);
    return vn;
}

static Node* parse_filter_comparison(PS& ps) {
    ps.skip_ws();
    auto* cn = make_node(IFCSEL_NODE_COMPARISON);

    // Longest-match order: >=, <=, *=, !=, !*=, =, >, <
    if (ps.try_consume(">=")) {
        node_add(cn, make_node(IFCSEL_NODE_MORETHANEQUALTO));
        return cn;
    }
    if (ps.try_consume("<=")) {
        node_add(cn, make_node(IFCSEL_NODE_LESSTHANEQUALTO));
        return cn;
    }
    if (ps.try_consume("*=")) {
        node_add(cn, make_node(IFCSEL_NODE_CONTAINS));
        return cn;
    }
    if (ps.cur() == '!') {
        size_t saved = ps.pos;
        ps.advance(1);
        ps.skip_ws();
        if (ps.try_consume("*=")) {
            node_add(cn, make_node(IFCSEL_NODE_NOT));
            node_add(cn, make_node(IFCSEL_NODE_CONTAINS));
            return cn;
        }
        if (ps.try_consume("=")) {
            node_add(cn, make_node(IFCSEL_NODE_NOT));
            node_add(cn, make_node(IFCSEL_NODE_EQUALS));
            return cn;
        }
        ps.pos = saved;
        delete cn;
        ps.fail("invalid comparison operator after '!'");
        return nullptr;
    }
    if (ps.try_consume("=")) {
        node_add(cn, make_node(IFCSEL_NODE_EQUALS));
        return cn;
    }
    if (ps.try_consume(">")) {
        node_add(cn, make_node(IFCSEL_NODE_MORETHAN));
        return cn;
    }
    if (ps.try_consume("<")) {
        node_add(cn, make_node(IFCSEL_NODE_LESSTHAN));
        return cn;
    }
    delete cn;
    ps.fail("expected comparison operator");
    return nullptr;
}

// pset or prop: quoted_string | regex_string | unquoted_string
static Node* parse_filter_pset_or_prop(PS& ps) {
    ps.skip_ws();
    char c = ps.cur();
    if (c == '"') return parse_quoted_string(ps);
    if (c == '/') return parse_regex_string(ps);
    return parse_filter_unquoted(ps);
}

// Returns true if current (after WS) is a comparison operator
static bool is_at_comparison(PS& ps) {
    size_t saved = ps.pos;
    ps.skip_ws();
    char c = ps.cur();
    bool result = (c == '=' || c == '>' || c == '<' || c == '*' ||
                   (c == '!' && (ps.peek(1) == '=' || ps.peek(1) == '*')));
    ps.pos = saved;
    return result;
}

// Parse a single facet
static Node* parse_facet(PS& ps);

static Node* parse_facet_list(PS& ps) {
    auto* fl = make_node(IFCSEL_NODE_FACET_LIST);
    Node* f = parse_facet(ps);
    if (!f) { delete fl; return nullptr; }
    node_add(fl, f);
    while (true) {
        size_t saved = ps.pos;
        ps.skip_ws();
        if (ps.cur() != ',') { ps.pos = saved; break; }
        ps.advance(1);
        f = parse_facet(ps);
        if (!f) { delete fl; return nullptr; }
        node_add(fl, f);
    }
    return fl;
}

static Node* parse_filter_group(PS& ps) {
    auto* fg = make_node(IFCSEL_NODE_FILTER_GROUP);
    Node* fl = parse_facet_list(ps);
    if (!fl) { delete fg; return nullptr; }
    node_add(fg, fl);
    while (true) {
        size_t saved = ps.pos;
        ps.skip_ws();
        if (ps.cur() != '+') { ps.pos = saved; break; }
        ps.advance(1);
        fl = parse_facet_list(ps);
        if (!fl) { delete fg; return nullptr; }
        node_add(fg, fl);
    }
    return fg;
}

static Node* parse_facet(PS& ps) {
    ps.skip_ws();

    // Check for negated instance or entity
    bool has_not = false;
    size_t not_pos = ps.pos;
    if (ps.cur() == '!') {
        // Peek ahead: if next non-ws char looks like globalid or Ifc*, it's a negated facet
        size_t tmp = ps.pos + 1;
        while (tmp < ps.len && std::isspace((unsigned char)ps.input[tmp])) ++tmp;
        char nc = (tmp < ps.len) ? ps.input[tmp] : '\0';
        if ((nc >= '0' && nc <= '3') || (nc == 'I' && tmp + 2 < ps.len &&
             ps.input[tmp+1] == 'f' && ps.input[tmp+2] == 'c' && is_word(ps.input[tmp+3 < ps.len ? tmp+3 : 0]))) {
            has_not = true;
            ps.advance(1);  // consume '!'
            ps.skip_ws();
        }
    }

    char c = ps.cur();

    if (!has_not) {
        // Quoted or regex pset → property
        if (c == '"' || c == '/') {
            // property: pset "." prop comparison value
            auto* pset_n = parse_filter_pset_or_prop(ps);
            if (!pset_n) return nullptr;
            ps.skip_ws();
            if (!ps.try_consume_char('.')) {
                node_free_impl(pset_n);
                ps.fail("expected '.' after pset in property facet");
                return nullptr;
            }
            auto* prop_n = parse_filter_pset_or_prop(ps);
            if (!prop_n) { node_free_impl(pset_n); return nullptr; }
            auto* cmp = parse_filter_comparison(ps);
            if (!cmp) { node_free_impl(pset_n); node_free_impl(prop_n); return nullptr; }
            auto* val = parse_filter_value(ps);
            if (!val) { node_free_impl(pset_n); node_free_impl(prop_n); node_free_impl(cmp); return nullptr; }

            auto* pset_wrap = make_node(IFCSEL_NODE_PSET);
            node_add(pset_wrap, pset_n);
            auto* prop_wrap = make_node(IFCSEL_NODE_PROP);
            node_add(prop_wrap, prop_n);

            auto* fn = make_node(IFCSEL_NODE_FACET);
            auto* prop_f = make_node(IFCSEL_NODE_PROPERTY);
            node_add(prop_f, pset_wrap);
            node_add(prop_f, prop_wrap);
            node_add(prop_f, cmp);
            node_add(prop_f, val);
            node_add(fn, prop_f);
            return fn;
        }

        // Keyword or identifier-based facets
        // Scan forward to determine facet type
        size_t ident_start = ps.pos;
        size_t ident_end   = ps.pos;
        while (ident_end < ps.len && is_word(ps.input[ident_end])) ++ident_end;
        std::string ident(ps.input + ident_start, ident_end - ident_start);

        // Check for 'query:' keyword
        if (ident == "query") {
            ps.advance(ident_end - ident_start);
            ps.skip_ws();
            if (!ps.try_consume_char(':')) {
                ps.fail("expected ':' after 'query'");
                return nullptr;
            }
            ps.skip_ws();
            // keys: quoted_string | unquoted_string
            Node* keys_child = nullptr;
            if (ps.cur() == '"')
                keys_child = parse_quoted_string(ps);
            else
                keys_child = parse_filter_unquoted(ps);
            if (!keys_child) return nullptr;

            auto* keys_n = make_node(IFCSEL_NODE_KEYS);
            node_add(keys_n, keys_child);

            auto* cmp = parse_filter_comparison(ps);
            if (!cmp) { node_free_impl(keys_n); return nullptr; }
            auto* val = parse_filter_value(ps);
            if (!val) { node_free_impl(keys_n); node_free_impl(cmp); return nullptr; }

            auto* qf = make_node(IFCSEL_NODE_QUERY_FACET);
            node_add(qf, keys_n);
            node_add(qf, cmp);
            node_add(qf, val);
            auto* fn = make_node(IFCSEL_NODE_FACET);
            node_add(fn, qf);
            return fn;
        }

        // Look past the identifier to decide
        size_t after_ident = ident_end;
        while (after_ident < ps.len && std::isspace((unsigned char)ps.input[after_ident])) ++after_ident;
        char after_c = (after_ident < ps.len) ? ps.input[after_ident] : '\0';

        // Dot after ident → property (pset is an unquoted_string)
        if (after_c == '.') {
            // Could be property OR could be identifier starting with Ifc...
            // Either way, property path: pset "." prop comparison value
            auto* pset_n = parse_filter_pset_or_prop(ps);
            if (!pset_n) return nullptr;
            ps.skip_ws();
            if (!ps.try_consume_char('.')) {
                node_free_impl(pset_n);
                ps.fail("expected '.' in property facet");
                return nullptr;
            }
            auto* prop_n = parse_filter_pset_or_prop(ps);
            if (!prop_n) { node_free_impl(pset_n); return nullptr; }
            auto* cmp = parse_filter_comparison(ps);
            if (!cmp) { node_free_impl(pset_n); node_free_impl(prop_n); return nullptr; }
            auto* val = parse_filter_value(ps);
            if (!val) { node_free_impl(pset_n); node_free_impl(prop_n); node_free_impl(cmp); return nullptr; }

            auto* pset_wrap = make_node(IFCSEL_NODE_PSET);
            node_add(pset_wrap, pset_n);
            auto* prop_wrap = make_node(IFCSEL_NODE_PROP);
            node_add(prop_wrap, prop_n);

            auto* fn = make_node(IFCSEL_NODE_FACET);
            auto* prop_f = make_node(IFCSEL_NODE_PROPERTY);
            node_add(prop_f, pset_wrap);
            node_add(prop_f, prop_wrap);
            node_add(prop_f, cmp);
            node_add(prop_f, val);
            node_add(fn, prop_f);
            return fn;
        }

        // Keyword facets (followed by comparison)
        auto parse_keyword_facet = [&](ifcsel_node_kind nk) -> Node* {
            ps.advance(ident_end - ident_start);
            auto* cmp = parse_filter_comparison(ps);
            if (!cmp) return nullptr;
            auto* val = parse_filter_value(ps);
            if (!val) { node_free_impl(cmp); return nullptr; }
            auto* kf = make_node(nk);
            node_add(kf, cmp);
            node_add(kf, val);
            auto* fn = make_node(IFCSEL_NODE_FACET);
            node_add(fn, kf);
            return fn;
        };

        if (ident == "type")           return parse_keyword_facet(IFCSEL_NODE_TYPE_FACET);
        if (ident == "material")       return parse_keyword_facet(IFCSEL_NODE_MATERIAL_FACET);
        if (ident == "classification") return parse_keyword_facet(IFCSEL_NODE_CLASSIFICATION);
        if (ident == "location")       return parse_keyword_facet(IFCSEL_NODE_LOCATION);
        if (ident == "group")          return parse_keyword_facet(IFCSEL_NODE_GROUP);
        if (ident == "parent")         return parse_keyword_facet(IFCSEL_NODE_PARENT);

        // globalid (no negation here since we checked has_not above)
        if (looks_like_globalid(ps, ps.pos)) {
            std::string gid(ps.input + ps.pos, 22);
            ps.advance(22);
            auto* gid_n = make_node(IFCSEL_NODE_GLOBALID);
            node_add(gid_n, make_token(IFCSEL_TOKEN_ANON, gid));
            auto* inst = make_node(IFCSEL_NODE_INSTANCE);
            node_add(inst, gid_n);
            auto* fn = make_node(IFCSEL_NODE_FACET);
            node_add(fn, inst);
            return fn;
        }

        // ifc_class: Ifc\w+
        if (ident.size() >= 3 && ident[0] == 'I' && ident[1] == 'f' && ident[2] == 'c') {
            ps.advance(ident_end - ident_start);
            auto* cls_n = make_node(IFCSEL_NODE_IFC_CLASS);
            node_add(cls_n, make_token(IFCSEL_TOKEN_ANON, ident));
            auto* ent = make_node(IFCSEL_NODE_ENTITY);
            node_add(ent, cls_n);
            auto* fn = make_node(IFCSEL_NODE_FACET);
            node_add(fn, ent);
            return fn;
        }

        // attribute_name: [A-Z]\w+ followed by comparison
        if (!ident.empty() && std::isupper((unsigned char)ident[0])) {
            ps.advance(ident_end - ident_start);
            auto* an = make_node(IFCSEL_NODE_ATTRIBUTE_NAME);
            node_add(an, make_token(IFCSEL_TOKEN_ANON, ident));
            auto* cmp = parse_filter_comparison(ps);
            if (!cmp) { node_free_impl(an); return nullptr; }
            auto* val = parse_filter_value(ps);
            if (!val) { node_free_impl(an); node_free_impl(cmp); return nullptr; }
            auto* attr = make_node(IFCSEL_NODE_ATTRIBUTE);
            node_add(attr, an);
            node_add(attr, cmp);
            node_add(attr, val);
            auto* fn = make_node(IFCSEL_NODE_FACET);
            node_add(fn, attr);
            return fn;
        }

        ps.fail("unrecognised facet");
        return nullptr;
    }

    // has_not == true: must be negated instance or entity
    if (looks_like_globalid(ps, ps.pos)) {
        std::string gid(ps.input + ps.pos, 22);
        ps.advance(22);
        auto* gid_n = make_node(IFCSEL_NODE_GLOBALID);
        node_add(gid_n, make_token(IFCSEL_TOKEN_ANON, gid));
        auto* inst = make_node(IFCSEL_NODE_INSTANCE);
        node_add(inst, make_node(IFCSEL_NODE_NOT));
        node_add(inst, gid_n);
        auto* fn = make_node(IFCSEL_NODE_FACET);
        node_add(fn, inst);
        return fn;
    }
    // ifc_class after '!'
    std::string w = scan_word(ps);
    if (w.size() >= 3 && w[0] == 'I' && w[1] == 'f' && w[2] == 'c') {
        ps.advance(w.size());
        auto* cls_n = make_node(IFCSEL_NODE_IFC_CLASS);
        node_add(cls_n, make_token(IFCSEL_TOKEN_ANON, w));
        auto* ent = make_node(IFCSEL_NODE_ENTITY);
        node_add(ent, make_node(IFCSEL_NODE_NOT));
        node_add(ent, cls_n);
        auto* fn = make_node(IFCSEL_NODE_FACET);
        node_add(fn, ent);
        return fn;
    }
    ps.fail("expected globalid or ifc_class after '!'");
    return nullptr;
}

// ============================================================
//  Get-element grammar
// ============================================================

static Node* parse_get_key(PS& ps) {
    ps.skip_ws();
    char c = ps.cur();
    Node* child = nullptr;
    if (c == '"')  child = parse_quoted_string(ps);
    else if (c == '/') child = parse_regex_string(ps);
    else           child = parse_get_unquoted(ps);
    if (!child) return nullptr;
    auto* kn = make_node(IFCSEL_NODE_KEY);
    node_add(kn, child);
    return kn;
}

static Node* parse_get_keys(PS& ps) {
    auto* kl = make_node(IFCSEL_NODE_KEYS_LIST);
    Node* k = parse_get_key(ps);
    if (!k) { delete kl; return nullptr; }
    node_add(kl, k);
    while (true) {
        size_t saved = ps.pos;
        ps.skip_ws();
        if (ps.cur() != '.') { ps.pos = saved; break; }
        ps.advance(1);
        k = parse_get_key(ps);
        if (!k) { delete kl; return nullptr; }
        node_add(kl, k);
    }
    return kl;
}

// ============================================================
//  Format grammar helpers
// ============================================================

// Parse a SIGNED_NUMBER token: ["+"|"-"]? (INT | DECIMAL | FLOAT)
// Returns nullptr if no number found.
static Node* try_parse_signed_number(PS& ps) {
    size_t start = ps.pos;

    // Optional sign
    if (ps.cur() == '+' || ps.cur() == '-') ps.advance(1);

    if (ps.pos >= ps.len || !std::isdigit((unsigned char)ps.cur())) {
        ps.pos = start;
        return nullptr;
    }
    // Integer part
    while (ps.pos < ps.len && std::isdigit((unsigned char)ps.cur())) ps.advance(1);
    // Optional decimal part
    if (ps.cur() == '.' && ps.pos + 1 < ps.len && std::isdigit((unsigned char)ps.input[ps.pos + 1])) {
        ps.advance(1);  // '.'
        while (ps.pos < ps.len && std::isdigit((unsigned char)ps.cur())) ps.advance(1);
    } else if (ps.cur() == '.') {
        // just "3." without trailing digit — keep just the integer
    }
    // Optional exponent
    if (ps.cur() == 'e' || ps.cur() == 'E') {
        ps.advance(1);
        if (ps.cur() == '+' || ps.cur() == '-') ps.advance(1);
        while (ps.pos < ps.len && std::isdigit((unsigned char)ps.cur())) ps.advance(1);
    }
    std::string text(ps.input + start, ps.pos - start);
    return make_token(IFCSEL_TOKEN_SIGNED_NUMBER, text);
}

// Parse a NUMBER (unsigned): INT | DECIMAL | FLOAT
static Node* try_parse_number(PS& ps) {
    size_t start = ps.pos;
    if (ps.pos >= ps.len || !std::isdigit((unsigned char)ps.cur())) return nullptr;
    while (ps.pos < ps.len && std::isdigit((unsigned char)ps.cur())) ps.advance(1);
    if (ps.cur() == '.') {
        ps.advance(1);
        while (ps.pos < ps.len && std::isdigit((unsigned char)ps.cur())) ps.advance(1);
    }
    if (ps.cur() == 'e' || ps.cur() == 'E') {
        ps.advance(1);
        if (ps.cur() == '+' || ps.cur() == '-') ps.advance(1);
        while (ps.pos < ps.len && std::isdigit((unsigned char)ps.cur())) ps.advance(1);
    }
    return make_token(IFCSEL_TOKEN_NUMBER, std::string(ps.input + start, ps.pos - start));
}

// Parse a SIGNED_INT: ["+"|"-"]? INT
static Node* try_parse_signed_int(PS& ps) {
    size_t start = ps.pos;
    if (ps.cur() == '+' || ps.cur() == '-') ps.advance(1);
    if (ps.pos >= ps.len || !std::isdigit((unsigned char)ps.cur())) {
        ps.pos = start;
        return nullptr;
    }
    while (ps.pos < ps.len && std::isdigit((unsigned char)ps.cur())) ps.advance(1);
    return make_token(IFCSEL_TOKEN_SIGNED_INT, std::string(ps.input + start, ps.pos - start));
}

// Forward declarations for format grammar
static Node* parse_fmt_expression(PS& ps);
static Node* parse_fmt_function(PS& ps);

static Node* parse_fmt_mul_div(PS& ps) {
    Node* left = parse_fmt_function(ps);
    if (!left) return nullptr;
    while (true) {
        size_t saved = ps.pos;
        ps.skip_ws();
        char op = ps.cur();
        if (op != '*' && op != '/') { ps.pos = saved; break; }
        ps.advance(1);
        Node* right = parse_fmt_function(ps);
        if (!right) { node_free_impl(left); return nullptr; }
        ifcsel_node_kind nk = (op == '*') ? IFCSEL_NODE_MULTIPLY : IFCSEL_NODE_DIVIDE;
        auto* binop = make_node(nk);
        node_add(binop, left);
        node_add(binop, right);
        left = binop;
    }
    return left;
}

static Node* parse_fmt_add_sub(PS& ps) {
    Node* left = parse_fmt_mul_div(ps);
    if (!left) return nullptr;
    while (true) {
        size_t saved = ps.pos;
        ps.skip_ws();
        char op = ps.cur();
        if (op != '+' && op != '-') { ps.pos = saved; break; }
        // Disambiguate: if '-' or '+' is followed by a digit and we are inside
        // an expression (left has been parsed), it is an operator.
        // If it is the start of a SIGNED_NUMBER we let parse_fmt_function handle it.
        // Since we already have 'left', any +/- here is an operator.
        ps.advance(1);
        Node* right = parse_fmt_mul_div(ps);
        if (!right) { node_free_impl(left); return nullptr; }
        ifcsel_node_kind nk = (op == '+') ? IFCSEL_NODE_ADD : IFCSEL_NODE_SUBTRACT;
        auto* binop = make_node(nk);
        node_add(binop, left);
        node_add(binop, right);
        left = binop;
    }
    return left;
}

static Node* parse_fmt_expression(PS& ps) {
    return parse_fmt_add_sub(ps);
}

// Helper: parse a named function call starting with keyword (already at '(')
// ps is positioned just after the keyword, expects to see optional-WS '(' next
static bool consume_open_paren(PS& ps) {
    ps.skip_ws();
    return ps.try_consume_char('(');
}
static bool consume_close_paren(PS& ps) {
    ps.skip_ws();
    return ps.try_consume_char(')');
}
static bool consume_comma(PS& ps) {
    ps.skip_ws();
    return ps.try_consume_char(',');
}

static Node* parse_fmt_boolean(PS& ps) {
    ps.skip_ws();
    if (ps.at_str("true") && (ps.pos + 4 >= ps.len || !is_word(ps.input[ps.pos + 4]))) {
        ps.advance(4);
        auto* bn = make_node(IFCSEL_NODE_BOOLEAN);
        node_add(bn, make_token(IFCSEL_TOKEN_TRUE, "true"));
        return bn;
    }
    if (ps.at_str("True") && (ps.pos + 4 >= ps.len || !is_word(ps.input[ps.pos + 4]))) {
        ps.advance(4);
        auto* bn = make_node(IFCSEL_NODE_BOOLEAN);
        node_add(bn, make_token(IFCSEL_TOKEN_TRUE, "True"));
        return bn;
    }
    if (ps.at_str("TRUE") && (ps.pos + 4 >= ps.len || !is_word(ps.input[ps.pos + 4]))) {
        ps.advance(4);
        auto* bn = make_node(IFCSEL_NODE_BOOLEAN);
        node_add(bn, make_token(IFCSEL_TOKEN_TRUE, "TRUE"));
        return bn;
    }
    if (ps.at_str("false") && (ps.pos + 5 >= ps.len || !is_word(ps.input[ps.pos + 5]))) {
        ps.advance(5);
        auto* bn = make_node(IFCSEL_NODE_BOOLEAN);
        node_add(bn, make_token(IFCSEL_TOKEN_FALSE, "false"));
        return bn;
    }
    if (ps.at_str("False") && (ps.pos + 5 >= ps.len || !is_word(ps.input[ps.pos + 5]))) {
        ps.advance(5);
        auto* bn = make_node(IFCSEL_NODE_BOOLEAN);
        node_add(bn, make_token(IFCSEL_TOKEN_FALSE, "False"));
        return bn;
    }
    if (ps.at_str("FALSE") && (ps.pos + 5 >= ps.len || !is_word(ps.input[ps.pos + 5]))) {
        ps.advance(5);
        auto* bn = make_node(IFCSEL_NODE_BOOLEAN);
        node_add(bn, make_token(IFCSEL_TOKEN_FALSE, "FALSE"));
        return bn;
    }
    return nullptr;
}

static Node* parse_fmt_function(PS& ps) {
    ps.skip_ws();

    char c = ps.cur();

    // Parenthesised expression: "(" expression ")"
    if (c == '(') {
        ps.advance(1);
        Node* inner = parse_fmt_expression(ps);
        if (!inner) return nullptr;
        ps.skip_ws();
        if (!ps.try_consume_char(')')) {
            node_free_impl(inner);
            ps.fail("expected ')'");
            return nullptr;
        }
        auto* fn = make_node(IFCSEL_NODE_FUNCTION);
        node_add(fn, inner);
        return fn;
    }

    // ESCAPED_STRING
    if (c == '"') {
        auto* tok = parse_quoted_string(ps);  // returns IFCSEL_NODE_QUOTED_STRING
        if (!tok) return nullptr;
        // Extract the ESCAPED_STRING token from inside quoted_string
        Node* es_tok = nullptr;
        if (!tok->children.empty()) {
            es_tok = tok->children[0];
            tok->children.clear();
        }
        delete tok;
        if (!es_tok) return nullptr;
        auto* fn = make_node(IFCSEL_NODE_FUNCTION);
        node_add(fn, es_tok);
        return fn;
    }

    // Variable: {{ query_path }}
    if (c == '{' && ps.peek(1) == '{') {
        ps.advance(2);
        size_t start = ps.pos;
        while (ps.pos < ps.len && !(ps.cur() == '}' && ps.peek(1) == '}')) ++ps.pos;
        if (ps.pos >= ps.len) { ps.fail("unterminated variable"); return nullptr; }
        std::string inner_text(ps.input + start, ps.pos - start);
        ps.advance(2);  // consume "}}"
        auto* qp = make_node(IFCSEL_NODE_QUERY_PATH);
        node_add(qp, make_token(IFCSEL_TOKEN_ANON, inner_text));
        auto* var = make_node(IFCSEL_NODE_VARIABLE);
        node_add(var, qp);
        auto* fn = make_node(IFCSEL_NODE_FUNCTION);
        node_add(fn, var);
        return fn;
    }

    // Named functions — try each keyword
    struct KwFunc {
        const char* kw;
        ifcsel_node_kind nk;
    };

    // We use lambdas below for each function type since they have varying signatures.
    // First check if at a keyword followed immediately by '('
    auto at_func = [&](const char* kw) -> bool {
        size_t kl = std::strlen(kw);
        if (!ps.at_str(kw)) return false;
        size_t tmp = ps.pos + kl;
        while (tmp < ps.len && std::isspace((unsigned char)ps.input[tmp])) ++tmp;
        return tmp < ps.len && ps.input[tmp] == '(';
    };

    if (at_func("round")) {
        ps.advance(std::strlen("round"));
        if (!consume_open_paren(ps)) { ps.fail("expected '(' after round"); return nullptr; }
        Node* expr = parse_fmt_expression(ps);
        if (!expr) return nullptr;
        if (!consume_comma(ps)) { node_free_impl(expr); ps.fail("expected ',' in round"); return nullptr; }
        ps.skip_ws();
        Node* num = try_parse_number(ps);
        if (!num) { node_free_impl(expr); ps.fail("expected NUMBER in round"); return nullptr; }
        if (!consume_close_paren(ps)) {
            node_free_impl(expr); node_free_impl(num);
            ps.fail("expected ')' after round"); return nullptr;
        }
        auto* rn = make_node(IFCSEL_NODE_ROUND);
        node_add(rn, expr);
        node_add(rn, num);
        auto* fn = make_node(IFCSEL_NODE_FUNCTION);
        node_add(fn, rn);
        return fn;
    }

    if (at_func("number")) {
        ps.advance(std::strlen("number"));
        if (!consume_open_paren(ps)) { ps.fail("expected '(' after number"); return nullptr; }
        Node* expr = parse_fmt_expression(ps);
        if (!expr) return nullptr;
        auto* nfn = make_node(IFCSEL_NODE_NUMBER_FN);
        node_add(nfn, expr);
        // Optional: ["," ESCAPED_STRING ["," ESCAPED_STRING]]
        size_t saved = ps.pos;
        ps.skip_ws();
        if (ps.cur() == ',') {
            ps.advance(1);
            ps.skip_ws();
            if (ps.cur() == '"') {
                auto* qs1 = parse_quoted_string(ps);
                if (!qs1) { node_free_impl(nfn); return nullptr; }
                // Extract token
                Node* es1 = qs1->children.empty() ? nullptr : qs1->children[0];
                qs1->children.clear(); delete qs1;
                if (es1) node_add(nfn, es1);
                // Second optional string
                saved = ps.pos;
                ps.skip_ws();
                if (ps.cur() == ',') {
                    ps.advance(1);
                    ps.skip_ws();
                    if (ps.cur() == '"') {
                        auto* qs2 = parse_quoted_string(ps);
                        if (!qs2) { node_free_impl(nfn); return nullptr; }
                        Node* es2 = qs2->children.empty() ? nullptr : qs2->children[0];
                        qs2->children.clear(); delete qs2;
                        if (es2) node_add(nfn, es2);
                    } else {
                        ps.pos = saved;
                    }
                }
            } else {
                ps.pos = saved;
            }
        }
        if (!consume_close_paren(ps)) { node_free_impl(nfn); ps.fail("expected ')' after number"); return nullptr; }
        auto* fn = make_node(IFCSEL_NODE_FUNCTION);
        node_add(fn, nfn);
        return fn;
    }

    if (at_func("int")) {
        ps.advance(std::strlen("int"));
        if (!consume_open_paren(ps)) { ps.fail("expected '(' after int"); return nullptr; }
        Node* expr = parse_fmt_expression(ps);
        if (!expr) return nullptr;
        if (!consume_close_paren(ps)) { node_free_impl(expr); ps.fail("expected ')' after int"); return nullptr; }
        auto* ifn = make_node(IFCSEL_NODE_INT_FN);
        node_add(ifn, expr);
        auto* fn = make_node(IFCSEL_NODE_FUNCTION);
        node_add(fn, ifn);
        return fn;
    }

    if (at_func("metric_length")) {
        ps.advance(std::strlen("metric_length"));
        if (!consume_open_paren(ps)) { ps.fail("expected '(' after metric_length"); return nullptr; }
        Node* expr = parse_fmt_expression(ps);
        if (!expr) return nullptr;
        if (!consume_comma(ps)) { node_free_impl(expr); ps.fail("expected ',' in metric_length"); return nullptr; }
        ps.skip_ws();
        Node* num1 = try_parse_number(ps);
        if (!num1) { node_free_impl(expr); ps.fail("expected NUMBER in metric_length"); return nullptr; }
        if (!consume_comma(ps)) { node_free_impl(expr); node_free_impl(num1); ps.fail("expected ',' in metric_length"); return nullptr; }
        ps.skip_ws();
        Node* num2 = try_parse_number(ps);
        if (!num2) { node_free_impl(expr); node_free_impl(num1); ps.fail("expected NUMBER in metric_length"); return nullptr; }
        if (!consume_close_paren(ps)) {
            node_free_impl(expr); node_free_impl(num1); node_free_impl(num2);
            ps.fail("expected ')' after metric_length"); return nullptr;
        }
        auto* ml = make_node(IFCSEL_NODE_METRIC_LENGTH);
        node_add(ml, expr);
        node_add(ml, num1);
        node_add(ml, num2);
        auto* fl = make_node(IFCSEL_NODE_FORMAT_LENGTH);
        node_add(fl, ml);
        auto* fn = make_node(IFCSEL_NODE_FUNCTION);
        node_add(fn, fl);
        return fn;
    }

    if (at_func("imperial_length")) {
        ps.advance(std::strlen("imperial_length"));
        if (!consume_open_paren(ps)) { ps.fail("expected '(' after imperial_length"); return nullptr; }
        Node* expr = parse_fmt_expression(ps);
        if (!expr) return nullptr;
        if (!consume_comma(ps)) { node_free_impl(expr); ps.fail("expected ',' in imperial_length"); return nullptr; }
        ps.skip_ws();
        Node* num = try_parse_number(ps);
        if (!num) { node_free_impl(expr); ps.fail("expected NUMBER in imperial_length"); return nullptr; }

        auto* il = make_node(IFCSEL_NODE_IMPERIAL_LENGTH);
        node_add(il, expr);
        node_add(il, num);

        // Optional: "," ESCAPED_STRING "," ESCAPED_STRING ["," boolean]
        size_t saved2 = ps.pos;
        ps.skip_ws();
        if (ps.cur() == ',') {
            ps.advance(1);
            ps.skip_ws();
            if (ps.cur() == '"') {
                auto* qs1 = parse_quoted_string(ps);
                if (!qs1) { node_free_impl(il); return nullptr; }
                Node* es1 = qs1->children.empty() ? nullptr : qs1->children[0];
                qs1->children.clear(); delete qs1;

                if (!consume_comma(ps)) { node_free_impl(il); if (es1) delete es1; ps.fail("expected ',' in imperial_length"); return nullptr; }
                ps.skip_ws();
                if (ps.cur() != '"') { node_free_impl(il); if (es1) delete es1; ps.fail("expected string in imperial_length"); return nullptr; }
                auto* qs2 = parse_quoted_string(ps);
                if (!qs2) { node_free_impl(il); if (es1) delete es1; return nullptr; }
                Node* es2 = qs2->children.empty() ? nullptr : qs2->children[0];
                qs2->children.clear(); delete qs2;

                if (es1) node_add(il, es1);
                if (es2) node_add(il, es2);

                // Optional boolean
                saved2 = ps.pos;
                ps.skip_ws();
                if (ps.cur() == ',') {
                    ps.advance(1);
                    Node* bln = parse_fmt_boolean(ps);
                    if (bln) {
                        node_add(il, bln);
                    } else {
                        ps.pos = saved2;
                    }
                }
            } else {
                // Could be a boolean directly (grammar has optional fields)
                Node* bln = parse_fmt_boolean(ps);
                if (bln) {
                    node_add(il, bln);
                } else {
                    ps.pos = saved2;
                }
            }
        }

        if (!consume_close_paren(ps)) { node_free_impl(il); ps.fail("expected ')' after imperial_length"); return nullptr; }
        auto* fl = make_node(IFCSEL_NODE_FORMAT_LENGTH);
        node_add(fl, il);
        auto* fn = make_node(IFCSEL_NODE_FUNCTION);
        node_add(fn, fl);
        return fn;
    }

    if (at_func("lower")) {
        ps.advance(std::strlen("lower"));
        if (!consume_open_paren(ps)) return nullptr;
        Node* expr = parse_fmt_expression(ps);
        if (!expr) return nullptr;
        if (!consume_close_paren(ps)) { node_free_impl(expr); ps.fail("expected ')'"); return nullptr; }
        auto* ln = make_node(IFCSEL_NODE_LOWER);
        node_add(ln, expr);
        auto* fn = make_node(IFCSEL_NODE_FUNCTION);
        node_add(fn, ln);
        return fn;
    }

    if (at_func("upper")) {
        ps.advance(std::strlen("upper"));
        if (!consume_open_paren(ps)) return nullptr;
        Node* expr = parse_fmt_expression(ps);
        if (!expr) return nullptr;
        if (!consume_close_paren(ps)) { node_free_impl(expr); ps.fail("expected ')'"); return nullptr; }
        auto* un = make_node(IFCSEL_NODE_UPPER);
        node_add(un, expr);
        auto* fn = make_node(IFCSEL_NODE_FUNCTION);
        node_add(fn, un);
        return fn;
    }

    if (at_func("title")) {
        ps.advance(std::strlen("title"));
        if (!consume_open_paren(ps)) return nullptr;
        Node* expr = parse_fmt_expression(ps);
        if (!expr) return nullptr;
        if (!consume_close_paren(ps)) { node_free_impl(expr); ps.fail("expected ')'"); return nullptr; }
        auto* tn = make_node(IFCSEL_NODE_TITLE);
        node_add(tn, expr);
        auto* fn = make_node(IFCSEL_NODE_FUNCTION);
        node_add(fn, tn);
        return fn;
    }

    if (at_func("concat")) {
        ps.advance(std::strlen("concat"));
        if (!consume_open_paren(ps)) return nullptr;
        auto* cn = make_node(IFCSEL_NODE_CONCAT);
        Node* expr = parse_fmt_expression(ps);
        if (!expr) { node_free_impl(cn); return nullptr; }
        node_add(cn, expr);
        while (true) {
            size_t sv = ps.pos;
            ps.skip_ws();
            if (ps.cur() != ',') { ps.pos = sv; break; }
            ps.advance(1);
            Node* e2 = parse_fmt_expression(ps);
            if (!e2) { node_free_impl(cn); return nullptr; }
            node_add(cn, e2);
        }
        if (!consume_close_paren(ps)) { node_free_impl(cn); ps.fail("expected ')'"); return nullptr; }
        auto* fn = make_node(IFCSEL_NODE_FUNCTION);
        node_add(fn, cn);
        return fn;
    }

    if (at_func("substr")) {
        ps.advance(std::strlen("substr"));
        if (!consume_open_paren(ps)) return nullptr;
        Node* expr = parse_fmt_expression(ps);
        if (!expr) return nullptr;
        if (!consume_comma(ps)) { node_free_impl(expr); ps.fail("expected ',' in substr"); return nullptr; }
        ps.skip_ws();
        Node* si1 = try_parse_signed_int(ps);
        if (!si1) { node_free_impl(expr); ps.fail("expected SIGNED_INT in substr"); return nullptr; }
        auto* sn = make_node(IFCSEL_NODE_SUBSTR);
        node_add(sn, expr);
        node_add(sn, si1);
        // Optional second SIGNED_INT
        size_t sv = ps.pos;
        ps.skip_ws();
        if (ps.cur() == ',') {
            ps.advance(1);
            ps.skip_ws();
            Node* si2 = try_parse_signed_int(ps);
            if (si2) {
                node_add(sn, si2);
            } else {
                ps.pos = sv;
            }
        } else {
            ps.pos = sv;
        }
        if (!consume_close_paren(ps)) { node_free_impl(sn); ps.fail("expected ')'"); return nullptr; }
        auto* fn = make_node(IFCSEL_NODE_FUNCTION);
        node_add(fn, sn);
        return fn;
    }

    if (at_func("sort")) {
        ps.advance(std::strlen("sort"));
        if (!consume_open_paren(ps)) return nullptr;
        Node* expr = parse_fmt_expression(ps);
        if (!expr) return nullptr;
        if (!consume_close_paren(ps)) { node_free_impl(expr); ps.fail("expected ')'"); return nullptr; }
        auto* sn = make_node(IFCSEL_NODE_SORT);
        node_add(sn, expr);
        auto* fn = make_node(IFCSEL_NODE_FUNCTION);
        node_add(fn, sn);
        return fn;
    }

    if (at_func("reverse")) {
        ps.advance(std::strlen("reverse"));
        if (!consume_open_paren(ps)) return nullptr;
        Node* expr = parse_fmt_expression(ps);
        if (!expr) return nullptr;
        if (!consume_close_paren(ps)) { node_free_impl(expr); ps.fail("expected ')'"); return nullptr; }
        auto* rn = make_node(IFCSEL_NODE_REVERSE);
        node_add(rn, expr);
        auto* fn = make_node(IFCSEL_NODE_FUNCTION);
        node_add(fn, rn);
        return fn;
    }

    if (at_func("join")) {
        ps.advance(std::strlen("join"));
        if (!consume_open_paren(ps)) return nullptr;
        ps.skip_ws();
        if (ps.cur() != '"') { ps.fail("expected ESCAPED_STRING as first arg to join"); return nullptr; }
        auto* qs = parse_quoted_string(ps);
        if (!qs) return nullptr;
        Node* es = qs->children.empty() ? nullptr : qs->children[0];
        qs->children.clear(); delete qs;
        if (!consume_comma(ps)) { if (es) delete es; ps.fail("expected ','"); return nullptr; }
        Node* expr = parse_fmt_expression(ps);
        if (!expr) { if (es) delete es; return nullptr; }
        if (!consume_close_paren(ps)) { if (es) delete es; node_free_impl(expr); ps.fail("expected ')'"); return nullptr; }
        auto* jn = make_node(IFCSEL_NODE_JOIN);
        if (es) node_add(jn, es);
        node_add(jn, expr);
        auto* fn = make_node(IFCSEL_NODE_FUNCTION);
        node_add(fn, jn);
        return fn;
    }

    // SIGNED_NUMBER: optional sign followed by digits (must attempt before failing)
    {
        size_t saved = ps.pos;
        Node* sn = try_parse_signed_number(ps);
        if (sn) {
            auto* fn = make_node(IFCSEL_NODE_FUNCTION);
            node_add(fn, sn);
            return fn;
        }
        ps.pos = saved;
    }

    ps.fail("unexpected token in format expression");
    return nullptr;
}

} // anonymous namespace

// ============================================================
//  Public C ABI
// ============================================================

extern "C" {

ifcopenshell_selector_node_t*
ifcopenshell_selector_parse_filter(const char* query) {
    if (!query) {
        ifcopenshell::capi::set_last_error("selector: null input");
        return nullptr;
    }
    PS ps(query);
    ps.skip_ws();
    if (ps.at_end()) {
        ifcopenshell::capi::set_last_error("selector: empty filter query");
        return nullptr;
    }
    Node* fg = parse_filter_group(ps);
    if (!fg || !ps.ok()) {
        node_free_impl(fg);
        ifcopenshell::capi::set_last_error(ps.error.empty() ? "selector: parse error" : ps.error);
        return nullptr;
    }
    ps.skip_ws();
    if (!ps.at_end()) {
        node_free_impl(fg);
        ifcopenshell::capi::set_last_error("selector: unexpected trailing input at offset " +
                                            std::to_string(ps.pos));
        return nullptr;
    }
    auto* root = make_node(IFCSEL_NODE_START);
    node_add(root, fg);
    return reinterpret_cast<ifcopenshell_selector_node_t*>(root);
}

ifcopenshell_selector_node_t*
ifcopenshell_selector_parse_get_element(const char* query) {
    if (!query) {
        ifcopenshell::capi::set_last_error("selector: null input");
        return nullptr;
    }
    PS ps(query);
    ps.skip_ws();
    if (ps.at_end()) {
        ifcopenshell::capi::set_last_error("selector: empty get_element query");
        return nullptr;
    }
    Node* keys = parse_get_keys(ps);
    if (!keys || !ps.ok()) {
        node_free_impl(keys);
        ifcopenshell::capi::set_last_error(ps.error.empty() ? "selector: parse error" : ps.error);
        return nullptr;
    }
    ps.skip_ws();
    if (!ps.at_end()) {
        node_free_impl(keys);
        ifcopenshell::capi::set_last_error("selector: unexpected trailing input at offset " +
                                            std::to_string(ps.pos));
        return nullptr;
    }
    auto* root = make_node(IFCSEL_NODE_START);
    node_add(root, keys);
    return reinterpret_cast<ifcopenshell_selector_node_t*>(root);
}

ifcopenshell_selector_node_t*
ifcopenshell_selector_parse_format(const char* query) {
    if (!query) {
        ifcopenshell::capi::set_last_error("selector: null input");
        return nullptr;
    }
    PS ps(query);
    ps.skip_ws();
    if (ps.at_end()) {
        ifcopenshell::capi::set_last_error("selector: empty format query");
        return nullptr;
    }
    Node* expr = parse_fmt_expression(ps);
    if (!expr || !ps.ok()) {
        node_free_impl(expr);
        ifcopenshell::capi::set_last_error(ps.error.empty() ? "selector: parse error" : ps.error);
        return nullptr;
    }
    ps.skip_ws();
    if (!ps.at_end()) {
        node_free_impl(expr);
        ifcopenshell::capi::set_last_error("selector: unexpected trailing input at offset " +
                                            std::to_string(ps.pos));
        return nullptr;
    }
    auto* root = make_node(IFCSEL_NODE_START);
    node_add(root, expr);
    return reinterpret_cast<ifcopenshell_selector_node_t*>(root);
}

ifcsel_node_kind
ifcopenshell_selector_node_kind(const ifcopenshell_selector_node_t* node) {
    if (!node) return IFCSEL_TOKEN_ANON;
    return reinterpret_cast<const Node*>(node)->kind;
}

size_t
ifcopenshell_selector_node_child_count(const ifcopenshell_selector_node_t* node) {
    if (!node) return 0;
    return reinterpret_cast<const Node*>(node)->children.size();
}

ifcopenshell_selector_node_t*
ifcopenshell_selector_node_child(const ifcopenshell_selector_node_t* node, size_t index) {
    if (!node) return nullptr;
    const auto* n = reinterpret_cast<const Node*>(node);
    if (index >= n->children.size()) return nullptr;
    return reinterpret_cast<ifcopenshell_selector_node_t*>(n->children[index]);
}

const char*
ifcopenshell_selector_node_text(const ifcopenshell_selector_node_t* node) {
    if (!node) return nullptr;
    const auto* n = reinterpret_cast<const Node*>(node);
    if (!n->is_token()) return nullptr;
    return n->text.c_str();
}

void
ifcopenshell_selector_node_free(ifcopenshell_selector_node_t* root) {
    node_free_impl(reinterpret_cast<Node*>(root));
}

} // extern "C"
