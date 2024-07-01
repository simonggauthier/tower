#include <string>
#include <sstream>
#include <fstream>

#include <Windows.h>

#include "json.hpp"

#include "App.h"

namespace tower {
    App::App() :
        _hInstance(0),
        _editor(nullptr),
        _currentFileName(L"") {
    }

    App::~App() {
        if (_editor != nullptr) {
            delete _editor;
        }
    }

    void App::createMainWindow(HINSTANCE hInstance) {
        const wchar_t CLASS_NAME[]  = L"Tower Window Class";
        
        _hInstance = hInstance;

        WNDCLASS wc = { };
        wc.lpfnWndProc = App::trueWndProc;
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

        _createMenu();
        _createEditor();
        _createAccelerators();

        ShowWindow(_handles.mainWindow, SW_SHOWDEFAULT);
    }

    void App::eventLoop() {
        MSG msg = { };

        while (GetMessage(&msg, NULL, 0, 0) > 0) {
            if (!TranslateAccelerator(_handles.mainWindow, _handles.acceleratorTable, &msg)) {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
    }

    void App::operationNewFile() {
        _editor->clear();

        _currentFileName = L"";

        _setWindowTitle();
    }

    void App::operationOpenFile() {
        std::wstring filePath = _askFilePath(true);

        if (filePath.empty()) {
            return;
        }

        _currentFileName = filePath;

        _readCurrentFile();
        _setWindowTitle();
    }

    void App::operationSaveFile() {
        if (_currentFileName.empty()) {
            operationSaveFileAs();
        } else {
            _writeCurrentFile();
        }
    }

    void App::operationSaveFileAs() {
        std::wstring filePath = _askFilePath(false);

        if (filePath.empty()) {
            return;
        }

        _currentFileName = filePath;

        _writeCurrentFile();
        _setWindowTitle();
    }

    void App::operationExit() {
        PostQuitMessage(0);
    }

    LRESULT CALLBACK App::wndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
        switch (uMsg) {
            case WM_DESTROY:
                PostQuitMessage(0);
                return 0;

            case WM_SIZE:
                {
                    if (_editor != NULL) {
                        RECT rcClient;

                        GetClientRect(hwnd, &rcClient);
                        _editor->setPosition(0, 0, rcClient.right, rcClient.bottom);
                    }
                }
                return 0;

            case WM_COMMAND:
                {
                    switch(LOWORD(wParam)) {
                        case ControlIds::newFileMenuItem:
                            operationNewFile();
                            break;

                        case ControlIds::openFileMenuItem:
                            operationOpenFile();
                            break;

                        case ControlIds::saveFileMenuItem:
                            operationSaveFile();
                            break;

                        case ControlIds::saveFileAsMenuItem:
                            operationSaveFileAs();
                            break;

                        case ControlIds::exitMenuItem:
                            operationExit();
                            break;
                    }
                }

            case WM_CTLCOLOREDIT:
                {
                    HDC hdc = reinterpret_cast<HDC>(wParam);
                    SetBkColor(hdc, RGB(10, 10, 10));
                    SetTextColor(hdc, RGB(200, 200, 200));

                    HBRUSH hBrush = CreateSolidBrush(RGB(10, 10, 10));

                    return reinterpret_cast<LRESULT>(hBrush);
                }
        }

        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    LRESULT CALLBACK App::trueWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
        App* app = NULL;
        
        if (uMsg == WM_NCCREATE) {
            CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
            app = reinterpret_cast<App*>(pCreate->lpCreateParams);
            SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(app));
        } else {
            app = reinterpret_cast<App*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
        }

        if (app) {
            return app->wndProc(hwnd, uMsg, wParam, lParam);
        }

        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    
    void App::_createMenu() {
        _handles.menuBar = CreateMenu();
        _handles.fileMenu = CreateMenu();

        AppendMenu(_handles.fileMenu, MF_STRING, ControlIds::newFileMenuItem, L"New\tCtrl+N");
        AppendMenu(_handles.fileMenu, MF_STRING, ControlIds::openFileMenuItem, L"Open...\tCtrl+O");
        AppendMenu(_handles.fileMenu, MF_STRING, ControlIds::saveFileMenuItem, L"Save\tCtrl+S");
        AppendMenu(_handles.fileMenu, MF_STRING, ControlIds::saveFileAsMenuItem, L"Save As...");
        AppendMenu(_handles.fileMenu, MF_MENUBREAK, 0, 0);
        AppendMenu(_handles.fileMenu, MF_STRING, ControlIds::exitMenuItem, L"Exit\tAlt+F4");
        AppendMenu(_handles.menuBar, MF_POPUP, reinterpret_cast<UINT_PTR>(_handles.fileMenu), L"File");
        
        SetMenu(_handles.mainWindow, _handles.menuBar);    
    }

    void App::_createEditor() {
        // Create editor
        std::ifstream cstream("c:\\dev\\projects\\tower\\tower.json");
        nlohmann::json config = nlohmann::json::parse(cstream);

        _editor = new Editor(_handles.mainWindow, _hInstance, config["editor"]["fontSize"]);   
    }

    void App::_createAccelerators() {
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

    void App::_readCurrentFile() {
        std::wifstream file(_currentFileName.c_str());

        if (!file.is_open()) {
            return;
        }

        std::wstringstream buffer;
        std::wstring line;

        while (std::getline(file, line)) {
            buffer << line << L"\r\n";
        }

        _editor->setText(buffer.str().c_str());
    }

    void App::_writeCurrentFile() {
        if (_currentFileName.empty()) {
            return;
        }

        std::wofstream file(_currentFileName.c_str());

        if (!file.is_open()) {
            return;
        }

        const int length = _editor->getTextLength();
        wchar_t* text = new wchar_t[length + 1];

        _editor->getText(text, length + 1);

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

    std::wstring App::_askFilePath(bool mustExist) {
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

            if(GetOpenFileName(&ofn) == TRUE) {
                return ofn.lpstrFile;
            }
        } else {
            ofn.Flags = OFN_PATHMUSTEXIST;

            if(GetSaveFileName(&ofn) == TRUE) {
                return ofn.lpstrFile;
            }
        }

        return L"";
    }

    void App::_setWindowTitle() {
        std::wstring title = L"Tower Editor";

        if (!_currentFileName.empty()) {
            title += L" - ";
            title += _currentFileName;
        }

        SetWindowText(_handles.mainWindow, title.c_str());
    }
}
