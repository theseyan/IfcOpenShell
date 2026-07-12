
import { afterAll, beforeAll, describe, expect, it } from 'vitest';
import { resolveWasmAssets } from '@ifcopenshell-js/wasm';
import { init, type IfcOpenShell } from '../src/index.js';
import { describeGeneratedOrSkip, GENERATED_WASM_DIR } from './_helper.js';

describeGeneratedOrSkip('generated sequence API', () => {
  let shell: IfcOpenShell;

  beforeAll(async () => {
    shell = await init({
      wasmAssets: await resolveWasmAssets(GENERATED_WASM_DIR!),
    });
    await shell.loadPlugin('schema', 'ifc4');
  });

  afterAll(() => undefined);

  it('accepts option objects for core scheduling operations', () => {
    const file = shell.raw.parse.newFile('IFC4', 0, '');
    expect(file).not.toBeNull();
    if (!file) return;

    try {
      const project = shell.raw.root.createEntity(file, {
        ifc_class: 'IfcProject',
        name: 'Project',
      });
      expect(project.isA('IfcProject')).toBe(true);

      const schedule = shell.raw.sequence.addWorkSchedule(file, {
        name: 'Baseline plan',
        predefined_type: 'PLANNED',
      });
      expect(schedule.isA('IfcWorkSchedule')).toBe(true);

      const firstTask = shell.raw.sequence.addTask(file, {
        work_schedule: schedule,
        name: 'Frame',
        identification: 'A100',
      });
      const secondTask = shell.raw.sequence.addTask(file, {
        work_schedule: schedule,
        name: 'Close',
        identification: 'A200',
      });
      expect(firstTask.isA('IfcTask')).toBe(true);
      expect(secondTask.isA('IfcTask')).toBe(true);

      const taskTime = shell.raw.sequence.addTaskTime(file, firstTask, { is_recurring: true });
      expect(taskTime.isA('IfcTaskTimeRecurring')).toBe(true);

      const sequence = shell.raw.sequence.assignSequence(file, firstTask, secondTask, {
        sequence_type: 'FINISH_START',
      });
      expect(sequence.isA('IfcRelSequence')).toBe(true);

      const duplicate = shell.raw.sequence.duplicateTask(file, firstTask, {});
      try {
        expect(duplicate.current.size()).toBeGreaterThan(0);
        expect(duplicate.duplicate.size()).toBeGreaterThan(0);
      } finally {
        duplicate.current.destroy();
        duplicate.duplicate.destroy();
      }

      shell.raw.sequence.createBaseline(file, schedule, { name: 'Baseline 1' });
      const schedules = file.byType('IfcWorkSchedule');
      try {
        expect(schedules.size()).toBeGreaterThanOrEqual(2);
      } finally {
        schedules.destroy();
      }

      shell.raw.sequence.removeTask(file, secondTask, {});
      expect(file.getTotalInverses(secondTask)).toBe(0);
    } finally {
      file.destroy();
    }
  });

  it('releases string date-time variant results', () => {
    const file = shell.raw.parse.newFile('IFC4', 0, '');
    expect(file).not.toBeNull();
    if (!file) return;

    try {
      for (let i = 0; i < 10; i++) {
        expect(shell.raw.sequence.addDateTime(file, `2026-07-10T00:00:${String(i).padStart(2, '0')}`)).toBeTypeOf('string');
      }
    } finally {
      file.destroy();
    }
  });
});
