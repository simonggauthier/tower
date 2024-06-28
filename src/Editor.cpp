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

    void Editor::SetPosition(int x, int y, int width, int height) {
        SetWindowPos(_hwnd, NULL, x, y, width, height, SWP_NOZORDER);
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

    int Editor::GetCurrentLineIndex() {
        return SendMessage(_hwnd, EM_LINEFROMCHAR, -1, 0);
    }

    void Editor::GetLine(int index, wchar_t* buffer, int length) {
        DWORD dLength = length;

        memcpy(buffer, &dLength, sizeof(DWORD));
        int copied = SendMessage(_hwnd, EM_GETLINE, index, (LPARAM) buffer);
        buffer[copied] = '\0';
    }

    LRESULT CALLBACK Editor::WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
        switch (uMsg) {
            case WM_CHAR:
                if (wParam == VK_TAB) {
                    SendMessage(hwnd, EM_REPLACESEL, TRUE, (LPARAM)L"    ");
                    return 0;
                } else if (wParam == VK_RETURN) {                    
                    int spaceCount = _CountSpacesAtStartOfLine();
                    std::wstring spaces(spaceCount, L' ');

                    SendMessage(hwnd, EM_REPLACESEL, FALSE, (LPARAM) L"\r\n");
                    SendMessage(hwnd, EM_REPLACESEL, FALSE, (LPARAM) spaces.c_str());

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

    int Editor::_CountSpacesAtStartOfLine() {
        const int SIZE = 100;
        wchar_t buffer[SIZE] = {0};
        int ret = 0;

        GetLine(GetCurrentLineIndex(), buffer, SIZE);

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
