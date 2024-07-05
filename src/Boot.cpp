#include "App.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
    tower::App app(hInstance);

    app.mainLoop();

    return 0;
}
