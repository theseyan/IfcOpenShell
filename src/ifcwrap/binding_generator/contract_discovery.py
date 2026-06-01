# This file was generated with the assistance of an AI coding tool.

from __future__ import annotations

from dataclasses import dataclass
from pathlib import Path
import re


@dataclass(frozen=True)
class MarkedFunction:
    header: Path
    name: str
    return_annotations: frozenset[str]
    param_annotations: dict[str, frozenset[str]]


_COMMENT_RE = re.compile(r"//.*?$|/\*.*?\*/", re.MULTILINE | re.DOTALL)
_ANNOTATIONS = frozenset({
    "IFCAPI_OWNED",
    "IFCAPI_COPY",
    "IFCAPI_STATIC",
    "IFCAPI_NULLABLE",
    "IFCAPI_DOUBLE_BUFFER",
    "IFCAPI_INT32_BUFFER",
})
_ANNOTATION_CALLS = ("IFCAPI_HANDLE_PARAM", "IFCAPI_HANDLE_RESULT")


def _strip_comments(text: str) -> str:
    return _COMMENT_RE.sub("", text)


def _split_params(params: str) -> tuple[str, ...]:
    params = params.strip()
    if not params or params == "void":
        return ()
    result: list[str] = []
    start = 0
    angle_depth = paren_depth = bracket_depth = 0
    for index, char in enumerate(params):
        if char == "<":
            angle_depth += 1
        elif char == ">" and angle_depth:
            angle_depth -= 1
        elif char == "(":
            paren_depth += 1
        elif char == ")" and paren_depth:
            paren_depth -= 1
        elif char == "[":
            bracket_depth += 1
        elif char == "]" and bracket_depth:
            bracket_depth -= 1
        elif char == "," and not angle_depth and not paren_depth and not bracket_depth:
            result.append(params[start:index].strip())
            start = index + 1
    result.append(params[start:].strip())
    return tuple(param for param in result if param)


def _leading_annotations(text: str) -> tuple[frozenset[str], str]:
    annotations: list[str] = []
    rest = text.strip()
    while True:
        match = re.match(r"(?P<token>IFCAPI_[A-Z_]+(?:\s*\([^)]*\))?)(?=\s|$)", rest)
        if match is None:
            break
        token = " ".join(match.group("token").split())
        if token not in _ANNOTATIONS and not any(token.startswith(f"{name}(") for name in _ANNOTATION_CALLS):
            break
        annotations.append(token)
        rest = rest[match.end() :].strip()
    return frozenset(annotations), rest


def _param_name(param: str) -> str:
    param = param.split("=", 1)[0].strip()
    matches = re.findall(r"\b([A-Za-z_]\w*)\b(?=\s*(?:\[[^\]]*\])?\s*$)", param)
    if not matches:
        msg = f"Unable to determine parameter name from contract declaration parameter: {param!r}"
        raise ValueError(msg)
    return matches[-1]


def _parse_param_annotations(params: str) -> dict[str, frozenset[str]]:
    parsed: dict[str, frozenset[str]] = {}
    for param in _split_params(params):
        annotations, rest = _leading_annotations(param)
        if not annotations:
            continue
        parsed[_param_name(rest)] = annotations
    return parsed


def discover_marked_functions_in_headers(
    headers: list[Path] | tuple[Path, ...],
    *,
    marker: str = "IFCAPI_BINDING",
) -> tuple[MarkedFunction, ...]:
    """Discover function declarations annotated with a binding contract marker."""
    marker_re = re.escape(marker)
    declaration_re = re.compile(
        rf"\b{marker_re}\s+"
        r"(?P<return_decl>[\w:<>~,\s*&]+?)\s+"
        r"(?P<name>[A-Za-z_]\w*)\s*\("
        r"(?P<params>[^;{{}}]*)\)\s*;",
        re.DOTALL,
    )
    discovered: list[MarkedFunction] = []
    for header in headers:
        text = _strip_comments(header.read_text(encoding="utf-8"))
        for match in declaration_re.finditer(text):
            return_annotations, _ = _leading_annotations(match.group("return_decl"))
            discovered.append(
                MarkedFunction(
                    header=header,
                    name=match.group("name"),
                    return_annotations=return_annotations,
                    param_annotations=_parse_param_annotations(match.group("params")),
                )
            )
    return tuple(discovered)
