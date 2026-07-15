import { afterAll, beforeAll, describe, expect, it } from 'vitest';
import { IfcFile, type Entity, type IfcOpenShell } from '../src/index.js';
import { createInstance, describeGeneratedOrSkip, GENERATED_WASM_DIR } from './_helper.js';

const EDIT_METHODS = [
  ['classification', 'editClassification'],
  ['classification', 'editReference'],
  ['constraint', 'editMetric'],
  ['constraint', 'editObjective'],
  ['document', 'editInformation'],
  ['document', 'editReference'],
  ['drawing', 'editTextLiteral'],
  ['group', 'editGroup'],
  ['layer', 'editLayer'],
  ['library', 'editLibrary'],
  ['library', 'editReference'],
  ['material', 'editAssignedMaterial'],
  ['material', 'editConstituent'],
  ['material', 'editLayer'],
  ['material', 'editLayerUsage'],
  ['material', 'editMaterial'],
  ['material', 'editProfile'],
  ['owner', 'editActor'],
  ['owner', 'editAddress'],
  ['owner', 'editApplication'],
  ['owner', 'editOrganisation'],
  ['owner', 'editPerson'],
  ['owner', 'editRole'],
  ['profile', 'editProfile'],
  ['psetTemplate', 'editPsetTemplate'],
  ['resource', 'editResource'],
  ['resource', 'editResourceQuantity'],
  ['structural', 'editStructuralAnalysisModel'],
  ['structural', 'editStructuralLoad'],
  ['structural', 'editStructuralLoadCase'],
  ['style', 'editPresentationStyle'],
  ['system', 'editSystem'],
  ['unit', 'editDerivedUnit'],
  ['unit', 'editMonetaryUnit'],
] as const;

describeGeneratedOrSkip('generated named edit wrappers', () => {
  let shell: IfcOpenShell;

  beforeAll(async () => {
    shell = await createInstance(GENERATED_WASM_DIR!);
    await shell.loadPlugin('schema', 'ifc4');
    await shell.loadPlugin('schema', 'ifc2x3');
  });

  afterAll(async () => {
    await shell?.dispose();
  });

  it('exposes every mechanical editor under its named module', () => {
    const api = shell.api as unknown as Record<string, Record<string, unknown>>;
    for (const [module, method] of EDIT_METHODS) {
      expect(typeof api[module]?.[method], `${module}.${method}`).toBe('function');
    }
    expect(typeof api.pset?.templateEditPsetTemplate).toBe('function');
    expect(typeof api.material?.reorderSetItem).toBe('function');
    expect(typeof api.shape?.builderGetPolylineCoords).toBe('function');
    expect(typeof api.shape?.builderSetPolylineCoords).toBe('function');
    expect(typeof api.shapeBuilder?.getPolylineCoords).toBe('function');
    expect(typeof api.shapeBuilder?.setPolylineCoords).toBe('function');
    expect(api.psetTemplate).toBe(api.pset_template);
    expect(api.shapeBuilder).toBe(api.shape_builder);
  });

  it('calls every named editor and lowers property values, entities, lists, and nulls', async () => {
    await using file = await IfcFile.createEmpty(shell, 'IFC4');
    const entities: Entity[] = [];
    const create = async (ifcClass: string): Promise<Entity> => {
      const entity = await shell.api.root.createEntity(file, { ifcClass });
      entities.push(entity);
      return entity;
    };

    try {
      const classification = await create('IfcClassification');
      const classificationReference = await create('IfcClassificationReference');
      const metric = await create('IfcMetric');
      const objective = await create('IfcObjective');
      const documentInformation = await create('IfcDocumentInformation');
      const documentReference = await create('IfcDocumentReference');
      const textLiteral = await create('IfcTextLiteral');
      const group = await create('IfcGroup');
      const layer = await create('IfcPresentationLayerAssignment');
      const library = await create('IfcLibraryInformation');
      const libraryReference = await create('IfcLibraryReference');
      const material = await create('IfcMaterial');
      const constituent = await create('IfcMaterialConstituent');
      const materialLayer = await create('IfcMaterialLayer');
      const layerUsage = await create('IfcMaterialLayerSetUsage');
      const materialProfile = await create('IfcMaterialProfile');
      const wall = await create('IfcWall');
      const actor = await create('IfcActor');
      const address = await create('IfcPostalAddress');
      const application = await create('IfcApplication');
      const organisation = await create('IfcOrganization');
      const person = await create('IfcPerson');
      const role = await create('IfcActorRole');
      const psetTemplate = await create('IfcPropertySetTemplate');
      const resource = await create('IfcLaborResource');
      const quantity = await create('IfcQuantityCount');
      const analysisModel = await create('IfcStructuralAnalysisModel');
      const load = await create('IfcStructuralLoadStatic');
      const loadCase = await create('IfcStructuralLoadCase');
      const presentationStyle = await create('IfcSurfaceStyle');
      const system = await create('IfcDistributionSystem');
      const derivedUnit = await create('IfcDerivedUnit');
      const monetaryUnit = await create('IfcMonetaryUnit');
      const profile = await create('IfcCircleProfileDef');

      shell.api.classification.editClassification(file, classification, {});
      shell.api.classification.editReference(file, classificationReference, {});
      shell.api.constraint.editMetric(file, metric, {});
      shell.api.constraint.editObjective(file, objective, {});
      shell.api.document.editInformation(file, documentInformation, {});
      shell.api.document.editReference(file, documentReference, {});
      shell.api.drawing.editTextLiteral(file, textLiteral, { Literal: 'Annotation' });
      shell.api.group.editGroup(file, group, { Name: 'Named group' });
      shell.api.layer.editLayer(file, layer, {});
      shell.api.library.editLibrary(file, library, {
        VersionDate: { value: '2026-07-15T12:34:56.123456+05:30', type: 'IfcDateTime' },
      });
      shell.api.library.editReference(file, libraryReference, {});
      shell.api.material.editAssignedMaterial(file, wall, {});
      shell.api.material.editConstituent(file, constituent, {}, null);
      shell.api.material.editLayer(file, materialLayer, {}, null);
      shell.api.material.editLayerUsage(file, layerUsage, {});
      shell.api.material.editMaterial(file, material, {});
      shell.api.material.editProfile(file, materialProfile, {}, null, null);
      shell.api.owner.editActor(file, actor, {});
      shell.api.owner.editAddress(file, address, {});
      shell.api.owner.editApplication(file, application, {});
      shell.api.owner.editOrganisation(file, organisation, {});
      shell.api.owner.editPerson(file, person, { MiddleNames: ['The'] });
      shell.api.owner.editRole(file, role, {});
      shell.api.profile.editProfile(profile, { ProfileName: 'Circle' });
      shell.api.psetTemplate.editPsetTemplate(file, psetTemplate, { Name: 'Pset_Named' });
      shell.api.resource.editResource(file, resource, {});
      shell.api.resource.editResourceQuantity(file, quantity, { CountValue: 2 });
      shell.api.structural.editStructuralAnalysisModel(file, analysisModel, {});
      shell.api.structural.editStructuralLoad(file, load, {});
      shell.api.structural.editStructuralLoadCase(file, loadCase, {});
      shell.api.style.editPresentationStyle(file, presentationStyle, {});
      shell.api.system.editSystem(file, system, {});
      shell.api.unit.editDerivedUnit(file, derivedUnit, {});
      shell.api.unit.editMonetaryUnit(file, monetaryUnit, {});

      expect(group.get('Name')).toBe('Named group');
      expect(person.get('MiddleNames')).toEqual(['The']);
      expect(library.get('VersionDate')).toBe('2026-07-15T12:34:56.123456+05:30');
    } finally {
      entities.forEach((entity) => entity.dispose());
    }
  });

  it('normalizes library VersionDate for IFC2X3 in one call', async () => {
    await using file = await IfcFile.createEmpty(shell, 'IFC2X3');
    using library = await shell.api.root.createEntity(file, { ifcClass: 'IfcLibraryInformation' });
    shell.api.library.editLibrary(file, library, {
      VersionDate: { value: '2026-07-15T12:34:56.123456+05:30', type: 'IfcDateTime' },
    });
    using versionDate = library.get('VersionDate') as Entity;
    expect(versionDate.type).toBe('IfcCalendarDate');
    expect(versionDate.get('DayComponent')).toBe(15);
  });
});
