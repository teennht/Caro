#ifndef CARO_VIET_CORE_BOARD_HPP
#define CARO_VIET_CORE_BOARD_HPP

#include <array>
#include "Player.hpp"
#include "Move.hpp"

namespace caro {

class Board {
public:
    static constexpr int Size = 12;

    Board();

    bool isInside(int row, int col) const;
    bool isEmpty(int row, int col) const;
    Player getCell(int row, int col) const;
    bool placeMove(const Move& move);
    void reset();
    bool isFull() const;

private:
    std::array<std::array<Player, Size>, Size> cells_;
};

} // namespace caro

#endif // CARO_VIET_CORE_BOARD_HPP
