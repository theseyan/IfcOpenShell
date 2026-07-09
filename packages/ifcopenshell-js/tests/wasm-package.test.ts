
import { describe, it, expect } from 'vitest';
import { execFileSync } from 'node:child_process';
import { resolvePackagedWasmDir } from './_helper.js';

describe('@ifcopenshell-js/wasm integration', () => {
  it('resolveWasmAssets prefers the staged wasm package directory', async () => {
    const wasm = await import('@ifcopenshell-js/wasm');
    const wasmDir = resolvePackagedWasmDir();
    expect(wasmDir).not.toBeNull();
    if (!wasmDir) return;

    if (wasm.wasmArtifactsPresent(wasmDir)) {
      const assets = await wasm.resolveWasmAssets(wasmDir);
      expect(typeof assets.initModule).toBe('function');
      expect(assets.manifest.schema?.ifc4).toBeTruthy();
      expect(assets.apiModuleUrl).toContain('ifcopenshell_api.mjs');
    }
  });

  it('browser condition resolves bundled asset URLs without an app wasm server', () => {
    const output = execFileSync(process.execPath, [
      '--conditions=browser',
      '--input-type=module',
      '--eval',
      [
        "import { resolveWasmAssets } from '@ifcopenshell-js/wasm';",
        'const assets = await resolveWasmAssets();',
        'console.log(JSON.stringify({',
        '  wasmUrl: assets.wasmUrl,',
        '  pluginBaseUrl: assets.pluginBaseUrl,',
        '  schemaUrl: assets.manifest.schema.ifc4.wasm,',
        '  hasFactory: typeof assets.initModule === "function",',
        '  hasApiFactory: typeof assets.createIfcOpenshellModule === "function"',
        '}));',
      ].join('\n'),
    ], { cwd: process.cwd(), encoding: 'utf8' });

    const resolved = JSON.parse(output) as {
      wasmUrl: string;
      pluginBaseUrl: string;
      schemaUrl: string;
      hasFactory: boolean;
      hasApiFactory: boolean;
    };
    expect(resolved.wasmUrl).toContain('ifcopenshell_wasm.wasm');
    expect(resolved.pluginBaseUrl).toContain('asset-manifest.js');
    expect(resolved.schemaUrl).toContain('ifcopenshell.parse.schema.ifc4.wasm');
    expect(resolved.hasFactory).toBe(true);
    expect(resolved.hasApiFactory).toBe(true);
  });
});
