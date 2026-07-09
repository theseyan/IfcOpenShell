import { useCallback, useEffect, useRef, useState, type ChangeEvent } from 'react';
import {
  GeomSettings,
  IfcFile,
  exportToBuffer,
  init,
  util,
} from '@ifcopenshell-js/web';
import type {
  IfcOpenShell,
} from '@ifcopenshell-js/web';
import ModelViewport, { type ViewMesh } from './ModelViewport';
import './App.css';

const { inspectEntity, meshColor } = util;

type RuntimeState = 'loading' | 'ready' | 'error';
type ExportFormat = 'obj' | 'ttl' | 'svg';

/** Selectable geometry kernel identifier. */
type KernelId = 'passthrough' | 'opencascade' | 'manifold';

type TypeCount = { name: string; count: number };

type ModelStats = {
  schema: string;
  entityCount: number;
  meshCount: number;
  vertexCount: number;
  faceCount: number;
  topTypes: TypeCount[];
  bounds: { min: [number, number, number]; max: [number, number, number] } | null;
  truncated: boolean;
};

const SAMPLE_MODELS = [
  { label: 'Large Offset (IFC4)', path: '/samples/large_offset.ifc', schema: 'ifc4' },
  { label: 'Faceted BRep (IFC2x3)', path: '/samples/faceted_brep.ifc', schema: 'ifc2x3' },
];

const SCHEMAS = [
  { id: 'ifc4', label: 'IFC4' },
  { id: 'ifc2x3', label: 'IFC2X3' },
  { id: 'ifc4x3_add2', label: 'IFC4x3' },
];

const KERNELS: { id: KernelId; label: string; hint: string }[] = [
  { id: 'opencascade', label: 'OpenCascade (OCCT)', hint: 'Best geometry quality' },
  { id: 'manifold', label: 'Manifold', hint: 'Fast, robust triangle cleanup' },
  { id: 'passthrough', label: 'Passthrough', hint: 'No tessellation' },
];

const EXPORT_FORMATS: { id: ExportFormat; label: string; note: string }[] = [
  { id: 'obj', label: 'OBJ', note: 'Wavefront mesh + MTL' },
  { id: 'ttl', label: 'TTL', note: 'Requires OCCT' },
  { id: 'svg', label: 'SVG', note: 'Requires OCCT' },
];

/** Cache of the most recently loaded IFC bytes for kernel-switch re-runs. */
type LoadedModel = { bytes: Uint8Array; name: string; schema: string };

function isCancelled(error: unknown): boolean {
  return error instanceof Error && error.message.includes('cancelled');
}

function throwIfCancelled(signal: AbortSignal): void {
  if (signal.aborted) throw new Error('IfcOpenShell operation was cancelled');
}

async function computeModelBounds(
  file: IfcFile,
  settings: GeomSettings,
  kernel: KernelId,
): Promise<{ min: [number, number, number]; max: [number, number, number] } | null> {
  const bounds = await file.bounds(settings, { kernel, numThreads: 1 });
  return bounds.min && bounds.max ? { min: bounds.min, max: bounds.max } : null;
}

export default function App() {
  const [runtimeState, setRuntimeState] = useState<RuntimeState>('loading');
  const [runtimeError, setRuntimeError] = useState<string | null>(null);
  const [busy, setBusy] = useState<string | null>('Initializing @ifcopenshell-js/web...');

  const [selectedSchema, setSelectedSchema] = useState<string>('ifc4');
  const [selectedKernel, setSelectedKernel] = useState<KernelId>('opencascade');
  const [activeKernel, setActiveKernel] = useState<KernelId | null>(null);
  const [plugins, setPlugins] = useState<string[]>([]);

  const [modelName, setModelName] = useState<string>('No model loaded');
  const [modelStats, setModelStats] = useState<ModelStats | null>(null);
  const [meshes, setMeshes] = useState<ViewMesh[]>([]);
  const [selectedId, setSelectedId] = useState<number | null>(null);
  const [selectedInfo, setSelectedInfo] = useState<Awaited<ReturnType<typeof inspectEntity>> | null>(null);
  const [modelRevision, setModelRevision] = useState(0);

  const [weldVertices, setWeldVertices] = useState(true);
  const [linearDeflection, setLinearDeflection] = useState(0.02);
  const [maxMeshes, setMaxMeshes] = useState(600);

  const [exportFormat, setExportFormat] = useState<ExportFormat>('obj');
  const [exportText, setExportText] = useState<string>('');
  const [exportPayload, setExportPayload] = useState<string>('');

  const shellRef = useRef<IfcOpenShell | null>(null);
  const fileRef = useRef<IfcFile | null>(null);
  const lastModelRef = useRef<LoadedModel | null>(null);
  const operationAbortRef = useRef<AbortController | null>(null);

  const beginOperation = useCallback(() => {
    operationAbortRef.current?.abort();
    const controller = new AbortController();
    operationAbortRef.current = controller;
    return controller;
  }, []);

  const closeActiveFile = useCallback(() => {
    const file = fileRef.current;
    fileRef.current = null;
    file?.dispose();
  }, []);

  // Boot the WASM runtime and pre-load the default schema + kernel pipeline.
  useEffect(() => {
    let cancelled = false;

    const boot = async () => {
      try {
        const shell = await init();
        if (cancelled) {
          shell.dispose();
          return;
        }
        shellRef.current = shell;
        setPlugins(await shell.loadedPlugins());

        setBusy('Loading IFC4 schema + OCCT kernel...');
        await shell.loadPlugin('schema', 'ifc4');
        await shell.loadPlugin('kernel', 'opencascade');
        await shell.loadPlugin('mapping', 'ifc4');
        if (cancelled) return;
        setPlugins(await shell.loadedPlugins());
        setActiveKernel('opencascade');

        setRuntimeState('ready');
        setBusy(null);
      } catch (error) {
        if (cancelled) return;
        setRuntimeState('error');
        setRuntimeError(error instanceof Error ? error.message : String(error));
        setBusy(null);
      }
    };

    void boot();

    return () => {
      cancelled = true;
      operationAbortRef.current?.abort();
      operationAbortRef.current = null;
      closeActiveFile();
      const shell = shellRef.current;
      shellRef.current = null;
      shell?.dispose();
    };
  }, [closeActiveFile]);

  /**
   * Ensure the schema + kernel + mapping plugins for the requested
   * combination are loaded. The kernel plugin is (re)loaded on every call
   * so switching kernels mid-session is cheap.
   */
  const ensurePipeline = useCallback(
    async (schemaId: string, kernelId: KernelId) => {
      const shell = shellRef.current;
      if (!shell) return;
      setBusy(`Loading ${schemaId} + ${kernelId} plugins...`);
      setRuntimeError(null);
      try {
        await shell.loadPlugin('schema', schemaId);
        await shell.loadPlugin('kernel', kernelId);
        await shell.loadPlugin('mapping', schemaId);
        setPlugins(await shell.loadedPlugins());
        setActiveKernel(kernelId);
      } catch (error) {
        setRuntimeError(error instanceof Error ? error.message : String(error));
      } finally {
        setBusy(null);
      }
    },
    [],
  );

  const onSchemaChange = useCallback(
    (ev: ChangeEvent<HTMLSelectElement>) => {
      const id = ev.target.value;
      setSelectedSchema(id);
      void ensurePipeline(id, selectedKernel);
    },
    [ensurePipeline, selectedKernel],
  );

  const loadIfcBytes = useCallback(
    async (bytes: Uint8Array, name: string, schemaId: string, kernelId: KernelId) => {
      const shell = shellRef.current;
      if (!shell || runtimeState !== 'ready') return;
      const operation = beginOperation();
      const { signal } = operation;
      let file: IfcFile | null = null;
      let settings: GeomSettings | null = null;
      let iter: ReturnType<IfcFile['meshes']> | null = null;

      setBusy('Parsing IFC...');
      setRuntimeError(null);
      setExportText('');
      setExportPayload('');
      setSelectedId(null);
      setMeshes([]);
      closeActiveFile();

      try {
        file = await IfcFile.open(shell, bytes, name, { signal });
        throwIfCancelled(signal);
        fileRef.current = file;
        lastModelRef.current = { bytes, name, schema: schemaId };

        settings = new GeomSettings(shell);
        await settings.set('weld-vertices', weldVertices);
        await settings.set('mesher-linear-deflection', linearDeflection);
        throwIfCancelled(signal);

        let bounds: { min: [number, number, number]; max: [number, number, number] } | null = null;
        try {
          bounds = await computeModelBounds(file, settings, kernelId);
        } catch (error) {
          if (isCancelled(error)) throw error;
          bounds = null;
        }
        throwIfCancelled(signal);

        iter = file.meshes(settings, { kernel: kernelId, numThreads: 1 });
        if (!(await iter.initialize())) {
          throw new Error(`Failed to initialize ${kernelId} geometry iterator`);
        }

        setBusy(`Triangulating geometry (${kernelId})...`);

        const collected: ViewMesh[] = [];
        let vertexCount = 0;
        let faceCount = 0;
        const typeCountMap = new Map<string, number>();
        const result = await iter.collect({
          limit: maxMeshes,
          skipEmpty: true,
          signal,
          onProgress: (progress) => {
            setBusy(`Triangulating geometry... ${Math.round((progress.ratio ?? 0) * 100)}% (${progress.meshes} meshes)`);
          },
        });
        throwIfCancelled(signal);

        for (const mesh of result.meshes) {
          const vertices = mesh.vertices;
          const faces = mesh.faces;
          const normals = mesh.normals;

          collected.push({
            id: mesh.id,
            guid: mesh.guid,
            type: mesh.type,
            name: mesh.name,
            vertices,
            faces,
            normals,
            transform: mesh.transform,
            color: meshColor(mesh.type, mesh.materialIds, mesh.colors),
          });

          vertexCount += vertices.length / 3;
          faceCount += Math.floor(faces.length / 3);
          typeCountMap.set(mesh.type, (typeCountMap.get(mesh.type) ?? 0) + 1);
        }

        const topTypes = Array.from(typeCountMap.entries())
          .sort((a, b) => b[1] - a[1])
          .slice(0, 12)
          .map(([typeName, count]) => ({ name: typeName, count }));

        let entityCount = 0;
        try {
          entityCount = file.entityCount;
        } catch {
          entityCount = 0;
        }

        setModelName(name);
        setMeshes(collected);
        setModelStats({
          schema: file.schema,
          entityCount,
          meshCount: collected.length,
          vertexCount,
          faceCount,
          topTypes,
          bounds,
          truncated: result.truncated,
        });
        setModelRevision((x) => x + 1);
        setBusy(null);
      } catch (error) {
        if (fileRef.current === file) closeActiveFile();
        if (isCancelled(error)) return;
        if (operationAbortRef.current !== operation) return;
        const message = error instanceof Error ? error.message : String(error);
        setRuntimeError(message);
        setBusy(null);
        setModelName('No model loaded');
        setModelStats(null);
        setMeshes([]);
      } finally {
        iter?.dispose();
        settings?.dispose();
        if (operationAbortRef.current === operation) operationAbortRef.current = null;
      }
    },
    [beginOperation, closeActiveFile, linearDeflection, maxMeshes, runtimeState, weldVertices],
  );

  const loadSample = useCallback(
    async (path: string, schemaId: string, label: string) => {
      try {
        setBusy(`Loading sample ${label}...`);
        await ensurePipeline(schemaId, selectedKernel);
        setSelectedSchema(schemaId);
        const response = await fetch(path);
        if (!response.ok) throw new Error(`Failed to load sample: ${response.status}`);
        const bytes = new Uint8Array(await response.arrayBuffer());
        await loadIfcBytes(bytes, label, schemaId, selectedKernel);
      } catch (error) {
        setBusy(null);
        setRuntimeError(error instanceof Error ? error.message : String(error));
      }
    },
    [ensurePipeline, loadIfcBytes, selectedKernel],
  );

  const onInputFile = useCallback(
    async (ev: ChangeEvent<HTMLInputElement>) => {
      const file = ev.target.files?.[0];
      if (!file) return;
      const bytes = new Uint8Array(await file.arrayBuffer());
      await loadIfcBytes(bytes, file.name, selectedSchema, selectedKernel);
      ev.target.value = '';
    },
    [loadIfcBytes, selectedKernel, selectedSchema],
  );

  /**
   * Switch the active kernel and, if a model is already loaded, re-run
   * geometry iteration against the cached bytes so the viewport reflects
   * the new kernel immediately.
   */
  const onKernelChange = useCallback(
    async (ev: ChangeEvent<HTMLSelectElement>) => {
      const id = ev.target.value as KernelId;
      setSelectedKernel(id);
      const cached = lastModelRef.current;
      await ensurePipeline(selectedSchema, id);
      if (cached) {
        await loadIfcBytes(cached.bytes, cached.name, cached.schema, id);
      }
    },
    [ensurePipeline, loadIfcBytes, selectedSchema],
  );

  const runExport = useCallback(async () => {
    const shell = shellRef.current;
    const file = fileRef.current;
    if (!shell || !file) return;
    const operation = beginOperation();

    setRuntimeError(null);
    setBusy(`Loading ${exportFormat.toUpperCase()} serializer...`);

    let settings: GeomSettings | null = null;

    try {
      settings = new GeomSettings(shell);
      await settings.set('weld-vertices', weldVertices);
      await settings.set('mesher-linear-deflection', linearDeflection);
      throwIfCancelled(operation.signal);
      await shell.loadPlugin('geometry_serializer', exportFormat);
      setPlugins(await shell.loadedPlugins());

      setBusy(`Exporting ${exportFormat.toUpperCase()}...`);
      const result = await exportToBuffer(shell, file, settings, exportFormat, {
        kernel: selectedKernel,
        numThreads: 1,
        signal: operation.signal,
        onProgress: (progress) => {
          if (progress.message) {
            setBusy(progress.current != null
              ? `${progress.message} (${progress.current} elements)`
              : progress.message);
          }
        },
      });
      throwIfCancelled(operation.signal);

      if (!result) {
        setExportText('');
        setExportPayload('');
        setRuntimeError('No geometry produced for export.');
        return;
      }

      const text = result.secondary
        ? `${result.primary}\n\n# --- Secondary (MTL) ---\n\n${result.secondary}`
        : result.primary;

      setExportPayload(text);
      setExportText(text.slice(0, 80000));
    } catch (error) {
      if (isCancelled(error)) return;
      if (operationAbortRef.current !== operation) return;
      const message = error instanceof Error ? error.message : String(error);
      setRuntimeError(message);
    } finally {
      settings?.dispose();
      if (operationAbortRef.current === operation) {
        operationAbortRef.current = null;
        setBusy(null);
      }
    }
  }, [beginOperation, exportFormat, linearDeflection, selectedKernel, weldVertices]);

  const downloadExport = useCallback(() => {
    if (!exportPayload) return;
    const blob = new Blob([exportPayload], { type: 'text/plain;charset=utf-8' });
    const url = URL.createObjectURL(blob);
    const a = document.createElement('a');
    a.href = url;
    a.download = `${modelName.replace(/\.[^.]+$/, '') || 'ifcopenshell'}.${exportFormat}`;
    a.click();
    URL.revokeObjectURL(url);
  }, [exportFormat, exportPayload, modelName]);

  useEffect(() => {
    let cancelled = false;
    const loadSelection = async () => {
      const file = fileRef.current;
      if (!file || selectedId == null) {
        setSelectedInfo(null);
        return;
      }
      try {
        const info = await file.inspect(selectedId);
        if (!cancelled) setSelectedInfo(info);
      } catch (error) {
        if (!cancelled) {
          setSelectedInfo({
            id: selectedId,
            type: '?',
            guid: null,
            attributes: [{ name: 'error', value: error instanceof Error ? error.message : String(error) }],
          });
        }
      }
    };
    void loadSelection();
    return () => {
      cancelled = true;
    };
    // Re-derive whenever the model (or selection) changes.
  }, [selectedId, modelRevision]);

  const runtimeReady = runtimeState === 'ready';

  return (
    <div className="app">
      <header className="app-header">
        <div className="app-header-brand">
          <div className="app-logo" aria-hidden="true">◇</div>
          <div>
            <h1>@ifcopenshell-js/web</h1>
            <p className="app-tagline">In-browser IFC toolkit · OCCT · Manifold · CGAL</p>
          </div>
        </div>
        <div className="app-header-status">
          <span className={`status status-${runtimeState}`}>
            <span className="status-dot" />
            {runtimeState}
            {activeKernel ? <span className="status-kernel">· {activeKernel}</span> : null}
          </span>
          {busy ? <span className="status status-busy">{busy}</span> : null}
        </div>
      </header>

      <main className="app-body">
        <aside className="sidebar sidebar-left">
          <section className="card">
            <h2>Pipeline</h2>
            <label className="field">
              <span className="field-label">Schema</span>
              <select value={selectedSchema} onChange={onSchemaChange} disabled={!runtimeReady}>
                {SCHEMAS.map((s) => (
                  <option key={s.id} value={s.id}>{s.label}</option>
                ))}
              </select>
            </label>
            <label className="field">
              <span className="field-label">Kernel</span>
              <select value={selectedKernel} onChange={onKernelChange} disabled={!runtimeReady}>
                {KERNELS.map((k) => (
                  <option key={k.id} value={k.id}>{k.label}</option>
                ))}
              </select>
            </label>
            <p className="field-hint">{KERNELS.find((k) => k.id === selectedKernel)?.hint ?? ''}</p>
          </section>

          <section className="card">
            <h2>Load Model</h2>
            <div className="button-stack">
              {SAMPLE_MODELS.map((sample) => (
                <button
                  key={sample.path}
                  className="btn btn-primary"
                  onClick={() => void loadSample(sample.path, sample.schema, sample.label)}
                  disabled={!runtimeReady}
                >
                  {sample.label}
                </button>
              ))}
            </div>
            <label className="upload">
              <span className="field-label">
                Upload .ifc
                <span className="field-meta">({SCHEMAS.find((s) => s.id === selectedSchema)?.label})</span>
              </span>
              <input type="file" accept=".ifc" onChange={onInputFile} disabled={!runtimeReady} />
            </label>
          </section>

          <section className="card">
            <h2>Geometry</h2>
            <label className="field">
              <span className="field-label">Linear deflection · {linearDeflection.toFixed(3)}</span>
              <input
                type="range"
                min={0.002}
                max={0.08}
                step={0.002}
                value={linearDeflection}
                onChange={(e) => setLinearDeflection(Number(e.target.value))}
              />
            </label>
            <label className="field">
              <span className="field-label">Max meshes · {maxMeshes}</span>
              <input
                type="range"
                min={50}
                max={1500}
                step={50}
                value={maxMeshes}
                onChange={(e) => setMaxMeshes(Number(e.target.value))}
              />
            </label>
            <label className="field field-toggle">
              <input
                type="checkbox"
                checked={weldVertices}
                onChange={(e) => setWeldVertices(e.target.checked)}
              />
              <span>Weld vertices</span>
            </label>
          </section>

          <section className="card">
            <h2>Export</h2>
            <label className="field">
              <span className="field-label">Format</span>
              <select
                value={exportFormat}
                onChange={(e) => setExportFormat(e.target.value as ExportFormat)}
              >
                {EXPORT_FORMATS.map((f) => (
                  <option key={f.id} value={f.id}>{f.label} · {f.note}</option>
                ))}
              </select>
            </label>
            <div className="button-row">
              <button
                className="btn btn-primary"
                onClick={() => void runExport()}
                disabled={!fileRef.current || !runtimeReady}
              >
                Generate
              </button>
              <button
                className="btn"
                onClick={downloadExport}
                disabled={!exportPayload}
              >
                Download
              </button>
            </div>
            {exportText ? (
              <pre className="code-preview">{exportText}</pre>
            ) : (
              <p className="field-hint">Generate to preview output.</p>
            )}
          </section>

          <section className="card">
            <h2>Loaded Plugins</h2>
            {plugins.length ? (
              <div className="chips">
                {plugins.map((p) => (
                  <span key={p} className="chip">{p}</span>
                ))}
              </div>
            ) : (
              <p className="field-hint">No plugins loaded.</p>
            )}
          </section>
        </aside>

        <section className="viewport-panel">
          <div className="viewport-head">
            <h2>{modelName}</h2>
            <p className="field-hint">Click a mesh to inspect its entity metadata.</p>
          </div>
          <ModelViewport meshes={meshes} selectedId={selectedId} onSelect={setSelectedId} />
        </section>

        <aside className="sidebar sidebar-right">
          <section className="card">
            <h2>Model</h2>
            {modelStats ? (
              <ul className="stats-list">
                <li><span>Schema</span><strong>{modelStats.schema}</strong></li>
                <li><span>Entities</span><strong>{modelStats.entityCount.toLocaleString()}</strong></li>
                <li><span>Meshes</span><strong>{modelStats.meshCount.toLocaleString()}</strong></li>
                <li><span>Vertices</span><strong>{Math.round(modelStats.vertexCount).toLocaleString()}</strong></li>
                <li><span>Faces</span><strong>{modelStats.faceCount.toLocaleString()}</strong></li>
                <li><span>Truncated</span><strong>{modelStats.truncated ? 'Yes' : 'No'}</strong></li>
                {modelStats.bounds ? (
                  <>
                    <li>
                      <span>Bounds min</span>
                      <strong>{modelStats.bounds.min.map((v) => v.toFixed(2)).join(', ')}</strong>
                    </li>
                    <li>
                      <span>Bounds max</span>
                      <strong>{modelStats.bounds.max.map((v) => v.toFixed(2)).join(', ')}</strong>
                    </li>
                  </>
                ) : null}
              </ul>
            ) : (
              <p className="field-hint">Load a model to see stats.</p>
            )}
            {modelStats?.topTypes?.length ? (
              <>
                <h3>Top Types</h3>
                <div className="chips">
                  {modelStats.topTypes.map((it) => (
                    <span key={it.name} className="chip">{it.name} · {it.count}</span>
                  ))}
                </div>
              </>
            ) : null}
          </section>

          <section className="card">
            <h2>Selection</h2>
            {selectedInfo ? (
              <>
                <ul className="stats-list">
                  <li><span>ID</span><strong>#{selectedInfo.id}</strong></li>
                  <li><span>Type</span><strong>{selectedInfo.type}</strong></li>
                  <li><span>GUID</span><strong>{selectedInfo.guid ?? '—'}</strong></li>
                </ul>
                <h3>Attributes</h3>
                <ul className="attrs">
                  {selectedInfo.attributes.map((a) => (
                    <li key={a.name}>
                      <span>{a.name}</span>
                      <strong>{a.value}</strong>
                    </li>
                  ))}
                </ul>
              </>
            ) : (
              <p className="field-hint">Select an element in the viewport.</p>
            )}
          </section>
        </aside>
      </main>

      {runtimeError ? <div className="app-error" role="alert">{runtimeError}</div> : null}
    </div>
  );
}
