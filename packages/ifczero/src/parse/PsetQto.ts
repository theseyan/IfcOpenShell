import type { EmscriptenModule } from "../wasm/types.js";
import { getModule } from "../api.js";
import * as bind from "./psetqto-bindings.generated.js";
import { readString } from "../wasm/memory.js";
import type { Ptr } from "./types.js";

const IFC2X3_TEMPLATE_URL = new URL("../wasm/psetqto/Pset_IFC2X3.ifc", import.meta.url);
const IFC4_TEMPLATE_URL = new URL("../wasm/psetqto/Pset_IFC4_ADD2.ifc", import.meta.url);
const IFC4X3_TEMPLATE_URL = new URL("../wasm/psetqto/Pset_IFC4X3.ifc", import.meta.url);
const AUTOLOAD_SCHEMAS = ["IFC2X3", "IFC4", "IFC4X3"] as const;

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
  private readonly templateLoadAttempts = new Map<string, Promise<boolean>>();

  constructor() {
    this.M = getModule();
    for (const schema of AUTOLOAD_SCHEMAS) {
      this.ensureBundledTemplateLoadAttempt(schema);
    }
  }

  private static templateUrlForSchema(schema: string): URL | null {
    const normalized = schema.trim().toUpperCase();
    if (normalized === "IFC4") return IFC4_TEMPLATE_URL;
    if (normalized === "IFC2X3") return IFC2X3_TEMPLATE_URL;
    if (normalized === "IFC4X3" || normalized === "IFC4X3_ADD2") return IFC4X3_TEMPLATE_URL;
    return null;
  }

  bundledTemplateUrl(schema: string): URL | null {
    return PsetQto.templateUrlForSchema(schema);
  }

  private ensureBundledTemplateLoadAttempt(schema: string): void {
    const normalized = schema.trim().toUpperCase();
    if (!PsetQto.templateUrlForSchema(normalized)) return;
    if (this.isTemplateLoaded(normalized)) return;
    if (this.templateLoadAttempts.has(normalized)) return;

    const attempt = this.loadBundledTemplate(normalized).catch(() => false);
    this.templateLoadAttempts.set(normalized, attempt);
    attempt.finally(() => {
      this.templateLoadAttempts.delete(normalized);
    });
  }

  loadTemplateFromMemory(schema: string, data: Uint8Array | ArrayBuffer): boolean {
    const bytes = data instanceof Uint8Array ? data : new Uint8Array(data);
    if (bytes.length === 0) {
      return false;
    }

    const dataPtr = this.M._malloc(bytes.length);
    if (!dataPtr) {
      throw new Error(`Out of WASM memory while allocating ${bytes.length} bytes for template`);
    }

    this.M.HEAPU8.set(bytes, dataPtr);
    try {
      return bind.load_template_from_memory(this.M, schema, dataPtr, bytes.length) !== 0;
    } finally {
      this.M._free(dataPtr);
    }
  }

  loadTemplateFromFile(schema: string, path: string): boolean {
    return bind.load_template_from_file(this.M, schema, path) !== 0;
  }

  async loadTemplateFromUrl(
    schema: string,
    url: string | URL,
    init?: RequestInit,
  ): Promise<boolean> {
    const response = await fetch(url, init);
    if (!response.ok) {
      throw new Error(`Failed to fetch template IFC: ${response.status} ${response.statusText}`);
    }
    const bytes = new Uint8Array(await response.arrayBuffer());
    return this.loadTemplateFromMemory(schema, bytes);
  }

  async loadBundledTemplate(schema: string, init?: RequestInit): Promise<boolean> {
    const url = this.bundledTemplateUrl(schema);
    if (!url) return false;
    return this.loadTemplateFromUrl(schema, url, init);
  }

  unloadTemplate(schema: string): boolean {
    return bind.unload_template(this.M, schema) !== 0;
  }

  isTemplateLoaded(schema: string): boolean {
    return bind.is_template_loaded(this.M, schema) !== 0;
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
    this.ensureBundledTemplateLoadAttempt(schema);
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
    this.ensureBundledTemplateLoadAttempt(schema);
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
    this.ensureBundledTemplateLoadAttempt(schema);
    const ptr = bind.get_by_name(this.M, schema, name);
    if (!ptr) return null;
    return this._readTemplate(ptr);
  }

  isTemplated(schema: string, name: string): boolean {
    this.ensureBundledTemplateLoadAttempt(schema);
    return bind.is_templated(this.M, schema, name) !== 0;
  }

  allTemplates(schema: string): PropertySetTemplate[] {
    this.ensureBundledTemplateLoadAttempt(schema);
    const handle = bind.all_templates(this.M, schema);
    if (!handle) return [];
    return this._readTemplateList(handle);
  }

  getProperties(templatePtr: Ptr): PropertyTemplate[] {
    const handle = bind.template_properties(this.M, templatePtr);
    if (!handle) return [];

    try {
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
      return result;
    } finally {
      bind.property_list_close(this.M, handle);
    }
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
    try {
      const count = bind.template_list_count(this.M, handle);
      const result: PropertySetTemplate[] = [];
      for (let i = 0; i < count; i++) {
        const ptr = bind.template_list_get(this.M, handle, i);
        if (!ptr) continue;
        result.push(this._readTemplate(ptr));
      }
      return result;
    } finally {
      bind.template_list_close(this.M, handle);
    }
  }

  private _readStringList(handle: Ptr): string[] {
    try {
      const count = bind.string_list_count(this.M, handle);
      const result: string[] = [];
      for (let i = 0; i < count; i++) {
        const ptr = bind.string_list_get(this.M, handle, i);
        if (!ptr) continue;
        const str = readString(ptr);
        if (str) result.push(str);
      }
      return result;
    } finally {
      bind.string_list_close(this.M, handle);
    }
  }
}
