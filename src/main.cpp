#define VK_USE_PLATFORM_WIN32_KHR

#include "window/Hwindow.hpp"

#include <iostream>

int main() {
    Hwindow* mainWindow = Hwindow::initialize(TEXT("Horizons"), 1280, 720);

    // Показать окно
    mainWindow->show();
    
    // Главный цикл
    while (mainWindow->is_running()) {
        mainWindow->process_messages();
        
        // Здесь можно добавить рендеринг
    }

    delete mainWindow;
    
    return 0;
}