#pragma once

#include <Windows.h>

#include <string>
#include <vector>

#include "EventListener.h"
#include "EventDispatcher.h"
#include "Folder.h"

namespace tower {
    class RenderedItem {
    public:
        RenderedItem(FolderItem folderItem, int x, int y, int width, int height) :
            _folderItem(folderItem),
            _x(x),
            _y(y),
            _width(width),
            _height(height) {
            
        }
        
        ~RenderedItem() {
        
        }
        
        FolderItem getFolderItem() const { return _folderItem; }
        const int getX() const { return _x; }
        const int getY() const { return _y; }
        const int getWidth() const { return _width; }
        const int getHeight() const { return _height; }
        
    private:
        FolderItem _folderItem;
        int _x;
        int _y;
        int _width;
        int _height;
    };

    class FolderTree : public EventDispatcher {
    public:
        FolderTree(HWND parentHwnd, HINSTANCE hInstance);
        ~FolderTree();
        
        void setPosition(int x, int y, int width, int height);
        
        void setFolder(std::wstring path);

        HWND getHwnd() const { return _hwnd; }

        LRESULT CALLBACK wndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
        static LRESULT CALLBACK trueWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    private:
        bool _onDraw(WPARAM wParam);
        
        RenderedItem* _getRenderedItem(int x, int y);

        HWND _hwnd;
        WNDPROC _originalWndProc;
        HFONT _font;
        int _fontSize;
        
        Folder* _folder;
        
        std::vector<RenderedItem> _renderedItems;
    };
}
