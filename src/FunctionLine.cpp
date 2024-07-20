#include "FunctionLine.h"

#include <Windows.h>

#include <string>

#include "FunctionLineContainer.h"

#include <stdio.h>

namespace tower {
    FunctionLine::FunctionLine(HWND parentHwnd, HINSTANCE hInstance, int fontSize) :
        _isVisible(false) {
        _hwnd = CreateWindowEx(
            0,
            L"EDIT",
            L"",
            WS_BORDER | WS_CHILD | WS_VISIBLE | ES_LEFT,
            0, 0, 100, 20,
            parentHwnd,
            nullptr,
            hInstance,
            this
        );
        
        _font = CreateFont(fontSize, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Consolas");
        SendMessage(_hwnd, WM_SETFONT, (WPARAM) _font, TRUE);
        
        _originalWndProc = (WNDPROC) SetWindowLongPtr(_hwnd, GWLP_WNDPROC, (LONG_PTR) FunctionLine::trueWndProc);

        ShowWindow(_hwnd, SW_HIDE);
    }
    
    FunctionLine::~FunctionLine() {
        DeleteObject(_font);
        DestroyWindow(_hwnd);
    }
    
    void FunctionLine::setPosition(int x, int y, int width, int height) {
        SetWindowPos(_hwnd, nullptr, x, y, width, height, SWP_NOZORDER);
    }
    
    void FunctionLine::show(const std::wstring& tmpl) {
        _isVisible = true;
        
        SetWindowText(_hwnd, tmpl.c_str());
        ShowWindow(_hwnd, SW_SHOW);
        SetFocus(_hwnd);
        
        int position = 0;
        
        for (std::wstring::size_type i = 0; i < tmpl.size(); i++) {
            if (tmpl[i] == '\"') {
                position = static_cast<int>(i) + 1;
                break;
            }
        }
        
        _setCarretPosition(position);
    }
    
    void FunctionLine::hide() {
        _isVisible = false;

        SetWindowText(_hwnd, L"");

        ShowWindow(_hwnd, SW_HIDE);
    }
    
    void FunctionLine::_setCarretPosition(int position)
    {
        SendMessage(_hwnd, EM_SETSEL, static_cast<WPARAM>(position), static_cast<WPARAM>(position));
    }
    
    LRESULT CALLBACK FunctionLine::wndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
        switch (uMsg) {
            case WM_CHAR: {
                // printf("CHAR\n");
                if (wParam == VK_RETURN) {
                    Event event("function");
                    
                    dispatchEvent(&event);
                    
                    return 0;
                }
            }
        }
    
        return CallWindowProc(_originalWndProc, hwnd, uMsg, wParam, lParam);
    }
    
    TRUE_WND_PROC_CONTROL(FunctionLine, FunctionLineContainer, getFunctionLine)
}
