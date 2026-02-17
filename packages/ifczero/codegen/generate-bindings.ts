/**
 * Parses C API headers and Zig export files to generate typed TypeScript
 * bindings around Emscripten's cwrap().
 *
 * Usage: npx tsx codegen/generate-bindings.ts
 */

import { readFileSync, writeFileSync } from "fs";
import { resolve, dirname } from "path";
import { fileURLToPath } from "url";

const __dirname = dirname(fileURLToPath(import.meta.url));
const ROOT = resolve(__dirname, "..");
const REPO = resolve(ROOT, "../..");

// ── C type → Emscripten cwrap type mapping ──────────────────────────

type CWrapType = "number" | "string" | "boolean" | null;

const C_TYPE_MAP: Record<string, { cwrap: CWrapType; ts: string }> = {
  void: { cwrap: null, ts: "void" },
  int: { cwrap: "number", ts: "number" },
  "unsigned int": { cwrap: "number", ts: "number" },
  size_t: { cwrap: "number", ts: "number" },
  double: { cwrap: "number", ts: "number" },
  float: { cwrap: "number", ts: "number" },
  "const char*": { cwrap: "string", ts: "string" },
  "char*": { cwrap: "string", ts: "string" },
};

// Opaque pointer types → all map to number (wasm pointer)
const OPAQUE_PTR_TYPES = [
  "file_t*",
  "const file_t*",
  "entity_ref_t*",
  "const entity_ref_t*",
  "entity_list_t*",
  "const entity_list_t*",
  "int_list_t*",
  "const int_list_t*",
  "double_list_t*",
  "const double_list_t*",
  "string_list_t*",
  "const string_list_t*",
  "type_ref_t*",
  "const type_ref_t*",
  "type_list_t*",
  "const type_list_t*",
  "attribute_ref_t*",
  "const attribute_ref_t*",
  "attribute_list_t*",
  "const attribute_list_t*",
  "inverse_attr_ref_t*",
  "const inverse_attr_ref_t*",
  "inverse_attribute_list_t*",
  "const inverse_attribute_list_t*",
  "parameter_type_ref_t*",
  "const parameter_type_ref_t*",
  "int_matrix_t*",
  "const int_matrix_t*",
  "double_matrix_t*",
  "const double_matrix_t*",
  "entity_matrix_t*",
  "const entity_matrix_t*",
  "settings_t*",
  "const settings_t*",
  "geom_settings_t*",
  "const geom_settings_t*",
  "iterator_t*",
  "const iterator_t*",
  "mesh_t*",
  "const mesh_t*",
  "serialized_t*",
  "const serialized_t*",
  "created_shape_t*",
  "const created_shape_t*",
  "serializer_t*",
  "const serializer_t*",
  "void*",
  "const void*",
  "const double*",
  "const int*",
  "const entity_ref_t* const*",
  "const char* const*",
];

interface ParsedFunction {
  name: string;
  shortName: string;
  returnType: string;
  returnCwrap: CWrapType;
  returnTs: string;
  params: { name: string; cType: string; cwrap: CWrapType; ts: string }[];
}

// ── C header parser ─────────────────────────────────────────────────

function resolveCType(raw: string): { cwrap: CWrapType; ts: string } {
  const t = raw.trim().replace(/\s+/g, " ");
  if (C_TYPE_MAP[t]) return C_TYPE_MAP[t];
  // Any pointer type → number
  if (t.endsWith("*") || t.includes("*")) return { cwrap: "number", ts: "number" };
  // Enum types (filetype_t, status_t, etc.) → number
  if (t.endsWith("_t")) return { cwrap: "number", ts: "number" };
  return { cwrap: "number", ts: "number" };
}

function parseCHeader(path: string, prefix: string): ParsedFunction[] {
  const content = readFileSync(path, "utf-8");
  const fns: ParsedFunction[] = [];

  // Match function declarations: return_type FUNCNAME(params)
  // They may span multiple lines. Look for ifcopenshell_ prefix.
  const lines = content.split("\n");
  let buffer = "";

  for (const line of lines) {
    const trimmed = line.trim();

    // Skip preprocessor, comments, empty
    if (
      trimmed.startsWith("#") ||
      trimmed.startsWith("//") ||
      trimmed.startsWith("/*") ||
      trimmed.startsWith("*") ||
      trimmed === ""
    )
      continue;

    buffer += " " + trimmed;

    // Check if we have a complete declaration (ends with ;)
    if (!buffer.includes(";")) continue;

    const decl = buffer.trim();
    buffer = "";

    // Look for ifcopenshell_ function declarations
    const match = decl.match(
      /^(?:EMSCRIPTEN_KEEPALIVE\s+)?(.+?)\s+(ifcopenshell_\w+)\s*\(([^)]*)\)\s*;/
    );
    if (!match) continue;

    const [, retTypeRaw, fullName, paramsRaw] = match;
    if (!fullName.startsWith(prefix)) continue;
    const retType = retTypeRaw
      .replace(/EMSCRIPTEN_KEEPALIVE/g, "")
      .replace(/extern\s+"C"/g, "")
      .trim();

    const shortName = fullName.replace(prefix, "");
    const retInfo = resolveCType(retType);

    const params: ParsedFunction["params"] = [];
    if (paramsRaw.trim() && paramsRaw.trim() !== "void") {
      // Split params by comma, handling nested types
      const paramParts = splitParams(paramsRaw);
      for (const param of paramParts) {
        const p = param.trim();
        if (!p) continue;
        const { name, type } = extractParamNameAndType(p);
        const info = resolveCType(type);
        params.push({ name, cType: type, cwrap: info.cwrap, ts: info.ts });
      }
    }

    fns.push({
      name: fullName,
      shortName,
      returnType: retType,
      returnCwrap: retInfo.cwrap,
      returnTs: retInfo.ts,
      params,
    });
  }

  return fns;
}

function splitParams(raw: string): string[] {
  const parts: string[] = [];
  let depth = 0;
  let current = "";
  for (const ch of raw) {
    if (ch === "(") depth++;
    else if (ch === ")") depth--;
    else if (ch === "," && depth === 0) {
      parts.push(current);
      current = "";
      continue;
    }
    current += ch;
  }
  if (current.trim()) parts.push(current);
  return parts;
}

function extractParamNameAndType(param: string): {
  name: string;
  type: string;
} {
  const p = param.trim();

  // Handle array params like "double out[16]"
  const arrayMatch = p.match(/^(.+?)\s+(\w+)\s*\[.*\]$/);
  if (arrayMatch) {
    return { type: arrayMatch[1].trim() + "*", name: arrayMatch[2] };
  }

  // Handle function-like params (shouldn't appear but just in case)
  // Normal: "const char* name" or "int x" or "const entity_ref_t*"
  // Find the last word that's a valid identifier (the parameter name)
  const parts = p.split(/\s+/);
  if (parts.length === 1) {
    // Just a type, no name (like "void")
    return { type: p, name: "" };
  }

  // Last part might be the name, possibly with * prefix
  let name = parts[parts.length - 1];
  let type: string;

  if (name.startsWith("*")) {
    // "const entity_ref_t *name" → type = "const entity_ref_t*", name = "name"
    name = name.slice(1);
    type = parts.slice(0, -1).join(" ") + "*";
  } else if (name === "*") {
    // Unnamed pointer param
    type = p;
    name = "";
  } else {
    type = parts.slice(0, -1).join(" ");
  }

  // Clean up: remove trailing * from type if name was pointer-qualified
  type = type.replace(/\s+\*$/, "*").replace(/\s+/g, " ").trim();

  return { type, name };
}

function normalizeZigType(type: string): string {
  return type.replace(/\s+/g, "");
}

function isZigCStringPointer(type: string): boolean {
  const t = normalizeZigType(type);
  return t.includes("[*:0]constu8");
}

// ── Zig export parser ───────────────────────────────────────────────

function parseZigExports(path: string, prefix: string): ParsedFunction[] {
  const content = readFileSync(path, "utf-8");
  const fns: ParsedFunction[] = [];

  const ZIG_TYPE_MAP: Record<string, { cwrap: CWrapType; ts: string }> = {
    "c_int": { cwrap: "number", ts: "number" },
    "c_uint": { cwrap: "number", ts: "number" },
    usize: { cwrap: "number", ts: "number" },
    f64: { cwrap: "number", ts: "number" },
    f32: { cwrap: "number", ts: "number" },
    void: { cwrap: null, ts: "void" },
  };

  // Buffer lines to handle multiline function declarations
  const lines = content.split("\n");
  let buffer = "";
  let inExport = false;

  for (const line of lines) {
    const trimmed = line.trim();

    if (trimmed.startsWith("export fn ifcopenshell_")) {
      inExport = true;
      buffer = trimmed;
    } else if (inExport) {
      buffer += " " + trimmed;
    }

    if (!inExport) continue;
    if (!buffer.includes("{")) continue;

    // We have a complete declaration
    inExport = false;
    const match = buffer.match(
      /export fn (ifcopenshell_\w+)\(([^)]*)\)\s*(?:callconv\([^)]*\)\s*)?(.*?)\s*\{/
    );
    buffer = "";
    if (!match) continue;

    const [, fullName, paramsRaw, retTypeRaw] = match;
    if (!fullName.startsWith(prefix)) continue;
    const shortName = fullName.replace(prefix, "");

    // Parse return type
    let retType = retTypeRaw.trim();
    let retInfo: { cwrap: CWrapType; ts: string };

    if (retType.startsWith("?")) {
      // Optional pointer → number (nullable)
      retInfo = { cwrap: "number", ts: "number" };
    } else if (ZIG_TYPE_MAP[retType]) {
      retInfo = ZIG_TYPE_MAP[retType];
    } else if (retType.startsWith("*") || retType.startsWith("[*")) {
      retInfo = { cwrap: "number", ts: "number" };
    } else {
      retInfo = { cwrap: "number", ts: "number" };
    }

    // Parse params
    const params: ParsedFunction["params"] = [];
    if (paramsRaw.trim()) {
      for (const param of paramsRaw.split(",")) {
        const p = param.trim();
        if (!p) continue;
        const colonIdx = p.indexOf(":");
        if (colonIdx === -1) continue;
        const name = p.slice(0, colonIdx).trim();
        let zigType = p.slice(colonIdx + 1).trim();

        let info: { cwrap: CWrapType; ts: string };
        if (zigType.startsWith("?")) {
          // Unwrap optional to check the inner type
          const inner = zigType.slice(1);
          if ((inner.startsWith("[*") || inner.startsWith("*")) && isZigCStringPointer(inner)) {
            info = { cwrap: "string", ts: "string | null" };
          } else {
            info = { cwrap: "number", ts: "number" };
          }
        } else if (ZIG_TYPE_MAP[zigType]) {
          info = ZIG_TYPE_MAP[zigType];
        } else if (zigType.startsWith("*") || zigType.startsWith("[*")) {
          if (isZigCStringPointer(zigType)) {
            info = { cwrap: "string", ts: "string" };
          } else {
            info = { cwrap: "number", ts: "number" };
          }
        } else {
          info = { cwrap: "number", ts: "number" };
        }

        params.push({ name, cType: zigType, cwrap: info.cwrap, ts: info.ts });
      }
    }

    fns.push({
      name: fullName,
      shortName,
      returnType: retTypeRaw,
      returnCwrap: retInfo.cwrap,
      returnTs: retInfo.ts,
      params,
    });
  }

  return fns;
}

// ── Code generation ─────────────────────────────────────────────────

function generateBindings(fns: ParsedFunction[], moduleName: string): string {
  const lines: string[] = [
    `// Auto-generated by codegen/generate-bindings.ts — DO NOT EDIT`,
    ``,
    `import type { EmscriptenModule } from "../wasm/types.js";`,
    ``,
    `export type Ptr = number;`,
    ``,
  ];

  for (const fn of fns) {
    const paramTypes = fn.params
      .map((p) => (p.cwrap === null ? "null" : `"${p.cwrap}"`))
      .join(", ");
    const paramList = fn.params
      .map((p, i) => `${p.name || `arg${i}`}: ${p.ts}`)
      .join(", ");
    const retCwrap = fn.returnCwrap === null ? "null" : `"${fn.returnCwrap}"`;

    const paramSuffix = paramList ? `, ${paramList}` : "";
    lines.push(
      `export function ${fn.shortName}(M: EmscriptenModule${paramSuffix}): ${fn.returnTs} {`
    );
    const args = fn.params.map((p, i) => p.name || `arg${i}`).join(", ");
    lines.push(
      `  return M.ccall("${fn.name}", ${retCwrap}, [${paramTypes}], [${args}]);`
    );
    lines.push(`}`);
    lines.push(``);
  }

  return lines.join("\n");
}

// ── Main ────────────────────────────────────────────────────────────

const modules: {
  name: string;
  prefix: string;
  headers: { path: string; lang: "c" | "zig" }[];
  output: string;
}[] = [
  {
    name: "parse",
    prefix: "ifcopenshell_ifcparse_",
    headers: [
      { path: "zig/lib/ifcparse/c_api.h", lang: "c" },
    ],
    output: "src/parse/bindings.generated.ts",
  },
  {
    name: "util",
    prefix: "ifcopenshell_ifcutil_",
    headers: [
      { path: "zig/lib/ifcutil_capi.zig", lang: "zig" },
    ],
    output: "src/parse/util-bindings.generated.ts",
  },
  {
    name: "geom",
    prefix: "ifcopenshell_ifcgeom_",
    headers: [
      { path: "zig/lib/ifcgeom/c_api.h", lang: "c" },
    ],
    output: "src/geom/bindings.generated.ts",
  },
  {
    name: "serializers",
    prefix: "ifcopenshell_ifcserializers_",
    headers: [
      { path: "zig/lib/serializers/c_api.h", lang: "c" },
    ],
    output: "src/serializers/bindings.generated.ts",
  },
  {
    name: "psetqto",
    prefix: "ifcopenshell_psetqto_",
    headers: [
      { path: "zig/lib/psetqto_capi.zig", lang: "zig" },
    ],
    output: "src/parse/psetqto-bindings.generated.ts",
  },
];

let totalFns = 0;
for (const mod of modules) {
  let fns: ParsedFunction[] = [];
  for (const header of mod.headers) {
    const absPath = resolve(REPO, header.path);
    if (header.lang === "zig") {
      fns.push(...parseZigExports(absPath, mod.prefix));
    } else {
      fns.push(...parseCHeader(absPath, mod.prefix));
    }
  }

  const output = generateBindings(fns, mod.name);
  const outPath = resolve(ROOT, mod.output);
  writeFileSync(outPath, output);
  console.log(`${mod.name}: ${fns.length} functions → ${mod.output}`);
  totalFns += fns.length;
}

console.log(`\nTotal: ${totalFns} bindings generated`);
