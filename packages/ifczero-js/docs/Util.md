# Util API

Import styles:

```ts
import { util } from "ifczero-js";
// or
import * as util from "ifczero-js/util";
```

These helpers operate on parse-level entities and IFC relationships/properties.

## Types

- `PropertyValueKind` - Property value kind enum.
- `PropertyValue` - Structured property value record.
- `PropertySet` - Pset/Qto result record with property map.
- `PropertyTableUnit` - Table unit pair record.
- `UnitType` - Unit type string literal union.
- `UnitType` constant - Named unit type constants.

## Element Relationship Helpers

- `getType(entity: Entity): Entity | null` - Gets assigned type object.
- `getTypes(entity: Entity): Entity[]` - Gets all assigned type objects.
- `getPredefinedType(entity: Entity): string | null` - Gets predefined/userdefined type text.
- `isUserDefinedType(entity: Entity): boolean` - Checks whether type is USERDEFINED.
- `getAggregate(entity: Entity): Entity | null` - Gets parent aggregate object.
- `getNest(entity: Entity): Entity | null` - Gets parent nesting object.
- `getFilledVoid(entity: Entity): Entity | null` - Gets opening filled by this element.
- `getVoidedElement(entity: Entity): Entity | null` - Gets element voided by this opening.
- `getContainer(entity: Entity, direct?: boolean, ifcClass?: string): Entity | null` - Gets spatial container.
- `getParent(entity: Entity): Entity | null` - Gets logical parent element.
- `getMaterial(entity: Entity, opts?: { skipUsage?: boolean; inherit?: boolean }): Entity | null` - Gets resolved material.

## Property Set and Property Helpers

- `getPsetEntities(entity: Entity, opts?: { psetsOnly?: boolean; qtosOnly?: boolean; inherit?: boolean }): Entity[]` - Gets pset definition entities.
- `getPsetEntity(entity: Entity, psetName: string, opts?: { psetsOnly?: boolean; qtosOnly?: boolean; inherit?: boolean }): Entity | null` - Gets one pset definition entity by name.
- `getPropertyEntities(definition: Entity): Entity[]` - Gets property entities under a definition.
- `getPropertyEntity(definition: Entity, propertyName: string): Entity | null` - Gets one property entity by name.
- `getPropertyValue(propertyEntity: Entity): PropertyValue | null` - Gets typed property value payload.
- `getPsets(entity: Entity, opts?: { psetsOnly?: boolean; qtosOnly?: boolean; inherit?: boolean }): PropertySet[]` - Gets decoded pset/qto data.
- `getPset(entity: Entity, psetName: string, opts?: { psetsOnly?: boolean; qtosOnly?: boolean; inherit?: boolean }): PropertySet | null` - Gets one decoded pset/qto by name.

## Classification Helpers

- `getClassificationReferences(entity: Entity, inherit?: boolean): Entity[]` - Gets classification reference entities.
- `getClassification(reference: Entity): Entity | null` - Gets classification entity for reference.
- `getInheritedClassificationReferences(reference: Entity): Entity[]` - Gets inherited classification references.

## Unit Helpers

- `unitScale(filePtr: number, unitType: string): number` - Computes project scale factor for unit type.
- `unitSiTypeName(unitType: string): string | null` - Gets SI base unit label.
- `getProjectUnit(filePtr: number, unitType: string): Entity | null` - Gets project unit entity for unit type.
- `getPropertyUnit(propertyEntity: Entity, filePtr: number): Entity | null` - Gets resolved property unit entity.
- `getPropertyTableUnit(propertyEntity: Entity, filePtr: number): PropertyTableUnit | null` - Gets defining/defined units for table properties.
- `unitConvert(value: number, fromPrefix: string, fromUnit: string, toPrefix: string, toUnit: string): number` - Converts numeric value between SI units.

## Raw Util Bindings

- util module export: `raw`.
- root module export: `ifcutilRaw`.

These are low-level direct bindings; use ergonomic helpers unless you need direct ABI calls.
