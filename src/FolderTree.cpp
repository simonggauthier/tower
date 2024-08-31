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
        _folder(nullptr),
        _selectedFile(nullptr) {

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
            WS_CHILD | WS_VISIBLE | WS_BORDER,
            0, 0, 0, 0,
            parentHwnd,
            nullptr,
            hInstance,
            this
        );

        _TOWER_DEBUG("FolderTree hwnd: " << _hwnd);

        _fontSize = GlobalConfiguration::getInstance().getConfiguration()["folderTree"]["fontSize"];

        _font = CreateFont(_fontSize, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Consolas");
        _directoryFont = CreateFont(_fontSize, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Consolas");

        _itemPadding[0] = 5;
        _itemPadding[1] = 5;

        ShowWindow(_hwnd, SW_SHOW);
    }

    FolderTree::~FolderTree() {
        DeleteObject(_font);
        DeleteObject(_directoryFont);
        
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

        RECT rect;
        GetWindowRect(_hwnd, &rect);

        SendMessage(_hwnd, WM_SIZE, 0, MAKELPARAM(rect.right - rect.left, rect.bottom - rect.top));
        UpdateWindow(_hwnd);
    }
    
    LRESULT CALLBACK FolderTree::wndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
        switch (uMsg) {
            case WM_PAINT: {
                _onDraw(wParam);
            
                break;
            }

            case WM_SIZE: {
                _onSize(lParam);

                break;
            }
            
            case WM_LBUTTONDOWN: {
                int x = LOWORD(lParam);
                int y = HIWORD(lParam);
                
                RenderedItem* item = _getRenderedItemAt(x, y);

                _TOWER_DEBUG("Click " << x << ", " << y);
                
                if (item != nullptr) {
                    if (item->getFsNode()->getType() == FsNodeTypes::file) {
                        _selectedFile = item->getFsNode();

                        InvalidateRect(_hwnd, 0, TRUE);
                    }
                }
                
                break;
            }

            case WM_VSCROLL: {
                _onScroll(wParam, lParam);

                break;
            }
        }

        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    void FolderTree::_onDraw(WPARAM wParam) {
        _renderedItems.clear();
    
        PAINTSTRUCT ps;
        HDC hdc;
        RECT clientRect;
        
        GetClientRect(_hwnd, &clientRect);
        
        // Paint the background
        hdc = BeginPaint(_hwnd, &ps);

        HBRUSH brush = CreateSolidBrush(RGB(10, 10, 15));
        FillRect(hdc, &clientRect, brush);
        DeleteObject(brush);

        // Paint folder items
        if (_folder != nullptr) {
            _renderedItems.clear();
            
            _drawFsNode(hdc, clientRect, _folder, 0, 0);
        }

        EndPaint(_hwnd, &ps);
    }

    void FolderTree::_onSize(LPARAM lParam) {
        int height = HIWORD(lParam);

        if (_countItems() > 0) {
            SCROLLINFO scrollInfo = { 0 };
            scrollInfo.cbSize = sizeof(scrollInfo); 
            scrollInfo.fMask = SIF_RANGE | SIF_PAGE; 
            scrollInfo.nMin = 0; 
            scrollInfo.nMax = _countItems();
            scrollInfo.nPage = height / _getItemHeight();
            SetScrollInfo(_hwnd, SB_VERT, &scrollInfo, TRUE);
        }
    }

    void FolderTree::_onScroll(WPARAM wParam, LPARAM lParam) {
        WORD action = LOWORD(wParam);

        SCROLLINFO scrollInfo = { 0 };
        scrollInfo.cbSize = sizeof(scrollInfo);
        scrollInfo.fMask = SIF_ALL;
        GetScrollInfo(_hwnd, SB_VERT, &scrollInfo);

        int currentPos = scrollInfo.nPos;
        int newPos = -1;

        switch (action) {
            case SB_TOP:
                newPos = scrollInfo.nMin;
                break;

            case SB_BOTTOM:
                newPos = scrollInfo.nMax;
                break;

            case SB_LINEDOWN:
                newPos = currentPos + 1;
                break;

            case SB_LINEUP:
                newPos = currentPos - 1;
                break;

            case SB_THUMBTRACK:
                newPos = scrollInfo.nTrackPos;
                break;

            case SB_PAGEDOWN:
                newPos = currentPos + scrollInfo.nPage;
                break;

            case SB_PAGEUP:
                newPos = currentPos - scrollInfo.nPage;
                break;

            default:
            case SB_THUMBPOSITION:
                newPos = currentPos;
                break;
        }

        if (newPos != -1) {
            SetScrollPos(_hwnd, SB_VERT, newPos, TRUE);
            
            newPos = GetScrollPos(_hwnd, SB_VERT);

            ScrollWindowEx(_hwnd, 0, (currentPos - newPos) * _getItemHeight(),
                           NULL, NULL, NULL, NULL, SW_ERASE | SW_INVALIDATE);
        }
    }
    
    int FolderTree::_countItems() {
        if (_folder != nullptr) {
            return _folder->countAllNodes();
        }
        
        return 0;
    }

    int FolderTree::_drawFsNode(HDC& hdc, RECT& clientRect, FsNode* fsNode, int count, int depth) {
        std::wstring path = fsNode->getName();
        RECT rect;
        int mCount = 1;

        int scrollY = GetScrollPos(_hwnd, SB_VERT);

        CopyRect(&rect, &clientRect);

        rect.left += depth * 25;
        rect.top += (count - scrollY) * _getItemHeight();

        if (fsNode->getType() == FsNodeTypes::directory) {
            path = L"- " + path;
            
            SelectObject(hdc, _directoryFont);
        } else {
            SelectObject(hdc, _font);
        }

        if (_selectedFile == fsNode) {
            RECT backgroundRect;

            backgroundRect.left = clientRect.left;
            backgroundRect.top = rect.top - _itemPadding[0];
            backgroundRect.right = clientRect.right;
            backgroundRect.bottom = rect.top + _getItemHeight();

            _TOWER_DEBUG("eq " << backgroundRect.left << " " <<
                                  backgroundRect.top << " " <<
                                  backgroundRect.right << " " <<
                                  backgroundRect.bottom);
            HBRUSH brush = CreateSolidBrush(RGB(100, 100, 100));
            FillRect(hdc, &backgroundRect, brush);
            DeleteObject(brush);
        }
    
        SetTextColor(hdc, RGB(222, 222, 222));
        SetBkMode(hdc, TRANSPARENT);
        DrawText(hdc, path.c_str(), -1, &rect, DT_SINGLELINE);

        if (fsNode->getType() == FsNodeTypes::directory) {
            auto iterator = fsNode->childrenBegin();
            
            while (iterator != fsNode->childrenEnd()) {
                mCount += _drawFsNode(hdc, clientRect, *iterator, count + mCount, depth + 1);

                iterator = std::next(iterator, 1);
            }
        }
        
        _renderedItems.push_back(RenderedItem(fsNode, rect.left, rect.top - _itemPadding[0], rect.right - rect.left, _fontSize + _itemPadding[0]));
    
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
