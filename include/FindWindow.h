#pragma once

#include <Windows.h>

#include "EventDispatcher.h"

namespace tower {
    struct FindWindowHandles {
        HWND findWindow;
        HWND searchEdit;
        HWND searchDownButton;
        HWND searchUpButton;
        HWND label;
        
        FindWindowHandles() :
            findWindow(nullptr),
            searchEdit(nullptr),
            searchDownButton(nullptr),
            searchUpButton(nullptr),
            label(nullptr) {
        
        }
    };
    
    enum FindWindowControlIds {
        searchDownButton = 101,
        searchUpButton = 102
    };

    class FindWindow : public EventDispatcher {
    public:
        FindWindow(HWND parentHwnd, HINSTANCE hInstance);
        
        ~FindWindow();

        void create();
        
        void show();
        void hide();
        
        LRESULT CALLBACK wndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
        static LRESULT CALLBACK trueWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
        
    private:
        HWND _parentHwnd;
        HINSTANCE _hInstance;
    
        FindWindowHandles _handles;
    };
}
