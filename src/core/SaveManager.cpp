#include "core/SaveManager.hpp"
#include <fstream>
#include <sstream>
#include <filesystem>
#include <cstdio>
#include <algorithm>
#include <cctype>

namespace caro {

namespace {

std::string trim(const std::string& value) {
    auto begin = std::find_if_not(value.begin(), value.end(), [](unsigned char ch) {
        return std::isspace(ch) != 0;
    });
    auto end = std::find_if_not(value.rbegin(), value.rend(), [](unsigned char ch) {
        return std::isspace(ch) != 0;
    }).base();
    if (begin >= end) {
        return "";
    }
    return std::string(begin, end);
}

} // namespace

SaveResult SaveManager::save(const GameSnapshot& snapshot, const std::string& path) {
    std::string tempPath = path + ".tmp";
    std::ofstream out(tempPath);
    if (!out.is_open()) {
        return {SaveError::PermissionDenied, "Could not open file for writing: " + tempPath};
    }

    out << "CARO_SAVE_V2\n";
    if (!snapshot.saveName.empty()) {
        out << "SAVE_NAME " << snapshot.saveName << "\n";
    }
    out << "BOARD_SIZE " << snapshot.boardSize << "\n";
    
    std::string modeStr = (snapshot.mode == GameMode::SinglePlayer) ? "SinglePlayer" : "TwoPlayers";
    out << "MODE " << modeStr << "\n";

    std::string diffStr = "None";
    if (snapshot.mode == GameMode::SinglePlayer) {
        if (snapshot.difficulty == AIDifficulty::Easy) diffStr = "Easy";
        else if (snapshot.difficulty == AIDifficulty::Normal) diffStr = "Normal";
        else if (snapshot.difficulty == AIDifficulty::Hard) diffStr = "Hard";
    }
    out << "DIFFICULTY " << diffStr << "\n";

    std::string turnStr = (snapshot.currentTurn == Player::Player1) ? "Player1" : "Player2";
    out << "CURRENT_PLAYER " << turnStr << "\n";

    std::string winnerStr = "None";
    if (snapshot.winner == Player::Player1) winnerStr = "Player1";
    else if (snapshot.winner == Player::Player2) winnerStr = "Player2";
    out << "WINNER " << winnerStr << "\n";

    std::string statusStr = "Playing";
    if (snapshot.status == GameStatus::Paused) statusStr = "Paused";
    else if (snapshot.status == GameStatus::GameOver) statusStr = "GameOver";
    out << "STATUS " << statusStr << "\n";

    out << "MOVE_COUNT " << snapshot.moves.size() << "\n";
    for (const auto& move : snapshot.moves) {
        std::string pStr = (move.player == Player::Player1) ? "Player1" : "Player2";
        out << move.row << " " << move.col << " " << pStr << "\n";
    }

    out << "WINNING_CELL_COUNT " << snapshot.winningCells.size() << "\n";
    for (const auto& cell : snapshot.winningCells) {
        out << cell.row << " " << cell.col << "\n";
    }

    out << "END\n";
    out.close();

    // Atomic rename
    std::error_code ec;
    std::filesystem::rename(tempPath, path, ec);
    if (ec) {
        // Fallback to std::rename if std::filesystem::rename fails
        if (std::rename(tempPath.c_str(), path.c_str()) != 0) {
            std::filesystem::remove(tempPath, ec);
            return {SaveError::PermissionDenied, "Atomic rename failed: " + ec.message()};
        }
    }

    return {SaveError::None, ""};
}

LoadResult SaveManager::load(const std::string& path) {
    std::ifstream in(path);
    if (!in.is_open()) {
        return {SaveError::FileNotFound, "File not found: " + path, {}};
    }

    std::string line;
    if (!std::getline(in, line)) {
        return {SaveError::InvalidFormat, "Empty save file", {}};
    }

    bool isV1 = (line == "CARO_SAVE_V1");
    bool isV2 = (line == "CARO_SAVE_V2");
    if (!isV1 && !isV2) {
        return {SaveError::UnsupportedVersion, "Unsupported save version: " + line, {}};
    }

    GameSnapshot snapshot;
    int moveCount = 0;
    bool hasEnd = false;

    while (std::getline(in, line)) {
        if (line.empty()) continue;
        std::stringstream ss(line);
        std::string key;
        ss >> key;

        if (key == "SAVE_NAME") {
            std::string rawName;
            std::getline(ss, rawName);
            snapshot.saveName = trim(rawName);
        } else if (key == "BOARD_SIZE") {
            int size = 0;
            ss >> size;
            if (size != 12) {
                return {SaveError::InvalidBoardSize, "Invalid board size: " + std::to_string(size), {}};
            }
            snapshot.boardSize = size;
        } else if (key == "MODE") {
            std::string modeVal;
            ss >> modeVal;
            if (modeVal == "SinglePlayer") {
                snapshot.mode = GameMode::SinglePlayer;
            } else if (modeVal == "TwoPlayers") {
                if (isV1) {
                    return {SaveError::InvalidMode, "V1 save does not support TwoPlayers", {}};
                }
                snapshot.mode = GameMode::TwoPlayers;
            } else {
                return {SaveError::InvalidMode, "Invalid mode: " + modeVal, {}};
            }
        } else if (key == "HUMAN_MARK" || key == "AI_MARK") {
            // Ignored in V2
        } else if (key == "CURRENT_TURN" || key == "CURRENT_PLAYER") {
            std::string turnVal;
            ss >> turnVal;
            if (turnVal == "Human" || turnVal == "Player1") {
                snapshot.currentTurn = Player::Player1;
            } else if (turnVal == "AI" || turnVal == "Player2") {
                snapshot.currentTurn = Player::Player2;
            } else {
                return {SaveError::InvalidTurn, "Invalid current turn: " + turnVal, {}};
            }
        } else if (key == "STATUS") {
            std::string statusVal;
            ss >> statusVal;
            if (statusVal == "Playing") snapshot.status = GameStatus::Playing;
            else if (statusVal == "Paused") snapshot.status = GameStatus::Paused;
            else if (statusVal == "GameOver") snapshot.status = GameStatus::GameOver;
            else return {SaveError::CorruptedData, "Invalid status: " + statusVal, {}};
        } else if (key == "WINNER") {
            std::string winnerVal;
            ss >> winnerVal;
            if (winnerVal == "None") snapshot.winner = Player::None;
            else if (winnerVal == "Human" || winnerVal == "Player1") snapshot.winner = Player::Player1;
            else if (winnerVal == "AI" || winnerVal == "Player2") snapshot.winner = Player::Player2;
            else return {SaveError::CorruptedData, "Invalid winner: " + winnerVal, {}};
        } else if (key == "AI_DIFFICULTY" || key == "DIFFICULTY") {
            std::string diffVal;
            ss >> diffVal;
            if (diffVal == "Easy") snapshot.difficulty = AIDifficulty::Easy;
            else if (diffVal == "Normal") snapshot.difficulty = AIDifficulty::Normal;
            else if (diffVal == "Hard") snapshot.difficulty = AIDifficulty::Hard;
            else if (diffVal == "None") {
                snapshot.difficulty = AIDifficulty::Normal; // default
            } else {
                return {SaveError::InvalidDifficulty, "Invalid difficulty: " + diffVal, {}};
            }
        } else if (key == "MOVE_COUNT") {
            ss >> moveCount;
            if (moveCount < 0 || moveCount > 144) {
                return {SaveError::CorruptedData, "Invalid move count: " + std::to_string(moveCount), {}};
            }
            for (int i = 0; i < moveCount; ++i) {
                std::string moveLine;
                if (!std::getline(in, moveLine)) {
                    return {SaveError::CorruptedData, "Unexpected end of file while reading moves", {}};
                }
                std::stringstream moveSS(moveLine);
                int r = -1, c = -1;
                std::string pStr;
                if (!(moveSS >> r >> c >> pStr)) {
                    return {SaveError::CorruptedData, "Malformed move line: " + moveLine, {}};
                }
                if (r < 0 || r >= 12 || c < 0 || c >= 12) {
                    return {SaveError::InvalidMove, "Move coordinates out of bounds: " + moveLine, {}};
                }
                Player p = Player::None;
                if (pStr == "Human" || pStr == "Player1") p = Player::Player1;
                else if (pStr == "AI" || pStr == "Player2") p = Player::Player2;
                else return {SaveError::InvalidPlayer, "Invalid player in move: " + pStr, {}};

                snapshot.moves.push_back({r, c, p});
            }
        } else if (key == "WINNING_CELL_COUNT") {
            int winningCellCount = 0;
            ss >> winningCellCount;
            if (winningCellCount < 0 || winningCellCount > 12) {
                return {SaveError::CorruptedData, "Invalid winning cell count: " + std::to_string(winningCellCount), {}};
            }

            for (int i = 0; i < winningCellCount; ++i) {
                std::string cellLine;
                if (!std::getline(in, cellLine)) {
                    return {SaveError::CorruptedData, "Unexpected end of file while reading winning cells", {}};
                }
                std::stringstream cellSS(cellLine);
                int r = -1;
                int c = -1;
                if (!(cellSS >> r >> c)) {
                    return {SaveError::CorruptedData, "Malformed winning cell line: " + cellLine, {}};
                }
                if (r < 0 || r >= 12 || c < 0 || c >= 12) {
                    return {SaveError::CorruptedData, "Winning cell coordinates out of bounds: " + cellLine, {}};
                }
                snapshot.winningCells.push_back({r, c});
            }
        } else if (key == "END") {
            hasEnd = true;
            break;
        }
    }

    if (!hasEnd) {
        return {SaveError::InvalidFormat, "Missing END token in save file", {}};
    }

    if (snapshot.moves.size() != static_cast<size_t>(moveCount)) {
        return {SaveError::CorruptedData, "Move count mismatch: expected " + std::to_string(moveCount) + ", got " + std::to_string(snapshot.moves.size()), {}};
    }

    return {SaveError::None, "", snapshot};
}

} // namespace caro
