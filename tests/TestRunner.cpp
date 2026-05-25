#include "core/Board.hpp"
#include "core/Game.hpp"
#include "core/WinChecker.hpp"
#include "core/SaveManager.hpp"
#include "ai/RandomAIPlayer.hpp"
#include "ai/HeuristicAIPlayer.hpp"
#include <iostream>
#include <cassert>
#include <filesystem>

using namespace caro;

void testBoard() {
    std::cout << "Running testBoard..." << std::endl;
    Board board;
    assert(board.isInside(0, 0));
    assert(board.isInside(11, 11));
    assert(!board.isInside(-1, 0));
    assert(!board.isInside(12, 12));

    assert(board.isEmpty(5, 5));
    board.placeMove({5, 5, Player::Player1});
    assert(!board.isEmpty(5, 5));
    assert(board.getCell(5, 5) == Player::Player1);

    board.reset();
    assert(board.isEmpty(5, 5));
    std::cout << "testBoard passed!" << std::endl;
}

void testWinChecker() {
    std::cout << "Running testWinChecker..." << std::endl;
    WinChecker checker;

    // Horizontal win
    {
        Board board;
        board.placeMove({0, 0, Player::Player1});
        board.placeMove({0, 1, Player::Player1});
        board.placeMove({0, 2, Player::Player1});
        board.placeMove({0, 3, Player::Player1});
        board.placeMove({0, 4, Player::Player1});
        auto res = checker.checkWin(board);
        assert(res.hasWinner);
        assert(res.winner == Player::Player1);
        assert(res.winningCells.size() == 5);
        assert(res.winningCells.front().row == 0 && res.winningCells.front().col == 0);
        assert(res.winningCells.back().row == 0 && res.winningCells.back().col == 4);
    }

    // Vertical win
    {
        Board board;
        board.placeMove({0, 0, Player::Player2});
        board.placeMove({1, 0, Player::Player2});
        board.placeMove({2, 0, Player::Player2});
        board.placeMove({3, 0, Player::Player2});
        board.placeMove({4, 0, Player::Player2});
        auto res = checker.checkWin(board);
        assert(res.hasWinner);
        assert(res.winner == Player::Player2);
        assert(res.winningCells.size() == 5);
        assert(res.winningCells.front().row == 0 && res.winningCells.front().col == 0);
        assert(res.winningCells.back().row == 4 && res.winningCells.back().col == 0);
    }

    // Diagonal down-right win
    {
        Board board;
        board.placeMove({0, 0, Player::Player1});
        board.placeMove({1, 1, Player::Player1});
        board.placeMove({2, 2, Player::Player1});
        board.placeMove({3, 3, Player::Player1});
        board.placeMove({4, 4, Player::Player1});
        auto res = checker.checkWin(board);
        assert(res.hasWinner);
        assert(res.winner == Player::Player1);
        assert(res.winningCells.size() == 5);
        assert(res.winningCells.front().row == 0 && res.winningCells.front().col == 0);
        assert(res.winningCells.back().row == 4 && res.winningCells.back().col == 4);
    }

    // Diagonal up-right win
    {
        Board board;
        board.placeMove({4, 0, Player::Player2});
        board.placeMove({3, 1, Player::Player2});
        board.placeMove({2, 2, Player::Player2});
        board.placeMove({1, 3, Player::Player2});
        board.placeMove({0, 4, Player::Player2});
        auto res = checker.checkWin(board);
        assert(res.hasWinner);
        assert(res.winner == Player::Player2);
        assert(res.winningCells.size() == 5);
        assert(res.winningCells.front().row == 4 && res.winningCells.front().col == 0);
        assert(res.winningCells.back().row == 0 && res.winningCells.back().col == 4);
    }

    std::cout << "testWinChecker passed!" << std::endl;
}

void testGame() {
    std::cout << "Running testGame..." << std::endl;
    GameConfig config;
    config.mode = GameMode::SinglePlayer;
    config.difficulty = AIDifficulty::Normal;
    Game game(config);

    assert(game.status() == GameStatus::Playing);
    assert(game.currentPlayer() == Player::Player1);
    assert(game.isHumanInputAllowed());
    assert(!game.isAITurn());

    // Make human move
    bool ok = game.makeMove(5, 5);
    assert(ok);
    assert(game.board().getCell(5, 5) == Player::Player1);
    assert(game.currentPlayer() == Player::Player2);
    assert(game.isAITurn());
    assert(!game.isHumanInputAllowed());

    // Make AI move
    Move aiMove{5, 6, Player::Player2};
    ok = game.makeAIMove(aiMove);
    assert(ok);
    assert(game.board().getCell(5, 6) == Player::Player2);
    assert(game.currentPlayer() == Player::Player1);
    assert(!game.isAITurn());

    std::cout << "testGame passed!" << std::endl;
}

void testSaveLoad() {
    std::cout << "Running testSaveLoad..." << std::endl;
    std::string path = "test_save.txt";

    GameConfig config;
    config.mode = GameMode::TwoPlayers;
    Game game(config);
    game.makeMove(5, 5); // Player1
    game.makeMove(5, 6); // Player2
    game.makeMove(6, 6); // Player1

    SaveManager sm;
    auto saveRes = sm.save(game.snapshot(), path);
    assert(saveRes.success());

    auto loadRes = sm.load(path);
    assert(loadRes.success());
    assert(loadRes.snapshot.mode == GameMode::TwoPlayers);
    assert(loadRes.snapshot.currentTurn == Player::Player2);
    assert(loadRes.snapshot.moves.size() == 3);
    assert(loadRes.snapshot.moves[0].row == 5);
    assert(loadRes.snapshot.moves[0].col == 5);
    assert(loadRes.snapshot.moves[0].player == Player::Player1);

    // Replay on game
    Game loadedGame;
    bool ok = loadedGame.loadFromSnapshot(loadRes.snapshot);
    assert(ok);
    assert(loadedGame.board().getCell(5, 5) == Player::Player1);
    assert(loadedGame.board().getCell(5, 6) == Player::Player2);
    assert(loadedGame.board().getCell(6, 6) == Player::Player1);
    assert(loadedGame.currentPlayer() == Player::Player2);

    // Clean up
    std::filesystem::remove(path);
    std::cout << "testSaveLoad passed!" << std::endl;
}

void testAI() {
    std::cout << "Running testAI..." << std::endl;
    
    // Test Random AI Player
    {
        RandomAIPlayer ai;
        Game game;
        game.makeMove(5, 5);
        auto snap = game.snapshot();
        Move m = ai.chooseMove(snap);
        assert(m.isValid());
        assert(game.board().isEmpty(m.row, m.col));
    }

    // Test Heuristic AI Player Immediate Win
    {
        HeuristicAIPlayer ai(AIDifficulty::Normal);
        Game game;
        // Make AI close to win (needs 1 more)
        game.makeMove(0, 0); // Player1
        game.makeAIMove({5, 0, Player::Player2});
        game.makeMove(0, 1); // Player1
        game.makeAIMove({5, 1, Player::Player2});
        game.makeMove(0, 2); // Player1
        game.makeAIMove({5, 2, Player::Player2});
        game.makeMove(0, 3); // Player1
        game.makeAIMove({5, 3, Player::Player2});
        game.makeMove(1, 0); // Player1
        
        // AI's turn (Player2) - AI should choose (5, 4) to win immediately
        auto snap = game.snapshot();
        Move m = ai.chooseMove(snap);
        std::cout << "Test 2 returned move: (" << m.row << ", " << m.col << ")" << std::endl;
        assert(m.row == 5 && m.col == 4);
    }

    // Test Heuristic AI Player Immediate Block
    {
        HeuristicAIPlayer ai(AIDifficulty::Normal);
        Game game;
        // Make Human close to win (needs 1 more)
        game.makeMove(5, 0); // Player1
        game.makeAIMove({0, 0, Player::Player2});
        game.makeMove(5, 1); // Player1
        game.makeAIMove({1, 2, Player::Player2});
        game.makeMove(5, 2); // Player1
        game.makeAIMove({2, 4, Player::Player2});
        game.makeMove(5, 3); // Player1
        game.makeAIMove({3, 6, Player::Player2});
        
        // AI's turn (Player2) - AI should choose (5, 4) to block human from winning
        auto snap = game.snapshot();
        Move m = ai.chooseMove(snap);
        std::cout << "Test 3 returned move: (" << m.row << ", " << m.col << ")" << std::endl;
        assert(m.row == 5 && m.col == 4);
    }

    std::cout << "testAI passed!" << std::endl;
}

int main() {
    std::cout << "=== Running Caro Việt Unit Tests ===" << std::endl;
    testBoard();
    testWinChecker();
    testGame();
    testSaveLoad();
    testAI();
    std::cout << "=== All Unit Tests Passed Successfully! ===" << std::endl;
    return 0;
}
