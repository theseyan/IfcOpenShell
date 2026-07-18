import { afterAll, beforeAll, describe, expect, it } from 'vitest';
import { IfcFile, type Entity, type IfcOpenShell } from '../src/index.js';
import { createInstance, describeGeneratedOrSkip, GENERATED_WASM_DIR } from './_helper.js';

describeGeneratedOrSkip('generated native alignment API', () => {
  let shell: IfcOpenShell;

  beforeAll(async () => {
    shell = await createInstance(GENERATED_WASM_DIR!);
    await shell.loadPlugin('schema', 'ifc4x3_add2');
    await shell.loadPlugin('mapping', 'ifc4x3_add2');
  });

  afterAll(async () => {
    await shell?.dispose();
  });

  async function newFile(): Promise<IfcFile> {
    const file = await IfcFile.createEmpty(shell, 'IFC4X3_ADD2');
    shell.api.root.createEntity(file, { ifcClass: 'IfcProject', name: 'Project' });
    shell.api.unit.assignUnit(file, {});
    const model = shell.api.context.addContext(file, {
      contextType: 'Model', contextIdentifier: '', targetView: '',
    });
    shell.api.context.addContext(file, {
      contextType: 'Model', contextIdentifier: 'Axis', targetView: 'MODEL_VIEW', parent: model,
    });
    return file;
  }

  it('creates, queries, maps, and maintains semantic alignment state', async () => {
    await using file = await newFile();
    const alignment = shell.api.alignment.create(file, {
      name: 'WASM alignment', includeVertical: true, includeCant: true,
      includeGeometry: true, startStation: 100,
    });
    expect(alignment.type).toBe('IfcAlignment');
    expect(shell.api.alignment.getAlignmentStartStation(file, alignment)).toBe(100);
    expect(shell.api.alignment.getAlignmentLayouts(alignment).map((entity) => entity.type)).toEqual([
      'IfcAlignmentHorizontal', 'IfcAlignmentVertical', 'IfcAlignmentCant',
    ]);
    const horizontal = shell.api.alignment.getHorizontalLayout(alignment)!;
    expect(shell.api.alignment.hasZeroLengthSegment(horizontal)).toBe(true);
    expect(shell.api.alignment.addZeroLengthSegment(file, horizontal)).toBe(false);
    expect(shell.api.alignment.getParentAlignment(alignment)).toBeNull();
    expect(shell.api.alignment.getCurve(alignment)?.type).toBe('IfcSegmentedReferenceCurve');
  });

  it('authors PI and CSV text workflows with exact result types', async () => {
    await using file = await newFile();
    const alignment = shell.api.alignment.createByPiMethod(file, {
      name: 'PI',
      horizontal: {
        startPoint: [0, 0],
        intersections: [{ point: [100, 0], radius: 25 }],
        endPoint: [200, 100],
      },
      vertical: {
        startPoint: [0, 10],
        intersections: [{ point: [100, 12], curveLength: 30 }],
        endPoint: [200, 10],
      },
    });
    expect(shell.api.alignment.getAlignmentStartStation(file, alignment)).toBe(0);
    const horizontal = shell.api.alignment.getHorizontalLayout(alignment)!;
    const semantic = shell.api.alignment.getLayoutSegments(horizontal);
    expect(semantic.length).toBeGreaterThan(3);
    const mapped: Entity[] = shell.api.alignment.mapSegment(file, { segment: semantic[0] });
    expect(mapped).toHaveLength(1);
    expect(mapped[0].type).toBe('IfcCurveSegment');
    expect(shell.api.alignment.getMappedSegments(semantic[0])).toHaveLength(1);

    const csv = shell.api.alignment.createFromCsvText(file, {
      csvText: '0,0,0,100,0,25,200,100,0\n0,10,0,100,12,30,200,10,0\n',
    });
    expect(shell.api.alignment.getVerticalLayout(csv)?.type).toBe('IfcAlignmentVertical');
    expect(shell.api.alignment.getBasisCurve(csv)?.type).toBe('IfcCompositeCurve');
  });

  it('supports a straight horizontal-only PI contract and rejects malformed input before mutation', async () => {
    await using file = await newFile();
    const straight = shell.api.alignment.createByPiMethod(file, {
      name: 'Straight',
      horizontal: {
        startPoint: [0, 0],
        intersections: [],
        endPoint: [100, 0],
      },
    });
    expect(shell.api.alignment.getVerticalLayout(straight)).toBeNull();
    const horizontal = shell.api.alignment.getHorizontalLayout(straight)!;
    expect(shell.api.alignment.getLayoutSegments(horizontal)).toHaveLength(2);

    const before = file.all('IfcAlignment').length;
    expect(() => shell.api.alignment.createByPiMethod(file, {
      name: 'Invalid',
      horizontal: {
        startPoint: [0, 0],
        intersections: [],
        endPoint: [Number.NaN, 0],
      },
    })).toThrow(/horizontal end_point must contain finite coordinates/);
    expect(file.all('IfcAlignment')).toHaveLength(before);
  });
});
