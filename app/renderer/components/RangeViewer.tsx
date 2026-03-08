import React, { useEffect, useState } from 'react'
import { RangeExplorer } from './RangeExplorer'
import { RangeView } from './RangeView'
import { Range, Strategy, TreeNode, FileNode } from '../types'
import { RangeSummary } from './RangeSummary'
import { Card } from './Card'

export const RangeViewer: React.FC = () => {
  const [tree, setTree] = useState<TreeNode[]>([])
  const [selectedPath, setSelectedPath] = useState<string>()
  const [range, setRange] = useState<Range | null>(null)
  const [strategy, setStrategy] = useState<Strategy>()

  useEffect(() => {
    window.pokerAPI.getPreflopRangeTree().then((data) => {
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
          <div>
            <RangeView
              range={range}
              strategy={strategy}
            />
            <div style={{ height: 10 }}></div>
            <RangeSummary
              range={range}
              strategy={strategy}
            />
          </div>
        ) : (
          <div style={{ color: '#888' }}>
            Select a range
          </div>
        )}
      </div>

      <div style={{ padding: 20 }}>
        <RangeExplorer
          tree={tree}
          selectedPath={selectedPath}
          onOpen={openRange}
        />
      </div>
    </div>
  )
}
