import React from 'react'

import { hands, checkFoldColor, callColor, betColors } from './utils'
import { Hand, Range, Strategy, RangeSummaryProps } from '../types'

export const RangeSummary: React.FC<RangeSummaryProps> = ({
  range,
  strategy,
  width = 650,
  height = 50,
}) => {
  let actions = strategy.keys().toArray()

  let actionToColor: Map<string, string> = new Map();
  let numBets = 0
  for (const action of actions) {
    if (action.includes('check') || action.includes('fold'))
      actionToColor.set(action, checkFoldColor)
    else if (action.includes('call'))
      actionToColor.set(action, callColor)
    else {
      actionToColor.set(action, betColors[numBets])
      numBets += 1
    }
  }

  let actionToCombos: Map<string, number> = new Map();
  let totalCombos = 0
  for (const action of actions) {
    actionToCombos.set(action, 0)
    for (const [hand, actionFreq] of (strategy.get(action) ?? [])) {
      let n
      if (hand[0] == hand[1])
        n = 6
      else if (hand[hand.length - 1] == 'o')
        n = 12
      else
        n = 4
      const freq = range.get(hand) ?? 0
      actionToCombos.set(action, (actionToCombos.get(action) ?? 0) + n * freq * actionFreq)
    }
  }
  for (const action of actions)
    totalCombos += actionToCombos.get(action) ?? 0
  console.log('combos:', actionToCombos)
  for (const action of actions)
    actionToCombos.set(action, (actionToCombos.get(action) ?? 0) / totalCombos)

  return (
    <div style={{
      width: width,
      height: height,
      display: 'flex',
    }}>
      {actions.map((action) => {
        const div = (
          <div style={{
            width: width / actions.length,
            backgroundColor: actionToColor.get(action),
            padding: 5,
            textShadow: `
              0 0 4px rgba(0,0,0,0.7),
              1px 1px 4px rgba(0,0,0,0.3),
              -1px 1px 4px rgba(0,0,0,0.3),
              1px -1px 4px rgba(0,0,0,0.3),
              -1px -1px 4px rgba(0,0,0,0.3)
              `,
            marginLeft: 2,
          }}>
            {action}: {Math.round(10000 * (actionToCombos.get(action) ?? 0.0)) / 100}%
          </div>
        )
        return div
      })}
    </div>
  )
}