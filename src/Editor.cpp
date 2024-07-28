#include <Windows.h>

#include <string>

#include "Editor.h"
#include "Container.h"
#include "Event.h"
#include "EventListener.h"
#include "GlobalConfiguration.h"
#include "Debug.h"

namespace tower {
    Editor::Editor(HWND parentHwnd, HINSTANCE hInstance) {
        setEventDispatcherId("editor");

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

        _TOWER_DEBUG("Editor hwnd: " << _hwnd);

        int fontSize = GlobalConfiguration::getInstance().getConfiguration()["editor"]["fontSize"];

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

    void Editor::setText(const wchar_t* text) {
        SetWindowText(_hwnd, text);
    }

    void Editor::clear() {
        SetWindowText(_hwnd, L"");
    }

    void Editor::setFocus() {
        SetFocus(_hwnd);
    }

    void Editor::scrollToTop() {
        int totalLines = getTotalLines();
        
        SendMessage(_hwnd, EM_LINESCROLL, 0, -totalLines);
    }

    void Editor::replaceAll(const std::wstring& needle, const std::wstring& replacement) {
        int length = getTextLength() + 1;
        wchar_t* buffer = new wchar_t[length];
        
        getText(buffer, length);
        
        std::wstring haystack(buffer);
        
        delete[] buffer;
        
        size_t position = haystack.find(needle);
        while (position != std::wstring::npos) {
            haystack.replace(position, needle.size(), replacement);
            position = haystack.find(needle, position + replacement.size());
        }
        
        setText(haystack.c_str());
        
        Event event("changed");
        dispatchEvent(&event);
    }

    void Editor::find(const std::wstring& needle) {
        _findContext.currentIndex = 0;
        _findContext.needle = needle;
        
        scrollToTop();

        findNext();
    }

    void Editor::findNext() {
        int length = getTextLength() + 1;
        wchar_t* buffer = new wchar_t[length];
        
        getText(buffer, length);
        
        std::wstring haystack(buffer);

        delete[] buffer;
        
        std::wstring::size_type index = haystack.find(_findContext.needle, _findContext.currentIndex);
        
        if (index != std::wstring::npos) {
            int currentLine = getCurrentLineIndex();

            setFocus();
            setSelection(index, _findContext.needle.size());
            _findContext.currentIndex = index + 1;

            int diffLine = getCurrentLineIndex() - currentLine;

            if (diffLine > 0) {                
                SendMessage(_hwnd, EM_LINESCROLL, 0, diffLine);
            }
        }
    }

    int Editor::getTextLength() const {
        return GetWindowTextLength(_hwnd);
    }

    void Editor::getText(wchar_t* buffer, int length) const {
        GetWindowText(_hwnd, buffer, length);
    }

    int Editor::getTotalLines() const {
        return SendMessage(_hwnd, EM_GETLINECOUNT, 0, 0);
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
                } else if (wParam == VK_ESCAPE) {
                    Event event("escape");
                
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
    
    LRESULT CALLBACK Editor::trueWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
        HWND parentHwnd = GetParent(hwnd);
        Container* parent = reinterpret_cast<Container*>(GetWindowLongPtr(parentHwnd, GWLP_USERDATA));

        if (parent && parent->getEditor()) {
            return parent->getEditor()->wndProc(hwnd, uMsg, wParam, lParam);\
        }

        return 0;
    }
}
