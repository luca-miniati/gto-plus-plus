#pragma once
#include <memory>
#include <vector>
#include <optional>
#include "utils/utils.h"
#include "info_set/info_set_abstraction.h"
#include "game/game_state.h"
class Node;
using NodePtr = std::shared_ptr<Node>;
using NodeIdx = std::size_t;

struct CFRData {
  std::vector<double> regret_sum, strategy, strategy_sum;
};

struct Node {
    bool is_chance_node;
    int current_player;
    int num_actions;
    std::optional<GameState> terminal_game_state;
    std::optional<std::vector<std::vector<float>>> terminal_utility_matrix;
    std::vector<NodeIdx> children;
    std::unordered_map<PrivateInfoKey, CFRData> strategies;
    Node(bool is_chance_node,
        int current_player,
        int num_actions,
        std::optional<GameState> terminal_game_state,
        std::vector<NodeIdx> children);
    bool HasStrategy(PrivateInfoKey key);
    void InitStrategy(PrivateInfoKey key);
    NodeIdx GetChildIdx(int action_idx);
    std::vector<NodeIdx> GetChildren();
    int GetCurrentPlayer();
    int GetNumActions();
    void UpdateStrategy(PrivateInfoKey key, double reach_p);
    std::vector<double> GetAverageStrategy(PrivateInfoKey key) const;
    double GetRegretSum(PrivateInfoKey key, int action_idx) const;
    double GetStrategySum(PrivateInfoKey key, int action_idx) const;
    double GetStrategy(PrivateInfoKey key, int action_idx) const;
    void SetStrategy(PrivateInfoKey key, int action_idx, double value);
    void NormalizeStrategy(PrivateInfoKey key, int action_idx, double norm);
    void UpdateRegret(PrivateInfoKey key, int action_idx, double value);
    void UpdateStrategySum(PrivateInfoKey key, int action_idx, double value);
    GameState GetTerminalGameState();
    bool IsChanceNode();
    bool IsTerminal();
};
