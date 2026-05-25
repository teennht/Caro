#ifndef CARO_VIET_CORE_GAME_HPP
#define CARO_VIET_CORE_GAME_HPP

#include <vector>
#include "Board.hpp"
#include "Player.hpp"
#include "GameMode.hpp"
#include "GameStatus.hpp"
#include "GameSnapshot.hpp"
#include "WinChecker.hpp"
#include "ai/AIDifficulty.hpp"

namespace caro {

struct GameConfig {
    GameMode mode = GameMode::SinglePlayer;
    AIDifficulty difficulty = AIDifficulty::Normal;
    Player startingPlayer = Player::Player1;
};

class Game {
public:
    explicit Game(const GameConfig& config = GameConfig{});

    bool makeMove(int row, int col);
    bool makeAIMove(const Move& move);
    void reset();

    // Load from snapshot
    bool loadFromSnapshot(const GameSnapshot& snapshot);

    GameSnapshot snapshot() const;
    const Board& board() const;
    Player currentPlayer() const;
    Player winner() const;
    GameStatus status() const;
    GameMode mode() const;
    AIDifficulty difficulty() const;
    const std::vector<Move>& moveHistory() const;
    const std::vector<Cell>& winningCells() const;

    bool isAITurn() const;
    bool isHumanInputAllowed() const;

private:
    void switchTurn();
    void checkGameEnd(const Move& lastMove);

    GameConfig config_;
    Board board_;
    Player currentPlayer_;
    Player winner_;
    GameStatus status_;
    std::vector<Move> moves_;
    std::vector<Cell> winningCells_;
    WinChecker winChecker_;
};

} // namespace caro

#endif // CARO_VIET_CORE_GAME_HPP
