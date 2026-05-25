#include "core/WinChecker.hpp"

namespace caro {

namespace {

std::vector<Cell> exactFiveCells(const std::vector<Cell>& orderedCells, const Cell& pivot) {
    if (orderedCells.size() <= 5) {
        return orderedCells;
    }

    int pivotIndex = 0;
    for (int i = 0; i < static_cast<int>(orderedCells.size()); ++i) {
        if (orderedCells[i].row == pivot.row && orderedCells[i].col == pivot.col) {
            pivotIndex = i;
            break;
        }
    }

    int start = pivotIndex - 4;
    if (start < 0) {
        start = 0;
    }
    const int maxStart = static_cast<int>(orderedCells.size()) - 5;
    if (start > maxStart) {
        start = maxStart;
    }

    return std::vector<Cell>(orderedCells.begin() + start, orderedCells.begin() + start + 5);
}

} // namespace

WinResult WinChecker::checkWin(const Board& board) const {
    // Check every cell on the board for a win in any direction
    for (int r = 0; r < Board::Size; ++r) {
        for (int c = 0; c < Board::Size; ++c) {
            Player p = board.getCell(r, c);
            if (p == Player::None) continue;

            Move lastMove{r, c, p};
            WinResult res = checkFromMove(board, lastMove);
            if (res.hasWinner) {
                return res;
            }
        }
    }
    return WinResult{};
}

WinResult WinChecker::checkFromMove(const Board& board, const Move& lastMove) const {
    if (!lastMove.isValid()) {
        return WinResult{};
    }

    Player player = lastMove.player;
    int r = lastMove.row;
    int c = lastMove.col;

    // The 4 direction vectors to check (dy, dx)
    // 1. Horizontal: (0, 1)
    // 2. Vertical: (1, 0)
    // 3. Diagonal Down-Right: (1, 1)
    // 4. Diagonal Up-Right: (-1, 1)
    static const int dirs[4][2] = {
        {0, 1},
        {1, 0},
        {1, 1},
        {-1, 1}
    };

    for (int d = 0; d < 4; ++d) {
        int dy = dirs[d][0];
        int dx = dirs[d][1];

        std::vector<Cell> negativeCells;
        std::vector<Cell> positiveCells;

        // Search positive direction
        int step = 1;
        while (true) {
            int nr = r + step * dy;
            int nc = c + step * dx;
            if (board.isInside(nr, nc) && board.getCell(nr, nc) == player) {
                positiveCells.push_back({nr, nc});
                step++;
            } else {
                break;
            }
        }

        // Search negative direction
        step = 1;
        while (true) {
            int nr = r - step * dy;
            int nc = c - step * dx;
            if (board.isInside(nr, nc) && board.getCell(nr, nc) == player) {
                negativeCells.push_back({nr, nc});
                step++;
            } else {
                break;
            }
        }

        std::vector<Cell> cells;
        for (auto it = negativeCells.rbegin(); it != negativeCells.rend(); ++it) {
            cells.push_back(*it);
        }
        cells.push_back({r, c});
        cells.insert(cells.end(), positiveCells.begin(), positiveCells.end());

        if (cells.size() >= 5) {
            WinResult res;
            res.hasWinner = true;
            res.winner = player;
            res.winningCells = exactFiveCells(cells, Cell{r, c});
            return res;
        }
    }

    return WinResult{};
}

} // namespace caro
