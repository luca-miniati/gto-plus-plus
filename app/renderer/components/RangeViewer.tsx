import React from 'react'

import { hands, checkFoldColor, betColors } from './utils'
import { Hand, Range, Strategy, RangeViewerProps } from '../types'

export const RangeViewer: React.FC<RangeViewerProps> = ({
  range,
  strategy,
  width = 650,
  height = 780,
}) => {
  const numCols = 13
  const numRows = 13

  const cellWidth = width / numCols
  const cellHeight = height / numRows

  let actionColors = {}
  let numBets = 0
  for (const action of strategy.keys()) {
    if (action.includes('check') || action.includes('fold'))
      actionColors[action] = checkFoldColor
    else if (action.includes('call'))
      actionColors[action] = callColor
    else {
      actionColors[action] = betColors[numBets]
      numBets += 1
    }
  }

  return (
    <div style={{
      display: 'inline-grid',
      gridTemplateColumns: `repeat(${numCols}, ${cellWidth}px)`,
      gridTemplateRows: `repeat(${numRows}, ${cellHeight}px)`,
      border: '0.25px solid #2d2d2d'
    }}>
      {hands.flat().map(hand => {
        const weight = range.get(hand) || 0
        let totalFreq = 0
        for (const [a, s] of strategy)
          totalFreq += weight * (s.get(hand) || 0)
        const vFill = weight
        let offset = 0

        return (
          <div
            key={hand}
            style={{
              overflow: 'hidden',
              width: cellWidth,
              height: cellHeight,
              border: '0.25px solid #2d2d2d',
              boxSizing: 'border-box',
              position: 'relative',
              backgroundColor: '#252526',
            }}
          >
          <div
            style={{
              position: 'absolute',
              bottom: 0,
              left: 0,
              width: '100%',
              height: `${vFill * 100}%`,
            }}
            >
            {strategy.keys().map((action, i) => {
              const freq = strategy.get(action)?.get(hand) || 0
              const freqPct = freq * 100

              const div = (
                <div
                key={action}
                style={{
                  position: 'absolute',
                  top: 0,
                  left: `${offset}%`,
                  width: `${freqPct}%`,
                  height: '100%',
                  backgroundColor: actionColors[action],
                }}
                />
              )

              offset += freqPct
              return div
            })}
            </div>
            <div
              style={{
                position: 'absolute',
                top: 0,
                left: 0,
                fontSize: Math.min(Number(cellWidth) * 0.35, Number(cellHeight) * 0.35),
                padding: '0.3rem',
                width: '100%',
                height: '100%',
                pointerEvents: 'none',
                color: totalFreq > 0 ? '#fff' : '#555',
                textShadow: totalFreq > 0
                  ? `
                  0 0 4px rgba(0,0,0,0.7),
                  1px 1px 4px rgba(0,0,0,0.3),
                  -1px 1px 4px rgba(0,0,0,0.3),
                  1px -1px 4px rgba(0,0,0,0.3),
                  -1px -1px 4px rgba(0,0,0,0.3)
                  `
                    : 'none',
              }}
              >
              {hand}
            </div>
          </div>
        )
      })}
    </div>
  )
}
