import { cpSync, existsSync, mkdirSync, readdirSync, rmSync, statSync } from "node:fs";
import { dirname, join } from "node:path";
import { fileURLToPath } from "node:url";

const __dirname = dirname(fileURLToPath(import.meta.url));
const pkgRoot = join(__dirname, "..");
const wasmDir = join(pkgRoot, "wasm");
const distDir = join(pkgRoot, "dist");
const distWasmDir = join(distDir, "wasm");

if (!existsSync(wasmDir)) {
  throw new Error(`Missing wasm directory: ${wasmDir}. Run 'zig build wasm-full' first.`);
}

const entries = readdirSync(wasmDir).filter((name) => statSync(join(wasmDir, name)).isFile());
if (entries.length === 0) {
  throw new Error(`No wasm artifacts found in ${wasmDir}. Run 'zig build wasm-full' first.`);
}

mkdirSync(distDir, { recursive: true });
rmSync(distWasmDir, { recursive: true, force: true });
mkdirSync(distWasmDir, { recursive: true });

for (const name of entries) {
  const src = join(wasmDir, name);
  cpSync(src, join(distWasmDir, name));
  cpSync(src, join(distDir, name));
}

console.log(`Copied ${entries.length} wasm artifact(s) to dist and dist/wasm`);
