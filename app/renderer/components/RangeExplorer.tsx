import React, { useState } from 'react';
import { TreeNode, FileNode } from '../types';

interface RangeExplorerProps {
  tree: TreeNode[];
  selectedPath?: string;
  onOpen: (file: FileNode) => void;
}

interface TreeNodeProps {
  node: TreeNode;
  selectedPath?: string;
  onOpen: (file: FileNode) => void;
}

const ord = ["utg","utg1","mp","co","btn","sb","bb"]

function rank(name: string) {
  const n = name.trim().toLowerCase()
  if (n === "rfi.json") return -1
  const idx = ord.findIndex(p => n.includes(p))
  return idx === -1 ? 999 : idx
}

function comp(u: TreeNode, v: TreeNode) {
  return rank(u.name) - rank(v.name)
}

const TreeNodeView: React.FC<TreeNodeProps> = ({
  node,
  selectedPath,
  onOpen,
}) => {
  const [open, setOpen] = useState(true);

  if (node.type === 'folder') {
    return (
      <div>
        <div
          style={{
            cursor: 'pointer',
            fontWeight: 'bold',
            userSelect: 'none',
            fontSize: 16,
          }}
          onClick={() => setOpen(!open)}
        >
          {open
            ?
              // 'v'
            (<svg xmlns="http://www.w3.org/2000/svg" fill="none" viewBox="0 0 24 24" stroke-width={1.5} stroke="currentColor" width={12} height={12}>
             <path stroke-linecap="round" stroke-linejoin="round" d="m19.5 8.25-7.5 7.5-7.5-7.5" />
             </svg>)
            :
              //'>'
            (<svg xmlns="http://www.w3.org/2000/svg" fill="none" viewBox="0 0 24 24" stroke-width={1.5} stroke="currentColor" width={12} height={12}>
             <path stroke-linecap="round" stroke-linejoin="round" d="m8.25 4.5 7.5 7.5-7.5 7.5" />
             </svg>)
          }
            <span style={{ paddingLeft: 8 }}>{node.name}</span>
        </div>

        {open && (
          <div style={{ paddingLeft: 16 }}>
          {[...node.children]
            .sort((u, v) => comp(u, v))
            .map((child, i) => (
              <TreeNodeView
              key={i}
              node={child}
              selectedPath={selectedPath}
              onOpen={onOpen}
              />
            ))}
            </div>
        )}
      </div>
    );
  }

  const isSelected = node.path === selectedPath;

  return (
    <div
      onClick={() => onOpen(node)}
      style={{
        cursor: 'pointer',
        paddingLeft: 4,
        background: isSelected ? '#2a2d2e' : 'transparent',
        borderRadius: 4,
        fontSize: 16,
      }}
    >
      <svg xmlns="http://www.w3.org/2000/svg" fill="none" viewBox="0 0 24 24" strokeWidth={1.5} stroke="currentColor" height={12} width={12}>
      <path strokeLinecap="round" strokeLinejoin="round" d="M19.5 14.25v-2.625a3.375 3.375 0 0 0-3.375-3.375h-1.5A1.125 1.125 0 0 1 13.5 7.125v-1.5a3.375 3.375 0 0 0-3.375-3.375H8.25m2.25 0H5.625c-.621 0-1.125.504-1.125 1.125v17.25c0 .621.504 1.125 1.125 1.125h12.75c.621 0 1.125-.504 1.125-1.125V11.25a9 9 0 0 0-9-9Z" />
      </svg> {node.name.replace('.json', '')}
    </div>
  );
};

export const RangeExplorer: React.FC<RangeExplorerProps> = ({
  tree,
  selectedPath,
  onOpen,
}) => {
  return (
    <div
      style={{
        width: 260,
        padding: 8,
        fontFamily: 'monospace',
        background: '#1b1b1b',
        color: '#ddd',
        height: '100vh',
      }}
    >
      {tree.map((node, i) => (
        <TreeNodeView
          key={i}
          node={node}
          selectedPath={selectedPath}
          onOpen={onOpen}
        />
      ))}
    </div>
  );
};
