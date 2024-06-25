#include "App.h"

namespace tower {
    App::App() {
        _currentFileName = L"";
    }

    App::~App() {
        DeleteObject(_editorFont);
    }

    void App::CreateMainWindow(HINSTANCE hInstance) {
        const wchar_t CLASS_NAME[]  = L"Tower Window Class";

        WNDCLASS wc = { };
        wc.lpfnWndProc = App::TrueWindowProc;
        wc.hInstance = hInstance;
        wc.lpszClassName = CLASS_NAME;
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);

        RegisterClass(&wc);

        _handles.mainWindow = CreateWindowEx(
            0,                              // Optional window styles.
            CLASS_NAME,                     // Window class
            L"Tower Editor",    // Window text
            WS_OVERLAPPEDWINDOW,            // Window style
            // Size and position
            CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
            NULL,       // Parent window    
            NULL,       // Menu
            hInstance,  // Instance handle
            this        // Additional application data
        );

        // Create buttons
        _handles.newFileButton = CreateWindowEx(
            0,
            L"BUTTON",
            L"New",
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            10, 10, 100, 30,
            _handles.mainWindow,
            (HMENU) ControlIds::newFileButton,
            hInstance,
            NULL
        );

        _handles.openFileButton = CreateWindowEx(
            0,
            L"BUTTON",
            L"Open...",
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            120, 10, 100, 30,
            _handles.mainWindow,
            (HMENU) ControlIds::openFileButton,
            hInstance,
            NULL
        );

        _handles.saveFileButton = CreateWindowEx(
            0,
            L"BUTTON",
            L"Save",
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            230, 10, 100, 30,
            _handles.mainWindow,
            (HMENU) ControlIds::saveFileButton,
            hInstance,
            NULL
        );

        _handles.saveFileAsButton = CreateWindowEx(
            0,
            L"BUTTON",
            L"Save As...",
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            340, 10, 100, 30,
            _handles.mainWindow,
            (HMENU) ControlIds::saveFileAsButton,
            hInstance,
            NULL
        );

        _handles.editor = CreateWindowEx(
            0,
            L"EDIT",
            L"",
            WS_BORDER | WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_LEFT |  ES_MULTILINE | ES_AUTOVSCROLL,
            10, 50, 400, 400,
            _handles.mainWindow,
            NULL,
            hInstance,
            NULL
        );

        _editorFont = CreateFont(18, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Consolas");
        SendMessage(_handles.editor, WM_SETFONT, (WPARAM)_editorFont, TRUE);

        _originalEditorWndProc = (WNDPROC) SetWindowLongPtr(_handles.editor, GWLP_WNDPROC, (LONG_PTR)App::TrueEditorProc);

        ShowWindow(_handles.mainWindow, SW_SHOWDEFAULT);
    }

    void App::EventLoop() {
        MSG msg = { };

        while (GetMessage(&msg, NULL, 0, 0) > 0)
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    int App::_GetEditorTextLength() {
        return GetWindowTextLength(_handles.editor);
    }

    void App::_SetEditorText(const std::wstring& text) {
        SetWindowText(_handles.editor, text.c_str());
    }

    void App::_ClearEditorText() {
        SetWindowText(_handles.editor, L"");
    }

    void App::_GetEditorText(wchar_t* buffer, int length) {
        GetWindowText(_handles.editor, buffer, length);
    }

    void App::_ReadCurrentFile() {
        std::wifstream file(_currentFileName.c_str());

        if (!file.is_open()) {
            return;
        }

        std::wstringstream buffer;
        std::wstring line;

        while (std::getline(file, line)) {
            buffer << line << L"\r\n";
        }

        _SetEditorText(buffer.str());
    }

    void App::_WriteCurrentFile() {
        if (_currentFileName.empty()) {
            return;
        }

        std::wofstream file(_currentFileName.c_str());

        if (!file.is_open()) {
            return;
        }

        const int length = _GetEditorTextLength();
        // Slow?
        wchar_t* text = new wchar_t[length];

        _GetEditorText(text, length);

        for (int i = 0; i < length; i++) {
            // ?????
            if (text[i] == 13) {
                continue;
            }

            file.put(text[i]);
        }

        file.close();

        delete[] text;
    }

    std::wstring App::_AskFilePath(bool mustExist) {
        OPENFILENAME ofn;
        wchar_t szFile[260];

        ZeroMemory(&ofn, sizeof(ofn));
        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = _handles.mainWindow;
        ofn.lpstrFile = szFile;
        ofn.lpstrFile[0] = '\0';
        ofn.nMaxFile = sizeof(szFile);
        ofn.lpstrFilter = L"All\0*.*\0Text\0*.TXT\0";
        ofn.nFilterIndex = 1;
        ofn.lpstrFileTitle = NULL;
        ofn.nMaxFileTitle = 0;
        ofn.lpstrInitialDir = NULL;

        if (mustExist) {
            ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
        } else {
            ofn.Flags = OFN_PATHMUSTEXIST;
        }        

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

    void App::_SetWindowTitle() {
        std::wstring title = L"Tower Editor";

        if (!_currentFileName.empty()) {
            title += L" - ";
            title += _currentFileName;
        }

        SetWindowText(_handles.mainWindow, title.c_str());
    }

    void App::OperationNewFile() {
        _ClearEditorText();

        _currentFileName = L"";

        _SetWindowTitle();
    }

    void App::OperationOpenFile() {
        std::wstring filePath = _AskFilePath(true);

        if (filePath.empty()) {
            return;
        }

        _currentFileName = filePath;

        _ReadCurrentFile();
        _SetWindowTitle();
    }

    void App::OperationSaveFile() {
        if (_currentFileName.empty()) {
            OperationSaveFileAs();
        } else {
            _WriteCurrentFile();
        }
    }

    void App::OperationSaveFileAs() {
        std::wstring filePath = _AskFilePath(false);

        if (filePath.empty()) {
            return;
        }

        _currentFileName = filePath;

        _WriteCurrentFile();
        _SetWindowTitle();
    }

    LRESULT CALLBACK App::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
        switch (uMsg) {
            case WM_DESTROY:
                PostQuitMessage(0);
                return 0;

            case WM_SIZE:
                {
                    RECT rcClient;
                    GetClientRect(hwnd, &rcClient);

                    SetWindowPos(_handles.editor, NULL, 0, 50, rcClient.right, rcClient.bottom - 50, SWP_NOZORDER);
                }
                return 0;

            case WM_COMMAND:
                {
                    switch(LOWORD(wParam)) {
                        case ControlIds::newFileButton:
                            OperationNewFile();
                            break;

                        case ControlIds::openFileButton:
                            OperationOpenFile();
                            break;

                        case ControlIds::saveFileButton:
                            OperationSaveFile();
                            break;

                        case ControlIds::saveFileAsButton:
                            OperationSaveFileAs();
                            break;
                    }
                }

            case WM_CTLCOLOREDIT:
                {
                    HDC hdc = (HDC)wParam;
                    SetBkColor(hdc, RGB(10, 10, 10));
                    SetTextColor(hdc, RGB(200, 200, 200));

                    HBRUSH hBrush = CreateSolidBrush(RGB(10, 10, 10));

                    return (LRESULT)hBrush;
                }
        }

        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    LRESULT CALLBACK App::EditorProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
        switch (uMsg) {
            case WM_CHAR:
                if (wParam == VK_TAB) {
                    SendMessage(hwnd, EM_REPLACESEL, TRUE, (LPARAM)L"    ");
                    return 0;
                } else if (wParam == VK_RETURN) {
                    SendMessage(hwnd, EM_REPLACESEL, TRUE, (LPARAM)L"\r\n");
                    return 0;
                }
        }
        
        return CallWindowProc(_originalEditorWndProc, hwnd, uMsg, wParam, lParam);
    }

    LRESULT CALLBACK App::TrueWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
        App* app = NULL;
        
        if (uMsg == WM_NCCREATE) {
            CREATESTRUCT* pCreate = (CREATESTRUCT*) lParam;
            app = (App*) pCreate->lpCreateParams;
            SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR) app);
        } else {
            app = (App*) GetWindowLongPtr(hwnd, GWLP_USERDATA);
        }

        if (app) {
            return app->WindowProc(hwnd, uMsg, wParam, lParam);
        }

        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    LRESULT CALLBACK App::TrueEditorProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
        HWND mainWindowHwnd = GetParent(hwnd);
        App* app = (App*) GetWindowLongPtr(hwnd, GWLP_USERDATA);

        if (app) {
            return app->EditorProc(hwnd, uMsg, wParam, lParam);
        }
        
        return 0;
    }
}
