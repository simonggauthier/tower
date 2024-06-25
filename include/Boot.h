#include "Tower.h"

namespace tower {
    // Handles
    static HWND _mainWindowHwnd;
    static HWND _newFileButtonHwnd;
    static HWND _openFileButtonHwnd;
    static HWND _saveFileButtonHwnd;
    static HWND _saveFileAsButtonHwnd;
    static HWND _editorHwnd;

    WNDPROC _originalEditorProc;

    // Button ids
    static const int _newFileButtonId = 1;
    static const int _openFileButtonId = 2;
    static const int _saveFileButtonId = 3;
    static const int _saveFileAsButtonId = 4;

    // Fonts
    HFONT _editorFont;

    // Data
    static std::wstring _editorText;
    static std::wstring _currentFileName;

    // Init
    void CreateMainWindow(HINSTANCE hInstance);

    void EventLoop();

    void Destroy();

    // Util
    void _SetEditorText();

    void _GetEditorText();

    void _ReadCurrentFile();

    void _WriteCurrentFile();
    
    std::wstring _AskFilePath(bool mustExist);

    std::wstring _GetPreviousLine();

    // Operations
    void OperationNewFile();

    void OperationOpenFile();

    void OperationSaveFile();

    void OperationSaveFileAs();

    LRESULT CALLBACK _WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    LRESULT CALLBACK _EditorProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
}
