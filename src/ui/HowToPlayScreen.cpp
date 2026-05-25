#include "ui/HowToPlayScreen.hpp"
#include "ui/ResourceManager.hpp"
#include "ui/ScreenManager.hpp"
#include "ui/MainMenuScreen.hpp"

namespace caro {

HowToPlayScreen::HowToPlayScreen()
    : txtTitle_(ResourceManager::instance().getFont()) {

    const sf::Font& font = ResourceManager::instance().getFont();

    // Title
    txtTitle_.setCharacterSize(58);
    txtTitle_.setFillColor(sf::Color(0xF6, 0xD9, 0x5F)); // gold

    // Back Button
    btnBack_ = std::make_unique<Button>("Back", sf::Vector2f(260.f, 72.f), font, []() {
        ResourceManager::instance().playSound("click");
        ScreenManager::instance().requestChange(std::make_unique<MainMenuScreen>());
    });
    btnBack_->setPosition({960.f, 900.f});

    updateLocalization();
}

void HowToPlayScreen::handleEvent(const sf::Event& event, sf::RenderWindow& window) {
    btnBack_->handleEvent(event, window);
}

void HowToPlayScreen::update(float dt) {
    background_.update(dt);
    btnBack_->update(dt);
}

void HowToPlayScreen::render(sf::RenderWindow& window) {
    background_.render(window);

    sf::RectangleShape panel({960.f, 720.f});
    panel.setOrigin({480.f, 360.f});
    panel.setPosition({960.f, 540.f});
    panel.setFillColor(sf::Color(0x15, 0x22, 0x38, 225));
    panel.setOutlineThickness(5.f);
    panel.setOutlineColor(sf::Color(0xF6, 0xD9, 0x5F, 220));
    window.draw(panel);

    window.draw(txtTitle_);

    for (const auto& line : instructions_) {
        window.draw(line);
    }

    btnBack_->render(window);
}

void HowToPlayScreen::updateLocalization() {
    bool isVi = (ResourceManager::instance().getLanguage() == Language::VI);

    txtTitle_.setString(toSfString(isVi ? "HƯỚNG DẪN CHƠI" : "HOW TO PLAY"));
    sf::FloatRect rTitle = txtTitle_.getLocalBounds();
    txtTitle_.setOrigin({rTitle.position.x + rTitle.size.x / 2.f, rTitle.position.y + rTitle.size.y / 2.f});
    txtTitle_.setPosition({960.f, 210.f});

    btnBack_->setLabel(toSfString(isVi ? "Quay Lại" : "Back"));

    // Instruction Lines
    instructions_.clear();
    std::vector<std::string> rawLines;

    if (isVi) {
        rawLines = {
            "1. Đặt dấu X hoặc O lên bàn cờ 12x12.",
            "2. Bên nào kết nối đủ 5 ô liên tiếp trước sẽ thắng.",
            "3. Chơi đơn: Đấu trí với Máy (Dễ, Thường, Khó).",
            "4. Hai người chơi: Chơi cùng bạn bè trên cùng máy.",
            "",
            "Điều khiển:",
            "- Click chuột trái: Đặt quân cờ",
            "- Phím Esc: Tạm dừng trận đấu",
            "- Tổ hợp Ctrl + S: Lưu trận nhanh"
        };
    } else {
        rawLines = {
            "1. Place X or O on the 12x12 grid board.",
            "2. Connect 5 marks in a row to win the match.",
            "3. Single Player: Play against the Bot (Easy, Normal, Hard).",
            "4. Two Players: Play with a friend on the same computer.",
            "",
            "Controls:",
            "- Left Mouse Click: Place your mark",
            "- Escape (Esc) key: Pause the gameplay match",
            "- Ctrl + S keys: Quick-save active match"
        };
    }

    float startY = 310.f;
    for (const auto& rLine : rawLines) {
        sf::Text txt(ResourceManager::instance().getFont());
        txt.setString(toSfString(rLine));
        txt.setCharacterSize(30);
        txt.setFillColor(sf::Color(0xF4, 0xF1, 0xE8));
        
        // Center text line
        sf::FloatRect bounds = txt.getLocalBounds();
        txt.setOrigin({bounds.position.x + bounds.size.x / 2.f, bounds.position.y + bounds.size.y / 2.f});
        txt.setPosition({960.f, startY});
        
        instructions_.push_back(std::move(txt));
        startY += 48.f;
    }
}

std::string HowToPlayScreen::t(const std::string& key) const {
    return ResourceManager::instance().getTranslation(key);
}

sf::String HowToPlayScreen::toSfString(const std::string& utf8Str) const {
    return ResourceManager::toSfString(utf8Str);
}

} // namespace caro
