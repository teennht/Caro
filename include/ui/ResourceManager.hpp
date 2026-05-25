#ifndef CARO_VIET_UI_RESOURCE_MANAGER_HPP
#define CARO_VIET_UI_RESOURCE_MANAGER_HPP

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <unordered_map>
#include <string>
#include <memory>
#include <vector>
#include <filesystem>

namespace caro {

enum class Language {
    EN,
    VI
};

struct AssetStatus {
    std::string type;         // "Texture", "Sound", "Font"
    std::string name;
    bool success;
    std::string resolvedPath; // Path where found or empty if procedural/failed
};

class ResourceManager {
public:
    static ResourceManager& instance();

    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;

    const sf::Font& getFont(const std::string& name = "default");
    const sf::Texture& getTexture(const std::string& name);
    const sf::SoundBuffer& getSoundBuffer(const std::string& name);

    // Language Settings
    Language getLanguage() const { return currentLanguage_; }
    void setLanguage(Language lang) { currentLanguage_ = lang; }
    std::string getTranslation(const std::string& key) const;
    static sf::String toSfString(const std::string& utf8Str);

    // Settings
    bool isSoundEnabled() const { return soundEnabled_; }
    void setSoundEnabled(bool enabled) { soundEnabled_ = enabled; }

    bool isMusicEnabled() const { return musicEnabled_; }
    void setMusicEnabled(bool enabled) { musicEnabled_ = enabled; }

    bool isAnimationsEnabled() const { return animationsEnabled_; }
    void setAnimationsEnabled(bool enabled) { animationsEnabled_ = enabled; }

    bool isCoordinatesShown() const { return showCoordinates_; }
    void setCoordinatesShown(bool show) { showCoordinates_ = show; }

    // Sound Player Helper
    void playSound(const std::string& name);
    void stopAllSounds();

    // Asset Tracking for Debug
    const std::vector<AssetStatus>& getAssetStatuses() const { return assetStatuses_; }
    void recordAssetStatus(const std::string& type, const std::string& name, bool success, const std::string& resolvedPath);
    std::string getCurrentWorkingDirectory() const;
    std::string getAssetRootSummary() const;
    bool wasAssetLoaded(const std::string& type, const std::string& name) const;

private:
    ResourceManager();

    void generateProceduralTexture(const std::string& name);
    void generateProceduralSound(const std::string& name);
    std::vector<std::filesystem::path> buildAssetRoots() const;
    std::vector<std::filesystem::path> candidatePaths(const std::string& relativePath) const;

    std::unordered_map<std::string, sf::Font> fonts_;
    std::unordered_map<std::string, sf::Texture> textures_;
    std::unordered_map<std::string, sf::SoundBuffer> sounds_;
    std::vector<sf::Sound> activeSounds_;
    std::vector<AssetStatus> assetStatuses_;
    std::vector<std::filesystem::path> assetRoots_;
    
    Language currentLanguage_;
    bool soundEnabled_ = true;
    bool musicEnabled_ = true;
    bool animationsEnabled_ = true;
    bool showCoordinates_ = true;
};

} // namespace caro

#endif // CARO_VIET_UI_RESOURCE_MANAGER_HPP
