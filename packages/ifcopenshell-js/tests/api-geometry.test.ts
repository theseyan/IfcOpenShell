
import { afterAll, beforeAll, describe, expect, it } from 'vitest';
import { IfcFile, type Entity, type IfcOpenShell } from '../src/index.js';
import type { IfcOpenShellGeometryWallMountedHandrailResult } from '../src/generated/ifcopenshell_api.js';
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
    it('computes ergonomic nested railing geometry without an IFC file', () => {
      const result: IfcOpenShellGeometryWallMountedHandrailResult =
        shell.api.geometry.computeWallMountedHandrailGeometry({
          railingPath: [[0, 0, 1], [2, 0, 1]],
          supportSpacing: 1,
          railingDiameter: 0.05,
          clearWidth: 0.04,
          height: 1,
          terminalType: 'NONE',
        });

      expect(result).toEqual({
        handrailPolyline: [[0, 0, 1.025], [2, 0, 1.025]],
        handrailArcPointIndices: [],
        handrailRadius: 0.025,
        supports: expect.any(Array),
      });
      expect(result.supports).toHaveLength(3);
      expect(result.supports[0]).toMatchObject({
        arcPolyline: expect.any(Array),
        arcRadius: 0.01,
        diskPosition: expect.any(Array),
        diskRadius: 0.025,
        diskDepth: 0.02,
        diskZRotation: expect.any(Number),
      });
      expect(result.supports[0].arcPolyline).toHaveLength(3);
      expect(result.supports[0].diskPosition).toEqual(result.supports[0].arcPolyline[2]);
    });

    it('handles manual, looped, collinear, and degenerate railing paths', () => {
      const required = {
        supportSpacing: 1,
        railingDiameter: 0.05,
        clearWidth: 0.04,
        height: 1,
      };
      const manual = shell.api.geometry.computeWallMountedHandrailGeometry({
        ...required,
        railingPath: [[0, 0, 1], [1, 0, 1], [2, 0, 1]],
        useManualSupports: true,
        terminalType: 'NONE',
      });
      expect(manual.supports).toHaveLength(1);

      const loop = shell.api.geometry.computeWallMountedHandrailGeometry({
        ...required,
        railingPath: [[0, 0, 1], [2, 0, 1], [2, 2, 1], [0, 2, 1]],
        loopedPath: true,
      });
      expect(loop.handrailArcPointIndices).toHaveLength(4);
      expect(loop.handrailPolyline.flat().every(Number.isFinite)).toBe(true);

      const degenerate = shell.api.geometry.computeWallMountedHandrailGeometry({
        ...required,
        railingPath: [[0, 0, 1], [0, 0, 1], [0.6, 0.8, 1], [1.2, 1.6, 1]],
        terminalType: 'NONE',
      });
      expect(degenerate.handrailArcPointIndices).toEqual([]);
      expect(degenerate.handrailPolyline.flat().every(Number.isFinite)).toBe(true);

      expect(() => shell.api.geometry.computeWallMountedHandrailGeometry({
        ...required,
        railingPath: [[0, 0, 1]],
      })).toThrow(/at least two/i);
    });

    it('materializes a default railing with the handrail item last', async () => {
      await using file = await IfcFile.createEmpty(shell, 'IFC4');
      const context = await withBodyContext(file);
      const representation = shell.api.geometry.addRailingRepresentation(file, {
        context,
        railingPath: [[0, 0, 1], [2, 0, 1]],
        terminalType: 'NONE',
      });

      expect(representation.type).toBe('IfcShapeRepresentation');
      expect(representation.get('RepresentationType')).toBe('SolidModel');
      using itemsAttribute = representation.attribute('Items');
      const items = itemsAttribute.entities();
      expect(items).toHaveLength(7);
      expect(items.at(-1)?.type).toBe('IfcSweptDiskSolid');
      items.forEach((item) => item.dispose());
    });

    it('enforces fixed geometry and placement contracts before mutation', async () => {
      await using file = await IfcFile.createEmpty(shell, 'IFC4');
      const context = await withBodyContext(file);
      const wall = shell.api.root.createEntity(file, { ifcClass: 'IfcWall' });

      const axis2d = shell.api.geometry.addAxisRepresentation(file, context, [[0, 0], [1, 0]]);
      const axis3d = shell.api.geometry.addAxisRepresentation(file, context, [[0, 0, 0], [0, 0, 1]]);
      expect(axis2d.type).toBe('IfcShapeRepresentation');
      expect(axis3d.type).toBe('IfcShapeRepresentation');

      const matrix = shell.api.placement.matrixFromAxes([0, 0, 0], [0, 0, 1], [1, 0, 0]);
      expect(matrix).toHaveLength(16);
      const placement = shell.api.geometry.editObjectPlacement(file, {
        product: wall,
        matrix,
        isSi: false,
        shouldTransformChildren: false,
      });
      expect(placement.type).toBe('IfcLocalPlacement');

      const block = shell.api.shape.builderBlock(file, {});
      expect(shell.api.geometry.clipSolid(file, {
        item: block,
        location: [0, 0, 0],
        normal: [0, 0, 1],
      }).type).toBe('IfcBooleanClippingResult');

      const entityCount = file.entityCount;
      expect(() => shell.api.geometry.addAxisRepresentation(
        file,
        context,
        [[0, 0], [1, 0, 0]] as never,
      )).toThrow(/variant alternative/);
      expect(() => shell.api.placement.matrixFromAxes(
        [0, 0] as never,
        [0, 0, 1],
        [1, 0, 0],
      )).toThrow(/(?:exactly|contain) 3 (?:array )?items/);
      expect(() => shell.api.geometry.editObjectPlacement(file, {
        product: wall,
        matrix: matrix.slice(0, 15) as never,
        isSi: false,
        shouldTransformChildren: false,
      })).toThrow(/(?:exactly|contain) 16 (?:array )?items/);
      expect(() => shell.api.geometry.clipSolid(file, {
        item: block,
        location: [0, 0, 0],
        normal: [0, 1] as never,
      })).toThrow(/(?:exactly|contain) 3 (?:array )?items/);
      expect(file.entityCount).toBe(entityCount);
    });

    it('enforces fixed contracts across boundary, grid, profile, and structural APIs', async () => {
      await using file = await IfcFile.createEmpty(shell, 'IFC4');
      await withBodyContext(file);
      const boundary = shell.api.root.createEntity(file, { ifcClass: 'IfcRelSpaceBoundary' });
      const grid = shell.api.root.createEntity(file, { ifcClass: 'IfcGrid', name: 'Grid' });
      const gridAxis = shell.api.grid.createGridAxis(file, grid, 'A', true, 'UAxes');
      const structuralMember = shell.api.root.createEntity(file, { ifcClass: 'IfcStructuralCurveMember' });
      const boundaryOptions = {
        outerBoundary: [[0, 0], [1, 0], [1, 1], [0, 0]] as [number, number][],
        location: [0, 0, 0] as [number, number, number],
        axis: [0, 0, 1] as [number, number, number],
        refDirection: [1, 0, 0] as [number, number, number],
        innerBoundaries: [],
        unitScale: 1,
      };

      shell.api.boundary.assignConnectionGeometry(file, boundary, boundaryOptions);
      shell.api.georeference.editTrueNorth(file, { trueNorth: [0, 1] });
      shell.api.grid.createAxisCurve(file, [0, 0, 0], [1, 0, 0], gridAxis, false);
      shell.api.structural.editStructuralItemAxis(file, structuralMember, [0, 1, 0]);
      expect(shell.api.profile.addArbitraryProfile(file, {
        profile: [[0, 0], [1, 0], [1, 1], [0, 0]],
      }).type).toMatch(/IfcArbitrary/);

      const entityCount = file.entityCount;
      expect(() => shell.api.boundary.assignConnectionGeometry(file, boundary, {
        ...boundaryOptions,
        location: [0, 0] as never,
      })).toThrow(/(?:exactly|contain) 3 (?:array )?items/);
      expect(() => shell.api.georeference.editTrueNorth(file, {
        trueNorth: [0, 1, 0] as never,
      })).toThrow(/(?:exactly|contain) 2 (?:array )?items/);
      expect(() => shell.api.grid.createAxisCurve(
        file,
        [0, 0] as never,
        [1, 0, 0],
        gridAxis,
        false,
      )).toThrow(/(?:exactly|contain) 3 (?:array )?items/);
      expect(() => shell.api.structural.editStructuralItemAxis(
        file,
        structuralMember,
        [0, 1] as never,
      )).toThrow(/(?:exactly|contain) 3 (?:array )?items/);
      expect(() => shell.api.profile.addArbitraryProfile(file, {
        profile: [[0, 0], [1, 0, 0]] as never,
      })).toThrow(/variant alternative/);
      expect(file.entityCount).toBe(entityCount);
    });

    it('creates every closed door operation with semantic properties', async () => {
      await using file = await IfcFile.createEmpty(shell, 'IFC4');
      const context = await withBodyContext(file);
      const operations = [
        'SINGLE_SWING_LEFT',
        'SINGLE_SWING_RIGHT',
        'DOUBLE_SWING_RIGHT',
        'DOUBLE_SWING_LEFT',
        'DOUBLE_DOOR_SINGLE_SWING',
        'DOUBLE_DOOR_DOUBLE_SWING',
        'SLIDING_TO_LEFT',
        'SLIDING_TO_RIGHT',
        'DOUBLE_DOOR_SLIDING',
      ] as const;

      for (const operationType of operations) {
        const representation = shell.api.geometry.addDoorRepresentation(file, {
          context,
          operationType,
          liningProperties: { transomThickness: 0.05 },
          panelProperties: { panelWidth: 0.9 },
        });
        expect(representation.type).toBe('IfcShapeRepresentation');
        representation.dispose();
      }
    });

    it('rejects invalid door inputs before file mutation', async () => {
      await using file = await IfcFile.createEmpty(shell, 'IFC4');
      const context = await withBodyContext(file);
      const entityCount = file.entityCount;

      expect(() => shell.api.geometry.addDoorRepresentation(file, {
        context,
        operationType: 'INVALID' as never,
      })).toThrow(/Invalid literal/);
      expect(file.entityCount).toBe(entityCount);

      expect(() => shell.api.geometry.addDoorRepresentation(file, {
        context,
        liningProperties: { liningDepth: Number.NaN },
      })).toThrow(/must be finite/);
      expect(file.entityCount).toBe(entityCount);
    });

    it('creates every window partition from semantic properties', async () => {
      await using file = await IfcFile.createEmpty(shell, 'IFC4');
      const context = await withBodyContext(file);
      const partitions = [
        ['SINGLE_PANEL', 4],
        ['DOUBLE_PANEL_HORIZONTAL', 8],
        ['DOUBLE_PANEL_VERTICAL', 8],
        ['TRIPLE_PANEL_BOTTOM', 12],
        ['TRIPLE_PANEL_HORIZONTAL', 13],
        ['TRIPLE_PANEL_LEFT', 12],
        ['TRIPLE_PANEL_RIGHT', 12],
        ['TRIPLE_PANEL_TOP', 12],
        ['TRIPLE_PANEL_VERTICAL', 13],
      ] as const;

      for (const [partitionType, expectedItems] of partitions) {
        const representation = shell.api.geometry.addWindowRepresentation(file, {
          context,
          partitionType,
          liningProperties: { liningDepth: 0.05, mullionThickness: 0.05 },
          panelProperties: [
            { frameDepth: 0.035 },
            { frameThickness: 0.035 },
            {},
          ],
        });
        expect(representation.type).toBe('IfcShapeRepresentation');
        using itemsAttribute = representation.attribute('Items');
        const items = itemsAttribute.entities();
        expect(items).toHaveLength(expectedItems);
        items.forEach((item) => item.dispose());
        representation.dispose();
      }
    });

    it('rejects invalid window inputs before file mutation', async () => {
      await using file = await IfcFile.createEmpty(shell, 'IFC4');
      const context = await withBodyContext(file);
      const entityCount = file.entityCount;

      expect(() => shell.api.geometry.addWindowRepresentation(file, {
        context,
        partitionType: 'INVALID' as never,
      })).toThrow(/Invalid literal/);
      expect(file.entityCount).toBe(entityCount);

      expect(() => shell.api.geometry.addWindowRepresentation(file, {
        context,
        liningProperties: { liningDepth: Number.NaN },
      })).toThrow(/must be finite/);
      expect(file.entityCount).toBe(entityCount);

      expect(() => shell.api.geometry.addWindowRepresentation(file, {
        context,
        partitionType: 'DOUBLE_PANEL_VERTICAL',
      })).toThrow(/do not cover/);
      expect(file.entityCount).toBe(entityCount);
    });

    it('applies wall and slab defaults with semantic clippings', async () => {
      await using file = await IfcFile.createEmpty(shell, 'IFC4');
      const context = await withBodyContext(file);
      const clipping = {
        location: [0, 0, 0.1] as [number, number, number],
        normal: [0, 0, 1] as [number, number, number],
      };

      const wall = shell.api.geometry.addWallRepresentation(file, { context });
      expect(await wall.get('RepresentationType')).toBe('SweptSolid');
      using wallItemsAttribute = wall.attribute('Items');
      const wallItems = wallItemsAttribute.entities();
      expect(await wallItems[0]!.get('Depth')).toBeCloseTo(3);
      wallItems.forEach((item) => item.dispose());

      const clippedWall = shell.api.geometry.addWallRepresentation(file, {
        context,
        clippings: [clipping],
      });
      expect(await clippedWall.get('RepresentationType')).toBe('Clipping');

      const slab = shell.api.geometry.addSlabRepresentation(file, { context });
      expect(await slab.get('RepresentationType')).toBe('SweptSolid');
      using slabItemsAttribute = slab.attribute('Items');
      const slabItems = slabItemsAttribute.entities();
      expect(await slabItems[0]!.get('Depth')).toBeCloseTo(0.2);
      slabItems.forEach((item) => item.dispose());

      const clippedSlab = shell.api.geometry.addSlabRepresentation(file, {
        context,
        clippings: [clipping],
      });
      expect(await clippedSlab.get('RepresentationType')).toBe('Clipping');
    });

    it('adds profile representation through the generated semantic API', async () => {
      await using file = await IfcFile.createEmpty(shell, 'IFC4');
      const context = await withBodyContext(file);
      const profile = await shell.api.profile.addParameterizedProfile(
        file, 'IfcRectangleProfileDef', 'AREA',
      );
      shell.api.profile.editProfile(profile, { XDim: 2.0, YDim: 1.0 });

      const rep = await shell.api.geometry.addProfileRepresentation(file, {
        context,
        profile,
      });

      expect(rep.type).toBe('IfcShapeRepresentation');
      expect(await rep.get('RepresentationIdentifier')).toBe('Body');
      expect(await rep.get('RepresentationType')).toBe('SweptSolid');
      using itemsAttribute = rep.attribute('Items');
      const items = itemsAttribute.entities();
      expect(await items[0]!.get('Depth')).toBeCloseTo(1);

      const preExisting = shell.api.geometry.clipSolid(file, {
        item: items[0]!,
        location: [2, 0, 0],
        normal: [1, 0, 0],
      });
      using originalFirstOperandAttribute = preExisting.attribute('FirstOperand');
      const originalFirstOperand = originalFirstOperandAttribute.entity();
      const clipped = shell.api.geometry.addProfileRepresentation(file, {
        context,
        profile,
        clippings: [
          { location: [1, 0, 0], normal: [1, 0, 0] },
          { entity: preExisting },
          { location: [3, 0, 0], normal: [1, 0, 0] },
        ],
      });
      expect(await clipped.get('RepresentationType')).toBe('Clipping');
      using firstOperandAfterAttribute = preExisting.attribute('FirstOperand');
      const firstOperandAfter = firstOperandAfterAttribute.entity();
      expect(firstOperandAfter?.id).toBe(originalFirstOperand?.id);
      originalFirstOperand?.dispose();
      firstOperandAfter?.dispose();
      items.forEach((item) => item.dispose());
    });

    it('rejects ambiguous clipping alternatives before file mutation', async () => {
      await using file = await IfcFile.createEmpty(shell, 'IFC4');
      const context = await withBodyContext(file);
      const entityCount = file.entityCount;
      const invalid = {
        location: [0, 0, 0] as [number, number, number],
        normal: [0, 0, 1] as [number, number, number],
        entity: context,
      };

      expect(() => shell.api.geometry.addWallRepresentation(file, {
        context,
        clippings: [invalid],
      })).toThrow(/exactly one variant alternative/);
      expect(file.entityCount).toBe(entityCount);
    });

    it('adds mesh representation and assigns to product', async () => {
      await using file = await IfcFile.createEmpty(shell, 'IFC4');
      const context = await withBodyContext(file);
      const wall = await shell.api.root.createEntity(file, {
        ifcClass: 'IfcWall',
        name: 'Wall',
      });

      const rep = await shell.api.geometry.addMeshRepresentation(file, context, {
        items: [{
          vertices: [[0, 0, 0], [1, 0, 0], [1, 1, 0], [0, 1, 0]],
          faces: [{ outer: [0, 1, 2] }, { outer: [0, 2, 3] }],
        }],
        coordinateOffset: [1, 2, 3],
        unitScale: 1,
      });
      expect(rep.type).toBeDefined();

      const entityCount = file.entityCount;
      expect(() => shell.api.geometry.addMeshRepresentation(file, context, {
        items: [{
          vertices: [[0, 0, 0], [1, 0, 0], [0, 1, 0]],
          faces: [{ outer: [0, 1, 3] }],
        }],
      })).toThrow(/out of range/);
      expect(() => shell.api.geometry.addMeshRepresentation(file, context, {
        items: [{
          vertices: [[0, 0, 0], [1, 0, 0], [0, 1, 0]],
          faces: [{ outer: [-1, 1, 2] }],
        }],
      })).toThrow(/out of range/);
      expect(file.entityCount).toBe(entityCount);

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
        items: [{
          vertices: [[0, 0, 0], [1, 0, 0], [1, 1, 0], [0, 1, 0]],
          faces: [{ outer: [0, 1, 2] }, { outer: [0, 2, 3] }],
        }],
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
        items: [{
          vertices: [[0, 0, 0], [1, 0, 0], [1, 1, 0], [0, 1, 0]],
          faces: [{ outer: [0, 1, 2] }, { outer: [0, 2, 3] }],
        }],
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
        items: [{
          vertices: [[0, 0, 0], [1, 0, 0], [1, 1, 0]],
          faces: [{ outer: [0, 1, 2] }],
        }],
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
