#ifndef CARO_VIET_CORE_GAME_SNAPSHOT_HPP
#define CARO_VIET_CORE_GAME_SNAPSHOT_HPP

#include <string>
#include <vector>
#include "Player.hpp"
#include "GameMode.hpp"
#include "GameStatus.hpp"
#include "ai/AIDifficulty.hpp"
#include "Cell.hpp"
#include "Move.hpp"

namespace caro {

struct GameSnapshot {
    std::string saveName;
    GameMode mode = GameMode::SinglePlayer;
    int boardSize = 12;
    AIDifficulty difficulty = AIDifficulty::Normal;
    Player currentTurn = Player::Player1;
    GameStatus status = GameStatus::Playing;
    Player winner = Player::None;
    std::vector<Move> moves;
    std::vector<Cell> winningCells;
};

} // namespace caro

#endif // CARO_VIET_CORE_GAME_SNAPSHOT_HPP
