#include "ui/BoardView.hpp"
#include "ui/ResourceManager.hpp"
#include <cmath>
#include <cstdint>

namespace caro {

BoardView::BoardView()
    : bounds_({0.f, 0.f}, {600.f, 600.f}), winPulseTime_(0.f) {
    for (int r = 0; r < 12; ++r) {
        for (int c = 0; c < 12; ++c) {
            cellScales_[r][c] = 0.0f;
        }
    }
}

void BoardView::setBoardBounds(const sf::FloatRect& bounds) {
    bounds_ = bounds;
}

std::optional<Cell> BoardView::cellFromMouse(sf::Vector2f mouse) const {
    if (!bounds_.contains(mouse)) {
        return std::nullopt;
    }
    float cellW = bounds_.size.x / 12.f;
    float cellH = bounds_.size.y / 12.f;
    int col = static_cast<int>((mouse.x - bounds_.position.x) / cellW);
    int row = static_cast<int>((mouse.y - bounds_.position.y) / cellH);
    
    if (row >= 0 && row < 12 && col >= 0 && col < 12) {
        return Cell{row, col};
    }
    return std::nullopt;
}

void BoardView::setHoveredCell(std::optional<Cell> cell) {
    hoveredCell_ = cell;
}

void BoardView::setLastMove(std::optional<Cell> cell) {
    lastMoveCell_ = cell;
}

void BoardView::setWinningCells(const std::vector<Cell>& cells) {
    winningCells_ = cells;
}

void BoardView::update(float dt) {
    winPulseTime_ += dt;
}

void BoardView::renderWinningLine(sf::RenderWindow& window, float cellW, float cellH) const {
    if (winningCells_.size() < 2) {
        return;
    }

    const Cell& first = winningCells_.front();
    const Cell& last = winningCells_.back();
    const sf::Vector2f start{
        bounds_.position.x + (static_cast<float>(first.col) + 0.5f) * cellW,
        bounds_.position.y + (static_cast<float>(first.row) + 0.5f) * cellH
    };
    const sf::Vector2f end{
        bounds_.position.x + (static_cast<float>(last.col) + 0.5f) * cellW,
        bounds_.position.y + (static_cast<float>(last.row) + 0.5f) * cellH
    };

    const sf::Vector2f delta = end - start;
    const float length = std::sqrt(delta.x * delta.x + delta.y * delta.y);
    if (length <= 0.f) {
        return;
    }

    const float angle = std::atan2(delta.y, delta.x) * 180.f / 3.14159265f;

    sf::RectangleShape glow({length, 18.f});
    glow.setOrigin({0.f, 9.f});
    glow.setPosition(start);
    glow.setRotation(sf::degrees(angle));
    glow.setFillColor(sf::Color(0xFF, 0xE0, 0x5C, 90));
    window.draw(glow);

    sf::RectangleShape shadow({length, 12.f});
    shadow.setOrigin({0.f, 6.f});
    shadow.setPosition({start.x + 3.f, start.y + 3.f});
    shadow.setRotation(sf::degrees(angle));
    shadow.setFillColor(sf::Color(0, 0, 0, 135));
    window.draw(shadow);

    sf::RectangleShape line({length, 10.f});
    line.setOrigin({0.f, 5.f});
    line.setPosition(start);
    line.setRotation(sf::degrees(angle));
    line.setFillColor(sf::Color(0xFF, 0xD4, 0x3D, 245));
    window.draw(line);
}

void BoardView::render(sf::RenderWindow& window, const Board& board) {
    float cellW = bounds_.size.x / 12.f;
    float cellH = bounds_.size.y / 12.f;

    sf::RectangleShape boardPaper(bounds_.size);
    boardPaper.setPosition(bounds_.position);
    boardPaper.setFillColor(sf::Color(0xF4, 0xEE, 0xD7));
    boardPaper.setOutlineThickness(5.f);
    boardPaper.setOutlineColor(sf::Color(0x7B, 0x66, 0x3E, 230));
    window.draw(boardPaper);

    sf::Color gridColor(0x9F, 0x8D, 0x68, 150);
    for (int i = 0; i <= 12; ++i) {
        const float x = std::round(bounds_.position.x + i * cellW);
        sf::RectangleShape vLine({2.f, bounds_.size.y});
        vLine.setPosition({x - 1.f, bounds_.position.y});
        vLine.setFillColor(gridColor);
        window.draw(vLine);

        const float y = std::round(bounds_.position.y + i * cellH);
        sf::RectangleShape hLine({bounds_.size.x, 2.f});
        hLine.setPosition({bounds_.position.x, y - 1.f});
        hLine.setFillColor(gridColor);
        window.draw(hLine);
    }

    // 2. Draw Coordinates Labels (A-L and 1-12)
    if (ResourceManager::instance().isCoordinatesShown()) {
        const sf::Font& font = ResourceManager::instance().getFont();
        
        // Column Labels (A-L) at the top
        for (int col = 0; col < 12; ++col) {
            sf::Text colText(font, std::string(1, 'A' + col), 28);
            colText.setFillColor(sf::Color(0xF4, 0xF1, 0xE8, 230));
            sf::FloatRect textBounds = colText.getLocalBounds();
            colText.setOrigin({textBounds.position.x + textBounds.size.x / 2.f, textBounds.position.y + textBounds.size.y / 2.f});
            
            float x = bounds_.position.x + (col + 0.5f) * cellW;
            float y = bounds_.position.y - 34.f;
            colText.setPosition({x, y});
            window.draw(colText);
        }

        // Row Labels (1-12) on the left
        for (int row = 0; row < 12; ++row) {
            sf::Text rowText(font, std::to_string(row + 1), 28);
            rowText.setFillColor(sf::Color(0xF4, 0xF1, 0xE8, 230));
            sf::FloatRect textBounds = rowText.getLocalBounds();
            rowText.setOrigin({textBounds.position.x + textBounds.size.x / 2.f, textBounds.position.y + textBounds.size.y / 2.f});
            
            float x = bounds_.position.x - 42.f;
            float y = bounds_.position.y + (row + 0.5f) * cellH;
            rowText.setPosition({x, y});
            window.draw(rowText);
        }
    }

    // 3. Draw Winning Cells glow
    if (!winningCells_.empty()) {
        float pulse = std::sin(winPulseTime_ * 12.f) * 0.4f + 0.6f; // Pulse between 0.2 and 1.0
        sf::Color winColor(0xF6, 0xD9, 0x5F, static_cast<std::uint8_t>(pulse * 200.f)); // Glowing Gold #F6D95F

        sf::RectangleShape winOverlay(sf::Vector2f(cellW, cellH));
        winOverlay.setFillColor(winColor);

        for (const auto& cell : winningCells_) {
            float x = bounds_.position.x + cell.col * cellW;
            float y = bounds_.position.y + cell.row * cellH;
            winOverlay.setPosition({x, y});
            window.draw(winOverlay);
        }
    }

    // 4. Draw Hover Highlight
    if (hoveredCell_ && board.isEmpty(hoveredCell_->row, hoveredCell_->col) && winningCells_.empty()) {
        sf::RectangleShape hoverOverlay(sf::Vector2f(cellW - 2.f, cellH - 2.f));
        hoverOverlay.setPosition({bounds_.position.x + hoveredCell_->col * cellW + 1.f, bounds_.position.y + hoveredCell_->row * cellH + 1.f});
        hoverOverlay.setFillColor(sf::Color(0x4F, 0x93, 0xC2, 50)); // Muted blue hover #4F93C2
        window.draw(hoverOverlay);
    }

    // 5. Draw Last Move Highlight
    if (lastMoveCell_ && winningCells_.empty()) {
        sf::RectangleShape lastMoveBorder(sf::Vector2f(cellW - 6.f, cellH - 6.f));
        lastMoveBorder.setPosition({bounds_.position.x + lastMoveCell_->col * cellW + 3.f, bounds_.position.y + lastMoveCell_->row * cellH + 3.f});
        lastMoveBorder.setFillColor(sf::Color::Transparent);
        lastMoveBorder.setOutlineColor(sf::Color(0xF6, 0xD9, 0x5F, 220)); // Gold ring #F6D95F
        lastMoveBorder.setOutlineThickness(4.f);
        window.draw(lastMoveBorder);
    }

    // 6. Draw Placed Marks (X and O)
    sf::Sprite xSprite(ResourceManager::instance().getTexture("x_mark"));
    sf::Sprite oSprite(ResourceManager::instance().getTexture("o_mark"));

    xSprite.setOrigin({32.f, 32.f});
    oSprite.setOrigin({32.f, 32.f});

    for (int r = 0; r < 12; ++r) {
        for (int c = 0; c < 12; ++c) {
            Player cell = board.getCell(r, c);
            if (cell == Player::None) {
                cellScales_[r][c] = 0.0f; // Reset scale if empty
                continue;
            }

            // Animate scale in
            if (cellScales_[r][c] < 1.0f) {
                cellScales_[r][c] += 12.0f * (1.0f - cellScales_[r][c]) * 0.016f; // approximate dt if update not called
                if (cellScales_[r][c] > 0.99f) cellScales_[r][c] = 1.0f;
            }

            float centerX = bounds_.position.x + (c + 0.5f) * cellW;
            float centerY = bounds_.position.y + (r + 0.5f) * cellH;

            float baseScaleX = cellW / 64.f * 0.72f;
            float baseScaleY = cellH / 64.f * 0.72f;

            float currentScaleX = baseScaleX * cellScales_[r][c];
            float currentScaleY = baseScaleY * cellScales_[r][c];

            if (cell == Player::Player1) {
                xSprite.setPosition({centerX, centerY});
                xSprite.setScale({currentScaleX, currentScaleY});
                window.draw(xSprite);
            } else if (cell == Player::Player2) {
                oSprite.setPosition({centerX, centerY});
                oSprite.setScale({currentScaleX, currentScaleY});
                window.draw(oSprite);
            }
        }
    }

    renderWinningLine(window, cellW, cellH);
}

} // namespace caro
