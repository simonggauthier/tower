#pragma once

#include <string>

#include <Windows.h>

#include "EditorContainer.h"
#include "EventListener.h"
#include "EventDispatcher.h"
#include "Event.h"

namespace tower {
    struct MainWindowHandles {
        HWND mainWindow;
        HMENU menuBar;
        HMENU fileMenu;
        HMENU findMenu;
        HACCEL acceleratorTable;

        MainWindowHandles() :
            mainWindow(nullptr),
            menuBar(nullptr),
            fileMenu(nullptr),
            findMenu(nullptr),
            acceleratorTable(nullptr) {

        }
    };

    enum MainWindowControlIds {
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
        virtual ~MainWindow();
        
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
        MainWindowHandles _handles;
        
        Editor* _editor;
    };
}
