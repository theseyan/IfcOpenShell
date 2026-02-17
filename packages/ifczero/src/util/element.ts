import type { EmscriptenModule } from "../wasm/types.js";
import { getModule } from "../api.js";
import * as bind from "../parse/util-bindings.generated.js";
import { Entity } from "../parse/Entity.js";
import { PropertyValueKind } from "./types.js";

type Ptr = number;

function readOwnedString(M: EmscriptenModule, ptr: number): string | null {
  if (!ptr) return null;
  const str = M.UTF8ToString(ptr);
  bind.free_string(M, ptr);
  return str;
}

function readEntityList(M: EmscriptenModule, listPtr: Ptr): Entity[] {
  if (!listPtr) return [];
  const count = bind.entity_list_count(M, listPtr);
  const result: Entity[] = [];
  for (let i = 0; i < count; i++) {
    const ePtr = bind.entity_list_get(M, listPtr, i);
    if (ePtr) result.push(new Entity(M, ePtr));
  }
  bind.entity_list_destroy(M, listPtr);
  return result;
}

export interface PropertyValue {
  kind: PropertyValueKind;
  propertyEntity?: Entity | null;
  int?: number;
  bool?: boolean;
  double?: number;
  string?: string;
  list?: PropertyValue[];
  listCount?: number;
}

export interface PropertySet {
  name: string;
  definitionPtr: Ptr;
  properties: Record<string, PropertyValue>;
}

export interface PropertyTableUnit {
  definingUnit: Entity | null;
  definedUnit: Entity | null;
}

export function getType(entity: Entity): Entity | null {
  const M = getModule();
  const ptr = bind.element_get_type(M, entity.ptr);
  return ptr ? new Entity(M, ptr) : null;
}

export function getTypes(entity: Entity): Entity[] {
  const M = getModule();
  return readEntityList(M, bind.element_get_types(M, entity.ptr));
}

export function getPredefinedType(entity: Entity): string | null {
  const M = getModule();
  return readOwnedString(M, bind.element_get_predefined_type_copy(M, entity.ptr));
}

export function isUserDefinedType(entity: Entity): boolean {
  const M = getModule();
  return bind.element_is_userdefined_type(M, entity.ptr) !== 0;
}

export function getAggregate(entity: Entity): Entity | null {
  const M = getModule();
  const ptr = bind.element_get_aggregate(M, entity.ptr);
  return ptr ? new Entity(M, ptr) : null;
}

export function getNest(entity: Entity): Entity | null {
  const M = getModule();
  const ptr = bind.element_get_nest(M, entity.ptr);
  return ptr ? new Entity(M, ptr) : null;
}

export function getFilledVoid(entity: Entity): Entity | null {
  const M = getModule();
  const ptr = bind.element_get_filled_void(M, entity.ptr);
  return ptr ? new Entity(M, ptr) : null;
}

export function getVoidedElement(entity: Entity): Entity | null {
  const M = getModule();
  const ptr = bind.element_get_voided_element(M, entity.ptr);
  return ptr ? new Entity(M, ptr) : null;
}

export function getContainer(
  entity: Entity,
  direct = true,
  ifcClass?: string,
): Entity | null {
  const M = getModule();
  const ptr = bind.element_get_container(M, entity.ptr, direct ? 1 : 0, ifcClass ?? null);
  return ptr ? new Entity(M, ptr) : null;
}

export function getParent(entity: Entity): Entity | null {
  const M = getModule();
  const ptr = bind.element_get_parent(M, entity.ptr);
  return ptr ? new Entity(M, ptr) : null;
}

export function getMaterial(
  entity: Entity,
  opts: { skipUsage?: boolean; inherit?: boolean } = {},
): Entity | null {
  const M = getModule();
  const ptr = bind.element_get_material(
    M,
    entity.ptr,
    opts.skipUsage ? 1 : 0,
    opts.inherit !== false ? 1 : 0,
  );
  return ptr ? new Entity(M, ptr) : null;
}

export function getPsetEntities(
  entity: Entity,
  opts: { psetsOnly?: boolean; qtosOnly?: boolean; inherit?: boolean } = {},
): Entity[] {
  const M = getModule();
  const listPtr = bind.element_get_pset_entities(
    M,
    entity.ptr,
    opts.psetsOnly ? 1 : 0,
    opts.qtosOnly ? 1 : 0,
    opts.inherit !== false ? 1 : 0,
  );
  return readEntityList(M, listPtr);
}

export function getPsetEntity(
  entity: Entity,
  psetName: string,
  opts: { psetsOnly?: boolean; qtosOnly?: boolean; inherit?: boolean } = {},
): Entity | null {
  const M = getModule();
  const ptr = bind.element_get_pset_entity(
    M,
    entity.ptr,
    psetName,
    opts.psetsOnly ? 1 : 0,
    opts.qtosOnly ? 1 : 0,
    opts.inherit !== false ? 1 : 0,
  );
  return ptr ? new Entity(M, ptr) : null;
}

export function getPropertyEntities(
  definition: Entity,
): Entity[] {
  const M = getModule();
  return readEntityList(M, bind.element_get_property_entities(M, definition.ptr));
}

export function getPropertyEntity(
  definition: Entity,
  propertyName: string,
): Entity | null {
  const M = getModule();
  const ptr = bind.element_get_property_entity(M, definition.ptr, propertyName);
  return ptr ? new Entity(M, ptr) : null;
}

export function getPropertyValue(
  propertyEntity: Entity,
): PropertyValue | null {
  const M = getModule();
  const handle = bind.element_get_property_value(M, propertyEntity.ptr);
  if (!handle) return null;

  try {
    const kind = bind.property_value_kind(M, handle) as PropertyValueKind;
    const value: PropertyValue = { kind };
    switch (kind) {
      case PropertyValueKind.INT:
        value.int = bind.property_value_int(M, handle);
        break;
      case PropertyValueKind.BOOL:
        value.bool = bind.property_value_int(M, handle) !== 0;
        break;
      case PropertyValueKind.DOUBLE:
        value.double = bind.property_value_double(M, handle);
        break;
      case PropertyValueKind.STRING:
        value.string = readOwnedString(M, bind.property_value_string_copy(M, handle)) ?? "";
        break;
      case PropertyValueKind.LIST:
        value.listCount = bind.property_value_list_count(M, handle);
        break;
    }
    return value;
  } finally {
    bind.property_value_destroy(M, handle);
  }
}

export function getPsets(
  entity: Entity,
  opts: { psetsOnly?: boolean; qtosOnly?: boolean; inherit?: boolean } = {},
): PropertySet[] {
  const M = getModule();
  const listPtr = bind.element_get_psets(
    M,
    entity.ptr,
    opts.psetsOnly ? 1 : 0,
    opts.qtosOnly ? 1 : 0,
    opts.inherit !== false ? 1 : 0,
  );
  if (!listPtr) return [];

  const count = bind.pset_list_count(M, listPtr);
  const result: PropertySet[] = [];
  try {
    for (let i = 0; i < count; i++) {
      const handle = bind.pset_list_get(M, listPtr, i);
      if (!handle) continue;
      result.push(readPset(M, handle, false));
    }
  } finally {
    bind.pset_list_destroy(M, listPtr);
  }
  return result;
}

export function getPset(
  entity: Entity,
  psetName: string,
  opts: { psetsOnly?: boolean; qtosOnly?: boolean; inherit?: boolean } = {},
): PropertySet | null {
  const M = getModule();
  const handle = bind.element_get_pset(
    M,
    entity.ptr,
    psetName,
    opts.psetsOnly ? 1 : 0,
    opts.qtosOnly ? 1 : 0,
    opts.inherit !== false ? 1 : 0,
  );

  if (!handle) return null;
  return readPset(M, handle, true);
}

function readPset(M: EmscriptenModule, handle: Ptr, destroy: boolean): PropertySet {
  try {
    const name = readOwnedString(M, bind.pset_name_copy(M, handle)) ?? "";
    const defPtr = bind.pset_definition(M, handle);
    const propCount = bind.pset_property_count(M, handle);
    const properties: Record<string, PropertyValue> = {};

    for (let j = 0; j < propCount; j++) {
      const propName = readOwnedString(M, bind.pset_property_name_copy(M, handle, j)) ?? `prop_${j}`;
      const kind = bind.pset_property_value_kind(M, handle, j) as PropertyValueKind;
      const value = readPsetPropertyValue(M, handle, j, kind);
      const propertyEntityPtr = bind.pset_property_entity(M, handle, j);
      value.propertyEntity = propertyEntityPtr ? new Entity(M, propertyEntityPtr) : null;
      properties[propName] = value;
    }

    return { name, definitionPtr: defPtr, properties };
  } finally {
    if (destroy) bind.pset_destroy(M, handle);
  }
}

function readPsetPropertyValue(
  M: EmscriptenModule,
  handle: Ptr,
  index: number,
  kind: PropertyValueKind,
): PropertyValue {
  const val: PropertyValue = { kind };

  switch (kind) {
    case PropertyValueKind.INT:
      val.int = bind.pset_property_value_int(M, handle, index);
      break;
    case PropertyValueKind.BOOL:
      val.bool = bind.pset_property_value_int(M, handle, index) !== 0;
      break;
    case PropertyValueKind.DOUBLE:
      val.double = bind.pset_property_value_double(M, handle, index);
      break;
    case PropertyValueKind.STRING:
      val.string = readOwnedString(M, bind.pset_property_value_string_copy(M, handle, index)) ?? "";
      break;
    case PropertyValueKind.LIST: {
      const listCount = bind.pset_property_value_list_count(M, handle, index);
      val.list = [];
      for (let k = 0; k < listCount; k++) {
        const itemKind = bind.pset_property_value_list_item_kind(M, handle, index, k) as PropertyValueKind;
        const item: PropertyValue = { kind: itemKind };
        switch (itemKind) {
          case PropertyValueKind.INT:
            item.int = bind.pset_property_value_list_item_int(M, handle, index, k);
            break;
          case PropertyValueKind.BOOL:
            item.bool = bind.pset_property_value_list_item_int(M, handle, index, k) !== 0;
            break;
          case PropertyValueKind.DOUBLE:
            item.double = bind.pset_property_value_list_item_double(M, handle, index, k);
            break;
          case PropertyValueKind.STRING:
            item.string = readOwnedString(M, bind.pset_property_value_list_item_string_copy(M, handle, index, k)) ?? "";
            break;
        }
        val.list.push(item);
      }
      break;
    }
  }

  return val;
}

export function getClassificationReferences(
  entity: Entity,
  inherit = true,
): Entity[] {
  const M = getModule();
  return readEntityList(M, bind.classification_get_references(M, entity.ptr, inherit ? 1 : 0));
}

export function getClassification(
  reference: Entity,
): Entity | null {
  const M = getModule();
  const ptr = bind.classification_get_classification(M, reference.ptr);
  return ptr ? new Entity(M, ptr) : null;
}

export function getInheritedClassificationReferences(
  reference: Entity,
): Entity[] {
  const M = getModule();
  return readEntityList(M, bind.classification_get_inherited_references(M, reference.ptr));
}

export function unitScale(filePtr: Ptr, unitType: string): number {
  const M = getModule();
  return bind.unit_calculate_unit_scale(M, filePtr, unitType);
}

export function unitSiTypeName(unitType: string): string | null {
  const M = getModule();
  return readOwnedString(M, bind.unit_si_type_name_copy(M, unitType));
}

export function getProjectUnit(filePtr: Ptr, unitType: string): Entity | null {
  const M = getModule();
  const ptr = bind.unit_get_project_unit(M, filePtr, unitType);
  return ptr ? new Entity(M, ptr) : null;
}

export function getPropertyUnit(propertyEntity: Entity, filePtr: Ptr): Entity | null {
  const M = getModule();
  const ptr = bind.unit_get_property_unit(M, propertyEntity.ptr, filePtr);
  return ptr ? new Entity(M, ptr) : null;
}

export function getPropertyTableUnit(
  propertyEntity: Entity,
  filePtr: Ptr,
): PropertyTableUnit | null {
  const M = getModule();
  const handle = bind.unit_get_property_table_unit(M, propertyEntity.ptr, filePtr);
  if (!handle) return null;

  try {
    const definingPtr = bind.property_table_unit_defining(M, handle);
    const definedPtr = bind.property_table_unit_defined(M, handle);
    return {
      definingUnit: definingPtr ? new Entity(M, definingPtr) : null,
      definedUnit: definedPtr ? new Entity(M, definedPtr) : null,
    };
  } finally {
    bind.property_table_unit_destroy(M, handle);
  }
}

export function unitConvert(
  value: number,
  fromPrefix: string,
  fromUnit: string,
  toPrefix: string,
  toUnit: string,
): number {
  const M = getModule();
  return bind.unit_convert(M, value, fromPrefix, fromUnit, toPrefix, toUnit);
}
