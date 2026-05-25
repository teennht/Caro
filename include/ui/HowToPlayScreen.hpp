#ifndef CARO_VIET_UI_HOW_TO_PLAY_SCREEN_HPP
#define CARO_VIET_UI_HOW_TO_PLAY_SCREEN_HPP

#include "Screen.hpp"
#include "Button.hpp"
#include "PixelBackground.hpp"
#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>

namespace caro {

class HowToPlayScreen : public Screen {
public:
    HowToPlayScreen();
    void handleEvent(const sf::Event& event, sf::RenderWindow& window) override;
    void update(float dt) override;
    void render(sf::RenderWindow& window) override;

private:
    PixelBackground background_;
    sf::Text txtTitle_;
    std::vector<sf::Text> instructions_;
    std::unique_ptr<Button> btnBack_;

    void updateLocalization();
    std::string t(const std::string& key) const;
    sf::String toSfString(const std::string& utf8Str) const;
};

} // namespace caro

#endif // CARO_VIET_UI_HOW_TO_PLAY_SCREEN_HPP
