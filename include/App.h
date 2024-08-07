#pragma once

#include <string>

#include <Windows.h>

#include "MainWindow.h"
#include "EventListener.h"
#include "Event.h"
#include "File.h"

namespace tower {
    class App : public EventListener {
    public:
        App(HINSTANCE hInstance);
        ~App();

        void mainLoop();
        
        void operationNewFile();
        void operationOpenFile();
        void operationSaveFile();
        void operationSaveFileAs();
        void operationExit();

        void onEvent(Event* event);

    private:
        void _readCurrentFile();
        void _writeCurrentFile();
        std::wstring _askFilePath(bool mustExist);
        
        void _setWindowTitle();
        
        bool _askQuitConfirmation();
        bool _askNewConfirmation();
        bool _askOpenConfirmation();
        std::wstring _getExecutablePath();

        void _saveState();

        HINSTANCE _hInstance;
        
        File* _currentFile;
        
        MainWindow* _mainWindow;
    };
}
