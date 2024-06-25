#include "App.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
    tower::App app;

    app.CreateMainWindow(hInstance);
    app.EventLoop();

    return 0;
}
