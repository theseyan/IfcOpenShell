
import { afterAll, beforeAll, describe, expect, it } from 'vitest';
import { IfcFile, type Entity, type IfcOpenShell } from '../src/index.js';
import { createInstance, describeGeneratedOrSkip, GENERATED_WASM_DIR } from './_helper.js';

describeGeneratedOrSkip('generated geometry and presentation API', () => {
  let shell: IfcOpenShell;

  beforeAll(async () => {
    shell = await createInstance(GENERATED_WASM_DIR!);
    await shell.loadPlugin('schema', 'ifc4');
  });

  afterAll(async () => {
    await shell?.dispose();
  });

  async function withProject(file: IfcFile): Promise<void> {
    await shell.api.root.createEntity(file, {
      ifcClass: 'IfcProject',
      name: 'Project',
    });
  }

  async function withBodyContext(file: IfcFile): Promise<Entity> {
    await withProject(file);

    const model = await shell.api.context.addContext(file, {
      contextType: 'Model',
      contextIdentifier: '',
      targetView: '',
    });

    const body = await shell.api.context.addContext(file, {
      contextType: 'Model',
      contextIdentifier: 'Body',
      targetView: 'MODEL_VIEW',
      parent: model,
    });

    expect(await body.get('ContextIdentifier')).toBe('Body');
    expect(await body.get('TargetView')).toBe('MODEL_VIEW');
    return body;
  }

  describe('boundary', () => {
    it('copies and removes space boundaries', async () => {
      await using file = await IfcFile.createEmpty(shell, 'IFC4');
      const space = await shell.api.root.createEntity(file, {
        ifcClass: 'IfcSpace',
        name: 'Space',
      });
      const wall = await shell.api.root.createEntity(file, {
        ifcClass: 'IfcWall',
        name: 'Wall',
      });
      const boundary = await shell.api.root.createEntity(file, {
        ifcClass: 'IfcRelSpaceBoundary',
        name: 'Boundary',
      });

      shell.api.boundary.editAttributes(boundary, {
        relatingSpace: space,
        relatedBuildingElement: wall,
        physicalOrVirtual: 'PHYSICAL',
        internalOrExternal: 'INTERNAL',
      });

      const copy = await shell.api.boundary.copyBoundary(file, boundary);
      expect(copy.type).toBe('IfcRelSpaceBoundary');

      shell.api.boundary.removeBoundary(file, copy);
      shell.api.boundary.removeBoundary(file, boundary);
    });
  });

  describe('context', () => {
    it('adds and removes geometric contexts', async () => {
      await using file = await IfcFile.createEmpty(shell, 'IFC4');
      await withProject(file);
      const ctx = await shell.api.context.addContext(file, {
        contextType: 'Model',
        contextIdentifier: '',
        targetView: '',
      });
      expect(ctx.type).toMatch(/^IfcGeometricRepresentation/);

      const found = await shell.api.representation.getContext(
        file, 'Model', '', '',
      );
      expect(found !== null).toBe(true);

      shell.api.context.removeContext(file, ctx);
    });
  });

  describe('drawing', () => {
    it('assigns and unassigns drawing products', async () => {
      await using file = await IfcFile.createEmpty(shell, 'IFC4');
      const annotation = await shell.api.root.createEntity(file, {
        ifcClass: 'IfcAnnotation',
        name: 'Annotation',
      });
      const wall = await shell.api.root.createEntity(file, {
        ifcClass: 'IfcWall',
        name: 'Wall',
      });

      const rel = await shell.api.drawing.assignProduct(file, {
        relatingProduct: annotation,
        relatedObject: wall,
      });
      expect(rel.type).toBeDefined();

      shell.api.drawing.unassignProduct(file, {
        relatingProduct: annotation,
        relatedObject: wall,
      });
    });
  });

  describe('feature', () => {
    it('manages openings and fillings', async () => {
      await using file = await IfcFile.createEmpty(shell, 'IFC4');
      const wall = await shell.api.root.createEntity(file, {
        ifcClass: 'IfcWall',
        name: 'Wall',
      });
      const opening = await shell.api.root.createEntity(file, {
        ifcClass: 'IfcOpeningElement',
        name: 'Opening',
      });

      await shell.api.feature.addFeature(file, {
        feature: opening,
        element: wall,
      });

      const openings = await shell.api.element.getOpenings(wall);
      expect(openings.length).toBeGreaterThanOrEqual(1);

      const door = await shell.api.root.createEntity(file, {
        ifcClass: 'IfcDoor',
        name: 'Door',
      });
      const filling = await shell.api.feature.addFilling(file, opening, door);
      expect(filling.type).toBeDefined();

      const voids = await shell.api.element.getFilledVoid(door);
      expect(voids).not.toBeNull();

      shell.api.feature.removeFilling(file, door);
      shell.api.feature.removeFeature(file, { feature: opening });
    });
  });

  describe('geometry', () => {
    it('adds profile representation through the generated API', async () => {
      await using file = await IfcFile.createEmpty(shell, 'IFC4');
      const context = await withBodyContext(file);
      const profile = await shell.api.profile.addParameterizedProfile(
        file, 'IfcRectangleProfileDef', 'AREA',
      );
      shell.api.profile.editProfile(profile, { XDim: 2.0, YDim: 1.0 });

      const rep = await shell.api.geometry.addProfileRepresentation(file, {
        context,
        profile,
        depth: 1.0,
        cardinalPoint: 'bottom left',
        clippingKinds: [],
        clippingLocations: [],
        clippingNormals: [],
        clippingEntities: [],
      });

      expect(rep.type).toBe('IfcShapeRepresentation');
      expect(await rep.get('RepresentationIdentifier')).toBe('Body');
      expect(await rep.get('RepresentationType')).toBe('SweptSolid');
    });

    it('adds mesh representation and assigns to product', async () => {
      await using file = await IfcFile.createEmpty(shell, 'IFC4');
      const context = await withBodyContext(file);
      const wall = await shell.api.root.createEntity(file, {
        ifcClass: 'IfcWall',
        name: 'Wall',
      });

      const rep = await shell.api.geometry.addMeshRepresentation(file, context, {
        vertices: [[[0, 0, 0], [1, 0, 0], [1, 1, 0], [0, 1, 0]]],
        faces: [[[[0, 1, 2], [0, 2, 3]]]],
      });
      expect(rep.type).toBeDefined();

      const rel = await shell.api.geometry.assignRepresentation(file, wall, rep);
      expect(rel.type).toBeDefined();

      const productRep = await shell.api.representation.getProductRepresentation(wall, {});
      expect(productRep.type).toBeDefined();

      shell.api.geometry.unassignRepresentation(file, wall, rep);

      shell.api.geometry.removeRepresentation(file, rep, {});
      shell.api.context.removeContext(file, context);
    });

    it('copies representation between products', async () => {
      await using file = await IfcFile.createEmpty(shell, 'IFC4');
      const context = await withBodyContext(file);
      const wall1 = await shell.api.root.createEntity(file, {
        ifcClass: 'IfcWall',
        name: 'Wall 1',
      });
      const wall2 = await shell.api.root.createEntity(file, {
        ifcClass: 'IfcWall',
        name: 'Wall 2',
      });

      const rep = await shell.api.geometry.addMeshRepresentation(file, context, {
        vertices: [[[0, 0, 0], [1, 0, 0], [1, 1, 0], [0, 1, 0]]],
        faces: [[[[0, 1, 2], [0, 2, 3]]]],
      });
      await shell.api.geometry.assignRepresentation(file, wall1, rep);

      expect(await rep.get('RepresentationIdentifier')).toBe('Body');

      const copy = await shell.api.geometry.copyRepresentation(file, {
        source: wall1,
        target: wall2,
        contextIdentifier: 'Body',
      });
      expect(copy).not.toBeNull();
      if (!copy) throw new Error('copyRepresentation returned null — source may lack Body representation');
      expect(copy.type).toBe('IfcShapeRepresentation');

      expect(copy.id).not.toBe(rep.id);

      const targetRep = await shell.api.representation.getProductRepresentation(wall2, {
        contextType: 'Model',
        subcontext: 'Body',
        targetView: 'MODEL_VIEW',
      });
      expect(targetRep).not.toBeNull();
      if (!targetRep) throw new Error('Target product has no Body representation');
      expect(targetRep.id).toBe(copy.id);

      using sourceContextAttribute = rep.attribute('ContextOfItems');
      using copiedContextAttribute = copy.attribute('ContextOfItems');
      const sourceContext = sourceContextAttribute.entity();
      const copiedContext = copiedContextAttribute.entity();
      expect(sourceContext).not.toBeNull();
      expect(copiedContext).not.toBeNull();
      if (!sourceContext || !copiedContext) throw new Error('Representation has no context');
      expect(copiedContext.id).toBe(sourceContext.id);

      const sourceAfterCopy = await shell.api.representation.getProductRepresentation(wall1, {
        contextType: 'Model',
        subcontext: 'Body',
        targetView: 'MODEL_VIEW',
      });
      expect(sourceAfterCopy).not.toBeNull();
      if (!sourceAfterCopy) throw new Error('Source product lost its Body representation');
      expect(sourceAfterCopy.id).toBe(rep.id);
      expect(await sourceAfterCopy.get('RepresentationIdentifier')).toBe('Body');

      shell.api.geometry.removeRepresentation(file, rep, {});
      shell.api.context.removeContext(file, context);
    });

    it('returns null when no matching representation exists', async () => {
      await using file = await IfcFile.createEmpty(shell, 'IFC4');
      const context = await withBodyContext(file);
      const wall1 = await shell.api.root.createEntity(file, {
        ifcClass: 'IfcWall',
        name: 'Wall 1',
      });
      const wall2 = await shell.api.root.createEntity(file, {
        ifcClass: 'IfcWall',
        name: 'Wall 2',
      });

      const noRepresentation = await shell.api.geometry.copyRepresentation(file, {
        source: wall1,
        target: wall2,
      });
      expect(noRepresentation).toBeNull();

      const rep = await shell.api.geometry.addMeshRepresentation(file, context, {
        vertices: [[[0, 0, 0], [1, 0, 0], [1, 1, 0], [0, 1, 0]]],
        faces: [[[[0, 1, 2], [0, 2, 3]]]],
      });
      await shell.api.geometry.assignRepresentation(file, wall1, rep);

      const wrongContext = await shell.api.geometry.copyRepresentation(file, {
        source: wall1,
        target: wall2,
        contextIdentifier: 'Axis',
      });
      expect(wrongContext).toBeNull();
    });

    it('edits object placement with identity matrix', async () => {
      await using file = await IfcFile.createEmpty(shell, 'IFC4');
      const wall = await shell.api.root.createEntity(file, {
        ifcClass: 'IfcWall',
        name: 'Wall',
      });

      const identity = [1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1];
      const result = await shell.api.geometry.editObjectPlacement(file, {
        product: wall,
        matrix: identity,
        isSi: true,
        shouldTransformChildren: false,
      });
      expect(result.type).toBeDefined();
    });
  });

  describe('georeference', () => {
    it('adds and removes georeferencing', async () => {
      await using file = await IfcFile.createEmpty(shell, 'IFC4');
      shell.api.georeference.addGeoreferencing(file, {
        ifcClass: 'IfcMapConversion',
        name: 'CRS',
      });

      shell.api.georeference.editTrueNorth(file, { trueNorth: [0, 1] });

      shell.api.georeference.removeGeoreferencing(file);
    });
  });

  describe('grid', () => {
    it('creates and removes grid axes', async () => {
      await using file = await IfcFile.createEmpty(shell, 'IFC4');
      const grid = await shell.api.root.createEntity(file, {
        ifcClass: 'IfcGrid',
        name: 'Grid',
      });

      const axis = await shell.api.grid.createGridAxis(
        file, grid, 'A', true, 'UAxes',
      );
      expect(axis.type).toBe('IfcGridAxis');

      shell.api.grid.createAxisCurve(
        file,
        [0, 0, 0],
        [10, 0, 0],
        axis,
        true,
      );

      shell.api.grid.removeGridAxis(file, axis);
    });
  });

  describe('layer', () => {
    it('creates and removes presentation layers', async () => {
      await using file = await IfcFile.createEmpty(shell, 'IFC4');
      const layer = await shell.api.layer.addLayer(file, 'Test Layer');
      expect(layer.type).toBe('IfcPresentationLayerAssignment');

      shell.api.layer.removeLayer(file, layer);
    });

    it('assigns layers and materials to products', async () => {
      await using file = await IfcFile.createEmpty(shell, 'IFC4');
      const wall = await shell.api.root.createEntity(file, {
        ifcClass: 'IfcWall',
        name: 'Wall',
      });

      const layer = await shell.api.layer.addLayer(file, 'Test Layer');
      expect(layer).not.toBeNull();
      if (!layer) throw new Error('addLayer returned null');

      shell.api.layer.assignLayer(file, [wall], layer);
      const assignedItems = layer.get('AssignedItems');
      expect(Array.isArray(assignedItems) && assignedItems.some((item) => item.id === wall.id)).toBe(true);

      const mat = await shell.api.material.addMaterial(file, { name: 'Concrete' });
      expect(mat).not.toBeNull();
      if (!mat) throw new Error('addMaterial returned null');

      shell.api.material.assignMaterial(file, [wall], { material: mat });
      expect(shell.api.element.getMaterial(wall, {})).not.toBeNull();

      shell.api.material.unassignMaterial(file, [wall], {});
      shell.api.layer.unassignLayer(file, [wall], layer);
      shell.api.material.removeMaterial(file, mat);
    });
  });

  describe('material', () => {
    it('creates and removes materials', async () => {
      await using file = await IfcFile.createEmpty(shell, 'IFC4');
      const mat = await shell.api.material.addMaterial(file, {
        name: 'Concrete',
        category: 'Building',
      });
      expect(mat.type).toBe('IfcMaterial');

      shell.api.material.removeMaterial(file, mat);
    });

    it('creates material layer sets', async () => {
      await using file = await IfcFile.createEmpty(shell, 'IFC4');
      const set = await shell.api.material.addMaterialSet(file, {
        name: 'Wall Layers',
        setType: 'IfcMaterialLayerSet',
      });
      expect(set.type).toBe('IfcMaterialLayerSet');

      shell.api.material.removeMaterialSet(file, set);
    });
  });

  describe('placement', () => {
    it('constructs matrices and queries placements', () => {
      const matrix = shell.api.placement.matrixFromAxes(
        [1, 2, 3],
        [0, 0, 1],
        [1, 0, 0],
      );
      expect(matrix).toHaveLength(16);
    });

    it('queries storey elevation from site', async () => {
      await using file = await IfcFile.createEmpty(shell, 'IFC4');
      const site = await shell.api.root.createEntity(file, {
        ifcClass: 'IfcSite',
        name: 'Site',
      });
      expect(typeof shell.api.placement.getStoreyElevation(site)).toBe('number');
    });

    it('gets local placement from existing entity', async () => {
      await using file = await IfcFile.createEmpty(shell, 'IFC4');
      const wall = await shell.api.root.createEntity(file, {
        ifcClass: 'IfcWall',
        name: 'Wall',
      });
      const matrix = shell.api.placement.getLocalPlacement(wall);
      expect(matrix).toHaveLength(16);
    });
  });

  describe('profile', () => {
    it('creates and copies parameterized profiles', async () => {
      await using file = await IfcFile.createEmpty(shell, 'IFC4');
      const profile = await shell.api.profile.addParameterizedProfile(
        file, 'IfcRectangleProfileDef', 'AREA',
      );
      expect(profile.type).toBe('IfcRectangleProfileDef');

      const copy = await shell.api.profile.copyProfile(file, profile);
      expect(copy.type).toBe('IfcRectangleProfileDef');

      shell.api.profile.removeProfile(file, copy);
      shell.api.profile.removeProfile(file, profile);
    });

    it('edits profile properties', async () => {
      await using file = await IfcFile.createEmpty(shell, 'IFC4');
      const profile = await shell.api.profile.addParameterizedProfile(
        file, 'IfcRectangleProfileDef', 'AREA',
      );

      shell.api.profile.editProfile(profile, {
        ProfileName: '100x200',
        XDim: 100.0,
        YDim: 200.0,
      });

      expect(await profile.get('ProfileName')).toBe('100x200');
      expect(await profile.get('XDim')).toBe(100.0);
      expect(await profile.get('YDim')).toBe(200.0);
    });
  });

  describe('representation', () => {
    it('resolves representation and its base items', async () => {
      await using file = await IfcFile.createEmpty(shell, 'IFC4');
      const context = await withBodyContext(file);

      const contexts = await shell.api.representation.getPrioritisedContexts(file);
      expect(contexts.length).toBeGreaterThanOrEqual(1);

      const wall = await shell.api.root.createEntity(file, {
        ifcClass: 'IfcWall',
        name: 'Wall',
      });
      const rep = await shell.api.geometry.addMeshRepresentation(file, context, {
        vertices: [[[0, 0, 0], [1, 0, 0], [1, 1, 0]]],
        faces: [[[[0, 1, 2]]]],
      });
      await shell.api.geometry.assignRepresentation(file, wall, rep);

      const resolved = await shell.api.representation.resolve(rep);
      expect(resolved.type).toBeDefined();

      const items = await shell.api.representation.resolveBaseItems(resolved);
      expect(items.length).toBeGreaterThanOrEqual(1);

      shell.api.geometry.removeRepresentation(file, rep, {});
      shell.api.context.removeContext(file, context);
    });
  });

  describe('shape', () => {
    it('tests floating-point comparison', () => {
      expect(shell.api.shape.isAlmostEqual(1, 1, 0.001)).toBe(true);
      expect(shell.api.shape.isAlmostEqual(1, 1.1, 0.001)).toBe(false);
    });
  });

  describe('style', () => {
    it('adds and removes surface styles', async () => {
      await using file = await IfcFile.createEmpty(shell, 'IFC4');
      const style = await shell.api.style.addStyle(
        file,
        'Test Style',
        'IfcSurfaceStyle',
      );
      expect(style.type).toBe('IfcSurfaceStyle');

      const wall = await shell.api.root.createEntity(file, {
        ifcClass: 'IfcWall',
        name: 'Wall',
      });

      const rel = await shell.api.style.assignItemStyle(file, {
        item: wall,
        style,
        shouldUsePresentationStyleAssignment: false,
      });
      expect(rel.type).toBeDefined();

      shell.api.style.removeStyle(file, style);
    });
  });
});
