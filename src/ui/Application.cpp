#include "ui/Application.hpp"
#include "ui/ScreenManager.hpp"
#include "ui/SplashScreen.hpp"
#include "ui/ResourceManager.hpp"
#include "ui/Layout.hpp"
#include <iostream>
#include <array>

namespace caro {

Application::Application() {
    window_.create(sf::VideoMode({ui::kVirtualWidth, ui::kVirtualHeight}), L"Caro Việt - C++ Desktop Game", sf::Style::Default);
    window_.setFramerateLimit(60);
    mainView_ = sf::View(sf::FloatRect({0.f, 0.f}, {ui::kVirtualWidthF, ui::kVirtualHeightF}));
    updateView();

    for (const auto& textureName : {"background", "victory_background", "cloud", "cup", "sun", "tree_1", "tree_2"}) {
        ResourceManager::instance().getTexture(textureName);
    }

    // Load initial screen
    ScreenManager::instance().requestChange(std::make_unique<SplashScreen>());
    ScreenManager::instance().applyPendingChange();
}

void Application::run() {
    sf::Clock clock;

    while (window_.isOpen()) {
        // Event loop
        while (const std::optional<sf::Event> event = window_.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                ResourceManager::instance().stopAllSounds();
                window_.close();
                break;
            }

            if (event->is<sf::Event::Resized>()) {
                updateView();
            }

            if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                if (keyPressed->code == sf::Keyboard::Key::F3) {
                    showDebugOverlay_ = !showDebugOverlay_;
                }
            }

            if (ScreenManager::instance().current()) {
                ScreenManager::instance().current()->handleEvent(*event, window_);
                if (!window_.isOpen()) {
                    ResourceManager::instance().stopAllSounds();
                    break;
                }
            }
        }

        if (!window_.isOpen()) {
            break;
        }

        // Measure delta time
        float dt = clock.restart().asSeconds();
        if (dt > 0.1f) {
            dt = 0.1f; // Cap delta time to prevent physics/animation glitches during lags
        }

        // Update active screen
        if (ScreenManager::instance().current()) {
            ScreenManager::instance().current()->update(dt);
        }

        // Apply pending screen changes safely after update but before render
        ScreenManager::instance().applyPendingChange();

        // Render active screen
        if (ScreenManager::instance().current()) {
            ScreenManager::instance().current()->render(window_);
        }

        // Render F3 debug overlay if enabled
        if (showDebugOverlay_) {
            renderDebugOverlay();
        }

        window_.display();
    }
}

void Application::renderDebugOverlay() {
    // Draw translucent background panel
    sf::RectangleShape panel(sf::Vector2f(760.f, 430.f));
    panel.setPosition({20.f, 20.f});
    panel.setFillColor(sf::Color(0, 0, 0, 220));
    panel.setOutlineThickness(2.f);
    panel.setOutlineColor(sf::Color(255, 255, 255, 100));
    window_.draw(panel);

    const sf::Font& font = ResourceManager::instance().getFont();
    sf::Text text(font);
    text.setCharacterSize(14);

    // Title
    text.setString("CARO VIET ASSET DEBUG (F3)");
    text.setFillColor(sf::Color(255, 215, 0)); // Gold
    text.setPosition({30.f, 30.f});
    window_.draw(text);

    float yOffset = 60.f;
    auto& resources = ResourceManager::instance();

    text.setCharacterSize(12);
    text.setFillColor(sf::Color::White);
    text.setString("CWD: " + resources.getCurrentWorkingDirectory());
    text.setPosition({30.f, yOffset});
    window_.draw(text);
    yOffset += 22.f;

    text.setString("Asset roots: " + resources.getAssetRootSummary());
    text.setPosition({30.f, yOffset});
    window_.draw(text);
    yOffset += 30.f;

    struct TextureDebugItem {
        const char* label;
        const char* textureName;
    };
    const std::array<TextureDebugItem, 7> requiredTextures = {
        TextureDebugItem{"BG_MAIN", "background"},
        TextureDebugItem{"BG_VICTORY", "victory_background"},
        TextureDebugItem{"CLOUD", "cloud"},
        TextureDebugItem{"SUN", "sun"},
        TextureDebugItem{"TREE_1", "tree_1"},
        TextureDebugItem{"TREE_2", "tree_2"},
        TextureDebugItem{"CUP", "cup"}
    };

    text.setCharacterSize(14);
    for (const auto& item : requiredTextures) {
        const bool loaded = resources.wasAssetLoaded("Texture", item.textureName);
        text.setFillColor(loaded ? sf::Color(50, 205, 50) : sf::Color(220, 20, 60));
        text.setString(std::string(item.label) + ": " + (loaded ? "loaded" : "missing"));
        text.setPosition({30.f, yOffset});
        window_.draw(text);
        yOffset += 20.f;
    }

    yOffset += 10.f;
    const auto& statuses = resources.getAssetStatuses();

    if (statuses.empty()) {
        text.setString("No assets queried yet.");
        text.setFillColor(sf::Color::White);
        text.setPosition({30.f, yOffset});
        window_.draw(text);
    } else {
        for (const auto& status : statuses) {
            std::string statusStr = "[" + status.type.substr(0, 1) + "] " + status.name + ": ";
            if (status.success) {
                statusStr += "LOADED (" + status.resolvedPath + ")";
                text.setFillColor(sf::Color(50, 205, 50)); // Lime Green
            } else {
                statusStr += "FAILED / PROCEDURAL FALLBACK";
                text.setFillColor(sf::Color(220, 20, 60)); // Crimson Red
            }
            text.setString(statusStr);
            text.setPosition({30.f, yOffset});
            window_.draw(text);
            yOffset += 20.f;
            if (yOffset > 430.f) {
                break; // Cap text display to fit inside panel
            }
        }
    }
}

void Application::updateView() {
    const auto size = window_.getSize();
    if (size.x == 0 || size.y == 0) {
        return;
    }

    const float windowRatio = static_cast<float>(size.x) / static_cast<float>(size.y);
    const float viewRatio = ui::kVirtualWidthF / ui::kVirtualHeightF;
    float viewportWidth = 1.f;
    float viewportHeight = 1.f;
    float viewportLeft = 0.f;
    float viewportTop = 0.f;

    if (windowRatio > viewRatio) {
        viewportWidth = viewRatio / windowRatio;
        viewportLeft = (1.f - viewportWidth) * 0.5f;
    } else if (windowRatio < viewRatio) {
        viewportHeight = windowRatio / viewRatio;
        viewportTop = (1.f - viewportHeight) * 0.5f;
    }

    mainView_.setSize({ui::kVirtualWidthF, ui::kVirtualHeightF});
    mainView_.setCenter({ui::kVirtualWidthF * 0.5f, ui::kVirtualHeightF * 0.5f});
    mainView_.setViewport(sf::FloatRect({viewportLeft, viewportTop}, {viewportWidth, viewportHeight}));
    window_.setView(mainView_);
}

} // namespace caro
