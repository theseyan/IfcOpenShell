
import { readFileSync, existsSync } from 'node:fs';
import { fileURLToPath, pathToFileURL } from 'node:url';
import { resolve, dirname } from 'node:path';
import { describe } from 'vitest';
import { init } from '../src/index.js';
import type { IfcOpenShell, PluginManifest } from '../src/index.js';
import type { EmscriptenModuleFactory, WasmAssets } from '../src/types.js';

const __dirname = dirname(fileURLToPath(import.meta.url));
const REPO_ROOT = resolve(__dirname, '..', '..', '..');

const REQUIRED_WASM_FILES = [
  'ifcopenshell_api.mjs',
  'ifcopenshell_wasm.mjs',
  'ifcopenshell_wasm.node.mjs',
  'ifcopenshell_wasm.wasm',
  'ifcopenshell_plugins.json',
] as const;

export const SAMPLE_IFC = resolve(REPO_ROOT, 'test', 'input', 'WallInstance_IFC4Add2.ifc');
export const LARGE_OFFSET_IFC = resolve(REPO_ROOT, 'test', 'input', 'large_offset.ifc');

type WasmArtifactKind = 'env' | 'packaged' | 'generated';

interface WasmArtifactCandidate {
  readonly kind: WasmArtifactKind;
  readonly path: string;
}

const ENV_WASM_DIR = process.env.IFCOPENSHELL_WASM_DIR ? resolve(process.env.IFCOPENSHELL_WASM_DIR) : null;

const WASM_ARTIFACT_CANDIDATES: readonly WasmArtifactCandidate[] = [
  ...(ENV_WASM_DIR ? [{ kind: 'env' as const, path: ENV_WASM_DIR }] : []),
  { kind: 'packaged', path: resolve(REPO_ROOT, 'packages', 'ifcopenshell-wasm', 'wasm') },
  { kind: 'generated', path: resolve(REPO_ROOT, 'build', 'wasm-native', 'ifcopenshell', 'full', 'ifcwrap', 'wasm') },
  { kind: 'generated', path: resolve(REPO_ROOT, 'build-wasm', 'ifcwrap', 'wasm') },
];

function hasRequiredWasmFiles(dir: string): boolean {
  return REQUIRED_WASM_FILES.every((name) => existsSync(resolve(dir, name)));
}

function findWasmArtifacts(kinds: readonly WasmArtifactKind[]): string | null {
  for (const kind of kinds) {
    for (const candidate of WASM_ARTIFACT_CANDIDATES) {
      if (candidate.kind === kind && hasRequiredWasmFiles(candidate.path)) {
        return candidate.path;
      }
    }
  }
  return null;
}

/** Package tests inspect the staged wasm package layout. */
export function resolvePackagedWasmDir(): string | null {
  return findWasmArtifacts(['env', 'packaged']);
}

/** Generated API tests need a build output, not the staged package copy. */
export function resolveGeneratedWasmDir(): string | null {
  return findWasmArtifacts(['env', 'generated']);
}

/** Runtime source-tree tests use artifacts generated from the current checkout. */
export function resolveRuntimeWasmDir(): string | null {
  return findWasmArtifacts(['env', 'generated', 'packaged']);
}

export const WASM_DIR = resolveRuntimeWasmDir();
export const GENERATED_WASM_DIR = resolveGeneratedWasmDir();

export const generatedWasmAvailable =
  GENERATED_WASM_DIR !== null && existsSync(SAMPLE_IFC) && existsSync(LARGE_OFFSET_IFC);

export const describeGeneratedOrSkip = generatedWasmAvailable ? describe : describe.skip;

export const wasmAvailable = WASM_DIR !== null && existsSync(SAMPLE_IFC) && existsSync(LARGE_OFFSET_IFC);

export const describeOrSkip = wasmAvailable ? describe : describe.skip;

function loadManifest(dir: string): PluginManifest {
  return JSON.parse(
    readFileSync(resolve(dir, 'ifcopenshell_plugins.json'), 'utf8'),
  ) as PluginManifest;
}

/** True when the manifest includes the full-profile OCCT kernel. */
export function isFullProfile(manifest: PluginManifest): boolean {
  return manifest.kernel?.opencascade !== undefined;
}

export const fullWasmAvailable = wasmAvailable && WASM_DIR !== null && isFullProfile(loadManifest(WASM_DIR));

export const describeOrSkipFull = fullWasmAvailable ? describe : describe.skip;

export async function resolveWasmAssetsFromDir(dir: string): Promise<WasmAssets> {
  const manifest = loadManifest(dir);
  const wasmModuleUrl = pathToFileURL(resolve(dir, 'ifcopenshell_wasm.node.mjs')).href;
  const wasmModule = (await import(wasmModuleUrl)).default as EmscriptenModuleFactory;
  const base = pathToFileURL(dir).href;
  return {
    initModule: wasmModule,
    wasmUrl: resolve(dir, 'ifcopenshell_wasm.wasm'),
    pluginBaseUrl: base.endsWith('/') ? base : `${base}/`,
    manifest,
    apiModuleUrl: pathToFileURL(resolve(dir, 'ifcopenshell_api.mjs')).href,
  };
}

export async function createInstance(wasmDir: string = WASM_DIR!): Promise<IfcOpenShell> {
  return init({ wasmRoot: wasmDir });
}

export function readSampleIfcBytes(): Uint8Array {
  return new Uint8Array(readFileSync(SAMPLE_IFC));
}

export function readLargeOffsetIfcBytes(): Uint8Array {
  return new Uint8Array(readFileSync(LARGE_OFFSET_IFC));
}
