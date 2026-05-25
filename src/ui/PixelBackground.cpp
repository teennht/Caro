#include "ui/PixelBackground.hpp"
#include "ui/ResourceManager.hpp"
#include "ui/Layout.hpp"
#include <algorithm>

namespace caro {

PixelBackground::PixelBackground()
    : bgSprite_(ResourceManager::instance().getTexture("background")),
      sunSprite_(ResourceManager::instance().getTexture("sun")),
      tree1Sprite_(ResourceManager::instance().getTexture("tree_1")),
      tree2Sprite_(ResourceManager::instance().getTexture("tree_2")),
      hasBackground_(ResourceManager::instance().wasAssetLoaded("Texture", "background")),
      hasSun_(ResourceManager::instance().wasAssetLoaded("Texture", "sun")),
      hasTree1_(ResourceManager::instance().wasAssetLoaded("Texture", "tree_1")),
      hasTree2_(ResourceManager::instance().wasAssetLoaded("Texture", "tree_2")) {

    sunSprite_.setPosition({1550.f, 70.f});
    sunSprite_.setScale({4.f, 4.f});

    tree1Sprite_.setScale({3.f, 3.f});
    tree1Sprite_.setPosition({120.f, 760.f});

    tree2Sprite_.setScale({3.f, 3.f});
    tree2Sprite_.setPosition({1620.f, 790.f});

}

void PixelBackground::scaleSpriteToCover(sf::Sprite& sprite, const sf::RenderWindow& window) {
    const auto& texture = sprite.getTexture();
    const auto textureSize = texture.getSize();
    const sf::Vector2f windowSize = window.getView().getSize();
    if (textureSize.x == 0 || textureSize.y == 0 || windowSize.x == 0 || windowSize.y == 0) {
        return;
    }

    const float scaleX = windowSize.x / static_cast<float>(textureSize.x);
    const float scaleY = windowSize.y / static_cast<float>(textureSize.y);
    const float scale = std::max(scaleX, scaleY);
    sprite.setScale({scale, scale});

    const auto bounds = sprite.getGlobalBounds();
    sprite.setPosition({
        (windowSize.x - bounds.size.x) * 0.5f,
        (windowSize.y - bounds.size.y) * 0.5f
    });
}

void PixelBackground::update(float dt) {
}

void PixelBackground::render(sf::RenderWindow& window) {
    if (hasBackground_) {
        scaleSpriteToCover(bgSprite_, window);
        window.draw(bgSprite_);
    } else {
        sf::RectangleShape fallbackSky(sf::Vector2f(
            ui::kVirtualWidthF,
            ui::kVirtualHeightF
        ));
        fallbackSky.setFillColor(sf::Color(74, 181, 245));
        window.draw(fallbackSky);
    }

    if (hasSun_) {
        window.draw(sunSprite_);
    }

    if (hasTree1_) {
        window.draw(tree1Sprite_);
    }
    if (hasTree2_) {
        window.draw(tree2Sprite_);
    }
}

} // namespace caro
