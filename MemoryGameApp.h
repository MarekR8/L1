#pragma once
#include <windows.h>

class MemoryGameApp {
public:
    MemoryGameApp(HINSTANCE hInst, int boardSize);
    int run(int nCmdShow);
private:
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT handleMessage(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    HINSTANCE hInstance;
    HWND hwndMain;
    int boardSize;
};
