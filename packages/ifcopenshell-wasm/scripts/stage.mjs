
import { cpSync, existsSync, mkdirSync, readdirSync, readFileSync, rmSync, statSync, writeFileSync } from 'node:fs';
import { dirname, join, resolve } from 'node:path';
import { fileURLToPath } from 'node:url';

const __dirname = dirname(fileURLToPath(import.meta.url));
const PACKAGE_ROOT = resolve(__dirname, '..');
const REPO_ROOT = resolve(PACKAGE_ROOT, '..', '..');
const DEST = join(PACKAGE_ROOT, 'wasm');

const DEFAULT_SOURCES = [
  process.env.IFCOPENSHELL_WASM_DIR,
  resolve(REPO_ROOT, 'build', 'wasm-native', 'ifcopenshell', 'full', 'ifcwrap', 'wasm'),
  resolve(REPO_ROOT, 'build-wasm', 'ifcwrap', 'wasm'),
].filter(Boolean);

const REQUIRED = [
  'ifcopenshell_wasm.mjs',
  'ifcopenshell_wasm.node.mjs',
  'ifcopenshell_wasm.wasm',
  'ifcopenshell_api.mjs',
  'ifcopenshell_plugins.json',
];

function copyDir(src, dest) {
  mkdirSync(dest, { recursive: true });
  for (const entry of readdirSync(src)) {
    const from = join(src, entry);
    const to = join(dest, entry);
    if (statSync(from).isDirectory()) {
      copyDir(from, to);
    } else {
      cpSync(from, to);
    }
  }
}

function resolveSource() {
  for (const candidate of DEFAULT_SOURCES) {
    if (REQUIRED.every((name) => existsSync(join(candidate, name)))) {
      return candidate;
    }
  }
  return null;
}

function toPosixPath(path) {
  return path.split('\\').join('/');
}

function writeAssetManifest() {
  const manifest = JSON.parse(readFileSync(join(DEST, 'ifcopenshell_plugins.json'), 'utf8'));
  const writeEntry = (entry) => {
    const fields = [
      `wasm: new URL(${JSON.stringify(`./wasm/${toPosixPath(entry.wasm)}`)}, import.meta.url).href`,
    ];
    if (entry.depends) {
      fields.push(`depends: ${JSON.stringify(entry.depends)}`);
    }
    return `{ ${fields.join(', ')} }`;
  };
  const lines = [
    '',
    "import initModule from './wasm/ifcopenshell_wasm.mjs';",
    "import { createIfcOpenshellModule } from './wasm/ifcopenshell_api.mjs';",
    '',
    'export { createIfcOpenshellModule, initModule };',
    "export const wasmUrl = new URL('./wasm/ifcopenshell_wasm.wasm', import.meta.url).href;",
    'export const pluginBaseUrl = import.meta.url;',
    'export const manifest = {',
  ];

  for (const [kind, entries] of Object.entries(manifest)) {
    lines.push(`  ${JSON.stringify(kind)}: {`);
    for (const [id, entry] of Object.entries(entries)) {
      lines.push(`    ${JSON.stringify(id)}: ${writeEntry(entry)},`);
    }
    lines.push('  },');
  }

  lines.push('};', '');
  writeFileSync(join(PACKAGE_ROOT, 'asset-manifest.js'), lines.join('\n'));
}

const source = resolveSource();
if (!source) {
  console.error(
    'No WASM build output found. Build the WASM target first, then rerun stage:\n' +
      '  python nix/wasm_native.py --profile full build\n' +
      'Or set IFCOPENSHELL_WASM_DIR to an existing ifcwrap/wasm directory.',
  );
  process.exit(1);
}

rmSync(DEST, { recursive: true, force: true });
copyDir(source, DEST);
rmSync(join(DEST, 'ifcopenshell_wasm.node.wasm'), { force: true });
writeAssetManifest();
console.log(`Staged IfcOpenShell WASM assets from ${source} -> ${DEST}`);
