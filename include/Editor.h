#pragma once

#include <Windows.h>

namespace tower {
    class Editor {
    public:
        Editor(HWND parentHwnd, HINSTANCE hInstance, int fontSize);
        ~Editor();
        
        void setPosition(int x, int y, int width, int height);

        int getTextLength() const;
        void getText(wchar_t* buffer, int length) const;
        void setText(const wchar_t* text);
        void clear();

        int getCurrentLineIndex() const;
        void getLine(int index, wchar_t* buffer, int length) const;

        HWND getHwnd() const { return _hwnd; }

        LRESULT CALLBACK wndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
        static LRESULT CALLBACK trueWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    private:
        int _countSpacesAtStartOfLine() const;

        HWND _hwnd;
        WNDPROC _originalWndProc;
        HFONT _font;
    };
}
