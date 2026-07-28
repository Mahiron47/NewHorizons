//#include "window/Hwindow.hpp"

#include <iostream>

int f(const int& x) { return x; }

int main(int, char**) {
    int x = 0;
    
    x = sizeof x;

    std::cout << f(x + 7) << std::endl;
    
    //MessageBox(NULL, TEXT("Could not register window class"), NULL, MB_ICONERROR);
}
/* 
    Hwindow& mainWindow = Hwindow::initialize(1280, 720);

    // Показать окно
    mainWindow.show();
    
    // Главный цикл
    while (mainWindow.isRunning()) {
        mainWindow.processMessages();
        
        // Здесь можно добавить рендеринг
    }
    
    return 0;
}*/