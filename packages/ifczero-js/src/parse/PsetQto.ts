import type { EmscriptenModule } from "../wasm/types.js";
import { getModule } from "../api.js";
import * as bind from "./psetqto-bindings.generated.js";
import { readString } from "../wasm/memory.js";
import type { Ptr } from "./types.js";

export interface PropertyTemplate {
  name: string | null;
  description: string | null;
  templateType: string | null;
  primaryMeasureType: string | null;
}

export interface PropertySetTemplate {
  ptr: Ptr;
  name: string | null;
  description: string | null;
  templateType: string | null;
  applicableEntity: string | null;
}

export class PsetQto {
  readonly M: EmscriptenModule;

  constructor() {
    this.M = getModule();
  }

  getApplicable(
    schema: string,
    ifcClass: string,
    options: {
      predefinedType?: string;
      psetOnly?: boolean;
      qtoOnly?: boolean;
    } = {},
  ): PropertySetTemplate[] {
    const handle = bind.get_applicable(
      this.M,
      schema,
      ifcClass,
      options.predefinedType ?? "",
      options.psetOnly ? 1 : 0,
      options.qtoOnly ? 1 : 0,
    );
    if (!handle) return [];
    return this._readTemplateList(handle);
  }

  getApplicableNames(
    schema: string,
    ifcClass: string,
    options: {
      predefinedType?: string;
      psetOnly?: boolean;
      qtoOnly?: boolean;
    } = {},
  ): string[] {
    const handle = bind.get_applicable_names(
      this.M,
      schema,
      ifcClass,
      options.predefinedType ?? "",
      options.psetOnly ? 1 : 0,
      options.qtoOnly ? 1 : 0,
    );
    if (!handle) return [];
    return this._readStringList(handle);
  }

  getByName(schema: string, name: string): PropertySetTemplate | null {
    const ptr = bind.get_by_name(this.M, schema, name);
    if (!ptr) return null;
    return this._readTemplate(ptr);
  }

  isTemplated(schema: string, name: string): boolean {
    return bind.is_templated(this.M, schema, name) !== 0;
  }

  allTemplates(schema: string): PropertySetTemplate[] {
    const handle = bind.all_templates(this.M, schema);
    if (!handle) return [];
    return this._readTemplateList(handle);
  }

  getProperties(templatePtr: Ptr): PropertyTemplate[] {
    const handle = bind.template_properties(this.M, templatePtr);
    if (!handle) return [];

    const count = bind.property_list_count(this.M, handle);
    const result: PropertyTemplate[] = [];
    for (let i = 0; i < count; i++) {
      const propPtr = bind.property_list_get(this.M, handle, i);
      if (!propPtr) continue;
      result.push({
        name: readString(bind.property_name_borrowed(this.M, propPtr)),
        description: readString(bind.property_description_borrowed(this.M, propPtr)),
        templateType: readString(bind.property_template_type_borrowed(this.M, propPtr)),
        primaryMeasureType: readString(bind.property_primary_measure_type_borrowed(this.M, propPtr)),
      });
    }
    bind.property_list_close(this.M, handle);
    return result;
  }

  deinit(): void {
    bind.deinit(this.M);
  }

  private _readTemplate(ptr: Ptr): PropertySetTemplate {
    return {
      ptr,
      name: readString(bind.template_name_borrowed(this.M, ptr)),
      description: readString(bind.template_description_borrowed(this.M, ptr)),
      templateType: readString(bind.template_type_borrowed(this.M, ptr)),
      applicableEntity: readString(bind.template_applicable_entity_borrowed(this.M, ptr)),
    };
  }

  private _readTemplateList(handle: Ptr): PropertySetTemplate[] {
    const count = bind.template_list_count(this.M, handle);
    const result: PropertySetTemplate[] = [];
    for (let i = 0; i < count; i++) {
      const ptr = bind.template_list_get(this.M, handle, i);
      if (!ptr) continue;
      result.push(this._readTemplate(ptr));
    }
    bind.template_list_close(this.M, handle);
    return result;
  }

  private _readStringList(handle: Ptr): string[] {
    const count = bind.string_list_count(this.M, handle);
    const result: string[] = [];
    for (let i = 0; i < count; i++) {
      const ptr = bind.string_list_get(this.M, handle, i);
      if (!ptr) continue;
      const str = readString(ptr);
      if (str) result.push(str);
    }
    bind.string_list_close(this.M, handle);
    return result;
  }
}
