import { afterAll, beforeAll, expect, it } from 'vitest';
import {
  IfcFile,
  IfcOpenShellError,
  IfcOpenShellErrorCode,
  IfcOpenShellErrorKind,
  type IfcOpenShell,
} from '../src/index.js';
import { createInstance, describeGeneratedOrSkip, GENERATED_WASM_DIR } from './_helper.js';

describeGeneratedOrSkip('typed cross-target error contract', () => {
  let shell: IfcOpenShell;

  beforeAll(async () => {
    shell = await createInstance(GENERATED_WASM_DIR!);
    await shell.loadPlugin('schema', 'ifc4');
  });

  afterAll(() => shell?.dispose());

  it('shares typed value errors across raw and high-level APIs', () => {
    let rawError: unknown;
    try {
      shell.raw.cogo.bearing2dd('N invalid E');
    } catch (error) {
      rawError = error;
    }

    expect(rawError).toBeInstanceOf(IfcOpenShellError);
    expect(rawError).toMatchObject({
      kind: IfcOpenShellErrorKind.VALUE,
      code: IfcOpenShellErrorCode.INVALID_QUADRANT_BEARING,
      message: 'Invalid bearing string',
    });

    let highLevelError: unknown;
    try {
      shell.api.cogo.bearing2dd('wording is diagnostic only');
    } catch (error) {
      highLevelError = error;
    }
    expect(highLevelError).toBeInstanceOf(IfcOpenShellError);
    expect(highLevelError).toMatchObject({
      kind: IfcOpenShellErrorKind.VALUE,
      code: IfcOpenShellErrorCode.INVALID_QUADRANT_BEARING,
    });

    const contextual = new IfcOpenShellError('Bearing conversion failed', rawError);
    expect(contextual.kind).toBe(IfcOpenShellErrorKind.VALUE);
    expect(contextual.code).toBe(IfcOpenShellErrorCode.INVALID_QUADRANT_BEARING);
    expect(contextual.cause).toBe(rawError);
  });

  it('preserves upstream resource validation precedence', async () => {
    await using file = await IfcFile.createEmpty(shell, 'IFC4');
    shell.api.root.createEntity(file, { ifcClass: 'IfcProject' });
    using resource = shell.api.resource.addResource(file, {});

    expect(() => shell.api.resource.addResourceQuantity(file, resource, 'IfcQuantityCount')).toThrowError(
      expect.objectContaining({
        kind: IfcOpenShellErrorKind.VALUE,
        code: IfcOpenShellErrorCode.UNSUPPORTED_RESOURCE_QUANTITY,
      }),
    );
    expect(() => shell.api.resource.addResourceQuantity(file, resource, 'IfcWall')).toThrowError(
      expect.objectContaining({
        kind: IfcOpenShellErrorKind.VALUE,
        code: IfcOpenShellErrorCode.UNSUPPORTED_RESOURCE_QUANTITY,
        message:
          "Resource type 'IfcCrewResource' does not support quantity type 'IfcWall'. Supported quantities: IfcQuantityTime",
      }),
    );
  });

  it('distinguishes nullable success from handle and void failures', () => {
    const file = shell.raw.parse.newFile('IFC4', 0, '');
    expect(file).not.toBeNull();
    if (!file) return;

    const first = shell.raw.sequence.addTask(file, {});
    const second = shell.raw.sequence.addTask(file, {});
    const firstTime = shell.raw.sequence.addTaskTime(file, first, {});
    const secondTime = shell.raw.sequence.addTaskTime(file, second, {});
    const forward = shell.raw.sequence.assignSequence(file, first, second, {});
    try {
      expect(shell.raw.selector.format(null, null, '{{undefined}}')).toBeNull();
      expect(() => shell.raw.sequence.assignSequence(file, second, first, {})).toThrowError(
        expect.objectContaining({
          kind: IfcOpenShellErrorKind.RECURSION,
          code: IfcOpenShellErrorCode.RECURSIVE_SCHEDULE_CASCADE,
        }),
      );
      expect(() => shell.raw.sequence.cascadeSchedule(file, first)).toThrowError(
        expect.objectContaining({
          kind: IfcOpenShellErrorKind.RECURSION,
          code: IfcOpenShellErrorCode.RECURSIVE_SCHEDULE_CASCADE,
        }),
      );
    } finally {
      forward.destroy();
      secondTime.destroy();
      firstTime.destroy();
      second.destroy();
      first.destroy();
      file.destroy();
    }
  });
});
