const { contextBridge, ipcRenderer } = require('electron')

contextBridge.exposeInMainWorld('pokerAPI', {
  loadPreflopRange: (path) => ipcRenderer.invoke('load-preflop-range', path),
  getPreflopRangeTree: () => ipcRenderer.invoke('get-preflop-range-tree'),
})

console.log('Preload script loaded!')
