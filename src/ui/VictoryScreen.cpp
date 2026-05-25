#include "ui/VictoryScreen.hpp"
#include "ui/GameplayScreen.hpp"
#include "ui/Layout.hpp"
#include "ui/MainMenuScreen.hpp"
#include "ui/ResourceManager.hpp"
#include "ui/ScreenManager.hpp"
#include "core/Game.hpp"

namespace caro {

VictoryScreen::VictoryScreen(AIDifficulty difficulty)
    : difficulty_(difficulty),
      background_(ResourceManager::instance().getTexture("victory_background")) {
    const sf::Font& font = ResourceManager::instance().getFont();

    scaleBackground();

    replayButton_ = std::make_unique<Button>("Replay", sf::Vector2f(300.f, 78.f), font, [this]() {
        replay();
    });
    replayButton_->setPosition({800.f, 760.f});
    replayButton_->setColors(sf::Color(0x4C, 0x8B, 0x57), sf::Color(0x5C, 0x9B, 0x67), sf::Color(0xF4, 0xF1, 0xE8));

    menuButton_ = std::make_unique<Button>("Main Menu", sf::Vector2f(300.f, 78.f), font, [this]() {
        returnToMenu();
    });
    menuButton_->setPosition({1120.f, 760.f});
    menuButton_->setColors(sf::Color(0xA9, 0x48, 0x48), sf::Color(0xC2, 0x56, 0x56), sf::Color(0xF4, 0xF1, 0xE8));
}

void VictoryScreen::handleEvent(const sf::Event& event, sf::RenderWindow& window) {
    replayButton_->handleEvent(event, window);
    menuButton_->handleEvent(event, window);
}

void VictoryScreen::update(float dt) {
    replayButton_->update(dt);
    menuButton_->update(dt);
}

void VictoryScreen::render(sf::RenderWindow& window) {
    window.clear(sf::Color(0x1D, 0x2B, 0x3F));
    window.draw(background_);

    replayButton_->render(window);
    menuButton_->render(window);
}

void VictoryScreen::replay() {
    ResourceManager::instance().playSound("click");

    GameConfig config;
    config.mode = GameMode::SinglePlayer;
    config.difficulty = difficulty_;
    config.startingPlayer = Player::Player1;
    ScreenManager::instance().requestChange(std::make_unique<GameplayScreen>(config));
}

void VictoryScreen::returnToMenu() {
    ResourceManager::instance().playSound("click");
    ScreenManager::instance().requestChange(std::make_unique<MainMenuScreen>());
}

void VictoryScreen::scaleBackground() {
    const auto size = background_.getTexture().getSize();
    if (size.x == 0 || size.y == 0) {
        return;
    }

    background_.setPosition({0.f, 0.f});
    background_.setScale({
        ui::kVirtualWidthF / static_cast<float>(size.x),
        ui::kVirtualHeightF / static_cast<float>(size.y)
    });
}

} // namespace caro
