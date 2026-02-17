import type { EmscriptenModule } from "../wasm/types.js";
import { getModule } from "../api.js";
import * as bind from "./bindings.generated.js";
import {
  AggregationType,
  DeclarationKind,
  InverseAggregationType,
  ParameterTypeKind,
  SimpleType,
} from "./types.js";

type Ptr = number;

export class TypeDeclaration {
  constructor(
    readonly M: EmscriptenModule,
    readonly ptr: Ptr,
  ) {}

  get name(): string {
    return bind.type_name(this.M, this.ptr);
  }

  get schemaName(): string {
    return bind.type_schema_name(this.M, this.ptr);
  }

  get index(): number {
    return bind.type_index(this.M, this.ptr);
  }

  get kind(): DeclarationKind {
    return bind.type_kind(this.M, this.ptr);
  }

  get isAbstract(): boolean {
    return bind.type_is_abstract(this.M, this.ptr) !== 0;
  }

  isA(typeName: string): boolean {
    return bind.type_is_a(this.M, this.ptr, typeName) !== 0;
  }

  get supertype(): TypeDeclaration | null {
    const ptr = bind.type_supertype(this.M, this.ptr);
    return ptr ? new TypeDeclaration(this.M, ptr) : null;
  }

  get subtypes(): TypeDeclaration[] {
    const listPtr = bind.type_subtypes(this.M, this.ptr);
    if (!listPtr) return [];
    const count = bind.type_list_count(this.M, listPtr);
    const result: TypeDeclaration[] = [];
    for (let i = 0; i < count; i++) {
      const tPtr = bind.type_list_get(this.M, listPtr, i);
      if (tPtr) result.push(new TypeDeclaration(this.M, tPtr));
    }
    bind.type_list_close(this.M, listPtr);
    return result;
  }

  get declaredType(): TypeDeclaration | null {
    const ptr = bind.type_declared_type(this.M, this.ptr);
    return ptr ? new TypeDeclaration(this.M, ptr) : null;
  }

  attributes(includeInherited = true): AttributeDeclaration[] {
    const listPtr = bind.type_attributes(this.M, this.ptr, includeInherited ? 1 : 0);
    if (!listPtr) return [];
    const count = bind.attribute_list_count(this.M, listPtr);
    const result: AttributeDeclaration[] = [];
    for (let i = 0; i < count; i++) {
      const aPtr = bind.attribute_list_get(this.M, listPtr, i);
      if (aPtr) result.push(new AttributeDeclaration(this.M, aPtr));
    }
    bind.attribute_list_close(this.M, listPtr);
    return result;
  }

  inverseAttributes(includeInherited = true): InverseAttributeDeclaration[] {
    const listPtr = bind.type_inverse_attributes(this.M, this.ptr, includeInherited ? 1 : 0);
    if (!listPtr) return [];
    const count = bind.inverse_attribute_list_count(this.M, listPtr);
    const result: InverseAttributeDeclaration[] = [];
    for (let i = 0; i < count; i++) {
      const aPtr = bind.inverse_attribute_list_get(this.M, listPtr, i);
      if (aPtr) result.push(new InverseAttributeDeclaration(this.M, aPtr));
    }
    bind.inverse_attribute_list_close(this.M, listPtr);
    return result;
  }

  attributeIndex(name: string): number {
    return bind.type_attribute_index(this.M, this.ptr, name);
  }

  enumerationItems(): string[] {
    const listPtr = bind.type_enumeration_items(this.M, this.ptr);
    if (!listPtr) return [];
    const count = bind.string_list_count(this.M, listPtr);
    const result: string[] = [];
    for (let i = 0; i < count; i++) {
      result.push(bind.string_list_get(this.M, listPtr, i));
    }
    bind.string_list_close(this.M, listPtr);
    return result;
  }

  selectList(): TypeDeclaration[] {
    const listPtr = bind.type_select_list(this.M, this.ptr);
    if (!listPtr) return [];
    const count = bind.type_list_count(this.M, listPtr);
    const result: TypeDeclaration[] = [];
    for (let i = 0; i < count; i++) {
      const tPtr = bind.type_list_get(this.M, listPtr, i);
      if (tPtr) result.push(new TypeDeclaration(this.M, tPtr));
    }
    bind.type_list_close(this.M, listPtr);
    return result;
  }
}

export class InverseAttributeDeclaration {
  constructor(
    readonly M: EmscriptenModule,
    readonly ptr: Ptr,
  ) {}

  get name(): string {
    return bind.inverse_attribute_name(this.M, this.ptr);
  }

  get aggregationType(): InverseAggregationType {
    return bind.inverse_attribute_aggregation_type(this.M, this.ptr) as InverseAggregationType;
  }

  get bound1(): number {
    return bind.inverse_attribute_bound1(this.M, this.ptr);
  }

  get bound2(): number {
    return bind.inverse_attribute_bound2(this.M, this.ptr);
  }

  get entityReference(): TypeDeclaration | null {
    const ptr = bind.inverse_attribute_entity_reference(this.M, this.ptr);
    return ptr ? new TypeDeclaration(this.M, ptr) : null;
  }

  get attributeReference(): AttributeDeclaration | null {
    const ptr = bind.inverse_attribute_attribute_reference(this.M, this.ptr);
    return ptr ? new AttributeDeclaration(this.M, ptr) : null;
  }
}

export class ParameterTypeDeclaration {
  constructor(
    readonly M: EmscriptenModule,
    readonly ptr: Ptr,
  ) {}

  get kind(): ParameterTypeKind {
    return bind.parameter_type_kind(this.M, this.ptr) as ParameterTypeKind;
  }

  get namedDeclaredType(): TypeDeclaration | null {
    const ptr = bind.parameter_type_named_declared_type(this.M, this.ptr);
    return ptr ? new TypeDeclaration(this.M, ptr) : null;
  }

  get simpleDeclaredType(): SimpleType {
    return bind.parameter_type_simple_declared_type(this.M, this.ptr) as SimpleType;
  }

  get aggregationType(): AggregationType {
    return bind.parameter_type_aggregation_type(this.M, this.ptr) as AggregationType;
  }

  get aggregationBound1(): number {
    return bind.parameter_type_aggregation_bound1(this.M, this.ptr);
  }

  get aggregationBound2(): number {
    return bind.parameter_type_aggregation_bound2(this.M, this.ptr);
  }

  get aggregationElementType(): ParameterTypeDeclaration | null {
    const ptr = bind.parameter_type_aggregation_element_type(this.M, this.ptr);
    return ptr ? new ParameterTypeDeclaration(this.M, ptr) : null;
  }
}

export class AttributeDeclaration {
  constructor(
    readonly M: EmscriptenModule,
    readonly ptr: Ptr,
  ) {}

  get name(): string {
    return bind.attribute_name(this.M, this.ptr);
  }

  get optional(): boolean {
    return bind.attribute_optional(this.M, this.ptr) !== 0;
  }

  get parameterType(): ParameterTypeDeclaration | null {
    const ptr = bind.attribute_parameter_type(this.M, this.ptr);
    return ptr ? new ParameterTypeDeclaration(this.M, ptr) : null;
  }
}

export class Schema {
  readonly M: EmscriptenModule;
  readonly name: string;

  constructor(name: string) {
    this.M = getModule();
    this.name = name;
  }

  declarationByName(typeName: string): TypeDeclaration | null {
    const ptr = bind.schema_declaration_by_name(this.M, this.name, typeName);
    return ptr ? new TypeDeclaration(this.M, ptr) : null;
  }

  entities(): TypeDeclaration[] {
    const listPtr = bind.schema_entities(this.M, this.name);
    if (!listPtr) return [];
    const count = bind.type_list_count(this.M, listPtr);
    const result: TypeDeclaration[] = [];
    for (let i = 0; i < count; i++) {
      const tPtr = bind.type_list_get(this.M, listPtr, i);
      if (tPtr) result.push(new TypeDeclaration(this.M, tPtr));
    }
    bind.type_list_close(this.M, listPtr);
    return result;
  }

  static availableSchemas(): string[] {
    const M = getModule();
    const listPtr = bind.schema_names(M);
    if (!listPtr) return [];
    const count = bind.string_list_count(M, listPtr);
    const result: string[] = [];
    for (let i = 0; i < count; i++) {
      result.push(bind.string_list_get(M, listPtr, i));
    }
    bind.string_list_close(M, listPtr);
    return result;
  }
}
