#include "App.h"

namespace tower {
    App::App() {
        _currentFileName = L"";
    }

    App::~App() {
        delete _editor;
    }

    void App::CreateMainWindow(HINSTANCE hInstance) {
        const wchar_t CLASS_NAME[]  = L"Tower Window Class";

        WNDCLASS wc = { };
        wc.lpfnWndProc = App::TrueWndProc;
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

        std::ifstream cstream("c:\\dev\\projects\\tower\\tower.json");
        nlohmann::json config = nlohmann::json::parse(cstream);

        _editor = new Editor(_handles.mainWindow, hInstance, config["editor"]["fontSize"]);

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

        _editor->SetText(buffer.str().c_str());
    }

    void App::_WriteCurrentFile() {
        if (_currentFileName.empty()) {
            return;
        }

        std::wofstream file(_currentFileName.c_str());

        if (!file.is_open()) {
            return;
        }

        const int length = _editor->GetTextLength();
        // Slow?
        wchar_t* text = new wchar_t[length + 1];

        _editor->GetText(text, length + 1);

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
        _editor->Clear();

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

    LRESULT CALLBACK App::WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
        switch (uMsg) {
            case WM_DESTROY:
                PostQuitMessage(0);
                return 0;

            case WM_SIZE:
                {
                    RECT rcClient;

                    GetClientRect(hwnd, &rcClient);
                    _editor->SetPosition(0, 50, rcClient.right, rcClient.bottom - 50);
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

    LRESULT CALLBACK App::TrueWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
        App* app = NULL;
        
        if (uMsg == WM_NCCREATE) {
            CREATESTRUCT* pCreate = (CREATESTRUCT*) lParam;
            app = (App*) pCreate->lpCreateParams;
            SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR) app);
        } else {
            app = (App*) GetWindowLongPtr(hwnd, GWLP_USERDATA);
        }

        if (app) {
            return app->WndProc(hwnd, uMsg, wParam, lParam);
        }

        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}
