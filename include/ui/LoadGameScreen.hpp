#ifndef CARO_VIET_UI_LOAD_GAME_SCREEN_HPP
#define CARO_VIET_UI_LOAD_GAME_SCREEN_HPP

#include "Screen.hpp"
#include "Button.hpp"
#include "PixelBackground.hpp"
#include "core/GameSnapshot.hpp"
#include <filesystem>
#include <memory>
#include <string>
#include <vector>

namespace caro {

class LoadGameScreen : public Screen {
public:
    LoadGameScreen();

    void handleEvent(const sf::Event& event, sf::RenderWindow& window) override;
    void update(float dt) override;
    void render(sf::RenderWindow& window) override;

private:
    struct SaveItem {
        std::filesystem::path path;
        std::string displayName;
        std::string detailLine;
        std::string modifiedLine;
        bool valid = false;
        std::string errorMessage;
        GameSnapshot snapshot;
        std::unique_ptr<Button> button;
    };

    void scanSaves();
    void buildRows();
    void loadSelected();
    void returnToMenu();
    std::string formatModifiedTime(const std::filesystem::path& path) const;
    std::string describeSnapshot(const GameSnapshot& snapshot) const;
    std::string t(const std::string& key) const;
    sf::String toSfString(const std::string& utf8Str) const;

    PixelBackground background_;
    std::vector<SaveItem> saves_;
    int selectedIndex_;
    int scrollOffset_;
    sf::Text txtTitle_;
    sf::Text txtEmpty_;
    sf::Text txtError_;
    std::unique_ptr<Button> btnPrevPage_;
    std::unique_ptr<Button> btnNextPage_;
    std::unique_ptr<Button> btnLoadSelected_;
    std::unique_ptr<Button> btnBack_;
};

} // namespace caro

#endif // CARO_VIET_UI_LOAD_GAME_SCREEN_HPP
