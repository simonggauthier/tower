#include <string>
#include <sstream>
#include <fstream>

#include <Windows.h>

#include "json.hpp"

#include "App.h"

namespace tower {
    App::App() :
        _hInstance(0),
        _editor(nullptr) {
        _currentFile = new File();
    }

    App::~App() {
        delete _currentFile;
        
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
        wc.hCursor = LoadCursor(nullptr, IDC_ARROW);

        RegisterClass(&wc);

        _handles.mainWindow = CreateWindowEx(
            0,                              // Optional window styles.
            CLASS_NAME,                     // Window class
            L"Tower Editor",    // Window text
            WS_OVERLAPPEDWINDOW,            // Window style
            // Size and position
            CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
            nullptr,       // Parent window    
            nullptr,       // Menu
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

        while (GetMessage(&msg, nullptr, 0, 0) > 0) {
            if (!TranslateAccelerator(_handles.mainWindow, _handles.acceleratorTable, &msg)) {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
    }

    void App::operationNewFile() {
        if (_currentFile->getState() != FileStates::saved &&
            _editor->getTextLength() > 0) {
            if (!_askNewConfirmation()) {
                return;
            }
        }
    
        _editor->clear();

        delete _currentFile;
        _currentFile = new File();

        _setWindowTitle();
    }

    void App::operationOpenFile() {
        if (_currentFile->getState() != FileStates::saved &&
            _editor->getTextLength() > 0) {
            if (!_askOpenConfirmation()) {
                return;
            }
        }
    
        std::wstring filename = _askFilePath(true);

        if (filename.empty()) {
            return;
        }

        delete _currentFile;
        _currentFile = new File(filename);

        _readCurrentFile();
        _setWindowTitle();
    }

    void App::operationSaveFile() {
        if (_currentFile->getState() == FileStates::uncreated) {
            operationSaveFileAs();
        } else {
            _writeCurrentFile();
            _setWindowTitle();
        }
    }

    void App::operationSaveFileAs() {
        std::wstring filename = _askFilePath(false);

        if (filename.empty()) {
            return;
        }

        delete _currentFile;
        _currentFile = new File(filename);

        _writeCurrentFile();
        _setWindowTitle();
    }

    void App::operationExit() {
        if (_currentFile->getState() != FileStates::saved &&
            _editor->getTextLength() > 0) {
           if (_askQuitConfirmation()) {
               DestroyWindow(_handles.mainWindow);
           }
        } else {    
            DestroyWindow(_handles.mainWindow);
        }
    }

    void App::onEvent() {
        _currentFile->setState(FileStates::modified);
        _setWindowTitle();
    }

    LRESULT CALLBACK App::wndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
        switch (uMsg) {
            case WM_DESTROY:
                PostQuitMessage(0);
                return 0;
                
            case WM_CLOSE:
                operationExit();
                return 0;
                
            case WM_SIZE:
                {
                    if (_editor != nullptr) {
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
        App* app = nullptr;
        
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
        
        _handles.findMenu = CreateMenu();
        AppendMenu(_handles.findMenu, MF_STRING, ControlIds::findMenuItem, L"Find...\tCtrl+F");
        AppendMenu(_handles.menuBar, MF_POPUP, reinterpret_cast<UINT_PTR>(_handles.findMenu), L"Search");
        
        SetMenu(_handles.mainWindow, _handles.menuBar);    
    }

    void App::_createEditor() {
        // Create editor
        std::ifstream cstream("c:\\dev\\projects\\tower\\tower.json");
        nlohmann::json config = nlohmann::json::parse(cstream);

        _editor = new Editor(_handles.mainWindow, _hInstance, config["editor"]["fontSize"]);
        _editor->addEventListener(this);
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
        std::wstring content = _currentFile->read();
    
        _editor->setText(content.c_str());
    }

    void App::_writeCurrentFile() {
        const int length = _editor->getTextLength();
        wchar_t* text = new wchar_t[length + 1];

        _editor->getText(text, length + 1);

        _currentFile->write(std::wstring(text));

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
        ofn.lpstrFileTitle = nullptr;
        ofn.nMaxFileTitle = 0;
        ofn.lpstrInitialDir = nullptr;

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

        if (_currentFile->getState() != FileStates::uncreated) {
            title += L" - ";
            title += _currentFile->getFilename();
        }
        
        if (_currentFile->getState() != FileStates::saved) {
            title += L" (Unsaved)";
        }

        SetWindowText(_handles.mainWindow, title.c_str());
    }
    
    bool App::_askQuitConfirmation() {
        return MessageBox(nullptr, L"You have unsaved changes. Do you still want to quit?", 
                                   L"Confirm application exit", MB_ICONEXCLAMATION | MB_YESNO) == IDYES;
    }
    
    bool App::_askNewConfirmation() {
        return MessageBox(nullptr, L"You have unsaved changes. Do you still want to create a new file?", 
                                   L"Confirm new file", MB_ICONEXCLAMATION | MB_YESNO) == IDYES;
    }
    
    bool App::_askOpenConfirmation() {
        return MessageBox(nullptr, L"You have unsaved changes. Do you still want to open a file?", 
                                   L"Confirm open file", MB_ICONEXCLAMATION | MB_YESNO) == IDYES;
    }
}
