import React from 'react'
import { CardProps } from '../types'

export const Card: React.FC<CardProps> = ({
  rank = '',
  suit = 'none',
  width = 40,
  height = 60,
}) => {
  const rankToColors = new Map();
  rankToColors.set('c', '#00A13B')
  rankToColors.set('d', '#2E78E3')
  rankToColors.set('h', '#EB3324')
  rankToColors.set('s', '#181818')
  rankToColors.set('none', '#8E403A')

  return (
    <div style={{
      width: width,
      height: height,
      borderRadius: 5,
      backgroundColor: rankToColors.get(suit),
      display: 'flex',
      justifyContent: 'center',
      alignItems: 'center',
      boxSizing: 'border-box',
      border: (suit == 'none' ? '4px solid #D4D4D4' : 'none')
    }}>
      <p style={{ fontSize: 28 }}>{rank.toUpperCase()}</p>
    </div>
  )
}