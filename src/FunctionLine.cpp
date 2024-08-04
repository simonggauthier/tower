#include "FunctionLine.h"

#include <Windows.h>

#include <string>

#include "FunctionEvent.h"
#include "Container.h"
#include "GlobalConfiguration.h"
#include "Debug.h"

namespace tower {
    FunctionLine::FunctionLine(HWND parentHwnd, HINSTANCE hInstance) :
        _isVisible(false) {

        setEventDispatcherId("functionLine");

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

        _TOWER_DEBUG("FunctionLine hwnd: " << _hwnd);

        int fontSize = GlobalConfiguration::getInstance().getConfiguration()["functionLine"]["fontSize"];

        _font = CreateFont(fontSize, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Consolas");
        SendMessage(_hwnd, WM_SETFONT, (WPARAM)_font, TRUE);

        _originalWndProc = reinterpret_cast<WNDPROC>(SetWindowLongPtr(_hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(FunctionLine::trueWndProc)));

        ShowWindow(_hwnd, SW_HIDE);
    }

    FunctionLine::~FunctionLine() {
        DeleteObject(_font);
        DestroyWindow(_hwnd);
    }

    void FunctionLine::setPosition(int x, int y, int width, int height) {
        SetWindowPos(_hwnd, nullptr, x, y, width, height, SWP_NOZORDER);
    }
    
    void FunctionLine::setSelection(int position, int length) {
        SendMessage(_hwnd, EM_SETSEL, position, position + length);
    }
    
    void FunctionLine::show(const std::wstring& tmpl) {
        _isVisible = true;

        SetWindowText(_hwnd, tmpl.c_str());
        ShowWindow(_hwnd, SW_SHOW);
        setFocus();
        
        _setCaretPosition(0);
        _positionCaretToNextArgument();
    }

    void FunctionLine::hide() {
        _isVisible = false;

        SetWindowText(_hwnd, L"");

        ShowWindow(_hwnd, SW_HIDE);
    }

    std::wstring FunctionLine::getText() {
        int length = GetWindowTextLength(_hwnd) + 1;
        wchar_t* buffer = new wchar_t[length];

        GetWindowText(_hwnd, buffer, length);
        buffer[length - 1] = L'\0';

        std::wstring ret = std::wstring(buffer);

        delete[] buffer;

        return ret;
    }

    int FunctionLine::getTextLength() {
        return GetWindowTextLength(_hwnd);
    }

    void FunctionLine::setFocus() {
        SetFocus(_hwnd);
    }

    LRESULT CALLBACK FunctionLine::wndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
        switch (uMsg) {
            case WM_CHAR: {
                if (_onChar(wParam)) {
                    return 0;
                }
            }
        }

        return CallWindowProc(_originalWndProc, hwnd, uMsg, wParam, lParam);
    }

    bool FunctionLine::_onChar(WPARAM wParam) {
        SHORT scanInfo = VkKeyScanA(wParam);
        
        // Handle tabs
        if (wParam == VK_TAB) { 
            _positionCaretToNextArgument();

            return true;
        }

        // Handle enter
        if (wParam == VK_RETURN) {
            FunctionEvent event;
            _setFunctionEvent(event);

            dispatchEvent(&event);

            return true;
        }

    	// Handle escape
        if (wParam == VK_ESCAPE) {
            Event event("escape");

            dispatchEvent(&event);

            return true;
        }
        
        // Handle CTRL + A select all
        if ((scanInfo & 0x00FF) == 'A' &&
            ((scanInfo >> 8) & 2)) {
            int length = getTextLength();
            
            setSelection(0, length);
            
            return true;
        }

        return false;
    }

    void FunctionLine::_setCaretPosition(int position) {
        SendMessage(_hwnd, EM_SETSEL, static_cast<WPARAM>(position), static_cast<WPARAM>(position));
    }
    
    void FunctionLine::_positionCaretToNextArgument() {
        int position;

        SendMessage(_hwnd, EM_GETSEL, reinterpret_cast<WPARAM>(&position), 0);

        std::wstring text = getText();

        if (position > 0) {
            if (text[position] == L'\"') {
                position += 2;
            }
        }

        for (std::wstring::size_type i = position; i < text.size(); i++) {
            if (text[i] == L'\"') {
                position = static_cast<int>(i) + 1;
                break;
            }
        }

        _setCaretPosition(position);        
    }

    void FunctionLine::_setFunctionEvent(FunctionEvent& event) {
        std::wstring line = getText();

        if (line.length() == 0) {
            return;
        }

        std::wstring acc = L"";
        bool inString = false;

        for (std::wstring::size_type i = 0; i < line.size(); i++) {
            // Space or eol
            if ((!inString && line[i] == L' ') || 
                i == line.size() - 1) {
                if (event.getFunctionName().length() == 0) {
                    event.setFunctionName(acc);
                    acc = L"";
                } else {
                    event.addArgument(acc);
                    acc = L"";
                }
            // ""
            } else if (line[i] == L'\"') {
                inString = !inString;
            } else {
                acc += line[i];
            }
        }
    }

    LRESULT CALLBACK FunctionLine::trueWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
        HWND parentHwnd = GetParent(hwnd);
        Container* parent = reinterpret_cast<Container*>(GetWindowLongPtr(parentHwnd, GWLP_USERDATA));

        if (parent && parent->getFunctionLine()) {
            return parent->getFunctionLine()->wndProc(hwnd, uMsg, wParam, lParam);\
        }

        return 0;
    }
}
