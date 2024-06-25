#pragma once

#include "Tower.h"
#include "Editor.h"
#include "EditorContainer.h"

namespace tower {
    struct Handles {
        HWND mainWindow;
        HWND newFileButton;
        HWND openFileButton;
        HWND saveFileButton;
        HWND saveFileAsButton;
    };

    enum ControlIds {
        newFileButton = 101,
        openFileButton = 102,
        saveFileButton = 103,
        saveFileAsButton = 104
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

        Editor* GetEditor() { return _editor; }

        LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

        static LRESULT CALLBACK TrueWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    private:
        void _ReadCurrentFile();
        void _WriteCurrentFile();
        std::wstring _AskFilePath(bool mustExist);
        void _SetWindowTitle();

        Editor* _editor;

        Handles _handles;
        WNDPROC _originalEditorWndProc;

        std::wstring _currentFileName;
    };
}
