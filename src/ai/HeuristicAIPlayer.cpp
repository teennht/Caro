#include "ai/HeuristicAIPlayer.hpp"
#include "core/WinChecker.hpp"
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <ctime>

namespace caro {

HeuristicAIPlayer::HeuristicAIPlayer(AIDifficulty difficulty)
    : difficulty_(difficulty) {}

Move HeuristicAIPlayer::chooseMove(const GameSnapshot& snapshot) {
    // Reconstruct board
    Board board;
    for (const auto& m : snapshot.moves) {
        board.placeMove(m);
    }

    if (board.isFull()) {
        return Move{-1, -1, Player::Player2};
    }

    // 1. Easy mode random check (30% random)
    if (difficulty_ == AIDifficulty::Easy) {
        static bool seeded = false;
        if (!seeded) {
            std::srand(static_cast<unsigned int>(std::time(nullptr)));
            seeded = true;
        }
        if (std::rand() % 100 < 30) {
            // Find empty cells radius 1
            auto rad1Candidates = getCandidates(board, 1);
            if (!rad1Candidates.empty()) {
                int idx = std::rand() % rad1Candidates.size();
                return Move{rad1Candidates[idx].first, rad1Candidates[idx].second, Player::Player2};
            }
        }
    }

    // 2. Center check if board is empty
    bool boardEmpty = true;
    for (int r = 0; r < Board::Size && boardEmpty; ++r) {
        for (int c = 0; c < Board::Size; ++c) {
            if (board.getCell(r, c) != Player::None) {
                boardEmpty = false;
                break;
            }
        }
    }
    if (boardEmpty) {
        if (board.isEmpty(6, 6)) return Move{6, 6, Player::Player2};
        if (board.isEmpty(5, 5)) return Move{5, 5, Player::Player2};
    }

    // 3. Generate candidate cells
    int radius = (difficulty_ == AIDifficulty::Hard) ? 2 : 1;
    auto candidates = getCandidates(board, radius);
    if (candidates.empty()) {
        // Fallback to all empty cells
        for (int r = 0; r < Board::Size; ++r) {
            for (int c = 0; c < Board::Size; ++c) {
                if (board.isEmpty(r, c)) candidates.push_back({r, c});
            }
        }
    }

    // 4. Check for immediate winning move for AI (Player2)
    WinChecker winChecker;
    for (const auto& cell : candidates) {
        Board tempBoard = board;
        Move testMove{cell.first, cell.second, Player::Player2};
        tempBoard.placeMove(testMove);
        if (winChecker.checkFromMove(tempBoard, testMove).hasWinner) {
            return testMove;
        }
    }

    // 5. Check for immediate winning move for Human (Player1), block it
    for (const auto& cell : candidates) {
        Board tempBoard = board;
        Move testMove{cell.first, cell.second, Player::Player1};
        tempBoard.placeMove(testMove);
        if (winChecker.checkFromMove(tempBoard, testMove).hasWinner) {
            return Move{cell.first, cell.second, Player::Player2};
        }
    }

    // 6. Hard Mode 1-ply lookahead
    if (difficulty_ == AIDifficulty::Hard) {
        struct Candidate {
            int r, c;
            int score;
        };
        std::vector<Candidate> ratedCandidates;
        for (const auto& cell : candidates) {
            int aiScore = evaluateCell(board, cell.first, cell.second, Player::Player2);
            int humanScore = evaluateCell(board, cell.first, cell.second, Player::Player1);
            int baseScore = aiScore + static_cast<int>(humanScore * 1.15);
            
            // Add center bonus
            baseScore += (6 - std::abs(6 - cell.first)) + (6 - std::abs(6 - cell.second));
            ratedCandidates.push_back({cell.first, cell.second, baseScore});
        }

        std::sort(ratedCandidates.begin(), ratedCandidates.end(), [](const Candidate& a, const Candidate& b) {
            return a.score > b.score;
        });

        int bestFinalScore = -99999999;
        std::pair<int, int> bestMove = candidates[0];

        int limit = std::min(15, static_cast<int>(ratedCandidates.size()));
        for (int i = 0; i < limit; ++i) {
            int r = ratedCandidates[i].r;
            int c = ratedCandidates[i].c;

            Board tempBoard = board;
            tempBoard.placeMove({r, c, Player::Player2});

            // Evaluate best human response
            int bestHumanResponse = 0;
            auto responseCandidates = getCandidates(tempBoard, 1);
            for (const auto& resp : responseCandidates) {
                if (winChecker.checkFromMove(tempBoard, {resp.first, resp.second, Player::Player1}).hasWinner) {
                    bestHumanResponse = 10000000; // Human can win immediately!
                    break;
                }
                int respScore = evaluateCell(tempBoard, resp.first, resp.second, Player::Player1);
                if (respScore > bestHumanResponse) {
                    bestHumanResponse = respScore;
                }
            }

            int finalScore = ratedCandidates[i].score - bestHumanResponse;
            if (finalScore > bestFinalScore) {
                bestFinalScore = finalScore;
                bestMove = {r, c};
            }
        }

        return Move{bestMove.first, bestMove.second, Player::Player2};
    }

    // 7. Normal/Easy Heuristic Move
    int bestScore = -1;
    std::pair<int, int> bestCell = candidates[0];

    for (const auto& cell : candidates) {
        int aiScore = evaluateCell(board, cell.first, cell.second, Player::Player2);
        int humanScore = evaluateCell(board, cell.first, cell.second, Player::Player1);
        int score = aiScore + static_cast<int>(humanScore * 1.15);

        // Add center bonus
        score += (6 - std::abs(6 - cell.first)) + (6 - std::abs(6 - cell.second));

        if (score > bestScore) {
            bestScore = score;
            bestCell = cell;
        }
    }

    return Move{bestCell.first, bestCell.second, Player::Player2};
}

std::vector<std::pair<int, int>> HeuristicAIPlayer::getCandidates(const Board& board, int radius) const {
    std::vector<std::pair<int, int>> candidates;
    for (int r = 0; r < Board::Size; ++r) {
        for (int c = 0; c < Board::Size; ++c) {
            if (!board.isEmpty(r, c)) continue;

            bool nearMark = false;
            for (int dr = -radius; dr <= radius && !nearMark; ++dr) {
                for (int dc = -radius; dc <= radius; ++dc) {
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
    return candidates;
}

int HeuristicAIPlayer::evaluateCell(const Board& board, int r, int c, Player p) const {
    static const int dirs[4][2] = {
        {0, 1},
        {1, 0},
        {1, 1},
        {-1, 1}
    };

    int totalScore = 0;
    for (int d = 0; d < 4; ++d) {
        int dy = dirs[d][0];
        int dx = dirs[d][1];

        std::string s = ".........";
        for (int i = -4; i <= 4; ++i) {
            int nr = r + i * dy;
            int nc = c + i * dx;
            if (i == 0) {
                s[i + 4] = 'X'; // temporarily filled with player p
            } else if (!board.isInside(nr, nc)) {
                s[i + 4] = 'O'; // boundary is blocked
            } else {
                Player cellVal = board.getCell(nr, nc);
                if (cellVal == p) {
                    s[i + 4] = 'X';
                } else if (cellVal == Player::None) {
                    s[i + 4] = '.';
                } else {
                    s[i + 4] = 'O';
                }
            }
        }
        totalScore += getLineScore(s);
    }
    return totalScore;
}

int HeuristicAIPlayer::getLineScore(const std::string& s) const {
    // 1. Five
    if (s.find("XXXXX") != std::string::npos) return 1000000;

    // 2. Open Four
    if (s.find(".XXXX.") != std::string::npos) return 200000;

    // 3. Blocked / Broken Four
    if (s.find("XXXX.") != std::string::npos ||
        s.find(".XXXX") != std::string::npos ||
        s.find("XXX.X") != std::string::npos ||
        s.find("X.XXX") != std::string::npos ||
        s.find("XX.XX") != std::string::npos) return 50000;

    // 4. Open Three
    if (s.find(".XXX..") != std::string::npos ||
        s.find("..XXX.") != std::string::npos ||
        s.find(".X.XX.") != std::string::npos ||
        s.find(".XX.X.") != std::string::npos) return 20000;

    // 5. Blocked / Broken Three
    if (s.find("XXX..") != std::string::npos ||
        s.find("..XXX") != std::string::npos ||
        s.find("XX.X") != std::string::npos ||
        s.find("X.XX") != std::string::npos ||
        s.find("X.X.X") != std::string::npos ||
        s.find(".XXX.") != std::string::npos) return 5000;

    // 6. Open Two
    if (s.find(".XX..") != std::string::npos ||
        s.find("..XX.") != std::string::npos ||
        s.find(".X.X.") != std::string::npos) return 1000;

    // 7. Blocked Two
    if (s.find("XX") != std::string::npos ||
        s.find("X.X") != std::string::npos) return 200;

    return 0;
}

} // namespace caro
