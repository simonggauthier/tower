#pragma once

#include "Tower.h"
#include "Editor.h"
#include "EditorContainer.h"

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

    class App : public EditorContainer {
    public:
        App();
        ~App();

        void CreateMainWindow(HINSTANCE hInstance);
        void EventLoop();
        
        void OperationNewFile();
        void OperationOpenFile();
        void OperationSaveFile();
        void OperationSaveFileAs();
        void OperationExit();

        Editor* GetEditor() { return _editor; }

        LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

        static LRESULT CALLBACK TrueWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    private:
        void _CreateMenu();
        void _CreateEditor();
        void _CreateAccelerators();
    
        void _ReadCurrentFile();
        void _WriteCurrentFile();
        std::wstring _AskFilePath(bool mustExist);
        void _SetWindowTitle();

        HINSTANCE _hInstance;
        Handles _handles;
        
        Editor* _editor;

        std::wstring _currentFileName;
    };
}
