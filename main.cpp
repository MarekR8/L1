#include <windows.h>
#include "MemoryGameApp.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR pCmdLine, int nCmdShow)
{
    int boardSize = _wtoi(pCmdLine);
    if (boardSize < 3 || boardSize > 10)
    {
        boardSize = 3;
    }

    MemoryGameApp app(hInstance, boardSize);
    return app.run(nCmdShow);
}