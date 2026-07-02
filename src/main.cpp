#include "threads/HmainThread.hpp"

int main(int, char**) {
    // Создание главного потока
    //HmainThread mainThread = HmainThread::initialize();

    //mainThread.execute();

    MessageBox(NULL, TEXT("Could not register window class"), NULL, MB_ICONERROR);
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