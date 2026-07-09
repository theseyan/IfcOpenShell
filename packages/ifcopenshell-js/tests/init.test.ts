
import { describe, expect, it } from 'vitest';
import { describeOrSkip } from './_helper.js';
import { init, IfcOpenShellError } from '../src/index.js';

describeOrSkip('init', () => {
  it('boots the default direct runtime in Node', async () => {
    const shell = await init();
    try {
      await shell.loadPlugin('schema', 'ifc4');
      expect(await shell.loadedPlugins()).toContain('schema:ifc4');
    } finally {
      await shell.dispose();
    }
  });

  it('rejects unknown plugins with the public error type', async () => {
    const shell = await init();
    try {
      await expect(shell.loadPlugin('schema', 'definitely-not-a-schema')).rejects.toBeInstanceOf(IfcOpenShellError);
    } finally {
      await shell.dispose();
    }
  });
});
