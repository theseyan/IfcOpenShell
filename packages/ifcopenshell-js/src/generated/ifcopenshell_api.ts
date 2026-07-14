// This file was generated with the assistance of an AI coding tool.

import { AttributeValue } from '../attribute.js';
import { Entity } from '../entity.js';
import { IfcFile } from '../file.js';
import { GeomIterator, GeomSettings, GeometryTree } from '../geom/index.js';
import { SerializerSettings } from '../serializers/index.js';
import {
  InstanceList,
  Value,
  fromRawValue,
  toRawValue,
  wrapList,
  wrapValue,
  type ApiData,
  type ValueData,
  type ValueInput,
} from '../api.js';
import { PsetProperties, toRawPsetProperties, type PsetInput } from '../pset.js';
import type { IfcOpenShell } from '../init.js';

type RawValue = ApiData | object | RawValue[];
type ApiInput = ApiData | PsetProperties | PsetInput;
type Disposable = { destroy(): void };
type RawApi = {
  aggregate: {
    assignObject: (file: RawValue, options: RawValue) => RawValue;
    unassignObject: (file: RawValue, options: RawValue) => void;
  };
  attribute: {
    editAttributes: (file: RawValue, options: RawValue) => void;
  };
  boundary: {
    assignConnectionGeometry: (file: RawValue, rel_space_boundary: RawValue, options: RawValue) => void;
    copyBoundary: (file: RawValue, boundary: RawValue) => RawValue;
    editAttributes: (entity: RawValue, options: RawValue) => void;
    removeBoundary: (file: RawValue, boundary: RawValue) => void;
  };
  classification: {
    addClassification: (file: RawValue, name: string) => RawValue;
    addReference: (file: RawValue, options: RawValue) => RawValue;
    getReferences: (element: RawValue, should_inherit: boolean) => RawValue;
    removeClassification: (file: RawValue, classification: RawValue) => void;
    removeReference: (file: RawValue, options: RawValue) => void;
  };
  cogo: {
    addSurveyPoint: (file: RawValue, options: RawValue) => RawValue;
    assignSurveyPoint: (annotation: RawValue, survey_point: RawValue) => void;
    editSurveyPoint: (annotation: RawValue, x: number, y: number, z: number) => void;
  };
  compute: {
    derived: (instance: RawValue, attribute_name: string) => RawValue;
  };
  constraint: {
    addMetric: (file: RawValue, objective: RawValue) => RawValue;
    addMetricReference: (file: RawValue, metric: RawValue, reference_path: string) => RawValue;
    addObjective: (file: RawValue) => RawValue;
    assignConstraint: (file: RawValue, options: RawValue) => RawValue;
    removeConstraint: (file: RawValue, constraint: RawValue) => void;
    removeMetric: (file: RawValue, metric: RawValue) => void;
    unassignConstraint: (file: RawValue, options: RawValue) => void;
  };
  context: {
    addContext: (file: RawValue, options: RawValue) => RawValue;
    editContext: (file: RawValue, context: RawValue, attributes: RawValue) => void;
    removeContext: (file: RawValue, context: RawValue) => void;
  };
  control: {
    assignControl: (file: RawValue, options: RawValue) => RawValue;
    unassignControl: (file: RawValue, options: RawValue) => void;
  };
  cost: {
    addCostItem: (file: RawValue, options: RawValue) => RawValue;
    addCostItemQuantity: (file: RawValue, cost_item: RawValue, ifc_class: string) => RawValue;
    addCostSchedule: (file: RawValue, name: string | null, predefined_type: string, update_date: string, owner_history: RawValue) => RawValue;
    addCostValue: (file: RawValue, parent: RawValue) => RawValue;
    assignCostItemQuantity: (file: RawValue, cost_item: RawValue, products: RawValue, prop_name: string | null, options: RawValue) => void;
    assignCostValue: (file: RawValue, cost_item: RawValue, cost_rate: RawValue) => void;
    calculateCostItemResourceValue: (file: RawValue, cost_item: RawValue) => void;
    copyCostItem: (file: RawValue, cost_item: RawValue) => RawValue;
    copyCostItemValues: (file: RawValue, source: RawValue, destination: RawValue) => void;
    copyCostSchedule: (file: RawValue, cost_schedule: RawValue, options: RawValue) => RawValue;
    editCostItem: (file: RawValue, cost_item: RawValue, attributes: RawValue) => void;
    editCostItemQuantity: (file: RawValue, physical_quantity: RawValue, attributes: RawValue) => void;
    editCostSchedule: (file: RawValue, cost_schedule: RawValue, attributes: RawValue) => void;
    editCostValue: (file: RawValue, cost_value: RawValue, attributes: RawValue, options: RawValue) => void;
    editCostValueFormula: (file: RawValue, cost_value: RawValue, formula: string) => void;
    removeCostItem: (file: RawValue, cost_item: RawValue) => void;
    removeCostItemQuantity: (file: RawValue, cost_item: RawValue, physical_quantity: RawValue) => void;
    removeCostSchedule: (file: RawValue, cost_schedule: RawValue) => void;
    removeCostValue: (file: RawValue, parent: RawValue, cost_value: RawValue) => void;
    unassignCostItemQuantity: (file: RawValue, cost_item: RawValue, products: RawValue, options: RawValue) => void;
  };
  document: {
    addInformation: (file: RawValue, options: RawValue) => RawValue;
    addReference: (file: RawValue, information: RawValue) => RawValue;
    assignDocument: (file: RawValue, options: RawValue) => RawValue;
    removeInformation: (file: RawValue, information: RawValue) => void;
    removeReference: (file: RawValue, reference: RawValue) => void;
    unassignDocument: (file: RawValue, options: RawValue) => void;
  };
  drawing: {
    assignProduct: (file: RawValue, options: RawValue) => RawValue;
    unassignProduct: (file: RawValue, options: RawValue) => void;
  };
  element: {
    getAggregate: (instance: RawValue) => RawValue;
    getContained: (element: RawValue) => RawValue;
    getContainer: (instance: RawValue, options: RawValue) => RawValue;
    getControls: (element: RawValue) => RawValue;
    getDecomposition: (element: RawValue, options: RawValue) => RawValue;
    getElementsByLayer: (layer: RawValue) => RawValue;
    getElementsByMaterial: (material: RawValue) => RawValue;
    getElementsByProfile: (profile: RawValue) => RawValue;
    getElementsByRepresentation: (representation: RawValue) => RawValue;
    getElementsByStyle: (style: RawValue) => RawValue;
    getFilledVoid: (element: RawValue) => RawValue;
    getGroups: (element: RawValue) => RawValue;
    getLayers: (element: RawValue) => RawValue;
    getMaterial: (instance: RawValue, options: RawValue) => RawValue;
    getNest: (instance: RawValue) => RawValue;
    getOpenings: (element: RawValue) => RawValue;
    getParent: (instance: RawValue) => RawValue;
    getParts: (element: RawValue) => RawValue;
    getPsetIds: (element: RawValue, options: RawValue) => RawValue;
    getReferencedElements: (reference: RawValue) => RawValue;
    getReferencedStructures: (element: RawValue) => RawValue;
    getShapeAspects: (element: RawValue, options: RawValue) => RawValue;
    getStructureReferencedElements: (structure: RawValue) => RawValue;
    getStyles: (element: RawValue) => RawValue;
    getType: (instance: RawValue) => RawValue;
    getTypes: (type_element: RawValue) => RawValue;
    getVoidedElement: (element: RawValue) => RawValue;
    isUserdefinedType: (element: RawValue) => boolean;
    removeDeep: (element: RawValue) => void;
    replaceElement: (old_element: RawValue, new_element: RawValue) => void;
  };
  entity: {
    removeDeep: (instance: RawValue) => void;
    removeDeepWithOptions: (instance: RawValue, options: RawValue) => void;
  };
  feature: {
    addFeature: (file: RawValue, options: RawValue) => RawValue;
    addFilling: (file: RawValue, opening: RawValue, element: RawValue) => RawValue;
    removeFeature: (file: RawValue, options: RawValue) => void;
    removeFilling: (file: RawValue, element: RawValue) => void;
  };
  geometry: {
    addAxisRepresentation: (file: RawValue, context: RawValue, axis: RawValue) => RawValue;
    addBoolean: (file: RawValue, first_item: RawValue, second_items: RawValue, operator_type: string) => RawValue;
    addDoorRepresentation: (file: RawValue, options: RawValue) => RawValue;
    addFootprintRepresentation: (file: RawValue, context: RawValue, curves: RawValue) => RawValue;
    addMeshRepresentation: (file: RawValue, context: RawValue, options: RawValue) => RawValue;
    addProfileRepresentation: (file: RawValue, options: RawValue) => RawValue;
    addRailingRepresentation: (file: RawValue, options: RawValue) => RawValue;
    addShapeAspect: (file: RawValue, options: RawValue) => RawValue;
    addSlabRepresentation: (file: RawValue, options: RawValue) => RawValue;
    addTopologyRepresentation: (file: RawValue, options: RawValue) => RawValue;
    addWallRepresentation: (file: RawValue, options: RawValue) => RawValue;
    addWindowRepresentation: (file: RawValue, options: RawValue) => RawValue;
    assignRepresentation: (file: RawValue, product: RawValue, representation: RawValue) => RawValue;
    clipSolid: (file: RawValue, options: RawValue) => RawValue;
    clipSolidBounded: (file: RawValue, options: RawValue) => RawValue;
    connectElement: (file: RawValue, options: RawValue) => RawValue;
    connectPath: (file: RawValue, options: RawValue) => RawValue;
    connectWall: (file: RawValue, options: RawValue) => RawValue;
    copyRepresentation: (file: RawValue, options: RawValue) => RawValue;
    create2ptWall: (file: RawValue, options: RawValue) => RawValue;
    disconnectElement: (file: RawValue, relating_element: RawValue, related_element: RawValue) => void;
    disconnectPath: (file: RawValue, options: RawValue) => void;
    editObjectPlacement: (file: RawValue, options: RawValue) => RawValue;
    mapRepresentation: (file: RawValue, representation: RawValue) => RawValue;
    profileExtents: (file: RawValue, profile: RawValue) => RawValue;
    regenerateWallRepresentation: (file: RawValue, options: RawValue) => RawValue;
    removeBoolean: (file: RawValue, item: RawValue) => void;
    removeRepresentation: (file: RawValue, representation: RawValue, options: RawValue) => void;
    unassignRepresentation: (file: RawValue, product: RawValue, representation: RawValue) => void;
    validateType: (file: RawValue, representation: RawValue, options: RawValue) => boolean;
  };
  georeference: {
    addGeoreferencing: (file: RawValue, options: RawValue) => void;
    editGeoreferencing: (file: RawValue, options: RawValue) => void;
    editTrueNorth: (file: RawValue, options: RawValue) => void;
    editWcs: (file: RawValue, options: RawValue) => void;
    removeGeoreferencing: (file: RawValue) => void;
  };
  grid: {
    createAxisCurve: (file: RawValue, p1: RawValue, p2: RawValue, grid_axis: RawValue, is_si: boolean) => void;
    createGridAxis: (file: RawValue, grid: RawValue, axis_tag: string, same_sense: boolean, uvw_axes: string) => RawValue;
    removeGridAxis: (file: RawValue, axis: RawValue) => void;
  };
  group: {
    addGroup: (file: RawValue, options: RawValue) => RawValue;
    assignGroup: (file: RawValue, options: RawValue) => RawValue;
    removeGroup: (file: RawValue, group: RawValue) => void;
    unassignGroup: (file: RawValue, options: RawValue) => void;
    updateGroupProducts: (file: RawValue, options: RawValue) => RawValue;
  };
  guid: {
    compress: (uuid_hex: string) => string;
    expand: (guid: string) => string;
    generate: () => string;
  };
  layer: {
    addLayer: (file: RawValue, name: string) => RawValue;
    addLayerWithStyle: (file: RawValue, name: string, options: RawValue) => RawValue;
    assignLayer: (file: RawValue, items: RawValue, layer: RawValue) => void;
    removeLayer: (file: RawValue, layer: RawValue) => void;
    unassignLayer: (file: RawValue, items: RawValue, layer: RawValue) => void;
  };
  library: {
    addLibrary: (file: RawValue, name: string) => RawValue;
    addReference: (file: RawValue, library: RawValue) => RawValue;
    assignReference: (file: RawValue, options: RawValue) => RawValue;
    removeLibrary: (file: RawValue, library: RawValue) => void;
    removeReference: (file: RawValue, reference: RawValue) => void;
    unassignReference: (file: RawValue, options: RawValue) => void;
  };
  material: {
    addConstituent: (file: RawValue, constituent_set: RawValue, options: RawValue) => RawValue;
    addLayer: (file: RawValue, layer_set: RawValue, options: RawValue) => RawValue;
    addListItem: (file: RawValue, material_list: RawValue, material: RawValue) => void;
    addMaterial: (file: RawValue, options: RawValue) => RawValue;
    addMaterialSet: (file: RawValue, options: RawValue) => RawValue;
    addProfile: (file: RawValue, profile_set: RawValue, options: RawValue) => RawValue;
    assignMaterial: (file: RawValue, products: RawValue, options: RawValue) => RawValue;
    assignProfile: (file: RawValue, material_profile: RawValue, profile: RawValue) => void;
    editProfileUsage: (file: RawValue, usage: RawValue, options: RawValue) => void;
    removeConstituent: (file: RawValue, constituent: RawValue, options: RawValue) => void;
    removeLayer: (file: RawValue, layer: RawValue, options: RawValue) => void;
    removeListItem: (file: RawValue, material_list: RawValue, options: RawValue) => void;
    removeMaterial: (file: RawValue, material: RawValue) => void;
    removeMaterialSet: (file: RawValue, material: RawValue) => void;
    removeProfile: (file: RawValue, profile: RawValue, options: RawValue) => void;
    reorderSetItem: (file: RawValue, material_set: RawValue, options: RawValue) => void;
    unassignMaterial: (file: RawValue, products: RawValue, options: RawValue) => void;
  };
  nest: {
    assignObject: (file: RawValue, options: RawValue) => RawValue;
    unassignObject: (file: RawValue, options: RawValue) => void;
  };
  owner: {
    addActor: (file: RawValue, options: RawValue) => RawValue;
    addAddress: (file: RawValue, assigned_object: RawValue, ifc_class: string) => RawValue;
    addApplication: (file: RawValue, options: RawValue) => RawValue;
    addOrganisation: (file: RawValue, identification: string, name: string) => RawValue;
    addPerson: (file: RawValue, identification: string, family_name: string, given_name: string) => RawValue;
    addPersonAndOrganisation: (file: RawValue, person: RawValue, organisation: RawValue) => RawValue;
    addRole: (file: RawValue, assigned_object: RawValue, role: string) => RawValue;
    assignActor: (file: RawValue, options: RawValue) => RawValue;
    createOwnerHistory: (file: RawValue, options: RawValue) => RawValue;
    removeActor: (file: RawValue, actor: RawValue) => void;
    removeAddress: (file: RawValue, address: RawValue) => void;
    removeApplication: (file: RawValue, application: RawValue) => void;
    removeOrganisation: (file: RawValue, organisation: RawValue) => void;
    removePerson: (file: RawValue, person: RawValue) => void;
    removePersonAndOrganisation: (file: RawValue, person_and_organisation: RawValue) => void;
    removeRole: (file: RawValue, role: RawValue) => void;
    unassignActor: (file: RawValue, options: RawValue) => void;
    updateOwnerHistory: (file: RawValue, options: RawValue) => RawValue;
  };
  placement: {
    getAxis2Placement: (instance: RawValue) => RawValue;
    getCartesianXform3d: (instance: RawValue) => RawValue;
    getLocalPlacement: (instance: RawValue) => RawValue;
    getMappeditemXform: (instance: RawValue) => RawValue;
    getStoreyElevation: (instance: RawValue) => number;
    matrixFromAxes: (origin: RawValue, z_axis: RawValue, x_axis: RawValue) => RawValue;
    rotation: (angle_rad: number, axis: string) => RawValue;
  };
  profile: {
    addArbitraryProfile: (file: RawValue, options: RawValue) => RawValue;
    addArbitraryProfileWithVoids: (file: RawValue, options: RawValue) => RawValue;
    addParameterizedProfile: (file: RawValue, ifc_class: string, profile_type: string) => RawValue;
    copyProfile: (file: RawValue, profile: RawValue) => RawValue;
    editProfile: (profile: RawValue, attributes: RawValue) => void;
    removeProfile: (file: RawValue, profile: RawValue) => void;
  };
  project: {
    assignDeclaration: (file: RawValue, options: RawValue) => RawValue;
    unassignDeclaration: (file: RawValue, options: RawValue) => void;
  };
  pset: {
    addPset: (file: RawValue, options: RawValue) => RawValue;
    addQto: (file: RawValue, options: RawValue) => RawValue;
    assignPset: (file: RawValue, options: RawValue) => RawValue;
    editPset: (file: RawValue, options: RawValue) => boolean;
    editQto: (file: RawValue, options: RawValue) => boolean;
    removePset: (file: RawValue, product: RawValue, pset: RawValue) => void;
    templateAddPropTemplate: (file: RawValue, pset_template: RawValue, name: string, description: string | null, template_type: string | null, primary_measure_type: string | null) => RawValue;
    templateAddPsetTemplate: (file: RawValue, name: string, template_type: string, applicable_entity: string) => RawValue;
    templateCreateFromFiles: (schema_identifier: string, template_files: RawValue) => RawValue;
    templateGetApplicable: (pqt: RawValue, ifc_class: string | null, predefined_type: string | null, pset_only: boolean, qto_only: boolean, schema_name: string | null) => RawValue;
    templateGetApplicableNames: (pqt: RawValue, ifc_class: string | null, predefined_type: string | null, pset_only: boolean, qto_only: boolean, schema_name: string | null) => RawValue;
    templateGetByName: (pqt: RawValue, name: string) => RawValue;
    templateGetTemplate: (schema_identifier: string) => RawValue;
    templateIsTemplated: (pqt: RawValue, name: string) => boolean;
    templatePsetType: (pset_template: RawValue) => string;
    templateRemovePropTemplate: (file: RawValue, prop_template: RawValue) => void;
    templateRemovePsetTemplate: (pset_template: RawValue) => void;
    templateSetTemplateDir: (dir: string) => void;
    unassignPset: (file: RawValue, products: RawValue, pset: RawValue) => void;
    unsharePset: (file: RawValue, options: RawValue) => RawValue;
  };
  register: {
    scratchFile: (schema_name: string | null, file: RawValue) => boolean;
  };
  representation: {
    getContext: (file: RawValue, context_type: string | null, subcontext: string | null, target_view: string | null) => RawValue;
    getPrioritisedContexts: (file: RawValue) => RawValue;
    getProductRepresentation: (element: RawValue, options: RawValue) => RawValue;
    resolve: (representation: RawValue) => RawValue;
    resolveBaseItems: (representation: RawValue) => RawValue;
  };
  resource: {
    editResourceTime: (file: RawValue, resource_time: RawValue, attributes: RawValue) => void;
  };
  root: {
    createEntity: (file: RawValue, options: RawValue) => RawValue;
    removeProduct: (file: RawValue, product: RawValue, options: RawValue) => void;
  };
  schema: {
    reassignClass: (file: RawValue, element: RawValue, new_class: string) => RawValue;
  };
  selector: {
    filterAll: (file: RawValue, query: string) => RawValue;
    filterElements: (file: RawValue, query: string, elements: RawValue) => RawValue;
    format: (file: RawValue, instance: RawValue, query: string) => string | null;
    getElementValue: (file: RawValue, element: RawValue, query: string) => RawValue;
    setElementValue: (file: RawValue, element: RawValue, query: string, value: RawValue, concat: string | null) => void;
  };
  sequence: {
    addDateTime: (file: RawValue, date_time: string) => RawValue;
    addTask: (file: RawValue, options: RawValue) => RawValue;
    addTaskTime: (file: RawValue, task: RawValue, options: RawValue) => RawValue;
    addTimePeriod: (file: RawValue, recurrence_pattern: RawValue, options: RawValue) => RawValue;
    addWorkCalendar: (file: RawValue, options: RawValue) => RawValue;
    addWorkPlan: (file: RawValue, options: RawValue) => RawValue;
    addWorkSchedule: (file: RawValue, options: RawValue) => RawValue;
    addWorkTime: (file: RawValue, work_calendar: RawValue, time_type: string) => RawValue;
    assignLagTime: (file: RawValue, rel_sequence: RawValue, lag_value: string, options: RawValue) => RawValue;
    assignProcess: (file: RawValue, relating_process: RawValue, related_object: RawValue, options: RawValue) => RawValue;
    assignProduct: (file: RawValue, relating_product: RawValue, related_object: RawValue, options: RawValue) => RawValue;
    assignRecurrencePattern: (file: RawValue, parent: RawValue, recurrence_type: string) => RawValue;
    assignSequence: (file: RawValue, relating_process: RawValue, related_process: RawValue, options: RawValue) => RawValue;
    assignWorkPlan: (file: RawValue, work_schedule: RawValue, work_plan: RawValue, options: RawValue) => RawValue;
    calculateTaskDuration: (file: RawValue, task: RawValue) => void;
    cascadeSchedule: (file: RawValue, task: RawValue) => void;
    copyWorkSchedule: (file: RawValue, work_schedule: RawValue, options: RawValue) => RawValue;
    createBaseline: (file: RawValue, work_schedule: RawValue, options: RawValue) => void;
    duplicateTask: (file: RawValue, task: RawValue, options: RawValue) => RawValue;
    editLagTime: (lag_time: RawValue, attributes: RawValue) => void;
    editRecurrencePattern: (recurrence_pattern: RawValue, attributes: RawValue) => void;
    editSequence: (rel_sequence: RawValue, attributes: RawValue) => void;
    editTask: (task: RawValue, attributes: RawValue) => void;
    editTaskTime: (file: RawValue, task_time: RawValue, attributes: RawValue) => void;
    editWorkCalendar: (work_calendar: RawValue, attributes: RawValue) => void;
    editWorkPlan: (work_plan: RawValue, attributes: RawValue) => void;
    editWorkSchedule: (work_schedule: RawValue, attributes: RawValue) => void;
    editWorkTime: (work_time: RawValue, attributes: RawValue) => void;
    recalculateSchedule: (file: RawValue, work_schedule: RawValue) => void;
    removeTask: (file: RawValue, task: RawValue, options: RawValue) => void;
    removeTimePeriod: (file: RawValue, time_period: RawValue) => void;
    removeWorkCalendar: (file: RawValue, work_calendar: RawValue, options: RawValue) => void;
    removeWorkPlan: (file: RawValue, work_plan: RawValue, options: RawValue) => void;
    removeWorkSchedule: (file: RawValue, work_schedule: RawValue, options: RawValue) => void;
    removeWorkTime: (file: RawValue, work_time: RawValue) => void;
    unassignLagTime: (file: RawValue, rel_sequence: RawValue) => void;
    unassignProcess: (file: RawValue, relating_process: RawValue, related_object: RawValue, options: RawValue) => RawValue;
    unassignProduct: (file: RawValue, relating_product: RawValue, related_object: RawValue, options: RawValue) => RawValue;
    unassignRecurrencePattern: (file: RawValue, recurrence_pattern: RawValue) => void;
    unassignSequence: (file: RawValue, relating_process: RawValue, related_process: RawValue) => void;
  };
  shape: {
    builderAxis2Placement2d: (file: RawValue, options: RawValue) => RawValue;
    builderAxis2Placement3d: (file: RawValue, options: RawValue) => RawValue;
    builderBlock: (file: RawValue, options: RawValue) => RawValue;
    builderCircle: (file: RawValue, center: RawValue, radius: number) => RawValue;
    builderCurveBetweenTwoPoints: (file: RawValue, points: RawValue) => RawValue;
    builderDeepCopy: (file: RawValue, element: RawValue) => RawValue;
    builderEdge: (file: RawValue, start: RawValue, end: RawValue) => RawValue;
    builderEllipseCurve: (file: RawValue, options: RawValue) => RawValue;
    builderExtrude: (file: RawValue, options: RawValue) => RawValue;
    builderFace: (file: RawValue, points: RawValue) => RawValue;
    builderFacetedBrep: (file: RawValue, points: RawValue, faces: RawValue) => RawValue;
    builderGetPolylineCoords: (polyline: RawValue) => RawValue;
    builderHalfSpaceSolid: (file: RawValue, options: RawValue) => RawValue;
    builderIndexedPolycurve2d: (file: RawValue, points: RawValue, segments: RawValue) => RawValue;
    builderMepBendShape: (file: RawValue, options: RawValue) => RawValue;
    builderMepTransitionCalculate: (options: RawValue) => number;
    builderMepTransitionLength: (options: RawValue) => number;
    builderMepTransitionShape: (file: RawValue, options: RawValue) => RawValue;
    builderMesh: (file: RawValue, points: RawValue, faces: RawValue) => RawValue;
    builderMirror: (file: RawValue, options: RawValue) => RawValue;
    builderPlane: (file: RawValue, location: RawValue, normal: RawValue) => RawValue;
    builderPolygonalFaceSet: (file: RawValue, points: RawValue, faces: RawValue) => RawValue;
    builderPolyline: (file: RawValue, options: RawValue) => RawValue;
    builderProfile: (file: RawValue, options: RawValue) => RawValue;
    builderRepresentation: (file: RawValue, options: RawValue) => RawValue;
    builderRotate: (file: RawValue, options: RawValue) => RawValue;
    builderSetPolylineCoords: (file: RawValue, polyline: RawValue, coords: RawValue) => RawValue;
    builderSphere: (file: RawValue, options: RawValue) => RawValue;
    builderSweptDiskSolid: (file: RawValue, path_curve: RawValue, radius: number) => RawValue;
    builderTranslate: (file: RawValue, options: RawValue) => RawValue;
    builderTriangulatedFaceSet: (file: RawValue, points: RawValue, faces: RawValue) => RawValue;
    builderVertex: (file: RawValue, position: RawValue) => RawValue;
    isAlmostEqual: (value: number, x: number, tolerance: number) => boolean;
  };
  spatial: {
    assignContainer: (file: RawValue, options: RawValue) => RawValue;
    dereferenceStructure: (file: RawValue, options: RawValue) => void;
    referenceStructure: (file: RawValue, options: RawValue) => RawValue;
    unassignContainer: (file: RawValue, options: RawValue) => void;
  };
  structural: {
    addStructuralActivity: (file: RawValue, applied_load: RawValue, structural_member: RawValue, ifc_class: string, predefined_type: string, global_or_local: string, options: RawValue) => RawValue;
    addStructuralAnalysisModel: (file: RawValue, owner_history: RawValue) => RawValue;
    addStructuralBoundaryCondition: (file: RawValue, ifc_class: string, options: RawValue) => RawValue;
    addStructuralLoad: (file: RawValue, ifc_class: string, name: string | null) => RawValue;
    addStructuralLoadCase: (file: RawValue, name: string, action_type: string, action_source: string, owner_history: RawValue) => RawValue;
    addStructuralLoadGroup: (file: RawValue, name: string, action_type: string, action_source: string, owner_history: RawValue) => RawValue;
    addStructuralMemberConnection: (file: RawValue, relating_structural_member: RawValue, related_structural_connection: RawValue, owner_history: RawValue) => RawValue;
    assignProduct: (file: RawValue, relating_product: RawValue, related_object: RawValue, owner_history: RawValue) => RawValue;
    assignStructuralAnalysisModel: (file: RawValue, products: RawValue, structural_analysis_model: RawValue, options: RawValue) => RawValue;
    assignToBuilding: (file: RawValue, structural_analysis_model: RawValue, building: RawValue, owner_history: RawValue) => RawValue;
    editStructuralBoundaryCondition: (file: RawValue, condition: RawValue, attributes: RawValue) => void;
    editStructuralConnectionCs: (file: RawValue, structural_item: RawValue, axis: RawValue, ref_direction: RawValue) => void;
    editStructuralItemAxis: (file: RawValue, structural_item: RawValue, axis: RawValue) => void;
    removeStructuralAnalysisModel: (file: RawValue, structural_analysis_model: RawValue) => void;
    removeStructuralBoundaryCondition: (file: RawValue, options: RawValue) => void;
    removeStructuralConnectionCondition: (file: RawValue, relation: RawValue) => void;
    removeStructuralLoad: (file: RawValue, structural_load: RawValue) => void;
    removeStructuralLoadCase: (file: RawValue, structural_load_case: RawValue) => void;
    removeStructuralLoadGroup: (file: RawValue, structural_load_group: RawValue) => void;
    unassignStructuralAnalysisModel: (file: RawValue, products: RawValue, structural_analysis_model: RawValue, options: RawValue) => void;
  };
  style: {
    addStyle: (file: RawValue, name: string | null, ifc_class: string) => RawValue;
    assignItemStyle: (file: RawValue, options: RawValue) => RawValue;
    assignMaterialStyle: (file: RawValue, material: RawValue, style: RawValue, context: RawValue, should_use_presentation_style_assignment: boolean) => void;
    assignRepresentationStyles: (file: RawValue, shape_representation: RawValue, styles: RawValue, should_use_presentation_style_assignment: boolean, replace_previous_same_type_style: boolean) => RawValue;
    editSurfaceStyle: (file: RawValue, style: RawValue, attributes: RawValue) => void;
    removeStyle: (file: RawValue, style: RawValue) => void;
    removeStyledRepresentation: (file: RawValue, representation: RawValue) => void;
    removeSurfaceStyle: (file: RawValue, style: RawValue) => void;
    unassignMaterialStyle: (file: RawValue, material: RawValue, style: RawValue, context: RawValue) => void;
    unassignRepresentationStyles: (file: RawValue, shape_representation: RawValue, styles: RawValue, should_use_presentation_style_assignment: boolean) => void;
  };
  system: {
    addPort: (file: RawValue, options: RawValue) => RawValue;
    addSystem: (file: RawValue, options: RawValue) => RawValue;
    assignFlowControl: (file: RawValue, options: RawValue) => RawValue;
    assignPort: (file: RawValue, options: RawValue) => RawValue;
    assignSystem: (file: RawValue, options: RawValue) => RawValue;
    connectPort: (file: RawValue, options: RawValue) => void;
    disconnectPort: (file: RawValue, port: RawValue) => void;
    removeSystem: (file: RawValue, system: RawValue) => void;
    unassignFlowControl: (file: RawValue, options: RawValue) => void;
    unassignPort: (file: RawValue, options: RawValue) => void;
    unassignSystem: (file: RawValue, options: RawValue) => void;
  };
  type: {
    assignType: (file: RawValue, options: RawValue) => RawValue;
    mapTypeRepresentations: (file: RawValue, related_object: RawValue, relating_type: RawValue) => boolean;
    unassignType: (file: RawValue, options: RawValue) => void;
  };
  unit: {
    addContextDependentUnit: (file: RawValue, unit_type: string, name: string, dimensions: RawValue) => RawValue;
    addDerivedUnit: (file: RawValue, unit_type: string, userdefinedtype: string | null, units: RawValue, exponents: RawValue) => RawValue;
    addMonetaryUnit: (file: RawValue, currency: string) => RawValue;
    addSiUnit: (file: RawValue, unit_type: string, prefix: string | null) => RawValue;
    calculateUnitScale: (file: RawValue, unit_type: string) => number;
    convert: (value: number, from_prefix: string, from_unit: string, to_prefix: string, to_unit: string) => number;
    convertUnit: (value: number, from_unit: RawValue, to_unit: RawValue) => number;
    formatLength: (value: number, precision: number, decimal_places: number, suppress_zero_inches: boolean, unit_system: string, input_unit: string, output_unit: string) => string;
    getFullUnitName: (unit: RawValue) => string;
    getMeasureClass: (unit_type: string) => string;
    getMeasureUnitType: (measure_class: string) => string;
    getNamedDimensions: (name: string) => RawValue;
    getPrefix: (text: string) => string;
    getPrefixMultiplier: (text: string) => number;
    getProjectUnit: (file: RawValue, unit_type: string) => RawValue;
    getSiDimensions: (name: string) => RawValue;
    getSymbolMeasureClass: (symbol: string) => string;
    getSymbolQuantityClass: (symbol: string) => string;
    getUnitAssignment: (file: RawValue) => RawValue;
    getUnitName: (text: string) => string;
    getUnitNameUniversal: (text: string) => string;
    getUnitSymbol: (unit: RawValue) => string;
    removeUnit: (file: RawValue, unit: RawValue) => void;
    resolvePropertyMeasureClass: (prop: RawValue) => string;
    resolvePropertyTableDefinedMeasureClass: (prop: RawValue) => string;
    resolvePropertyTableDefinedUnit: (prop: RawValue) => RawValue;
    resolvePropertyTableDefiningMeasureClass: (prop: RawValue) => string;
    resolvePropertyTableDefiningUnit: (prop: RawValue) => RawValue;
    resolvePropertyUnit: (prop: RawValue) => RawValue;
    unassignUnit: (file: RawValue, units: RawValue) => void;
  };
};

export interface IfcOpenShellSequenceDuplicateTaskResult {
  /** Original tasks in depth-first order. */
  current: Entity[];
  /** Corresponding duplicated tasks in the same order. */
  duplicate: Entity[];
}

export interface IfcOpenShellShapeBuilderMepBendShapeResult {
  /** IfcShapeRepresentation containing the bend geometry. */
  representation: Entity;
  /** Length of the start straight section in model units. */
  startLength: number;
  /** Length of the end straight section in model units. */
  endLength: number;
  /** Bend radius in model units. */
  radius: number;
  /** Bend angle in degrees. */
  angle: number;
  /** Index of the lateral axis (0 = X, 1 = Y). */
  lateralAxis: number;
  /** Sign of the lateral direction (-1.0 or 1.0). */
  lateralSign: number;
  /** Sign of the Z axis direction (-1 or 1). */
  zAxisSign: number;
  /** Dimension of the main profile along the lateral axis. */
  mainProfileDimension: number;
}

export interface IfcOpenShellShapeBuilderMepTransitionShapeResult {
  /** IfcShapeRepresentation containing the transition geometry. */
  representation: Entity;
  /** Length of the start segment in model units. */
  startLength: number;
  /** Length of the end segment in model units. */
  endLength: number;
  /** Transition angle in degrees. */
  angle: number;
  /** XY offset between the two profile centres. */
  profileOffset: number[];
  /** Computed transition length in model units. */
  transitionLength: number;
  /** Total length (start + transition + end) in model units. */
  fullTransitionLength: number;
}

export interface PsetTemplate {
  readonly ptr: number;
  destroy(): void;
}

export interface IfcOpenShellAggregateAssignObjectOptions {
  /** Products to assign as parts of the relating object. */
  products: Entity[];
  /** The parent object that will own the aggregate (e.g. an IfcSite, IfcBuilding, or IfcElementAssembly). */
  relatingObject: Entity;
  /** Optional owner history. A new one is created from user/application if not provided. */
  ownerHistory?: Entity;
  /** Optional user for owner history creation. */
  user?: Entity;
  /** Optional application for owner history creation. */
  application?: Entity;
}

export interface IfcOpenShellAggregateUnassignObjectOptions {
  /** Products to remove from their current aggregate relationships. */
  products: Entity[];
  /** Optional user for owner history updates on modified relationships. */
  user?: Entity;
  /** Optional application for owner history updates on modified relationships. */
  application?: Entity;
}

export interface IfcOpenShellAttributeEditAttributesOptions {
  /** Product whose attributes are being edited. */
  product: Entity;
  /** Property bag of attribute name/value pairs to apply to the product. */
  attributes: PsetProperties | PsetInput;
  /** When true, synchronize PredefinedType based on ElementType or ObjectType. */
  syncPredefinedType: boolean;
  /** When true, update the product's OwnerHistory after modifying attributes. */
  updateOwnerHistory: boolean;
  /** IfcPersonAndOrganization used for OwnerHistory updates. Omit to use the file default. */
  user?: Entity;
  /** IfcApplication used for OwnerHistory updates. Omit to use the file default. */
  application?: Entity;
}

export interface IfcOpenShellBoundaryAssignConnectionGeometryOptions {
  /** Outer boundary of the connection plane, in SI metres and converted to project units using unit_scale. */
  outerBoundary: number[][];
  /** Origin of the connection plane relative to the bounded space, in SI metres and converted to project units using unit_scale. */
  location: number[];
  /** Local axis direction of the connection plane. */
  axis: number[];
  /** Local reference direction of the connection plane. */
  refDirection: number[];
  /** Inner boundaries representing openings in the connection plane, in SI metres and converted to project units using unit_scale. */
  innerBoundaries: number[][][];
  /** Scale that converts model units to SI units. */
  unitScale: number;
}

export interface IfcOpenShellBoundaryEditAttributesOptions {
  /** The space that the boundary relates to. */
  relatingSpace: Entity;
  /** The building element that defines the boundary. */
  relatedBuildingElement: Entity;
  /** Optional parent boundary for inner boundaries. */
  parentBoundary?: Entity;
  /** Optional corresponding boundary on the other side of the element. */
  correspondingBoundary?: Entity;
  /** Physical or virtual enum value. */
  physicalOrVirtual: string;
  /** Internal or external enum value. */
  internalOrExternal: string;
}

export interface IfcOpenShellClassificationAddReferenceOptions {
  /** Products to associate with the classification reference. */
  products: Entity[];
  /** An existing classification reference from a library. If provided, identification/name are ignored. */
  reference?: Entity;
  /** Optional identification code for a new reference (e.g. "Pr_12_23_34"). */
  identification?: string;
  /** Optional human-readable name for a new reference. */
  name?: string;
  /** Optional IfcClassification that the reference belongs to. */
  classification?: Entity;
  /** Optional owner history. A new one is created from user/application if not provided. */
  ownerHistory?: Entity;
  /** Optional user for owner history creation. */
  user?: Entity;
  /** Optional application for owner history creation. */
  application?: Entity;
}

export interface IfcOpenShellClassificationRemoveReferenceOptions {
  /** The classification reference to disassociate. */
  reference: Entity;
  /** Products to disassociate from the reference. */
  products: Entity[];
  /** Optional user for owner history updates on modified relationships. */
  user?: Entity;
  /** Optional application for owner history updates on modified relationships. */
  application?: Entity;
}

export interface IfcOpenShellCogoAddSurveyPointOptions {
  /** IfcPoint (IfcCartesianPoint or IfcPointOnCurve) representing the survey location. */
  surveyPoint: Entity;
  /** IfcSite to contain the annotation. When omitted, the first IfcSite in the file is used. */
  site?: Entity;
  /** Owner history applied to created root entities. When omitted, one is created from user/application. */
  ownerHistory?: Entity;
  /** IfcPersonAndOrganization used to create an IfcOwnerHistory when owner_history is omitted. */
  user?: Entity;
  /** IfcApplication used to create an IfcOwnerHistory when owner_history is omitted. */
  application?: Entity;
}

export interface IfcOpenShellConstraintAssignConstraintOptions {
  /** Products to associate with the constraint. */
  products: Entity[];
  /** The constraint to assign. */
  constraint: Entity;
  /** Optional owner history. A new one is created from user/application if not provided. */
  ownerHistory?: Entity;
  /** Optional user for owner history creation. */
  user?: Entity;
  /** Optional application for owner history creation. */
  application?: Entity;
}

export interface IfcOpenShellConstraintUnassignConstraintOptions {
  /** Products to disassociate from the constraint. */
  products: Entity[];
  /** The constraint to unassign. */
  constraint: Entity;
  /** Optional user for owner history updates on modified relationships. */
  user?: Entity;
  /** Optional application for owner history updates on modified relationships. */
  application?: Entity;
}

export interface IfcOpenShellContextAddContextOptions {
  /** Context type string (e.g. "Plan", "Model", or a custom label). */
  contextType: string;
  /** Context identifier (e.g. "Model View", "Design Transfer View"). Used only for subcontexts. */
  contextIdentifier: string;
  /** Target view for subcontexts (e.g. "MODEL_VIEW", "PLAN_VIEW", "SKETCH_VIEW"). */
  targetView: string;
  /** Optional target scale for subcontexts. Omitted if not set. */
  targetScale?: number;
  /**
   * Optional parent context. If provided, creates an IfcGeometricRepresentationSubContext;
   * if omitted, creates an IfcGeometricRepresentationContext attached to the IfcProject.
   */
  parent?: Entity;
}

export interface IfcOpenShellControlAssignControlOptions {
  /** The planning control (e.g. IfcTask, IfcCostItem) to assign. */
  relatingControl: Entity;
  /** Objects to associate with the control. */
  relatedObjects: Entity[];
  /** Optional owner history. A new one is created from user/application if not provided. */
  ownerHistory?: Entity;
  /** Optional user for owner history creation. */
  user?: Entity;
  /** Optional application for owner history creation. */
  application?: Entity;
}

export interface IfcOpenShellControlUnassignControlOptions {
  /** The planning control to unassign from. */
  relatingControl: Entity;
  /** Objects to disassociate from the control. */
  relatedObjects: Entity[];
  /** Optional user for owner history updates on modified relationships. */
  user?: Entity;
  /** Optional application for owner history updates on modified relationships. */
  application?: Entity;
}

export interface IfcOpenShellCostAddCostItemOptions {
  /** IfcCostSchedule to receive the new IfcCostItem. Takes precedence over cost_item when both are provided. */
  costSchedule?: Entity;
  /** Parent IfcCostItem under which to nest the new item. Used only when cost_schedule is omitted. */
  costItem?: Entity;
  /** Owner history applied to created entities. When omitted, one is created from user/application. */
  ownerHistory?: Entity;
  /** IfcPersonAndOrganization used to create an IfcOwnerHistory when owner_history is omitted. */
  user?: Entity;
  /** IfcApplication used to create an IfcOwnerHistory when owner_history is omitted. */
  application?: Entity;
}

export interface IfcOpenShellCostAssignCostItemQuantityOptions {
  /** Owner history applied to the IfcRelAssignsToControl relationship. When omitted, one is created from user/application. */
  ownerHistory?: Entity;
  /** IfcPersonAndOrganization used to create an IfcOwnerHistory when owner_history is omitted. */
  user?: Entity;
  /** IfcApplication used to create an IfcOwnerHistory when owner_history is omitted. */
  application?: Entity;
}

export interface IfcOpenShellCostCopyCostScheduleOptions {
  /** Owner history applied to the copied schedule and its items. When omitted, one is created from user/application. */
  ownerHistory?: Entity;
  /** IfcPersonAndOrganization used to create an IfcOwnerHistory when owner_history is omitted. */
  user?: Entity;
  /** IfcApplication used to create an IfcOwnerHistory when owner_history is omitted. */
  application?: Entity;
}

export interface IfcOpenShellCostEditCostValueOptions {
  /** When true, the UnitBasis (IfcMeasureWithUnit) is replaced using value_component and unit_component. */
  editUnitBasis: boolean;
  /** When true (with edit_unit_basis), the existing UnitBasis is removed rather than replaced. */
  clearUnitBasis: boolean;
  /** Numeric value for the new UnitBasis measure. Used when edit_unit_basis is true and clear_unit_basis is false. */
  valueComponent: number;
  /** IfcUnit for the new UnitBasis. Required when edit_unit_basis is true and clear_unit_basis is false. */
  unitComponent?: Entity;
}

export interface IfcOpenShellCostUnassignCostItemQuantityOptions {
  /** IfcPersonAndOrganization used to update the relationship OwnerHistory. */
  user?: Entity;
  /** IfcApplication used to update the relationship OwnerHistory. */
  application?: Entity;
}

export interface IfcOpenShellDocumentAddInformationOptions {
  /**
   * Optional parent entity. Can be an IfcProject, IfcContext, or IfcDocumentInformation.
   * If omitted, defaults to the first IfcProject.
   */
  parent?: Entity;
  /** Optional owner history. A new one is created from user/application if not provided. */
  ownerHistory?: Entity;
  /** Optional user for owner history creation. */
  user?: Entity;
  /** Optional application for owner history creation. */
  application?: Entity;
}

export interface IfcOpenShellDocumentAssignDocumentOptions {
  /** Products to associate with the document. */
  products: Entity[];
  /** The IfcDocumentInformation or IfcDocumentReference to assign. */
  document: Entity;
  /** Optional owner history. A new one is created from user/application if not provided. */
  ownerHistory?: Entity;
  /** Optional user for owner history creation. */
  user?: Entity;
  /** Optional application for owner history creation. */
  application?: Entity;
}

export interface IfcOpenShellDocumentUnassignDocumentOptions {
  /** Products to disassociate from the document. */
  products: Entity[];
  /** The IfcDocumentInformation or IfcDocumentReference to unassign. */
  document: Entity;
  /** Optional user for owner history updates on modified relationships. */
  user?: Entity;
  /** Optional application for owner history updates on modified relationships. */
  application?: Entity;
}

export interface IfcOpenShellDrawingAssignProductOptions {
  /** Product to assign (e.g. IfcGridAxis, IfcProduct). */
  relatingProduct: Entity;
  /** Annotation or object receiving the assignment. */
  relatedObject: Entity;
  /** Optional existing IfcOwnerHistory; created from user/application if omitted. */
  ownerHistory?: Entity;
  /** Optional IfcPersonAndOrganization for OwnerHistory creation. */
  user?: Entity;
  /** Optional IfcApplication for OwnerHistory creation. */
  application?: Entity;
}

export interface IfcOpenShellDrawingUnassignProductOptions {
  /** Product to unassign (e.g. IfcGridAxis, IfcProduct). */
  relatingProduct: Entity;
  /** Annotation or object losing the assignment. */
  relatedObject: Entity;
  /** Optional IfcPersonAndOrganization for OwnerHistory update. */
  user?: Entity;
  /** Optional IfcApplication for OwnerHistory update. */
  application?: Entity;
}

export interface IfcOpenShellElementGetContainerOptions {
  /** When true, return only a directly containing spatial element. When false (default), include indirect containers. */
  directOnly?: boolean;
  /** If set, only return a container that is of this IFC class (e.g. "IfcBuildingStorey"). */
  ifcClass?: string;
}

export interface IfcOpenShellElementGetDecompositionOptions {
  /** When true (default), recursively traverse the spatial decomposition tree. When false, return only direct children. */
  isRecursive?: boolean;
}

export interface IfcOpenShellElementGetMaterialOptions {
  /** When true, unwrap IfcMaterialLayerSetUsage and IfcMaterialProfileSetUsage to their underlying set. */
  shouldSkipUsage?: boolean;
  /** When true (default), inherit the material from the element's type if the element has no direct material. */
  shouldInherit?: boolean;
}

export interface IfcOpenShellElementGetPsetIdsOptions {
  /** When true, exclude element quantities and return only property sets. */
  psetsOnly?: boolean;
  /** When true, exclude property sets and return only element quantities. */
  qtosOnly?: boolean;
  /** When true (default), include property sets inherited from the element's type. */
  shouldInherit?: boolean;
}

export interface IfcOpenShellElementGetShapeAspectsOptions {
  /** When true (default), include shape aspects inherited from the element's type. */
  shouldInherit?: boolean;
}

export interface IfcOpenShellEntityRemoveDeepOptions {
  /** Additional entities to include when determining whether references are external to the subgraph. */
  alsoConsider: Entity[];
  /** Entities to preserve even when they belong to the subgraph. */
  doNotDelete: Entity[];
}

export interface IfcOpenShellFeatureAddFeatureOptions {
  /** The feature element (IfcOpeningElement, IfcProjectionElement, IfcSurfaceFeature, etc.). */
  feature: Entity;
  /** The host element that the feature is attached to. */
  element: Entity;
  /** Optional user for owner history creation. */
  user?: Entity;
  /** Optional application for owner history creation. */
  application?: Entity;
}

export interface IfcOpenShellFeatureRemoveFeatureOptions {
  /** The feature element to remove. */
  feature: Entity;
  /** Optional user for owner history updates on modified relationships. */
  user?: Entity;
  /** Optional application for owner history updates on modified relationships. */
  application?: Entity;
}

export interface IfcOpenShellGeometryAddDoorRepresentationOptions {
  /** IfcGeometricRepresentationContext for the representation. */
  context: Entity;
  /** Overall door height in model units. */
  overallHeight: number;
  /** Overall door width in model units. */
  overallWidth: number;
  /** Door operation type (e.g. "SINGLE_SWING_LEFT", "DOUBLE_SWING"). */
  operationType: string;
  /** Lining properties as a flat double array. */
  liningProperties: number[];
  /** Panel properties as a flat double array. */
  panelProperties: number[];
  /** Optional IfcProductDefinitionShape to attach a shape aspect to. */
  partOfProduct?: Entity;
  /** Scale factor from model units to SI metres. Defaults to 1.0. */
  unitScale: number;
}

export interface IfcOpenShellGeometryAddMeshRepresentationOptions {
  /** Vertices for each mesh item. Each point is an XYZ coordinate. */
  vertices: number[][][];
  /** Faces for each mesh item. Each face may contain one outer loop and optional inner loops. */
  faces: number[][][][];
  /** Force faceted BRep output instead of tessellated face sets. Defaults to false. */
  forceFacetedBrep?: boolean;
}

export interface IfcOpenShellGeometryAddProfileRepresentationOptions {
  /** IfcGeometricRepresentationContext for the representation. */
  context: Entity;
  /** IfcProfileDef to extrude. */
  profile: Entity;
  /** Extrusion depth in SI metres. */
  depth: number;
  /** Canonical cardinal-point name; when omitted, the profile origin is used. */
  cardinalPoint?: string;
  /** Optional placement Z axis; defaults to (0, 0, 1) when omitted. */
  placementZAxis?: number[];
  /** Optional placement X axis; defaults to (1, 0, 0) when omitted. */
  placementXAxis?: number[];
  /** Clipping kinds in input order: 0 = plane, 1 = pre-existing entity. */
  clippingKinds: number[];
  /** Plane clipping locations in SI metres, in plane-only order. */
  clippingLocations: number[][];
  /** Plane clipping normals, in the same order as clipping_locations. */
  clippingNormals: number[][];
  /** Pre-existing clipping entities, in entity-only input order; each is copied before use. */
  clippingEntities: Entity[];
}

export interface IfcOpenShellGeometryAddRailingRepresentationOptions {
  /** IfcGeometricRepresentationContext for the representation. */
  context: Entity;
  /** Ordered XYZ points defining the railing path. */
  railingPath: number[][];
  /** If true, use manually placed supports instead of auto-spacing. */
  useManualSupports: boolean;
  /** Spacing between automatic supports in model units. Defaults to 1.0. */
  supportSpacing: number;
  /** Railing tube diameter in model units. Defaults to 0.05. */
  railingDiameter: number;
  /** Clear width between rail elements in model units. Defaults to 0.05. */
  clearWidth: number;
  /** Terminal type string (e.g. "FLAT", "BLOB"). */
  terminalType: string;
  /** Railing height in model units. Defaults to 1.0. */
  height: number;
  /** If true, close the railing path into a loop. Defaults to false. */
  loopedPath: boolean;
  /** Scale factor from model units to SI metres. Defaults to 1.0. */
  unitScale: number;
}

export interface IfcOpenShellGeometryAddShapeAspectOptions {
  /** Shape aspect name. */
  name: string;
  /** Items that belong to the aspect representation. */
  items: Entity[];
  /** Representation containing the aspect items. */
  representation: Entity;
  /** Product definition shape that owns the aspect. */
  partOfProduct: Entity;
  /** Optional shape aspect description. */
  description?: string;
}

export interface IfcOpenShellGeometryAddSlabRepresentationOptions {
  /** IfcGeometricRepresentationContext for the representation. */
  context: Entity;
  /** Slab depth (thickness) in SI metres. */
  depth: number;
  /** Extrusion direction sense: "POSITIVE" or "NEGATIVE". */
  directionSense: string;
  /** Offset from the reference plane along the extrusion direction, in SI metres. */
  offset: number;
  /** Angle of the extrusion direction from vertical, in radians. */
  xAngle: number;
  /** Clipping plane types: 0 = plane, 1 = entity. */
  clippingKinds: number[];
  /** Clipping plane locations in SI metres (one XYZ point per plane-kind clipping). */
  clippingLocations: number[][];
  /** Clipping plane normals (one direction per plane-kind clipping). */
  clippingNormals: number[][];
  /** Pre-existing boolean clipping entities (used for entity-kind clippings). */
  clippingEntities: Entity[];
  /** Optional outer boundary polyline in SI metres. Defaults to a unit square when omitted. */
  polyline?: number[][];
}

export interface IfcOpenShellGeometryAddTopologyRepresentationOptions {
  /** IfcGeometricRepresentationContext for the representation. */
  context: Entity;
  /** Topology item (IfcVertex, IfcEdge, IfcFace, etc.). */
  item: Entity;
  /** Optional representation identifier. Defaults to the context identifier when omitted. */
  representationIdentifier?: string;
  /** Optional representation type (e.g. "Vertex", "Edge"). Auto-detected when omitted. */
  representationType?: string;
}

export interface IfcOpenShellGeometryAddWallRepresentationOptions {
  /** IfcGeometricRepresentationContext for the representation. */
  context: Entity;
  /** Wall length in SI metres. */
  length: number;
  /** Wall height in SI metres. */
  height: number;
  /** Extrusion direction sense: "POSITIVE" or "NEGATIVE". */
  directionSense: string;
  /** Offset from the reference plane along the extrusion direction, in SI metres. */
  offset: number;
  /** Wall thickness in SI metres. */
  thickness: number;
  /** Angle of the extrusion direction from vertical, in radians. */
  xAngle: number;
  /** Clipping plane types: 0 = plane, 1 = entity. */
  clippingKinds: number[];
  /** Clipping plane locations in SI metres (one XYZ point per plane-kind clipping). */
  clippingLocations: number[][];
  /** Clipping plane normals (one direction per plane-kind clipping). */
  clippingNormals: number[][];
  /** Pre-existing boolean clipping entities (used for entity-kind clippings). */
  clippingEntities: Entity[];
  /** Pre-existing boolean operand entities applied before clippings. */
  booleans: Entity[];
}

export interface IfcOpenShellGeometryAddWindowRepresentationOptions {
  /** IfcGeometricRepresentationContext for the representation. */
  context: Entity;
  /** Overall window height in model units. */
  overallHeight: number;
  /** Overall window width in model units. */
  overallWidth: number;
  /** Panel layout schema: each entry is {panel_index, operation_type}. */
  panelSchema: number[][];
  /** Lining properties as a flat double array. */
  liningProperties: number[];
  /** Per-panel properties as arrays of doubles. */
  panelProperties: number[][];
  /** Optional IfcProductDefinitionShape to attach a shape aspect to. */
  partOfProduct?: Entity;
  /** Glass thickness in model units. Defaults to 0.01. */
  glassThickness: number;
}

export interface IfcOpenShellGeometryClipSolidBoundedOptions {
  /** Solid item to clip (first operand). */
  item: Entity;
  /** XYZ point on the clipping plane, in model units. */
  location: number[];
  /** Direction ratios of the clipping plane normal. */
  normal: number[];
  /** XY points defining the polygonal boundary of the clipping region. */
  boundaryPoints: number[][];
  /** XYZ position of the boundary polygon's local origin. */
  boundaryPosition: number[];
  /** Optional owning element for BBIM_Boolean tracking. */
  element?: Entity;
  /** Optional existing IfcOwnerHistory. */
  ownerHistory?: Entity;
  /** Optional IfcPersonAndOrganization for OwnerHistory creation. */
  user?: Entity;
  /** Optional IfcApplication for OwnerHistory creation. */
  application?: Entity;
}

export interface IfcOpenShellGeometryClipSolidOptions {
  /** Solid item to clip (first operand). */
  item: Entity;
  /** XYZ point on the clipping plane, in model units. */
  location: number[];
  /** Direction ratios of the clipping plane normal. */
  normal: number[];
  /** Optional owning element for BBIM_Boolean tracking. */
  element?: Entity;
  /** Optional existing IfcOwnerHistory. */
  ownerHistory?: Entity;
  /** Optional IfcPersonAndOrganization for OwnerHistory creation. */
  user?: Entity;
  /** Optional IfcApplication for OwnerHistory creation. */
  application?: Entity;
}

export interface IfcOpenShellGeometryConnectElementOptions {
  /** The element initiating the connection. */
  relatingElement: Entity;
  /** The element being connected to. */
  relatedElement: Entity;
  /** Optional description of the connection. */
  description?: string;
  /** Optional existing IfcOwnerHistory for the relationship. */
  ownerHistory?: Entity;
  /** Optional IfcPersonAndOrganization for OwnerHistory creation. */
  user?: Entity;
  /** Optional IfcApplication for OwnerHistory creation. */
  application?: Entity;
}

export interface IfcOpenShellGeometryConnectPathOptions {
  /** The element initiating the connection. */
  relatingElement: Entity;
  /** The element being connected to. */
  relatedElement: Entity;
  /** Connection type on the relating side: "ATSTART", "ATEND", or "ATPATH". */
  relatingConnection: string;
  /** Connection type on the related side: "ATSTART", "ATEND", or "ATPATH". */
  relatedConnection: string;
  /** Optional description of the connection. */
  description?: string;
  /** Optional IfcConnectionGeometry for the relationship. */
  connectionGeometry?: Entity;
  /** Optional existing IfcOwnerHistory for the relationship. */
  ownerHistory?: Entity;
  /** Optional IfcPersonAndOrganization for OwnerHistory creation. */
  user?: Entity;
  /** Optional IfcApplication for OwnerHistory creation. */
  application?: Entity;
}

export interface IfcOpenShellGeometryConnectWallOptions {
  /** First wall in the connection. */
  firstWall: Entity;
  /** Second wall in the connection. */
  secondWall: Entity;
  /** If true, connect along the path (ATPATH) instead of at a terminal end. */
  isAtpath: boolean;
  /** Optional existing IfcOwnerHistory for the relationship. */
  ownerHistory?: Entity;
  /** Optional IfcPersonAndOrganization for OwnerHistory creation. */
  user?: Entity;
  /** Optional IfcApplication for OwnerHistory creation. */
  application?: Entity;
}

export interface IfcOpenShellGeometryCopyRepresentationOptions {
  /** Source product to copy the representation from. */
  source: Entity;
  /** Target product to receive the copied representation. */
  target: Entity;
  /** Representation identifier to copy (e.g. "Body"). Defaults to "Body" when omitted. */
  contextIdentifier?: string;
}

export interface IfcOpenShellGeometryCreate2PtWallOptions {
  /** IfcWall (or similar) element to receive the geometry. */
  element: Entity;
  /** IfcGeometricRepresentationContext for the body representation. */
  context: Entity;
  /** XY start point of the wall baseline. */
  start: number[];
  /** XY end point of the wall baseline. */
  end: number[];
  /** Wall base elevation in SI metres (or model units when is_si is false). */
  elevation: number;
  /** Wall height in SI metres (or model units when is_si is false). */
  height: number;
  /** Wall thickness in SI metres (or model units when is_si is false). */
  thickness: number;
  /** If true, start/end/elevation/height/thickness are in SI metres. Defaults to true. */
  isSi: boolean;
}

export interface IfcOpenShellGeometryDisconnectPathOptions {
  /** Element whose connections to remove (used with connection_type). */
  element?: Entity;
  /** Connection type to match (e.g. "ATSTART"). Used with element. */
  connectionType?: string;
  /** Relating element of the specific connection to remove. */
  relatingElement?: Entity;
  /** Related element of the specific connection to remove. */
  relatedElement?: Entity;
}

export interface IfcOpenShellGeometryEditObjectPlacementOptions {
  /** Product whose ObjectPlacement to set. */
  product: Entity;
  /** 16-element row-major 4x4 transformation matrix. */
  matrix: number[];
  /** If true, translation components are in SI metres. Defaults to true. */
  isSi: boolean;
  /** If true, child local placements are left unchanged so children move with the parent. If false (default), child world positions are preserved. */
  shouldTransformChildren: boolean;
}

export interface IfcOpenShellGeometryRegenerateWallRepresentationOptions {
  /** IfcWall entity whose representations to regenerate. */
  wall: Entity;
  /** Wall length in SI metres. */
  length: number;
  /** Wall height in SI metres. */
  height: number;
  /** Optional extrusion angle in radians. Defaults to 0 when omitted. */
  angle?: number;
}

export interface IfcOpenShellGeometryRemoveRepresentationOptions {
  /** If true, IfcProfileDef entities with non-empty ProfileName are preserved. Defaults to true. */
  shouldKeepNamedProfiles?: boolean;
}

export interface IfcOpenShellGeometryValidateTypeOptions {
  /** Preferred first operand for boolean consolidation. Auto-selected when omitted. */
  preferredItem?: Entity;
}

export interface IfcOpenShellGeoreferenceAddGeoreferencingOptions {
  /** IFC class for the coordinate operation, e.g. "IfcMapConversion", "IfcMapConversionScaled", or "IfcRigidOperation". */
  ifcClass: string;
  /** Name for the IfcProjectedCRS. */
  name: string;
  /** Owner history applied to created entities. When omitted, one is created from user/application. */
  ownerHistory?: Entity;
  /** IfcPersonAndOrganization used to create an IfcOwnerHistory when owner_history is omitted. */
  user?: Entity;
  /** IfcApplication used to create an IfcOwnerHistory when owner_history is omitted. */
  application?: Entity;
}

export interface IfcOpenShellGeoreferenceEditGeoreferencingOptions {
  /** Property bag for IfcCoordinateOperation attributes (e.g. Eastings, Northings). Omit to leave it unchanged. */
  coordinateOperation?: PsetProperties | PsetInput;
  /** Property bag for IfcProjectedCRS attributes (e.g. Name, MapZone). Omit to leave it unchanged. */
  projectedCrs?: PsetProperties | PsetInput;
}

export interface IfcOpenShellGeoreferenceEditTrueNorthOptions {
  /** Direction ratios (X, Y) for true north. When omitted, true north is removed. */
  trueNorth?: number[];
}

export interface IfcOpenShellGeoreferenceEditWcsOptions {
  /** X offset in SI metres (or project units when is_si is false). Defaults to 0.0. */
  x?: number;
  /** Y offset in SI metres (or project units when is_si is false). Defaults to 0.0. */
  y?: number;
  /** Z offset in SI metres (or project units when is_si is false). Defaults to 0.0. */
  z?: number;
  /** Rotation of the X axis in degrees. Defaults to 0.0. */
  rotation?: number;
  /** When true (default), x/y/z are in SI metres and are scaled to project units. When false, values are used as-is. */
  isSi?: boolean;
}

export interface IfcOpenShellGroupAddGroupOptions {
  /** Name for the group. */
  name: string;
  /** Optional description. */
  description?: string;
  /** Optional owner history. A new one is created from user/application if not provided. */
  ownerHistory?: Entity;
  /** Optional user for owner history creation. */
  user?: Entity;
  /** Optional application for owner history creation. */
  application?: Entity;
}

export interface IfcOpenShellGroupAssignGroupOptions {
  /** Products to assign to the group. */
  products: Entity[];
  /** The group to assign products to. */
  group: Entity;
  /** Optional owner history. A new one is created from user/application if not provided. */
  ownerHistory?: Entity;
  /** Optional user for owner history creation. */
  user?: Entity;
  /** Optional application for owner history creation. */
  application?: Entity;
}

export interface IfcOpenShellGroupUnassignGroupOptions {
  /** Products to remove from the group. */
  products: Entity[];
  /** The group to remove products from. */
  group: Entity;
  /** Optional user for owner history updates on modified relationships. */
  user?: Entity;
  /** Optional application for owner history updates on modified relationships. */
  application?: Entity;
}

export interface IfcOpenShellGroupUpdateGroupProductsOptions {
  /** The group whose product list is replaced. */
  group: Entity;
  /**
   * New product list. Replaces all existing products. Child IfcGroup entities
   * from the existing relationship are preserved and merged.
   */
  products: Entity[];
  /** Optional owner history. A new one is created from user/application if not provided. */
  ownerHistory?: Entity;
  /** Optional user for owner history creation. */
  user?: Entity;
  /** Optional application for owner history creation. */
  application?: Entity;
}

export interface IfcOpenShellLayerAddLayerWithStyleOptions {
  /** Whether the layer is visible. When omitted, the IFC logical value is UNKNOWN. */
  on?: boolean;
  /** Whether the layer is frozen. When omitted, the IFC logical value is UNKNOWN. */
  frozen?: boolean;
  /** Whether the layer is blocked. When omitted, the IFC logical value is UNKNOWN. */
  blocked?: boolean;
  /** Styles to assign to the layer. */
  styles: Entity[];
}

export interface IfcOpenShellLibraryAssignReferenceOptions {
  /** Products to associate with the library reference. */
  products: Entity[];
  /** The IfcLibraryReference to assign. */
  reference: Entity;
  /** Optional owner history. A new one is created from user/application if not provided. */
  ownerHistory?: Entity;
  /** Optional user for owner history creation. */
  user?: Entity;
  /** Optional application for owner history creation. */
  application?: Entity;
}

export interface IfcOpenShellLibraryUnassignReferenceOptions {
  /** The IfcLibraryReference to unassign. */
  reference: Entity;
  /** Products to disassociate from the reference. */
  products: Entity[];
  /** Optional user for owner history updates on modified relationships. */
  user?: Entity;
  /** Optional application for owner history updates on modified relationships. */
  application?: Entity;
}

export interface IfcOpenShellMaterialAddConstituentOptions {
  /** The IfcMaterial for this constituent. */
  material: Entity;
  /** Optional name for the IfcMaterialConstituent. */
  name?: string;
}

export interface IfcOpenShellMaterialAddLayerOptions {
  /** The IfcMaterial for this layer. */
  material: Entity;
  /** Optional name for the IfcMaterialLayer (ignored in IFC2X3). */
  name?: string;
}

export interface IfcOpenShellMaterialAddMaterialOptions {
  /** Optional material name. Defaults to "Unnamed" if omitted. */
  name?: string;
  /** Optional category (e.g. "Concrete", "Steel"). */
  category?: string;
  /** Optional description. */
  description?: string;
}

export interface IfcOpenShellMaterialAddMaterialSetOptions {
  /** Optional name for the set. Defaults to "Unnamed" if omitted. */
  name?: string;
  /**
   * IFC class of the set to create. Defaults to "IfcMaterialConstituentSet".
   * Valid values: "IfcMaterialConstituentSet", "IfcMaterialLayerSet",
   * "IfcMaterialProfileSet", "IfcMaterialList".
   */
  setType?: string;
}

export interface IfcOpenShellMaterialAddProfileOptions {
  /** Optional IfcMaterial for this profile. Omitted if not set. */
  material?: Entity;
  /** Optional IfcProfileDef for this profile. Omitted if not set. */
  profile?: Entity;
  /** Optional name for the IfcMaterialProfile. */
  name?: string;
}

export interface IfcOpenShellMaterialAssignMaterialOptions {
  /**
   * Material type to assign. Defaults to "IfcMaterial". Valid values:
   * "IfcMaterial", "IfcMaterialConstituentSet", "IfcMaterialLayerSet",
   * "IfcMaterialProfileSet", "IfcMaterialLayerSetUsage",
   * "IfcMaterialProfileSetUsage", "IfcMaterialList".
   */
  type?: string;
  /**
   * Optional existing material or material set to use. For usage types,
   * must be the corresponding set type (e.g. IfcMaterialLayerSet for
   * IfcMaterialLayerSetUsage). If omitted, a new set is created.
   */
  material?: Entity;
  /** Optional owner history. A new one is created from user/application if not provided. */
  ownerHistory?: Entity;
  /** Optional user for owner history creation. */
  user?: Entity;
  /** Optional application for owner history creation. */
  application?: Entity;
}

export interface IfcOpenShellMaterialEditProfileUsageOptions {
  /** Attribute key-value pairs to apply to the usage entity. */
  attributes: PsetProperties | PsetInput;
  /**
   * Profile width in model units, used for cardinal point placement.
   * Required along with profile_height when changing CardinalPoint.
   */
  profileWidth?: number;
  /**
   * Profile height in model units, used for cardinal point placement.
   * Required along with profile_width when changing CardinalPoint.
   */
  profileHeight?: number;
}

export interface IfcOpenShellMaterialRemoveItemOptions {
  /** If true, the associated IfcMaterial is also deleted. Defaults to false. */
  shouldRemoveMaterial?: boolean;
}

export interface IfcOpenShellMaterialRemoveListItemOptions {
  /** Zero-based index of the item to remove. Defaults to 0. */
  materialIndex?: number;
}

export interface IfcOpenShellMaterialRemoveProfileOptions {
  /** If true, the associated IfcProfileDef is also deleted. Defaults to false. */
  shouldRemoveProfileDef?: boolean;
  /** If true, the associated IfcMaterial is also deleted. Defaults to false. */
  shouldRemoveMaterial?: boolean;
}

export interface IfcOpenShellMaterialReorderSetItemOptions {
  /** Zero-based index of the item to move. Defaults to 0. */
  oldIndex?: number;
  /** Zero-based index of the destination position. Defaults to 0. */
  newIndex?: number;
}

export interface IfcOpenShellMaterialUnassignMaterialOptions {
  /** Optional user for owner history updates on modified relationships. */
  user?: Entity;
  /** Optional application for owner history updates on modified relationships. */
  application?: Entity;
}

export interface IfcOpenShellNestAssignObjectOptions {
  /** Objects to nest under the relating object. */
  products: Entity[];
  /** The parent object that will own the nested children. */
  relatingObject: Entity;
  /** Optional owner history. A new one is created from user/application if not provided. */
  ownerHistory?: Entity;
  /** Optional user for owner history creation. */
  user?: Entity;
  /** Optional application for owner history creation. */
  application?: Entity;
}

export interface IfcOpenShellNestUnassignObjectOptions {
  /** Objects to remove from their current nesting relationships. */
  products: Entity[];
  /** Optional user for owner history updates on modified relationships. */
  user?: Entity;
  /** Optional application for owner history updates on modified relationships. */
  application?: Entity;
}

export interface IfcOpenShellOwnerAddActorOptions {
  /** IfcPerson, IfcOrganization, or IfcPersonAndOrganization to act as the actor. */
  actor: Entity;
  /** IFC class name for the actor entity, e.g. "IfcActor" or "IfcOccupant". Defaults to "IfcActor" when empty. */
  ifcClass: string;
  /** Owner history applied to the created entity. When omitted, one is created from user/application. */
  ownerHistory?: Entity;
  /** IfcPersonAndOrganization used to create an IfcOwnerHistory when owner_history is omitted. */
  user?: Entity;
  /** IfcApplication used to create an IfcOwnerHistory when owner_history is omitted. */
  application?: Entity;
}

export interface IfcOpenShellOwnerAddApplicationOptions {
  /** IfcOrganization for the ApplicationDeveloper attribute. When omitted, a default IfcOpenShell organisation is created. */
  applicationDeveloper?: Entity;
  /** Version string for the application. */
  version: string;
  /** Full human-readable name of the application. */
  applicationFullName: string;
  /** Short identifier for the application. */
  applicationIdentifier: string;
  /** Owner history used when creating the default application_developer organisation. */
  ownerHistory?: Entity;
  /** IfcPersonAndOrganization used when creating the default application_developer organisation. */
  user?: Entity;
  /** IfcApplication used when creating the default application_developer organisation. */
  application?: Entity;
}

export interface IfcOpenShellOwnerAssignActorOptions {
  /** IfcActor (or IfcOccupant) that performs the role. */
  relatingActor: Entity;
  /** Object to which the actor is assigned. */
  relatedObject: Entity;
  /** Owner history applied to the IfcRelAssignsToActor relationship. When omitted, one is created from user/application. */
  ownerHistory?: Entity;
  /** IfcPersonAndOrganization used to create an IfcOwnerHistory when owner_history is omitted. */
  user?: Entity;
  /** IfcApplication used to create an IfcOwnerHistory when owner_history is omitted. */
  application?: Entity;
}

export interface IfcOpenShellOwnerCreateOwnerHistoryOptions {
  /** IfcPersonAndOrganization for the OwningUser attribute. Required. */
  user?: Entity;
  /** IfcApplication for the OwningApplication attribute. Required. */
  application?: Entity;
}

export interface IfcOpenShellOwnerUnassignActorOptions {
  /** IfcActor to unassign from the object. */
  relatingActor: Entity;
  /** Object from which to unassign the actor. */
  relatedObject: Entity;
  /** IfcPersonAndOrganization used to update the relationship OwnerHistory. */
  user?: Entity;
  /** IfcApplication used to update the relationship OwnerHistory. */
  application?: Entity;
}

export interface IfcOpenShellOwnerUpdateOwnerHistoryOptions {
  /** IfcRoot element whose OwnerHistory to update. When omitted, no action is taken. */
  element?: Entity;
  /** IfcPersonAndOrganization used to set LastModifyingUser and for creating a new OwnerHistory if absent. */
  user?: Entity;
  /** IfcApplication used to set LastModifyingApplication and for creating a new OwnerHistory if absent. */
  application?: Entity;
}

export interface IfcOpenShellProfileAddArbitraryProfileOptions {
  /** Ordered XYZ or XY points defining the closed outer curve, in SI metres. */
  profile: number[][];
  /** Optional profile name. When omitted, the profile name is empty. */
  name?: string;
}

export interface IfcOpenShellProfileAddArbitraryProfileWithVoidsOptions {
  /** Ordered XYZ or XY points defining the outer curve, in SI metres. */
  outerProfile: number[][];
  /** Inner void curves, each as ordered XY or XYZ points in SI metres. */
  innerProfiles: number[][][];
  /** Optional profile name. When omitted, the profile name is empty. */
  name?: string;
}

export interface IfcOpenShellProjectAssignDeclarationOptions {
  /** Objects to declare to the project or project library. */
  definitions: Entity[];
  /** The IfcProject or IfcProjectLibrary context. */
  relatingContext: Entity;
  /** Optional owner history. A new one is created from user/application if not provided. */
  ownerHistory?: Entity;
  /** Optional user for owner history creation. */
  user?: Entity;
  /** Optional application for owner history creation. */
  application?: Entity;
}

export interface IfcOpenShellProjectUnassignDeclarationOptions {
  /** Objects to remove from the declaration. */
  definitions: Entity[];
  /** The IfcProject or IfcProjectLibrary context. */
  relatingContext: Entity;
  /** Optional user for owner history updates on modified relationships. */
  user?: Entity;
  /** Optional application for owner history updates on modified relationships. */
  application?: Entity;
}

export interface IfcOpenShellPsetAddPsetOptions {
  /**
   * The object to attach the property set to (IfcObject, IfcContext,
   * IfcTypeObject, IfcMaterial, IfcMaterialDefinition, or IfcProfileDef).
   */
  product: Entity;
  /** Name of the property set (e.g. "Pset_WallCommon"). */
  name: string;
  /** Optional owner history. A new one is created from user/application if not provided. */
  ownerHistory?: Entity;
  /** Optional user for owner history creation. */
  user?: Entity;
  /** Optional application for owner history creation. */
  application?: Entity;
  /**
   * Optional IFC2X3 subclass for material/profile property sets (e.g.
   * "IfcExtendedMaterialProperties"). Defaults to "IfcExtendedMaterialProperties"
   * for materials and "IfcGeneralProfileProperties" for profiles.
   */
  ifc2x3Subclass?: string;
}

export interface IfcOpenShellPsetAddQtoOptions {
  /** The object to attach the quantity set to (IfcObject, IfcContext, or IfcTypeObject). */
  product: Entity;
  /** Name of the quantity set (e.g. "BaseQuantities"). */
  name: string;
  /** Optional owner history. A new one is created from user/application if not provided. */
  ownerHistory?: Entity;
  /** Optional user for owner history creation. */
  user?: Entity;
  /** Optional application for owner history creation. */
  application?: Entity;
}

export interface IfcOpenShellPsetAssignPsetOptions {
  /**
   * Products to associate with the property set. IfcTypeProduct instances
   * are added to HasPropertySets; others are linked via IfcRelDefinesByProperties.
   */
  products: Entity[];
  /** The property set to assign. */
  pset: Entity;
  /** Optional owner history. A new one is created from user/application if not provided. */
  ownerHistory?: Entity;
  /** Optional user for owner history creation. */
  user?: Entity;
  /** Optional application for owner history creation. */
  application?: Entity;
}

export interface IfcOpenShellPsetEditPsetOptions {
  /** The IfcPropertySet (or IfcMaterialProperties, IfcProfileProperties, etc.) to edit. */
  pset: Entity;
  /** Optional new name for the property set. */
  name?: string;
  /**
   * Property name-to-value entries to set. Properties not mentioned remain
   * unchanged. Use pset_props_set_null to assign a blank property value.
   */
  properties: PsetProperties | PsetInput;
  /**
   * Optional IfcPropertySetTemplate to use for type inference. If omitted,
   * the template is looked up by the property set's Name attribute.
   */
  psetTemplate?: Entity;
  /**
   * If true (default), blank-valued properties are removed from the set.
   * If false, blank-valued properties retain a blank NominalValue.
   */
  shouldPurge: boolean;
}

export interface IfcOpenShellPsetEditQtoOptions {
  /** The IfcElementQuantity (or IfcPhysicalComplexQuantity) to edit. */
  qto: Entity;
  /** Optional new name for the quantity set. */
  name?: string;
  /**
   * Quantity key-value pairs to set. Keys are quantity names. Scalar values
   * are set as IfcPhysicalSimpleQuantity subtypes (inferred from name/value).
   * Mapping values with a "Discrimination" key create IfcPhysicalComplexQuantity.
   */
  properties: PsetProperties | PsetInput;
  /**
   * Optional IfcPropertySetTemplate to use for quantity type inference.
   * If omitted, the template is looked up by the quantity set's Name attribute.
   */
  qtoTemplate?: Entity;
}

export interface IfcOpenShellPsetUnsharePsetOptions {
  /**
   * Products that should receive their own copy of the property set.
   * Must be a subset of the products currently sharing the pset.
   */
  products: Entity[];
  /** The shared property set to unshare. */
  pset: Entity;
  /** Optional owner history for the new IfcRelDefinesByProperties relationships. */
  ownerHistory?: Entity;
  /** Optional user for owner history creation. */
  user?: Entity;
  /** Optional application for owner history creation. */
  application?: Entity;
}

export interface IfcOpenShellRepresentationGetProductRepresentationOptions {
  /** Specific context entity to match against. When set, overrides context_type, subcontext, and target_view. */
  context?: Entity;
  /** Context type filter (e.g. "Model", "Plan"). Ignored when context is set. */
  contextType?: string;
  /** Subcontext identifier filter (e.g. "Body", "Axis"). Ignored when context is set. */
  subcontext?: string;
  /** Target view filter (e.g. "MODEL_VIEW", "GRAPH_VIEW"). Ignored when context is set. */
  targetView?: string;
}

export interface IfcOpenShellRootCreateEntityOptions {
  /** IFC entity class to instantiate, for example `IfcWall` or `IfcProject`. */
  ifcClass: string;
  /** Optional predefined type. Unsupported enum values are stored as user-defined type labels when possible. */
  predefinedType?: string;
  /** Optional entity name. */
  name?: string;
  /** Optional owner history to assign when the target schema supports it. */
  ownerHistory?: Entity;
}

export interface IfcOpenShellRootRemoveProductOptions {
  /** Optional user for owner history updates on modified relationships. */
  user?: Entity;
  /** Optional application for owner history updates on modified relationships. */
  application?: Entity;
}

export interface IfcOpenShellSequenceAddTaskOptions {
  /** IfcWorkSchedule to assign the task to via IfcRelAssignsToControl. Takes precedence over parent_task when both are provided. */
  workSchedule?: Entity;
  /** Parent IfcTask to nest under via IfcRelNests. Used only when work_schedule is omitted. */
  parentTask?: Entity;
  /** Task name. */
  name?: string;
  /** Task description. */
  description?: string;
  /** Task identification string. For nested tasks, auto-generated as "parent.N" when omitted. */
  identification?: string;
  /** Predefined type enum value. Defaults to "NOTDEFINED". */
  predefinedType?: string;
  /** Owner history applied to created entities. When omitted, one is created from user/application. */
  ownerHistory?: Entity;
  /** IfcPersonAndOrganization used to create an IfcOwnerHistory when owner_history is omitted. */
  user?: Entity;
  /** IfcApplication used to create an IfcOwnerHistory when owner_history is omitted. */
  application?: Entity;
}

export interface IfcOpenShellSequenceAddTaskTimeOptions {
  /** When true, creates an IfcTaskTimeRecurring instead of IfcTaskTime. */
  isRecurring?: boolean;
}

export interface IfcOpenShellSequenceAddTimePeriodOptions {
  /** ISO 8601 time string for the StartTime attribute (e.g. "08:00:00"). */
  startTime?: string;
  /** ISO 8601 time string for the EndTime attribute (e.g. "17:00:00"). */
  endTime?: string;
}

export interface IfcOpenShellSequenceAddWorkCalendarOptions {
  /** Calendar name. Defaults to "Unnamed". */
  name?: string;
  /** Predefined type enum value. Defaults to "NOTDEFINED". */
  predefinedType?: string;
  /** Owner history applied to created entities. When omitted, one is created from user/application. */
  ownerHistory?: Entity;
  /** IfcPersonAndOrganization used to create an IfcOwnerHistory when owner_history is omitted. */
  user?: Entity;
  /** IfcApplication used to create an IfcOwnerHistory when owner_history is omitted. */
  application?: Entity;
}

export interface IfcOpenShellSequenceAddWorkPlanOptions {
  /** Work plan name. */
  name?: string;
  /** Predefined type enum value. Defaults to "NOTDEFINED". */
  predefinedType?: string;
  /** ISO 8601 date-time for CreationDate. Defaults to the current time. */
  creationDate?: string;
  /** ISO 8601 date-time for StartTime. Defaults to creation_date. */
  startTime?: string;
  /** IfcPerson to set as the creator. Appended to the Creators aggregate. */
  creatorPerson?: Entity;
  /** Owner history applied to created entities. When omitted, one is created from user/application. */
  ownerHistory?: Entity;
  /** IfcPersonAndOrganization used to create an IfcOwnerHistory when owner_history is omitted. */
  user?: Entity;
  /** IfcApplication used to create an IfcOwnerHistory when owner_history is omitted. */
  application?: Entity;
}

export interface IfcOpenShellSequenceAddWorkScheduleOptions {
  /** Schedule name. Defaults to "Unnamed". */
  name?: string;
  /** Predefined type enum value. Defaults to "NOTDEFINED". */
  predefinedType?: string;
  /** ObjectType string. */
  objectType?: string;
  /** ISO 8601 date-time for CreationDate. Defaults to the current time. */
  creationDate?: string;
  /** ISO 8601 date-time for StartTime. Defaults to creation_date. */
  startTime?: string;
  /** IfcWorkPlan to nest the schedule under via IfcRelAggregates. */
  workPlan?: Entity;
  /** IfcPerson to set as the creator. Appended to the Creators aggregate. */
  creatorPerson?: Entity;
  /** Owner history applied to created entities. When omitted, one is created from user/application. */
  ownerHistory?: Entity;
  /** IfcPersonAndOrganization used to create an IfcOwnerHistory when owner_history is omitted. */
  user?: Entity;
  /** IfcApplication used to create an IfcOwnerHistory when owner_history is omitted. */
  application?: Entity;
}

export interface IfcOpenShellSequenceAssignLagTimeOptions {
  /** Duration type enum value for the lag. Defaults to "WORKTIME". */
  durationType?: string;
}

export interface IfcOpenShellSequenceAssignProcessOptions {
  /** Owner history applied to the IfcRelAssignsToProcess. When omitted, one is created from user/application. */
  ownerHistory?: Entity;
  /** IfcPersonAndOrganization used to create an IfcOwnerHistory when owner_history is omitted. */
  user?: Entity;
  /** IfcApplication used to create an IfcOwnerHistory when owner_history is omitted. */
  application?: Entity;
}

export interface IfcOpenShellSequenceAssignProductOptions {
  /** Owner history applied to the IfcRelAssignsToProduct. When omitted, one is created from user/application. */
  ownerHistory?: Entity;
  /** IfcPersonAndOrganization used to create an IfcOwnerHistory when owner_history is omitted. */
  user?: Entity;
  /** IfcApplication used to create an IfcOwnerHistory when owner_history is omitted. */
  application?: Entity;
}

export interface IfcOpenShellSequenceAssignSequenceOptions {
  /** Sequence type enum value. Defaults to "FINISH_START". */
  sequenceType?: string;
  /** Owner history applied to the IfcRelSequence. When omitted, one is created from user/application. */
  ownerHistory?: Entity;
  /** IfcPersonAndOrganization used to create an IfcOwnerHistory when owner_history is omitted. */
  user?: Entity;
  /** IfcApplication used to create an IfcOwnerHistory when owner_history is omitted. */
  application?: Entity;
}

export interface IfcOpenShellSequenceAssignWorkPlanOptions {
  /** Owner history applied to the aggregate relationship. When omitted, one is created from user/application. */
  ownerHistory?: Entity;
  /** IfcPersonAndOrganization used to create an IfcOwnerHistory when owner_history is omitted. */
  user?: Entity;
  /** IfcApplication used to create an IfcOwnerHistory when owner_history is omitted. */
  application?: Entity;
}

export interface IfcOpenShellSequenceCopyWorkScheduleOptions {
  /** Owner history applied to copied entities. When omitted, one is created from user/application. */
  ownerHistory?: Entity;
  /** IfcPersonAndOrganization used to create an IfcOwnerHistory when owner_history is omitted. */
  user?: Entity;
  /** IfcApplication used to create an IfcOwnerHistory when owner_history is omitted. */
  application?: Entity;
}

export interface IfcOpenShellSequenceCreateBaselineOptions {
  /** Name for the baseline schedule. When omitted, the baseline Name is left blank. */
  name?: string;
  /** Owner history applied to baseline entities. When omitted, one is created from user/application. */
  ownerHistory?: Entity;
  /** IfcPersonAndOrganization used to create an IfcOwnerHistory when owner_history is omitted. */
  user?: Entity;
  /** IfcApplication used to create an IfcOwnerHistory when owner_history is omitted. */
  application?: Entity;
}

export interface IfcOpenShellSequenceDuplicateTaskOptions {
  /** Owner history applied to duplicated entities. When omitted, one is created from user/application. */
  ownerHistory?: Entity;
  /** IfcPersonAndOrganization used to create an IfcOwnerHistory when owner_history is omitted. */
  user?: Entity;
  /** IfcApplication used to create an IfcOwnerHistory when owner_history is omitted. */
  application?: Entity;
}

export interface IfcOpenShellSequenceRemoveOptions {
  /** IfcPersonAndOrganization used to update or remove relationships. */
  user?: Entity;
  /** IfcApplication used to update or remove relationships. */
  application?: Entity;
}

export interface IfcOpenShellShapeBuilderAxis2Placement2dOptions {
  /** XY coordinates of the placement origin. */
  position: number[];
  /** Optional reference direction. Defaults to (1, 0) when omitted. */
  xDirection?: number[];
}

export interface IfcOpenShellShapeBuilderAxis2Placement3dOptions {
  /** XYZ coordinates of the placement origin. */
  position: number[];
  /** Direction ratios for the Z axis. */
  zAxis: number[];
  /** Direction ratios for the X axis (Y is derived). */
  xAxis: number[];
}

export interface IfcOpenShellShapeBuilderBlockOptions {
  /** XYZ position of the block's local origin (corner from which X/Y/Z lengths extend). */
  position: number[];
  /** Block length along X in model units. Defaults to 1.0. */
  xLength: number;
  /** Block length along Y in model units. Defaults to 1.0. */
  yLength: number;
  /** Block length along Z in model units. Defaults to 1.0. */
  zLength: number;
}

export interface IfcOpenShellShapeBuilderEllipseCurveOptions {
  /** First semi-axis radius (X direction). */
  xAxisRadius: number;
  /** Second semi-axis radius (Y direction). */
  yAxisRadius: number;
  /** XY centre of the ellipse. */
  position: number[];
  /** Two trim points in XY. When non-empty, wraps the ellipse in an IfcTrimmedCurve. */
  trimPoints: number[][];
  /** Optional reference X direction for the ellipse placement. */
  refXDirection?: number[];
  /** Cardinal index mask (0=+X, 1=+Y, 2=-X, 3=-Y) for trim points, used when trim_points is empty. */
  trimPointsMask: number[];
}

export interface IfcOpenShellShapeBuilderExtrudeOptions {
  /** IfcProfileDef or IfcCurve to extrude. Curves are wrapped in an IfcArbitraryClosedProfileDef. */
  profileOrCurve: Entity;
  /** Extrusion depth in model units. Must be greater than zero. */
  magnitude: number;
  /** XYZ position of the solid's local origin. */
  position: number[];
  /** Direction ratios for the extrusion direction. */
  extrusionVector: number[];
  /** Z axis direction ratios for the solid's placement. */
  positionZAxis: number[];
  /** X axis direction ratios for the solid's placement. */
  positionXAxis: number[];
  /** Optional Y axis; when provided, the Z axis is computed as cross(X, Y). */
  positionYAxis?: number[];
}

export interface IfcOpenShellShapeBuilderHalfSpaceSolidOptions {
  /** IfcPlane defining the bounding surface. */
  plane: Entity;
  /** If true, the half-space is on the side of the plane normal. */
  agreementFlag: boolean;
}

export interface IfcOpenShellShapeBuilderMepBendShapeOptions {
  /** MEP segment entity (must have a single material profile). */
  segment: Entity;
  /** Length of the start straight section in model units. */
  startLength: number;
  /** Length of the end straight section in model units. */
  endLength: number;
  /** Bend angle in radians. */
  angle: number;
  /** Bend radius in model units. */
  radius: number;
  /** XY direction indicating the bend plane. */
  bendVector: number[];
  /** If true, flip the Z axis direction. Defaults to false. */
  flipZAxis: boolean;
}

export interface IfcOpenShellShapeBuilderMepTransitionCalculateOptions {
  /** Half-dimensions of the start profile (X, Y). */
  startHalfDim: number[];
  /** Half-dimensions of the end profile (X, Y). */
  endHalfDim: number[];
  /** XY offset between the two profile centres. */
  offset: number[];
  /** Optional override for the absolute XY difference of half-dimensions. */
  diff?: number[];
  /** If true, swap X/Y axes for the end profile. Defaults to false. */
  endProfile: boolean;
  /** Transition length in model units. Provide either length or angle, not both. */
  length?: number;
  /** Transition angle in degrees. Provide either length or angle, not both. */
  angle?: number;
}

export interface IfcOpenShellShapeBuilderMepTransitionLengthOptions {
  /** Half-dimensions of the start profile (X, Y). */
  startHalfDim: number[];
  /** Half-dimensions of the end profile (X, Y). */
  endHalfDim: number[];
  /** Transition angle in degrees. */
  angle: number;
  /** XY offset between the two profile centres. */
  profileOffset: number[];
}

export interface IfcOpenShellShapeBuilderMepTransitionShapeOptions {
  /** Start MEP segment (IfcFlowSegment with material profile). */
  startSegment: Entity;
  /** End MEP segment (IfcFlowSegment with material profile). */
  endSegment: Entity;
  /** Length of the start straight section in model units. */
  startLength: number;
  /** Length of the end straight section in model units. */
  endLength: number;
  /** Transition angle in degrees. */
  angle: number;
  /** XY offset between the two profile centres. */
  profileOffset: number[];
}

export interface IfcOpenShellShapeBuilderMirrorOptions {
  /** Geometry item to mirror (polyline, circle, ellipse, trimmed curve, or extruded solid). */
  item: Entity;
  /** Mirror axes: non-zero values flip the corresponding axis (index 0 = X, index 1 = Y). */
  mirrorAxes: number[];
  /** XY point through which the mirror plane passes. */
  mirrorPoint: number[];
  /** If true, mirror an independent copy instead of the supplied item. Defaults to false. */
  createCopy: boolean;
  /** Optional 3x3 or 4x4 placement matrix for local-space mirroring. */
  placementMatrix: number[];
}

export interface IfcOpenShellShapeBuilderPolylineOptions {
  /** Ordered coordinate points (each XY or XYZ). */
  points: number[][];
  /** Whether the polyline is closed. Defaults to false. */
  closed?: boolean;
  /** Optional offset added to every point before storage. */
  positionOffset?: number[];
  /** Zero-based indices of arc middle points. Each selected point and its adjacent points form an arc segment. */
  arcPoints: number[];
}

export interface IfcOpenShellShapeBuilderProfileOptions {
  /** Outer boundary curve (IfcCurve entity). */
  outerCurve: Entity;
  /** Optional profile name. */
  name?: string;
  /** Inner void curves. If non-empty, creates IfcArbitraryProfileDefWithVoids. */
  innerCurves: Entity[];
  /** Profile type string (e.g. "AREA"). Defaults to "AREA". */
  profileType?: string;
}

export interface IfcOpenShellShapeBuilderRepresentationOptions {
  /** IfcGeometricRepresentationContext for the representation. */
  context: Entity;
  /** Representation items (geometric or topological entities). */
  items: Entity[];
  /** Representation type (e.g. "Tessellation", "Curve2D"). Auto-detected if omitted. */
  representationType?: string;
}

export interface IfcOpenShellShapeBuilderRotateOptions {
  /** Geometry item to rotate (polyline, circle, or extruded solid). */
  item: Entity;
  /** Rotation angle in degrees. */
  angle: number;
  /** XY pivot point for the rotation. */
  pivotPoint: number[];
  /** If true, rotate counter-clockwise; if false, clockwise. Defaults to false. */
  counterClockwise: boolean;
  /** If true, rotate an independent copy instead of the supplied item. Defaults to false. */
  createCopy: boolean;
}

export interface IfcOpenShellShapeBuilderSphereOptions {
  /** Sphere radius in model units. Defaults to 1.0. */
  radius: number;
  /** XYZ centre of the sphere. */
  center: number[];
}

export interface IfcOpenShellShapeBuilderTranslateOptions {
  /** Geometry item to translate (polyline, circle, extruded solid, or shape representation). */
  item: Entity;
  /** Translation (XY or XYZ). */
  translation: number[];
  /** If true, translate an independent copy instead of the supplied item. Defaults to false. */
  createCopy: boolean;
}

export interface IfcOpenShellSpatialAssignContainerOptions {
  /** Products to contain in the spatial structure. */
  products: Entity[];
  /** The IfcSpatialStructureElement (e.g. IfcBuildingStorey, IfcSpace) to contain the products. */
  relatingStructure: Entity;
  /** Optional owner history. A new one is created from user/application if not provided. */
  ownerHistory?: Entity;
  /** Optional user for owner history creation. */
  user?: Entity;
  /** Optional application for owner history creation. */
  application?: Entity;
}

export interface IfcOpenShellSpatialDereferenceStructureOptions {
  /** Products to remove from the spatial reference. */
  products: Entity[];
  /** The IfcSpatialStructureElement to dereference from. */
  relatingStructure: Entity;
  /** Optional user for owner history updates on modified relationships. */
  user?: Entity;
  /** Optional application for owner history updates on modified relationships. */
  application?: Entity;
}

export interface IfcOpenShellSpatialReferenceStructureOptions {
  /** Products to reference in the spatial structure. */
  products: Entity[];
  /** The IfcSpatialStructureElement to reference the products from. */
  relatingStructure: Entity;
  /** Optional owner history. A new one is created from user/application if not provided. */
  ownerHistory?: Entity;
  /** Optional user for owner history creation. */
  user?: Entity;
  /** Optional application for owner history creation. */
  application?: Entity;
}

export interface IfcOpenShellSpatialUnassignContainerOptions {
  /** Products to remove from their spatial containment relationships. */
  products: Entity[];
  /** Optional user for owner history updates on modified relationships. */
  user?: Entity;
  /** Optional application for owner history updates on modified relationships. */
  application?: Entity;
}

export interface IfcOpenShellStructuralAddStructuralActivityOptions {
  /** Owner history for the IfcStructuralActivity entity. When omitted, no OwnerHistory is set. */
  activityOwnerHistory?: Entity;
  /** Owner history for the IfcRelConnectsStructuralActivity relationship. When omitted, no OwnerHistory is set. */
  relationshipOwnerHistory?: Entity;
}

export interface IfcOpenShellStructuralAddStructuralBoundaryConditionOptions {
  /** Name for the boundary condition entity. */
  name?: string;
  /** IfcRelConnectsStructuralMember or IfcStructuralConnection to which the condition is applied. When provided, the boundary class is inferred from the connection type. */
  connection?: Entity;
}

export interface IfcOpenShellStructuralAssignStructuralAnalysisModelOptions {
  /** Owner history applied to the IfcRelAssignsToGroup relationship. When omitted, one is created from user/application. */
  ownerHistory?: Entity;
  /** IfcPersonAndOrganization used to create an IfcOwnerHistory when owner_history is omitted. */
  user?: Entity;
  /** IfcApplication used to create an IfcOwnerHistory when owner_history is omitted. */
  application?: Entity;
}

export interface IfcOpenShellStructuralRemoveStructuralBoundaryConditionOptions {
  /** IfcStructuralConnection whose AppliedCondition to clear and delete. Takes precedence over boundary_condition. */
  connection?: Entity;
  /** IfcBoundaryCondition entity to remove. Used when connection is omitted. */
  boundaryCondition?: Entity;
}

export interface IfcOpenShellStructuralUnassignStructuralAnalysisModelOptions {
  /** IfcPersonAndOrganization used to update the relationship OwnerHistory. */
  user?: Entity;
  /** IfcApplication used to update the relationship OwnerHistory. */
  application?: Entity;
}

export interface IfcOpenShellStyleAssignItemStyleOptions {
  /** The representation item to assign the style to. */
  item: Entity;
  /** Optional style to assign. When omitted, the existing style is removed. */
  style?: Entity;
  /** Whether to use IfcPresentationStyleAssignment (for IFC2X3 compat). */
  shouldUsePresentationStyleAssignment: boolean;
}

export interface IfcOpenShellSystemAddPortOptions {
  /**
   * Optional element to assign the port to. If omitted, the port is created
   * standalone and must be assigned later via system_assign_port.
   */
  element?: Entity;
  /** Optional owner history. A new one is created from user/application if not provided. */
  ownerHistory?: Entity;
  /** Optional user for owner history creation. */
  user?: Entity;
  /** Optional application for owner history creation. */
  application?: Entity;
}

export interface IfcOpenShellSystemAddSystemOptions {
  /**
   * IFC class for the system (e.g. "IfcDistributionSystem", "IfcZone",
   * "IfcBuildingSystem"). In IFC2X3, "IfcDistributionSystem" is
   * automatically mapped to "IfcSystem".
   */
  ifcClass: string;
  /** Optional owner history. */
  ownerHistory?: Entity;
}

export interface IfcOpenShellSystemAssignFlowControlOptions {
  /** The flow element (e.g. IfcFlowSegment) that owns the control. */
  relatingFlowElement: Entity;
  /** The flow control element (e.g. IfcFlowController) to assign. */
  relatedFlowControl: Entity;
  /** Optional owner history. A new one is created from user/application if not provided. */
  ownerHistory?: Entity;
  /** Optional user for owner history creation. */
  user?: Entity;
  /** Optional application for owner history creation. */
  application?: Entity;
}

export interface IfcOpenShellSystemAssignPortOptions {
  /** The element to assign the port to. */
  element: Entity;
  /** The IfcDistributionPort to assign. */
  port: Entity;
  /** Optional owner history. A new one is created from user/application if not provided. */
  ownerHistory?: Entity;
  /** Optional user for owner history creation. */
  user?: Entity;
  /** Optional application for owner history creation. */
  application?: Entity;
}

export interface IfcOpenShellSystemAssignSystemOptions {
  /** Products to assign to the system. Must be compatible with the system type. */
  products: Entity[];
  /** The system (IfcSystem subclass or IfcGroup) to assign products to. */
  system: Entity;
  /** Optional owner history. A new one is created from user/application if not provided. */
  ownerHistory?: Entity;
  /** Optional user for owner history creation. */
  user?: Entity;
  /** Optional application for owner history creation. */
  application?: Entity;
}

export interface IfcOpenShellSystemConnectPortOptions {
  /** The first port in the connection. */
  port1: Entity;
  /** The second port in the connection. */
  port2: Entity;
  /**
   * Flow direction: "SOURCE" (port1 sources, port2 sinks), "SINK" (port1
   * sinks, port2 sources), "SOURCEANDSINK", or "NOTDEFINED".
   */
  direction: string;
  /** Optional realizing element (e.g. a pipe or duct connecting the ports). */
  element?: Entity;
  /** Optional owner history. A new one is created from user/application if not provided. */
  ownerHistory?: Entity;
  /** Optional user for owner history creation. */
  user?: Entity;
  /** Optional application for owner history creation. */
  application?: Entity;
}

export interface IfcOpenShellSystemUnassignFlowControlOptions {
  /** The flow element that owns the control. */
  relatingFlowElement: Entity;
  /** The flow control element to unassign. */
  relatedFlowControl: Entity;
  /** Optional user for owner history updates on modified relationships. */
  user?: Entity;
  /** Optional application for owner history updates on modified relationships. */
  application?: Entity;
}

export interface IfcOpenShellSystemUnassignPortOptions {
  /** The element the port is assigned to. */
  element: Entity;
  /** The IfcDistributionPort to unassign. */
  port: Entity;
  /** Optional user for owner history updates on modified relationships. */
  user?: Entity;
  /** Optional application for owner history updates on modified relationships. */
  application?: Entity;
}

export interface IfcOpenShellSystemUnassignSystemOptions {
  /** Products to remove from the system. */
  products: Entity[];
  /** The system to remove products from. */
  system: Entity;
  /** Optional user for owner history updates on modified relationships. */
  user?: Entity;
  /** Optional application for owner history updates on modified relationships. */
  application?: Entity;
}

export interface IfcOpenShellTypeAssignTypeOptions {
  /** Element occurrences to assign the type to. */
  objects: Entity[];
  /** The IfcTypeObject to assign. */
  relatingType: Entity;
  /**
   * If true (default), IfcRepresentationMaps from the type are propagated
   * onto each occurrence, and PredefinedType/ObjectType are cleared on
   * objects when the type already declares a non-NOTDEFINED PredefinedType.
   */
  shouldMapRepresentations?: boolean;
  /** Optional owner history. A new one is created from user/application if not provided. */
  ownerHistory?: Entity;
  /** Optional user for owner history creation. */
  user?: Entity;
  /** Optional application for owner history creation. */
  application?: Entity;
}

export interface IfcOpenShellTypeUnassignTypeOptions {
  /** Element occurrences to remove type assignments from. */
  objects: Entity[];
  /** Optional user for owner history updates on modified relationships. */
  user?: Entity;
  /** Optional application for owner history updates on modified relationships. */
  application?: Entity;
}

export interface AggregateApi {
    /**
     * Assign products as parts of an aggregate object via IfcRelAggregates.
     *
     * Products already aggregated under a different parent are moved. Products
     * previously contained in a spatial structure (IfcRelContainedInSpatialStructure)
     * are removed from that containment. If an existing IfcRelAggregates
     * relationship already relates to the same parent, products are merged into it.
     */
    assignObject(file: IfcFile, options: IfcOpenShellAggregateAssignObjectOptions): Entity;
    /**
     * Remove products from their IfcRelAggregates relationships.
     *
     * If no related objects remain after removal, the IfcRelAggregates
     * relationship itself is deleted.
     */
    unassignObject(file: IfcFile, options: IfcOpenShellAggregateUnassignObjectOptions): void;
}
export interface AttributeApi {
    /**
     * Edit arbitrary attributes on a product.
     *
     * Applies the attribute values from the property bag to the product.
     * Optionally synchronizes PredefinedType when ElementType or ObjectType
     * changes, and optionally updates OwnerHistory.
     *
     * @param file The IFC file containing the product.
     * @param options Attribute editing options.
     */
    editAttributes(file: IfcFile, options: IfcOpenShellAttributeEditAttributesOptions): void;
}
export interface BoundaryApi {
    /** Assign a planar connection geometry to a space boundary relationship. */
    assignConnectionGeometry(file: IfcFile, rel_space_boundary: Entity, options: IfcOpenShellBoundaryAssignConnectionGeometryOptions): void;
    /**
     * Create a copy of a space boundary relationship and its connection geometry.
     *
     * @param file File that receives the copied entities.
     * @param boundary IfcRelSpaceBoundary entity to copy.
     * @return Newly created boundary relationship, or no result if the copy cannot be created.
     */
    copyBoundary(file: IfcFile, boundary: Entity): Entity;
    /**
     * Edit attributes of a space boundary relationship.
     *
     * Updates the relating space, related building element, and boundary
     * classification. ParentBoundary and CorrespondingBoundary are set only when
     * the schema supports them (IFC4+). When omitted, those attributes are cleared.
     *
     * @param entity IfcRelSpaceBoundary entity to modify.
     * @param options Attribute values to set.
     */
    editAttributes(entity: Entity, options: IfcOpenShellBoundaryEditAttributesOptions): void;
    /**
     * Remove a space boundary relationship and its connection geometry.
     *
     * Removes the connection geometry and then removes the boundary relationship.
     * Unreferenced entities belonging to the connection geometry are removed.
     *
     * @param file IFC file containing the boundary.
     * @param boundary IfcRelSpaceBoundary entity to remove.
     */
    removeBoundary(file: IfcFile, boundary: Entity): void;
}
export interface ClassificationApi {
    /**
     * Create a new IfcClassification and associate it with the project.
     *
     * The classification is linked to the first IfcProject via
     * IfcRelAssociatesClassification so that it persists in the file.
     */
    addClassification(file: IfcFile, name: string): Entity;
    /**
     * Add a classification reference and associate it with products.
     *
     * If an existing classification reference is provided, it is used directly.
     * Otherwise, a new IfcClassificationReference is created using the
     * optional identification, name, and classification fields.
     */
    addReference(file: IfcFile, options: IfcOpenShellClassificationAddReferenceOptions): Entity;
    /**
     * Return all classification references associated with an element.
     *
     * For IfcRoot-derived elements, returns references from IfcRelAssociatesClassification.
     * For non-IfcRoot elements (e.g. IfcMaterial), returns references via
     * IfcExternalReferenceRelationship. When should_inherit is true, references
     * from the element's type are merged, with occurrence-level references
     * overriding type-level ones within the same classification system.
     */
    getReferences(element: Entity, should_inherit: boolean): Entity[];
    /**
     * Remove an IfcClassification and all its references.
     *
     * For IFC4+, recursively deletes every IfcClassificationReference in the
     * classification hierarchy via HasReferences. For IFC2X3, deletes only
     * references whose ReferencedSource is the classification itself; nested
     * references are not traversed and may be orphaned. Any
     * IfcRelAssociatesClassification that becomes orphaned is also deleted.
     */
    removeClassification(file: IfcFile, classification: Entity): void;
    /**
     * Remove a classification reference association from products.
     *
     * If the reference is no longer associated with any products after
     * removal, the reference entity itself is also deleted.
     */
    removeReference(file: IfcFile, options: IfcOpenShellClassificationRemoveReferenceOptions): void;
}
export interface CogoApi {
    /**
     * Add a single survey point as an IfcAnnotation.
     *
     * Creates an IfcAnnotation with PredefinedType SURVEY, wraps the given
     * IfcPoint in an IfcShapeRepresentation (Annotation/Point), and places
     * the annotation in the given or first IfcSite. The annotation's
     * ObjectPlacement is set to the world coordinate system of the
     * Model/Annotation/MODEL_VIEW representation context.
     *
     * @param file File that receives the new entities.
     * @param options Survey point geometry and placement options.
     * @return The newly created IfcAnnotation, or no result if creation fails.
     */
    addSurveyPoint(file: IfcFile, options: IfcOpenShellCogoAddSurveyPointOptions): Entity;
    /**
     * Replace the survey point geometry of an existing annotation.
     *
     * Replaces the annotation's existing survey point with the given IfcPoint.
     * The annotation must already have a shape representation.
     *
     * @param annotation IfcAnnotation whose survey point to replace.
     * @param survey_point IfcPoint to assign as the new geometry.
     */
    assignSurveyPoint(annotation: Entity, survey_point: Entity): void;
    /**
     * Update the coordinates of the survey point inside an existing annotation.
     *
     * Updates the coordinates of the annotation's survey point. If the existing
     * point is two-dimensional, only x and y are written; otherwise all three
     * coordinates are used.
     *
     * @param annotation IfcAnnotation containing the survey point.
     * @param x Easting or X coordinate in model units.
     * @param y Northing or Y coordinate in model units.
     * @param z Elevation or Z coordinate in model units.
     */
    editSurveyPoint(annotation: Entity, x: number, y: number, z: number): void;
}
export interface ComputeApi {
    /**
     * Compute a derived attribute value for an instance.
     *
     * Evaluates derived attributes defined in the IFC schema (e.g.
     * IfcDirection.DirectionRatios) for the given instance and attribute.
     *
     * @param instance The entity instance.
     * @param attribute_name The name of the derived attribute.
     * @return The computed value, or no result if it cannot be computed. Release it with value_free.
     */
    derived(instance: Entity, attribute_name: string): ValueData | null;
}
export interface ConstraintApi {
    /**
     * Add a new IfcMetric constraint. If an objective is provided, the metric
     * is appended to the objective's benchmark values.
     */
    addMetric(file: IfcFile, objective: Entity): Entity;
    /**
     * Create a chain of IfcReference entities from a dot-separated path and
     * assign it to a metric's ReferencePath attribute.
     *
     * For example, a path "Pset_WallCommon.FireRating" produces two IfcReference
     * entities linked by InnerReference, with AttributeIdentifier set to
     * "Pset_WallCommon" and "FireRating" respectively.
     *
     * @return The created IfcReference chain in order from outermost to innermost.
     */
    addMetricReference(file: IfcFile, metric: Entity, reference_path: string): Entity[];
    /**
     * Create a new IfcObjective constraint with default attributes.
     *
     * Sets Name to "Unnamed", ConstraintGrade and ObjectiveQualifier to "NOTDEFINED".
     */
    addObjective(file: IfcFile): Entity;
    /**
     * Assign a constraint to a list of products via IfcRelAssociatesConstraint.
     *
     * If the constraint is already associated with some of the products, only
     * new products are added to the existing relationship.
     */
    assignConstraint(file: IfcFile, options: IfcOpenShellConstraintAssignConstraintOptions): Entity;
    /**
     * Remove a constraint entity and clean up orphaned IfcRelAssociatesConstraint
     * relationships that reference it.
     */
    removeConstraint(file: IfcFile, constraint: Entity): void;
    /**
     * Remove an IfcMetric and its reference path chain.
     *
     * Deletes the metric's IfcReference chain (via InnerReference), then removes
     * the metric entity. Orphaned IfcRelAssociatesConstraint and
     * IfcResourceConstraintRelationship entities are also deleted.
     */
    removeMetric(file: IfcFile, metric: Entity): void;
    /**
     * Remove constraint associations from products.
     *
     * Products are removed from existing IfcRelAssociatesConstraint
     * relationships. If no products remain, the relationship is removed.
     */
    unassignConstraint(file: IfcFile, options: IfcOpenShellConstraintUnassignConstraintOptions): void;
}
export interface ContextApi {
    /**
     * Create a geometric representation context or subcontext.
     *
     * When parent is omitted, creates an IfcGeometricRepresentationContext with
     * a world coordinate system and attaches it to the IfcProject's
     * RepresentationContexts. A context_type of "Plan" creates a 2D context
     * (coordinate space dimension 2); all other values create a 3D context.
     *
     * When parent is provided, creates an IfcGeometricRepresentationSubContext
     * as a child of the given parent context.
     */
    addContext(file: IfcFile, options: IfcOpenShellContextAddContextOptions): Entity;
    /**
     * Edit attributes of an existing geometric representation context.
     *
     * Applies attribute key-value pairs from the props builder to the context entity.
     */
    editContext(file: IfcFile, context: Entity, attributes: PsetProperties | PsetInput): void;
    /**
     * Remove a geometric representation context and its subcontexts recursively.
     *
     * For subcontexts, IfcCoordinateOperation references are removed and other
     * referencing entities are redirected to the parent context. For top-level
     * contexts, representations using the context are unassigned from their
     * elements and removed.
     */
    removeContext(file: IfcFile, context: Entity): void;
}
export interface ControlApi {
    /**
     * Assign a planning control or constraint to objects via IfcRelAssignsToControl.
     *
     * Objects already assigned to the control are skipped. If an existing
     * IfcRelAssignsToControl relationship exists for the control, new objects
     * are merged into it.
     */
    assignControl(file: IfcFile, options: IfcOpenShellControlAssignControlOptions): Entity;
    /**
     * Remove objects from an IfcRelAssignsToControl relationship.
     *
     * If no related objects remain after removal, the relationship is deleted.
     */
    unassignControl(file: IfcFile, options: IfcOpenShellControlUnassignControlOptions): void;
}
export interface CostApi {
    /**
     * Create an IfcCostItem entity.
     *
     * When both cost_schedule and cost_item are provided, cost_schedule takes
     * precedence and the item is assigned to it via IfcRelAssignsToControl.
     * When only cost_item is provided, the item is nested under it via
     * IfcRelNests.
     *
     * @param file File that receives the new entity.
     * @param options Schedule/parent placement and ownership options.
     * @return Newly created IfcCostItem.
     */
    addCostItem(file: IfcFile, options: IfcOpenShellCostAddCostItemOptions): Entity;
    /**
     * Create an IfcPhysicalQuantity and add it to a cost item.
     *
     * Creates a quantity of the given IFC class (e.g. "IfcQuantityLength",
     * "IfcQuantityCount") with Name "Unnamed" and appends it to the cost item's
     * CostQuantities aggregate. For IfcQuantityCount, the initial value is set
     * to the number of controlled objects.
     *
     * @param file File that receives the new entity.
     * @param cost_item IfcCostItem to receive the quantity.
     * @param ifc_class IFC quantity class name.
     * @return Newly created IfcPhysicalQuantity.
     */
    addCostItemQuantity(file: IfcFile, cost_item: Entity, ifc_class: string): Entity;
    /**
     * Create an IfcCostSchedule entity.
     *
     * Creates the schedule with the given name, predefined type, and update date.
     * The update_date is stored as an IfcDateAndTime on IFC2X3 or as a string
     * on IFC4+.
     *
     * @param file File that receives the new entity.
     * @param name Schedule name. When omitted or empty, no name is assigned.
     * @param predefined_type IFC predefined type enum value (e.g. "BUDGET", "COSTPLAN").
     * @param update_date ISO 8601 date-time string for the UpdateDate attribute.
     * @param owner_history Owner history for the new entity. When omitted, no owner history is assigned.
     * @return Newly created IfcCostSchedule.
     */
    addCostSchedule(file: IfcFile, name: string, predefined_type: string, update_date: string, owner_history: Entity): Entity;
    /**
     * Create an IfcCostValue and attach it to a parent entity.
     *
     * Appends the new IfcCostValue to the parent's CostValues (IfcCostItem),
     * BaseCosts (IfcConstructionResource), or Components (IfcCostValue)
     * aggregate, depending on the parent's type.
     *
     * @param file File that receives the new entity.
     * @param parent IfcCostItem, IfcConstructionResource, or IfcCostValue to receive the value.
     * @return Newly created IfcCostValue.
     */
    addCostValue(file: IfcFile, parent: Entity): Entity;
    /**
     * Assign product quantities to a cost item.
     *
     * For each product, creates an IfcRelAssignsToControl linking the cost item
     * to the product. If prop_name is provided, matching quantities from the
     * products' IfcElementQuantity property sets are collected into the cost
     * item's CostQuantities. If prop_name is omitted or empty and the cost item has a
     * single IfcQuantityCount, its value is updated to the count of assigned
     * non-resource objects. IfcSpatialElement products are skipped.
     *
     * @param file File containing the cost item and products.
     * @param cost_item IfcCostItem to assign quantities to.
     * @param products Products whose quantities to collect.
     * @param prop_name Quantity property name to match. When omitted, no named quantity is collected.
     * @param options Ownership options for the assignment relationship.
     */
    assignCostItemQuantity(file: IfcFile, cost_item: Entity, products: Entity[], prop_name: string, options: IfcOpenShellCostAssignCostItemQuantityOptions): void;
    /**
     * Assign a cost rate's values to a cost item.
     *
     * Removes the cost item's existing CostValues, then assigns the cost rate's
     * CostValues aggregate directly (sharing, not copying).
     *
     * @param file File containing both entities.
     * @param cost_item IfcCostItem to receive the values.
     * @param cost_rate IfcCostItem whose CostValues to assign.
     */
    assignCostValue(file: IfcFile, cost_item: Entity, cost_rate: Entity): void;
    /**
     * Calculate and populate cost values from assigned resources.
     *
     * Removes existing CostValues from the cost item, then for each
     * IfcConstructionResource assigned (directly or via an IfcTask), creates
     * an IfcCostValue with the resource's base cost rate multiplied by its
     * quantity. Resources with day-based units are converted using an 8-hour
     * workday.
     *
     * @param file File containing the cost item.
     * @param cost_item IfcCostItem to populate.
     */
    calculateCostItemResourceValue(file: IfcFile, cost_item: Entity): void;
    /**
     * Copy an IfcCostItem and its nested children.
     *
     * Creates independent copies of the cost item, nested child items, property
     * sets, and IfcRelDefinesByProperties relationships. The returned list contains
     * the new root item followed by its descendants.
     *
     * @param file File that receives the copied entities.
     * @param cost_item IfcCostItem to copy.
     * @return List of newly created IfcCostItem entities, with the root first.
     */
    copyCostItem(file: IfcFile, cost_item: Entity): Entity[];
    /**
     * Copy the cost values from one cost item to another.
     *
     * Removes existing CostValues from the destination, then creates independent
     * copies of the source values and their component trees.
     *
     * @param file File containing both cost items.
     * @param source IfcCostItem to copy values from.
     * @param destination IfcCostItem to copy values to.
     */
    copyCostItemValues(file: IfcFile, source: Entity, destination: Entity): void;
    /**
     * Copy an IfcCostSchedule and all its controlled cost items.
     *
     * Creates an independent schedule and independent copies of each controlled
     * IfcCostItem, then assigns the copies to the new schedule.
     *
     * @param file File that receives the copied entities.
     * @param cost_schedule IfcCostSchedule to copy.
     * @param options Ownership options for the new schedule and its items.
     * @return Newly created IfcCostSchedule.
     */
    copyCostSchedule(file: IfcFile, cost_schedule: Entity, options: IfcOpenShellCostCopyCostScheduleOptions): Entity;
    /**
     * Edit attributes of an IfcCostItem.
     *
     * Applies attribute changes from the property bag to the cost item.
     *
     * @param file File containing the cost item.
     * @param cost_item IfcCostItem entity to edit.
     * @param attributes Property bag of attribute name/value pairs.
     */
    editCostItem(file: IfcFile, cost_item: Entity, attributes: PsetProperties | PsetInput): void;
    /**
     * Edit attributes of an IfcPhysicalQuantity.
     *
     * Applies attribute changes from the property bag to the physical quantity.
     *
     * @param file File containing the quantity.
     * @param physical_quantity IfcPhysicalQuantity entity to edit.
     * @param attributes Property bag of attribute name/value pairs.
     */
    editCostItemQuantity(file: IfcFile, physical_quantity: Entity, attributes: PsetProperties | PsetInput): void;
    /**
     * Edit attributes of an IfcCostSchedule.
     *
     * Applies attribute changes from the property bag to the cost schedule.
     *
     * @param file File containing the cost schedule.
     * @param cost_schedule IfcCostSchedule entity to edit.
     * @param attributes Property bag of attribute name/value pairs.
     */
    editCostSchedule(file: IfcFile, cost_schedule: Entity, attributes: PsetProperties | PsetInput): void;
    /**
     * Edit attributes of an IfcCostValue.
     *
     * Applies attribute changes from the property bag. The AppliedValue attribute
     * is wrapped as IfcMonetaryMeasure. When edit_unit_basis is true, the
     * UnitBasis (IfcMeasureWithUnit) is replaced or cleared.
     *
     * @param file File containing the cost value.
     * @param cost_value IfcCostValue entity to edit.
     * @param attributes Property bag of attribute name/value pairs.
     * @param options Unit basis replacement options.
     */
    editCostValue(file: IfcFile, cost_value: Entity, attributes: PsetProperties | PsetInput, options: IfcOpenShellCostEditCostValueOptions): void;
    /**
     * Set an IfcCostValue's attributes from a formula expression.
     *
     * Parses a text formula (e.g. "100*5", "sum(10+20)") and applies the
     * resulting value tree to the cost value's AppliedValue, Category,
     * ArithmeticOperator, and Component cost values.
     *
     * @param file File containing the cost value.
     * @param cost_value IfcCostValue entity to update.
     * @param formula Formula string to parse.
     */
    editCostValueFormula(file: IfcFile, cost_value: Entity, formula: string): void;
    /**
     * Remove an IfcCostItem and its nested children from the file.
     *
     * Recursively removes nested child cost items, then removes the item's
     * IfcRelNests and IfcRelAssignsToControl relationships (when the item is
     * the sole RelatedObject), and finally removes the item itself.
     *
     * @param file File containing the cost item.
     * @param cost_item IfcCostItem to remove.
     */
    removeCostItem(file: IfcFile, cost_item: Entity): void;
    /**
     * Remove a physical quantity from a cost item.
     *
     * If the quantity is only referenced by this cost item, it is deleted from
     * the file. Otherwise it is removed from the CostQuantities aggregate only.
     *
     * @param file File containing the cost item.
     * @param cost_item IfcCostItem that owns the quantity.
     * @param physical_quantity IfcPhysicalQuantity to remove.
     */
    removeCostItemQuantity(file: IfcFile, cost_item: Entity, physical_quantity: Entity): void;
    /**
     * Remove an IfcCostSchedule and all its controlled cost items from the file.
     *
     * Removes each controlled IfcCostItem (recursively), then removes the
     * schedule's IfcRelAssignsToControl relationships and the schedule itself.
     *
     * @param file File containing the cost schedule.
     * @param cost_schedule IfcCostSchedule to remove.
     */
    removeCostSchedule(file: IfcFile, cost_schedule: Entity): void;
    /**
     * Remove an IfcCostValue from its parent and the file.
     *
     * If the cost value is only referenced by this parent, it is deleted from
     * the file. Otherwise it is removed from the parent's CostValues, BaseCosts,
     * or Components aggregate, depending on the parent's type.
     *
     * @param file File containing the cost value.
     * @param parent IfcCostItem, IfcConstructionResource, or IfcCostValue that owns the value.
     * @param cost_value IfcCostValue to remove.
     */
    removeCostValue(file: IfcFile, parent: Entity, cost_value: Entity): void;
    /**
     * Remove product quantity assignments from a cost item.
     *
     * Removes the cost item's control relationship over the given products and
     * strips any quantities from CostQuantities that are referenced by those
     * products' IfcElementQuantity property sets. If a lone IfcQuantityCount
     * remains, its value is updated to the remaining object count or removed
     * if zero.
     *
     * @param file File containing the cost item and products.
     * @param cost_item IfcCostItem to unassign from.
     * @param products Products to unassign.
     * @param options Ownership options for updating the relationship.
     */
    unassignCostItemQuantity(file: IfcFile, cost_item: Entity, products: Entity[], options: IfcOpenShellCostUnassignCostItemQuantityOptions): void;
}
export interface DocumentApi {
    /**
     * Create a new IfcDocumentInformation and link it to a parent.
     *
     * If the parent is an IfcProject or IfcContext, the document is linked via
     * IfcRelAssociatesDocument. If the parent is another IfcDocumentInformation,
     * the new document is linked as a child via IfcDocumentInformationRelationship.
     * If parent is omitted, the first IfcProject is used.
     */
    addInformation(file: IfcFile, options: IfcOpenShellDocumentAddInformationOptions): Entity;
    /**
     * Create a new IfcDocumentReference.
     *
     * If an IfcDocumentInformation is provided, the reference is linked to it
     * (via ReferencedDocument in IFC4+ or DocumentReferences in IFC2X3).
     */
    addReference(file: IfcFile, information: Entity): Entity;
    /**
     * Assign a document (IfcDocumentInformation or IfcDocumentReference) to products.
     *
     * Creates or updates an IfcRelAssociatesDocument relationship. Products
     * already associated with the document are skipped.
     */
    assignDocument(file: IfcFile, options: IfcOpenShellDocumentAssignDocumentOptions): Entity;
    /**
     * Remove an IfcDocumentInformation and its entire document tree.
     *
     * Recursively deletes all child IfcDocumentReference entities, child
     * IfcDocumentInformation entities (via IsPointer relationships), and all
     * associated IfcRelAssociatesDocument relationships.
     */
    removeInformation(file: IfcFile, information: Entity): void;
    /**
     * Remove an IfcDocumentReference and its association relationships.
     *
     * Deletes the reference entity and all IfcRelAssociatesDocument relationships
     * that reference it.
     */
    removeReference(file: IfcFile, reference: Entity): void;
    /**
     * Remove products from a document association.
     *
     * Products are removed from IfcRelAssociatesDocument relationships. If no
     * products remain in a relationship, the relationship is deleted.
     */
    unassignDocument(file: IfcFile, options: IfcOpenShellDocumentUnassignDocumentOptions): void;
}
export interface DrawingApi {
    /**
     * Assign a product to an annotation object via IfcRelAssignsToProduct.
     *
     * When the relating product is an IfcGridAxis, the assignment is created
     * against the owning IfcGrid with the axis tag as the relationship name;
     * a new IfcRelAssignsToProduct is always created for grid axes even if one
     * already exists for the same axis tag. For non-grid products, if an
     * existing IfcRelAssignsToProduct already references the relating product,
     * the related object is appended to its RelatedObjects aggregate instead
     * of creating a new relationship. Returns no result for an exact duplicate.
     *
     * @param file IFC file to modify.
     * @param options Assignment parameters.
     * @return IfcRelAssignsToProduct relationship, or no result for a duplicate or failure.
     */
    assignProduct(file: IfcFile, options: IfcOpenShellDrawingAssignProductOptions): Entity;
    /**
     * Remove a product assignment from an annotation object.
     *
     * Finds and removes the IfcRelAssignsToProduct relating the product to the
     * object. If the relationship contains only this one related object, the
     * entire relationship is removed; otherwise only the object is removed from
     * the RelatedObjects aggregate.
     *
     * @param file IFC file to modify.
     * @param options Unassignment parameters.
     */
    unassignProduct(file: IfcFile, options: IfcOpenShellDrawingUnassignProductOptions): void;
}
export interface ElementApi {
    /**
     * Return the aggregate parent of an element.
     *
     * Returns the RelatingObject of an IfcRelAggregates relationship. In IFC2X3,
     * returns no result when the decomposition uses IfcRelNests instead.
     *
     * @param instance The element to query.
     * @return The aggregate parent, or no result if the element is not aggregated.
     */
    getAggregate(instance: Entity): Entity | null;
    /**
     * Return elements directly contained in a spatial element.
     *
     * Returns RelatedElements from the spatial element's containment relationships.
     *
     * @param element The spatial element (e.g. IfcBuildingStorey).
     * @return List of contained elements.
     */
    getContained(element: Entity): Entity[];
    /**
     * Return the spatial container of an element.
     *
     * By default considers indirect spatial containers (e.g. a building storey
     * for an element inside an aggregate). When
     * direct_only is true, only a direct ContainedInStructure relationship
     * is considered.
     *
     * @param instance The element to query.
     * @param options Container lookup options.
     * @return The spatial container, or no result if the element is not contained.
     */
    getContainer(instance: Entity, options: IfcOpenShellElementGetContainerOptions): Entity | null;
    /**
     * Return the controls assigned to an element.
     *
     * Returns controls from the element's IfcRelAssignsToControl relationships.
     *
     * @param element The element to query.
     * @return List of IfcControl entities.
     */
    getControls(element: Entity): Entity[];
    /**
     * Return the full spatial decomposition of an element.
     *
     * Returns subelements related through containment, aggregation, openings,
     * fillings, and nesting. When is_recursive is true (default), the result
     * includes the full hierarchy in breadth-first order.
     *
     * @param element The root element.
     * @param options Decomposition traversal options.
     * @return Ordered list of decomposed subelements (excluding the root).
     */
    getDecomposition(element: Entity, options: IfcOpenShellElementGetDecompositionOptions): Entity[];
    /**
     * Return elements assigned to a presentation layer.
     *
     * Returns elements whose geometry appears in AssignedItems of the
     * IfcPresentationLayerAssignment.
     *
     * @param layer The IfcPresentationLayerAssignment entity.
     * @return List of elements on the layer.
     */
    getElementsByLayer(layer: Entity): Entity[];
    /**
     * Return elements that use a material, directly or via a material set.
     *
     * Returns elements associated through IfcRelAssociatesMaterial, including
     * elements using the material as part of a layer, profile, constituent, or
     * material list.
     *
     * @param material The IfcMaterial or material set entity.
     * @return List of elements using the material.
     */
    getElementsByMaterial(material: Entity): Entity[];
    /**
     * Return elements that use a profile definition in their representation.
     *
     * Returns elements whose geometry references the profile through their
     * representation items.
     *
     * @param profile The IfcProfileDef entity.
     * @return List of elements using the profile.
     */
    getElementsByProfile(profile: Entity): Entity[];
    /**
     * Return elements that use a geometric representation.
     *
     * Returns IfcProduct and IfcTypeProduct entities that reference the
     * representation through their product representation or representation map.
     *
     * @param representation The IfcShapeRepresentation entity.
     * @return List of elements using the representation.
     */
    getElementsByRepresentation(representation: Entity): Entity[];
    /**
     * Return elements whose geometric representation uses a style.
     *
     * Returns elements whose shape representations contain the style through
     * IfcStyledItem relationships.
     *
     * @param style The IfcPresentationStyle entity.
     * @return List of elements using the style.
     */
    getElementsByStyle(style: Entity): Entity[];
    /**
     * Return the opening element that an element fills.
     *
     * Returns the RelatingOpeningElement from the element's filling relationship.
     * This typically applies to windows and doors.
     *
     * @param element The filling element (e.g. IfcWindow).
     * @return The IfcOpeningElement being filled, or no result if none is associated.
     */
    getFilledVoid(element: Entity): Entity | null;
    /**
     * Return the groups that an element is assigned to.
     *
     * Returns groups from the element's IfcRelAssignsToGroup relationships.
     *
     * @param element The element to query.
     * @return List of IfcGroup entities.
     */
    getGroups(element: Entity): Entity[];
    /**
     * Return the presentation layers that an element is part of.
     *
     * Returns IfcPresentationLayerAssignment entities referenced by the
     * element's representation.
     *
     * @param element The element to query.
     * @return List of IfcPresentationLayerAssignment entities.
     */
    getLayers(element: Entity): Entity[];
    /**
     * Return the material associated with an element.
     *
     * Searches HasAssociations for IfcRelAssociatesMaterial. When
     * should_inherit is true (default), falls back to the element's type.
     * When should_skip_usage is true, unwraps material set usages
     * (IfcMaterialLayerSetUsage, IfcMaterialProfileSetUsage) to their
     * underlying set.
     *
     * @param instance The element to query.
     * @param options Material lookup options.
     * @return The material entity, or no result if none is associated.
     */
    getMaterial(instance: Entity, options: IfcOpenShellElementGetMaterialOptions): Entity | null;
    /**
     * Return the nest parent of an element.
     *
     * Returns the RelatingObject of the applicable IfcRelNests relationship for
     * the schema.
     *
     * @param instance The element to query.
     * @return The nesting parent, or no result if the element is not nested.
     */
    getNest(instance: Entity): Entity | null;
    /**
     * Return opening elements associated with an element.
     *
     * Returns RelatedOpeningElement values from the element's opening
     * relationships. Also includes openings inherited from aggregate parents.
     *
     * @param element The building element (e.g. IfcWall).
     * @return List of IfcOpeningElement entities.
     */
    getOpenings(element: Entity): Entity[];
    /**
     * Return the parent of an element in the spatial hierarchy.
     *
     * Checks spatial containment, aggregation, nesting, filling, and voiding
     * relationships in that order, returning the first parent found.
     *
     * @param instance The element to query.
     * @return The parent element, or no result if the element is at the top of the hierarchy.
     */
    getParent(instance: Entity): Entity | null;
    /**
     * Return the direct aggregation parts of an element.
     *
     * Returns RelatedObjects from the element's IfcRelAggregates relationships.
     *
     * @param element The element to query.
     * @return List of aggregated parts.
     */
    getParts(element: Entity): Entity[];
    /**
     * Return property set and quantity identifiers of an element.
     *
     * Returns IfcPropertySet, IfcElementQuantity, and related property definition
     * entities. For IfcTypeObject, uses HasPropertySets; for other objects, uses
     * the applicable property-definition relationship.
     * When should_inherit is true (default), also includes property sets
     * from the element's type.
     *
     * @param element The element to query.
     * @param options Property set filtering options.
     * @return List of property definition entities.
     */
    getPsetIds(element: Entity, options: IfcOpenShellElementGetPsetIdsOptions): Entity[];
    /**
     * Return elements that have an external reference assigned.
     *
     * For IfcExternalReference subtypes, returns resources from the applicable
     * external-reference relationship. For classification, document, and library
     * references, returns elements from the corresponding IFC relationship.
     *
     * @param reference The IfcExternalReference or IfcExternalInformation entity.
     * @return List of elements using the reference.
     */
    getReferencedElements(reference: Entity): Entity[];
    /**
     * Return spatial elements that reference an element.
     *
     * Returns RelatingStructure values from the element's spatial reference
     * relationships. This includes multi-storey elements and elements spanning
     * multiple spaces.
     *
     * @param element The element to query.
     * @return List of referenced IfcSpatialElement entities.
     */
    getReferencedStructures(element: Entity): Entity[];
    /**
     * Return the shape aspects of an element.
     *
     * For an IfcProduct, returns shape aspects from its representation. For an
     * IfcTypeProduct, returns shape aspects from its representation maps. When
     * should_inherit is true (default), also includes shape aspects from
     * the element's type.
     *
     * @param element The IfcProduct or IfcTypeProduct.
     * @param options Shape aspect lookup options.
     * @return List of IfcShapeAspect entities.
     */
    getShapeAspects(element: Entity, options: IfcOpenShellElementGetShapeAspectsOptions): Entity[];
    /**
     * Return elements referenced by a spatial structure.
     *
     * Returns RelatedElements from the spatial element's reference relationships.
     *
     * @param structure The spatial element (e.g. IfcBuildingStorey).
     * @return List of referenced elements.
     */
    getStructureReferencedElements(structure: Entity): Entity[];
    /**
     * Return the surface styles used in an element's representation.
     *
     * Collects styles from the element's materials and body representation,
     * including styles on mapped items and boolean operands.
     *
     * @param element The element to query.
     * @return List of IfcSurfaceStyle entities.
     */
    getStyles(element: Entity): Entity[];
    /**
     * Return the type element associated with an element occurrence.
     *
     * For an IfcTypeObject, returns the element itself. For an IfcObject,
     * returns the type assigned through the schema's type relationship.
     *
     * @param instance The element to query.
     * @return The related type element, or no result if none is assigned.
     */
    getType(instance: Entity): Entity | null;
    /**
     * Return all occurrences of a type element.
     *
     * Returns the RelatedObjects of the applicable type relationship for the
     * schema.
     *
     * @param type_element The type element (e.g. IfcWallType).
     * @return List of element occurrences of that type.
     */
    getTypes(type_element: Entity): Entity[];
    /**
     * Return the building element voided by an opening.
     *
     * Returns the RelatingBuildingElement from the opening relationship.
     *
     * @param element The IfcOpeningElement.
     * @return The building element being voided, or no result if none is associated.
     */
    getVoidedElement(element: Entity): Entity | null;
    /**
     * Check whether an element's predefined type is user-defined.
     *
     * Returns true if the element's PredefinedType is USERDEFINED, or if
     * the type's PredefinedType is USERDEFINED or not set and an
     * ElementType or ObjectType is present.
     *
     * @param element The element to check.
     * @return True if the predefined type is user-defined.
     */
    isUserdefinedType(element: Entity): boolean;
    /**
     * Recursively remove an element and its owned subgraph.
     *
     * Removes the element and owned subelements that have no references outside
     * the removal set. Protected elements and externally referenced elements are
     * preserved.
     *
     * @param element The root element to remove.
     */
    removeDeep(element: Entity): void;
    /**
     * Replace all references to an element with another element.
     *
     * Replaces references to old_element in all inverse relationships with
     * references to new_element.
     *
     * @param old_element The element to be replaced.
     * @param new_element The replacement element.
     */
    replaceElement(old_element: Entity, new_element: Entity): void;
}
export interface EntityApi {
    /**
     * Remove an entity and the unshared entities it owns, recursively.
     *
     * Equivalent to entity_remove_deep_with_options with the default options.
     * The start element must have no inverses outside the subgraph.
     *
     * @param instance The root entity to remove.
     */
    removeDeep(instance: Entity): void;
    /**
     * Remove an entity and its owned subgraph with fine-grained control.
     *
     * An owned entity is removed only when it has no references from outside the
     * removal set. The also_consider list extends that set for this decision, and
     * the do_not_delete list protects specific entities from deletion.
     *
     * @param instance The root entity to remove.
     * @param options Additional control over the removal process.
     */
    removeDeepWithOptions(instance: Entity, options: IfcOpenShellEntityRemoveDeepOptions): void;
}
export interface FeatureApi {
    /**
     * Associate a feature element with a host element.
     *
     * For IfcFeatureElementSubtraction (e.g. IfcOpeningElement), creates
     * IfcRelVoidsElement. For IfcFeatureElementAddition (e.g. IfcProjectionElement),
     * creates IfcRelProjectsElement. For IfcSurfaceFeature, creates
     * IfcRelAdheresToElement (IFC4X3) or aggregates via IfcRelAggregates (IFC4).
     * If the feature is already associated with the same host, returns the
     * existing relationship. If associated with a different host, the old
     * relationship is removed first.
     */
    addFeature(file: IfcFile, options: IfcOpenShellFeatureAddFeatureOptions): Entity;
    /**
     * Fill an opening with an element via IfcRelFillsElement.
     *
     * If the element already fills the same opening, returns the existing
     * relationship. If it fills a different opening, the old relationship is
     * removed first.
     */
    addFilling(file: IfcFile, opening: Entity, element: Entity): Entity;
    /**
     * Remove a feature element and its relationships.
     *
     * For IfcFeatureElementSubtraction subclasses, removes the IfcRelVoidsElement.
     * For IfcFeatureElementAddition subclasses, removes the IfcRelProjectsElement.
     * For IfcSurfaceFeature in IFC4, unassigns from the aggregate parent. In
     * other schemas, no feature-specific relationship is removed before the
     * element itself is removed.
     * IfcOpeningElement fillings are also removed. Nested elements, property sets,
     * representations, and other inverse relationships are cleaned up as part of
     * removing the feature.
     */
    removeFeature(file: IfcFile, options: IfcOpenShellFeatureRemoveFeatureOptions): void;
    /**
     * Remove a filling relationship for an element.
     *
     * Finds and deletes the IfcRelFillsElement where the element is the
     * RelatedBuildingElement.
     */
    removeFilling(file: IfcFile, element: Entity): void;
}
export interface GeometryApi {
    /**
     * Create an axis representation (Curve2D or Curve3D) from a polyline.
     *
     * Points are converted from SI metres to project units.
     *
     * @param file IFC file that receives the representation.
     * @param context IfcGeometricRepresentationContext.
     * @param axis Ordered XY or XYZ points defining the axis curve.
     * @return IfcShapeRepresentation entity, or no result if creation fails.
     */
    addAxisRepresentation(file: IfcFile, context: Entity, axis: number[][]): Entity;
    /**
     * Add boolean operands to a solid representation item.
     *
     * Creates IfcBooleanResult entities (or IfcBooleanClippingResult for
     * DIFFERENCE with half-space solids) by combining the first item with each
     * additional operand. The returned entities are listed in creation order.
     *
     * @param file IFC file that receives the boolean entities.
     * @param first_item Base solid operand.
     * @param second_items Additional operands to apply.
     * @param operator_type Boolean operator: "DIFFERENCE", "UNION", or "INTERSECTION".
     * @return Created boolean result entities, or an empty list if creation fails.
     */
    addBoolean(file: IfcFile, first_item: Entity, second_items: Entity[], operator_type: string): Entity[];
    /**
     * Create a door representation with lining and panel geometry.
     *
     * @param file IFC file that receives the representation.
     * @param options Door dimensions, operation type, and lining/panel properties.
     * @return IfcShapeRepresentation entity, or no result if creation fails.
     */
    addDoorRepresentation(file: IfcFile, options: IfcOpenShellGeometryAddDoorRepresentationOptions): Entity;
    /**
     * Create a footprint representation (GeometricCurveSet) from curves.
     *
     * @param file IFC file that receives the representation.
     * @param context IfcGeometricRepresentationContext.
     * @param curves IfcCurve entities to include in the footprint.
     * @return IfcShapeRepresentation entity, or no result if creation fails.
     */
    addFootprintRepresentation(file: IfcFile, context: Entity, curves: Entity[]): Entity;
    /**
     * Create a mesh representation from vertices and face indices.
     *
     * Uses IfcPolygonalFaceSet (IFC4+) or IfcFacetedBrep (IFC2X3). IFC2X3
     * always uses faceted BRep regardless of force_faceted_brep.
     *
     * @param file IFC file that receives the representation.
     * @param context IfcGeometricRepresentationContext.
     * @param options Vertices, faces, and optional faceted BRep override.
     * @return IfcShapeRepresentation entity, or no result if creation fails.
     */
    addMeshRepresentation(file: IfcFile, context: Entity, options: IfcOpenShellGeometryAddMeshRepresentationOptions): Entity;
    /**
     * Create a profile-based IfcExtrudedAreaSolid representation.
     *
     * Depth and plane locations are supplied in SI metres and converted to project
     * length units. Clipping kinds preserve input order but are applied from last
     * to first; entity clippings are copied before their FirstOperand is changed.
     * The placement defaults to Z=(0,0,1), X=(1,0,0), and the origin when no
     * cardinal point is supplied. The result is SweptSolid without clippings and
     * Clipping otherwise.
     *
     * @param file IFC file that receives the representation.
     * @param options Context, profile, extrusion, placement, cardinal point, and clippings.
     * @return IfcShapeRepresentation entity, or no result if creation fails.
     */
    addProfileRepresentation(file: IfcFile, options: IfcOpenShellGeometryAddProfileRepresentationOptions): Entity;
    /**
     * Create a railing representation along a path.
     *
     * @param file IFC file that receives the representation.
     * @param options Railing path, support spacing, dimensions, and terminal type.
     * @return IfcShapeRepresentation entity, or no result if creation fails.
     */
    addRailingRepresentation(file: IfcFile, options: IfcOpenShellGeometryAddRailingRepresentationOptions): Entity;
    /**
     * Create or update an IfcShapeAspect within a product definition shape.
     *
     * If an aspect with the same name already exists, its items are merged.
     * Items are removed from other aspects in the same product definition shape.
     *
     * @param file IFC file that receives the aspect.
     * @param options Aspect name, items, representation, and owning product.
     * @return IfcShapeAspect entity, or no result if creation fails.
     */
    addShapeAspect(file: IfcFile, options: IfcOpenShellGeometryAddShapeAspectOptions): Entity;
    /**
     * Create a slab-style swept solid representation (IfcExtrudedAreaSolid).
     *
     * @param file IFC file that receives the representation.
     * @param options Slab dimensions, direction, clippings, and boundary polyline.
     * @return IfcShapeRepresentation entity, or no result if creation fails.
     */
    addSlabRepresentation(file: IfcFile, options: IfcOpenShellGeometryAddSlabRepresentationOptions): Entity;
    /**
     * Create an IfcTopologyRepresentation for a single topology item.
     *
     * @param file IFC file that receives the representation.
     * @param options Context, topology item, and optional identifier/type.
     * @return IfcTopologyRepresentation entity, or no result if creation fails.
     */
    addTopologyRepresentation(file: IfcFile, options: IfcOpenShellGeometryAddTopologyRepresentationOptions): Entity;
    /**
     * Create a wall-style swept solid representation (IfcExtrudedAreaSolid).
     *
     * @param file IFC file that receives the representation.
     * @param options Wall dimensions, direction, clippings, and booleans.
     * @return IfcShapeRepresentation entity, or no result if creation fails.
     */
    addWallRepresentation(file: IfcFile, options: IfcOpenShellGeometryAddWallRepresentationOptions): Entity;
    /**
     * Create a window representation with lining and panel geometry.
     *
     * @param file IFC file that receives the representation.
     * @param options Window dimensions, panel schema, lining/panel properties.
     * @return IfcShapeRepresentation entity, or no result if creation fails.
     */
    addWindowRepresentation(file: IfcFile, options: IfcOpenShellGeometryAddWindowRepresentationOptions): Entity;
    /**
     * Assign a representation to a product or type product.
     *
     * For IfcProduct, the representation is appended to the product's
     * IfcProductDefinitionShape. For IfcTypeProduct, an IfcRepresentationMap is
     * created and mapped representations are assigned to all related occurrences.
     * If the product has a type with existing maps and the representation is not
     * already mapped, the assignment is redirected to the type product.
     *
     * @param file IFC file to modify.
     * @param product IfcProduct or IfcTypeProduct entity.
     * @param representation IfcShapeRepresentation entity.
     * @return The product receiving the representation, or no result if assignment fails.
     */
    assignRepresentation(file: IfcFile, product: Entity, representation: Entity): Entity;
    /**
     * Clip a solid with an infinite half-space plane (IfcBooleanClippingResult).
     *
     * Creates a DIFFERENCE boolean between the item and an IfcHalfSpaceSolid.
     * If an owning element is provided, the clipping is registered in its
     * BBIM_Boolean property set.
     *
     * @param file IFC file that receives the clipping.
     * @param options Solid, plane point, normal, and optional element/history.
     * @return IfcBooleanClippingResult entity, or no result if creation fails.
     */
    clipSolid(file: IfcFile, options: IfcOpenShellGeometryClipSolidOptions): Entity;
    /**
     * Clip a solid with a polygonal-bounded half-space (IfcPolygonalBoundedHalfSpace).
     *
     * Creates a DIFFERENCE boolean between the item and an
     * IfcPolygonalBoundedHalfSpace. Locations are in model units.
     *
     * @param file IFC file that receives the clipping.
     * @param options Solid, plane, boundary polygon, and optional element/history.
     * @return IfcBooleanClippingResult entity, or no result if creation fails.
     */
    clipSolidBounded(file: IfcFile, options: IfcOpenShellGeometryClipSolidBoundedOptions): Entity;
    /**
     * Create an IfcRelConnectsElements between two elements.
     *
     * Removes any existing incompatible element connections between the two
     * elements before creating the new relationship.
     *
     * @param file IFC file that receives the relationship.
     * @param options Relating element, related element, and optional description/history.
     * @return IfcRelConnectsElements entity, or no result if creation fails.
     */
    connectElement(file: IfcFile, options: IfcOpenShellGeometryConnectElementOptions): Entity;
    /**
     * Create an IfcRelConnectsPathElements between two path-based elements.
     *
     * Removes incompatible existing path connections (same terminal end or same
     * pair) before creating the new relationship. Connection types are "ATSTART",
     * "ATEND", or "ATPATH".
     *
     * @param file IFC file that receives the relationship.
     * @param options Elements, connection types, and optional description/geometry/history.
     * @return IfcRelConnectsPathElements entity, or no result if creation fails.
     */
    connectPath(file: IfcFile, options: IfcOpenShellGeometryConnectPathOptions): Entity;
    /**
     * Connect two walls at their nearest endpoint.
     *
     * Computes the intersection of the second wall's axis with the first wall's
     * axis to determine ATSTART or ATEND. When is_atpath is true, the connection
     * type is ATPATH. Creates an IfcRelConnectsPathElements.
     *
     * @param file IFC file that receives the connection.
     * @param options Walls, connection mode, and optional owner history.
     * @return IfcRelConnectsPathElements entity, or no result if creation fails.
     */
    connectWall(file: IfcFile, options: IfcOpenShellGeometryConnectWallOptions): Entity;
    /**
     * Copy a representation from one product to another.
     *
     * Copies the "Body" (or specified context) representation from the source
     * product, replaces any existing representation of the same context on the
     * target, and assigns the copy.
     *
     * @param file IFC file to modify.
     * @param options Source product, target product, and optional context identifier.
     * @return Newly created IfcShapeRepresentation, or no value when the source
     * has no representation for the requested context identifier.
     */
    copyRepresentation(file: IfcFile, options: IfcOpenShellGeometryCopyRepresentationOptions): Entity | null;
    /**
     * Create a wall from two XY endpoints with body representation and placement.
     *
     * Computes the wall length from the endpoint distance, creates a wall
     * representation, and sets the object placement with rotation to align the
     * wall along the baseline.
     *
     * @param file IFC file that receives the wall geometry.
     * @param options Element, context, endpoints, elevation, height, thickness, and unit flag.
     * @return IfcShapeRepresentation entity, or no result if creation fails.
     */
    create2ptWall(file: IfcFile, options: IfcOpenShellGeometryCreate2PtWallOptions): Entity;
    /**
     * Remove all IfcRelConnectsElements relationships between two elements.
     *
     * Searches both ConnectedTo and ConnectedFrom inverse references on both
     * elements and removes matching relationships.
     *
     * @param file IFC file to modify.
     * @param relating_element First element.
     * @param related_element Second element.
     */
    disconnectElement(file: IfcFile, relating_element: Entity, related_element: Entity): void;
    /**
     * Remove path-based element connections.
     *
     * When element and connection_type are provided, removes all path connections
     * matching that type on the element. When relating_element and
     * related_element are provided, removes the specific connection between them.
     *
     * @param file IFC file to modify.
     * @param options Element, connection type, or relating/related element pair.
     */
    disconnectPath(file: IfcFile, options: IfcOpenShellGeometryDisconnectPathOptions): void;
    /**
     * Set the ObjectPlacement of a product from a 4x4 transformation matrix.
     *
     * Creates a new IfcLocalPlacement relative to the product's parent
     * (determined from aggregation, nesting, containment, or void relationships).
     * The old placement is removed if unreferenced. When
     * should_transform_children is false, child placements are re-applied in their
     * previous world-space positions.
     *
     * @param file IFC file to modify.
     * @param options Product, matrix, SI flag, and child transform flag.
     * @return Newly created IfcLocalPlacement, or no result if creation fails.
     */
    editObjectPlacement(file: IfcFile, options: IfcOpenShellGeometryEditObjectPlacementOptions): Entity;
    /**
     * Create a mapped representation (IfcRepresentationMap + IfcMappedItem).
     *
     * Wraps the given representation in an IfcRepresentationMap and returns a new
     * IfcShapeRepresentation of type "MappedRepresentation" containing one
     * IfcMappedItem.
     *
     * @param file IFC file that receives the mapped entities.
     * @param representation IfcShapeRepresentation to map.
     * @return New IfcShapeRepresentation with MappedRepresentation type.
     */
    mapRepresentation(file: IfcFile, representation: Entity): Entity;
    /**
     * Return the axis-aligned 2D bounding box extents of a profile.
     *
     * Computes the X and Y extents from the profile's parameterized attributes
     * (e.g. OverallWidth/OverallDepth for I-shaped profiles). When those values
     * are unavailable, geometry evaluation is used when available. Returns an
     * empty list if the extents cannot be determined.
     *
     * @param file IFC file containing the profile.
     * @param profile IfcProfileDef entity.
     * @return Two-element list {x_extent, y_extent} in model units, or an empty list.
     */
    profileExtents(file: IfcFile, profile: Entity): number[];
    /**
     * Regenerate a wall's body and axis representations from its material layers.
     *
     * Rebuilds the wall's body and axis representations using its material layers
     * and connected-wall geometry.
     *
     * @param file IFC file containing the wall.
     * @param options Wall entity, length, height, and optional angle.
     * @return New IfcShapeRepresentation for the body, or no result if regeneration fails.
     */
    regenerateWallRepresentation(file: IfcFile, options: IfcOpenShellGeometryRegenerateWallRepresentationOptions): Entity;
    /**
     * Remove boolean operands from a solid representation.
     *
     * Removes boolean operations involving the given item, restores the primary
     * operand in its parent references, and exposes the other operands in the
     * owning representation.
     *
     * @param file IFC file to modify.
     * @param item Solid operand whose boolean chain to remove.
     */
    removeBoolean(file: IfcFile, item: Entity): void;
    /**
     * Remove a representation and its unreferenced sub-entities.
     *
     * Cleans up styled items, presentation layer assignments, textures, and
     * colours. Geometric representation contexts are never deleted. Named
     * profiles are preserved when should_keep_named_profiles is true.
     *
     * @param file IFC file to modify.
     * @param representation IfcShapeRepresentation to remove.
     * @param options Whether to preserve named profiles.
     */
    removeRepresentation(file: IfcFile, representation: Entity, options: IfcOpenShellGeometryRemoveRepresentationOptions): void;
    /**
     * Unassign a representation from a product or type product.
     *
     * For IfcProduct, removes the representation from the
     * IfcProductDefinitionShape and removes an empty shape definition. For
     * IfcTypeProduct, removes the matching IfcRepresentationMap and unmaps
     * occurrences. Shape aspects referencing the representation are also removed.
     *
     * @param file IFC file to modify.
     * @param product IfcProduct or IfcTypeProduct entity.
     * @param representation IfcShapeRepresentation to unassign.
     */
    unassignRepresentation(file: IfcFile, product: Entity, representation: Entity): void;
    /**
     * Validate and correct a representation's RepresentationType.
     *
     * When the representation contains multiple boolean operands, they are
     * consolidated into a single boolean chain via UNION. Returns true if the
     * final representation type is "CSG" or was successfully auto-detected.
     *
     * @param file IFC file containing the representation.
     * @param representation IfcShapeRepresentation to validate.
     * @param options Optional preferred operand for boolean consolidation.
     * @return True if the representation type is valid or was corrected.
     */
    validateType(file: IfcFile, representation: Entity, options: IfcOpenShellGeometryValidateTypeOptions): boolean;
}
export interface GeoreferenceApi {
    /**
     * Add georeferencing entities to a model.
     *
     * Creates an IfcProjectedCRS and a coordinate operation (IfcMapConversion,
     * IfcMapConversionScaled, or IfcRigidOperation) linking the Model context
     * to the projected CRS. For IFC2X3, creates ePSet_MapConversion and
     * ePSet_ProjectedCRS property sets on the IfcProject instead. Does nothing
     * if georeferencing entities already exist.
     *
     * @param file File that receives the new entities.
     * @param options Coordinate operation class, CRS name, and ownership options.
     */
    addGeoreferencing(file: IfcFile, options: IfcOpenShellGeoreferenceAddGeoreferencingOptions): void;
    /**
     * Edit map conversion and projected CRS attributes.
     *
     * For IFC4+, applies attribute changes from the property bags to the first
     * IfcCoordinateOperation and/or IfcProjectedCRS in the file. For IFC2X3,
     * applies changes to the ePSet_MapConversion and ePSet_ProjectedCRS property
     * sets on the IfcProject. Entities must already exist (call
     * georeference_add_georeferencing first if needed).
     *
     * @param file File containing the georeferencing entities.
     * @param options Property bags for coordinate operation and projected CRS.
     */
    editGeoreferencing(file: IfcFile, options: IfcOpenShellGeoreferenceEditGeoreferencingOptions): void;
    /**
     * Set or remove the true north direction on all geometric representation contexts.
     *
     * When omitted, any existing TrueNorth reference is removed from every
     * IfcGeometricRepresentationContext. When provided, the first two values are
     * used as (X, Y) direction ratios; missing values default to 0.0. The
     * direction is not normalized.
     *
     * @param file File whose contexts to update.
     * @param options True north direction ratios, or omission to remove true north.
     */
    editTrueNorth(file: IfcFile, options: IfcOpenShellGeoreferenceEditTrueNorthOptions): void;
    /**
     * Edit the world coordinate system (WCS) on all geometric representation contexts.
     *
     * Replaces the WorldCoordinateSystem placement on every
     * IfcGeometricRepresentationContext. The rotation parameter specifies the
     * X-axis rotation in degrees. When is_si is true (default), coordinates are
     * interpreted as SI metres and scaled to project length units. The old WCS
     * placement is deleted if unreferenced.
     *
     * @param file File whose contexts to update.
     * @param options WCS offset, rotation, and unit-system options.
     */
    editWcs(file: IfcFile, options: IfcOpenShellGeoreferenceEditWcsOptions): void;
    /**
     * Remove all georeferencing entities from a model.
     *
     * For IFC4+, deletes all IfcProjectedCRS and IfcCoordinateOperation entities
     * (including orphaned MapUnit). For IFC2X3, removes the ePSet_MapConversion
     * and ePSet_ProjectedCRS property sets from the IfcProject.
     *
     * @param file File from which to remove georeferencing.
     */
    removeGeoreferencing(file: IfcFile): void;
}
export interface GridApi {
    /**
     * Create or replace the AxisCurve of a grid axis as a 2D IfcPolyline.
     *
     * Points are given in world coordinates; when is_si is true they are divided
     * by the file's LENGTHUNIT scale. The points are transformed into the grid's
     * local coordinate system using the grid's ObjectPlacement. An existing
     * AxisCurve is removed after replacement.
     *
     * @param file IFC file that receives the polyline.
     * @param p1 First endpoint (at least three coordinates; X and Y are used).
     * @param p2 Second endpoint (at least three coordinates; X and Y are used).
     * @param grid_axis IfcGridAxis whose AxisCurve to set.
     * @param is_si True if p1/p2 are in SI metres; false if already in model units.
     */
    createAxisCurve(file: IfcFile, p1: number[], p2: number[], grid_axis: Entity, is_si: boolean): void;
    /**
     * Create an IfcGridAxis and append it to the specified grid axis aggregate.
     *
     * @param file IFC file that receives the new axis.
     * @param grid IfcGrid entity owning the axis.
     * @param axis_tag Label for the axis (e.g. "A", "1").
     * @param same_sense True if the axis direction agrees with the curve direction.
     * @param uvw_axes Name of the grid aggregate to append to: "UAxes", "VAxes", or "WAxes".
     * @return Newly created IfcGridAxis, or no result if creation fails.
     */
    createGridAxis(file: IfcFile, grid: Entity, axis_tag: string, same_sense: boolean, uvw_axes: string): Entity;
    /**
     * Remove an IfcGridAxis and its associated AxisCurve.
     *
     * The axis entity and its associated AxisCurve are removed from the file.
     *
     * @param file IFC file to modify.
     * @param axis IfcGridAxis entity to remove.
     */
    removeGridAxis(file: IfcFile, axis: Entity): void;
}
export interface GroupApi {
    /** Create a new IfcGroup with generated GlobalId and OwnerHistory. */
    addGroup(file: IfcFile, options: IfcOpenShellGroupAddGroupOptions): Entity;
    /**
     * Assign products to a group via IfcRelAssignsToGroup.
     *
     * If an existing relationship is present, new products are merged into it.
     * Products already in the group are skipped.
     */
    assignGroup(file: IfcFile, options: IfcOpenShellGroupAssignGroupOptions): Entity;
    /**
     * Remove a group and its relationships.
     *
     * Removes associated IfcRelDefinesByProperties (property sets) and
     * IfcRelAssignsToGroup relationships, then deletes the group entity.
     */
    removeGroup(file: IfcFile, group: Entity): void;
    /**
     * Remove products from an IfcRelAssignsToGroup relationship.
     *
     * If no products remain in the relationship after removal, the relationship
     * is deleted.
     */
    unassignGroup(file: IfcFile, options: IfcOpenShellGroupUnassignGroupOptions): void;
    /**
     * Replace the product list of an existing IfcRelAssignsToGroup relationship.
     *
     * If no relationship exists, a new one is created. Child IfcGroup entities
     * from the existing relationship are preserved and merged with the new list.
     * Multiple existing relationships are consolidated into one.
     */
    updateGroupProducts(file: IfcFile, options: IfcOpenShellGroupUpdateGroupProductsOptions): Entity;
}
export interface GuidApi {
    compress(uuid_hex: string): string;
    expand(guid: string): string;
    generate(): string;
}
export interface LayerApi {
    /**
     * Create an IfcPresentationLayerAssignment with the given name.
     *
     * @param file IFC file that receives the layer.
     * @param name Layer name.
     * @return Newly created IfcPresentationLayerAssignment.
     */
    addLayer(file: IfcFile, name: string): Entity;
    /**
     * Create an IfcPresentationLayerWithStyle with visibility flags and styles.
     *
     * @param file IFC file that receives the layer.
     * @param name Layer name.
     * @param options Visibility flags and styles.
     * @return Newly created IfcPresentationLayerWithStyle.
     */
    addLayerWithStyle(file: IfcFile, name: string, options: IfcOpenShellLayerAddLayerWithStyleOptions): Entity;
    /**
     * Assign representation items to a presentation layer.
     *
     * Items already present in the layer's AssignedItems are not duplicated.
     *
     * @param file IFC file (unused, reserved for consistency).
     * @param items Representation items to assign.
     * @param layer IfcPresentationLayerAssignment or IfcPresentationLayerWithStyle.
     */
    assignLayer(file: IfcFile, items: Entity[], layer: Entity): void;
    /**
     * Remove a presentation layer entity from the file.
     *
     * @param file IFC file to modify.
     * @param layer IfcPresentationLayerAssignment or IfcPresentationLayerWithStyle to remove.
     */
    removeLayer(file: IfcFile, layer: Entity): void;
    /**
     * Remove representation items from a presentation layer.
     *
     * If all assigned items are removed, the layer entity itself is deleted from
     * the file. Does nothing silently when any specified item is not currently
     * assigned to the layer.
     *
     * @param file IFC file to modify.
     * @param items Representation items to unassign.
     * @param layer IfcPresentationLayerAssignment or IfcPresentationLayerWithStyle.
     */
    unassignLayer(file: IfcFile, items: Entity[], layer: Entity): void;
}
export interface LibraryApi {
    /** Create a new IfcLibraryInformation with the given name. */
    addLibrary(file: IfcFile, name: string): Entity;
    /**
     * Create a new IfcLibraryReference linked to a library.
     *
     * In IFC4+, the reference's ReferencedLibrary attribute is set. In IFC2X3,
     * the reference is appended to the library's LibraryReference aggregate.
     */
    addReference(file: IfcFile, library: Entity): Entity;
    /**
     * Assign a library reference to products via IfcRelAssociatesLibrary.
     *
     * Products already associated with the reference are skipped. If an
     * existing relationship is present, new products are merged into it.
     */
    assignReference(file: IfcFile, options: IfcOpenShellLibraryAssignReferenceOptions): Entity;
    /**
     * Remove an IfcLibraryInformation and all its references.
     *
     * Deletes all child IfcLibraryReference entities, the library entity itself,
     * and all associated IfcRelAssociatesLibrary relationships.
     */
    removeLibrary(file: IfcFile, library: Entity): void;
    /**
     * Remove an IfcLibraryReference and its association relationships.
     *
     * Deletes the reference entity and all IfcRelAssociatesLibrary relationships
     * that reference it.
     */
    removeReference(file: IfcFile, reference: Entity): void;
    /**
     * Remove products from an IfcRelAssociatesLibrary relationship.
     *
     * If no products remain after removal, the relationship is deleted.
     */
    unassignReference(file: IfcFile, options: IfcOpenShellLibraryUnassignReferenceOptions): void;
}
export interface MaterialApi {
    /** Add an IfcMaterialConstituent to an IfcMaterialConstituentSet. */
    addConstituent(file: IfcFile, constituent_set: Entity, options: IfcOpenShellMaterialAddConstituentOptions): Entity;
    /**
     * Add an IfcMaterialLayer to an IfcMaterialLayerSet.
     *
     * The layer thickness is initialized to 0.1 metres, converted to the file's
     * project units (e.g. 100 for a millimetre project).
     */
    addLayer(file: IfcFile, layer_set: Entity, options: IfcOpenShellMaterialAddLayerOptions): Entity;
    /** Append an IfcMaterial to an IfcMaterialList. */
    addListItem(file: IfcFile, material_list: Entity, material: Entity): void;
    /** Create a new IfcMaterial. */
    addMaterial(file: IfcFile, options: IfcOpenShellMaterialAddMaterialOptions): Entity;
    /**
     * Create a new material set of the requested type.
     *
     * Defaults to IfcMaterialConstituentSet if set_type is omitted.
     */
    addMaterialSet(file: IfcFile, options: IfcOpenShellMaterialAddMaterialSetOptions): Entity;
    /** Add an IfcMaterialProfile to an IfcMaterialProfileSet. */
    addProfile(file: IfcFile, profile_set: Entity, options: IfcOpenShellMaterialAddProfileOptions): Entity;
    /**
     * Assign a material to products via IfcRelAssociatesMaterial.
     *
     * Any existing material assignment on the products is removed first.
     * For "IfcMaterialLayerSetUsage" and "IfcMaterialProfileSetUsage", the
     * products are grouped by material set and direction; each group gets its
     * own usage entity and relationship. Returns the created relationship(s).
     */
    assignMaterial(file: IfcFile, products: Entity[], options: IfcOpenShellMaterialAssignMaterialOptions): Entity[];
    /**
     * Replace the profile on an IfcMaterialProfile and update representations.
     *
     * Sets the Profile attribute on the material profile. Updates the
     * SweptArea of any IfcSweptAreaSolid in representations of elements
     * using the parent IfcMaterialProfileSetUsage. The old profile is
     * deleted if it has no remaining references.
     */
    assignProfile(file: IfcFile, material_profile: Entity, profile: Entity): void;
    /**
     * Edit attributes of an IfcMaterialProfileSetUsage.
     *
     * Applies attribute key-value pairs from the props builder. If CardinalPoint
     * changes and both profile_width and profile_height are provided, the
     * ObjectPlacement of associated elements is updated to reflect the new
     * cardinal point position.
     */
    editProfileUsage(file: IfcFile, usage: Entity, options: IfcOpenShellMaterialEditProfileUsageOptions): void;
    /**
     * Remove an IfcMaterialConstituent from its set.
     *
     * If should_remove_material is true, the associated IfcMaterial is also
     * deleted.
     */
    removeConstituent(file: IfcFile, constituent: Entity, options: IfcOpenShellMaterialRemoveItemOptions): void;
    /**
     * Remove an IfcMaterialLayer from its set.
     *
     * If should_remove_material is true, the associated IfcMaterial is also
     * deleted.
     */
    removeLayer(file: IfcFile, layer: Entity, options: IfcOpenShellMaterialRemoveItemOptions): void;
    /** Remove an item from an IfcMaterialList by index. */
    removeListItem(file: IfcFile, material_list: Entity, options: IfcOpenShellMaterialRemoveListItemOptions): void;
    /**
     * Remove an IfcMaterial and its associated constituents, layers, and profiles.
     *
     * Deletes the material entity. Constituent, layer, or profile entities
     * that reference it are also removed. Associated IfcRelAssociatesMaterial,
     * IfcMaterialProperties, and IfcMaterialDefinitionRepresentation entities
     * are cleaned up.
     */
    removeMaterial(file: IfcFile, material: Entity): void;
    /**
     * Remove a material set and its items.
     *
     * Deletes all items (layers, profiles, constituents) in the set, then
     * removes the set entity. For IfcMaterialLayerSet and IfcMaterialProfileSet,
     * associated usage entities and their relationships are also deleted.
     * Associated IfcRelAssociatesMaterial and IfcMaterialProperties are
     * cleaned up.
     */
    removeMaterialSet(file: IfcFile, material: Entity): void;
    /**
     * Remove an IfcMaterialProfile from its set.
     *
     * Optionally deletes the associated IfcMaterial and/or IfcProfileDef
     * if the corresponding flags are set.
     */
    removeProfile(file: IfcFile, profile: Entity, options: IfcOpenShellMaterialRemoveProfileOptions): void;
    /**
     * Reorder an item within a material set.
     *
     * Moves the item from old_index to new_index within the set's aggregate
     * (MaterialConstituents, MaterialLayers, MaterialProfiles, or Materials).
     * Supports IfcMaterialConstituentSet, IfcMaterialLayerSet,
     * IfcMaterialProfileSet, and IfcMaterialList.
     */
    reorderSetItem(file: IfcFile, material_set: Entity, options: IfcOpenShellMaterialReorderSetItemOptions): void;
    /**
     * Remove material assignments from products.
     *
     * Deletes IfcRelAssociatesMaterial relationships for the products. If the
     * material is an IfcMaterialLayerSetUsage or IfcMaterialProfileSetUsage
     * with no remaining products, the usage entity is also deleted.
     */
    unassignMaterial(file: IfcFile, products: Entity[], options: IfcOpenShellMaterialUnassignMaterialOptions): void;
}
export interface NestApi {
    /**
     * Assign objects as nested children of a parent host via IfcRelNests.
     *
     * Objects previously nested under a different parent are moved. Objects
     * previously contained in a spatial structure or aggregated under a
     * different parent are removed from those relationships. If an existing
     * IfcRelNests relationship already relates to the same parent, objects
     * are merged into it while preserving insertion order.
     */
    assignObject(file: IfcFile, options: IfcOpenShellNestAssignObjectOptions): Entity;
    /**
     * Remove objects from their IfcRelNests relationships.
     *
     * If no related objects remain after removal, the IfcRelNests
     * relationship itself is deleted.
     */
    unassignObject(file: IfcFile, options: IfcOpenShellNestUnassignObjectOptions): void;
}
export interface OwnerApi {
    /**
     * Create an IfcActor (or subclass) entity.
     *
     * @param file File that receives the new entity.
     * @param options Actor identity and class options.
     * @return Newly created IfcActor or subclass.
     */
    addActor(file: IfcFile, options: IfcOpenShellOwnerAddActorOptions): Entity;
    /**
     * Add an IfcAddress subclass to an IfcOrganization or IfcPerson.
     *
     * Creates an entity of the given IFC class (e.g. "IfcTelecomAddress",
     * "IfcPostalAddress") with Purpose set to OFFICE and appends it to the
     * assigned object's Addresses aggregate.
     *
     * @param file File that receives the new address.
     * @param assigned_object IfcOrganization or IfcPerson to receive the address.
     * @param ifc_class IFC class name of the address to create.
     * @return Newly created IfcAddress subclass.
     */
    addAddress(file: IfcFile, assigned_object: Entity, ifc_class: string): Entity;
    /**
     * Create an IfcApplication entity.
     *
     * If application_developer is omitted, a default IfcOpenShell organisation
     * with a web-page address is created automatically.
     *
     * @param file File that receives the new entity.
     * @param options Application identity options.
     * @return Newly created IfcApplication.
     */
    addApplication(file: IfcFile, options: IfcOpenShellOwnerAddApplicationOptions): Entity;
    /**
     * Create an IfcOrganization entity.
     *
     * Uses the Identification attribute on IFC4+ and Id on IFC2X3.
     *
     * @param file File that receives the new entity.
     * @param identification Organisation identifier.
     * @param name Organisation name.
     * @return Newly created IfcOrganization.
     */
    addOrganisation(file: IfcFile, identification: string, name: string): Entity;
    /**
     * Create an IfcPerson entity.
     *
     * Uses the Identification attribute on IFC4+ and Id on IFC2X3.
     *
     * @param file File that receives the new entity.
     * @param identification Person identifier.
     * @param family_name Family (sur)name.
     * @param given_name Given (first) name.
     * @return Newly created IfcPerson.
     */
    addPerson(file: IfcFile, identification: string, family_name: string, given_name: string): Entity;
    /**
     * Create an IfcPersonAndOrganization entity linking a person and an organisation.
     *
     * @param file File that receives the new entity.
     * @param person IfcPerson to link.
     * @param organisation IfcOrganization to link.
     * @return Newly created IfcPersonAndOrganization.
     */
    addPersonAndOrganisation(file: IfcFile, person: Entity, organisation: Entity): Entity;
    /**
     * Add an IfcActorRole to an IfcOrganization, IfcPerson, or IfcPersonAndOrganization.
     *
     * If the role string matches a valid IFC enum value (e.g. "ARCHITECT"), it is
     * set directly. Otherwise, Role is set to USERDEFINED and UserDefinedRole
     * receives the string. The role is appended to the assigned object's Roles aggregate.
     *
     * @param file File that receives the new role.
     * @param assigned_object Entity to receive the role.
     * @param role Role name or user-defined string.
     * @return Newly created IfcActorRole.
     */
    addRole(file: IfcFile, assigned_object: Entity, role: string): Entity;
    /**
     * Assign an actor to an object via IfcRelAssignsToActor.
     *
     * If an IfcRelAssignsToActor already exists for this actor and object, the
     * existing relationship is returned. If the actor already has a relationship,
     * the object is appended to its RelatedObjects. Otherwise a new relationship
     * is created.
     *
     * @param file File that receives the relationship.
     * @param options Actor, object, and ownership options.
     * @return The IfcRelAssignsToActor relationship.
     */
    assignActor(file: IfcFile, options: IfcOpenShellOwnerAssignActorOptions): Entity;
    /**
     * Create a new IfcOwnerHistory.
     *
     * Sets CreationDate and LastModifiedDate to the current time, State to
     * READWRITE, and ChangeAction to ADDED. Both user and application are
     * required; if either is omitted, no owner history is created.
     *
     * @param file File that receives the new entity.
     * @param options User and application for the owner history.
     * @return Newly created IfcOwnerHistory, or no result if creation fails.
     */
    createOwnerHistory(file: IfcFile, options: IfcOpenShellOwnerCreateOwnerHistoryOptions): Entity;
    /**
     * Remove an IfcActor (or subclass) from the file.
     *
     * Removes the entity and its owner history.
     *
     * @param file File containing the actor.
     * @param actor IfcActor entity to remove.
     */
    removeActor(file: IfcFile, actor: Entity): void;
    /**
     * Remove an IfcAddress subclass from the file.
     *
     * If the address is the only entry in its parent's Addresses aggregate,
     * that aggregate is cleared before the entity is removed.
     *
     * @param file File containing the address.
     * @param address IfcAddress entity to remove.
     */
    removeAddress(file: IfcFile, address: Entity): void;
    /**
     * Remove an IfcApplication from the file.
     *
     * @param file File containing the application.
     * @param application IfcApplication entity to remove.
     */
    removeApplication(file: IfcFile, application: Entity): void;
    /**
     * Remove an IfcOrganization from the file.
     *
     * Cascades removal to roles, addresses, IfcPersonAndOrganization entities,
     * IfcActor entities, IfcOrganizationRelationship entities, and IfcApplication
     * entities that reference this organisation as their sole member.
     *
     * @param file File containing the organisation.
     * @param organisation IfcOrganization entity to remove.
     */
    removeOrganisation(file: IfcFile, organisation: Entity): void;
    /**
     * Remove an IfcPerson from the file.
     *
     * Cascades removal to roles, addresses, IfcPersonAndOrganization entities,
     * IfcActor entities, and IfcResourceLevelRelationship entities that reference
     * this person as their sole member.
     *
     * @param file File containing the person.
     * @param person IfcPerson entity to remove.
     */
    removePerson(file: IfcFile, person: Entity): void;
    /**
     * Remove an IfcPersonAndOrganization from the file.
     *
     * Cascades removal to IfcOwnerHistory, IfcActor, and
     * IfcResourceLevelRelationship entities that reference this entity as their
     * sole member.
     *
     * @param file File containing the person-and-organisation entity.
     * @param person_and_organisation IfcPersonAndOrganization entity to remove.
     */
    removePersonAndOrganisation(file: IfcFile, person_and_organisation: Entity): void;
    /**
     * Remove an IfcActorRole from the file.
     *
     * If the role is the sole entry in its parent's Roles aggregate, that
     * aggregate is cleared before the entity is removed.
     *
     * @param file File containing the role.
     * @param role IfcActorRole entity to remove.
     */
    removeRole(file: IfcFile, role: Entity): void;
    /**
     * Remove an object assignment from an actor via IfcRelAssignsToActor.
     *
     * If the relationship has only one RelatedObjects entry, the entire
     * relationship is removed. Otherwise the object is removed from the
     * RelatedObjects aggregate and the OwnerHistory is updated.
     *
     * @param file File containing the relationship.
     * @param options Actor and object to unassign.
     */
    unassignActor(file: IfcFile, options: IfcOpenShellOwnerUnassignActorOptions): void;
    /**
     * Update or create the OwnerHistory on an IfcRoot element.
     *
     * If the element already has an IfcOwnerHistory, its LastModifiedDate,
     * LastModifyingUser, and LastModifyingApplication are updated. If not,
     * a new IfcOwnerHistory is created and assigned. Requires both user and
     * application to be provided.
     *
     * @param file File containing the element.
     * @param options Element, user, and application.
     * @return The updated or newly created IfcOwnerHistory, or no result if the element is not an IfcRoot.
     */
    updateOwnerHistory(file: IfcFile, options: IfcOpenShellOwnerUpdateOwnerHistoryOptions): Entity;
}
export interface PlacementApi {
    /**
     * Extract a 4x4 row-major matrix from an IfcAxis2Placement entity.
     *
     * Supports IfcAxis2Placement2D, IfcAxis2Placement3D, and IfcAxis1Placement.
     * Returns an identity matrix if no instance is provided or the instance is unsupported.
     *
     * @param instance IfcAxis2Placement entity.
     * @return 16-element row-major 4x4 matrix.
     */
    getAxis2Placement(instance: Entity): number[];
    /**
     * Extract a 4x4 row-major matrix from an IfcCartesianTransformationOperator3D.
     *
     * Handles uniform and non-uniform scaling. Returns an identity matrix if no
     * instance is provided or the instance is not a cartesian transformation operator.
     *
     * @param instance IfcCartesianTransformationOperator3D entity.
     * @return 16-element row-major 4x4 matrix.
     */
    getCartesianXform3d(instance: Entity): number[];
    /**
     * Compute the cumulative 4x4 row-major world matrix of an IfcLocalPlacement.
     *
     * Combines the placement with its parent placements to compute the full
     * transformation. Returns an identity matrix when the placement is omitted.
     *
     * @param instance IfcLocalPlacement entity. When omitted, returns the identity matrix.
     * @return 16-element row-major 4x4 matrix.
     */
    getLocalPlacement(instance: Entity): number[];
    /**
     * Compute the combined 4x4 row-major matrix for an IfcMappedItem.
     *
     * Multiplies the MappingTarget transformation by the MappingOrigin placement.
     * Returns an identity matrix if no instance is provided or the instance is
     * not an IfcMappedItem.
     *
     * @param instance IfcMappedItem entity.
     * @return 16-element row-major 4x4 matrix.
     */
    getMappeditemXform(instance: Entity): number[];
    /**
     * Return the elevation of a building storey in model units.
     *
     * Uses the Z-translation of the storey's ObjectPlacement when available,
     * falling back to the Elevation attribute. Returns 0.0 if no instance is
     * provided or the instance has no placement.
     *
     * @param instance IfcBuildingStorey entity.
     * @return Elevation in model units.
     */
    getStoreyElevation(instance: Entity): number;
    /**
     * Compute a 4x4 row-major transformation matrix from origin and axes.
     *
     * @param origin XYZ origin of the placement.
     * @param z_axis Direction ratios for the Z axis.
     * @param x_axis Direction ratios for the X axis (Y is derived).
     * @return 16-element row-major 4x4 matrix.
     */
    matrixFromAxes(origin: number[], z_axis: number[], x_axis: number[]): number[];
    /**
     * Build a 4x4 row-major rotation matrix about a principal axis.
     *
     * @param angle_rad Rotation angle in radians.
     * @param axis Rotation axis: "X", "Y", or "Z" (case-insensitive).
     * @return 16-element row-major 4x4 rotation matrix.
     */
    rotation(angle_rad: number, axis: string): number[];
}
export interface ProfileApi {
    /**
     * Create an IfcArbitraryClosedProfileDef from an ordered polyline.
     *
     * Points are converted from SI metres to project units before creating the
     * outer curve. The curve representation is chosen automatically (IfcPolyline
     * for IFC2X3, IfcIndexedPolyCurve otherwise).
     *
     * @param file IFC file that receives the profile.
     * @param options Outer curve points and optional name.
     * @return Newly created IfcArbitraryClosedProfileDef.
     */
    addArbitraryProfile(file: IfcFile, options: IfcOpenShellProfileAddArbitraryProfileOptions): Entity;
    /**
     * Create an IfcArbitraryProfileDefWithVoids from an outer curve and inner voids.
     *
     * Points are converted from SI metres to project units. The outer curve uses
     * IfcCartesianPointList3D for IFC4+ schemas and IfcPolyline for IFC2X3.
     *
     * @param file IFC file that receives the profile.
     * @param options Outer curve, inner void curves, and optional name.
     * @return Newly created IfcArbitraryProfileDefWithVoids.
     */
    addArbitraryProfileWithVoids(file: IfcFile, options: IfcOpenShellProfileAddArbitraryProfileWithVoidsOptions): Entity;
    /**
     * Create a parameterized IFC profile definition.
     *
     * @param file IFC file that receives the profile.
     * @param ifc_class IFC entity class name (e.g. "IfcRectangleProfileDef").
     * @param profile_type Profile type string, typically "AREA".
     * @return Newly created IfcProfileDef entity.
     */
    addParameterizedProfile(file: IfcFile, ifc_class: string, profile_type: string): Entity;
    /**
     * Copy a profile and its associated IfcProfileProperties.
     *
     * @param file IFC file that receives the copied profile.
     * @param profile IfcProfileDef entity to copy.
     * @return Newly created independent copy of the profile.
     */
    copyProfile(file: IfcFile, profile: Entity): Entity;
    /**
     * Edit attributes of an existing profile definition.
     *
     * @param profile IfcProfileDef entity to modify.
     * @param attributes Attribute name-to-value mapping.
     */
    editProfile(profile: Entity, attributes: PsetProperties | PsetInput): void;
    /**
     * Remove a profile definition and its directly referenced sub-entities.
     *
     * Removes associated IfcProfileProperties first, then removes the profile
     * entity and removes unreferenced entities belonging to its direct geometry,
     * such as curves and placements.
     *
     * @param file IFC file to modify.
     * @param profile IfcProfileDef entity to remove.
     */
    removeProfile(file: IfcFile, profile: Entity): void;
}
export interface ProjectApi {
    /**
     * Declare objects to a project or project library context.
     *
     * Creates or updates an IfcRelDeclares relationship linking the given
     * definitions to the relating context. If the definitions are already
     * declared to a different context, they are moved.
     */
    assignDeclaration(file: IfcFile, options: IfcOpenShellProjectAssignDeclarationOptions): Entity;
    /**
     * Remove declarations of objects from a project or project library context.
     *
     * Removes the definitions from any existing IfcRelDeclares relationship.
     * If no definitions remain, the relationship itself is removed.
     */
    unassignDeclaration(file: IfcFile, options: IfcOpenShellProjectUnassignDeclarationOptions): void;
}
export interface PsetApi {
    /**
     * Add or return an existing property set attached to an object, context, type,
     * material, or profile definition.
     *
     * For IfcObject/IfcContext, creates IfcRelDefinesByProperties. For
     * IfcTypeObject, appends to HasPropertySets. For IfcMaterial, creates
     * IfcMaterialProperties (or IfcExtendedMaterialProperties in IFC2X3).
     * For IfcProfileDef, creates IfcProfileProperties (or
     * IfcGeneralProfileProperties in IFC2X3). If a property set with the
     * same name already exists on the product, it is returned without
     * creating a new one.
     */
    addPset(file: IfcFile, options: IfcOpenShellPsetAddPsetOptions): Entity;
    /**
     * Add or return an existing quantity set (IfcElementQuantity) on an object.
     *
     * If the name ends with "BaseQuantities", MethodOfMeasurement is set to
     * "BaseQuantities". If a quantity set with the same name already exists,
     * it is returned without creating a new one.
     */
    addQto(file: IfcFile, options: IfcOpenShellPsetAddQtoOptions): Entity;
    /**
     * Assign a property set to products.
     *
     * For IfcTypeProduct instances, the pset is appended to HasPropertySets.
     * For other products, creates or updates an IfcRelDefinesByProperties
     * relationship.
     */
    assignPset(file: IfcFile, options: IfcOpenShellPsetAssignPsetOptions): Entity;
    /**
     * Edit properties on an existing property set.
     *
     * Updates existing properties in-place (when not shared with other psets),
     * adds new properties for keys not yet present, and optionally removes
     * blank-valued properties. Uses the pset template for type inference when
     * available. Returns true on success, false on error.
     */
    editPset(file: IfcFile, options: IfcOpenShellPsetEditPsetOptions): boolean;
    /**
     * Edit quantities on an existing quantity set.
     *
     * Updates existing quantities in-place, adds new quantities for keys not
     * yet present. Quantity types (Length, Area, Volume, Count, Weight, Time)
     * are inferred from the value type and name. Returns true on success,
     * false on error.
     */
    editQto(file: IfcFile, options: IfcOpenShellPsetEditQtoOptions): boolean;
    /**
     * Remove a property set from a specific product.
     *
     * Removes the IfcRelDefinesByProperties relationship between the product
     * and pset. If the pset has no remaining associations, the pset and its
     * properties are deleted.
     */
    removePset(file: IfcFile, product: Entity, pset: Entity): void;
    /**
     * Create a new IfcSimplePropertyTemplate and add it to a property set template.
     *
     * Property templates are kept sorted by name within the parent template.
     * If template_type is omitted, defaults to "P_SINGLEVALUE" for psets or
     * "Q_LENGTH" for qtos. If primary_measure_type is omitted, defaults to
     * "IfcLabel" for psets.
     */
    templateAddPropTemplate(file: IfcFile, pset_template: Entity, name: string, description: string, template_type: string, primary_measure_type: string): Entity;
    /**
     * Create a new IfcPropertySetTemplate in the file.
     *
     * @param file The IFC file.
     * @param name Template name (e.g. "Pset_WallCommon").
     * @param template_type Template type string (e.g. "PSET_TYPEDRIVENOVERRIDE", "QTO").
     * @param applicable_entity Applicable IFC entity name (e.g. "IfcWall").
     */
    templateAddPsetTemplate(file: IfcFile, name: string, template_type: string, applicable_entity: string): Entity;
    /**
     * Create a property template collection from custom IFC template files.
     *
     * Loads IfcPropertySetTemplate and IfcSimplePropertyTemplate entities from
     * the provided files. The returned collection remains valid until it is
     * released with pset_template_free.
     */
    templateCreateFromFiles(schema_identifier: string, template_files: IfcFile[]): PsetTemplate | null;
    /**
     * Return property set templates applicable to an IFC class and predefined type.
     *
     * Filters by pset_only (PSET templates) or qto_only (QTO templates).
     * If neither flag is set, returns both types. When predefined_type or
     * schema_name is omitted, the default is used.
     */
    templateGetApplicable(pqt: PsetTemplate, ifc_class: string, predefined_type: string, pset_only: boolean, qto_only: boolean, schema_name: string): Entity[];
    /**
     * Return names of property set templates applicable to an IFC class.
     *
     * Same filtering as pset_template_get_applicable but returns only the
     * Name strings.
     */
    templateGetApplicableNames(pqt: PsetTemplate, ifc_class: string, predefined_type: string, pset_only: boolean, qto_only: boolean, schema_name: string): string[];
    /**
     * Look up a property set template by name.
     *
     * Returns the IfcPropertySetTemplate entity with the given name, or a
     * no result if the template is not found.
     */
    templateGetByName(pqt: PsetTemplate, name: string): Entity;
    /**
     * Return the cached property template collection for the given schema (e.g. "IFC4", "IFC2X3").
     *
     * Loads and caches the built-in templates on first call. Returns no result
     * if the schema is unknown or templates are not available.
     */
    templateGetTemplate(schema_identifier: string): PsetTemplate | null;
    /** Return true if a property set template with the given name exists. */
    templateIsTemplated(pqt: PsetTemplate, name: string): boolean;
    /**
     * Return the template type category for a property set template.
     *
     * Returns "PSET" for property set templates, "QTO" for quantity templates,
     * or an empty string if unknown.
     */
    templatePsetType(pset_template: Entity): string;
    /**
     * Remove a property template from its parent set template.
     *
     * Removes the IfcSimplePropertyTemplate from its parent and deletes the
     * template entity.
     */
    templateRemovePropTemplate(file: IfcFile, prop_template: Entity): void;
    /**
     * Remove a property set template and its child property templates.
     *
     * Deletes the IfcPropertySetTemplate and all its child
     * IfcSimplePropertyTemplate entities.
     */
    templateRemovePsetTemplate(pset_template: Entity): void;
    /**
     * Set the directory used to locate built-in pset/qto template files.
     *
     * Must be called before pset_template_get_template to override the default
     * search path for IFC property set template files.
     */
    templateSetTemplateDir(dir: string): void;
    /**
     * Remove products from a property set's associations.
     *
     * For IfcTypeProduct instances, removes the pset from HasPropertySets.
     * For other products, removes them from IfcRelDefinesByProperties
     * relationships. Orphaned relationships are deleted. The pset itself is
     * not deleted.
     */
    unassignPset(file: IfcFile, products: Entity[], pset: Entity): void;
    /**
     * Unshare a property set by creating independent copies for specified products.
     *
     * When all assigned products are selected, one product retains the original
     * and the other products receive copies. When only some products are
     * selected, each selected product receives a copy and the original remains
     * assigned to the unselected products. Returns the newly created copies.
     */
    unsharePset(file: IfcFile, options: IfcOpenShellPsetUnsharePsetOptions): Entity[];
}
export interface RegisterApi {
    /**
     * Register an IFC file for schema-aware derived-value evaluation.
     *
     * The registered file is used when evaluating derived attributes for the
     * specified schema.
     *
     * @param schema_name The IFC schema identifier (e.g. "IFC4").
     * @param file The IFC file to register.
     * @return True after the file is registered.
     */
    scratchFile(schema_name: string, file: IfcFile): boolean;
}
export interface RepresentationApi {
    /**
     * Return a geometric representation context matching the given criteria.
     *
     * Searches for IfcGeometricRepresentationSubContext when subcontext or
     * target_view is provided, otherwise searches for
     * IfcGeometricRepresentationContext (excluding subcontexts).
     *
     * @param file The IFC file to search.
     * @param context_type Context type filter (e.g. "Model", "Plan").
     * @param subcontext Context identifier filter (e.g. "Body", "Axis").
     * @param target_view Target view filter (e.g. "MODEL_VIEW", "GRAPH_VIEW").
     * @return The first matching context, or no result if none is found.
     */
    getContext(file: IfcFile, context_type: string, subcontext: string, target_view: string): Entity;
    /**
     * Return all geometric representation contexts sorted by priority.
     *
     * Sorts by ContextType (Model > Plan > Annotation), then by
     * ContextIdentifier (Body > Body-FallBack > ...), then by
     * TargetView (MODEL_VIEW > PLAN_VIEW > ...), then by TargetScale. Ties
     * preserve the order of contexts in the IFC file.
     *
     * @param file The IFC file to search.
     * @return Ordered list of IfcGeometricRepresentationContext entities.
     */
    getPrioritisedContexts(file: IfcFile): Entity[];
    /**
     * Return a product's representation filtered by context.
     *
     * For an IfcProduct, reads Representation/Representations. For an
     * IfcTypeProduct, reads RepresentationMaps/MappedRepresentation.
     * Returns the first representation matching the context filters.
     *
     * @param element The IfcProduct or IfcTypeProduct.
     * @param options Context filtering options.
     * @return The matching IfcShapeRepresentation, or no result if none is found.
     */
    getProductRepresentation(element: Entity, options: IfcOpenShellRepresentationGetProductRepresentationOptions): Entity;
    /**
     * Resolve a representation through single mapped items.
     *
     * If a representation contains a single IfcMappedItem whose
     * MappingSource points to another representation, this function
     * follows the chain and returns the innermost representation. A representation
     * that does not meet this condition is returned unchanged.
     *
     * @param representation The IfcShapeRepresentation to resolve.
     * @return The resolved representation, or the original when no mapping is followed.
     */
    resolve(representation: Entity): Entity;
    /**
     * Return the base items of a representation, unwrapping mapped items and boolean operands.
     *
     * Returns leaf-level representation items in traversal order. Within each
     * representation, later items are returned before earlier items; for boolean
     * results, the second operand is returned before the first operand.
     *
     * @param representation The IfcShapeRepresentation to resolve.
     * @return List of leaf-level IfcRepresentationItem entities.
     */
    resolveBaseItems(representation: Entity): Entity[];
}
export interface ResourceApi {
    /**
     * Edit attributes of an IfcResourceTime entity.
     *
     * Applies attribute changes from the property bag to the resource time.
     * Attributes that are hard-constrained (ConstraintGrade HARD and Benchmark
     * EQUALTO) on the owning IfcResource are skipped. If ScheduleUsage is
     * changed and a ScheduleWork constraint exists, the associated IfcTask
     * duration is recalculated. Duration values use ISO 8601 format
     * (e.g. "PT8H", "P5D"). Date-time values use ISO 8601 extended format
     * (e.g. "2024-01-15T09:00:00").
     *
     * @param file File containing the resource time.
     * @param resource_time IfcResourceTime entity to edit.
     * @param attributes Property bag of attribute name/value pairs.
     */
    editResourceTime(file: IfcFile, resource_time: Entity, attributes: PsetProperties | PsetInput): void;
}
export interface RootApi {
    /**
     * Create an IFC entity with generated identity, optional name, and optional
     * predefined type.
     *
     * Sets GlobalId (for IfcRoot-derived entities). OwnerHistory is assigned only
     * when the owner_history option is provided; it is not created
     * automatically. Schema-specific defaults are applied for spatial elements,
     * element types, and door/window styles. If the predefined type is not a valid
     * enum value, it is stored as USERDEFINED with the value in ObjectType
     * (or ElementType/ProcessType where applicable).
     */
    createEntity(file: IfcFile, options: IfcOpenShellRootCreateEntityOptions): Entity;
    /**
     * Remove a product and all its relationships.
     *
     * Removes the product and cleans up its related representations, object placements,
     * opening elements, property sets, material assignments, type definitions,
     * space boundaries, nesting relationships, aggregate relationships, spatial
     * containment, element connections, port connections, group memberships,
     * and grid axes. Related entities are removed only when they are no longer
     * needed by the remaining model.
     */
    removeProduct(file: IfcFile, product: Entity, options: IfcOpenShellRootRemoveProductOptions): void;
}
export interface SchemaApi {
    /**
     * Change the IFC class of an entity.
     *
     * Removes the element and recreates it as the new class with the same
     * STEP id. Compatible attributes are preserved; incompatible attributes
     * are dropped. Inverse references to the old element are updated to
     * point to the new element.
     *
     * If the element is already of the requested class, returns it unchanged.
     *
     * @param file IFC file to modify. When omitted, the element's file is used.
     * @param element The entity to reassign.
     * @param new_class The target IFC class name (e.g. "IfcWall").
     * @return The new entity of the requested class, or no result if the operation fails.
     */
    reassignClass(file: IfcFile, element: Entity, new_class: string): Entity;
}
export interface SelectorApi {
    /**
     * Filter all elements in a file using a selector query.
     *
     * Parses the query as a filter expression and returns all matching
     * elements as a list value.
     *
     * @param file The IFC file to search.
     * @param query The filter query string.
     * @return List value of matching elements, or no result if the query cannot be evaluated. Release it with value_free.
     */
    filterAll(file: IfcFile, query: string): ValueData | null;
    /**
     * Filter a list of elements using a selector query.
     *
     * Parses the query as a filter expression and returns the subset
     * of elements that match.
     *
     * @param file The IFC file context.
     * @param query The filter query string.
     * @param elements The elements to filter.
     * @return List value of matching elements, or no result if the query cannot be evaluated. Release it with value_free.
     */
    filterElements(file: IfcFile, query: string, elements: Entity[]): ValueData | null;
    /**
     * Format a selector query against an element.
     *
     * Evaluates a format expression (e.g. attribute names, arithmetic,
     * string functions) against an element and returns the formatted string.
     *
     * @param file Optional IFC file context.
     * @param instance The element to format against.
     * @param query The format expression string.
     * @return The formatted result, or no result if evaluation fails.
     */
    format(file: IfcFile, instance: Entity, query: string): string | null;
    /**
     * Get a value from an element using a selector query.
     *
     * Parses the query as a get-element key path and extracts the
     * corresponding value from the element's attributes.
     *
     * @param file Optional IFC file context.
     * @param element The element to query.
     * @param query The selector key path (e.g. "Name", "Pset_WallCommon.FireRating").
     * @return The extracted value, or no result if the query cannot be evaluated. Release it with value_free.
     */
    getElementValue(file: IfcFile, element: Entity, query: string): ValueData | null;
    /**
     * Set a value on an element using a selector query.
     *
     * Parses the query to identify the target attribute (e.g. a property
     * set property or direct attribute) and sets it to the provided value.
     *
     * @param file The IFC file context.
     * @param element The element to modify.
     * @param query The selector key path identifying the target.
     * @param value The value to set. When omitted, the target is unset.
     * @param concat When provided and non-empty, it is prepended to the value.
     */
    setElementValue(file: IfcFile, element: Entity, query: string, value: ValueInput | null, concat: string): void;
}
export interface SequenceApi {
    /**
     * Parse an ISO 8601 date-time string and return a schema-appropriate representation.
     *
     * For IFC4+, returns the string as-is. For IFC2X3, returns an IfcDateAndTime
     * entity constructed from the parsed components.
     *
     * @param file File to determine the schema version.
     * @param date_time ISO 8601 date-time string (e.g. "2024-01-15T09:00:00").
     * @return An IfcDateAndTime entity (IFC2X3) or the original string (IFC4+).
     */
    addDateTime(file: IfcFile, date_time: string): Entity | string;
    /**
     * Create an IfcTask entity.
     *
     * When both work_schedule and parent_task are provided, work_schedule takes
     * precedence and the task is assigned to it via IfcRelAssignsToControl.
     * When only parent_task is provided, the task is nested under it via
     * IfcRelNests and its Identification is auto-generated as "parent.N" on IFC4+.
     *
     * @param file File that receives the new entity.
     * @param options Task placement and attribute options.
     * @return Newly created IfcTask.
     */
    addTask(file: IfcFile, options: IfcOpenShellSequenceAddTaskOptions): Entity;
    /**
     * Create an IfcTaskTime (or IfcTaskTimeRecurring) and assign it to a task.
     *
     * Replaces any existing TaskTime on the task.
     *
     * @param file File that receives the new entity.
     * @param task IfcTask to receive the task time.
     * @param options Whether to create a recurring task time.
     * @return Newly created IfcTaskTime or IfcTaskTimeRecurring.
     */
    addTaskTime(file: IfcFile, task: Entity, options: IfcOpenShellSequenceAddTaskTimeOptions): Entity;
    /**
     * Create an IfcTimePeriod and add it to a recurrence pattern.
     *
     * @param file File that receives the new entity.
     * @param recurrence_pattern IfcRecurrencePattern to receive the time period.
     * @param options Start and end time strings.
     * @return Newly created IfcTimePeriod.
     */
    addTimePeriod(file: IfcFile, recurrence_pattern: Entity, options: IfcOpenShellSequenceAddTimePeriodOptions): Entity;
    /**
     * Create an IfcWorkCalendar and declare it on the project.
     *
     * @param file File that receives the new entity.
     * @param options Calendar name, predefined type, and ownership options.
     * @return Newly created IfcWorkCalendar.
     */
    addWorkCalendar(file: IfcFile, options: IfcOpenShellSequenceAddWorkCalendarOptions): Entity;
    /**
     * Create an IfcWorkPlan and declare it on the project.
     *
     * @param file File that receives the new entity.
     * @param options Work plan name, dates, creator, and ownership options.
     * @return Newly created IfcWorkPlan.
     */
    addWorkPlan(file: IfcFile, options: IfcOpenShellSequenceAddWorkPlanOptions): Entity;
    /**
     * Create an IfcWorkSchedule.
     *
     * When work_plan is provided, the schedule is nested under it via
     * IfcRelAggregates. Otherwise it is declared directly on the project
     * context (IFC4+).
     *
     * @param file File that receives the new entity.
     * @param options Schedule name, type, dates, work plan, and ownership options.
     * @return Newly created IfcWorkSchedule.
     */
    addWorkSchedule(file: IfcFile, options: IfcOpenShellSequenceAddWorkScheduleOptions): Entity;
    /**
     * Create an IfcWorkTime and add it to a work calendar.
     *
     * @param file File that receives the new entity.
     * @param work_calendar IfcWorkCalendar to receive the work time.
     * @param time_type "WorkingTimes" or "ExceptionTimes".
     * @return Newly created IfcWorkTime.
     */
    addWorkTime(file: IfcFile, work_calendar: Entity, time_type: string): Entity;
    /**
     * Create an IfcLagTime and assign it to an IfcRelSequence.
     *
     * Replaces any existing TimeLag on the sequence (removing the old one if
     * it is sole-referenced).
     *
     * @param file File that receives the new entity.
     * @param rel_sequence IfcRelSequence to receive the lag time.
     * @param lag_value ISO 8601 duration string (e.g. "P2D", "PT8H").
     * @param options Duration type option. Defaults to "WORKTIME".
     * @return Newly created IfcLagTime.
     */
    assignLagTime(file: IfcFile, rel_sequence: Entity, lag_value: string, options: IfcOpenShellSequenceAssignLagTimeOptions): Entity;
    /**
     * Assign a process (IfcTask) to an object via IfcRelAssignsToProcess.
     *
     * If a relationship already exists for this process and object, it is
     * returned. Otherwise a new relationship is created or the object is
     * appended to an existing relationship.
     *
     * @param file File that receives the relationship.
     * @param relating_process IfcTask (process) to assign.
     * @param related_object Object to assign to the process.
     * @param options Ownership options.
     * @return The IfcRelAssignsToProcess relationship.
     */
    assignProcess(file: IfcFile, relating_process: Entity, related_object: Entity, options: IfcOpenShellSequenceAssignProcessOptions): Entity;
    /**
     * Assign a product to an object via IfcRelAssignsToProduct.
     *
     * @param file File that receives the relationship.
     * @param relating_product IfcProduct to assign.
     * @param related_object Object to assign to the product.
     * @param options Ownership options.
     * @return The IfcRelAssignsToProduct relationship.
     */
    assignProduct(file: IfcFile, relating_product: Entity, related_object: Entity, options: IfcOpenShellSequenceAssignProductOptions): Entity;
    /**
     * Create an IfcRecurrencePattern and assign it to a parent entity.
     *
     * Assigns the pattern to an IfcWorkTime's RecurrencePattern or an
     * IfcTaskTimeRecurring's Recurrence attribute. Replaces any existing
     * pattern on the parent (removing the old one if sole-referenced).
     *
     * @param file File that receives the new entity.
     * @param parent IfcWorkTime or IfcTaskTimeRecurring to receive the pattern.
     * @param recurrence_type RecurrenceType enum value (e.g. "WEEKLY").
     * @return Newly created IfcRecurrencePattern.
     */
    assignRecurrencePattern(file: IfcFile, parent: Entity, recurrence_type: string): Entity;
    /**
     * Create an IfcRelSequence linking two tasks.
     *
     * If a sequence relationship already exists between the two tasks, the
     * existing relationship is returned. After creating the relationship,
     * cascades schedule changes from the relating (predecessor) task.
     *
     * @param file File that receives the relationship.
     * @param relating_process Predecessor IfcTask.
     * @param related_process Successor IfcTask.
     * @param options Sequence type and ownership options.
     * @return The IfcRelSequence relationship.
     */
    assignSequence(file: IfcFile, relating_process: Entity, related_process: Entity, options: IfcOpenShellSequenceAssignSequenceOptions): Entity;
    /**
     * Assign an IfcWorkSchedule to an IfcWorkPlan via IfcRelAggregates.
     *
     * First unassigns the schedule from its current project declaration, then
     * nests it under the work plan.
     *
     * @param file File containing both entities.
     * @param work_schedule IfcWorkSchedule to assign.
     * @param work_plan IfcWorkPlan to assign to.
     * @param options Ownership options.
     * @return The IfcRelAggregates relationship.
     */
    assignWorkPlan(file: IfcFile, work_schedule: Entity, work_plan: Entity, options: IfcOpenShellSequenceAssignWorkPlanOptions): Entity;
    /**
     * Recalculate a task's ScheduleDuration from its assigned resources.
     *
     * Computes the maximum resource usage duration across all IfcConstructionResource
     * entities assigned to the task (directly or via IfcRelAssignsToProcess),
     * accounting for ScheduleWork and ScheduleUsage. The result is written to the
     * task's IfcTaskTime.ScheduleDuration. Creates an IfcTaskTime if absent.
     *
     * @param file File containing the task.
     * @param task IfcTask entity to recalculate.
     */
    calculateTaskDuration(file: IfcFile, task: Entity): void;
    /**
     * Cascade schedule dates from a task to all its successors.
     *
     * Propagates ScheduleStart and ScheduleFinish dates forward through the
     * task network, respecting sequence types (FS, SS, FF, SF), lag times,
     * duration types, and work calendars.
     *
     * @param file File containing the task network.
     * @param task IfcTask from which to cascade.
     */
    cascadeSchedule(file: IfcFile, task: Entity): void;
    /**
     * Create an independent copy of an IfcWorkSchedule and its controlled tasks.
     *
     * Copies the schedule and each controlled IfcTask, including its subtasks and
     * relationships, then assigns the copies to the new schedule.
     *
     * @param file File that receives the copied entities.
     * @param work_schedule IfcWorkSchedule to copy.
     * @param options Ownership options for the copied entities.
     * @return Newly created IfcWorkSchedule.
     */
    copyWorkSchedule(file: IfcFile, work_schedule: Entity, options: IfcOpenShellSequenceCopyWorkScheduleOptions): Entity;
    /**
     * Create a baseline snapshot of an IfcWorkSchedule.
     *
     * Only operates on schedules with PredefinedType PLANNED. Creates a new
     * IfcWorkSchedule with PredefinedType BASELINE, deep-copies all controlled
     * tasks into it, and creates IfcRelDefinesByObject references linking
     * original tasks to their baseline copies.
     *
     * @param file File that receives the baseline entities.
     * @param work_schedule IfcWorkSchedule to baseline.
     * @param options Name override and ownership options.
     */
    createBaseline(file: IfcFile, work_schedule: Entity, options: IfcOpenShellSequenceCreateBaselineOptions): void;
    /**
     * Create an independent copy of a task and its subtasks, property sets, and
     * sequence relationships.
     *
     * Creates copies of the task, its nested child tasks, property sets, and
     * IfcRelSequence relationships between copied tasks. Returns parallel lists
     * of original and copied tasks in depth-first order.
     *
     * @param file File that receives the duplicated entities.
     * @param task IfcTask to duplicate.
     * @param options Ownership options for duplicated entities.
     * @return Parallel lists of original and copied tasks.
     */
    duplicateTask(file: IfcFile, task: Entity, options: IfcOpenShellSequenceDuplicateTaskOptions): IfcOpenShellSequenceDuplicateTaskResult;
    /**
     * Edit attributes of an IfcLagTime entity.
     *
     * @param lag_time IfcLagTime entity to edit.
     * @param attributes Property bag of attribute name/value pairs.
     */
    editLagTime(lag_time: Entity, attributes: PsetProperties | PsetInput): void;
    /**
     * Edit attributes of an IfcRecurrencePattern entity.
     *
     * @param recurrence_pattern IfcRecurrencePattern entity to edit.
     * @param attributes Property bag of attribute name/value pairs.
     */
    editRecurrencePattern(recurrence_pattern: Entity, attributes: PsetProperties | PsetInput): void;
    /**
     * Edit attributes of an IfcRelSequence entity.
     *
     * @param rel_sequence IfcRelSequence entity to edit.
     * @param attributes Property bag of attribute name/value pairs.
     */
    editSequence(rel_sequence: Entity, attributes: PsetProperties | PsetInput): void;
    /**
     * Edit attributes of an IfcTask entity.
     *
     * @param task IfcTask entity to edit.
     * @param attributes Property bag of attribute name/value pairs.
     */
    editTask(task: Entity, attributes: PsetProperties | PsetInput): void;
    /**
     * Edit attributes of an IfcTaskTime entity.
     *
     * Applies attribute changes from the property bag. Automatically derives
     * ScheduleFinish from ScheduleStart + ScheduleDuration, or ScheduleDuration
     * from ScheduleStart and ScheduleFinish, as appropriate. Dates are normalised
     * to the soonest working day based on the task's calendar. After editing,
     * cascades schedule changes to successor tasks.
     *
     * @param file File containing the task time.
     * @param task_time IfcTaskTime entity to edit.
     * @param attributes Property bag of attribute name/value pairs.
     */
    editTaskTime(file: IfcFile, task_time: Entity, attributes: PsetProperties | PsetInput): void;
    /**
     * Edit attributes of an IfcWorkCalendar entity.
     *
     * @param work_calendar IfcWorkCalendar entity to edit.
     * @param attributes Property bag of attribute name/value pairs.
     */
    editWorkCalendar(work_calendar: Entity, attributes: PsetProperties | PsetInput): void;
    /**
     * Edit attributes of an IfcWorkPlan entity.
     *
     * @param work_plan IfcWorkPlan entity to edit.
     * @param attributes Property bag of attribute name/value pairs.
     */
    editWorkPlan(work_plan: Entity, attributes: PsetProperties | PsetInput): void;
    /**
     * Edit attributes of an IfcWorkSchedule entity.
     *
     * @param work_schedule IfcWorkSchedule entity to edit.
     * @param attributes Property bag of attribute name/value pairs.
     */
    editWorkSchedule(work_schedule: Entity, attributes: PsetProperties | PsetInput): void;
    /**
     * Edit attributes of an IfcWorkTime entity.
     *
     * Date attributes "Start"/"StartDate" and "Finish"/"FinishDate" are
     * automatically mapped to the correct attribute name for the schema version.
     *
     * @param work_time IfcWorkTime entity to edit.
     * @param attributes Property bag of attribute name/value pairs.
     */
    editWorkTime(work_time: Entity, attributes: PsetProperties | PsetInput): void;
    /**
     * Recalculate an entire work schedule using the critical path method.
     *
     * Performs forward and backward passes through the task network to compute
     * early/late start/finish dates, total float, free float, and critical
     * path flags on each IfcTaskTime.
     *
     * @param file File containing the work schedule.
     * @param work_schedule IfcWorkSchedule to recalculate.
     */
    recalculateSchedule(file: IfcFile, work_schedule: Entity): void;
    /**
     * Remove an IfcTask and its subtasks from the file.
     *
     * Recursively removes nested child tasks, their IfcTaskTime entities,
     * sequence relationships, control assignments, process assignments,
     * product assignments, property sets, and project declarations.
     *
     * @param file File containing the task.
     * @param task IfcTask to remove.
     * @param options Ownership options for relationship cleanup.
     */
    removeTask(file: IfcFile, task: Entity, options: IfcOpenShellSequenceRemoveOptions): void;
    /**
     * Remove an IfcTimePeriod from the file.
     *
     * @param file File containing the time period.
     * @param time_period IfcTimePeriod to remove.
     */
    removeTimePeriod(file: IfcFile, time_period: Entity): void;
    /**
     * Remove an IfcWorkCalendar from the file.
     *
     * Unassigns the calendar from all controlled tasks, removes all working
     * and exception times, and removes the project declaration.
     *
     * @param file File containing the calendar.
     * @param work_calendar IfcWorkCalendar to remove.
     * @param options Ownership options for relationship cleanup.
     */
    removeWorkCalendar(file: IfcFile, work_calendar: Entity, options: IfcOpenShellSequenceRemoveOptions): void;
    /**
     * Remove an IfcWorkPlan from the file.
     *
     * Unassigns the plan from its project declaration and removes the aggregate
     * relationships to child work schedules (without removing the schedules).
     *
     * @param file File containing the work plan.
     * @param work_plan IfcWorkPlan to remove.
     * @param options Ownership options for relationship cleanup.
     */
    removeWorkPlan(file: IfcFile, work_plan: Entity, options: IfcOpenShellSequenceRemoveOptions): void;
    /**
     * Remove an IfcWorkSchedule and its controlled tasks from the file.
     *
     * Recursively removes nested work schedules, all controlled IfcTask
     * entities (with their subtasks and relationships), aggregate and
     * declaration relationships, and the schedule itself.
     *
     * @param file File containing the work schedule.
     * @param work_schedule IfcWorkSchedule to remove.
     * @param options Ownership options for relationship cleanup.
     */
    removeWorkSchedule(file: IfcFile, work_schedule: Entity, options: IfcOpenShellSequenceRemoveOptions): void;
    /**
     * Remove an IfcWorkTime and its associated recurrence pattern from the file.
     *
     * If the work time has a RecurrencePattern, all its TimePeriods and the
     * pattern itself are deleted first.
     *
     * @param file File containing the work time.
     * @param work_time IfcWorkTime to remove.
     */
    removeWorkTime(file: IfcFile, work_time: Entity): void;
    /**
     * Remove the TimeLag from an IfcRelSequence.
     *
     * If the IfcLagTime is only referenced by this sequence, it is deleted.
     * Otherwise the TimeLag attribute is cleared.
     *
     * @param file File containing the sequence.
     * @param rel_sequence IfcRelSequence to remove the lag from.
     */
    unassignLagTime(file: IfcFile, rel_sequence: Entity): void;
    /**
     * Remove a process assignment via IfcRelAssignsToProcess.
     *
     * If the relationship has only one RelatedObjects entry, the entire
     * relationship is removed. Otherwise the object is removed from the
     * aggregate.
     *
     * @param file File containing the relationship.
     * @param relating_process IfcTask to unassign from.
     * @param related_object Object to unassign.
     * @param options Ownership options.
     * @return The modified relationship, or no result when it is removed.
     */
    unassignProcess(file: IfcFile, relating_process: Entity, related_object: Entity, options: IfcOpenShellSequenceRemoveOptions): Entity;
    /**
     * Remove a product assignment via IfcRelAssignsToProduct.
     *
     * If the relationship has only one RelatedObjects entry, the entire
     * relationship is removed. Otherwise the object is removed from the
     * aggregate.
     *
     * @param file File containing the relationship.
     * @param relating_product IfcProduct to unassign from.
     * @param related_object Object to unassign.
     * @param options Ownership options.
     * @return The modified relationship, or no result when it is removed.
     */
    unassignProduct(file: IfcFile, relating_product: Entity, related_object: Entity, options: IfcOpenShellSequenceRemoveOptions): Entity;
    /**
     * Remove an IfcRecurrencePattern and its time periods from the file.
     *
     * Deletes all IfcTimePeriod entities in the pattern's TimePeriods aggregate,
     * then deletes the pattern itself.
     *
     * @param file File containing the recurrence pattern.
     * @param recurrence_pattern IfcRecurrencePattern to remove.
     */
    unassignRecurrencePattern(file: IfcFile, recurrence_pattern: Entity): void;
    /**
     * Remove an IfcRelSequence between two tasks.
     *
     * Removes the sequence relationship where relating_process is the
     * predecessor and related_process is the successor, then cascades
     * schedule changes from the successor.
     *
     * @param file File containing the tasks.
     * @param relating_process Predecessor IfcTask.
     * @param related_process Successor IfcTask.
     */
    unassignSequence(file: IfcFile, relating_process: Entity, related_process: Entity): void;
}
export interface ShapeApi {
    /**
     * Create an IfcAxis2Placement2D from position and optional reference direction.
     *
     * @param file IFC file that receives the placement.
     * @param options Position and optional X direction.
     * @return IfcAxis2Placement2D entity.
     */
    builderAxis2Placement2d(file: IfcFile, options: IfcOpenShellShapeBuilderAxis2Placement2dOptions): Entity;
    /**
     * Create an IfcAxis2Placement3D from position and axes.
     *
     * @param file IFC file that receives the placement.
     * @param options Position, Z axis, and X axis direction ratios.
     * @return IfcAxis2Placement3D entity.
     */
    builderAxis2Placement3d(file: IfcFile, options: IfcOpenShellShapeBuilderAxis2Placement3dOptions): Entity;
    /**
     * Create an IfcBlock (rectangular parallelepiped).
     *
     * @param file IFC file that receives the geometry.
     * @param options Position and XYZ dimensions.
     * @return IfcBlock entity.
     */
    builderBlock(file: IfcFile, options: IfcOpenShellShapeBuilderBlockOptions): Entity;
    /**
     * Create an IfcCircle centred at the given position.
     *
     * @param file IFC file that receives the geometry.
     * @param center XY coordinates of the circle centre.
     * @param radius Circle radius in model units.
     * @return IfcCircle entity.
     */
    builderCircle(file: IfcFile, center: number[], radius: number): Entity;
    /**
     * Create a 2D IfcIndexedPolyCurve arc between two points.
     *
     * Inserts a midpoint slightly offset along the dominant axis to form a
     * three-point arc segment.
     *
     * @param file IFC file that receives the geometry.
     * @param points Two XY endpoints.
     * @return IfcIndexedPolyCurve entity with one arc segment.
     */
    builderCurveBetweenTwoPoints(file: IfcFile, points: number[][]): Entity;
    /**
     * Create an independent copy of an IFC entity and the entities it references.
     *
     * New GlobalId attributes are generated for the copied entities.
     *
     * @param file IFC file that receives the copy.
     * @param element Entity to copy.
     * @return Root entity of the copied subgraph.
     */
    builderDeepCopy(file: IfcFile, element: Entity): Entity;
    /**
     * Create an IfcEdge between two vertex positions.
     *
     * @param file IFC file that receives the geometry.
     * @param start XYZ coordinates of the edge start.
     * @param end XYZ coordinates of the edge end.
     * @return IfcEdge entity.
     */
    builderEdge(file: IfcFile, start: number[], end: number[]): Entity;
    /**
     * Create an IfcEllipse, optionally trimmed to an IfcTrimmedCurve.
     *
     * @param file IFC file that receives the geometry.
     * @param options Radii, position, trim points, and reference direction.
     * @return IfcEllipse or IfcTrimmedCurve entity.
     */
    builderEllipseCurve(file: IfcFile, options: IfcOpenShellShapeBuilderEllipseCurveOptions): Entity;
    /**
     * Create an IfcExtrudedAreaSolid by extruding a profile along a direction.
     *
     * If the profile_or_curve is a bare IfcCurve (not an IfcProfileDef), it is
     * wrapped in an IfcArbitraryClosedProfileDef automatically.
     *
     * @param file IFC file that receives the geometry.
     * @param options Profile, depth, position, and extrusion direction.
     * @return IfcExtrudedAreaSolid entity.
     */
    builderExtrude(file: IfcFile, options: IfcOpenShellShapeBuilderExtrudeOptions): Entity;
    /**
     * Create an IfcFace from an ordered list of polygon vertices.
     *
     * @param file IFC file that receives the geometry.
     * @param points XYZ coordinates defining the face outer boundary.
     * @return IfcFace entity with an IfcFaceOuterBound.
     */
    builderFace(file: IfcFile, points: number[][]): Entity;
    /**
     * Create an IfcFacetedBrep from vertices and face index lists.
     *
     * @param file IFC file that receives the geometry.
     * @param points XYZ vertex coordinates.
     * @param faces Face index lists (zero-based). Each face is a single outer loop.
     * @return IfcFacetedBrep entity with an IfcClosedShell.
     */
    builderFacetedBrep(file: IfcFile, points: number[][], faces: number[][]): Entity;
    /**
     * Read the coordinate list from an IfcPolyline or IfcIndexedPolyCurve.
     *
     * @param polyline IfcPolyline or IfcIndexedPolyCurve entity.
     * @return Ordered XY or XYZ coordinate sequences.
     */
    builderGetPolylineCoords(polyline: Entity): number[][];
    /**
     * Create an IfcHalfSpaceSolid bounded by a plane.
     *
     * @param file IFC file that receives the geometry.
     * @param options Bounding plane and agreement flag.
     * @return IfcHalfSpaceSolid entity.
     */
    builderHalfSpaceSolid(file: IfcFile, options: IfcOpenShellShapeBuilderHalfSpaceSolidOptions): Entity;
    /**
     * Create a 2D IfcIndexedPolyCurve from explicit points and segment indices.
     *
     * Segments with two indices are line segments; segments with three indices
     * are arc segments.
     *
     * @param file IFC file that receives the geometry.
     * @param points XY coordinates for the point list.
     * @param segments Segment index arrays (1-based).
     * @return IfcIndexedPolyCurve entity.
     */
    builderIndexedPolycurve2d(file: IfcFile, points: number[][], segments: number[][]): Entity;
    /**
     * Build MEP bend geometry for a duct segment.
     *
     * Generates a swept disk solid (for circular profiles) or an extruded bend
     * shape (for rectangular profiles), plus optional start and end straight
     * extensions.
     *
     * @param file IFC file that receives the geometry.
     * @param options Segment, lengths, angle, radius, bend direction, and Z flip.
     * @return Bend result with representation and computed parameters.
     */
    builderMepBendShape(file: IfcFile, options: IfcOpenShellShapeBuilderMepBendShapeOptions): IfcOpenShellShapeBuilderMepBendShapeResult;
    /**
     * Compute the transition angle or length between two MEP duct profiles.
     *
     * Provide either length or angle (not both) to solve for the other.
     * Returns NaN when no valid solution exists.
     *
     * @param options Profile half-dimensions, offset, and one of length or angle.
     * @return The solved angle (degrees) or length (model units), or NaN.
     */
    builderMepTransitionCalculate(options: IfcOpenShellShapeBuilderMepTransitionCalculateOptions): number;
    /**
     * Compute the minimum transition length between two MEP duct profiles at a given angle.
     *
     * @param options Profile half-dimensions, angle in degrees, and profile offset.
     * @return Required transition length in model units, or NaN if no valid solution.
     */
    builderMepTransitionLength(options: IfcOpenShellShapeBuilderMepTransitionLengthOptions): number;
    /**
     * Build MEP transition geometry between two duct segments.
     *
     * Generates start/end extrusions and a connecting transition mesh.
     * Returns no result when the segments lack material profiles or the
     * transition cannot be computed.
     *
     * @param file IFC file that receives the geometry.
     * @param options Start/end segments, lengths, angle, and profile offset.
     * @return Transition result with representation and dimensions, or no result.
     */
    builderMepTransitionShape(file: IfcFile, options: IfcOpenShellShapeBuilderMepTransitionShapeOptions): IfcOpenShellShapeBuilderMepTransitionShapeResult | null;
    /**
     * Create a mesh representation using IfcPolygonalFaceSet (IFC4+) or IfcFacetedBrep (IFC2X3).
     *
     * @param file IFC file that receives the geometry.
     * @param points XYZ vertex coordinates.
     * @param faces Face index lists (zero-based).
     * @return IfcPolygonalFaceSet or IfcFacetedBrep entity.
     */
    builderMesh(file: IfcFile, points: number[][], faces: number[][]): Entity;
    /**
     * Mirror a geometry item across 2D axes.
     *
     * Supports IfcIndexedPolyCurve, IfcPolyline, IfcCircle, IfcEllipse,
     * IfcTrimmedCurve, and IfcExtrudedAreaSolid. A non-zero value in
     * mirror_axes at index 0 or 1 flips the corresponding coordinate.
     *
     * @param file IFC file containing the item.
     * @param options Item, axes, point, copy flag, and optional placement matrix.
     * @return The mirrored item, either the supplied entity or an independent copy.
     */
    builderMirror(file: IfcFile, options: IfcOpenShellShapeBuilderMirrorOptions): Entity;
    /**
     * Create an IfcPlane from a point and normal direction.
     *
     * The reference direction is computed automatically from the normal.
     *
     * @param file IFC file that receives the geometry.
     * @param location XYZ point on the plane.
     * @param normal Direction ratios of the plane normal.
     * @return IfcPlane entity.
     */
    builderPlane(file: IfcFile, location: number[], normal: number[]): Entity;
    /**
     * Create an IfcPolygonalFaceSet from vertices and polygonal face loops.
     *
     * Each face may contain one outer loop and optional inner loops (voids).
     * Faces with multiple loops create IfcIndexedPolygonalFaceWithVoids entities.
     *
     * @param file IFC file that receives the geometry.
     * @param points XYZ vertex coordinates.
     * @param faces Per-face loop index lists (zero-based).
     * @return IfcPolygonalFaceSet entity.
     */
    builderPolygonalFaceSet(file: IfcFile, points: number[][], faces: number[][][]): Entity;
    /**
     * Create a 2D or 3D polyline (IfcPolyline for IFC2X3, IfcIndexedPolyCurve otherwise).
     *
     * @param file IFC file that receives the geometry.
     * @param options Points, closure, offset, and arc segment indices.
     * @return IfcPolyline or IfcIndexedPolyCurve entity.
     */
    builderPolyline(file: IfcFile, options: IfcOpenShellShapeBuilderPolylineOptions): Entity;
    /**
     * Create an IfcArbitraryClosedProfileDef or IfcArbitraryProfileDefWithVoids.
     *
     * @param file IFC file that receives the profile.
     * @param options Outer curve, optional inner curves, name, and profile type.
     * @return IfcProfileDef entity.
     */
    builderProfile(file: IfcFile, options: IfcOpenShellShapeBuilderProfileOptions): Entity;
    /**
     * Create an IfcShapeRepresentation or IfcTopologyRepresentation.
     *
     * The representation type is auto-detected from the items when omitted.
     * Topology types (Vertex, Edge, Path, Face, Shell) produce IfcTopologyRepresentation;
     * all others produce IfcShapeRepresentation.
     *
     * @param file IFC file that receives the representation.
     * @param options Context, items, and optional representation type.
     * @return IfcShapeRepresentation or IfcTopologyRepresentation entity.
     */
    builderRepresentation(file: IfcFile, options: IfcOpenShellShapeBuilderRepresentationOptions): Entity;
    /**
     * Rotate a geometry item around a 2D pivot point.
     *
     * Supports IfcIndexedPolyCurve, IfcPolyline, IfcCircle, and
     * IfcExtrudedAreaSolid. The angle is in degrees.
     *
     * @param file IFC file containing the item.
     * @param options Item, angle, pivot, direction, and copy flag.
     * @return The rotated item, either the supplied entity or an independent copy.
     */
    builderRotate(file: IfcFile, options: IfcOpenShellShapeBuilderRotateOptions): Entity;
    /**
     * Replace the coordinate list of an IfcPolyline or IfcIndexedPolyCurve.
     *
     * For IfcPolyline, the new coordinate count must match the existing point count.
     *
     * @param file IFC file containing the polyline.
     * @param polyline IfcPolyline or IfcIndexedPolyCurve entity.
     * @param coords Replacement coordinates.
     * @return The modified polyline entity.
     */
    builderSetPolylineCoords(file: IfcFile, polyline: Entity, coords: number[][]): Entity;
    /**
     * Create an IfcSphere.
     *
     * @param file IFC file that receives the geometry.
     * @param options Radius and centre position.
     * @return IfcSphere entity.
     */
    builderSphere(file: IfcFile, options: IfcOpenShellShapeBuilderSphereOptions): Entity;
    /**
     * Create an IfcSweptDiskSolid along a directrix curve.
     *
     * @param file IFC file that receives the geometry.
     * @param path_curve IfcCurve entity defining the sweep path.
     * @param radius Disk radius in model units.
     * @return IfcSweptDiskSolid entity.
     */
    builderSweptDiskSolid(file: IfcFile, path_curve: Entity, radius: number): Entity;
    /**
     * Translate a geometry item by a direction and distance.
     *
     * Supports IfcIndexedPolyCurve, IfcPolyline, IfcCircle, IfcEllipse,
     * IfcExtrudedAreaSolid, IfcTessellatedFaceSet, IfcShapeRepresentation,
     * and IfcTrimmedCurve.
     *
     * @param file IFC file containing the item.
     * @param options Item, translation, and copy flag.
     * @return The translated item, either the supplied entity or an independent copy.
     */
    builderTranslate(file: IfcFile, options: IfcOpenShellShapeBuilderTranslateOptions): Entity;
    /**
     * Create an IfcTriangulatedFaceSet from vertices and triangle index lists.
     *
     * @param file IFC file that receives the geometry.
     * @param points XYZ vertex coordinates.
     * @param faces Triangle index lists (zero-based, truncated to 3 vertices each).
     * @return IfcTriangulatedFaceSet entity.
     */
    builderTriangulatedFaceSet(file: IfcFile, points: number[][], faces: number[][]): Entity;
    /**
     * Create an IfcVertexPoint at the given position.
     *
     * @param file IFC file that receives the geometry.
     * @param position XYZ coordinates of the vertex.
     * @return IfcVertexPoint entity.
     */
    builderVertex(file: IfcFile, position: number[]): Entity;
    /**
     * Check whether two floating-point values are approximately equal.
     *
     * Uses strict less-than comparison. When tolerance is 0.0, it defaults
     * to 1e-6.
     *
     * @param value First value.
     * @param x Second value.
     * @param tolerance Maximum absolute difference (exclusive). Defaults to 1e-6 when 0.
     * @return True if |value - x| < tolerance.
     */
    isAlmostEqual(value: number, x: number, tolerance: number): boolean;
}
export interface SpatialApi {
    /**
     * Contain products in a spatial structure via IfcRelContainedInSpatialStructure.
     *
     * Products previously contained in a different structure are moved. Products
     * previously aggregated under a different parent (IfcRelAggregates) are
     * removed from that aggregation. If an existing containment relationship
     * already relates to the same structure, products are merged into it.
     */
    assignContainer(file: IfcFile, options: IfcOpenShellSpatialAssignContainerOptions): Entity;
    /**
     * Remove products from a spatial structure reference relationship.
     *
     * Removes products from IfcRelReferencedInSpatialStructure relationships
     * for the given structure. If no products remain, the relationship is deleted.
     */
    dereferenceStructure(file: IfcFile, options: IfcOpenShellSpatialDereferenceStructureOptions): void;
    /**
     * Reference products in a spatial structure via IfcRelReferencedInSpatialStructure.
     *
     * Unlike containment, referencing does not establish ownership. Products
     * already referenced by the same structure are skipped.
     */
    referenceStructure(file: IfcFile, options: IfcOpenShellSpatialReferenceStructureOptions): Entity;
    /**
     * Remove products from their IfcRelContainedInSpatialStructure relationships.
     *
     * If no related elements remain after removal, the relationship is deleted.
     */
    unassignContainer(file: IfcFile, options: IfcOpenShellSpatialUnassignContainerOptions): void;
}
export interface StructuralApi {
    /**
     * Create an IfcStructuralActivity and connect it to a structural member.
     *
     * Creates the activity entity of the given class and predefined type, sets
     * its AppliedLoad and GlobalOrLocal attributes, and creates an
     * IfcRelConnectsStructuralActivity linking the activity to the structural
     * member.
     *
     * @param file File that receives the new entities.
     * @param applied_load IfcStructuralLoad to apply.
     * @param structural_member Structural member to connect the activity to.
     * @param ifc_class IFC class name (e.g. "IfcStructuralPlanarAction").
     * @param predefined_type Predefined type enum value.
     * @param global_or_local "GLOBAL_COORDS" or "LOCAL_COORDS".
     * @param options Ownership options for the activity and relationship.
     * @return Newly created IfcStructuralActivity.
     */
    addStructuralActivity(file: IfcFile, applied_load: Entity, structural_member: Entity, ifc_class: string, predefined_type: string, global_or_local: string, options: IfcOpenShellStructuralAddStructuralActivityOptions): Entity;
    /**
     * Create an IfcStructuralAnalysisModel with PredefinedType LOADING_3D.
     *
     * @param file File that receives the new entity.
     * @param owner_history Owner history for the new entity. When omitted, no owner history is assigned.
     * @return Newly created IfcStructuralAnalysisModel.
     */
    addStructuralAnalysisModel(file: IfcFile, owner_history: Entity): Entity;
    /**
     * Create an IfcBoundaryCondition subclass entity.
     *
     * When a connection is provided, the boundary class is inferred:
     * IfcStructuralPointConnection yields IfcBoundaryNodeCondition,
     * IfcStructuralCurveConnection yields IfcBoundaryEdgeCondition,
     * IfcStructuralSurfaceConnection yields IfcBoundaryFaceCondition.
     * The condition is applied to the connection's AppliedCondition attribute.
     *
     * @param file File that receives the new entity.
     * @param ifc_class IFC boundary condition class name. Overridden when connection is provided.
     * @param options Name and connection options.
     * @return Newly created IfcBoundaryCondition subclass.
     */
    addStructuralBoundaryCondition(file: IfcFile, ifc_class: string, options: IfcOpenShellStructuralAddStructuralBoundaryConditionOptions): Entity;
    /**
     * Create an IfcStructuralLoad subclass entity.
     *
     * @param file File that receives the new entity.
     * @param ifc_class IFC class name (e.g. "IfcStructuralLoadConfiguration").
     * @param name Optional name for the load entity.
     * @return Newly created IfcStructuralLoad subclass.
     */
    addStructuralLoad(file: IfcFile, ifc_class: string, name: string): Entity;
    /**
     * Create an IfcStructuralLoadCase entity.
     *
     * Creates the load case with PredefinedType LOAD_CASE and the given
     * ActionType and ActionSource enum values.
     *
     * @param file File that receives the new entity.
     * @param name Name for the load case.
     * @param action_type ActionType enum value (e.g. "PERMANENT_G", "VARIABLE_Q").
     * @param action_source ActionSource enum value (e.g. "WIND", "IMPOSED").
     * @param owner_history Owner history for the new entity. When omitted, no owner history is assigned.
     * @return Newly created IfcStructuralLoadCase.
     */
    addStructuralLoadCase(file: IfcFile, name: string, action_type: string, action_source: string, owner_history: Entity): Entity;
    /**
     * Create an IfcStructuralLoadGroup entity.
     *
     * Creates the load group with PredefinedType LOAD_GROUP and the given
     * ActionType and ActionSource enum values.
     *
     * @param file File that receives the new entity.
     * @param name Name for the load group.
     * @param action_type ActionType enum value.
     * @param action_source ActionSource enum value.
     * @param owner_history Owner history for the new entity. When omitted, no owner history is assigned.
     * @return Newly created IfcStructuralLoadGroup.
     */
    addStructuralLoadGroup(file: IfcFile, name: string, action_type: string, action_source: string, owner_history: Entity): Entity;
    /**
     * Connect a structural member to a structural connection via IfcRelConnectsStructuralMember.
     *
     * If a connection already exists between the two, the existing relationship
     * is returned.
     *
     * @param file File containing both entities.
     * @param relating_structural_member IfcStructuralMember to connect.
     * @param related_structural_connection IfcStructuralConnection to connect to.
     * @param owner_history Owner history for the new relationship. When omitted, no owner history is assigned.
     * @return The IfcRelConnectsStructuralMember relationship.
     */
    addStructuralMemberConnection(file: IfcFile, relating_structural_member: Entity, related_structural_connection: Entity, owner_history: Entity): Entity;
    /**
     * Assign a structural member or connection to a product via IfcRelAssignsToProduct.
     *
     * If a relationship already exists between the two, the object is appended
     * to the existing relationship's RelatedObjects. Otherwise a new
     * IfcRelAssignsToProduct is created.
     *
     * @param file File containing both entities.
     * @param relating_product IfcProduct that the structural item references.
     * @param related_object Structural item to assign.
     * @param owner_history Owner history for new relationships. When omitted, no owner history is assigned.
     * @return The IfcRelAssignsToProduct relationship.
     */
    assignProduct(file: IfcFile, relating_product: Entity, related_object: Entity, owner_history: Entity): Entity;
    /**
     * Assign products to an IfcStructuralAnalysisModel via IfcRelAssignsToGroup.
     *
     * @param file File containing the products and model.
     * @param products Products to assign to the model.
     * @param structural_analysis_model IfcStructuralAnalysisModel to assign to.
     * @param options Ownership options for the relationship.
     * @return The IfcRelAssignsToGroup relationship.
     */
    assignStructuralAnalysisModel(file: IfcFile, products: Entity[], structural_analysis_model: Entity, options: IfcOpenShellStructuralAssignStructuralAnalysisModelOptions): Entity;
    /**
     * Assign an IfcStructuralAnalysisModel to a building via IfcRelServicesBuildings.
     *
     * If a relationship already exists for this model, the building is appended
     * to its RelatedBuildings aggregate.
     *
     * @param file File containing both entities.
     * @param structural_analysis_model IfcStructuralAnalysisModel to assign.
     * @param building IfcBuilding to assign to.
     * @param owner_history Owner history for the new relationship. When omitted, no owner history is assigned.
     * @return The IfcRelServicesBuildings relationship.
     */
    assignToBuilding(file: IfcFile, structural_analysis_model: Entity, building: Entity, owner_history: Entity): Entity;
    /**
     * Edit attributes of an IfcBoundaryCondition subclass.
     *
     * Each entry in the attributes mapping must contain "type" and
     * "value" sub-entries. The type specifies the IFC typed value class (e.g.
     * "IfcBoolean", "IfcForceMeasure") or "string"/"blank" for direct values.
     *
     * @param file File containing the boundary condition.
     * @param condition IfcBoundaryCondition entity to edit.
     * @param attributes Property bag of typed attribute entries.
     */
    editStructuralBoundaryCondition(file: IfcFile, condition: Entity, attributes: PsetProperties | PsetInput): void;
    /**
     * Edit the ConditionCoordinateSystem of a structural item.
     *
     * Replaces the Axis and RefDirection directions on the item's
     * ConditionCoordinateSystem (IfcAxis2Placement3D). Creates the placement
     * if absent. Old direction entities are deleted if unreferenced.
     *
     * @param file File containing the structural item.
     * @param structural_item Structural item (e.g. IfcStructuralPointConnection).
     * @param axis 3-element direction ratios for the Axis attribute.
     * @param ref_direction 3-element direction ratios for the RefDirection attribute.
     */
    editStructuralConnectionCs(file: IfcFile, structural_item: Entity, axis: number[], ref_direction: number[]): void;
    /**
     * Edit the Axis direction of a structural item.
     *
     * Replaces the Axis direction on the structural item (e.g.
     * IfcStructuralCurveMember). The old direction entity is deleted if
     * unreferenced.
     *
     * @param file File containing the structural item.
     * @param structural_item Structural item with an Axis attribute.
     * @param axis 3-element direction ratios.
     */
    editStructuralItemAxis(file: IfcFile, structural_item: Entity, axis: number[]): void;
    /**
     * Remove an IfcStructuralAnalysisModel from the file.
     *
     * Removes the model's IfcRelAssignsToGroup relationships and then the
     * model entity itself.
     *
     * @param file File containing the model.
     * @param structural_analysis_model IfcStructuralAnalysisModel to remove.
     */
    removeStructuralAnalysisModel(file: IfcFile, structural_analysis_model: Entity): void;
    /**
     * Remove a boundary condition from a structural connection.
     *
     * When connection is provided, clears and deletes its AppliedCondition.
     * When boundary_condition is provided instead, clears AppliedCondition on
     * all referencing entities and deletes the condition. At least one of
     * connection or boundary_condition must be provided.
     *
     * @param file File containing the entities.
     * @param options Connection or boundary condition to remove.
     */
    removeStructuralBoundaryCondition(file: IfcFile, options: IfcOpenShellStructuralRemoveStructuralBoundaryConditionOptions): void;
    /**
     * Remove an IfcRelConnectsStructuralMember and its AppliedCondition.
     *
     * Removes the boundary condition from the related structural connection
     * (if any), then removes the relationship and its OwnerHistory.
     *
     * @param file File containing the relationship.
     * @param relation IfcRelConnectsStructuralMember to remove.
     */
    removeStructuralConnectionCondition(file: IfcFile, relation: Entity): void;
    /**
     * Remove an IfcStructuralLoad subclass from the file.
     *
     * @param file File containing the load.
     * @param structural_load IfcStructuralLoad entity to remove.
     */
    removeStructuralLoad(file: IfcFile, structural_load: Entity): void;
    /**
     * Remove an IfcStructuralLoadCase from the file.
     *
     * Removes the load case's IfcRelAssignsToGroup relationships and then the
     * entity itself.
     *
     * @param file File containing the load case.
     * @param structural_load_case IfcStructuralLoadCase to remove.
     */
    removeStructuralLoadCase(file: IfcFile, structural_load_case: Entity): void;
    /**
     * Remove an IfcStructuralLoadGroup from the file.
     *
     * Removes singleton IfcRelAssignsToGroup relationships and then the entity
     * itself.
     *
     * @param file File containing the load group.
     * @param structural_load_group IfcStructuralLoadGroup to remove.
     */
    removeStructuralLoadGroup(file: IfcFile, structural_load_group: Entity): void;
    /**
     * Remove products from an IfcStructuralAnalysisModel's group.
     *
     * @param file File containing the products and model.
     * @param products Products to unassign from the model.
     * @param structural_analysis_model IfcStructuralAnalysisModel to unassign from.
     * @param options Ownership options for updating the relationship.
     */
    unassignStructuralAnalysisModel(file: IfcFile, products: Entity[], structural_analysis_model: Entity, options: IfcOpenShellStructuralUnassignStructuralAnalysisModelOptions): void;
}
export interface StyleApi {
    /**
     * Create a new presentation style entity.
     *
     * For IfcSurfaceStyle, the Side attribute defaults to "BOTH".
     *
     * @param file IFC file that receives the style.
     * @param name Style name. When omitted, the style is unnamed.
     * @param ifc_class IFC entity class (e.g. "IfcSurfaceStyle", "IfcFillAreaStyle").
     * @return Newly created style entity.
     */
    addStyle(file: IfcFile, name: string, ifc_class: string): Entity;
    /**
     * Assign or replace a style on a single representation item.
     *
     * Creates an IfcStyledItem (and optionally an IfcPresentationStyleAssignment
     * for IFC2X3) linking the item to the given style. If the item already has a
     * styled item, the existing style is replaced. When style is omitted, the
     * styled item is removed from the representation item.
     *
     * @param file IFC file to modify.
     * @param options Item, style, and IFC2X3 compat flag.
     * @return The IfcStyledItem, or no result when the style is removed.
     */
    assignItemStyle(file: IfcFile, options: IfcOpenShellStyleAssignItemStyleOptions): Entity;
    /**
     * Assign a style to a material via IfcMaterialDefinitionRepresentation.
     *
     * Creates or extends a styled representation under the material's definition.
     * Also propagates the style to shape aspects whose names match material
     * constituent names.
     *
     * @param file IFC file to modify.
     * @param material IfcMaterial entity.
     * @param style Presentation style entity to assign.
     * @param context IfcGeometricRepresentationContext for the styled representation.
     * @param should_use_presentation_style_assignment Wrap style in IfcPresentationStyleAssignment.
     */
    assignMaterialStyle(file: IfcFile, material: Entity, style: Entity, context: Entity, should_use_presentation_style_assignment: boolean): void;
    /**
     * Assign styles to the geometric items within a shape representation.
     *
     * Assigns the styles to representation items in sequence. When
     * replace_previous_same_type_style is true, an existing style of the same IFC
     * class is replaced instead of appended.
     *
     * @param file IFC file to modify.
     * @param shape_representation IfcShapeRepresentation to assign styles to.
     * @param styles Presentation style entities to assign.
     * @param should_use_presentation_style_assignment Wrap styles in IfcPresentationStyleAssignment.
     * @param replace_previous_same_type_style Replace existing styles of the same type.
     * @return List of newly created IfcStyledItem entities.
     */
    assignRepresentationStyles(file: IfcFile, shape_representation: Entity, styles: Entity[], should_use_presentation_style_assignment: boolean, replace_previous_same_type_style: boolean): Entity[];
    /**
     * Edit attributes of an IfcSurfaceStyle or its nested surface styles.
     *
     * Handles colour attributes (IfcColourRgb), specular highlights, and
     * colour-or-factor properties specially; other attributes are set directly.
     *
     * @param file IFC file containing the style.
     * @param style IfcSurfaceStyle entity to modify.
     * @param attributes Attribute name-to-value mapping.
     */
    editSurfaceStyle(file: IfcFile, style: Entity, attributes: PsetProperties | PsetInput): void;
    /**
     * Remove a presentation style and clean up all references to it.
     *
     * Removes inverse IfcStyledItem references, nested surface styles (for
     * IfcSurfaceStyle), fill area style hatching, and orphaned
     * IfcPresentationStyleAssignment entities.
     *
     * @param file IFC file to modify.
     * @param style Presentation style entity to remove.
     */
    removeStyle(file: IfcFile, style: Entity): void;
    /**
     * Remove an IfcStyledRepresentation and its parent IfcMaterialDefinitionRepresentation.
     *
     * @param file IFC file to modify.
     * @param representation IfcStyledRepresentation entity to remove.
     */
    removeStyledRepresentation(file: IfcFile, representation: Entity): void;
    /**
     * Remove an IfcSurfaceStyleWithTextures or IfcSurfaceStyleRendering and its nested entities.
     *
     * Removes texture coordinates, textures, and colour entities belonging to the
     * surface style when they are no longer referenced.
     *
     * @param file IFC file to modify.
     * @param style Surface style sub-entity to remove.
     */
    removeSurfaceStyle(file: IfcFile, style: Entity): void;
    /**
     * Remove a style from a material's styled representation.
     *
     * Removes empty IfcStyledItem, IfcStyledRepresentation, and
     * IfcMaterialDefinitionRepresentation entities, and propagates the removal to
     * matching shape aspects.
     *
     * @param file IFC file to modify.
     * @param material IfcMaterial entity.
     * @param style Presentation style entity to remove.
     * @param context IfcGeometricRepresentationContext identifying the styled representation.
     */
    unassignMaterialStyle(file: IfcFile, material: Entity, style: Entity, context: Entity): void;
    /**
     * Remove styles from the geometric items within a shape representation.
     *
     * Removes matching styles from IfcStyledItem and
     * IfcPresentationStyleAssignment entities in the representation.
     *
     * @param file IFC file to modify.
     * @param shape_representation IfcShapeRepresentation to unassign styles from.
     * @param styles Presentation style entities to remove.
     * @param should_use_presentation_style_assignment Also search inside IfcPresentationStyleAssignment.
     */
    unassignRepresentationStyles(file: IfcFile, shape_representation: Entity, styles: Entity[], should_use_presentation_style_assignment: boolean): void;
}
export interface SystemApi {
    /**
     * Create a new IfcDistributionPort and optionally assign it to an element.
     *
     * If an element is provided, the port is assigned to it via IfcRelNests
     * (IFC4+) or IfcRelConnectsPortToElement (IFC2X3).
     */
    addPort(file: IfcFile, options: IfcOpenShellSystemAddPortOptions): Entity;
    /**
     * Create a new system with the given IFC class.
     *
     * In IFC2X3, "IfcDistributionSystem" is automatically resolved to "IfcSystem".
     * The system is created with Name set to "Unnamed".
     */
    addSystem(file: IfcFile, options: IfcOpenShellSystemAddSystemOptions): Entity;
    /**
     * Assign a flow control element to a flow element via IfcRelFlowControlElements.
     *
     * If the flow control is already assigned to a different element, no change
     * is made and no relationship is returned.
     */
    assignFlowControl(file: IfcFile, options: IfcOpenShellSystemAssignFlowControlOptions): Entity;
    /**
     * Assign a port to an element.
     *
     * In IFC2X3, uses IfcRelConnectsPortToElement. In IFC4+, uses IfcRelNests
     * to nest the port under the element. Updates the port's ObjectPlacement
     * if it has an IfcLocalPlacement.
     */
    assignPort(file: IfcFile, options: IfcOpenShellSystemAssignPortOptions): Entity;
    /**
     * Assign products to a system via IfcRelAssignsToGroup.
     *
     * Validates that each product is compatible with the system type (e.g.
     * only IfcDistributionElement for IfcDistributionSystem). The operation fails
     * if a product is not valid for the system type.
     */
    assignSystem(file: IfcFile, options: IfcOpenShellSystemAssignSystemOptions): Entity;
    /**
     * Connect two ports together with a given flow direction.
     *
     * Creates IfcRelConnectsPorts relationships and sets FlowDirection on both
     * ports. Existing connections to other ports are removed first. The
     * direction parameter controls which port is SOURCE, SINK, or both.
     */
    connectPort(file: IfcFile, options: IfcOpenShellSystemConnectPortOptions): void;
    /**
     * Disconnect a port from all connected ports.
     *
     * Removes all IfcRelConnectsPorts relationships involving the port and
     * clears FlowDirection on both sides.
     */
    disconnectPort(file: IfcFile, port: Entity): void;
    /**
     * Remove a system and its relationships.
     *
     * Removes associated IfcRelDefinesByProperties (property sets) and
     * IfcRelAssignsToGroup relationships, then deletes the system entity.
     */
    removeSystem(file: IfcFile, system: Entity): void;
    /**
     * Remove a flow control element from a flow element's IfcRelFlowControlElements.
     *
     * If the control element is the only one in the relationship, the
     * relationship is deleted. Otherwise, the control element is removed from
     * the RelatedControlElements aggregate.
     */
    unassignFlowControl(file: IfcFile, options: IfcOpenShellSystemUnassignFlowControlOptions): void;
    /**
     * Remove a port from an element.
     *
     * In IFC2X3, removes the IfcRelConnectsPortToElement. In IFC4+, removes
     * the port from the IfcRelNests relationship (or deletes the relationship
     * if it was the only nested object).
     */
    unassignPort(file: IfcFile, options: IfcOpenShellSystemUnassignPortOptions): void;
    /** Remove products from a system. */
    unassignSystem(file: IfcFile, options: IfcOpenShellSystemUnassignSystemOptions): void;
}
export interface TypeApi {
    /**
     * Assign a type to element occurrences via IfcRelDefinesByType.
     *
     * Objects previously typed by a different type are moved. If an existing
     * IfcRelDefinesByType relationship already relates to the same type,
     * objects are merged into it. When should_map_representations is true
     * (default), IfcRepresentationMaps from the type are propagated onto
     * each occurrence and PredefinedType/ObjectType are cleared to avoid
     * double-typing.
     */
    assignType(file: IfcFile, options: IfcOpenShellTypeAssignTypeOptions): Entity;
    /**
     * Map IfcRepresentationMaps from a type onto a related object.
     *
     * Clones mapped representations from the type's RepresentationMaps and
     * assigns them to the object. Returns true if any representations were
     * mapped.
     */
    mapTypeRepresentations(file: IfcFile, related_object: Entity, relating_type: Entity): boolean;
    /**
     * Remove type assignments from element occurrences.
     *
     * Removes objects from IfcRelDefinesByType relationships. If no objects
     * remain in a relationship, the relationship is deleted.
     */
    unassignType(file: IfcFile, options: IfcOpenShellTypeUnassignTypeOptions): void;
}
export interface UnitApi {
    /**
     * Create an IfcContextDependentUnit entity.
     *
     * Creates a unit with custom dimensional exponents, unit type, and name.
     *
     * @param file File that receives the new entity.
     * @param unit_type IFC unit type enum value (e.g. "LENGTHUNIT").
     * @param name Display name for the unit (e.g. "bag", "each").
     * @param dimensions 7-element sequence of dimensional exponents.
     * @return Newly created IfcContextDependentUnit.
     */
    addContextDependentUnit(file: IfcFile, unit_type: string, name: string, dimensions: bigint[]): Entity;
    /**
     * Create an IfcDerivedUnit entity.
     *
     * Constructs a derived unit from a list of component units and their
     * exponents (e.g. m/s from ["METRE", "SECOND"] with exponents [1, -1]).
     *
     * @param file File that receives the new entity.
     * @param unit_type IFC unit type enum value (e.g. "VELOCITYUNIT").
     * @param userdefinedtype UserDefinedType string. When omitted, it is left blank.
     * @param units Component IfcUnit entities.
     * @param exponents Exponent for each component unit (must match units in length).
     * @return Newly created IfcDerivedUnit.
     */
    addDerivedUnit(file: IfcFile, unit_type: string, userdefinedtype: string, units: Entity[], exponents: bigint[]): Entity;
    /**
     * Create an IfcMonetaryUnit entity.
     *
     * @param file File that receives the new entity.
     * @param currency ISO 4217 currency code (e.g. "USD", "EUR").
     * @return Newly created IfcMonetaryUnit.
     */
    addMonetaryUnit(file: IfcFile, currency: string): Entity;
    /**
     * Create an IfcSIUnit entity.
     *
     * Sets the UnitType, Name (derived from the unit type), and optional
     * Prefix. When prefix is omitted, the Prefix attribute is left blank.
     *
     * @param file File that receives the new entity.
     * @param unit_type IFC unit type enum value (e.g. "LENGTHUNIT").
     * @param prefix SI prefix (e.g. "KILO", "MILLI"). When omitted, the base unit is used.
     * @return Newly created IfcSIUnit.
     */
    addSiUnit(file: IfcFile, unit_type: string, prefix: string): Entity;
    /**
     * Calculate the scale factor from SI for a project unit type.
     *
     * Returns the multiplier needed to convert from the project's unit to SI
     * base units. For example, if the project length unit is millimetres,
     * returns 0.001. Defaults to 1.0 when no unit is found.
     *
     * @param file File to query for the project unit.
     * @param unit_type IFC unit type (e.g. "LENGTHUNIT"). Defaults to "LENGTHUNIT" when empty.
     * @return Scale factor to SI.
     */
    calculateUnitScale(file: IfcFile, unit_type: string): number;
    /**
     * Convert a numeric value between units specified by prefix and name strings.
     *
     * Supports SI conversions and prefix-aware scaling. For example,
     * converting from ("KILO", "METRE") to ("", "METRE") multiplies by 1000.
     *
     * @param value Numeric value to convert.
     * @param from_prefix Source SI prefix (e.g. "KILO"). Empty for base unit.
     * @param from_unit Source unit name (e.g. "METRE", "GRAM").
     * @param to_prefix Target SI prefix.
     * @param to_unit Target unit name.
     * @return The converted value.
     */
    convert(value: number, from_prefix: string, from_unit: string, to_prefix: string, to_unit: string): number;
    /**
     * Convert a numeric value between two IFC unit entities.
     *
     * Reads the Prefix and Name attributes from each unit to perform the
     * conversion.
     *
     * @param value Numeric value to convert.
     * @param from_unit Source IfcUnit entity.
     * @param to_unit Target IfcUnit entity.
     * @return The converted value.
     */
    convertUnit(value: number, from_unit: Entity, to_unit: Entity): number;
    /**
     * Format a length value as an imperial or metric string.
     *
     * For imperial output: returns feet-inches notation with fractional
     * precision (e.g. "5' - 3 1/4\""). For metric: returns a rounded
     * decimal value.
     *
     * @param value Length value to format.
     * @param precision Fractional precision denominator for imperial (e.g. 16 for 1/16").
     * @param decimal_places Number of decimal places for metric output.
     * @param suppress_zero_inches When true, omits zero inches in imperial output.
     * @param unit_system "imperial" or "metric".
     * @param input_unit Source unit: "foot" or "inch". Defaults to "foot" when empty.
     * @param output_unit Target unit: "foot" or "inch". Defaults to "foot" when empty.
     * @return Formatted length string.
     */
    formatLength(value: number, precision: number, decimal_places: number, suppress_zero_inches: boolean, unit_system: string, input_unit: string, output_unit: string): string;
    /**
     * Return the full name of an IfcSIUnit (prefix + name).
     *
     * For example, a unit with Prefix "KILO" and Name "METRE" returns
     * "KILOMETRE".
     *
     * @param unit IfcSIUnit entity.
     * @return Concatenated prefix and name string.
     */
    getFullUnitName(unit: Entity): string;
    /**
     * Derive the IFC measure class name from a unit type string.
     *
     * For example, "LENGTHUNIT" returns "IfcLengthMeasure",
     * "USERDEFINED" returns "IfcNumericMeasure". Returns an empty string
     * for unrecognised input.
     *
     * @param unit_type IFC unit type enum value (e.g. "LENGTHUNIT").
     * @return The corresponding IFC measure class name.
     */
    getMeasureClass(unit_type: string): string;
    /**
     * Derive the IFC unit type string from a measure class name.
     *
     * The inverse of unit_get_measure_class. For example,
     * "IfcLengthMeasure" returns "LENGTHUNIT".
     *
     * @param measure_class IFC measure class name.
     * @return The corresponding IFC unit type, or empty string if unrecognised.
     */
    getMeasureUnitType(measure_class: string): string;
    /**
     * Return the named dimensional exponents for a given unit type name.
     *
     * Like unit_get_si_dimensions but returns all zeros for unknown types
     * instead of falling back.
     *
     * @param name Unit type name.
     * @return 7-element sequence of dimensional exponents.
     */
    getNamedDimensions(name: string): number[];
    /**
     * Extract the SI prefix from a combined unit text string.
     *
     * For example, "KILOMETRE" returns "KILO", "MILLIGRAM" returns "MILLI".
     * Returns an empty string if no prefix is found.
     *
     * @param text Combined prefix and unit name string.
     * @return The prefix portion, or empty string if none.
     */
    getPrefix(text: string): string;
    /**
     * Return the numeric multiplier for an SI prefix.
     *
     * For example, "KILO" returns 1000.0, "MILLI" returns 0.001.
     * Returns 1.0 if no prefix is found.
     *
     * @param text SI prefix string (e.g. "KILO", "MILLI").
     * @return The multiplier value.
     */
    getPrefixMultiplier(text: string): number;
    /**
     * Return the project unit for a given unit type.
     *
     * Searches the project's IfcUnitAssignment for a unit whose UnitType
     * matches the given string.
     *
     * @param file File to query.
     * @param unit_type IFC unit type enum value (e.g. "LENGTHUNIT").
     * @return The matching unit entity, or no result if it is not found.
     */
    getProjectUnit(file: IfcFile, unit_type: string): Entity;
    /**
     * Return the SI dimensional exponents for a given unit type name.
     *
     * Returns a 7-element sequence of integers corresponding to the
     * IfcDimensionalExponents attributes: Length, Mass, Time,
     * ElectricCurrent, ThermodynamicTemperature, AmountOfSubstance,
     * LuminousIntensity. Falls back to the "OTHERWISE" entry for
     * unknown types.
     *
     * @param name Unit type name (e.g. "LENGTHUNIT", "MASSUNIT").
     * @return 7-element sequence of dimensional exponents.
     */
    getSiDimensions(name: string): number[];
    /**
     * Infer the IFC measure class from a unit symbol string.
     *
     * For example, "m" returns "IfcLengthMeasure", "m2" returns "IfcAreaMeasure",
     * "kg" returns "IfcMassMeasure". Returns "IfcNumericMeasure" for unknown symbols.
     *
     * @param symbol Unit symbol (e.g. "m", "ft", "kg", "hr").
     * @return The corresponding IFC measure class name.
     */
    getSymbolMeasureClass(symbol: string): string;
    /**
     * Infer the IFC quantity class from a unit symbol string.
     *
     * For example, "m" returns "IfcQuantityLength", "m2" returns "IfcQuantityArea",
     * "kg" returns "IfcQuantityWeight". Returns "IfcQuantityCount" for unknown symbols.
     *
     * @param symbol Unit symbol (e.g. "m", "ft", "kg", "hr").
     * @return The corresponding IFC quantity class name.
     */
    getSymbolQuantityClass(symbol: string): string;
    /**
     * Return the IfcUnitAssignment entity for the project.
     *
     * @param file File to query.
     * @return The IfcUnitAssignment entity, or no result if it is not found.
     */
    getUnitAssignment(file: IfcFile): Entity;
    /**
     * Extract the base unit name from a combined unit text string.
     *
     * Normalises "METER" to "METRE" before matching. For example,
     * "KILOMETRE" returns "METRE", "SQUARE_METRE" returns "SQUARE_METRE".
     * Returns an empty string if no unit name is found.
     *
     * @param text Combined prefix and unit name string.
     * @return The base unit name, or empty string if not recognised.
     */
    getUnitName(text: string): string;
    /**
     * Extract the base unit name, accepting both metric and imperial names.
     *
     * Like unit_get_unit_name but also recognises imperial unit names
     * (e.g. "FOOT", "INCH"). Returns an empty string if not recognised.
     *
     * @param text Unit name string.
     * @return The base unit name, or empty string if not recognised.
     */
    getUnitNameUniversal(text: string): string;
    /**
     * Return the display symbol for a unit entity.
     *
     * For IfcSIUnit: prefix symbol + unit symbol (e.g. "km").
     * For IfcContextDependentUnit with USERDEFINED UnitType: the Name attribute.
     * Returns "?" for unrecognised units.
     *
     * @param unit IfcUnit entity (IfcSIUnit, IfcContextDependentUnit, etc.).
     * @return Symbol string.
     */
    getUnitSymbol(unit: Entity): string;
    /**
     * Remove a unit entity from the file.
     *
     * Removes the unit from the project's IfcUnitAssignment (if present) and
     * then deletes the unit entity (and its dimensional exponents, if owned).
     *
     * @param file File containing the unit.
     * @param unit IfcUnit entity to remove.
     */
    removeUnit(file: IfcFile, unit: Entity): void;
    /**
     * Resolve the IFC measure class for a property or quantity.
     *
     * Returns the measure class name (e.g. "IfcLengthMeasure") derived from
     * the property's NominalValue or quantity type. Returns an empty string
     * if a unit entity is directly attached (use unit_resolve_property_unit
     * instead).
     *
     * @param prop IfcProperty or IfcPhysicalQuantity entity.
     * @return Measure class name, or empty string if a unit entity is attached.
     */
    resolvePropertyMeasureClass(prop: Entity): string;
    /**
     * Resolve the defined measure class of an IfcPropertyTableValue.
     *
     * Returns the measure class name derived from the DefinedValues.
     * Returns an empty string if a DefinedUnit entity is directly attached.
     *
     * @param prop IfcPropertyTableValue entity.
     * @return Measure class name, or empty string if a unit entity is attached.
     */
    resolvePropertyTableDefinedMeasureClass(prop: Entity): string;
    /**
     * Resolve the defined unit of an IfcPropertyTableValue.
     *
     * Returns the DefinedUnit attribute, or no result if the unit must be
     * inferred from the DefinedValues measure class.
     *
     * @param prop IfcPropertyTableValue entity.
     * @return The DefinedUnit entity, or no result.
     */
    resolvePropertyTableDefinedUnit(prop: Entity): Entity;
    /**
     * Resolve the defining measure class of an IfcPropertyTableValue.
     *
     * Returns the measure class name derived from the DefiningValues.
     * Returns an empty string if a DefiningUnit entity is directly attached.
     *
     * @param prop IfcPropertyTableValue entity.
     * @return Measure class name, or empty string if a unit entity is attached.
     */
    resolvePropertyTableDefiningMeasureClass(prop: Entity): string;
    /**
     * Resolve the defining unit of an IfcPropertyTableValue.
     *
     * Returns the DefiningUnit attribute, or no result if the unit must be
     * inferred from the DefiningValues measure class.
     *
     * @param prop IfcPropertyTableValue entity.
     * @return The DefiningUnit entity, or no result.
     */
    resolvePropertyTableDefiningUnit(prop: Entity): Entity;
    /**
     * Resolve the unit entity attached to a property or quantity.
     *
     * Returns the Unit attribute directly attached to the property (for
     * IfcPropertySingleValue, IfcPhysicalSimpleQuantity, etc.). Returns no result
     * if the unit must be inferred from the measure class.
     *
     * @param prop IfcProperty or IfcPhysicalQuantity entity.
     * @return The attached IfcUnit, or no result if none is attached.
     */
    resolvePropertyUnit(prop: Entity): Entity;
    /**
     * Remove units from the project's IfcUnitAssignment.
     *
     * Removes the given units from the IfcUnitAssignment's Units aggregate.
     * If the aggregate becomes empty, the IfcUnitAssignment entity itself is
     * deleted. The unit entities are not deleted.
     *
     * @param file File containing the unit assignment.
     * @param units Units to unassign.
     */
    unassignUnit(file: IfcFile, units: Entity[]): void;
}

export interface Api {
  aggregate: AggregateApi;
  attribute: AttributeApi;
  boundary: BoundaryApi;
  classification: ClassificationApi;
  cogo: CogoApi;
  compute: ComputeApi;
  constraint: ConstraintApi;
  context: ContextApi;
  control: ControlApi;
  cost: CostApi;
  document: DocumentApi;
  drawing: DrawingApi;
  element: ElementApi;
  entity: EntityApi;
  feature: FeatureApi;
  geometry: GeometryApi;
  georeference: GeoreferenceApi;
  grid: GridApi;
  group: GroupApi;
  guid: GuidApi;
  layer: LayerApi;
  library: LibraryApi;
  material: MaterialApi;
  nest: NestApi;
  owner: OwnerApi;
  placement: PlacementApi;
  profile: ProfileApi;
  project: ProjectApi;
  pset: PsetApi;
  register: RegisterApi;
  representation: RepresentationApi;
  resource: ResourceApi;
  root: RootApi;
  schema: SchemaApi;
  selector: SelectorApi;
  sequence: SequenceApi;
  shape: ShapeApi;
  spatial: SpatialApi;
  structural: StructuralApi;
  style: StyleApi;
  system: SystemApi;
  type: TypeApi;
  unit: UnitApi;
}

/**
 * @internal
 */
export function createApi(shell: IfcOpenShell): Api {
  const raw = shell.raw as object as RawApi;
  return Object.freeze({
    aggregate: Object.freeze({
    /**
     * Assign products as parts of an aggregate object via IfcRelAggregates.
     *
     * Products already aggregated under a different parent are moved. Products
     * previously contained in a spatial structure (IfcRelContainedInSpatialStructure)
     * are removed from that containment. If an existing IfcRelAggregates
     * relationship already relates to the same parent, products are merged into it.
     */
    assignObject(file: IfcFile, options: IfcOpenShellAggregateAssignObjectOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.aggregate.assignObject(file.raw, encodeOptions(options, {"application": "application", "ownerHistory": "owner_history", "products": "products", "relatingObject": "relating_object", "user": "user"}, shell, temps, [], ["products"]));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Remove products from their IfcRelAggregates relationships.
     *
     * If no related objects remain after removal, the IfcRelAggregates
     * relationship itself is deleted.
     */
    unassignObject(file: IfcFile, options: IfcOpenShellAggregateUnassignObjectOptions): void {
      const temps: Disposable[] = [];
      try {
        raw.aggregate.unassignObject(file.raw, encodeOptions(options, {"application": "application", "products": "products", "user": "user"}, shell, temps, [], ["products"]));
      } finally {
        disposeAll(temps);
      }
    },
    }),
    attribute: Object.freeze({
    /**
     * Edit arbitrary attributes on a product.
     *
     * Applies the attribute values from the property bag to the product.
     * Optionally synchronizes PredefinedType when ElementType or ObjectType
     * changes, and optionally updates OwnerHistory.
     *
     * @param file The IFC file containing the product.
     * @param options Attribute editing options.
     */
    editAttributes(file: IfcFile, options: IfcOpenShellAttributeEditAttributesOptions): void {
      const temps: Disposable[] = [];
      try {
        raw.attribute.editAttributes(file.raw, encodeOptions(options, {"application": "application", "attributes": "attributes", "product": "product", "syncPredefinedType": "sync_predefined_type", "updateOwnerHistory": "update_owner_history", "user": "user"}, shell, temps, ["attributes"]));
      } finally {
        disposeAll(temps);
      }
    },
    }),
    boundary: Object.freeze({
    /** Assign a planar connection geometry to a space boundary relationship. */
    assignConnectionGeometry(file: IfcFile, rel_space_boundary: Entity, options: IfcOpenShellBoundaryAssignConnectionGeometryOptions): void {
      const temps: Disposable[] = [];
      try {
        raw.boundary.assignConnectionGeometry(file.raw, rel_space_boundary.raw, encodeOptions(options, {"axis": "axis", "innerBoundaries": "inner_boundaries", "location": "location", "outerBoundary": "outer_boundary", "refDirection": "ref_direction", "unitScale": "unit_scale"}, shell, temps));
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Create a copy of a space boundary relationship and its connection geometry.
     *
     * @param file File that receives the copied entities.
     * @param boundary IfcRelSpaceBoundary entity to copy.
     * @return Newly created boundary relationship, or no result if the copy cannot be created.
     */
    copyBoundary(file: IfcFile, boundary: Entity): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.boundary.copyBoundary(file.raw, boundary.raw);
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Edit attributes of a space boundary relationship.
     *
     * Updates the relating space, related building element, and boundary
     * classification. ParentBoundary and CorrespondingBoundary are set only when
     * the schema supports them (IFC4+). When omitted, those attributes are cleared.
     *
     * @param entity IfcRelSpaceBoundary entity to modify.
     * @param options Attribute values to set.
     */
    editAttributes(entity: Entity, options: IfcOpenShellBoundaryEditAttributesOptions): void {
      const temps: Disposable[] = [];
      try {
        raw.boundary.editAttributes(entity.raw, encodeOptions(options, {"correspondingBoundary": "corresponding_boundary", "internalOrExternal": "internal_or_external", "parentBoundary": "parent_boundary", "physicalOrVirtual": "physical_or_virtual", "relatedBuildingElement": "related_building_element", "relatingSpace": "relating_space"}, shell, temps));
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Remove a space boundary relationship and its connection geometry.
     *
     * Removes the connection geometry and then removes the boundary relationship.
     * Unreferenced entities belonging to the connection geometry are removed.
     *
     * @param file IFC file containing the boundary.
     * @param boundary IfcRelSpaceBoundary entity to remove.
     */
    removeBoundary(file: IfcFile, boundary: Entity): void {
      const temps: Disposable[] = [];
      try {
        raw.boundary.removeBoundary(file.raw, boundary.raw);
      } finally {
        disposeAll(temps);
      }
    },
    }),
    classification: Object.freeze({
    /**
     * Create a new IfcClassification and associate it with the project.
     *
     * The classification is linked to the first IfcProject via
     * IfcRelAssociatesClassification so that it persists in the file.
     */
    addClassification(file: IfcFile, name: string): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.classification.addClassification(file.raw, name);
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Add a classification reference and associate it with products.
     *
     * If an existing classification reference is provided, it is used directly.
     * Otherwise, a new IfcClassificationReference is created using the
     * optional identification, name, and classification fields.
     */
    addReference(file: IfcFile, options: IfcOpenShellClassificationAddReferenceOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.classification.addReference(file.raw, encodeOptions(options, {"application": "application", "classification": "classification", "identification": "identification", "name": "name", "ownerHistory": "owner_history", "products": "products", "reference": "reference", "user": "user"}, shell, temps, [], ["products"]));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Return all classification references associated with an element.
     *
     * For IfcRoot-derived elements, returns references from IfcRelAssociatesClassification.
     * For non-IfcRoot elements (e.g. IfcMaterial), returns references via
     * IfcExternalReferenceRelationship. When should_inherit is true, references
     * from the element's type are merged, with occurrence-level references
     * overriding type-level ones within the same classification system.
     */
    getReferences(element: Entity, should_inherit: boolean): Entity[] {
      const temps: Disposable[] = [];
      try {
        const result = raw.classification.getReferences(element.raw, should_inherit);
        return wrapEntities(shell, result as never) as Entity[];
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Remove an IfcClassification and all its references.
     *
     * For IFC4+, recursively deletes every IfcClassificationReference in the
     * classification hierarchy via HasReferences. For IFC2X3, deletes only
     * references whose ReferencedSource is the classification itself; nested
     * references are not traversed and may be orphaned. Any
     * IfcRelAssociatesClassification that becomes orphaned is also deleted.
     */
    removeClassification(file: IfcFile, classification: Entity): void {
      const temps: Disposable[] = [];
      try {
        raw.classification.removeClassification(file.raw, classification.raw);
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Remove a classification reference association from products.
     *
     * If the reference is no longer associated with any products after
     * removal, the reference entity itself is also deleted.
     */
    removeReference(file: IfcFile, options: IfcOpenShellClassificationRemoveReferenceOptions): void {
      const temps: Disposable[] = [];
      try {
        raw.classification.removeReference(file.raw, encodeOptions(options, {"application": "application", "products": "products", "reference": "reference", "user": "user"}, shell, temps, [], ["products"]));
      } finally {
        disposeAll(temps);
      }
    },
    }),
    cogo: Object.freeze({
    /**
     * Add a single survey point as an IfcAnnotation.
     *
     * Creates an IfcAnnotation with PredefinedType SURVEY, wraps the given
     * IfcPoint in an IfcShapeRepresentation (Annotation/Point), and places
     * the annotation in the given or first IfcSite. The annotation's
     * ObjectPlacement is set to the world coordinate system of the
     * Model/Annotation/MODEL_VIEW representation context.
     *
     * @param file File that receives the new entities.
     * @param options Survey point geometry and placement options.
     * @return The newly created IfcAnnotation, or no result if creation fails.
     */
    addSurveyPoint(file: IfcFile, options: IfcOpenShellCogoAddSurveyPointOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.cogo.addSurveyPoint(file.raw, encodeOptions(options, {"application": "application", "ownerHistory": "owner_history", "site": "site", "surveyPoint": "survey_point", "user": "user"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Replace the survey point geometry of an existing annotation.
     *
     * Replaces the annotation's existing survey point with the given IfcPoint.
     * The annotation must already have a shape representation.
     *
     * @param annotation IfcAnnotation whose survey point to replace.
     * @param survey_point IfcPoint to assign as the new geometry.
     */
    assignSurveyPoint(annotation: Entity, survey_point: Entity): void {
      const temps: Disposable[] = [];
      try {
        raw.cogo.assignSurveyPoint(annotation.raw, survey_point.raw);
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Update the coordinates of the survey point inside an existing annotation.
     *
     * Updates the coordinates of the annotation's survey point. If the existing
     * point is two-dimensional, only x and y are written; otherwise all three
     * coordinates are used.
     *
     * @param annotation IfcAnnotation containing the survey point.
     * @param x Easting or X coordinate in model units.
     * @param y Northing or Y coordinate in model units.
     * @param z Elevation or Z coordinate in model units.
     */
    editSurveyPoint(annotation: Entity, x: number, y: number, z: number): void {
      const temps: Disposable[] = [];
      try {
        raw.cogo.editSurveyPoint(annotation.raw, x, y, z);
      } finally {
        disposeAll(temps);
      }
    },
    }),
    compute: Object.freeze({
    /**
     * Compute a derived attribute value for an instance.
     *
     * Evaluates derived attributes defined in the IFC schema (e.g.
     * IfcDirection.DirectionRatios) for the given instance and attribute.
     *
     * @param instance The entity instance.
     * @param attribute_name The name of the derived attribute.
     * @return The computed value, or no result if it cannot be computed. Release it with value_free.
     */
    derived(instance: Entity, attribute_name: string): ValueData | null {
      const temps: Disposable[] = [];
      try {
        const result = raw.compute.derived(instance.raw, attribute_name);
        return fromRawValue(shell, result as never) as ValueData | null;
      } finally {
        disposeAll(temps);
      }
    },
    }),
    constraint: Object.freeze({
    /**
     * Add a new IfcMetric constraint. If an objective is provided, the metric
     * is appended to the objective's benchmark values.
     */
    addMetric(file: IfcFile, objective: Entity): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.constraint.addMetric(file.raw, objective.raw);
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Create a chain of IfcReference entities from a dot-separated path and
     * assign it to a metric's ReferencePath attribute.
     *
     * For example, a path "Pset_WallCommon.FireRating" produces two IfcReference
     * entities linked by InnerReference, with AttributeIdentifier set to
     * "Pset_WallCommon" and "FireRating" respectively.
     *
     * @return The created IfcReference chain in order from outermost to innermost.
     */
    addMetricReference(file: IfcFile, metric: Entity, reference_path: string): Entity[] {
      const temps: Disposable[] = [];
      try {
        const result = raw.constraint.addMetricReference(file.raw, metric.raw, reference_path);
        return wrapEntities(shell, result as never) as Entity[];
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Create a new IfcObjective constraint with default attributes.
     *
     * Sets Name to "Unnamed", ConstraintGrade and ObjectiveQualifier to "NOTDEFINED".
     */
    addObjective(file: IfcFile): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.constraint.addObjective(file.raw);
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Assign a constraint to a list of products via IfcRelAssociatesConstraint.
     *
     * If the constraint is already associated with some of the products, only
     * new products are added to the existing relationship.
     */
    assignConstraint(file: IfcFile, options: IfcOpenShellConstraintAssignConstraintOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.constraint.assignConstraint(file.raw, encodeOptions(options, {"application": "application", "constraint": "constraint", "ownerHistory": "owner_history", "products": "products", "user": "user"}, shell, temps, [], ["products"]));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Remove a constraint entity and clean up orphaned IfcRelAssociatesConstraint
     * relationships that reference it.
     */
    removeConstraint(file: IfcFile, constraint: Entity): void {
      const temps: Disposable[] = [];
      try {
        raw.constraint.removeConstraint(file.raw, constraint.raw);
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Remove an IfcMetric and its reference path chain.
     *
     * Deletes the metric's IfcReference chain (via InnerReference), then removes
     * the metric entity. Orphaned IfcRelAssociatesConstraint and
     * IfcResourceConstraintRelationship entities are also deleted.
     */
    removeMetric(file: IfcFile, metric: Entity): void {
      const temps: Disposable[] = [];
      try {
        raw.constraint.removeMetric(file.raw, metric.raw);
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Remove constraint associations from products.
     *
     * Products are removed from existing IfcRelAssociatesConstraint
     * relationships. If no products remain, the relationship is removed.
     */
    unassignConstraint(file: IfcFile, options: IfcOpenShellConstraintUnassignConstraintOptions): void {
      const temps: Disposable[] = [];
      try {
        raw.constraint.unassignConstraint(file.raw, encodeOptions(options, {"application": "application", "constraint": "constraint", "products": "products", "user": "user"}, shell, temps, [], ["products"]));
      } finally {
        disposeAll(temps);
      }
    },
    }),
    context: Object.freeze({
    /**
     * Create a geometric representation context or subcontext.
     *
     * When parent is omitted, creates an IfcGeometricRepresentationContext with
     * a world coordinate system and attaches it to the IfcProject's
     * RepresentationContexts. A context_type of "Plan" creates a 2D context
     * (coordinate space dimension 2); all other values create a 3D context.
     *
     * When parent is provided, creates an IfcGeometricRepresentationSubContext
     * as a child of the given parent context.
     */
    addContext(file: IfcFile, options: IfcOpenShellContextAddContextOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.context.addContext(file.raw, encodeOptions(options, {"contextIdentifier": "context_identifier", "contextType": "context_type", "parent": "parent", "targetScale": "target_scale", "targetView": "target_view"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Edit attributes of an existing geometric representation context.
     *
     * Applies attribute key-value pairs from the props builder to the context entity.
     */
    editContext(file: IfcFile, context: Entity, attributes: PsetProperties | PsetInput): void {
      const temps: Disposable[] = [];
      try {
        raw.context.editContext(file.raw, context.raw, toRawPsetProperties(shell, attributes as PsetProperties | PsetInput, temps));
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Remove a geometric representation context and its subcontexts recursively.
     *
     * For subcontexts, IfcCoordinateOperation references are removed and other
     * referencing entities are redirected to the parent context. For top-level
     * contexts, representations using the context are unassigned from their
     * elements and removed.
     */
    removeContext(file: IfcFile, context: Entity): void {
      const temps: Disposable[] = [];
      try {
        raw.context.removeContext(file.raw, context.raw);
      } finally {
        disposeAll(temps);
      }
    },
    }),
    control: Object.freeze({
    /**
     * Assign a planning control or constraint to objects via IfcRelAssignsToControl.
     *
     * Objects already assigned to the control are skipped. If an existing
     * IfcRelAssignsToControl relationship exists for the control, new objects
     * are merged into it.
     */
    assignControl(file: IfcFile, options: IfcOpenShellControlAssignControlOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.control.assignControl(file.raw, encodeOptions(options, {"application": "application", "ownerHistory": "owner_history", "relatedObjects": "related_objects", "relatingControl": "relating_control", "user": "user"}, shell, temps, [], ["relatedObjects"]));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Remove objects from an IfcRelAssignsToControl relationship.
     *
     * If no related objects remain after removal, the relationship is deleted.
     */
    unassignControl(file: IfcFile, options: IfcOpenShellControlUnassignControlOptions): void {
      const temps: Disposable[] = [];
      try {
        raw.control.unassignControl(file.raw, encodeOptions(options, {"application": "application", "relatedObjects": "related_objects", "relatingControl": "relating_control", "user": "user"}, shell, temps, [], ["relatedObjects"]));
      } finally {
        disposeAll(temps);
      }
    },
    }),
    cost: Object.freeze({
    /**
     * Create an IfcCostItem entity.
     *
     * When both cost_schedule and cost_item are provided, cost_schedule takes
     * precedence and the item is assigned to it via IfcRelAssignsToControl.
     * When only cost_item is provided, the item is nested under it via
     * IfcRelNests.
     *
     * @param file File that receives the new entity.
     * @param options Schedule/parent placement and ownership options.
     * @return Newly created IfcCostItem.
     */
    addCostItem(file: IfcFile, options: IfcOpenShellCostAddCostItemOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.cost.addCostItem(file.raw, encodeOptions(options, {"application": "application", "costItem": "cost_item", "costSchedule": "cost_schedule", "ownerHistory": "owner_history", "user": "user"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Create an IfcPhysicalQuantity and add it to a cost item.
     *
     * Creates a quantity of the given IFC class (e.g. "IfcQuantityLength",
     * "IfcQuantityCount") with Name "Unnamed" and appends it to the cost item's
     * CostQuantities aggregate. For IfcQuantityCount, the initial value is set
     * to the number of controlled objects.
     *
     * @param file File that receives the new entity.
     * @param cost_item IfcCostItem to receive the quantity.
     * @param ifc_class IFC quantity class name.
     * @return Newly created IfcPhysicalQuantity.
     */
    addCostItemQuantity(file: IfcFile, cost_item: Entity, ifc_class: string): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.cost.addCostItemQuantity(file.raw, cost_item.raw, ifc_class);
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Create an IfcCostSchedule entity.
     *
     * Creates the schedule with the given name, predefined type, and update date.
     * The update_date is stored as an IfcDateAndTime on IFC2X3 or as a string
     * on IFC4+.
     *
     * @param file File that receives the new entity.
     * @param name Schedule name. When omitted or empty, no name is assigned.
     * @param predefined_type IFC predefined type enum value (e.g. "BUDGET", "COSTPLAN").
     * @param update_date ISO 8601 date-time string for the UpdateDate attribute.
     * @param owner_history Owner history for the new entity. When omitted, no owner history is assigned.
     * @return Newly created IfcCostSchedule.
     */
    addCostSchedule(file: IfcFile, name: string, predefined_type: string, update_date: string, owner_history: Entity): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.cost.addCostSchedule(file.raw, name, predefined_type, update_date, owner_history.raw);
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Create an IfcCostValue and attach it to a parent entity.
     *
     * Appends the new IfcCostValue to the parent's CostValues (IfcCostItem),
     * BaseCosts (IfcConstructionResource), or Components (IfcCostValue)
     * aggregate, depending on the parent's type.
     *
     * @param file File that receives the new entity.
     * @param parent IfcCostItem, IfcConstructionResource, or IfcCostValue to receive the value.
     * @return Newly created IfcCostValue.
     */
    addCostValue(file: IfcFile, parent: Entity): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.cost.addCostValue(file.raw, parent.raw);
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Assign product quantities to a cost item.
     *
     * For each product, creates an IfcRelAssignsToControl linking the cost item
     * to the product. If prop_name is provided, matching quantities from the
     * products' IfcElementQuantity property sets are collected into the cost
     * item's CostQuantities. If prop_name is omitted or empty and the cost item has a
     * single IfcQuantityCount, its value is updated to the count of assigned
     * non-resource objects. IfcSpatialElement products are skipped.
     *
     * @param file File containing the cost item and products.
     * @param cost_item IfcCostItem to assign quantities to.
     * @param products Products whose quantities to collect.
     * @param prop_name Quantity property name to match. When omitted, no named quantity is collected.
     * @param options Ownership options for the assignment relationship.
     */
    assignCostItemQuantity(file: IfcFile, cost_item: Entity, products: Entity[], prop_name: string, options: IfcOpenShellCostAssignCostItemQuantityOptions): void {
      const temps: Disposable[] = [];
      try {
        raw.cost.assignCostItemQuantity(file.raw, cost_item.raw, toRawSequence(products, shell, temps), prop_name, encodeOptions(options, {"application": "application", "ownerHistory": "owner_history", "user": "user"}, shell, temps));
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Assign a cost rate's values to a cost item.
     *
     * Removes the cost item's existing CostValues, then assigns the cost rate's
     * CostValues aggregate directly (sharing, not copying).
     *
     * @param file File containing both entities.
     * @param cost_item IfcCostItem to receive the values.
     * @param cost_rate IfcCostItem whose CostValues to assign.
     */
    assignCostValue(file: IfcFile, cost_item: Entity, cost_rate: Entity): void {
      const temps: Disposable[] = [];
      try {
        raw.cost.assignCostValue(file.raw, cost_item.raw, cost_rate.raw);
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Calculate and populate cost values from assigned resources.
     *
     * Removes existing CostValues from the cost item, then for each
     * IfcConstructionResource assigned (directly or via an IfcTask), creates
     * an IfcCostValue with the resource's base cost rate multiplied by its
     * quantity. Resources with day-based units are converted using an 8-hour
     * workday.
     *
     * @param file File containing the cost item.
     * @param cost_item IfcCostItem to populate.
     */
    calculateCostItemResourceValue(file: IfcFile, cost_item: Entity): void {
      const temps: Disposable[] = [];
      try {
        raw.cost.calculateCostItemResourceValue(file.raw, cost_item.raw);
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Copy an IfcCostItem and its nested children.
     *
     * Creates independent copies of the cost item, nested child items, property
     * sets, and IfcRelDefinesByProperties relationships. The returned list contains
     * the new root item followed by its descendants.
     *
     * @param file File that receives the copied entities.
     * @param cost_item IfcCostItem to copy.
     * @return List of newly created IfcCostItem entities, with the root first.
     */
    copyCostItem(file: IfcFile, cost_item: Entity): Entity[] {
      const temps: Disposable[] = [];
      try {
        const result = raw.cost.copyCostItem(file.raw, cost_item.raw);
        return wrapEntities(shell, result as never) as Entity[];
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Copy the cost values from one cost item to another.
     *
     * Removes existing CostValues from the destination, then creates independent
     * copies of the source values and their component trees.
     *
     * @param file File containing both cost items.
     * @param source IfcCostItem to copy values from.
     * @param destination IfcCostItem to copy values to.
     */
    copyCostItemValues(file: IfcFile, source: Entity, destination: Entity): void {
      const temps: Disposable[] = [];
      try {
        raw.cost.copyCostItemValues(file.raw, source.raw, destination.raw);
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Copy an IfcCostSchedule and all its controlled cost items.
     *
     * Creates an independent schedule and independent copies of each controlled
     * IfcCostItem, then assigns the copies to the new schedule.
     *
     * @param file File that receives the copied entities.
     * @param cost_schedule IfcCostSchedule to copy.
     * @param options Ownership options for the new schedule and its items.
     * @return Newly created IfcCostSchedule.
     */
    copyCostSchedule(file: IfcFile, cost_schedule: Entity, options: IfcOpenShellCostCopyCostScheduleOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.cost.copyCostSchedule(file.raw, cost_schedule.raw, encodeOptions(options, {"application": "application", "ownerHistory": "owner_history", "user": "user"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Edit attributes of an IfcCostItem.
     *
     * Applies attribute changes from the property bag to the cost item.
     *
     * @param file File containing the cost item.
     * @param cost_item IfcCostItem entity to edit.
     * @param attributes Property bag of attribute name/value pairs.
     */
    editCostItem(file: IfcFile, cost_item: Entity, attributes: PsetProperties | PsetInput): void {
      const temps: Disposable[] = [];
      try {
        raw.cost.editCostItem(file.raw, cost_item.raw, toRawPsetProperties(shell, attributes as PsetProperties | PsetInput, temps));
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Edit attributes of an IfcPhysicalQuantity.
     *
     * Applies attribute changes from the property bag to the physical quantity.
     *
     * @param file File containing the quantity.
     * @param physical_quantity IfcPhysicalQuantity entity to edit.
     * @param attributes Property bag of attribute name/value pairs.
     */
    editCostItemQuantity(file: IfcFile, physical_quantity: Entity, attributes: PsetProperties | PsetInput): void {
      const temps: Disposable[] = [];
      try {
        raw.cost.editCostItemQuantity(file.raw, physical_quantity.raw, toRawPsetProperties(shell, attributes as PsetProperties | PsetInput, temps));
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Edit attributes of an IfcCostSchedule.
     *
     * Applies attribute changes from the property bag to the cost schedule.
     *
     * @param file File containing the cost schedule.
     * @param cost_schedule IfcCostSchedule entity to edit.
     * @param attributes Property bag of attribute name/value pairs.
     */
    editCostSchedule(file: IfcFile, cost_schedule: Entity, attributes: PsetProperties | PsetInput): void {
      const temps: Disposable[] = [];
      try {
        raw.cost.editCostSchedule(file.raw, cost_schedule.raw, toRawPsetProperties(shell, attributes as PsetProperties | PsetInput, temps));
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Edit attributes of an IfcCostValue.
     *
     * Applies attribute changes from the property bag. The AppliedValue attribute
     * is wrapped as IfcMonetaryMeasure. When edit_unit_basis is true, the
     * UnitBasis (IfcMeasureWithUnit) is replaced or cleared.
     *
     * @param file File containing the cost value.
     * @param cost_value IfcCostValue entity to edit.
     * @param attributes Property bag of attribute name/value pairs.
     * @param options Unit basis replacement options.
     */
    editCostValue(file: IfcFile, cost_value: Entity, attributes: PsetProperties | PsetInput, options: IfcOpenShellCostEditCostValueOptions): void {
      const temps: Disposable[] = [];
      try {
        raw.cost.editCostValue(file.raw, cost_value.raw, toRawPsetProperties(shell, attributes as PsetProperties | PsetInput, temps), encodeOptions(options, {"clearUnitBasis": "clear_unit_basis", "editUnitBasis": "edit_unit_basis", "unitComponent": "unit_component", "valueComponent": "value_component"}, shell, temps));
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Set an IfcCostValue's attributes from a formula expression.
     *
     * Parses a text formula (e.g. "100*5", "sum(10+20)") and applies the
     * resulting value tree to the cost value's AppliedValue, Category,
     * ArithmeticOperator, and Component cost values.
     *
     * @param file File containing the cost value.
     * @param cost_value IfcCostValue entity to update.
     * @param formula Formula string to parse.
     */
    editCostValueFormula(file: IfcFile, cost_value: Entity, formula: string): void {
      const temps: Disposable[] = [];
      try {
        raw.cost.editCostValueFormula(file.raw, cost_value.raw, formula);
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Remove an IfcCostItem and its nested children from the file.
     *
     * Recursively removes nested child cost items, then removes the item's
     * IfcRelNests and IfcRelAssignsToControl relationships (when the item is
     * the sole RelatedObject), and finally removes the item itself.
     *
     * @param file File containing the cost item.
     * @param cost_item IfcCostItem to remove.
     */
    removeCostItem(file: IfcFile, cost_item: Entity): void {
      const temps: Disposable[] = [];
      try {
        raw.cost.removeCostItem(file.raw, cost_item.raw);
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Remove a physical quantity from a cost item.
     *
     * If the quantity is only referenced by this cost item, it is deleted from
     * the file. Otherwise it is removed from the CostQuantities aggregate only.
     *
     * @param file File containing the cost item.
     * @param cost_item IfcCostItem that owns the quantity.
     * @param physical_quantity IfcPhysicalQuantity to remove.
     */
    removeCostItemQuantity(file: IfcFile, cost_item: Entity, physical_quantity: Entity): void {
      const temps: Disposable[] = [];
      try {
        raw.cost.removeCostItemQuantity(file.raw, cost_item.raw, physical_quantity.raw);
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Remove an IfcCostSchedule and all its controlled cost items from the file.
     *
     * Removes each controlled IfcCostItem (recursively), then removes the
     * schedule's IfcRelAssignsToControl relationships and the schedule itself.
     *
     * @param file File containing the cost schedule.
     * @param cost_schedule IfcCostSchedule to remove.
     */
    removeCostSchedule(file: IfcFile, cost_schedule: Entity): void {
      const temps: Disposable[] = [];
      try {
        raw.cost.removeCostSchedule(file.raw, cost_schedule.raw);
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Remove an IfcCostValue from its parent and the file.
     *
     * If the cost value is only referenced by this parent, it is deleted from
     * the file. Otherwise it is removed from the parent's CostValues, BaseCosts,
     * or Components aggregate, depending on the parent's type.
     *
     * @param file File containing the cost value.
     * @param parent IfcCostItem, IfcConstructionResource, or IfcCostValue that owns the value.
     * @param cost_value IfcCostValue to remove.
     */
    removeCostValue(file: IfcFile, parent: Entity, cost_value: Entity): void {
      const temps: Disposable[] = [];
      try {
        raw.cost.removeCostValue(file.raw, parent.raw, cost_value.raw);
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Remove product quantity assignments from a cost item.
     *
     * Removes the cost item's control relationship over the given products and
     * strips any quantities from CostQuantities that are referenced by those
     * products' IfcElementQuantity property sets. If a lone IfcQuantityCount
     * remains, its value is updated to the remaining object count or removed
     * if zero.
     *
     * @param file File containing the cost item and products.
     * @param cost_item IfcCostItem to unassign from.
     * @param products Products to unassign.
     * @param options Ownership options for updating the relationship.
     */
    unassignCostItemQuantity(file: IfcFile, cost_item: Entity, products: Entity[], options: IfcOpenShellCostUnassignCostItemQuantityOptions): void {
      const temps: Disposable[] = [];
      try {
        raw.cost.unassignCostItemQuantity(file.raw, cost_item.raw, toRawSequence(products, shell, temps), encodeOptions(options, {"application": "application", "user": "user"}, shell, temps));
      } finally {
        disposeAll(temps);
      }
    },
    }),
    document: Object.freeze({
    /**
     * Create a new IfcDocumentInformation and link it to a parent.
     *
     * If the parent is an IfcProject or IfcContext, the document is linked via
     * IfcRelAssociatesDocument. If the parent is another IfcDocumentInformation,
     * the new document is linked as a child via IfcDocumentInformationRelationship.
     * If parent is omitted, the first IfcProject is used.
     */
    addInformation(file: IfcFile, options: IfcOpenShellDocumentAddInformationOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.document.addInformation(file.raw, encodeOptions(options, {"application": "application", "ownerHistory": "owner_history", "parent": "parent", "user": "user"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Create a new IfcDocumentReference.
     *
     * If an IfcDocumentInformation is provided, the reference is linked to it
     * (via ReferencedDocument in IFC4+ or DocumentReferences in IFC2X3).
     */
    addReference(file: IfcFile, information: Entity): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.document.addReference(file.raw, information.raw);
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Assign a document (IfcDocumentInformation or IfcDocumentReference) to products.
     *
     * Creates or updates an IfcRelAssociatesDocument relationship. Products
     * already associated with the document are skipped.
     */
    assignDocument(file: IfcFile, options: IfcOpenShellDocumentAssignDocumentOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.document.assignDocument(file.raw, encodeOptions(options, {"application": "application", "document": "document", "ownerHistory": "owner_history", "products": "products", "user": "user"}, shell, temps, [], ["products"]));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Remove an IfcDocumentInformation and its entire document tree.
     *
     * Recursively deletes all child IfcDocumentReference entities, child
     * IfcDocumentInformation entities (via IsPointer relationships), and all
     * associated IfcRelAssociatesDocument relationships.
     */
    removeInformation(file: IfcFile, information: Entity): void {
      const temps: Disposable[] = [];
      try {
        raw.document.removeInformation(file.raw, information.raw);
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Remove an IfcDocumentReference and its association relationships.
     *
     * Deletes the reference entity and all IfcRelAssociatesDocument relationships
     * that reference it.
     */
    removeReference(file: IfcFile, reference: Entity): void {
      const temps: Disposable[] = [];
      try {
        raw.document.removeReference(file.raw, reference.raw);
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Remove products from a document association.
     *
     * Products are removed from IfcRelAssociatesDocument relationships. If no
     * products remain in a relationship, the relationship is deleted.
     */
    unassignDocument(file: IfcFile, options: IfcOpenShellDocumentUnassignDocumentOptions): void {
      const temps: Disposable[] = [];
      try {
        raw.document.unassignDocument(file.raw, encodeOptions(options, {"application": "application", "document": "document", "products": "products", "user": "user"}, shell, temps, [], ["products"]));
      } finally {
        disposeAll(temps);
      }
    },
    }),
    drawing: Object.freeze({
    /**
     * Assign a product to an annotation object via IfcRelAssignsToProduct.
     *
     * When the relating product is an IfcGridAxis, the assignment is created
     * against the owning IfcGrid with the axis tag as the relationship name;
     * a new IfcRelAssignsToProduct is always created for grid axes even if one
     * already exists for the same axis tag. For non-grid products, if an
     * existing IfcRelAssignsToProduct already references the relating product,
     * the related object is appended to its RelatedObjects aggregate instead
     * of creating a new relationship. Returns no result for an exact duplicate.
     *
     * @param file IFC file to modify.
     * @param options Assignment parameters.
     * @return IfcRelAssignsToProduct relationship, or no result for a duplicate or failure.
     */
    assignProduct(file: IfcFile, options: IfcOpenShellDrawingAssignProductOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.drawing.assignProduct(file.raw, encodeOptions(options, {"application": "application", "ownerHistory": "owner_history", "relatedObject": "related_object", "relatingProduct": "relating_product", "user": "user"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Remove a product assignment from an annotation object.
     *
     * Finds and removes the IfcRelAssignsToProduct relating the product to the
     * object. If the relationship contains only this one related object, the
     * entire relationship is removed; otherwise only the object is removed from
     * the RelatedObjects aggregate.
     *
     * @param file IFC file to modify.
     * @param options Unassignment parameters.
     */
    unassignProduct(file: IfcFile, options: IfcOpenShellDrawingUnassignProductOptions): void {
      const temps: Disposable[] = [];
      try {
        raw.drawing.unassignProduct(file.raw, encodeOptions(options, {"application": "application", "relatedObject": "related_object", "relatingProduct": "relating_product", "user": "user"}, shell, temps));
      } finally {
        disposeAll(temps);
      }
    },
    }),
    element: Object.freeze({
    /**
     * Return the aggregate parent of an element.
     *
     * Returns the RelatingObject of an IfcRelAggregates relationship. In IFC2X3,
     * returns no result when the decomposition uses IfcRelNests instead.
     *
     * @param instance The element to query.
     * @return The aggregate parent, or no result if the element is not aggregated.
     */
    getAggregate(instance: Entity): Entity | null {
      const temps: Disposable[] = [];
      try {
        const result = raw.element.getAggregate(instance.raw);
        return wrapEntity(shell, result) as Entity | null;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Return elements directly contained in a spatial element.
     *
     * Returns RelatedElements from the spatial element's containment relationships.
     *
     * @param element The spatial element (e.g. IfcBuildingStorey).
     * @return List of contained elements.
     */
    getContained(element: Entity): Entity[] {
      const temps: Disposable[] = [];
      try {
        const result = raw.element.getContained(element.raw);
        return wrapEntities(shell, result as never) as Entity[];
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Return the spatial container of an element.
     *
     * By default considers indirect spatial containers (e.g. a building storey
     * for an element inside an aggregate). When
     * direct_only is true, only a direct ContainedInStructure relationship
     * is considered.
     *
     * @param instance The element to query.
     * @param options Container lookup options.
     * @return The spatial container, or no result if the element is not contained.
     */
    getContainer(instance: Entity, options: IfcOpenShellElementGetContainerOptions): Entity | null {
      const temps: Disposable[] = [];
      try {
        const result = raw.element.getContainer(instance.raw, encodeOptions(options, {"directOnly": "direct_only", "ifcClass": "ifc_class"}, shell, temps));
        return wrapEntity(shell, result) as Entity | null;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Return the controls assigned to an element.
     *
     * Returns controls from the element's IfcRelAssignsToControl relationships.
     *
     * @param element The element to query.
     * @return List of IfcControl entities.
     */
    getControls(element: Entity): Entity[] {
      const temps: Disposable[] = [];
      try {
        const result = raw.element.getControls(element.raw);
        return wrapEntities(shell, result as never) as Entity[];
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Return the full spatial decomposition of an element.
     *
     * Returns subelements related through containment, aggregation, openings,
     * fillings, and nesting. When is_recursive is true (default), the result
     * includes the full hierarchy in breadth-first order.
     *
     * @param element The root element.
     * @param options Decomposition traversal options.
     * @return Ordered list of decomposed subelements (excluding the root).
     */
    getDecomposition(element: Entity, options: IfcOpenShellElementGetDecompositionOptions): Entity[] {
      const temps: Disposable[] = [];
      try {
        const result = raw.element.getDecomposition(element.raw, encodeOptions(options, {"isRecursive": "is_recursive"}, shell, temps));
        return wrapEntities(shell, result as never) as Entity[];
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Return elements assigned to a presentation layer.
     *
     * Returns elements whose geometry appears in AssignedItems of the
     * IfcPresentationLayerAssignment.
     *
     * @param layer The IfcPresentationLayerAssignment entity.
     * @return List of elements on the layer.
     */
    getElementsByLayer(layer: Entity): Entity[] {
      const temps: Disposable[] = [];
      try {
        const result = raw.element.getElementsByLayer(layer.raw);
        return wrapEntities(shell, result as never) as Entity[];
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Return elements that use a material, directly or via a material set.
     *
     * Returns elements associated through IfcRelAssociatesMaterial, including
     * elements using the material as part of a layer, profile, constituent, or
     * material list.
     *
     * @param material The IfcMaterial or material set entity.
     * @return List of elements using the material.
     */
    getElementsByMaterial(material: Entity): Entity[] {
      const temps: Disposable[] = [];
      try {
        const result = raw.element.getElementsByMaterial(material.raw);
        return wrapEntities(shell, result as never) as Entity[];
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Return elements that use a profile definition in their representation.
     *
     * Returns elements whose geometry references the profile through their
     * representation items.
     *
     * @param profile The IfcProfileDef entity.
     * @return List of elements using the profile.
     */
    getElementsByProfile(profile: Entity): Entity[] {
      const temps: Disposable[] = [];
      try {
        const result = raw.element.getElementsByProfile(profile.raw);
        return wrapEntities(shell, result as never) as Entity[];
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Return elements that use a geometric representation.
     *
     * Returns IfcProduct and IfcTypeProduct entities that reference the
     * representation through their product representation or representation map.
     *
     * @param representation The IfcShapeRepresentation entity.
     * @return List of elements using the representation.
     */
    getElementsByRepresentation(representation: Entity): Entity[] {
      const temps: Disposable[] = [];
      try {
        const result = raw.element.getElementsByRepresentation(representation.raw);
        return wrapEntities(shell, result as never) as Entity[];
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Return elements whose geometric representation uses a style.
     *
     * Returns elements whose shape representations contain the style through
     * IfcStyledItem relationships.
     *
     * @param style The IfcPresentationStyle entity.
     * @return List of elements using the style.
     */
    getElementsByStyle(style: Entity): Entity[] {
      const temps: Disposable[] = [];
      try {
        const result = raw.element.getElementsByStyle(style.raw);
        return wrapEntities(shell, result as never) as Entity[];
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Return the opening element that an element fills.
     *
     * Returns the RelatingOpeningElement from the element's filling relationship.
     * This typically applies to windows and doors.
     *
     * @param element The filling element (e.g. IfcWindow).
     * @return The IfcOpeningElement being filled, or no result if none is associated.
     */
    getFilledVoid(element: Entity): Entity | null {
      const temps: Disposable[] = [];
      try {
        const result = raw.element.getFilledVoid(element.raw);
        return wrapEntity(shell, result) as Entity | null;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Return the groups that an element is assigned to.
     *
     * Returns groups from the element's IfcRelAssignsToGroup relationships.
     *
     * @param element The element to query.
     * @return List of IfcGroup entities.
     */
    getGroups(element: Entity): Entity[] {
      const temps: Disposable[] = [];
      try {
        const result = raw.element.getGroups(element.raw);
        return wrapEntities(shell, result as never) as Entity[];
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Return the presentation layers that an element is part of.
     *
     * Returns IfcPresentationLayerAssignment entities referenced by the
     * element's representation.
     *
     * @param element The element to query.
     * @return List of IfcPresentationLayerAssignment entities.
     */
    getLayers(element: Entity): Entity[] {
      const temps: Disposable[] = [];
      try {
        const result = raw.element.getLayers(element.raw);
        return wrapEntities(shell, result as never) as Entity[];
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Return the material associated with an element.
     *
     * Searches HasAssociations for IfcRelAssociatesMaterial. When
     * should_inherit is true (default), falls back to the element's type.
     * When should_skip_usage is true, unwraps material set usages
     * (IfcMaterialLayerSetUsage, IfcMaterialProfileSetUsage) to their
     * underlying set.
     *
     * @param instance The element to query.
     * @param options Material lookup options.
     * @return The material entity, or no result if none is associated.
     */
    getMaterial(instance: Entity, options: IfcOpenShellElementGetMaterialOptions): Entity | null {
      const temps: Disposable[] = [];
      try {
        const result = raw.element.getMaterial(instance.raw, encodeOptions(options, {"shouldInherit": "should_inherit", "shouldSkipUsage": "should_skip_usage"}, shell, temps));
        return wrapEntity(shell, result) as Entity | null;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Return the nest parent of an element.
     *
     * Returns the RelatingObject of the applicable IfcRelNests relationship for
     * the schema.
     *
     * @param instance The element to query.
     * @return The nesting parent, or no result if the element is not nested.
     */
    getNest(instance: Entity): Entity | null {
      const temps: Disposable[] = [];
      try {
        const result = raw.element.getNest(instance.raw);
        return wrapEntity(shell, result) as Entity | null;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Return opening elements associated with an element.
     *
     * Returns RelatedOpeningElement values from the element's opening
     * relationships. Also includes openings inherited from aggregate parents.
     *
     * @param element The building element (e.g. IfcWall).
     * @return List of IfcOpeningElement entities.
     */
    getOpenings(element: Entity): Entity[] {
      const temps: Disposable[] = [];
      try {
        const result = raw.element.getOpenings(element.raw);
        return wrapEntities(shell, result as never) as Entity[];
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Return the parent of an element in the spatial hierarchy.
     *
     * Checks spatial containment, aggregation, nesting, filling, and voiding
     * relationships in that order, returning the first parent found.
     *
     * @param instance The element to query.
     * @return The parent element, or no result if the element is at the top of the hierarchy.
     */
    getParent(instance: Entity): Entity | null {
      const temps: Disposable[] = [];
      try {
        const result = raw.element.getParent(instance.raw);
        return wrapEntity(shell, result) as Entity | null;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Return the direct aggregation parts of an element.
     *
     * Returns RelatedObjects from the element's IfcRelAggregates relationships.
     *
     * @param element The element to query.
     * @return List of aggregated parts.
     */
    getParts(element: Entity): Entity[] {
      const temps: Disposable[] = [];
      try {
        const result = raw.element.getParts(element.raw);
        return wrapEntities(shell, result as never) as Entity[];
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Return property set and quantity identifiers of an element.
     *
     * Returns IfcPropertySet, IfcElementQuantity, and related property definition
     * entities. For IfcTypeObject, uses HasPropertySets; for other objects, uses
     * the applicable property-definition relationship.
     * When should_inherit is true (default), also includes property sets
     * from the element's type.
     *
     * @param element The element to query.
     * @param options Property set filtering options.
     * @return List of property definition entities.
     */
    getPsetIds(element: Entity, options: IfcOpenShellElementGetPsetIdsOptions): Entity[] {
      const temps: Disposable[] = [];
      try {
        const result = raw.element.getPsetIds(element.raw, encodeOptions(options, {"psetsOnly": "psets_only", "qtosOnly": "qtos_only", "shouldInherit": "should_inherit"}, shell, temps));
        return wrapEntities(shell, result as never) as Entity[];
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Return elements that have an external reference assigned.
     *
     * For IfcExternalReference subtypes, returns resources from the applicable
     * external-reference relationship. For classification, document, and library
     * references, returns elements from the corresponding IFC relationship.
     *
     * @param reference The IfcExternalReference or IfcExternalInformation entity.
     * @return List of elements using the reference.
     */
    getReferencedElements(reference: Entity): Entity[] {
      const temps: Disposable[] = [];
      try {
        const result = raw.element.getReferencedElements(reference.raw);
        return wrapEntities(shell, result as never) as Entity[];
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Return spatial elements that reference an element.
     *
     * Returns RelatingStructure values from the element's spatial reference
     * relationships. This includes multi-storey elements and elements spanning
     * multiple spaces.
     *
     * @param element The element to query.
     * @return List of referenced IfcSpatialElement entities.
     */
    getReferencedStructures(element: Entity): Entity[] {
      const temps: Disposable[] = [];
      try {
        const result = raw.element.getReferencedStructures(element.raw);
        return wrapEntities(shell, result as never) as Entity[];
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Return the shape aspects of an element.
     *
     * For an IfcProduct, returns shape aspects from its representation. For an
     * IfcTypeProduct, returns shape aspects from its representation maps. When
     * should_inherit is true (default), also includes shape aspects from
     * the element's type.
     *
     * @param element The IfcProduct or IfcTypeProduct.
     * @param options Shape aspect lookup options.
     * @return List of IfcShapeAspect entities.
     */
    getShapeAspects(element: Entity, options: IfcOpenShellElementGetShapeAspectsOptions): Entity[] {
      const temps: Disposable[] = [];
      try {
        const result = raw.element.getShapeAspects(element.raw, encodeOptions(options, {"shouldInherit": "should_inherit"}, shell, temps));
        return wrapEntities(shell, result as never) as Entity[];
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Return elements referenced by a spatial structure.
     *
     * Returns RelatedElements from the spatial element's reference relationships.
     *
     * @param structure The spatial element (e.g. IfcBuildingStorey).
     * @return List of referenced elements.
     */
    getStructureReferencedElements(structure: Entity): Entity[] {
      const temps: Disposable[] = [];
      try {
        const result = raw.element.getStructureReferencedElements(structure.raw);
        return wrapEntities(shell, result as never) as Entity[];
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Return the surface styles used in an element's representation.
     *
     * Collects styles from the element's materials and body representation,
     * including styles on mapped items and boolean operands.
     *
     * @param element The element to query.
     * @return List of IfcSurfaceStyle entities.
     */
    getStyles(element: Entity): Entity[] {
      const temps: Disposable[] = [];
      try {
        const result = raw.element.getStyles(element.raw);
        return wrapEntities(shell, result as never) as Entity[];
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Return the type element associated with an element occurrence.
     *
     * For an IfcTypeObject, returns the element itself. For an IfcObject,
     * returns the type assigned through the schema's type relationship.
     *
     * @param instance The element to query.
     * @return The related type element, or no result if none is assigned.
     */
    getType(instance: Entity): Entity | null {
      const temps: Disposable[] = [];
      try {
        const result = raw.element.getType(instance.raw);
        return wrapEntity(shell, result) as Entity | null;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Return all occurrences of a type element.
     *
     * Returns the RelatedObjects of the applicable type relationship for the
     * schema.
     *
     * @param type_element The type element (e.g. IfcWallType).
     * @return List of element occurrences of that type.
     */
    getTypes(type_element: Entity): Entity[] {
      const temps: Disposable[] = [];
      try {
        const result = raw.element.getTypes(type_element.raw);
        return wrapEntities(shell, result as never) as Entity[];
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Return the building element voided by an opening.
     *
     * Returns the RelatingBuildingElement from the opening relationship.
     *
     * @param element The IfcOpeningElement.
     * @return The building element being voided, or no result if none is associated.
     */
    getVoidedElement(element: Entity): Entity | null {
      const temps: Disposable[] = [];
      try {
        const result = raw.element.getVoidedElement(element.raw);
        return wrapEntity(shell, result) as Entity | null;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Check whether an element's predefined type is user-defined.
     *
     * Returns true if the element's PredefinedType is USERDEFINED, or if
     * the type's PredefinedType is USERDEFINED or not set and an
     * ElementType or ObjectType is present.
     *
     * @param element The element to check.
     * @return True if the predefined type is user-defined.
     */
    isUserdefinedType(element: Entity): boolean {
      const temps: Disposable[] = [];
      try {
        const result = raw.element.isUserdefinedType(element.raw);
        return wrap(shell, result) as boolean;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Recursively remove an element and its owned subgraph.
     *
     * Removes the element and owned subelements that have no references outside
     * the removal set. Protected elements and externally referenced elements are
     * preserved.
     *
     * @param element The root element to remove.
     */
    removeDeep(element: Entity): void {
      const temps: Disposable[] = [];
      try {
        raw.element.removeDeep(element.raw);
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Replace all references to an element with another element.
     *
     * Replaces references to old_element in all inverse relationships with
     * references to new_element.
     *
     * @param old_element The element to be replaced.
     * @param new_element The replacement element.
     */
    replaceElement(old_element: Entity, new_element: Entity): void {
      const temps: Disposable[] = [];
      try {
        raw.element.replaceElement(old_element.raw, new_element.raw);
      } finally {
        disposeAll(temps);
      }
    },
    }),
    entity: Object.freeze({
    /**
     * Remove an entity and the unshared entities it owns, recursively.
     *
     * Equivalent to entity_remove_deep_with_options with the default options.
     * The start element must have no inverses outside the subgraph.
     *
     * @param instance The root entity to remove.
     */
    removeDeep(instance: Entity): void {
      const temps: Disposable[] = [];
      try {
        raw.entity.removeDeep(instance.raw);
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Remove an entity and its owned subgraph with fine-grained control.
     *
     * An owned entity is removed only when it has no references from outside the
     * removal set. The also_consider list extends that set for this decision, and
     * the do_not_delete list protects specific entities from deletion.
     *
     * @param instance The root entity to remove.
     * @param options Additional control over the removal process.
     */
    removeDeepWithOptions(instance: Entity, options: IfcOpenShellEntityRemoveDeepOptions): void {
      const temps: Disposable[] = [];
      try {
        raw.entity.removeDeepWithOptions(instance.raw, encodeOptions(options, {"alsoConsider": "also_consider", "doNotDelete": "do_not_delete"}, shell, temps, [], ["alsoConsider", "doNotDelete"]));
      } finally {
        disposeAll(temps);
      }
    },
    }),
    feature: Object.freeze({
    /**
     * Associate a feature element with a host element.
     *
     * For IfcFeatureElementSubtraction (e.g. IfcOpeningElement), creates
     * IfcRelVoidsElement. For IfcFeatureElementAddition (e.g. IfcProjectionElement),
     * creates IfcRelProjectsElement. For IfcSurfaceFeature, creates
     * IfcRelAdheresToElement (IFC4X3) or aggregates via IfcRelAggregates (IFC4).
     * If the feature is already associated with the same host, returns the
     * existing relationship. If associated with a different host, the old
     * relationship is removed first.
     */
    addFeature(file: IfcFile, options: IfcOpenShellFeatureAddFeatureOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.feature.addFeature(file.raw, encodeOptions(options, {"application": "application", "element": "element", "feature": "feature", "user": "user"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Fill an opening with an element via IfcRelFillsElement.
     *
     * If the element already fills the same opening, returns the existing
     * relationship. If it fills a different opening, the old relationship is
     * removed first.
     */
    addFilling(file: IfcFile, opening: Entity, element: Entity): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.feature.addFilling(file.raw, opening.raw, element.raw);
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Remove a feature element and its relationships.
     *
     * For IfcFeatureElementSubtraction subclasses, removes the IfcRelVoidsElement.
     * For IfcFeatureElementAddition subclasses, removes the IfcRelProjectsElement.
     * For IfcSurfaceFeature in IFC4, unassigns from the aggregate parent. In
     * other schemas, no feature-specific relationship is removed before the
     * element itself is removed.
     * IfcOpeningElement fillings are also removed. Nested elements, property sets,
     * representations, and other inverse relationships are cleaned up as part of
     * removing the feature.
     */
    removeFeature(file: IfcFile, options: IfcOpenShellFeatureRemoveFeatureOptions): void {
      const temps: Disposable[] = [];
      try {
        raw.feature.removeFeature(file.raw, encodeOptions(options, {"application": "application", "feature": "feature", "user": "user"}, shell, temps));
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Remove a filling relationship for an element.
     *
     * Finds and deletes the IfcRelFillsElement where the element is the
     * RelatedBuildingElement.
     */
    removeFilling(file: IfcFile, element: Entity): void {
      const temps: Disposable[] = [];
      try {
        raw.feature.removeFilling(file.raw, element.raw);
      } finally {
        disposeAll(temps);
      }
    },
    }),
    geometry: Object.freeze({
    /**
     * Create an axis representation (Curve2D or Curve3D) from a polyline.
     *
     * Points are converted from SI metres to project units.
     *
     * @param file IFC file that receives the representation.
     * @param context IfcGeometricRepresentationContext.
     * @param axis Ordered XY or XYZ points defining the axis curve.
     * @return IfcShapeRepresentation entity, or no result if creation fails.
     */
    addAxisRepresentation(file: IfcFile, context: Entity, axis: number[][]): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.geometry.addAxisRepresentation(file.raw, context.raw, toRawSequence(axis, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Add boolean operands to a solid representation item.
     *
     * Creates IfcBooleanResult entities (or IfcBooleanClippingResult for
     * DIFFERENCE with half-space solids) by combining the first item with each
     * additional operand. The returned entities are listed in creation order.
     *
     * @param file IFC file that receives the boolean entities.
     * @param first_item Base solid operand.
     * @param second_items Additional operands to apply.
     * @param operator_type Boolean operator: "DIFFERENCE", "UNION", or "INTERSECTION".
     * @return Created boolean result entities, or an empty list if creation fails.
     */
    addBoolean(file: IfcFile, first_item: Entity, second_items: Entity[], operator_type: string): Entity[] {
      const temps: Disposable[] = [];
      try {
        const result = raw.geometry.addBoolean(file.raw, first_item.raw, toRawSequence(second_items, shell, temps), operator_type);
        return wrapEntities(shell, result as never) as Entity[];
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Create a door representation with lining and panel geometry.
     *
     * @param file IFC file that receives the representation.
     * @param options Door dimensions, operation type, and lining/panel properties.
     * @return IfcShapeRepresentation entity, or no result if creation fails.
     */
    addDoorRepresentation(file: IfcFile, options: IfcOpenShellGeometryAddDoorRepresentationOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.geometry.addDoorRepresentation(file.raw, encodeOptions(options, {"context": "context", "liningProperties": "lining_properties", "operationType": "operation_type", "overallHeight": "overall_height", "overallWidth": "overall_width", "panelProperties": "panel_properties", "partOfProduct": "part_of_product", "unitScale": "unit_scale"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Create a footprint representation (GeometricCurveSet) from curves.
     *
     * @param file IFC file that receives the representation.
     * @param context IfcGeometricRepresentationContext.
     * @param curves IfcCurve entities to include in the footprint.
     * @return IfcShapeRepresentation entity, or no result if creation fails.
     */
    addFootprintRepresentation(file: IfcFile, context: Entity, curves: Entity[]): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.geometry.addFootprintRepresentation(file.raw, context.raw, toRawSequence(curves, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Create a mesh representation from vertices and face indices.
     *
     * Uses IfcPolygonalFaceSet (IFC4+) or IfcFacetedBrep (IFC2X3). IFC2X3
     * always uses faceted BRep regardless of force_faceted_brep.
     *
     * @param file IFC file that receives the representation.
     * @param context IfcGeometricRepresentationContext.
     * @param options Vertices, faces, and optional faceted BRep override.
     * @return IfcShapeRepresentation entity, or no result if creation fails.
     */
    addMeshRepresentation(file: IfcFile, context: Entity, options: IfcOpenShellGeometryAddMeshRepresentationOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.geometry.addMeshRepresentation(file.raw, context.raw, encodeOptions(options, {"faces": "faces", "forceFacetedBrep": "force_faceted_brep", "vertices": "vertices"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Create a profile-based IfcExtrudedAreaSolid representation.
     *
     * Depth and plane locations are supplied in SI metres and converted to project
     * length units. Clipping kinds preserve input order but are applied from last
     * to first; entity clippings are copied before their FirstOperand is changed.
     * The placement defaults to Z=(0,0,1), X=(1,0,0), and the origin when no
     * cardinal point is supplied. The result is SweptSolid without clippings and
     * Clipping otherwise.
     *
     * @param file IFC file that receives the representation.
     * @param options Context, profile, extrusion, placement, cardinal point, and clippings.
     * @return IfcShapeRepresentation entity, or no result if creation fails.
     */
    addProfileRepresentation(file: IfcFile, options: IfcOpenShellGeometryAddProfileRepresentationOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.geometry.addProfileRepresentation(file.raw, encodeOptions(options, {"cardinalPoint": "cardinal_point", "clippingEntities": "clipping_entities", "clippingKinds": "clipping_kinds", "clippingLocations": "clipping_locations", "clippingNormals": "clipping_normals", "context": "context", "depth": "depth", "placementXAxis": "placement_x_axis", "placementZAxis": "placement_z_axis", "profile": "profile"}, shell, temps, [], ["clippingEntities"]));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Create a railing representation along a path.
     *
     * @param file IFC file that receives the representation.
     * @param options Railing path, support spacing, dimensions, and terminal type.
     * @return IfcShapeRepresentation entity, or no result if creation fails.
     */
    addRailingRepresentation(file: IfcFile, options: IfcOpenShellGeometryAddRailingRepresentationOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.geometry.addRailingRepresentation(file.raw, encodeOptions(options, {"clearWidth": "clear_width", "context": "context", "height": "height", "loopedPath": "looped_path", "railingDiameter": "railing_diameter", "railingPath": "railing_path", "supportSpacing": "support_spacing", "terminalType": "terminal_type", "unitScale": "unit_scale", "useManualSupports": "use_manual_supports"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Create or update an IfcShapeAspect within a product definition shape.
     *
     * If an aspect with the same name already exists, its items are merged.
     * Items are removed from other aspects in the same product definition shape.
     *
     * @param file IFC file that receives the aspect.
     * @param options Aspect name, items, representation, and owning product.
     * @return IfcShapeAspect entity, or no result if creation fails.
     */
    addShapeAspect(file: IfcFile, options: IfcOpenShellGeometryAddShapeAspectOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.geometry.addShapeAspect(file.raw, encodeOptions(options, {"description": "description", "items": "items", "name": "name", "partOfProduct": "part_of_product", "representation": "representation"}, shell, temps, [], ["items"]));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Create a slab-style swept solid representation (IfcExtrudedAreaSolid).
     *
     * @param file IFC file that receives the representation.
     * @param options Slab dimensions, direction, clippings, and boundary polyline.
     * @return IfcShapeRepresentation entity, or no result if creation fails.
     */
    addSlabRepresentation(file: IfcFile, options: IfcOpenShellGeometryAddSlabRepresentationOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.geometry.addSlabRepresentation(file.raw, encodeOptions(options, {"clippingEntities": "clipping_entities", "clippingKinds": "clipping_kinds", "clippingLocations": "clipping_locations", "clippingNormals": "clipping_normals", "context": "context", "depth": "depth", "directionSense": "direction_sense", "offset": "offset", "polyline": "polyline", "xAngle": "x_angle"}, shell, temps, [], ["clippingEntities"]));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Create an IfcTopologyRepresentation for a single topology item.
     *
     * @param file IFC file that receives the representation.
     * @param options Context, topology item, and optional identifier/type.
     * @return IfcTopologyRepresentation entity, or no result if creation fails.
     */
    addTopologyRepresentation(file: IfcFile, options: IfcOpenShellGeometryAddTopologyRepresentationOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.geometry.addTopologyRepresentation(file.raw, encodeOptions(options, {"context": "context", "item": "item", "representationIdentifier": "representation_identifier", "representationType": "representation_type"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Create a wall-style swept solid representation (IfcExtrudedAreaSolid).
     *
     * @param file IFC file that receives the representation.
     * @param options Wall dimensions, direction, clippings, and booleans.
     * @return IfcShapeRepresentation entity, or no result if creation fails.
     */
    addWallRepresentation(file: IfcFile, options: IfcOpenShellGeometryAddWallRepresentationOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.geometry.addWallRepresentation(file.raw, encodeOptions(options, {"booleans": "booleans", "clippingEntities": "clipping_entities", "clippingKinds": "clipping_kinds", "clippingLocations": "clipping_locations", "clippingNormals": "clipping_normals", "context": "context", "directionSense": "direction_sense", "height": "height", "length": "length", "offset": "offset", "thickness": "thickness", "xAngle": "x_angle"}, shell, temps, [], ["booleans", "clippingEntities"]));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Create a window representation with lining and panel geometry.
     *
     * @param file IFC file that receives the representation.
     * @param options Window dimensions, panel schema, lining/panel properties.
     * @return IfcShapeRepresentation entity, or no result if creation fails.
     */
    addWindowRepresentation(file: IfcFile, options: IfcOpenShellGeometryAddWindowRepresentationOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.geometry.addWindowRepresentation(file.raw, encodeOptions(options, {"context": "context", "glassThickness": "glass_thickness", "liningProperties": "lining_properties", "overallHeight": "overall_height", "overallWidth": "overall_width", "panelProperties": "panel_properties", "panelSchema": "panel_schema", "partOfProduct": "part_of_product"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Assign a representation to a product or type product.
     *
     * For IfcProduct, the representation is appended to the product's
     * IfcProductDefinitionShape. For IfcTypeProduct, an IfcRepresentationMap is
     * created and mapped representations are assigned to all related occurrences.
     * If the product has a type with existing maps and the representation is not
     * already mapped, the assignment is redirected to the type product.
     *
     * @param file IFC file to modify.
     * @param product IfcProduct or IfcTypeProduct entity.
     * @param representation IfcShapeRepresentation entity.
     * @return The product receiving the representation, or no result if assignment fails.
     */
    assignRepresentation(file: IfcFile, product: Entity, representation: Entity): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.geometry.assignRepresentation(file.raw, product.raw, representation.raw);
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Clip a solid with an infinite half-space plane (IfcBooleanClippingResult).
     *
     * Creates a DIFFERENCE boolean between the item and an IfcHalfSpaceSolid.
     * If an owning element is provided, the clipping is registered in its
     * BBIM_Boolean property set.
     *
     * @param file IFC file that receives the clipping.
     * @param options Solid, plane point, normal, and optional element/history.
     * @return IfcBooleanClippingResult entity, or no result if creation fails.
     */
    clipSolid(file: IfcFile, options: IfcOpenShellGeometryClipSolidOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.geometry.clipSolid(file.raw, encodeOptions(options, {"application": "application", "element": "element", "item": "item", "location": "location", "normal": "normal", "ownerHistory": "owner_history", "user": "user"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Clip a solid with a polygonal-bounded half-space (IfcPolygonalBoundedHalfSpace).
     *
     * Creates a DIFFERENCE boolean between the item and an
     * IfcPolygonalBoundedHalfSpace. Locations are in model units.
     *
     * @param file IFC file that receives the clipping.
     * @param options Solid, plane, boundary polygon, and optional element/history.
     * @return IfcBooleanClippingResult entity, or no result if creation fails.
     */
    clipSolidBounded(file: IfcFile, options: IfcOpenShellGeometryClipSolidBoundedOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.geometry.clipSolidBounded(file.raw, encodeOptions(options, {"application": "application", "boundaryPoints": "boundary_points", "boundaryPosition": "boundary_position", "element": "element", "item": "item", "location": "location", "normal": "normal", "ownerHistory": "owner_history", "user": "user"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Create an IfcRelConnectsElements between two elements.
     *
     * Removes any existing incompatible element connections between the two
     * elements before creating the new relationship.
     *
     * @param file IFC file that receives the relationship.
     * @param options Relating element, related element, and optional description/history.
     * @return IfcRelConnectsElements entity, or no result if creation fails.
     */
    connectElement(file: IfcFile, options: IfcOpenShellGeometryConnectElementOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.geometry.connectElement(file.raw, encodeOptions(options, {"application": "application", "description": "description", "ownerHistory": "owner_history", "relatedElement": "related_element", "relatingElement": "relating_element", "user": "user"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Create an IfcRelConnectsPathElements between two path-based elements.
     *
     * Removes incompatible existing path connections (same terminal end or same
     * pair) before creating the new relationship. Connection types are "ATSTART",
     * "ATEND", or "ATPATH".
     *
     * @param file IFC file that receives the relationship.
     * @param options Elements, connection types, and optional description/geometry/history.
     * @return IfcRelConnectsPathElements entity, or no result if creation fails.
     */
    connectPath(file: IfcFile, options: IfcOpenShellGeometryConnectPathOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.geometry.connectPath(file.raw, encodeOptions(options, {"application": "application", "connectionGeometry": "connection_geometry", "description": "description", "ownerHistory": "owner_history", "relatedConnection": "related_connection", "relatedElement": "related_element", "relatingConnection": "relating_connection", "relatingElement": "relating_element", "user": "user"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Connect two walls at their nearest endpoint.
     *
     * Computes the intersection of the second wall's axis with the first wall's
     * axis to determine ATSTART or ATEND. When is_atpath is true, the connection
     * type is ATPATH. Creates an IfcRelConnectsPathElements.
     *
     * @param file IFC file that receives the connection.
     * @param options Walls, connection mode, and optional owner history.
     * @return IfcRelConnectsPathElements entity, or no result if creation fails.
     */
    connectWall(file: IfcFile, options: IfcOpenShellGeometryConnectWallOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.geometry.connectWall(file.raw, encodeOptions(options, {"application": "application", "firstWall": "first_wall", "isAtpath": "is_atpath", "ownerHistory": "owner_history", "secondWall": "second_wall", "user": "user"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Copy a representation from one product to another.
     *
     * Copies the "Body" (or specified context) representation from the source
     * product, replaces any existing representation of the same context on the
     * target, and assigns the copy.
     *
     * @param file IFC file to modify.
     * @param options Source product, target product, and optional context identifier.
     * @return Newly created IfcShapeRepresentation, or no value when the source
     * has no representation for the requested context identifier.
     */
    copyRepresentation(file: IfcFile, options: IfcOpenShellGeometryCopyRepresentationOptions): Entity | null {
      const temps: Disposable[] = [];
      try {
        const result = raw.geometry.copyRepresentation(file.raw, encodeOptions(options, {"contextIdentifier": "context_identifier", "source": "source", "target": "target"}, shell, temps));
        return wrapEntity(shell, result) as Entity | null;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Create a wall from two XY endpoints with body representation and placement.
     *
     * Computes the wall length from the endpoint distance, creates a wall
     * representation, and sets the object placement with rotation to align the
     * wall along the baseline.
     *
     * @param file IFC file that receives the wall geometry.
     * @param options Element, context, endpoints, elevation, height, thickness, and unit flag.
     * @return IfcShapeRepresentation entity, or no result if creation fails.
     */
    create2ptWall(file: IfcFile, options: IfcOpenShellGeometryCreate2PtWallOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.geometry.create2ptWall(file.raw, encodeOptions(options, {"context": "context", "element": "element", "elevation": "elevation", "end": "end", "height": "height", "isSi": "is_si", "start": "start", "thickness": "thickness"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Remove all IfcRelConnectsElements relationships between two elements.
     *
     * Searches both ConnectedTo and ConnectedFrom inverse references on both
     * elements and removes matching relationships.
     *
     * @param file IFC file to modify.
     * @param relating_element First element.
     * @param related_element Second element.
     */
    disconnectElement(file: IfcFile, relating_element: Entity, related_element: Entity): void {
      const temps: Disposable[] = [];
      try {
        raw.geometry.disconnectElement(file.raw, relating_element.raw, related_element.raw);
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Remove path-based element connections.
     *
     * When element and connection_type are provided, removes all path connections
     * matching that type on the element. When relating_element and
     * related_element are provided, removes the specific connection between them.
     *
     * @param file IFC file to modify.
     * @param options Element, connection type, or relating/related element pair.
     */
    disconnectPath(file: IfcFile, options: IfcOpenShellGeometryDisconnectPathOptions): void {
      const temps: Disposable[] = [];
      try {
        raw.geometry.disconnectPath(file.raw, encodeOptions(options, {"connectionType": "connection_type", "element": "element", "relatedElement": "related_element", "relatingElement": "relating_element"}, shell, temps));
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Set the ObjectPlacement of a product from a 4x4 transformation matrix.
     *
     * Creates a new IfcLocalPlacement relative to the product's parent
     * (determined from aggregation, nesting, containment, or void relationships).
     * The old placement is removed if unreferenced. When
     * should_transform_children is false, child placements are re-applied in their
     * previous world-space positions.
     *
     * @param file IFC file to modify.
     * @param options Product, matrix, SI flag, and child transform flag.
     * @return Newly created IfcLocalPlacement, or no result if creation fails.
     */
    editObjectPlacement(file: IfcFile, options: IfcOpenShellGeometryEditObjectPlacementOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.geometry.editObjectPlacement(file.raw, encodeOptions(options, {"isSi": "is_si", "matrix": "matrix", "product": "product", "shouldTransformChildren": "should_transform_children"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Create a mapped representation (IfcRepresentationMap + IfcMappedItem).
     *
     * Wraps the given representation in an IfcRepresentationMap and returns a new
     * IfcShapeRepresentation of type "MappedRepresentation" containing one
     * IfcMappedItem.
     *
     * @param file IFC file that receives the mapped entities.
     * @param representation IfcShapeRepresentation to map.
     * @return New IfcShapeRepresentation with MappedRepresentation type.
     */
    mapRepresentation(file: IfcFile, representation: Entity): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.geometry.mapRepresentation(file.raw, representation.raw);
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Return the axis-aligned 2D bounding box extents of a profile.
     *
     * Computes the X and Y extents from the profile's parameterized attributes
     * (e.g. OverallWidth/OverallDepth for I-shaped profiles). When those values
     * are unavailable, geometry evaluation is used when available. Returns an
     * empty list if the extents cannot be determined.
     *
     * @param file IFC file containing the profile.
     * @param profile IfcProfileDef entity.
     * @return Two-element list {x_extent, y_extent} in model units, or an empty list.
     */
    profileExtents(file: IfcFile, profile: Entity): number[] {
      const temps: Disposable[] = [];
      try {
        const result = raw.geometry.profileExtents(file.raw, profile.raw);
        return wrap(shell, result) as number[];
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Regenerate a wall's body and axis representations from its material layers.
     *
     * Rebuilds the wall's body and axis representations using its material layers
     * and connected-wall geometry.
     *
     * @param file IFC file containing the wall.
     * @param options Wall entity, length, height, and optional angle.
     * @return New IfcShapeRepresentation for the body, or no result if regeneration fails.
     */
    regenerateWallRepresentation(file: IfcFile, options: IfcOpenShellGeometryRegenerateWallRepresentationOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.geometry.regenerateWallRepresentation(file.raw, encodeOptions(options, {"angle": "angle", "height": "height", "length": "length", "wall": "wall"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Remove boolean operands from a solid representation.
     *
     * Removes boolean operations involving the given item, restores the primary
     * operand in its parent references, and exposes the other operands in the
     * owning representation.
     *
     * @param file IFC file to modify.
     * @param item Solid operand whose boolean chain to remove.
     */
    removeBoolean(file: IfcFile, item: Entity): void {
      const temps: Disposable[] = [];
      try {
        raw.geometry.removeBoolean(file.raw, item.raw);
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Remove a representation and its unreferenced sub-entities.
     *
     * Cleans up styled items, presentation layer assignments, textures, and
     * colours. Geometric representation contexts are never deleted. Named
     * profiles are preserved when should_keep_named_profiles is true.
     *
     * @param file IFC file to modify.
     * @param representation IfcShapeRepresentation to remove.
     * @param options Whether to preserve named profiles.
     */
    removeRepresentation(file: IfcFile, representation: Entity, options: IfcOpenShellGeometryRemoveRepresentationOptions): void {
      const temps: Disposable[] = [];
      try {
        raw.geometry.removeRepresentation(file.raw, representation.raw, encodeOptions(options, {"shouldKeepNamedProfiles": "should_keep_named_profiles"}, shell, temps));
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Unassign a representation from a product or type product.
     *
     * For IfcProduct, removes the representation from the
     * IfcProductDefinitionShape and removes an empty shape definition. For
     * IfcTypeProduct, removes the matching IfcRepresentationMap and unmaps
     * occurrences. Shape aspects referencing the representation are also removed.
     *
     * @param file IFC file to modify.
     * @param product IfcProduct or IfcTypeProduct entity.
     * @param representation IfcShapeRepresentation to unassign.
     */
    unassignRepresentation(file: IfcFile, product: Entity, representation: Entity): void {
      const temps: Disposable[] = [];
      try {
        raw.geometry.unassignRepresentation(file.raw, product.raw, representation.raw);
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Validate and correct a representation's RepresentationType.
     *
     * When the representation contains multiple boolean operands, they are
     * consolidated into a single boolean chain via UNION. Returns true if the
     * final representation type is "CSG" or was successfully auto-detected.
     *
     * @param file IFC file containing the representation.
     * @param representation IfcShapeRepresentation to validate.
     * @param options Optional preferred operand for boolean consolidation.
     * @return True if the representation type is valid or was corrected.
     */
    validateType(file: IfcFile, representation: Entity, options: IfcOpenShellGeometryValidateTypeOptions): boolean {
      const temps: Disposable[] = [];
      try {
        const result = raw.geometry.validateType(file.raw, representation.raw, encodeOptions(options, {"preferredItem": "preferred_item"}, shell, temps));
        return wrap(shell, result) as boolean;
      } finally {
        disposeAll(temps);
      }
    },
    }),
    georeference: Object.freeze({
    /**
     * Add georeferencing entities to a model.
     *
     * Creates an IfcProjectedCRS and a coordinate operation (IfcMapConversion,
     * IfcMapConversionScaled, or IfcRigidOperation) linking the Model context
     * to the projected CRS. For IFC2X3, creates ePSet_MapConversion and
     * ePSet_ProjectedCRS property sets on the IfcProject instead. Does nothing
     * if georeferencing entities already exist.
     *
     * @param file File that receives the new entities.
     * @param options Coordinate operation class, CRS name, and ownership options.
     */
    addGeoreferencing(file: IfcFile, options: IfcOpenShellGeoreferenceAddGeoreferencingOptions): void {
      const temps: Disposable[] = [];
      try {
        raw.georeference.addGeoreferencing(file.raw, encodeOptions(options, {"application": "application", "ifcClass": "ifc_class", "name": "name", "ownerHistory": "owner_history", "user": "user"}, shell, temps));
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Edit map conversion and projected CRS attributes.
     *
     * For IFC4+, applies attribute changes from the property bags to the first
     * IfcCoordinateOperation and/or IfcProjectedCRS in the file. For IFC2X3,
     * applies changes to the ePSet_MapConversion and ePSet_ProjectedCRS property
     * sets on the IfcProject. Entities must already exist (call
     * georeference_add_georeferencing first if needed).
     *
     * @param file File containing the georeferencing entities.
     * @param options Property bags for coordinate operation and projected CRS.
     */
    editGeoreferencing(file: IfcFile, options: IfcOpenShellGeoreferenceEditGeoreferencingOptions): void {
      const temps: Disposable[] = [];
      try {
        raw.georeference.editGeoreferencing(file.raw, encodeOptions(options, {"coordinateOperation": "coordinate_operation", "projectedCrs": "projected_crs"}, shell, temps, ["coordinateOperation", "projectedCrs"]));
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Set or remove the true north direction on all geometric representation contexts.
     *
     * When omitted, any existing TrueNorth reference is removed from every
     * IfcGeometricRepresentationContext. When provided, the first two values are
     * used as (X, Y) direction ratios; missing values default to 0.0. The
     * direction is not normalized.
     *
     * @param file File whose contexts to update.
     * @param options True north direction ratios, or omission to remove true north.
     */
    editTrueNorth(file: IfcFile, options: IfcOpenShellGeoreferenceEditTrueNorthOptions): void {
      const temps: Disposable[] = [];
      try {
        raw.georeference.editTrueNorth(file.raw, encodeOptions(options, {"trueNorth": "true_north"}, shell, temps));
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Edit the world coordinate system (WCS) on all geometric representation contexts.
     *
     * Replaces the WorldCoordinateSystem placement on every
     * IfcGeometricRepresentationContext. The rotation parameter specifies the
     * X-axis rotation in degrees. When is_si is true (default), coordinates are
     * interpreted as SI metres and scaled to project length units. The old WCS
     * placement is deleted if unreferenced.
     *
     * @param file File whose contexts to update.
     * @param options WCS offset, rotation, and unit-system options.
     */
    editWcs(file: IfcFile, options: IfcOpenShellGeoreferenceEditWcsOptions): void {
      const temps: Disposable[] = [];
      try {
        raw.georeference.editWcs(file.raw, encodeOptions(options, {"isSi": "is_si", "rotation": "rotation", "x": "x", "y": "y", "z": "z"}, shell, temps));
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Remove all georeferencing entities from a model.
     *
     * For IFC4+, deletes all IfcProjectedCRS and IfcCoordinateOperation entities
     * (including orphaned MapUnit). For IFC2X3, removes the ePSet_MapConversion
     * and ePSet_ProjectedCRS property sets from the IfcProject.
     *
     * @param file File from which to remove georeferencing.
     */
    removeGeoreferencing(file: IfcFile): void {
      const temps: Disposable[] = [];
      try {
        raw.georeference.removeGeoreferencing(file.raw);
      } finally {
        disposeAll(temps);
      }
    },
    }),
    grid: Object.freeze({
    /**
     * Create or replace the AxisCurve of a grid axis as a 2D IfcPolyline.
     *
     * Points are given in world coordinates; when is_si is true they are divided
     * by the file's LENGTHUNIT scale. The points are transformed into the grid's
     * local coordinate system using the grid's ObjectPlacement. An existing
     * AxisCurve is removed after replacement.
     *
     * @param file IFC file that receives the polyline.
     * @param p1 First endpoint (at least three coordinates; X and Y are used).
     * @param p2 Second endpoint (at least three coordinates; X and Y are used).
     * @param grid_axis IfcGridAxis whose AxisCurve to set.
     * @param is_si True if p1/p2 are in SI metres; false if already in model units.
     */
    createAxisCurve(file: IfcFile, p1: number[], p2: number[], grid_axis: Entity, is_si: boolean): void {
      const temps: Disposable[] = [];
      try {
        raw.grid.createAxisCurve(file.raw, toRawSequence(p1, shell, temps), toRawSequence(p2, shell, temps), grid_axis.raw, is_si);
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Create an IfcGridAxis and append it to the specified grid axis aggregate.
     *
     * @param file IFC file that receives the new axis.
     * @param grid IfcGrid entity owning the axis.
     * @param axis_tag Label for the axis (e.g. "A", "1").
     * @param same_sense True if the axis direction agrees with the curve direction.
     * @param uvw_axes Name of the grid aggregate to append to: "UAxes", "VAxes", or "WAxes".
     * @return Newly created IfcGridAxis, or no result if creation fails.
     */
    createGridAxis(file: IfcFile, grid: Entity, axis_tag: string, same_sense: boolean, uvw_axes: string): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.grid.createGridAxis(file.raw, grid.raw, axis_tag, same_sense, uvw_axes);
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Remove an IfcGridAxis and its associated AxisCurve.
     *
     * The axis entity and its associated AxisCurve are removed from the file.
     *
     * @param file IFC file to modify.
     * @param axis IfcGridAxis entity to remove.
     */
    removeGridAxis(file: IfcFile, axis: Entity): void {
      const temps: Disposable[] = [];
      try {
        raw.grid.removeGridAxis(file.raw, axis.raw);
      } finally {
        disposeAll(temps);
      }
    },
    }),
    group: Object.freeze({
    /** Create a new IfcGroup with generated GlobalId and OwnerHistory. */
    addGroup(file: IfcFile, options: IfcOpenShellGroupAddGroupOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.group.addGroup(file.raw, encodeOptions(options, {"application": "application", "description": "description", "name": "name", "ownerHistory": "owner_history", "user": "user"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Assign products to a group via IfcRelAssignsToGroup.
     *
     * If an existing relationship is present, new products are merged into it.
     * Products already in the group are skipped.
     */
    assignGroup(file: IfcFile, options: IfcOpenShellGroupAssignGroupOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.group.assignGroup(file.raw, encodeOptions(options, {"application": "application", "group": "group", "ownerHistory": "owner_history", "products": "products", "user": "user"}, shell, temps, [], ["products"]));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Remove a group and its relationships.
     *
     * Removes associated IfcRelDefinesByProperties (property sets) and
     * IfcRelAssignsToGroup relationships, then deletes the group entity.
     */
    removeGroup(file: IfcFile, group: Entity): void {
      const temps: Disposable[] = [];
      try {
        raw.group.removeGroup(file.raw, group.raw);
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Remove products from an IfcRelAssignsToGroup relationship.
     *
     * If no products remain in the relationship after removal, the relationship
     * is deleted.
     */
    unassignGroup(file: IfcFile, options: IfcOpenShellGroupUnassignGroupOptions): void {
      const temps: Disposable[] = [];
      try {
        raw.group.unassignGroup(file.raw, encodeOptions(options, {"application": "application", "group": "group", "products": "products", "user": "user"}, shell, temps, [], ["products"]));
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Replace the product list of an existing IfcRelAssignsToGroup relationship.
     *
     * If no relationship exists, a new one is created. Child IfcGroup entities
     * from the existing relationship are preserved and merged with the new list.
     * Multiple existing relationships are consolidated into one.
     */
    updateGroupProducts(file: IfcFile, options: IfcOpenShellGroupUpdateGroupProductsOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.group.updateGroupProducts(file.raw, encodeOptions(options, {"application": "application", "group": "group", "ownerHistory": "owner_history", "products": "products", "user": "user"}, shell, temps, [], ["products"]));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    }),
    guid: Object.freeze({
    compress(uuid_hex: string): string {
      const temps: Disposable[] = [];
      try {
        const result = raw.guid.compress(uuid_hex);
        return wrap(shell, result) as string;
      } finally {
        disposeAll(temps);
      }
    },
    expand(guid: string): string {
      const temps: Disposable[] = [];
      try {
        const result = raw.guid.expand(guid);
        return wrap(shell, result) as string;
      } finally {
        disposeAll(temps);
      }
    },
    generate(): string {
      const temps: Disposable[] = [];
      try {
        const result = raw.guid.generate();
        return wrap(shell, result) as string;
      } finally {
        disposeAll(temps);
      }
    },
    }),
    layer: Object.freeze({
    /**
     * Create an IfcPresentationLayerAssignment with the given name.
     *
     * @param file IFC file that receives the layer.
     * @param name Layer name.
     * @return Newly created IfcPresentationLayerAssignment.
     */
    addLayer(file: IfcFile, name: string): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.layer.addLayer(file.raw, name);
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Create an IfcPresentationLayerWithStyle with visibility flags and styles.
     *
     * @param file IFC file that receives the layer.
     * @param name Layer name.
     * @param options Visibility flags and styles.
     * @return Newly created IfcPresentationLayerWithStyle.
     */
    addLayerWithStyle(file: IfcFile, name: string, options: IfcOpenShellLayerAddLayerWithStyleOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.layer.addLayerWithStyle(file.raw, name, encodeOptions(options, {"blocked": "blocked", "frozen": "frozen", "on": "on", "styles": "styles"}, shell, temps, [], ["styles"]));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Assign representation items to a presentation layer.
     *
     * Items already present in the layer's AssignedItems are not duplicated.
     *
     * @param file IFC file (unused, reserved for consistency).
     * @param items Representation items to assign.
     * @param layer IfcPresentationLayerAssignment or IfcPresentationLayerWithStyle.
     */
    assignLayer(file: IfcFile, items: Entity[], layer: Entity): void {
      const temps: Disposable[] = [];
      try {
        raw.layer.assignLayer(file.raw, toRawSequence(items, shell, temps), layer.raw);
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Remove a presentation layer entity from the file.
     *
     * @param file IFC file to modify.
     * @param layer IfcPresentationLayerAssignment or IfcPresentationLayerWithStyle to remove.
     */
    removeLayer(file: IfcFile, layer: Entity): void {
      const temps: Disposable[] = [];
      try {
        raw.layer.removeLayer(file.raw, layer.raw);
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Remove representation items from a presentation layer.
     *
     * If all assigned items are removed, the layer entity itself is deleted from
     * the file. Does nothing silently when any specified item is not currently
     * assigned to the layer.
     *
     * @param file IFC file to modify.
     * @param items Representation items to unassign.
     * @param layer IfcPresentationLayerAssignment or IfcPresentationLayerWithStyle.
     */
    unassignLayer(file: IfcFile, items: Entity[], layer: Entity): void {
      const temps: Disposable[] = [];
      try {
        raw.layer.unassignLayer(file.raw, toRawSequence(items, shell, temps), layer.raw);
      } finally {
        disposeAll(temps);
      }
    },
    }),
    library: Object.freeze({
    /** Create a new IfcLibraryInformation with the given name. */
    addLibrary(file: IfcFile, name: string): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.library.addLibrary(file.raw, name);
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Create a new IfcLibraryReference linked to a library.
     *
     * In IFC4+, the reference's ReferencedLibrary attribute is set. In IFC2X3,
     * the reference is appended to the library's LibraryReference aggregate.
     */
    addReference(file: IfcFile, library: Entity): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.library.addReference(file.raw, library.raw);
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Assign a library reference to products via IfcRelAssociatesLibrary.
     *
     * Products already associated with the reference are skipped. If an
     * existing relationship is present, new products are merged into it.
     */
    assignReference(file: IfcFile, options: IfcOpenShellLibraryAssignReferenceOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.library.assignReference(file.raw, encodeOptions(options, {"application": "application", "ownerHistory": "owner_history", "products": "products", "reference": "reference", "user": "user"}, shell, temps, [], ["products"]));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Remove an IfcLibraryInformation and all its references.
     *
     * Deletes all child IfcLibraryReference entities, the library entity itself,
     * and all associated IfcRelAssociatesLibrary relationships.
     */
    removeLibrary(file: IfcFile, library: Entity): void {
      const temps: Disposable[] = [];
      try {
        raw.library.removeLibrary(file.raw, library.raw);
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Remove an IfcLibraryReference and its association relationships.
     *
     * Deletes the reference entity and all IfcRelAssociatesLibrary relationships
     * that reference it.
     */
    removeReference(file: IfcFile, reference: Entity): void {
      const temps: Disposable[] = [];
      try {
        raw.library.removeReference(file.raw, reference.raw);
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Remove products from an IfcRelAssociatesLibrary relationship.
     *
     * If no products remain after removal, the relationship is deleted.
     */
    unassignReference(file: IfcFile, options: IfcOpenShellLibraryUnassignReferenceOptions): void {
      const temps: Disposable[] = [];
      try {
        raw.library.unassignReference(file.raw, encodeOptions(options, {"application": "application", "products": "products", "reference": "reference", "user": "user"}, shell, temps, [], ["products"]));
      } finally {
        disposeAll(temps);
      }
    },
    }),
    material: Object.freeze({
    /** Add an IfcMaterialConstituent to an IfcMaterialConstituentSet. */
    addConstituent(file: IfcFile, constituent_set: Entity, options: IfcOpenShellMaterialAddConstituentOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.material.addConstituent(file.raw, constituent_set.raw, encodeOptions(options, {"material": "material", "name": "name"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Add an IfcMaterialLayer to an IfcMaterialLayerSet.
     *
     * The layer thickness is initialized to 0.1 metres, converted to the file's
     * project units (e.g. 100 for a millimetre project).
     */
    addLayer(file: IfcFile, layer_set: Entity, options: IfcOpenShellMaterialAddLayerOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.material.addLayer(file.raw, layer_set.raw, encodeOptions(options, {"material": "material", "name": "name"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /** Append an IfcMaterial to an IfcMaterialList. */
    addListItem(file: IfcFile, material_list: Entity, material: Entity): void {
      const temps: Disposable[] = [];
      try {
        raw.material.addListItem(file.raw, material_list.raw, material.raw);
      } finally {
        disposeAll(temps);
      }
    },
    /** Create a new IfcMaterial. */
    addMaterial(file: IfcFile, options: IfcOpenShellMaterialAddMaterialOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.material.addMaterial(file.raw, encodeOptions(options, {"category": "category", "description": "description", "name": "name"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Create a new material set of the requested type.
     *
     * Defaults to IfcMaterialConstituentSet if set_type is omitted.
     */
    addMaterialSet(file: IfcFile, options: IfcOpenShellMaterialAddMaterialSetOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.material.addMaterialSet(file.raw, encodeOptions(options, {"name": "name", "setType": "set_type"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /** Add an IfcMaterialProfile to an IfcMaterialProfileSet. */
    addProfile(file: IfcFile, profile_set: Entity, options: IfcOpenShellMaterialAddProfileOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.material.addProfile(file.raw, profile_set.raw, encodeOptions(options, {"material": "material", "name": "name", "profile": "profile"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Assign a material to products via IfcRelAssociatesMaterial.
     *
     * Any existing material assignment on the products is removed first.
     * For "IfcMaterialLayerSetUsage" and "IfcMaterialProfileSetUsage", the
     * products are grouped by material set and direction; each group gets its
     * own usage entity and relationship. Returns the created relationship(s).
     */
    assignMaterial(file: IfcFile, products: Entity[], options: IfcOpenShellMaterialAssignMaterialOptions): Entity[] {
      const temps: Disposable[] = [];
      try {
        const result = raw.material.assignMaterial(file.raw, toRawSequence(products, shell, temps), encodeOptions(options, {"application": "application", "material": "material", "ownerHistory": "owner_history", "type": "type", "user": "user"}, shell, temps));
        return wrapEntities(shell, result as never) as Entity[];
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Replace the profile on an IfcMaterialProfile and update representations.
     *
     * Sets the Profile attribute on the material profile. Updates the
     * SweptArea of any IfcSweptAreaSolid in representations of elements
     * using the parent IfcMaterialProfileSetUsage. The old profile is
     * deleted if it has no remaining references.
     */
    assignProfile(file: IfcFile, material_profile: Entity, profile: Entity): void {
      const temps: Disposable[] = [];
      try {
        raw.material.assignProfile(file.raw, material_profile.raw, profile.raw);
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Edit attributes of an IfcMaterialProfileSetUsage.
     *
     * Applies attribute key-value pairs from the props builder. If CardinalPoint
     * changes and both profile_width and profile_height are provided, the
     * ObjectPlacement of associated elements is updated to reflect the new
     * cardinal point position.
     */
    editProfileUsage(file: IfcFile, usage: Entity, options: IfcOpenShellMaterialEditProfileUsageOptions): void {
      const temps: Disposable[] = [];
      try {
        raw.material.editProfileUsage(file.raw, usage.raw, encodeOptions(options, {"attributes": "attributes", "profileHeight": "profile_height", "profileWidth": "profile_width"}, shell, temps, ["attributes"]));
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Remove an IfcMaterialConstituent from its set.
     *
     * If should_remove_material is true, the associated IfcMaterial is also
     * deleted.
     */
    removeConstituent(file: IfcFile, constituent: Entity, options: IfcOpenShellMaterialRemoveItemOptions): void {
      const temps: Disposable[] = [];
      try {
        raw.material.removeConstituent(file.raw, constituent.raw, encodeOptions(options, {"shouldRemoveMaterial": "should_remove_material"}, shell, temps));
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Remove an IfcMaterialLayer from its set.
     *
     * If should_remove_material is true, the associated IfcMaterial is also
     * deleted.
     */
    removeLayer(file: IfcFile, layer: Entity, options: IfcOpenShellMaterialRemoveItemOptions): void {
      const temps: Disposable[] = [];
      try {
        raw.material.removeLayer(file.raw, layer.raw, encodeOptions(options, {"shouldRemoveMaterial": "should_remove_material"}, shell, temps));
      } finally {
        disposeAll(temps);
      }
    },
    /** Remove an item from an IfcMaterialList by index. */
    removeListItem(file: IfcFile, material_list: Entity, options: IfcOpenShellMaterialRemoveListItemOptions): void {
      const temps: Disposable[] = [];
      try {
        raw.material.removeListItem(file.raw, material_list.raw, encodeOptions(options, {"materialIndex": "material_index"}, shell, temps));
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Remove an IfcMaterial and its associated constituents, layers, and profiles.
     *
     * Deletes the material entity. Constituent, layer, or profile entities
     * that reference it are also removed. Associated IfcRelAssociatesMaterial,
     * IfcMaterialProperties, and IfcMaterialDefinitionRepresentation entities
     * are cleaned up.
     */
    removeMaterial(file: IfcFile, material: Entity): void {
      const temps: Disposable[] = [];
      try {
        raw.material.removeMaterial(file.raw, material.raw);
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Remove a material set and its items.
     *
     * Deletes all items (layers, profiles, constituents) in the set, then
     * removes the set entity. For IfcMaterialLayerSet and IfcMaterialProfileSet,
     * associated usage entities and their relationships are also deleted.
     * Associated IfcRelAssociatesMaterial and IfcMaterialProperties are
     * cleaned up.
     */
    removeMaterialSet(file: IfcFile, material: Entity): void {
      const temps: Disposable[] = [];
      try {
        raw.material.removeMaterialSet(file.raw, material.raw);
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Remove an IfcMaterialProfile from its set.
     *
     * Optionally deletes the associated IfcMaterial and/or IfcProfileDef
     * if the corresponding flags are set.
     */
    removeProfile(file: IfcFile, profile: Entity, options: IfcOpenShellMaterialRemoveProfileOptions): void {
      const temps: Disposable[] = [];
      try {
        raw.material.removeProfile(file.raw, profile.raw, encodeOptions(options, {"shouldRemoveMaterial": "should_remove_material", "shouldRemoveProfileDef": "should_remove_profile_def"}, shell, temps));
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Reorder an item within a material set.
     *
     * Moves the item from old_index to new_index within the set's aggregate
     * (MaterialConstituents, MaterialLayers, MaterialProfiles, or Materials).
     * Supports IfcMaterialConstituentSet, IfcMaterialLayerSet,
     * IfcMaterialProfileSet, and IfcMaterialList.
     */
    reorderSetItem(file: IfcFile, material_set: Entity, options: IfcOpenShellMaterialReorderSetItemOptions): void {
      const temps: Disposable[] = [];
      try {
        raw.material.reorderSetItem(file.raw, material_set.raw, encodeOptions(options, {"newIndex": "new_index", "oldIndex": "old_index"}, shell, temps));
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Remove material assignments from products.
     *
     * Deletes IfcRelAssociatesMaterial relationships for the products. If the
     * material is an IfcMaterialLayerSetUsage or IfcMaterialProfileSetUsage
     * with no remaining products, the usage entity is also deleted.
     */
    unassignMaterial(file: IfcFile, products: Entity[], options: IfcOpenShellMaterialUnassignMaterialOptions): void {
      const temps: Disposable[] = [];
      try {
        raw.material.unassignMaterial(file.raw, toRawSequence(products, shell, temps), encodeOptions(options, {"application": "application", "user": "user"}, shell, temps));
      } finally {
        disposeAll(temps);
      }
    },
    }),
    nest: Object.freeze({
    /**
     * Assign objects as nested children of a parent host via IfcRelNests.
     *
     * Objects previously nested under a different parent are moved. Objects
     * previously contained in a spatial structure or aggregated under a
     * different parent are removed from those relationships. If an existing
     * IfcRelNests relationship already relates to the same parent, objects
     * are merged into it while preserving insertion order.
     */
    assignObject(file: IfcFile, options: IfcOpenShellNestAssignObjectOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.nest.assignObject(file.raw, encodeOptions(options, {"application": "application", "ownerHistory": "owner_history", "products": "products", "relatingObject": "relating_object", "user": "user"}, shell, temps, [], ["products"]));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Remove objects from their IfcRelNests relationships.
     *
     * If no related objects remain after removal, the IfcRelNests
     * relationship itself is deleted.
     */
    unassignObject(file: IfcFile, options: IfcOpenShellNestUnassignObjectOptions): void {
      const temps: Disposable[] = [];
      try {
        raw.nest.unassignObject(file.raw, encodeOptions(options, {"application": "application", "products": "products", "user": "user"}, shell, temps, [], ["products"]));
      } finally {
        disposeAll(temps);
      }
    },
    }),
    owner: Object.freeze({
    /**
     * Create an IfcActor (or subclass) entity.
     *
     * @param file File that receives the new entity.
     * @param options Actor identity and class options.
     * @return Newly created IfcActor or subclass.
     */
    addActor(file: IfcFile, options: IfcOpenShellOwnerAddActorOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.owner.addActor(file.raw, encodeOptions(options, {"actor": "actor", "application": "application", "ifcClass": "ifc_class", "ownerHistory": "owner_history", "user": "user"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Add an IfcAddress subclass to an IfcOrganization or IfcPerson.
     *
     * Creates an entity of the given IFC class (e.g. "IfcTelecomAddress",
     * "IfcPostalAddress") with Purpose set to OFFICE and appends it to the
     * assigned object's Addresses aggregate.
     *
     * @param file File that receives the new address.
     * @param assigned_object IfcOrganization or IfcPerson to receive the address.
     * @param ifc_class IFC class name of the address to create.
     * @return Newly created IfcAddress subclass.
     */
    addAddress(file: IfcFile, assigned_object: Entity, ifc_class: string): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.owner.addAddress(file.raw, assigned_object.raw, ifc_class);
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Create an IfcApplication entity.
     *
     * If application_developer is omitted, a default IfcOpenShell organisation
     * with a web-page address is created automatically.
     *
     * @param file File that receives the new entity.
     * @param options Application identity options.
     * @return Newly created IfcApplication.
     */
    addApplication(file: IfcFile, options: IfcOpenShellOwnerAddApplicationOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.owner.addApplication(file.raw, encodeOptions(options, {"application": "application", "applicationDeveloper": "application_developer", "applicationFullName": "application_full_name", "applicationIdentifier": "application_identifier", "ownerHistory": "owner_history", "user": "user", "version": "version"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Create an IfcOrganization entity.
     *
     * Uses the Identification attribute on IFC4+ and Id on IFC2X3.
     *
     * @param file File that receives the new entity.
     * @param identification Organisation identifier.
     * @param name Organisation name.
     * @return Newly created IfcOrganization.
     */
    addOrganisation(file: IfcFile, identification: string, name: string): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.owner.addOrganisation(file.raw, identification, name);
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Create an IfcPerson entity.
     *
     * Uses the Identification attribute on IFC4+ and Id on IFC2X3.
     *
     * @param file File that receives the new entity.
     * @param identification Person identifier.
     * @param family_name Family (sur)name.
     * @param given_name Given (first) name.
     * @return Newly created IfcPerson.
     */
    addPerson(file: IfcFile, identification: string, family_name: string, given_name: string): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.owner.addPerson(file.raw, identification, family_name, given_name);
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Create an IfcPersonAndOrganization entity linking a person and an organisation.
     *
     * @param file File that receives the new entity.
     * @param person IfcPerson to link.
     * @param organisation IfcOrganization to link.
     * @return Newly created IfcPersonAndOrganization.
     */
    addPersonAndOrganisation(file: IfcFile, person: Entity, organisation: Entity): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.owner.addPersonAndOrganisation(file.raw, person.raw, organisation.raw);
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Add an IfcActorRole to an IfcOrganization, IfcPerson, or IfcPersonAndOrganization.
     *
     * If the role string matches a valid IFC enum value (e.g. "ARCHITECT"), it is
     * set directly. Otherwise, Role is set to USERDEFINED and UserDefinedRole
     * receives the string. The role is appended to the assigned object's Roles aggregate.
     *
     * @param file File that receives the new role.
     * @param assigned_object Entity to receive the role.
     * @param role Role name or user-defined string.
     * @return Newly created IfcActorRole.
     */
    addRole(file: IfcFile, assigned_object: Entity, role: string): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.owner.addRole(file.raw, assigned_object.raw, role);
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Assign an actor to an object via IfcRelAssignsToActor.
     *
     * If an IfcRelAssignsToActor already exists for this actor and object, the
     * existing relationship is returned. If the actor already has a relationship,
     * the object is appended to its RelatedObjects. Otherwise a new relationship
     * is created.
     *
     * @param file File that receives the relationship.
     * @param options Actor, object, and ownership options.
     * @return The IfcRelAssignsToActor relationship.
     */
    assignActor(file: IfcFile, options: IfcOpenShellOwnerAssignActorOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.owner.assignActor(file.raw, encodeOptions(options, {"application": "application", "ownerHistory": "owner_history", "relatedObject": "related_object", "relatingActor": "relating_actor", "user": "user"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Create a new IfcOwnerHistory.
     *
     * Sets CreationDate and LastModifiedDate to the current time, State to
     * READWRITE, and ChangeAction to ADDED. Both user and application are
     * required; if either is omitted, no owner history is created.
     *
     * @param file File that receives the new entity.
     * @param options User and application for the owner history.
     * @return Newly created IfcOwnerHistory, or no result if creation fails.
     */
    createOwnerHistory(file: IfcFile, options: IfcOpenShellOwnerCreateOwnerHistoryOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.owner.createOwnerHistory(file.raw, encodeOptions(options, {"application": "application", "user": "user"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Remove an IfcActor (or subclass) from the file.
     *
     * Removes the entity and its owner history.
     *
     * @param file File containing the actor.
     * @param actor IfcActor entity to remove.
     */
    removeActor(file: IfcFile, actor: Entity): void {
      const temps: Disposable[] = [];
      try {
        raw.owner.removeActor(file.raw, actor.raw);
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Remove an IfcAddress subclass from the file.
     *
     * If the address is the only entry in its parent's Addresses aggregate,
     * that aggregate is cleared before the entity is removed.
     *
     * @param file File containing the address.
     * @param address IfcAddress entity to remove.
     */
    removeAddress(file: IfcFile, address: Entity): void {
      const temps: Disposable[] = [];
      try {
        raw.owner.removeAddress(file.raw, address.raw);
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Remove an IfcApplication from the file.
     *
     * @param file File containing the application.
     * @param application IfcApplication entity to remove.
     */
    removeApplication(file: IfcFile, application: Entity): void {
      const temps: Disposable[] = [];
      try {
        raw.owner.removeApplication(file.raw, application.raw);
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Remove an IfcOrganization from the file.
     *
     * Cascades removal to roles, addresses, IfcPersonAndOrganization entities,
     * IfcActor entities, IfcOrganizationRelationship entities, and IfcApplication
     * entities that reference this organisation as their sole member.
     *
     * @param file File containing the organisation.
     * @param organisation IfcOrganization entity to remove.
     */
    removeOrganisation(file: IfcFile, organisation: Entity): void {
      const temps: Disposable[] = [];
      try {
        raw.owner.removeOrganisation(file.raw, organisation.raw);
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Remove an IfcPerson from the file.
     *
     * Cascades removal to roles, addresses, IfcPersonAndOrganization entities,
     * IfcActor entities, and IfcResourceLevelRelationship entities that reference
     * this person as their sole member.
     *
     * @param file File containing the person.
     * @param person IfcPerson entity to remove.
     */
    removePerson(file: IfcFile, person: Entity): void {
      const temps: Disposable[] = [];
      try {
        raw.owner.removePerson(file.raw, person.raw);
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Remove an IfcPersonAndOrganization from the file.
     *
     * Cascades removal to IfcOwnerHistory, IfcActor, and
     * IfcResourceLevelRelationship entities that reference this entity as their
     * sole member.
     *
     * @param file File containing the person-and-organisation entity.
     * @param person_and_organisation IfcPersonAndOrganization entity to remove.
     */
    removePersonAndOrganisation(file: IfcFile, person_and_organisation: Entity): void {
      const temps: Disposable[] = [];
      try {
        raw.owner.removePersonAndOrganisation(file.raw, person_and_organisation.raw);
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Remove an IfcActorRole from the file.
     *
     * If the role is the sole entry in its parent's Roles aggregate, that
     * aggregate is cleared before the entity is removed.
     *
     * @param file File containing the role.
     * @param role IfcActorRole entity to remove.
     */
    removeRole(file: IfcFile, role: Entity): void {
      const temps: Disposable[] = [];
      try {
        raw.owner.removeRole(file.raw, role.raw);
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Remove an object assignment from an actor via IfcRelAssignsToActor.
     *
     * If the relationship has only one RelatedObjects entry, the entire
     * relationship is removed. Otherwise the object is removed from the
     * RelatedObjects aggregate and the OwnerHistory is updated.
     *
     * @param file File containing the relationship.
     * @param options Actor and object to unassign.
     */
    unassignActor(file: IfcFile, options: IfcOpenShellOwnerUnassignActorOptions): void {
      const temps: Disposable[] = [];
      try {
        raw.owner.unassignActor(file.raw, encodeOptions(options, {"application": "application", "relatedObject": "related_object", "relatingActor": "relating_actor", "user": "user"}, shell, temps));
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Update or create the OwnerHistory on an IfcRoot element.
     *
     * If the element already has an IfcOwnerHistory, its LastModifiedDate,
     * LastModifyingUser, and LastModifyingApplication are updated. If not,
     * a new IfcOwnerHistory is created and assigned. Requires both user and
     * application to be provided.
     *
     * @param file File containing the element.
     * @param options Element, user, and application.
     * @return The updated or newly created IfcOwnerHistory, or no result if the element is not an IfcRoot.
     */
    updateOwnerHistory(file: IfcFile, options: IfcOpenShellOwnerUpdateOwnerHistoryOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.owner.updateOwnerHistory(file.raw, encodeOptions(options, {"application": "application", "element": "element", "user": "user"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    }),
    placement: Object.freeze({
    /**
     * Extract a 4x4 row-major matrix from an IfcAxis2Placement entity.
     *
     * Supports IfcAxis2Placement2D, IfcAxis2Placement3D, and IfcAxis1Placement.
     * Returns an identity matrix if no instance is provided or the instance is unsupported.
     *
     * @param instance IfcAxis2Placement entity.
     * @return 16-element row-major 4x4 matrix.
     */
    getAxis2Placement(instance: Entity): number[] {
      const temps: Disposable[] = [];
      try {
        const result = raw.placement.getAxis2Placement(instance.raw);
        return wrap(shell, result) as number[];
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Extract a 4x4 row-major matrix from an IfcCartesianTransformationOperator3D.
     *
     * Handles uniform and non-uniform scaling. Returns an identity matrix if no
     * instance is provided or the instance is not a cartesian transformation operator.
     *
     * @param instance IfcCartesianTransformationOperator3D entity.
     * @return 16-element row-major 4x4 matrix.
     */
    getCartesianXform3d(instance: Entity): number[] {
      const temps: Disposable[] = [];
      try {
        const result = raw.placement.getCartesianXform3d(instance.raw);
        return wrap(shell, result) as number[];
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Compute the cumulative 4x4 row-major world matrix of an IfcLocalPlacement.
     *
     * Combines the placement with its parent placements to compute the full
     * transformation. Returns an identity matrix when the placement is omitted.
     *
     * @param instance IfcLocalPlacement entity. When omitted, returns the identity matrix.
     * @return 16-element row-major 4x4 matrix.
     */
    getLocalPlacement(instance: Entity): number[] {
      const temps: Disposable[] = [];
      try {
        const result = raw.placement.getLocalPlacement(instance.raw);
        return wrap(shell, result) as number[];
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Compute the combined 4x4 row-major matrix for an IfcMappedItem.
     *
     * Multiplies the MappingTarget transformation by the MappingOrigin placement.
     * Returns an identity matrix if no instance is provided or the instance is
     * not an IfcMappedItem.
     *
     * @param instance IfcMappedItem entity.
     * @return 16-element row-major 4x4 matrix.
     */
    getMappeditemXform(instance: Entity): number[] {
      const temps: Disposable[] = [];
      try {
        const result = raw.placement.getMappeditemXform(instance.raw);
        return wrap(shell, result) as number[];
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Return the elevation of a building storey in model units.
     *
     * Uses the Z-translation of the storey's ObjectPlacement when available,
     * falling back to the Elevation attribute. Returns 0.0 if no instance is
     * provided or the instance has no placement.
     *
     * @param instance IfcBuildingStorey entity.
     * @return Elevation in model units.
     */
    getStoreyElevation(instance: Entity): number {
      const temps: Disposable[] = [];
      try {
        const result = raw.placement.getStoreyElevation(instance.raw);
        return wrap(shell, result) as number;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Compute a 4x4 row-major transformation matrix from origin and axes.
     *
     * @param origin XYZ origin of the placement.
     * @param z_axis Direction ratios for the Z axis.
     * @param x_axis Direction ratios for the X axis (Y is derived).
     * @return 16-element row-major 4x4 matrix.
     */
    matrixFromAxes(origin: number[], z_axis: number[], x_axis: number[]): number[] {
      const temps: Disposable[] = [];
      try {
        const result = raw.placement.matrixFromAxes(toRawSequence(origin, shell, temps), toRawSequence(z_axis, shell, temps), toRawSequence(x_axis, shell, temps));
        return wrap(shell, result) as number[];
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Build a 4x4 row-major rotation matrix about a principal axis.
     *
     * @param angle_rad Rotation angle in radians.
     * @param axis Rotation axis: "X", "Y", or "Z" (case-insensitive).
     * @return 16-element row-major 4x4 rotation matrix.
     */
    rotation(angle_rad: number, axis: string): number[] {
      const temps: Disposable[] = [];
      try {
        const result = raw.placement.rotation(angle_rad, axis);
        return wrap(shell, result) as number[];
      } finally {
        disposeAll(temps);
      }
    },
    }),
    profile: Object.freeze({
    /**
     * Create an IfcArbitraryClosedProfileDef from an ordered polyline.
     *
     * Points are converted from SI metres to project units before creating the
     * outer curve. The curve representation is chosen automatically (IfcPolyline
     * for IFC2X3, IfcIndexedPolyCurve otherwise).
     *
     * @param file IFC file that receives the profile.
     * @param options Outer curve points and optional name.
     * @return Newly created IfcArbitraryClosedProfileDef.
     */
    addArbitraryProfile(file: IfcFile, options: IfcOpenShellProfileAddArbitraryProfileOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.profile.addArbitraryProfile(file.raw, encodeOptions(options, {"name": "name", "profile": "profile"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Create an IfcArbitraryProfileDefWithVoids from an outer curve and inner voids.
     *
     * Points are converted from SI metres to project units. The outer curve uses
     * IfcCartesianPointList3D for IFC4+ schemas and IfcPolyline for IFC2X3.
     *
     * @param file IFC file that receives the profile.
     * @param options Outer curve, inner void curves, and optional name.
     * @return Newly created IfcArbitraryProfileDefWithVoids.
     */
    addArbitraryProfileWithVoids(file: IfcFile, options: IfcOpenShellProfileAddArbitraryProfileWithVoidsOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.profile.addArbitraryProfileWithVoids(file.raw, encodeOptions(options, {"innerProfiles": "inner_profiles", "name": "name", "outerProfile": "outer_profile"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Create a parameterized IFC profile definition.
     *
     * @param file IFC file that receives the profile.
     * @param ifc_class IFC entity class name (e.g. "IfcRectangleProfileDef").
     * @param profile_type Profile type string, typically "AREA".
     * @return Newly created IfcProfileDef entity.
     */
    addParameterizedProfile(file: IfcFile, ifc_class: string, profile_type: string): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.profile.addParameterizedProfile(file.raw, ifc_class, profile_type);
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Copy a profile and its associated IfcProfileProperties.
     *
     * @param file IFC file that receives the copied profile.
     * @param profile IfcProfileDef entity to copy.
     * @return Newly created independent copy of the profile.
     */
    copyProfile(file: IfcFile, profile: Entity): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.profile.copyProfile(file.raw, profile.raw);
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Edit attributes of an existing profile definition.
     *
     * @param profile IfcProfileDef entity to modify.
     * @param attributes Attribute name-to-value mapping.
     */
    editProfile(profile: Entity, attributes: PsetProperties | PsetInput): void {
      const temps: Disposable[] = [];
      try {
        raw.profile.editProfile(profile.raw, toRawPsetProperties(shell, attributes as PsetProperties | PsetInput, temps));
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Remove a profile definition and its directly referenced sub-entities.
     *
     * Removes associated IfcProfileProperties first, then removes the profile
     * entity and removes unreferenced entities belonging to its direct geometry,
     * such as curves and placements.
     *
     * @param file IFC file to modify.
     * @param profile IfcProfileDef entity to remove.
     */
    removeProfile(file: IfcFile, profile: Entity): void {
      const temps: Disposable[] = [];
      try {
        raw.profile.removeProfile(file.raw, profile.raw);
      } finally {
        disposeAll(temps);
      }
    },
    }),
    project: Object.freeze({
    /**
     * Declare objects to a project or project library context.
     *
     * Creates or updates an IfcRelDeclares relationship linking the given
     * definitions to the relating context. If the definitions are already
     * declared to a different context, they are moved.
     */
    assignDeclaration(file: IfcFile, options: IfcOpenShellProjectAssignDeclarationOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.project.assignDeclaration(file.raw, encodeOptions(options, {"application": "application", "definitions": "definitions", "ownerHistory": "owner_history", "relatingContext": "relating_context", "user": "user"}, shell, temps, [], ["definitions"]));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Remove declarations of objects from a project or project library context.
     *
     * Removes the definitions from any existing IfcRelDeclares relationship.
     * If no definitions remain, the relationship itself is removed.
     */
    unassignDeclaration(file: IfcFile, options: IfcOpenShellProjectUnassignDeclarationOptions): void {
      const temps: Disposable[] = [];
      try {
        raw.project.unassignDeclaration(file.raw, encodeOptions(options, {"application": "application", "definitions": "definitions", "relatingContext": "relating_context", "user": "user"}, shell, temps, [], ["definitions"]));
      } finally {
        disposeAll(temps);
      }
    },
    }),
    pset: Object.freeze({
    /**
     * Add or return an existing property set attached to an object, context, type,
     * material, or profile definition.
     *
     * For IfcObject/IfcContext, creates IfcRelDefinesByProperties. For
     * IfcTypeObject, appends to HasPropertySets. For IfcMaterial, creates
     * IfcMaterialProperties (or IfcExtendedMaterialProperties in IFC2X3).
     * For IfcProfileDef, creates IfcProfileProperties (or
     * IfcGeneralProfileProperties in IFC2X3). If a property set with the
     * same name already exists on the product, it is returned without
     * creating a new one.
     */
    addPset(file: IfcFile, options: IfcOpenShellPsetAddPsetOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.pset.addPset(file.raw, encodeOptions(options, {"application": "application", "ifc2x3Subclass": "ifc2x3_subclass", "name": "name", "ownerHistory": "owner_history", "product": "product", "user": "user"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Add or return an existing quantity set (IfcElementQuantity) on an object.
     *
     * If the name ends with "BaseQuantities", MethodOfMeasurement is set to
     * "BaseQuantities". If a quantity set with the same name already exists,
     * it is returned without creating a new one.
     */
    addQto(file: IfcFile, options: IfcOpenShellPsetAddQtoOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.pset.addQto(file.raw, encodeOptions(options, {"application": "application", "name": "name", "ownerHistory": "owner_history", "product": "product", "user": "user"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Assign a property set to products.
     *
     * For IfcTypeProduct instances, the pset is appended to HasPropertySets.
     * For other products, creates or updates an IfcRelDefinesByProperties
     * relationship.
     */
    assignPset(file: IfcFile, options: IfcOpenShellPsetAssignPsetOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.pset.assignPset(file.raw, encodeOptions(options, {"application": "application", "ownerHistory": "owner_history", "products": "products", "pset": "pset", "user": "user"}, shell, temps, [], ["products"]));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Edit properties on an existing property set.
     *
     * Updates existing properties in-place (when not shared with other psets),
     * adds new properties for keys not yet present, and optionally removes
     * blank-valued properties. Uses the pset template for type inference when
     * available. Returns true on success, false on error.
     */
    editPset(file: IfcFile, options: IfcOpenShellPsetEditPsetOptions): boolean {
      const temps: Disposable[] = [];
      try {
        const result = raw.pset.editPset(file.raw, encodeOptions(options, {"name": "name", "properties": "properties", "pset": "pset", "psetTemplate": "pset_template", "shouldPurge": "should_purge"}, shell, temps, ["properties"]));
        return wrap(shell, result) as boolean;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Edit quantities on an existing quantity set.
     *
     * Updates existing quantities in-place, adds new quantities for keys not
     * yet present. Quantity types (Length, Area, Volume, Count, Weight, Time)
     * are inferred from the value type and name. Returns true on success,
     * false on error.
     */
    editQto(file: IfcFile, options: IfcOpenShellPsetEditQtoOptions): boolean {
      const temps: Disposable[] = [];
      try {
        const result = raw.pset.editQto(file.raw, encodeOptions(options, {"name": "name", "properties": "properties", "qto": "qto", "qtoTemplate": "qto_template"}, shell, temps, ["properties"]));
        return wrap(shell, result) as boolean;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Remove a property set from a specific product.
     *
     * Removes the IfcRelDefinesByProperties relationship between the product
     * and pset. If the pset has no remaining associations, the pset and its
     * properties are deleted.
     */
    removePset(file: IfcFile, product: Entity, pset: Entity): void {
      const temps: Disposable[] = [];
      try {
        raw.pset.removePset(file.raw, product.raw, pset.raw);
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Create a new IfcSimplePropertyTemplate and add it to a property set template.
     *
     * Property templates are kept sorted by name within the parent template.
     * If template_type is omitted, defaults to "P_SINGLEVALUE" for psets or
     * "Q_LENGTH" for qtos. If primary_measure_type is omitted, defaults to
     * "IfcLabel" for psets.
     */
    templateAddPropTemplate(file: IfcFile, pset_template: Entity, name: string, description: string, template_type: string, primary_measure_type: string): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.pset.templateAddPropTemplate(file.raw, pset_template.raw, name, description, template_type, primary_measure_type);
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Create a new IfcPropertySetTemplate in the file.
     *
     * @param file The IFC file.
     * @param name Template name (e.g. "Pset_WallCommon").
     * @param template_type Template type string (e.g. "PSET_TYPEDRIVENOVERRIDE", "QTO").
     * @param applicable_entity Applicable IFC entity name (e.g. "IfcWall").
     */
    templateAddPsetTemplate(file: IfcFile, name: string, template_type: string, applicable_entity: string): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.pset.templateAddPsetTemplate(file.raw, name, template_type, applicable_entity);
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Create a property template collection from custom IFC template files.
     *
     * Loads IfcPropertySetTemplate and IfcSimplePropertyTemplate entities from
     * the provided files. The returned collection remains valid until it is
     * released with pset_template_free.
     */
    templateCreateFromFiles(schema_identifier: string, template_files: IfcFile[]): PsetTemplate | null {
      const temps: Disposable[] = [];
      try {
        const result = raw.pset.templateCreateFromFiles(schema_identifier, toRawSequence(template_files, shell, temps));
        return result as PsetTemplate | null;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Return property set templates applicable to an IFC class and predefined type.
     *
     * Filters by pset_only (PSET templates) or qto_only (QTO templates).
     * If neither flag is set, returns both types. When predefined_type or
     * schema_name is omitted, the default is used.
     */
    templateGetApplicable(pqt: PsetTemplate, ifc_class: string, predefined_type: string, pset_only: boolean, qto_only: boolean, schema_name: string): Entity[] {
      const temps: Disposable[] = [];
      try {
        const result = raw.pset.templateGetApplicable(pqt, ifc_class, predefined_type, pset_only, qto_only, schema_name);
        return wrapEntities(shell, result as never) as Entity[];
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Return names of property set templates applicable to an IFC class.
     *
     * Same filtering as pset_template_get_applicable but returns only the
     * Name strings.
     */
    templateGetApplicableNames(pqt: PsetTemplate, ifc_class: string, predefined_type: string, pset_only: boolean, qto_only: boolean, schema_name: string): string[] {
      const temps: Disposable[] = [];
      try {
        const result = raw.pset.templateGetApplicableNames(pqt, ifc_class, predefined_type, pset_only, qto_only, schema_name);
        return wrap(shell, result) as string[];
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Look up a property set template by name.
     *
     * Returns the IfcPropertySetTemplate entity with the given name, or a
     * no result if the template is not found.
     */
    templateGetByName(pqt: PsetTemplate, name: string): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.pset.templateGetByName(pqt, name);
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Return the cached property template collection for the given schema (e.g. "IFC4", "IFC2X3").
     *
     * Loads and caches the built-in templates on first call. Returns no result
     * if the schema is unknown or templates are not available.
     */
    templateGetTemplate(schema_identifier: string): PsetTemplate | null {
      const temps: Disposable[] = [];
      try {
        const result = raw.pset.templateGetTemplate(schema_identifier);
        return result as PsetTemplate | null;
      } finally {
        disposeAll(temps);
      }
    },
    /** Return true if a property set template with the given name exists. */
    templateIsTemplated(pqt: PsetTemplate, name: string): boolean {
      const temps: Disposable[] = [];
      try {
        const result = raw.pset.templateIsTemplated(pqt, name);
        return wrap(shell, result) as boolean;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Return the template type category for a property set template.
     *
     * Returns "PSET" for property set templates, "QTO" for quantity templates,
     * or an empty string if unknown.
     */
    templatePsetType(pset_template: Entity): string {
      const temps: Disposable[] = [];
      try {
        const result = raw.pset.templatePsetType(pset_template.raw);
        return wrap(shell, result) as string;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Remove a property template from its parent set template.
     *
     * Removes the IfcSimplePropertyTemplate from its parent and deletes the
     * template entity.
     */
    templateRemovePropTemplate(file: IfcFile, prop_template: Entity): void {
      const temps: Disposable[] = [];
      try {
        raw.pset.templateRemovePropTemplate(file.raw, prop_template.raw);
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Remove a property set template and its child property templates.
     *
     * Deletes the IfcPropertySetTemplate and all its child
     * IfcSimplePropertyTemplate entities.
     */
    templateRemovePsetTemplate(pset_template: Entity): void {
      const temps: Disposable[] = [];
      try {
        raw.pset.templateRemovePsetTemplate(pset_template.raw);
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Set the directory used to locate built-in pset/qto template files.
     *
     * Must be called before pset_template_get_template to override the default
     * search path for IFC property set template files.
     */
    templateSetTemplateDir(dir: string): void {
      const temps: Disposable[] = [];
      try {
        raw.pset.templateSetTemplateDir(dir);
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Remove products from a property set's associations.
     *
     * For IfcTypeProduct instances, removes the pset from HasPropertySets.
     * For other products, removes them from IfcRelDefinesByProperties
     * relationships. Orphaned relationships are deleted. The pset itself is
     * not deleted.
     */
    unassignPset(file: IfcFile, products: Entity[], pset: Entity): void {
      const temps: Disposable[] = [];
      try {
        raw.pset.unassignPset(file.raw, toRawSequence(products, shell, temps), pset.raw);
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Unshare a property set by creating independent copies for specified products.
     *
     * When all assigned products are selected, one product retains the original
     * and the other products receive copies. When only some products are
     * selected, each selected product receives a copy and the original remains
     * assigned to the unselected products. Returns the newly created copies.
     */
    unsharePset(file: IfcFile, options: IfcOpenShellPsetUnsharePsetOptions): Entity[] {
      const temps: Disposable[] = [];
      try {
        const result = raw.pset.unsharePset(file.raw, encodeOptions(options, {"application": "application", "ownerHistory": "owner_history", "products": "products", "pset": "pset", "user": "user"}, shell, temps, [], ["products"]));
        return wrapEntities(shell, result as never) as Entity[];
      } finally {
        disposeAll(temps);
      }
    },
    }),
    register: Object.freeze({
    /**
     * Register an IFC file for schema-aware derived-value evaluation.
     *
     * The registered file is used when evaluating derived attributes for the
     * specified schema.
     *
     * @param schema_name The IFC schema identifier (e.g. "IFC4").
     * @param file The IFC file to register.
     * @return True after the file is registered.
     */
    scratchFile(schema_name: string, file: IfcFile): boolean {
      const temps: Disposable[] = [];
      try {
        const result = raw.register.scratchFile(schema_name, file.raw);
        return wrap(shell, result) as boolean;
      } finally {
        disposeAll(temps);
      }
    },
    }),
    representation: Object.freeze({
    /**
     * Return a geometric representation context matching the given criteria.
     *
     * Searches for IfcGeometricRepresentationSubContext when subcontext or
     * target_view is provided, otherwise searches for
     * IfcGeometricRepresentationContext (excluding subcontexts).
     *
     * @param file The IFC file to search.
     * @param context_type Context type filter (e.g. "Model", "Plan").
     * @param subcontext Context identifier filter (e.g. "Body", "Axis").
     * @param target_view Target view filter (e.g. "MODEL_VIEW", "GRAPH_VIEW").
     * @return The first matching context, or no result if none is found.
     */
    getContext(file: IfcFile, context_type: string, subcontext: string, target_view: string): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.representation.getContext(file.raw, context_type, subcontext, target_view);
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Return all geometric representation contexts sorted by priority.
     *
     * Sorts by ContextType (Model > Plan > Annotation), then by
     * ContextIdentifier (Body > Body-FallBack > ...), then by
     * TargetView (MODEL_VIEW > PLAN_VIEW > ...), then by TargetScale. Ties
     * preserve the order of contexts in the IFC file.
     *
     * @param file The IFC file to search.
     * @return Ordered list of IfcGeometricRepresentationContext entities.
     */
    getPrioritisedContexts(file: IfcFile): Entity[] {
      const temps: Disposable[] = [];
      try {
        const result = raw.representation.getPrioritisedContexts(file.raw);
        return wrapEntities(shell, result as never) as Entity[];
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Return a product's representation filtered by context.
     *
     * For an IfcProduct, reads Representation/Representations. For an
     * IfcTypeProduct, reads RepresentationMaps/MappedRepresentation.
     * Returns the first representation matching the context filters.
     *
     * @param element The IfcProduct or IfcTypeProduct.
     * @param options Context filtering options.
     * @return The matching IfcShapeRepresentation, or no result if none is found.
     */
    getProductRepresentation(element: Entity, options: IfcOpenShellRepresentationGetProductRepresentationOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.representation.getProductRepresentation(element.raw, encodeOptions(options, {"context": "context", "contextType": "context_type", "subcontext": "subcontext", "targetView": "target_view"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Resolve a representation through single mapped items.
     *
     * If a representation contains a single IfcMappedItem whose
     * MappingSource points to another representation, this function
     * follows the chain and returns the innermost representation. A representation
     * that does not meet this condition is returned unchanged.
     *
     * @param representation The IfcShapeRepresentation to resolve.
     * @return The resolved representation, or the original when no mapping is followed.
     */
    resolve(representation: Entity): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.representation.resolve(representation.raw);
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Return the base items of a representation, unwrapping mapped items and boolean operands.
     *
     * Returns leaf-level representation items in traversal order. Within each
     * representation, later items are returned before earlier items; for boolean
     * results, the second operand is returned before the first operand.
     *
     * @param representation The IfcShapeRepresentation to resolve.
     * @return List of leaf-level IfcRepresentationItem entities.
     */
    resolveBaseItems(representation: Entity): Entity[] {
      const temps: Disposable[] = [];
      try {
        const result = raw.representation.resolveBaseItems(representation.raw);
        return wrapEntities(shell, result as never) as Entity[];
      } finally {
        disposeAll(temps);
      }
    },
    }),
    resource: Object.freeze({
    /**
     * Edit attributes of an IfcResourceTime entity.
     *
     * Applies attribute changes from the property bag to the resource time.
     * Attributes that are hard-constrained (ConstraintGrade HARD and Benchmark
     * EQUALTO) on the owning IfcResource are skipped. If ScheduleUsage is
     * changed and a ScheduleWork constraint exists, the associated IfcTask
     * duration is recalculated. Duration values use ISO 8601 format
     * (e.g. "PT8H", "P5D"). Date-time values use ISO 8601 extended format
     * (e.g. "2024-01-15T09:00:00").
     *
     * @param file File containing the resource time.
     * @param resource_time IfcResourceTime entity to edit.
     * @param attributes Property bag of attribute name/value pairs.
     */
    editResourceTime(file: IfcFile, resource_time: Entity, attributes: PsetProperties | PsetInput): void {
      const temps: Disposable[] = [];
      try {
        raw.resource.editResourceTime(file.raw, resource_time.raw, toRawPsetProperties(shell, attributes as PsetProperties | PsetInput, temps));
      } finally {
        disposeAll(temps);
      }
    },
    }),
    root: Object.freeze({
    /**
     * Create an IFC entity with generated identity, optional name, and optional
     * predefined type.
     *
     * Sets GlobalId (for IfcRoot-derived entities). OwnerHistory is assigned only
     * when the owner_history option is provided; it is not created
     * automatically. Schema-specific defaults are applied for spatial elements,
     * element types, and door/window styles. If the predefined type is not a valid
     * enum value, it is stored as USERDEFINED with the value in ObjectType
     * (or ElementType/ProcessType where applicable).
     */
    createEntity(file: IfcFile, options: IfcOpenShellRootCreateEntityOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.root.createEntity(file.raw, encodeOptions(options, {"ifcClass": "ifc_class", "name": "name", "ownerHistory": "owner_history", "predefinedType": "predefined_type"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Remove a product and all its relationships.
     *
     * Removes the product and cleans up its related representations, object placements,
     * opening elements, property sets, material assignments, type definitions,
     * space boundaries, nesting relationships, aggregate relationships, spatial
     * containment, element connections, port connections, group memberships,
     * and grid axes. Related entities are removed only when they are no longer
     * needed by the remaining model.
     */
    removeProduct(file: IfcFile, product: Entity, options: IfcOpenShellRootRemoveProductOptions): void {
      const temps: Disposable[] = [];
      try {
        raw.root.removeProduct(file.raw, product.raw, encodeOptions(options, {"application": "application", "user": "user"}, shell, temps));
      } finally {
        disposeAll(temps);
      }
    },
    }),
    schema: Object.freeze({
    /**
     * Change the IFC class of an entity.
     *
     * Removes the element and recreates it as the new class with the same
     * STEP id. Compatible attributes are preserved; incompatible attributes
     * are dropped. Inverse references to the old element are updated to
     * point to the new element.
     *
     * If the element is already of the requested class, returns it unchanged.
     *
     * @param file IFC file to modify. When omitted, the element's file is used.
     * @param element The entity to reassign.
     * @param new_class The target IFC class name (e.g. "IfcWall").
     * @return The new entity of the requested class, or no result if the operation fails.
     */
    reassignClass(file: IfcFile, element: Entity, new_class: string): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.schema.reassignClass(file.raw, element.raw, new_class);
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    }),
    selector: Object.freeze({
    /**
     * Filter all elements in a file using a selector query.
     *
     * Parses the query as a filter expression and returns all matching
     * elements as a list value.
     *
     * @param file The IFC file to search.
     * @param query The filter query string.
     * @return List value of matching elements, or no result if the query cannot be evaluated. Release it with value_free.
     */
    filterAll(file: IfcFile, query: string): ValueData | null {
      const temps: Disposable[] = [];
      try {
        const result = raw.selector.filterAll(file.raw, query);
        return fromRawValue(shell, result as never) as ValueData | null;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Filter a list of elements using a selector query.
     *
     * Parses the query as a filter expression and returns the subset
     * of elements that match.
     *
     * @param file The IFC file context.
     * @param query The filter query string.
     * @param elements The elements to filter.
     * @return List value of matching elements, or no result if the query cannot be evaluated. Release it with value_free.
     */
    filterElements(file: IfcFile, query: string, elements: Entity[]): ValueData | null {
      const temps: Disposable[] = [];
      try {
        const result = raw.selector.filterElements(file.raw, query, toRawSequence(elements, shell, temps));
        return fromRawValue(shell, result as never) as ValueData | null;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Format a selector query against an element.
     *
     * Evaluates a format expression (e.g. attribute names, arithmetic,
     * string functions) against an element and returns the formatted string.
     *
     * @param file Optional IFC file context.
     * @param instance The element to format against.
     * @param query The format expression string.
     * @return The formatted result, or no result if evaluation fails.
     */
    format(file: IfcFile, instance: Entity, query: string): string | null {
      const temps: Disposable[] = [];
      try {
        const result = raw.selector.format(file.raw, instance.raw, query);
        return wrap(shell, result) as string | null;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Get a value from an element using a selector query.
     *
     * Parses the query as a get-element key path and extracts the
     * corresponding value from the element's attributes.
     *
     * @param file Optional IFC file context.
     * @param element The element to query.
     * @param query The selector key path (e.g. "Name", "Pset_WallCommon.FireRating").
     * @return The extracted value, or no result if the query cannot be evaluated. Release it with value_free.
     */
    getElementValue(file: IfcFile, element: Entity, query: string): ValueData | null {
      const temps: Disposable[] = [];
      try {
        const result = raw.selector.getElementValue(file.raw, element.raw, query);
        return fromRawValue(shell, result as never) as ValueData | null;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Set a value on an element using a selector query.
     *
     * Parses the query to identify the target attribute (e.g. a property
     * set property or direct attribute) and sets it to the provided value.
     *
     * @param file The IFC file context.
     * @param element The element to modify.
     * @param query The selector key path identifying the target.
     * @param value The value to set. When omitted, the target is unset.
     * @param concat When provided and non-empty, it is prepended to the value.
     */
    setElementValue(file: IfcFile, element: Entity, query: string, value: ValueInput | null, concat: string): void {
      const temps: Disposable[] = [];
      try {
        raw.selector.setElementValue(file.raw, element.raw, query, value == null ? null : toRawValue(shell, value, temps), concat);
      } finally {
        disposeAll(temps);
      }
    },
    }),
    sequence: Object.freeze({
    /**
     * Parse an ISO 8601 date-time string and return a schema-appropriate representation.
     *
     * For IFC4+, returns the string as-is. For IFC2X3, returns an IfcDateAndTime
     * entity constructed from the parsed components.
     *
     * @param file File to determine the schema version.
     * @param date_time ISO 8601 date-time string (e.g. "2024-01-15T09:00:00").
     * @return An IfcDateAndTime entity (IFC2X3) or the original string (IFC4+).
     */
    addDateTime(file: IfcFile, date_time: string): Entity | string {
      const temps: Disposable[] = [];
      try {
        const result = raw.sequence.addDateTime(file.raw, date_time);
        return wrap(shell, result) as Entity | string;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Create an IfcTask entity.
     *
     * When both work_schedule and parent_task are provided, work_schedule takes
     * precedence and the task is assigned to it via IfcRelAssignsToControl.
     * When only parent_task is provided, the task is nested under it via
     * IfcRelNests and its Identification is auto-generated as "parent.N" on IFC4+.
     *
     * @param file File that receives the new entity.
     * @param options Task placement and attribute options.
     * @return Newly created IfcTask.
     */
    addTask(file: IfcFile, options: IfcOpenShellSequenceAddTaskOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.sequence.addTask(file.raw, encodeOptions(options, {"application": "application", "description": "description", "identification": "identification", "name": "name", "ownerHistory": "owner_history", "parentTask": "parent_task", "predefinedType": "predefined_type", "user": "user", "workSchedule": "work_schedule"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Create an IfcTaskTime (or IfcTaskTimeRecurring) and assign it to a task.
     *
     * Replaces any existing TaskTime on the task.
     *
     * @param file File that receives the new entity.
     * @param task IfcTask to receive the task time.
     * @param options Whether to create a recurring task time.
     * @return Newly created IfcTaskTime or IfcTaskTimeRecurring.
     */
    addTaskTime(file: IfcFile, task: Entity, options: IfcOpenShellSequenceAddTaskTimeOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.sequence.addTaskTime(file.raw, task.raw, encodeOptions(options, {"isRecurring": "is_recurring"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Create an IfcTimePeriod and add it to a recurrence pattern.
     *
     * @param file File that receives the new entity.
     * @param recurrence_pattern IfcRecurrencePattern to receive the time period.
     * @param options Start and end time strings.
     * @return Newly created IfcTimePeriod.
     */
    addTimePeriod(file: IfcFile, recurrence_pattern: Entity, options: IfcOpenShellSequenceAddTimePeriodOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.sequence.addTimePeriod(file.raw, recurrence_pattern.raw, encodeOptions(options, {"endTime": "end_time", "startTime": "start_time"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Create an IfcWorkCalendar and declare it on the project.
     *
     * @param file File that receives the new entity.
     * @param options Calendar name, predefined type, and ownership options.
     * @return Newly created IfcWorkCalendar.
     */
    addWorkCalendar(file: IfcFile, options: IfcOpenShellSequenceAddWorkCalendarOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.sequence.addWorkCalendar(file.raw, encodeOptions(options, {"application": "application", "name": "name", "ownerHistory": "owner_history", "predefinedType": "predefined_type", "user": "user"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Create an IfcWorkPlan and declare it on the project.
     *
     * @param file File that receives the new entity.
     * @param options Work plan name, dates, creator, and ownership options.
     * @return Newly created IfcWorkPlan.
     */
    addWorkPlan(file: IfcFile, options: IfcOpenShellSequenceAddWorkPlanOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.sequence.addWorkPlan(file.raw, encodeOptions(options, {"application": "application", "creationDate": "creation_date", "creatorPerson": "creator_person", "name": "name", "ownerHistory": "owner_history", "predefinedType": "predefined_type", "startTime": "start_time", "user": "user"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Create an IfcWorkSchedule.
     *
     * When work_plan is provided, the schedule is nested under it via
     * IfcRelAggregates. Otherwise it is declared directly on the project
     * context (IFC4+).
     *
     * @param file File that receives the new entity.
     * @param options Schedule name, type, dates, work plan, and ownership options.
     * @return Newly created IfcWorkSchedule.
     */
    addWorkSchedule(file: IfcFile, options: IfcOpenShellSequenceAddWorkScheduleOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.sequence.addWorkSchedule(file.raw, encodeOptions(options, {"application": "application", "creationDate": "creation_date", "creatorPerson": "creator_person", "name": "name", "objectType": "object_type", "ownerHistory": "owner_history", "predefinedType": "predefined_type", "startTime": "start_time", "user": "user", "workPlan": "work_plan"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Create an IfcWorkTime and add it to a work calendar.
     *
     * @param file File that receives the new entity.
     * @param work_calendar IfcWorkCalendar to receive the work time.
     * @param time_type "WorkingTimes" or "ExceptionTimes".
     * @return Newly created IfcWorkTime.
     */
    addWorkTime(file: IfcFile, work_calendar: Entity, time_type: string): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.sequence.addWorkTime(file.raw, work_calendar.raw, time_type);
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Create an IfcLagTime and assign it to an IfcRelSequence.
     *
     * Replaces any existing TimeLag on the sequence (removing the old one if
     * it is sole-referenced).
     *
     * @param file File that receives the new entity.
     * @param rel_sequence IfcRelSequence to receive the lag time.
     * @param lag_value ISO 8601 duration string (e.g. "P2D", "PT8H").
     * @param options Duration type option. Defaults to "WORKTIME".
     * @return Newly created IfcLagTime.
     */
    assignLagTime(file: IfcFile, rel_sequence: Entity, lag_value: string, options: IfcOpenShellSequenceAssignLagTimeOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.sequence.assignLagTime(file.raw, rel_sequence.raw, lag_value, encodeOptions(options, {"durationType": "duration_type"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Assign a process (IfcTask) to an object via IfcRelAssignsToProcess.
     *
     * If a relationship already exists for this process and object, it is
     * returned. Otherwise a new relationship is created or the object is
     * appended to an existing relationship.
     *
     * @param file File that receives the relationship.
     * @param relating_process IfcTask (process) to assign.
     * @param related_object Object to assign to the process.
     * @param options Ownership options.
     * @return The IfcRelAssignsToProcess relationship.
     */
    assignProcess(file: IfcFile, relating_process: Entity, related_object: Entity, options: IfcOpenShellSequenceAssignProcessOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.sequence.assignProcess(file.raw, relating_process.raw, related_object.raw, encodeOptions(options, {"application": "application", "ownerHistory": "owner_history", "user": "user"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Assign a product to an object via IfcRelAssignsToProduct.
     *
     * @param file File that receives the relationship.
     * @param relating_product IfcProduct to assign.
     * @param related_object Object to assign to the product.
     * @param options Ownership options.
     * @return The IfcRelAssignsToProduct relationship.
     */
    assignProduct(file: IfcFile, relating_product: Entity, related_object: Entity, options: IfcOpenShellSequenceAssignProductOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.sequence.assignProduct(file.raw, relating_product.raw, related_object.raw, encodeOptions(options, {"application": "application", "ownerHistory": "owner_history", "user": "user"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Create an IfcRecurrencePattern and assign it to a parent entity.
     *
     * Assigns the pattern to an IfcWorkTime's RecurrencePattern or an
     * IfcTaskTimeRecurring's Recurrence attribute. Replaces any existing
     * pattern on the parent (removing the old one if sole-referenced).
     *
     * @param file File that receives the new entity.
     * @param parent IfcWorkTime or IfcTaskTimeRecurring to receive the pattern.
     * @param recurrence_type RecurrenceType enum value (e.g. "WEEKLY").
     * @return Newly created IfcRecurrencePattern.
     */
    assignRecurrencePattern(file: IfcFile, parent: Entity, recurrence_type: string): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.sequence.assignRecurrencePattern(file.raw, parent.raw, recurrence_type);
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Create an IfcRelSequence linking two tasks.
     *
     * If a sequence relationship already exists between the two tasks, the
     * existing relationship is returned. After creating the relationship,
     * cascades schedule changes from the relating (predecessor) task.
     *
     * @param file File that receives the relationship.
     * @param relating_process Predecessor IfcTask.
     * @param related_process Successor IfcTask.
     * @param options Sequence type and ownership options.
     * @return The IfcRelSequence relationship.
     */
    assignSequence(file: IfcFile, relating_process: Entity, related_process: Entity, options: IfcOpenShellSequenceAssignSequenceOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.sequence.assignSequence(file.raw, relating_process.raw, related_process.raw, encodeOptions(options, {"application": "application", "ownerHistory": "owner_history", "sequenceType": "sequence_type", "user": "user"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Assign an IfcWorkSchedule to an IfcWorkPlan via IfcRelAggregates.
     *
     * First unassigns the schedule from its current project declaration, then
     * nests it under the work plan.
     *
     * @param file File containing both entities.
     * @param work_schedule IfcWorkSchedule to assign.
     * @param work_plan IfcWorkPlan to assign to.
     * @param options Ownership options.
     * @return The IfcRelAggregates relationship.
     */
    assignWorkPlan(file: IfcFile, work_schedule: Entity, work_plan: Entity, options: IfcOpenShellSequenceAssignWorkPlanOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.sequence.assignWorkPlan(file.raw, work_schedule.raw, work_plan.raw, encodeOptions(options, {"application": "application", "ownerHistory": "owner_history", "user": "user"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Recalculate a task's ScheduleDuration from its assigned resources.
     *
     * Computes the maximum resource usage duration across all IfcConstructionResource
     * entities assigned to the task (directly or via IfcRelAssignsToProcess),
     * accounting for ScheduleWork and ScheduleUsage. The result is written to the
     * task's IfcTaskTime.ScheduleDuration. Creates an IfcTaskTime if absent.
     *
     * @param file File containing the task.
     * @param task IfcTask entity to recalculate.
     */
    calculateTaskDuration(file: IfcFile, task: Entity): void {
      const temps: Disposable[] = [];
      try {
        raw.sequence.calculateTaskDuration(file.raw, task.raw);
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Cascade schedule dates from a task to all its successors.
     *
     * Propagates ScheduleStart and ScheduleFinish dates forward through the
     * task network, respecting sequence types (FS, SS, FF, SF), lag times,
     * duration types, and work calendars.
     *
     * @param file File containing the task network.
     * @param task IfcTask from which to cascade.
     */
    cascadeSchedule(file: IfcFile, task: Entity): void {
      const temps: Disposable[] = [];
      try {
        raw.sequence.cascadeSchedule(file.raw, task.raw);
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Create an independent copy of an IfcWorkSchedule and its controlled tasks.
     *
     * Copies the schedule and each controlled IfcTask, including its subtasks and
     * relationships, then assigns the copies to the new schedule.
     *
     * @param file File that receives the copied entities.
     * @param work_schedule IfcWorkSchedule to copy.
     * @param options Ownership options for the copied entities.
     * @return Newly created IfcWorkSchedule.
     */
    copyWorkSchedule(file: IfcFile, work_schedule: Entity, options: IfcOpenShellSequenceCopyWorkScheduleOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.sequence.copyWorkSchedule(file.raw, work_schedule.raw, encodeOptions(options, {"application": "application", "ownerHistory": "owner_history", "user": "user"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Create a baseline snapshot of an IfcWorkSchedule.
     *
     * Only operates on schedules with PredefinedType PLANNED. Creates a new
     * IfcWorkSchedule with PredefinedType BASELINE, deep-copies all controlled
     * tasks into it, and creates IfcRelDefinesByObject references linking
     * original tasks to their baseline copies.
     *
     * @param file File that receives the baseline entities.
     * @param work_schedule IfcWorkSchedule to baseline.
     * @param options Name override and ownership options.
     */
    createBaseline(file: IfcFile, work_schedule: Entity, options: IfcOpenShellSequenceCreateBaselineOptions): void {
      const temps: Disposable[] = [];
      try {
        raw.sequence.createBaseline(file.raw, work_schedule.raw, encodeOptions(options, {"application": "application", "name": "name", "ownerHistory": "owner_history", "user": "user"}, shell, temps));
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Create an independent copy of a task and its subtasks, property sets, and
     * sequence relationships.
     *
     * Creates copies of the task, its nested child tasks, property sets, and
     * IfcRelSequence relationships between copied tasks. Returns parallel lists
     * of original and copied tasks in depth-first order.
     *
     * @param file File that receives the duplicated entities.
     * @param task IfcTask to duplicate.
     * @param options Ownership options for duplicated entities.
     * @return Parallel lists of original and copied tasks.
     */
    duplicateTask(file: IfcFile, task: Entity, options: IfcOpenShellSequenceDuplicateTaskOptions): IfcOpenShellSequenceDuplicateTaskResult {
      const temps: Disposable[] = [];
      try {
        const result = raw.sequence.duplicateTask(file.raw, task.raw, encodeOptions(options, {"application": "application", "ownerHistory": "owner_history", "user": "user"}, shell, temps));
        const data = result as NonNullable<IfcOpenShellSequenceDuplicateTaskResult>;
        return { current: wrapEntities(shell, data.current as never), duplicate: wrapEntities(shell, data.duplicate as never) } as IfcOpenShellSequenceDuplicateTaskResult;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Edit attributes of an IfcLagTime entity.
     *
     * @param lag_time IfcLagTime entity to edit.
     * @param attributes Property bag of attribute name/value pairs.
     */
    editLagTime(lag_time: Entity, attributes: PsetProperties | PsetInput): void {
      const temps: Disposable[] = [];
      try {
        raw.sequence.editLagTime(lag_time.raw, toRawPsetProperties(shell, attributes as PsetProperties | PsetInput, temps));
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Edit attributes of an IfcRecurrencePattern entity.
     *
     * @param recurrence_pattern IfcRecurrencePattern entity to edit.
     * @param attributes Property bag of attribute name/value pairs.
     */
    editRecurrencePattern(recurrence_pattern: Entity, attributes: PsetProperties | PsetInput): void {
      const temps: Disposable[] = [];
      try {
        raw.sequence.editRecurrencePattern(recurrence_pattern.raw, toRawPsetProperties(shell, attributes as PsetProperties | PsetInput, temps));
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Edit attributes of an IfcRelSequence entity.
     *
     * @param rel_sequence IfcRelSequence entity to edit.
     * @param attributes Property bag of attribute name/value pairs.
     */
    editSequence(rel_sequence: Entity, attributes: PsetProperties | PsetInput): void {
      const temps: Disposable[] = [];
      try {
        raw.sequence.editSequence(rel_sequence.raw, toRawPsetProperties(shell, attributes as PsetProperties | PsetInput, temps));
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Edit attributes of an IfcTask entity.
     *
     * @param task IfcTask entity to edit.
     * @param attributes Property bag of attribute name/value pairs.
     */
    editTask(task: Entity, attributes: PsetProperties | PsetInput): void {
      const temps: Disposable[] = [];
      try {
        raw.sequence.editTask(task.raw, toRawPsetProperties(shell, attributes as PsetProperties | PsetInput, temps));
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Edit attributes of an IfcTaskTime entity.
     *
     * Applies attribute changes from the property bag. Automatically derives
     * ScheduleFinish from ScheduleStart + ScheduleDuration, or ScheduleDuration
     * from ScheduleStart and ScheduleFinish, as appropriate. Dates are normalised
     * to the soonest working day based on the task's calendar. After editing,
     * cascades schedule changes to successor tasks.
     *
     * @param file File containing the task time.
     * @param task_time IfcTaskTime entity to edit.
     * @param attributes Property bag of attribute name/value pairs.
     */
    editTaskTime(file: IfcFile, task_time: Entity, attributes: PsetProperties | PsetInput): void {
      const temps: Disposable[] = [];
      try {
        raw.sequence.editTaskTime(file.raw, task_time.raw, toRawPsetProperties(shell, attributes as PsetProperties | PsetInput, temps));
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Edit attributes of an IfcWorkCalendar entity.
     *
     * @param work_calendar IfcWorkCalendar entity to edit.
     * @param attributes Property bag of attribute name/value pairs.
     */
    editWorkCalendar(work_calendar: Entity, attributes: PsetProperties | PsetInput): void {
      const temps: Disposable[] = [];
      try {
        raw.sequence.editWorkCalendar(work_calendar.raw, toRawPsetProperties(shell, attributes as PsetProperties | PsetInput, temps));
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Edit attributes of an IfcWorkPlan entity.
     *
     * @param work_plan IfcWorkPlan entity to edit.
     * @param attributes Property bag of attribute name/value pairs.
     */
    editWorkPlan(work_plan: Entity, attributes: PsetProperties | PsetInput): void {
      const temps: Disposable[] = [];
      try {
        raw.sequence.editWorkPlan(work_plan.raw, toRawPsetProperties(shell, attributes as PsetProperties | PsetInput, temps));
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Edit attributes of an IfcWorkSchedule entity.
     *
     * @param work_schedule IfcWorkSchedule entity to edit.
     * @param attributes Property bag of attribute name/value pairs.
     */
    editWorkSchedule(work_schedule: Entity, attributes: PsetProperties | PsetInput): void {
      const temps: Disposable[] = [];
      try {
        raw.sequence.editWorkSchedule(work_schedule.raw, toRawPsetProperties(shell, attributes as PsetProperties | PsetInput, temps));
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Edit attributes of an IfcWorkTime entity.
     *
     * Date attributes "Start"/"StartDate" and "Finish"/"FinishDate" are
     * automatically mapped to the correct attribute name for the schema version.
     *
     * @param work_time IfcWorkTime entity to edit.
     * @param attributes Property bag of attribute name/value pairs.
     */
    editWorkTime(work_time: Entity, attributes: PsetProperties | PsetInput): void {
      const temps: Disposable[] = [];
      try {
        raw.sequence.editWorkTime(work_time.raw, toRawPsetProperties(shell, attributes as PsetProperties | PsetInput, temps));
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Recalculate an entire work schedule using the critical path method.
     *
     * Performs forward and backward passes through the task network to compute
     * early/late start/finish dates, total float, free float, and critical
     * path flags on each IfcTaskTime.
     *
     * @param file File containing the work schedule.
     * @param work_schedule IfcWorkSchedule to recalculate.
     */
    recalculateSchedule(file: IfcFile, work_schedule: Entity): void {
      const temps: Disposable[] = [];
      try {
        raw.sequence.recalculateSchedule(file.raw, work_schedule.raw);
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Remove an IfcTask and its subtasks from the file.
     *
     * Recursively removes nested child tasks, their IfcTaskTime entities,
     * sequence relationships, control assignments, process assignments,
     * product assignments, property sets, and project declarations.
     *
     * @param file File containing the task.
     * @param task IfcTask to remove.
     * @param options Ownership options for relationship cleanup.
     */
    removeTask(file: IfcFile, task: Entity, options: IfcOpenShellSequenceRemoveOptions): void {
      const temps: Disposable[] = [];
      try {
        raw.sequence.removeTask(file.raw, task.raw, encodeOptions(options, {"application": "application", "user": "user"}, shell, temps));
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Remove an IfcTimePeriod from the file.
     *
     * @param file File containing the time period.
     * @param time_period IfcTimePeriod to remove.
     */
    removeTimePeriod(file: IfcFile, time_period: Entity): void {
      const temps: Disposable[] = [];
      try {
        raw.sequence.removeTimePeriod(file.raw, time_period.raw);
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Remove an IfcWorkCalendar from the file.
     *
     * Unassigns the calendar from all controlled tasks, removes all working
     * and exception times, and removes the project declaration.
     *
     * @param file File containing the calendar.
     * @param work_calendar IfcWorkCalendar to remove.
     * @param options Ownership options for relationship cleanup.
     */
    removeWorkCalendar(file: IfcFile, work_calendar: Entity, options: IfcOpenShellSequenceRemoveOptions): void {
      const temps: Disposable[] = [];
      try {
        raw.sequence.removeWorkCalendar(file.raw, work_calendar.raw, encodeOptions(options, {"application": "application", "user": "user"}, shell, temps));
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Remove an IfcWorkPlan from the file.
     *
     * Unassigns the plan from its project declaration and removes the aggregate
     * relationships to child work schedules (without removing the schedules).
     *
     * @param file File containing the work plan.
     * @param work_plan IfcWorkPlan to remove.
     * @param options Ownership options for relationship cleanup.
     */
    removeWorkPlan(file: IfcFile, work_plan: Entity, options: IfcOpenShellSequenceRemoveOptions): void {
      const temps: Disposable[] = [];
      try {
        raw.sequence.removeWorkPlan(file.raw, work_plan.raw, encodeOptions(options, {"application": "application", "user": "user"}, shell, temps));
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Remove an IfcWorkSchedule and its controlled tasks from the file.
     *
     * Recursively removes nested work schedules, all controlled IfcTask
     * entities (with their subtasks and relationships), aggregate and
     * declaration relationships, and the schedule itself.
     *
     * @param file File containing the work schedule.
     * @param work_schedule IfcWorkSchedule to remove.
     * @param options Ownership options for relationship cleanup.
     */
    removeWorkSchedule(file: IfcFile, work_schedule: Entity, options: IfcOpenShellSequenceRemoveOptions): void {
      const temps: Disposable[] = [];
      try {
        raw.sequence.removeWorkSchedule(file.raw, work_schedule.raw, encodeOptions(options, {"application": "application", "user": "user"}, shell, temps));
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Remove an IfcWorkTime and its associated recurrence pattern from the file.
     *
     * If the work time has a RecurrencePattern, all its TimePeriods and the
     * pattern itself are deleted first.
     *
     * @param file File containing the work time.
     * @param work_time IfcWorkTime to remove.
     */
    removeWorkTime(file: IfcFile, work_time: Entity): void {
      const temps: Disposable[] = [];
      try {
        raw.sequence.removeWorkTime(file.raw, work_time.raw);
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Remove the TimeLag from an IfcRelSequence.
     *
     * If the IfcLagTime is only referenced by this sequence, it is deleted.
     * Otherwise the TimeLag attribute is cleared.
     *
     * @param file File containing the sequence.
     * @param rel_sequence IfcRelSequence to remove the lag from.
     */
    unassignLagTime(file: IfcFile, rel_sequence: Entity): void {
      const temps: Disposable[] = [];
      try {
        raw.sequence.unassignLagTime(file.raw, rel_sequence.raw);
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Remove a process assignment via IfcRelAssignsToProcess.
     *
     * If the relationship has only one RelatedObjects entry, the entire
     * relationship is removed. Otherwise the object is removed from the
     * aggregate.
     *
     * @param file File containing the relationship.
     * @param relating_process IfcTask to unassign from.
     * @param related_object Object to unassign.
     * @param options Ownership options.
     * @return The modified relationship, or no result when it is removed.
     */
    unassignProcess(file: IfcFile, relating_process: Entity, related_object: Entity, options: IfcOpenShellSequenceRemoveOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.sequence.unassignProcess(file.raw, relating_process.raw, related_object.raw, encodeOptions(options, {"application": "application", "user": "user"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Remove a product assignment via IfcRelAssignsToProduct.
     *
     * If the relationship has only one RelatedObjects entry, the entire
     * relationship is removed. Otherwise the object is removed from the
     * aggregate.
     *
     * @param file File containing the relationship.
     * @param relating_product IfcProduct to unassign from.
     * @param related_object Object to unassign.
     * @param options Ownership options.
     * @return The modified relationship, or no result when it is removed.
     */
    unassignProduct(file: IfcFile, relating_product: Entity, related_object: Entity, options: IfcOpenShellSequenceRemoveOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.sequence.unassignProduct(file.raw, relating_product.raw, related_object.raw, encodeOptions(options, {"application": "application", "user": "user"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Remove an IfcRecurrencePattern and its time periods from the file.
     *
     * Deletes all IfcTimePeriod entities in the pattern's TimePeriods aggregate,
     * then deletes the pattern itself.
     *
     * @param file File containing the recurrence pattern.
     * @param recurrence_pattern IfcRecurrencePattern to remove.
     */
    unassignRecurrencePattern(file: IfcFile, recurrence_pattern: Entity): void {
      const temps: Disposable[] = [];
      try {
        raw.sequence.unassignRecurrencePattern(file.raw, recurrence_pattern.raw);
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Remove an IfcRelSequence between two tasks.
     *
     * Removes the sequence relationship where relating_process is the
     * predecessor and related_process is the successor, then cascades
     * schedule changes from the successor.
     *
     * @param file File containing the tasks.
     * @param relating_process Predecessor IfcTask.
     * @param related_process Successor IfcTask.
     */
    unassignSequence(file: IfcFile, relating_process: Entity, related_process: Entity): void {
      const temps: Disposable[] = [];
      try {
        raw.sequence.unassignSequence(file.raw, relating_process.raw, related_process.raw);
      } finally {
        disposeAll(temps);
      }
    },
    }),
    shape: Object.freeze({
    /**
     * Create an IfcAxis2Placement2D from position and optional reference direction.
     *
     * @param file IFC file that receives the placement.
     * @param options Position and optional X direction.
     * @return IfcAxis2Placement2D entity.
     */
    builderAxis2Placement2d(file: IfcFile, options: IfcOpenShellShapeBuilderAxis2Placement2dOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.shape.builderAxis2Placement2d(file.raw, encodeOptions(options, {"position": "position", "xDirection": "x_direction"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Create an IfcAxis2Placement3D from position and axes.
     *
     * @param file IFC file that receives the placement.
     * @param options Position, Z axis, and X axis direction ratios.
     * @return IfcAxis2Placement3D entity.
     */
    builderAxis2Placement3d(file: IfcFile, options: IfcOpenShellShapeBuilderAxis2Placement3dOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.shape.builderAxis2Placement3d(file.raw, encodeOptions(options, {"position": "position", "xAxis": "x_axis", "zAxis": "z_axis"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Create an IfcBlock (rectangular parallelepiped).
     *
     * @param file IFC file that receives the geometry.
     * @param options Position and XYZ dimensions.
     * @return IfcBlock entity.
     */
    builderBlock(file: IfcFile, options: IfcOpenShellShapeBuilderBlockOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.shape.builderBlock(file.raw, encodeOptions(options, {"position": "position", "xLength": "x_length", "yLength": "y_length", "zLength": "z_length"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Create an IfcCircle centred at the given position.
     *
     * @param file IFC file that receives the geometry.
     * @param center XY coordinates of the circle centre.
     * @param radius Circle radius in model units.
     * @return IfcCircle entity.
     */
    builderCircle(file: IfcFile, center: number[], radius: number): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.shape.builderCircle(file.raw, toRawSequence(center, shell, temps), radius);
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Create a 2D IfcIndexedPolyCurve arc between two points.
     *
     * Inserts a midpoint slightly offset along the dominant axis to form a
     * three-point arc segment.
     *
     * @param file IFC file that receives the geometry.
     * @param points Two XY endpoints.
     * @return IfcIndexedPolyCurve entity with one arc segment.
     */
    builderCurveBetweenTwoPoints(file: IfcFile, points: number[][]): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.shape.builderCurveBetweenTwoPoints(file.raw, toRawSequence(points, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Create an independent copy of an IFC entity and the entities it references.
     *
     * New GlobalId attributes are generated for the copied entities.
     *
     * @param file IFC file that receives the copy.
     * @param element Entity to copy.
     * @return Root entity of the copied subgraph.
     */
    builderDeepCopy(file: IfcFile, element: Entity): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.shape.builderDeepCopy(file.raw, element.raw);
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Create an IfcEdge between two vertex positions.
     *
     * @param file IFC file that receives the geometry.
     * @param start XYZ coordinates of the edge start.
     * @param end XYZ coordinates of the edge end.
     * @return IfcEdge entity.
     */
    builderEdge(file: IfcFile, start: number[], end: number[]): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.shape.builderEdge(file.raw, toRawSequence(start, shell, temps), toRawSequence(end, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Create an IfcEllipse, optionally trimmed to an IfcTrimmedCurve.
     *
     * @param file IFC file that receives the geometry.
     * @param options Radii, position, trim points, and reference direction.
     * @return IfcEllipse or IfcTrimmedCurve entity.
     */
    builderEllipseCurve(file: IfcFile, options: IfcOpenShellShapeBuilderEllipseCurveOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.shape.builderEllipseCurve(file.raw, encodeOptions(options, {"position": "position", "refXDirection": "ref_x_direction", "trimPoints": "trim_points", "trimPointsMask": "trim_points_mask", "xAxisRadius": "x_axis_radius", "yAxisRadius": "y_axis_radius"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Create an IfcExtrudedAreaSolid by extruding a profile along a direction.
     *
     * If the profile_or_curve is a bare IfcCurve (not an IfcProfileDef), it is
     * wrapped in an IfcArbitraryClosedProfileDef automatically.
     *
     * @param file IFC file that receives the geometry.
     * @param options Profile, depth, position, and extrusion direction.
     * @return IfcExtrudedAreaSolid entity.
     */
    builderExtrude(file: IfcFile, options: IfcOpenShellShapeBuilderExtrudeOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.shape.builderExtrude(file.raw, encodeOptions(options, {"extrusionVector": "extrusion_vector", "magnitude": "magnitude", "position": "position", "positionXAxis": "position_x_axis", "positionYAxis": "position_y_axis", "positionZAxis": "position_z_axis", "profileOrCurve": "profile_or_curve"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Create an IfcFace from an ordered list of polygon vertices.
     *
     * @param file IFC file that receives the geometry.
     * @param points XYZ coordinates defining the face outer boundary.
     * @return IfcFace entity with an IfcFaceOuterBound.
     */
    builderFace(file: IfcFile, points: number[][]): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.shape.builderFace(file.raw, toRawSequence(points, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Create an IfcFacetedBrep from vertices and face index lists.
     *
     * @param file IFC file that receives the geometry.
     * @param points XYZ vertex coordinates.
     * @param faces Face index lists (zero-based). Each face is a single outer loop.
     * @return IfcFacetedBrep entity with an IfcClosedShell.
     */
    builderFacetedBrep(file: IfcFile, points: number[][], faces: number[][]): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.shape.builderFacetedBrep(file.raw, toRawSequence(points, shell, temps), toRawSequence(faces, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Read the coordinate list from an IfcPolyline or IfcIndexedPolyCurve.
     *
     * @param polyline IfcPolyline or IfcIndexedPolyCurve entity.
     * @return Ordered XY or XYZ coordinate sequences.
     */
    builderGetPolylineCoords(polyline: Entity): number[][] {
      const temps: Disposable[] = [];
      try {
        const result = raw.shape.builderGetPolylineCoords(polyline.raw);
        return wrap(shell, result) as number[][];
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Create an IfcHalfSpaceSolid bounded by a plane.
     *
     * @param file IFC file that receives the geometry.
     * @param options Bounding plane and agreement flag.
     * @return IfcHalfSpaceSolid entity.
     */
    builderHalfSpaceSolid(file: IfcFile, options: IfcOpenShellShapeBuilderHalfSpaceSolidOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.shape.builderHalfSpaceSolid(file.raw, encodeOptions(options, {"agreementFlag": "agreement_flag", "plane": "plane"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Create a 2D IfcIndexedPolyCurve from explicit points and segment indices.
     *
     * Segments with two indices are line segments; segments with three indices
     * are arc segments.
     *
     * @param file IFC file that receives the geometry.
     * @param points XY coordinates for the point list.
     * @param segments Segment index arrays (1-based).
     * @return IfcIndexedPolyCurve entity.
     */
    builderIndexedPolycurve2d(file: IfcFile, points: number[][], segments: number[][]): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.shape.builderIndexedPolycurve2d(file.raw, toRawSequence(points, shell, temps), toRawSequence(segments, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Build MEP bend geometry for a duct segment.
     *
     * Generates a swept disk solid (for circular profiles) or an extruded bend
     * shape (for rectangular profiles), plus optional start and end straight
     * extensions.
     *
     * @param file IFC file that receives the geometry.
     * @param options Segment, lengths, angle, radius, bend direction, and Z flip.
     * @return Bend result with representation and computed parameters.
     */
    builderMepBendShape(file: IfcFile, options: IfcOpenShellShapeBuilderMepBendShapeOptions): IfcOpenShellShapeBuilderMepBendShapeResult {
      const temps: Disposable[] = [];
      try {
        const result = raw.shape.builderMepBendShape(file.raw, encodeOptions(options, {"angle": "angle", "bendVector": "bend_vector", "endLength": "end_length", "flipZAxis": "flip_z_axis", "radius": "radius", "segment": "segment", "startLength": "start_length"}, shell, temps));
        const data = result as NonNullable<IfcOpenShellShapeBuilderMepBendShapeResult>;
        return { representation: wrapEntity(shell, data.representation), startLength: data.startLength, endLength: data.endLength, radius: data.radius, angle: data.angle, lateralAxis: data.lateralAxis, lateralSign: data.lateralSign, zAxisSign: data.zAxisSign, mainProfileDimension: data.mainProfileDimension } as IfcOpenShellShapeBuilderMepBendShapeResult;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Compute the transition angle or length between two MEP duct profiles.
     *
     * Provide either length or angle (not both) to solve for the other.
     * Returns NaN when no valid solution exists.
     *
     * @param options Profile half-dimensions, offset, and one of length or angle.
     * @return The solved angle (degrees) or length (model units), or NaN.
     */
    builderMepTransitionCalculate(options: IfcOpenShellShapeBuilderMepTransitionCalculateOptions): number {
      const temps: Disposable[] = [];
      try {
        const result = raw.shape.builderMepTransitionCalculate(encodeOptions(options, {"angle": "angle", "diff": "diff", "endHalfDim": "end_half_dim", "endProfile": "end_profile", "length": "length", "offset": "offset", "startHalfDim": "start_half_dim"}, shell, temps));
        return wrap(shell, result) as number;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Compute the minimum transition length between two MEP duct profiles at a given angle.
     *
     * @param options Profile half-dimensions, angle in degrees, and profile offset.
     * @return Required transition length in model units, or NaN if no valid solution.
     */
    builderMepTransitionLength(options: IfcOpenShellShapeBuilderMepTransitionLengthOptions): number {
      const temps: Disposable[] = [];
      try {
        const result = raw.shape.builderMepTransitionLength(encodeOptions(options, {"angle": "angle", "endHalfDim": "end_half_dim", "profileOffset": "profile_offset", "startHalfDim": "start_half_dim"}, shell, temps));
        return wrap(shell, result) as number;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Build MEP transition geometry between two duct segments.
     *
     * Generates start/end extrusions and a connecting transition mesh.
     * Returns no result when the segments lack material profiles or the
     * transition cannot be computed.
     *
     * @param file IFC file that receives the geometry.
     * @param options Start/end segments, lengths, angle, and profile offset.
     * @return Transition result with representation and dimensions, or no result.
     */
    builderMepTransitionShape(file: IfcFile, options: IfcOpenShellShapeBuilderMepTransitionShapeOptions): IfcOpenShellShapeBuilderMepTransitionShapeResult | null {
      const temps: Disposable[] = [];
      try {
        const result = raw.shape.builderMepTransitionShape(file.raw, encodeOptions(options, {"angle": "angle", "endLength": "end_length", "endSegment": "end_segment", "profileOffset": "profile_offset", "startLength": "start_length", "startSegment": "start_segment"}, shell, temps));
        if (result === null) return null;
        const data = result as NonNullable<IfcOpenShellShapeBuilderMepTransitionShapeResult | null>;
        return { representation: wrapEntity(shell, data.representation), startLength: data.startLength, endLength: data.endLength, angle: data.angle, profileOffset: wrap(shell, data.profileOffset), transitionLength: data.transitionLength, fullTransitionLength: data.fullTransitionLength } as IfcOpenShellShapeBuilderMepTransitionShapeResult | null;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Create a mesh representation using IfcPolygonalFaceSet (IFC4+) or IfcFacetedBrep (IFC2X3).
     *
     * @param file IFC file that receives the geometry.
     * @param points XYZ vertex coordinates.
     * @param faces Face index lists (zero-based).
     * @return IfcPolygonalFaceSet or IfcFacetedBrep entity.
     */
    builderMesh(file: IfcFile, points: number[][], faces: number[][]): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.shape.builderMesh(file.raw, toRawSequence(points, shell, temps), toRawSequence(faces, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Mirror a geometry item across 2D axes.
     *
     * Supports IfcIndexedPolyCurve, IfcPolyline, IfcCircle, IfcEllipse,
     * IfcTrimmedCurve, and IfcExtrudedAreaSolid. A non-zero value in
     * mirror_axes at index 0 or 1 flips the corresponding coordinate.
     *
     * @param file IFC file containing the item.
     * @param options Item, axes, point, copy flag, and optional placement matrix.
     * @return The mirrored item, either the supplied entity or an independent copy.
     */
    builderMirror(file: IfcFile, options: IfcOpenShellShapeBuilderMirrorOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.shape.builderMirror(file.raw, encodeOptions(options, {"createCopy": "create_copy", "item": "item", "mirrorAxes": "mirror_axes", "mirrorPoint": "mirror_point", "placementMatrix": "placement_matrix"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Create an IfcPlane from a point and normal direction.
     *
     * The reference direction is computed automatically from the normal.
     *
     * @param file IFC file that receives the geometry.
     * @param location XYZ point on the plane.
     * @param normal Direction ratios of the plane normal.
     * @return IfcPlane entity.
     */
    builderPlane(file: IfcFile, location: number[], normal: number[]): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.shape.builderPlane(file.raw, toRawSequence(location, shell, temps), toRawSequence(normal, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Create an IfcPolygonalFaceSet from vertices and polygonal face loops.
     *
     * Each face may contain one outer loop and optional inner loops (voids).
     * Faces with multiple loops create IfcIndexedPolygonalFaceWithVoids entities.
     *
     * @param file IFC file that receives the geometry.
     * @param points XYZ vertex coordinates.
     * @param faces Per-face loop index lists (zero-based).
     * @return IfcPolygonalFaceSet entity.
     */
    builderPolygonalFaceSet(file: IfcFile, points: number[][], faces: number[][][]): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.shape.builderPolygonalFaceSet(file.raw, toRawSequence(points, shell, temps), toRawSequence(faces, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Create a 2D or 3D polyline (IfcPolyline for IFC2X3, IfcIndexedPolyCurve otherwise).
     *
     * @param file IFC file that receives the geometry.
     * @param options Points, closure, offset, and arc segment indices.
     * @return IfcPolyline or IfcIndexedPolyCurve entity.
     */
    builderPolyline(file: IfcFile, options: IfcOpenShellShapeBuilderPolylineOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.shape.builderPolyline(file.raw, encodeOptions(options, {"arcPoints": "arc_points", "closed": "closed", "points": "points", "positionOffset": "position_offset"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Create an IfcArbitraryClosedProfileDef or IfcArbitraryProfileDefWithVoids.
     *
     * @param file IFC file that receives the profile.
     * @param options Outer curve, optional inner curves, name, and profile type.
     * @return IfcProfileDef entity.
     */
    builderProfile(file: IfcFile, options: IfcOpenShellShapeBuilderProfileOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.shape.builderProfile(file.raw, encodeOptions(options, {"innerCurves": "inner_curves", "name": "name", "outerCurve": "outer_curve", "profileType": "profile_type"}, shell, temps, [], ["innerCurves"]));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Create an IfcShapeRepresentation or IfcTopologyRepresentation.
     *
     * The representation type is auto-detected from the items when omitted.
     * Topology types (Vertex, Edge, Path, Face, Shell) produce IfcTopologyRepresentation;
     * all others produce IfcShapeRepresentation.
     *
     * @param file IFC file that receives the representation.
     * @param options Context, items, and optional representation type.
     * @return IfcShapeRepresentation or IfcTopologyRepresentation entity.
     */
    builderRepresentation(file: IfcFile, options: IfcOpenShellShapeBuilderRepresentationOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.shape.builderRepresentation(file.raw, encodeOptions(options, {"context": "context", "items": "items", "representationType": "representation_type"}, shell, temps, [], ["items"]));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Rotate a geometry item around a 2D pivot point.
     *
     * Supports IfcIndexedPolyCurve, IfcPolyline, IfcCircle, and
     * IfcExtrudedAreaSolid. The angle is in degrees.
     *
     * @param file IFC file containing the item.
     * @param options Item, angle, pivot, direction, and copy flag.
     * @return The rotated item, either the supplied entity or an independent copy.
     */
    builderRotate(file: IfcFile, options: IfcOpenShellShapeBuilderRotateOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.shape.builderRotate(file.raw, encodeOptions(options, {"angle": "angle", "counterClockwise": "counter_clockwise", "createCopy": "create_copy", "item": "item", "pivotPoint": "pivot_point"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Replace the coordinate list of an IfcPolyline or IfcIndexedPolyCurve.
     *
     * For IfcPolyline, the new coordinate count must match the existing point count.
     *
     * @param file IFC file containing the polyline.
     * @param polyline IfcPolyline or IfcIndexedPolyCurve entity.
     * @param coords Replacement coordinates.
     * @return The modified polyline entity.
     */
    builderSetPolylineCoords(file: IfcFile, polyline: Entity, coords: number[][]): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.shape.builderSetPolylineCoords(file.raw, polyline.raw, toRawSequence(coords, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Create an IfcSphere.
     *
     * @param file IFC file that receives the geometry.
     * @param options Radius and centre position.
     * @return IfcSphere entity.
     */
    builderSphere(file: IfcFile, options: IfcOpenShellShapeBuilderSphereOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.shape.builderSphere(file.raw, encodeOptions(options, {"center": "center", "radius": "radius"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Create an IfcSweptDiskSolid along a directrix curve.
     *
     * @param file IFC file that receives the geometry.
     * @param path_curve IfcCurve entity defining the sweep path.
     * @param radius Disk radius in model units.
     * @return IfcSweptDiskSolid entity.
     */
    builderSweptDiskSolid(file: IfcFile, path_curve: Entity, radius: number): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.shape.builderSweptDiskSolid(file.raw, path_curve.raw, radius);
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Translate a geometry item by a direction and distance.
     *
     * Supports IfcIndexedPolyCurve, IfcPolyline, IfcCircle, IfcEllipse,
     * IfcExtrudedAreaSolid, IfcTessellatedFaceSet, IfcShapeRepresentation,
     * and IfcTrimmedCurve.
     *
     * @param file IFC file containing the item.
     * @param options Item, translation, and copy flag.
     * @return The translated item, either the supplied entity or an independent copy.
     */
    builderTranslate(file: IfcFile, options: IfcOpenShellShapeBuilderTranslateOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.shape.builderTranslate(file.raw, encodeOptions(options, {"createCopy": "create_copy", "item": "item", "translation": "translation"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Create an IfcTriangulatedFaceSet from vertices and triangle index lists.
     *
     * @param file IFC file that receives the geometry.
     * @param points XYZ vertex coordinates.
     * @param faces Triangle index lists (zero-based, truncated to 3 vertices each).
     * @return IfcTriangulatedFaceSet entity.
     */
    builderTriangulatedFaceSet(file: IfcFile, points: number[][], faces: number[][]): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.shape.builderTriangulatedFaceSet(file.raw, toRawSequence(points, shell, temps), toRawSequence(faces, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Create an IfcVertexPoint at the given position.
     *
     * @param file IFC file that receives the geometry.
     * @param position XYZ coordinates of the vertex.
     * @return IfcVertexPoint entity.
     */
    builderVertex(file: IfcFile, position: number[]): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.shape.builderVertex(file.raw, toRawSequence(position, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Check whether two floating-point values are approximately equal.
     *
     * Uses strict less-than comparison. When tolerance is 0.0, it defaults
     * to 1e-6.
     *
     * @param value First value.
     * @param x Second value.
     * @param tolerance Maximum absolute difference (exclusive). Defaults to 1e-6 when 0.
     * @return True if |value - x| < tolerance.
     */
    isAlmostEqual(value: number, x: number, tolerance: number): boolean {
      const temps: Disposable[] = [];
      try {
        const result = raw.shape.isAlmostEqual(value, x, tolerance);
        return wrap(shell, result) as boolean;
      } finally {
        disposeAll(temps);
      }
    },
    }),
    spatial: Object.freeze({
    /**
     * Contain products in a spatial structure via IfcRelContainedInSpatialStructure.
     *
     * Products previously contained in a different structure are moved. Products
     * previously aggregated under a different parent (IfcRelAggregates) are
     * removed from that aggregation. If an existing containment relationship
     * already relates to the same structure, products are merged into it.
     */
    assignContainer(file: IfcFile, options: IfcOpenShellSpatialAssignContainerOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.spatial.assignContainer(file.raw, encodeOptions(options, {"application": "application", "ownerHistory": "owner_history", "products": "products", "relatingStructure": "relating_structure", "user": "user"}, shell, temps, [], ["products"]));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Remove products from a spatial structure reference relationship.
     *
     * Removes products from IfcRelReferencedInSpatialStructure relationships
     * for the given structure. If no products remain, the relationship is deleted.
     */
    dereferenceStructure(file: IfcFile, options: IfcOpenShellSpatialDereferenceStructureOptions): void {
      const temps: Disposable[] = [];
      try {
        raw.spatial.dereferenceStructure(file.raw, encodeOptions(options, {"application": "application", "products": "products", "relatingStructure": "relating_structure", "user": "user"}, shell, temps, [], ["products"]));
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Reference products in a spatial structure via IfcRelReferencedInSpatialStructure.
     *
     * Unlike containment, referencing does not establish ownership. Products
     * already referenced by the same structure are skipped.
     */
    referenceStructure(file: IfcFile, options: IfcOpenShellSpatialReferenceStructureOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.spatial.referenceStructure(file.raw, encodeOptions(options, {"application": "application", "ownerHistory": "owner_history", "products": "products", "relatingStructure": "relating_structure", "user": "user"}, shell, temps, [], ["products"]));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Remove products from their IfcRelContainedInSpatialStructure relationships.
     *
     * If no related elements remain after removal, the relationship is deleted.
     */
    unassignContainer(file: IfcFile, options: IfcOpenShellSpatialUnassignContainerOptions): void {
      const temps: Disposable[] = [];
      try {
        raw.spatial.unassignContainer(file.raw, encodeOptions(options, {"application": "application", "products": "products", "user": "user"}, shell, temps, [], ["products"]));
      } finally {
        disposeAll(temps);
      }
    },
    }),
    structural: Object.freeze({
    /**
     * Create an IfcStructuralActivity and connect it to a structural member.
     *
     * Creates the activity entity of the given class and predefined type, sets
     * its AppliedLoad and GlobalOrLocal attributes, and creates an
     * IfcRelConnectsStructuralActivity linking the activity to the structural
     * member.
     *
     * @param file File that receives the new entities.
     * @param applied_load IfcStructuralLoad to apply.
     * @param structural_member Structural member to connect the activity to.
     * @param ifc_class IFC class name (e.g. "IfcStructuralPlanarAction").
     * @param predefined_type Predefined type enum value.
     * @param global_or_local "GLOBAL_COORDS" or "LOCAL_COORDS".
     * @param options Ownership options for the activity and relationship.
     * @return Newly created IfcStructuralActivity.
     */
    addStructuralActivity(file: IfcFile, applied_load: Entity, structural_member: Entity, ifc_class: string, predefined_type: string, global_or_local: string, options: IfcOpenShellStructuralAddStructuralActivityOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.structural.addStructuralActivity(file.raw, applied_load.raw, structural_member.raw, ifc_class, predefined_type, global_or_local, encodeOptions(options, {"activityOwnerHistory": "activity_owner_history", "relationshipOwnerHistory": "relationship_owner_history"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Create an IfcStructuralAnalysisModel with PredefinedType LOADING_3D.
     *
     * @param file File that receives the new entity.
     * @param owner_history Owner history for the new entity. When omitted, no owner history is assigned.
     * @return Newly created IfcStructuralAnalysisModel.
     */
    addStructuralAnalysisModel(file: IfcFile, owner_history: Entity): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.structural.addStructuralAnalysisModel(file.raw, owner_history.raw);
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Create an IfcBoundaryCondition subclass entity.
     *
     * When a connection is provided, the boundary class is inferred:
     * IfcStructuralPointConnection yields IfcBoundaryNodeCondition,
     * IfcStructuralCurveConnection yields IfcBoundaryEdgeCondition,
     * IfcStructuralSurfaceConnection yields IfcBoundaryFaceCondition.
     * The condition is applied to the connection's AppliedCondition attribute.
     *
     * @param file File that receives the new entity.
     * @param ifc_class IFC boundary condition class name. Overridden when connection is provided.
     * @param options Name and connection options.
     * @return Newly created IfcBoundaryCondition subclass.
     */
    addStructuralBoundaryCondition(file: IfcFile, ifc_class: string, options: IfcOpenShellStructuralAddStructuralBoundaryConditionOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.structural.addStructuralBoundaryCondition(file.raw, ifc_class, encodeOptions(options, {"connection": "connection", "name": "name"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Create an IfcStructuralLoad subclass entity.
     *
     * @param file File that receives the new entity.
     * @param ifc_class IFC class name (e.g. "IfcStructuralLoadConfiguration").
     * @param name Optional name for the load entity.
     * @return Newly created IfcStructuralLoad subclass.
     */
    addStructuralLoad(file: IfcFile, ifc_class: string, name: string): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.structural.addStructuralLoad(file.raw, ifc_class, name);
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Create an IfcStructuralLoadCase entity.
     *
     * Creates the load case with PredefinedType LOAD_CASE and the given
     * ActionType and ActionSource enum values.
     *
     * @param file File that receives the new entity.
     * @param name Name for the load case.
     * @param action_type ActionType enum value (e.g. "PERMANENT_G", "VARIABLE_Q").
     * @param action_source ActionSource enum value (e.g. "WIND", "IMPOSED").
     * @param owner_history Owner history for the new entity. When omitted, no owner history is assigned.
     * @return Newly created IfcStructuralLoadCase.
     */
    addStructuralLoadCase(file: IfcFile, name: string, action_type: string, action_source: string, owner_history: Entity): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.structural.addStructuralLoadCase(file.raw, name, action_type, action_source, owner_history.raw);
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Create an IfcStructuralLoadGroup entity.
     *
     * Creates the load group with PredefinedType LOAD_GROUP and the given
     * ActionType and ActionSource enum values.
     *
     * @param file File that receives the new entity.
     * @param name Name for the load group.
     * @param action_type ActionType enum value.
     * @param action_source ActionSource enum value.
     * @param owner_history Owner history for the new entity. When omitted, no owner history is assigned.
     * @return Newly created IfcStructuralLoadGroup.
     */
    addStructuralLoadGroup(file: IfcFile, name: string, action_type: string, action_source: string, owner_history: Entity): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.structural.addStructuralLoadGroup(file.raw, name, action_type, action_source, owner_history.raw);
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Connect a structural member to a structural connection via IfcRelConnectsStructuralMember.
     *
     * If a connection already exists between the two, the existing relationship
     * is returned.
     *
     * @param file File containing both entities.
     * @param relating_structural_member IfcStructuralMember to connect.
     * @param related_structural_connection IfcStructuralConnection to connect to.
     * @param owner_history Owner history for the new relationship. When omitted, no owner history is assigned.
     * @return The IfcRelConnectsStructuralMember relationship.
     */
    addStructuralMemberConnection(file: IfcFile, relating_structural_member: Entity, related_structural_connection: Entity, owner_history: Entity): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.structural.addStructuralMemberConnection(file.raw, relating_structural_member.raw, related_structural_connection.raw, owner_history.raw);
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Assign a structural member or connection to a product via IfcRelAssignsToProduct.
     *
     * If a relationship already exists between the two, the object is appended
     * to the existing relationship's RelatedObjects. Otherwise a new
     * IfcRelAssignsToProduct is created.
     *
     * @param file File containing both entities.
     * @param relating_product IfcProduct that the structural item references.
     * @param related_object Structural item to assign.
     * @param owner_history Owner history for new relationships. When omitted, no owner history is assigned.
     * @return The IfcRelAssignsToProduct relationship.
     */
    assignProduct(file: IfcFile, relating_product: Entity, related_object: Entity, owner_history: Entity): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.structural.assignProduct(file.raw, relating_product.raw, related_object.raw, owner_history.raw);
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Assign products to an IfcStructuralAnalysisModel via IfcRelAssignsToGroup.
     *
     * @param file File containing the products and model.
     * @param products Products to assign to the model.
     * @param structural_analysis_model IfcStructuralAnalysisModel to assign to.
     * @param options Ownership options for the relationship.
     * @return The IfcRelAssignsToGroup relationship.
     */
    assignStructuralAnalysisModel(file: IfcFile, products: Entity[], structural_analysis_model: Entity, options: IfcOpenShellStructuralAssignStructuralAnalysisModelOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.structural.assignStructuralAnalysisModel(file.raw, toRawSequence(products, shell, temps), structural_analysis_model.raw, encodeOptions(options, {"application": "application", "ownerHistory": "owner_history", "user": "user"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Assign an IfcStructuralAnalysisModel to a building via IfcRelServicesBuildings.
     *
     * If a relationship already exists for this model, the building is appended
     * to its RelatedBuildings aggregate.
     *
     * @param file File containing both entities.
     * @param structural_analysis_model IfcStructuralAnalysisModel to assign.
     * @param building IfcBuilding to assign to.
     * @param owner_history Owner history for the new relationship. When omitted, no owner history is assigned.
     * @return The IfcRelServicesBuildings relationship.
     */
    assignToBuilding(file: IfcFile, structural_analysis_model: Entity, building: Entity, owner_history: Entity): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.structural.assignToBuilding(file.raw, structural_analysis_model.raw, building.raw, owner_history.raw);
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Edit attributes of an IfcBoundaryCondition subclass.
     *
     * Each entry in the attributes mapping must contain "type" and
     * "value" sub-entries. The type specifies the IFC typed value class (e.g.
     * "IfcBoolean", "IfcForceMeasure") or "string"/"blank" for direct values.
     *
     * @param file File containing the boundary condition.
     * @param condition IfcBoundaryCondition entity to edit.
     * @param attributes Property bag of typed attribute entries.
     */
    editStructuralBoundaryCondition(file: IfcFile, condition: Entity, attributes: PsetProperties | PsetInput): void {
      const temps: Disposable[] = [];
      try {
        raw.structural.editStructuralBoundaryCondition(file.raw, condition.raw, toRawPsetProperties(shell, attributes as PsetProperties | PsetInput, temps));
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Edit the ConditionCoordinateSystem of a structural item.
     *
     * Replaces the Axis and RefDirection directions on the item's
     * ConditionCoordinateSystem (IfcAxis2Placement3D). Creates the placement
     * if absent. Old direction entities are deleted if unreferenced.
     *
     * @param file File containing the structural item.
     * @param structural_item Structural item (e.g. IfcStructuralPointConnection).
     * @param axis 3-element direction ratios for the Axis attribute.
     * @param ref_direction 3-element direction ratios for the RefDirection attribute.
     */
    editStructuralConnectionCs(file: IfcFile, structural_item: Entity, axis: number[], ref_direction: number[]): void {
      const temps: Disposable[] = [];
      try {
        raw.structural.editStructuralConnectionCs(file.raw, structural_item.raw, toRawSequence(axis, shell, temps), toRawSequence(ref_direction, shell, temps));
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Edit the Axis direction of a structural item.
     *
     * Replaces the Axis direction on the structural item (e.g.
     * IfcStructuralCurveMember). The old direction entity is deleted if
     * unreferenced.
     *
     * @param file File containing the structural item.
     * @param structural_item Structural item with an Axis attribute.
     * @param axis 3-element direction ratios.
     */
    editStructuralItemAxis(file: IfcFile, structural_item: Entity, axis: number[]): void {
      const temps: Disposable[] = [];
      try {
        raw.structural.editStructuralItemAxis(file.raw, structural_item.raw, toRawSequence(axis, shell, temps));
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Remove an IfcStructuralAnalysisModel from the file.
     *
     * Removes the model's IfcRelAssignsToGroup relationships and then the
     * model entity itself.
     *
     * @param file File containing the model.
     * @param structural_analysis_model IfcStructuralAnalysisModel to remove.
     */
    removeStructuralAnalysisModel(file: IfcFile, structural_analysis_model: Entity): void {
      const temps: Disposable[] = [];
      try {
        raw.structural.removeStructuralAnalysisModel(file.raw, structural_analysis_model.raw);
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Remove a boundary condition from a structural connection.
     *
     * When connection is provided, clears and deletes its AppliedCondition.
     * When boundary_condition is provided instead, clears AppliedCondition on
     * all referencing entities and deletes the condition. At least one of
     * connection or boundary_condition must be provided.
     *
     * @param file File containing the entities.
     * @param options Connection or boundary condition to remove.
     */
    removeStructuralBoundaryCondition(file: IfcFile, options: IfcOpenShellStructuralRemoveStructuralBoundaryConditionOptions): void {
      const temps: Disposable[] = [];
      try {
        raw.structural.removeStructuralBoundaryCondition(file.raw, encodeOptions(options, {"boundaryCondition": "boundary_condition", "connection": "connection"}, shell, temps));
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Remove an IfcRelConnectsStructuralMember and its AppliedCondition.
     *
     * Removes the boundary condition from the related structural connection
     * (if any), then removes the relationship and its OwnerHistory.
     *
     * @param file File containing the relationship.
     * @param relation IfcRelConnectsStructuralMember to remove.
     */
    removeStructuralConnectionCondition(file: IfcFile, relation: Entity): void {
      const temps: Disposable[] = [];
      try {
        raw.structural.removeStructuralConnectionCondition(file.raw, relation.raw);
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Remove an IfcStructuralLoad subclass from the file.
     *
     * @param file File containing the load.
     * @param structural_load IfcStructuralLoad entity to remove.
     */
    removeStructuralLoad(file: IfcFile, structural_load: Entity): void {
      const temps: Disposable[] = [];
      try {
        raw.structural.removeStructuralLoad(file.raw, structural_load.raw);
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Remove an IfcStructuralLoadCase from the file.
     *
     * Removes the load case's IfcRelAssignsToGroup relationships and then the
     * entity itself.
     *
     * @param file File containing the load case.
     * @param structural_load_case IfcStructuralLoadCase to remove.
     */
    removeStructuralLoadCase(file: IfcFile, structural_load_case: Entity): void {
      const temps: Disposable[] = [];
      try {
        raw.structural.removeStructuralLoadCase(file.raw, structural_load_case.raw);
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Remove an IfcStructuralLoadGroup from the file.
     *
     * Removes singleton IfcRelAssignsToGroup relationships and then the entity
     * itself.
     *
     * @param file File containing the load group.
     * @param structural_load_group IfcStructuralLoadGroup to remove.
     */
    removeStructuralLoadGroup(file: IfcFile, structural_load_group: Entity): void {
      const temps: Disposable[] = [];
      try {
        raw.structural.removeStructuralLoadGroup(file.raw, structural_load_group.raw);
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Remove products from an IfcStructuralAnalysisModel's group.
     *
     * @param file File containing the products and model.
     * @param products Products to unassign from the model.
     * @param structural_analysis_model IfcStructuralAnalysisModel to unassign from.
     * @param options Ownership options for updating the relationship.
     */
    unassignStructuralAnalysisModel(file: IfcFile, products: Entity[], structural_analysis_model: Entity, options: IfcOpenShellStructuralUnassignStructuralAnalysisModelOptions): void {
      const temps: Disposable[] = [];
      try {
        raw.structural.unassignStructuralAnalysisModel(file.raw, toRawSequence(products, shell, temps), structural_analysis_model.raw, encodeOptions(options, {"application": "application", "user": "user"}, shell, temps));
      } finally {
        disposeAll(temps);
      }
    },
    }),
    style: Object.freeze({
    /**
     * Create a new presentation style entity.
     *
     * For IfcSurfaceStyle, the Side attribute defaults to "BOTH".
     *
     * @param file IFC file that receives the style.
     * @param name Style name. When omitted, the style is unnamed.
     * @param ifc_class IFC entity class (e.g. "IfcSurfaceStyle", "IfcFillAreaStyle").
     * @return Newly created style entity.
     */
    addStyle(file: IfcFile, name: string, ifc_class: string): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.style.addStyle(file.raw, name, ifc_class);
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Assign or replace a style on a single representation item.
     *
     * Creates an IfcStyledItem (and optionally an IfcPresentationStyleAssignment
     * for IFC2X3) linking the item to the given style. If the item already has a
     * styled item, the existing style is replaced. When style is omitted, the
     * styled item is removed from the representation item.
     *
     * @param file IFC file to modify.
     * @param options Item, style, and IFC2X3 compat flag.
     * @return The IfcStyledItem, or no result when the style is removed.
     */
    assignItemStyle(file: IfcFile, options: IfcOpenShellStyleAssignItemStyleOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.style.assignItemStyle(file.raw, encodeOptions(options, {"item": "item", "shouldUsePresentationStyleAssignment": "should_use_presentation_style_assignment", "style": "style"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Assign a style to a material via IfcMaterialDefinitionRepresentation.
     *
     * Creates or extends a styled representation under the material's definition.
     * Also propagates the style to shape aspects whose names match material
     * constituent names.
     *
     * @param file IFC file to modify.
     * @param material IfcMaterial entity.
     * @param style Presentation style entity to assign.
     * @param context IfcGeometricRepresentationContext for the styled representation.
     * @param should_use_presentation_style_assignment Wrap style in IfcPresentationStyleAssignment.
     */
    assignMaterialStyle(file: IfcFile, material: Entity, style: Entity, context: Entity, should_use_presentation_style_assignment: boolean): void {
      const temps: Disposable[] = [];
      try {
        raw.style.assignMaterialStyle(file.raw, material.raw, style.raw, context.raw, should_use_presentation_style_assignment);
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Assign styles to the geometric items within a shape representation.
     *
     * Assigns the styles to representation items in sequence. When
     * replace_previous_same_type_style is true, an existing style of the same IFC
     * class is replaced instead of appended.
     *
     * @param file IFC file to modify.
     * @param shape_representation IfcShapeRepresentation to assign styles to.
     * @param styles Presentation style entities to assign.
     * @param should_use_presentation_style_assignment Wrap styles in IfcPresentationStyleAssignment.
     * @param replace_previous_same_type_style Replace existing styles of the same type.
     * @return List of newly created IfcStyledItem entities.
     */
    assignRepresentationStyles(file: IfcFile, shape_representation: Entity, styles: Entity[], should_use_presentation_style_assignment: boolean, replace_previous_same_type_style: boolean): Entity[] {
      const temps: Disposable[] = [];
      try {
        const result = raw.style.assignRepresentationStyles(file.raw, shape_representation.raw, toRawSequence(styles, shell, temps), should_use_presentation_style_assignment, replace_previous_same_type_style);
        return wrapEntities(shell, result as never) as Entity[];
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Edit attributes of an IfcSurfaceStyle or its nested surface styles.
     *
     * Handles colour attributes (IfcColourRgb), specular highlights, and
     * colour-or-factor properties specially; other attributes are set directly.
     *
     * @param file IFC file containing the style.
     * @param style IfcSurfaceStyle entity to modify.
     * @param attributes Attribute name-to-value mapping.
     */
    editSurfaceStyle(file: IfcFile, style: Entity, attributes: PsetProperties | PsetInput): void {
      const temps: Disposable[] = [];
      try {
        raw.style.editSurfaceStyle(file.raw, style.raw, toRawPsetProperties(shell, attributes as PsetProperties | PsetInput, temps));
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Remove a presentation style and clean up all references to it.
     *
     * Removes inverse IfcStyledItem references, nested surface styles (for
     * IfcSurfaceStyle), fill area style hatching, and orphaned
     * IfcPresentationStyleAssignment entities.
     *
     * @param file IFC file to modify.
     * @param style Presentation style entity to remove.
     */
    removeStyle(file: IfcFile, style: Entity): void {
      const temps: Disposable[] = [];
      try {
        raw.style.removeStyle(file.raw, style.raw);
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Remove an IfcStyledRepresentation and its parent IfcMaterialDefinitionRepresentation.
     *
     * @param file IFC file to modify.
     * @param representation IfcStyledRepresentation entity to remove.
     */
    removeStyledRepresentation(file: IfcFile, representation: Entity): void {
      const temps: Disposable[] = [];
      try {
        raw.style.removeStyledRepresentation(file.raw, representation.raw);
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Remove an IfcSurfaceStyleWithTextures or IfcSurfaceStyleRendering and its nested entities.
     *
     * Removes texture coordinates, textures, and colour entities belonging to the
     * surface style when they are no longer referenced.
     *
     * @param file IFC file to modify.
     * @param style Surface style sub-entity to remove.
     */
    removeSurfaceStyle(file: IfcFile, style: Entity): void {
      const temps: Disposable[] = [];
      try {
        raw.style.removeSurfaceStyle(file.raw, style.raw);
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Remove a style from a material's styled representation.
     *
     * Removes empty IfcStyledItem, IfcStyledRepresentation, and
     * IfcMaterialDefinitionRepresentation entities, and propagates the removal to
     * matching shape aspects.
     *
     * @param file IFC file to modify.
     * @param material IfcMaterial entity.
     * @param style Presentation style entity to remove.
     * @param context IfcGeometricRepresentationContext identifying the styled representation.
     */
    unassignMaterialStyle(file: IfcFile, material: Entity, style: Entity, context: Entity): void {
      const temps: Disposable[] = [];
      try {
        raw.style.unassignMaterialStyle(file.raw, material.raw, style.raw, context.raw);
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Remove styles from the geometric items within a shape representation.
     *
     * Removes matching styles from IfcStyledItem and
     * IfcPresentationStyleAssignment entities in the representation.
     *
     * @param file IFC file to modify.
     * @param shape_representation IfcShapeRepresentation to unassign styles from.
     * @param styles Presentation style entities to remove.
     * @param should_use_presentation_style_assignment Also search inside IfcPresentationStyleAssignment.
     */
    unassignRepresentationStyles(file: IfcFile, shape_representation: Entity, styles: Entity[], should_use_presentation_style_assignment: boolean): void {
      const temps: Disposable[] = [];
      try {
        raw.style.unassignRepresentationStyles(file.raw, shape_representation.raw, toRawSequence(styles, shell, temps), should_use_presentation_style_assignment);
      } finally {
        disposeAll(temps);
      }
    },
    }),
    system: Object.freeze({
    /**
     * Create a new IfcDistributionPort and optionally assign it to an element.
     *
     * If an element is provided, the port is assigned to it via IfcRelNests
     * (IFC4+) or IfcRelConnectsPortToElement (IFC2X3).
     */
    addPort(file: IfcFile, options: IfcOpenShellSystemAddPortOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.system.addPort(file.raw, encodeOptions(options, {"application": "application", "element": "element", "ownerHistory": "owner_history", "user": "user"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Create a new system with the given IFC class.
     *
     * In IFC2X3, "IfcDistributionSystem" is automatically resolved to "IfcSystem".
     * The system is created with Name set to "Unnamed".
     */
    addSystem(file: IfcFile, options: IfcOpenShellSystemAddSystemOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.system.addSystem(file.raw, encodeOptions(options, {"ifcClass": "ifc_class", "ownerHistory": "owner_history"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Assign a flow control element to a flow element via IfcRelFlowControlElements.
     *
     * If the flow control is already assigned to a different element, no change
     * is made and no relationship is returned.
     */
    assignFlowControl(file: IfcFile, options: IfcOpenShellSystemAssignFlowControlOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.system.assignFlowControl(file.raw, encodeOptions(options, {"application": "application", "ownerHistory": "owner_history", "relatedFlowControl": "related_flow_control", "relatingFlowElement": "relating_flow_element", "user": "user"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Assign a port to an element.
     *
     * In IFC2X3, uses IfcRelConnectsPortToElement. In IFC4+, uses IfcRelNests
     * to nest the port under the element. Updates the port's ObjectPlacement
     * if it has an IfcLocalPlacement.
     */
    assignPort(file: IfcFile, options: IfcOpenShellSystemAssignPortOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.system.assignPort(file.raw, encodeOptions(options, {"application": "application", "element": "element", "ownerHistory": "owner_history", "port": "port", "user": "user"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Assign products to a system via IfcRelAssignsToGroup.
     *
     * Validates that each product is compatible with the system type (e.g.
     * only IfcDistributionElement for IfcDistributionSystem). The operation fails
     * if a product is not valid for the system type.
     */
    assignSystem(file: IfcFile, options: IfcOpenShellSystemAssignSystemOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.system.assignSystem(file.raw, encodeOptions(options, {"application": "application", "ownerHistory": "owner_history", "products": "products", "system": "system", "user": "user"}, shell, temps, [], ["products"]));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Connect two ports together with a given flow direction.
     *
     * Creates IfcRelConnectsPorts relationships and sets FlowDirection on both
     * ports. Existing connections to other ports are removed first. The
     * direction parameter controls which port is SOURCE, SINK, or both.
     */
    connectPort(file: IfcFile, options: IfcOpenShellSystemConnectPortOptions): void {
      const temps: Disposable[] = [];
      try {
        raw.system.connectPort(file.raw, encodeOptions(options, {"application": "application", "direction": "direction", "element": "element", "ownerHistory": "owner_history", "port1": "port1", "port2": "port2", "user": "user"}, shell, temps));
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Disconnect a port from all connected ports.
     *
     * Removes all IfcRelConnectsPorts relationships involving the port and
     * clears FlowDirection on both sides.
     */
    disconnectPort(file: IfcFile, port: Entity): void {
      const temps: Disposable[] = [];
      try {
        raw.system.disconnectPort(file.raw, port.raw);
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Remove a system and its relationships.
     *
     * Removes associated IfcRelDefinesByProperties (property sets) and
     * IfcRelAssignsToGroup relationships, then deletes the system entity.
     */
    removeSystem(file: IfcFile, system: Entity): void {
      const temps: Disposable[] = [];
      try {
        raw.system.removeSystem(file.raw, system.raw);
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Remove a flow control element from a flow element's IfcRelFlowControlElements.
     *
     * If the control element is the only one in the relationship, the
     * relationship is deleted. Otherwise, the control element is removed from
     * the RelatedControlElements aggregate.
     */
    unassignFlowControl(file: IfcFile, options: IfcOpenShellSystemUnassignFlowControlOptions): void {
      const temps: Disposable[] = [];
      try {
        raw.system.unassignFlowControl(file.raw, encodeOptions(options, {"application": "application", "relatedFlowControl": "related_flow_control", "relatingFlowElement": "relating_flow_element", "user": "user"}, shell, temps));
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Remove a port from an element.
     *
     * In IFC2X3, removes the IfcRelConnectsPortToElement. In IFC4+, removes
     * the port from the IfcRelNests relationship (or deletes the relationship
     * if it was the only nested object).
     */
    unassignPort(file: IfcFile, options: IfcOpenShellSystemUnassignPortOptions): void {
      const temps: Disposable[] = [];
      try {
        raw.system.unassignPort(file.raw, encodeOptions(options, {"application": "application", "element": "element", "port": "port", "user": "user"}, shell, temps));
      } finally {
        disposeAll(temps);
      }
    },
    /** Remove products from a system. */
    unassignSystem(file: IfcFile, options: IfcOpenShellSystemUnassignSystemOptions): void {
      const temps: Disposable[] = [];
      try {
        raw.system.unassignSystem(file.raw, encodeOptions(options, {"application": "application", "products": "products", "system": "system", "user": "user"}, shell, temps, [], ["products"]));
      } finally {
        disposeAll(temps);
      }
    },
    }),
    type: Object.freeze({
    /**
     * Assign a type to element occurrences via IfcRelDefinesByType.
     *
     * Objects previously typed by a different type are moved. If an existing
     * IfcRelDefinesByType relationship already relates to the same type,
     * objects are merged into it. When should_map_representations is true
     * (default), IfcRepresentationMaps from the type are propagated onto
     * each occurrence and PredefinedType/ObjectType are cleared to avoid
     * double-typing.
     */
    assignType(file: IfcFile, options: IfcOpenShellTypeAssignTypeOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.type.assignType(file.raw, encodeOptions(options, {"application": "application", "objects": "objects", "ownerHistory": "owner_history", "relatingType": "relating_type", "shouldMapRepresentations": "should_map_representations", "user": "user"}, shell, temps, [], ["objects"]));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Map IfcRepresentationMaps from a type onto a related object.
     *
     * Clones mapped representations from the type's RepresentationMaps and
     * assigns them to the object. Returns true if any representations were
     * mapped.
     */
    mapTypeRepresentations(file: IfcFile, related_object: Entity, relating_type: Entity): boolean {
      const temps: Disposable[] = [];
      try {
        const result = raw.type.mapTypeRepresentations(file.raw, related_object.raw, relating_type.raw);
        return wrap(shell, result) as boolean;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Remove type assignments from element occurrences.
     *
     * Removes objects from IfcRelDefinesByType relationships. If no objects
     * remain in a relationship, the relationship is deleted.
     */
    unassignType(file: IfcFile, options: IfcOpenShellTypeUnassignTypeOptions): void {
      const temps: Disposable[] = [];
      try {
        raw.type.unassignType(file.raw, encodeOptions(options, {"application": "application", "objects": "objects", "user": "user"}, shell, temps, [], ["objects"]));
      } finally {
        disposeAll(temps);
      }
    },
    }),
    unit: Object.freeze({
    /**
     * Create an IfcContextDependentUnit entity.
     *
     * Creates a unit with custom dimensional exponents, unit type, and name.
     *
     * @param file File that receives the new entity.
     * @param unit_type IFC unit type enum value (e.g. "LENGTHUNIT").
     * @param name Display name for the unit (e.g. "bag", "each").
     * @param dimensions 7-element sequence of dimensional exponents.
     * @return Newly created IfcContextDependentUnit.
     */
    addContextDependentUnit(file: IfcFile, unit_type: string, name: string, dimensions: bigint[]): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.unit.addContextDependentUnit(file.raw, unit_type, name, toRawSequence(dimensions, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Create an IfcDerivedUnit entity.
     *
     * Constructs a derived unit from a list of component units and their
     * exponents (e.g. m/s from ["METRE", "SECOND"] with exponents [1, -1]).
     *
     * @param file File that receives the new entity.
     * @param unit_type IFC unit type enum value (e.g. "VELOCITYUNIT").
     * @param userdefinedtype UserDefinedType string. When omitted, it is left blank.
     * @param units Component IfcUnit entities.
     * @param exponents Exponent for each component unit (must match units in length).
     * @return Newly created IfcDerivedUnit.
     */
    addDerivedUnit(file: IfcFile, unit_type: string, userdefinedtype: string, units: Entity[], exponents: bigint[]): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.unit.addDerivedUnit(file.raw, unit_type, userdefinedtype, toRawSequence(units, shell, temps), toRawSequence(exponents, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Create an IfcMonetaryUnit entity.
     *
     * @param file File that receives the new entity.
     * @param currency ISO 4217 currency code (e.g. "USD", "EUR").
     * @return Newly created IfcMonetaryUnit.
     */
    addMonetaryUnit(file: IfcFile, currency: string): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.unit.addMonetaryUnit(file.raw, currency);
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Create an IfcSIUnit entity.
     *
     * Sets the UnitType, Name (derived from the unit type), and optional
     * Prefix. When prefix is omitted, the Prefix attribute is left blank.
     *
     * @param file File that receives the new entity.
     * @param unit_type IFC unit type enum value (e.g. "LENGTHUNIT").
     * @param prefix SI prefix (e.g. "KILO", "MILLI"). When omitted, the base unit is used.
     * @return Newly created IfcSIUnit.
     */
    addSiUnit(file: IfcFile, unit_type: string, prefix: string): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.unit.addSiUnit(file.raw, unit_type, prefix);
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Calculate the scale factor from SI for a project unit type.
     *
     * Returns the multiplier needed to convert from the project's unit to SI
     * base units. For example, if the project length unit is millimetres,
     * returns 0.001. Defaults to 1.0 when no unit is found.
     *
     * @param file File to query for the project unit.
     * @param unit_type IFC unit type (e.g. "LENGTHUNIT"). Defaults to "LENGTHUNIT" when empty.
     * @return Scale factor to SI.
     */
    calculateUnitScale(file: IfcFile, unit_type: string): number {
      const temps: Disposable[] = [];
      try {
        const result = raw.unit.calculateUnitScale(file.raw, unit_type);
        return wrap(shell, result) as number;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Convert a numeric value between units specified by prefix and name strings.
     *
     * Supports SI conversions and prefix-aware scaling. For example,
     * converting from ("KILO", "METRE") to ("", "METRE") multiplies by 1000.
     *
     * @param value Numeric value to convert.
     * @param from_prefix Source SI prefix (e.g. "KILO"). Empty for base unit.
     * @param from_unit Source unit name (e.g. "METRE", "GRAM").
     * @param to_prefix Target SI prefix.
     * @param to_unit Target unit name.
     * @return The converted value.
     */
    convert(value: number, from_prefix: string, from_unit: string, to_prefix: string, to_unit: string): number {
      const temps: Disposable[] = [];
      try {
        const result = raw.unit.convert(value, from_prefix, from_unit, to_prefix, to_unit);
        return wrap(shell, result) as number;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Convert a numeric value between two IFC unit entities.
     *
     * Reads the Prefix and Name attributes from each unit to perform the
     * conversion.
     *
     * @param value Numeric value to convert.
     * @param from_unit Source IfcUnit entity.
     * @param to_unit Target IfcUnit entity.
     * @return The converted value.
     */
    convertUnit(value: number, from_unit: Entity, to_unit: Entity): number {
      const temps: Disposable[] = [];
      try {
        const result = raw.unit.convertUnit(value, from_unit.raw, to_unit.raw);
        return wrap(shell, result) as number;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Format a length value as an imperial or metric string.
     *
     * For imperial output: returns feet-inches notation with fractional
     * precision (e.g. "5' - 3 1/4\""). For metric: returns a rounded
     * decimal value.
     *
     * @param value Length value to format.
     * @param precision Fractional precision denominator for imperial (e.g. 16 for 1/16").
     * @param decimal_places Number of decimal places for metric output.
     * @param suppress_zero_inches When true, omits zero inches in imperial output.
     * @param unit_system "imperial" or "metric".
     * @param input_unit Source unit: "foot" or "inch". Defaults to "foot" when empty.
     * @param output_unit Target unit: "foot" or "inch". Defaults to "foot" when empty.
     * @return Formatted length string.
     */
    formatLength(value: number, precision: number, decimal_places: number, suppress_zero_inches: boolean, unit_system: string, input_unit: string, output_unit: string): string {
      const temps: Disposable[] = [];
      try {
        const result = raw.unit.formatLength(value, precision, decimal_places, suppress_zero_inches, unit_system, input_unit, output_unit);
        return wrap(shell, result) as string;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Return the full name of an IfcSIUnit (prefix + name).
     *
     * For example, a unit with Prefix "KILO" and Name "METRE" returns
     * "KILOMETRE".
     *
     * @param unit IfcSIUnit entity.
     * @return Concatenated prefix and name string.
     */
    getFullUnitName(unit: Entity): string {
      const temps: Disposable[] = [];
      try {
        const result = raw.unit.getFullUnitName(unit.raw);
        return wrap(shell, result) as string;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Derive the IFC measure class name from a unit type string.
     *
     * For example, "LENGTHUNIT" returns "IfcLengthMeasure",
     * "USERDEFINED" returns "IfcNumericMeasure". Returns an empty string
     * for unrecognised input.
     *
     * @param unit_type IFC unit type enum value (e.g. "LENGTHUNIT").
     * @return The corresponding IFC measure class name.
     */
    getMeasureClass(unit_type: string): string {
      const temps: Disposable[] = [];
      try {
        const result = raw.unit.getMeasureClass(unit_type);
        return wrap(shell, result) as string;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Derive the IFC unit type string from a measure class name.
     *
     * The inverse of unit_get_measure_class. For example,
     * "IfcLengthMeasure" returns "LENGTHUNIT".
     *
     * @param measure_class IFC measure class name.
     * @return The corresponding IFC unit type, or empty string if unrecognised.
     */
    getMeasureUnitType(measure_class: string): string {
      const temps: Disposable[] = [];
      try {
        const result = raw.unit.getMeasureUnitType(measure_class);
        return wrap(shell, result) as string;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Return the named dimensional exponents for a given unit type name.
     *
     * Like unit_get_si_dimensions but returns all zeros for unknown types
     * instead of falling back.
     *
     * @param name Unit type name.
     * @return 7-element sequence of dimensional exponents.
     */
    getNamedDimensions(name: string): number[] {
      const temps: Disposable[] = [];
      try {
        const result = raw.unit.getNamedDimensions(name);
        return wrap(shell, result) as number[];
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Extract the SI prefix from a combined unit text string.
     *
     * For example, "KILOMETRE" returns "KILO", "MILLIGRAM" returns "MILLI".
     * Returns an empty string if no prefix is found.
     *
     * @param text Combined prefix and unit name string.
     * @return The prefix portion, or empty string if none.
     */
    getPrefix(text: string): string {
      const temps: Disposable[] = [];
      try {
        const result = raw.unit.getPrefix(text);
        return wrap(shell, result) as string;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Return the numeric multiplier for an SI prefix.
     *
     * For example, "KILO" returns 1000.0, "MILLI" returns 0.001.
     * Returns 1.0 if no prefix is found.
     *
     * @param text SI prefix string (e.g. "KILO", "MILLI").
     * @return The multiplier value.
     */
    getPrefixMultiplier(text: string): number {
      const temps: Disposable[] = [];
      try {
        const result = raw.unit.getPrefixMultiplier(text);
        return wrap(shell, result) as number;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Return the project unit for a given unit type.
     *
     * Searches the project's IfcUnitAssignment for a unit whose UnitType
     * matches the given string.
     *
     * @param file File to query.
     * @param unit_type IFC unit type enum value (e.g. "LENGTHUNIT").
     * @return The matching unit entity, or no result if it is not found.
     */
    getProjectUnit(file: IfcFile, unit_type: string): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.unit.getProjectUnit(file.raw, unit_type);
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Return the SI dimensional exponents for a given unit type name.
     *
     * Returns a 7-element sequence of integers corresponding to the
     * IfcDimensionalExponents attributes: Length, Mass, Time,
     * ElectricCurrent, ThermodynamicTemperature, AmountOfSubstance,
     * LuminousIntensity. Falls back to the "OTHERWISE" entry for
     * unknown types.
     *
     * @param name Unit type name (e.g. "LENGTHUNIT", "MASSUNIT").
     * @return 7-element sequence of dimensional exponents.
     */
    getSiDimensions(name: string): number[] {
      const temps: Disposable[] = [];
      try {
        const result = raw.unit.getSiDimensions(name);
        return wrap(shell, result) as number[];
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Infer the IFC measure class from a unit symbol string.
     *
     * For example, "m" returns "IfcLengthMeasure", "m2" returns "IfcAreaMeasure",
     * "kg" returns "IfcMassMeasure". Returns "IfcNumericMeasure" for unknown symbols.
     *
     * @param symbol Unit symbol (e.g. "m", "ft", "kg", "hr").
     * @return The corresponding IFC measure class name.
     */
    getSymbolMeasureClass(symbol: string): string {
      const temps: Disposable[] = [];
      try {
        const result = raw.unit.getSymbolMeasureClass(symbol);
        return wrap(shell, result) as string;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Infer the IFC quantity class from a unit symbol string.
     *
     * For example, "m" returns "IfcQuantityLength", "m2" returns "IfcQuantityArea",
     * "kg" returns "IfcQuantityWeight". Returns "IfcQuantityCount" for unknown symbols.
     *
     * @param symbol Unit symbol (e.g. "m", "ft", "kg", "hr").
     * @return The corresponding IFC quantity class name.
     */
    getSymbolQuantityClass(symbol: string): string {
      const temps: Disposable[] = [];
      try {
        const result = raw.unit.getSymbolQuantityClass(symbol);
        return wrap(shell, result) as string;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Return the IfcUnitAssignment entity for the project.
     *
     * @param file File to query.
     * @return The IfcUnitAssignment entity, or no result if it is not found.
     */
    getUnitAssignment(file: IfcFile): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.unit.getUnitAssignment(file.raw);
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Extract the base unit name from a combined unit text string.
     *
     * Normalises "METER" to "METRE" before matching. For example,
     * "KILOMETRE" returns "METRE", "SQUARE_METRE" returns "SQUARE_METRE".
     * Returns an empty string if no unit name is found.
     *
     * @param text Combined prefix and unit name string.
     * @return The base unit name, or empty string if not recognised.
     */
    getUnitName(text: string): string {
      const temps: Disposable[] = [];
      try {
        const result = raw.unit.getUnitName(text);
        return wrap(shell, result) as string;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Extract the base unit name, accepting both metric and imperial names.
     *
     * Like unit_get_unit_name but also recognises imperial unit names
     * (e.g. "FOOT", "INCH"). Returns an empty string if not recognised.
     *
     * @param text Unit name string.
     * @return The base unit name, or empty string if not recognised.
     */
    getUnitNameUniversal(text: string): string {
      const temps: Disposable[] = [];
      try {
        const result = raw.unit.getUnitNameUniversal(text);
        return wrap(shell, result) as string;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Return the display symbol for a unit entity.
     *
     * For IfcSIUnit: prefix symbol + unit symbol (e.g. "km").
     * For IfcContextDependentUnit with USERDEFINED UnitType: the Name attribute.
     * Returns "?" for unrecognised units.
     *
     * @param unit IfcUnit entity (IfcSIUnit, IfcContextDependentUnit, etc.).
     * @return Symbol string.
     */
    getUnitSymbol(unit: Entity): string {
      const temps: Disposable[] = [];
      try {
        const result = raw.unit.getUnitSymbol(unit.raw);
        return wrap(shell, result) as string;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Remove a unit entity from the file.
     *
     * Removes the unit from the project's IfcUnitAssignment (if present) and
     * then deletes the unit entity (and its dimensional exponents, if owned).
     *
     * @param file File containing the unit.
     * @param unit IfcUnit entity to remove.
     */
    removeUnit(file: IfcFile, unit: Entity): void {
      const temps: Disposable[] = [];
      try {
        raw.unit.removeUnit(file.raw, unit.raw);
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Resolve the IFC measure class for a property or quantity.
     *
     * Returns the measure class name (e.g. "IfcLengthMeasure") derived from
     * the property's NominalValue or quantity type. Returns an empty string
     * if a unit entity is directly attached (use unit_resolve_property_unit
     * instead).
     *
     * @param prop IfcProperty or IfcPhysicalQuantity entity.
     * @return Measure class name, or empty string if a unit entity is attached.
     */
    resolvePropertyMeasureClass(prop: Entity): string {
      const temps: Disposable[] = [];
      try {
        const result = raw.unit.resolvePropertyMeasureClass(prop.raw);
        return wrap(shell, result) as string;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Resolve the defined measure class of an IfcPropertyTableValue.
     *
     * Returns the measure class name derived from the DefinedValues.
     * Returns an empty string if a DefinedUnit entity is directly attached.
     *
     * @param prop IfcPropertyTableValue entity.
     * @return Measure class name, or empty string if a unit entity is attached.
     */
    resolvePropertyTableDefinedMeasureClass(prop: Entity): string {
      const temps: Disposable[] = [];
      try {
        const result = raw.unit.resolvePropertyTableDefinedMeasureClass(prop.raw);
        return wrap(shell, result) as string;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Resolve the defined unit of an IfcPropertyTableValue.
     *
     * Returns the DefinedUnit attribute, or no result if the unit must be
     * inferred from the DefinedValues measure class.
     *
     * @param prop IfcPropertyTableValue entity.
     * @return The DefinedUnit entity, or no result.
     */
    resolvePropertyTableDefinedUnit(prop: Entity): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.unit.resolvePropertyTableDefinedUnit(prop.raw);
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Resolve the defining measure class of an IfcPropertyTableValue.
     *
     * Returns the measure class name derived from the DefiningValues.
     * Returns an empty string if a DefiningUnit entity is directly attached.
     *
     * @param prop IfcPropertyTableValue entity.
     * @return Measure class name, or empty string if a unit entity is attached.
     */
    resolvePropertyTableDefiningMeasureClass(prop: Entity): string {
      const temps: Disposable[] = [];
      try {
        const result = raw.unit.resolvePropertyTableDefiningMeasureClass(prop.raw);
        return wrap(shell, result) as string;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Resolve the defining unit of an IfcPropertyTableValue.
     *
     * Returns the DefiningUnit attribute, or no result if the unit must be
     * inferred from the DefiningValues measure class.
     *
     * @param prop IfcPropertyTableValue entity.
     * @return The DefiningUnit entity, or no result.
     */
    resolvePropertyTableDefiningUnit(prop: Entity): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.unit.resolvePropertyTableDefiningUnit(prop.raw);
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Resolve the unit entity attached to a property or quantity.
     *
     * Returns the Unit attribute directly attached to the property (for
     * IfcPropertySingleValue, IfcPhysicalSimpleQuantity, etc.). Returns no result
     * if the unit must be inferred from the measure class.
     *
     * @param prop IfcProperty or IfcPhysicalQuantity entity.
     * @return The attached IfcUnit, or no result if none is attached.
     */
    resolvePropertyUnit(prop: Entity): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.unit.resolvePropertyUnit(prop.raw);
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /**
     * Remove units from the project's IfcUnitAssignment.
     *
     * Removes the given units from the IfcUnitAssignment's Units aggregate.
     * If the aggregate becomes empty, the IfcUnitAssignment entity itself is
     * deleted. The unit entities are not deleted.
     *
     * @param file File containing the unit assignment.
     * @param units Units to unassign.
     */
    unassignUnit(file: IfcFile, units: Entity[]): void {
      const temps: Disposable[] = [];
      try {
        raw.unit.unassignUnit(file.raw, toRawSequence(units, shell, temps));
      } finally {
        disposeAll(temps);
      }
    },
    }),
  });
}

function encodeOptions(
  value: object,
  fields: Record<string, string>,
  shell: IfcOpenShell,
  temps: Disposable[],
  psetFields?: string[],
  entityListFields?: string[],
): Record<string, RawValue> {
  const data = value as Record<string, ApiInput | undefined>;
  const psetFieldSet = psetFields ? new Set(psetFields) : undefined;
  const entityListFieldSet = entityListFields ? new Set(entityListFields) : undefined;
  return Object.fromEntries(
    Object.entries(fields)
      .filter(([publicName]) => data[publicName] !== undefined)
      .map(([publicName, nativeName]) => [nativeName, encodeOptionValue(publicName, data[publicName] as ApiInput, shell, temps, psetFieldSet, entityListFieldSet)]),
  ) as Record<string, RawValue>;
}

function toRawSequence(value: ApiInput, shell: IfcOpenShell, temps: Disposable[]): RawValue {
  if (value === null) return null;
  if (Array.isArray(value)) return value.map((item) => toRawSequence(item, shell, temps));
  return toRaw(value, shell, temps);
}

function toRaw(value: ApiInput, shell: IfcOpenShell, temps: Disposable[]): RawValue {
  if (value instanceof IfcFile) return value.raw;
  if (value instanceof Entity) return value.raw;
  if (value instanceof AttributeValue) return value.raw;
  if (value instanceof GeomSettings) return value.raw;
  if (value instanceof SerializerSettings) return value.raw;
  if (value instanceof GeomIterator) return value.raw;
  if (value instanceof GeometryTree) return value.raw;
  if (value instanceof InstanceList) return value.raw;
  if (value instanceof Value) return value.raw;
  if (value instanceof PsetProperties) return value.raw;
  if (isEntityArray(value)) {
    const raw = shell.raw.parse.instanceListCreateFromHandles(value.map((item) => item.raw));
    temps.push(raw);
    return raw;
  }
  if (Array.isArray(value)) return value.map((item) => toRaw(item, shell, temps));
  if (isPlainObject(value)) {
    return Object.fromEntries(Object.entries(value).map(([key, item]) => [key, toRaw(item, shell, temps)]));
  }
  return value;
}

function toRawEntityList(value: ApiInput, shell: IfcOpenShell, temps: Disposable[]): RawValue {
  if (!Array.isArray(value)) throw new TypeError('Expected an entity array.');
  const raw = shell.raw.parse.instanceListCreateFromHandles(value.map((item) => (item as Entity).raw));
  temps.push(raw);
  return raw;
}

function encodeOptionValue(publicName: string, value: ApiInput, shell: IfcOpenShell, temps: Disposable[], psetFields?: Set<string>, entityListFields?: Set<string>): RawValue {
  if (psetFields?.has(publicName)) {
    return toRawPsetProperties(shell, value as PsetProperties | PsetInput, temps);
  }
  if (entityListFields?.has(publicName)) {
    return toRawEntityList(value, shell, temps);
  }
  return toRaw(value, shell, temps);
}

function wrapEntities(shell: IfcOpenShell, value: RawValue): Entity[] {
  const list = wrapList(shell, value as never);
  if (list === null) return [];
  try {
    return list.items();
  } finally {
    list.dispose();
  }
}

function wrap(shell: IfcOpenShell, value: RawValue): ApiData {
  if (isRawEntity(value)) return wrapEntity(shell, value);
  if (isRawList(value)) return wrapEntities(shell, value);
  if (isRawValue(value)) return wrapValue(shell, value as never);
  if (Array.isArray(value)) return value.map((item) => wrap(shell, item));
  if (isPlainObject(value)) return Object.fromEntries(Object.entries(value).map(([key, item]) => [key, wrap(shell, item)]));
  return value as never;
}

function wrapEntity(shell: IfcOpenShell, value: RawValue): Entity | null {
  return Entity.wrap(shell, value as never);
}

function isRawEntity(value: RawValue): boolean {
  return Boolean(value && typeof value === 'object' && 'id' in value && 'className' in value);
}

function isRawList(value: RawValue): boolean {
  return Boolean(value && typeof value === 'object' && 'size' in value && 'get' in value);
}

function isRawValue(value: RawValue): boolean {
  return Boolean(value && typeof value === 'object' && 'ptr' in value && Object.getPrototypeOf(value)?.constructor?.name === 'IfcOpenshellValue');
}

function isEntityArray(value: ApiInput): value is Entity[] {
  return Array.isArray(value) && value.length > 0 && value.every((item) => item instanceof Entity);
}

function isPlainObject(value: RawValue): value is Record<string, ApiInput> {
  return typeof value === 'object' && value !== null && Object.getPrototypeOf(value) === Object.prototype;
}

function disposeAll(values: Disposable[]): void {
  for (let index = values.length - 1; index >= 0; index -= 1) values[index]?.destroy();
}
