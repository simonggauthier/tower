#pragma once

#include <Windows.h>

#include <string>
#include <vector>

#include "EventListener.h"
#include "EventDispatcher.h"
#include "FsNode.h"

namespace tower {
    class RenderedItem {
    public:
        RenderedItem(FsNode* fsNode, int x, int y, int width, int height) :
            _fsNode(fsNode),
            _x(x),
            _y(y),
            _width(width),
            _height(height),
            _opened(false) {
            
        }
        
        ~RenderedItem() {
        
        }
        
        FsNode* getFsNode() { return _fsNode; }
        const int getX() const { return _x; }
        const int getY() const { return _y; }
        const int getWidth() const { return _width; }
        const int getHeight() const { return _height; }
        
    private:
        FsNode* _fsNode;
        int _x;
        int _y;
        int _width;
        int _height;
        bool _opened;
    };

    class FolderTree : public EventDispatcher {
    public:
        FolderTree(HWND parentHwnd, HINSTANCE hInstance);
        ~FolderTree();
        
        void setPosition(int x, int y, int width, int height);
        
        void openFolder(std::wstring path);

        HWND getHwnd() const { return _hwnd; }

        LRESULT CALLBACK wndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
        static LRESULT CALLBACK trueWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    private:
        bool _onDraw(WPARAM wParam);
        int _drawFsNode(HDC& hdc, RECT& clientRect, FsNode* fsNode, int count, int depth);
        RenderedItem* _getRenderedItemAt(int x, int y);

        HWND _hwnd;
        WNDPROC _originalWndProc;
        HFONT _font;
        int _fontSize;
        int _padding[2];
        
        FsNode* _folder;
        
        std::vector<RenderedItem> _renderedItems;
    };
}
