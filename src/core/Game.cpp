#include "core/Game.hpp"

namespace caro {

Game::Game(const GameConfig& config)
    : config_(config) {
    reset();
}

bool Game::makeMove(int row, int col) {
    if (status_ != GameStatus::Playing) {
        return false;
    }
    if (isAITurn()) {
        return false;
    }
    if (!board_.isInside(row, col) || !board_.isEmpty(row, col)) {
        return false;
    }

    Move move{row, col, currentPlayer_};
    if (!board_.placeMove(move)) {
        return false;
    }
    moves_.push_back(move);

    checkGameEnd(move);
    if (status_ == GameStatus::Playing) {
        switchTurn();
    }
    return true;
}

bool Game::makeAIMove(const Move& move) {
    if (status_ != GameStatus::Playing) {
        return false;
    }
    if (!isAITurn()) {
        return false;
    }
    if (move.player != currentPlayer_) {
        return false;
    }
    if (!board_.isInside(move.row, move.col) || !board_.isEmpty(move.row, move.col)) {
        return false;
    }

    if (!board_.placeMove(move)) {
        return false;
    }
    moves_.push_back(move);

    checkGameEnd(move);
    if (status_ == GameStatus::Playing) {
        switchTurn();
    }
    return true;
}

void Game::reset() {
    board_.reset();
    currentPlayer_ = config_.startingPlayer;
    winner_ = Player::None;
    status_ = GameStatus::Playing;
    moves_.clear();
    winningCells_.clear();
}

bool Game::loadFromSnapshot(const GameSnapshot& snapshot) {
    board_.reset();
    moves_.clear();
    winningCells_.clear();

    config_.mode = snapshot.mode;
    config_.difficulty = snapshot.difficulty;

    for (const auto& move : snapshot.moves) {
        if (!board_.isInside(move.row, move.col) || !board_.isEmpty(move.row, move.col)) {
            return false;
        }
        board_.placeMove(move);
        moves_.push_back(move);
    }

    currentPlayer_ = snapshot.currentTurn;
    winner_ = snapshot.winner;
    status_ = snapshot.status;
    winningCells_ = snapshot.winningCells;

    if (winningCells_.empty() && status_ == GameStatus::GameOver && winner_ != Player::None) {
        auto result = winChecker_.checkWin(board_);
        if (result.hasWinner) {
            winningCells_ = result.winningCells;
        }
    }

    return true;
}

GameSnapshot Game::snapshot() const {
    GameSnapshot snap;
    snap.mode = config_.mode;
    snap.boardSize = Board::Size;
    snap.difficulty = config_.difficulty;
    snap.currentTurn = currentPlayer_;
    snap.status = status_;
    snap.winner = winner_;
    snap.moves = moves_;
    snap.winningCells = winningCells_;
    return snap;
}

const Board& Game::board() const {
    return board_;
}

Player Game::currentPlayer() const {
    return currentPlayer_;
}

Player Game::winner() const {
    return winner_;
}

GameStatus Game::status() const {
    return status_;
}

GameMode Game::mode() const {
    return config_.mode;
}

AIDifficulty Game::difficulty() const {
    return config_.difficulty;
}

const std::vector<Move>& Game::moveHistory() const {
    return moves_;
}

const std::vector<Cell>& Game::winningCells() const {
    return winningCells_;
}

bool Game::isAITurn() const {
    return config_.mode == GameMode::SinglePlayer &&
           currentPlayer_ == Player::Player2 &&
           status_ == GameStatus::Playing;
}

bool Game::isHumanInputAllowed() const {
    if (status_ != GameStatus::Playing) {
        return false;
    }
    if (config_.mode == GameMode::SinglePlayer) {
        return currentPlayer_ == Player::Player1;
    }
    // TwoPlayers
    return true;
}

void Game::switchTurn() {
    currentPlayer_ = (currentPlayer_ == Player::Player1) ? Player::Player2 : Player::Player1;
}

void Game::checkGameEnd(const Move& lastMove) {
    auto result = winChecker_.checkFromMove(board_, lastMove);
    if (result.hasWinner) {
        winner_ = result.winner;
        winningCells_ = result.winningCells;
        status_ = GameStatus::GameOver;
    } else if (board_.isFull()) {
        winner_ = Player::None;
        status_ = GameStatus::GameOver;
    }
}

} // namespace caro
