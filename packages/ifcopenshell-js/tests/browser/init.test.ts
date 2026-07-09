
import { describe, expect, it } from 'vitest';
import { init } from '../../src/index.js';

describe('browser runtime', () => {
  it('initializes the browser package and loads an IFC schema plugin', async () => {
    const shell = await init();
    try {
      await shell.loadPlugin('schema', 'ifc4');
      expect(shell.loadedPlugins()).toContain('schema:ifc4');
    } finally {
      shell.dispose();
    }
  });
});
