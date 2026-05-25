#include "ui/LoadGameScreen.hpp"
#include "ui/GameplayScreen.hpp"
#include "ui/Layout.hpp"
#include "ui/MainMenuScreen.hpp"
#include "ui/ResourceManager.hpp"
#include "ui/ScreenManager.hpp"
#include "core/SaveManager.hpp"
#include <algorithm>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

namespace caro {

namespace {

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

} // namespace

LoadGameScreen::LoadGameScreen()
    : selectedIndex_(-1),
      scrollOffset_(0),
      txtTitle_(ResourceManager::instance().getFont()),
      txtEmpty_(ResourceManager::instance().getFont()),
      txtError_(ResourceManager::instance().getFont()) {
    const sf::Font& font = ResourceManager::instance().getFont();

    txtTitle_.setString(toSfString(t("load.title")));
    txtTitle_.setCharacterSize(56);
    txtTitle_.setFillColor(sf::Color(0xF6, 0xD9, 0x5F));
    centerText(txtTitle_, {960.f, 185.f});

    txtEmpty_.setString(toSfString(t("load.empty")));
    txtEmpty_.setCharacterSize(34);
    txtEmpty_.setFillColor(sf::Color(0xF4, 0xF1, 0xE8));
    centerText(txtEmpty_, {960.f, 510.f});

    txtError_.setString("");
    txtError_.setCharacterSize(26);
    txtError_.setFillColor(sf::Color(0xD4, 0x5D, 0x5D));

    btnPrevPage_ = std::make_unique<Button>("<", sf::Vector2f(90.f, 58.f), font, [this]() {
        if (scrollOffset_ > 0) {
            --scrollOffset_;
        }
    });
    btnPrevPage_->setPosition({770.f, 815.f});

    btnNextPage_ = std::make_unique<Button>(">", sf::Vector2f(90.f, 58.f), font, [this]() {
        if (scrollOffset_ + 7 < static_cast<int>(saves_.size())) {
            ++scrollOffset_;
        }
    });
    btnNextPage_->setPosition({1150.f, 815.f});

    btnLoadSelected_ = std::make_unique<Button>(toSfString(t("load.load_selected")), sf::Vector2f(330.f, 72.f), font, [this]() {
        loadSelected();
    });
    btnLoadSelected_->setPosition({820.f, 900.f});
    btnLoadSelected_->setColors(sf::Color(0x4C, 0x8B, 0x57), sf::Color(0x5C, 0x9B, 0x67), sf::Color(0xF4, 0xF1, 0xE8));

    btnBack_ = std::make_unique<Button>(toSfString(t("load.back")), sf::Vector2f(260.f, 72.f), font, [this]() {
        returnToMenu();
    });
    btnBack_->setPosition({1130.f, 900.f});
    btnBack_->setColors(sf::Color(0xA9, 0x48, 0x48), sf::Color(0xC2, 0x56, 0x56), sf::Color(0xF4, 0xF1, 0xE8));

    scanSaves();
    buildRows();
}

void LoadGameScreen::scanSaves() {
    saves_.clear();
    std::vector<std::filesystem::directory_entry> entries;
    const std::filesystem::path savesDir("./saves");

    std::error_code ec;
    if (!std::filesystem::exists(savesDir, ec) || !std::filesystem::is_directory(savesDir, ec)) {
        return;
    }

    for (const auto& entry : std::filesystem::directory_iterator(savesDir, ec)) {
        if (!ec && entry.is_regular_file() && entry.path().extension() == ".txt") {
            entries.push_back(entry);
        }
    }

    std::sort(entries.begin(), entries.end(), [](const auto& a, const auto& b) {
        std::error_code eca;
        std::error_code ecb;
        return std::filesystem::last_write_time(a.path(), eca) > std::filesystem::last_write_time(b.path(), ecb);
    });

    SaveManager saveManager;
    for (const auto& entry : entries) {
        SaveItem item;
        item.path = entry.path();
        item.displayName = entry.path().stem().string();
        item.modifiedLine = formatModifiedTime(entry.path());

        auto result = saveManager.load(entry.path().string());
        item.valid = result.success();
        if (item.valid) {
            item.snapshot = result.snapshot;
            if (!item.snapshot.saveName.empty()) {
                item.displayName = item.snapshot.saveName;
            }
            item.detailLine = describeSnapshot(item.snapshot);
        } else {
            item.errorMessage = result.errorMessage;
            item.detailLine = t("load.invalid") + result.errorMessage;
        }

        saves_.push_back(std::move(item));
    }
}

void LoadGameScreen::buildRows() {
    const sf::Font& font = ResourceManager::instance().getFont();
    for (std::size_t i = 0; i < saves_.size(); ++i) {
        auto label = saves_[i].displayName + "   " + saves_[i].detailLine + "   " + saves_[i].modifiedLine;
        saves_[i].button = std::make_unique<Button>(toSfString(label), sf::Vector2f(1040.f, 64.f), font, [this, i]() {
            selectedIndex_ = static_cast<int>(i);
            txtError_.setString("");
        });
        saves_[i].button->setCharacterSize(24);
        if (!saves_[i].valid) {
            saves_[i].button->setColors(sf::Color(0x73, 0x35, 0x35), sf::Color(0x8B, 0x45, 0x45), sf::Color(0xF4, 0xF1, 0xE8));
        }
    }
}

void LoadGameScreen::handleEvent(const sf::Event& event, sf::RenderWindow& window) {
    constexpr int visibleRows = 7;
    for (int row = 0; row < visibleRows; ++row) {
        const int index = scrollOffset_ + row;
        if (index >= static_cast<int>(saves_.size())) {
            break;
        }
        if (saves_[static_cast<std::size_t>(index)].button) {
            saves_[static_cast<std::size_t>(index)].button->handleEvent(event, window);
        }
    }
    btnPrevPage_->handleEvent(event, window);
    btnNextPage_->handleEvent(event, window);
    btnLoadSelected_->handleEvent(event, window);
    btnBack_->handleEvent(event, window);
}

void LoadGameScreen::update(float dt) {
    background_.update(dt);
    constexpr int visibleRows = 7;
    const float startY = 300.f;
    const float rowGap = 78.f;
    for (int row = 0; row < visibleRows; ++row) {
        const int index = scrollOffset_ + row;
        if (index >= static_cast<int>(saves_.size())) {
            break;
        }
        auto& item = saves_[static_cast<std::size_t>(index)];
        if (item.button) {
            item.button->setPosition({960.f, startY + rowGap * static_cast<float>(row)});
            item.button->setState(index == selectedIndex_ ? ButtonState::Selected : ButtonState::Normal);
            item.button->update(dt);
        }
    }
    btnPrevPage_->setState(scrollOffset_ > 0 ? ButtonState::Normal : ButtonState::Disabled);
    btnNextPage_->setState(scrollOffset_ + visibleRows < static_cast<int>(saves_.size()) ? ButtonState::Normal : ButtonState::Disabled);
    btnPrevPage_->update(dt);
    btnNextPage_->update(dt);
    btnLoadSelected_->update(dt);
    btnBack_->update(dt);
}

void LoadGameScreen::render(sf::RenderWindow& window) {
    background_.render(window);

    sf::RectangleShape shadow({1160.f, 810.f});
    shadow.setOrigin({580.f, 405.f});
    shadow.setPosition({972.f, 552.f});
    shadow.setFillColor(sf::Color(0, 0, 0, 95));
    window.draw(shadow);

    sf::RectangleShape panel({1160.f, 810.f});
    panel.setOrigin({580.f, 405.f});
    panel.setPosition({960.f, 540.f});
    panel.setFillColor(sf::Color(0x15, 0x22, 0x38, 232));
    panel.setOutlineThickness(5.f);
    panel.setOutlineColor(sf::Color(0xF6, 0xD9, 0x5F));
    window.draw(panel);

    window.draw(txtTitle_);

    if (saves_.empty()) {
        window.draw(txtEmpty_);
    } else {
        constexpr int visibleRows = 7;
        for (int row = 0; row < visibleRows; ++row) {
            const int index = scrollOffset_ + row;
            if (index >= static_cast<int>(saves_.size())) {
                break;
            }
            if (saves_[static_cast<std::size_t>(index)].button) {
                saves_[static_cast<std::size_t>(index)].button->render(window);
            }
        }
        btnPrevPage_->render(window);
        btnNextPage_->render(window);
    }

    if (txtError_.getString() != "") {
        fitTextToWidth(txtError_, 960.f, 20);
        centerText(txtError_, {960.f, 825.f});
        window.draw(txtError_);
    }

    btnLoadSelected_->render(window);
    btnBack_->render(window);
}

void LoadGameScreen::loadSelected() {
    if (selectedIndex_ < 0 || selectedIndex_ >= static_cast<int>(saves_.size())) {
        ResourceManager::instance().playSound("invalid");
        txtError_.setString(toSfString(t("load.select_prompt")));
        return;
    }

    const auto& item = saves_[static_cast<std::size_t>(selectedIndex_)];
    if (!item.valid) {
        ResourceManager::instance().playSound("invalid");
        txtError_.setString(toSfString(t("load.failed") + item.errorMessage));
        return;
    }

    ResourceManager::instance().playSound("save");
    ScreenManager::instance().requestChange(std::make_unique<GameplayScreen>(item.snapshot, item.path.string()));
}

void LoadGameScreen::returnToMenu() {
    ResourceManager::instance().playSound("click");
    ScreenManager::instance().requestChange(std::make_unique<MainMenuScreen>());
}

std::string LoadGameScreen::formatModifiedTime(const std::filesystem::path& path) const {
    std::error_code ec;
    const auto fileTime = std::filesystem::last_write_time(path, ec);
    if (ec) {
        return "";
    }

    const auto systemTime = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
        fileTime - decltype(fileTime)::clock::now() + std::chrono::system_clock::now());
    const auto time = std::chrono::system_clock::to_time_t(systemTime);
    std::tm tm = *std::localtime(&time);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M");
    return oss.str();
}

std::string LoadGameScreen::describeSnapshot(const GameSnapshot& snapshot) const {
    std::string mode = snapshot.mode == GameMode::SinglePlayer ? t("load.mode_bot") : t("load.mode_2p");
    std::string difficulty = "N/A";
    if (snapshot.mode == GameMode::SinglePlayer) {
        if (snapshot.difficulty == AIDifficulty::Easy) difficulty = t("menu.easy");
        else if (snapshot.difficulty == AIDifficulty::Normal) difficulty = t("menu.normal");
        else difficulty = t("menu.hard");
    }
    return mode + " | " + t("load.difficulty") + difficulty + " | " + t("game.moves") + std::to_string(snapshot.moves.size());
}

std::string LoadGameScreen::t(const std::string& key) const {
    return ResourceManager::instance().getTranslation(key);
}

sf::String LoadGameScreen::toSfString(const std::string& utf8Str) const {
    return ResourceManager::toSfString(utf8Str);
}

} // namespace caro
