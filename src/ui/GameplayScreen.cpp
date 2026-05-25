#include "ui/GameplayScreen.hpp"
#include "ui/ResourceManager.hpp"
#include "ui/ScreenManager.hpp"
#include "ui/MainMenuScreen.hpp"
#include "ui/Layout.hpp"
#include "ui/VictoryScreen.hpp"
#include "ai/RandomAIPlayer.hpp"
#include "ai/HeuristicAIPlayer.hpp"
#include "core/SaveManager.hpp"
#include <ctime>
#include <iomanip>
#include <sstream>
#include <filesystem>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <cctype>

namespace caro {

namespace {

void drawFullScreenBackground(sf::RenderWindow& window, const std::string& textureName, sf::Color tint) {
    const auto& texture = ResourceManager::instance().getTexture(textureName);
    sf::Sprite sprite(texture);
    const auto size = texture.getSize();
    if (size.x == 0 || size.y == 0 || !ResourceManager::instance().wasAssetLoaded("Texture", textureName)) {
        sf::RectangleShape fallback({ui::kVirtualWidthF, ui::kVirtualHeightF});
        fallback.setFillColor(sf::Color(0x1D, 0x2B, 0x3F));
        window.draw(fallback);
        return;
    }

    sprite.setPosition({0.f, 0.f});
    sprite.setScale({
        ui::kVirtualWidthF / static_cast<float>(size.x),
        ui::kVirtualHeightF / static_cast<float>(size.y)
    });
    sprite.setColor(tint);
    window.draw(sprite);
}

void centerText(sf::Text& text, sf::Vector2f position) {
    const auto bounds = text.getLocalBounds();
    text.setOrigin({bounds.position.x + bounds.size.x / 2.f, bounds.position.y + bounds.size.y / 2.f});
    text.setPosition(position);
}

void fitTextToWidth(sf::Text& text, float maxWidth, unsigned int minSize) {
    while (text.getCharacterSize() > minSize && text.getLocalBounds().size.x > maxWidth) {
        text.setCharacterSize(text.getCharacterSize() - 1);
    }
}

bool isHumanBotVictory(const Game& game) {
    return game.mode() == GameMode::SinglePlayer &&
           game.status() == GameStatus::GameOver &&
           game.winner() == Player::Player1;
}

std::string trimCopy(const std::string& value) {
    auto begin = std::find_if_not(value.begin(), value.end(), [](unsigned char ch) {
        return std::isspace(ch) != 0;
    });
    auto end = std::find_if_not(value.rbegin(), value.rend(), [](unsigned char ch) {
        return std::isspace(ch) != 0;
    }).base();
    if (begin >= end) {
        return "";
    }
    return std::string(begin, end);
}

} // namespace

GameplayScreen::GameplayScreen(const GameConfig& config)
    : game_(config),
      showMenuConfirm_(false),
      txtConfirmTitle_(ResourceManager::instance().getFont()),
      hasUnsavedChanges_(false),
      showSaveModal_(false),
      returnToMenuAfterSave_(false),
      txtSaveTitle_(ResourceManager::instance().getFont()),
      txtSaveHint_(ResourceManager::instance().getFont()),
      txtSaveInput_(ResourceManager::instance().getFont()),
      txtSaveError_(ResourceManager::instance().getFont()),
      isPaused_(false),
      txtPauseTitle_(ResourceManager::instance().getFont()),
      txtPanelTitle_(ResourceManager::instance().getFont()),
      txtMode_(ResourceManager::instance().getFont()),
      txtTurn_(ResourceManager::instance().getFont()),
      txtDifficulty_(ResourceManager::instance().getFont()),
      txtMoveCount_(ResourceManager::instance().getFont()),
      txtLastMove_(ResourceManager::instance().getFont()),
      txtStatus_(ResourceManager::instance().getFont()),
      txtStatusNotice_(ResourceManager::instance().getFont()),
      statusNoticeTimer_(0.f),
      aiThinking_(false),
      aiThinkingTimer_(0.f),
      aiThinkingDelay_(0.5f),
      victoryOverlayAlpha_(0.f),
      victoryBannerScale_(0.f),
      victoryTriggered_(false) {
    initUI();
}

GameplayScreen::GameplayScreen(const GameSnapshot& loadedSnapshot)
    : showMenuConfirm_(false),
      txtConfirmTitle_(ResourceManager::instance().getFont()),
      hasUnsavedChanges_(false),
      showSaveModal_(false),
      returnToMenuAfterSave_(false),
      txtSaveTitle_(ResourceManager::instance().getFont()),
      txtSaveHint_(ResourceManager::instance().getFont()),
      txtSaveInput_(ResourceManager::instance().getFont()),
      txtSaveError_(ResourceManager::instance().getFont()),
      isPaused_(false),
      txtPauseTitle_(ResourceManager::instance().getFont()),
      txtPanelTitle_(ResourceManager::instance().getFont()),
      txtMode_(ResourceManager::instance().getFont()),
      txtTurn_(ResourceManager::instance().getFont()),
      txtDifficulty_(ResourceManager::instance().getFont()),
      txtMoveCount_(ResourceManager::instance().getFont()),
      txtLastMove_(ResourceManager::instance().getFont()),
      txtStatus_(ResourceManager::instance().getFont()),
      txtStatusNotice_(ResourceManager::instance().getFont()),
      statusNoticeTimer_(0.f),
      aiThinking_(false),
      aiThinkingTimer_(0.f),
      aiThinkingDelay_(0.5f),
      victoryOverlayAlpha_(0.f),
      victoryBannerScale_(0.f),
      victoryTriggered_(false) {
    game_.loadFromSnapshot(loadedSnapshot);
    initUI();
    if (!game_.moveHistory().empty()) {
        const auto& lastMove = game_.moveHistory().back();
        boardView_.setLastMove(Cell{lastMove.row, lastMove.col});
    }
    boardView_.setWinningCells(game_.winningCells());
}

GameplayScreen::GameplayScreen(const GameSnapshot& loadedSnapshot, const std::string& loadedPath)
    : GameplayScreen(loadedSnapshot) {
    currentSavePath_ = loadedPath;
    hasUnsavedChanges_ = false;
}

void GameplayScreen::initUI() {
    const sf::Font& font = ResourceManager::instance().getFont();

    // Setup BoardView position
    boardView_.setBoardBounds(sf::FloatRect({110.f, 170.f}, {840.f, 840.f}));

    // Setup AI player
    if (game_.mode() == GameMode::SinglePlayer) {
        if (game_.difficulty() == AIDifficulty::Easy) {
            aiPlayer_ = std::make_unique<RandomAIPlayer>();
        } else {
            aiPlayer_ = std::make_unique<HeuristicAIPlayer>(game_.difficulty());
        }
    }

    // Top control buttons
    btnLangToggle_ = std::make_unique<Button>("EN", sf::Vector2f(110.f, 64.f), font, [this]() {
        ResourceManager::instance().playSound("click");
        Language nextLang = (ResourceManager::instance().getLanguage() == Language::EN) ? Language::VI : Language::EN;
        ResourceManager::instance().setLanguage(nextLang);
        updateLocalization();
    });
    btnLangToggle_->setPosition({1190.f, 77.f});
    btnLangToggle_->setColors(sf::Color(0x2E, 0x30, 0x4C), sf::Color(0x3E, 0x42, 0x66), sf::Color(0xF4, 0xF1, 0xE8));

    btnMenu_ = std::make_unique<Button>("Menu", sf::Vector2f(170.f, 64.f), font, [this]() {
        ResourceManager::instance().playSound("click");
        showMenuConfirm_ = true;
    });
    btnMenu_->setPosition({1330.f, 77.f});

    btnPause_ = std::make_unique<Button>("Pause", sf::Vector2f(170.f, 64.f), font, [this]() {
        ResourceManager::instance().playSound("click");
        isPaused_ = !isPaused_;
        updateLocalization();
    });
    btnPause_->setPosition({1518.f, 77.f});

    btnSave_ = std::make_unique<Button>("Save", sf::Vector2f(170.f, 64.f), font, [this]() {
        triggerSave();
    });
    btnSave_->setPosition({1706.f, 77.f});
    btnSave_->setColors(sf::Color(0x4C, 0x8B, 0x57), sf::Color(0x5C, 0x9B, 0x67), sf::Color(0xF4, 0xF1, 0xE8)); // Save green

    // Side panel text labels
    txtPanelTitle_.setString("Match Info");
    txtPanelTitle_.setCharacterSize(46);
    txtPanelTitle_.setFillColor(sf::Color(0xF6, 0xD9, 0x5F)); // Gold
    sf::FloatRect rPTitle = txtPanelTitle_.getLocalBounds();
    txtPanelTitle_.setOrigin({rPTitle.position.x + rPTitle.size.x / 2.f, rPTitle.position.y + rPTitle.size.y / 2.f});
    txtPanelTitle_.setPosition({1405.f, 235.f});

    txtMode_.setCharacterSize(32);
    txtMode_.setFillColor(sf::Color(0xF4, 0xF1, 0xE8));

    txtTurn_.setCharacterSize(32);
    txtTurn_.setFillColor(sf::Color(0xF4, 0xF1, 0xE8));

    txtDifficulty_.setCharacterSize(32);
    txtDifficulty_.setFillColor(sf::Color(0xF4, 0xF1, 0xE8));

    txtMoveCount_.setCharacterSize(32);
    txtMoveCount_.setFillColor(sf::Color(0xF4, 0xF1, 0xE8));

    txtLastMove_.setCharacterSize(32);
    txtLastMove_.setFillColor(sf::Color(0xF4, 0xF1, 0xE8));

    txtStatus_.setCharacterSize(34);
    txtStatus_.setFillColor(sf::Color(0xF6, 0xD9, 0x5F));

    // Save/Status notifications
    txtStatusNotice_.setCharacterSize(28);
    txtStatusNotice_.setFillColor(sf::Color(0xF4, 0xF1, 0xE8));

    // Confirmation Overlay
    overlayBg_.setSize({ui::kVirtualWidthF, ui::kVirtualHeightF});
    overlayBg_.setFillColor(sf::Color(0, 0, 0, 130));

    txtConfirmTitle_.setString("Return to main menu?\nUnsaved progress may be lost.");
    txtConfirmTitle_.setCharacterSize(34);
    txtConfirmTitle_.setFillColor(sf::Color(0xF4, 0xF1, 0xE8));
    sf::FloatRect rConf = txtConfirmTitle_.getLocalBounds();
    txtConfirmTitle_.setOrigin({rConf.position.x + rConf.size.x / 2.f, rConf.position.y + rConf.size.y / 2.f});
    txtConfirmTitle_.setPosition({960.f, 420.f});

    btnConfirmCancel_ = std::make_unique<Button>("Cancel", sf::Vector2f(250.f, 70.f), font, [this]() {
        ResourceManager::instance().playSound("click");
        showMenuConfirm_ = false;
    });
    btnConfirmCancel_->setPosition({610.f, 675.f});

    btnConfirmSaveMenu_ = std::make_unique<Button>("Save & Return", sf::Vector2f(330.f, 70.f), font, [this]() {
        ResourceManager::instance().playSound("click");
        showMenuConfirm_ = false;
        if (currentSavePath_.has_value()) {
            if (saveCurrentGame("", true)) {
                returnToMenu();
            }
        } else {
            openSaveModal(true);
        }
    });
    btnConfirmSaveMenu_->setPosition({960.f, 675.f});
    btnConfirmSaveMenu_->setColors(sf::Color(0x4C, 0x8B, 0x57), sf::Color(0x5C, 0x9B, 0x67), sf::Color(0xF4, 0xF1, 0xE8));

    btnConfirmMenuNoSave_ = std::make_unique<Button>("Menu Without Saving", sf::Vector2f(330.f, 70.f), font, [this]() {
        returnToMenu();
    });
    btnConfirmMenuNoSave_->setPosition({1310.f, 675.f});
    btnConfirmMenuNoSave_->setColors(sf::Color(0xA9, 0x48, 0x48), sf::Color(0xC2, 0x56, 0x56), sf::Color(0xF4, 0xF1, 0xE8));

    txtSaveTitle_.setString("Save Game");
    txtSaveTitle_.setCharacterSize(44);
    txtSaveTitle_.setFillColor(sf::Color(0xF6, 0xD9, 0x5F));
    centerText(txtSaveTitle_, {960.f, 370.f});

    txtSaveHint_.setString("Enter a save name. Leave empty for timestamp.");
    txtSaveHint_.setCharacterSize(28);
    txtSaveHint_.setFillColor(sf::Color(0xF4, 0xF1, 0xE8));
    centerText(txtSaveHint_, {960.f, 440.f});

    saveInputBox_.setSize({620.f, 66.f});
    saveInputBox_.setOrigin({310.f, 33.f});
    saveInputBox_.setPosition({960.f, 515.f});
    saveInputBox_.setFillColor(sf::Color(0xF4, 0xF1, 0xE8, 245));
    saveInputBox_.setOutlineThickness(4.f);
    saveInputBox_.setOutlineColor(sf::Color(0xF6, 0xD9, 0x5F));

    txtSaveInput_.setCharacterSize(30);
    txtSaveInput_.setFillColor(sf::Color(0x15, 0x22, 0x38));
    txtSaveInput_.setPosition({680.f, 492.f});

    txtSaveError_.setCharacterSize(24);
    txtSaveError_.setFillColor(sf::Color(0xD4, 0x5D, 0x5D));
    centerText(txtSaveError_, {960.f, 575.f});

    btnSaveCancel_ = std::make_unique<Button>("Cancel", sf::Vector2f(250.f, 70.f), font, [this]() {
        ResourceManager::instance().playSound("click");
        showSaveModal_ = false;
        returnToMenuAfterSave_ = false;
        saveNameInput_.clear();
        txtSaveError_.setString("");
    });
    btnSaveCancel_->setPosition({805.f, 665.f});

    btnSaveConfirm_ = std::make_unique<Button>("Save", sf::Vector2f(250.f, 70.f), font, [this]() {
        const bool shouldReturn = returnToMenuAfterSave_;
        if (saveCurrentGame(saveNameInput_, false) && shouldReturn) {
            returnToMenu();
        }
    });
    btnSaveConfirm_->setPosition({1115.f, 665.f});
    btnSaveConfirm_->setColors(sf::Color(0x4C, 0x8B, 0x57), sf::Color(0x5C, 0x9B, 0x67), sf::Color(0xF4, 0xF1, 0xE8));

    // Pause Overlay
    txtPauseTitle_.setString("PAUSED");
    txtPauseTitle_.setCharacterSize(76);
    txtPauseTitle_.setFillColor(sf::Color(0xF6, 0xD9, 0x5F));
    sf::FloatRect rP = txtPauseTitle_.getLocalBounds();
    txtPauseTitle_.setOrigin({rP.position.x + rP.size.x / 2.f, rP.position.y + rP.size.y / 2.f});
    txtPauseTitle_.setPosition({960.f, 460.f});

    btnPauseResume_ = std::make_unique<Button>("Resume Game", sf::Vector2f(320.f, 74.f), font, [this]() {
        ResourceManager::instance().playSound("click");
        isPaused_ = false;
        updateLocalization();
    });
    btnPauseResume_->setPosition({960.f, 590.f});

    // Game Over Panel Buttons (visible on side panel)
    btnGameOverRestart_ = std::make_unique<Button>("Restart Match", sf::Vector2f(280.f, 64.f), font, [this]() {
        ResourceManager::instance().playSound("click");
        game_.reset();
        boardView_.setWinningCells({});
        boardView_.setLastMove(std::nullopt);
        aiThinking_ = false;
        isPaused_ = false;
        victoryOverlayAlpha_ = 0.f;
        victoryBannerScale_ = 0.f;
        victoryTriggered_ = false;
        victoryParticles_.clear();
        resetSaveState();
    });
    btnGameOverRestart_->setPosition({1240.f, 725.f});

    btnGameOverMenu_ = std::make_unique<Button>("Main Menu", sf::Vector2f(280.f, 64.f), font, [this]() {
        returnToMenu();
    });
    btnGameOverMenu_->setPosition({1570.f, 725.f});
    btnGameOverMenu_->setColors(sf::Color(0xA9, 0x48, 0x48), sf::Color(0xC2, 0x56, 0x56), sf::Color(0xF4, 0xF1, 0xE8));

    // Victory Screen Buttons
    btnVictoryReplay_ = std::make_unique<Button>("Replay", sf::Vector2f(280.f, 72.f), font, [this]() {
        ResourceManager::instance().playSound("click");
        game_.reset();
        boardView_.setWinningCells({});
        boardView_.setLastMove(std::nullopt);
        aiThinking_ = false;
        isPaused_ = false;
        victoryOverlayAlpha_ = 0.f;
        victoryBannerScale_ = 0.f;
        victoryTriggered_ = false;
        victoryParticles_.clear();
        resetSaveState();
    });
    btnVictoryReplay_->setPosition({790.f, 655.f});
    btnVictoryReplay_->setColors(sf::Color(0x4C, 0x8B, 0x57), sf::Color(0x5C, 0x9B, 0x67), sf::Color(0xF4, 0xF1, 0xE8));

    btnVictoryMenu_ = std::make_unique<Button>("Main Menu", sf::Vector2f(280.f, 72.f), font, [this]() {
        returnToMenu();
    });
    btnVictoryMenu_->setPosition({1130.f, 655.f});
    btnVictoryMenu_->setColors(sf::Color(0xA9, 0x48, 0x48), sf::Color(0xC2, 0x56, 0x56), sf::Color(0xF4, 0xF1, 0xE8));

    updateLocalization();
}

void GameplayScreen::handleEvent(const sf::Event& event, sf::RenderWindow& window) {
    if (showSaveModal_) {
        if (const auto* text = event.getIf<sf::Event::TextEntered>()) {
            const auto unicode = text->unicode;
            if (unicode >= 32 && unicode < 127 && saveNameInput_.size() < 48) {
                saveNameInput_.push_back(static_cast<char>(unicode));
            }
        }
        if (const auto* key = event.getIf<sf::Event::KeyPressed>()) {
            if (key->code == sf::Keyboard::Key::Backspace && !saveNameInput_.empty()) {
                saveNameInput_.pop_back();
            } else if (key->code == sf::Keyboard::Key::Enter) {
                const bool shouldReturn = returnToMenuAfterSave_;
                if (saveCurrentGame(saveNameInput_, false) && shouldReturn) {
                    returnToMenu();
                }
            } else if (key->code == sf::Keyboard::Key::Escape) {
                showSaveModal_ = false;
                returnToMenuAfterSave_ = false;
            }
        }
        btnSaveCancel_->handleEvent(event, window);
        btnSaveConfirm_->handleEvent(event, window);
        return;
    }

    if (showMenuConfirm_) {
        btnConfirmCancel_->handleEvent(event, window);
        if (hasUnsavedChanges_) {
            btnConfirmSaveMenu_->handleEvent(event, window);
        }
        btnConfirmMenuNoSave_->handleEvent(event, window);
        return;
    }

    if (isPaused_) {
        btnPauseResume_->handleEvent(event, window);
        return;
    }

    if (victoryTriggered_) {
        btnVictoryReplay_->handleEvent(event, window);
        btnVictoryMenu_->handleEvent(event, window);
        return;
    }

    btnMenu_->handleEvent(event, window);
    btnPause_->handleEvent(event, window);
    btnSave_->handleEvent(event, window);
    btnLangToggle_->handleEvent(event, window);

    // Human click board input
    if (game_.isHumanInputAllowed() && !aiThinking_) {
        if (event.getIf<sf::Event::MouseButtonReleased>()) {
            if (auto* mouseRel = event.getIf<sf::Event::MouseButtonReleased>()) {
                if (mouseRel->button == sf::Mouse::Button::Left) {
                    sf::Vector2f mousePosF = window.mapPixelToCoords(mouseRel->position);
                    auto cellOpt = boardView_.cellFromMouse(mousePosF);
                    if (cellOpt.has_value()) {
                        int r = cellOpt->row;
                        int c = cellOpt->col;

                        if (game_.makeMove(r, c)) {
                            ResourceManager::instance().playSound("click");
                            markUnsaved();
                            
                            boardView_.setLastMove(Cell{r, c});

                            if (game_.status() == GameStatus::GameOver) {
                                if (game_.winner() != Player::None) {
                                    ResourceManager::instance().playSound("win");
                                    boardView_.setWinningCells(game_.winningCells());
                                }
                            }
                        } else {
                            // Invalid move buzz
                            ResourceManager::instance().playSound("invalid");
                        }
                    }
                }
            }
        }
    }
}

void GameplayScreen::update(float dt) {
    boardView_.update(dt);

    if (showSaveModal_) {
        btnSaveCancel_->update(dt);
        btnSaveConfirm_->update(dt);
        return;
    }

    if (showMenuConfirm_) {
        btnConfirmCancel_->update(dt);
        if (hasUnsavedChanges_) {
            btnConfirmSaveMenu_->update(dt);
        }
        btnConfirmMenuNoSave_->update(dt);
        return;
    }

    if (isPaused_) {
        btnPauseResume_->update(dt);
        return;
    }

    if (game_.status() == GameStatus::GameOver) {
        if (isHumanBotVictory(game_)) {
            ScreenManager::instance().requestChange(std::make_unique<VictoryScreen>(game_.difficulty()));
            return;
        }

        if (!victoryTriggered_) {
            victoryTriggered_ = true;
            spawnVictoryParticles();

            // Play sound effect
            if (game_.winner() == Player::Player1 || (game_.mode() == GameMode::TwoPlayers && game_.winner() == Player::Player2)) {
                ResourceManager::instance().playSound("win");
            } else {
                ResourceManager::instance().playSound("invalid");
            }
        }

        if (victoryOverlayAlpha_ < 1.0f) {
            victoryOverlayAlpha_ += 2.0f * dt;
            if (victoryOverlayAlpha_ > 1.0f) victoryOverlayAlpha_ = 1.0f;
        }
        victoryBannerScale_ += 8.0f * (1.0f - victoryBannerScale_) * dt;
        updateVictoryParticles(dt);

        btnVictoryReplay_->update(dt);
        btnVictoryMenu_->update(dt);
        btnGameOverRestart_->update(dt);
        btnGameOverMenu_->update(dt);
        return;
    }

    btnMenu_->update(dt);
    btnPause_->update(dt);
    btnSave_->update(dt);
    btnLangToggle_->update(dt);

    // Side panel status update
    if (statusNoticeTimer_ > 0.f) {
        statusNoticeTimer_ -= dt;
        if (statusNoticeTimer_ <= 0.f) {
            txtStatusNotice_.setString("");
        }
    }

    // AI Turn handling
    if (game_.isAITurn() && !aiThinking_) {
        aiThinking_ = true;
        aiThinkingTimer_ = 0.f;
        // Generate AI move in thread-safe snapshot
        pendingAIMove_ = aiPlayer_->chooseMove(game_.snapshot());
    }

    if (aiThinking_) {
        aiThinkingTimer_ += dt;
        if (aiThinkingTimer_ >= aiThinkingDelay_) {
            aiThinking_ = false;
            if (pendingAIMove_.isValid()) {
                if (game_.makeAIMove(pendingAIMove_)) {
                    ResourceManager::instance().playSound("click");
                    markUnsaved();
                    boardView_.setLastMove(Cell{pendingAIMove_.row, pendingAIMove_.col});

                    if (game_.status() == GameStatus::GameOver) {
                        if (game_.winner() != Player::None) {
                            ResourceManager::instance().playSound("win");
                            boardView_.setWinningCells(game_.winningCells());
                        }
                    }
                }
            }
        }
    }
}

void GameplayScreen::render(sf::RenderWindow& window) {
    sf::Vector2f mousePosF = window.mapPixelToCoords(sf::Mouse::getPosition(window));
    boardView_.setHoveredCell(boardView_.cellFromMouse(mousePosF));

    window.clear(sf::Color(0x1D, 0x2B, 0x3F));
    drawFullScreenBackground(window, "background", sf::Color(255, 255, 255, 135));

    sf::RectangleShape readabilityOverlay({ui::kVirtualWidthF, ui::kVirtualHeightF});
    readabilityOverlay.setFillColor(sf::Color(0, 0, 0, 55));
    window.draw(readabilityOverlay);

    sf::RectangleShape boardShadow({858.f, 858.f});
    boardShadow.setPosition({103.f, 163.f});
    boardShadow.setFillColor(sf::Color(0, 0, 0, 85));
    window.draw(boardShadow);

    boardView_.render(window, game_.board());

    sf::RectangleShape panelShadow({650.f, 620.f});
    panelShadow.setPosition({1090.f, 190.f});
    panelShadow.setFillColor(sf::Color(0, 0, 0, 85));
    window.draw(panelShadow);

    sf::RectangleShape panelBg({650.f, 620.f});
    panelBg.setPosition({1080.f, 180.f});
    panelBg.setFillColor(sf::Color(0x15, 0x22, 0x38, 230));
    panelBg.setOutlineThickness(4.f);
    panelBg.setOutlineColor(sf::Color(0xF6, 0xD9, 0x5F, 230));
    window.draw(panelBg);

    window.draw(txtPanelTitle_);

    std::string modeKey = (game_.mode() == GameMode::SinglePlayer) ? "game.mode_sp" : "game.mode_2p";
    txtMode_.setString(toSfString(t(modeKey)));
    txtMode_.setCharacterSize(32);
    fitTextToWidth(txtMode_, 550.f, 24);
    txtMode_.setFillColor(sf::Color(0xF4, 0xF1, 0xE8));
    txtMode_.setPosition({1140.f, 315.f});
    window.draw(txtMode_);

    std::string turnStr;
    if (game_.mode() == GameMode::SinglePlayer) {
        turnStr = (game_.currentPlayer() == Player::Player1) ? t("game.turn_you") : t("game.turn_bot");
    } else {
        turnStr = (game_.currentPlayer() == Player::Player1) ? t("game.turn_p1") : t("game.turn_p2");
    }
    txtTurn_.setString(toSfString(turnStr));
    txtTurn_.setCharacterSize(32);
    fitTextToWidth(txtTurn_, 550.f, 24);
    txtTurn_.setFillColor(game_.currentPlayer() == Player::Player1 ? sf::Color(0xF0, 0x75, 0x68) : sf::Color(0x79, 0xB8, 0xFF));
    txtTurn_.setPosition({1140.f, 380.f});
    window.draw(txtTurn_);

    std::string diffText = "Difficulty: N/A";
    if (game_.mode() == GameMode::SinglePlayer) {
        std::string diffKey = (game_.difficulty() == AIDifficulty::Easy) ? "game.diff_easy" : (game_.difficulty() == AIDifficulty::Normal ? "game.diff_normal" : "game.diff_hard");
        diffText = t(diffKey);
    }
    txtDifficulty_.setString(toSfString(diffText));
    txtDifficulty_.setCharacterSize(32);
    fitTextToWidth(txtDifficulty_, 550.f, 24);
    txtDifficulty_.setFillColor(sf::Color(0xF4, 0xF1, 0xE8));
    txtDifficulty_.setPosition({1140.f, 445.f});
    window.draw(txtDifficulty_);

    txtMoveCount_.setString(toSfString(t("game.moves") + std::to_string(game_.moveHistory().size())));
    txtMoveCount_.setCharacterSize(32);
    fitTextToWidth(txtMoveCount_, 550.f, 24);
    txtMoveCount_.setFillColor(sf::Color(0xF4, 0xF1, 0xE8));
    txtMoveCount_.setPosition({1140.f, 510.f});
    window.draw(txtMoveCount_);

    std::string lmStr = "None";
    if (!game_.moveHistory().empty()) {
        const auto& lm = game_.moveHistory().back();
        char colLetter = 'A' + lm.col;
        lmStr = std::string(1, colLetter) + std::to_string(lm.row + 1);
    }
    std::string prefix = (ResourceManager::instance().getLanguage() == Language::EN ? "Last Move: " : "Nước cuối: ");
    if (lmStr == "None") prefix = t("game.last_move_none");
    else prefix = prefix + lmStr;
    txtLastMove_.setString(toSfString(prefix));
    txtLastMove_.setCharacterSize(32);
    fitTextToWidth(txtLastMove_, 550.f, 24);
    txtLastMove_.setFillColor(sf::Color(0xF4, 0xF1, 0xE8));
    txtLastMove_.setPosition({1140.f, 575.f});
    window.draw(txtLastMove_);

    std::string statusText;
    if (game_.status() == GameStatus::GameOver) {
        if (game_.winner() == Player::Player1) {
            statusText = (game_.mode() == GameMode::SinglePlayer) ? t("victory.title_victory") : t("victory.sub_p1_win");
        } else if (game_.winner() == Player::Player2) {
            statusText = (game_.mode() == GameMode::SinglePlayer) ? t("victory.title_defeat") : t("victory.sub_p2_win");
        } else {
            statusText = t("victory.title_draw");
        }
    } else if (aiThinking_) {
        statusText = t("game.status_bot_thinking");
    } else if (game_.currentPlayer() == Player::Player1) {
        statusText = (game_.mode() == GameMode::SinglePlayer) ? t("game.status_your_turn") : t("game.status_p1_turn");
    } else {
        statusText = t("game.status_p2_turn");
    }

    txtStatus_.setString(toSfString(statusText));
    txtStatus_.setCharacterSize(34);
    fitTextToWidth(txtStatus_, 550.f, 24);
    txtStatus_.setFillColor(sf::Color(0xF6, 0xD9, 0x5F));
    txtStatus_.setPosition({1140.f, 655.f});
    window.draw(txtStatus_);

    if (txtStatusNotice_.getString() != "") {
        txtStatusNotice_.setPosition({1140.f, 730.f});
        window.draw(txtStatusNotice_);
    }

    if (game_.status() == GameStatus::GameOver) {
        btnGameOverRestart_->render(window);
        btnGameOverMenu_->render(window);
    }

    btnMenu_->render(window);
    btnPause_->render(window);
    btnSave_->render(window);
    btnLangToggle_->render(window);

    if (victoryTriggered_) {
        sf::RectangleShape victoryOverlay({ui::kVirtualWidthF, ui::kVirtualHeightF});
        victoryOverlay.setFillColor(sf::Color(0, 0, 0, static_cast<std::uint8_t>(victoryOverlayAlpha_ * 130.f)));
        window.draw(victoryOverlay);

        renderVictoryParticles(window);

        sf::RectangleShape modalShadow({800.f, 430.f});
        modalShadow.setOrigin({400.f, 215.f});
        modalShadow.setPosition({970.f, 525.f});
        modalShadow.setFillColor(sf::Color(0, 0, 0, static_cast<std::uint8_t>(victoryOverlayAlpha_ * 110.f)));
        modalShadow.setScale({victoryBannerScale_, victoryBannerScale_});
        window.draw(modalShadow);

        sf::RectangleShape modal({800.f, 430.f});
        modal.setOrigin({400.f, 215.f});
        modal.setPosition({960.f, 515.f});
        modal.setFillColor(sf::Color(0xF4, 0xF1, 0xE8, static_cast<std::uint8_t>(victoryOverlayAlpha_ * 248.f)));
        modal.setOutlineThickness(6.f);
        modal.setOutlineColor(sf::Color(0xF6, 0xD9, 0x5F, static_cast<std::uint8_t>(victoryOverlayAlpha_ * 255.f)));
        modal.setScale({victoryBannerScale_, victoryBannerScale_});
        window.draw(modal);

        const sf::Font& font = ResourceManager::instance().getFont();
        sf::Text txtTitle(font);
        txtTitle.setCharacterSize(70);
        txtTitle.setStyle(sf::Text::Bold);
        sf::Text txtSub(font);
        txtSub.setCharacterSize(34);

        std::string titleStr = "VICTORY!";
        std::string subStr;
        sf::Color titleColor = sf::Color(0xF6, 0xD9, 0x5F);

        if (game_.winner() == Player::Player1) {
            titleStr = t("victory.title_victory");
            titleColor = sf::Color(0x4C, 0x8B, 0x57);
            subStr = (game_.mode() == GameMode::SinglePlayer) ? t("victory.sub_bot_defeat") : t("victory.sub_p1_win");
        } else if (game_.winner() == Player::Player2) {
            if (game_.mode() == GameMode::SinglePlayer) {
                titleStr = t("victory.title_defeat");
                titleColor = sf::Color(0xA9, 0x48, 0x48);
                subStr = t("victory.sub_bot_win");
            } else {
                titleStr = t("victory.title_victory");
                titleColor = sf::Color(0x4C, 0x8B, 0x57);
                subStr = t("victory.sub_p2_win");
            }
        } else {
            titleStr = t("victory.title_draw");
            titleColor = sf::Color(0x2E, 0x30, 0x4C);
            subStr = t("victory.sub_draw");
        }

        txtTitle.setString(toSfString(titleStr));
        txtTitle.setCharacterSize(70);
        fitTextToWidth(txtTitle, 720.f, 44);
        txtTitle.setFillColor(sf::Color(titleColor.r, titleColor.g, titleColor.b, static_cast<std::uint8_t>(victoryOverlayAlpha_ * 255.f)));
        centerText(txtTitle, {960.f, 395.f});
        txtTitle.setScale({victoryBannerScale_, victoryBannerScale_});

        txtSub.setString(toSfString(subStr));
        txtSub.setCharacterSize(34);
        fitTextToWidth(txtSub, 720.f, 24);
        txtSub.setFillColor(sf::Color(0x15, 0x22, 0x38, static_cast<std::uint8_t>(victoryOverlayAlpha_ * 240.f)));
        centerText(txtSub, {960.f, 500.f});
        txtSub.setScale({victoryBannerScale_, victoryBannerScale_});

        window.draw(txtTitle);
        window.draw(txtSub);

        if (victoryBannerScale_ > 0.8f) {
            btnVictoryReplay_->render(window);
            btnVictoryMenu_->render(window);
        }
    }

    if (showMenuConfirm_) {
        window.draw(overlayBg_);
        const bool dirty = hasUnsavedChanges_;
        const sf::Vector2f dialogSize = dirty ? sf::Vector2f{980.f, 390.f} : sf::Vector2f{720.f, 320.f};
        sf::RectangleShape dialog(dialogSize);
        dialog.setOrigin(dialogSize / 2.f);
        dialog.setPosition({960.f, dirty ? 550.f : 535.f});
        dialog.setFillColor(sf::Color(0x15, 0x22, 0x38, 245));
        dialog.setOutlineColor(sf::Color(0xF6, 0xD9, 0x5F));
        dialog.setOutlineThickness(5.f);
        window.draw(dialog);

        txtConfirmTitle_.setString(toSfString(dirty ? t("confirm.title") : t("confirm.title_clean")));
        txtConfirmTitle_.setCharacterSize(dirty ? 34 : 38);
        fitTextToWidth(txtConfirmTitle_, dirty ? 860.f : 620.f, 24);
        centerText(txtConfirmTitle_, {960.f, dirty ? 445.f : 455.f});
        window.draw(txtConfirmTitle_);

        if (dirty) {
            btnConfirmCancel_->setPosition({610.f, 675.f});
            btnConfirmSaveMenu_->setPosition({960.f, 675.f});
            btnConfirmMenuNoSave_->setPosition({1310.f, 675.f});
            btnConfirmMenuNoSave_->setLabel(toSfString(t("confirm.menu_no_save")));
        } else {
            btnConfirmCancel_->setPosition({810.f, 625.f});
            btnConfirmMenuNoSave_->setPosition({1110.f, 625.f});
            btnConfirmMenuNoSave_->setLabel(toSfString(t("confirm.back_menu")));
        }
        btnConfirmCancel_->render(window);
        if (dirty) {
            btnConfirmSaveMenu_->render(window);
        }
        btnConfirmMenuNoSave_->render(window);
    } else if (showSaveModal_) {
        window.draw(overlayBg_);
        sf::RectangleShape dialogShadow({760.f, 420.f});
        dialogShadow.setOrigin({380.f, 210.f});
        dialogShadow.setPosition({972.f, 552.f});
        dialogShadow.setFillColor(sf::Color(0, 0, 0, 105));
        window.draw(dialogShadow);

        sf::RectangleShape dialog({760.f, 420.f});
        dialog.setOrigin({380.f, 210.f});
        dialog.setPosition({960.f, 540.f});
        dialog.setFillColor(sf::Color(0x15, 0x22, 0x38, 248));
        dialog.setOutlineColor(sf::Color(0xF6, 0xD9, 0x5F));
        dialog.setOutlineThickness(5.f);
        window.draw(dialog);

        window.draw(txtSaveTitle_);
        window.draw(txtSaveHint_);
        window.draw(saveInputBox_);

        const std::string displayInput = saveNameInput_.empty() ? defaultSaveName() : saveNameInput_;
        txtSaveInput_.setString(toSfString(displayInput));
        txtSaveInput_.setFillColor(saveNameInput_.empty() ? sf::Color(0x15, 0x22, 0x38, 140) : sf::Color(0x15, 0x22, 0x38));
        fitTextToWidth(txtSaveInput_, 560.f, 20);
        window.draw(txtSaveInput_);

        if (txtSaveError_.getString() != "") {
            centerText(txtSaveError_, {960.f, 590.f});
            window.draw(txtSaveError_);
        }

        btnSaveCancel_->render(window);
        btnSaveConfirm_->render(window);
    } else if (isPaused_) {
        window.draw(overlayBg_);
        sf::RectangleShape dialog({520.f, 260.f});
        dialog.setOrigin({260.f, 130.f});
        dialog.setPosition({960.f, 545.f});
        dialog.setFillColor(sf::Color(0x15, 0x22, 0x38, 245));
        dialog.setOutlineColor(sf::Color(0xF6, 0xD9, 0x5F));
        dialog.setOutlineThickness(5.f);
        window.draw(dialog);
        window.draw(txtPauseTitle_);
        btnPauseResume_->render(window);
    }
}

void GameplayScreen::triggerSave() {
    ResourceManager::instance().playSound("click");
    openSaveModal(false);
}

void GameplayScreen::openSaveModal(bool returnToMenuAfterSave) {
    showSaveModal_ = true;
    showMenuConfirm_ = false;
    returnToMenuAfterSave_ = returnToMenuAfterSave;
    saveNameInput_.clear();
    txtSaveError_.setString("");
}

std::string GameplayScreen::defaultSaveName() const {
    auto now = std::time(nullptr);
    auto tm = *std::localtime(&now);
    std::ostringstream oss;
    oss << "caro_save_" << std::put_time(&tm, "%Y%m%d_%H%M%S");
    return oss.str();
}

std::string GameplayScreen::sanitizeSaveName(const std::string& name) const {
    std::string trimmed = trimCopy(name);
    if (trimmed.empty()) {
        trimmed = defaultSaveName();
    }

    std::string sanitized;
    sanitized.reserve(trimmed.size());
    const std::string invalid = "<>:\"/\\|?*";
    for (unsigned char ch : trimmed) {
        if (std::iscntrl(ch) || invalid.find(static_cast<char>(ch)) != std::string::npos) {
            sanitized.push_back('_');
        } else {
            sanitized.push_back(static_cast<char>(ch));
        }
    }

    sanitized = trimCopy(sanitized);
    while (!sanitized.empty() && (sanitized.back() == '.' || sanitized.back() == ' ')) {
        sanitized.pop_back();
    }

    if (sanitized.empty()) {
        sanitized = defaultSaveName();
    }
    if (sanitized.size() < 4 || sanitized.substr(sanitized.size() - 4) != ".txt") {
        sanitized += ".txt";
    }
    return sanitized;
}

bool GameplayScreen::saveCurrentGame(const std::string& requestedName, bool reuseCurrentPath) {
    std::error_code ec;
    std::filesystem::create_directories("./saves", ec);
    if (ec) {
        ResourceManager::instance().playSound("invalid");
        txtSaveError_.setString(toSfString("Could not create saves folder."));
        txtStatusNotice_.setString(toSfString(t("game.save_failed") + ec.message()));
        txtStatusNotice_.setFillColor(sf::Color(0xD4, 0x5D, 0x5D));
        statusNoticeTimer_ = 3.f;
        return false;
    }

    std::string path;
    std::string displayName;
    if (reuseCurrentPath && currentSavePath_.has_value()) {
        path = *currentSavePath_;
        displayName = std::filesystem::path(path).stem().string();
    } else {
        const std::string filename = sanitizeSaveName(requestedName);
        path = (std::filesystem::path("./saves") / filename).string();
        displayName = std::filesystem::path(filename).stem().string();
    }

    auto snapshot = game_.snapshot();
    snapshot.saveName = displayName;
    SaveManager sm;
    auto saveRes = sm.save(snapshot, path);
    if (saveRes.success()) {
        ResourceManager::instance().playSound("save");
        currentSavePath_ = path;
        hasUnsavedChanges_ = false;
        showSaveModal_ = false;
        saveNameInput_.clear();
        txtSaveError_.setString("");

        txtStatusNotice_.setString(toSfString(t("game.save_success")));
        txtStatusNotice_.setFillColor(sf::Color(0x4C, 0x8B, 0x57)); // green
        statusNoticeTimer_ = 3.f;
        return true;
    } else {
        ResourceManager::instance().playSound("invalid");

        txtSaveError_.setString(toSfString(saveRes.errorMessage));
        txtStatusNotice_.setString(toSfString(t("game.save_failed") + saveRes.errorMessage));
        txtStatusNotice_.setFillColor(sf::Color(0xD4, 0x5D, 0x5D)); // red
        statusNoticeTimer_ = 3.f;
        return false;
    }
}

void GameplayScreen::markUnsaved() {
    hasUnsavedChanges_ = true;
}

void GameplayScreen::resetSaveState() {
    hasUnsavedChanges_ = false;
    currentSavePath_.reset();
}

void GameplayScreen::returnToMenu() {
    ResourceManager::instance().playSound("click");
    ScreenManager::instance().requestChange(std::make_unique<MainMenuScreen>());
}

void GameplayScreen::spawnVictoryParticles() {
    victoryParticles_.clear();
    
    // Seed random generator
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    std::vector<sf::Color> colors = {
        sf::Color(0xF6, 0xD9, 0x5F), // Gold
        sf::Color(0x4C, 0x8B, 0x57), // Green
        sf::Color(0x4F, 0x93, 0xC2), // Blue
        sf::Color(0xA9, 0x48, 0x48), // Red
        sf::Color(0xE6, 0x7E, 0x22), // Orange
        sf::Color(0x9B, 0x59, 0xB6), // Purple
        sf::Color(0x1A, 0xBC, 0x9C)  // Turquoise
    };

    // Burst 100 particles from center
    for (int i = 0; i < 100; ++i) {
        float angle = (static_cast<float>(std::rand()) / RAND_MAX) * 2.f * 3.14159f;
        float speed = 150.f + (static_cast<float>(std::rand()) / RAND_MAX) * 250.f;
        
        Particle p;
        p.position = { 960.f, 515.f };
        p.velocity = { std::cos(angle) * speed, std::sin(angle) * speed - 80.f }; // push slightly upwards
        p.color = colors[std::rand() % colors.size()];
        p.maxLifetime = 1.5f + (static_cast<float>(std::rand()) / RAND_MAX) * 1.5f;
        p.lifetime = p.maxLifetime;
        p.size = 5.f + (static_cast<float>(std::rand()) / RAND_MAX) * 7.f;
        
        victoryParticles_.push_back(p);
    }
}

void GameplayScreen::updateVictoryParticles(float dt) {
    for (auto it = victoryParticles_.begin(); it != victoryParticles_.end();) {
        it->lifetime -= dt;
        if (it->lifetime <= 0.f) {
            it = victoryParticles_.erase(it);
        } else {
            // Apply gravity
            it->velocity.y += 280.f * dt;
            // Update position
            it->position += it->velocity * dt;
            ++it;
        }
    }
}

void GameplayScreen::renderVictoryParticles(sf::RenderWindow& window) {
    sf::RectangleShape shape;
    for (const auto& p : victoryParticles_) {
        shape.setSize({p.size, p.size});
        shape.setOrigin({p.size / 2.f, p.size / 2.f});
        shape.setPosition(p.position);
        
        // Fade out particle over its lifetime
        float ratio = p.lifetime / p.maxLifetime;
        sf::Color color = p.color;
        color.a = static_cast<std::uint8_t>(ratio * 255.f);
        
        shape.setFillColor(color);
        window.draw(shape);
    }
}

void GameplayScreen::updateLocalization() {
    btnMenu_->setLabel(toSfString(t("game.btn_menu")));
    btnPause_->setLabel(toSfString(t(isPaused_ ? "game.btn_resume" : "game.btn_pause")));
    btnSave_->setLabel(toSfString(t("game.btn_save")));
    btnLangToggle_->setLabel(ResourceManager::instance().getLanguage() == Language::EN ? "EN" : "VI");

    btnGameOverRestart_->setLabel(toSfString(t("game.btn_restart_match")));
    btnGameOverMenu_->setLabel(toSfString(t("game.btn_main_menu")));

    btnVictoryReplay_->setLabel(toSfString(t("victory.btn_replay")));
    btnVictoryMenu_->setLabel(toSfString(t("victory.btn_menu")));

    txtConfirmTitle_.setString(toSfString(t("confirm.title")));
    sf::FloatRect rC = txtConfirmTitle_.getLocalBounds();
    txtConfirmTitle_.setOrigin({rC.position.x + rC.size.x / 2.f, rC.position.y + rC.size.y / 2.f});
    txtConfirmTitle_.setPosition({960.f, 420.f});

    btnConfirmCancel_->setLabel(toSfString(t("confirm.cancel")));
    btnConfirmSaveMenu_->setLabel(toSfString(t("confirm.save_menu")));
    btnConfirmMenuNoSave_->setLabel(toSfString(t("confirm.menu_no_save")));

    txtSaveTitle_.setString(toSfString(t("save.title")));
    centerText(txtSaveTitle_, {960.f, 370.f});
    txtSaveHint_.setString(toSfString(t("save.hint")));
    centerText(txtSaveHint_, {960.f, 440.f});
    btnSaveCancel_->setLabel(toSfString(t("confirm.cancel")));
    btnSaveConfirm_->setLabel(toSfString(t("game.btn_save")));

    txtPauseTitle_.setString(toSfString(t("pause.title")));
    sf::FloatRect rP = txtPauseTitle_.getLocalBounds();
    txtPauseTitle_.setOrigin({rP.position.x + rP.size.x / 2.f, rP.position.y + rP.size.y / 2.f});
    txtPauseTitle_.setPosition({960.f, 460.f});

    btnPauseResume_->setLabel(toSfString(t("pause.resume")));

    txtPanelTitle_.setString(toSfString(t("game.panel_title")));
    sf::FloatRect rPT = txtPanelTitle_.getLocalBounds();
    txtPanelTitle_.setOrigin({rPT.position.x + rPT.size.x / 2.f, rPT.position.y + rPT.size.y / 2.f});
    txtPanelTitle_.setPosition({1405.f, 235.f});
}

std::string GameplayScreen::t(const std::string& key) const {
    return ResourceManager::instance().getTranslation(key);
}

sf::String GameplayScreen::toSfString(const std::string& utf8Str) const {
    return ResourceManager::toSfString(utf8Str);
}

} // namespace caro
