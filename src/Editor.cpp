#include <Windows.h>

#include <string>

#include "Wndproc.h"
#include "Editor.h"
#include "EditorContainer.h"
#include "Event.h"
#include "EventListener.h"

namespace tower {
    Editor::Editor(HWND parentHwnd, HINSTANCE hInstance, int fontSize) {
        _hwnd = CreateWindowEx(
            0,
            L"EDIT",
            L"",
            WS_BORDER | WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_LEFT |  ES_MULTILINE | ES_AUTOVSCROLL,
            10, 50, 400, 400,
            parentHwnd,
            nullptr,
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
        SetWindowPos(_hwnd, nullptr, x, y, width, height, SWP_NOZORDER);
    }
    
    void Editor::setSelection(int position, int length) {
        SendMessage(_hwnd, EM_SETSEL, position, position + length);
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
            case WM_CHAR: {
                if (wParam == VK_TAB) {
                    SendMessage(hwnd, EM_REPLACESEL, TRUE, reinterpret_cast<LPARAM>(L"    "));
                    
                    Event event("changed");
                    dispatchEvent(&event);
                    
                    return 0;
                } else if (wParam == VK_RETURN) {                    
                    EditorLineInfo lineInfo = _getCurrentLineInfo();
                    
                    SendMessage(hwnd, EM_REPLACESEL, FALSE, reinterpret_cast<LPARAM>(L"\r\n"));

                    // if (!lineInfo.hasOnlySpaces()) {
                        std::wstring spaces(lineInfo.spacesAtStart, L' ');
                        
                        SendMessage(hwnd, EM_REPLACESEL, FALSE, reinterpret_cast<LPARAM>(spaces.c_str()));
                    // }

                    Event event("changed");
                    dispatchEvent(&event);

                    return 0;
                }
            }

            case EN_CHANGE:
                Event event("changed");
                dispatchEvent(&event);
                
                break;
        }
        
        return CallWindowProc(_originalWndProc, hwnd, uMsg, wParam, lParam);
    }
    
    EditorLineInfo Editor::_getCurrentLineInfo() const {
        const int SIZE = 100;
        wchar_t buffer[SIZE] = {0};
        EditorLineInfo ret;

        getLine(getCurrentLineIndex(), buffer, SIZE);
        
        bool stillSpaces = true;
        
        for(int i = 0; i < SIZE; i++) {
            if (buffer[i] == L' ') {
                if (stillSpaces) {
                    ret.spacesAtStart++;
                }
            } else if (buffer[i] == '\0') {
                break;
            } else if (stillSpaces) {
                stillSpaces = false;
            }

            ret.totalChars++;
        }

        return ret;
    }
    
    TRUE_WND_PROC_CONTROL(Editor, EditorContainer, getEditor)
}
