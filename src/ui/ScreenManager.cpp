#include "ui/ScreenManager.hpp"

namespace caro {

ScreenManager& ScreenManager::instance() {
    static ScreenManager* inst = new ScreenManager();
    return *inst;
}

void ScreenManager::requestChange(std::unique_ptr<Screen> nextScreen) {
    pendingScreen_ = std::move(nextScreen);
}

void ScreenManager::applyPendingChange() {
    if (pendingScreen_) {
        currentScreen_ = std::move(pendingScreen_);
    }
}

Screen* ScreenManager::current() const {
    return currentScreen_.get();
}

} // namespace caro
