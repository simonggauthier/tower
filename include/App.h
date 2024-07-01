#pragma once

#include <string>

#include <Windows.h>

#include "Editor.h"
#include "EditorContainer.h"
#include "File.h"
#include "EventListener.h"

namespace tower {
    struct Handles {
        HWND mainWindow;
        HMENU menuBar;
        HMENU fileMenu;
        HACCEL acceleratorTable;
    };

    enum ControlIds {
        newFileMenuItem = 101,
        openFileMenuItem = 102,
        saveFileMenuItem = 103,
        saveFileAsMenuItem = 104,
        exitMenuItem = 105
    };

    class App : public EditorContainer, public EventListener {
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

        LRESULT CALLBACK wndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
        static LRESULT CALLBACK trueWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    private:
        void _createMenu();
        void _createEditor();
        void _createAccelerators();
    
        void _readCurrentFile();
        void _writeCurrentFile();
        std::wstring _askFilePath(bool mustExist);
        
        void _setWindowTitle();
        
        bool _askConfirmation(const std::wstring& title, const std::wstring& message);

        HINSTANCE _hInstance;
        Handles _handles;
        
        Editor* _editor;
        File* _currentFile;
    };
}
