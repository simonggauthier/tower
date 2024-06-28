#pragma once

#include "Tower.h"

namespace tower {
    class Editor {
    public:
        Editor(HWND parentHwnd, HINSTANCE hInstance, int fontSize);
        
        ~Editor();
        
        void SetPosition(int x, int y, int width, int height);
        int GetTextLength();
        void GetText(wchar_t* buffer, int length);
        void SetText(const wchar_t* text);
        void Clear();

        int GetCurrentLineIndex();
        void GetLine(int index, wchar_t* buffer, int length);

        HWND GetHwnd() { return _hwnd; }

        LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
        static LRESULT CALLBACK TrueWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    private:
        int _CountSpacesAtStartOfLine();

        HWND _hwnd;
        WNDPROC _originalWndProc;
        HFONT _font;
    };
}
