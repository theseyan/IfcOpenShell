import { afterAll, beforeAll, describe, expect, it } from 'vitest';
import { IfcFile, type IfcOpenShell } from '../src/index.js';
import { createInstance, describeGeneratedOrSkip, GENERATED_WASM_DIR } from './_helper.js';

describeGeneratedOrSkip('generated native ShapeBuilder API', () => {
  let shell: IfcOpenShell;

  beforeAll(async () => {
    shell = await createInstance(GENERATED_WASM_DIR!);
    await shell.loadPlugin('schema', 'ifc4');
  });

  afterAll(async () => {
    await shell?.dispose();
  });

  async function newFile(): Promise<IfcFile> {
    return IfcFile.createEmpty(shell, 'IFC4');
  }

  it('round-trips explicit line and arc segments', async () => {
    await using file = await newFile();
    const points: [number, number][] = [[0, 0], [2, 0], [2.5, 0.5], [2, 1]];
    const segments = [
      { lineIndices: [0, 1] },
      { arcIndices: [1, 2, 3] as [number, number, number] },
    ];
    const polyline = shell.api.shape.builderPolyline(file, { points, segments });
    const indexed = shell.api.shape.builderIndexedPolycurve2d(file, { points, segments });

    expect(polyline.type).toBe('IfcIndexedPolyCurve');
    expect(indexed.type).toBe('IfcIndexedPolyCurve');
    expect(shell.api.shape.builderGetPolylineCoords(polyline)).toEqual(points);
    expect(shell.api.shape.builderGetPolylineCoords(indexed)).toEqual(points);
    expect((polyline.get('Segments') as { type: string }[]).map((segment) => segment.type)).toEqual([
      'IfcLineIndex', 'IfcArcIndex',
    ]);
    expect((indexed.get('Segments') as { type: string }[]).map((segment) => segment.type)).toEqual([
      'IfcLineIndex', 'IfcArcIndex',
    ]);
  });

  it('restores omitted geometry defaults through generated options', async () => {
    await using file = await newFile();
    expect(shell.api.shape.builderAxis2Placement2d(file, {}).type).toBe('IfcAxis2Placement2D');
    expect(shell.api.shape.builderAxis2Placement3d(file, {}).type).toBe('IfcAxis2Placement3D');
    const block = shell.api.shape.builderBlock(file, {});
    const sphere = shell.api.shape.builderSphere(file, {});
    expect([block.get('XLength'), block.get('YLength'), block.get('ZLength')]).toEqual([1, 1, 1]);
    expect(sphere.get('Radius')).toBe(1);

    const boundary = shell.api.shape.builderPolyline(file, {
      points: [[0, 0], [2, 0], [2, 1], [0, 1]],
      segments: [{ lineIndices: [0, 1, 2, 3, 0] }],
    });
    const profile = shell.api.shape.builderProfile(file, { outerCurve: boundary });
    expect(profile.get('ProfileType')).toBe('AREA');
    const extrusion = shell.api.shape.builderExtrude(file, { profileOrCurve: profile });
    expect(extrusion.get('Depth')).toBe(1);

    const plane = shell.api.shape.builderPlane(file, [0, 0, 0], [0, 0, 1]);
    expect(shell.api.shape.builderHalfSpaceSolid(file, { plane }).get('AgreementFlag')).toBe(false);
  });

  it('applies rotate, translate, and mirror defaults when omitted', async () => {
    await using file = await newFile();
    const translated = shell.api.shape.builderPolyline(file, { points: [[0, 0], [1, 0]] });
    const translatedResult = shell.api.shape.builderTranslate(file, {
      item: translated,
      translation: [2, 3],
    });
    expect(translatedResult.id).toBe(translated.id);
    expect(shell.api.shape.builderGetPolylineCoords(translated)).toEqual([[2, 3], [3, 3]]);

    const rotated = shell.api.shape.builderPolyline(file, { points: [[1, 0], [2, 0]] });
    expect(shell.api.shape.builderRotate(file, { item: rotated }).id).toBe(rotated.id);
    const rotatedPoints = shell.api.shape.builderGetPolylineCoords(rotated);
    expect(rotatedPoints[0]![0]).toBeCloseTo(0);
    expect(rotatedPoints[0]![1]).toBeCloseTo(-1);

    const mirrored = shell.api.shape.builderPolyline(file, { points: [[1, 2], [2, 3]] });
    expect(shell.api.shape.builderMirror(file, { item: mirrored }).id).toBe(mirrored.id);
    expect(shell.api.shape.builderGetPolylineCoords(mirrored)).toEqual([[-1, -2], [-2, -3]]);
  });

  it('rejects invalid alternatives before mutating the file', async () => {
    await using file = await newFile();
    expect(shell.api.shape.builderEllipseCurve(file, {
      xAxisRadius: 2,
      yAxisRadius: 1,
      trim: { value: { points: [[2, 0], [0, 1]] } },
    }).type).toBe('IfcTrimmedCurve');
    expect(shell.api.shape.builderEllipseCurve(file, {
      xAxisRadius: 2,
      yAxisRadius: 1,
      trim: { value: { cardinalPoints: [0, 1] } },
    }).type).toBe('IfcTrimmedCurve');
    const ellipseCount = file.all('IfcEllipse').length;
    expect(() => shell.api.shape.builderEllipseCurve(file, {
      xAxisRadius: 2,
      yAxisRadius: 1,
      trim: { value: { points: [[2, 0], [0, 1]], cardinalPoints: [0, 1] } },
    })).toThrow(/exactly one variant alternative/);
    expect(file.all('IfcEllipse')).toHaveLength(ellipseCount);

    const curveCount = file.all('IfcIndexedPolyCurve').length;
    expect(() => shell.api.shape.builderPolyline(file, {
      points: [[0, 0], [1, 0]],
      segments: [{ lineIndices: [0] }],
    })).toThrow(/line segment/);
    expect(file.all('IfcIndexedPolyCurve')).toHaveLength(curveCount);

    const dimensions = { halfX: 1, halfY: 0.5, depth: 0 };
    const common = { startHalfDim: dimensions, endHalfDim: dimensions, offset: { x: 0, y: 0 } };
    expect(shell.api.shape.builderMepTransitionCalculate({
      ...common,
      calculation: { angle: 30 },
    })).toBeTypeOf('number');
    expect(() => shell.api.shape.builderMepTransitionCalculate({
      ...common,
      calculation: { angle: 30, length: 2 },
    })).toThrow(/exactly one/);
  });

  it('enforces fixed vectors and consistent point dimensions', async () => {
    await using file = await newFile();
    const polyline3d = shell.api.shape.builderPolyline(file, {
      points: [[0, 0, 0], [1, 0, 0]],
    });
    expect(shell.api.shape.builderGetPolylineCoords(polyline3d)).toEqual([[0, 0, 0], [1, 0, 0]]);
    expect(shell.api.shape.builderPlane(file, [0, 0, 0], [0, 0, 1]).type).toBe('IfcPlane');

    const entityCount = file.entityCount;
    expect(() => shell.api.shape.builderPolyline(file, {
      points: [[0, 0], [1, 0, 0]] as never,
    })).toThrow(/variant alternative/);
    expect(() => shell.api.shape.builderPlane(
      file,
      [0, 0] as never,
      [0, 0, 1],
    )).toThrow(/(?:exactly|contain) 3 (?:array )?items/);
    expect(() => shell.api.shape.builderTranslate(file, {
      item: polyline3d,
      translation: [1, 2, 3, 4] as never,
    })).toThrow(/variant alternative/);
    expect(file.entityCount).toBe(entityCount);
  });
});
