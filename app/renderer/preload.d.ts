import { RangeFile, TreeNode } from './types';

declare global {
  interface PokerAPI {
    loadPreflopRange: (path: string) => Promise<RangeFile>;
    getPreflopRangeTree: () => Promise<TreeNode[]>;
  }

  interface Window {
    pokerAPI: PokerAPI;
  }
}

export {};
