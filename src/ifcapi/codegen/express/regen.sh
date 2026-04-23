#!/usr/bin/env bash
# SPDX-License-Identifier: LGPL-3.0-or-later
# This file was generated with the assistance of an AI coding tool.
#
# Regenerate src/ifcapi/src/express/generated/<SCHEMA>.cpp from the
# upstream EXPRESS schemas. The .cpp outputs are committed; this
# script is only needed when bumping the supported schemas or when
# fixing the codegen tool itself.
#
# Requires:
#   * python3 with the IfcOpenShell-Python checkout importable
#     (pip install -e src/ifcopenshell-python)
#   * curl
#   * a built ifcopenshell_wrapper (e.g. via cmake --build build-capi-stable)
#
# Usage:
#   bash src/ifcapi/codegen/express/regen.sh                   # all schemas
#   bash src/ifcapi/codegen/express/regen.sh IFC4 IFC4X1       # subset
#
# Sources are downloaded from the canonical buildingSMART standards
# server when available, with fallbacks for the X-series schemas
# that bSI does not host publicly.

set -euo pipefail

REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../../../.." && pwd)"
CACHE_DIR="${IFCAPI_EXP_CACHE:-${REPO_ROOT}/.exp-cache}"
OUT_DIR="${REPO_ROOT}/src/ifcapi/src/express/generated"
COMPILER="${REPO_ROOT}/src/ifcapi/codegen/express/express_cpp_compiler.py"

mkdir -p "${CACHE_DIR}" "${OUT_DIR}"

declare SCHEMAS_LIST="IFC2X3_TC1 IFC4 IFC4X1 IFC4X3_ADD2"

url_for() {
    case "$1" in
        IFC2X3_TC1)  echo "https://standards.buildingsmart.org/IFC/RELEASE/IFC2x3/TC1/EXPRESS/IFC2X3_TC1.exp" ;;
        IFC4)        echo "https://standards.buildingsmart.org/IFC/RELEASE/IFC4/ADD2_TC1/EXPRESS/IFC4.exp" ;;
        IFC4X1)      echo "https://raw.githubusercontent.com/ifcwebserver/ifcwebserver/master/express/IFC4X1.exp" ;;
        IFC4X3_ADD2) echo "https://raw.githubusercontent.com/ifcwebserver/ifcwebserver/master/express/IFC4X3_ADD2.exp" ;;
        *)           echo "" ;;
    esac
}

# Default: all known schemas. Can override via CLI args.
if [ "$#" -eq 0 ]; then
    set -- ${SCHEMAS_LIST}
fi

for SCHEMA in "$@"; do
    URL="$(url_for "${SCHEMA}")"
    if [ -z "${URL}" ]; then
        echo "Unknown schema: ${SCHEMA}" >&2
        exit 1
    fi
    EXP="${CACHE_DIR}/${SCHEMA}.exp"
    if [ ! -f "${EXP}" ]; then
        echo "[regen] downloading ${SCHEMA} from ${URL}"
        curl -fsSL "${URL}" -o "${EXP}"
    else
        echo "[regen] using cached ${EXP}"
    fi
    OUT="${OUT_DIR}/${SCHEMA}.cpp"
    echo "[regen] generating ${OUT}"
    PYTHONPATH="${REPO_ROOT}/src/ifcopenshell-python:${REPO_ROOT}/src/ifcopenshell-python/ifcopenshell/express:${PYTHONPATH:-}" \
        python3 "${COMPILER}" "${EXP}" "${OUT}"
done

echo "[regen] done. Generated:"
ls -la "${OUT_DIR}"/*.cpp
