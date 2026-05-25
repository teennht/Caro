#ifndef CARO_VIET_AI_IAI_PLAYER_HPP
#define CARO_VIET_AI_IAI_PLAYER_HPP

#include "core/Move.hpp"
#include "core/GameSnapshot.hpp"

namespace caro {

class IAIPlayer {
public:
    virtual ~IAIPlayer() = default;
    virtual Move chooseMove(const GameSnapshot& snapshot) = 0;
};

} // namespace caro

#endif // CARO_VIET_AI_IAI_PLAYER_HPP
