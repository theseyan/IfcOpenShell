
import { afterAll, beforeAll, describe, expect, it } from 'vitest';
import { createInstance, describeOrSkip, readLargeOffsetIfcBytes, readSampleIfcBytes } from './_helper.js';
import { Entity, IfcFile, IfcOpenShellError, type IfcOpenShell } from '../src/index.js';

describeOrSkip('Entity', () => {
  let shell: IfcOpenShell;
  let file: IfcFile;

  beforeAll(async () => {
    shell = await createInstance();
    await shell.loadPlugin('schema', 'ifc4');
    file = await IfcFile.open(shell, readSampleIfcBytes(), 'WallInstance_IFC4Add2.ifc');
  });

  afterAll(async () => {
    await file.dispose();
    await shell.dispose();
  });

  it('exposes id, type and schema class checks', async () => {
    await using wall = await file.get(315);
    expect(wall?.id).toBe(315);
    expect(wall?.type).toBe('IfcWall');
    expect(await wall?.className()).toBe('IfcWall');
    expect(await wall?.className(true)).toContain('IfcWall');
    expect(await wall?.isA('IfcRoot')).toBe(true);
    expect(await wall?.isA('IfcSlab')).toBe(false);
  });

  it('reads attributes by index or name', async () => {
    await using wall = await file.get(315);
    await using guid = await wall!.attribute('GlobalId');
    expect(await guid.string()).toBe('37cU2nGL100xirW00wMfmm');

    const names = await wall!.attributes();
    expect(names).toContain('GlobalId');
    expect(names).toContain('Name');
    expect(await wall!.attributeIndex('GlobalId')).toBe(0);
    expect(await wall!.attributeName(0)).toBe('GlobalId');
    expect(await wall!.attributeType(0)).toBeTruthy();
    expect(await wall!.attributeCategory('GlobalId')).toBeGreaterThanOrEqual(0);

    await using first = await wall!.attribute(0);
    expect(first.type.length).toBeGreaterThan(0);
    expect(await wall!.get('GlobalId')).toBe('37cU2nGL100xirW00wMfmm');

    const info = wall!.info();
    expect(info).toMatchObject({
      id: 315,
      type: 'IfcWall',
      attributes: {
        GlobalId: '37cU2nGL100xirW00wMfmm',
      },
    });
    expect(wall!.entries().some(([name]) => name === 'Name')).toBe(true);
  });

  it('reads inverse attributes and mutates simple attribute values', async () => {
    await using editable = await IfcFile.createEmpty(shell, 'IFC4');
    await using wall = await editable.create('IfcWall', { name: 'Before' });
    await using nameBefore = await wall.attribute('Name');
    expect(await nameBefore.string()).toBe('Before');

    await wall.set('Name', 'After');
    expect(await wall.get('Name')).toBe('After');
    await using nameAfter = await wall.attribute('Name');
    expect(await nameAfter.string()).toBe('After');

    await wall.unset('Name');
    await using nameUnset = await wall.attribute('Name');
    expect(nameUnset.isNull).toBe(true);

    expect(await wall.inverseAttributes()).toContain('HasAssignments');
    expect(await wall.inverse('HasAssignments')).toEqual([]);
  });

  it('writes empty aggregates according to native schema types', async () => {
    await using editable = await IfcFile.createEmpty(shell, 'IFC4');
    using point = editable.create('IfcCartesianPoint');
    using person = editable.create('IfcPerson');
    using polyline = editable.create('IfcPolyline');
    using face = editable.create('IfcIndexedPolygonalFace');
    using faceWithVoids = editable.create('IfcIndexedPolygonalFaceWithVoids');

    point.set('Coordinates', []);
    person.set('MiddleNames', []);
    polyline.set('Points', []);
    face.set('CoordIndex', []);
    faceWithVoids.set('InnerCoordIndices', []);

    expect(point.get('Coordinates')).toEqual([]);
    expect(person.get('MiddleNames')).toEqual([]);
    expect(polyline.get('Points')).toEqual([]);
    expect(face.get('CoordIndex')).toEqual([]);
    expect(faceWithVoids.get('InnerCoordIndices')).toEqual([]);
  });

  it('round-trips nested coordinate and indexed-face aggregates', async () => {
    await using editable = await IfcFile.createEmpty(shell, 'IFC4');
    using points2d = editable.create('IfcCartesianPointList2D');
    using points3d = editable.create('IfcCartesianPointList3D');
    using face = editable.create('IfcIndexedPolygonalFaceWithVoids');

    points2d.set('CoordList', [[1.25, 2.5], [3.75, 4.125]]);
    points3d.set('CoordList', [[1, 2, 3], [4, 5, 6]]);
    face.set('CoordIndex', [1, 2, 3]);
    face.set('InnerCoordIndices', [[4, 5, 6], [7, 8, 9]]);

    expect(points2d.get('CoordList')).toEqual([[1.25, 2.5], [3.75, 4.125]]);
    expect(points3d.get('CoordList')).toEqual([[1, 2, 3], [4, 5, 6]]);
    expect(face.get('CoordIndex')).toEqual([1, 2, 3]);
    expect(face.get('InnerCoordIndices')).toEqual([[4, 5, 6], [7, 8, 9]]);
  });

  it('round-trips IFC LOGICAL TRUE, FALSE, and UNKNOWN', async () => {
    await using editable = await IfcFile.createEmpty(shell, 'IFC4');
    using logical = createDefinedType(editable, 'IfcLogical');

    logical.set('wrappedValue', true);
    expect(logical.get('wrappedValue')).toBe(true);
    logical.set('wrappedValue', false);
    expect(logical.get('wrappedValue')).toBe(false);
    logical.set('wrappedValue', 'UNKNOWN');
    expect(logical.get('wrappedValue')).toBe('UNKNOWN');
  });

  it('rejects mixed arrays and scalar/type mismatches', async () => {
    await using editable = await IfcFile.createEmpty(shell, 'IFC4');
    using point = editable.create('IfcCartesianPoint');
    using face = editable.create('IfcIndexedPolygonalFace');

    expect(() => point.set('Coordinates', [1, 'bad'] as unknown as number[])).toThrow(
      /IfcCartesianPoint\.Coordinates expects AGGREGATE OF DOUBLE.*incompatible array/,
    );
    expect(() => point.set('Coordinates', 4)).toThrow(/expects AGGREGATE OF DOUBLE.*incompatible number/);
    expect(() => face.set('CoordIndex', [1, 2.5, 3])).toThrow(/expects AGGREGATE OF INT/);
    expect(() => point.set('Coordinates', [], { type: 'AGGREGATE OF INT' })).toThrow(/native type/);
  });

  it('rejects scalar, list, and nested entity references from another file even when IDs collide', async () => {
    await using targetFile = await IfcFile.createEmpty(shell, 'IFC4');
    await using foreignFile = await IfcFile.createEmpty(shell, 'IFC4');
    using localPoint1 = targetFile.create('IfcCartesianPoint');
    using localPoint2 = targetFile.create('IfcCartesianPoint');
    using foreignPoint1 = foreignFile.create('IfcCartesianPoint');
    using foreignPoint2 = foreignFile.create('IfcCartesianPoint');
    using placement = targetFile.create('IfcAxis2Placement3D');
    using polyline = targetFile.create('IfcPolyline');
    using surface = targetFile.create('IfcBSplineSurface');

    expect([foreignPoint1.id, foreignPoint2.id]).toEqual([localPoint1.id, localPoint2.id]);
    expect(() => placement.set('Location', foreignPoint1)).toThrow(/different IFC file/);
    expect(() => polyline.set('Points', [foreignPoint1, foreignPoint2])).toThrow(/different IFC file/);
    expect(() => surface.set('ControlPointsList', [[foreignPoint1, foreignPoint2]])).toThrow(/different IFC file/);
    expect(placement.get('Location')).toBeNull();
    expect(polyline.get('Points')).toBeNull();
    expect(surface.get('ControlPointsList')).toBeNull();

    placement.set('Location', localPoint1);
    polyline.set('Points', [localPoint1, localPoint2]);
    surface.set('ControlPointsList', [[localPoint1, localPoint2]]);
    expect(placement.text()).toContain(`#${localPoint1.id}`);
    expect(polyline.text()).toContain(`(#${localPoint1.id},#${localPoint2.id})`);
    expect(surface.text()).toContain(`((#${localPoint1.id},#${localPoint2.id}))`);
  });

  it('reports missing attributes descriptively while retaining defined-type access', async () => {
    using wall = file.get(315)!;
    expect(() => wall.get('Missing')).toThrow(/Attribute 'Missing' not found on entity named IfcWall/);
  });

  it('keeps Large Offset attribute names stable', async () => {
    await using largeOffset = await IfcFile.open(shell, readLargeOffsetIfcBytes(), 'large_offset.ifc');
    await using opening = await largeOffset.get(2156);
    expect(await opening!.attributes()).toEqual([
      'GlobalId',
      'OwnerHistory',
      'Name',
      'Description',
      'ObjectType',
      'ObjectPlacement',
      'Representation',
      'Tag',
      'PredefinedType',
    ]);
  });

  it('dispose is idempotent and guards released handles', async () => {
    const wall = await file.get(315);
    wall!.dispose();
    wall!.dispose();
    expect(() => wall!.raw).toThrow(IfcOpenShellError);
  });
});

function createDefinedType(file: IfcFile, name: string): Entity {
  const schema = file.shell.raw.parse.schemaByName(file.schemaName);
  if (!schema || schema.ptr === 0) throw new Error(`Schema ${file.schemaName} is unavailable`);
  try {
    const declaration = schema.declarationByName(name);
    if (!declaration || declaration.ptr === 0) throw new Error(`Declaration ${name} is unavailable`);
    try {
      const entity = Entity.wrap(file.shell, file.raw.create(declaration, -1));
      if (!entity) throw new Error(`Failed to create ${name}`);
      return entity;
    } finally {
      declaration.destroy();
    }
  } finally {
    schema.destroy();
  }
}
