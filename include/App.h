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

        std::string _dumbWideToNarrow(const std::wstring& wide) {
            std::string narrow(wide.begin(), wide.end());
            
            return narrow;            
        }

        std::wstring _dumbNarrowToWide(const std::string& narrow) {
            std::wstring wide(narrow.begin(), narrow.end());
            
            return wide;
        }

        HINSTANCE _hInstance;
        
        File* _currentFile;
        
        MainWindow* _mainWindow;
    };
}
