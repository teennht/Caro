#ifndef CARO_VIET_UI_SCREEN_MANAGER_HPP
#define CARO_VIET_UI_SCREEN_MANAGER_HPP

#include <memory>
#include "Screen.hpp"

namespace caro {

class ScreenManager {
public:
    static ScreenManager& instance();

    ScreenManager(const ScreenManager&) = delete;
    ScreenManager& operator=(const ScreenManager&) = delete;

    void requestChange(std::unique_ptr<Screen> nextScreen);
    void applyPendingChange();
    Screen* current() const;

private:
    ScreenManager() = default;

    std::unique_ptr<Screen> currentScreen_;
    std::unique_ptr<Screen> pendingScreen_;
};

} // namespace caro

#endif // CARO_VIET_UI_SCREEN_MANAGER_HPP
