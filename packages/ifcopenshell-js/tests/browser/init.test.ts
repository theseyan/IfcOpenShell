
import { describe, expect, it } from 'vitest';
import {
  IfcOpenShellError,
  IfcOpenShellErrorCode,
  IfcOpenShellErrorKind,
  abortError,
  init,
  isIfcOpenShellAbortError,
} from '../../src/index.js';

describe('browser runtime', () => {
  it('initializes the browser package and loads an IFC schema plugin', async () => {
    const shell = await init();
    try {
      await shell.loadPlugin('schema', 'ifc4');
      expect(shell.loadedPlugins()).toContain('schema:ifc4');
      const file = shell.raw.parse.newFile('IFC4', 0, '');
      expect(file).not.toBeNull();
      file?.destroy();
    } finally {
      shell.dispose();
    }
  });

  it('exposes the shared typed error and abort contract', async () => {
    const shell = await init();
    try {
      let error: unknown;
      try {
        shell.raw.cogo.bearing2dd('not a quadrant bearing');
      } catch (caught) {
        error = caught;
      }
      expect(error).toBeInstanceOf(IfcOpenShellError);
      expect(error).toMatchObject({
        kind: IfcOpenShellErrorKind.VALUE,
        code: IfcOpenShellErrorCode.INVALID_QUADRANT_BEARING,
      });
      expect(shell.raw.selector.format(null, null, '{{undefined}}')).toBeNull();

      const reason = new Error('browser abort reason');
      const cancelled = abortError('Browser operation cancelled', reason);
      expect(cancelled).toBeInstanceOf(IfcOpenShellError);
      expect(isIfcOpenShellAbortError(cancelled)).toBe(true);
      expect(cancelled).toMatchObject({
        name: 'AbortError',
        kind: IfcOpenShellErrorKind.CANCELLED,
        code: IfcOpenShellErrorCode.OPERATION_CANCELLED,
        cause: reason,
      });
    } finally {
      shell.dispose();
    }
  });
});
