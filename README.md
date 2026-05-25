# Caro - Nhóm 3

Caro is a high-performance, beautiful desktop implementation of the classic Vietnamese Caro (Gomoku) game. It is built in C++17 and SFML 3.x.

## Features
- **Offline Single-Player Mode**: Play against an advanced Heuristic AI with multiple difficulty levels (Easy, Normal, Hard).
- **Offline Local 2-Player Mode**: Play with a friend on the same computer.
- **Rich Visuals and Audio**: Beautiful UI styling, animations, cell scale-in effects, and custom procedural audio/sound effects.
- **Save and Load System**: Supports saving matches locally at any point and resuming them later from the Main Menu.

## Requirements
- Windows 10 or later
- Visual Studio 2022 with **Desktop development with C++**
- CMake 3.20 or later
- vcpkg
- SFML 3.x

## Build on Windows

### 1. Install Visual Studio 2022
Install Visual Studio 2022 Community from Microsoft.

During installation, select:
- **Desktop development with C++**
- MSVC v143 build tools
- Windows SDK
- CMake tools for Windows

### 2. Install vcpkg
Open PowerShell and run:
```powershell
cd C:\Users\%USERNAME%
git clone https://github.com/microsoft/vcpkg.git
cd vcpkg
.\bootstrap-vcpkg.bat
```

### 3. Install SFML 3
From the `vcpkg` folder, run:
```powershell
.\vcpkg install sfml:x64-windows
```

### 4. Configure the Project
Open PowerShell in the project folder:
```powershell
cd "C:\path\to\Caro"
```

If `cmake` is available in PATH, run:
```powershell
cmake -S . -B build-vcpkg -DCMAKE_TOOLCHAIN_FILE=C:/Users/%USERNAME%/vcpkg/scripts/buildsystems/vcpkg.cmake -A x64
```

If `cmake` is not available in PATH, use Visual Studio's bundled CMake:
```powershell
& "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe" -S . -B build-vcpkg -DCMAKE_TOOLCHAIN_FILE=C:/Users/%USERNAME%/vcpkg/scripts/buildsystems/vcpkg.cmake -A x64
```

### 5. Build the Game
With `cmake` in PATH:
```powershell
cmake --build build-vcpkg --config Debug
```

With Visual Studio's bundled CMake:
```powershell
& "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe" --build build-vcpkg --config Debug
```

### 6. Run the Game
```powershell
.\build-vcpkg\Debug\CaroViet.exe
```

The build copies the `assets` folder and required SFML DLL files next to the executable.

### 7. Run Backend Unit Tests
```powershell
.\build-vcpkg\Debug\CaroVietTests.exe
```

## Folder Structure
- `include/core/` / `src/core/`: Decoupled game board rules, state logic, win detection, and save manager.
- `include/ai/` / `src/ai/`: Interface and player implementations for Random AI and Heuristic AI.
- `include/ui/` / `src/ui/`: SFML window loop, procedural resource generator, screen manager, buttons, and views.
- `assets/`: Sound fonts, textures, and fallback resources.
- `saves/`: Directory where match state `.txt` saves are generated.
- `tests/`: Automated unit testing framework.
