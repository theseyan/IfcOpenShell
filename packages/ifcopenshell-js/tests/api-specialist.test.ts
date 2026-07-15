
import { afterAll, beforeAll, describe, expect, it } from 'vitest';
import { IfcFile, type IfcOpenShell } from '../src/index.js';
import { createInstance, describeGeneratedOrSkip, GENERATED_WASM_DIR } from './_helper.js';

describeGeneratedOrSkip('generated specialist API', () => {
  let shell: IfcOpenShell;

  beforeAll(async () => {
    shell = await createInstance(GENERATED_WASM_DIR!);
    await shell.loadPlugin('schema', 'ifc4');
  });

  afterAll(async () => {
    await shell?.dispose();
  });

  async function createOwnerHistoryWithPrerequisites(file: IfcFile) {
    const person = await shell.api.owner.addPerson(file, 'P001', 'Doe', 'John');
    const org = await shell.api.owner.addOrganisation(file, 'ORG01', 'Acme');
    const pando = await shell.api.owner.addPersonAndOrganisation(file, person, org);
    const app = await shell.api.owner.addApplication(file, {
      applicationDeveloper: org,
      version: '1.0',
      applicationFullName: 'IfcOpenShell Test',
      applicationIdentifier: 'com.ifcopenshell.test',
    });
    const history = await shell.api.owner.createOwnerHistory(file, {
      user: pando,
      application: app,
    });
    return { person, org, pando, app, history };
  }

  describe('cogo', () => {
    it('adds survey points with correct context hierarchy', async () => {
      await using file = await IfcFile.createEmpty(shell, 'IFC4');
      await shell.api.root.createEntity(file, {
        ifcClass: 'IfcProject',
        name: 'Project',
      });
      const site = await shell.api.root.createEntity(file, {
        ifcClass: 'IfcSite',
        name: 'Site',
      });

      const parentCtx = await shell.api.context.addContext(file, {
        contextType: 'Model',
        contextIdentifier: '',
        targetView: '',
      });
      await shell.api.context.addContext(file, {
        contextType: 'Model',
        contextIdentifier: 'Annotation',
        targetView: 'MODEL_VIEW',
        parent: parentCtx,
      });

      const sp = await shell.api.root.createEntity(file, {
        ifcClass: 'IfcCartesianPoint',
        name: 'Survey Point',
      });

      const point = await shell.api.cogo.addSurveyPoint(file, {
        surveyPoint: sp,
        site,
      });
      expect(point).not.toBeNull();
      if (!point) throw new Error('addSurveyPoint returned null');
      expect(point.type).toBeDefined();
      shell.api.cogo.editSurveyPoint(sp, 10, 20, 30);
    });
  });

  describe('cost', () => {
    it('manages cost schedule lifecycle with valid owner history', async () => {
      await using file = await IfcFile.createEmpty(shell, 'IFC4');
      await shell.api.root.createEntity(file, {
        ifcClass: 'IfcProject',
        name: 'Project',
      });
      const { history } = await createOwnerHistoryWithPrerequisites(file);
      expect(history).not.toBeNull();
      if (!history) throw new Error('createOwnerHistory returned null');

      const schedule = await shell.api.cost.addCostSchedule(
        file, 'Budget', 'BUDGET', '2026-01-01', history,
      );
      expect(schedule.type).toBe('IfcCostSchedule');

      const item = await shell.api.cost.addCostItem(file, {
        costSchedule: schedule,
        name: 'Foundation',
      });
      expect(item.type).toBe('IfcCostItem');

      const value = await shell.api.cost.addCostValue(file, item);
      expect(value.type).toBe('IfcCostValue');

      shell.api.cost.editCostValueFormula(file, value, '1000');

      shell.api.cost.removeCostValue(file, item, value);
      shell.api.cost.removeCostItem(file, item);
      shell.api.cost.removeCostSchedule(file, schedule);
    });

    it('edits cost item properties', async () => {
      await using file = await IfcFile.createEmpty(shell, 'IFC4');
      const { history } = await createOwnerHistoryWithPrerequisites(file);
      const schedule = await shell.api.cost.addCostSchedule(
        file, 'Budget', 'BUDGET', '2026-01-01', history,
      );
      const item = await shell.api.cost.addCostItem(file, {
        costSchedule: schedule,
        name: 'Original',
      });

      shell.api.cost.editCostItem(file, item, { Name: 'Updated' });

      expect(await item.get('Name')).toBe('Updated');
    });
  });

  describe('resource', () => {
    it('creates resource entities', async () => {
      await using file = await IfcFile.createEmpty(shell, 'IFC4');
      const resource = await shell.api.root.createEntity(file, {
        ifcClass: 'IfcConstructionEquipmentResource',
        name: 'Crane',
      });
      expect(resource.type).toBe('IfcConstructionEquipmentResource');
    });

    it('edits resource time properties', async () => {
      await using file = await IfcFile.createEmpty(shell, 'IFC4');
      const resource = await shell.api.root.createEntity(file, {
        ifcClass: 'IfcLaborResource',
      });
      const resourceTime = await shell.api.root.createEntity(file, {
        ifcClass: 'IfcResourceTime',
        name: 'Resource Time',
      });
      resource.set('Usage', resourceTime);

      shell.api.resource.editResourceTime(file, resourceTime, {
        ScheduleUsage: 0.5,
      });

      expect(await resourceTime.get('ScheduleUsage')).toBe(0.5);
    });
  });

  describe('sequence', () => {
    it('manages scheduling lifecycle via public API', async () => {
      await using file = await IfcFile.createEmpty(shell, 'IFC4');
      await shell.api.root.createEntity(file, {
        ifcClass: 'IfcProject',
        name: 'Project',
      });

      const schedule = await shell.api.sequence.addWorkSchedule(file, {
        name: 'Construction Plan',
        predefinedType: 'PLANNED',
      });
      expect(schedule).not.toBeNull();
      if (!schedule) throw new Error('addWorkSchedule returned null');

      expect(schedule.type).toBe('IfcWorkSchedule');

      const task1 = await shell.api.sequence.addTask(file, {
        workSchedule: schedule,
        name: 'Task A',
        identification: 'A100',
      });
      expect(task1.type).toBe('IfcTask');

      const task2 = await shell.api.sequence.addTask(file, {
        workSchedule: schedule,
        name: 'Task B',
        identification: 'A200',
      });
      expect(task2.type).toBe('IfcTask');

      const seq = await shell.api.sequence.assignSequence(
        file, task1, task2, { sequenceType: 'FINISH_START' },
      );
      expect(seq.type).toBe('IfcRelSequence');

      await shell.api.sequence.calculateTaskDuration(file, task1);

      const taskTime = await shell.api.sequence.addTaskTime(file, task1, {
        isRecurring: false,
      });
      expect(taskTime.type).toBe('IfcTaskTime');

      const duplicated = await shell.api.sequence.duplicateTask(file, task1, {});
      expect(duplicated.current.length).toBeGreaterThan(0);
      expect(duplicated.duplicate.length).toBeGreaterThan(0);

      await shell.api.sequence.createBaseline(file, schedule, {
        name: 'Baseline 1',
      });

      shell.api.sequence.unassignSequence(file, task1, task2);
      shell.api.sequence.removeTask(file, task2, {});
      shell.api.sequence.removeTask(file, task1, {});
      shell.api.sequence.removeWorkSchedule(file, schedule, {});
    });

    it('edits sequence properties', async () => {
      await using file = await IfcFile.createEmpty(shell, 'IFC4');
      const firstTask = await shell.api.root.createEntity(file, { ifcClass: 'IfcTask' });
      const secondTask = await shell.api.root.createEntity(file, { ifcClass: 'IfcTask' });
      const sequence = await shell.api.sequence.assignSequence(
        file, firstTask, secondTask, { sequenceType: 'FINISH_START' },
      );

      shell.api.sequence.editSequence(file, sequence, { SequenceType: 'START_START' });

      expect(await sequence.get('SequenceType')).toBe('START_START');
    });
  });

  describe('structural', () => {
    it('creates structural model, loads, and groups with valid owner history', async () => {
      await using file = await IfcFile.createEmpty(shell, 'IFC4');
      await shell.api.root.createEntity(file, {
        ifcClass: 'IfcProject',
        name: 'Project',
      });
      const { history } = await createOwnerHistoryWithPrerequisites(file);
      expect(history).not.toBeNull();
      if (!history) throw new Error('createOwnerHistory returned null');

      const model = await shell.api.structural.addStructuralAnalysisModel(
        file, history,
      );
      expect(model.type).toBe('IfcStructuralAnalysisModel');

      const load = await shell.api.structural.addStructuralLoad(
        file, 'IfcStructuralLoadSingleForce', 'Point Load',
      );
      expect(load.type).toBe('IfcStructuralLoadSingleForce');

      const loadCase = await shell.api.structural.addStructuralLoadCase(
        file, 'Dead Load', 'NOTDEFINED', 'NOTDEFINED', history,
      );
      expect(loadCase.type).toBe('IfcStructuralLoadCase');

      const loadGroup = await shell.api.structural.addStructuralLoadGroup(
        file, 'Group 1', 'NOTDEFINED', 'NOTDEFINED', history,
      );
      expect(loadGroup.type).toBe('IfcStructuralLoadGroup');

      const wall = await shell.api.root.createEntity(file, {
        ifcClass: 'IfcWall',
        name: 'Wall',
      });

      await shell.api.structural.assignProduct(file, load, wall, history);

      shell.api.structural.removeStructuralLoad(file, load);
      shell.api.structural.removeStructuralLoadCase(file, loadCase);
      shell.api.structural.removeStructuralLoadGroup(file, loadGroup);
      shell.api.structural.removeStructuralAnalysisModel(file, model);
    });

    it('assignStructuralAnalysisModel positional Entity[]', async () => {
      await using file = await IfcFile.createEmpty(shell, 'IFC4');
      await shell.api.root.createEntity(file, {
        ifcClass: 'IfcProject',
        name: 'Project',
      });
      const { history } = await createOwnerHistoryWithPrerequisites(file);
      expect(history).not.toBeNull();
      if (!history) throw new Error('createOwnerHistory returned null');

      const model = await shell.api.structural.addStructuralAnalysisModel(
        file, history,
      );
      expect(model.type).toBe('IfcStructuralAnalysisModel');

      const wall = await shell.api.root.createEntity(file, {
        ifcClass: 'IfcWall',
        name: 'Wall',
      });

      shell.api.structural.assignStructuralAnalysisModel(
        file, [wall], model, {},
      );
      shell.api.structural.unassignStructuralAnalysisModel(
        file, [wall], model, {},
      );

      shell.api.structural.removeStructuralAnalysisModel(file, model);
    });
  });

  describe('compute', () => {
    it('evaluates derived Dim attribute on IfcCartesianPoint', async () => {
      await using file = await IfcFile.createEmpty(shell, 'IFC4');
      const point = await shell.api.root.createEntity(file, {
        ifcClass: 'IfcCartesianPoint',
        name: 'Point',
      });
      await point.set('Coordinates', [1.0, 2.0, 3.0]);

      const result = shell.api.compute.derived(point, 'Dim');
      expect(result).toBe(3);
    });
  });

  describe('register', () => {
    it('registers scratch files', async () => {
      await using file = await IfcFile.createEmpty(shell, 'IFC4');
      const result = shell.api.register.scratchFile('IFC4', file);
      expect(typeof result).toBe('boolean');
    });
  });
});
