#include <windows.h>
#include "MemoryGameApp.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR pCmdLine, int nCmdShow)
{
    // Pobranie rozmiaru planszy z linii poleceñ (jeœli nie podano lub wartoœæ jest poza zakresem, u¿yjemy 3)
    int boardSize = _wtoi(pCmdLine);
    if (boardSize < 3 || boardSize > 10)
    {
        boardSize = 3;
    }

    MemoryGameApp app(hInstance, boardSize);
    return app.run(nCmdShow);
}