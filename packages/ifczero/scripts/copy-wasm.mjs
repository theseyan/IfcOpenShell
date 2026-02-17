import { cpSync, existsSync, mkdirSync, readdirSync, statSync } from "node:fs";
import { dirname, join } from "node:path";
import { fileURLToPath } from "node:url";

const __dirname = dirname(fileURLToPath(import.meta.url));
const pkgRoot = join(__dirname, "..");
const wasmDir = join(pkgRoot, "wasm");
const distWasmDir = join(pkgRoot, "dist", "wasm");

if (!existsSync(wasmDir)) {
  throw new Error(`Missing wasm directory: ${wasmDir}. Run 'zig build wasm-full' first.`);
}

const entries = readdirSync(wasmDir).filter((name) => statSync(join(wasmDir, name)).isFile());
if (entries.length === 0) {
  throw new Error(`No wasm artifacts found in ${wasmDir}. Run 'zig build wasm-full' first.`);
}

mkdirSync(distWasmDir, { recursive: true });
for (const name of entries) {
  cpSync(join(wasmDir, name), join(distWasmDir, name));
}

console.log(`Copied ${entries.length} wasm artifact(s) to dist/wasm`);
