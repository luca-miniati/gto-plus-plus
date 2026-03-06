import { defineConfig } from 'vite'
import react from '@vitejs/plugin-react'
import path from 'path'

export default defineConfig({
  root: 'renderer',
  plugins: [react()],
  base: './',
  build: {
    outDir: '../dist',
    emptyOutDir: true,
    rollupOptions: { input: path.resolve(__dirname, 'renderer/index.html') },
  },
})
