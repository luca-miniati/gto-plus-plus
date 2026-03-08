export type Hand = string; // e.g., 'AA', 'AKs', 'JTo'
export type Range = Map<Hand, number>; // 0.0 to 1.0
export type Strategy = Map<string, Map<Hand, number>>; // action -> hand -> freq

export interface RangeViewerProps {
  range: Range;
  strategy: Strategy;
  width?: number;
  height?: number;
}
export interface RangeSummaryProps {
  range: Range;
  strategy: Strategy;
  width?: number;
  height?: number;
}

export interface RangeFile {
  name: string;
  range: Range;
  strategy: Strategy;
}

export type TreeNode = FolderNode | FileNode;

export interface FolderNode {
  type: 'folder';
  name: string;
  children: TreeNode[];
  orderKey: number;
}

export interface FileNode {
  type: 'file';
  name: string;
  path: string;
  orderKey: number;
}

export interface CardProps {
  rank?: string;
  suit?: string;
  width?: number;
  height?: number;
}