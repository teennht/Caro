#ifndef CARO_VIET_CORE_WIN_CHECKER_HPP
#define CARO_VIET_CORE_WIN_CHECKER_HPP

#include <vector>
#include "Board.hpp"
#include "Cell.hpp"

namespace caro {

struct WinResult {
    bool hasWinner = false;
    Player winner = Player::None;
    std::vector<Cell> winningCells;
};

class WinChecker {
public:
    WinResult checkWin(const Board& board) const;
    WinResult checkFromMove(const Board& board, const Move& lastMove) const;
};

} // namespace caro

#endif // CARO_VIET_CORE_WIN_CHECKER_HPP
