#include "App.h"

namespace tower {
    App::App() :
        _hInstance(0),
        _editor(0),
        _currentFileName(L"") {
    }

    App::~App() {
        delete _editor;
    }

    void App::CreateMainWindow(HINSTANCE hInstance) {
        const wchar_t CLASS_NAME[]  = L"Tower Window Class";
        
        _hInstance = hInstance;

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

        _CreateMenu();
        _CreateEditor();
        _CreateAccelerators();

        ShowWindow(_handles.mainWindow, SW_SHOWDEFAULT);
    }

    void App::EventLoop() {
        MSG msg = { };

        while (GetMessage(&msg, NULL, 0, 0) > 0) {
            if (!TranslateAccelerator(_handles.mainWindow, _handles.acceleratorTable, &msg)) {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
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

    void App::OperationExit() {
        PostQuitMessage(0);
    }

    LRESULT CALLBACK App::WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
        switch (uMsg) {
            case WM_DESTROY:
                PostQuitMessage(0);
                return 0;

            case WM_SIZE:
                {
                    if (_editor != NULL) {
                        RECT rcClient;

                        GetClientRect(hwnd, &rcClient);
                        _editor->SetPosition(0, 0, rcClient.right, rcClient.bottom);
                    }
                }
                return 0;

            case WM_COMMAND:
                {
                    switch(LOWORD(wParam)) {
                        case ControlIds::newFileMenuItem:
                            OperationNewFile();
                            break;

                        case ControlIds::openFileMenuItem:
                            OperationOpenFile();
                            break;

                        case ControlIds::saveFileMenuItem:
                            OperationSaveFile();
                            break;

                        case ControlIds::saveFileAsMenuItem:
                            OperationSaveFileAs();
                            break;

                        case ControlIds::exitMenuItem:
                            OperationExit();
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
    
    void App::_CreateMenu() {
        _handles.menuBar = CreateMenu();
        _handles.fileMenu = CreateMenu();

        AppendMenu(_handles.fileMenu, MF_STRING, ControlIds::newFileMenuItem, L"New\tCtrl+N");
        AppendMenu(_handles.fileMenu, MF_STRING, ControlIds::openFileMenuItem, L"Open...\tCtrl+O");
        AppendMenu(_handles.fileMenu, MF_STRING, ControlIds::saveFileMenuItem, L"Save\tCtrl+S");
        AppendMenu(_handles.fileMenu, MF_STRING, ControlIds::saveFileAsMenuItem, L"Save As...");
        AppendMenu(_handles.fileMenu, MF_MENUBREAK, 0, 0);
        AppendMenu(_handles.fileMenu, MF_STRING, ControlIds::exitMenuItem, L"Exit\tAlt+F4");
        AppendMenu(_handles.menuBar, MF_POPUP, (UINT_PTR)_handles.fileMenu, L"File");
        
        SetMenu(_handles.mainWindow, _handles.menuBar);    
    }

    void App::_CreateEditor() {
        // Create editor
        std::ifstream cstream("c:\\dev\\projects\\tower\\tower.json");
        nlohmann::json config = nlohmann::json::parse(cstream);

        _editor = new Editor(_handles.mainWindow, _hInstance, config["editor"]["fontSize"]);   
    }

    void App::_CreateAccelerators() {
        ACCEL* acc = new ACCEL[3];
        
        acc[0].fVirt = FVIRTKEY | FCONTROL;
        acc[0].key = 'N';
        acc[0].cmd = ControlIds::newFileMenuItem;
        acc[1].fVirt = FVIRTKEY | FCONTROL;
        acc[1].key = 'O';
        acc[1].cmd = ControlIds::openFileMenuItem;
        acc[2].fVirt = FVIRTKEY | FCONTROL;
        acc[2].key = 'S';
        acc[2].cmd = ControlIds::saveFileMenuItem;
               
        _handles.acceleratorTable = CreateAcceleratorTable(acc, 3);
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
}
