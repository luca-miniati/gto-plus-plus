import React, { useEffect, useState } from 'react'
import { Card } from './components/Card'
import { RangeViewer } from './components/RangeViewer'

export const App: React.FC = () => {
  const ranks = '23456789TJQKA'
  const suits = 'cdhs'

  return (
    <div>
    <RangeViewer />
    </div>
    // <div style={{ height: '100vh' }}>
    //   <div style={{ display: 'flex' }}>
    //     {Object.entries(ranks).map((r) => {
    //       return (
    //         <div style={{ marginRight: 5 }}>
    //           <Card rank={r[1]} suit={'c'} />
    //         </div>
    //       )
    //     })}
    //   </div>
    //   <div style={{ display: 'flex' }}>
    //     {Object.entries(ranks).map((r) => {
    //       return (
    //         <div style={{ marginRight: 5 }}>
    //           <Card rank={r[1]} suit={'d'} />
    //         </div>
    //       )
    //     })}
    //   </div>
    //   <div style={{ display: 'flex' }}>
    //     {Object.entries(ranks).map((r) => {
    //       return (
    //         <div style={{ marginRight: 5 }}>
    //           <Card rank={r[1]} suit={'h'} />
    //         </div>
    //       )
    //     })}
    //   </div>
    //   <div style={{ display: 'flex' }}>
    //     {Object.entries(ranks).map((r) => {
    //       return (
    //         <div style={{ marginRight: 5 }}>
    //           <Card rank={r[1]} suit={'s'} />
    //         </div>
    //       )
    //     })}
    //   </div>
    //   <div style={{ display: 'flex' }}>
    //     <div style={{ marginRight: 5 }}>
    //       <Card />
    //     </div>
    //     <div style={{ marginRight: 5 }}>
    //       <Card />
    //     </div>
    //   </div>
    // </div>
  )
}
