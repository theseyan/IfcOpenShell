import { afterAll, beforeAll, describe, expect, it } from 'vitest';
import { IfcFile, type Entity, type IfcOpenShell } from '../src/index.js';
import { createInstance, describeGeneratedOrSkip, GENERATED_WASM_DIR } from './_helper.js';

describeGeneratedOrSkip('generated style and material API', () => {
  let shell: IfcOpenShell;

  beforeAll(async () => {
    shell = await createInstance(GENERATED_WASM_DIR!);
    await shell.loadPlugin('schema', 'ifc4');
  });

  afterAll(async () => {
    await shell?.dispose();
  });

  it('creates and replaces surface components', async () => {
    await using file = await IfcFile.createEmpty(shell, 'IFC4');
    const parent = file.create('IfcSurfaceStyle');
    const shading = shell.api.style.addSurfaceStyle(file, parent, 'IfcSurfaceStyleShading', {});
    const rendering = shell.api.style.addSurfaceStyle(file, parent, 'IfcSurfaceStyleRendering', {});
    expect(shading.type).toBe('IfcSurfaceStyleShading');
    expect(rendering.type).toBe('IfcSurfaceStyleRendering');
    expect(file.all('IfcSurfaceStyleShading', { includeSubtypes: false })).toHaveLength(0);
    expect(file.all('IfcSurfaceStyleRendering')).toHaveLength(1);
    expect((await parent.get('Styles') as Entity[]).map((style) => style.raw.identity()))
      .toEqual([rendering.raw.identity()]);
  });

  it('lowers descriptor objects and returns mapped entity arrays', async () => {
    await using file = await IfcFile.createEmpty(shell, 'IFC4');
    const old = file.create('IfcImageTexture');
    old.set('RepeatS', true);
    old.set('RepeatT', true);
    old.set('URLReference', 'old.png');
    const uvMap = file.create('IfcTextureCoordinateGenerator');
    uvMap.set('Maps', [old]);
    uvMap.set('Mode', 'COORD');
    const transform = file.create('IfcCartesianTransformationOperator2D');
    transform.set('Scale', 2);

    const textures = shell.api.style.addSurfaceTextures(file, [
      { repeatS: true, repeatT: false, urlReference: 'generated.png', uvMode: 'Generated' },
      { repeatS: false, repeatT: true, mode: 'NORMAL', urlReference: 'camera.png', uvMode: 'Camera' },
      {
        repeatS: true,
        repeatT: true,
        urlReference: 'uv.png',
        textureTransform: transform,
        parameter: ['A', 'B'],
        uvMode: 'UV',
      },
    ], [uvMap]);

    expect(textures.map((texture) => texture.type)).toEqual([
      'IfcImageTexture', 'IfcImageTexture', 'IfcImageTexture',
    ]);
    expect(await textures[0].get('URLReference')).toBe('generated.png');
    expect(await textures[1].get('Mode')).toBe('NORMAL');
    expect(await textures[2].get('Parameter')).toEqual(['A', 'B']);
    expect((await uvMap.get('Maps') as Entity[]).map((texture) => texture.raw.identity()))
      .toEqual([old.raw.identity(), textures[2].raw.identity()]);
    expect(file.all('IfcTextureCoordinateGenerator')).toHaveLength(3);
  });

  it('copies material graphs while reusing referenced materials', async () => {
    await using file = await IfcFile.createEmpty(shell, 'IFC4');
    const material = file.create('IfcMaterial');
    material.set('Name', 'Core');
    const first = file.create('IfcMaterialConstituent');
    first.set('Name', 'First');
    first.set('Material', material);
    const second = file.create('IfcMaterialConstituent');
    second.set('Name', 'Second');
    second.set('Material', material);
    const materialSet = file.create('IfcMaterialConstituentSet');
    materialSet.set('MaterialConstituents', [first, second]);

    const copied = shell.api.material.copyMaterial(file, materialSet);
    const copiedItems = await copied.get('MaterialConstituents') as Entity[];
    expect(copiedItems.map((item) => item.type)).toEqual([
      'IfcMaterialConstituent', 'IfcMaterialConstituent',
    ]);
    expect(copiedItems.map((item) => item.raw.identity()))
      .not.toEqual([first.raw.identity(), second.raw.identity()]);
    expect((await copiedItems[0].get('Material') as Entity).raw.identity())
      .toBe(material.raw.identity());
    expect(file.all('IfcMaterial')).toHaveLength(1);
  });

  it('assigns, reuses, replaces, and styles named constituents', async () => {
    await using file = await IfcFile.createEmpty(shell, 'IFC4');
    const context = file.create('IfcGeometricRepresentationContext');
    const item = file.create('IfcCartesianPoint');
    item.set('Coordinates', [0, 0, 0]);
    const representation = file.create('IfcShapeRepresentation');
    representation.set('ContextOfItems', context);
    representation.set('Items', [item]);
    const definition = file.create('IfcProductDefinitionShape');
    definition.set('Representations', [representation]);
    const wall = file.create('IfcWall');
    wall.set('Representation', definition);
    const aspectRepresentation = file.create('IfcShapeRepresentation');
    aspectRepresentation.set('ContextOfItems', context);
    aspectRepresentation.set('Items', [item]);
    const aspect = file.create('IfcShapeAspect');
    aspect.set('Name', 'Part');
    aspect.set('ShapeRepresentations', [aspectRepresentation]);
    aspect.set('PartOfProductDefinitionShape', definition);

    const first = file.create('IfcMaterial');
    const second = file.create('IfcMaterial');
    const style = file.create('IfcSurfaceStyle');
    style.set('Side', 'BOTH');
    const materialItem = file.create('IfcStyledItem');
    materialItem.set('Styles', [style]);
    const styledRepresentation = file.create('IfcStyledRepresentation');
    styledRepresentation.set('ContextOfItems', context);
    styledRepresentation.set('Items', [materialItem]);
    const materialDefinition = file.create('IfcMaterialDefinitionRepresentation');
    materialDefinition.set('Representations', [styledRepresentation]);
    materialDefinition.set('RepresentedMaterial', first);

    shell.api.material.setShapeAspectConstituents(
      file, wall, context, [{ name: 'Part', material: first }], {},
    );
    const originalSet = file.all('IfcMaterialConstituentSet')[0];
    shell.api.material.setShapeAspectConstituents(
      file, wall, context, [{ name: 'Part', material: first }], {},
    );
    expect(file.all('IfcMaterialConstituentSet')[0].raw.identity())
      .toBe(originalSet.raw.identity());
    const styledItems = file.all('IfcStyledItem');
    const assignedItems = await Promise.all(styledItems.map(async (styledItem) => ({
      styledItem,
      item: await styledItem.get('Item') as Entity | null,
    })));
    expect(assignedItems.some((entry) => entry.item?.raw.identity() === item.raw.identity()))
      .toBe(true);

    shell.api.material.setShapeAspectConstituents(
      file, wall, context, [{ name: 'Part', material: second }], {},
    );
    expect(file.all('IfcMaterialConstituentSet')).toHaveLength(1);
    expect(file.all('IfcMaterialConstituentSet')[0].raw.identity())
      .not.toBe(originalSet.raw.identity());
  });
});
