#pragma once

#include "Tower.h"

namespace tower {
    struct Handles {
        HWND mainWindow;
        HWND newFileButton;
        HWND openFileButton;
        HWND saveFileButton;
        HWND saveFileAsButton;
        HWND editor;
    };

    enum ControlIds {
        newFileButton = 101,
        openFileButton = 102,
        saveFileButton = 103,
        saveFileAsButton = 104
    };

    class App {
    public:
        App();
        ~App();

        void CreateMainWindow(HINSTANCE hInstance);
        void EventLoop();
        
        void OperationNewFile();
        void OperationOpenFile();
        void OperationSaveFile();
        void OperationSaveFileAs();

        LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
        LRESULT CALLBACK EditorProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

        static LRESULT CALLBACK TrueWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
        static LRESULT CALLBACK TrueEditorProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    private:
        int _GetEditorTextLength();
        void _SetEditorText(const std::wstring& text);
        void _ClearEditorText();
        void _GetEditorText(wchar_t* buffer, int length);
        void _ReadCurrentFile();
        void _WriteCurrentFile();
        std::wstring _AskFilePath(bool mustExist);
        void _SetWindowTitle();

        Handles _handles;
        WNDPROC _originalEditorWndProc;
        
        HFONT _editorFont;

        std::wstring _currentFileName;
    };
}
