# This file was generated with the assistance of an AI coding tool.

from __future__ import annotations

from dataclasses import dataclass
from pathlib import Path
import re


@dataclass(frozen=True)
class MarkedFunction:
    header: Path
    name: str


_COMMENT_RE = re.compile(r"//.*?$|/\*.*?\*/", re.MULTILINE | re.DOTALL)


def _strip_comments(text: str) -> str:
    return _COMMENT_RE.sub("", text)


def discover_marked_functions_in_headers(
    headers: list[Path] | tuple[Path, ...],
    *,
    marker: str = "IFCAPI_BINDING",
) -> tuple[MarkedFunction, ...]:
    """Discover function declarations annotated with a binding contract marker."""
    marker_re = re.escape(marker)
    declaration_re = re.compile(
        rf"\b{marker_re}\s+"
        r"(?:[\w:<>~,\s*&]+?)\s+"
        r"(?P<name>[A-Za-z_]\w*)\s*\("
        r"(?P<params>[^;{{}}]*)\)\s*;",
        re.DOTALL,
    )
    discovered: list[MarkedFunction] = []
    for header in headers:
        text = _strip_comments(header.read_text(encoding="utf-8"))
        for match in declaration_re.finditer(text):
            discovered.append(MarkedFunction(header=header, name=match.group("name")))
    return tuple(discovered)
