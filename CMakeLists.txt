cmake_minimum_required(VERSION 3.20)
project(CaroViet LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (MSVC)
    add_compile_options(/W4 /permissive-)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Wno-unused-parameter)
endif()

# Core & AI Source Files (No SFML dependencies)
set(CORE_SOURCES
    src/core/Board.cpp
    src/core/Game.cpp
    src/core/WinChecker.cpp
    src/core/SaveManager.cpp
    src/ai/RandomAIPlayer.cpp
    src/ai/HeuristicAIPlayer.cpp
)

include_directories(include)

# Test target
add_executable(CaroVietTests
    ${CORE_SOURCES}
    tests/TestRunner.cpp
)

# SFML 3.x configuration
find_package(SFML 3 COMPONENTS Graphics Window System Audio REQUIRED)

# Main Executable
set(UI_SOURCES
    src/ui/ResourceManager.cpp
    src/ui/Button.cpp
    src/ui/BoardView.cpp
    src/ui/ScreenManager.cpp
    src/ui/PixelBackground.cpp
    src/ui/SplashScreen.cpp
    src/ui/MainMenuScreen.cpp
    src/ui/LoadGameScreen.cpp
    src/ui/HowToPlayScreen.cpp
    src/ui/SettingsScreen.cpp
    src/ui/GameplayScreen.cpp
    src/ui/VictoryScreen.cpp
    src/ui/Application.cpp
    src/main.cpp
)

add_executable(CaroViet
    ${CORE_SOURCES}
    ${UI_SOURCES}
)

target_link_libraries(CaroViet PRIVATE
    SFML::Graphics
    SFML::Window
    SFML::System
    SFML::Audio
)

# Post-build step to copy the assets folder to the build target directory
add_custom_command(TARGET CaroViet POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_CURRENT_SOURCE_DIR}/assets"
    "$<TARGET_FILE_DIR:CaroViet>/assets"
    COMMENT "Copying assets directory next to the CaroViet executable"
)
