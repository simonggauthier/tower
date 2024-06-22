#include "Boot.h"

namespace tower {    
    void CreateMainWindow(HINSTANCE hInstance) {
        const wchar_t CLASS_NAME[]  = L"Tower Window Class";

        WNDCLASS wc = { };
        wc.lpfnWndProc = _WindowProc;
        wc.hInstance = hInstance;
        wc.lpszClassName = CLASS_NAME;
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);

        RegisterClass(&wc);

        _mainWindowHwnd = CreateWindowEx(
            0,                              // Optional window styles.
            CLASS_NAME,                     // Window class
            L"Tower Editor",    // Window text
            WS_OVERLAPPEDWINDOW,            // Window style
            // Size and position
            CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
            NULL,       // Parent window    
            NULL,       // Menu
            hInstance,  // Instance handle
            NULL        // Additional application data
        );

        // Create buttons
        _newFileButtonHwnd = CreateWindowEx(
            0,
            L"BUTTON",
            L"New",
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            10, 10, 100, 30,
            _mainWindowHwnd,
            (HMENU) _newFileButtonId,
            hInstance,
            NULL
        );

        _openFileButtonHwnd = CreateWindowEx(
            0,
            L"BUTTON",
            L"Open...",
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            120, 10, 100, 30,
            _mainWindowHwnd,
            (HMENU) _openFileButtonId,
            hInstance,
            NULL
        );

        _saveFileButtonHwnd = CreateWindowEx(
            0,
            L"BUTTON",
            L"Save",
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            230, 10, 100, 30,
            _mainWindowHwnd,
            (HMENU) _saveFileButtonId,
            hInstance,
            NULL
        );

        _saveFileAsButtonHwnd = CreateWindowEx(
            0,
            L"BUTTON",
            L"Save As...",
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            340, 10, 100, 30,
            _mainWindowHwnd,
            (HMENU) _saveFileAsButtonId,
            hInstance,
            NULL
        );

        _editorHwnd = CreateWindowEx(
            0,
            L"EDIT",
            L"",
            WS_BORDER | WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_LEFT |  ES_MULTILINE | ES_AUTOVSCROLL,
            10, 50, 400, 400,
            _mainWindowHwnd,
            NULL,
            hInstance,
            NULL
        );

        ShowWindow(_mainWindowHwnd, SW_SHOWDEFAULT);
    }

    void eventLoop() {
        MSG msg = { };

        while (GetMessage(&msg, NULL, 0, 0) > 0)
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    void _SetEditorText() {
        SetWindowText(_editorHwnd, _editorText.c_str());
    }

    void _GetEditorText() {
        int length = GetWindowTextLength(_editorHwnd);
        wchar_t* buffer = new wchar_t[length + 1];

        GetWindowText(_editorHwnd, buffer, length + 1);

        _editorText = buffer;

        delete[] buffer;
    }

    void _ReadCurrentFile() {
        std::wifstream file(_currentFileName.c_str());

        if (!file.is_open()) {
            return;
        }

        std::wstringstream buffer;
        std::wstring line;

        while (std::getline(file, line)) {
            buffer << line << L"\r\n";
        }

        _editorText = buffer.str();
    }

    void _WriteCurrentFile() {
        if (_currentFileName.empty()) {
            return;
        }

        std::wofstream file(_currentFileName.c_str());

        if (!file.is_open()) {
            return;
        }

        const wchar_t* text = _editorText.c_str();

        for (int i = 0; i < _editorText.size(); i++) {
            // ?????
            if (text[i] == 13) {
                continue;
            }

            file.put(text[i]);
        }

        file.close();
    }

    std::wstring _AskFilePath(bool mustExist) {
        OPENFILENAME ofn;
        wchar_t szFile[260];

        ZeroMemory(&ofn, sizeof(ofn));
        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = _mainWindowHwnd;
        ofn.lpstrFile = szFile;
        ofn.lpstrFile[0] = '\0';
        ofn.nMaxFile = sizeof(szFile);
        ofn.lpstrFilter = L"All\0*.*\0Text\0*.TXT\0";
        ofn.nFilterIndex = 1;
        ofn.lpstrFileTitle = NULL;
        ofn.nMaxFileTitle = 0;
        ofn.lpstrInitialDir = NULL;
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

        if (mustExist) {
            if(GetOpenFileName(&ofn) == TRUE) {
                return ofn.lpstrFile;
            }
        } else {
            if(GetSaveFileName(&ofn) == TRUE) {
                return ofn.lpstrFile;
            }
        }

        return L"";
    }

    void OperationNewFile() {
        _editorText = L"";
        _currentFileName = L"";

        _SetEditorText();
    }

    void OperationOpenFile() {
        std::wstring filePath = _AskFilePath(true);

        if (filePath.empty()) {
            return;
        }

        _currentFileName = filePath;

        _ReadCurrentFile();

        _SetEditorText();
    }

    void OperationSaveFile() {
        if (_currentFileName.empty()) {
            OperationSaveFileAs();
        } else {
            _GetEditorText();
            _WriteCurrentFile();
        }
    }

    void OperationSaveFileAs() {
        std::wstring filePath = _AskFilePath(false);

        if (filePath.empty()) {
            return;
        }

        _currentFileName = filePath;

        _GetEditorText();
        _WriteCurrentFile();
    }

    LRESULT CALLBACK _WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
        switch (uMsg) {
            case WM_DESTROY:
                PostQuitMessage(0);
                return 0;

            case WM_SIZE:
                {
                    RECT rcClient;
                    GetClientRect(hwnd, &rcClient);

                    SetWindowPos(_editorHwnd, NULL, 0, 50, rcClient.right, rcClient.bottom - 50, SWP_NOZORDER);
                }
                return 0;

            case WM_COMMAND:
                {
                    switch(LOWORD(wParam)) {
                        case _newFileButtonId:
                            OperationNewFile();
                            break;

                        case _openFileButtonId:
                            OperationOpenFile();
                            break;

                        case _saveFileButtonId:
                            OperationSaveFile();
                            break;

                        case _saveFileAsButtonId:
                            OperationSaveFileAs();
                            break;
                    }
                }
        }

        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
    tower::CreateMainWindow(hInstance);

    tower::eventLoop();

    return 0;
}
