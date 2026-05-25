#ifndef CARO_VIET_UI_MAIN_MENU_SCREEN_HPP
#define CARO_VIET_UI_MAIN_MENU_SCREEN_HPP

#include "Screen.hpp"
#include "Button.hpp"
#include "PixelBackground.hpp"
#include "core/GameMode.hpp"
#include "ai/AIDifficulty.hpp"
#include <vector>
#include <memory>

namespace caro {

class MainMenuScreen : public Screen {
public:
    MainMenuScreen();

    void handleEvent(const sf::Event& event, sf::RenderWindow& window) override;
    void update(float dt) override;
    void render(sf::RenderWindow& window) override;

private:
    void startNewGame();
    void loadSavedGame();

    GameMode selectedMode_;
    AIDifficulty selectedDifficulty_;

    std::unique_ptr<Button> btnSinglePlayer_;
    std::unique_ptr<Button> btnTwoPlayers_;
    std::unique_ptr<Button> btnEasy_;
    std::unique_ptr<Button> btnNormal_;
    std::unique_ptr<Button> btnHard_;
    std::unique_ptr<Button> btnNewGame_;
    std::unique_ptr<Button> btnLoadGame_;
    std::unique_ptr<Button> btnHowToPlay_;
    std::unique_ptr<Button> btnSettings_;
    std::unique_ptr<Button> btnExit_;
    std::unique_ptr<Button> btnLangToggle_;
    std::unique_ptr<Button> btnExitCancel_;
    std::unique_ptr<Button> btnExitConfirm_;

    PixelBackground background_;
    bool showExitConfirm_;
    sf::RectangleShape modalOverlay_;
    sf::RectangleShape modalCard_;

    sf::Text txtTitle_;
    sf::Text txtSubtitle_;
    sf::Text txtModeLabel_;
    sf::Text txtDifficultyLabel_;
    sf::Text txtDifficultyHint_;
    sf::Text txtStatus_;
    sf::Text txtExitConfirm_;
    float statusTimer_;

    void updateLocalization();
    std::string t(const std::string& key) const;
    sf::String toSfString(const std::string& utf8Str) const;
};

} // namespace caro

#endif // CARO_VIET_UI_MAIN_MENU_SCREEN_HPP
