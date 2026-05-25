#include "ui/SplashScreen.hpp"
#include "ui/ResourceManager.hpp"
#include "ui/ScreenManager.hpp"
#include "ui/MainMenuScreen.hpp"

namespace caro {

SplashScreen::SplashScreen()
    : txtPrompt_(ResourceManager::instance().getFont()),
      timer_(0.f),
      promptBlinkTimer_(0.f),
      showPrompt_(true) {

    txtPrompt_.setString("Press any key to start");
    txtPrompt_.setCharacterSize(30);
    txtPrompt_.setFillColor(sf::Color(0xF4, 0xF1, 0xE8));
    sf::FloatRect rPrompt = txtPrompt_.getLocalBounds();
    txtPrompt_.setOrigin({rPrompt.position.x + rPrompt.size.x / 2.f, rPrompt.position.y + rPrompt.size.y / 2.f});
    txtPrompt_.setPosition({960.f, 850.f});
}

void SplashScreen::handleEvent(const sf::Event& event, sf::RenderWindow& window) {
    if (event.is<sf::Event::KeyPressed>() || event.is<sf::Event::MouseButtonPressed>()) {
        ResourceManager::instance().playSound("click");
        ScreenManager::instance().requestChange(std::make_unique<MainMenuScreen>());
    }
}

void SplashScreen::update(float dt) {
    background_.update(dt);
    timer_ += dt;

    if (timer_ >= 3.0f) {
        ScreenManager::instance().requestChange(std::make_unique<MainMenuScreen>());
        return;
    }

    promptBlinkTimer_ += dt;
    if (promptBlinkTimer_ >= 0.5f) {
        showPrompt_ = !showPrompt_;
        promptBlinkTimer_ = 0.f;
    }

    Language currentLang = ResourceManager::instance().getLanguage();
    if (currentLang == Language::VI) {
        txtPrompt_.setString(ResourceManager::toSfString("Nhấn phím bất kỳ để bắt đầu"));
    } else {
        txtPrompt_.setString("Press any key to start");
    }

    sf::FloatRect rPrompt = txtPrompt_.getLocalBounds();
    txtPrompt_.setOrigin({rPrompt.position.x + rPrompt.size.x / 2.f, rPrompt.position.y + rPrompt.size.y / 2.f});
}

void SplashScreen::render(sf::RenderWindow& window) {
    background_.render(window);

    if (showPrompt_) {
        window.draw(txtPrompt_);
    }
}

} // namespace caro
