import { app, BrowserWindow, Menu, ipcMain } from 'electron';
import path from 'path';
import fs from 'fs';
import { examplePreflopRanges, hands } from '../renderer/components/utils.js';
import { fileURLToPath } from 'url';
const __filename = fileURLToPath(import.meta.url);
const __dirname = path.dirname(__filename);

function getPreflopRangesRoot() {
  return path.join(app.getPath('userData'), 'preflop-ranges')
}

function ensurePreflopRangesDir() {
  const dir = getPreflopRangesRoot()
  if (!fs.existsSync(dir)) fs.mkdirSync(dir, { recursive: true })
}

function buildPreflopRangeTree(dir) {
  const entries = fs.readdirSync(dir, { withFileTypes: true })
  return entries.map((entry) => {
    const fullPath = path.join(dir, entry.name)
    if (entry.isDirectory()) {
      return {
        type: 'folder',
        name: entry.name,
        children: buildPreflopRangeTree(fullPath),
      }
    }
    const filePath = path.relative(getPreflopRangesRoot(), fullPath)
    return {
      type: 'file',
      name: entry.name,
      path: filePath,
    }
  })
}

function loadPreflopRange(relativePath) {
  const fullPath = path.join(getPreflopRangesRoot(), relativePath)
  const raw = fs.readFileSync(fullPath, 'utf-8')
  return JSON.parse(raw)['data']
}

function createExampleRanges() {
  for (const range of examplePreflopRanges) {
    const fn = path.join(getPreflopRangesRoot(), range['path'])


    // const fn2 = fn + ".fold.json" 
    // let range2 = range
    // let fold = {}
    // for (const row of hands) {
    //   for (const hand of row) {
    //     fold[hand] = 1.0
    //     for (const [_, s] of Object.entries(range.data.strategy)) {
    //       fold[hand] -= s[hand]
    //     }
    //   }
    // }
    // range2.data.strategy['fold'] = fold
    // fs.writeFileSync(fn2, JSON.stringify(range2), null, 2)


    if (fs.existsSync(fn))
      continue
    fs.mkdirSync(path.dirname(fn), { recursive: true })
    fs.writeFileSync(fn, JSON.stringify(range), null, 2)
  }
}

function createWindow() {
  const win = new BrowserWindow({
    width: 1920,
    height: 1080,
    webPreferences: {
      preload: path.join(__dirname, 'preload.js'),
      contextIsolation: true,
      nodeIntegration: false,
    },
  })

  Menu.setApplicationMenu(null)
  // win.webContents.openDevTools()
  win.loadFile(path.join(__dirname, '../dist/index.html'))
}

app.whenReady().then(() => {
  ensurePreflopRangesDir()
  createExampleRanges()
  createWindow()

  ipcMain.handle('load-preflop-range', (_, p) => loadPreflopRange(p))
  ipcMain.handle('get-preflop-range-tree', () => buildPreflopRangeTree(getPreflopRangesRoot()))
})

app.on('window-all-closed', () => {
  if (process.platform !== 'darwin') app.quit()
})

app.on('activate', () => {
  if (BrowserWindow.getAllWindows().length === 0) createWindow()
})
