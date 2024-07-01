#include <Windows.h>

#include <string>

#include "Editor.h"
#include "EditorContainer.h"

namespace tower {
    Editor::Editor(HWND parentHwnd, HINSTANCE hInstance, int fontSize) {
        _hwnd = CreateWindowEx(
            0,
            L"EDIT",
            L"",
            WS_BORDER | WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_LEFT |  ES_MULTILINE | ES_AUTOVSCROLL,
            10, 50, 400, 400,
            parentHwnd,
            NULL,
            hInstance,
            this
        );

        _font = CreateFont(fontSize, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Consolas");
        SendMessage(_hwnd, WM_SETFONT, (WPARAM) _font, TRUE);

        _originalWndProc = (WNDPROC) SetWindowLongPtr(_hwnd, GWLP_WNDPROC, (LONG_PTR) Editor::trueWndProc);
    }

    Editor::~Editor() {
        DeleteObject(_font);
        DestroyWindow(_hwnd);
    }

    void Editor::setPosition(int x, int y, int width, int height) {
        SetWindowPos(_hwnd, NULL, x, y, width, height, SWP_NOZORDER);
    }

    int Editor::getTextLength() const {
        return GetWindowTextLength(_hwnd);
    }

    void Editor::getText(wchar_t* buffer, int length) const {
        GetWindowText(_hwnd, buffer, length);
    }

    void Editor::setText(const wchar_t* text) {
        SetWindowText(_hwnd, text);
    }

    void Editor::clear() {
        SetWindowText(_hwnd, L"");
    }

    int Editor::getCurrentLineIndex() const {
        return SendMessage(_hwnd, EM_LINEFROMCHAR, -1, 0);
    }

    void Editor::getLine(int index, wchar_t* buffer, int length) const {
        DWORD dLength = length;

        memcpy(buffer, &dLength, sizeof(DWORD));
        int copied = SendMessage(_hwnd, EM_GETLINE, index, reinterpret_cast<LPARAM>(buffer));
        buffer[copied] = '\0';
    }

    LRESULT CALLBACK Editor::wndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
        switch (uMsg) {
            case WM_CHAR:
                if (wParam == VK_TAB) {
                    SendMessage(hwnd, EM_REPLACESEL, TRUE, reinterpret_cast<LPARAM>(L"    "));
                    return 0;
                } else if (wParam == VK_RETURN) {                    
                    int spaceCount = _countSpacesAtStartOfLine();
                    std::wstring spaces(spaceCount, L' ');

                    SendMessage(hwnd, EM_REPLACESEL, FALSE, reinterpret_cast<LPARAM>(L"\r\n"));
                    SendMessage(hwnd, EM_REPLACESEL, FALSE, reinterpret_cast<LPARAM>(spaces.c_str()));

                    return 0;
                }
        }
        
        return CallWindowProc(_originalWndProc, hwnd, uMsg, wParam, lParam);
    }

    LRESULT CALLBACK Editor::trueWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
        HWND mainWindowHwnd = GetParent(hwnd);
        EditorContainer* app = reinterpret_cast<EditorContainer*>(GetWindowLongPtr(mainWindowHwnd, GWLP_USERDATA));

        if (app) {
            return app->getEditor()->wndProc(hwnd, uMsg, wParam, lParam);
        }
        
        return 0;
    }

    int Editor::_countSpacesAtStartOfLine() const {
        const int SIZE = 100;
        wchar_t buffer[SIZE] = {0};
        int ret = 0;

        getLine(getCurrentLineIndex(), buffer, SIZE);

        for(int i = 0; i < SIZE; i++) {
            if (buffer[i] == L' ') {
                ret++;
            } else {
                break;
            }
        }

        return ret;
    }
}
