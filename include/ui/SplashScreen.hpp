#ifndef CARO_VIET_UI_SPLASH_SCREEN_HPP
#define CARO_VIET_UI_SPLASH_SCREEN_HPP

#include "Screen.hpp"
#include "PixelBackground.hpp"
#include <SFML/Graphics.hpp>

namespace caro {

class SplashScreen : public Screen {
public:
    SplashScreen();
    void handleEvent(const sf::Event& event, sf::RenderWindow& window) override;
    void update(float dt) override;
    void render(sf::RenderWindow& window) override;

private:
    PixelBackground background_;
    sf::Text txtPrompt_;
    
    float timer_;
    float promptBlinkTimer_;
    bool showPrompt_;
};

} // namespace caro

#endif // CARO_VIET_UI_SPLASH_SCREEN_HPP
