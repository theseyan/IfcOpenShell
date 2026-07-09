import { useEffect, useMemo, useRef } from 'react';
import * as THREE from 'three';
import { OrbitControls } from 'three/examples/jsm/controls/OrbitControls.js';

export type ViewMesh = {
  id: number;
  guid: string;
  type: string;
  name: string;
  vertices: Float32Array;
  faces: Uint32Array;
  normals: Float32Array | null;
  transform: Float64Array;
  color: [number, number, number];
};

type Props = {
  meshes: ViewMesh[];
  selectedId: number | null;
  onSelect: (id: number | null) => void;
};

function disposeObjectTree(root: THREE.Object3D): void {
  root.traverse((obj) => {
    const mesh = obj as THREE.Mesh;
    if (!mesh.isMesh) return;
    mesh.geometry.dispose();
    const mat = mesh.material;
    if (Array.isArray(mat)) {
      for (const m of mat) m.dispose();
    } else {
      mat.dispose();
    }
  });
}

export default function ModelViewport({ meshes, selectedId, onSelect }: Props) {
  const hostRef = useRef<HTMLDivElement | null>(null);
  const rendererRef = useRef<THREE.WebGLRenderer | null>(null);
  const sceneRef = useRef<THREE.Scene | null>(null);
  const cameraRef = useRef<THREE.PerspectiveCamera | null>(null);
  const controlsRef = useRef<OrbitControls | null>(null);
  const contentRef = useRef<THREE.Group | null>(null);
  const gridRef = useRef<THREE.GridHelper | null>(null);
  const idToMeshesRef = useRef<Map<number, THREE.Mesh[]>>(new Map());

  const raycaster = useMemo(() => new THREE.Raycaster(), []);
  const pointer = useMemo(() => new THREE.Vector2(), []);

  useEffect(() => {
    if (!hostRef.current) return;

    const host = hostRef.current;
    const renderer = new THREE.WebGLRenderer({ antialias: true, alpha: true });
    renderer.setPixelRatio(Math.min(window.devicePixelRatio, 2));
    renderer.setSize(host.clientWidth, host.clientHeight);
    renderer.outputColorSpace = THREE.SRGBColorSpace;
    host.appendChild(renderer.domElement);
    rendererRef.current = renderer;

    const scene = new THREE.Scene();
    scene.background = new THREE.Color('#f8f9fa');
    sceneRef.current = scene;

    const camera = new THREE.PerspectiveCamera(48, host.clientWidth / host.clientHeight, 0.1, 100000);
    camera.up.set(0, 0, 1);
    camera.position.set(12, -14, 8);
    cameraRef.current = camera;

    const controls = new OrbitControls(camera, renderer.domElement);
    controls.enableDamping = true;
    controls.dampingFactor = 0.08;
    controls.target.set(0, 0, 0);
    controlsRef.current = controls;

    const hemi = new THREE.HemisphereLight(0xcde6ff, 0xe7d4be, 1.0);
    scene.add(hemi);

    const key = new THREE.DirectionalLight(0xffffff, 1.3);
    key.position.set(12, 14, 10);
    scene.add(key);

    const fill = new THREE.DirectionalLight(0x88a7ff, 0.6);
    fill.position.set(-8, 4, -10);
    scene.add(fill);

    const grid = new THREE.GridHelper(100, 60, 0xb7bfd0, 0xdfe3ee);
    grid.rotation.x = Math.PI / 2;
    grid.position.z = -0.01;
    scene.add(grid);
    gridRef.current = grid;

    const content = new THREE.Group();
    scene.add(content);
    contentRef.current = content;

    let dragging = false;
    let moved = false;
    let downX = 0;
    let downY = 0;

    const onDown = (ev: PointerEvent) => {
      dragging = true;
      moved = false;
      downX = ev.clientX;
      downY = ev.clientY;
    };

    const onMove = (ev: PointerEvent) => {
      if (!dragging) return;
      if (Math.abs(ev.clientX - downX) > 4 || Math.abs(ev.clientY - downY) > 4) {
        moved = true;
      }
    };

    const onUp = (ev: PointerEvent) => {
      if (!dragging) return;
      dragging = false;
      if (moved) return;
      if (!cameraRef.current || !contentRef.current || !rendererRef.current) return;

      const rect = rendererRef.current.domElement.getBoundingClientRect();
      pointer.x = ((ev.clientX - rect.left) / rect.width) * 2 - 1;
      pointer.y = -((ev.clientY - rect.top) / rect.height) * 2 + 1;
      raycaster.setFromCamera(pointer, cameraRef.current);
      const hits = raycaster.intersectObjects(contentRef.current.children, true);
      if (hits.length === 0) {
        onSelect(null);
        return;
      }
      const hit = hits[0]!.object as THREE.Mesh;
      const hitId = typeof hit.userData.ifcId === 'number' ? hit.userData.ifcId : null;
      onSelect(hitId);
    };

    renderer.domElement.addEventListener('pointerdown', onDown);
    renderer.domElement.addEventListener('pointermove', onMove);
    renderer.domElement.addEventListener('pointerup', onUp);

    const onResize = () => {
      if (!hostRef.current || !cameraRef.current || !rendererRef.current) return;
      const width = hostRef.current.clientWidth;
      const height = hostRef.current.clientHeight;
      cameraRef.current.aspect = width / height;
      cameraRef.current.updateProjectionMatrix();
      rendererRef.current.setSize(width, height);
    };

    window.addEventListener('resize', onResize);

    let raf = 0;
    const tick = () => {
      raf = requestAnimationFrame(tick);
      controls.update();
      renderer.render(scene, camera);
    };
    tick();

    return () => {
      cancelAnimationFrame(raf);
      window.removeEventListener('resize', onResize);
      renderer.domElement.removeEventListener('pointerdown', onDown);
      renderer.domElement.removeEventListener('pointermove', onMove);
      renderer.domElement.removeEventListener('pointerup', onUp);
      if (contentRef.current) disposeObjectTree(contentRef.current);
      controls.dispose();
      renderer.dispose();
      host.removeChild(renderer.domElement);
      idToMeshesRef.current.clear();
      contentRef.current = null;
      gridRef.current = null;
      sceneRef.current = null;
      cameraRef.current = null;
      rendererRef.current = null;
      controlsRef.current = null;
    };
  }, [onSelect, pointer, raycaster]);

  useEffect(() => {
    const scene = sceneRef.current;
    const camera = cameraRef.current;
    const controls = controlsRef.current;
    const content = contentRef.current;
    const grid = gridRef.current;
    if (!scene || !camera || !controls || !content) return;

    disposeObjectTree(content);
    content.clear();
    content.position.set(0, 0, 0);
    idToMeshesRef.current.clear();

    for (const item of meshes) {
      if (item.faces.length === 0 || item.vertices.length === 0) continue;

      const geometry = new THREE.BufferGeometry();
      geometry.setAttribute('position', new THREE.BufferAttribute(item.vertices, 3));
      geometry.setIndex(new THREE.BufferAttribute(item.faces, 1));
      if (item.normals && item.normals.length === item.vertices.length) {
        geometry.setAttribute('normal', new THREE.BufferAttribute(item.normals, 3));
      } else {
        geometry.computeVertexNormals();
      }

      const material = new THREE.MeshStandardMaterial({
        color: new THREE.Color(item.color[0], item.color[1], item.color[2]),
        roughness: 0.74,
        metalness: 0.06,
      });

      const mesh = new THREE.Mesh(geometry, material);
      // The wrapper produces a 4x4 column-major matrix as Float64Array;
      // Matrix4.fromArray accepts any ArrayLike<number>.
      mesh.matrix.fromArray(item.transform);
      mesh.matrixAutoUpdate = false;
      mesh.userData.ifcId = item.id;
      mesh.userData.ifcGuid = item.guid;
      mesh.userData.ifcType = item.type;
      mesh.userData.ifcName = item.name;
      content.add(mesh);

      const arr = idToMeshesRef.current.get(item.id) ?? [];
      arr.push(mesh);
      idToMeshesRef.current.set(item.id, arr);
    }

    const box = new THREE.Box3().setFromObject(content);
    if (!box.isEmpty()) {
      const center = box.getCenter(new THREE.Vector3());
      const size = box.getSize(new THREE.Vector3());
      const maxDim = Math.max(size.x, size.y, size.z);
      const fov = (camera.fov * Math.PI) / 180;
      const distance = Math.max(5, (maxDim / (2 * Math.tan(fov / 2))) * 1.35);

      // IFC files are Z-up and often georeferenced with very large X/Y
      // offsets. Recenter the rendered scene so WebGL receives small
      // coordinates while metadata and displayed model bounds stay in IFC
      // world coordinates.
      content.position.copy(center).multiplyScalar(-1);

      const gridSize = Math.max(20, maxDim * 1.6);
      if (grid) {
        grid.scale.setScalar(gridSize / 100);
        grid.position.set(0, 0, -size.z / 2 - 0.01);
      }

      camera.near = Math.max(0.01, distance / 1000);
      camera.far = Math.max(1000, distance * 10);
      camera.position.set(distance, -distance, distance * 0.62);
      camera.updateProjectionMatrix();
      controls.target.set(0, 0, 0);
      controls.update();
    }
  }, [meshes]);

  useEffect(() => {
    const map = idToMeshesRef.current;
    for (const [, meshList] of map) {
      for (const mesh of meshList) {
        const mat = mesh.material as THREE.MeshStandardMaterial;
        mat.emissive.setHex(0x000000);
        mat.emissiveIntensity = 0;
      }
    }

    if (selectedId == null) return;
    const selectedMeshes = map.get(selectedId) ?? [];
    for (const mesh of selectedMeshes) {
      const mat = mesh.material as THREE.MeshStandardMaterial;
      mat.emissive.setHex(0x1a76ff);
      mat.emissiveIntensity = 0.42;
    }
  }, [selectedId]);

  return <div className="viewport" ref={hostRef} />;
}
