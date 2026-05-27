#include "ui/ResourceManager.hpp"
#include <iostream>
#include <cmath>
#include <algorithm>
#include <vector>
#include <sstream>

namespace caro {

ResourceManager& ResourceManager::instance() {
    static ResourceManager* inst = new ResourceManager();
    return *inst;
}

ResourceManager::ResourceManager()
    : assetRoots_(buildAssetRoots()),
      currentLanguage_(Language::EN) {
}

std::vector<std::filesystem::path> ResourceManager::buildAssetRoots() const {
    std::vector<std::filesystem::path> roots;
    const auto cwd = std::filesystem::current_path();
    roots.push_back(cwd / "assets");
    roots.push_back(cwd / "../assets");
    roots.push_back(cwd / "../../assets");
    roots.push_back("assets");
    roots.push_back("../assets");
    roots.push_back("../../assets");
    return roots;
}

std::vector<std::filesystem::path> ResourceManager::candidatePaths(const std::string& relativePath) const {
    std::vector<std::filesystem::path> paths;
    for (const auto& root : assetRoots_) {
        paths.push_back(root / relativePath);
    }
    return paths;
}

std::string ResourceManager::getTranslation(const std::string& key) const {
    static const std::unordered_map<std::string, std::unordered_map<Language, std::string>> translations = {
        {"menu.title", {{Language::EN, "CARO VIỆT"}, {Language::VI, "CARO VIỆT"}}},
        {"menu.mode_label", {{Language::EN, "GAME MODE"}, {Language::VI, "CHẾ ĐỘ CHƠI"}}},
        {"menu.single_player", {{Language::EN, "vs Bot"}, {Language::VI, "Đấu Máy"}}},
        {"menu.two_players", {{Language::EN, "2 Players"}, {Language::VI, "2 Người"}}},
        {"menu.difficulty_label", {{Language::EN, "DIFFICULTY"}, {Language::VI, "ĐỘ KHÓ"}}},
        {"menu.easy", {{Language::EN, "Easy"}, {Language::VI, "Dễ"}}},
        {"menu.normal", {{Language::EN, "Medium"}, {Language::VI, "Thường"}}},
        {"menu.hard", {{Language::EN, "Hard"}, {Language::VI, "Khó"}}},
        {"menu.difficulty_hint", {{Language::EN, "Difficulty is used only in Single Player mode."}, {Language::VI, "Độ khó chỉ dùng trong chế độ chơi với Máy."}}},
        {"menu.new_game", {{Language::EN, "New Game"}, {Language::VI, "Ván Mới"}}},
        {"menu.load_game", {{Language::EN, "Load Game"}, {Language::VI, "Tải Ván Cũ"}}},
        {"menu.how_to_play", {{Language::EN, "How To Play"}, {Language::VI, "Hướng Dẫn"}}},
        {"menu.settings", {{Language::EN, "Settings"}, {Language::VI, "Cài Đặt"}}},
        {"menu.exit", {{Language::EN, "Exit Game"}, {Language::VI, "Thoát Game"}}},
        {"menu.exit_confirm", {{Language::EN, "Exit Caro Viet?"}, {Language::VI, "Thoát Caro Việt?"}}},
        {"menu.cancel", {{Language::EN, "Cancel"}, {Language::VI, "Hủy"}}},
        {"menu.confirm_exit", {{Language::EN, "Exit"}, {Language::VI, "Thoát"}}},
        {"menu.no_saves", {{Language::EN, "No save files found in './saves/'"}, {Language::VI, "Không tìm thấy file lưu trong './saves/'"}}},
        {"menu.load_failed", {{Language::EN, "Failed to load save: "}, {Language::VI, "Không thể tải file lưu: "}}},
        
        // Gameplay Sidebar Panel
        {"game.panel_title", {{Language::EN, "MATCH INFO"}, {Language::VI, "THÔNG TIN TRẬN ĐẤU"}}},
        {"game.mode_sp", {{Language::EN, "Mode: Human vs Bot"}, {Language::VI, "Chế độ: Đấu với Máy"}}},
        {"game.mode_2p", {{Language::EN, "Mode: Local 2 Players"}, {Language::VI, "Chế độ: 2 Người chơi"}}},
        {"game.diff_easy", {{Language::EN, "Difficulty: Easy"}, {Language::VI, "Độ khó: Dễ"}}},
        {"game.diff_normal", {{Language::EN, "Difficulty: Normal"}, {Language::VI, "Độ khó: Trung bình"}}},
        {"game.diff_hard", {{Language::EN, "Difficulty: Hard"}, {Language::VI, "Độ khó: Khó"}}},
        {"game.turn_you", {{Language::EN, "Turn: You (X)"}, {Language::VI, "Lượt: Bạn (X)"}}},
        {"game.turn_p1", {{Language::EN, "Turn: Player 1 (X)"}, {Language::VI, "Lượt: Người chơi 1 (X)"}}},
        {"game.turn_p2", {{Language::EN, "Turn: Player 2 (O)"}, {Language::VI, "Lượt: Người chơi 2 (O)"}}},
        {"game.turn_bot", {{Language::EN, "Turn: Bot (O)"}, {Language::VI, "Lượt: Máy (O)"}}},
        {"game.moves", {{Language::EN, "Moves: "}, {Language::VI, "Số nước đi: "}}},
        {"game.last_move_none", {{Language::EN, "Last: None"}, {Language::VI, "Nước cuối: Chưa có"}}},
        {"game.status_your_turn", {{Language::EN, "Your Turn"}, {Language::VI, "Lượt của bạn"}}},
        {"game.status_bot_thinking", {{Language::EN, "Bot is thinking..."}, {Language::VI, "Máy đang nghĩ..."}}},
        {"game.status_p1_turn", {{Language::EN, "Player 1's Turn"}, {Language::VI, "Lượt Người chơi 1"}}},
        {"game.status_p2_turn", {{Language::EN, "Player 2's Turn"}, {Language::VI, "Lượt Người chơi 2"}}},
        
        // Sidebar buttons
        {"game.btn_menu", {{Language::EN, "Menu"}, {Language::VI, "Menu"}}},
        {"game.btn_pause", {{Language::EN, "Pause"}, {Language::VI, "Tạm Dừng"}}},
        {"game.btn_resume", {{Language::EN, "Resume"}, {Language::VI, "Tiếp Tục"}}},
        {"game.btn_save", {{Language::EN, "Save"}, {Language::VI, "Lưu Ván"}}},
        {"game.save_success", {{Language::EN, "Game saved successfully!"}, {Language::VI, "Lưu ván thành công!"}}},
        {"game.save_failed", {{Language::EN, "Save failed: "}, {Language::VI, "Lưu ván thất bại: "}}},

        // Sidebar gameover buttons
        {"game.btn_restart_match", {{Language::EN, "Restart Match"}, {Language::VI, "Đấu Lại"}}},
        {"game.btn_main_menu", {{Language::EN, "Main Menu"}, {Language::VI, "Menu Chính"}}},
        
        // Confirmation Overlay
        {"confirm.title", {{Language::EN, "Return to main menu? Unsaved progress may be lost."}, {Language::VI, "Quay lại menu chính? Tiến trình chưa lưu sẽ bị mất."}}},
        {"confirm.title_clean", {{Language::EN, "Return to main menu?"}, {Language::VI, "Quay lại menu chính?"}}},
        {"confirm.cancel", {{Language::EN, "Cancel"}, {Language::VI, "Hủy"}}},
        {"confirm.save_menu", {{Language::EN, "Save & Return"}, {Language::VI, "Lưu & Về Menu"}}},
        {"confirm.menu_no_save", {{Language::EN, "Menu Without Saving"}, {Language::VI, "Về Menu không lưu"}}},
        {"confirm.back_menu", {{Language::EN, "Back to Menu"}, {Language::VI, "Về Menu"}}},
        
        // Save/Load
        {"save.title", {{Language::EN, "Save Game"}, {Language::VI, "Lưu Ván"}}},
        {"save.hint", {{Language::EN, "Enter a save name. Leave empty for timestamp."}, {Language::VI, "Nhập tên file lưu. Để trống để dùng thời gian."}}},
        {"load.title", {{Language::EN, "Load Game"}, {Language::VI, "Tải Ván Cũ"}}},
        {"load.empty", {{Language::EN, "No saved games found."}, {Language::VI, "Không tìm thấy ván đã lưu."}}},
        {"load.load_selected", {{Language::EN, "Load Selected"}, {Language::VI, "Tải mục đã chọn"}}},
        {"load.back", {{Language::EN, "Back"}, {Language::VI, "Quay lại"}}},
        {"load.select_prompt", {{Language::EN, "Select a save file first."}, {Language::VI, "Hãy chọn một file lưu trước."}}},
        {"load.failed", {{Language::EN, "Could not load save: "}, {Language::VI, "Không thể tải file lưu: "}}},
        {"load.invalid", {{Language::EN, "Invalid save: "}, {Language::VI, "File lưu lỗi: "}}},
        {"load.mode_bot", {{Language::EN, "Human vs Bot"}, {Language::VI, "Đấu Máy"}}},
        {"load.mode_2p", {{Language::EN, "2 Players"}, {Language::VI, "2 Người"}}},
        {"load.difficulty", {{Language::EN, "Difficulty: "}, {Language::VI, "Độ khó: "}}},
        
        // Pause Overlay
        {"pause.title", {{Language::EN, "PAUSED"}, {Language::VI, "TẠM DỰNG"}}},
        {"pause.resume", {{Language::EN, "Resume Game"}, {Language::VI, "Tiếp Tục Chơi"}}},
        
        // Victory Card
        {"victory.title_victory", {{Language::EN, "VICTORY!"}, {Language::VI, "CHIẾN THẮNG!"}}},
        {"victory.title_defeat", {{Language::EN, "DEFEAT!"}, {Language::VI, "THẤT BẠI!"}}},
        {"victory.title_draw", {{Language::EN, "DRAW MATCH!"}, {Language::VI, "HÒA CỜ!"}}},
        {"victory.sub_bot_defeat", {{Language::EN, "Congratulations! You have defeated the Bot."}, {Language::VI, "Chúc mừng! Bạn đã đánh bại Máy."}}},
        {"victory.sub_bot_win", {{Language::EN, "The Bot has won the match. Better luck next time!"}, {Language::VI, "Máy đã thắng. Chúc bạn may mắn lần sau!"}}},
        {"victory.sub_p1_win", {{Language::EN, "Player 1 (X) has won the match!"}, {Language::VI, "Người chơi 1 (X) đã thắng trận đấu!"}}},
        {"victory.sub_p2_win", {{Language::EN, "Player 2 (O) has won the match!"}, {Language::VI, "Người chơi 2 (O) đã thắng trận đấu!"}}},
        {"victory.sub_draw", {{Language::EN, "The board is full. The match ends in a draw!"}, {Language::VI, "Bàn cờ đã đầy. Trận đấu kết thúc với kết quả hòa!"}}},
        {"victory.btn_replay", {{Language::EN, "Replay"}, {Language::VI, "Đấu Lại"}}},
        {"victory.btn_menu", {{Language::EN, "Main Menu"}, {Language::VI, "Menu Chính"}}},
    };

    auto it = translations.find(key);
    if (it != translations.end()) {
        auto langIt = it->second.find(currentLanguage_);
        if (langIt != it->second.end()) {
            return langIt->second;
        }
    }
    return key;
}

const sf::Font& ResourceManager::getFont(const std::string& name) {
    auto it = fonts_.find(name);
    if (it != fonts_.end()) {
        return it->second;
    }

    sf::Font font;
    bool loaded = false;
    std::string foundPath = "";
    std::vector<std::string> attempted;

    for (const auto& relativePath : {std::string("fonts/dearpix-2-01.otf"), std::string("dearpix-2-01.otf")}) {
        for (const auto& path : candidatePaths(relativePath)) {
            attempted.push_back(path.string());
            if (!font.openFromFile(path.string())) {
                continue;
            }
            loaded = true;
            foundPath = path.string();
            break;
        }
        if (loaded) break;
    }

    if (!loaded) {
        std::cerr << "[Asset] Failed to load font " << name << ". Attempted:";
        for (const auto& path : attempted) {
            std::cerr << "\n  - " << path;
        }
        std::cerr << "\n[Asset] Trying system font fallback..." << std::endl;
        std::vector<std::string> fallbackPaths = {
            "/System/Library/Fonts/Supplemental/dearpix-2-01.otf",
            "/System/Library/Fonts/dearpix-2-01.otf",
            "/Library/Fonts/dearpix-2-01.otf",
            "C:\\Windows\\Fonts\\dearpix-2-01.otf"
        };
        for (const auto& path : fallbackPaths) {
            if (font.openFromFile(path)) {
                loaded = true;
                foundPath = path;
                break;
            }
        }
    }

    if (loaded) {
        fonts_[name] = std::move(font);
        std::cout << "[Asset] Loaded font " << name << " from " << foundPath << std::endl;
        recordAssetStatus("Font", name, true, foundPath);
        return fonts_[name];
    } else {
        std::cerr << "[Asset] Failed to load any usable font for " << name << std::endl;
        recordAssetStatus("Font", name, false, "");
        fonts_[name] = sf::Font();
        return fonts_[name];
    }
}

const sf::Texture& ResourceManager::getTexture(const std::string& name) {
    auto it = textures_.find(name);
    if (it != textures_.end()) {
        return it->second;
    }

    sf::Texture texture;
    bool loaded = false;
    std::string foundPath = "";
    std::vector<std::string> relativePaths;
    std::vector<std::string> attempted;

    if (name.find('/') != std::string::npos || name.find('\\') != std::string::npos) {
        relativePaths.push_back(name);
    } else {
        relativePaths.push_back("textures/" + name + ".png");
        relativePaths.push_back(name + ".png");
    }

    for (const auto& relativePath : relativePaths) {
        for (const auto& path : candidatePaths(relativePath)) {
            attempted.push_back(path.string());
            if (!texture.loadFromFile(path.string())) {
                continue;
            }
            loaded = true;
            foundPath = path.string();
            break;
        }
        if (loaded) break;
    }

    if (loaded) {
        texture.setSmooth(false);
        textures_[name] = std::move(texture);
        std::cout << "[Asset] Loaded texture " << name << " from " << foundPath << std::endl;
        recordAssetStatus("Texture", name, true, foundPath);
        return textures_[name];
    } else {
        std::cerr << "[Asset] Failed to load texture " << name << ". Attempted:";
        for (const auto& path : attempted) {
            std::cerr << "\n  - " << path;
        }
        std::cerr << std::endl;
        recordAssetStatus("Texture", name, false, "");
        generateProceduralTexture(name);
        return textures_[name];
    }
}

const sf::SoundBuffer& ResourceManager::getSoundBuffer(const std::string& name) {
    auto it = sounds_.find(name);
    if (it != sounds_.end()) {
        return it->second;
    }

    sf::SoundBuffer buffer;
    bool loaded = false;
    std::string foundPath = "";
    std::vector<std::string> attempted;

    for (const auto& relativePath : {std::string("audio/") + name + ".wav", name + ".wav"}) {
        for (const auto& path : candidatePaths(relativePath)) {
            attempted.push_back(path.string());
            if (!buffer.loadFromFile(path.string())) {
                continue;
            }
            loaded = true;
            foundPath = path.string();
            break;
        }
        if (loaded) break;
    }

    if (loaded) {
        sounds_[name] = std::move(buffer);
        std::cout << "[Asset] Loaded sound " << name << " from " << foundPath << std::endl;
        recordAssetStatus("Sound", name, true, foundPath);
        return sounds_[name];
    } else {
        std::cerr << "[Asset] Failed to load sound " << name << ". Attempted:";
        for (const auto& path : attempted) {
            std::cerr << "\n  - " << path;
        }
        std::cerr << std::endl;
        recordAssetStatus("Sound", name, false, "");
        generateProceduralSound(name);
        return sounds_[name];
    }
}

void ResourceManager::generateProceduralTexture(const std::string& name) {
    if (name == "board_bg") {
        unsigned int size = 600;
        sf::RenderTexture rt;
        if (!rt.resize({size, size})) return;
        rt.clear(sf::Color(0xF4, 0xEE, 0xD7)); // Cream color #F4EED7

        sf::VertexArray lines(sf::PrimitiveType::Lines);
        sf::Color gridColor(0xB7, 0xAA, 0x86, 128); // #B7AA86 semi-transparent

        float cellSize = static_cast<float>(size) / 12.0f;
        for (int i = 0; i <= 12; ++i) {
            float pos = i * cellSize;
            
            sf::Vertex v1;
            v1.position = {pos, 0.f};
            v1.color = gridColor;
            
            sf::Vertex v2;
            v2.position = {pos, static_cast<float>(size)};
            v2.color = gridColor;

            sf::Vertex h1;
            h1.position = {0.f, pos};
            h1.color = gridColor;

            sf::Vertex h2;
            h2.position = {static_cast<float>(size), pos};
            h2.color = gridColor;

            lines.append(v1);
            lines.append(v2);
            lines.append(h1);
            lines.append(h2);
        }
        rt.draw(lines);
        rt.display();
        textures_[name] = rt.getTexture();
    }
    else if (name == "x_mark") {
        unsigned int size = 64;
        sf::RenderTexture rt;
        if (!rt.resize({size, size})) return;
        rt.clear(sf::Color::Transparent);

        sf::RectangleShape line1(sf::Vector2f(72.f, 6.f));
        line1.setOrigin({36.f, 3.f});
        line1.setPosition({32.f, 32.f});
        line1.setRotation(sf::degrees(45.f));
        line1.setFillColor(sf::Color(0xD4, 0x5D, 0x5D)); // #D45D5D red

        sf::RectangleShape line2(sf::Vector2f(72.f, 6.f));
        line2.setOrigin({36.f, 3.f});
        line2.setPosition({32.f, 32.f});
        line2.setRotation(sf::degrees(135.f));
        line2.setFillColor(sf::Color(0xD4, 0x5D, 0x5D)); // #D45D5D red

        rt.draw(line1);
        rt.draw(line2);
        rt.display();
        textures_[name] = rt.getTexture();
    }
    else if (name == "o_mark") {
        unsigned int size = 64;
        sf::RenderTexture rt;
        if (!rt.resize({size, size})) return;
        rt.clear(sf::Color::Transparent);

        sf::CircleShape circle(22.f);
        circle.setOrigin({22.f, 22.f});
        circle.setPosition({32.f, 32.f});
        circle.setFillColor(sf::Color::Transparent);
        circle.setOutlineColor(sf::Color(0x5E, 0x8C, 0xE6)); // #5E8CE6 blue
        circle.setOutlineThickness(6.f);

        rt.draw(circle);
        rt.display();
        textures_[name] = rt.getTexture();
    }
    else {
        // Fallback dummy textures - sky-blue color as requested by Requirement 7
        unsigned int size = 32;
        sf::RenderTexture rt;
        if (rt.resize({size, size})) {
            rt.clear(sf::Color(135, 206, 235)); // Sky-blue fallback #87CEEB
            rt.display();
            textures_[name] = rt.getTexture();
            textures_[name].setSmooth(false);
        }
    }
}

void ResourceManager::generateProceduralSound(const std::string& name) {
    unsigned int sampleRate = 44100;
    std::vector<std::int16_t> samples;

    if (name == "click") {
        float duration = 0.05f;
        float freq = 800.0f;
        unsigned int numSamples = static_cast<unsigned int>(sampleRate * duration);
        samples.resize(numSamples);
        for (unsigned int i = 0; i < numSamples; ++i) {
            float t = static_cast<float>(i) / sampleRate;
            float envelope = 1.0f - (static_cast<float>(i) / numSamples);
            samples[i] = static_cast<std::int16_t>(10000.0f * std::sin(2.0f * 3.14159265f * freq * t) * envelope);
        }
    }
    else if (name == "invalid") {
        float duration = 0.15f;
        float freq = 150.0f;
        unsigned int numSamples = static_cast<unsigned int>(sampleRate * duration);
        samples.resize(numSamples);
        for (unsigned int i = 0; i < numSamples; ++i) {
            float t = static_cast<float>(i) / sampleRate;
            float envelope = 1.0f - (static_cast<float>(i) / numSamples);
            samples[i] = static_cast<std::int16_t>(10000.0f * std::sin(2.0f * 3.14159265f * freq * t) * envelope);
        }
    }
    else if (name == "place") {
        float duration = 0.08f;
        float freq = 500.0f;
        unsigned int numSamples = static_cast<unsigned int>(sampleRate * duration);
        samples.resize(numSamples);
        for (unsigned int i = 0; i < numSamples; ++i) {
            float t = static_cast<float>(i) / sampleRate;
            float envelope = 1.0f - (static_cast<float>(i) / numSamples);
            samples[i] = static_cast<std::int16_t>(8000.0f * std::sin(2.0f * 3.14159265f * freq * t) * envelope);
        }
    }
    else if (name == "win") {
        float duration = 0.6f;
        unsigned int numSamples = static_cast<unsigned int>(sampleRate * duration);
        samples.resize(numSamples);
        for (unsigned int i = 0; i < numSamples; ++i) {
            float t = static_cast<float>(i) / sampleRate;
            float freq = 400.0f + 400.0f * (static_cast<float>(i) / numSamples);
            float envelope = 1.0f - (static_cast<float>(i) / numSamples);
            samples[i] = static_cast<std::int16_t>(10000.0f * std::sin(2.0f * 3.14159265f * freq * t) * envelope);
        }
    }
    else if (name == "save") {
        float duration = 0.3f;
        unsigned int numSamples = static_cast<unsigned int>(sampleRate * duration);
        samples.resize(numSamples);
        for (unsigned int i = 0; i < numSamples; ++i) {
            float t = static_cast<float>(i) / sampleRate;
            float freq = 600.0f;
            float envelope = 1.0f - (static_cast<float>(i) / numSamples);
            samples[i] = static_cast<std::int16_t>(10000.0f * (std::sin(2.0f * 3.14159265f * freq * t) + std::sin(2.0f * 3.14159265f * 1.5f * freq * t)) * 0.5f * envelope);
        }
    }

    if (!samples.empty()) {
        sf::SoundBuffer buffer;
        if (buffer.loadFromSamples(samples.data(), samples.size(), 1, sampleRate, { sf::SoundChannel::Mono })) {
            sounds_[name] = std::move(buffer);
        }
    }
}

void ResourceManager::recordAssetStatus(const std::string& type, const std::string& name, bool success, const std::string& resolvedPath) {
    for (auto& status : assetStatuses_) {
        if (status.type == type && status.name == name) {
            status.success = success;
            status.resolvedPath = resolvedPath;
            return;
        }
    }
    assetStatuses_.push_back({type, name, success, resolvedPath});
}

std::string ResourceManager::getCurrentWorkingDirectory() const {
    return std::filesystem::current_path().string();
}

std::string ResourceManager::getAssetRootSummary() const {
    std::ostringstream oss;
    for (std::size_t i = 0; i < assetRoots_.size(); ++i) {
        if (i != 0) {
            oss << " | ";
        }
        oss << assetRoots_[i].string();
    }
    return oss.str();
}

bool ResourceManager::wasAssetLoaded(const std::string& type, const std::string& name) const {
    for (const auto& status : assetStatuses_) {
        if (status.type == type && status.name == name) {
            return status.success;
        }
    }
    return false;
}

sf::String ResourceManager::toSfString(const std::string& utf8Str) {
    return sf::String::fromUtf8(utf8Str.begin(), utf8Str.end());
}

void ResourceManager::playSound(const std::string& name) {
    if (!soundEnabled_) return;

    try {
        activeSounds_.erase(
            std::remove_if(activeSounds_.begin(), activeSounds_.end(),
                           [](const sf::Sound& s) { return s.getStatus() == sf::Sound::Status::Stopped; }),
            activeSounds_.end()
        );

        activeSounds_.emplace_back(getSoundBuffer(name));
        activeSounds_.back().play();
    } catch (const std::exception& e) {
        std::cerr << "[Audio] Failed to play sound '" << name << "': " << e.what() << std::endl;
    }
}

void ResourceManager::stopAllSounds() {
    try {
        for (auto& sound : activeSounds_) {
            sound.stop();
        }
        activeSounds_.clear();
    } catch (const std::exception& e) {
        std::cerr << "[Audio] Failed to stop sounds cleanly: " << e.what() << std::endl;
    }
}

} // namespace caro
