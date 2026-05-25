#ifndef CARO_VIET_UI_SCREEN_HPP
#define CARO_VIET_UI_SCREEN_HPP

#include <SFML/Graphics.hpp>

namespace caro {

class Screen {
public:
    virtual ~Screen() = default;
    virtual void handleEvent(const sf::Event& event, sf::RenderWindow& window) = 0;
    virtual void update(float dt) = 0;
    virtual void render(sf::RenderWindow& window) = 0;
};

} // namespace caro

#endif // CARO_VIET_UI_SCREEN_HPP
