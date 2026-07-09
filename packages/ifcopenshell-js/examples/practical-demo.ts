
import { existsSync, readFileSync } from 'node:fs';
import { dirname, resolve } from 'node:path';
import { fileURLToPath } from 'node:url';
import { GeomSettings, IfcFile, init } from '@ifcopenshell-js/web';

const __dirname = dirname(fileURLToPath(import.meta.url));
const repo = resolve(__dirname, '..', '..', '..', '..');
const sample = resolve(repo, 'test', 'input', 'WallInstance_IFC4Add2.ifc');

const wasmFiles = [
  'ifcopenshell_api.mjs',
  'ifcopenshell_wasm.mjs',
  'ifcopenshell_wasm.wasm',
  'ifcopenshell_plugins.json',
] as const;

function wasmRoot(): string {
  const dirs = [
    process.env.IFCOPENSHELL_WASM_DIR,
    resolve(repo, 'build-wasm', 'ifcwrap', 'wasm'),
    resolve(repo, 'build', 'wasm-native', 'ifcopenshell', 'full', 'ifcwrap', 'wasm'),
    resolve(repo, 'packages', 'ifcopenshell-wasm', 'wasm'),
  ].filter((dir): dir is string => Boolean(dir));

  const root = dirs.find((dir) => wasmFiles.every((file) => existsSync(resolve(dir, file))));
  if (!root) throw new Error('No WASM artifacts found. Build or stage @ifcopenshell-js/wasm first.');
  return root;
}

function requireFile(path: string): void {
  if (!existsSync(path)) throw new Error(`File not found: ${path}`);
}

function heading(text: string): void {
  console.log(`\n${text}`);
  console.log('-'.repeat(text.length));
}

function label(entity: { id: number; type: string; get(name: string): unknown }): string {
  const name = entity.get('Name');
  return `#${entity.id} ${entity.type}${typeof name === 'string' && name ? ` "${name}"` : ''}`;
}

async function main(): Promise<void> {
  requireFile(sample);

  const shell = await init({ wasmRoot: wasmRoot() });

  await shell.loadPlugin('schema', 'ifc4');
  await shell.loadPlugin('kernel', 'passthrough');
  await shell.loadPlugin('mapping', 'ifc4');

  heading('Parse Existing IFC');
  const model = await IfcFile.open(shell, readFileSync(sample), 'WallInstance_IFC4Add2.ifc');

  const walls = model.all('IfcWall');
  const firstWall = walls[0];
  if (!firstWall) throw new Error('Sample model contains no IfcWall entities');

  console.log({
    schema: model.schema,
    entities: model.entityCount,
    walls: walls.length,
    firstWall: label(firstWall),
    firstWallAttributes: firstWall.attributes().slice(0, 8),
  });

  heading('Geometry');
  const settings = new GeomSettings(shell);
  const meshes = model.meshes(settings, { kernel: 'passthrough' });

  await meshes.initialize();
  const geom = await meshes.collect({ limit: 1 });
  const mesh = geom.meshes[0];

  console.log({
    meshes: geom.meshes.length,
    truncated: geom.truncated,
    firstMesh: mesh
      ? {
          id: mesh.id,
          guid: mesh.guid,
          type: mesh.type,
          vertices: mesh.vertices.length,
          faces: mesh.faces.length,
          hasTriangles: mesh.vertices.length > 0 && mesh.faces.length > 0,
        }
      : null,
  });

  heading('Create and Edit IFC');
  const file = await IfcFile.create(shell, 'IFC4');

  const project = file.create('IfcProject', { name: 'Demo Project' });
  const site = file.create('IfcSite', { name: 'Demo Site' });
  const building = file.create('IfcBuilding', { name: 'Demo Building' });
  const storey = file.create('IfcBuildingStorey', { name: 'Level 01' });
  const wall = file.create('IfcWall', { name: 'Demo Wall' });

  shell.api.aggregate.assignObject(file, { relatingObject: project, products: [site] });
  shell.api.aggregate.assignObject(file, { relatingObject: site, products: [building] });
  shell.api.aggregate.assignObject(file, { relatingObject: building, products: [storey] });
  shell.api.spatial.assignContainer(file, { relatingStructure: storey, products: [wall] });

  const pset = shell.api.pset.addPset(file, { product: wall, name: 'Pset_Demo' });
  shell.api.pset.editPset(file, {
    pset,
    properties: {
      Reference: 'W-001',
      LoadBearing: true,
      FireRating: { type: 'IfcLabel', value: '2HR' },
      DemoFactors: [1.25, 2.5],
    },
    shouldPurge: true,
  });

  const selectedName = shell.api.selector.getElementValue(file, wall, 'Name');
  const note = shell.api.value.newString('created with ifcopenshell-js');
  const notes = shell.api.value.newList();
  if (!note || !notes) throw new Error('Failed to create value objects');
  shell.api.value.listAppend(notes, note);

  const children = shell.api.element.getParts(project).map(label);
  const container = shell.api.element.getContainer(wall, { directOnly: true });

  console.log({
    schema: file.schema,
    entities: file.entityCount,
    project: label(project),
    spatialContainer: label(container),
    projectChildren: children,
    wallName: selectedName?.value(),
    notes: notes.value(),
    pset: label(pset),
    spfBytes: file.text().length,
  });
}

await main();
