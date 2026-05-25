#ifndef CARO_VIET_UI_SETTINGS_SCREEN_HPP
#define CARO_VIET_UI_SETTINGS_SCREEN_HPP

#include "Screen.hpp"
#include "Button.hpp"
#include "PixelBackground.hpp"
#include <SFML/Graphics.hpp>
#include <memory>

namespace caro {

class SettingsScreen : public Screen {
public:
    SettingsScreen();
    void handleEvent(const sf::Event& event, sf::RenderWindow& window) override;
    void update(float dt) override;
    void render(sf::RenderWindow& window) override;

private:
    PixelBackground background_;
    sf::Text txtTitle_;

    sf::Text txtSoundLabel_;
    sf::Text txtMusicLabel_;
    sf::Text txtAnimLabel_;
    sf::Text txtCoordLabel_;

    std::unique_ptr<Button> btnSoundToggle_;
    std::unique_ptr<Button> btnMusicToggle_;
    std::unique_ptr<Button> btnAnimToggle_;
    std::unique_ptr<Button> btnCoordToggle_;
    std::unique_ptr<Button> btnBack_;

    void updateLocalization();
    void updateToggleLabels();
    std::string t(const std::string& key) const;
    sf::String toSfString(const std::string& utf8Str) const;
};

} // namespace caro

#endif // CARO_VIET_UI_SETTINGS_SCREEN_HPP
