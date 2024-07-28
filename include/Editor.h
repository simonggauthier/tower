#pragma once

#include <Windows.h>

#include <string>

#include "EventListener.h"
#include "EventDispatcher.h"
#include "Event.h"

namespace tower {
    struct EditorLineInfo {
        int spacesAtStart;
        int totalChars;

        EditorLineInfo() :
            spacesAtStart(0),
            totalChars(0) {

        }
        
        bool hasOnlySpaces() { return spacesAtStart == totalChars; }
    };
    
    struct EditorFindContext {
        std::wstring needle;
        int currentIndex;
        
        EditorFindContext() :
            needle(L""),
            currentIndex(0) {
            
        }
    };

    class Editor : public EventDispatcher {
    public:
        Editor(HWND parentHwnd, HINSTANCE hInstance);
        ~Editor();
        
        void setPosition(int x, int y, int width, int height);
        void setSelection(int position, int length);
        
        void setText(const wchar_t* text);
        void clear();
        
        void setFocus();
        void scrollToTop();
        
        void replaceAll(const std::wstring& needle, const std::wstring& replacement);
        void find(const std::wstring& needle);
        void findNext();

        int getTextLength() const;
        void getText(wchar_t* buffer, int length) const;

        int getTotalLines() const;
        int getCurrentLineIndex() const;
        void getLine(int index, wchar_t* buffer, int length) const;

        HWND getHwnd() const { return _hwnd; }

        LRESULT CALLBACK wndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
        static LRESULT CALLBACK trueWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    private:
        EditorLineInfo _getCurrentLineInfo() const;
        
        EditorFindContext _findContext;
        
        HWND _hwnd;
        WNDPROC _originalWndProc;
        HFONT _font;
    };
}
