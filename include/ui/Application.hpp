#ifndef CARO_VIET_UI_APPLICATION_HPP
#define CARO_VIET_UI_APPLICATION_HPP

#include <SFML/Graphics.hpp>

namespace caro {

class Application {
public:
    Application();
    void run();

private:
    void updateView();
    void renderDebugOverlay();

    sf::RenderWindow window_;
    sf::View mainView_;
    bool showDebugOverlay_ = false;
};

} // namespace caro

#endif // CARO_VIET_UI_APPLICATION_HPP
