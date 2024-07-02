#pragma once

#include <string>

#include <Windows.h>

#include "EditorContainer.h"

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

    class MainWindow : public EditorContainer {
    public:
        MainWindow(HINSTANCE hInstance);

        LRESULT CALLBACK wndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
        static LRESULT CALLBACK trueWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
        
        void setTitle(const std::wstring& title);

    private:
        void _createMenu();
        void _createEditor();
        void _createAccelerators();

        HINSTANCE _hInstance;

        Handles _handles;
    };
}
