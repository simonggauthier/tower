#pragma once

#include "Tower.h"

namespace tower {
    class Editor {
    public:
        Editor(HWND parentHwnd, HINSTANCE hInstance, int fontSize);
        
        ~Editor();
        
        int GetTextLength();
        
        void GetText(wchar_t* buffer, int length);

        void SetText(const wchar_t* text);

        void Clear();

        HWND getHwnd() { return _hwnd; }

        LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

        static LRESULT CALLBACK TrueWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    private:
        HWND _hwnd;
        WNDPROC _originalWndProc;
        HFONT _font;
    };
}
