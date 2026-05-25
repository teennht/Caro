#ifndef CARO_VIET_AI_HEURISTIC_AI_PLAYER_HPP
#define CARO_VIET_AI_HEURISTIC_AI_PLAYER_HPP

#include "IAIPlayer.hpp"
#include "AIDifficulty.hpp"
#include "core/Board.hpp"
#include <string>
#include <vector>

namespace caro {

class HeuristicAIPlayer : public IAIPlayer {
public:
    explicit HeuristicAIPlayer(AIDifficulty difficulty);
    Move chooseMove(const GameSnapshot& snapshot) override;

private:
    int evaluateCell(const Board& board, int r, int c, Player p) const;
    int getLineScore(const std::string& s) const;
    std::vector<std::pair<int, int>> getCandidates(const Board& board, int radius) const;

    AIDifficulty difficulty_;
};

} // namespace caro

#endif // CARO_VIET_AI_HEURISTIC_AI_PLAYER_HPP
