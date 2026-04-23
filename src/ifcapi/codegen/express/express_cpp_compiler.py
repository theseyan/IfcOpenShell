import ast
import os
import re
import sys
import json
import hashlib
import operator
import functools
import itertools

import ifcopenshell.express
import ifcopenshell.express.express_parser

import networkx as nx

from codegen import indent

DEBUG = False


def to_graph(tree):
    g = nx.DiGraph()

    # Convert
    def write_to_graph(val, name=None):
        if isinstance(val, list):
            pairs = ((None, v) for v in val)
        elif isinstance(val, dict):
            pairs = val.items()
        else:
            assert name
            g.add_edge(name, name + "_value")
            return g.add_node(name + "_value", label=val)

        for i, (k, v) in enumerate(pairs):
            i = f"{i:03d}"
            nid = f"{name or 'root'}/{k or i}"
            g.add_node(nid, label=k)
            if name:
                g.add_edge(name, nid)
            write_to_graph(v, nid)

    write_to_graph(tree)

    to_remove = set()

    # Remove intermediate anonymous nodes. Often the result of ZeroOrMore() productions in
    # bootstrap.py that result in an intermediate list index node in to_tree()

    # Start with the intermediate nodes and filter out root (needs to have predecessors)
    intermediate = [n for n in g.nodes if g.nodes[n].get("label") is None and list(g.predecessors(n))]

    for n in intermediate:
        pr = list(g.predecessors(n))
        if len(pr) == 1 and g.nodes[pr[0]].get("label"):
            # when eliminating a grouping node with heterogeneous content
            # rather copy the predecessor node label to the grouping node
            # and later delete the predecessor
            sc = list(g.successors(n))
            if len(sc) > 1:
                sc_labels = list(map(lambda x: g.nodes[x].get("label"), sc))
                if len(set(sc_labels)) > 1 and None not in sc_labels:
                    g.nodes[n]["label"] = g.nodes[pr[0]].get("label")
                    to_remove.add(pr[0])
                    continue

        for ab in itertools.product(g.predecessors(n), g.successors(n)):
            g.add_edge(*ab)
        g.remove_node(n)

    # The removal process above can decide to not fold the anonymous node, but rather
    # the predecessor of it, in which case it is deleted in this step.
    for n in to_remove:
        for ab in itertools.product(g.predecessors(n), g.successors(n)):
            g.add_edge(*ab)
        g.remove_node(n)

    for n in g.nodes:
        if (
            len(list(g.successors(n))) == 0
            and g.nodes[n].get("label") not in ifcopenshell.express.express_parser.all_rules
        ):
            g.nodes[n]["is_terminal"] = True

    return g


def write_dot(fn, g):

    with open(fn, "w") as f:

        def w(*args, **kwargs):
            print(*args, file=f, **kwargs)

        w("digraph", "{")

        def nodename(n):
            return "N" + hashlib.md5(n.encode()).hexdigest()

        def format(di):
            Q = '"'
            inner = ",".join(
                f"{k}={'' if v.startswith('<') else Q}{v}{'' if v.startswith('<') else Q}" for k, v in di.items()
            )
            if inner:
                inner = f"[{inner}]"
            return inner

        for n in g.nodes:
            lbl = g.nodes[n].get("label")
            if lbl:
                attrs = {"label": lbl}
            else:
                attrs = {"label": n}

            if g.nodes[n].get("is_terminal"):
                attrs["shape"] = "rect"
                attrs["label"] = f"\\\"{attrs['label']}\\\""
            else:
                attrs["shape"] = "none"

            w(nodename(n), format(attrs), ";", sep="")

        for a, b in g.edges:
            w(nodename(a), "->", nodename(b), ";")

        w("}", flush=True)


from pyparsing import *

SLASH = Suppress("/")
identifier = Word(alphanums + "_")
rule = identifier + (ZeroOrMore(SLASH + identifier))


def paths(G, root, length):
    if length == 1:
        yield (G.nodes[root].get("label"),)
        return

    sd = dict(nx.bfs_successors(G, root, depth_limit=length - 1))

    def r(x, p=None):
        if p and len(p) == length:
            yield tuple(map(lambda n: G.nodes[n].get("label"), p))
        else:
            for y in sd.get(x, []):
                yield from r(y, (p or [x]) + [y])

    yield from r(root)


class context:
    def __init__(self, graph, rules):
        self.graph = graph
        self.rules = rules

    def __getattr__(self, k):
        def inner():
            for r in self.rules:
                label_id_pairs = map(
                    lambda n: (self.graph.nodes[n].get("label"), n),
                    # itertools.chain.from_iterable(
                    #     dict(nx.bfs_successors(self.graph, r)).values()
                    # )
                    self.graph.successors(r),
                )
                matching = filter(lambda p: p[0] == k, label_id_pairs)
                yield from map(operator.itemgetter(1), matching)

        return context(self.graph, list(inner()))

    def has_inverse(self, a):
        for r in self.rules:
            if a in map(lambda n: self.graph.nodes[n].get("label"), self.graph.predecessors(r)):
                return True
        return False

    def __iter__(self):
        for r in self.rules:
            yield context(self.graph, [r])

    def descendants(self):
        return [b.rules[0][len(self.rules[0]) + 1 :] for b in self.branches(allow_multiple=True)]

    def __repr__(self):
        try:
            s = "\n\n" + str(self)
        except:
            s = ""
        return f"<rule_context ({' '.join(self.descendants())})>{s}"

    def __str__(self):
        assert len(self.rules) == 1
        nodes = itertools.chain(
            self.rules,
            itertools.chain.from_iterable(dict(nx.bfs_successors(self.graph, self.rules[0])).values()),
        )
        terminals_or_values = list(
            filter(
                lambda n: self.graph.nodes[n].get("is_terminal") or self.graph.nodes[n].get("value"),
                nodes,
            )
        )
        # assert len(terminals) == 1
        attrs = [self.graph.nodes[tv] for tv in terminals_or_values]
        attrs = [a.get("value", a["label"]) for a in attrs]
        attr_types = list(map(type, attrs))
        if empty in attr_types[0:1]:
            return ""
        attrs = list(filter(lambda s: isinstance(s, str), attrs))
        return attrs[0]

    def __eq__(self, other):
        return self.graph == other.graph and self.rules == other.rules

    def __hash__(self):
        return hash(self.rules)

    def branches(self, allow_multiple=False, exclude=()):
        if not allow_multiple:
            assert len(self.rules) == 1
        combined = sum(
            [
                sorted(
                    (context(self.graph, [n]) for n in self.graph.successors(R)),
                    key=lambda c: c.rules[0] if c.rules else "",
                )
                for R in self.rules
            ],
            [],
        )
        return [c for c in combined if c not in exclude]

    def parent(self):
        assert len(self.rules) == 1
        return context(self.graph, list(self.graph.predecessors(self.rules[0])))

    def branch(self, i):
        return self.branches()[i]

    def __len__(self):
        return len(self.rules)

    def __getitem__(self, k):
        return list(self)[k]

    def key(self):
        parts = list(
            map(
                lambda s: tuple(map(lambda p: "n" if p.isdigit() else p, s.split("/"))),
                self.rules,
            )
        )
        assert len(set(parts)) == 1
        return [x for x in parts[0][::-1] if x != "n"][0]


# @todo
context_class = context


class codegen_rule:
    def __init__(self, pattern, fn):
        self.pattern = tuple(rule.parseString(pattern))
        self.fn = fn
        if not hasattr(codegen_rule, "all_rules"):
            codegen_rule.all_rules = []
        codegen_rule.all_rules.append(self)

    def __call__(self, graph, node):
        # try:
        v = self.fn(context(graph, [node]))
        # except:
        #     v = "ERROR!!"
        graph.nodes[node]["value"] = v
        return v

    @staticmethod
    def apply(G):
        v = None
        for n in reversed(list(nx.topological_sort(G))):
            for r in codegen_rule.all_rules:
                if r.pattern in paths(G, n, len(r.pattern)):
                    v = r(G, n)
        return v


def _attrs_used(text, attrs):
    """Return the subset of *attrs* (case-insensitive lower) referenced in
    *text*. Used to emit `Value <name> = express_getattr(self, "<Name>");`
    aliasing lines at the top of WHERE-rule and DERIVE-rule bodies. The
    text may use either the bare lowercase form (`predefinedtype`) or the
    reserved-keyword-escaped form (`operator_`); both count as a hit."""
    out = []
    for a in attrs:
        lo = a.lower()
        safe = _safe_ident(lo)
        if re.search(rf"\b{re.escape(lo)}\b", text) or re.search(rf"\b{re.escape(safe)}\b", text):
            out.append(a)
    return out


def process_rule_decl(context):
    # Global EXPRESS rule -> bool fn taking IfcFile*. Returns false if any
    # WHERE assertion fails, true otherwise.
    rule_id = str(context.rule_head.rule_id)
    entity_ref = str(context.rule_head.entity_ref)
    body_parts = []
    if context.algorithm_head.local_decl:
        body_parts.append(str(context.algorithm_head.local_decl))
    if context.stmt:
        body_parts.extend(map(str, context.stmt.branches()))
    if context.where_clause and context.where_clause.domain_rule:
        body_parts.append(str(context.where_clause.domain_rule))
    body = "\n".join(p for p in body_parts if p and p.strip())
    return (
        f'bool {rule_id}(IfcFile* file) {{\n'
        f'    Value {entity_ref} = ifcapi::express::file_by_type(file, "{entity_ref}");\n'
        f'{indent(4, body)}\n'
        f'    return true;\n'
        f'}}'
    )


class empty:
    pass


wb = r"\b"


def process_type_decl(scope, context):
    class_name = str(context.type_id if scope == "type" else context.entity_head.entity_id)

    attributes = []
    if scope == "entity":

        def get_attributes(nm):
            ent = schema.entities[nm]
            if ent.supertypes:
                yield from get_attributes(ent.supertypes[0])
            yield from [a.name for a in ent.attributes]
            yield from [a.name for a in ent.inverse]
            yield from [a[0] for a in ent.derive if isinstance(a[0], str)]

        attributes = list(get_attributes(class_name))

    def format_rule(domain_rule):
        rule_id = f"{class_name}_{domain_rule.rule_label_id}"
        body_text = str(domain_rule)
        used = _attrs_used(body_text, attributes)
        alias_lines = [f'Value {_safe_ident(a.lower())} = express_getattr(self, "{a}");' for a in used]
        body = "\n".join(alias_lines + [body_text])
        return (
            f'bool {rule_id}(EntityRef self) {{\n'
            f'{indent(4, body)}\n'
            f'    return true;\n'
            f'}}'
        )

    rule_parent = context if scope == "type" else context.entity_body
    statements = []
    if rule_parent.where_clause:
        statements.extend(map(format_rule, rule_parent.where_clause.branches()))

    if scope == "entity":

        def format_derived(derived_attr):
            attr_qualifier = str(derived_attr.attribute_decl.redeclared_attribute.qualified_attribute.attribute_qualifier)[1:] \
                if derived_attr.attribute_decl.redeclared_attribute else str(derived_attr.attribute_decl)
            expr_text = str(derived_attr.expression)
            used = _attrs_used(expr_text, attributes)
            alias_lines = [f'Value {_safe_ident(a.lower())} = express_getattr(self, "{a}");' for a in used]
            body = "\n".join(alias_lines + [f"return {expr_text};"])
            return (
                f'Value calc_{class_name}_{attr_qualifier}(EntityRef self) {{\n'
                f'{indent(4, body)}\n'
                f'}}'
            )

        if context.entity_body.derive_clause:
            statements.extend(map(format_derived, context.entity_body.derive_clause.branches()))

    return "\n\n".join(statements)


def process_domain_rule(context):
    # WHERE-rule assertion: short-circuit-return false on failure.
    return f"if (!Value({context.expression}).truthy()) return false;"


def wrap_parens(s):
    s = str(s)
    if " " in s:
        s = "(%s)" % s
    return s


def process_expression(context):
    def concat(a, b, **kwargs):
        return " ".join(
            map(
                str,
                sum(
                    zip(
                        [None] + a.branches(**kwargs),
                        map(wrap_parens, b.branches(**kwargs)),
                    ),
                    (),
                )[1:],
            )
        )

    if context.rel_op_extended:
        # EXPRESS `IN` membership test → runtime helper `express_in(needle, hay)`.
        if str(context.rel_op_extended).lower() == "in":
            ops = list(context.simple_expression.branches())
            if len(ops) == 2:
                a, b = map(str, ops)
                return f"ifcapi::express::express_in({a}, {b})"
        if context.term:
            # IfcSameValue
            return concat(
                context.rel_op_extended,
                context,
                allow_multiple=True,
                exclude=[context.rel_op_extended],
            )
        else:
            return concat(context.rel_op_extended, context.simple_expression)
    elif context.multiplication_like_op:
        if str(context.multiplication_like_op.branches()[0]) == "||":
            all_args = {}
            most_concrete_type = None
            most_concrete_type_inheritance_chain_length = -1

            for s in context.factor.branches():
                typename, args = str(s).split("(", 1)
                args = args[:-1]

                break_points = [[0]]
                bracket_nesting = 0
                for i, tk in enumerate(args):
                    if tk in "[(":
                        bracket_nesting += 1
                    if tk in ")]":
                        bracket_nesting -= 1
                    if tk == "," and bracket_nesting == 0:
                        break_points[-1].append(i)
                        break_points.append([i + 1])

                break_points[-1].append(len(args))

                # @todo don't depend on registered schema
                S = ifcopenshell.ifcopenshell_wrapper.schema_by_name(schema.name)
                entity = S.declaration_by_name(typename)
                entity_attributes = entity.attributes()

                def count_chain_length(ent):
                    length = 0
                    while ent:
                        ent = ent.supertype()
                        length += 1
                    return length

                args = [args[slice(*x)] for x in break_points]

                for i, arg in filter(lambda p: p[1], enumerate(args)):
                    all_args[entity_attributes[i].name()] = arg

                cl = count_chain_length(entity)
                if cl > most_concrete_type_inheritance_chain_length:
                    most_concrete_type = entity.name()
                    most_concrete_type_inheritance_chain_length = cl

            # Build positional args in declaration order, defaulting unset
            # positions to a fresh INDETERMINATE Value so the C++ entity-
            # constructor wrapper can always be invoked with full arity.
            S = ifcopenshell.ifcopenshell_wrapper.schema_by_name(schema.name)
            ent_decl = S.declaration_by_name(most_concrete_type)
            full_attrs = ent_decl.all_attributes()
            ordered = [all_args.get(a.name(), "Value()") for a in full_attrs]
            return f"{most_concrete_type}({', '.join(ordered)})"
        else:
            return concat(context.multiplication_like_op, context.factor)
    elif context.add_like_op:
        if context.factor or len(context.term) > 1:
            # @todo now sure why this is required (in IfcCrossProduct)
            # @todo not sure what's going on here, why we have both factor and term as direct child productions of simple_expression (in IfcDotProduct)
            return concat(
                context.add_like_op,
                context,
                allow_multiple=True,
                exclude=[context.add_like_op],
            )
        else:
            return concat(context.add_like_op, context.term)


def process_interval(context):
    op0, op1 = context.interval_op.branches()
    return " ".join(
        map(
            str,
            (
                context.interval_low,
                op0,
                context.interval_item,
                op1,
                context.interval_high,
            ),
        )
    )


def simple_concat(context):
    # simple_factor: only to join unary op (-) with operand
    # primary: only to join index with qualifyable operand

    def qualifier_position(s):
        # unary operators (Python and C++ flavors)
        if s in ("-", "+", "not", "!"):
            return -1
        # qualifiers
        if s and s[0] in (".", "["):
            return 1
        return 0

    branches = sorted(map(str, context.branches()), key=qualifier_position)

    if len(branches) == 2 and branches[0] in ("not", "!"):
        return f"!({wrap_parens(branches[1])})"
    else:
        v = "".join(branches)

    return v


def process_rel_op(context):
    # @todo the distinction between value comparison and instance comparison
    if str(context) == "<>" or str(context) == ":<>:":
        return "!="
    elif str(context) == "=" or str(context) == ":=:":
        return "=="


def process_if_stmt(context):
    cond = context.logical_expression if context.logical_expression.branches() else context.expression
    s = f"if (Value({cond}).truthy()) {{\n{indent(4, context.stmt.branches())}\n}}"
    if context.else_stmt:
        s += f" else {{\n{indent(4, context.else_stmt.branches())}\n}}"
    return s


def process_repeat_stmt(context):
    ic = context.repeat_control.increment_control
    var = _safe_ident(str(ic.variable_id).lower())
    return (
        f"for (Value {var} = {ic.bound_1}; (Value({var}) <= Value({ic.bound_2})).truthy(); "
        f"{var} = {var} + Value((int64_t)1)) {{\n"
        f"{indent(4, context.stmt.branches())}\n"
        f"}}"
    )


def process_function_decl(context):
    arguments = list(map(
        str.lower,
        map(
            str,
            context.function_head.formal_parameter.parameter_id.branches(allow_multiple=True),
        ),
    ))
    arguments = [_safe_ident(a) for a in arguments]
    params = ", ".join("Value " + a for a in arguments)
    locals_text = str(context.algorithm_head.local_decl) if context.algorithm_head.local_decl else ""
    body_text = "\n".join(map(str, context.stmt.branches()))
    # Auto-declare any assignment target (lhs identifier) not already a parameter
    # or in the explicit local_decl block. EXPRESS technically requires LOCAL,
    # but some schemas omit it and Python's dynamic typing papers over the gap.
    declared = set(arguments)
    for m in re.finditer(r"\bValue\s+([A-Za-z_]\w*)\b", locals_text):
        declared.add(m.group(1))
    auto_locals = []
    seen_auto = set()
    for m in re.finditer(r"^\s*([A-Za-z_]\w*)\s*=\s", body_text, flags=re.MULTILINE):
        nm = m.group(1)
        if nm in declared or nm in seen_auto or nm in _CPP_RESERVED:
            continue
        seen_auto.add(nm)
        auto_locals.append("Value " + nm + ";")
    auto_locals_text = "\n".join(auto_locals)
    body = "\n".join(p for p in (auto_locals_text, locals_text, body_text) if p and p.strip())
    return (
        f"Value {context.function_head.function_id}({params}) {{\n"
        f"{indent(4, body)}\n"
        f"    return Value();\n"
        f"}}"
    )


def process_query(context):
    var = _safe_ident(str(context.variable_id).lower())
    cond = context.logical_expression if context.logical_expression and context.logical_expression.branches() else context.expression
    return (
        f"([&]() {{ auto __r = Value::make_list({{}}); "
        f"for (auto& {var} : ifcapi::express::iter({context.aggregate_source})) "
        f"{{ if (Value({cond}).truthy()) __r.append({var}); }} return __r; }})()"
    )


def process_local_variable(context):
    if context.expression:
        expr = str(context.expression)
        if context.parameter_type.generalized_types.general_aggregation_types.general_set_type:
            expr = re.sub(r"(\[[^\]]*\])", "express_set(\\1)", expr)
        return f"Value {_safe_ident(str(context.variable_id).lower())} = {expr};"
    else:
        return empty()


def process_function_call(context):
    raw_nm = f"{context.built_in_function if context.built_in_function else context.function_ref}"
    nm = _BUILTIN_FN_REMAP.get(raw_nm.lower(), raw_nm)
    args = f"{context.actual_parameter_list if context.actual_parameter_list and context.actual_parameter_list.branches() else ''}"
    return f"{nm}({args})"


# EXPRESS built-in / often-used function names that collide with C++ keywords or
# need explicit runtime aliases. Lowercase keys; values are the ifcapi runtime
# symbol that lives in `ifcapi::express` (and is therefore ADL-reachable from
# generated code).
_BUILTIN_FN_REMAP = {
    "sizeof": "sizeof_",
    "typeof": "typeof_",
    "abs":    "math_abs",
    "sqrt":   "math_sqrt",
    "sin":    "math_sin",
    "cos":    "math_cos",
    "tan":    "math_tan",
    "asin":   "math_asin",
    "acos":   "math_acos",
    "atan":   "math_atan",
    "log":    "math_log",
    "log2":   "math_log",
    "log10":  "math_log",
    "exp":    "math_exp",
    "value":  "express_value",
}


def make_lowercase(context):
    return _safe_ident(str(context).lower())


def make_lowercase_if(fn):
    def inner(context):
        if fn(context):
            return make_lowercase(context)

    return inner


def _parse_lvalue_segments(lhs):
    """Split `name[i].attr[j]...` into [(name, [(seg_kind, value), ...])].
    seg_kind is either 'idx' or 'attr'."""
    m = re.match(r"^([A-Za-z_]\w*)", lhs)
    if not m:
        return None
    base = m.group(1)
    rest = lhs[m.end():]
    segs = []
    while rest:
        if rest.startswith('.'):
            am = re.match(r"\.([A-Za-z_]\w*)", rest)
            if not am:
                return None
            segs.append(('attr', am.group(1)))
            rest = rest[am.end():]
        elif rest.startswith('['):
            depth = 0
            for i, ch in enumerate(rest):
                if ch == '[':
                    depth += 1
                elif ch == ']':
                    depth -= 1
                    if depth == 0:
                        segs.append(('idx', rest[1:i]))
                        rest = rest[i+1:]
                        break
            else:
                return None
        else:
            return None
    return base, segs


def _emit_lvalue_set(base, segs, rhs):
    """Emit a C++ functional assignment: base = chain_of_setters(...)."""
    if not segs:
        return f"{base} = {rhs};"

    def _build(getter, idx):
        if idx == len(segs):
            return rhs
        kind, val = segs[idx]
        if kind == 'idx':
            inner = _build(f"ifcapi::express::express_getitem({getter}, {val})", idx + 1)
            return f"ifcapi::express::set_index({getter}, {val}, {inner})"
        else:
            inner = _build(f"ifcapi::express::express_getattr({getter}, \"{val}\")", idx + 1)
            return f"ifcapi::express::set_attr({getter}, \"{val}\", {inner})"

    return f"{base} = {_build(base, 0)};"


def process_assignment(context):
    lhs = str(context.general_ref)
    if context.qualifier:
        lhs += str(context.qualifier)
    rhs = str(context.expression)
    parsed = _parse_lvalue_segments(lhs)
    if parsed is None or not parsed[1]:
        return f"{lhs} = {rhs};"
    base, segs = parsed
    return _emit_lvalue_set(base, segs, rhs)


def process_case_action(context):
    first = context.parent().branches().index(context)
    pred = "} else if" if first else "if"
    if re.match(r"^'[a-z0-9]+'$", str(context.expression)):
        lower = ".lower()"
    else:
        lower = ""
    return f"{pred} (Value(({context.parent().expression}{lower}) == {context.expression}).truthy()) {{\n{indent(4, context.stmt.branches())}\n"


def process_case_statement(context):
    branches = context.branches(
        exclude=[getattr(context, v) for v in context.descendants() if not v.startswith("case_action")]
    )
    out = "\n".join(map(str, branches))
    if context.stmt and context.stmt.branches():
        out += f"}} else {{\n{indent(4, context.stmt)}\n"
    out += "}"
    return out


def process_aggregate_initializer(context):
    if context.element.repetition:
        return f"ifcapi::express::repeat({context.element.expression}, {context.element.repetition})"
    items = ", ".join(map(str, context.element.branches() if context.element else ()))
    return f"Value::make_list({{{items}}})"


def process_index(context):
    if context.parent().key() == "index_qualifier":
        return context
    return f"[{context} - 1]"


def _split_qualifiers(s):
    """Tokenise a string of chained EXPRESS qualifiers (e.g. ``[1 - 1].Dim``)
    into individual qualifier strings preserving order and bracket pairing.
    Each emitted token starts with `.` (attribute/group) or `[` (index)."""
    out = []
    i = 0
    n = len(s)
    while i < n:
        c = s[i]
        if c == ".":
            j = i + 1
            while j < n and s[j] not in ".[":
                j += 1
            out.append(s[i:j])
            i = j
        elif c == "[":
            depth = 1
            j = i + 1
            while j < n and depth:
                if s[j] == "[":
                    depth += 1
                elif s[j] == "]":
                    depth -= 1
                j += 1
            out.append(s[i:j])
            i = j
        else:
            # unrecognised char — bail out, emit the rest verbatim
            out.append(s[i:])
            break
    return out


def process_primary(context):
    """primary = qualifiable_factor (+ qualifier_chain). Translate attribute
    and index qualifiers into express_getattr / express_getitem calls so
    we never emit raw `obj.Member` or `obj[n]` syntax in C++.

    Special-case: if `base` is the name of a schema enumeration type and
    the first qualifier is `.<MEMBER>`, emit a string-typed Value rather
    than a runtime entity-attribute fetch (enums are compared by name)."""
    branches = list(map(str, context.branches()))
    if not branches:
        return ""
    base = branches[0]
    qualifiers = []
    for q in branches[1:]:
        qualifiers.extend(_split_qualifiers(q))

    if (
        qualifiers
        and qualifiers[0].startswith(".")
        and base in getattr(schema, "enumerations", {})
    ):
        member = qualifiers[0][1:]
        base = f'Value(std::string("{member}"))'
        qualifiers = qualifiers[1:]

    for q in qualifiers:
        if q.startswith("."):
            base = f'express_getattr({base}, "{q[1:]}")'
        elif q.startswith("["):
            inner = q[1:-1]
            base = f'express_getitem({base}, {inner})'
        else:
            base = base + q
    return base


def process_simple_string_literal(context):
    """EXPRESS uses single-quoted strings; rewrite to C++ double-quoted
    string literals wrapped in a Value so they participate in operators."""
    raw = str(context)
    if len(raw) >= 2 and raw[0] == "'" and raw[-1] == "'":
        body = raw[1:-1].replace("\\", "\\\\").replace('"', '\\"')
        return f'Value(std::string("{body}"))'
    return raw


# implemented sizeof() function in generated code
# codegen_rule("built_in_function/SIZEOF", lambda context: f"len")
# @todo
codegen_rule("function_call", process_function_call)
codegen_rule(
    "actual_parameter_list",
    lambda context: ", ".join(map(str, context.expression.branches() if context.expression else [])),
)
codegen_rule("entity_decl", functools.partial(process_type_decl, "entity"))
codegen_rule("rule_decl", process_rule_decl)
codegen_rule("type_decl", functools.partial(process_type_decl, "type"))
codegen_rule("function_decl", process_function_decl)
codegen_rule("domain_rule", process_domain_rule)
codegen_rule("expression", process_expression)
codegen_rule("simple_expression", process_expression)
codegen_rule("logical_expression", process_expression)
codegen_rule("term", process_expression)
codegen_rule("query_expression", process_query)
codegen_rule("aggregate_initializer", process_aggregate_initializer)
codegen_rule("interval", process_interval)
codegen_rule("simple_factor", simple_concat)
codegen_rule("primary", process_primary)
codegen_rule("qualifier", simple_concat)
codegen_rule("return_stmt", lambda context: "return %s;" % context)
codegen_rule("compound_stmt", lambda context: "\n".join(map(str, context.stmt.branches())))
codegen_rule("if_stmt", process_if_stmt)
codegen_rule("repeat_stmt", process_repeat_stmt)
codegen_rule("index", process_index)
codegen_rule("index_qualifier", process_index)
codegen_rule("group_qualifier", lambda context: empty())
codegen_rule("attribute_qualifier", lambda context: ".%s" % context)
codegen_rule("rel_op", process_rel_op)
codegen_rule(
    "built_in_constant",
    lambda context: "Value()" if str(context) == "?" else str(context),
)
codegen_rule("assignment_stmt", process_assignment)
codegen_rule("local_variable", process_local_variable)
codegen_rule("local_decl", lambda context: "\n".join(map(str, context.branches())))
codegen_rule("general_ref/parameter_ref", make_lowercase)
codegen_rule(
    "qualifiable_factor/attribute_ref",
    make_lowercase_if(lambda context: str(context) not in set(map(str, schema.all_declarations.keys()))),
)
codegen_rule("case_action", process_case_action)
codegen_rule("case_stmt", process_case_statement)
codegen_rule("simple_string_literal", process_simple_string_literal)
codegen_rule("string_literal", process_simple_string_literal)
codegen_rule("escape_stmt", lambda context: "break;")

codegen_rule("XOR", lambda context: "^")  # logical XOR; Value::operator^ overload
codegen_rule("MOD", lambda context: "%")
codegen_rule("TRUE", lambda context: "true")
codegen_rule("FALSE", lambda context: "false")
codegen_rule("AND", lambda context: "&&")
codegen_rule("OR", lambda context: "||")
codegen_rule("NOT", lambda context: "!")
codegen_rule("DIV", lambda context: "/")


class AttributeGetattrTransformer(ast.NodeTransformer):
    def visit_Attribute(self, node):
        parents = []
        n = node
        while n := getattr(n, "parent", 0):
            parents.append(n)

        custom_funcs = "is_entity", "usedin", "express_len", "express_getitem", "typeof", "express_getattr"
        function_defs = [p.name for p in parents if isinstance(p, ast.FunctionDef)]
        if any(fn in function_defs for fn in custom_funcs):
            return node

        # Check if the Attribute node is the target of an assignment statement
        if isinstance(node.ctx, ast.Store):
            return node

        if node.attr == "create_entity":
            return node

        if node.attr.startswith("__"):
            return node

        # Don't rewrite at module scope (top-level, no indent)
        enclosing_stmt = next((p for p in parents if isinstance(p, ast.stmt)), None)
        if enclosing_stmt is not None and isinstance(getattr(enclosing_stmt, "parent", None), ast.Module):
            return node

        new_value = self.visit(node.value)

        # Replace the Attribute node with a call to the built-in `getattr` function
        return ast.copy_location(
            ast.Call(
                func=ast.Name(id="express_getattr", ctx=ast.Load()),
                args=[
                    new_value,
                    ast.Str(s=node.attr),
                    ast.Name(id="INDETERMINATE", ctx=ast.Load()),
                ],
                keywords=[],
            ),
            node,
        )

    def visit_Subscript(self, node):
        parents = []
        n = node
        while n := getattr(n, "parent", 0):
            parents.append(n)

        custom_funcs = "is_entity", "usedin", "express_len", "express_getitem", "typeof", "express_getattr"
        function_defs = [p.name for p in parents if isinstance(p, ast.FunctionDef)]
        if any(fn in function_defs for fn in custom_funcs):
            return node

        # Check if the Attribute node is the target of an assignment statement
        if isinstance(node.ctx, ast.Store):
            return node

        assert (
            isinstance(node.slice, ast.Name)
            or isinstance(node.slice, ast.Constant)
            or isinstance(node.slice, ast.BinOp)
        )

        new_value = self.visit(node.value)

        # Replace the Attribute node with a call to the built-in `getattr` function
        return ast.copy_location(
            ast.Call(
                func=ast.Name(id="express_getitem", ctx=ast.Load()),
                args=[
                    new_value,
                    node.slice,
                    ast.Name(id="INDETERMINATE", ctx=ast.Load()),
                ],
                keywords=[],
            ),
            node,
        )

    def assign_parent_refs(self, tree):
        for node in ast.walk(tree):
            for child in ast.iter_child_nodes(node):
                child.parent = node


# C++ keywords / common collisions that can appear as EXPRESS identifiers
# (entity attribute names, parameter names) and must be renamed in emitted
# code. We append a trailing underscore — the JSON/string key passed to
# make_entity / express_getattr keeps the original name, so this only
# affects local variable / parameter naming.
_CPP_RESERVED = {
    "operator", "new", "delete", "class", "struct", "template", "typename",
    "namespace", "using", "this", "auto", "register", "explicit", "virtual",
    "private", "public", "protected", "friend", "inline", "static", "extern",
    "const", "volatile", "mutable", "typedef", "default", "switch", "case",
    "break", "continue", "return", "throw", "try", "catch", "if", "else",
    "for", "while", "do", "goto", "sizeof", "typeid", "and", "or", "not",
    "xor", "bitand", "bitor", "compl", "true", "false", "nullptr",
    "char", "int", "long", "short", "float", "double", "void", "bool",
    "signed", "unsigned", "union", "enum",
}


def _safe_ident(name):
    """Return *name* possibly rewritten to avoid clashing with a C++
    reserved word. The transformation is reversible and stable so different
    emit sites agree on the same rewritten symbol."""
    if name.lower() in _CPP_RESERVED:
        return name + "_"
    return name


def _collect_entity_attrs(schema, ent_name):
    """Return the list of explicit attribute names for *ent_name*, ordered
    supertypes-first then own. Mirrors EXPRESS positional-constructor
    semantics."""
    out = []
    seen = set()

    def _walk(name):
        if name in seen:
            return
        seen.add(name)
        ent = schema.entities.get(name)
        if not ent:
            return
        for st in (ent.supertypes or []):
            _walk(str(st))
        for a in ent.attributes:
            out.append(a.name)

    _walk(ent_name)
    return out


def emit_cpp(schema_path, output_path):
    """Generate a C++ source file with native EXPRESS DERIVE/WHERE rules
    for the schema in *schema_path*, written to *output_path*.

    X5a: Skeleton only. Emits prelude + namespace + per-declaration
    placeholder comments + closing brace. No expression/statement code yet.
    """
    import io
    global schema  # codegen rules reference module-level `schema`

    schema = ifcopenshell.express.express_parser.parse(schema_path).schema

    try:
        ifcopenshell.ifcopenshell_wrapper.schema_by_name(schema.name)
    except Exception:
        builder = ifcopenshell.express.parse(schema_path)
        ifcopenshell.register_schema(builder)

    output = io.StringIO()

    nl = chr(10)
    schema_ns = schema.name.upper()

    output.write('// THIS FILE IS AUTO-GENERATED. DO NOT EDIT.' + nl)
    output.write('// Source: ' + os.path.basename(schema_path) + nl)
    output.write('// Schema: ' + schema.name + nl + nl)
    output.write('#include "ifcapi/express/runtime.h"' + nl)
    output.write('#include <cmath>' + nl + nl)
    output.write('namespace ifcapi { namespace express { namespace ' + schema_ns + ' {' + nl + nl)
    output.write('using ifcapi::express::Value;' + nl)
    output.write('using ifcapi::express::EntityRef;' + nl + nl)

    output.write('// --- EXPRESS literals ---' + nl)
    output.write('inline const Value unknown = Value();  // EXPRESS unknown logical' + nl)
    output.write('inline const Value indeterminate = Value();' + nl + nl)

    # Enum value constants — emitted FIRST so entity rules can reference
    # bare lowercase enum names (e.g. `area`) directly.
    seen_enum = set()
    output.write('// --- enum constants ---' + nl)
    for k, v in schema.enumerations.items():
        for vi in v.values:
            lname = _safe_ident(vi.lower())
            if lname in seen_enum:
                continue
            seen_enum.add(lname)
            output.write('inline const Value ' + lname + ' = Value(std::string("' + vi + '"));' + nl)
    output.write(nl)

    # Per-schema entity-constructor inline wrappers (X5h) — emitted BEFORE
    # rule bodies so rules can call `IfcDirection(...)` etc. inline.
    output.write('// --- entity constructors ---' + nl)
    for ent_name in schema.entities.keys():
        attr_names = _collect_entity_attrs(schema, ent_name)
        params = ', '.join('Value ' + _safe_ident(a) for a in attr_names) if attr_names else ''
        kvs = ', '.join('{"' + a + '", ' + _safe_ident(a) + '}' for a in attr_names)
        output.write(
            'inline Value ' + ent_name + '(' + params + ') { return ifcapi::express::make_entity("' +
            schema.name + '", "' + ent_name + '", {' + kvs + '}); }' + nl
        )
    output.write(nl)

    # Helper to dispatch a single decl through the codegen rules and emit
    # its translated C++ to *output*.
    def _emit(nm):
        try:
            tree = ifcopenshell.express.express_parser.to_tree(schema[nm])
            G = to_graph(tree)
            rule_code = codegen_rule.apply(G)
        except Exception as exc:
            output.write('// ===== ' + nm + ' =====' + nl)
            output.write('// codegen error: ' + str(exc).replace(nl, ' ') + nl + nl)
            return
        if rule_code is None or isinstance(rule_code, empty):
            return
        text = str(rule_code).strip()
        if text:
            output.write(text + nl + nl)

    # Functions next — rules can call them. Emit forward declarations first
    # so functions can call one another in any order.
    def _count_params(fn):
        try:
            fh = fn.dict_tokens['function_head'][0]
            flat = list(fh.flat)
            # Find tokens between the function-name's first '(' and the matching ')'.
            try:
                lp = flat.index('(')
            except ValueError:
                return 0
            depth = 0
            end = lp
            for i in range(lp, len(flat)):
                if flat[i] == '(':
                    depth += 1
                elif flat[i] == ')':
                    depth -= 1
                    if depth == 0:
                        end = i
                        break
            inner = flat[lp + 1:end]
            n = 0
            for i, tok in enumerate(inner):
                if i + 1 < len(inner) and inner[i + 1] in (',', ':'):
                    if re.match(r"^[A-Za-z_]\w*$", tok):
                        n += 1
            return n
        except Exception:
            return 0
    output.write('// --- function forward declarations ---' + nl)
    for nm, fn in schema.functions.items():
        n = _count_params(fn)
        sig_params = ", ".join(["Value"] * n)
        output.write(f"Value {nm}({sig_params});" + nl)
    output.write(nl)
    output.write('// --- function definitions ---' + nl)
    for nm in schema.functions.keys():
        _emit(nm)

    # Then the rules: type WHERE clauses, entity WHERE / DERIVE, and
    # global rules.
    output.write('// --- type / entity / rule definitions ---' + nl)
    for nm in schema.types.keys():
        _emit(nm)
    for nm in schema.entities.keys():
        _emit(nm)
    for nm in schema.rules.keys():
        _emit(nm)

    output.write(nl)

    # --- Derived-attribute registration trailer ---------------------
    # For every entity that declares a DERIVE clause, register a
    # mapping (entity_name, attr_name) -> &calc_<entity>_<attr>.
    output.write('// --- derived-attribute registry ---' + nl)
    reg_calls = []
    for ent_name, ent in schema.entities.items():
        derive_list = getattr(ent, 'derive', None) or []
        for entry in derive_list:
            try:
                head = entry[0]
            except Exception:
                continue
            if isinstance(head, tuple):
                # Redeclared attribute: (SuperEntity, AttrName)
                attr_name = head[-1]
            else:
                attr_name = str(head).rsplit('.', 1)[-1]
            reg_calls.append(
                f'    register_derived("{schema.name}", "{ent_name}", "{attr_name}", '
                f'(DeriveFn)&calc_{ent_name}_{attr_name});'
            )
    if reg_calls:
        output.write('namespace { struct __derived_register_t { __derived_register_t() {' + nl)
        for line in reg_calls:
            output.write(line + nl)
        output.write('} }; static __derived_register_t __derived_register_instance;}' + nl)
    output.write(nl)

    output.write('}}}  // namespace ifcapi::express::' + schema_ns + nl)

    if output_path == '-':
        sys.stdout.write(output.getvalue())
    else:
        os.makedirs(os.path.dirname(output_path) or '.', exist_ok=True)
        with open(output_path, 'w') as f:
            f.write(output.getvalue())


if __name__ == '__main__':
    if len(sys.argv) < 2:
        print('usage: express_cpp_compiler.py <schema.exp> [output.cpp]', file=sys.stderr)
        sys.exit(2)
    schema_path = sys.argv[1]
    if len(sys.argv) >= 3:
        output_path = sys.argv[2]
    else:
        from ifcopenshell.express import express_parser as _ep
        sname = _ep.parse(schema_path).schema.name
        output_path = os.path.join(os.path.dirname(__file__), '..', '..', 'src', 'express', 'generated', sname + '.cpp')
    emit_cpp(schema_path, output_path)
