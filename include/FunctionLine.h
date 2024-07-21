#pragma once

#include <Windows.h>

#include <string>

#include "EventListener.h"
#include "EventDispatcher.h"
#include "FunctionEvent.h"

namespace tower {
    class FunctionLine : public EventDispatcher {
    public:
        FunctionLine(HWND parentHwnd, HINSTANCE hInstance, int fontSize);
        ~FunctionLine();
        
        void setPosition(int x, int y, int width, int height);

        HWND getHwnd() const { return _hwnd; }

        void show(const std::wstring& tmpl);
        void hide();

        std::wstring getText();
        bool isVisible() const { return _isVisible; }

        void setFocus();

        LRESULT CALLBACK wndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
        static LRESULT CALLBACK trueWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    private:
        void _setCaretPosition(int position);
        void _positionCaretToNextArgument();
        void _setFunctionEvent(FunctionEvent& event);
    
        HWND _hwnd;
        WNDPROC _originalWndProc;
        HFONT _font;
        
        bool _isVisible;
    };
}
