#pragma once

#include <Windows.h>

#include <string>

#include "Wndproc.h"
#include "EventListener.h"
#include "EventDispatcher.h"
#include "Event.h"

namespace tower {
    class FunctionLine : public EventDispatcher {
    public:
        FunctionLine(HWND parentHwnd, HINSTANCE hInstance, int fontSize);
        ~FunctionLine();
        
        void setPosition(int x, int y, int width, int height);

        HWND getHwnd() const { return _hwnd; }

        void show(const std::wstring& tmpl);
        void hide();

        bool isVisible() const { return _isVisible; }

        LRESULT CALLBACK wndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
        TRUE_WND_PROC_DEF;

    private:
        void _setCarretPosition(int position);
    
        HWND _hwnd;
        WNDPROC _originalWndProc;
        HFONT _font;
        
        bool _isVisible;
    };
}
