#pragma once

#include <string>

#include <Windows.h>

#include "EditorContainer.h"
#include "EventListener.h"
#include "EventDispatcher.h"
#include "Event.h"

namespace tower {
    struct Handles {
        HWND mainWindow;
        HMENU menuBar;
        HMENU fileMenu;
        HMENU findMenu;
        HACCEL acceleratorTable;

        Handles() :
            mainWindow(nullptr),
            menuBar(nullptr),
            fileMenu(nullptr),
            findMenu(nullptr),
            acceleratorTable(nullptr) {

        }
    };

    enum ControlIds {
        newFileMenuItem = 101,
        openFileMenuItem = 102,
        saveFileMenuItem = 103,
        saveFileAsMenuItem = 104,
        exitMenuItem = 105,
        findMenuItem = 106
    };

    class MainWindow : public EditorContainer, public EventListener, public EventDispatcher {
    public:
        MainWindow(HINSTANCE hInstance);
        
        void mainLoop();
        
        void destroy();

        void setTitle(const std::wstring& title);
        
        void onEvent(Event* event);
        
        HWND getHandle() { return _handles.mainWindow; }
        Editor* getEditor() { return _editor; }

        LRESULT CALLBACK wndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
        static LRESULT CALLBACK trueWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    private:
        void _createMenu();
        void _createEditor();
        void _createAccelerators();

        HINSTANCE _hInstance;
        Handles _handles;
        
        Editor* _editor;
    };
}
