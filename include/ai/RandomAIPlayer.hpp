#ifndef CARO_VIET_AI_RANDOM_AI_PLAYER_HPP
#define CARO_VIET_AI_RANDOM_AI_PLAYER_HPP

#include "IAIPlayer.hpp"

namespace caro {

class RandomAIPlayer : public IAIPlayer {
public:
    Move chooseMove(const GameSnapshot& snapshot) override;
};

} // namespace caro

#endif // CARO_VIET_AI_RANDOM_AI_PLAYER_HPP
