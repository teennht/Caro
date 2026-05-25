#ifndef CARO_VIET_UI_VICTORY_SCREEN_HPP
#define CARO_VIET_UI_VICTORY_SCREEN_HPP

#include "Screen.hpp"
#include "Button.hpp"
#include "ai/AIDifficulty.hpp"
#include <SFML/Graphics.hpp>
#include <memory>

namespace caro {

class VictoryScreen : public Screen {
public:
    explicit VictoryScreen(AIDifficulty difficulty);

    void handleEvent(const sf::Event& event, sf::RenderWindow& window) override;
    void update(float dt) override;
    void render(sf::RenderWindow& window) override;

private:
    void replay();
    void returnToMenu();
    void scaleBackground();

    AIDifficulty difficulty_;
    sf::Sprite background_;
    std::unique_ptr<Button> replayButton_;
    std::unique_ptr<Button> menuButton_;
};

} // namespace caro

#endif // CARO_VIET_UI_VICTORY_SCREEN_HPP
