#ifndef CARO_VIET_UI_BOARD_VIEW_HPP
#define CARO_VIET_UI_BOARD_VIEW_HPP

#include <SFML/Graphics.hpp>
#include <optional>
#include <vector>
#include "core/Board.hpp"
#include "core/Cell.hpp"

namespace caro {

class BoardView {
public:
    BoardView();

    void setBoardBounds(const sf::FloatRect& bounds);
    std::optional<Cell> cellFromMouse(sf::Vector2f mouse) const;
    void setHoveredCell(std::optional<Cell> cell);
    void setLastMove(std::optional<Cell> cell);
    void setWinningCells(const std::vector<Cell>& cells);
    void update(float dt);
    void render(sf::RenderWindow& window, const Board& board);

private:
    void renderWinningLine(sf::RenderWindow& window, float cellW, float cellH) const;

    sf::FloatRect bounds_;
    std::optional<Cell> hoveredCell_;
    std::optional<Cell> lastMoveCell_;
    std::vector<Cell> winningCells_;

    float cellScales_[12][12];
    float winPulseTime_;
};

} // namespace caro

#endif // CARO_VIET_UI_BOARD_VIEW_HPP
