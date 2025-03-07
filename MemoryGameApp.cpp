#include "MemoryGameApp.h"
#include <string>

MemoryGameApp::MemoryGameApp(HINSTANCE hInst, int boardSize)
    : hInstance(hInst), boardSize(boardSize)
{
}

int MemoryGameApp::run(int nCmdShow)
{
    const wchar_t CLASS_NAME[] = L"MemoryGameWindowClass";

    WNDCLASSEX wc = {};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.lpfnWndProc = MemoryGameApp::WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = CreateSolidBrush(RGB(255, 253, 208));
    RegisterClassEx(&wc);

    int clientWidth = boardSize * 100;
    int clientHeight = boardSize * 100;

    RECT rect = { 0, 0, clientWidth, clientHeight };
    DWORD style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
    AdjustWindowRect(&rect, style, FALSE);
    int windowWidth = rect.right - rect.left;
    int windowHeight = rect.bottom - rect.top;

    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    int x = (screenWidth - windowWidth) / 2;
    int y = (screenHeight - windowHeight) / 2;

    hwndMain = CreateWindowEx(
        0,
        CLASS_NAME,
        L"Score: 0, Naciœnij ESC, aby rozpocz¹æ!",
        style,
        x, y, windowWidth, windowHeight,
        nullptr,
        nullptr,
        hInstance,
        this
    );
    if (!hwndMain) {
        return 0;
    }
    ShowWindow(hwndMain, nCmdShow);

    // Pêtla komunikatów
    MSG msg = {};
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return (int)msg.wParam;
}

// Statyczna procedura okna – przekazuje komunikaty do metody handleMessage
LRESULT CALLBACK MemoryGameApp::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    MemoryGameApp* pThis = nullptr;
    if (uMsg == WM_NCCREATE)
    {
        // Pobranie wskaŸnika do obiektu z lParam (CREATESTRUCT)
        CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
        pThis = reinterpret_cast<MemoryGameApp*>(pCreate->lpCreateParams);
        SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pThis));
    }
    else
    {
        pThis = reinterpret_cast<MemoryGameApp*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    }
    if (pThis)
    {
        return pThis->handleMessage(hwnd, uMsg, wParam, lParam);
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

// Metoda obs³uguj¹ca komunikaty
LRESULT MemoryGameApp::handleMessage(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    case WM_KEYDOWN:
        if (wParam == VK_ESCAPE)
        {
            // Na etapie 1, naciœniêcie ESC koñczy dzia³anie aplikacji
            PostQuitMessage(0);
            return 0;
        }
        break;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
