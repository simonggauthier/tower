#include "FolderTree.h"

#include <Windows.h>

#include <string>
#include <iterator>

#include "FunctionEvent.h"
#include "Container.h"
#include "FsNode.h"
#include "GlobalConfiguration.h"
#include "Debug.h"

namespace tower {
    FolderTree::FolderTree(HWND parentHwnd, HINSTANCE hInstance) :
        _folder(nullptr) {

        setEventDispatcherId("folderTree");

        WNDCLASS wc = { 0 };
        wc.lpszClassName = L"Folder Tree";
        wc.style = CS_GLOBALCLASS | CS_HREDRAW | CS_VREDRAW;
        wc.lpfnWndProc = FolderTree::trueWndProc;
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);
        
        RegisterClass(&wc);
        
        _hwnd = CreateWindow(
            L"Folder Tree",
            nullptr,
            WS_CHILD | WS_VISIBLE,
            0, 0, 0, 0,
            parentHwnd,
            nullptr,
            hInstance,
            this
        );

        _TOWER_DEBUG("FolderTree hwnd: " << _hwnd);

        _fontSize = GlobalConfiguration::getInstance().getConfiguration()["folderTree"]["fontSize"];

        _font = CreateFont(_fontSize, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Consolas");

        _padding[0] = 5;
        _padding[1] = 5;

        ShowWindow(_hwnd, SW_SHOW);
    }

    FolderTree::~FolderTree() {
        DeleteObject(_font);
        DestroyWindow(_hwnd);
        
        if (_folder != nullptr) {
            delete _folder;
        }

        UnregisterClass(L"Folder Tree", nullptr);
    }

    void FolderTree::setPosition(int x, int y, int width, int height) {
        SetWindowPos(_hwnd, nullptr, x, y, width, height, SWP_NOZORDER);
    }
    
    void FolderTree::openFolder(std::wstring path) {
        if (_folder != nullptr) {
            delete _folder;
        }
        
        _folder = new FsNode(path);
        
        _TOWER_DEBUG_PRINT_FSNODE(_folder);

        SendMessage(_hwnd, WM_SETREDRAW, TRUE, 0);
    }
    
    LRESULT CALLBACK FolderTree::wndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
        switch (uMsg) {
            case WM_PAINT: {
                if (_onDraw(wParam)) {
                    return 0;
                }
            
                break;
            }
            
            case WM_LBUTTONDOWN: {
                int x = LOWORD(lParam);
                int y = HIWORD(lParam);
                
                RenderedItem* item = _getRenderedItemAt(x, y);
                
                if (item != nullptr) {
                    _TOWER_DEBUGW(L"Click on  " << item->getFsNode()->getPath());
                }
                
                break;
            }
        }

        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    bool FolderTree::_onDraw(WPARAM wParam) {
        _renderedItems.clear();
    
        PAINTSTRUCT ps;
        HDC hdc;
        RECT clientRect;
        
        GetClientRect(_hwnd, &clientRect);
        
        // Paint the background
        hdc = BeginPaint(_hwnd, &ps);
        HBRUSH brush = CreateSolidBrush(RGB(10, 10, 30));
        FillRect(hdc, &clientRect, brush);

        // Paint folder items
        if (_folder != nullptr) {
            _renderedItems.clear();
            
            _drawFsNode(hdc, clientRect, _folder, 0, 0);
        }

        EndPaint(_hwnd, &ps);

        return false;
    }
    
    int FolderTree::_drawFsNode(HDC& hdc, RECT& clientRect, FsNode* fsNode, int count, int depth) {
        std::wstring path = fsNode->getName();
        RECT rect;
        int mCount = 1;

        CopyRect(&rect, &clientRect);

        rect.left += depth * 25;
        rect.top += count * (_fontSize + 5);

        if (fsNode->getType() == FsNodeTypes::directory) {
            path = L"- " + path;
        }
    
        SetTextColor(hdc, RGB(222, 222, 222));
        SetBkMode(hdc, TRANSPARENT);
        SelectObject(hdc, _font);
        DrawText(hdc, path.c_str(), -1, &rect, DT_SINGLELINE);

        if (fsNode->getType() == FsNodeTypes::directory) {
            auto iterator = fsNode->childrenBegin();
            
            while (iterator != fsNode->childrenEnd()) {
                mCount += _drawFsNode(hdc, clientRect, *iterator, count + mCount, depth + 1);

                iterator = std::next(iterator, 1);
            }
        }
        
        _renderedItems.push_back(RenderedItem(fsNode, clientRect.left, clientRect.top, clientRect.right - clientRect.left, _fontSize));
    
        return mCount;
    }

    RenderedItem* FolderTree::_getRenderedItemAt(int x, int y) {
        for (auto& item : _renderedItems) {
            if (x > item.getX() &&
                y > item.getY() &&
                x < item.getX() + item.getWidth() &&
                y < item.getY() + item.getHeight()) {
                return &item;   
            }
        }
        
        return nullptr;
    }
    
    LRESULT CALLBACK FolderTree::trueWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
        FolderTree* folderTree = nullptr;

        if (uMsg == WM_NCCREATE) {
            CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
            folderTree = reinterpret_cast<FolderTree*>(pCreate->lpCreateParams);
            SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(folderTree));
        } else {
            folderTree = reinterpret_cast<FolderTree*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
        }

        if (folderTree != nullptr) {
            return folderTree->wndProc(hwnd, uMsg, wParam, lParam);
        }

        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}
