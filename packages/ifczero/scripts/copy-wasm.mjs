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

const fileCount = (dir) => {
  let count = 0;
  for (const entry of readdirSync(dir)) {
    const fullPath = join(dir, entry);
    const stat = statSync(fullPath);
    if (stat.isDirectory()) {
      count += fileCount(fullPath);
    } else if (stat.isFile()) {
      count += 1;
    }
  }
  return count;
};

const entries = fileCount(wasmDir);
if (entries === 0) {
  throw new Error(`No wasm artifacts found in ${wasmDir}. Run 'zig build wasm-full' first.`);
}

mkdirSync(distWasmDir, { recursive: true });
for (const entry of readdirSync(wasmDir)) {
  cpSync(join(wasmDir, entry), join(distWasmDir, entry), { recursive: true, force: true });
}

console.log(`Copied ${entries} wasm artifact(s) to dist/wasm`);
