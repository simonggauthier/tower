#include "FolderTree.h"

#include <Windows.h>

#include <string>

#include "FunctionEvent.h"
#include "Container.h"
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
    
    void FolderTree::setFolder(std::wstring path) {
        if (_folder != nullptr) {
            delete _folder;
        }
        
        _folder = new Folder(path);
        
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
                
                RenderedItem* item = _getRenderedItem(x, y);
                
                if (item != nullptr) {
                    _TOWER_DEBUGW(L"Click on  " << item->getFolderItem().getPath());
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
            // Padding
            clientRect.top += 5;
            clientRect.left += 5;

            for (const FolderItem& item : _folder->getItems()) {
                std::wstring path = item.getPath();

                if (item.getType() == FolderItemTypes::FOLDER) {
                    path = L"+ " + path;
                } else {
                    path = L"  " + path;
                }

                SetTextColor(hdc, RGB(222, 222, 222));
                SetBkMode(hdc, TRANSPARENT);
                SelectObject(hdc, _font);
                DrawText(hdc, path.c_str(), -1, &clientRect, DT_SINGLELINE);

                _renderedItems.push_back(RenderedItem(item, clientRect.left, clientRect.top, clientRect.right - clientRect.left, _fontSize + 5));

                clientRect.top += _fontSize + 5;
            }
        }

        EndPaint(_hwnd, &ps);

        return false;
    }

    RenderedItem* FolderTree::_getRenderedItem(int x, int y) {
        _TOWER_DEBUGW(L"Get rendered item at " << x << L", " << y);

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
