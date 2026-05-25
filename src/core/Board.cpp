#include "core/Board.hpp"

namespace caro {

Board::Board() {
    reset();
}

bool Board::isInside(int row, int col) const {
    return row >= 0 && row < Size && col >= 0 && col < Size;
}

bool Board::isEmpty(int row, int col) const {
    return isInside(row, col) && cells_[row][col] == Player::None;
}

Player Board::getCell(int row, int col) const {
    if (!isInside(row, col)) {
        return Player::None;
    }
    return cells_[row][col];
}

bool Board::placeMove(const Move& move) {
    if (!isInside(move.row, move.col) || !isEmpty(move.row, move.col)) {
        return false;
    }
    cells_[move.row][move.col] = move.player;
    return true;
}

void Board::reset() {
    for (int r = 0; r < Size; ++r) {
        for (int c = 0; c < Size; ++c) {
            cells_[r][c] = Player::None;
        }
    }
}

bool Board::isFull() const {
    for (int r = 0; r < Size; ++r) {
        for (int c = 0; c < Size; ++c) {
            if (cells_[r][c] == Player::None) {
                return false;
            }
        }
    }
    return true;
}

} // namespace caro
