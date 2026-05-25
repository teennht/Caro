#ifndef CARO_VIET_UI_PIXEL_BACKGROUND_HPP
#define CARO_VIET_UI_PIXEL_BACKGROUND_HPP

#include <SFML/Graphics.hpp>

namespace caro {

class PixelBackground {
public:
    PixelBackground();
    void update(float dt);
    void render(sf::RenderWindow& window);

private:
    void scaleSpriteToCover(sf::Sprite& sprite, const sf::RenderWindow& window);

    sf::Sprite bgSprite_;
    sf::Sprite sunSprite_;
    sf::Sprite tree1Sprite_;
    sf::Sprite tree2Sprite_;
    bool hasBackground_;
    bool hasSun_;
    bool hasTree1_;
    bool hasTree2_;
};

} // namespace caro

#endif // CARO_VIET_UI_PIXEL_BACKGROUND_HPP
