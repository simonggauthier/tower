#include <Windows.h>

#include "MainWindow.h"

#include <fstream>

#include "json.hpp"

namespace tower {
    MainWindow::MainWindow(HINSTANCE hInstance) :
        _hInstance(hInstance),
        _editor(nullptr) {
        const wchar_t CLASS_NAME[]  = L"Tower Window Class";

        WNDCLASS wc = { };
        wc.lpfnWndProc = MainWindow::trueWndProc;
        wc.hInstance = hInstance;
        wc.lpszClassName = CLASS_NAME;
        wc.hCursor = LoadCursor(nullptr, IDC_ARROW);

        RegisterClass(&wc);

        _handles.mainWindow = CreateWindowEx(
            0,                              // Optional window styles.
            CLASS_NAME,                     // Window class
            L"Tower Editor",    // Window text
            WS_OVERLAPPEDWINDOW,            // Window style
            // Position and size
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
    
    MainWindow::~MainWindow() {
        delete _editor;
    }
    
    void MainWindow::mainLoop() {
        MSG msg = { };

        while (GetMessage(&msg, nullptr, 0, 0) > 0) {
            if (!TranslateAccelerator(_handles.mainWindow, _handles.acceleratorTable, &msg)) {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
    }
    
    void MainWindow::destroy() {
        DestroyWindow(_handles.mainWindow);
    }
    
    void MainWindow::setTitle(const std::wstring& title) {
        SetWindowText(_handles.mainWindow, title.c_str());
    }
    
    void MainWindow::onEvent(Event* event) {
        if (event->getName() == "changed") {
            dispatchEvent(event);
        }
    }
    
    LRESULT CALLBACK MainWindow::wndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
        switch (uMsg) {
            case WM_DESTROY:
                PostQuitMessage(0);
                return 0;
                
            case WM_CLOSE: {
                Event event("exit");
                dispatchEvent(&event);
                return 0;
            }
            
            case WM_SIZE: {
                if (_editor != nullptr) {
                    RECT rcClient;

                    GetClientRect(hwnd, &rcClient);
                    _editor->setPosition(0, 0, rcClient.right, rcClient.bottom);
                }
                
                return 0;
            }

            case WM_COMMAND: {
                switch(LOWORD(wParam)) {
                    case MainWindowControlIds::newFileMenuItem: {
                        Event event("newFile");
                        dispatchEvent(&event);
                        break;
                    }

                    case MainWindowControlIds::openFileMenuItem: {
                        Event event("openFile");
                        dispatchEvent(&event);
                        break;
                    }

                    case MainWindowControlIds::saveFileMenuItem: {
                        Event event("saveFile");
                        dispatchEvent(&event);
                        break;
                    }

                    case MainWindowControlIds::saveFileAsMenuItem: {
                        Event event("saveFileAs");
                        dispatchEvent(&event);
                        break;
                    }

                    case MainWindowControlIds::exitMenuItem: {
                        Event event("exit");
                        dispatchEvent(&event);
                        break;
                    }
                    
                    case MainWindowControlIds::findMenuItem: {
                        Event event("find");
                        dispatchEvent(&event);
                        break;
                    }
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

    LRESULT CALLBACK MainWindow::trueWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
        MainWindow* mainWindow = nullptr;
        
        if (uMsg == WM_NCCREATE) {
            CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
            mainWindow = reinterpret_cast<MainWindow*>(pCreate->lpCreateParams);
            SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(mainWindow));
        } else {
            mainWindow = reinterpret_cast<MainWindow*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
        }

        if (mainWindow != nullptr) {
            return mainWindow->wndProc(hwnd, uMsg, wParam, lParam);
        }

        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    
    void MainWindow::_createMenu() {
        _handles.menuBar = CreateMenu();
        
        _handles.fileMenu = CreateMenu();
        AppendMenu(_handles.fileMenu, MF_STRING, MainWindowControlIds::newFileMenuItem, L"New\tCtrl+N");
        AppendMenu(_handles.fileMenu, MF_STRING, MainWindowControlIds::openFileMenuItem, L"Open...\tCtrl+O");
        AppendMenu(_handles.fileMenu, MF_STRING, MainWindowControlIds::saveFileMenuItem, L"Save\tCtrl+S");
        AppendMenu(_handles.fileMenu, MF_STRING, MainWindowControlIds::saveFileAsMenuItem, L"Save As...");
        AppendMenu(_handles.fileMenu, MF_MENUBREAK, 0, 0);
        AppendMenu(_handles.fileMenu, MF_STRING, MainWindowControlIds::exitMenuItem, L"Exit\tAlt+F4");
        AppendMenu(_handles.menuBar, MF_POPUP, reinterpret_cast<UINT_PTR>(_handles.fileMenu), L"File");
        
        _handles.findMenu = CreateMenu();
        AppendMenu(_handles.findMenu, MF_STRING, MainWindowControlIds::findMenuItem, L"Find...\tCtrl+F");
        AppendMenu(_handles.menuBar, MF_POPUP, reinterpret_cast<UINT_PTR>(_handles.findMenu), L"Search");
        
        SetMenu(_handles.mainWindow, _handles.menuBar);    
    }

    void MainWindow::_createEditor() {
        // Create editor
        std::ifstream cstream("c:\\dev\\projects\\tower\\tower.json");
        nlohmann::json config = nlohmann::json::parse(cstream);

        _editor = new Editor(_handles.mainWindow, _hInstance, config["editor"]["fontSize"]);
        _editor->addEventListener(this);
    }

    void MainWindow::_createAccelerators() {
        const size_t SIZE = 4;
        ACCEL* acc = new ACCEL[SIZE];
        
        acc[0].fVirt = FVIRTKEY | FCONTROL;
        acc[0].key = 'N';
        acc[0].cmd = MainWindowControlIds::newFileMenuItem;
        acc[1].fVirt = FVIRTKEY | FCONTROL;
        acc[1].key = 'O';
        acc[1].cmd = MainWindowControlIds::openFileMenuItem;
        acc[2].fVirt = FVIRTKEY | FCONTROL;
        acc[2].key = 'S';
        acc[2].cmd = MainWindowControlIds::saveFileMenuItem;
        acc[3].fVirt = FVIRTKEY | FCONTROL;
        acc[3].key = 'F';
        acc[3].cmd = MainWindowControlIds::findMenuItem;
               
        _handles.acceleratorTable = CreateAcceleratorTable(acc, SIZE);

        delete[] acc;
    }
}
