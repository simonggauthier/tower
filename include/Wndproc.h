#pragma once

#define TRUE_WND_PROC_DEF static LRESULT CALLBACK trueWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)

#define TRUE_WND_PROC(className) LRESULT CALLBACK className::trueWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {\
        className* window = nullptr;\
\
        if (uMsg == WM_NCCREATE) {\
            CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);\
            window = reinterpret_cast<className*>(pCreate->lpCreateParams);\
            SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(window));\
        } else {\
            window = reinterpret_cast<className*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));\
        }\
\
        if (window != nullptr) {\
            return window->wndProc(hwnd, uMsg, wParam, lParam);\
        }\
\
        return DefWindowProc(hwnd, uMsg, wParam, lParam);\
    }\

#define TRUE_WND_PROC_CONTROL(className, castClassName, selfPointerFunction) LRESULT CALLBACK className::trueWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {\
        HWND parentHwnd = GetParent(hwnd);\
        castClassName* parent = reinterpret_cast<castClassName*>(GetWindowLongPtr(parentHwnd, GWLP_USERDATA));\
\
        if (parent) {\
            return parent->selfPointerFunction()->wndProc(hwnd, uMsg, wParam, lParam);\
        }\
\
        return 0;\
    }\

