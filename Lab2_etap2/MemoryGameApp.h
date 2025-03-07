#pragma once
#include <windows.h>
#include "Board.h"
#include <vector>

class MemoryGameApp {
public:
    MemoryGameApp(HINSTANCE hInst, int boardSize);
    int run(int nCmdShow);
private:
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT handleMessage(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    void createChildWindows(HWND hwnd);

    HINSTANCE hInstance;
    HWND hwndMain;
    int boardSize;
    Board board;
    HBRUSH hTileBrush; // Pêdzel do pól (kolor #7C0A02)
};
