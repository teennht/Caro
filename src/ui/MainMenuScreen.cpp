#include "ui/MainMenuScreen.hpp"
#include "ui/ResourceManager.hpp"
#include "ui/ScreenManager.hpp"
#include "ui/GameplayScreen.hpp"
#include "ui/HowToPlayScreen.hpp"
#include "ui/LoadGameScreen.hpp"
#include "ui/SettingsScreen.hpp"
#include <iostream>

namespace caro {

MainMenuScreen::MainMenuScreen()
    : selectedMode_(GameMode::SinglePlayer),
      selectedDifficulty_(AIDifficulty::Normal),
      showExitConfirm_(false),
      txtTitle_(ResourceManager::instance().getFont()),
      txtSubtitle_(ResourceManager::instance().getFont()),
      txtModeLabel_(ResourceManager::instance().getFont()),
      txtDifficultyLabel_(ResourceManager::instance().getFont()),
      txtDifficultyHint_(ResourceManager::instance().getFont()),
      txtStatus_(ResourceManager::instance().getFont()),
      txtExitConfirm_(ResourceManager::instance().getFont()),
      statusTimer_(0.f) {
    
    const sf::Font& font = ResourceManager::instance().getFont();

    // Title
    // txtTitle_.setString(L"Caro Việt");
    // txtTitle_.setCharacterSize(104);
    // txtTitle_.setFillColor(sf::Color(0xF6, 0xD9, 0x5F)); // Gold #F6D95F
    // sf::FloatRect rTitle = txtTitle_.getLocalBounds();
    // txtTitle_.setOrigin({rTitle.position.x + rTitle.size.x / 2.f, rTitle.position.y + rTitle.size.y / 2.f});
    // txtTitle_.setPosition({960.f, 130.f});

    // Subtitle
    // txtSubtitle_.setString("Offline Vietnamese Caro Game");
    // txtSubtitle_.setCharacterSize(32);
    // txtSubtitle_.setFillColor(sf::Color(0xB8, 0xBE, 0xCF)); // Muted text #B8BECF
    // sf::FloatRect rSub = txtSubtitle_.getLocalBounds();
    // txtSubtitle_.setOrigin({rSub.position.x + rSub.size.x / 2.f, rSub.position.y + rSub.size.y / 2.f});
    // txtSubtitle_.setPosition({960.f, 220.f});

    // Mode Section Label
    txtModeLabel_.setString("Game Mode");
    txtModeLabel_.setCharacterSize(34);
    txtModeLabel_.setFillColor(sf::Color(0xF4, 0xF1, 0xE8));
    sf::FloatRect rModeL = txtModeLabel_.getLocalBounds();
    txtModeLabel_.setOrigin({rModeL.position.x + rModeL.size.x / 2.f, rModeL.position.y + rModeL.size.y / 2.f});
    txtModeLabel_.setPosition({960.f, 330.f});

    // Mode Buttons
    btnSinglePlayer_ = std::make_unique<Button>("Single Player", sf::Vector2f(220.f, 70.f), font, [this]() {
        selectedMode_ = GameMode::SinglePlayer;
        btnSinglePlayer_->setState(ButtonState::Selected);
        btnTwoPlayers_->setState(ButtonState::Normal);
    });
    btnSinglePlayer_->setPosition({840.f, 410.f});
    btnSinglePlayer_->setState(ButtonState::Selected);

    btnTwoPlayers_ = std::make_unique<Button>("2 Players", sf::Vector2f(220.f, 70.f), font, [this]() {
        selectedMode_ = GameMode::TwoPlayers;
        btnSinglePlayer_->setState(ButtonState::Normal);
        btnTwoPlayers_->setState(ButtonState::Selected);
    });
    btnTwoPlayers_->setPosition({1080.f, 410.f});

    // Difficulty Section Label
    txtDifficultyLabel_.setString("Difficulty");
    txtDifficultyLabel_.setCharacterSize(34);
    txtDifficultyLabel_.setFillColor(sf::Color(0xF4, 0xF1, 0xE8));
    sf::FloatRect rDiffL = txtDifficultyLabel_.getLocalBounds();
    txtDifficultyLabel_.setOrigin({rDiffL.position.x + rDiffL.size.x / 2.f, rDiffL.position.y + rDiffL.size.y / 2.f});
    txtDifficultyLabel_.setPosition({960.f, 510.f});

    // Difficulty Buttons
    btnEasy_ = std::make_unique<Button>("Easy", sf::Vector2f(150.f, 64.f), font, [this]() {
        if (selectedMode_ == GameMode::SinglePlayer) {
            selectedDifficulty_ = AIDifficulty::Easy;
            btnEasy_->setState(ButtonState::Selected);
            btnNormal_->setState(ButtonState::Normal);
            btnHard_->setState(ButtonState::Normal);
        }
    });
    btnEasy_->setPosition({790.f, 590.f});

    btnNormal_ = std::make_unique<Button>("Normal", sf::Vector2f(170.f, 64.f), font, [this]() {
        if (selectedMode_ == GameMode::SinglePlayer) {
            selectedDifficulty_ = AIDifficulty::Normal;
            btnEasy_->setState(ButtonState::Normal);
            btnNormal_->setState(ButtonState::Selected);
            btnHard_->setState(ButtonState::Normal);
        }
    });
    btnNormal_->setPosition({960.f, 590.f});
    btnNormal_->setState(ButtonState::Selected);

    btnHard_ = std::make_unique<Button>("Hard", sf::Vector2f(150.f, 64.f), font, [this]() {
        if (selectedMode_ == GameMode::SinglePlayer) {
            selectedDifficulty_ = AIDifficulty::Hard;
            btnEasy_->setState(ButtonState::Normal);
            btnNormal_->setState(ButtonState::Normal);
            btnHard_->setState(ButtonState::Selected);
        }
    });
    btnHard_->setPosition({1130.f, 590.f});

    // Difficulty Hint
    txtDifficultyHint_.setString(toSfString(t("menu.difficulty_hint")));
    txtDifficultyHint_.setCharacterSize(24);
    txtDifficultyHint_.setFillColor(sf::Color(0xB8, 0xBE, 0xCF, 150));
    sf::FloatRect rHint = txtDifficultyHint_.getLocalBounds();
    txtDifficultyHint_.setOrigin({rHint.position.x + rHint.size.x / 2.f, rHint.position.y + rHint.size.y / 2.f});
    txtDifficultyHint_.setPosition({960.f, 655.f});

    // Action Buttons
    btnNewGame_ = std::make_unique<Button>("New Game", sf::Vector2f(420.f, 76.f), font, [this]() {
        startNewGame();
    });
    btnNewGame_->setPosition({960.f, 740.f});

    btnLoadGame_ = std::make_unique<Button>("Load Game", sf::Vector2f(420.f, 76.f), font, [this]() {
        loadSavedGame();
    });
    btnLoadGame_->setPosition({960.f, 834.f});

    btnHowToPlay_ = std::make_unique<Button>("How To Play", sf::Vector2f(200.f, 64.f), font, []() {
        ResourceManager::instance().playSound("click");
        ScreenManager::instance().requestChange(std::make_unique<HowToPlayScreen>());
    });
    btnHowToPlay_->setPosition({850.f, 930.f});

    btnSettings_ = std::make_unique<Button>("Settings", sf::Vector2f(200.f, 64.f), font, []() {
        ResourceManager::instance().playSound("click");
        ScreenManager::instance().requestChange(std::make_unique<SettingsScreen>());
    });
    btnSettings_->setPosition({1070.f, 930.f});

    btnExit_ = std::make_unique<Button>("Exit", sf::Vector2f(170.f, 58.f), font, [this]() {
        ResourceManager::instance().playSound("click");
        showExitConfirm_ = true;
    });
    btnExit_->setColors(sf::Color(0xA9, 0x48, 0x48), sf::Color(0xC2, 0x56, 0x56), sf::Color(0xF4, 0xF1, 0xE8));
    btnExit_->setPosition({1740.f, 80.f});

    // Status Text
    txtStatus_.setString("");
    txtStatus_.setCharacterSize(28);
    txtStatus_.setFillColor(sf::Color(0xD4, 0x5D, 0x5D));
    txtStatus_.setPosition({960.f, 1010.f});

    // Language Toggle Button
    btnLangToggle_ = std::make_unique<Button>("EN", sf::Vector2f(110.f, 58.f), font, [this]() {
        ResourceManager::instance().playSound("click");
        Language nextLang = (ResourceManager::instance().getLanguage() == Language::EN) ? Language::VI : Language::EN;
        ResourceManager::instance().setLanguage(nextLang);
        updateLocalization();
    });
    btnLangToggle_->setPosition({1600.f, 80.f});
    btnLangToggle_->setColors(sf::Color(0x2E, 0x30, 0x4C), sf::Color(0x3E, 0x42, 0x66), sf::Color(0xF4, 0xF1, 0xE8));

    modalOverlay_.setSize({1920.f, 1080.f});
    modalOverlay_.setFillColor(sf::Color(0, 0, 0, 150));

    modalCard_.setSize({720.f, 340.f});
    modalCard_.setOrigin({360.f, 170.f});
    modalCard_.setPosition({960.f, 540.f});
    modalCard_.setFillColor(sf::Color(0x15, 0x22, 0x38, 248));
    modalCard_.setOutlineThickness(5.f);
    modalCard_.setOutlineColor(sf::Color(0xF6, 0xD9, 0x5F));

    txtExitConfirm_.setString(ResourceManager::toSfString("Thoát Caro Việt?"));
    txtExitConfirm_.setCharacterSize(44);
    txtExitConfirm_.setFillColor(sf::Color(0xF4, 0xF1, 0xE8));
    sf::FloatRect rExit = txtExitConfirm_.getLocalBounds();
    txtExitConfirm_.setOrigin({rExit.position.x + rExit.size.x / 2.f, rExit.position.y + rExit.size.y / 2.f});
    txtExitConfirm_.setPosition({960.f, 455.f});

    btnExitCancel_ = std::make_unique<Button>(ResourceManager::toSfString("Hủy"), sf::Vector2f(240.f, 74.f), font, [this]() {
        ResourceManager::instance().playSound("click");
        showExitConfirm_ = false;
    });
    btnExitCancel_->setPosition({820.f, 620.f});

    btnExitConfirm_ = std::make_unique<Button>(ResourceManager::toSfString("Thoát"), sf::Vector2f(240.f, 74.f), font, []() {});
    btnExitConfirm_->setColors(sf::Color(0xA9, 0x48, 0x48), sf::Color(0xC2, 0x56, 0x56), sf::Color(0xF4, 0xF1, 0xE8));
    btnExitConfirm_->setPosition({1100.f, 620.f});

    updateLocalization();
}

void MainMenuScreen::handleEvent(const sf::Event& event, sf::RenderWindow& window) {
    if (showExitConfirm_) {
        btnExitCancel_->handleEvent(event, window);
        btnExitConfirm_->handleEvent(event, window);

        if (const auto* mouseRelease = event.getIf<sf::Event::MouseButtonReleased>()) {
            if (mouseRelease->button == sf::Mouse::Button::Left) {
                sf::Vector2f mousePosF = window.mapPixelToCoords(mouseRelease->position);
                if (btnExitConfirm_->getGlobalBounds().contains(mousePosF)) {
                    ResourceManager::instance().playSound("click");
                    ResourceManager::instance().stopAllSounds();
                    window.close();
                }
            }
        }
        return;
    }

    btnSinglePlayer_->handleEvent(event, window);
    btnTwoPlayers_->handleEvent(event, window);
    btnLangToggle_->handleEvent(event, window);

    if (selectedMode_ == GameMode::SinglePlayer) {
        btnEasy_->handleEvent(event, window);
        btnNormal_->handleEvent(event, window);
        btnHard_->handleEvent(event, window);
    }

    btnNewGame_->handleEvent(event, window);
    btnLoadGame_->handleEvent(event, window);
    btnHowToPlay_->handleEvent(event, window);
    btnSettings_->handleEvent(event, window);
    btnExit_->handleEvent(event, window);
}

void MainMenuScreen::update(float dt) {
    background_.update(dt);

    if (showExitConfirm_) {
        btnExitCancel_->update(dt);
        btnExitConfirm_->update(dt);
        return;
    }

    btnSinglePlayer_->update(dt);
    btnTwoPlayers_->update(dt);

    if (selectedMode_ == GameMode::TwoPlayers) {
        btnEasy_->setState(ButtonState::Disabled);
        btnNormal_->setState(ButtonState::Disabled);
        btnHard_->setState(ButtonState::Disabled);
        txtDifficultyHint_.setFillColor(sf::Color(0xB8, 0xBE, 0xCF, 255)); // show hint clearly
    } else {
        txtDifficultyHint_.setFillColor(sf::Color(0xB8, 0xBE, 0xCF, 100)); // fade hint
        
        // Ensure one difficulty is selected
        if (selectedDifficulty_ == AIDifficulty::Easy) {
            btnEasy_->setState(ButtonState::Selected);
            btnNormal_->setState(ButtonState::Normal);
            btnHard_->setState(ButtonState::Normal);
        } else if (selectedDifficulty_ == AIDifficulty::Normal) {
            btnEasy_->setState(ButtonState::Normal);
            btnNormal_->setState(ButtonState::Selected);
            btnHard_->setState(ButtonState::Normal);
        } else {
            btnEasy_->setState(ButtonState::Normal);
            btnNormal_->setState(ButtonState::Normal);
            btnHard_->setState(ButtonState::Selected);
        }
    }

    btnEasy_->update(dt);
    btnNormal_->update(dt);
    btnHard_->update(dt);

    btnNewGame_->update(dt);
    btnLoadGame_->update(dt);
    btnHowToPlay_->update(dt);
    btnSettings_->update(dt);
    btnExit_->update(dt);
    btnLangToggle_->update(dt);

    if (statusTimer_ > 0.f) {
        statusTimer_ -= dt;
        if (statusTimer_ <= 0.f) {
            txtStatus_.setString("");
        }
    }
}

void MainMenuScreen::render(sf::RenderWindow& window) {
    background_.render(window);

    sf::RectangleShape panelShadow({620.f, 760.f});
    panelShadow.setOrigin({310.f, 380.f});
    panelShadow.setPosition({970.f, 612.f});
    panelShadow.setFillColor(sf::Color(0, 0, 0, 90));
    window.draw(panelShadow);

    sf::RectangleShape panel({620.f, 760.f});
    panel.setOrigin({310.f, 380.f});
    panel.setPosition({960.f, 600.f});
    panel.setFillColor(sf::Color(0x15, 0x22, 0x38, 215));
    panel.setOutlineThickness(5.f);
    panel.setOutlineColor(sf::Color(0xF6, 0xD9, 0x5F, 220));
    window.draw(panel);

    window.draw(txtSubtitle_);
    window.draw(txtModeLabel_);
    
    btnSinglePlayer_->render(window);
    btnTwoPlayers_->render(window);
    
    window.draw(txtDifficultyLabel_);
    btnEasy_->render(window);
    btnNormal_->render(window);
    btnHard_->render(window);
    window.draw(txtDifficultyHint_);

    btnNewGame_->render(window);
    btnLoadGame_->render(window);
    btnHowToPlay_->render(window);
    btnSettings_->render(window);
    btnExit_->render(window);
    btnLangToggle_->render(window);

    if (txtStatus_.getString() != "") {
        sf::FloatRect rStat = txtStatus_.getLocalBounds();
        txtStatus_.setOrigin({rStat.position.x + rStat.size.x / 2.f, rStat.position.y + rStat.size.y / 2.f});
        txtStatus_.setPosition({960.f, 1010.f});
        window.draw(txtStatus_);
    }

    if (showExitConfirm_) {
        window.draw(modalOverlay_);
        sf::RectangleShape modalShadow({720.f, 340.f});
        modalShadow.setOrigin({360.f, 170.f});
        modalShadow.setPosition({972.f, 552.f});
        modalShadow.setFillColor(sf::Color(0, 0, 0, 105));
        window.draw(modalShadow);

        window.draw(modalCard_);
        window.draw(txtExitConfirm_);
        btnExitCancel_->render(window);
        btnExitConfirm_->render(window);
    }
}

void MainMenuScreen::startNewGame() {
    ResourceManager::instance().playSound("click");

    GameConfig config;
    config.mode = selectedMode_;
    config.difficulty = selectedDifficulty_;
    config.startingPlayer = Player::Player1;

    ScreenManager::instance().requestChange(std::make_unique<GameplayScreen>(config));
}

void MainMenuScreen::loadSavedGame() {
    ResourceManager::instance().playSound("click");
    ScreenManager::instance().requestChange(std::make_unique<LoadGameScreen>());
}

void MainMenuScreen::updateLocalization() {
    txtTitle_.setString(toSfString(t("menu.title")));
    sf::FloatRect rTitle = txtTitle_.getLocalBounds();
    txtTitle_.setOrigin({rTitle.position.x + rTitle.size.x / 2.f, rTitle.position.y + rTitle.size.y / 2.f});

    // txtSubtitle_.setString(toSfString(t("menu.subtitle")));
    // sf::FloatRect rSub = txtSubtitle_.getLocalBounds();
    // txtSubtitle_.setOrigin({rSub.position.x + rSub.size.x / 2.f, rSub.position.y + rSub.size.y / 2.f});

    txtModeLabel_.setString(toSfString(t("menu.mode_label")));
    sf::FloatRect rModeL = txtModeLabel_.getLocalBounds();
    txtModeLabel_.setOrigin({rModeL.position.x + rModeL.size.x / 2.f, rModeL.position.y + rModeL.size.y / 2.f});

    btnSinglePlayer_->setLabel(toSfString(t("menu.single_player")));
    btnTwoPlayers_->setLabel(toSfString(t("menu.two_players")));

    txtDifficultyLabel_.setString(toSfString(t("menu.difficulty_label")));
    sf::FloatRect rDiffL = txtDifficultyLabel_.getLocalBounds();
    txtDifficultyLabel_.setOrigin({rDiffL.position.x + rDiffL.size.x / 2.f, rDiffL.position.y + rDiffL.size.y / 2.f});

    btnEasy_->setLabel(toSfString(t("menu.easy")));
    btnNormal_->setLabel(toSfString(t("menu.normal")));
    btnHard_->setLabel(toSfString(t("menu.hard")));

    txtDifficultyHint_.setString(toSfString(t("menu.difficulty_hint")));
    sf::FloatRect rHint = txtDifficultyHint_.getLocalBounds();
    txtDifficultyHint_.setOrigin({rHint.position.x + rHint.size.x / 2.f, rHint.position.y + rHint.size.y / 2.f});

    btnNewGame_->setLabel(toSfString(t("menu.new_game")));
    btnLoadGame_->setLabel(toSfString(t("menu.load_game")));
    btnHowToPlay_->setLabel(toSfString(t("menu.how_to_play")));
    btnSettings_->setLabel(toSfString(t("menu.settings")));
    btnExit_->setLabel(toSfString(t("menu.exit")));
    btnExitCancel_->setLabel(toSfString("Hủy"));
    btnExitConfirm_->setLabel(toSfString("Thoát"));

    txtExitConfirm_.setString(toSfString("Thoát Caro Việt?"));
    sf::FloatRect rExit = txtExitConfirm_.getLocalBounds();
    txtExitConfirm_.setOrigin({rExit.position.x + rExit.size.x / 2.f, rExit.position.y + rExit.size.y / 2.f});
    txtExitConfirm_.setPosition({960.f, 455.f});

    btnLangToggle_->setLabel(ResourceManager::instance().getLanguage() == Language::EN ? "EN" : "VI");
}

std::string MainMenuScreen::t(const std::string& key) const {
    return ResourceManager::instance().getTranslation(key);
}

sf::String MainMenuScreen::toSfString(const std::string& utf8Str) const {
    return ResourceManager::toSfString(utf8Str);
}

} // namespace caro
