#include "FindWindow.h"

#include <Windows.h>

namespace tower {
    FindWindow::FindWindow(HWND parentHwnd, HINSTANCE hInstance) :
        _hInstance(hInstance) {
        const wchar_t CLASS_NAME[]  = L"Tower Find Window Class";
        
        WNDCLASS wc = { };
        wc.lpfnWndProc = FindWindow::trueWndProc;
        wc.hInstance = _hInstance;
        wc.lpszClassName = CLASS_NAME;
        wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
        
        RegisterClass(&wc);
        
        _handles.findWindow = CreateWindowEx(
            0,
            CLASS_NAME,
            L"Find",
            WS_OVERLAPPEDWINDOW,
            0, 0, 300, 150,
            parentHwnd,
            nullptr,
            _hInstance,
            this
        );
        
        _handles.searchDownButton = CreateWindow( 
            L"BUTTON",
            L"Find next",
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            0, 0, 50, 20,
            _handles.findWindow,
            nullptr,
            _hInstance, 
            nullptr
        );
    }
    
    FindWindow::~FindWindow() {
        DestroyWindow(_handles.findWindow);
    }

    void FindWindow::create() {

    }
    
    void FindWindow::show() {
        ShowWindow(_handles.findWindow, SW_SHOWDEFAULT);
    }
    
    void FindWindow::hide() {
        ShowWindow(_handles.findWindow, SW_HIDE);
    }
    
    LRESULT CALLBACK FindWindow::wndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
        switch (uMsg) {                
            case WM_CLOSE: {
                Event event("exit");
                dispatchEvent(&event);
                
                hide();
                
                return 0;
            }
        }

        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    LRESULT CALLBACK FindWindow::trueWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
        FindWindow* window = nullptr;
        
        if (uMsg == WM_NCCREATE) {
            CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
            window = reinterpret_cast<FindWindow*>(pCreate->lpCreateParams);
            SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(window));
        } else {
            window = reinterpret_cast<FindWindow*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
        }

        if (window != nullptr) {
            return window->wndProc(hwnd, uMsg, wParam, lParam);
        }

        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}
