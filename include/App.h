#pragma once

#include <string>

#include <Windows.h>

#include "Editor.h"
#include "EditorContainer.h"
#include "File.h"
#include "EventListener.h"

namespace tower {
    class App : public EventListener {
    public:
        App();
        ~App();

        void createMainWindow(HINSTANCE hInstance);
        void eventLoop();
        
        void operationNewFile();
        void operationOpenFile();
        void operationSaveFile();
        void operationSaveFileAs();
        void operationExit();

        Editor* getEditor() { return _editor; }
        
        void onEvent();

    private:
        void _readCurrentFile();
        void _writeCurrentFile();
        std::wstring _askFilePath(bool mustExist);
        
        bool _askQuitConfirmation();
        bool _askNewConfirmation();
        bool _askOpenConfirmation();

        HINSTANCE _hInstance;
        
        Editor* _editor;
        File* _currentFile;
    };
}
