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

        _originalWndProc = (WNDPROC) SetWindowLongPtr(_hwnd, GWLP_WNDPROC, (LONG_PTR) Editor::TrueWndProc);
    }

    Editor::~Editor() {
        DeleteObject(_font);
        DestroyWindow(_hwnd);
    }

    int Editor::GetTextLength() {
        return GetWindowTextLength(_hwnd);
    }

    void Editor::GetText(wchar_t* buffer, int length) {
        GetWindowText(_hwnd, buffer, length);
    }

    void Editor::SetText(const wchar_t* text) {
        SetWindowText(_hwnd, text);
    }

    void Editor::Clear() {
        SetWindowText(_hwnd, L"");
    }

    LRESULT CALLBACK Editor::WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
        switch (uMsg) {
            case WM_CHAR:
                if (wParam == VK_TAB) {
                    SendMessage(hwnd, EM_REPLACESEL, TRUE, (LPARAM)L"    ");
                    return 0;
                } else if (wParam == VK_RETURN) {
                    SendMessage(hwnd, EM_REPLACESEL, TRUE, (LPARAM)L"\r\n");
                    return 0;
                }
        }
        
        return CallWindowProc(_originalWndProc, hwnd, uMsg, wParam, lParam);
    }

    LRESULT CALLBACK Editor::TrueWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
        HWND mainWindowHwnd = GetParent(hwnd);
        EditorContainer* app = (EditorContainer*) GetWindowLongPtr(mainWindowHwnd, GWLP_USERDATA);

        if (app) {
            return app->GetEditor()->WndProc(hwnd, uMsg, wParam, lParam);
        }
        
        return 0;
    }
}
