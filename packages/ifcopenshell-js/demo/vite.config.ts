import { defineConfig } from 'vite';
import react from '@vitejs/plugin-react';
import { dirname, resolve } from 'node:path';
import { fileURLToPath } from 'node:url';


const __dirname = dirname(fileURLToPath(import.meta.url));
const REPO_ROOT = resolve(__dirname, '..', '..', '..');

export default defineConfig({
  plugins: [react()],
  resolve: {
    alias: {
      '@ifcopenshell-js/web': resolve(__dirname, '..', 'src', 'index.ts'),
    },
  },
  server: {
    fs: {
      allow: [__dirname, REPO_ROOT],
    },
  },
});
