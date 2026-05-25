#include "ui/Application.hpp"
#include <iostream>
#include <exception>

int main() {
    try {
        caro::Application app;
        app.run();
    } catch (const std::exception& e) {
        std::cerr << "Application crashed with exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Application crashed with unknown exception." << std::endl;
        return 1;
    }
    return 0;
}
