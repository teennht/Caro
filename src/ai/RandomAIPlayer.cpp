#include "ai/RandomAIPlayer.hpp"
#include "core/Board.hpp"
#include <vector>
#include <cstdlib>
#include <ctime>

namespace caro {

Move RandomAIPlayer::chooseMove(const GameSnapshot& snapshot) {
    // Reconstruct board
    Board board;
    for (const auto& m : snapshot.moves) {
        board.placeMove(m);
    }

    if (board.isFull()) {
        return Move{-1, -1, Player::Player2};
    }

    // Generate candidates within radius 1 of any marked cell
    std::vector<std::pair<int, int>> candidates;
    for (int r = 0; r < Board::Size; ++r) {
        for (int c = 0; c < Board::Size; ++c) {
            if (!board.isEmpty(r, c)) continue;

            // Check if there is any marked cell in the 3x3 neighborhood
            bool nearMark = false;
            for (int dr = -1; dr <= 1 && !nearMark; ++dr) {
                for (int dc = -1; dc <= 1; ++dc) {
                    if (dr == 0 && dc == 0) continue;
                    int nr = r + dr;
                    int nc = c + dc;
                    if (board.isInside(nr, nc) && board.getCell(nr, nc) != Player::None) {
                        nearMark = true;
                        break;
                    }
                }
            }

            if (nearMark) {
                candidates.push_back({r, c});
            }
        }
    }

    // Fallback if no cells are near marks (e.g. empty board)
    if (candidates.empty()) {
        if (board.isEmpty(5, 5)) {
            return Move{5, 5, Player::Player2};
        } else if (board.isEmpty(6, 6)) {
            return Move{6, 6, Player::Player2};
        }

        // Collect all empty cells
        for (int r = 0; r < Board::Size; ++r) {
            for (int c = 0; c < Board::Size; ++c) {
                if (board.isEmpty(r, c)) {
                    candidates.push_back({r, c});
                }
            }
        }
    }

    if (candidates.empty()) {
        return Move{-1, -1, Player::Player2};
    }

    // Pick a random candidate
    static bool seeded = false;
    if (!seeded) {
        std::srand(static_cast<unsigned int>(std::time(nullptr)));
        seeded = true;
    }

    int idx = std::rand() % candidates.size();
    return Move{candidates[idx].first, candidates[idx].second, Player::Player2};
}

} // namespace caro
