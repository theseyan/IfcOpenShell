// This file was generated with the assistance of an AI coding tool.

declare module 'ifcopenshell-api' {
  export type IfcOpenshellRawValue = null | boolean | number | bigint | string | object | IfcOpenshellRawValue[];

  export interface IfcOpenshellSequenceDuplicateTaskResult {
    current: IfcOpenshellParseInstanceList;
    duplicate: IfcOpenshellParseInstanceList;
  }

  export interface IfcOpenshellShapeBuilderMepBendShapeResult {
    representation: IfcOpenshellInstance;
    start_length: number;
    end_length: number;
    radius: number;
    angle: number;
    lateral_axis: number;
    lateral_sign: number;
    z_axis_sign: number;
    main_profile_dimension: number;
  }

  export interface IfcOpenshellShapeBuilderMepTransitionShapeResult {
    representation: IfcOpenshellInstance;
    start_length: number;
    end_length: number;
    angle: number;
    profile_offset: number[];
    transition_length: number;
    full_transition_length: number;
  }

  export interface IfcOpenshellAggregateAssignObjectOptions {
    products: IfcOpenshellParseInstanceList;
    relating_object: IfcOpenshellInstance;
    owner_history?: IfcOpenshellInstance;
    user?: IfcOpenshellInstance;
    application?: IfcOpenshellInstance;
  }

  export interface IfcOpenshellAggregateUnassignObjectOptions {
    products: IfcOpenshellParseInstanceList;
    user?: IfcOpenshellInstance;
    application?: IfcOpenshellInstance;
  }

  export interface IfcOpenshellAttributeEditAttributesOptions {
    product: IfcOpenshellInstance;
    attributes: number;
    sync_predefined_type: boolean;
    update_owner_history: boolean;
    user?: IfcOpenshellInstance;
    application?: IfcOpenshellInstance;
  }

  export interface IfcOpenshellBoundaryAssignConnectionGeometryOptions {
    outer_boundary: number[][];
    location: number[];
    axis: number[];
    ref_direction: number[];
    inner_boundaries: number[][][];
    unit_scale: number;
  }

  export interface IfcOpenshellBoundaryEditAttributesOptions {
    relating_space: IfcOpenshellInstance;
    related_building_element: IfcOpenshellInstance;
    parent_boundary?: IfcOpenshellInstance;
    corresponding_boundary?: IfcOpenshellInstance;
    physical_or_virtual: string;
    internal_or_external: string;
  }

  export interface IfcOpenshellClassificationAddReferenceOptions {
    products: IfcOpenshellParseInstanceList;
    reference?: IfcOpenshellInstance;
    identification?: string;
    name?: string;
    classification?: IfcOpenshellInstance;
    owner_history?: IfcOpenshellInstance;
    user?: IfcOpenshellInstance;
    application?: IfcOpenshellInstance;
  }

  export interface IfcOpenshellClassificationRemoveReferenceOptions {
    reference: IfcOpenshellInstance;
    products: IfcOpenshellParseInstanceList;
    user?: IfcOpenshellInstance;
    application?: IfcOpenshellInstance;
  }

  export interface IfcOpenshellCogoAddSurveyPointOptions {
    survey_point: IfcOpenshellInstance;
    site?: IfcOpenshellInstance;
    owner_history?: IfcOpenshellInstance;
    user?: IfcOpenshellInstance;
    application?: IfcOpenshellInstance;
  }

  export interface IfcOpenshellConstraintAssignConstraintOptions {
    products: IfcOpenshellParseInstanceList;
    constraint: IfcOpenshellInstance;
    owner_history?: IfcOpenshellInstance;
    user?: IfcOpenshellInstance;
    application?: IfcOpenshellInstance;
  }

  export interface IfcOpenshellConstraintUnassignConstraintOptions {
    products: IfcOpenshellParseInstanceList;
    constraint: IfcOpenshellInstance;
    user?: IfcOpenshellInstance;
    application?: IfcOpenshellInstance;
  }

  export interface IfcOpenshellContextAddContextOptions {
    context_type: string;
    context_identifier: string;
    target_view: string;
    target_scale?: number;
    parent?: IfcOpenshellInstance;
  }

  export interface IfcOpenshellControlAssignControlOptions {
    relating_control: IfcOpenshellInstance;
    related_objects: IfcOpenshellParseInstanceList;
    owner_history?: IfcOpenshellInstance;
    user?: IfcOpenshellInstance;
    application?: IfcOpenshellInstance;
  }

  export interface IfcOpenshellControlUnassignControlOptions {
    relating_control: IfcOpenshellInstance;
    related_objects: IfcOpenshellParseInstanceList;
    user?: IfcOpenshellInstance;
    application?: IfcOpenshellInstance;
  }

  export interface IfcOpenshellCostAddCostItemOptions {
    cost_schedule?: IfcOpenshellInstance;
    cost_item?: IfcOpenshellInstance;
    owner_history?: IfcOpenshellInstance;
    user?: IfcOpenshellInstance;
    application?: IfcOpenshellInstance;
  }

  export interface IfcOpenshellCostAssignCostItemQuantityOptions {
    owner_history?: IfcOpenshellInstance;
    user?: IfcOpenshellInstance;
    application?: IfcOpenshellInstance;
  }

  export interface IfcOpenshellCostCopyCostScheduleOptions {
    owner_history?: IfcOpenshellInstance;
    user?: IfcOpenshellInstance;
    application?: IfcOpenshellInstance;
  }

  export interface IfcOpenshellCostEditCostValueOptions {
    edit_unit_basis: boolean;
    clear_unit_basis: boolean;
    value_component: number;
    unit_component?: IfcOpenshellInstance;
  }

  export interface IfcOpenshellCostUnassignCostItemQuantityOptions {
    user?: IfcOpenshellInstance;
    application?: IfcOpenshellInstance;
  }

  export interface IfcOpenshellDocumentAddInformationOptions {
    parent?: IfcOpenshellInstance;
    owner_history?: IfcOpenshellInstance;
    user?: IfcOpenshellInstance;
    application?: IfcOpenshellInstance;
  }

  export interface IfcOpenshellDocumentAssignDocumentOptions {
    products: IfcOpenshellParseInstanceList;
    document: IfcOpenshellInstance;
    owner_history?: IfcOpenshellInstance;
    user?: IfcOpenshellInstance;
    application?: IfcOpenshellInstance;
  }

  export interface IfcOpenshellDocumentUnassignDocumentOptions {
    products: IfcOpenshellParseInstanceList;
    document: IfcOpenshellInstance;
    user?: IfcOpenshellInstance;
    application?: IfcOpenshellInstance;
  }

  export interface IfcOpenshellDrawingAssignProductOptions {
    relating_product: IfcOpenshellInstance;
    related_object: IfcOpenshellInstance;
    owner_history?: IfcOpenshellInstance;
    user?: IfcOpenshellInstance;
    application?: IfcOpenshellInstance;
  }

  export interface IfcOpenshellDrawingUnassignProductOptions {
    relating_product: IfcOpenshellInstance;
    related_object: IfcOpenshellInstance;
    user?: IfcOpenshellInstance;
    application?: IfcOpenshellInstance;
  }

  export interface IfcOpenshellElementGetContainerOptions {
    direct_only?: boolean;
    ifc_class?: string;
  }

  export interface IfcOpenshellElementGetDecompositionOptions {
    is_recursive?: boolean;
  }

  export interface IfcOpenshellElementGetMaterialOptions {
    should_skip_usage?: boolean;
    should_inherit?: boolean;
  }

  export interface IfcOpenshellElementGetPsetIdsOptions {
    psets_only?: boolean;
    qtos_only?: boolean;
    should_inherit?: boolean;
  }

  export interface IfcOpenshellElementGetShapeAspectsOptions {
    should_inherit?: boolean;
  }

  export interface IfcOpenshellFeatureAddFeatureOptions {
    feature: IfcOpenshellInstance;
    element: IfcOpenshellInstance;
    user?: IfcOpenshellInstance;
    application?: IfcOpenshellInstance;
  }

  export interface IfcOpenshellFeatureRemoveFeatureOptions {
    feature: IfcOpenshellInstance;
    user?: IfcOpenshellInstance;
    application?: IfcOpenshellInstance;
  }

  export interface IfcOpenshellGeometryAddDoorRepresentationOptions {
    context: IfcOpenshellInstance;
    overall_height: number;
    overall_width: number;
    operation_type: string;
    lining_properties: number[];
    panel_properties: number[];
    part_of_product?: IfcOpenshellInstance;
    unit_scale: number;
  }

  export interface IfcOpenshellGeometryAddMeshRepresentationOptions {
    vertices: number[][][];
    faces: number[][][][];
    force_faceted_brep?: boolean;
  }

  export interface IfcOpenshellGeometryAddRailingRepresentationOptions {
    context: IfcOpenshellInstance;
    railing_path: number[][];
    use_manual_supports: boolean;
    support_spacing: number;
    railing_diameter: number;
    clear_width: number;
    terminal_type: string;
    height: number;
    looped_path: boolean;
    unit_scale: number;
  }

  export interface IfcOpenshellGeometryAddShapeAspectOptions {
    name: string;
    items: IfcOpenshellParseInstanceList;
    representation: IfcOpenshellInstance;
    part_of_product: IfcOpenshellInstance;
    description?: string;
  }

  export interface IfcOpenshellGeometryAddSlabRepresentationOptions {
    context: IfcOpenshellInstance;
    depth: number;
    direction_sense: string;
    offset: number;
    x_angle: number;
    clipping_kinds: number[];
    clipping_locations: number[][];
    clipping_normals: number[][];
    clipping_entities: IfcOpenshellParseInstanceList;
    polyline?: number[][];
  }

  export interface IfcOpenshellGeometryAddTopologyRepresentationOptions {
    context: IfcOpenshellInstance;
    item: IfcOpenshellInstance;
    representation_identifier?: string;
    representation_type?: string;
  }

  export interface IfcOpenshellGeometryAddWallRepresentationOptions {
    context: IfcOpenshellInstance;
    length: number;
    height: number;
    direction_sense: string;
    offset: number;
    thickness: number;
    x_angle: number;
    clipping_kinds: number[];
    clipping_locations: number[][];
    clipping_normals: number[][];
    clipping_entities: IfcOpenshellParseInstanceList;
    booleans: IfcOpenshellParseInstanceList;
  }

  export interface IfcOpenshellGeometryAddWindowRepresentationOptions {
    context: IfcOpenshellInstance;
    overall_height: number;
    overall_width: number;
    panel_schema: number[][];
    lining_properties: number[];
    panel_properties: number[][];
    part_of_product?: IfcOpenshellInstance;
    glass_thickness: number;
  }

  export interface IfcOpenshellGeometryClipSolidBoundedOptions {
    item: IfcOpenshellInstance;
    location: number[];
    normal: number[];
    boundary_points: number[][];
    boundary_position: number[];
    element?: IfcOpenshellInstance;
    owner_history?: IfcOpenshellInstance;
    user?: IfcOpenshellInstance;
    application?: IfcOpenshellInstance;
  }

  export interface IfcOpenshellGeometryClipSolidOptions {
    item: IfcOpenshellInstance;
    location: number[];
    normal: number[];
    element?: IfcOpenshellInstance;
    owner_history?: IfcOpenshellInstance;
    user?: IfcOpenshellInstance;
    application?: IfcOpenshellInstance;
  }

  export interface IfcOpenshellGeometryConnectElementOptions {
    relating_element: IfcOpenshellInstance;
    related_element: IfcOpenshellInstance;
    description?: string;
    owner_history?: IfcOpenshellInstance;
    user?: IfcOpenshellInstance;
    application?: IfcOpenshellInstance;
  }

  export interface IfcOpenshellGeometryConnectPathOptions {
    relating_element: IfcOpenshellInstance;
    related_element: IfcOpenshellInstance;
    relating_connection: string;
    related_connection: string;
    description?: string;
    connection_geometry?: IfcOpenshellInstance;
    owner_history?: IfcOpenshellInstance;
    user?: IfcOpenshellInstance;
    application?: IfcOpenshellInstance;
  }

  export interface IfcOpenshellGeometryConnectWallOptions {
    first_wall: IfcOpenshellInstance;
    second_wall: IfcOpenshellInstance;
    is_atpath: boolean;
    owner_history?: IfcOpenshellInstance;
    user?: IfcOpenshellInstance;
    application?: IfcOpenshellInstance;
  }

  export interface IfcOpenshellGeometryCopyRepresentationOptions {
    source: IfcOpenshellInstance;
    target: IfcOpenshellInstance;
    context_identifier?: string;
  }

  export interface IfcOpenshellGeometryCreate2PtWallOptions {
    element: IfcOpenshellInstance;
    context: IfcOpenshellInstance;
    start: number[];
    end: number[];
    elevation: number;
    height: number;
    thickness: number;
    is_si: boolean;
  }

  export interface IfcOpenshellGeometryDisconnectPathOptions {
    element?: IfcOpenshellInstance;
    connection_type?: string;
    relating_element?: IfcOpenshellInstance;
    related_element?: IfcOpenshellInstance;
  }

  export interface IfcOpenshellGeometryEditObjectPlacementOptions {
    product: IfcOpenshellInstance;
    matrix: number[];
    is_si: boolean;
    should_transform_children: boolean;
  }

  export interface IfcOpenshellGeometryRegenerateWallRepresentationOptions {
    wall: IfcOpenshellInstance;
    length: number;
    height: number;
    angle?: number;
  }

  export interface IfcOpenshellGeometryRemoveRepresentationOptions {
    should_keep_named_profiles?: boolean;
  }

  export interface IfcOpenshellGeometryValidateTypeOptions {
    preferred_item?: IfcOpenshellInstance;
  }

  export interface IfcOpenshellGeoreferenceAddGeoreferencingOptions {
    ifc_class: string;
    name: string;
    owner_history?: IfcOpenshellInstance;
    user?: IfcOpenshellInstance;
    application?: IfcOpenshellInstance;
  }

  export interface IfcOpenshellGeoreferenceEditGeoreferencingOptions {
    coordinate_operation?: number;
    projected_crs?: number;
  }

  export interface IfcOpenshellGeoreferenceEditTrueNorthOptions {
    true_north?: number[];
  }

  export interface IfcOpenshellGeoreferenceEditWcsOptions {
    x?: number;
    y?: number;
    z?: number;
    rotation?: number;
    is_si?: boolean;
  }

  export interface IfcOpenshellGroupAddGroupOptions {
    name: string;
    description?: string;
    owner_history?: IfcOpenshellInstance;
    user?: IfcOpenshellInstance;
    application?: IfcOpenshellInstance;
  }

  export interface IfcOpenshellGroupAssignGroupOptions {
    products: IfcOpenshellParseInstanceList;
    group: IfcOpenshellInstance;
    owner_history?: IfcOpenshellInstance;
    user?: IfcOpenshellInstance;
    application?: IfcOpenshellInstance;
  }

  export interface IfcOpenshellGroupUnassignGroupOptions {
    products: IfcOpenshellParseInstanceList;
    group: IfcOpenshellInstance;
    user?: IfcOpenshellInstance;
    application?: IfcOpenshellInstance;
  }

  export interface IfcOpenshellGroupUpdateGroupProductsOptions {
    group: IfcOpenshellInstance;
    products: IfcOpenshellParseInstanceList;
    owner_history?: IfcOpenshellInstance;
    user?: IfcOpenshellInstance;
    application?: IfcOpenshellInstance;
  }

  export interface IfcOpenshellLayerAddLayerWithStyleOptions {
    on?: boolean;
    frozen?: boolean;
    blocked?: boolean;
    styles: IfcOpenshellParseInstanceList;
  }

  export interface IfcOpenshellLibraryAssignReferenceOptions {
    products: IfcOpenshellParseInstanceList;
    reference: IfcOpenshellInstance;
    owner_history?: IfcOpenshellInstance;
    user?: IfcOpenshellInstance;
    application?: IfcOpenshellInstance;
  }

  export interface IfcOpenshellLibraryUnassignReferenceOptions {
    reference: IfcOpenshellInstance;
    products: IfcOpenshellParseInstanceList;
    user?: IfcOpenshellInstance;
    application?: IfcOpenshellInstance;
  }

  export interface IfcOpenshellMaterialAddConstituentOptions {
    material: IfcOpenshellInstance;
    name?: string;
  }

  export interface IfcOpenshellMaterialAddLayerOptions {
    material: IfcOpenshellInstance;
    name?: string;
  }

  export interface IfcOpenshellMaterialAddMaterialOptions {
    name?: string;
    category?: string;
    description?: string;
  }

  export interface IfcOpenshellMaterialAddMaterialSetOptions {
    name?: string;
    set_type?: string;
  }

  export interface IfcOpenshellMaterialAddProfileOptions {
    material?: IfcOpenshellInstance;
    profile?: IfcOpenshellInstance;
    name?: string;
  }

  export interface IfcOpenshellMaterialAssignMaterialOptions {
    type?: string;
    material?: IfcOpenshellInstance;
    owner_history?: IfcOpenshellInstance;
    user?: IfcOpenshellInstance;
    application?: IfcOpenshellInstance;
  }

  export interface IfcOpenshellMaterialEditProfileUsageOptions {
    attributes: number;
    profile_width?: number;
    profile_height?: number;
  }

  export interface IfcOpenshellMaterialRemoveItemOptions {
    should_remove_material?: boolean;
  }

  export interface IfcOpenshellMaterialRemoveListItemOptions {
    material_index?: number;
  }

  export interface IfcOpenshellMaterialRemoveProfileOptions {
    should_remove_profile_def?: boolean;
    should_remove_material?: boolean;
  }

  export interface IfcOpenshellMaterialReorderSetItemOptions {
    old_index?: number;
    new_index?: number;
  }

  export interface IfcOpenshellMaterialUnassignMaterialOptions {
    user?: IfcOpenshellInstance;
    application?: IfcOpenshellInstance;
  }

  export interface IfcOpenshellNestAssignObjectOptions {
    products: IfcOpenshellParseInstanceList;
    relating_object: IfcOpenshellInstance;
    owner_history?: IfcOpenshellInstance;
    user?: IfcOpenshellInstance;
    application?: IfcOpenshellInstance;
  }

  export interface IfcOpenshellNestUnassignObjectOptions {
    products: IfcOpenshellParseInstanceList;
    user?: IfcOpenshellInstance;
    application?: IfcOpenshellInstance;
  }

  export interface IfcOpenshellOwnerAddActorOptions {
    actor: IfcOpenshellInstance;
    ifc_class: string;
    owner_history?: IfcOpenshellInstance;
    user?: IfcOpenshellInstance;
    application?: IfcOpenshellInstance;
  }

  export interface IfcOpenshellOwnerAddApplicationOptions {
    application_developer?: IfcOpenshellInstance;
    version: string;
    application_full_name: string;
    application_identifier: string;
    owner_history?: IfcOpenshellInstance;
    user?: IfcOpenshellInstance;
    application?: IfcOpenshellInstance;
  }

  export interface IfcOpenshellOwnerAssignActorOptions {
    relating_actor: IfcOpenshellInstance;
    related_object: IfcOpenshellInstance;
    owner_history?: IfcOpenshellInstance;
    user?: IfcOpenshellInstance;
    application?: IfcOpenshellInstance;
  }

  export interface IfcOpenshellOwnerCreateOwnerHistoryOptions {
    user?: IfcOpenshellInstance;
    application?: IfcOpenshellInstance;
  }

  export interface IfcOpenshellOwnerUnassignActorOptions {
    relating_actor: IfcOpenshellInstance;
    related_object: IfcOpenshellInstance;
    user?: IfcOpenshellInstance;
    application?: IfcOpenshellInstance;
  }

  export interface IfcOpenshellOwnerUpdateOwnerHistoryOptions {
    element?: IfcOpenshellInstance;
    user?: IfcOpenshellInstance;
    application?: IfcOpenshellInstance;
  }

  export interface IfcOpenshellProfileAddArbitraryProfileOptions {
    profile: number[][];
    name?: string;
  }

  export interface IfcOpenshellProfileAddArbitraryProfileWithVoidsOptions {
    outer_profile: number[][];
    inner_profiles: number[][][];
    name?: string;
  }

  export interface IfcOpenshellProjectAssignDeclarationOptions {
    definitions: IfcOpenshellParseInstanceList;
    relating_context: IfcOpenshellInstance;
    owner_history?: IfcOpenshellInstance;
    user?: IfcOpenshellInstance;
    application?: IfcOpenshellInstance;
  }

  export interface IfcOpenshellProjectUnassignDeclarationOptions {
    definitions: IfcOpenshellParseInstanceList;
    relating_context: IfcOpenshellInstance;
    user?: IfcOpenshellInstance;
    application?: IfcOpenshellInstance;
  }

  export interface IfcOpenshellPsetAddPsetOptions {
    product: IfcOpenshellInstance;
    name: string;
    owner_history?: IfcOpenshellInstance;
    user?: IfcOpenshellInstance;
    application?: IfcOpenshellInstance;
    ifc2x3_subclass?: string;
  }

  export interface IfcOpenshellPsetAddQtoOptions {
    product: IfcOpenshellInstance;
    name: string;
    owner_history?: IfcOpenshellInstance;
    user?: IfcOpenshellInstance;
    application?: IfcOpenshellInstance;
  }

  export interface IfcOpenshellPsetAssignPsetOptions {
    products: IfcOpenshellParseInstanceList;
    pset: IfcOpenshellInstance;
    owner_history?: IfcOpenshellInstance;
    user?: IfcOpenshellInstance;
    application?: IfcOpenshellInstance;
  }

  export interface IfcOpenshellPsetEditPsetOptions {
    pset: IfcOpenshellInstance;
    name?: string;
    properties: number;
    pset_template?: IfcOpenshellInstance;
    should_purge: boolean;
  }

  export interface IfcOpenshellPsetEditQtoOptions {
    qto: IfcOpenshellInstance;
    name?: string;
    properties: number;
    qto_template?: IfcOpenshellInstance;
  }

  export interface IfcOpenshellPsetUnsharePsetOptions {
    products: IfcOpenshellParseInstanceList;
    pset: IfcOpenshellInstance;
    owner_history?: IfcOpenshellInstance;
    user?: IfcOpenshellInstance;
    application?: IfcOpenshellInstance;
  }

  export interface IfcOpenshellRepresentationGetProductRepresentationOptions {
    context?: IfcOpenshellInstance;
    context_type?: string;
    subcontext?: string;
    target_view?: string;
  }

  export interface IfcOpenshellRootCreateEntityOptions {
    ifc_class: string;
    predefined_type?: string;
    name?: string;
    owner_history?: IfcOpenshellInstance;
  }

  export interface IfcOpenshellRootRemoveProductOptions {
    user?: IfcOpenshellInstance;
    application?: IfcOpenshellInstance;
  }

  export interface IfcOpenshellSequenceAddTaskOptions {
    work_schedule?: IfcOpenshellInstance;
    parent_task?: IfcOpenshellInstance;
    name?: string;
    description?: string;
    identification?: string;
    predefined_type?: string;
    owner_history?: IfcOpenshellInstance;
    user?: IfcOpenshellInstance;
    application?: IfcOpenshellInstance;
  }

  export interface IfcOpenshellSequenceAddTaskTimeOptions {
    is_recurring?: boolean;
  }

  export interface IfcOpenshellSequenceAddTimePeriodOptions {
    start_time?: string;
    end_time?: string;
  }

  export interface IfcOpenshellSequenceAddWorkCalendarOptions {
    name?: string;
    predefined_type?: string;
    owner_history?: IfcOpenshellInstance;
    user?: IfcOpenshellInstance;
    application?: IfcOpenshellInstance;
  }

  export interface IfcOpenshellSequenceAddWorkPlanOptions {
    name?: string;
    predefined_type?: string;
    creation_date?: string;
    start_time?: string;
    creator_person?: IfcOpenshellInstance;
    owner_history?: IfcOpenshellInstance;
    user?: IfcOpenshellInstance;
    application?: IfcOpenshellInstance;
  }

  export interface IfcOpenshellSequenceAddWorkScheduleOptions {
    name?: string;
    predefined_type?: string;
    object_type?: string;
    creation_date?: string;
    start_time?: string;
    work_plan?: IfcOpenshellInstance;
    creator_person?: IfcOpenshellInstance;
    owner_history?: IfcOpenshellInstance;
    user?: IfcOpenshellInstance;
    application?: IfcOpenshellInstance;
  }

  export interface IfcOpenshellSequenceAssignLagTimeOptions {
    duration_type?: string;
  }

  export interface IfcOpenshellSequenceAssignProcessOptions {
    owner_history?: IfcOpenshellInstance;
    user?: IfcOpenshellInstance;
    application?: IfcOpenshellInstance;
  }

  export interface IfcOpenshellSequenceAssignProductOptions {
    owner_history?: IfcOpenshellInstance;
    user?: IfcOpenshellInstance;
    application?: IfcOpenshellInstance;
  }

  export interface IfcOpenshellSequenceAssignSequenceOptions {
    sequence_type?: string;
    owner_history?: IfcOpenshellInstance;
    user?: IfcOpenshellInstance;
    application?: IfcOpenshellInstance;
  }

  export interface IfcOpenshellSequenceAssignWorkPlanOptions {
    owner_history?: IfcOpenshellInstance;
    user?: IfcOpenshellInstance;
    application?: IfcOpenshellInstance;
  }

  export interface IfcOpenshellSequenceCopyWorkScheduleOptions {
    owner_history?: IfcOpenshellInstance;
    user?: IfcOpenshellInstance;
    application?: IfcOpenshellInstance;
  }

  export interface IfcOpenshellSequenceCreateBaselineOptions {
    name?: string;
    owner_history?: IfcOpenshellInstance;
    user?: IfcOpenshellInstance;
    application?: IfcOpenshellInstance;
  }

  export interface IfcOpenshellSequenceDuplicateTaskOptions {
    owner_history?: IfcOpenshellInstance;
    user?: IfcOpenshellInstance;
    application?: IfcOpenshellInstance;
  }

  export interface IfcOpenshellSequenceRemoveOptions {
    user?: IfcOpenshellInstance;
    application?: IfcOpenshellInstance;
  }

  export interface IfcOpenshellShapeBuilderAxis2Placement2dOptions {
    position: number[];
    x_direction?: number[];
  }

  export interface IfcOpenshellShapeBuilderAxis2Placement3dOptions {
    position: number[];
    z_axis: number[];
    x_axis: number[];
  }

  export interface IfcOpenshellShapeBuilderBlockOptions {
    position: number[];
    x_length: number;
    y_length: number;
    z_length: number;
  }

  export interface IfcOpenshellShapeBuilderEllipseCurveOptions {
    x_axis_radius: number;
    y_axis_radius: number;
    position: number[];
    trim_points: number[][];
    ref_x_direction?: number[];
    trim_points_mask: number[];
  }

  export interface IfcOpenshellShapeBuilderExtrudeOptions {
    profile_or_curve: IfcOpenshellInstance;
    magnitude: number;
    position: number[];
    extrusion_vector: number[];
    position_z_axis: number[];
    position_x_axis: number[];
    position_y_axis?: number[];
  }

  export interface IfcOpenshellShapeBuilderHalfSpaceSolidOptions {
    plane: IfcOpenshellInstance;
    agreement_flag: boolean;
  }

  export interface IfcOpenshellShapeBuilderMepBendShapeOptions {
    segment: IfcOpenshellInstance;
    start_length: number;
    end_length: number;
    angle: number;
    radius: number;
    bend_vector: number[];
    flip_z_axis: boolean;
  }

  export interface IfcOpenshellShapeBuilderMepTransitionCalculateOptions {
    start_half_dim: number[];
    end_half_dim: number[];
    offset: number[];
    diff?: number[];
    end_profile: boolean;
    length?: number;
    angle?: number;
  }

  export interface IfcOpenshellShapeBuilderMepTransitionLengthOptions {
    start_half_dim: number[];
    end_half_dim: number[];
    angle: number;
    profile_offset: number[];
  }

  export interface IfcOpenshellShapeBuilderMepTransitionShapeOptions {
    start_segment: IfcOpenshellInstance;
    end_segment: IfcOpenshellInstance;
    start_length: number;
    end_length: number;
    angle: number;
    profile_offset: number[];
  }

  export interface IfcOpenshellShapeBuilderMirrorOptions {
    item: IfcOpenshellInstance;
    mirror_axes: number[];
    mirror_point: number[];
    create_copy: boolean;
    placement_matrix: number[];
  }

  export interface IfcOpenshellShapeBuilderPolylineOptions {
    points: number[][];
    closed?: boolean;
    position_offset?: number[];
    arc_points: number[];
  }

  export interface IfcOpenshellShapeBuilderProfileOptions {
    outer_curve: IfcOpenshellInstance;
    name?: string;
    inner_curves: IfcOpenshellParseInstanceList;
    profile_type?: string;
  }

  export interface IfcOpenshellShapeBuilderRepresentationOptions {
    context: IfcOpenshellInstance;
    items: IfcOpenshellParseInstanceList;
    representation_type?: string;
  }

  export interface IfcOpenshellShapeBuilderRotateOptions {
    item: IfcOpenshellInstance;
    angle: number;
    pivot_point: number[];
    counter_clockwise: boolean;
    create_copy: boolean;
  }

  export interface IfcOpenshellShapeBuilderSphereOptions {
    radius: number;
    center: number[];
  }

  export interface IfcOpenshellShapeBuilderTranslateOptions {
    item: IfcOpenshellInstance;
    translation: number[];
    create_copy: boolean;
  }

  export interface IfcOpenshellSpatialAssignContainerOptions {
    products: IfcOpenshellParseInstanceList;
    relating_structure: IfcOpenshellInstance;
    owner_history?: IfcOpenshellInstance;
    user?: IfcOpenshellInstance;
    application?: IfcOpenshellInstance;
  }

  export interface IfcOpenshellSpatialDereferenceStructureOptions {
    products: IfcOpenshellParseInstanceList;
    relating_structure: IfcOpenshellInstance;
    user?: IfcOpenshellInstance;
    application?: IfcOpenshellInstance;
  }

  export interface IfcOpenshellSpatialReferenceStructureOptions {
    products: IfcOpenshellParseInstanceList;
    relating_structure: IfcOpenshellInstance;
    owner_history?: IfcOpenshellInstance;
    user?: IfcOpenshellInstance;
    application?: IfcOpenshellInstance;
  }

  export interface IfcOpenshellSpatialUnassignContainerOptions {
    products: IfcOpenshellParseInstanceList;
    user?: IfcOpenshellInstance;
    application?: IfcOpenshellInstance;
  }

  export interface IfcOpenshellStructuralAddStructuralActivityOptions {
    activity_owner_history?: IfcOpenshellInstance;
    relationship_owner_history?: IfcOpenshellInstance;
  }

  export interface IfcOpenshellStructuralAddStructuralBoundaryConditionOptions {
    name?: string;
    connection?: IfcOpenshellInstance;
  }

  export interface IfcOpenshellStructuralAssignStructuralAnalysisModelOptions {
    owner_history?: IfcOpenshellInstance;
    user?: IfcOpenshellInstance;
    application?: IfcOpenshellInstance;
  }

  export interface IfcOpenshellStructuralRemoveStructuralBoundaryConditionOptions {
    connection?: IfcOpenshellInstance;
    boundary_condition?: IfcOpenshellInstance;
  }

  export interface IfcOpenshellStructuralUnassignStructuralAnalysisModelOptions {
    user?: IfcOpenshellInstance;
    application?: IfcOpenshellInstance;
  }

  export interface IfcOpenshellStyleAssignItemStyleOptions {
    item: IfcOpenshellInstance;
    style?: IfcOpenshellInstance;
    should_use_presentation_style_assignment: boolean;
  }

  export interface IfcOpenshellSystemAddPortOptions {
    element?: IfcOpenshellInstance;
    owner_history?: IfcOpenshellInstance;
    user?: IfcOpenshellInstance;
    application?: IfcOpenshellInstance;
  }

  export interface IfcOpenshellSystemAddSystemOptions {
    ifc_class: string;
    owner_history?: IfcOpenshellInstance;
  }

  export interface IfcOpenshellSystemAssignFlowControlOptions {
    relating_flow_element: IfcOpenshellInstance;
    related_flow_control: IfcOpenshellInstance;
    owner_history?: IfcOpenshellInstance;
    user?: IfcOpenshellInstance;
    application?: IfcOpenshellInstance;
  }

  export interface IfcOpenshellSystemAssignPortOptions {
    element: IfcOpenshellInstance;
    port: IfcOpenshellInstance;
    owner_history?: IfcOpenshellInstance;
    user?: IfcOpenshellInstance;
    application?: IfcOpenshellInstance;
  }

  export interface IfcOpenshellSystemAssignSystemOptions {
    products: IfcOpenshellParseInstanceList;
    system: IfcOpenshellInstance;
    owner_history?: IfcOpenshellInstance;
    user?: IfcOpenshellInstance;
    application?: IfcOpenshellInstance;
  }

  export interface IfcOpenshellSystemConnectPortOptions {
    port1: IfcOpenshellInstance;
    port2: IfcOpenshellInstance;
    direction: string;
    element?: IfcOpenshellInstance;
    owner_history?: IfcOpenshellInstance;
    user?: IfcOpenshellInstance;
    application?: IfcOpenshellInstance;
  }

  export interface IfcOpenshellSystemUnassignFlowControlOptions {
    relating_flow_element: IfcOpenshellInstance;
    related_flow_control: IfcOpenshellInstance;
    user?: IfcOpenshellInstance;
    application?: IfcOpenshellInstance;
  }

  export interface IfcOpenshellSystemUnassignPortOptions {
    element: IfcOpenshellInstance;
    port: IfcOpenshellInstance;
    user?: IfcOpenshellInstance;
    application?: IfcOpenshellInstance;
  }

  export interface IfcOpenshellSystemUnassignSystemOptions {
    products: IfcOpenshellParseInstanceList;
    system: IfcOpenshellInstance;
    user?: IfcOpenshellInstance;
    application?: IfcOpenshellInstance;
  }

  export interface IfcOpenshellTypeAssignTypeOptions {
    objects: IfcOpenshellParseInstanceList;
    relating_type: IfcOpenshellInstance;
    should_map_representations?: boolean;
    owner_history?: IfcOpenshellInstance;
    user?: IfcOpenshellInstance;
    application?: IfcOpenshellInstance;
  }

  export interface IfcOpenshellTypeUnassignTypeOptions {
    objects: IfcOpenshellParseInstanceList;
    user?: IfcOpenshellInstance;
    application?: IfcOpenshellInstance;
  }

  export class IfcOpenshellAggregationType {
    readonly ptr: number;
    destroy(): void;
    asAggregationType(): IfcOpenshellAggregationType | null;
    bound1(): number;
    bound2(): number;
    kind(): string;
    typeOfAggregation(): number;
    typeOfAggregationString(): string;
    typeOfElement(): IfcOpenshellParameterType | null;
  }

  export class IfcOpenshellAttribute {
    readonly ptr: number;
    destroy(): void;
    name(): string;
    optional(): boolean;
    typeOfAttribute(): IfcOpenshellParameterType | null;
  }

  export class IfcOpenshellParseAttributeValue {
    readonly ptr: number;
    destroy(): void;
    asBool(): boolean;
    asDouble(): number;
    asDoubleList(): number[];
    asDoubleListList(): number[][];
    asEnumerationIndex(): number;
    asEnumerationType(): IfcOpenshellEnumeration | null;
    asEnumerationValue(): string;
    asInstance(): IfcOpenshellInstance | null;
    asInstanceIdListList(): number[][];
    asInstanceList(): IfcOpenshellParseInstanceList;
    asInt32(): number;
    asInt32List(): number[];
    asInt32ListList(): number[][];
    asString(): string;
    asStringList(): string[];
    isNull(): boolean;
    size(): number;
    type(): string;
  }

  export class IfcOpenshellGeomBrepElement {
    readonly ptr: number;
    destroy(): void;
    calcSurfaceArea(): number;
    calcVolume(): number;
    calculateProjectedSurfaceArea(along_x: number, along_y: number, along_z: number): boolean;
    geometry(): IfcOpenshellGeomBrepRepresentation;
  }

  export class IfcOpenshellGeomBrepRepresentation {
    readonly ptr: number;
    destroy(): void;
    asCompound(force_meters: boolean): IfcOpenshellGeomConversionResultShape | null;
    calculateProjectedSurfaceArea(ax: IfcOpenshellGeomTaxonomyMatrix4, along_x: number, along_y: number, along_z: number): boolean;
    calculateSurfaceArea(): number;
    calculateVolume(): number;
    entity(): string;
    id(): string;
    item(i: number): IfcOpenshellGeomConversionResultShape | null;
    itemId(i: number): number;
    settings(): IfcOpenshellGeomSettings;
    size(): number;
  }

  export class IfcOpenshellGeomBuffer {
    readonly ptr: number;
    destroy(): void;
    getValue(): string;
    isReady(): boolean;
  }

  export class IfcOpenshellGeomConversionResultShape {
    readonly ptr: number;
    destroy(): void;
    add(arg_0: IfcOpenshellGeomConversionResultShape): IfcOpenshellGeomConversionResultShape | null;
    area(): number;
    box(): IfcOpenshellGeomConversionResultShape | null;
    concat(arg_0: IfcOpenshellGeomConversionResultShape): IfcOpenshellGeomConversionResultShape | null;
    convexTag(value: boolean): void;
    edges(): IfcOpenshellGeomConversionResultShape[];
    facets(): IfcOpenshellGeomConversionResultShape[];
    halfspaces(): IfcOpenshellGeomConversionResultShape | null;
    intersect(arg_0: IfcOpenshellGeomConversionResultShape): IfcOpenshellGeomConversionResultShape | null;
    isManifold(): boolean;
    length(): number;
    moved(arg_0: IfcOpenshellGeomTaxonomyMatrix4): IfcOpenshellGeomConversionResultShape | null;
    numEdges(): number;
    numFaces(): number;
    numVertices(): number;
    serialize(): string;
    serializeObj(): string;
    solid(): IfcOpenshellGeomConversionResultShape | null;
    solidMt(): IfcOpenshellGeomConversionResultShape | null;
    subtract(arg_0: IfcOpenshellGeomConversionResultShape): IfcOpenshellGeomConversionResultShape | null;
    surfaceAreaAlongDirection(tol: number, arg_1: IfcOpenshellGeomTaxonomyMatrix4, along_x: number, along_y: number, along_z: number): boolean;
    surfaceGenus(): number;
    vertices(): IfcOpenshellGeomConversionResultShape[];
    volume(): number;
    wrapInCompound(): IfcOpenshellGeomConversionResultShape | null;
  }

  export class IfcOpenshellDeclaration {
    readonly ptr: number;
    destroy(): void;
    asEntity(): IfcOpenshellEntity | null;
    asEnumerationType(): IfcOpenshellEnumeration | null;
    asSelectType(): IfcOpenshellSelectType | null;
    asTypeDeclaration(): IfcOpenshellTypeDeclaration | null;
    indexInSchema(): number;
    isA(name: string): boolean;
    name(): string;
    nameUc(): string;
    schema(): IfcOpenshellSchema | null;
    type(): number;
  }

  export class IfcOpenshellGeomElement {
    readonly ptr: number;
    destroy(): void;
    context(): string;
    guid(): string;
    id(): number;
    name(): string;
    parentId(): number;
    parents(): IfcOpenshellGeomElement[];
    product(): IfcOpenshellInstance;
    transformation(): IfcOpenshellGeomTransformation;
    transformationBuffer(): IfcOpenshellRawValue;
    transformationBufferSize(): number;
    type(): string;
    uniqueId(): string;
  }

  export class IfcOpenshellEntity {
    readonly ptr: number;
    destroy(): void;
    allAttributes(): IfcOpenshellAttribute[];
    allInverseAttributes(): IfcOpenshellInverseAttribute[];
    argumentTypes(): string[];
    asEntity(): IfcOpenshellEntity | null;
    attributeByIndex(index: number): IfcOpenshellAttribute | null;
    attributeCount(): number;
    attributeIndex(attr_name: string): number;
    attributes(): IfcOpenshellAttribute[];
    derived(): boolean[];
    inverseAttributes(): IfcOpenshellInverseAttribute[];
    isAbstract(): boolean;
    setAttributes(attributes: IfcOpenshellAttribute[], derived: boolean[]): void;
    setInverseAttributes(inverse_attributes: IfcOpenshellInverseAttribute[]): void;
    setSubtypes(subtypes: IfcOpenshellEntity[]): void;
    subtypes(): IfcOpenshellEntity[];
    supertype(): IfcOpenshellEntity | null;
  }

  export class IfcOpenshellEnumeration {
    readonly ptr: number;
    destroy(): void;
    argumentTypes(): string[];
    asEnumerationType(): IfcOpenshellEnumeration | null;
    enumerationItems(): string[];
    lookupEnumOffset(value_name: string): number;
    lookupEnumValue(i: number): string;
  }

  export class IfcOpenshellFile {
    readonly ptr: number;
    destroy(): void;
    add(entity: IfcOpenshellInstance, instance_id: number): IfcOpenshellInstance;
    addEntity(instance: IfcOpenshellInstance, id: number): IfcOpenshellInstance;
    addTypeRef(new_entity: IfcOpenshellInstance): void;
    batch(): void;
    buildInverses(): void;
    buildInverses(entity: IfcOpenshellInstance): void;
    byGuid(global_id: string): IfcOpenshellInstance;
    byId(instance_id: number): IfcOpenshellInstance;
    byType(type_name: string): IfcOpenshellParseInstanceList;
    byTypeExclSubtypes(type_name: string): IfcOpenshellParseInstanceList;
    bypassType(type_name: string): void;
    create(declaration: IfcOpenshellDeclaration, instance_id: number): IfcOpenshellInstance;
    createEntityByName(type_name: string): IfcOpenshellInstance;
    createEntityByNameWithId(type_name: string, id: number): IfcOpenshellInstance;
    createTimestamp(): string;
    entityNames(): number[];
    filePointer(): number;
    freshId(): number;
    getInverse(instance: IfcOpenshellInstance): IfcOpenshellParseInstanceList;
    getInverseIndices(instance: IfcOpenshellInstance): number[];
    getInverseIndicesById(instance_id: number): number[];
    getInversesByDeclaration(instance_id: number, declaration: IfcOpenshellDeclaration, attribute_index: number): IfcOpenshellInstance[];
    getMaxId(): number;
    getTotalInverses(instance: IfcOpenshellInstance): number;
    getTotalInversesById(instance_id: number): number;
    getUnit(unit_type: string): number;
    good(): number;
    header(): IfcOpenshellHeader | null;
    headerFileDescription(): IfcOpenshellInstance | null;
    headerFileName(): IfcOpenshellInstance | null;
    headerFileSchema(): IfcOpenshellInstance | null;
    ifcrootType(): IfcOpenshellDeclaration | null;
    initialize(path: string, type: number, read_only: boolean): boolean;
    instancesByReference(reference_id: number): IfcOpenshellParseInstanceList;
    keyValueStoreIter(prefix: string): string[];
    keyValueStoreQuery(key: string): number[];
    processDeletionInverse(entity: IfcOpenshellInstance): void;
    recalculateIdCounter(): void;
    remove(entity: IfcOpenshellInstance): void;
    removeTypeRef(new_entity: IfcOpenshellInstance): void;
    resetIdentityCache(): void;
    schema(): IfcOpenshellSchema | null;
    schemaName(): string;
    storageMode(): number;
    toString(): string;
    traverse(instance: IfcOpenshellInstance, max_depth: number): IfcOpenshellParseInstanceList;
    traverseBreadthFirst(instance: IfcOpenshellInstance, max_depth: number): IfcOpenshellParseInstanceList;
    types(): string[];
    unbatch(): void;
    write(path: string): void;
  }

  export class IfcOpenshellFileDescription {
    readonly ptr: number;
    destroy(): void;
    class(): IfcOpenshellEntity;
    description(): string[];
    implementationLevel(): string;
    initialize(v1_description: string[], v2_implementation_level: string): IfcOpenshellFileDescription;
    setdescription(v: string[]): void;
    setimplementationLevel(v: string): void;
  }

  export class IfcOpenshellFileName {
    readonly ptr: number;
    destroy(): void;
    author(): string[];
    authorization(): string;
    class(): IfcOpenshellEntity;
    initialize(v1_name: string, v2_time_stamp: string, v3_author: string[], v4_organization: string[], v5_preprocessor_version: string, v6_originating_system: string, v7_authorization: string): IfcOpenshellFileName;
    name(): string;
    organization(): string[];
    originatingSystem(): string;
    preprocessorVersion(): string;
    setauthor(v: string[]): void;
    setauthorization(v: string): void;
    setname(v: string): void;
    setorganization(v: string[]): void;
    setoriginatingSystem(v: string): void;
    setpreprocessorVersion(v: string): void;
    settimeStamp(v: string): void;
    timeStamp(): string;
  }

  export class IfcOpenshellFileSchema {
    readonly ptr: number;
    destroy(): void;
    class(): IfcOpenshellEntity;
    initialize(v1_schema_identifiers: string[]): IfcOpenshellFileSchema;
    schemaIdentifiers(): string[];
    setschemaIdentifiers(v: string[]): void;
  }

  export class IfcOpenshellGeomFunctionItemEvaluator {
    readonly ptr: number;
    destroy(): void;
    evaluate(): IfcOpenshellGeomTaxonomyItem;
    evaluateAt(u: number): number[];
    evaluateRange(ustart: number, uend: number, nsteps: number): IfcOpenshellGeomTaxonomyItem;
    evaluationPoints(): number[];
    evaluationPointsRange(ustart: number, uend: number, nsteps: number): number[];
  }

  export class IfcOpenshellGeomGeometrySerializer {
    readonly ptr: number;
    destroy(): void;
    finalize(): void;
    geometrySettings(): IfcOpenshellGeomSettings;
    isStreaming(): boolean;
    isTesselated(): boolean;
    read(f: IfcOpenshellFile, guid: string, representation_id: string, rt: number): IfcOpenshellGeomElement | null;
    ready(): boolean;
    setFile(arg_0: IfcOpenshellFile): void;
    setUnitNameAndMagnitude(name: string, magnitude: number): void;
    settings(): IfcOpenshellGeomSerializerSettings;
    writeBrepElement(o: IfcOpenshellGeomBrepElement): void;
    writeHeader(): void;
    writeTriangulationElement(o: IfcOpenshellGeomTriangulationElement): void;
  }

  export class IfcOpenshellHeader {
    readonly ptr: number;
    destroy(): void;
    file(): IfcOpenshellFile | null;
    fileDescription(): IfcOpenshellFileDescription;
    fileName(): IfcOpenshellFileName;
    fileSchema(): IfcOpenshellFileSchema;
    write(): string;
  }

  export class IfcOpenshellInstance {
    readonly ptr: number;
    destroy(): void;
    className(with_schema: boolean): string;
    declaration(): IfcOpenshellDeclaration;
    file(): IfcOpenshellFile | null;
    filePointer(): number;
    getArgument(attribute_index: number): IfcOpenshellParseAttributeValue;
    getArgumentByName(name: string): IfcOpenshellParseAttributeValue;
    getArgumentIndex(name: string): number;
    getArgumentName(index: number): string;
    getArgumentType(index: number): string;
    getAttributeCategory(name: string): number;
    getAttributeNames(): string[];
    getAttributeValue(index: number): IfcOpenshellParseAttributeValue;
    getInverse(name: string): IfcOpenshellParseInstanceList;
    getInverseAttributeByName(name: string): IfcOpenshellParseInstanceList;
    getInverseAttributeNames(): string[];
    id(): number;
    identity(): number;
    isA(declaration_name: string): boolean;
    setArgumentAsAggregateOfAggregateOfEntityInstance(index: number, value: number[][]): void;
    setArgumentBool(index: number, value: boolean): void;
    setArgumentDouble(index: number, value: number): void;
    setArgumentDoubleList(index: number, value: number[]): void;
    setArgumentDoubleListList(index: number, value: number[][]): void;
    setArgumentEnumeration(index: number, enumeration: IfcOpenshellEnumeration, enumeration_index: number): void;
    setArgumentEnumerationByName(index: number, value: string): boolean;
    setArgumentInstance(index: number, value: IfcOpenshellInstance): void;
    setArgumentInstanceList(index: number, value: IfcOpenshellParseInstanceList): void;
    setArgumentInt32(index: number, value: number): void;
    setArgumentInt32List(index: number, value: number[]): void;
    setArgumentInt32ListList(index: number, value: number[][]): void;
    setArgumentLogical(index: number, value: number): void;
    setArgumentString(index: number, value: string): void;
    setArgumentStringList(index: number, value: string[]): void;
    setAttributeValue(name: string, value: IfcOpenshellParseAttributeValue): void;
    toString(valid_spf: boolean): string;
    unsetArgument(index: number): void;
    unsetAttributeValue(name: string): void;
  }

  export class IfcOpenshellParseInstanceList {
    readonly ptr: number;
    destroy(): void;
    get(index: number): IfcOpenshellInstance | null;
    size(): number;
  }

  export class IfcOpenshellInstanceStreamer {
    readonly ptr: number;
    destroy(): void;
    bypassedInstances(): number[];
    hasSemicolon(): boolean;
    inverses(): string;
    pushPage(page_data: string): void;
    readInstancePy(type_as_declaration_instance: boolean): string;
    references(): string;
    semicolonCount(): number;
    status(): number;
  }

  export class IfcOpenshellInverseAttribute {
    readonly ptr: number;
    destroy(): void;
    attributeReference(): IfcOpenshellAttribute | null;
    bound1(): number;
    bound2(): number;
    entityReference(): IfcOpenshellEntity | null;
    name(): string;
    typeOfAggregation(): number;
    typeOfAggregationString(): string;
  }

  export class IfcOpenshellGeomIterator {
    readonly ptr: number;
    destroy(): void;
    boundsMax(): IfcOpenshellGeomTaxonomyPoint3;
    boundsMin(): IfcOpenshellGeomTaxonomyPoint3;
    computeBounds(with_geometry: boolean): void;
    create(): IfcOpenshellInstance;
    file(): IfcOpenshellFile | null;
    get(): IfcOpenshellGeomElement | null;
    getAsBrepElement(): IfcOpenshellGeomBrepElement | null;
    getAsSerializedElement(): IfcOpenshellGeomSerializedElement | null;
    getAsTriangulationElement(): IfcOpenshellGeomTriangulationElement | null;
    getLog(): string;
    getNative(): IfcOpenshellGeomBrepElement | null;
    getObject(id: number): IfcOpenshellGeomElement | null;
    getTaskItems(): IfcOpenshellGeomTaxonomyItem[];
    getTaskProducts(): IfcOpenshellInstance[][];
    hadErrorProcessingElements(): boolean;
    initialize(): boolean;
    next(): boolean;
    progress(): number;
    unitMagnitude(): number;
    unitName(): string;
  }

  export class IfcOpenshellNamedType {
    readonly ptr: number;
    destroy(): void;
    asNamedType(): IfcOpenshellNamedType | null;
    declaredType(): IfcOpenshellDeclaration | null;
    isA(name: string): boolean;
  }

  export class IfcOpenshellGeomOpaqueNumber {
    readonly ptr: number;
    destroy(): void;
    add(other: IfcOpenshellGeomOpaqueNumber): IfcOpenshellGeomOpaqueNumber;
    clone(): IfcOpenshellGeomOpaqueNumber | null;
    divide(other: IfcOpenshellGeomOpaqueNumber): IfcOpenshellGeomOpaqueNumber;
    equals(other: IfcOpenshellGeomOpaqueNumber): boolean;
    lessThan(other: IfcOpenshellGeomOpaqueNumber): boolean;
    multiply(other: IfcOpenshellGeomOpaqueNumber): IfcOpenshellGeomOpaqueNumber;
    negate(): IfcOpenshellGeomOpaqueNumber;
    subtract(other: IfcOpenshellGeomOpaqueNumber): IfcOpenshellGeomOpaqueNumber;
    toDouble(): number;
    toString(): string;
  }

  export class IfcOpenshellParameterType {
    readonly ptr: number;
    destroy(): void;
    asAggregationType(): IfcOpenshellAggregationType | null;
    asNamedType(): IfcOpenshellNamedType | null;
    asSimpleType(): IfcOpenshellSimpleType | null;
    kind(): string;
  }

  export class IfcOpenshellPsetTemplateHandle {
    readonly ptr: number;
    destroy(): void;
  }

  export class IfcOpenshellSchema {
    readonly ptr: number;
    destroy(): void;
    declarationByIndex(declaration_index: number): IfcOpenshellDeclaration | null;
    declarationByName(name: string): IfcOpenshellDeclaration | null;
    declarations(): IfcOpenshellDeclaration[];
    entities(): IfcOpenshellEntity[];
    enumerationTypes(): IfcOpenshellEnumeration[];
    name(): string;
    selectTypes(): IfcOpenshellSelectType[];
    typeDeclarations(): IfcOpenshellTypeDeclaration[];
  }

  export class IfcOpenshellSelectType {
    readonly ptr: number;
    destroy(): void;
    asSelectType(): IfcOpenshellSelectType | null;
    selectList(): IfcOpenshellDeclaration[];
    selectListNames(): string[];
  }

  export class IfcOpenshellGeomSerialization {
    readonly ptr: number;
    destroy(): void;
    brepData(): string;
    surfaceStyleIds(): number[];
    surfaceStyles(): number[];
  }

  export class IfcOpenshellGeomSerializedElement {
    readonly ptr: number;
    destroy(): void;
    geometry(): IfcOpenshellGeomSerialization;
  }

  export class IfcOpenshellGeomSerializer {
    readonly ptr: number;
    destroy(): void;
    finalize(): void;
    isStreaming(): boolean;
    ready(): boolean;
    setFile(arg_0: IfcOpenshellFile): void;
    writeHeader(): void;
  }

  export class IfcOpenshellGeomSerializerSettings {
    readonly ptr: number;
    destroy(): void;
    getBool(name: string): boolean;
    getDouble(name: string): number;
    getInt(name: string): number;
    getIntSet(name: string): number[];
    getString(name: string): string;
    getType(name: string): string;
    setBool(name: string, value: boolean): void;
    setDouble(name: string, value: number): void;
    setInt(name: string, value: number): void;
    setIntSet(name: string, value: number[]): void;
    setString(name: string, value: string): void;
    settingNames(): string[];
  }

  export class IfcOpenshellGeomSettings {
    readonly ptr: number;
    destroy(): void;
    getBool(name: string): boolean;
    getDouble(name: string): number;
    getDoubleList(name: string): number[];
    getInt(name: string): number;
    getIntSet(name: string): number[];
    getString(name: string): string;
    getStringSet(name: string): string[];
    getType(name: string): string;
    setBool(name: string, value: boolean): void;
    setDouble(name: string, value: number): void;
    setDoubleList(name: string, value: number[]): void;
    setInt(name: string, value: number): void;
    setIntSet(name: string, value: number[]): void;
    setString(name: string, value: string): void;
    setStringSet(name: string, value: string[]): void;
    settingNames(): string[];
  }

  export class IfcOpenshellSimpleType {
    readonly ptr: number;
    destroy(): void;
    asSimpleType(): IfcOpenshellSimpleType | null;
    declaredType(): number;
    kind(): string;
  }

  export class IfcOpenshellGeomSvgfillPolygon {
    readonly ptr: number;
    destroy(): void;
    boundaryPoint(index: number): number[];
    boundarySize(): number;
    innerBoundaryCount(): number;
    innerBoundaryPoint(boundary_index: number, point_index: number): number[];
    innerBoundarySize(boundary_index: number): number;
    pointInside(): number[];
  }

  export class IfcOpenshellGeomTaxonomyBooleanResult {
    readonly ptr: number;
    destroy(): void;
    addItem(item: IfcOpenshellGeomTaxonomyItem): void;
    itemAt(index: number): IfcOpenshellGeomTaxonomyItem;
    itemCount(): number;
    operation(): number;
  }

  export class IfcOpenshellGeomTaxonomyBsplineCurve {
    readonly ptr: number;
    destroy(): void;
    asItem(): IfcOpenshellGeomTaxonomyItem;
    controlPointAt(index: number): IfcOpenshellGeomTaxonomyPoint3;
    controlPointCount(): number;
    degree(): number;
    hasWeights(): boolean;
    knots(): number[];
    multiplicities(): number[];
    weights(): number[];
  }

  export class IfcOpenshellGeomTaxonomyBsplineSurface {
    readonly ptr: number;
    destroy(): void;
    asItem(): IfcOpenshellGeomTaxonomyItem;
    controlPointAt(row: number, col: number): IfcOpenshellGeomTaxonomyPoint3;
    controlPointColCountAt(row: number): number;
    controlPointRowCount(): number;
    degreeU(): number;
    degreeV(): number;
    hasWeights(): boolean;
    knotsU(): number[];
    knotsV(): number[];
    multiplicitiesU(): number[];
    multiplicitiesV(): number[];
    weightAt(row: number, col: number): number;
    weightColCountAt(row: number): number;
    weightRowCount(): number;
  }

  export class IfcOpenshellGeomTaxonomyCircle {
    readonly ptr: number;
    destroy(): void;
    matrix(): IfcOpenshellGeomTaxonomyMatrix4;
    radius(): number;
  }

  export class IfcOpenshellGeomTaxonomyCollection {
    readonly ptr: number;
    destroy(): void;
    addItem(item: IfcOpenshellGeomTaxonomyItem): void;
    itemAt(index: number): IfcOpenshellGeomTaxonomyItem;
    itemCount(): number;
  }

  export class IfcOpenshellGeomTaxonomyColour {
    readonly ptr: number;
    destroy(): void;
    getData(): number[];
  }

  export class IfcOpenshellGeomTaxonomyCylinder {
    readonly ptr: number;
    destroy(): void;
    matrix(): IfcOpenshellGeomTaxonomyMatrix4;
    radius(): number;
  }

  export class IfcOpenshellGeomTaxonomyDirection3 {
    readonly ptr: number;
    destroy(): void;
    getData(): number[];
  }

  export class IfcOpenshellGeomTaxonomyEdge {
    readonly ptr: number;
    destroy(): void;
  }

  export class IfcOpenshellGeomTaxonomyEllipse {
    readonly ptr: number;
    destroy(): void;
    matrix(): IfcOpenshellGeomTaxonomyMatrix4;
    radius1(): number;
    radius2(): number;
  }

  export class IfcOpenshellGeomTaxonomyExtrusion {
    readonly ptr: number;
    destroy(): void;
    basis(): IfcOpenshellGeomTaxonomyItem;
    depth(): number;
    direction(): IfcOpenshellGeomTaxonomyDirection3;
    matrix(): IfcOpenshellGeomTaxonomyMatrix4;
  }

  export class IfcOpenshellGeomTaxonomyFace {
    readonly ptr: number;
    destroy(): void;
    asItem(): IfcOpenshellGeomTaxonomyItem;
    basis(): IfcOpenshellGeomTaxonomyItem;
    loopAt(index: number): IfcOpenshellGeomTaxonomyLoop;
    loopCount(): number;
    matrix(): IfcOpenshellGeomTaxonomyMatrix4;
  }

  export class IfcOpenshellGeomTaxonomyItem {
    readonly ptr: number;
    destroy(): void;
    hash(): number;
    identity(): number;
    kind(): number;
  }

  export class IfcOpenshellGeomTaxonomyLine {
    readonly ptr: number;
    destroy(): void;
    asItem(): IfcOpenshellGeomTaxonomyItem;
    matrix(): IfcOpenshellGeomTaxonomyMatrix4;
  }

  export class IfcOpenshellGeomTaxonomyLoft {
    readonly ptr: number;
    destroy(): void;
    addItem(item: IfcOpenshellGeomTaxonomyItem): void;
    axis(): IfcOpenshellGeomTaxonomyItem;
    hasAxis(): boolean;
    itemAt(index: number): IfcOpenshellGeomTaxonomyItem;
    itemCount(): number;
    setAxis(value: IfcOpenshellGeomTaxonomyItem): void;
  }

  export class IfcOpenshellGeomTaxonomyLoop {
    readonly ptr: number;
    destroy(): void;
    edgeAt(index: number): IfcOpenshellGeomTaxonomyEdge;
    edgeCount(): number;
  }

  export class IfcOpenshellGeomTaxonomyMatrix4 {
    readonly ptr: number;
    destroy(): void;
    getData(): number[];
  }

  export class IfcOpenshellGeomTaxonomyNode {
    readonly ptr: number;
    destroy(): void;
  }

  export class IfcOpenshellGeomTaxonomyOffsetCurve {
    readonly ptr: number;
    destroy(): void;
    asItem(): IfcOpenshellGeomTaxonomyItem;
    basis(): IfcOpenshellGeomTaxonomyItem;
    offset(): number;
    reference(): IfcOpenshellGeomTaxonomyDirection3;
  }

  export class IfcOpenshellGeomTaxonomyPlane {
    readonly ptr: number;
    destroy(): void;
    matrix(): IfcOpenshellGeomTaxonomyMatrix4;
  }

  export class IfcOpenshellGeomTaxonomyPoint3 {
    readonly ptr: number;
    destroy(): void;
    getData(): number[];
  }

  export class IfcOpenshellGeomTaxonomyRevolve {
    readonly ptr: number;
    destroy(): void;
    angle(): number;
    axisOrigin(): IfcOpenshellGeomTaxonomyPoint3;
    basis(): IfcOpenshellGeomTaxonomyItem;
    direction(): IfcOpenshellGeomTaxonomyDirection3;
    hasAngle(): boolean;
    matrix(): IfcOpenshellGeomTaxonomyMatrix4;
  }

  export class IfcOpenshellGeomTaxonomyShell {
    readonly ptr: number;
    destroy(): void;
    faceAt(index: number): IfcOpenshellGeomTaxonomyFace;
    faceCount(): number;
  }

  export class IfcOpenshellGeomTaxonomySolid {
    readonly ptr: number;
    destroy(): void;
    matrix(): IfcOpenshellGeomTaxonomyMatrix4;
    shellAt(index: number): IfcOpenshellGeomTaxonomyShell;
    shellCount(): number;
  }

  export class IfcOpenshellGeomTaxonomySphere {
    readonly ptr: number;
    destroy(): void;
    matrix(): IfcOpenshellGeomTaxonomyMatrix4;
    radius(): number;
  }

  export class IfcOpenshellGeomTaxonomyStyle {
    readonly ptr: number;
    destroy(): void;
    diffuse(): IfcOpenshellGeomTaxonomyColour;
    hasSpecularity(): boolean;
    hasTransparency(): boolean;
    instanceId(): number;
    name(): string;
    specular(): IfcOpenshellGeomTaxonomyColour;
    specularity(): number;
    surface(): IfcOpenshellGeomTaxonomyColour;
    transparency(): number;
    useSurfaceColor(): boolean;
  }

  export class IfcOpenshellGeomTaxonomySweepAlongCurve {
    readonly ptr: number;
    destroy(): void;
    basis(): IfcOpenshellGeomTaxonomyItem;
    curve(): IfcOpenshellGeomTaxonomyItem;
    direction(): IfcOpenshellGeomTaxonomyDirection3;
    hasBasis(): boolean;
    hasCurve(): boolean;
    hasDirection(): boolean;
    hasMatrix(): boolean;
    hasSurface(): boolean;
    matrix(): IfcOpenshellGeomTaxonomyMatrix4;
    surface(): IfcOpenshellGeomTaxonomyItem;
  }

  export class IfcOpenshellGeomTaxonomyTorus {
    readonly ptr: number;
    destroy(): void;
    matrix(): IfcOpenshellGeomTaxonomyMatrix4;
    radius1(): number;
    radius2(): number;
  }

  export class IfcOpenshellGeomTransformation {
    readonly ptr: number;
    destroy(): void;
    matrix(): number[];
  }

  export class IfcOpenshellGeomTree {
    readonly ptr: number;
    destroy(): void;
    addFile(file: IfcOpenshellFile, settings: IfcOpenshellGeomSettings): void;
    addIterator(iterator: IfcOpenshellGeomIterator): void;
    clashAt(clashes: IfcOpenshellGeomTreeClashList, index: number): IfcOpenshellGeomTreeClash;
    clashClearanceMany(set_a: IfcOpenshellInstance[], set_b: IfcOpenshellInstance[], clearance: number, check_all: boolean): IfcOpenshellGeomTreeClashList;
    clashCollisionMany(set_a: IfcOpenshellInstance[], set_b: IfcOpenshellInstance[], allow_touching: boolean): IfcOpenshellGeomTreeClashList;
    clashCount(clashes: IfcOpenshellGeomTreeClashList): number;
    clashIntersectionMany(set_a: IfcOpenshellInstance[], set_b: IfcOpenshellInstance[], tolerance: number, check_all: boolean): IfcOpenshellGeomTreeClashList;
    distances(): number[];
    enableFaceStyles(): boolean;
    isManifold(faces: number[]): boolean;
    protrusionDistances(): number[];
    rayIntersectionAt(intersections: IfcOpenshellGeomTreeRayIntersectionList, index: number): IfcOpenshellGeomTreeRayIntersection;
    rayIntersectionCount(intersections: IfcOpenshellGeomTreeRayIntersectionList): number;
    selectBoxBounds(xmin: number, ymin: number, zmin: number, xmax: number, ymax: number, zmax: number, completely_within: boolean): IfcOpenshellParseInstanceList;
    selectBoxElement(instance: IfcOpenshellInstance, completely_within: boolean, extend: number): IfcOpenshellParseInstanceList;
    selectBoxPoint(x: number, y: number, z: number, extend: number): IfcOpenshellParseInstanceList;
    selectBrepElement(element: IfcOpenshellGeomBrepElement, completely_within: boolean, extend: number): IfcOpenshellParseInstanceList;
    selectElement(instance: IfcOpenshellInstance, completely_within: boolean, extend: number): IfcOpenshellParseInstanceList;
    selectPoint(x: number, y: number, z: number, extend: number): IfcOpenshellParseInstanceList;
    selectRay(origin_x: number, origin_y: number, origin_z: number, dir_x: number, dir_y: number, dir_z: number, length: number): IfcOpenshellGeomTreeRayIntersectionList;
    setEnableFaceStyles(enable: boolean): void;
    styleAt(index: number): IfcOpenshellGeomTaxonomyStyle;
    styleCount(): number;
    styles(): IfcOpenshellGeomTaxonomyStyle[];
    uint8ToB64(uuids_array: number[]): string;
  }

  export class IfcOpenshellGeomTreeClash {
    readonly ptr: number;
    destroy(): void;
    a(): IfcOpenshellInstance;
    b(): IfcOpenshellInstance;
    distance(): number;
    p1(): number[];
    p2(): number[];
    type(): number;
  }

  export class IfcOpenshellGeomTreeClashList {
    readonly ptr: number;
    destroy(): void;
  }

  export class IfcOpenshellGeomTreeRayIntersection {
    readonly ptr: number;
    destroy(): void;
    distance(): number;
    dotProduct(): number;
    instance(): IfcOpenshellInstance;
    normal(): number[];
    position(): number[];
    rayDistance(): number;
    styleIndex(): number;
  }

  export class IfcOpenshellGeomTreeRayIntersectionList {
    readonly ptr: number;
    destroy(): void;
  }

  export class IfcOpenshellGeomTriangulation {
    readonly ptr: number;
    destroy(): void;
    colorsBuffer(): number[];
    colorsBufferSize(): number;
    edges(): number[];
    edgesBuffer(): IfcOpenshellRawValue;
    edgesBufferSize(): number;
    edgesItemIds(): number[];
    edgesItemIdsBuffer(): IfcOpenshellRawValue;
    edgesItemIdsBufferSize(): number;
    faces(): number[];
    facesBuffer(): IfcOpenshellRawValue;
    facesBufferSize(): number;
    itemIds(): number[];
    itemIdsBuffer(): IfcOpenshellRawValue;
    itemIdsBufferSize(): number;
    materialAt(index: number): IfcOpenshellGeomTaxonomyStyle;
    materialCount(): number;
    materialIds(): number[];
    materialIdsBuffer(): IfcOpenshellRawValue;
    materialIdsBufferSize(): number;
    materials(): IfcOpenshellGeomTaxonomyStyle[];
    normals(): number[];
    normalsBuffer(): IfcOpenshellRawValue;
    normalsBufferSize(): number;
    polyhedralFacesWithHoles(): number[][][];
    polyhedralFacesWithoutHoles(): number[][];
    uvs(): number[];
    uvsBuffer(): IfcOpenshellRawValue;
    uvsBufferSize(): number;
    verts(): number[];
    vertsBuffer(): IfcOpenshellRawValue;
    vertsBufferSize(): number;
  }

  export class IfcOpenshellGeomTriangulationElement {
    readonly ptr: number;
    destroy(): void;
    geometry(): IfcOpenshellGeomTriangulation;
  }

  export class IfcOpenshellTypeDeclaration {
    readonly ptr: number;
    destroy(): void;
    argumentTypes(): string[];
    asTypeDeclaration(): IfcOpenshellTypeDeclaration | null;
    declaredType(): IfcOpenshellParameterType | null;
  }

  export class IfcOpenshellValue {
    readonly ptr: number;
    destroy(): void;
  }

  export interface IfcOpenshellAggregateModule {
    /** Assign products as parts of an aggregate object. */
    assignObject(file: IfcOpenshellFile, options: IfcOpenshellAggregateAssignObjectOptions): IfcOpenshellInstance;
    /** Remove products from their aggregate relationships. */
    unassignObject(file: IfcOpenshellFile, options: IfcOpenshellAggregateUnassignObjectOptions): void;
  }

  export interface IfcOpenshellAttributeModule {
    editAttributes(file: IfcOpenshellFile, options: IfcOpenshellAttributeEditAttributesOptions): void;
  }

  export interface IfcOpenshellBoundaryModule {
    /** Assign a planar connection geometry to a space boundary relationship. */
    assignConnectionGeometry(file: IfcOpenshellFile, rel_space_boundary: IfcOpenshellInstance, options: IfcOpenshellBoundaryAssignConnectionGeometryOptions): void;
    copyBoundary(file: IfcOpenshellFile, boundary: IfcOpenshellInstance): IfcOpenshellInstance;
    editAttributes(entity: IfcOpenshellInstance, options: IfcOpenshellBoundaryEditAttributesOptions): void;
    removeBoundary(file: IfcOpenshellFile, boundary: IfcOpenshellInstance): void;
  }

  export interface IfcOpenshellClassificationModule {
    addClassification(file: IfcOpenshellFile, name: string): IfcOpenshellInstance;
    /**
     * Add a classification reference and associate it with products.
     *
     * If an existing reference handle is provided, it is used directly.
     * Otherwise, a new IfcClassificationReference is created using the
     * optional identification, name, and classification fields.
     */
    addReference(file: IfcOpenshellFile, options: IfcOpenshellClassificationAddReferenceOptions): IfcOpenshellInstance;
    getReferences(element: IfcOpenshellInstance, should_inherit: boolean): IfcOpenshellParseInstanceList;
    removeClassification(file: IfcOpenshellFile, classification: IfcOpenshellInstance): void;
    /**
     * Remove a classification reference association from products.
     *
     * If the reference is no longer associated with any products after
     * removal, the reference entity itself is also deleted.
     */
    removeReference(file: IfcOpenshellFile, options: IfcOpenshellClassificationRemoveReferenceOptions): void;
  }

  export interface IfcOpenshellCogoModule {
    /** Add a single survey point as an IfcAnnotation. */
    addSurveyPoint(file: IfcOpenshellFile, options: IfcOpenshellCogoAddSurveyPointOptions): IfcOpenshellInstance;
    assignSurveyPoint(annotation: IfcOpenshellInstance, survey_point: IfcOpenshellInstance): void;
    editSurveyPoint(annotation: IfcOpenshellInstance, x: number, y: number, z: number): void;
  }

  export interface IfcOpenshellComputeModule {
    derived(instance: IfcOpenshellInstance, attribute_name: string): IfcOpenshellValue | null;
  }

  export interface IfcOpenshellConstraintModule {
    /**
     * Add a new IfcMetric constraint. If an objective is provided, the metric
     * is appended to the objective's benchmark values.
     */
    addMetric(file: IfcOpenshellFile, objective: IfcOpenshellInstance | null): IfcOpenshellInstance;
    addMetricReference(file: IfcOpenshellFile, metric: IfcOpenshellInstance, reference_path: string): IfcOpenshellParseInstanceList;
    addObjective(file: IfcOpenshellFile): IfcOpenshellInstance;
    /**
     * Assign a constraint to a list of products via IfcRelAssociatesConstraint.
     *
     * If the constraint is already associated with some of the products, only
     * new products are added to the existing relationship.
     */
    assignConstraint(file: IfcOpenshellFile, options: IfcOpenshellConstraintAssignConstraintOptions): IfcOpenshellInstance;
    removeConstraint(file: IfcOpenshellFile, constraint: IfcOpenshellInstance): void;
    removeMetric(file: IfcOpenshellFile, metric: IfcOpenshellInstance): void;
    /**
     * Remove constraint associations from products.
     *
     * Products are removed from existing IfcRelAssociatesConstraint
     * relationships. If no products remain, the relationship is removed.
     */
    unassignConstraint(file: IfcOpenshellFile, options: IfcOpenshellConstraintUnassignConstraintOptions): void;
  }

  export interface IfcOpenshellContextModule {
    addContext(file: IfcOpenshellFile, options: IfcOpenshellContextAddContextOptions): IfcOpenshellInstance;
    editContext(file: IfcOpenshellFile, context: IfcOpenshellInstance, attributes: number): void;
    removeContext(file: IfcOpenshellFile, context: IfcOpenshellInstance): void;
  }

  export interface IfcOpenshellControlModule {
    /** Assign a planning control or constraint to a list of objects. */
    assignControl(file: IfcOpenshellFile, options: IfcOpenshellControlAssignControlOptions): IfcOpenshellInstance;
    /** Remove a planning control or constraint from a list of objects. */
    unassignControl(file: IfcOpenshellFile, options: IfcOpenshellControlUnassignControlOptions): void;
  }

  export interface IfcOpenshellCostModule {
    addCostItem(file: IfcOpenshellFile, options: IfcOpenshellCostAddCostItemOptions): IfcOpenshellInstance;
    addCostItemQuantity(file: IfcOpenshellFile, cost_item: IfcOpenshellInstance, ifc_class: string): IfcOpenshellInstance;
    addCostSchedule(file: IfcOpenshellFile, name: string | null, predefined_type: string, update_date: string, owner_history: IfcOpenshellInstance | null): IfcOpenshellInstance;
    addCostValue(file: IfcOpenshellFile, parent: IfcOpenshellInstance): IfcOpenshellInstance;
    assignCostItemQuantity(file: IfcOpenshellFile, cost_item: IfcOpenshellInstance, products: IfcOpenshellInstance[], prop_name: string | null, options: IfcOpenshellCostAssignCostItemQuantityOptions): void;
    assignCostValue(file: IfcOpenshellFile, cost_item: IfcOpenshellInstance, cost_rate: IfcOpenshellInstance): void;
    calculateCostItemResourceValue(file: IfcOpenshellFile, cost_item: IfcOpenshellInstance): void;
    copyCostItem(file: IfcOpenshellFile, cost_item: IfcOpenshellInstance): IfcOpenshellParseInstanceList;
    copyCostItemValues(file: IfcOpenshellFile, source: IfcOpenshellInstance, destination: IfcOpenshellInstance): void;
    copyCostSchedule(file: IfcOpenshellFile, cost_schedule: IfcOpenshellInstance, options: IfcOpenshellCostCopyCostScheduleOptions): IfcOpenshellInstance;
    editCostItem(file: IfcOpenshellFile, cost_item: IfcOpenshellInstance, attributes: number): void;
    editCostItemQuantity(file: IfcOpenshellFile, physical_quantity: IfcOpenshellInstance, attributes: number): void;
    editCostSchedule(file: IfcOpenshellFile, cost_schedule: IfcOpenshellInstance, attributes: number): void;
    editCostValue(file: IfcOpenshellFile, cost_value: IfcOpenshellInstance, attributes: number, options: IfcOpenshellCostEditCostValueOptions): void;
    editCostValueFormula(file: IfcOpenshellFile, cost_value: IfcOpenshellInstance, formula: string): void;
    removeCostItem(file: IfcOpenshellFile, cost_item: IfcOpenshellInstance): void;
    removeCostItemQuantity(file: IfcOpenshellFile, cost_item: IfcOpenshellInstance, physical_quantity: IfcOpenshellInstance): void;
    removeCostSchedule(file: IfcOpenshellFile, cost_schedule: IfcOpenshellInstance): void;
    removeCostValue(file: IfcOpenshellFile, parent: IfcOpenshellInstance, cost_value: IfcOpenshellInstance): void;
    unassignCostItemQuantity(file: IfcOpenshellFile, cost_item: IfcOpenshellInstance, products: IfcOpenshellInstance[], options: IfcOpenshellCostUnassignCostItemQuantityOptions): void;
  }

  export interface IfcOpenshellDocumentModule {
    /** Add a new document information to the project. */
    addInformation(file: IfcOpenshellFile, options: IfcOpenshellDocumentAddInformationOptions): IfcOpenshellInstance;
    /** Create a new reference to a document. */
    addReference(file: IfcOpenshellFile, information: IfcOpenshellInstance | null): IfcOpenshellInstance;
    /** Assign a document to a list of products. */
    assignDocument(file: IfcOpenshellFile, options: IfcOpenshellDocumentAssignDocumentOptions): IfcOpenshellInstance;
    removeInformation(file: IfcOpenshellFile, information: IfcOpenshellInstance): void;
    removeReference(file: IfcOpenshellFile, reference: IfcOpenshellInstance): void;
    /** Unassign a document from a list of products. */
    unassignDocument(file: IfcOpenshellFile, options: IfcOpenshellDocumentUnassignDocumentOptions): void;
  }

  export interface IfcOpenshellDrawingModule {
    assignProduct(file: IfcOpenshellFile, options: IfcOpenshellDrawingAssignProductOptions): IfcOpenshellInstance;
    unassignProduct(file: IfcOpenshellFile, options: IfcOpenshellDrawingUnassignProductOptions): void;
  }

  export interface IfcOpenshellElementModule {
    getAggregate(instance: IfcOpenshellInstance): IfcOpenshellInstance | null;
    getContained(element: IfcOpenshellInstance): IfcOpenshellParseInstanceList;
    getContainer(instance: IfcOpenshellInstance, options: IfcOpenshellElementGetContainerOptions): IfcOpenshellInstance | null;
    getControls(element: IfcOpenshellInstance): IfcOpenshellParseInstanceList;
    getDecomposition(element: IfcOpenshellInstance, options: IfcOpenshellElementGetDecompositionOptions): IfcOpenshellParseInstanceList;
    getElementsByLayer(layer: IfcOpenshellInstance): IfcOpenshellParseInstanceList;
    getElementsByMaterial(material: IfcOpenshellInstance): IfcOpenshellParseInstanceList;
    getElementsByProfile(profile: IfcOpenshellInstance): IfcOpenshellParseInstanceList;
    getElementsByRepresentation(representation: IfcOpenshellInstance): IfcOpenshellParseInstanceList;
    getElementsByStyle(style: IfcOpenshellInstance): IfcOpenshellParseInstanceList;
    getFilledVoid(element: IfcOpenshellInstance): IfcOpenshellInstance | null;
    getGroups(element: IfcOpenshellInstance): IfcOpenshellParseInstanceList;
    getLayers(element: IfcOpenshellInstance): IfcOpenshellParseInstanceList;
    getMaterial(instance: IfcOpenshellInstance, options: IfcOpenshellElementGetMaterialOptions): IfcOpenshellInstance | null;
    getNest(instance: IfcOpenshellInstance): IfcOpenshellInstance | null;
    getOpenings(element: IfcOpenshellInstance): IfcOpenshellParseInstanceList;
    getParent(instance: IfcOpenshellInstance): IfcOpenshellInstance | null;
    getParts(element: IfcOpenshellInstance): IfcOpenshellParseInstanceList;
    getPsetIds(element: IfcOpenshellInstance, options: IfcOpenshellElementGetPsetIdsOptions): IfcOpenshellParseInstanceList;
    getReferencedElements(reference: IfcOpenshellInstance): IfcOpenshellParseInstanceList;
    getReferencedStructures(element: IfcOpenshellInstance): IfcOpenshellParseInstanceList;
    getShapeAspects(element: IfcOpenshellInstance, options: IfcOpenshellElementGetShapeAspectsOptions): IfcOpenshellParseInstanceList;
    getStructureReferencedElements(structure: IfcOpenshellInstance): IfcOpenshellParseInstanceList;
    getStyles(element: IfcOpenshellInstance): IfcOpenshellParseInstanceList;
    getType(instance: IfcOpenshellInstance): IfcOpenshellInstance | null;
    getTypes(type_element: IfcOpenshellInstance): IfcOpenshellParseInstanceList;
    getVoidedElement(element: IfcOpenshellInstance): IfcOpenshellInstance | null;
    isUserdefinedType(element: IfcOpenshellInstance): boolean;
    removeDeep(element: IfcOpenshellInstance): void;
    replaceElement(old_element: IfcOpenshellInstance, new_element: IfcOpenshellInstance): void;
  }

  export interface IfcOpenshellEntityModule {
    removeDeep2(instance: IfcOpenshellInstance): void;
    removeDeep2Ex(instance: IfcOpenshellInstance, also_consider: IfcOpenshellInstance[], do_not_delete: IfcOpenshellInstance[]): void;
  }

  export interface IfcOpenshellFeatureModule {
    addFeature(file: IfcOpenshellFile, options: IfcOpenshellFeatureAddFeatureOptions): IfcOpenshellInstance;
    addFilling(file: IfcOpenshellFile, opening: IfcOpenshellInstance, element: IfcOpenshellInstance): IfcOpenshellInstance;
    removeFeature(file: IfcOpenshellFile, options: IfcOpenshellFeatureRemoveFeatureOptions): void;
    removeFilling(file: IfcOpenshellFile, element: IfcOpenshellInstance): void;
  }

  export interface IfcOpenshellGeomModule {
    arrangePolygons(polygons_cpp: IfcOpenshellGeomSvgfillPolygon[]): IfcOpenshellGeomSvgfillPolygon[];
    convertLoopToFunctionItem(loop_item_cpp: IfcOpenshellGeomTaxonomyItem): IfcOpenshellGeomTaxonomyItem;
    createBuffer(): IfcOpenshellGeomBuffer;
    createBufferFromFilename(filename: string): IfcOpenshellGeomBuffer;
    createEpeckFromDouble(value: number): IfcOpenshellGeomOpaqueNumber | null;
    createEpeckFromInt(value: number): IfcOpenshellGeomOpaqueNumber | null;
    createEpeckFromString(value_cpp: string): IfcOpenshellGeomOpaqueNumber | null;
    createFunctionItemEvaluator(settings_cpp: IfcOpenshellGeomSettings, fn_item_cpp: IfcOpenshellGeomTaxonomyItem): IfcOpenshellGeomFunctionItemEvaluator | null;
    createGeometrySerializerByPath(format: string, output_filename: string, output_temp_filename: string, geometry_settings: IfcOpenshellGeomSettings, serializer_settings: IfcOpenshellGeomSerializerSettings): IfcOpenshellGeomGeometrySerializer | null;
    createGeometrySerializerByStream(format: string, output: IfcOpenshellGeomBuffer, output_temp: IfcOpenshellGeomBuffer, geometry_settings: IfcOpenshellGeomSettings, serializer_settings: IfcOpenshellGeomSerializerSettings): IfcOpenshellGeomGeometrySerializer | null;
    createIterator(geometry_library_cpp: string, settings_cpp: IfcOpenshellGeomSettings, file_cpp: IfcOpenshellFile, num_threads: number): IfcOpenshellGeomIterator | null;
    createIteratorWithIncludeExclude(geometry_library_cpp: string, settings_cpp: IfcOpenshellGeomSettings, file_cpp: IfcOpenshellFile, elems_cpp: string[], include: boolean, num_threads: number): IfcOpenshellGeomIterator | null;
    createIteratorWithIncludeExcludeGlobalid(geometry_library_cpp: string, settings_cpp: IfcOpenshellGeomSettings, file_cpp: IfcOpenshellFile, elems_cpp: string[], include: boolean, num_threads: number): IfcOpenshellGeomIterator | null;
    createIteratorWithIncludeExcludeId(geometry_library_cpp: string, settings_cpp: IfcOpenshellGeomSettings, file_cpp: IfcOpenshellFile, elems_cpp: number[], include: boolean, num_threads: number): IfcOpenshellGeomIterator | null;
    createJsonSerializer(file: IfcOpenshellFile, filename: string): IfcOpenshellGeomSerializer;
    createRocksdbSerializerStreaming(input_filename: string, rocksdb_filename: string): IfcOpenshellGeomSerializer;
    createSerializerSettings(): IfcOpenshellGeomSerializerSettings;
    createSettings(): IfcOpenshellGeomSettings;
    createShape(settings_cpp: IfcOpenshellGeomSettings, instance_cpp: IfcOpenshellInstance, representation: IfcOpenshellInstance | null, geometry_library: string | null): IfcOpenshellGeomElement | null;
    createTree(): IfcOpenshellGeomTree;
    createTreeFromFile(file: IfcOpenshellFile): IfcOpenshellGeomTree;
    createTreeFromFileWithSettings(file: IfcOpenshellFile, settings: IfcOpenshellGeomSettings): IfcOpenshellGeomTree;
    createTreeFromIterator(iterator: IfcOpenshellGeomIterator): IfcOpenshellGeomTree;
    createXmlSerializer(file: IfcOpenshellFile, filename: string): IfcOpenshellGeomSerializer;
    helmertCurvePoint(A0: number, A1: number, A2: number, s: number): number[];
    lineSegmentsToPolygons(solver: number, eps: number, segments_json_cpp: string): IfcOpenshellGeomSvgfillPolygon[];
    mapShape(settings_cpp: IfcOpenshellGeomSettings, instance_cpp: IfcOpenshellInstance): IfcOpenshellGeomTaxonomyItem;
    naryUnion(shapes_cpp: IfcOpenshellGeomConversionResultShape[]): IfcOpenshellGeomConversionResultShape | null;
    svgToLineSegments(svg_data_cpp: string, class_name: string | null): string;
    svgToPolygons(svg_data_cpp: string, class_name: string | null): IfcOpenshellGeomSvgfillPolygon[];
    taxonomyCreateBooleanResult(operation: number): IfcOpenshellGeomTaxonomyBooleanResult;
    taxonomyCreateBox(dx: number, dy: number, dz: number): IfcOpenshellGeomTaxonomySolid;
    taxonomyCreateBsplineCurve(degree: number): IfcOpenshellGeomTaxonomyBsplineCurve;
    taxonomyCreateBsplineSurface(degree_u: number, degree_v: number): IfcOpenshellGeomTaxonomyBsplineSurface;
    taxonomyCreateCircle(origin_x: number, origin_y: number, origin_z: number, dir_x: number, dir_y: number, dir_z: number, radius: number): IfcOpenshellGeomTaxonomyCircle;
    taxonomyCreateCollection(): IfcOpenshellGeomTaxonomyCollection;
    taxonomyCreateCylinder(origin_x: number, origin_y: number, origin_z: number, dir_x: number, dir_y: number, dir_z: number, radius: number): IfcOpenshellGeomTaxonomyCylinder;
    taxonomyCreateDirection3(x: number, y: number, z: number): IfcOpenshellGeomTaxonomyDirection3;
    taxonomyCreateEllipse(origin_x: number, origin_y: number, origin_z: number, dir_x: number, dir_y: number, dir_z: number, radius1: number, radius2: number): IfcOpenshellGeomTaxonomyEllipse;
    taxonomyCreateExtrusion(basis_cpp: IfcOpenshellGeomTaxonomyItem, direction_cpp: IfcOpenshellGeomTaxonomyDirection3, depth: number): IfcOpenshellGeomTaxonomyExtrusion;
    taxonomyCreateLine(origin_x: number, origin_y: number, origin_z: number, dir_x: number, dir_y: number, dir_z: number): IfcOpenshellGeomTaxonomyLine;
    taxonomyCreateLoft(): IfcOpenshellGeomTaxonomyLoft;
    taxonomyCreateNode(): IfcOpenshellGeomTaxonomyNode;
    taxonomyCreateOffsetCurve(basis: IfcOpenshellGeomTaxonomyItem, reference: IfcOpenshellGeomTaxonomyDirection3, offset: number): IfcOpenshellGeomTaxonomyOffsetCurve;
    taxonomyCreatePlane(origin_x: number, origin_y: number, origin_z: number, dir_x: number, dir_y: number, dir_z: number): IfcOpenshellGeomTaxonomyPlane;
    taxonomyCreatePoint3(x: number, y: number, z: number): IfcOpenshellGeomTaxonomyPoint3;
    taxonomyCreateRevolve(basis_cpp: IfcOpenshellGeomTaxonomyItem, axis_origin_cpp: IfcOpenshellGeomTaxonomyPoint3, direction_cpp: IfcOpenshellGeomTaxonomyDirection3, angle: number): IfcOpenshellGeomTaxonomyRevolve;
    taxonomyCreateSphere(origin_x: number, origin_y: number, origin_z: number, dir_x: number, dir_y: number, dir_z: number, radius: number): IfcOpenshellGeomTaxonomySphere;
    taxonomyCreateSweepAlongCurve(basis_face_cpp: IfcOpenshellGeomTaxonomyFace, directrix_cpp: IfcOpenshellGeomTaxonomyItem, reference_direction_cpp: IfcOpenshellGeomTaxonomyDirection3): IfcOpenshellGeomTaxonomySweepAlongCurve;
    taxonomyCreateTorus(origin_x: number, origin_y: number, origin_z: number, dir_x: number, dir_y: number, dir_z: number, radius1: number, radius2: number): IfcOpenshellGeomTaxonomyTorus;
    taxonomyFunctionItemEnd(item_cpp: IfcOpenshellGeomTaxonomyItem): number;
    taxonomyFunctionItemStart(item_cpp: IfcOpenshellGeomTaxonomyItem): number;
  }

  export interface IfcOpenshellGeometryModule {
    addAxisRepresentation(file: IfcOpenshellFile, context: IfcOpenshellInstance, axis: number[][]): IfcOpenshellInstance;
    addBoolean(file: IfcOpenshellFile, first_item: IfcOpenshellInstance, second_items: IfcOpenshellInstance[], operator_type: string): IfcOpenshellParseInstanceList;
    addDoorRepresentation(file: IfcOpenshellFile, options: IfcOpenshellGeometryAddDoorRepresentationOptions): IfcOpenshellInstance;
    addFootprintRepresentation(file: IfcOpenshellFile, context: IfcOpenshellInstance, curves: IfcOpenshellInstance[]): IfcOpenshellInstance;
    addMeshRepresentation(file: IfcOpenshellFile, context: IfcOpenshellInstance, options: IfcOpenshellGeometryAddMeshRepresentationOptions): IfcOpenshellInstance;
    addRailingRepresentation(file: IfcOpenshellFile, options: IfcOpenshellGeometryAddRailingRepresentationOptions): IfcOpenshellInstance;
    addShapeAspect(file: IfcOpenshellFile, options: IfcOpenshellGeometryAddShapeAspectOptions): IfcOpenshellInstance;
    addSlabRepresentation(file: IfcOpenshellFile, options: IfcOpenshellGeometryAddSlabRepresentationOptions): IfcOpenshellInstance;
    addTopologyRepresentation(file: IfcOpenshellFile, options: IfcOpenshellGeometryAddTopologyRepresentationOptions): IfcOpenshellInstance;
    addWallRepresentation(file: IfcOpenshellFile, options: IfcOpenshellGeometryAddWallRepresentationOptions): IfcOpenshellInstance;
    addWindowRepresentation(file: IfcOpenshellFile, options: IfcOpenshellGeometryAddWindowRepresentationOptions): IfcOpenshellInstance;
    assignRepresentation(file: IfcOpenshellFile, product: IfcOpenshellInstance, representation: IfcOpenshellInstance): IfcOpenshellInstance;
    clipSolid(file: IfcOpenshellFile, options: IfcOpenshellGeometryClipSolidOptions): IfcOpenshellInstance;
    clipSolidBounded(file: IfcOpenshellFile, options: IfcOpenshellGeometryClipSolidBoundedOptions): IfcOpenshellInstance;
    connectElement(file: IfcOpenshellFile, options: IfcOpenshellGeometryConnectElementOptions): IfcOpenshellInstance;
    connectPath(file: IfcOpenshellFile, options: IfcOpenshellGeometryConnectPathOptions): IfcOpenshellInstance;
    connectWall(file: IfcOpenshellFile, options: IfcOpenshellGeometryConnectWallOptions): IfcOpenshellInstance;
    copyRepresentation(file: IfcOpenshellFile, options: IfcOpenshellGeometryCopyRepresentationOptions): IfcOpenshellInstance;
    create2ptWall(file: IfcOpenshellFile, options: IfcOpenshellGeometryCreate2PtWallOptions): IfcOpenshellInstance;
    disconnectElement(file: IfcOpenshellFile, relating_element: IfcOpenshellInstance, related_element: IfcOpenshellInstance): void;
    disconnectPath(file: IfcOpenshellFile, options: IfcOpenshellGeometryDisconnectPathOptions): void;
    editObjectPlacement(file: IfcOpenshellFile, options: IfcOpenshellGeometryEditObjectPlacementOptions): IfcOpenshellInstance;
    mapRepresentation(file: IfcOpenshellFile, representation: IfcOpenshellInstance): IfcOpenshellInstance;
    profileExtents(file: IfcOpenshellFile, profile: IfcOpenshellInstance): number[];
    regenerateWallRepresentation(file: IfcOpenshellFile, options: IfcOpenshellGeometryRegenerateWallRepresentationOptions): IfcOpenshellInstance;
    removeBoolean(file: IfcOpenshellFile, item: IfcOpenshellInstance): void;
    removeRepresentation(file: IfcOpenshellFile, representation: IfcOpenshellInstance, options: IfcOpenshellGeometryRemoveRepresentationOptions): void;
    unassignRepresentation(file: IfcOpenshellFile, product: IfcOpenshellInstance, representation: IfcOpenshellInstance): void;
    validateType(file: IfcOpenshellFile, representation: IfcOpenshellInstance, options: IfcOpenshellGeometryValidateTypeOptions): boolean;
  }

  export interface IfcOpenshellGeoreferenceModule {
    addGeoreferencing(file: IfcOpenshellFile, options: IfcOpenshellGeoreferenceAddGeoreferencingOptions): void;
    editGeoreferencing(file: IfcOpenshellFile, options: IfcOpenshellGeoreferenceEditGeoreferencingOptions): void;
    editTrueNorth(file: IfcOpenshellFile, options: IfcOpenshellGeoreferenceEditTrueNorthOptions): void;
    editWcs(file: IfcOpenshellFile, options: IfcOpenshellGeoreferenceEditWcsOptions): void;
    removeGeoreferencing(file: IfcOpenshellFile): void;
  }

  export interface IfcOpenshellGridModule {
    createAxisCurve(file: IfcOpenshellFile, p1: number[], p2: number[], grid_axis: IfcOpenshellInstance, is_si: boolean): void;
    createGridAxis(file: IfcOpenshellFile, grid: IfcOpenshellInstance, axis_tag: string, same_sense: boolean, uvw_axes: string): IfcOpenshellInstance;
    removeGridAxis(file: IfcOpenshellFile, axis: IfcOpenshellInstance): void;
  }

  export interface IfcOpenshellGroupModule {
    /** Create a new IfcGroup with generated identity and ownership metadata. */
    addGroup(file: IfcOpenshellFile, options: IfcOpenshellGroupAddGroupOptions): IfcOpenshellInstance;
    /** Assign products to a group, merging into an existing relationship when present. */
    assignGroup(file: IfcOpenshellFile, options: IfcOpenshellGroupAssignGroupOptions): IfcOpenshellInstance;
    /** Remove a group and its relationships. */
    removeGroup(file: IfcOpenshellFile, group: IfcOpenshellInstance): void;
    /** Remove products from a group relationship. */
    unassignGroup(file: IfcOpenshellFile, options: IfcOpenshellGroupUnassignGroupOptions): void;
    /** Replace the product list of an existing group relationship. */
    updateGroupProducts(file: IfcOpenshellFile, options: IfcOpenshellGroupUpdateGroupProductsOptions): IfcOpenshellInstance;
  }

  export interface IfcOpenshellGuidModule {
    compress(uuid_hex: string): string;
    expand(guid: string): string;
    new(): string;
  }

  export interface IfcOpenshellLayerModule {
    addLayer(file: IfcOpenshellFile, name: string): IfcOpenshellInstance;
    addLayerWithStyle(file: IfcOpenshellFile, name: string, options: IfcOpenshellLayerAddLayerWithStyleOptions): IfcOpenshellInstance;
    assignLayer(file: IfcOpenshellFile, items: IfcOpenshellInstance[], layer: IfcOpenshellInstance): void;
    removeLayer(file: IfcOpenshellFile, layer: IfcOpenshellInstance): void;
    unassignLayer(file: IfcOpenshellFile, items: IfcOpenshellInstance[], layer: IfcOpenshellInstance): void;
  }

  export interface IfcOpenshellLibraryModule {
    addLibrary(file: IfcOpenshellFile, name: string): IfcOpenshellInstance;
    addReference(file: IfcOpenshellFile, library: IfcOpenshellInstance): IfcOpenshellInstance;
    /** Assign a library reference to a list of products. */
    assignReference(file: IfcOpenshellFile, options: IfcOpenshellLibraryAssignReferenceOptions): IfcOpenshellInstance;
    removeLibrary(file: IfcOpenshellFile, library: IfcOpenshellInstance): void;
    removeReference(file: IfcOpenshellFile, reference: IfcOpenshellInstance): void;
    /** Unassign a library reference from a list of products. */
    unassignReference(file: IfcOpenshellFile, options: IfcOpenshellLibraryUnassignReferenceOptions): void;
  }

  export interface IfcOpenshellMaterialModule {
    addConstituent(file: IfcOpenshellFile, constituent_set: IfcOpenshellInstance, options: IfcOpenshellMaterialAddConstituentOptions): IfcOpenshellInstance;
    addLayer(file: IfcOpenshellFile, layer_set: IfcOpenshellInstance, options: IfcOpenshellMaterialAddLayerOptions): IfcOpenshellInstance;
    addListItem(file: IfcOpenshellFile, material_list: IfcOpenshellInstance, material: IfcOpenshellInstance): void;
    addMaterial(file: IfcOpenshellFile, options: IfcOpenshellMaterialAddMaterialOptions): IfcOpenshellInstance;
    addMaterialSet(file: IfcOpenshellFile, options: IfcOpenshellMaterialAddMaterialSetOptions): IfcOpenshellInstance;
    addProfile(file: IfcOpenshellFile, profile_set: IfcOpenshellInstance, options: IfcOpenshellMaterialAddProfileOptions): IfcOpenshellInstance;
    assignMaterial(file: IfcOpenshellFile, products: IfcOpenshellInstance[], options: IfcOpenshellMaterialAssignMaterialOptions): IfcOpenshellParseInstanceList;
    assignProfile(file: IfcOpenshellFile, material_profile: IfcOpenshellInstance, profile: IfcOpenshellInstance): void;
    editProfileUsage(file: IfcOpenshellFile, usage: IfcOpenshellInstance, options: IfcOpenshellMaterialEditProfileUsageOptions): void;
    removeConstituent(file: IfcOpenshellFile, constituent: IfcOpenshellInstance, options: IfcOpenshellMaterialRemoveItemOptions): void;
    removeLayer(file: IfcOpenshellFile, layer: IfcOpenshellInstance, options: IfcOpenshellMaterialRemoveItemOptions): void;
    removeListItem(file: IfcOpenshellFile, material_list: IfcOpenshellInstance, options: IfcOpenshellMaterialRemoveListItemOptions): void;
    removeMaterial(file: IfcOpenshellFile, material: IfcOpenshellInstance): void;
    removeMaterialSet(file: IfcOpenshellFile, material: IfcOpenshellInstance): void;
    removeProfile(file: IfcOpenshellFile, profile: IfcOpenshellInstance, options: IfcOpenshellMaterialRemoveProfileOptions): void;
    reorderSetItem(file: IfcOpenshellFile, material_set: IfcOpenshellInstance, options: IfcOpenshellMaterialReorderSetItemOptions): void;
    unassignMaterial(file: IfcOpenshellFile, products: IfcOpenshellInstance[], options: IfcOpenshellMaterialUnassignMaterialOptions): void;
  }

  export interface IfcOpenshellNestModule {
    /** Assign objects as nested children of a parent host. */
    assignObject(file: IfcOpenshellFile, options: IfcOpenshellNestAssignObjectOptions): IfcOpenshellInstance;
    /** Remove objects from their nesting relationships. */
    unassignObject(file: IfcOpenshellFile, options: IfcOpenshellNestUnassignObjectOptions): void;
  }

  export interface IfcOpenshellOwnerModule {
    addActor(file: IfcOpenshellFile, options: IfcOpenshellOwnerAddActorOptions): IfcOpenshellInstance;
    addAddress(file: IfcOpenshellFile, assigned_object: IfcOpenshellInstance, ifc_class: string): IfcOpenshellInstance;
    addApplication(file: IfcOpenshellFile, options: IfcOpenshellOwnerAddApplicationOptions): IfcOpenshellInstance;
    addOrganisation(file: IfcOpenshellFile, identification: string, name: string): IfcOpenshellInstance;
    addPerson(file: IfcOpenshellFile, identification: string, family_name: string, given_name: string): IfcOpenshellInstance;
    addPersonAndOrganisation(file: IfcOpenshellFile, person: IfcOpenshellInstance, organisation: IfcOpenshellInstance): IfcOpenshellInstance;
    addRole(file: IfcOpenshellFile, assigned_object: IfcOpenshellInstance, role: string): IfcOpenshellInstance;
    assignActor(file: IfcOpenshellFile, options: IfcOpenshellOwnerAssignActorOptions): IfcOpenshellInstance;
    createOwnerHistory(file: IfcOpenshellFile, options: IfcOpenshellOwnerCreateOwnerHistoryOptions): IfcOpenshellInstance;
    removeActor(file: IfcOpenshellFile, actor: IfcOpenshellInstance): void;
    removeAddress(file: IfcOpenshellFile, address: IfcOpenshellInstance): void;
    removeApplication(file: IfcOpenshellFile, application: IfcOpenshellInstance): void;
    removeOrganisation(file: IfcOpenshellFile, organisation: IfcOpenshellInstance): void;
    removePerson(file: IfcOpenshellFile, person: IfcOpenshellInstance): void;
    removePersonAndOrganisation(file: IfcOpenshellFile, person_and_organisation: IfcOpenshellInstance): void;
    removeRole(file: IfcOpenshellFile, role: IfcOpenshellInstance): void;
    unassignActor(file: IfcOpenshellFile, options: IfcOpenshellOwnerUnassignActorOptions): void;
    updateOwnerHistory(file: IfcOpenshellFile, options: IfcOpenshellOwnerUpdateOwnerHistoryOptions): IfcOpenshellInstance;
  }

  export interface IfcOpenshellParseModule {
    argumentTypeToString(type: number): string;
    clearSchemas(): void;
    escapeXml(text: string): void;
    fromParameterType(parameter_type: IfcOpenshellParameterType): number;
    generalTokenPtr(start: number, token: string): number;
    getFeature(name: string): boolean;
    getInfoCpp(instance: IfcOpenshellInstance, include_identifier: boolean): string;
    getLog(): string;
    getPluginSearchPaths(): string[];
    getSiEquivalent(named_unit: IfcOpenshellInstance): number;
    guessFileType(path: string): number;
    instanceListCreateFromHandles(instances: IfcOpenshellInstance[]): IfcOpenshellParseInstanceList;
    makeAggregate(element_type: number): number;
    newFile(schema_identifier: string, file_type: number, path: string): IfcOpenshellFile | null;
    open(path: string, readonly: boolean): IfcOpenshellFile | null;
    openBypass(path: string, type_names: string[]): IfcOpenshellFile | null;
    operatorTokenPtr(start: number, data: string): number;
    readMemory(data: number, length: number): IfcOpenshellFile | null;
    registerSchema(schema: IfcOpenshellSchema): void;
    sanitateMaterialName(material_name: string): void;
    schemaByName(schema_name: string): IfcOpenshellSchema | null;
    schemaNames(): string[];
    schemaPluginRegistrationSymbol(): string;
    setFeature(name: string, value: boolean): void;
    setLogFormatJson(): void;
    setLogFormatText(): void;
    siPrefixToValue(prefix: string): number;
    stream(): IfcOpenshellInstanceStreamer | null;
    streamFromPath(path: string, mmap: boolean): IfcOpenshellInstanceStreamer | null;
    streamFromString(data: string): IfcOpenshellInstanceStreamer | null;
    traverse(instance: IfcOpenshellInstance, max_depth: number): IfcOpenshellParseInstanceList;
    traverseBreadthFirst(instance: IfcOpenshellInstance, max_depth: number): IfcOpenshellParseInstanceList;
    turnOffDetailedLogging(): void;
    turnOnDetailedLogging(): void;
    unescapeXml(text: string): void;
    validBinaryString(binary_string: string): boolean;
    version(): string;
    openBytes(bytes: Uint8Array | ArrayBuffer | ArrayBufferView, filename?: string, readonly?: boolean): IfcOpenshellFile | null;
  }

  export interface IfcOpenshellPlacementModule {
    a2p(origin: number[], z_axis: number[], x_axis: number[]): number[];
    getAxis2placement(instance: IfcOpenshellInstance): number[];
    getCartesianXform3d(instance: IfcOpenshellInstance): number[];
    getLocalPlacement(instance: IfcOpenshellInstance | null): number[];
    getMappeditemXform(instance: IfcOpenshellInstance): number[];
    getStoreyElevation(instance: IfcOpenshellInstance): number;
    rotation(angle_rad: number, axis: string): number[];
  }

  export interface IfcOpenshellProfileModule {
    addArbitraryProfile(file: IfcOpenshellFile, options: IfcOpenshellProfileAddArbitraryProfileOptions): IfcOpenshellInstance;
    addArbitraryProfileWithVoids(file: IfcOpenshellFile, options: IfcOpenshellProfileAddArbitraryProfileWithVoidsOptions): IfcOpenshellInstance;
    addParameterizedProfile(file: IfcOpenshellFile, ifc_class: string, profile_type: string): IfcOpenshellInstance;
    copyProfile(file: IfcOpenshellFile, profile: IfcOpenshellInstance): IfcOpenshellInstance;
    editProfile(profile: IfcOpenshellInstance, attributes: number): void;
    removeProfile(file: IfcOpenshellFile, profile: IfcOpenshellInstance): void;
  }

  export interface IfcOpenshellProjectModule {
    /**
     * Declare objects to a project or project library context.
     *
     * Creates or updates an IfcRelDeclares relationship linking the given
     * definitions to the relating context. If the definitions are already
     * declared to a different context, they are moved.
     */
    assignDeclaration(file: IfcOpenshellFile, options: IfcOpenshellProjectAssignDeclarationOptions): IfcOpenshellInstance;
    /**
     * Remove declarations of objects from a project or project library context.
     *
     * Removes the definitions from any existing IfcRelDeclares relationship.
     * If no definitions remain, the relationship itself is removed.
     */
    unassignDeclaration(file: IfcOpenshellFile, options: IfcOpenshellProjectUnassignDeclarationOptions): void;
  }

  export interface IfcOpenshellPsetModule {
    /**
     * Add or return an existing property set attached to an object, context, type,
     * material, or profile definition.
     */
    addPset(file: IfcOpenshellFile, options: IfcOpenshellPsetAddPsetOptions): IfcOpenshellInstance;
    addQto(file: IfcOpenshellFile, options: IfcOpenshellPsetAddQtoOptions): IfcOpenshellInstance;
    assignPset(file: IfcOpenshellFile, options: IfcOpenshellPsetAssignPsetOptions): IfcOpenshellInstance;
    editPset(file: IfcOpenshellFile, options: IfcOpenshellPsetEditPsetOptions): boolean;
    editQto(file: IfcOpenshellFile, options: IfcOpenshellPsetEditQtoOptions): boolean;
    propsFree(props: number): void;
    propsNew(): number | null;
    propsSetBool(props: number, key: string, value: boolean): void;
    propsSetDate(props: number, key: string, year: number, month: number, day: number): void;
    propsSetDatetime(props: number, key: string, year: number, month: number, day: number, hour: number, minute: number, second: number, microsecond: number, has_timezone: boolean, timezone_offset_minutes: number): void;
    propsSetDict(outer: number, key: string, inner: number): void;
    propsSetDouble(props: number, key: string, value: number): void;
    propsSetDoubleList(props: number, key: string, values: number[]): void;
    propsSetDuration(props: number, key: string, negative: boolean, years: number, months: number, days: number, hours: number, minutes: number, seconds: number, microseconds: number): void;
    propsSetInstance(props: number, key: string, value: IfcOpenshellInstance | null): void;
    propsSetInstanceList(props: number, key: string, values: IfcOpenshellInstance[]): void;
    propsSetInt(props: number, key: string, value: bigint): void;
    propsSetIntList(props: number, key: string, values: bigint[]): void;
    propsSetNull(props: number, key: string): void;
    propsSetString(props: number, key: string, value: string): void;
    propsSetStringList(props: number, key: string, values: string[]): void;
    propsSetTypedBool(props: number, key: string, value: boolean, ifc_type: string): void;
    propsSetTypedDouble(props: number, key: string, value: number, ifc_type: string): void;
    propsSetTypedInt(props: number, key: string, value: bigint, ifc_type: string): void;
    propsSetTypedString(props: number, key: string, value: string, ifc_type: string): void;
    propsSetUnitForLast(props: number, unit: IfcOpenshellInstance | null): void;
    removePset(file: IfcOpenshellFile, product: IfcOpenshellInstance, pset: IfcOpenshellInstance): void;
    templateAddPropTemplate(file: IfcOpenshellFile, pset_template: IfcOpenshellInstance, name: string, description: string | null, template_type: string | null, primary_measure_type: string | null): IfcOpenshellInstance;
    templateAddPsetTemplate(file: IfcOpenshellFile, name: string, template_type: string, applicable_entity: string): IfcOpenshellInstance;
    templateCreateFromFiles(schema_identifier: string, template_files: IfcOpenshellFile[]): IfcOpenshellPsetTemplateHandle | null;
    templateGetApplicable(pqt: IfcOpenshellPsetTemplateHandle, ifc_class: string | null, predefined_type: string | null, pset_only: boolean, qto_only: boolean, schema_name: string | null): IfcOpenshellParseInstanceList;
    templateGetApplicableNames(pqt: IfcOpenshellPsetTemplateHandle, ifc_class: string | null, predefined_type: string | null, pset_only: boolean, qto_only: boolean, schema_name: string | null): string[];
    templateGetByName(pqt: IfcOpenshellPsetTemplateHandle, name: string): IfcOpenshellInstance;
    templateGetTemplate(schema_identifier: string): IfcOpenshellPsetTemplateHandle | null;
    templateIsTemplated(pqt: IfcOpenshellPsetTemplateHandle, name: string): boolean;
    templatePsetType(pset_template: IfcOpenshellInstance): string;
    templateRemovePropTemplate(file: IfcOpenshellFile, prop_template: IfcOpenshellInstance): void;
    templateRemovePsetTemplate(pset_template: IfcOpenshellInstance): void;
    templateSetTemplateDir(dir: string): void;
    unassignPset(file: IfcOpenshellFile, products: IfcOpenshellInstance[], pset: IfcOpenshellInstance): void;
    unsharePset(file: IfcOpenshellFile, options: IfcOpenshellPsetUnsharePsetOptions): IfcOpenshellParseInstanceList;
  }

  export interface IfcOpenshellRegisterModule {
    scratchFile(schema_name: string | null, file: IfcOpenshellFile): boolean;
  }

  export interface IfcOpenshellRepresentationModule {
    getContext(file: IfcOpenshellFile, context_type: string | null, subcontext: string | null, target_view: string | null): IfcOpenshellInstance;
    getPrioritisedContexts(file: IfcOpenshellFile): IfcOpenshellParseInstanceList;
    getProductRepresentation(element: IfcOpenshellInstance, options: IfcOpenshellRepresentationGetProductRepresentationOptions): IfcOpenshellInstance;
    resolve(representation: IfcOpenshellInstance): IfcOpenshellInstance;
    resolveBaseItems(representation: IfcOpenshellInstance): IfcOpenshellParseInstanceList;
  }

  export interface IfcOpenshellResourceModule {
    editResourceTime(file: IfcOpenshellFile, resource_time: IfcOpenshellInstance, attributes: number): void;
  }

  export interface IfcOpenshellRootModule {
    /**
     * Create an IFC entity with generated identity, ownership metadata, optional
     * name, and optional predefined type.
     */
    createEntity(file: IfcOpenshellFile, options: IfcOpenshellRootCreateEntityOptions): IfcOpenshellInstance;
    removeProduct(file: IfcOpenshellFile, product: IfcOpenshellInstance, options: IfcOpenshellRootRemoveProductOptions): void;
  }

  export interface IfcOpenshellSchemaModule {
    reassignClass(file: IfcOpenshellFile | null, element: IfcOpenshellInstance, new_class: string): IfcOpenshellInstance;
  }

  export interface IfcOpenshellSelectorModule {
    filterAll(file: IfcOpenshellFile, query: string): IfcOpenshellValue | null;
    filterElements(file: IfcOpenshellFile, query: string, elements: IfcOpenshellInstance[]): IfcOpenshellValue | null;
    format(file: IfcOpenshellFile | null, instance: IfcOpenshellInstance | null, query: string): string | null;
    getElementValue(file: IfcOpenshellFile | null, element: IfcOpenshellInstance, query: string): IfcOpenshellValue | null;
    keysCount(keys: number | null): number;
    keysFree(keys: number | null): void;
    keysGet(keys: number | null, index: number): string;
    keysIsRegex(keys: number | null, index: number): boolean;
    nodeChild(node: number | null, index: number): number | null;
    nodeChildCount(node: number | null): number;
    nodeFree(root: number | null): void;
    nodeKind(node: number | null): number;
    nodeText(node: number | null): string;
    parseFilter(query: string): number | null;
    parseFormat(query: string): number | null;
    parseGetElement(query: string): number | null;
    parseKeys(query: string): number | null;
    setElementValue(file: IfcOpenshellFile, element: IfcOpenshellInstance | null, query: string, value: IfcOpenshellValue | null, concat: string | null): void;
  }

  export interface IfcOpenshellSequenceModule {
    addDateTime(file: IfcOpenshellFile, date_time: string): IfcOpenshellInstance | string;
    addTask(file: IfcOpenshellFile, options: IfcOpenshellSequenceAddTaskOptions): IfcOpenshellInstance;
    addTaskTime(file: IfcOpenshellFile, task: IfcOpenshellInstance, options: IfcOpenshellSequenceAddTaskTimeOptions): IfcOpenshellInstance;
    addTimePeriod(file: IfcOpenshellFile, recurrence_pattern: IfcOpenshellInstance, options: IfcOpenshellSequenceAddTimePeriodOptions): IfcOpenshellInstance;
    addWorkCalendar(file: IfcOpenshellFile, options: IfcOpenshellSequenceAddWorkCalendarOptions): IfcOpenshellInstance;
    addWorkPlan(file: IfcOpenshellFile, options: IfcOpenshellSequenceAddWorkPlanOptions): IfcOpenshellInstance;
    addWorkSchedule(file: IfcOpenshellFile, options: IfcOpenshellSequenceAddWorkScheduleOptions): IfcOpenshellInstance;
    addWorkTime(file: IfcOpenshellFile, work_calendar: IfcOpenshellInstance, time_type: string): IfcOpenshellInstance;
    assignLagTime(file: IfcOpenshellFile, rel_sequence: IfcOpenshellInstance, lag_value: string, options: IfcOpenshellSequenceAssignLagTimeOptions): IfcOpenshellInstance;
    assignProcess(file: IfcOpenshellFile, relating_process: IfcOpenshellInstance, related_object: IfcOpenshellInstance, options: IfcOpenshellSequenceAssignProcessOptions): IfcOpenshellInstance;
    assignProduct(file: IfcOpenshellFile, relating_product: IfcOpenshellInstance, related_object: IfcOpenshellInstance, options: IfcOpenshellSequenceAssignProductOptions): IfcOpenshellInstance;
    assignRecurrencePattern(file: IfcOpenshellFile, parent: IfcOpenshellInstance, recurrence_type: string): IfcOpenshellInstance;
    assignSequence(file: IfcOpenshellFile, relating_process: IfcOpenshellInstance, related_process: IfcOpenshellInstance, options: IfcOpenshellSequenceAssignSequenceOptions): IfcOpenshellInstance;
    assignWorkPlan(file: IfcOpenshellFile, work_schedule: IfcOpenshellInstance, work_plan: IfcOpenshellInstance, options: IfcOpenshellSequenceAssignWorkPlanOptions): IfcOpenshellInstance;
    calculateTaskDuration(file: IfcOpenshellFile, task: IfcOpenshellInstance): void;
    cascadeSchedule(file: IfcOpenshellFile, task: IfcOpenshellInstance): void;
    copyWorkSchedule(file: IfcOpenshellFile, work_schedule: IfcOpenshellInstance, options: IfcOpenshellSequenceCopyWorkScheduleOptions): IfcOpenshellInstance;
    createBaseline(file: IfcOpenshellFile, work_schedule: IfcOpenshellInstance, options: IfcOpenshellSequenceCreateBaselineOptions): void;
    duplicateTask(file: IfcOpenshellFile, task: IfcOpenshellInstance, options: IfcOpenshellSequenceDuplicateTaskOptions): IfcOpenshellSequenceDuplicateTaskResult;
    editLagTime(lag_time: IfcOpenshellInstance, attributes: number): void;
    editRecurrencePattern(recurrence_pattern: IfcOpenshellInstance, attributes: number): void;
    editSequence(rel_sequence: IfcOpenshellInstance, attributes: number): void;
    editTask(task: IfcOpenshellInstance, attributes: number): void;
    editTaskTime(file: IfcOpenshellFile, task_time: IfcOpenshellInstance, attributes: number): void;
    editWorkCalendar(work_calendar: IfcOpenshellInstance, attributes: number): void;
    editWorkPlan(work_plan: IfcOpenshellInstance, attributes: number): void;
    editWorkSchedule(work_schedule: IfcOpenshellInstance, attributes: number): void;
    editWorkTime(work_time: IfcOpenshellInstance, attributes: number): void;
    recalculateSchedule(file: IfcOpenshellFile, work_schedule: IfcOpenshellInstance): void;
    removeTask(file: IfcOpenshellFile, task: IfcOpenshellInstance, options: IfcOpenshellSequenceRemoveOptions): void;
    removeTimePeriod(file: IfcOpenshellFile, time_period: IfcOpenshellInstance): void;
    removeWorkCalendar(file: IfcOpenshellFile, work_calendar: IfcOpenshellInstance, options: IfcOpenshellSequenceRemoveOptions): void;
    removeWorkPlan(file: IfcOpenshellFile, work_plan: IfcOpenshellInstance, options: IfcOpenshellSequenceRemoveOptions): void;
    removeWorkSchedule(file: IfcOpenshellFile, work_schedule: IfcOpenshellInstance, options: IfcOpenshellSequenceRemoveOptions): void;
    removeWorkTime(file: IfcOpenshellFile, work_time: IfcOpenshellInstance): void;
    unassignLagTime(file: IfcOpenshellFile, rel_sequence: IfcOpenshellInstance): void;
    unassignProcess(file: IfcOpenshellFile, relating_process: IfcOpenshellInstance, related_object: IfcOpenshellInstance, options: IfcOpenshellSequenceRemoveOptions): IfcOpenshellInstance;
    unassignProduct(file: IfcOpenshellFile, relating_product: IfcOpenshellInstance, related_object: IfcOpenshellInstance, options: IfcOpenshellSequenceRemoveOptions): IfcOpenshellInstance;
    unassignRecurrencePattern(file: IfcOpenshellFile, recurrence_pattern: IfcOpenshellInstance): void;
    unassignSequence(file: IfcOpenshellFile, relating_process: IfcOpenshellInstance, related_process: IfcOpenshellInstance): void;
  }

  export interface IfcOpenshellShapeModule {
    builderAxis2Placement2d(file: IfcOpenshellFile, options: IfcOpenshellShapeBuilderAxis2Placement2dOptions): IfcOpenshellInstance;
    builderAxis2Placement3d(file: IfcOpenshellFile, options: IfcOpenshellShapeBuilderAxis2Placement3dOptions): IfcOpenshellInstance;
    builderBlock(file: IfcOpenshellFile, options: IfcOpenshellShapeBuilderBlockOptions): IfcOpenshellInstance;
    builderCircle(file: IfcOpenshellFile, center: number[], radius: number): IfcOpenshellInstance;
    builderCurveBetweenTwoPoints(file: IfcOpenshellFile, points: number[][]): IfcOpenshellInstance;
    builderDeepCopy(file: IfcOpenshellFile, element: IfcOpenshellInstance): IfcOpenshellInstance;
    builderEdge(file: IfcOpenshellFile, start: number[], end: number[]): IfcOpenshellInstance;
    builderEllipseCurve(file: IfcOpenshellFile, options: IfcOpenshellShapeBuilderEllipseCurveOptions): IfcOpenshellInstance;
    builderExtrude(file: IfcOpenshellFile, options: IfcOpenshellShapeBuilderExtrudeOptions): IfcOpenshellInstance;
    builderFace(file: IfcOpenshellFile, points: number[][]): IfcOpenshellInstance;
    builderFacetedBrep(file: IfcOpenshellFile, points: number[][], faces: number[][]): IfcOpenshellInstance;
    builderGetPolylineCoords(polyline: IfcOpenshellInstance): number[][];
    builderHalfSpaceSolid(file: IfcOpenshellFile, options: IfcOpenshellShapeBuilderHalfSpaceSolidOptions): IfcOpenshellInstance;
    builderIndexedPolycurve2d(file: IfcOpenshellFile, points: number[][], segments: number[][]): IfcOpenshellInstance;
    builderMepBendShape(file: IfcOpenshellFile, options: IfcOpenshellShapeBuilderMepBendShapeOptions): IfcOpenshellShapeBuilderMepBendShapeResult;
    builderMepTransitionCalculate(options: IfcOpenshellShapeBuilderMepTransitionCalculateOptions): number;
    builderMepTransitionLength(options: IfcOpenshellShapeBuilderMepTransitionLengthOptions): number;
    builderMepTransitionShape(file: IfcOpenshellFile, options: IfcOpenshellShapeBuilderMepTransitionShapeOptions): IfcOpenshellShapeBuilderMepTransitionShapeResult | null;
    builderMesh(file: IfcOpenshellFile, points: number[][], faces: number[][]): IfcOpenshellInstance;
    builderMirror(file: IfcOpenshellFile, options: IfcOpenshellShapeBuilderMirrorOptions): IfcOpenshellInstance;
    builderPlane(file: IfcOpenshellFile, location: number[], normal: number[]): IfcOpenshellInstance;
    builderPolygonalFaceSet(file: IfcOpenshellFile, points: number[][], faces: number[][][]): IfcOpenshellInstance;
    builderPolyline(file: IfcOpenshellFile, options: IfcOpenshellShapeBuilderPolylineOptions): IfcOpenshellInstance;
    builderProfile(file: IfcOpenshellFile, options: IfcOpenshellShapeBuilderProfileOptions): IfcOpenshellInstance;
    builderRepresentation(file: IfcOpenshellFile, options: IfcOpenshellShapeBuilderRepresentationOptions): IfcOpenshellInstance;
    builderRotate(file: IfcOpenshellFile, options: IfcOpenshellShapeBuilderRotateOptions): IfcOpenshellInstance;
    builderSetPolylineCoords(file: IfcOpenshellFile, polyline: IfcOpenshellInstance, coords: number[][]): IfcOpenshellInstance;
    builderSphere(file: IfcOpenshellFile, options: IfcOpenshellShapeBuilderSphereOptions): IfcOpenshellInstance;
    builderSweptDiskSolid(file: IfcOpenshellFile, path_curve: IfcOpenshellInstance, radius: number): IfcOpenshellInstance;
    builderTranslate(file: IfcOpenshellFile, options: IfcOpenshellShapeBuilderTranslateOptions): IfcOpenshellInstance;
    builderTriangulatedFaceSet(file: IfcOpenshellFile, points: number[][], faces: number[][]): IfcOpenshellInstance;
    builderVertex(file: IfcOpenshellFile, position: number[]): IfcOpenshellInstance;
    isX(value: number, x: number, tolerance: number): boolean;
  }

  export interface IfcOpenshellSpatialModule {
    /** Assign products to be contained hierarchically in a spatial structure. */
    assignContainer(file: IfcOpenshellFile, options: IfcOpenshellSpatialAssignContainerOptions): IfcOpenshellInstance;
    /** Dereference products from a spatial structure. */
    dereferenceStructure(file: IfcOpenshellFile, options: IfcOpenshellSpatialDereferenceStructureOptions): void;
    /** Reference products in a spatial structure without containment. */
    referenceStructure(file: IfcOpenshellFile, options: IfcOpenshellSpatialReferenceStructureOptions): IfcOpenshellInstance;
    /** Remove products from their spatial containment relationships. */
    unassignContainer(file: IfcOpenshellFile, options: IfcOpenshellSpatialUnassignContainerOptions): void;
  }

  export interface IfcOpenshellStructuralModule {
    addStructuralActivity(file: IfcOpenshellFile, applied_load: IfcOpenshellInstance, structural_member: IfcOpenshellInstance, ifc_class: string, predefined_type: string, global_or_local: string, options: IfcOpenshellStructuralAddStructuralActivityOptions): IfcOpenshellInstance;
    addStructuralAnalysisModel(file: IfcOpenshellFile, owner_history: IfcOpenshellInstance | null): IfcOpenshellInstance;
    addStructuralBoundaryCondition(file: IfcOpenshellFile, ifc_class: string, options: IfcOpenshellStructuralAddStructuralBoundaryConditionOptions): IfcOpenshellInstance;
    addStructuralLoad(file: IfcOpenshellFile, ifc_class: string, name: string | null): IfcOpenshellInstance;
    addStructuralLoadCase(file: IfcOpenshellFile, name: string, action_type: string, action_source: string, owner_history: IfcOpenshellInstance | null): IfcOpenshellInstance;
    addStructuralLoadGroup(file: IfcOpenshellFile, name: string, action_type: string, action_source: string, owner_history: IfcOpenshellInstance | null): IfcOpenshellInstance;
    addStructuralMemberConnection(file: IfcOpenshellFile, relating_structural_member: IfcOpenshellInstance, related_structural_connection: IfcOpenshellInstance, owner_history: IfcOpenshellInstance | null): IfcOpenshellInstance;
    assignProduct(file: IfcOpenshellFile, relating_product: IfcOpenshellInstance, related_object: IfcOpenshellInstance, owner_history: IfcOpenshellInstance | null): IfcOpenshellInstance;
    assignStructuralAnalysisModel(file: IfcOpenshellFile, products: IfcOpenshellInstance[], structural_analysis_model: IfcOpenshellInstance, options: IfcOpenshellStructuralAssignStructuralAnalysisModelOptions): IfcOpenshellInstance;
    assignToBuilding(file: IfcOpenshellFile, structural_analysis_model: IfcOpenshellInstance, building: IfcOpenshellInstance, owner_history: IfcOpenshellInstance | null): IfcOpenshellInstance;
    editStructuralBoundaryCondition(file: IfcOpenshellFile, condition: IfcOpenshellInstance, attributes: number): void;
    editStructuralConnectionCs(file: IfcOpenshellFile, structural_item: IfcOpenshellInstance, axis: number[], ref_direction: number[]): void;
    editStructuralItemAxis(file: IfcOpenshellFile, structural_item: IfcOpenshellInstance, axis: number[]): void;
    removeStructuralAnalysisModel(file: IfcOpenshellFile, structural_analysis_model: IfcOpenshellInstance): void;
    removeStructuralBoundaryCondition(file: IfcOpenshellFile, options: IfcOpenshellStructuralRemoveStructuralBoundaryConditionOptions): void;
    removeStructuralConnectionCondition(file: IfcOpenshellFile, relation: IfcOpenshellInstance): void;
    removeStructuralLoad(file: IfcOpenshellFile, structural_load: IfcOpenshellInstance): void;
    removeStructuralLoadCase(file: IfcOpenshellFile, structural_load_case: IfcOpenshellInstance): void;
    removeStructuralLoadGroup(file: IfcOpenshellFile, structural_load_group: IfcOpenshellInstance): void;
    unassignStructuralAnalysisModel(file: IfcOpenshellFile, products: IfcOpenshellInstance[], structural_analysis_model: IfcOpenshellInstance, options: IfcOpenshellStructuralUnassignStructuralAnalysisModelOptions): void;
  }

  export interface IfcOpenshellStyleModule {
    addStyle(file: IfcOpenshellFile, name: string | null, ifc_class: string): IfcOpenshellInstance;
    assignItemStyle(file: IfcOpenshellFile, options: IfcOpenshellStyleAssignItemStyleOptions): IfcOpenshellInstance;
    assignMaterialStyle(file: IfcOpenshellFile, material: IfcOpenshellInstance, style: IfcOpenshellInstance, context: IfcOpenshellInstance, should_use_presentation_style_assignment: boolean): void;
    assignRepresentationStyles(file: IfcOpenshellFile, shape_representation: IfcOpenshellInstance, styles: IfcOpenshellInstance[], should_use_presentation_style_assignment: boolean, replace_previous_same_type_style: boolean): IfcOpenshellParseInstanceList;
    editSurfaceStyle(file: IfcOpenshellFile, style: IfcOpenshellInstance, attributes: number): void;
    removeStyle(file: IfcOpenshellFile, style: IfcOpenshellInstance): void;
    removeStyledRepresentation(file: IfcOpenshellFile, representation: IfcOpenshellInstance): void;
    removeSurfaceStyle(file: IfcOpenshellFile, style: IfcOpenshellInstance): void;
    unassignMaterialStyle(file: IfcOpenshellFile, material: IfcOpenshellInstance, style: IfcOpenshellInstance, context: IfcOpenshellInstance): void;
    unassignRepresentationStyles(file: IfcOpenshellFile, shape_representation: IfcOpenshellInstance, styles: IfcOpenshellInstance[], should_use_presentation_style_assignment: boolean): void;
  }

  export interface IfcOpenshellSystemModule {
    /** Create a new distribution port and optionally assign it to an element. */
    addPort(file: IfcOpenshellFile, options: IfcOpenshellSystemAddPortOptions): IfcOpenshellInstance;
    /** Create a new system with the given IFC class. */
    addSystem(file: IfcOpenshellFile, options: IfcOpenshellSystemAddSystemOptions): IfcOpenshellInstance;
    /** Assign a flow control element to a flow element. */
    assignFlowControl(file: IfcOpenshellFile, options: IfcOpenshellSystemAssignFlowControlOptions): IfcOpenshellInstance;
    /** Assign a port to an element. */
    assignPort(file: IfcOpenshellFile, options: IfcOpenshellSystemAssignPortOptions): IfcOpenshellInstance;
    /** Assign products to a system. */
    assignSystem(file: IfcOpenshellFile, options: IfcOpenshellSystemAssignSystemOptions): IfcOpenshellInstance;
    /** Connect two ports together with a given flow direction. */
    connectPort(file: IfcOpenshellFile, options: IfcOpenshellSystemConnectPortOptions): void;
    /** Disconnect a port from all connected ports. */
    disconnectPort(file: IfcOpenshellFile, port: IfcOpenshellInstance): void;
    /** Remove a system and its relationships. */
    removeSystem(file: IfcOpenshellFile, system: IfcOpenshellInstance): void;
    /** Unassign a flow control element from a flow element. */
    unassignFlowControl(file: IfcOpenshellFile, options: IfcOpenshellSystemUnassignFlowControlOptions): void;
    /** Unassign a port from an element. */
    unassignPort(file: IfcOpenshellFile, options: IfcOpenshellSystemUnassignPortOptions): void;
    /** Unassign products from a system. */
    unassignSystem(file: IfcOpenshellFile, options: IfcOpenshellSystemUnassignSystemOptions): void;
  }

  export interface IfcOpenshellTypeModule {
    /** Assign a type to element occurrences, creating or merging IfcRelDefinesByType. */
    assignType(file: IfcOpenshellFile, options: IfcOpenshellTypeAssignTypeOptions): IfcOpenshellInstance;
    /** Map representation maps from a type onto a related object. */
    mapTypeRepresentations(file: IfcOpenshellFile, related_object: IfcOpenshellInstance, relating_type: IfcOpenshellInstance): boolean;
    /** Remove type assignments from element occurrences. */
    unassignType(file: IfcOpenshellFile, options: IfcOpenshellTypeUnassignTypeOptions): void;
  }

  export interface IfcOpenshellUnitModule {
    addContextDependentUnit(file: IfcOpenshellFile, unit_type: string, name: string, dimensions: bigint[]): IfcOpenshellInstance;
    addDerivedUnit(file: IfcOpenshellFile, unit_type: string, userdefinedtype: string | null, units: IfcOpenshellInstance[], exponents: bigint[]): IfcOpenshellInstance;
    addMonetaryUnit(file: IfcOpenshellFile, currency: string): IfcOpenshellInstance;
    addSiUnit(file: IfcOpenshellFile, unit_type: string, prefix: string | null): IfcOpenshellInstance;
    calculateUnitScale(file: IfcOpenshellFile, unit_type: string): number;
    convert(value: number, from_prefix: string, from_unit: string, to_prefix: string, to_unit: string): number;
    convertUnit(value: number, from_unit: IfcOpenshellInstance, to_unit: IfcOpenshellInstance): number;
    formatLength(value: number, precision: number, decimal_places: number, suppress_zero_inches: boolean, unit_system: string, input_unit: string, output_unit: string): string;
    getFullUnitName(unit: IfcOpenshellInstance): string;
    getMeasureClass(unit_type: string): string;
    getMeasureUnitType(measure_class: string): string;
    getNamedDimensions(name: string): number[];
    getPrefix(text: string): string;
    getPrefixMultiplier(text: string): number;
    getProjectUnit(file: IfcOpenshellFile, unit_type: string): IfcOpenshellInstance;
    getSiDimensions(name: string): number[];
    getSymbolMeasureClass(symbol: string): string;
    getSymbolQuantityClass(symbol: string): string;
    getUnitAssignment(file: IfcOpenshellFile): IfcOpenshellInstance;
    getUnitName(text: string): string;
    getUnitNameUniversal(text: string): string;
    getUnitSymbol(unit: IfcOpenshellInstance): string;
    removeUnit(file: IfcOpenshellFile, unit: IfcOpenshellInstance): void;
    resolvePropertyMeasureClass(prop: IfcOpenshellInstance): string;
    resolvePropertyTableDefinedMeasureClass(prop: IfcOpenshellInstance): string;
    resolvePropertyTableDefinedUnit(prop: IfcOpenshellInstance): IfcOpenshellInstance;
    resolvePropertyTableDefiningMeasureClass(prop: IfcOpenshellInstance): string;
    resolvePropertyTableDefiningUnit(prop: IfcOpenshellInstance): IfcOpenshellInstance;
    resolvePropertyUnit(prop: IfcOpenshellInstance): IfcOpenshellInstance;
    unassignUnit(file: IfcOpenshellFile, units: IfcOpenshellInstance[]): void;
  }

  export interface IfcOpenshellValueModule {
    asBool(value: IfcOpenshellValue): boolean;
    asDouble(value: IfcOpenshellValue): number;
    asInstance(value: IfcOpenshellValue): IfcOpenshellInstance;
    asInt64(value: IfcOpenshellValue): bigint;
    asString(value: IfcOpenshellValue): string;
    dictKeyAt(value: IfcOpenshellValue, index: number): string;
    dictSet(dict: IfcOpenshellValue, key: string, value: IfcOpenshellValue | null): boolean;
    dictSize(value: IfcOpenshellValue): number;
    dictValueAt(value: IfcOpenshellValue, index: number): IfcOpenshellValue | null;
    kind(value: IfcOpenshellValue): number;
    listAppend(list: IfcOpenshellValue, item: IfcOpenshellValue | null): boolean;
    listAt(value: IfcOpenshellValue, index: number): IfcOpenshellValue | null;
    listSize(value: IfcOpenshellValue): number;
    newBool(value: boolean): IfcOpenshellValue | null;
    newDict(): IfcOpenshellValue | null;
    newDouble(value: number): IfcOpenshellValue | null;
    newInstance(value: IfcOpenshellInstance | null): IfcOpenshellValue | null;
    newInt(value: bigint): IfcOpenshellValue | null;
    newList(): IfcOpenshellValue | null;
    newNone(): IfcOpenshellValue | null;
    newString(value: string): IfcOpenshellValue | null;
  }

  export interface IfcOpenshellModule {
    IfcOpenshellAggregationType: typeof IfcOpenshellAggregationType;
    IfcOpenshellAttribute: typeof IfcOpenshellAttribute;
    IfcOpenshellDeclaration: typeof IfcOpenshellDeclaration;
    IfcOpenshellEntity: typeof IfcOpenshellEntity;
    IfcOpenshellEnumeration: typeof IfcOpenshellEnumeration;
    IfcOpenshellFileDescription: typeof IfcOpenshellFileDescription;
    IfcOpenshellFileName: typeof IfcOpenshellFileName;
    IfcOpenshellFileSchema: typeof IfcOpenshellFileSchema;
    IfcOpenshellFile: typeof IfcOpenshellFile;
    IfcOpenshellGeomBrepElement: typeof IfcOpenshellGeomBrepElement;
    IfcOpenshellGeomBrepRepresentation: typeof IfcOpenshellGeomBrepRepresentation;
    IfcOpenshellGeomBuffer: typeof IfcOpenshellGeomBuffer;
    IfcOpenshellGeomConversionResultShape: typeof IfcOpenshellGeomConversionResultShape;
    IfcOpenshellGeomElement: typeof IfcOpenshellGeomElement;
    IfcOpenshellGeomFunctionItemEvaluator: typeof IfcOpenshellGeomFunctionItemEvaluator;
    IfcOpenshellGeomGeometrySerializer: typeof IfcOpenshellGeomGeometrySerializer;
    IfcOpenshellGeomIterator: typeof IfcOpenshellGeomIterator;
    IfcOpenshellGeomOpaqueNumber: typeof IfcOpenshellGeomOpaqueNumber;
    IfcOpenshellGeomSerialization: typeof IfcOpenshellGeomSerialization;
    IfcOpenshellGeomSerializedElement: typeof IfcOpenshellGeomSerializedElement;
    IfcOpenshellGeomSerializerSettings: typeof IfcOpenshellGeomSerializerSettings;
    IfcOpenshellGeomSerializer: typeof IfcOpenshellGeomSerializer;
    IfcOpenshellGeomSettings: typeof IfcOpenshellGeomSettings;
    IfcOpenshellGeomSvgfillPolygon: typeof IfcOpenshellGeomSvgfillPolygon;
    IfcOpenshellGeomTaxonomyBooleanResult: typeof IfcOpenshellGeomTaxonomyBooleanResult;
    IfcOpenshellGeomTaxonomyBsplineCurve: typeof IfcOpenshellGeomTaxonomyBsplineCurve;
    IfcOpenshellGeomTaxonomyBsplineSurface: typeof IfcOpenshellGeomTaxonomyBsplineSurface;
    IfcOpenshellGeomTaxonomyCircle: typeof IfcOpenshellGeomTaxonomyCircle;
    IfcOpenshellGeomTaxonomyCollection: typeof IfcOpenshellGeomTaxonomyCollection;
    IfcOpenshellGeomTaxonomyColour: typeof IfcOpenshellGeomTaxonomyColour;
    IfcOpenshellGeomTaxonomyCylinder: typeof IfcOpenshellGeomTaxonomyCylinder;
    IfcOpenshellGeomTaxonomyDirection3: typeof IfcOpenshellGeomTaxonomyDirection3;
    IfcOpenshellGeomTaxonomyEdge: typeof IfcOpenshellGeomTaxonomyEdge;
    IfcOpenshellGeomTaxonomyEllipse: typeof IfcOpenshellGeomTaxonomyEllipse;
    IfcOpenshellGeomTaxonomyExtrusion: typeof IfcOpenshellGeomTaxonomyExtrusion;
    IfcOpenshellGeomTaxonomyFace: typeof IfcOpenshellGeomTaxonomyFace;
    IfcOpenshellGeomTaxonomyItem: typeof IfcOpenshellGeomTaxonomyItem;
    IfcOpenshellGeomTaxonomyLine: typeof IfcOpenshellGeomTaxonomyLine;
    IfcOpenshellGeomTaxonomyLoft: typeof IfcOpenshellGeomTaxonomyLoft;
    IfcOpenshellGeomTaxonomyLoop: typeof IfcOpenshellGeomTaxonomyLoop;
    IfcOpenshellGeomTaxonomyMatrix4: typeof IfcOpenshellGeomTaxonomyMatrix4;
    IfcOpenshellGeomTaxonomyNode: typeof IfcOpenshellGeomTaxonomyNode;
    IfcOpenshellGeomTaxonomyOffsetCurve: typeof IfcOpenshellGeomTaxonomyOffsetCurve;
    IfcOpenshellGeomTaxonomyPlane: typeof IfcOpenshellGeomTaxonomyPlane;
    IfcOpenshellGeomTaxonomyPoint3: typeof IfcOpenshellGeomTaxonomyPoint3;
    IfcOpenshellGeomTaxonomyRevolve: typeof IfcOpenshellGeomTaxonomyRevolve;
    IfcOpenshellGeomTaxonomyShell: typeof IfcOpenshellGeomTaxonomyShell;
    IfcOpenshellGeomTaxonomySolid: typeof IfcOpenshellGeomTaxonomySolid;
    IfcOpenshellGeomTaxonomySphere: typeof IfcOpenshellGeomTaxonomySphere;
    IfcOpenshellGeomTaxonomyStyle: typeof IfcOpenshellGeomTaxonomyStyle;
    IfcOpenshellGeomTaxonomySweepAlongCurve: typeof IfcOpenshellGeomTaxonomySweepAlongCurve;
    IfcOpenshellGeomTaxonomyTorus: typeof IfcOpenshellGeomTaxonomyTorus;
    IfcOpenshellGeomTransformation: typeof IfcOpenshellGeomTransformation;
    IfcOpenshellGeomTreeClashList: typeof IfcOpenshellGeomTreeClashList;
    IfcOpenshellGeomTreeClash: typeof IfcOpenshellGeomTreeClash;
    IfcOpenshellGeomTreeRayIntersectionList: typeof IfcOpenshellGeomTreeRayIntersectionList;
    IfcOpenshellGeomTreeRayIntersection: typeof IfcOpenshellGeomTreeRayIntersection;
    IfcOpenshellGeomTree: typeof IfcOpenshellGeomTree;
    IfcOpenshellGeomTriangulationElement: typeof IfcOpenshellGeomTriangulationElement;
    IfcOpenshellGeomTriangulation: typeof IfcOpenshellGeomTriangulation;
    IfcOpenshellHeader: typeof IfcOpenshellHeader;
    IfcOpenshellInstanceStreamer: typeof IfcOpenshellInstanceStreamer;
    IfcOpenshellInstance: typeof IfcOpenshellInstance;
    IfcOpenshellInverseAttribute: typeof IfcOpenshellInverseAttribute;
    IfcOpenshellNamedType: typeof IfcOpenshellNamedType;
    IfcOpenshellParameterType: typeof IfcOpenshellParameterType;
    IfcOpenshellParseAttributeValue: typeof IfcOpenshellParseAttributeValue;
    IfcOpenshellParseInstanceList: typeof IfcOpenshellParseInstanceList;
    IfcOpenshellPsetTemplateHandle: typeof IfcOpenshellPsetTemplateHandle;
    IfcOpenshellSchema: typeof IfcOpenshellSchema;
    IfcOpenshellSelectType: typeof IfcOpenshellSelectType;
    IfcOpenshellSimpleType: typeof IfcOpenshellSimpleType;
    IfcOpenshellTypeDeclaration: typeof IfcOpenshellTypeDeclaration;
    IfcOpenshellValue: typeof IfcOpenshellValue;
    aggregate: IfcOpenshellAggregateModule;
    attribute: IfcOpenshellAttributeModule;
    boundary: IfcOpenshellBoundaryModule;
    classification: IfcOpenshellClassificationModule;
    cogo: IfcOpenshellCogoModule;
    compute: IfcOpenshellComputeModule;
    constraint: IfcOpenshellConstraintModule;
    context: IfcOpenshellContextModule;
    control: IfcOpenshellControlModule;
    cost: IfcOpenshellCostModule;
    document: IfcOpenshellDocumentModule;
    drawing: IfcOpenshellDrawingModule;
    element: IfcOpenshellElementModule;
    entity: IfcOpenshellEntityModule;
    feature: IfcOpenshellFeatureModule;
    geom: IfcOpenshellGeomModule;
    geometry: IfcOpenshellGeometryModule;
    georeference: IfcOpenshellGeoreferenceModule;
    grid: IfcOpenshellGridModule;
    group: IfcOpenshellGroupModule;
    guid: IfcOpenshellGuidModule;
    layer: IfcOpenshellLayerModule;
    library: IfcOpenshellLibraryModule;
    material: IfcOpenshellMaterialModule;
    nest: IfcOpenshellNestModule;
    owner: IfcOpenshellOwnerModule;
    parse: IfcOpenshellParseModule;
    placement: IfcOpenshellPlacementModule;
    profile: IfcOpenshellProfileModule;
    project: IfcOpenshellProjectModule;
    pset: IfcOpenshellPsetModule;
    register: IfcOpenshellRegisterModule;
    representation: IfcOpenshellRepresentationModule;
    resource: IfcOpenshellResourceModule;
    root: IfcOpenshellRootModule;
    schema: IfcOpenshellSchemaModule;
    selector: IfcOpenshellSelectorModule;
    sequence: IfcOpenshellSequenceModule;
    shape: IfcOpenshellShapeModule;
    spatial: IfcOpenshellSpatialModule;
    structural: IfcOpenshellStructuralModule;
    style: IfcOpenshellStyleModule;
    system: IfcOpenshellSystemModule;
    type: IfcOpenshellTypeModule;
    unit: IfcOpenshellUnitModule;
    value: IfcOpenshellValueModule;
    loadPlugin(kind: 'schema' | 'kernel' | 'mapping' | 'tree' | 'document' | 'geometry_serializer', id: string): Promise<void>;
    loadedPlugins(): string[];
  }

  export function createIfcOpenshellModule(
    initModule: (options?: Record<string, IfcOpenshellRawValue>) => Promise<object>,
    wasmUrl?: string,
    options?: {
      pluginBaseUrl?: string;
      pluginManifest?: Record<string, Record<string, { wasm: string; depends?: string[] }>>;
      pluginLoader?: (
        url: string,
        plugin: { kind: string; id: string; entry: { wasm: string; depends?: string[] } },
      ) => Promise<Uint8Array | ArrayBuffer | ArrayBufferView> | Uint8Array | ArrayBuffer | ArrayBufferView;
    },
  ): Promise<IfcOpenshellModule>;
}
