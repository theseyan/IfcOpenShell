
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
type RawFn = (...args: RawValue[]) => RawValue;
type Disposable = { destroy(): void };
type RawApi = {
  aggregate: {
    assignObject: RawFn;
    unassignObject: RawFn;
  };
  attribute: {
    editAttributes: RawFn;
  };
  boundary: {
    assignConnectionGeometry: RawFn;
    copyBoundary: RawFn;
    editAttributes: RawFn;
    removeBoundary: RawFn;
  };
  classification: {
    addClassification: RawFn;
    addReference: RawFn;
    getReferences: RawFn;
    removeClassification: RawFn;
    removeReference: RawFn;
  };
  cogo: {
    addSurveyPoint: RawFn;
    assignSurveyPoint: RawFn;
    editSurveyPoint: RawFn;
  };
  compute: {
    derived: RawFn;
  };
  constraint: {
    addMetric: RawFn;
    addMetricReference: RawFn;
    addObjective: RawFn;
    assignConstraint: RawFn;
    removeConstraint: RawFn;
    removeMetric: RawFn;
    unassignConstraint: RawFn;
  };
  context: {
    addContext: RawFn;
    editContext: RawFn;
    removeContext: RawFn;
  };
  control: {
    assignControl: RawFn;
    unassignControl: RawFn;
  };
  cost: {
    addCostItem: RawFn;
    addCostItemQuantity: RawFn;
    addCostSchedule: RawFn;
    addCostValue: RawFn;
    assignCostItemQuantity: RawFn;
    assignCostValue: RawFn;
    calculateCostItemResourceValue: RawFn;
    copyCostItem: RawFn;
    copyCostItemValues: RawFn;
    copyCostSchedule: RawFn;
    editCostItem: RawFn;
    editCostItemQuantity: RawFn;
    editCostSchedule: RawFn;
    editCostValue: RawFn;
    editCostValueFormula: RawFn;
    removeCostItem: RawFn;
    removeCostItemQuantity: RawFn;
    removeCostSchedule: RawFn;
    removeCostValue: RawFn;
    unassignCostItemQuantity: RawFn;
  };
  document: {
    addInformation: RawFn;
    addReference: RawFn;
    assignDocument: RawFn;
    removeInformation: RawFn;
    removeReference: RawFn;
    unassignDocument: RawFn;
  };
  drawing: {
    assignProduct: RawFn;
    unassignProduct: RawFn;
  };
  element: {
    getAggregate: RawFn;
    getContained: RawFn;
    getContainer: RawFn;
    getControls: RawFn;
    getDecomposition: RawFn;
    getElementsByLayer: RawFn;
    getElementsByMaterial: RawFn;
    getElementsByProfile: RawFn;
    getElementsByRepresentation: RawFn;
    getElementsByStyle: RawFn;
    getFilledVoid: RawFn;
    getGroups: RawFn;
    getLayers: RawFn;
    getMaterial: RawFn;
    getNest: RawFn;
    getOpenings: RawFn;
    getParent: RawFn;
    getParts: RawFn;
    getPsetIds: RawFn;
    getReferencedElements: RawFn;
    getReferencedStructures: RawFn;
    getShapeAspects: RawFn;
    getStructureReferencedElements: RawFn;
    getStyles: RawFn;
    getType: RawFn;
    getTypes: RawFn;
    getVoidedElement: RawFn;
    isUserdefinedType: RawFn;
    removeDeep: RawFn;
    replaceElement: RawFn;
  };
  entity: {
    removeDeep2: RawFn;
    removeDeep2Ex: RawFn;
  };
  feature: {
    addFeature: RawFn;
    addFilling: RawFn;
    removeFeature: RawFn;
    removeFilling: RawFn;
  };
  geometry: {
    addAxisRepresentation: RawFn;
    addBoolean: RawFn;
    addDoorRepresentation: RawFn;
    addFootprintRepresentation: RawFn;
    addMeshRepresentation: RawFn;
    addRailingRepresentation: RawFn;
    addShapeAspect: RawFn;
    addSlabRepresentation: RawFn;
    addTopologyRepresentation: RawFn;
    addWallRepresentation: RawFn;
    addWindowRepresentation: RawFn;
    assignRepresentation: RawFn;
    clipSolid: RawFn;
    clipSolidBounded: RawFn;
    connectElement: RawFn;
    connectPath: RawFn;
    connectWall: RawFn;
    copyRepresentation: RawFn;
    create2ptWall: RawFn;
    disconnectElement: RawFn;
    disconnectPath: RawFn;
    editObjectPlacement: RawFn;
    mapRepresentation: RawFn;
    profileExtents: RawFn;
    regenerateWallRepresentation: RawFn;
    removeBoolean: RawFn;
    removeRepresentation: RawFn;
    unassignRepresentation: RawFn;
    validateType: RawFn;
  };
  georeference: {
    addGeoreferencing: RawFn;
    editGeoreferencing: RawFn;
    editTrueNorth: RawFn;
    editWcs: RawFn;
    removeGeoreferencing: RawFn;
  };
  grid: {
    createAxisCurve: RawFn;
    createGridAxis: RawFn;
    removeGridAxis: RawFn;
  };
  group: {
    addGroup: RawFn;
    assignGroup: RawFn;
    removeGroup: RawFn;
    unassignGroup: RawFn;
    updateGroupProducts: RawFn;
  };
  guid: {
    compress: RawFn;
    expand: RawFn;
    "new": RawFn;
  };
  layer: {
    addLayer: RawFn;
    addLayerWithStyle: RawFn;
    assignLayer: RawFn;
    removeLayer: RawFn;
    unassignLayer: RawFn;
  };
  library: {
    addLibrary: RawFn;
    addReference: RawFn;
    assignReference: RawFn;
    removeLibrary: RawFn;
    removeReference: RawFn;
    unassignReference: RawFn;
  };
  material: {
    addConstituent: RawFn;
    addLayer: RawFn;
    addListItem: RawFn;
    addMaterial: RawFn;
    addMaterialSet: RawFn;
    addProfile: RawFn;
    assignMaterial: RawFn;
    assignProfile: RawFn;
    editProfileUsage: RawFn;
    removeConstituent: RawFn;
    removeLayer: RawFn;
    removeListItem: RawFn;
    removeMaterial: RawFn;
    removeMaterialSet: RawFn;
    removeProfile: RawFn;
    reorderSetItem: RawFn;
    unassignMaterial: RawFn;
  };
  nest: {
    assignObject: RawFn;
    unassignObject: RawFn;
  };
  owner: {
    addActor: RawFn;
    addAddress: RawFn;
    addApplication: RawFn;
    addOrganisation: RawFn;
    addPerson: RawFn;
    addPersonAndOrganisation: RawFn;
    addRole: RawFn;
    assignActor: RawFn;
    createOwnerHistory: RawFn;
    removeActor: RawFn;
    removeAddress: RawFn;
    removeApplication: RawFn;
    removeOrganisation: RawFn;
    removePerson: RawFn;
    removePersonAndOrganisation: RawFn;
    removeRole: RawFn;
    unassignActor: RawFn;
    updateOwnerHistory: RawFn;
  };
  placement: {
    a2p: RawFn;
    getAxis2placement: RawFn;
    getCartesianXform3d: RawFn;
    getLocalPlacement: RawFn;
    getMappeditemXform: RawFn;
    getStoreyElevation: RawFn;
    rotation: RawFn;
  };
  profile: {
    addArbitraryProfile: RawFn;
    addArbitraryProfileWithVoids: RawFn;
    addParameterizedProfile: RawFn;
    copyProfile: RawFn;
    editProfile: RawFn;
    removeProfile: RawFn;
  };
  project: {
    assignDeclaration: RawFn;
    unassignDeclaration: RawFn;
  };
  pset: {
    addPset: RawFn;
    addQto: RawFn;
    assignPset: RawFn;
    editPset: RawFn;
    editQto: RawFn;
    removePset: RawFn;
    templateAddPropTemplate: RawFn;
    templateAddPsetTemplate: RawFn;
    templateCreateFromFiles: RawFn;
    templateGetApplicable: RawFn;
    templateGetApplicableNames: RawFn;
    templateGetByName: RawFn;
    templateGetTemplate: RawFn;
    templateIsTemplated: RawFn;
    templatePsetType: RawFn;
    templateRemovePropTemplate: RawFn;
    templateRemovePsetTemplate: RawFn;
    templateSetTemplateDir: RawFn;
    unassignPset: RawFn;
    unsharePset: RawFn;
  };
  register: {
    scratchFile: RawFn;
  };
  representation: {
    getContext: RawFn;
    getPrioritisedContexts: RawFn;
    getProductRepresentation: RawFn;
    resolve: RawFn;
    resolveBaseItems: RawFn;
  };
  resource: {
    editResourceTime: RawFn;
  };
  root: {
    createEntity: RawFn;
    removeProduct: RawFn;
  };
  schema: {
    reassignClass: RawFn;
  };
  selector: {
    filterAll: RawFn;
    filterElements: RawFn;
    format: RawFn;
    getElementValue: RawFn;
    setElementValue: RawFn;
  };
  sequence: {
    addDateTime: RawFn;
    addTask: RawFn;
    addTaskTime: RawFn;
    addTimePeriod: RawFn;
    addWorkCalendar: RawFn;
    addWorkPlan: RawFn;
    addWorkSchedule: RawFn;
    addWorkTime: RawFn;
    assignLagTime: RawFn;
    assignProcess: RawFn;
    assignProduct: RawFn;
    assignRecurrencePattern: RawFn;
    assignSequence: RawFn;
    assignWorkPlan: RawFn;
    calculateTaskDuration: RawFn;
    cascadeSchedule: RawFn;
    copyWorkSchedule: RawFn;
    createBaseline: RawFn;
    duplicateTask: RawFn;
    editLagTime: RawFn;
    editRecurrencePattern: RawFn;
    editSequence: RawFn;
    editTask: RawFn;
    editTaskTime: RawFn;
    editWorkCalendar: RawFn;
    editWorkPlan: RawFn;
    editWorkSchedule: RawFn;
    editWorkTime: RawFn;
    recalculateSchedule: RawFn;
    removeTask: RawFn;
    removeTimePeriod: RawFn;
    removeWorkCalendar: RawFn;
    removeWorkPlan: RawFn;
    removeWorkSchedule: RawFn;
    removeWorkTime: RawFn;
    unassignLagTime: RawFn;
    unassignProcess: RawFn;
    unassignProduct: RawFn;
    unassignRecurrencePattern: RawFn;
    unassignSequence: RawFn;
  };
  shape: {
    builderAxis2Placement2d: RawFn;
    builderAxis2Placement3d: RawFn;
    builderBlock: RawFn;
    builderCircle: RawFn;
    builderCurveBetweenTwoPoints: RawFn;
    builderDeepCopy: RawFn;
    builderEdge: RawFn;
    builderEllipseCurve: RawFn;
    builderExtrude: RawFn;
    builderFace: RawFn;
    builderFacetedBrep: RawFn;
    builderGetPolylineCoords: RawFn;
    builderHalfSpaceSolid: RawFn;
    builderIndexedPolycurve2d: RawFn;
    builderMepBendShape: RawFn;
    builderMepTransitionCalculate: RawFn;
    builderMepTransitionLength: RawFn;
    builderMepTransitionShape: RawFn;
    builderMesh: RawFn;
    builderMirror: RawFn;
    builderPlane: RawFn;
    builderPolygonalFaceSet: RawFn;
    builderPolyline: RawFn;
    builderProfile: RawFn;
    builderRepresentation: RawFn;
    builderRotate: RawFn;
    builderSetPolylineCoords: RawFn;
    builderSphere: RawFn;
    builderSweptDiskSolid: RawFn;
    builderTranslate: RawFn;
    builderTriangulatedFaceSet: RawFn;
    builderVertex: RawFn;
    isX: RawFn;
  };
  spatial: {
    assignContainer: RawFn;
    dereferenceStructure: RawFn;
    referenceStructure: RawFn;
    unassignContainer: RawFn;
  };
  structural: {
    addStructuralActivity: RawFn;
    addStructuralAnalysisModel: RawFn;
    addStructuralBoundaryCondition: RawFn;
    addStructuralLoad: RawFn;
    addStructuralLoadCase: RawFn;
    addStructuralLoadGroup: RawFn;
    addStructuralMemberConnection: RawFn;
    assignProduct: RawFn;
    assignStructuralAnalysisModel: RawFn;
    assignToBuilding: RawFn;
    editStructuralBoundaryCondition: RawFn;
    editStructuralConnectionCs: RawFn;
    editStructuralItemAxis: RawFn;
    removeStructuralAnalysisModel: RawFn;
    removeStructuralBoundaryCondition: RawFn;
    removeStructuralConnectionCondition: RawFn;
    removeStructuralLoad: RawFn;
    removeStructuralLoadCase: RawFn;
    removeStructuralLoadGroup: RawFn;
    unassignStructuralAnalysisModel: RawFn;
  };
  style: {
    addStyle: RawFn;
    assignItemStyle: RawFn;
    assignMaterialStyle: RawFn;
    assignRepresentationStyles: RawFn;
    editSurfaceStyle: RawFn;
    removeStyle: RawFn;
    removeStyledRepresentation: RawFn;
    removeSurfaceStyle: RawFn;
    unassignMaterialStyle: RawFn;
    unassignRepresentationStyles: RawFn;
  };
  system: {
    addPort: RawFn;
    addSystem: RawFn;
    assignFlowControl: RawFn;
    assignPort: RawFn;
    assignSystem: RawFn;
    connectPort: RawFn;
    disconnectPort: RawFn;
    removeSystem: RawFn;
    unassignFlowControl: RawFn;
    unassignPort: RawFn;
    unassignSystem: RawFn;
  };
  type: {
    assignType: RawFn;
    mapTypeRepresentations: RawFn;
    unassignType: RawFn;
  };
  unit: {
    addContextDependentUnit: RawFn;
    addDerivedUnit: RawFn;
    addMonetaryUnit: RawFn;
    addSiUnit: RawFn;
    calculateUnitScale: RawFn;
    convert: RawFn;
    convertUnit: RawFn;
    formatLength: RawFn;
    getFullUnitName: RawFn;
    getMeasureClass: RawFn;
    getMeasureUnitType: RawFn;
    getNamedDimensions: RawFn;
    getPrefix: RawFn;
    getPrefixMultiplier: RawFn;
    getProjectUnit: RawFn;
    getSiDimensions: RawFn;
    getSymbolMeasureClass: RawFn;
    getSymbolQuantityClass: RawFn;
    getUnitAssignment: RawFn;
    getUnitName: RawFn;
    getUnitNameUniversal: RawFn;
    getUnitSymbol: RawFn;
    removeUnit: RawFn;
    resolvePropertyMeasureClass: RawFn;
    resolvePropertyTableDefinedMeasureClass: RawFn;
    resolvePropertyTableDefinedUnit: RawFn;
    resolvePropertyTableDefiningMeasureClass: RawFn;
    resolvePropertyTableDefiningUnit: RawFn;
    resolvePropertyUnit: RawFn;
    unassignUnit: RawFn;
  };
};

export interface IfcOpenShellSequenceDuplicateTaskResult {
  current: Entity[];
  duplicate: Entity[];
}

export interface IfcOpenShellShapeBuilderMepBendShapeResult {
  representation: Entity;
  startLength: number;
  endLength: number;
  radius: number;
  angle: number;
  lateralAxis: number;
  lateralSign: number;
  zAxisSign: number;
  mainProfileDimension: number;
}

export interface IfcOpenShellShapeBuilderMepTransitionShapeResult {
  representation: Entity;
  startLength: number;
  endLength: number;
  angle: number;
  profileOffset: number[];
  transitionLength: number;
  fullTransitionLength: number;
}

export interface PsetTemplate {
  readonly ptr: number;
  destroy(): void;
}

export interface IfcOpenShellAggregateAssignObjectOptions {
  products: Entity[];
  relatingObject: Entity;
  ownerHistory?: Entity;
  user?: Entity;
  application?: Entity;
}

export interface IfcOpenShellAggregateUnassignObjectOptions {
  products: Entity[];
  user?: Entity;
  application?: Entity;
}

export interface IfcOpenShellAttributeEditAttributesOptions {
  product: Entity;
  attributes: PsetProperties | PsetInput;
  syncPredefinedType: boolean;
  updateOwnerHistory: boolean;
  user?: Entity;
  application?: Entity;
}

export interface IfcOpenShellBoundaryAssignConnectionGeometryOptions {
  outerBoundary: number[][];
  location: number[];
  axis: number[];
  refDirection: number[];
  innerBoundaries: number[][][];
  unitScale: number;
}

export interface IfcOpenShellBoundaryEditAttributesOptions {
  relatingSpace: Entity;
  relatedBuildingElement: Entity;
  parentBoundary?: Entity;
  correspondingBoundary?: Entity;
  physicalOrVirtual: string;
  internalOrExternal: string;
}

export interface IfcOpenShellClassificationAddReferenceOptions {
  products: Entity[];
  reference?: Entity;
  identification?: string;
  name?: string;
  classification?: Entity;
  ownerHistory?: Entity;
  user?: Entity;
  application?: Entity;
}

export interface IfcOpenShellClassificationRemoveReferenceOptions {
  reference: Entity;
  products: Entity[];
  user?: Entity;
  application?: Entity;
}

export interface IfcOpenShellCogoAddSurveyPointOptions {
  surveyPoint: Entity;
  site?: Entity;
  ownerHistory?: Entity;
  user?: Entity;
  application?: Entity;
}

export interface IfcOpenShellConstraintAssignConstraintOptions {
  products: Entity[];
  constraint: Entity;
  ownerHistory?: Entity;
  user?: Entity;
  application?: Entity;
}

export interface IfcOpenShellConstraintUnassignConstraintOptions {
  products: Entity[];
  constraint: Entity;
  user?: Entity;
  application?: Entity;
}

export interface IfcOpenShellContextAddContextOptions {
  contextType: string;
  contextIdentifier: string;
  targetView: string;
  targetScale?: number;
  parent?: Entity;
}

export interface IfcOpenShellControlAssignControlOptions {
  relatingControl: Entity;
  relatedObjects: Entity[];
  ownerHistory?: Entity;
  user?: Entity;
  application?: Entity;
}

export interface IfcOpenShellControlUnassignControlOptions {
  relatingControl: Entity;
  relatedObjects: Entity[];
  user?: Entity;
  application?: Entity;
}

export interface IfcOpenShellCostAddCostItemOptions {
  costSchedule?: Entity;
  costItem?: Entity;
  ownerHistory?: Entity;
  user?: Entity;
  application?: Entity;
}

export interface IfcOpenShellCostAssignCostItemQuantityOptions {
  ownerHistory?: Entity;
  user?: Entity;
  application?: Entity;
}

export interface IfcOpenShellCostCopyCostScheduleOptions {
  ownerHistory?: Entity;
  user?: Entity;
  application?: Entity;
}

export interface IfcOpenShellCostEditCostValueOptions {
  editUnitBasis: boolean;
  clearUnitBasis: boolean;
  valueComponent: number;
  unitComponent?: Entity;
}

export interface IfcOpenShellCostUnassignCostItemQuantityOptions {
  user?: Entity;
  application?: Entity;
}

export interface IfcOpenShellDocumentAddInformationOptions {
  parent?: Entity;
  ownerHistory?: Entity;
  user?: Entity;
  application?: Entity;
}

export interface IfcOpenShellDocumentAssignDocumentOptions {
  products: Entity[];
  document: Entity;
  ownerHistory?: Entity;
  user?: Entity;
  application?: Entity;
}

export interface IfcOpenShellDocumentUnassignDocumentOptions {
  products: Entity[];
  document: Entity;
  user?: Entity;
  application?: Entity;
}

export interface IfcOpenShellDrawingAssignProductOptions {
  relatingProduct: Entity;
  relatedObject: Entity;
  ownerHistory?: Entity;
  user?: Entity;
  application?: Entity;
}

export interface IfcOpenShellDrawingUnassignProductOptions {
  relatingProduct: Entity;
  relatedObject: Entity;
  user?: Entity;
  application?: Entity;
}

export interface IfcOpenShellElementGetContainerOptions {
  directOnly?: boolean;
  ifcClass?: string;
}

export interface IfcOpenShellElementGetDecompositionOptions {
  isRecursive?: boolean;
}

export interface IfcOpenShellElementGetMaterialOptions {
  shouldSkipUsage?: boolean;
  shouldInherit?: boolean;
}

export interface IfcOpenShellElementGetPsetIdsOptions {
  psetsOnly?: boolean;
  qtosOnly?: boolean;
  shouldInherit?: boolean;
}

export interface IfcOpenShellElementGetShapeAspectsOptions {
  shouldInherit?: boolean;
}

export interface IfcOpenShellFeatureAddFeatureOptions {
  feature: Entity;
  element: Entity;
  user?: Entity;
  application?: Entity;
}

export interface IfcOpenShellFeatureRemoveFeatureOptions {
  feature: Entity;
  user?: Entity;
  application?: Entity;
}

export interface IfcOpenShellGeometryAddDoorRepresentationOptions {
  context: Entity;
  overallHeight: number;
  overallWidth: number;
  operationType: string;
  liningProperties: number[];
  panelProperties: number[];
  partOfProduct?: Entity;
  unitScale: number;
}

export interface IfcOpenShellGeometryAddMeshRepresentationOptions {
  vertices: number[][][];
  faces: number[][][][];
  forceFacetedBrep?: boolean;
}

export interface IfcOpenShellGeometryAddRailingRepresentationOptions {
  context: Entity;
  railingPath: number[][];
  useManualSupports: boolean;
  supportSpacing: number;
  railingDiameter: number;
  clearWidth: number;
  terminalType: string;
  height: number;
  loopedPath: boolean;
  unitScale: number;
}

export interface IfcOpenShellGeometryAddShapeAspectOptions {
  name: string;
  items: Entity[];
  representation: Entity;
  partOfProduct: Entity;
  description?: string;
}

export interface IfcOpenShellGeometryAddSlabRepresentationOptions {
  context: Entity;
  depth: number;
  directionSense: string;
  offset: number;
  xAngle: number;
  clippingKinds: number[];
  clippingLocations: number[][];
  clippingNormals: number[][];
  clippingEntities: Entity[];
  polyline?: number[][];
}

export interface IfcOpenShellGeometryAddTopologyRepresentationOptions {
  context: Entity;
  item: Entity;
  representationIdentifier?: string;
  representationType?: string;
}

export interface IfcOpenShellGeometryAddWallRepresentationOptions {
  context: Entity;
  length: number;
  height: number;
  directionSense: string;
  offset: number;
  thickness: number;
  xAngle: number;
  clippingKinds: number[];
  clippingLocations: number[][];
  clippingNormals: number[][];
  clippingEntities: Entity[];
  booleans: Entity[];
}

export interface IfcOpenShellGeometryAddWindowRepresentationOptions {
  context: Entity;
  overallHeight: number;
  overallWidth: number;
  panelSchema: number[][];
  liningProperties: number[];
  panelProperties: number[][];
  partOfProduct?: Entity;
  glassThickness: number;
}

export interface IfcOpenShellGeometryClipSolidBoundedOptions {
  item: Entity;
  location: number[];
  normal: number[];
  boundaryPoints: number[][];
  boundaryPosition: number[];
  element?: Entity;
  ownerHistory?: Entity;
  user?: Entity;
  application?: Entity;
}

export interface IfcOpenShellGeometryClipSolidOptions {
  item: Entity;
  location: number[];
  normal: number[];
  element?: Entity;
  ownerHistory?: Entity;
  user?: Entity;
  application?: Entity;
}

export interface IfcOpenShellGeometryConnectElementOptions {
  relatingElement: Entity;
  relatedElement: Entity;
  description?: string;
  ownerHistory?: Entity;
  user?: Entity;
  application?: Entity;
}

export interface IfcOpenShellGeometryConnectPathOptions {
  relatingElement: Entity;
  relatedElement: Entity;
  relatingConnection: string;
  relatedConnection: string;
  description?: string;
  connectionGeometry?: Entity;
  ownerHistory?: Entity;
  user?: Entity;
  application?: Entity;
}

export interface IfcOpenShellGeometryConnectWallOptions {
  firstWall: Entity;
  secondWall: Entity;
  isAtpath: boolean;
  ownerHistory?: Entity;
  user?: Entity;
  application?: Entity;
}

export interface IfcOpenShellGeometryCopyRepresentationOptions {
  source: Entity;
  target: Entity;
  contextIdentifier?: string;
}

export interface IfcOpenShellGeometryCreate2PtWallOptions {
  element: Entity;
  context: Entity;
  start: number[];
  end: number[];
  elevation: number;
  height: number;
  thickness: number;
  isSi: boolean;
}

export interface IfcOpenShellGeometryDisconnectPathOptions {
  element?: Entity;
  connectionType?: string;
  relatingElement?: Entity;
  relatedElement?: Entity;
}

export interface IfcOpenShellGeometryEditObjectPlacementOptions {
  product: Entity;
  matrix: number[];
  isSi: boolean;
  shouldTransformChildren: boolean;
}

export interface IfcOpenShellGeometryRegenerateWallRepresentationOptions {
  wall: Entity;
  length: number;
  height: number;
  angle?: number;
}

export interface IfcOpenShellGeometryRemoveRepresentationOptions {
  shouldKeepNamedProfiles?: boolean;
}

export interface IfcOpenShellGeometryValidateTypeOptions {
  preferredItem?: Entity;
}

export interface IfcOpenShellGeoreferenceAddGeoreferencingOptions {
  ifcClass: string;
  name: string;
  ownerHistory?: Entity;
  user?: Entity;
  application?: Entity;
}

export interface IfcOpenShellGeoreferenceEditGeoreferencingOptions {
  coordinateOperation?: PsetProperties | PsetInput;
  projectedCrs?: PsetProperties | PsetInput;
}

export interface IfcOpenShellGeoreferenceEditTrueNorthOptions {
  trueNorth?: number[];
}

export interface IfcOpenShellGeoreferenceEditWcsOptions {
  x?: number;
  y?: number;
  z?: number;
  rotation?: number;
  isSi?: boolean;
}

export interface IfcOpenShellGroupAddGroupOptions {
  name: string;
  description?: string;
  ownerHistory?: Entity;
  user?: Entity;
  application?: Entity;
}

export interface IfcOpenShellGroupAssignGroupOptions {
  products: Entity[];
  group: Entity;
  ownerHistory?: Entity;
  user?: Entity;
  application?: Entity;
}

export interface IfcOpenShellGroupUnassignGroupOptions {
  products: Entity[];
  group: Entity;
  user?: Entity;
  application?: Entity;
}

export interface IfcOpenShellGroupUpdateGroupProductsOptions {
  group: Entity;
  products: Entity[];
  ownerHistory?: Entity;
  user?: Entity;
  application?: Entity;
}

export interface IfcOpenShellLibraryAssignReferenceOptions {
  products: Entity[];
  reference: Entity;
  ownerHistory?: Entity;
  user?: Entity;
  application?: Entity;
}

export interface IfcOpenShellLibraryUnassignReferenceOptions {
  reference: Entity;
  products: Entity[];
  user?: Entity;
  application?: Entity;
}

export interface IfcOpenShellMaterialAddConstituentOptions {
  material: Entity;
  name?: string;
}

export interface IfcOpenShellMaterialAddLayerOptions {
  material: Entity;
  name?: string;
}

export interface IfcOpenShellMaterialAddMaterialOptions {
  name?: string;
  category?: string;
  description?: string;
}

export interface IfcOpenShellMaterialAddMaterialSetOptions {
  name?: string;
  setType?: string;
}

export interface IfcOpenShellMaterialAddProfileOptions {
  material?: Entity;
  profile?: Entity;
  name?: string;
}

export interface IfcOpenShellMaterialAssignMaterialOptions {
  type?: string;
  material?: Entity;
  ownerHistory?: Entity;
  user?: Entity;
  application?: Entity;
}

export interface IfcOpenShellMaterialEditProfileUsageOptions {
  attributes: PsetProperties | PsetInput;
  profileWidth?: number;
  profileHeight?: number;
}

export interface IfcOpenShellMaterialRemoveItemOptions {
  shouldRemoveMaterial?: boolean;
}

export interface IfcOpenShellMaterialRemoveListItemOptions {
  materialIndex?: number;
}

export interface IfcOpenShellMaterialRemoveProfileOptions {
  shouldRemoveProfileDef?: boolean;
  shouldRemoveMaterial?: boolean;
}

export interface IfcOpenShellMaterialReorderSetItemOptions {
  oldIndex?: number;
  newIndex?: number;
}

export interface IfcOpenShellMaterialUnassignMaterialOptions {
  user?: Entity;
  application?: Entity;
}

export interface IfcOpenShellNestAssignObjectOptions {
  products: Entity[];
  relatingObject: Entity;
  ownerHistory?: Entity;
  user?: Entity;
  application?: Entity;
}

export interface IfcOpenShellNestUnassignObjectOptions {
  products: Entity[];
  user?: Entity;
  application?: Entity;
}

export interface IfcOpenShellOwnerAddActorOptions {
  actor: Entity;
  ifcClass: string;
  ownerHistory?: Entity;
  user?: Entity;
  application?: Entity;
}

export interface IfcOpenShellOwnerAddApplicationOptions {
  applicationDeveloper?: Entity;
  version: string;
  applicationFullName: string;
  applicationIdentifier: string;
  ownerHistory?: Entity;
  user?: Entity;
  application?: Entity;
}

export interface IfcOpenShellOwnerAssignActorOptions {
  relatingActor: Entity;
  relatedObject: Entity;
  ownerHistory?: Entity;
  user?: Entity;
  application?: Entity;
}

export interface IfcOpenShellOwnerCreateOwnerHistoryOptions {
  user?: Entity;
  application?: Entity;
}

export interface IfcOpenShellOwnerUnassignActorOptions {
  relatingActor: Entity;
  relatedObject: Entity;
  user?: Entity;
  application?: Entity;
}

export interface IfcOpenShellOwnerUpdateOwnerHistoryOptions {
  element?: Entity;
  user?: Entity;
  application?: Entity;
}

export interface IfcOpenShellProfileAddArbitraryProfileOptions {
  profile: number[][];
  name?: string;
}

export interface IfcOpenShellProfileAddArbitraryProfileWithVoidsOptions {
  outerProfile: number[][];
  innerProfiles: number[][][];
  name?: string;
}

export interface IfcOpenShellProjectAssignDeclarationOptions {
  definitions: Entity[];
  relatingContext: Entity;
  ownerHistory?: Entity;
  user?: Entity;
  application?: Entity;
}

export interface IfcOpenShellProjectUnassignDeclarationOptions {
  definitions: Entity[];
  relatingContext: Entity;
  user?: Entity;
  application?: Entity;
}

export interface IfcOpenShellPsetAddPsetOptions {
  product: Entity;
  name: string;
  ownerHistory?: Entity;
  user?: Entity;
  application?: Entity;
  ifc2x3Subclass?: string;
}

export interface IfcOpenShellPsetAddQtoOptions {
  product: Entity;
  name: string;
  ownerHistory?: Entity;
  user?: Entity;
  application?: Entity;
}

export interface IfcOpenShellPsetAssignPsetOptions {
  products: Entity[];
  pset: Entity;
  ownerHistory?: Entity;
  user?: Entity;
  application?: Entity;
}

export interface IfcOpenShellPsetEditPsetOptions {
  pset: Entity;
  name?: string;
  properties: PsetProperties | PsetInput;
  psetTemplate?: Entity;
  shouldPurge: boolean;
}

export interface IfcOpenShellPsetEditQtoOptions {
  qto: Entity;
  name?: string;
  properties: PsetProperties | PsetInput;
  qtoTemplate?: Entity;
}

export interface IfcOpenShellPsetUnsharePsetOptions {
  products: Entity[];
  pset: Entity;
  ownerHistory?: Entity;
  user?: Entity;
  application?: Entity;
}

export interface IfcOpenShellRepresentationGetProductRepresentationOptions {
  context?: Entity;
  contextType?: string;
  subcontext?: string;
  targetView?: string;
}

export interface IfcOpenShellRootCreateEntityOptions {
  ifcClass: string;
  predefinedType?: string;
  name?: string;
  ownerHistory?: Entity;
}

export interface IfcOpenShellRootRemoveProductOptions {
  user?: Entity;
  application?: Entity;
}

export interface IfcOpenShellSequenceAddTaskOptions {
  workSchedule?: Entity;
  parentTask?: Entity;
  name?: string;
  description?: string;
  identification?: string;
  predefinedType?: string;
  ownerHistory?: Entity;
  user?: Entity;
  application?: Entity;
}

export interface IfcOpenShellSequenceAddTaskTimeOptions {
  isRecurring?: boolean;
}

export interface IfcOpenShellSequenceAddTimePeriodOptions {
  startTime?: string;
  endTime?: string;
}

export interface IfcOpenShellSequenceAddWorkCalendarOptions {
  name?: string;
  predefinedType?: string;
  ownerHistory?: Entity;
  user?: Entity;
  application?: Entity;
}

export interface IfcOpenShellSequenceAddWorkPlanOptions {
  name?: string;
  predefinedType?: string;
  creationDate?: string;
  startTime?: string;
  creatorPerson?: Entity;
  ownerHistory?: Entity;
  user?: Entity;
  application?: Entity;
}

export interface IfcOpenShellSequenceAddWorkScheduleOptions {
  name?: string;
  predefinedType?: string;
  objectType?: string;
  creationDate?: string;
  startTime?: string;
  workPlan?: Entity;
  creatorPerson?: Entity;
  ownerHistory?: Entity;
  user?: Entity;
  application?: Entity;
}

export interface IfcOpenShellSequenceAssignLagTimeOptions {
  durationType?: string;
}

export interface IfcOpenShellSequenceAssignProcessOptions {
  ownerHistory?: Entity;
  user?: Entity;
  application?: Entity;
}

export interface IfcOpenShellSequenceAssignProductOptions {
  ownerHistory?: Entity;
  user?: Entity;
  application?: Entity;
}

export interface IfcOpenShellSequenceAssignSequenceOptions {
  sequenceType?: string;
  ownerHistory?: Entity;
  user?: Entity;
  application?: Entity;
}

export interface IfcOpenShellSequenceAssignWorkPlanOptions {
  ownerHistory?: Entity;
  user?: Entity;
  application?: Entity;
}

export interface IfcOpenShellSequenceCopyWorkScheduleOptions {
  ownerHistory?: Entity;
  user?: Entity;
  application?: Entity;
}

export interface IfcOpenShellSequenceCreateBaselineOptions {
  name?: string;
  ownerHistory?: Entity;
  user?: Entity;
  application?: Entity;
}

export interface IfcOpenShellSequenceDuplicateTaskOptions {
  ownerHistory?: Entity;
  user?: Entity;
  application?: Entity;
}

export interface IfcOpenShellSequenceRemoveOptions {
  user?: Entity;
  application?: Entity;
}

export interface IfcOpenShellShapeBuilderAxis2Placement2dOptions {
  position: number[];
  xDirection?: number[];
}

export interface IfcOpenShellShapeBuilderAxis2Placement3dOptions {
  position: number[];
  zAxis: number[];
  xAxis: number[];
}

export interface IfcOpenShellShapeBuilderBlockOptions {
  position: number[];
  xLength: number;
  yLength: number;
  zLength: number;
}

export interface IfcOpenShellShapeBuilderEllipseCurveOptions {
  xAxisRadius: number;
  yAxisRadius: number;
  position: number[];
  trimPoints: number[][];
  refXDirection?: number[];
  trimPointsMask: number[];
}

export interface IfcOpenShellShapeBuilderExtrudeOptions {
  profileOrCurve: Entity;
  magnitude: number;
  position: number[];
  extrusionVector: number[];
  positionZAxis: number[];
  positionXAxis: number[];
  positionYAxis?: number[];
}

export interface IfcOpenShellShapeBuilderHalfSpaceSolidOptions {
  plane: Entity;
  agreementFlag: boolean;
}

export interface IfcOpenShellShapeBuilderMepBendShapeOptions {
  segment: Entity;
  startLength: number;
  endLength: number;
  angle: number;
  radius: number;
  bendVector: number[];
  flipZAxis: boolean;
}

export interface IfcOpenShellShapeBuilderMepTransitionCalculateOptions {
  startHalfDim: number[];
  endHalfDim: number[];
  offset: number[];
  diff?: number[];
  endProfile: boolean;
  length?: number;
  angle?: number;
}

export interface IfcOpenShellShapeBuilderMepTransitionLengthOptions {
  startHalfDim: number[];
  endHalfDim: number[];
  angle: number;
  profileOffset: number[];
}

export interface IfcOpenShellShapeBuilderMepTransitionShapeOptions {
  startSegment: Entity;
  endSegment: Entity;
  startLength: number;
  endLength: number;
  angle: number;
  profileOffset: number[];
}

export interface IfcOpenShellShapeBuilderMirrorOptions {
  item: Entity;
  mirrorAxes: number[];
  mirrorPoint: number[];
  createCopy: boolean;
  placementMatrix: number[];
}

export interface IfcOpenShellShapeBuilderPolylineOptions {
  points: number[][];
  closed?: boolean;
  positionOffset?: number[];
  arcPoints: number[];
}

export interface IfcOpenShellShapeBuilderProfileOptions {
  outerCurve: Entity;
  name?: string;
  innerCurves: Entity[];
  profileType?: string;
}

export interface IfcOpenShellShapeBuilderRepresentationOptions {
  context: Entity;
  items: Entity[];
  representationType?: string;
}

export interface IfcOpenShellShapeBuilderRotateOptions {
  item: Entity;
  angle: number;
  pivotPoint: number[];
  counterClockwise: boolean;
  createCopy: boolean;
}

export interface IfcOpenShellShapeBuilderSphereOptions {
  radius: number;
  center: number[];
}

export interface IfcOpenShellShapeBuilderTranslateOptions {
  item: Entity;
  translation: number[];
  createCopy: boolean;
}

export interface IfcOpenShellSpatialAssignContainerOptions {
  products: Entity[];
  relatingStructure: Entity;
  ownerHistory?: Entity;
  user?: Entity;
  application?: Entity;
}

export interface IfcOpenShellSpatialDereferenceStructureOptions {
  products: Entity[];
  relatingStructure: Entity;
  user?: Entity;
  application?: Entity;
}

export interface IfcOpenShellSpatialReferenceStructureOptions {
  products: Entity[];
  relatingStructure: Entity;
  ownerHistory?: Entity;
  user?: Entity;
  application?: Entity;
}

export interface IfcOpenShellSpatialUnassignContainerOptions {
  products: Entity[];
  user?: Entity;
  application?: Entity;
}

export interface IfcOpenShellStructuralAddStructuralActivityOptions {
  activityOwnerHistory?: Entity;
  relationshipOwnerHistory?: Entity;
}

export interface IfcOpenShellStructuralAddStructuralBoundaryConditionOptions {
  name?: string;
  connection?: Entity;
}

export interface IfcOpenShellStructuralAssignStructuralAnalysisModelOptions {
  ownerHistory?: Entity;
  user?: Entity;
  application?: Entity;
}

export interface IfcOpenShellStructuralRemoveStructuralBoundaryConditionOptions {
  connection?: Entity;
  boundaryCondition?: Entity;
}

export interface IfcOpenShellStructuralUnassignStructuralAnalysisModelOptions {
  user?: Entity;
  application?: Entity;
}

export interface IfcOpenShellStyleAssignItemStyleOptions {
  item: Entity;
  style?: Entity;
  shouldUsePresentationStyleAssignment: boolean;
}

export interface IfcOpenShellSystemAddPortOptions {
  element?: Entity;
  ownerHistory?: Entity;
  user?: Entity;
  application?: Entity;
}

export interface IfcOpenShellSystemAddSystemOptions {
  ifcClass: string;
  ownerHistory?: Entity;
}

export interface IfcOpenShellSystemAssignFlowControlOptions {
  relatingFlowElement: Entity;
  relatedFlowControl: Entity;
  ownerHistory?: Entity;
  user?: Entity;
  application?: Entity;
}

export interface IfcOpenShellSystemAssignPortOptions {
  element: Entity;
  port: Entity;
  ownerHistory?: Entity;
  user?: Entity;
  application?: Entity;
}

export interface IfcOpenShellSystemAssignSystemOptions {
  products: Entity[];
  system: Entity;
  ownerHistory?: Entity;
  user?: Entity;
  application?: Entity;
}

export interface IfcOpenShellSystemConnectPortOptions {
  port1: Entity;
  port2: Entity;
  direction: string;
  element?: Entity;
  ownerHistory?: Entity;
  user?: Entity;
  application?: Entity;
}

export interface IfcOpenShellSystemUnassignFlowControlOptions {
  relatingFlowElement: Entity;
  relatedFlowControl: Entity;
  user?: Entity;
  application?: Entity;
}

export interface IfcOpenShellSystemUnassignPortOptions {
  element: Entity;
  port: Entity;
  user?: Entity;
  application?: Entity;
}

export interface IfcOpenShellSystemUnassignSystemOptions {
  products: Entity[];
  system: Entity;
  user?: Entity;
  application?: Entity;
}

export interface IfcOpenShellTypeAssignTypeOptions {
  objects: Entity[];
  relatingType: Entity;
  shouldMapRepresentations?: boolean;
  ownerHistory?: Entity;
  user?: Entity;
  application?: Entity;
}

export interface IfcOpenShellTypeUnassignTypeOptions {
  objects: Entity[];
  user?: Entity;
  application?: Entity;
}

export interface AggregateApi {
    /** Assign products as parts of an aggregate object. */
    assignObject(file: IfcFile, options: IfcOpenShellAggregateAssignObjectOptions): Entity;
    /** Remove products from their aggregate relationships. */
    unassignObject(file: IfcFile, options: IfcOpenShellAggregateUnassignObjectOptions): void;
}
export interface AttributeApi {
    editAttributes(file: IfcFile, options: IfcOpenShellAttributeEditAttributesOptions): void;
}
export interface BoundaryApi {
    /** Assign a planar connection geometry to a space boundary relationship. */
    assignConnectionGeometry(file: IfcFile, rel_space_boundary: Entity, options: IfcOpenShellBoundaryAssignConnectionGeometryOptions): void;
    copyBoundary(file: IfcFile, boundary: Entity): Entity;
    editAttributes(entity: Entity, options: IfcOpenShellBoundaryEditAttributesOptions): void;
    removeBoundary(file: IfcFile, boundary: Entity): void;
}
export interface ClassificationApi {
    addClassification(file: IfcFile, name: string): Entity;
    /**
     * Add a classification reference and associate it with products.
     *
     * If an existing reference handle is provided, it is used directly.
     * Otherwise, a new IfcClassificationReference is created using the
     * optional identification, name, and classification fields.
     */
    addReference(file: IfcFile, options: IfcOpenShellClassificationAddReferenceOptions): Entity;
    getReferences(element: Entity, should_inherit: boolean): Entity[];
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
    /** Add a single survey point as an IfcAnnotation. */
    addSurveyPoint(file: IfcFile, options: IfcOpenShellCogoAddSurveyPointOptions): Entity;
    assignSurveyPoint(annotation: Entity, survey_point: Entity): void;
    editSurveyPoint(annotation: Entity, x: number, y: number, z: number): void;
}
export interface ComputeApi {
    derived(instance: Entity, attribute_name: string): ValueData | null;
}
export interface ConstraintApi {
    /**
     * Add a new IfcMetric constraint. If an objective is provided, the metric
     * is appended to the objective's benchmark values.
     */
    addMetric(file: IfcFile, objective: Entity): Entity;
    addMetricReference(file: IfcFile, metric: Entity, reference_path: string): Entity[];
    addObjective(file: IfcFile): Entity;
    /**
     * Assign a constraint to a list of products via IfcRelAssociatesConstraint.
     *
     * If the constraint is already associated with some of the products, only
     * new products are added to the existing relationship.
     */
    assignConstraint(file: IfcFile, options: IfcOpenShellConstraintAssignConstraintOptions): Entity;
    removeConstraint(file: IfcFile, constraint: Entity): void;
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
    addContext(file: IfcFile, options: IfcOpenShellContextAddContextOptions): Entity;
    editContext(file: IfcFile, context: Entity, attributes: ApiData): void;
    removeContext(file: IfcFile, context: Entity): void;
}
export interface ControlApi {
    /** Assign a planning control or constraint to a list of objects. */
    assignControl(file: IfcFile, options: IfcOpenShellControlAssignControlOptions): Entity;
    /** Remove a planning control or constraint from a list of objects. */
    unassignControl(file: IfcFile, options: IfcOpenShellControlUnassignControlOptions): void;
}
export interface CostApi {
    addCostItem(file: IfcFile, options: IfcOpenShellCostAddCostItemOptions): Entity;
    addCostItemQuantity(file: IfcFile, cost_item: Entity, ifc_class: string): Entity;
    addCostSchedule(file: IfcFile, name: string, predefined_type: string, update_date: string, owner_history: Entity): Entity;
    addCostValue(file: IfcFile, parent: Entity): Entity;
    assignCostItemQuantity(file: IfcFile, cost_item: Entity, products: Entity[], prop_name: string, options: IfcOpenShellCostAssignCostItemQuantityOptions): void;
    assignCostValue(file: IfcFile, cost_item: Entity, cost_rate: Entity): void;
    calculateCostItemResourceValue(file: IfcFile, cost_item: Entity): void;
    copyCostItem(file: IfcFile, cost_item: Entity): Entity[];
    copyCostItemValues(file: IfcFile, source: Entity, destination: Entity): void;
    copyCostSchedule(file: IfcFile, cost_schedule: Entity, options: IfcOpenShellCostCopyCostScheduleOptions): Entity;
    editCostItem(file: IfcFile, cost_item: Entity, attributes: ApiData): void;
    editCostItemQuantity(file: IfcFile, physical_quantity: Entity, attributes: ApiData): void;
    editCostSchedule(file: IfcFile, cost_schedule: Entity, attributes: ApiData): void;
    editCostValue(file: IfcFile, cost_value: Entity, attributes: ApiData, options: IfcOpenShellCostEditCostValueOptions): void;
    editCostValueFormula(file: IfcFile, cost_value: Entity, formula: string): void;
    removeCostItem(file: IfcFile, cost_item: Entity): void;
    removeCostItemQuantity(file: IfcFile, cost_item: Entity, physical_quantity: Entity): void;
    removeCostSchedule(file: IfcFile, cost_schedule: Entity): void;
    removeCostValue(file: IfcFile, parent: Entity, cost_value: Entity): void;
    unassignCostItemQuantity(file: IfcFile, cost_item: Entity, products: Entity[], options: IfcOpenShellCostUnassignCostItemQuantityOptions): void;
}
export interface DocumentApi {
    /** Add a new document information to the project. */
    addInformation(file: IfcFile, options: IfcOpenShellDocumentAddInformationOptions): Entity;
    /** Create a new reference to a document. */
    addReference(file: IfcFile, information: Entity): Entity;
    /** Assign a document to a list of products. */
    assignDocument(file: IfcFile, options: IfcOpenShellDocumentAssignDocumentOptions): Entity;
    removeInformation(file: IfcFile, information: Entity): void;
    removeReference(file: IfcFile, reference: Entity): void;
    /** Unassign a document from a list of products. */
    unassignDocument(file: IfcFile, options: IfcOpenShellDocumentUnassignDocumentOptions): void;
}
export interface DrawingApi {
    assignProduct(file: IfcFile, options: IfcOpenShellDrawingAssignProductOptions): Entity;
    unassignProduct(file: IfcFile, options: IfcOpenShellDrawingUnassignProductOptions): void;
}
export interface ElementApi {
    getAggregate(instance: Entity): Entity | null;
    getContained(element: Entity): Entity[];
    getContainer(instance: Entity, options: IfcOpenShellElementGetContainerOptions): Entity | null;
    getControls(element: Entity): Entity[];
    getDecomposition(element: Entity, options: IfcOpenShellElementGetDecompositionOptions): Entity[];
    getElementsByLayer(layer: Entity): Entity[];
    getElementsByMaterial(material: Entity): Entity[];
    getElementsByProfile(profile: Entity): Entity[];
    getElementsByRepresentation(representation: Entity): Entity[];
    getElementsByStyle(style: Entity): Entity[];
    getFilledVoid(element: Entity): Entity | null;
    getGroups(element: Entity): Entity[];
    getLayers(element: Entity): Entity[];
    getMaterial(instance: Entity, options: IfcOpenShellElementGetMaterialOptions): Entity | null;
    getNest(instance: Entity): Entity | null;
    getOpenings(element: Entity): Entity[];
    getParent(instance: Entity): Entity | null;
    getParts(element: Entity): Entity[];
    getPsetIds(element: Entity, options: IfcOpenShellElementGetPsetIdsOptions): Entity[];
    getReferencedElements(reference: Entity): Entity[];
    getReferencedStructures(element: Entity): Entity[];
    getShapeAspects(element: Entity, options: IfcOpenShellElementGetShapeAspectsOptions): Entity[];
    getStructureReferencedElements(structure: Entity): Entity[];
    getStyles(element: Entity): Entity[];
    getType(instance: Entity): Entity | null;
    getTypes(type_element: Entity): Entity[];
    getVoidedElement(element: Entity): Entity | null;
    isUserdefinedType(element: Entity): boolean;
    removeDeep(element: Entity): void;
    replaceElement(old_element: Entity, new_element: Entity): void;
}
export interface EntityApi {
    removeDeep2(instance: Entity): void;
    removeDeep2Ex(instance: Entity, also_consider: Entity[], do_not_delete: Entity[]): void;
}
export interface FeatureApi {
    addFeature(file: IfcFile, options: IfcOpenShellFeatureAddFeatureOptions): Entity;
    addFilling(file: IfcFile, opening: Entity, element: Entity): Entity;
    removeFeature(file: IfcFile, options: IfcOpenShellFeatureRemoveFeatureOptions): void;
    removeFilling(file: IfcFile, element: Entity): void;
}
export interface GeometryApi {
    addAxisRepresentation(file: IfcFile, context: Entity, axis: number[][]): Entity;
    addBoolean(file: IfcFile, first_item: Entity, second_items: Entity[], operator_type: string): Entity[];
    addDoorRepresentation(file: IfcFile, options: IfcOpenShellGeometryAddDoorRepresentationOptions): Entity;
    addFootprintRepresentation(file: IfcFile, context: Entity, curves: Entity[]): Entity;
    addMeshRepresentation(file: IfcFile, context: Entity, options: IfcOpenShellGeometryAddMeshRepresentationOptions): Entity;
    addRailingRepresentation(file: IfcFile, options: IfcOpenShellGeometryAddRailingRepresentationOptions): Entity;
    addShapeAspect(file: IfcFile, options: IfcOpenShellGeometryAddShapeAspectOptions): Entity;
    addSlabRepresentation(file: IfcFile, options: IfcOpenShellGeometryAddSlabRepresentationOptions): Entity;
    addTopologyRepresentation(file: IfcFile, options: IfcOpenShellGeometryAddTopologyRepresentationOptions): Entity;
    addWallRepresentation(file: IfcFile, options: IfcOpenShellGeometryAddWallRepresentationOptions): Entity;
    addWindowRepresentation(file: IfcFile, options: IfcOpenShellGeometryAddWindowRepresentationOptions): Entity;
    assignRepresentation(file: IfcFile, product: Entity, representation: Entity): Entity;
    clipSolid(file: IfcFile, options: IfcOpenShellGeometryClipSolidOptions): Entity;
    clipSolidBounded(file: IfcFile, options: IfcOpenShellGeometryClipSolidBoundedOptions): Entity;
    connectElement(file: IfcFile, options: IfcOpenShellGeometryConnectElementOptions): Entity;
    connectPath(file: IfcFile, options: IfcOpenShellGeometryConnectPathOptions): Entity;
    connectWall(file: IfcFile, options: IfcOpenShellGeometryConnectWallOptions): Entity;
    copyRepresentation(file: IfcFile, options: IfcOpenShellGeometryCopyRepresentationOptions): Entity;
    create2ptWall(file: IfcFile, options: IfcOpenShellGeometryCreate2PtWallOptions): Entity;
    disconnectElement(file: IfcFile, relating_element: Entity, related_element: Entity): void;
    disconnectPath(file: IfcFile, options: IfcOpenShellGeometryDisconnectPathOptions): void;
    editObjectPlacement(file: IfcFile, options: IfcOpenShellGeometryEditObjectPlacementOptions): Entity;
    mapRepresentation(file: IfcFile, representation: Entity): Entity;
    profileExtents(file: IfcFile, profile: Entity): number[];
    regenerateWallRepresentation(file: IfcFile, options: IfcOpenShellGeometryRegenerateWallRepresentationOptions): Entity;
    removeBoolean(file: IfcFile, item: Entity): void;
    removeRepresentation(file: IfcFile, representation: Entity, options: IfcOpenShellGeometryRemoveRepresentationOptions): void;
    unassignRepresentation(file: IfcFile, product: Entity, representation: Entity): void;
    validateType(file: IfcFile, representation: Entity, options: IfcOpenShellGeometryValidateTypeOptions): boolean;
}
export interface GeoreferenceApi {
    addGeoreferencing(file: IfcFile, options: IfcOpenShellGeoreferenceAddGeoreferencingOptions): void;
    editGeoreferencing(file: IfcFile, options: IfcOpenShellGeoreferenceEditGeoreferencingOptions): void;
    editTrueNorth(file: IfcFile, options: IfcOpenShellGeoreferenceEditTrueNorthOptions): void;
    editWcs(file: IfcFile, options: IfcOpenShellGeoreferenceEditWcsOptions): void;
    removeGeoreferencing(file: IfcFile): void;
}
export interface GridApi {
    createAxisCurve(file: IfcFile, p1: number[], p2: number[], grid_axis: Entity, is_si: boolean): void;
    createGridAxis(file: IfcFile, grid: Entity, axis_tag: string, same_sense: boolean, uvw_axes: string): Entity;
    removeGridAxis(file: IfcFile, axis: Entity): void;
}
export interface GroupApi {
    /** Create a new IfcGroup with generated identity and ownership metadata. */
    addGroup(file: IfcFile, options: IfcOpenShellGroupAddGroupOptions): Entity;
    /** Assign products to a group, merging into an existing relationship when present. */
    assignGroup(file: IfcFile, options: IfcOpenShellGroupAssignGroupOptions): Entity;
    /** Remove a group and its relationships. */
    removeGroup(file: IfcFile, group: Entity): void;
    /** Remove products from a group relationship. */
    unassignGroup(file: IfcFile, options: IfcOpenShellGroupUnassignGroupOptions): void;
    /** Replace the product list of an existing group relationship. */
    updateGroupProducts(file: IfcFile, options: IfcOpenShellGroupUpdateGroupProductsOptions): Entity;
}
export interface GuidApi {
    compress(uuid_hex: string): string;
    expand(guid: string): string;
    "new"(): string;
}
export interface LayerApi {
    addLayer(file: IfcFile, name: string): Entity;
    addLayerWithStyle(file: IfcFile, name: string, on: boolean, frozen: boolean, blocked: boolean, styles: Entity[]): Entity;
    assignLayer(file: IfcFile, items: Entity[], layer: Entity): void;
    removeLayer(file: IfcFile, layer: Entity): void;
    unassignLayer(file: IfcFile, items: Entity[], layer: Entity): void;
}
export interface LibraryApi {
    addLibrary(file: IfcFile, name: string): Entity;
    addReference(file: IfcFile, library: Entity): Entity;
    /** Assign a library reference to a list of products. */
    assignReference(file: IfcFile, options: IfcOpenShellLibraryAssignReferenceOptions): Entity;
    removeLibrary(file: IfcFile, library: Entity): void;
    removeReference(file: IfcFile, reference: Entity): void;
    /** Unassign a library reference from a list of products. */
    unassignReference(file: IfcFile, options: IfcOpenShellLibraryUnassignReferenceOptions): void;
}
export interface MaterialApi {
    addConstituent(file: IfcFile, constituent_set: Entity, options: IfcOpenShellMaterialAddConstituentOptions): Entity;
    addLayer(file: IfcFile, layer_set: Entity, options: IfcOpenShellMaterialAddLayerOptions): Entity;
    addListItem(file: IfcFile, material_list: Entity, material: Entity): void;
    addMaterial(file: IfcFile, options: IfcOpenShellMaterialAddMaterialOptions): Entity;
    addMaterialSet(file: IfcFile, options: IfcOpenShellMaterialAddMaterialSetOptions): Entity;
    addProfile(file: IfcFile, profile_set: Entity, options: IfcOpenShellMaterialAddProfileOptions): Entity;
    assignMaterial(file: IfcFile, products: Entity[], options: IfcOpenShellMaterialAssignMaterialOptions): Entity[];
    assignProfile(file: IfcFile, material_profile: Entity, profile: Entity): void;
    editProfileUsage(file: IfcFile, usage: Entity, options: IfcOpenShellMaterialEditProfileUsageOptions): void;
    removeConstituent(file: IfcFile, constituent: Entity, options: IfcOpenShellMaterialRemoveItemOptions): void;
    removeLayer(file: IfcFile, layer: Entity, options: IfcOpenShellMaterialRemoveItemOptions): void;
    removeListItem(file: IfcFile, material_list: Entity, options: IfcOpenShellMaterialRemoveListItemOptions): void;
    removeMaterial(file: IfcFile, material: Entity): void;
    removeMaterialSet(file: IfcFile, material: Entity): void;
    removeProfile(file: IfcFile, profile: Entity, options: IfcOpenShellMaterialRemoveProfileOptions): void;
    reorderSetItem(file: IfcFile, material_set: Entity, options: IfcOpenShellMaterialReorderSetItemOptions): void;
    unassignMaterial(file: IfcFile, products: Entity[], options: IfcOpenShellMaterialUnassignMaterialOptions): void;
}
export interface NestApi {
    /** Assign objects as nested children of a parent host. */
    assignObject(file: IfcFile, options: IfcOpenShellNestAssignObjectOptions): Entity;
    /** Remove objects from their nesting relationships. */
    unassignObject(file: IfcFile, options: IfcOpenShellNestUnassignObjectOptions): void;
}
export interface OwnerApi {
    addActor(file: IfcFile, options: IfcOpenShellOwnerAddActorOptions): Entity;
    addAddress(file: IfcFile, assigned_object: Entity, ifc_class: string): Entity;
    addApplication(file: IfcFile, options: IfcOpenShellOwnerAddApplicationOptions): Entity;
    addOrganisation(file: IfcFile, identification: string, name: string): Entity;
    addPerson(file: IfcFile, identification: string, family_name: string, given_name: string): Entity;
    addPersonAndOrganisation(file: IfcFile, person: Entity, organisation: Entity): Entity;
    addRole(file: IfcFile, assigned_object: Entity, role: string): Entity;
    assignActor(file: IfcFile, options: IfcOpenShellOwnerAssignActorOptions): Entity;
    createOwnerHistory(file: IfcFile, options: IfcOpenShellOwnerCreateOwnerHistoryOptions): Entity;
    removeActor(file: IfcFile, actor: Entity): void;
    removeAddress(file: IfcFile, address: Entity): void;
    removeApplication(file: IfcFile, application: Entity): void;
    removeOrganisation(file: IfcFile, organisation: Entity): void;
    removePerson(file: IfcFile, person: Entity): void;
    removePersonAndOrganisation(file: IfcFile, person_and_organisation: Entity): void;
    removeRole(file: IfcFile, role: Entity): void;
    unassignActor(file: IfcFile, options: IfcOpenShellOwnerUnassignActorOptions): void;
    updateOwnerHistory(file: IfcFile, options: IfcOpenShellOwnerUpdateOwnerHistoryOptions): Entity;
}
export interface PlacementApi {
    a2p(origin: number[], z_axis: number[], x_axis: number[]): number[];
    getAxis2placement(instance: Entity): number[];
    getCartesianXform3d(instance: Entity): number[];
    getLocalPlacement(instance: Entity): number[];
    getMappeditemXform(instance: Entity): number[];
    getStoreyElevation(instance: Entity): number;
    rotation(angle_rad: number, axis: string): number[];
}
export interface ProfileApi {
    addArbitraryProfile(file: IfcFile, options: IfcOpenShellProfileAddArbitraryProfileOptions): Entity;
    addArbitraryProfileWithVoids(file: IfcFile, options: IfcOpenShellProfileAddArbitraryProfileWithVoidsOptions): Entity;
    addParameterizedProfile(file: IfcFile, ifc_class: string, profile_type: string): Entity;
    copyProfile(file: IfcFile, profile: Entity): Entity;
    editProfile(profile: Entity, attributes: ApiData): void;
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
     */
    addPset(file: IfcFile, options: IfcOpenShellPsetAddPsetOptions): Entity;
    addQto(file: IfcFile, options: IfcOpenShellPsetAddQtoOptions): Entity;
    assignPset(file: IfcFile, options: IfcOpenShellPsetAssignPsetOptions): Entity;
    editPset(file: IfcFile, options: IfcOpenShellPsetEditPsetOptions): boolean;
    editQto(file: IfcFile, options: IfcOpenShellPsetEditQtoOptions): boolean;
    removePset(file: IfcFile, product: Entity, pset: Entity): void;
    templateAddPropTemplate(file: IfcFile, pset_template: Entity, name: string, description: string, template_type: string, primary_measure_type: string): Entity;
    templateAddPsetTemplate(file: IfcFile, name: string, template_type: string, applicable_entity: string): Entity;
    templateCreateFromFiles(schema_identifier: string, template_files: IfcFile[]): PsetTemplate | null;
    templateGetApplicable(pqt: PsetTemplate, ifc_class: string, predefined_type: string, pset_only: boolean, qto_only: boolean, schema_name: string): Entity[];
    templateGetApplicableNames(pqt: PsetTemplate, ifc_class: string, predefined_type: string, pset_only: boolean, qto_only: boolean, schema_name: string): string[];
    templateGetByName(pqt: PsetTemplate, name: string): Entity;
    templateGetTemplate(schema_identifier: string): PsetTemplate | null;
    templateIsTemplated(pqt: PsetTemplate, name: string): boolean;
    templatePsetType(pset_template: Entity): string;
    templateRemovePropTemplate(file: IfcFile, prop_template: Entity): void;
    templateRemovePsetTemplate(pset_template: Entity): void;
    templateSetTemplateDir(dir: string): void;
    unassignPset(file: IfcFile, products: Entity[], pset: Entity): void;
    unsharePset(file: IfcFile, options: IfcOpenShellPsetUnsharePsetOptions): Entity[];
}
export interface RegisterApi {
    scratchFile(schema_name: string, file: IfcFile): boolean;
}
export interface RepresentationApi {
    getContext(file: IfcFile, context_type: string, subcontext: string, target_view: string): Entity;
    getPrioritisedContexts(file: IfcFile): Entity[];
    getProductRepresentation(element: Entity, options: IfcOpenShellRepresentationGetProductRepresentationOptions): Entity;
    resolve(representation: Entity): Entity;
    resolveBaseItems(representation: Entity): Entity[];
}
export interface ResourceApi {
    editResourceTime(file: IfcFile, resource_time: Entity, attributes: ApiData): void;
}
export interface RootApi {
    /**
     * Create an IFC entity with generated identity, ownership metadata, optional
     * name, and optional predefined type.
     */
    createEntity(file: IfcFile, options: IfcOpenShellRootCreateEntityOptions): Entity;
    removeProduct(file: IfcFile, product: Entity, options: IfcOpenShellRootRemoveProductOptions): void;
}
export interface SchemaApi {
    reassignClass(file: IfcFile, element: Entity, new_class: string): Entity;
}
export interface SelectorApi {
    filterAll(file: IfcFile, query: string): ValueData | null;
    filterElements(file: IfcFile, query: string, elements: Entity[]): ValueData | null;
    format(file: IfcFile, instance: Entity, query: string): string | null;
    getElementValue(file: IfcFile, element: Entity, query: string): ValueData | null;
    setElementValue(file: IfcFile, element: Entity, query: string, value: ValueInput | null, concat: string): void;
}
export interface SequenceApi {
    addDateTime(file: IfcFile, date_time: string): Entity | string;
    addTask(file: IfcFile, options: IfcOpenShellSequenceAddTaskOptions): Entity;
    addTaskTime(file: IfcFile, task: Entity, options: IfcOpenShellSequenceAddTaskTimeOptions): Entity;
    addTimePeriod(file: IfcFile, recurrence_pattern: Entity, options: IfcOpenShellSequenceAddTimePeriodOptions): Entity;
    addWorkCalendar(file: IfcFile, options: IfcOpenShellSequenceAddWorkCalendarOptions): Entity;
    addWorkPlan(file: IfcFile, options: IfcOpenShellSequenceAddWorkPlanOptions): Entity;
    addWorkSchedule(file: IfcFile, options: IfcOpenShellSequenceAddWorkScheduleOptions): Entity;
    addWorkTime(file: IfcFile, work_calendar: Entity, time_type: string): Entity;
    assignLagTime(file: IfcFile, rel_sequence: Entity, lag_value: string, options: IfcOpenShellSequenceAssignLagTimeOptions): Entity;
    assignProcess(file: IfcFile, relating_process: Entity, related_object: Entity, options: IfcOpenShellSequenceAssignProcessOptions): Entity;
    assignProduct(file: IfcFile, relating_product: Entity, related_object: Entity, options: IfcOpenShellSequenceAssignProductOptions): Entity;
    assignRecurrencePattern(file: IfcFile, parent: Entity, recurrence_type: string): Entity;
    assignSequence(file: IfcFile, relating_process: Entity, related_process: Entity, options: IfcOpenShellSequenceAssignSequenceOptions): Entity;
    assignWorkPlan(file: IfcFile, work_schedule: Entity, work_plan: Entity, options: IfcOpenShellSequenceAssignWorkPlanOptions): Entity;
    calculateTaskDuration(file: IfcFile, task: Entity): void;
    cascadeSchedule(file: IfcFile, task: Entity): void;
    copyWorkSchedule(file: IfcFile, work_schedule: Entity, options: IfcOpenShellSequenceCopyWorkScheduleOptions): Entity;
    createBaseline(file: IfcFile, work_schedule: Entity, options: IfcOpenShellSequenceCreateBaselineOptions): void;
    duplicateTask(file: IfcFile, task: Entity, options: IfcOpenShellSequenceDuplicateTaskOptions): IfcOpenShellSequenceDuplicateTaskResult;
    editLagTime(lag_time: Entity, attributes: ApiData): void;
    editRecurrencePattern(recurrence_pattern: Entity, attributes: ApiData): void;
    editSequence(rel_sequence: Entity, attributes: ApiData): void;
    editTask(task: Entity, attributes: ApiData): void;
    editTaskTime(file: IfcFile, task_time: Entity, attributes: ApiData): void;
    editWorkCalendar(work_calendar: Entity, attributes: ApiData): void;
    editWorkPlan(work_plan: Entity, attributes: ApiData): void;
    editWorkSchedule(work_schedule: Entity, attributes: ApiData): void;
    editWorkTime(work_time: Entity, attributes: ApiData): void;
    recalculateSchedule(file: IfcFile, work_schedule: Entity): void;
    removeTask(file: IfcFile, task: Entity, options: IfcOpenShellSequenceRemoveOptions): void;
    removeTimePeriod(file: IfcFile, time_period: Entity): void;
    removeWorkCalendar(file: IfcFile, work_calendar: Entity, options: IfcOpenShellSequenceRemoveOptions): void;
    removeWorkPlan(file: IfcFile, work_plan: Entity, options: IfcOpenShellSequenceRemoveOptions): void;
    removeWorkSchedule(file: IfcFile, work_schedule: Entity, options: IfcOpenShellSequenceRemoveOptions): void;
    removeWorkTime(file: IfcFile, work_time: Entity): void;
    unassignLagTime(file: IfcFile, rel_sequence: Entity): void;
    unassignProcess(file: IfcFile, relating_process: Entity, related_object: Entity, options: IfcOpenShellSequenceRemoveOptions): Entity;
    unassignProduct(file: IfcFile, relating_product: Entity, related_object: Entity, options: IfcOpenShellSequenceRemoveOptions): Entity;
    unassignRecurrencePattern(file: IfcFile, recurrence_pattern: Entity): void;
    unassignSequence(file: IfcFile, relating_process: Entity, related_process: Entity): void;
}
export interface ShapeApi {
    builderAxis2Placement2d(file: IfcFile, options: IfcOpenShellShapeBuilderAxis2Placement2dOptions): Entity;
    builderAxis2Placement3d(file: IfcFile, options: IfcOpenShellShapeBuilderAxis2Placement3dOptions): Entity;
    builderBlock(file: IfcFile, options: IfcOpenShellShapeBuilderBlockOptions): Entity;
    builderCircle(file: IfcFile, center: number[], radius: number): Entity;
    builderCurveBetweenTwoPoints(file: IfcFile, points: number[][]): Entity;
    builderDeepCopy(file: IfcFile, element: Entity): Entity;
    builderEdge(file: IfcFile, start: number[], end: number[]): Entity;
    builderEllipseCurve(file: IfcFile, options: IfcOpenShellShapeBuilderEllipseCurveOptions): Entity;
    builderExtrude(file: IfcFile, options: IfcOpenShellShapeBuilderExtrudeOptions): Entity;
    builderFace(file: IfcFile, points: number[][]): Entity;
    builderFacetedBrep(file: IfcFile, points: number[][], faces: number[][]): Entity;
    builderGetPolylineCoords(polyline: Entity): number[][];
    builderHalfSpaceSolid(file: IfcFile, options: IfcOpenShellShapeBuilderHalfSpaceSolidOptions): Entity;
    builderIndexedPolycurve2d(file: IfcFile, points: number[][], segments: number[][]): Entity;
    builderMepBendShape(file: IfcFile, options: IfcOpenShellShapeBuilderMepBendShapeOptions): IfcOpenShellShapeBuilderMepBendShapeResult;
    builderMepTransitionCalculate(options: IfcOpenShellShapeBuilderMepTransitionCalculateOptions): number;
    builderMepTransitionLength(options: IfcOpenShellShapeBuilderMepTransitionLengthOptions): number;
    builderMepTransitionShape(file: IfcFile, options: IfcOpenShellShapeBuilderMepTransitionShapeOptions): IfcOpenShellShapeBuilderMepTransitionShapeResult | null;
    builderMesh(file: IfcFile, points: number[][], faces: number[][]): Entity;
    builderMirror(file: IfcFile, options: IfcOpenShellShapeBuilderMirrorOptions): Entity;
    builderPlane(file: IfcFile, location: number[], normal: number[]): Entity;
    builderPolygonalFaceSet(file: IfcFile, points: number[][], faces: number[][][]): Entity;
    builderPolyline(file: IfcFile, options: IfcOpenShellShapeBuilderPolylineOptions): Entity;
    builderProfile(file: IfcFile, options: IfcOpenShellShapeBuilderProfileOptions): Entity;
    builderRepresentation(file: IfcFile, options: IfcOpenShellShapeBuilderRepresentationOptions): Entity;
    builderRotate(file: IfcFile, options: IfcOpenShellShapeBuilderRotateOptions): Entity;
    builderSetPolylineCoords(file: IfcFile, polyline: Entity, coords: number[][]): Entity;
    builderSphere(file: IfcFile, options: IfcOpenShellShapeBuilderSphereOptions): Entity;
    builderSweptDiskSolid(file: IfcFile, path_curve: Entity, radius: number): Entity;
    builderTranslate(file: IfcFile, options: IfcOpenShellShapeBuilderTranslateOptions): Entity;
    builderTriangulatedFaceSet(file: IfcFile, points: number[][], faces: number[][]): Entity;
    builderVertex(file: IfcFile, position: number[]): Entity;
    isX(value: number, x: number, tolerance: number): boolean;
}
export interface SpatialApi {
    /** Assign products to be contained hierarchically in a spatial structure. */
    assignContainer(file: IfcFile, options: IfcOpenShellSpatialAssignContainerOptions): Entity;
    /** Dereference products from a spatial structure. */
    dereferenceStructure(file: IfcFile, options: IfcOpenShellSpatialDereferenceStructureOptions): void;
    /** Reference products in a spatial structure without containment. */
    referenceStructure(file: IfcFile, options: IfcOpenShellSpatialReferenceStructureOptions): Entity;
    /** Remove products from their spatial containment relationships. */
    unassignContainer(file: IfcFile, options: IfcOpenShellSpatialUnassignContainerOptions): void;
}
export interface StructuralApi {
    addStructuralActivity(file: IfcFile, applied_load: Entity, structural_member: Entity, ifc_class: string, predefined_type: string, global_or_local: string, options: IfcOpenShellStructuralAddStructuralActivityOptions): Entity;
    addStructuralAnalysisModel(file: IfcFile, owner_history: Entity): Entity;
    addStructuralBoundaryCondition(file: IfcFile, ifc_class: string, options: IfcOpenShellStructuralAddStructuralBoundaryConditionOptions): Entity;
    addStructuralLoad(file: IfcFile, ifc_class: string, name: string): Entity;
    addStructuralLoadCase(file: IfcFile, name: string, action_type: string, action_source: string, owner_history: Entity): Entity;
    addStructuralLoadGroup(file: IfcFile, name: string, action_type: string, action_source: string, owner_history: Entity): Entity;
    addStructuralMemberConnection(file: IfcFile, relating_structural_member: Entity, related_structural_connection: Entity, owner_history: Entity): Entity;
    assignProduct(file: IfcFile, relating_product: Entity, related_object: Entity, owner_history: Entity): Entity;
    assignStructuralAnalysisModel(file: IfcFile, products: Entity[], structural_analysis_model: Entity, options: IfcOpenShellStructuralAssignStructuralAnalysisModelOptions): Entity;
    assignToBuilding(file: IfcFile, structural_analysis_model: Entity, building: Entity, owner_history: Entity): Entity;
    editStructuralBoundaryCondition(file: IfcFile, condition: Entity, attributes: ApiData): void;
    editStructuralConnectionCs(file: IfcFile, structural_item: Entity, axis: number[], ref_direction: number[]): void;
    editStructuralItemAxis(file: IfcFile, structural_item: Entity, axis: number[]): void;
    removeStructuralAnalysisModel(file: IfcFile, structural_analysis_model: Entity): void;
    removeStructuralBoundaryCondition(file: IfcFile, options: IfcOpenShellStructuralRemoveStructuralBoundaryConditionOptions): void;
    removeStructuralConnectionCondition(file: IfcFile, relation: Entity): void;
    removeStructuralLoad(file: IfcFile, structural_load: Entity): void;
    removeStructuralLoadCase(file: IfcFile, structural_load_case: Entity): void;
    removeStructuralLoadGroup(file: IfcFile, structural_load_group: Entity): void;
    unassignStructuralAnalysisModel(file: IfcFile, products: Entity[], structural_analysis_model: Entity, options: IfcOpenShellStructuralUnassignStructuralAnalysisModelOptions): void;
}
export interface StyleApi {
    addStyle(file: IfcFile, name: string, ifc_class: string): Entity;
    assignItemStyle(file: IfcFile, options: IfcOpenShellStyleAssignItemStyleOptions): Entity;
    assignMaterialStyle(file: IfcFile, material: Entity, style: Entity, context: Entity, should_use_presentation_style_assignment: boolean): void;
    assignRepresentationStyles(file: IfcFile, shape_representation: Entity, styles: Entity[], should_use_presentation_style_assignment: boolean, replace_previous_same_type_style: boolean): Entity[];
    editSurfaceStyle(file: IfcFile, style: Entity, attributes: ApiData): void;
    removeStyle(file: IfcFile, style: Entity): void;
    removeStyledRepresentation(file: IfcFile, representation: Entity): void;
    removeSurfaceStyle(file: IfcFile, style: Entity): void;
    unassignMaterialStyle(file: IfcFile, material: Entity, style: Entity, context: Entity): void;
    unassignRepresentationStyles(file: IfcFile, shape_representation: Entity, styles: Entity[], should_use_presentation_style_assignment: boolean): void;
}
export interface SystemApi {
    /** Create a new distribution port and optionally assign it to an element. */
    addPort(file: IfcFile, options: IfcOpenShellSystemAddPortOptions): Entity;
    /** Create a new system with the given IFC class. */
    addSystem(file: IfcFile, options: IfcOpenShellSystemAddSystemOptions): Entity;
    /** Assign a flow control element to a flow element. */
    assignFlowControl(file: IfcFile, options: IfcOpenShellSystemAssignFlowControlOptions): Entity;
    /** Assign a port to an element. */
    assignPort(file: IfcFile, options: IfcOpenShellSystemAssignPortOptions): Entity;
    /** Assign products to a system. */
    assignSystem(file: IfcFile, options: IfcOpenShellSystemAssignSystemOptions): Entity;
    /** Connect two ports together with a given flow direction. */
    connectPort(file: IfcFile, options: IfcOpenShellSystemConnectPortOptions): void;
    /** Disconnect a port from all connected ports. */
    disconnectPort(file: IfcFile, port: Entity): void;
    /** Remove a system and its relationships. */
    removeSystem(file: IfcFile, system: Entity): void;
    /** Unassign a flow control element from a flow element. */
    unassignFlowControl(file: IfcFile, options: IfcOpenShellSystemUnassignFlowControlOptions): void;
    /** Unassign a port from an element. */
    unassignPort(file: IfcFile, options: IfcOpenShellSystemUnassignPortOptions): void;
    /** Unassign products from a system. */
    unassignSystem(file: IfcFile, options: IfcOpenShellSystemUnassignSystemOptions): void;
}
export interface TypeApi {
    /** Assign a type to element occurrences, creating or merging IfcRelDefinesByType. */
    assignType(file: IfcFile, options: IfcOpenShellTypeAssignTypeOptions): Entity;
    /** Map representation maps from a type onto a related object. */
    mapTypeRepresentations(file: IfcFile, related_object: Entity, relating_type: Entity): boolean;
    /** Remove type assignments from element occurrences. */
    unassignType(file: IfcFile, options: IfcOpenShellTypeUnassignTypeOptions): void;
}
export interface UnitApi {
    addContextDependentUnit(file: IfcFile, unit_type: string, name: string, dimensions: bigint[]): Entity;
    addDerivedUnit(file: IfcFile, unit_type: string, userdefinedtype: string, units: Entity[], exponents: bigint[]): Entity;
    addMonetaryUnit(file: IfcFile, currency: string): Entity;
    addSiUnit(file: IfcFile, unit_type: string, prefix: string): Entity;
    calculateUnitScale(file: IfcFile, unit_type: string): number;
    convert(value: number, from_prefix: string, from_unit: string, to_prefix: string, to_unit: string): number;
    convertUnit(value: number, from_unit: Entity, to_unit: Entity): number;
    formatLength(value: number, precision: number, decimal_places: number, suppress_zero_inches: boolean, unit_system: string, input_unit: string, output_unit: string): string;
    getFullUnitName(unit: Entity): string;
    getMeasureClass(unit_type: string): string;
    getMeasureUnitType(measure_class: string): string;
    getNamedDimensions(name: string): number[];
    getPrefix(text: string): string;
    getPrefixMultiplier(text: string): number;
    getProjectUnit(file: IfcFile, unit_type: string): Entity;
    getSiDimensions(name: string): number[];
    getSymbolMeasureClass(symbol: string): string;
    getSymbolQuantityClass(symbol: string): string;
    getUnitAssignment(file: IfcFile): Entity;
    getUnitName(text: string): string;
    getUnitNameUniversal(text: string): string;
    getUnitSymbol(unit: Entity): string;
    removeUnit(file: IfcFile, unit: Entity): void;
    resolvePropertyMeasureClass(prop: Entity): string;
    resolvePropertyTableDefinedMeasureClass(prop: Entity): string;
    resolvePropertyTableDefinedUnit(prop: Entity): Entity;
    resolvePropertyTableDefiningMeasureClass(prop: Entity): string;
    resolvePropertyTableDefiningUnit(prop: Entity): Entity;
    resolvePropertyUnit(prop: Entity): Entity;
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

export function createApi(shell: IfcOpenShell): Api {
  const raw = shell.raw as object as RawApi;
  return Object.freeze({
    aggregate: Object.freeze({
    /** Assign products as parts of an aggregate object. */
    assignObject(file: IfcFile, options: IfcOpenShellAggregateAssignObjectOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.aggregate.assignObject(file.raw, encodeOptions(options, {"application": "application", "ownerHistory": "owner_history", "products": "products", "relatingObject": "relating_object", "user": "user"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /** Remove products from their aggregate relationships. */
    unassignObject(file: IfcFile, options: IfcOpenShellAggregateUnassignObjectOptions): void {
      const temps: Disposable[] = [];
      try {
        raw.aggregate.unassignObject(file.raw, encodeOptions(options, {"application": "application", "products": "products", "user": "user"}, shell, temps));
      } finally {
        disposeAll(temps);
      }
    },
    }),
    attribute: Object.freeze({
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
    copyBoundary(file: IfcFile, boundary: Entity): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.boundary.copyBoundary(file.raw, boundary.raw);
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    editAttributes(entity: Entity, options: IfcOpenShellBoundaryEditAttributesOptions): void {
      const temps: Disposable[] = [];
      try {
        raw.boundary.editAttributes(entity.raw, encodeOptions(options, {"correspondingBoundary": "corresponding_boundary", "internalOrExternal": "internal_or_external", "parentBoundary": "parent_boundary", "physicalOrVirtual": "physical_or_virtual", "relatedBuildingElement": "related_building_element", "relatingSpace": "relating_space"}, shell, temps));
      } finally {
        disposeAll(temps);
      }
    },
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
     * If an existing reference handle is provided, it is used directly.
     * Otherwise, a new IfcClassificationReference is created using the
     * optional identification, name, and classification fields.
     */
    addReference(file: IfcFile, options: IfcOpenShellClassificationAddReferenceOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.classification.addReference(file.raw, encodeOptions(options, {"application": "application", "classification": "classification", "identification": "identification", "name": "name", "ownerHistory": "owner_history", "products": "products", "reference": "reference", "user": "user"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    getReferences(element: Entity, should_inherit: boolean): Entity[] {
      const temps: Disposable[] = [];
      try {
        const result = raw.classification.getReferences(element.raw, should_inherit);
        return wrapEntities(shell, result as never) as Entity[];
      } finally {
        disposeAll(temps);
      }
    },
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
        raw.classification.removeReference(file.raw, encodeOptions(options, {"application": "application", "products": "products", "reference": "reference", "user": "user"}, shell, temps));
      } finally {
        disposeAll(temps);
      }
    },
    }),
    cogo: Object.freeze({
    /** Add a single survey point as an IfcAnnotation. */
    addSurveyPoint(file: IfcFile, options: IfcOpenShellCogoAddSurveyPointOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.cogo.addSurveyPoint(file.raw, encodeOptions(options, {"application": "application", "ownerHistory": "owner_history", "site": "site", "surveyPoint": "survey_point", "user": "user"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    assignSurveyPoint(annotation: Entity, survey_point: Entity): void {
      const temps: Disposable[] = [];
      try {
        raw.cogo.assignSurveyPoint(annotation.raw, survey_point.raw);
      } finally {
        disposeAll(temps);
      }
    },
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
    addMetricReference(file: IfcFile, metric: Entity, reference_path: string): Entity[] {
      const temps: Disposable[] = [];
      try {
        const result = raw.constraint.addMetricReference(file.raw, metric.raw, reference_path);
        return wrapEntities(shell, result as never) as Entity[];
      } finally {
        disposeAll(temps);
      }
    },
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
        const result = raw.constraint.assignConstraint(file.raw, encodeOptions(options, {"application": "application", "constraint": "constraint", "ownerHistory": "owner_history", "products": "products", "user": "user"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    removeConstraint(file: IfcFile, constraint: Entity): void {
      const temps: Disposable[] = [];
      try {
        raw.constraint.removeConstraint(file.raw, constraint.raw);
      } finally {
        disposeAll(temps);
      }
    },
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
        raw.constraint.unassignConstraint(file.raw, encodeOptions(options, {"application": "application", "constraint": "constraint", "products": "products", "user": "user"}, shell, temps));
      } finally {
        disposeAll(temps);
      }
    },
    }),
    context: Object.freeze({
    addContext(file: IfcFile, options: IfcOpenShellContextAddContextOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.context.addContext(file.raw, encodeOptions(options, {"contextIdentifier": "context_identifier", "contextType": "context_type", "parent": "parent", "targetScale": "target_scale", "targetView": "target_view"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    editContext(file: IfcFile, context: Entity, attributes: ApiData): void {
      const temps: Disposable[] = [];
      try {
        raw.context.editContext(file.raw, context.raw, attributes);
      } finally {
        disposeAll(temps);
      }
    },
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
    /** Assign a planning control or constraint to a list of objects. */
    assignControl(file: IfcFile, options: IfcOpenShellControlAssignControlOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.control.assignControl(file.raw, encodeOptions(options, {"application": "application", "ownerHistory": "owner_history", "relatedObjects": "related_objects", "relatingControl": "relating_control", "user": "user"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /** Remove a planning control or constraint from a list of objects. */
    unassignControl(file: IfcFile, options: IfcOpenShellControlUnassignControlOptions): void {
      const temps: Disposable[] = [];
      try {
        raw.control.unassignControl(file.raw, encodeOptions(options, {"application": "application", "relatedObjects": "related_objects", "relatingControl": "relating_control", "user": "user"}, shell, temps));
      } finally {
        disposeAll(temps);
      }
    },
    }),
    cost: Object.freeze({
    addCostItem(file: IfcFile, options: IfcOpenShellCostAddCostItemOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.cost.addCostItem(file.raw, encodeOptions(options, {"application": "application", "costItem": "cost_item", "costSchedule": "cost_schedule", "ownerHistory": "owner_history", "user": "user"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    addCostItemQuantity(file: IfcFile, cost_item: Entity, ifc_class: string): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.cost.addCostItemQuantity(file.raw, cost_item.raw, ifc_class);
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    addCostSchedule(file: IfcFile, name: string, predefined_type: string, update_date: string, owner_history: Entity): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.cost.addCostSchedule(file.raw, name, predefined_type, update_date, owner_history.raw);
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    addCostValue(file: IfcFile, parent: Entity): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.cost.addCostValue(file.raw, parent.raw);
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    assignCostItemQuantity(file: IfcFile, cost_item: Entity, products: Entity[], prop_name: string, options: IfcOpenShellCostAssignCostItemQuantityOptions): void {
      const temps: Disposable[] = [];
      try {
        raw.cost.assignCostItemQuantity(file.raw, cost_item.raw, toRaw(products, shell, temps), prop_name, encodeOptions(options, {"application": "application", "ownerHistory": "owner_history", "user": "user"}, shell, temps));
      } finally {
        disposeAll(temps);
      }
    },
    assignCostValue(file: IfcFile, cost_item: Entity, cost_rate: Entity): void {
      const temps: Disposable[] = [];
      try {
        raw.cost.assignCostValue(file.raw, cost_item.raw, cost_rate.raw);
      } finally {
        disposeAll(temps);
      }
    },
    calculateCostItemResourceValue(file: IfcFile, cost_item: Entity): void {
      const temps: Disposable[] = [];
      try {
        raw.cost.calculateCostItemResourceValue(file.raw, cost_item.raw);
      } finally {
        disposeAll(temps);
      }
    },
    copyCostItem(file: IfcFile, cost_item: Entity): Entity[] {
      const temps: Disposable[] = [];
      try {
        const result = raw.cost.copyCostItem(file.raw, cost_item.raw);
        return wrapEntities(shell, result as never) as Entity[];
      } finally {
        disposeAll(temps);
      }
    },
    copyCostItemValues(file: IfcFile, source: Entity, destination: Entity): void {
      const temps: Disposable[] = [];
      try {
        raw.cost.copyCostItemValues(file.raw, source.raw, destination.raw);
      } finally {
        disposeAll(temps);
      }
    },
    copyCostSchedule(file: IfcFile, cost_schedule: Entity, options: IfcOpenShellCostCopyCostScheduleOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.cost.copyCostSchedule(file.raw, cost_schedule.raw, encodeOptions(options, {"application": "application", "ownerHistory": "owner_history", "user": "user"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    editCostItem(file: IfcFile, cost_item: Entity, attributes: ApiData): void {
      const temps: Disposable[] = [];
      try {
        raw.cost.editCostItem(file.raw, cost_item.raw, attributes);
      } finally {
        disposeAll(temps);
      }
    },
    editCostItemQuantity(file: IfcFile, physical_quantity: Entity, attributes: ApiData): void {
      const temps: Disposable[] = [];
      try {
        raw.cost.editCostItemQuantity(file.raw, physical_quantity.raw, attributes);
      } finally {
        disposeAll(temps);
      }
    },
    editCostSchedule(file: IfcFile, cost_schedule: Entity, attributes: ApiData): void {
      const temps: Disposable[] = [];
      try {
        raw.cost.editCostSchedule(file.raw, cost_schedule.raw, attributes);
      } finally {
        disposeAll(temps);
      }
    },
    editCostValue(file: IfcFile, cost_value: Entity, attributes: ApiData, options: IfcOpenShellCostEditCostValueOptions): void {
      const temps: Disposable[] = [];
      try {
        raw.cost.editCostValue(file.raw, cost_value.raw, attributes, encodeOptions(options, {"clearUnitBasis": "clear_unit_basis", "editUnitBasis": "edit_unit_basis", "unitComponent": "unit_component", "valueComponent": "value_component"}, shell, temps));
      } finally {
        disposeAll(temps);
      }
    },
    editCostValueFormula(file: IfcFile, cost_value: Entity, formula: string): void {
      const temps: Disposable[] = [];
      try {
        raw.cost.editCostValueFormula(file.raw, cost_value.raw, formula);
      } finally {
        disposeAll(temps);
      }
    },
    removeCostItem(file: IfcFile, cost_item: Entity): void {
      const temps: Disposable[] = [];
      try {
        raw.cost.removeCostItem(file.raw, cost_item.raw);
      } finally {
        disposeAll(temps);
      }
    },
    removeCostItemQuantity(file: IfcFile, cost_item: Entity, physical_quantity: Entity): void {
      const temps: Disposable[] = [];
      try {
        raw.cost.removeCostItemQuantity(file.raw, cost_item.raw, physical_quantity.raw);
      } finally {
        disposeAll(temps);
      }
    },
    removeCostSchedule(file: IfcFile, cost_schedule: Entity): void {
      const temps: Disposable[] = [];
      try {
        raw.cost.removeCostSchedule(file.raw, cost_schedule.raw);
      } finally {
        disposeAll(temps);
      }
    },
    removeCostValue(file: IfcFile, parent: Entity, cost_value: Entity): void {
      const temps: Disposable[] = [];
      try {
        raw.cost.removeCostValue(file.raw, parent.raw, cost_value.raw);
      } finally {
        disposeAll(temps);
      }
    },
    unassignCostItemQuantity(file: IfcFile, cost_item: Entity, products: Entity[], options: IfcOpenShellCostUnassignCostItemQuantityOptions): void {
      const temps: Disposable[] = [];
      try {
        raw.cost.unassignCostItemQuantity(file.raw, cost_item.raw, toRaw(products, shell, temps), encodeOptions(options, {"application": "application", "user": "user"}, shell, temps));
      } finally {
        disposeAll(temps);
      }
    },
    }),
    document: Object.freeze({
    /** Add a new document information to the project. */
    addInformation(file: IfcFile, options: IfcOpenShellDocumentAddInformationOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.document.addInformation(file.raw, encodeOptions(options, {"application": "application", "ownerHistory": "owner_history", "parent": "parent", "user": "user"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /** Create a new reference to a document. */
    addReference(file: IfcFile, information: Entity): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.document.addReference(file.raw, information.raw);
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /** Assign a document to a list of products. */
    assignDocument(file: IfcFile, options: IfcOpenShellDocumentAssignDocumentOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.document.assignDocument(file.raw, encodeOptions(options, {"application": "application", "document": "document", "ownerHistory": "owner_history", "products": "products", "user": "user"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    removeInformation(file: IfcFile, information: Entity): void {
      const temps: Disposable[] = [];
      try {
        raw.document.removeInformation(file.raw, information.raw);
      } finally {
        disposeAll(temps);
      }
    },
    removeReference(file: IfcFile, reference: Entity): void {
      const temps: Disposable[] = [];
      try {
        raw.document.removeReference(file.raw, reference.raw);
      } finally {
        disposeAll(temps);
      }
    },
    /** Unassign a document from a list of products. */
    unassignDocument(file: IfcFile, options: IfcOpenShellDocumentUnassignDocumentOptions): void {
      const temps: Disposable[] = [];
      try {
        raw.document.unassignDocument(file.raw, encodeOptions(options, {"application": "application", "document": "document", "products": "products", "user": "user"}, shell, temps));
      } finally {
        disposeAll(temps);
      }
    },
    }),
    drawing: Object.freeze({
    assignProduct(file: IfcFile, options: IfcOpenShellDrawingAssignProductOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.drawing.assignProduct(file.raw, encodeOptions(options, {"application": "application", "ownerHistory": "owner_history", "relatedObject": "related_object", "relatingProduct": "relating_product", "user": "user"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
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
    getAggregate(instance: Entity): Entity | null {
      const temps: Disposable[] = [];
      try {
        const result = raw.element.getAggregate(instance.raw);
        return wrapEntity(shell, result) as Entity | null;
      } finally {
        disposeAll(temps);
      }
    },
    getContained(element: Entity): Entity[] {
      const temps: Disposable[] = [];
      try {
        const result = raw.element.getContained(element.raw);
        return wrapEntities(shell, result as never) as Entity[];
      } finally {
        disposeAll(temps);
      }
    },
    getContainer(instance: Entity, options: IfcOpenShellElementGetContainerOptions): Entity | null {
      const temps: Disposable[] = [];
      try {
        const result = raw.element.getContainer(instance.raw, encodeOptions(options, {"directOnly": "direct_only", "ifcClass": "ifc_class"}, shell, temps));
        return wrapEntity(shell, result) as Entity | null;
      } finally {
        disposeAll(temps);
      }
    },
    getControls(element: Entity): Entity[] {
      const temps: Disposable[] = [];
      try {
        const result = raw.element.getControls(element.raw);
        return wrapEntities(shell, result as never) as Entity[];
      } finally {
        disposeAll(temps);
      }
    },
    getDecomposition(element: Entity, options: IfcOpenShellElementGetDecompositionOptions): Entity[] {
      const temps: Disposable[] = [];
      try {
        const result = raw.element.getDecomposition(element.raw, encodeOptions(options, {"isRecursive": "is_recursive"}, shell, temps));
        return wrapEntities(shell, result as never) as Entity[];
      } finally {
        disposeAll(temps);
      }
    },
    getElementsByLayer(layer: Entity): Entity[] {
      const temps: Disposable[] = [];
      try {
        const result = raw.element.getElementsByLayer(layer.raw);
        return wrapEntities(shell, result as never) as Entity[];
      } finally {
        disposeAll(temps);
      }
    },
    getElementsByMaterial(material: Entity): Entity[] {
      const temps: Disposable[] = [];
      try {
        const result = raw.element.getElementsByMaterial(material.raw);
        return wrapEntities(shell, result as never) as Entity[];
      } finally {
        disposeAll(temps);
      }
    },
    getElementsByProfile(profile: Entity): Entity[] {
      const temps: Disposable[] = [];
      try {
        const result = raw.element.getElementsByProfile(profile.raw);
        return wrapEntities(shell, result as never) as Entity[];
      } finally {
        disposeAll(temps);
      }
    },
    getElementsByRepresentation(representation: Entity): Entity[] {
      const temps: Disposable[] = [];
      try {
        const result = raw.element.getElementsByRepresentation(representation.raw);
        return wrapEntities(shell, result as never) as Entity[];
      } finally {
        disposeAll(temps);
      }
    },
    getElementsByStyle(style: Entity): Entity[] {
      const temps: Disposable[] = [];
      try {
        const result = raw.element.getElementsByStyle(style.raw);
        return wrapEntities(shell, result as never) as Entity[];
      } finally {
        disposeAll(temps);
      }
    },
    getFilledVoid(element: Entity): Entity | null {
      const temps: Disposable[] = [];
      try {
        const result = raw.element.getFilledVoid(element.raw);
        return wrapEntity(shell, result) as Entity | null;
      } finally {
        disposeAll(temps);
      }
    },
    getGroups(element: Entity): Entity[] {
      const temps: Disposable[] = [];
      try {
        const result = raw.element.getGroups(element.raw);
        return wrapEntities(shell, result as never) as Entity[];
      } finally {
        disposeAll(temps);
      }
    },
    getLayers(element: Entity): Entity[] {
      const temps: Disposable[] = [];
      try {
        const result = raw.element.getLayers(element.raw);
        return wrapEntities(shell, result as never) as Entity[];
      } finally {
        disposeAll(temps);
      }
    },
    getMaterial(instance: Entity, options: IfcOpenShellElementGetMaterialOptions): Entity | null {
      const temps: Disposable[] = [];
      try {
        const result = raw.element.getMaterial(instance.raw, encodeOptions(options, {"shouldInherit": "should_inherit", "shouldSkipUsage": "should_skip_usage"}, shell, temps));
        return wrapEntity(shell, result) as Entity | null;
      } finally {
        disposeAll(temps);
      }
    },
    getNest(instance: Entity): Entity | null {
      const temps: Disposable[] = [];
      try {
        const result = raw.element.getNest(instance.raw);
        return wrapEntity(shell, result) as Entity | null;
      } finally {
        disposeAll(temps);
      }
    },
    getOpenings(element: Entity): Entity[] {
      const temps: Disposable[] = [];
      try {
        const result = raw.element.getOpenings(element.raw);
        return wrapEntities(shell, result as never) as Entity[];
      } finally {
        disposeAll(temps);
      }
    },
    getParent(instance: Entity): Entity | null {
      const temps: Disposable[] = [];
      try {
        const result = raw.element.getParent(instance.raw);
        return wrapEntity(shell, result) as Entity | null;
      } finally {
        disposeAll(temps);
      }
    },
    getParts(element: Entity): Entity[] {
      const temps: Disposable[] = [];
      try {
        const result = raw.element.getParts(element.raw);
        return wrapEntities(shell, result as never) as Entity[];
      } finally {
        disposeAll(temps);
      }
    },
    getPsetIds(element: Entity, options: IfcOpenShellElementGetPsetIdsOptions): Entity[] {
      const temps: Disposable[] = [];
      try {
        const result = raw.element.getPsetIds(element.raw, encodeOptions(options, {"psetsOnly": "psets_only", "qtosOnly": "qtos_only", "shouldInherit": "should_inherit"}, shell, temps));
        return wrapEntities(shell, result as never) as Entity[];
      } finally {
        disposeAll(temps);
      }
    },
    getReferencedElements(reference: Entity): Entity[] {
      const temps: Disposable[] = [];
      try {
        const result = raw.element.getReferencedElements(reference.raw);
        return wrapEntities(shell, result as never) as Entity[];
      } finally {
        disposeAll(temps);
      }
    },
    getReferencedStructures(element: Entity): Entity[] {
      const temps: Disposable[] = [];
      try {
        const result = raw.element.getReferencedStructures(element.raw);
        return wrapEntities(shell, result as never) as Entity[];
      } finally {
        disposeAll(temps);
      }
    },
    getShapeAspects(element: Entity, options: IfcOpenShellElementGetShapeAspectsOptions): Entity[] {
      const temps: Disposable[] = [];
      try {
        const result = raw.element.getShapeAspects(element.raw, encodeOptions(options, {"shouldInherit": "should_inherit"}, shell, temps));
        return wrapEntities(shell, result as never) as Entity[];
      } finally {
        disposeAll(temps);
      }
    },
    getStructureReferencedElements(structure: Entity): Entity[] {
      const temps: Disposable[] = [];
      try {
        const result = raw.element.getStructureReferencedElements(structure.raw);
        return wrapEntities(shell, result as never) as Entity[];
      } finally {
        disposeAll(temps);
      }
    },
    getStyles(element: Entity): Entity[] {
      const temps: Disposable[] = [];
      try {
        const result = raw.element.getStyles(element.raw);
        return wrapEntities(shell, result as never) as Entity[];
      } finally {
        disposeAll(temps);
      }
    },
    getType(instance: Entity): Entity | null {
      const temps: Disposable[] = [];
      try {
        const result = raw.element.getType(instance.raw);
        return wrapEntity(shell, result) as Entity | null;
      } finally {
        disposeAll(temps);
      }
    },
    getTypes(type_element: Entity): Entity[] {
      const temps: Disposable[] = [];
      try {
        const result = raw.element.getTypes(type_element.raw);
        return wrapEntities(shell, result as never) as Entity[];
      } finally {
        disposeAll(temps);
      }
    },
    getVoidedElement(element: Entity): Entity | null {
      const temps: Disposable[] = [];
      try {
        const result = raw.element.getVoidedElement(element.raw);
        return wrapEntity(shell, result) as Entity | null;
      } finally {
        disposeAll(temps);
      }
    },
    isUserdefinedType(element: Entity): boolean {
      const temps: Disposable[] = [];
      try {
        const result = raw.element.isUserdefinedType(element.raw);
        return wrap(shell, result) as boolean;
      } finally {
        disposeAll(temps);
      }
    },
    removeDeep(element: Entity): void {
      const temps: Disposable[] = [];
      try {
        raw.element.removeDeep(element.raw);
      } finally {
        disposeAll(temps);
      }
    },
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
    removeDeep2(instance: Entity): void {
      const temps: Disposable[] = [];
      try {
        raw.entity.removeDeep2(instance.raw);
      } finally {
        disposeAll(temps);
      }
    },
    removeDeep2Ex(instance: Entity, also_consider: Entity[], do_not_delete: Entity[]): void {
      const temps: Disposable[] = [];
      try {
        raw.entity.removeDeep2Ex(instance.raw, toRaw(also_consider, shell, temps), toRaw(do_not_delete, shell, temps));
      } finally {
        disposeAll(temps);
      }
    },
    }),
    feature: Object.freeze({
    addFeature(file: IfcFile, options: IfcOpenShellFeatureAddFeatureOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.feature.addFeature(file.raw, encodeOptions(options, {"application": "application", "element": "element", "feature": "feature", "user": "user"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    addFilling(file: IfcFile, opening: Entity, element: Entity): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.feature.addFilling(file.raw, opening.raw, element.raw);
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    removeFeature(file: IfcFile, options: IfcOpenShellFeatureRemoveFeatureOptions): void {
      const temps: Disposable[] = [];
      try {
        raw.feature.removeFeature(file.raw, encodeOptions(options, {"application": "application", "feature": "feature", "user": "user"}, shell, temps));
      } finally {
        disposeAll(temps);
      }
    },
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
    addAxisRepresentation(file: IfcFile, context: Entity, axis: number[][]): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.geometry.addAxisRepresentation(file.raw, context.raw, toRaw(axis, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    addBoolean(file: IfcFile, first_item: Entity, second_items: Entity[], operator_type: string): Entity[] {
      const temps: Disposable[] = [];
      try {
        const result = raw.geometry.addBoolean(file.raw, first_item.raw, toRaw(second_items, shell, temps), operator_type);
        return wrapEntities(shell, result as never) as Entity[];
      } finally {
        disposeAll(temps);
      }
    },
    addDoorRepresentation(file: IfcFile, options: IfcOpenShellGeometryAddDoorRepresentationOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.geometry.addDoorRepresentation(file.raw, encodeOptions(options, {"context": "context", "liningProperties": "lining_properties", "operationType": "operation_type", "overallHeight": "overall_height", "overallWidth": "overall_width", "panelProperties": "panel_properties", "partOfProduct": "part_of_product", "unitScale": "unit_scale"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    addFootprintRepresentation(file: IfcFile, context: Entity, curves: Entity[]): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.geometry.addFootprintRepresentation(file.raw, context.raw, toRaw(curves, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    addMeshRepresentation(file: IfcFile, context: Entity, options: IfcOpenShellGeometryAddMeshRepresentationOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.geometry.addMeshRepresentation(file.raw, context.raw, encodeOptions(options, {"faces": "faces", "forceFacetedBrep": "force_faceted_brep", "vertices": "vertices"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    addRailingRepresentation(file: IfcFile, options: IfcOpenShellGeometryAddRailingRepresentationOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.geometry.addRailingRepresentation(file.raw, encodeOptions(options, {"clearWidth": "clear_width", "context": "context", "height": "height", "loopedPath": "looped_path", "railingDiameter": "railing_diameter", "railingPath": "railing_path", "supportSpacing": "support_spacing", "terminalType": "terminal_type", "unitScale": "unit_scale", "useManualSupports": "use_manual_supports"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    addShapeAspect(file: IfcFile, options: IfcOpenShellGeometryAddShapeAspectOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.geometry.addShapeAspect(file.raw, encodeOptions(options, {"description": "description", "items": "items", "name": "name", "partOfProduct": "part_of_product", "representation": "representation"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    addSlabRepresentation(file: IfcFile, options: IfcOpenShellGeometryAddSlabRepresentationOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.geometry.addSlabRepresentation(file.raw, encodeOptions(options, {"clippingEntities": "clipping_entities", "clippingKinds": "clipping_kinds", "clippingLocations": "clipping_locations", "clippingNormals": "clipping_normals", "context": "context", "depth": "depth", "directionSense": "direction_sense", "offset": "offset", "polyline": "polyline", "xAngle": "x_angle"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    addTopologyRepresentation(file: IfcFile, options: IfcOpenShellGeometryAddTopologyRepresentationOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.geometry.addTopologyRepresentation(file.raw, encodeOptions(options, {"context": "context", "item": "item", "representationIdentifier": "representation_identifier", "representationType": "representation_type"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    addWallRepresentation(file: IfcFile, options: IfcOpenShellGeometryAddWallRepresentationOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.geometry.addWallRepresentation(file.raw, encodeOptions(options, {"booleans": "booleans", "clippingEntities": "clipping_entities", "clippingKinds": "clipping_kinds", "clippingLocations": "clipping_locations", "clippingNormals": "clipping_normals", "context": "context", "directionSense": "direction_sense", "height": "height", "length": "length", "offset": "offset", "thickness": "thickness", "xAngle": "x_angle"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    addWindowRepresentation(file: IfcFile, options: IfcOpenShellGeometryAddWindowRepresentationOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.geometry.addWindowRepresentation(file.raw, encodeOptions(options, {"context": "context", "glassThickness": "glass_thickness", "liningProperties": "lining_properties", "overallHeight": "overall_height", "overallWidth": "overall_width", "panelProperties": "panel_properties", "panelSchema": "panel_schema", "partOfProduct": "part_of_product"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    assignRepresentation(file: IfcFile, product: Entity, representation: Entity): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.geometry.assignRepresentation(file.raw, product.raw, representation.raw);
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    clipSolid(file: IfcFile, options: IfcOpenShellGeometryClipSolidOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.geometry.clipSolid(file.raw, encodeOptions(options, {"application": "application", "element": "element", "item": "item", "location": "location", "normal": "normal", "ownerHistory": "owner_history", "user": "user"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    clipSolidBounded(file: IfcFile, options: IfcOpenShellGeometryClipSolidBoundedOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.geometry.clipSolidBounded(file.raw, encodeOptions(options, {"application": "application", "boundaryPoints": "boundary_points", "boundaryPosition": "boundary_position", "element": "element", "item": "item", "location": "location", "normal": "normal", "ownerHistory": "owner_history", "user": "user"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    connectElement(file: IfcFile, options: IfcOpenShellGeometryConnectElementOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.geometry.connectElement(file.raw, encodeOptions(options, {"application": "application", "description": "description", "ownerHistory": "owner_history", "relatedElement": "related_element", "relatingElement": "relating_element", "user": "user"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    connectPath(file: IfcFile, options: IfcOpenShellGeometryConnectPathOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.geometry.connectPath(file.raw, encodeOptions(options, {"application": "application", "connectionGeometry": "connection_geometry", "description": "description", "ownerHistory": "owner_history", "relatedConnection": "related_connection", "relatedElement": "related_element", "relatingConnection": "relating_connection", "relatingElement": "relating_element", "user": "user"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    connectWall(file: IfcFile, options: IfcOpenShellGeometryConnectWallOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.geometry.connectWall(file.raw, encodeOptions(options, {"application": "application", "firstWall": "first_wall", "isAtpath": "is_atpath", "ownerHistory": "owner_history", "secondWall": "second_wall", "user": "user"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    copyRepresentation(file: IfcFile, options: IfcOpenShellGeometryCopyRepresentationOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.geometry.copyRepresentation(file.raw, encodeOptions(options, {"contextIdentifier": "context_identifier", "source": "source", "target": "target"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    create2ptWall(file: IfcFile, options: IfcOpenShellGeometryCreate2PtWallOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.geometry.create2ptWall(file.raw, encodeOptions(options, {"context": "context", "element": "element", "elevation": "elevation", "end": "end", "height": "height", "isSi": "is_si", "start": "start", "thickness": "thickness"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    disconnectElement(file: IfcFile, relating_element: Entity, related_element: Entity): void {
      const temps: Disposable[] = [];
      try {
        raw.geometry.disconnectElement(file.raw, relating_element.raw, related_element.raw);
      } finally {
        disposeAll(temps);
      }
    },
    disconnectPath(file: IfcFile, options: IfcOpenShellGeometryDisconnectPathOptions): void {
      const temps: Disposable[] = [];
      try {
        raw.geometry.disconnectPath(file.raw, encodeOptions(options, {"connectionType": "connection_type", "element": "element", "relatedElement": "related_element", "relatingElement": "relating_element"}, shell, temps));
      } finally {
        disposeAll(temps);
      }
    },
    editObjectPlacement(file: IfcFile, options: IfcOpenShellGeometryEditObjectPlacementOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.geometry.editObjectPlacement(file.raw, encodeOptions(options, {"isSi": "is_si", "matrix": "matrix", "product": "product", "shouldTransformChildren": "should_transform_children"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    mapRepresentation(file: IfcFile, representation: Entity): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.geometry.mapRepresentation(file.raw, representation.raw);
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    profileExtents(file: IfcFile, profile: Entity): number[] {
      const temps: Disposable[] = [];
      try {
        const result = raw.geometry.profileExtents(file.raw, profile.raw);
        return wrap(shell, result) as number[];
      } finally {
        disposeAll(temps);
      }
    },
    regenerateWallRepresentation(file: IfcFile, options: IfcOpenShellGeometryRegenerateWallRepresentationOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.geometry.regenerateWallRepresentation(file.raw, encodeOptions(options, {"angle": "angle", "height": "height", "length": "length", "wall": "wall"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    removeBoolean(file: IfcFile, item: Entity): void {
      const temps: Disposable[] = [];
      try {
        raw.geometry.removeBoolean(file.raw, item.raw);
      } finally {
        disposeAll(temps);
      }
    },
    removeRepresentation(file: IfcFile, representation: Entity, options: IfcOpenShellGeometryRemoveRepresentationOptions): void {
      const temps: Disposable[] = [];
      try {
        raw.geometry.removeRepresentation(file.raw, representation.raw, encodeOptions(options, {"shouldKeepNamedProfiles": "should_keep_named_profiles"}, shell, temps));
      } finally {
        disposeAll(temps);
      }
    },
    unassignRepresentation(file: IfcFile, product: Entity, representation: Entity): void {
      const temps: Disposable[] = [];
      try {
        raw.geometry.unassignRepresentation(file.raw, product.raw, representation.raw);
      } finally {
        disposeAll(temps);
      }
    },
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
    addGeoreferencing(file: IfcFile, options: IfcOpenShellGeoreferenceAddGeoreferencingOptions): void {
      const temps: Disposable[] = [];
      try {
        raw.georeference.addGeoreferencing(file.raw, encodeOptions(options, {"application": "application", "ifcClass": "ifc_class", "name": "name", "ownerHistory": "owner_history", "user": "user"}, shell, temps));
      } finally {
        disposeAll(temps);
      }
    },
    editGeoreferencing(file: IfcFile, options: IfcOpenShellGeoreferenceEditGeoreferencingOptions): void {
      const temps: Disposable[] = [];
      try {
        raw.georeference.editGeoreferencing(file.raw, encodeOptions(options, {"coordinateOperation": "coordinate_operation", "projectedCrs": "projected_crs"}, shell, temps, ["coordinateOperation", "projectedCrs"]));
      } finally {
        disposeAll(temps);
      }
    },
    editTrueNorth(file: IfcFile, options: IfcOpenShellGeoreferenceEditTrueNorthOptions): void {
      const temps: Disposable[] = [];
      try {
        raw.georeference.editTrueNorth(file.raw, encodeOptions(options, {"trueNorth": "true_north"}, shell, temps));
      } finally {
        disposeAll(temps);
      }
    },
    editWcs(file: IfcFile, options: IfcOpenShellGeoreferenceEditWcsOptions): void {
      const temps: Disposable[] = [];
      try {
        raw.georeference.editWcs(file.raw, encodeOptions(options, {"isSi": "is_si", "rotation": "rotation", "x": "x", "y": "y", "z": "z"}, shell, temps));
      } finally {
        disposeAll(temps);
      }
    },
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
    createAxisCurve(file: IfcFile, p1: number[], p2: number[], grid_axis: Entity, is_si: boolean): void {
      const temps: Disposable[] = [];
      try {
        raw.grid.createAxisCurve(file.raw, toRaw(p1, shell, temps), toRaw(p2, shell, temps), grid_axis.raw, is_si);
      } finally {
        disposeAll(temps);
      }
    },
    createGridAxis(file: IfcFile, grid: Entity, axis_tag: string, same_sense: boolean, uvw_axes: string): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.grid.createGridAxis(file.raw, grid.raw, axis_tag, same_sense, uvw_axes);
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
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
    /** Create a new IfcGroup with generated identity and ownership metadata. */
    addGroup(file: IfcFile, options: IfcOpenShellGroupAddGroupOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.group.addGroup(file.raw, encodeOptions(options, {"application": "application", "description": "description", "name": "name", "ownerHistory": "owner_history", "user": "user"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /** Assign products to a group, merging into an existing relationship when present. */
    assignGroup(file: IfcFile, options: IfcOpenShellGroupAssignGroupOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.group.assignGroup(file.raw, encodeOptions(options, {"application": "application", "group": "group", "ownerHistory": "owner_history", "products": "products", "user": "user"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /** Remove a group and its relationships. */
    removeGroup(file: IfcFile, group: Entity): void {
      const temps: Disposable[] = [];
      try {
        raw.group.removeGroup(file.raw, group.raw);
      } finally {
        disposeAll(temps);
      }
    },
    /** Remove products from a group relationship. */
    unassignGroup(file: IfcFile, options: IfcOpenShellGroupUnassignGroupOptions): void {
      const temps: Disposable[] = [];
      try {
        raw.group.unassignGroup(file.raw, encodeOptions(options, {"application": "application", "group": "group", "products": "products", "user": "user"}, shell, temps));
      } finally {
        disposeAll(temps);
      }
    },
    /** Replace the product list of an existing group relationship. */
    updateGroupProducts(file: IfcFile, options: IfcOpenShellGroupUpdateGroupProductsOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.group.updateGroupProducts(file.raw, encodeOptions(options, {"application": "application", "group": "group", "ownerHistory": "owner_history", "products": "products", "user": "user"}, shell, temps));
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
    "new"(): string {
      const temps: Disposable[] = [];
      try {
        const result = raw.guid["new"]();
        return wrap(shell, result) as string;
      } finally {
        disposeAll(temps);
      }
    },
    }),
    layer: Object.freeze({
    addLayer(file: IfcFile, name: string): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.layer.addLayer(file.raw, name);
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    addLayerWithStyle(file: IfcFile, name: string, on: boolean, frozen: boolean, blocked: boolean, styles: Entity[]): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.layer.addLayerWithStyle(file.raw, name, on, frozen, blocked, toRaw(styles, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    assignLayer(file: IfcFile, items: Entity[], layer: Entity): void {
      const temps: Disposable[] = [];
      try {
        raw.layer.assignLayer(file.raw, toRaw(items, shell, temps), layer.raw);
      } finally {
        disposeAll(temps);
      }
    },
    removeLayer(file: IfcFile, layer: Entity): void {
      const temps: Disposable[] = [];
      try {
        raw.layer.removeLayer(file.raw, layer.raw);
      } finally {
        disposeAll(temps);
      }
    },
    unassignLayer(file: IfcFile, items: Entity[], layer: Entity): void {
      const temps: Disposable[] = [];
      try {
        raw.layer.unassignLayer(file.raw, toRaw(items, shell, temps), layer.raw);
      } finally {
        disposeAll(temps);
      }
    },
    }),
    library: Object.freeze({
    addLibrary(file: IfcFile, name: string): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.library.addLibrary(file.raw, name);
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    addReference(file: IfcFile, library: Entity): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.library.addReference(file.raw, library.raw);
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /** Assign a library reference to a list of products. */
    assignReference(file: IfcFile, options: IfcOpenShellLibraryAssignReferenceOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.library.assignReference(file.raw, encodeOptions(options, {"application": "application", "ownerHistory": "owner_history", "products": "products", "reference": "reference", "user": "user"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    removeLibrary(file: IfcFile, library: Entity): void {
      const temps: Disposable[] = [];
      try {
        raw.library.removeLibrary(file.raw, library.raw);
      } finally {
        disposeAll(temps);
      }
    },
    removeReference(file: IfcFile, reference: Entity): void {
      const temps: Disposable[] = [];
      try {
        raw.library.removeReference(file.raw, reference.raw);
      } finally {
        disposeAll(temps);
      }
    },
    /** Unassign a library reference from a list of products. */
    unassignReference(file: IfcFile, options: IfcOpenShellLibraryUnassignReferenceOptions): void {
      const temps: Disposable[] = [];
      try {
        raw.library.unassignReference(file.raw, encodeOptions(options, {"application": "application", "products": "products", "reference": "reference", "user": "user"}, shell, temps));
      } finally {
        disposeAll(temps);
      }
    },
    }),
    material: Object.freeze({
    addConstituent(file: IfcFile, constituent_set: Entity, options: IfcOpenShellMaterialAddConstituentOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.material.addConstituent(file.raw, constituent_set.raw, encodeOptions(options, {"material": "material", "name": "name"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    addLayer(file: IfcFile, layer_set: Entity, options: IfcOpenShellMaterialAddLayerOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.material.addLayer(file.raw, layer_set.raw, encodeOptions(options, {"material": "material", "name": "name"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    addListItem(file: IfcFile, material_list: Entity, material: Entity): void {
      const temps: Disposable[] = [];
      try {
        raw.material.addListItem(file.raw, material_list.raw, material.raw);
      } finally {
        disposeAll(temps);
      }
    },
    addMaterial(file: IfcFile, options: IfcOpenShellMaterialAddMaterialOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.material.addMaterial(file.raw, encodeOptions(options, {"category": "category", "description": "description", "name": "name"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    addMaterialSet(file: IfcFile, options: IfcOpenShellMaterialAddMaterialSetOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.material.addMaterialSet(file.raw, encodeOptions(options, {"name": "name", "setType": "set_type"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    addProfile(file: IfcFile, profile_set: Entity, options: IfcOpenShellMaterialAddProfileOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.material.addProfile(file.raw, profile_set.raw, encodeOptions(options, {"material": "material", "name": "name", "profile": "profile"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    assignMaterial(file: IfcFile, products: Entity[], options: IfcOpenShellMaterialAssignMaterialOptions): Entity[] {
      const temps: Disposable[] = [];
      try {
        const result = raw.material.assignMaterial(file.raw, toRaw(products, shell, temps), encodeOptions(options, {"application": "application", "material": "material", "ownerHistory": "owner_history", "type": "type", "user": "user"}, shell, temps));
        return wrapEntities(shell, result as never) as Entity[];
      } finally {
        disposeAll(temps);
      }
    },
    assignProfile(file: IfcFile, material_profile: Entity, profile: Entity): void {
      const temps: Disposable[] = [];
      try {
        raw.material.assignProfile(file.raw, material_profile.raw, profile.raw);
      } finally {
        disposeAll(temps);
      }
    },
    editProfileUsage(file: IfcFile, usage: Entity, options: IfcOpenShellMaterialEditProfileUsageOptions): void {
      const temps: Disposable[] = [];
      try {
        raw.material.editProfileUsage(file.raw, usage.raw, encodeOptions(options, {"attributes": "attributes", "profileHeight": "profile_height", "profileWidth": "profile_width"}, shell, temps, ["attributes"]));
      } finally {
        disposeAll(temps);
      }
    },
    removeConstituent(file: IfcFile, constituent: Entity, options: IfcOpenShellMaterialRemoveItemOptions): void {
      const temps: Disposable[] = [];
      try {
        raw.material.removeConstituent(file.raw, constituent.raw, encodeOptions(options, {"shouldRemoveMaterial": "should_remove_material"}, shell, temps));
      } finally {
        disposeAll(temps);
      }
    },
    removeLayer(file: IfcFile, layer: Entity, options: IfcOpenShellMaterialRemoveItemOptions): void {
      const temps: Disposable[] = [];
      try {
        raw.material.removeLayer(file.raw, layer.raw, encodeOptions(options, {"shouldRemoveMaterial": "should_remove_material"}, shell, temps));
      } finally {
        disposeAll(temps);
      }
    },
    removeListItem(file: IfcFile, material_list: Entity, options: IfcOpenShellMaterialRemoveListItemOptions): void {
      const temps: Disposable[] = [];
      try {
        raw.material.removeListItem(file.raw, material_list.raw, encodeOptions(options, {"materialIndex": "material_index"}, shell, temps));
      } finally {
        disposeAll(temps);
      }
    },
    removeMaterial(file: IfcFile, material: Entity): void {
      const temps: Disposable[] = [];
      try {
        raw.material.removeMaterial(file.raw, material.raw);
      } finally {
        disposeAll(temps);
      }
    },
    removeMaterialSet(file: IfcFile, material: Entity): void {
      const temps: Disposable[] = [];
      try {
        raw.material.removeMaterialSet(file.raw, material.raw);
      } finally {
        disposeAll(temps);
      }
    },
    removeProfile(file: IfcFile, profile: Entity, options: IfcOpenShellMaterialRemoveProfileOptions): void {
      const temps: Disposable[] = [];
      try {
        raw.material.removeProfile(file.raw, profile.raw, encodeOptions(options, {"shouldRemoveMaterial": "should_remove_material", "shouldRemoveProfileDef": "should_remove_profile_def"}, shell, temps));
      } finally {
        disposeAll(temps);
      }
    },
    reorderSetItem(file: IfcFile, material_set: Entity, options: IfcOpenShellMaterialReorderSetItemOptions): void {
      const temps: Disposable[] = [];
      try {
        raw.material.reorderSetItem(file.raw, material_set.raw, encodeOptions(options, {"newIndex": "new_index", "oldIndex": "old_index"}, shell, temps));
      } finally {
        disposeAll(temps);
      }
    },
    unassignMaterial(file: IfcFile, products: Entity[], options: IfcOpenShellMaterialUnassignMaterialOptions): void {
      const temps: Disposable[] = [];
      try {
        raw.material.unassignMaterial(file.raw, toRaw(products, shell, temps), encodeOptions(options, {"application": "application", "user": "user"}, shell, temps));
      } finally {
        disposeAll(temps);
      }
    },
    }),
    nest: Object.freeze({
    /** Assign objects as nested children of a parent host. */
    assignObject(file: IfcFile, options: IfcOpenShellNestAssignObjectOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.nest.assignObject(file.raw, encodeOptions(options, {"application": "application", "ownerHistory": "owner_history", "products": "products", "relatingObject": "relating_object", "user": "user"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /** Remove objects from their nesting relationships. */
    unassignObject(file: IfcFile, options: IfcOpenShellNestUnassignObjectOptions): void {
      const temps: Disposable[] = [];
      try {
        raw.nest.unassignObject(file.raw, encodeOptions(options, {"application": "application", "products": "products", "user": "user"}, shell, temps));
      } finally {
        disposeAll(temps);
      }
    },
    }),
    owner: Object.freeze({
    addActor(file: IfcFile, options: IfcOpenShellOwnerAddActorOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.owner.addActor(file.raw, encodeOptions(options, {"actor": "actor", "application": "application", "ifcClass": "ifc_class", "ownerHistory": "owner_history", "user": "user"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    addAddress(file: IfcFile, assigned_object: Entity, ifc_class: string): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.owner.addAddress(file.raw, assigned_object.raw, ifc_class);
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    addApplication(file: IfcFile, options: IfcOpenShellOwnerAddApplicationOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.owner.addApplication(file.raw, encodeOptions(options, {"application": "application", "applicationDeveloper": "application_developer", "applicationFullName": "application_full_name", "applicationIdentifier": "application_identifier", "ownerHistory": "owner_history", "user": "user", "version": "version"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    addOrganisation(file: IfcFile, identification: string, name: string): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.owner.addOrganisation(file.raw, identification, name);
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    addPerson(file: IfcFile, identification: string, family_name: string, given_name: string): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.owner.addPerson(file.raw, identification, family_name, given_name);
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    addPersonAndOrganisation(file: IfcFile, person: Entity, organisation: Entity): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.owner.addPersonAndOrganisation(file.raw, person.raw, organisation.raw);
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    addRole(file: IfcFile, assigned_object: Entity, role: string): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.owner.addRole(file.raw, assigned_object.raw, role);
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    assignActor(file: IfcFile, options: IfcOpenShellOwnerAssignActorOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.owner.assignActor(file.raw, encodeOptions(options, {"application": "application", "ownerHistory": "owner_history", "relatedObject": "related_object", "relatingActor": "relating_actor", "user": "user"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    createOwnerHistory(file: IfcFile, options: IfcOpenShellOwnerCreateOwnerHistoryOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.owner.createOwnerHistory(file.raw, encodeOptions(options, {"application": "application", "user": "user"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    removeActor(file: IfcFile, actor: Entity): void {
      const temps: Disposable[] = [];
      try {
        raw.owner.removeActor(file.raw, actor.raw);
      } finally {
        disposeAll(temps);
      }
    },
    removeAddress(file: IfcFile, address: Entity): void {
      const temps: Disposable[] = [];
      try {
        raw.owner.removeAddress(file.raw, address.raw);
      } finally {
        disposeAll(temps);
      }
    },
    removeApplication(file: IfcFile, application: Entity): void {
      const temps: Disposable[] = [];
      try {
        raw.owner.removeApplication(file.raw, application.raw);
      } finally {
        disposeAll(temps);
      }
    },
    removeOrganisation(file: IfcFile, organisation: Entity): void {
      const temps: Disposable[] = [];
      try {
        raw.owner.removeOrganisation(file.raw, organisation.raw);
      } finally {
        disposeAll(temps);
      }
    },
    removePerson(file: IfcFile, person: Entity): void {
      const temps: Disposable[] = [];
      try {
        raw.owner.removePerson(file.raw, person.raw);
      } finally {
        disposeAll(temps);
      }
    },
    removePersonAndOrganisation(file: IfcFile, person_and_organisation: Entity): void {
      const temps: Disposable[] = [];
      try {
        raw.owner.removePersonAndOrganisation(file.raw, person_and_organisation.raw);
      } finally {
        disposeAll(temps);
      }
    },
    removeRole(file: IfcFile, role: Entity): void {
      const temps: Disposable[] = [];
      try {
        raw.owner.removeRole(file.raw, role.raw);
      } finally {
        disposeAll(temps);
      }
    },
    unassignActor(file: IfcFile, options: IfcOpenShellOwnerUnassignActorOptions): void {
      const temps: Disposable[] = [];
      try {
        raw.owner.unassignActor(file.raw, encodeOptions(options, {"application": "application", "relatedObject": "related_object", "relatingActor": "relating_actor", "user": "user"}, shell, temps));
      } finally {
        disposeAll(temps);
      }
    },
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
    a2p(origin: number[], z_axis: number[], x_axis: number[]): number[] {
      const temps: Disposable[] = [];
      try {
        const result = raw.placement.a2p(toRaw(origin, shell, temps), toRaw(z_axis, shell, temps), toRaw(x_axis, shell, temps));
        return wrap(shell, result) as number[];
      } finally {
        disposeAll(temps);
      }
    },
    getAxis2placement(instance: Entity): number[] {
      const temps: Disposable[] = [];
      try {
        const result = raw.placement.getAxis2placement(instance.raw);
        return wrap(shell, result) as number[];
      } finally {
        disposeAll(temps);
      }
    },
    getCartesianXform3d(instance: Entity): number[] {
      const temps: Disposable[] = [];
      try {
        const result = raw.placement.getCartesianXform3d(instance.raw);
        return wrap(shell, result) as number[];
      } finally {
        disposeAll(temps);
      }
    },
    getLocalPlacement(instance: Entity): number[] {
      const temps: Disposable[] = [];
      try {
        const result = raw.placement.getLocalPlacement(instance.raw);
        return wrap(shell, result) as number[];
      } finally {
        disposeAll(temps);
      }
    },
    getMappeditemXform(instance: Entity): number[] {
      const temps: Disposable[] = [];
      try {
        const result = raw.placement.getMappeditemXform(instance.raw);
        return wrap(shell, result) as number[];
      } finally {
        disposeAll(temps);
      }
    },
    getStoreyElevation(instance: Entity): number {
      const temps: Disposable[] = [];
      try {
        const result = raw.placement.getStoreyElevation(instance.raw);
        return wrap(shell, result) as number;
      } finally {
        disposeAll(temps);
      }
    },
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
    addArbitraryProfile(file: IfcFile, options: IfcOpenShellProfileAddArbitraryProfileOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.profile.addArbitraryProfile(file.raw, encodeOptions(options, {"name": "name", "profile": "profile"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    addArbitraryProfileWithVoids(file: IfcFile, options: IfcOpenShellProfileAddArbitraryProfileWithVoidsOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.profile.addArbitraryProfileWithVoids(file.raw, encodeOptions(options, {"innerProfiles": "inner_profiles", "name": "name", "outerProfile": "outer_profile"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    addParameterizedProfile(file: IfcFile, ifc_class: string, profile_type: string): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.profile.addParameterizedProfile(file.raw, ifc_class, profile_type);
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    copyProfile(file: IfcFile, profile: Entity): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.profile.copyProfile(file.raw, profile.raw);
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    editProfile(profile: Entity, attributes: ApiData): void {
      const temps: Disposable[] = [];
      try {
        raw.profile.editProfile(profile.raw, attributes);
      } finally {
        disposeAll(temps);
      }
    },
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
        const result = raw.project.assignDeclaration(file.raw, encodeOptions(options, {"application": "application", "definitions": "definitions", "ownerHistory": "owner_history", "relatingContext": "relating_context", "user": "user"}, shell, temps));
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
        raw.project.unassignDeclaration(file.raw, encodeOptions(options, {"application": "application", "definitions": "definitions", "relatingContext": "relating_context", "user": "user"}, shell, temps));
      } finally {
        disposeAll(temps);
      }
    },
    }),
    pset: Object.freeze({
    /**
     * Add or return an existing property set attached to an object, context, type,
     * material, or profile definition.
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
    addQto(file: IfcFile, options: IfcOpenShellPsetAddQtoOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.pset.addQto(file.raw, encodeOptions(options, {"application": "application", "name": "name", "ownerHistory": "owner_history", "product": "product", "user": "user"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    assignPset(file: IfcFile, options: IfcOpenShellPsetAssignPsetOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.pset.assignPset(file.raw, encodeOptions(options, {"application": "application", "ownerHistory": "owner_history", "products": "products", "pset": "pset", "user": "user"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    editPset(file: IfcFile, options: IfcOpenShellPsetEditPsetOptions): boolean {
      const temps: Disposable[] = [];
      try {
        const result = raw.pset.editPset(file.raw, encodeOptions(options, {"name": "name", "properties": "properties", "pset": "pset", "psetTemplate": "pset_template", "shouldPurge": "should_purge"}, shell, temps, ["properties"]));
        return wrap(shell, result) as boolean;
      } finally {
        disposeAll(temps);
      }
    },
    editQto(file: IfcFile, options: IfcOpenShellPsetEditQtoOptions): boolean {
      const temps: Disposable[] = [];
      try {
        const result = raw.pset.editQto(file.raw, encodeOptions(options, {"name": "name", "properties": "properties", "qto": "qto", "qtoTemplate": "qto_template"}, shell, temps, ["properties"]));
        return wrap(shell, result) as boolean;
      } finally {
        disposeAll(temps);
      }
    },
    removePset(file: IfcFile, product: Entity, pset: Entity): void {
      const temps: Disposable[] = [];
      try {
        raw.pset.removePset(file.raw, product.raw, pset.raw);
      } finally {
        disposeAll(temps);
      }
    },
    templateAddPropTemplate(file: IfcFile, pset_template: Entity, name: string, description: string, template_type: string, primary_measure_type: string): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.pset.templateAddPropTemplate(file.raw, pset_template.raw, name, description, template_type, primary_measure_type);
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    templateAddPsetTemplate(file: IfcFile, name: string, template_type: string, applicable_entity: string): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.pset.templateAddPsetTemplate(file.raw, name, template_type, applicable_entity);
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    templateCreateFromFiles(schema_identifier: string, template_files: IfcFile[]): PsetTemplate | null {
      const temps: Disposable[] = [];
      try {
        const result = raw.pset.templateCreateFromFiles(schema_identifier, toRaw(template_files, shell, temps));
        return result as PsetTemplate | null;
      } finally {
        disposeAll(temps);
      }
    },
    templateGetApplicable(pqt: PsetTemplate, ifc_class: string, predefined_type: string, pset_only: boolean, qto_only: boolean, schema_name: string): Entity[] {
      const temps: Disposable[] = [];
      try {
        const result = raw.pset.templateGetApplicable(pqt, ifc_class, predefined_type, pset_only, qto_only, schema_name);
        return wrapEntities(shell, result as never) as Entity[];
      } finally {
        disposeAll(temps);
      }
    },
    templateGetApplicableNames(pqt: PsetTemplate, ifc_class: string, predefined_type: string, pset_only: boolean, qto_only: boolean, schema_name: string): string[] {
      const temps: Disposable[] = [];
      try {
        const result = raw.pset.templateGetApplicableNames(pqt, ifc_class, predefined_type, pset_only, qto_only, schema_name);
        return wrap(shell, result) as string[];
      } finally {
        disposeAll(temps);
      }
    },
    templateGetByName(pqt: PsetTemplate, name: string): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.pset.templateGetByName(pqt, name);
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    templateGetTemplate(schema_identifier: string): PsetTemplate | null {
      const temps: Disposable[] = [];
      try {
        const result = raw.pset.templateGetTemplate(schema_identifier);
        return result as PsetTemplate | null;
      } finally {
        disposeAll(temps);
      }
    },
    templateIsTemplated(pqt: PsetTemplate, name: string): boolean {
      const temps: Disposable[] = [];
      try {
        const result = raw.pset.templateIsTemplated(pqt, name);
        return wrap(shell, result) as boolean;
      } finally {
        disposeAll(temps);
      }
    },
    templatePsetType(pset_template: Entity): string {
      const temps: Disposable[] = [];
      try {
        const result = raw.pset.templatePsetType(pset_template.raw);
        return wrap(shell, result) as string;
      } finally {
        disposeAll(temps);
      }
    },
    templateRemovePropTemplate(file: IfcFile, prop_template: Entity): void {
      const temps: Disposable[] = [];
      try {
        raw.pset.templateRemovePropTemplate(file.raw, prop_template.raw);
      } finally {
        disposeAll(temps);
      }
    },
    templateRemovePsetTemplate(pset_template: Entity): void {
      const temps: Disposable[] = [];
      try {
        raw.pset.templateRemovePsetTemplate(pset_template.raw);
      } finally {
        disposeAll(temps);
      }
    },
    templateSetTemplateDir(dir: string): void {
      const temps: Disposable[] = [];
      try {
        raw.pset.templateSetTemplateDir(dir);
      } finally {
        disposeAll(temps);
      }
    },
    unassignPset(file: IfcFile, products: Entity[], pset: Entity): void {
      const temps: Disposable[] = [];
      try {
        raw.pset.unassignPset(file.raw, toRaw(products, shell, temps), pset.raw);
      } finally {
        disposeAll(temps);
      }
    },
    unsharePset(file: IfcFile, options: IfcOpenShellPsetUnsharePsetOptions): Entity[] {
      const temps: Disposable[] = [];
      try {
        const result = raw.pset.unsharePset(file.raw, encodeOptions(options, {"application": "application", "ownerHistory": "owner_history", "products": "products", "pset": "pset", "user": "user"}, shell, temps));
        return wrapEntities(shell, result as never) as Entity[];
      } finally {
        disposeAll(temps);
      }
    },
    }),
    register: Object.freeze({
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
    getContext(file: IfcFile, context_type: string, subcontext: string, target_view: string): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.representation.getContext(file.raw, context_type, subcontext, target_view);
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    getPrioritisedContexts(file: IfcFile): Entity[] {
      const temps: Disposable[] = [];
      try {
        const result = raw.representation.getPrioritisedContexts(file.raw);
        return wrapEntities(shell, result as never) as Entity[];
      } finally {
        disposeAll(temps);
      }
    },
    getProductRepresentation(element: Entity, options: IfcOpenShellRepresentationGetProductRepresentationOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.representation.getProductRepresentation(element.raw, encodeOptions(options, {"context": "context", "contextType": "context_type", "subcontext": "subcontext", "targetView": "target_view"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    resolve(representation: Entity): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.representation.resolve(representation.raw);
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
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
    editResourceTime(file: IfcFile, resource_time: Entity, attributes: ApiData): void {
      const temps: Disposable[] = [];
      try {
        raw.resource.editResourceTime(file.raw, resource_time.raw, attributes);
      } finally {
        disposeAll(temps);
      }
    },
    }),
    root: Object.freeze({
    /**
     * Create an IFC entity with generated identity, ownership metadata, optional
     * name, and optional predefined type.
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
    filterAll(file: IfcFile, query: string): ValueData | null {
      const temps: Disposable[] = [];
      try {
        const result = raw.selector.filterAll(file.raw, query);
        return fromRawValue(shell, result as never) as ValueData | null;
      } finally {
        disposeAll(temps);
      }
    },
    filterElements(file: IfcFile, query: string, elements: Entity[]): ValueData | null {
      const temps: Disposable[] = [];
      try {
        const result = raw.selector.filterElements(file.raw, query, toRaw(elements, shell, temps));
        return fromRawValue(shell, result as never) as ValueData | null;
      } finally {
        disposeAll(temps);
      }
    },
    format(file: IfcFile, instance: Entity, query: string): string | null {
      const temps: Disposable[] = [];
      try {
        const result = raw.selector.format(file.raw, instance.raw, query);
        return wrap(shell, result) as string | null;
      } finally {
        disposeAll(temps);
      }
    },
    getElementValue(file: IfcFile, element: Entity, query: string): ValueData | null {
      const temps: Disposable[] = [];
      try {
        const result = raw.selector.getElementValue(file.raw, element.raw, query);
        return fromRawValue(shell, result as never) as ValueData | null;
      } finally {
        disposeAll(temps);
      }
    },
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
    addDateTime(file: IfcFile, date_time: string): Entity | string {
      const temps: Disposable[] = [];
      try {
        const result = raw.sequence.addDateTime(file.raw, date_time);
        return wrap(shell, result) as Entity | string;
      } finally {
        disposeAll(temps);
      }
    },
    addTask(file: IfcFile, options: IfcOpenShellSequenceAddTaskOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.sequence.addTask(file.raw, encodeOptions(options, {"application": "application", "description": "description", "identification": "identification", "name": "name", "ownerHistory": "owner_history", "parentTask": "parent_task", "predefinedType": "predefined_type", "user": "user", "workSchedule": "work_schedule"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    addTaskTime(file: IfcFile, task: Entity, options: IfcOpenShellSequenceAddTaskTimeOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.sequence.addTaskTime(file.raw, task.raw, encodeOptions(options, {"isRecurring": "is_recurring"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    addTimePeriod(file: IfcFile, recurrence_pattern: Entity, options: IfcOpenShellSequenceAddTimePeriodOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.sequence.addTimePeriod(file.raw, recurrence_pattern.raw, encodeOptions(options, {"endTime": "end_time", "startTime": "start_time"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    addWorkCalendar(file: IfcFile, options: IfcOpenShellSequenceAddWorkCalendarOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.sequence.addWorkCalendar(file.raw, encodeOptions(options, {"application": "application", "name": "name", "ownerHistory": "owner_history", "predefinedType": "predefined_type", "user": "user"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    addWorkPlan(file: IfcFile, options: IfcOpenShellSequenceAddWorkPlanOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.sequence.addWorkPlan(file.raw, encodeOptions(options, {"application": "application", "creationDate": "creation_date", "creatorPerson": "creator_person", "name": "name", "ownerHistory": "owner_history", "predefinedType": "predefined_type", "startTime": "start_time", "user": "user"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    addWorkSchedule(file: IfcFile, options: IfcOpenShellSequenceAddWorkScheduleOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.sequence.addWorkSchedule(file.raw, encodeOptions(options, {"application": "application", "creationDate": "creation_date", "creatorPerson": "creator_person", "name": "name", "objectType": "object_type", "ownerHistory": "owner_history", "predefinedType": "predefined_type", "startTime": "start_time", "user": "user", "workPlan": "work_plan"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    addWorkTime(file: IfcFile, work_calendar: Entity, time_type: string): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.sequence.addWorkTime(file.raw, work_calendar.raw, time_type);
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    assignLagTime(file: IfcFile, rel_sequence: Entity, lag_value: string, options: IfcOpenShellSequenceAssignLagTimeOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.sequence.assignLagTime(file.raw, rel_sequence.raw, lag_value, encodeOptions(options, {"durationType": "duration_type"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    assignProcess(file: IfcFile, relating_process: Entity, related_object: Entity, options: IfcOpenShellSequenceAssignProcessOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.sequence.assignProcess(file.raw, relating_process.raw, related_object.raw, encodeOptions(options, {"application": "application", "ownerHistory": "owner_history", "user": "user"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    assignProduct(file: IfcFile, relating_product: Entity, related_object: Entity, options: IfcOpenShellSequenceAssignProductOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.sequence.assignProduct(file.raw, relating_product.raw, related_object.raw, encodeOptions(options, {"application": "application", "ownerHistory": "owner_history", "user": "user"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    assignRecurrencePattern(file: IfcFile, parent: Entity, recurrence_type: string): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.sequence.assignRecurrencePattern(file.raw, parent.raw, recurrence_type);
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    assignSequence(file: IfcFile, relating_process: Entity, related_process: Entity, options: IfcOpenShellSequenceAssignSequenceOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.sequence.assignSequence(file.raw, relating_process.raw, related_process.raw, encodeOptions(options, {"application": "application", "ownerHistory": "owner_history", "sequenceType": "sequence_type", "user": "user"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    assignWorkPlan(file: IfcFile, work_schedule: Entity, work_plan: Entity, options: IfcOpenShellSequenceAssignWorkPlanOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.sequence.assignWorkPlan(file.raw, work_schedule.raw, work_plan.raw, encodeOptions(options, {"application": "application", "ownerHistory": "owner_history", "user": "user"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    calculateTaskDuration(file: IfcFile, task: Entity): void {
      const temps: Disposable[] = [];
      try {
        raw.sequence.calculateTaskDuration(file.raw, task.raw);
      } finally {
        disposeAll(temps);
      }
    },
    cascadeSchedule(file: IfcFile, task: Entity): void {
      const temps: Disposable[] = [];
      try {
        raw.sequence.cascadeSchedule(file.raw, task.raw);
      } finally {
        disposeAll(temps);
      }
    },
    copyWorkSchedule(file: IfcFile, work_schedule: Entity, options: IfcOpenShellSequenceCopyWorkScheduleOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.sequence.copyWorkSchedule(file.raw, work_schedule.raw, encodeOptions(options, {"application": "application", "ownerHistory": "owner_history", "user": "user"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    createBaseline(file: IfcFile, work_schedule: Entity, options: IfcOpenShellSequenceCreateBaselineOptions): void {
      const temps: Disposable[] = [];
      try {
        raw.sequence.createBaseline(file.raw, work_schedule.raw, encodeOptions(options, {"application": "application", "name": "name", "ownerHistory": "owner_history", "user": "user"}, shell, temps));
      } finally {
        disposeAll(temps);
      }
    },
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
    editLagTime(lag_time: Entity, attributes: ApiData): void {
      const temps: Disposable[] = [];
      try {
        raw.sequence.editLagTime(lag_time.raw, attributes);
      } finally {
        disposeAll(temps);
      }
    },
    editRecurrencePattern(recurrence_pattern: Entity, attributes: ApiData): void {
      const temps: Disposable[] = [];
      try {
        raw.sequence.editRecurrencePattern(recurrence_pattern.raw, attributes);
      } finally {
        disposeAll(temps);
      }
    },
    editSequence(rel_sequence: Entity, attributes: ApiData): void {
      const temps: Disposable[] = [];
      try {
        raw.sequence.editSequence(rel_sequence.raw, attributes);
      } finally {
        disposeAll(temps);
      }
    },
    editTask(task: Entity, attributes: ApiData): void {
      const temps: Disposable[] = [];
      try {
        raw.sequence.editTask(task.raw, attributes);
      } finally {
        disposeAll(temps);
      }
    },
    editTaskTime(file: IfcFile, task_time: Entity, attributes: ApiData): void {
      const temps: Disposable[] = [];
      try {
        raw.sequence.editTaskTime(file.raw, task_time.raw, attributes);
      } finally {
        disposeAll(temps);
      }
    },
    editWorkCalendar(work_calendar: Entity, attributes: ApiData): void {
      const temps: Disposable[] = [];
      try {
        raw.sequence.editWorkCalendar(work_calendar.raw, attributes);
      } finally {
        disposeAll(temps);
      }
    },
    editWorkPlan(work_plan: Entity, attributes: ApiData): void {
      const temps: Disposable[] = [];
      try {
        raw.sequence.editWorkPlan(work_plan.raw, attributes);
      } finally {
        disposeAll(temps);
      }
    },
    editWorkSchedule(work_schedule: Entity, attributes: ApiData): void {
      const temps: Disposable[] = [];
      try {
        raw.sequence.editWorkSchedule(work_schedule.raw, attributes);
      } finally {
        disposeAll(temps);
      }
    },
    editWorkTime(work_time: Entity, attributes: ApiData): void {
      const temps: Disposable[] = [];
      try {
        raw.sequence.editWorkTime(work_time.raw, attributes);
      } finally {
        disposeAll(temps);
      }
    },
    recalculateSchedule(file: IfcFile, work_schedule: Entity): void {
      const temps: Disposable[] = [];
      try {
        raw.sequence.recalculateSchedule(file.raw, work_schedule.raw);
      } finally {
        disposeAll(temps);
      }
    },
    removeTask(file: IfcFile, task: Entity, options: IfcOpenShellSequenceRemoveOptions): void {
      const temps: Disposable[] = [];
      try {
        raw.sequence.removeTask(file.raw, task.raw, encodeOptions(options, {"application": "application", "user": "user"}, shell, temps));
      } finally {
        disposeAll(temps);
      }
    },
    removeTimePeriod(file: IfcFile, time_period: Entity): void {
      const temps: Disposable[] = [];
      try {
        raw.sequence.removeTimePeriod(file.raw, time_period.raw);
      } finally {
        disposeAll(temps);
      }
    },
    removeWorkCalendar(file: IfcFile, work_calendar: Entity, options: IfcOpenShellSequenceRemoveOptions): void {
      const temps: Disposable[] = [];
      try {
        raw.sequence.removeWorkCalendar(file.raw, work_calendar.raw, encodeOptions(options, {"application": "application", "user": "user"}, shell, temps));
      } finally {
        disposeAll(temps);
      }
    },
    removeWorkPlan(file: IfcFile, work_plan: Entity, options: IfcOpenShellSequenceRemoveOptions): void {
      const temps: Disposable[] = [];
      try {
        raw.sequence.removeWorkPlan(file.raw, work_plan.raw, encodeOptions(options, {"application": "application", "user": "user"}, shell, temps));
      } finally {
        disposeAll(temps);
      }
    },
    removeWorkSchedule(file: IfcFile, work_schedule: Entity, options: IfcOpenShellSequenceRemoveOptions): void {
      const temps: Disposable[] = [];
      try {
        raw.sequence.removeWorkSchedule(file.raw, work_schedule.raw, encodeOptions(options, {"application": "application", "user": "user"}, shell, temps));
      } finally {
        disposeAll(temps);
      }
    },
    removeWorkTime(file: IfcFile, work_time: Entity): void {
      const temps: Disposable[] = [];
      try {
        raw.sequence.removeWorkTime(file.raw, work_time.raw);
      } finally {
        disposeAll(temps);
      }
    },
    unassignLagTime(file: IfcFile, rel_sequence: Entity): void {
      const temps: Disposable[] = [];
      try {
        raw.sequence.unassignLagTime(file.raw, rel_sequence.raw);
      } finally {
        disposeAll(temps);
      }
    },
    unassignProcess(file: IfcFile, relating_process: Entity, related_object: Entity, options: IfcOpenShellSequenceRemoveOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.sequence.unassignProcess(file.raw, relating_process.raw, related_object.raw, encodeOptions(options, {"application": "application", "user": "user"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    unassignProduct(file: IfcFile, relating_product: Entity, related_object: Entity, options: IfcOpenShellSequenceRemoveOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.sequence.unassignProduct(file.raw, relating_product.raw, related_object.raw, encodeOptions(options, {"application": "application", "user": "user"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    unassignRecurrencePattern(file: IfcFile, recurrence_pattern: Entity): void {
      const temps: Disposable[] = [];
      try {
        raw.sequence.unassignRecurrencePattern(file.raw, recurrence_pattern.raw);
      } finally {
        disposeAll(temps);
      }
    },
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
    builderAxis2Placement2d(file: IfcFile, options: IfcOpenShellShapeBuilderAxis2Placement2dOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.shape.builderAxis2Placement2d(file.raw, encodeOptions(options, {"position": "position", "xDirection": "x_direction"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    builderAxis2Placement3d(file: IfcFile, options: IfcOpenShellShapeBuilderAxis2Placement3dOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.shape.builderAxis2Placement3d(file.raw, encodeOptions(options, {"position": "position", "xAxis": "x_axis", "zAxis": "z_axis"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    builderBlock(file: IfcFile, options: IfcOpenShellShapeBuilderBlockOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.shape.builderBlock(file.raw, encodeOptions(options, {"position": "position", "xLength": "x_length", "yLength": "y_length", "zLength": "z_length"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    builderCircle(file: IfcFile, center: number[], radius: number): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.shape.builderCircle(file.raw, toRaw(center, shell, temps), radius);
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    builderCurveBetweenTwoPoints(file: IfcFile, points: number[][]): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.shape.builderCurveBetweenTwoPoints(file.raw, toRaw(points, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    builderDeepCopy(file: IfcFile, element: Entity): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.shape.builderDeepCopy(file.raw, element.raw);
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    builderEdge(file: IfcFile, start: number[], end: number[]): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.shape.builderEdge(file.raw, toRaw(start, shell, temps), toRaw(end, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    builderEllipseCurve(file: IfcFile, options: IfcOpenShellShapeBuilderEllipseCurveOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.shape.builderEllipseCurve(file.raw, encodeOptions(options, {"position": "position", "refXDirection": "ref_x_direction", "trimPoints": "trim_points", "trimPointsMask": "trim_points_mask", "xAxisRadius": "x_axis_radius", "yAxisRadius": "y_axis_radius"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    builderExtrude(file: IfcFile, options: IfcOpenShellShapeBuilderExtrudeOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.shape.builderExtrude(file.raw, encodeOptions(options, {"extrusionVector": "extrusion_vector", "magnitude": "magnitude", "position": "position", "positionXAxis": "position_x_axis", "positionYAxis": "position_y_axis", "positionZAxis": "position_z_axis", "profileOrCurve": "profile_or_curve"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    builderFace(file: IfcFile, points: number[][]): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.shape.builderFace(file.raw, toRaw(points, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    builderFacetedBrep(file: IfcFile, points: number[][], faces: number[][]): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.shape.builderFacetedBrep(file.raw, toRaw(points, shell, temps), toRaw(faces, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    builderGetPolylineCoords(polyline: Entity): number[][] {
      const temps: Disposable[] = [];
      try {
        const result = raw.shape.builderGetPolylineCoords(polyline.raw);
        return wrap(shell, result) as number[][];
      } finally {
        disposeAll(temps);
      }
    },
    builderHalfSpaceSolid(file: IfcFile, options: IfcOpenShellShapeBuilderHalfSpaceSolidOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.shape.builderHalfSpaceSolid(file.raw, encodeOptions(options, {"agreementFlag": "agreement_flag", "plane": "plane"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    builderIndexedPolycurve2d(file: IfcFile, points: number[][], segments: number[][]): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.shape.builderIndexedPolycurve2d(file.raw, toRaw(points, shell, temps), toRaw(segments, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
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
    builderMepTransitionCalculate(options: IfcOpenShellShapeBuilderMepTransitionCalculateOptions): number {
      const temps: Disposable[] = [];
      try {
        const result = raw.shape.builderMepTransitionCalculate(encodeOptions(options, {"angle": "angle", "diff": "diff", "endHalfDim": "end_half_dim", "endProfile": "end_profile", "length": "length", "offset": "offset", "startHalfDim": "start_half_dim"}, shell, temps));
        return wrap(shell, result) as number;
      } finally {
        disposeAll(temps);
      }
    },
    builderMepTransitionLength(options: IfcOpenShellShapeBuilderMepTransitionLengthOptions): number {
      const temps: Disposable[] = [];
      try {
        const result = raw.shape.builderMepTransitionLength(encodeOptions(options, {"angle": "angle", "endHalfDim": "end_half_dim", "profileOffset": "profile_offset", "startHalfDim": "start_half_dim"}, shell, temps));
        return wrap(shell, result) as number;
      } finally {
        disposeAll(temps);
      }
    },
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
    builderMesh(file: IfcFile, points: number[][], faces: number[][]): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.shape.builderMesh(file.raw, toRaw(points, shell, temps), toRaw(faces, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    builderMirror(file: IfcFile, options: IfcOpenShellShapeBuilderMirrorOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.shape.builderMirror(file.raw, encodeOptions(options, {"createCopy": "create_copy", "item": "item", "mirrorAxes": "mirror_axes", "mirrorPoint": "mirror_point", "placementMatrix": "placement_matrix"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    builderPlane(file: IfcFile, location: number[], normal: number[]): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.shape.builderPlane(file.raw, toRaw(location, shell, temps), toRaw(normal, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    builderPolygonalFaceSet(file: IfcFile, points: number[][], faces: number[][][]): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.shape.builderPolygonalFaceSet(file.raw, toRaw(points, shell, temps), toRaw(faces, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    builderPolyline(file: IfcFile, options: IfcOpenShellShapeBuilderPolylineOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.shape.builderPolyline(file.raw, encodeOptions(options, {"arcPoints": "arc_points", "closed": "closed", "points": "points", "positionOffset": "position_offset"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    builderProfile(file: IfcFile, options: IfcOpenShellShapeBuilderProfileOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.shape.builderProfile(file.raw, encodeOptions(options, {"innerCurves": "inner_curves", "name": "name", "outerCurve": "outer_curve", "profileType": "profile_type"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    builderRepresentation(file: IfcFile, options: IfcOpenShellShapeBuilderRepresentationOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.shape.builderRepresentation(file.raw, encodeOptions(options, {"context": "context", "items": "items", "representationType": "representation_type"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    builderRotate(file: IfcFile, options: IfcOpenShellShapeBuilderRotateOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.shape.builderRotate(file.raw, encodeOptions(options, {"angle": "angle", "counterClockwise": "counter_clockwise", "createCopy": "create_copy", "item": "item", "pivotPoint": "pivot_point"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    builderSetPolylineCoords(file: IfcFile, polyline: Entity, coords: number[][]): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.shape.builderSetPolylineCoords(file.raw, polyline.raw, toRaw(coords, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    builderSphere(file: IfcFile, options: IfcOpenShellShapeBuilderSphereOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.shape.builderSphere(file.raw, encodeOptions(options, {"center": "center", "radius": "radius"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    builderSweptDiskSolid(file: IfcFile, path_curve: Entity, radius: number): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.shape.builderSweptDiskSolid(file.raw, path_curve.raw, radius);
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    builderTranslate(file: IfcFile, options: IfcOpenShellShapeBuilderTranslateOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.shape.builderTranslate(file.raw, encodeOptions(options, {"createCopy": "create_copy", "item": "item", "translation": "translation"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    builderTriangulatedFaceSet(file: IfcFile, points: number[][], faces: number[][]): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.shape.builderTriangulatedFaceSet(file.raw, toRaw(points, shell, temps), toRaw(faces, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    builderVertex(file: IfcFile, position: number[]): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.shape.builderVertex(file.raw, toRaw(position, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    isX(value: number, x: number, tolerance: number): boolean {
      const temps: Disposable[] = [];
      try {
        const result = raw.shape.isX(value, x, tolerance);
        return wrap(shell, result) as boolean;
      } finally {
        disposeAll(temps);
      }
    },
    }),
    spatial: Object.freeze({
    /** Assign products to be contained hierarchically in a spatial structure. */
    assignContainer(file: IfcFile, options: IfcOpenShellSpatialAssignContainerOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.spatial.assignContainer(file.raw, encodeOptions(options, {"application": "application", "ownerHistory": "owner_history", "products": "products", "relatingStructure": "relating_structure", "user": "user"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /** Dereference products from a spatial structure. */
    dereferenceStructure(file: IfcFile, options: IfcOpenShellSpatialDereferenceStructureOptions): void {
      const temps: Disposable[] = [];
      try {
        raw.spatial.dereferenceStructure(file.raw, encodeOptions(options, {"application": "application", "products": "products", "relatingStructure": "relating_structure", "user": "user"}, shell, temps));
      } finally {
        disposeAll(temps);
      }
    },
    /** Reference products in a spatial structure without containment. */
    referenceStructure(file: IfcFile, options: IfcOpenShellSpatialReferenceStructureOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.spatial.referenceStructure(file.raw, encodeOptions(options, {"application": "application", "ownerHistory": "owner_history", "products": "products", "relatingStructure": "relating_structure", "user": "user"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /** Remove products from their spatial containment relationships. */
    unassignContainer(file: IfcFile, options: IfcOpenShellSpatialUnassignContainerOptions): void {
      const temps: Disposable[] = [];
      try {
        raw.spatial.unassignContainer(file.raw, encodeOptions(options, {"application": "application", "products": "products", "user": "user"}, shell, temps));
      } finally {
        disposeAll(temps);
      }
    },
    }),
    structural: Object.freeze({
    addStructuralActivity(file: IfcFile, applied_load: Entity, structural_member: Entity, ifc_class: string, predefined_type: string, global_or_local: string, options: IfcOpenShellStructuralAddStructuralActivityOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.structural.addStructuralActivity(file.raw, applied_load.raw, structural_member.raw, ifc_class, predefined_type, global_or_local, encodeOptions(options, {"activityOwnerHistory": "activity_owner_history", "relationshipOwnerHistory": "relationship_owner_history"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    addStructuralAnalysisModel(file: IfcFile, owner_history: Entity): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.structural.addStructuralAnalysisModel(file.raw, owner_history.raw);
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    addStructuralBoundaryCondition(file: IfcFile, ifc_class: string, options: IfcOpenShellStructuralAddStructuralBoundaryConditionOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.structural.addStructuralBoundaryCondition(file.raw, ifc_class, encodeOptions(options, {"connection": "connection", "name": "name"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    addStructuralLoad(file: IfcFile, ifc_class: string, name: string): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.structural.addStructuralLoad(file.raw, ifc_class, name);
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    addStructuralLoadCase(file: IfcFile, name: string, action_type: string, action_source: string, owner_history: Entity): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.structural.addStructuralLoadCase(file.raw, name, action_type, action_source, owner_history.raw);
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    addStructuralLoadGroup(file: IfcFile, name: string, action_type: string, action_source: string, owner_history: Entity): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.structural.addStructuralLoadGroup(file.raw, name, action_type, action_source, owner_history.raw);
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    addStructuralMemberConnection(file: IfcFile, relating_structural_member: Entity, related_structural_connection: Entity, owner_history: Entity): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.structural.addStructuralMemberConnection(file.raw, relating_structural_member.raw, related_structural_connection.raw, owner_history.raw);
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    assignProduct(file: IfcFile, relating_product: Entity, related_object: Entity, owner_history: Entity): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.structural.assignProduct(file.raw, relating_product.raw, related_object.raw, owner_history.raw);
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    assignStructuralAnalysisModel(file: IfcFile, products: Entity[], structural_analysis_model: Entity, options: IfcOpenShellStructuralAssignStructuralAnalysisModelOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.structural.assignStructuralAnalysisModel(file.raw, toRaw(products, shell, temps), structural_analysis_model.raw, encodeOptions(options, {"application": "application", "ownerHistory": "owner_history", "user": "user"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    assignToBuilding(file: IfcFile, structural_analysis_model: Entity, building: Entity, owner_history: Entity): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.structural.assignToBuilding(file.raw, structural_analysis_model.raw, building.raw, owner_history.raw);
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    editStructuralBoundaryCondition(file: IfcFile, condition: Entity, attributes: ApiData): void {
      const temps: Disposable[] = [];
      try {
        raw.structural.editStructuralBoundaryCondition(file.raw, condition.raw, attributes);
      } finally {
        disposeAll(temps);
      }
    },
    editStructuralConnectionCs(file: IfcFile, structural_item: Entity, axis: number[], ref_direction: number[]): void {
      const temps: Disposable[] = [];
      try {
        raw.structural.editStructuralConnectionCs(file.raw, structural_item.raw, toRaw(axis, shell, temps), toRaw(ref_direction, shell, temps));
      } finally {
        disposeAll(temps);
      }
    },
    editStructuralItemAxis(file: IfcFile, structural_item: Entity, axis: number[]): void {
      const temps: Disposable[] = [];
      try {
        raw.structural.editStructuralItemAxis(file.raw, structural_item.raw, toRaw(axis, shell, temps));
      } finally {
        disposeAll(temps);
      }
    },
    removeStructuralAnalysisModel(file: IfcFile, structural_analysis_model: Entity): void {
      const temps: Disposable[] = [];
      try {
        raw.structural.removeStructuralAnalysisModel(file.raw, structural_analysis_model.raw);
      } finally {
        disposeAll(temps);
      }
    },
    removeStructuralBoundaryCondition(file: IfcFile, options: IfcOpenShellStructuralRemoveStructuralBoundaryConditionOptions): void {
      const temps: Disposable[] = [];
      try {
        raw.structural.removeStructuralBoundaryCondition(file.raw, encodeOptions(options, {"boundaryCondition": "boundary_condition", "connection": "connection"}, shell, temps));
      } finally {
        disposeAll(temps);
      }
    },
    removeStructuralConnectionCondition(file: IfcFile, relation: Entity): void {
      const temps: Disposable[] = [];
      try {
        raw.structural.removeStructuralConnectionCondition(file.raw, relation.raw);
      } finally {
        disposeAll(temps);
      }
    },
    removeStructuralLoad(file: IfcFile, structural_load: Entity): void {
      const temps: Disposable[] = [];
      try {
        raw.structural.removeStructuralLoad(file.raw, structural_load.raw);
      } finally {
        disposeAll(temps);
      }
    },
    removeStructuralLoadCase(file: IfcFile, structural_load_case: Entity): void {
      const temps: Disposable[] = [];
      try {
        raw.structural.removeStructuralLoadCase(file.raw, structural_load_case.raw);
      } finally {
        disposeAll(temps);
      }
    },
    removeStructuralLoadGroup(file: IfcFile, structural_load_group: Entity): void {
      const temps: Disposable[] = [];
      try {
        raw.structural.removeStructuralLoadGroup(file.raw, structural_load_group.raw);
      } finally {
        disposeAll(temps);
      }
    },
    unassignStructuralAnalysisModel(file: IfcFile, products: Entity[], structural_analysis_model: Entity, options: IfcOpenShellStructuralUnassignStructuralAnalysisModelOptions): void {
      const temps: Disposable[] = [];
      try {
        raw.structural.unassignStructuralAnalysisModel(file.raw, toRaw(products, shell, temps), structural_analysis_model.raw, encodeOptions(options, {"application": "application", "user": "user"}, shell, temps));
      } finally {
        disposeAll(temps);
      }
    },
    }),
    style: Object.freeze({
    addStyle(file: IfcFile, name: string, ifc_class: string): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.style.addStyle(file.raw, name, ifc_class);
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    assignItemStyle(file: IfcFile, options: IfcOpenShellStyleAssignItemStyleOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.style.assignItemStyle(file.raw, encodeOptions(options, {"item": "item", "shouldUsePresentationStyleAssignment": "should_use_presentation_style_assignment", "style": "style"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    assignMaterialStyle(file: IfcFile, material: Entity, style: Entity, context: Entity, should_use_presentation_style_assignment: boolean): void {
      const temps: Disposable[] = [];
      try {
        raw.style.assignMaterialStyle(file.raw, material.raw, style.raw, context.raw, should_use_presentation_style_assignment);
      } finally {
        disposeAll(temps);
      }
    },
    assignRepresentationStyles(file: IfcFile, shape_representation: Entity, styles: Entity[], should_use_presentation_style_assignment: boolean, replace_previous_same_type_style: boolean): Entity[] {
      const temps: Disposable[] = [];
      try {
        const result = raw.style.assignRepresentationStyles(file.raw, shape_representation.raw, toRaw(styles, shell, temps), should_use_presentation_style_assignment, replace_previous_same_type_style);
        return wrapEntities(shell, result as never) as Entity[];
      } finally {
        disposeAll(temps);
      }
    },
    editSurfaceStyle(file: IfcFile, style: Entity, attributes: ApiData): void {
      const temps: Disposable[] = [];
      try {
        raw.style.editSurfaceStyle(file.raw, style.raw, attributes);
      } finally {
        disposeAll(temps);
      }
    },
    removeStyle(file: IfcFile, style: Entity): void {
      const temps: Disposable[] = [];
      try {
        raw.style.removeStyle(file.raw, style.raw);
      } finally {
        disposeAll(temps);
      }
    },
    removeStyledRepresentation(file: IfcFile, representation: Entity): void {
      const temps: Disposable[] = [];
      try {
        raw.style.removeStyledRepresentation(file.raw, representation.raw);
      } finally {
        disposeAll(temps);
      }
    },
    removeSurfaceStyle(file: IfcFile, style: Entity): void {
      const temps: Disposable[] = [];
      try {
        raw.style.removeSurfaceStyle(file.raw, style.raw);
      } finally {
        disposeAll(temps);
      }
    },
    unassignMaterialStyle(file: IfcFile, material: Entity, style: Entity, context: Entity): void {
      const temps: Disposable[] = [];
      try {
        raw.style.unassignMaterialStyle(file.raw, material.raw, style.raw, context.raw);
      } finally {
        disposeAll(temps);
      }
    },
    unassignRepresentationStyles(file: IfcFile, shape_representation: Entity, styles: Entity[], should_use_presentation_style_assignment: boolean): void {
      const temps: Disposable[] = [];
      try {
        raw.style.unassignRepresentationStyles(file.raw, shape_representation.raw, toRaw(styles, shell, temps), should_use_presentation_style_assignment);
      } finally {
        disposeAll(temps);
      }
    },
    }),
    system: Object.freeze({
    /** Create a new distribution port and optionally assign it to an element. */
    addPort(file: IfcFile, options: IfcOpenShellSystemAddPortOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.system.addPort(file.raw, encodeOptions(options, {"application": "application", "element": "element", "ownerHistory": "owner_history", "user": "user"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /** Create a new system with the given IFC class. */
    addSystem(file: IfcFile, options: IfcOpenShellSystemAddSystemOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.system.addSystem(file.raw, encodeOptions(options, {"ifcClass": "ifc_class", "ownerHistory": "owner_history"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /** Assign a flow control element to a flow element. */
    assignFlowControl(file: IfcFile, options: IfcOpenShellSystemAssignFlowControlOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.system.assignFlowControl(file.raw, encodeOptions(options, {"application": "application", "ownerHistory": "owner_history", "relatedFlowControl": "related_flow_control", "relatingFlowElement": "relating_flow_element", "user": "user"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /** Assign a port to an element. */
    assignPort(file: IfcFile, options: IfcOpenShellSystemAssignPortOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.system.assignPort(file.raw, encodeOptions(options, {"application": "application", "element": "element", "ownerHistory": "owner_history", "port": "port", "user": "user"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /** Assign products to a system. */
    assignSystem(file: IfcFile, options: IfcOpenShellSystemAssignSystemOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.system.assignSystem(file.raw, encodeOptions(options, {"application": "application", "ownerHistory": "owner_history", "products": "products", "system": "system", "user": "user"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /** Connect two ports together with a given flow direction. */
    connectPort(file: IfcFile, options: IfcOpenShellSystemConnectPortOptions): void {
      const temps: Disposable[] = [];
      try {
        raw.system.connectPort(file.raw, encodeOptions(options, {"application": "application", "direction": "direction", "element": "element", "ownerHistory": "owner_history", "port1": "port1", "port2": "port2", "user": "user"}, shell, temps));
      } finally {
        disposeAll(temps);
      }
    },
    /** Disconnect a port from all connected ports. */
    disconnectPort(file: IfcFile, port: Entity): void {
      const temps: Disposable[] = [];
      try {
        raw.system.disconnectPort(file.raw, port.raw);
      } finally {
        disposeAll(temps);
      }
    },
    /** Remove a system and its relationships. */
    removeSystem(file: IfcFile, system: Entity): void {
      const temps: Disposable[] = [];
      try {
        raw.system.removeSystem(file.raw, system.raw);
      } finally {
        disposeAll(temps);
      }
    },
    /** Unassign a flow control element from a flow element. */
    unassignFlowControl(file: IfcFile, options: IfcOpenShellSystemUnassignFlowControlOptions): void {
      const temps: Disposable[] = [];
      try {
        raw.system.unassignFlowControl(file.raw, encodeOptions(options, {"application": "application", "relatedFlowControl": "related_flow_control", "relatingFlowElement": "relating_flow_element", "user": "user"}, shell, temps));
      } finally {
        disposeAll(temps);
      }
    },
    /** Unassign a port from an element. */
    unassignPort(file: IfcFile, options: IfcOpenShellSystemUnassignPortOptions): void {
      const temps: Disposable[] = [];
      try {
        raw.system.unassignPort(file.raw, encodeOptions(options, {"application": "application", "element": "element", "port": "port", "user": "user"}, shell, temps));
      } finally {
        disposeAll(temps);
      }
    },
    /** Unassign products from a system. */
    unassignSystem(file: IfcFile, options: IfcOpenShellSystemUnassignSystemOptions): void {
      const temps: Disposable[] = [];
      try {
        raw.system.unassignSystem(file.raw, encodeOptions(options, {"application": "application", "products": "products", "system": "system", "user": "user"}, shell, temps));
      } finally {
        disposeAll(temps);
      }
    },
    }),
    type: Object.freeze({
    /** Assign a type to element occurrences, creating or merging IfcRelDefinesByType. */
    assignType(file: IfcFile, options: IfcOpenShellTypeAssignTypeOptions): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.type.assignType(file.raw, encodeOptions(options, {"application": "application", "objects": "objects", "ownerHistory": "owner_history", "relatingType": "relating_type", "shouldMapRepresentations": "should_map_representations", "user": "user"}, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    /** Map representation maps from a type onto a related object. */
    mapTypeRepresentations(file: IfcFile, related_object: Entity, relating_type: Entity): boolean {
      const temps: Disposable[] = [];
      try {
        const result = raw.type.mapTypeRepresentations(file.raw, related_object.raw, relating_type.raw);
        return wrap(shell, result) as boolean;
      } finally {
        disposeAll(temps);
      }
    },
    /** Remove type assignments from element occurrences. */
    unassignType(file: IfcFile, options: IfcOpenShellTypeUnassignTypeOptions): void {
      const temps: Disposable[] = [];
      try {
        raw.type.unassignType(file.raw, encodeOptions(options, {"application": "application", "objects": "objects", "user": "user"}, shell, temps));
      } finally {
        disposeAll(temps);
      }
    },
    }),
    unit: Object.freeze({
    addContextDependentUnit(file: IfcFile, unit_type: string, name: string, dimensions: bigint[]): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.unit.addContextDependentUnit(file.raw, unit_type, name, toRaw(dimensions, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    addDerivedUnit(file: IfcFile, unit_type: string, userdefinedtype: string, units: Entity[], exponents: bigint[]): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.unit.addDerivedUnit(file.raw, unit_type, userdefinedtype, toRaw(units, shell, temps), toRaw(exponents, shell, temps));
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    addMonetaryUnit(file: IfcFile, currency: string): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.unit.addMonetaryUnit(file.raw, currency);
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    addSiUnit(file: IfcFile, unit_type: string, prefix: string): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.unit.addSiUnit(file.raw, unit_type, prefix);
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    calculateUnitScale(file: IfcFile, unit_type: string): number {
      const temps: Disposable[] = [];
      try {
        const result = raw.unit.calculateUnitScale(file.raw, unit_type);
        return wrap(shell, result) as number;
      } finally {
        disposeAll(temps);
      }
    },
    convert(value: number, from_prefix: string, from_unit: string, to_prefix: string, to_unit: string): number {
      const temps: Disposable[] = [];
      try {
        const result = raw.unit.convert(value, from_prefix, from_unit, to_prefix, to_unit);
        return wrap(shell, result) as number;
      } finally {
        disposeAll(temps);
      }
    },
    convertUnit(value: number, from_unit: Entity, to_unit: Entity): number {
      const temps: Disposable[] = [];
      try {
        const result = raw.unit.convertUnit(value, from_unit.raw, to_unit.raw);
        return wrap(shell, result) as number;
      } finally {
        disposeAll(temps);
      }
    },
    formatLength(value: number, precision: number, decimal_places: number, suppress_zero_inches: boolean, unit_system: string, input_unit: string, output_unit: string): string {
      const temps: Disposable[] = [];
      try {
        const result = raw.unit.formatLength(value, precision, decimal_places, suppress_zero_inches, unit_system, input_unit, output_unit);
        return wrap(shell, result) as string;
      } finally {
        disposeAll(temps);
      }
    },
    getFullUnitName(unit: Entity): string {
      const temps: Disposable[] = [];
      try {
        const result = raw.unit.getFullUnitName(unit.raw);
        return wrap(shell, result) as string;
      } finally {
        disposeAll(temps);
      }
    },
    getMeasureClass(unit_type: string): string {
      const temps: Disposable[] = [];
      try {
        const result = raw.unit.getMeasureClass(unit_type);
        return wrap(shell, result) as string;
      } finally {
        disposeAll(temps);
      }
    },
    getMeasureUnitType(measure_class: string): string {
      const temps: Disposable[] = [];
      try {
        const result = raw.unit.getMeasureUnitType(measure_class);
        return wrap(shell, result) as string;
      } finally {
        disposeAll(temps);
      }
    },
    getNamedDimensions(name: string): number[] {
      const temps: Disposable[] = [];
      try {
        const result = raw.unit.getNamedDimensions(name);
        return wrap(shell, result) as number[];
      } finally {
        disposeAll(temps);
      }
    },
    getPrefix(text: string): string {
      const temps: Disposable[] = [];
      try {
        const result = raw.unit.getPrefix(text);
        return wrap(shell, result) as string;
      } finally {
        disposeAll(temps);
      }
    },
    getPrefixMultiplier(text: string): number {
      const temps: Disposable[] = [];
      try {
        const result = raw.unit.getPrefixMultiplier(text);
        return wrap(shell, result) as number;
      } finally {
        disposeAll(temps);
      }
    },
    getProjectUnit(file: IfcFile, unit_type: string): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.unit.getProjectUnit(file.raw, unit_type);
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    getSiDimensions(name: string): number[] {
      const temps: Disposable[] = [];
      try {
        const result = raw.unit.getSiDimensions(name);
        return wrap(shell, result) as number[];
      } finally {
        disposeAll(temps);
      }
    },
    getSymbolMeasureClass(symbol: string): string {
      const temps: Disposable[] = [];
      try {
        const result = raw.unit.getSymbolMeasureClass(symbol);
        return wrap(shell, result) as string;
      } finally {
        disposeAll(temps);
      }
    },
    getSymbolQuantityClass(symbol: string): string {
      const temps: Disposable[] = [];
      try {
        const result = raw.unit.getSymbolQuantityClass(symbol);
        return wrap(shell, result) as string;
      } finally {
        disposeAll(temps);
      }
    },
    getUnitAssignment(file: IfcFile): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.unit.getUnitAssignment(file.raw);
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    getUnitName(text: string): string {
      const temps: Disposable[] = [];
      try {
        const result = raw.unit.getUnitName(text);
        return wrap(shell, result) as string;
      } finally {
        disposeAll(temps);
      }
    },
    getUnitNameUniversal(text: string): string {
      const temps: Disposable[] = [];
      try {
        const result = raw.unit.getUnitNameUniversal(text);
        return wrap(shell, result) as string;
      } finally {
        disposeAll(temps);
      }
    },
    getUnitSymbol(unit: Entity): string {
      const temps: Disposable[] = [];
      try {
        const result = raw.unit.getUnitSymbol(unit.raw);
        return wrap(shell, result) as string;
      } finally {
        disposeAll(temps);
      }
    },
    removeUnit(file: IfcFile, unit: Entity): void {
      const temps: Disposable[] = [];
      try {
        raw.unit.removeUnit(file.raw, unit.raw);
      } finally {
        disposeAll(temps);
      }
    },
    resolvePropertyMeasureClass(prop: Entity): string {
      const temps: Disposable[] = [];
      try {
        const result = raw.unit.resolvePropertyMeasureClass(prop.raw);
        return wrap(shell, result) as string;
      } finally {
        disposeAll(temps);
      }
    },
    resolvePropertyTableDefinedMeasureClass(prop: Entity): string {
      const temps: Disposable[] = [];
      try {
        const result = raw.unit.resolvePropertyTableDefinedMeasureClass(prop.raw);
        return wrap(shell, result) as string;
      } finally {
        disposeAll(temps);
      }
    },
    resolvePropertyTableDefinedUnit(prop: Entity): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.unit.resolvePropertyTableDefinedUnit(prop.raw);
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    resolvePropertyTableDefiningMeasureClass(prop: Entity): string {
      const temps: Disposable[] = [];
      try {
        const result = raw.unit.resolvePropertyTableDefiningMeasureClass(prop.raw);
        return wrap(shell, result) as string;
      } finally {
        disposeAll(temps);
      }
    },
    resolvePropertyTableDefiningUnit(prop: Entity): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.unit.resolvePropertyTableDefiningUnit(prop.raw);
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    resolvePropertyUnit(prop: Entity): Entity {
      const temps: Disposable[] = [];
      try {
        const result = raw.unit.resolvePropertyUnit(prop.raw);
        return wrapEntity(shell, result) as Entity;
      } finally {
        disposeAll(temps);
      }
    },
    unassignUnit(file: IfcFile, units: Entity[]): void {
      const temps: Disposable[] = [];
      try {
        raw.unit.unassignUnit(file.raw, toRaw(units, shell, temps));
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
): Record<string, RawValue> {
  const data = value as Record<string, ApiInput | undefined>;
  const psetFieldSet = psetFields ? new Set(psetFields) : undefined;
  return Object.fromEntries(
    Object.entries(fields)
      .filter(([publicName]) => data[publicName] !== undefined)
      .map(([publicName, nativeName]) => [nativeName, encodeOptionValue(publicName, data[publicName] as ApiInput, shell, temps, psetFieldSet)]),
  ) as Record<string, RawValue>;
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

function encodeOptionValue(publicName: string, value: ApiInput, shell: IfcOpenShell, temps: Disposable[], psetFields?: Set<string>): RawValue {
  if (psetFields?.has(publicName)) {
    return toRawPsetProperties(shell, value as PsetProperties | PsetInput, temps);
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
  return Array.isArray(value) && value.every((item) => item instanceof Entity);
}

function isPlainObject(value: RawValue): value is Record<string, ApiInput> {
  return typeof value === 'object' && value !== null && Object.getPrototypeOf(value) === Object.prototype;
}

function disposeAll(values: Disposable[]): void {
  for (let index = values.length - 1; index >= 0; index -= 1) values[index]?.destroy();
}
