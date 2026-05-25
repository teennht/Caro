#include "ui/SettingsScreen.hpp"
#include "ui/ResourceManager.hpp"
#include "ui/ScreenManager.hpp"
#include "ui/MainMenuScreen.hpp"

namespace caro {

SettingsScreen::SettingsScreen()
    : txtTitle_(ResourceManager::instance().getFont()),
      txtSoundLabel_(ResourceManager::instance().getFont()),
      txtMusicLabel_(ResourceManager::instance().getFont()),
      txtAnimLabel_(ResourceManager::instance().getFont()),
      txtCoordLabel_(ResourceManager::instance().getFont()) {

    const sf::Font& font = ResourceManager::instance().getFont();

    // Title
    txtTitle_.setCharacterSize(58);
    txtTitle_.setFillColor(sf::Color(0xF6, 0xD9, 0x5F)); // gold

    // Set label properties
    std::vector<sf::Text*> labels = {&txtSoundLabel_, &txtMusicLabel_, &txtAnimLabel_, &txtCoordLabel_};
    float startY = 330.f;
    for (auto* label : labels) {
        label->setCharacterSize(34);
        label->setFillColor(sf::Color(0xF4, 0xF1, 0xE8));
        label->setPosition({650.f, startY});
        startY += 90.f;
    }

    // Toggle Buttons
    btnSoundToggle_ = std::make_unique<Button>("On", sf::Vector2f(220.f, 70.f), font, [this]() {
        ResourceManager::instance().setSoundEnabled(!ResourceManager::instance().isSoundEnabled());
        ResourceManager::instance().playSound("click");
        updateToggleLabels();
    });
    btnSoundToggle_->setPosition({1210.f, 340.f});

    btnMusicToggle_ = std::make_unique<Button>("On", sf::Vector2f(220.f, 70.f), font, [this]() {
        ResourceManager::instance().setMusicEnabled(!ResourceManager::instance().isMusicEnabled());
        ResourceManager::instance().playSound("click");
        updateToggleLabels();
    });
    btnMusicToggle_->setPosition({1210.f, 430.f});

    btnAnimToggle_ = std::make_unique<Button>("Normal", sf::Vector2f(220.f, 70.f), font, [this]() {
        ResourceManager::instance().setAnimationsEnabled(!ResourceManager::instance().isAnimationsEnabled());
        ResourceManager::instance().playSound("click");
        updateToggleLabels();
    });
    btnAnimToggle_->setPosition({1210.f, 520.f});

    btnCoordToggle_ = std::make_unique<Button>("On", sf::Vector2f(220.f, 70.f), font, [this]() {
        ResourceManager::instance().setCoordinatesShown(!ResourceManager::instance().isCoordinatesShown());
        ResourceManager::instance().playSound("click");
        updateToggleLabels();
    });
    btnCoordToggle_->setPosition({1210.f, 610.f});

    // Back Button
    btnBack_ = std::make_unique<Button>("Back", sf::Vector2f(260.f, 72.f), font, []() {
        ResourceManager::instance().playSound("click");
        ScreenManager::instance().requestChange(std::make_unique<MainMenuScreen>());
    });
    btnBack_->setPosition({960.f, 850.f});

    updateLocalization();
}

void SettingsScreen::handleEvent(const sf::Event& event, sf::RenderWindow& window) {
    btnSoundToggle_->handleEvent(event, window);
    btnMusicToggle_->handleEvent(event, window);
    btnAnimToggle_->handleEvent(event, window);
    btnCoordToggle_->handleEvent(event, window);
    btnBack_->handleEvent(event, window);
}

void SettingsScreen::update(float dt) {
    background_.update(dt);
    btnSoundToggle_->update(dt);
    btnMusicToggle_->update(dt);
    btnAnimToggle_->update(dt);
    btnCoordToggle_->update(dt);
    btnBack_->update(dt);
}

void SettingsScreen::render(sf::RenderWindow& window) {
    background_.render(window);

    sf::RectangleShape panel({860.f, 680.f});
    panel.setOrigin({430.f, 340.f});
    panel.setPosition({960.f, 540.f});
    panel.setFillColor(sf::Color(0x15, 0x22, 0x38, 225));
    panel.setOutlineThickness(5.f);
    panel.setOutlineColor(sf::Color(0xF6, 0xD9, 0x5F, 220));
    window.draw(panel);

    window.draw(txtTitle_);

    window.draw(txtSoundLabel_);
    window.draw(txtMusicLabel_);
    window.draw(txtAnimLabel_);
    window.draw(txtCoordLabel_);

    btnSoundToggle_->render(window);
    btnMusicToggle_->render(window);
    btnAnimToggle_->render(window);
    btnCoordToggle_->render(window);
    btnBack_->render(window);
}

void SettingsScreen::updateLocalization() {
    bool isVi = (ResourceManager::instance().getLanguage() == Language::VI);

    txtTitle_.setString(toSfString(isVi ? "CÀI ĐẶT GAME" : "GAME SETTINGS"));
    sf::FloatRect rTitle = txtTitle_.getLocalBounds();
    txtTitle_.setOrigin({rTitle.position.x + rTitle.size.x / 2.f, rTitle.position.y + rTitle.size.y / 2.f});
    txtTitle_.setPosition({960.f, 220.f});

    txtSoundLabel_.setString(toSfString(isVi ? "Âm Thanh:" : "Sound Effects:"));
    txtMusicLabel_.setString(toSfString(isVi ? "Nhạc Nền:" : "Background Music:"));
    txtAnimLabel_.setString(toSfString(isVi ? "Hiệu Ứng:" : "Animations:"));
    txtCoordLabel_.setString(toSfString(isVi ? "Tọa Độ Bàn Cờ:" : "Show Grid Coords:"));

    btnBack_->setLabel(toSfString(isVi ? "Quay Lại" : "Back"));

    updateToggleLabels();
}

void SettingsScreen::updateToggleLabels() {
    bool isVi = (ResourceManager::instance().getLanguage() == Language::VI);

    // Sound
    if (ResourceManager::instance().isSoundEnabled()) {
        btnSoundToggle_->setLabel(toSfString(isVi ? "Bật" : "On"));
        btnSoundToggle_->setColors(sf::Color(0x4C, 0x8B, 0x57), sf::Color(0x5C, 0x9B, 0x67), sf::Color(0xF4, 0xF1, 0xE8));
    } else {
        btnSoundToggle_->setLabel(toSfString(isVi ? "Tắt" : "Off"));
        btnSoundToggle_->setColors(sf::Color(0xA9, 0x48, 0x48), sf::Color(0xC2, 0x56, 0x56), sf::Color(0xF4, 0xF1, 0xE8));
    }

    // Music
    if (ResourceManager::instance().isMusicEnabled()) {
        btnMusicToggle_->setLabel(toSfString(isVi ? "Bật" : "On"));
        btnMusicToggle_->setColors(sf::Color(0x4C, 0x8B, 0x57), sf::Color(0x5C, 0x9B, 0x67), sf::Color(0xF4, 0xF1, 0xE8));
    } else {
        btnMusicToggle_->setLabel(toSfString(isVi ? "Tắt" : "Off"));
        btnMusicToggle_->setColors(sf::Color(0xA9, 0x48, 0x48), sf::Color(0xC2, 0x56, 0x56), sf::Color(0xF4, 0xF1, 0xE8));
    }

    // Animations
    if (ResourceManager::instance().isAnimationsEnabled()) {
        btnAnimToggle_->setLabel(toSfString(isVi ? "Bình Thường" : "Normal"));
        btnAnimToggle_->setColors(sf::Color(0x2E, 0x30, 0x4C), sf::Color(0x3E, 0x42, 0x66), sf::Color(0xF4, 0xF1, 0xE8));
    } else {
        btnAnimToggle_->setLabel(toSfString(isVi ? "Giảm Thiểu" : "Reduced"));
        btnAnimToggle_->setColors(sf::Color(0xA9, 0x48, 0x48), sf::Color(0xC2, 0x56, 0x56), sf::Color(0xF4, 0xF1, 0xE8));
    }

    // Coordinates
    if (ResourceManager::instance().isCoordinatesShown()) {
        btnCoordToggle_->setLabel(toSfString(isVi ? "Bật" : "On"));
        btnCoordToggle_->setColors(sf::Color(0x4C, 0x8B, 0x57), sf::Color(0x5C, 0x9B, 0x67), sf::Color(0xF4, 0xF1, 0xE8));
    } else {
        btnCoordToggle_->setLabel(toSfString(isVi ? "Tắt" : "Off"));
        btnCoordToggle_->setColors(sf::Color(0xA9, 0x48, 0x48), sf::Color(0xC2, 0x56, 0x56), sf::Color(0xF4, 0xF1, 0xE8));
    }
}

std::string SettingsScreen::t(const std::string& key) const {
    return ResourceManager::instance().getTranslation(key);
}

sf::String SettingsScreen::toSfString(const std::string& utf8Str) const {
    return ResourceManager::toSfString(utf8Str);
}

} // namespace caro
