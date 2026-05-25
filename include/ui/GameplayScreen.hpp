#ifndef CARO_VIET_UI_GAMEPLAY_SCREEN_HPP
#define CARO_VIET_UI_GAMEPLAY_SCREEN_HPP

#include "Screen.hpp"
#include "Button.hpp"
#include "BoardView.hpp"
#include "core/Game.hpp"
#include "core/GameSnapshot.hpp"
#include "ai/IAIPlayer.hpp"
#include <memory>
#include <optional>
#include <string>

namespace caro {

class GameplayScreen : public Screen {
public:
    explicit GameplayScreen(const GameConfig& config);
    explicit GameplayScreen(const GameSnapshot& loadedSnapshot);
    GameplayScreen(const GameSnapshot& loadedSnapshot, const std::string& loadedPath);

    void handleEvent(const sf::Event& event, sf::RenderWindow& window) override;
    void update(float dt) override;
    void render(sf::RenderWindow& window) override;

private:
    void initUI();
    void triggerSave();
    void openSaveModal(bool returnToMenuAfterSave);
    bool saveCurrentGame(const std::string& requestedName, bool reuseCurrentPath);
    std::string defaultSaveName() const;
    std::string sanitizeSaveName(const std::string& name) const;
    void markUnsaved();
    void resetSaveState();
    void returnToMenu();

    Game game_;
    BoardView boardView_;
    std::unique_ptr<IAIPlayer> aiPlayer_;

    // UI Buttons
    std::unique_ptr<Button> btnMenu_;
    std::unique_ptr<Button> btnPause_;
    std::unique_ptr<Button> btnSave_;
    std::unique_ptr<Button> btnLangToggle_;

    // Menu Confirmation Overlay
    bool showMenuConfirm_;
    sf::RectangleShape overlayBg_;
    sf::Text txtConfirmTitle_;
    std::unique_ptr<Button> btnConfirmCancel_;
    std::unique_ptr<Button> btnConfirmSaveMenu_;
    std::unique_ptr<Button> btnConfirmMenuNoSave_;

    // Save state and modal
    bool hasUnsavedChanges_;
    std::optional<std::string> currentSavePath_;
    bool showSaveModal_;
    bool returnToMenuAfterSave_;
    std::string saveNameInput_;
    sf::RectangleShape saveInputBox_;
    sf::Text txtSaveTitle_;
    sf::Text txtSaveHint_;
    sf::Text txtSaveInput_;
    sf::Text txtSaveError_;
    std::unique_ptr<Button> btnSaveCancel_;
    std::unique_ptr<Button> btnSaveConfirm_;

    // Pause Overlay
    bool isPaused_;
    sf::Text txtPauseTitle_;
    std::unique_ptr<Button> btnPauseResume_;

    // Game Over Overlay
    std::unique_ptr<Button> btnGameOverRestart_;
    std::unique_ptr<Button> btnGameOverMenu_;
    std::unique_ptr<Button> btnVictoryReplay_;
    std::unique_ptr<Button> btnVictoryMenu_;

    // Panel Labels
    sf::Text txtPanelTitle_;
    sf::Text txtMode_;
    sf::Text txtTurn_;
    sf::Text txtDifficulty_;
    sf::Text txtMoveCount_;
    sf::Text txtLastMove_;
    sf::Text txtStatus_;

    // Match status notice
    sf::Text txtStatusNotice_;
    float statusNoticeTimer_;

    // AI thinking state
    bool aiThinking_;
    float aiThinkingTimer_;
    float aiThinkingDelay_;
    Move pendingAIMove_;

    // Victory notification effect state
    float victoryOverlayAlpha_;
    float victoryBannerScale_;
    bool victoryTriggered_;
    
    struct Particle {
        sf::Vector2f position;
        sf::Vector2f velocity;
        sf::Color color;
        float lifetime;
        float maxLifetime;
        float size;
    };
    std::vector<Particle> victoryParticles_;
    void spawnVictoryParticles();
    void updateVictoryParticles(float dt);
    void renderVictoryParticles(sf::RenderWindow& window);

    void updateLocalization();
    std::string t(const std::string& key) const;
    sf::String toSfString(const std::string& utf8Str) const;
};

} // namespace caro

#endif // CARO_VIET_UI_GAMEPLAY_SCREEN_HPP
