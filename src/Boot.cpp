#include "App.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
    tower::App app;

    app.createMainWindow(hInstance);
    app.eventLoop();

    return 0;
}
