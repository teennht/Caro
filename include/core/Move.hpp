#ifndef CARO_VIET_CORE_MOVE_HPP
#define CARO_VIET_CORE_MOVE_HPP

#include "Player.hpp"

namespace caro {

struct Move {
    int row = -1;
    int col = -1;
    Player player = Player::None;

    bool isValid() const {
        return row >= 0 && col >= 0 && player != Player::None;
    }
};

} // namespace caro

#endif // CARO_VIET_CORE_MOVE_HPP
