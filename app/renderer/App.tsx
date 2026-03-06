import React, { useEffect, useState } from 'react'
import { RangeExplorer } from './components/RangeExplorer'
import { RangeViewer } from './components/RangeViewer'
import { Range, Strategy, TreeNode, FileNode } from './types'

export const App: React.FC = () => {
  const [tree, setTree] = useState<TreeNode[]>([])
  const [selectedPath, setSelectedPath] = useState<string>()
  const [range, setRange] = useState<Range | null>(null)
  const [strategy, setStrategy] = useState<Strategy>()

  useEffect(() => {
    window.pokerAPI.getPreflopRangeTree().then((data) => {
      console.log("TREE:", data)
      setTree(data)
    })
  }, [])

  const openRange = async (file: FileNode) => {
    const data = await window.pokerAPI.loadPreflopRange(file.path)

    setSelectedPath(file.path)
    setRange(new Map(Object.entries(data.range)))
    setStrategy(new Map(
      Object.entries(data.strategy).map(([k, v]) => [
        k,
        new Map(Object.entries(v as Record<string, number>))
      ])
    ))
  }

  return (
    <div style={{ display: 'flex', height: '100vh' }}>
      <div style={{ flex: 1, padding: 20 }}>
        {(range && strategy) ? (
          <RangeViewer
            range={range}
            strategy={strategy}
            width={1200}
            height={800}
          />
        ) : (
          <div style={{ color: '#888' }}>
            Select a range
          </div>
        )}
      </div>
      
      <RangeExplorer
        tree={tree}
        selectedPath={selectedPath}
        onOpen={openRange}
      />
    </div>
  )
}
