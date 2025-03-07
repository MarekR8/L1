#include "MemoryGameApp.h"
#include <string>

MemoryGameApp::MemoryGameApp(HINSTANCE hInst, int boardSize)
    : hInstance(hInst), boardSize(boardSize), board(boardSize)
{
    // Utworzenie pêdzla dla pól (kolor #7C0A02)
    hTileBrush = CreateSolidBrush(RGB(124, 10, 2));
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
    // T³o g³ównego okna: #FFFDD0
    wc.hbrBackground = CreateSolidBrush(RGB(255, 253, 208));

    RegisterClassEx(&wc);

    // Obliczenie rozmiaru obszaru klienta: boardSize * 100 (szerokoœæ i wysokoœæ komórki)
    int clientWidth = boardSize * board.getCellWidth();
    int clientHeight = boardSize * board.getCellHeight();

    RECT rect = { 0, 0, clientWidth, clientHeight };
    DWORD style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
    AdjustWindowRect(&rect, style, FALSE);
    int windowWidth = rect.right - rect.left;
    int windowHeight = rect.bottom - rect.top;

    // Centrowanie okna na ekranie
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    int x = (screenWidth - windowWidth) / 2;
    int y = (screenHeight - windowHeight) / 2;

    // Tworzenie g³ównego okna – przekazujemy wskaŸnik do obiektu w lpParam
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
    if (!hwndMain)
    {
        return 0;
    }
    ShowWindow(hwndMain, nCmdShow);

    MSG msg = {};
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return (int)msg.wParam;
}

LRESULT CALLBACK MemoryGameApp::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    MemoryGameApp* pThis;
    if (uMsg == WM_NCCREATE)
    {
        // Pobranie wskaŸnika do obiektu z CREATESTRUCT
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

LRESULT MemoryGameApp::handleMessage(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CREATE:
        // Utworzenie pól planszy (okien potomnych)
        createChildWindows(hwnd);
        return 0;
    case WM_CTLCOLORSTATIC:
    {
        // Ustawienie koloru t³a dla kontrolek STATIC (naszych pól)
        HDC hdcStatic = (HDC)wParam;
        SetBkMode(hdcStatic, TRANSPARENT);
        return (INT_PTR)hTileBrush;
    }
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    case WM_KEYDOWN:
        if (wParam == VK_ESCAPE)
        {
            PostQuitMessage(0);
            return 0;
        }
        break;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void MemoryGameApp::createChildWindows(HWND hwnd)
{
    const std::vector<RECT>& cells = board.getCells();
    for (size_t i = 0; i < cells.size(); i++)
    {
        const RECT& rc = cells[i];
        CreateWindowEx(
            0,
            L"STATIC",
            nullptr,
            WS_CHILD | WS_VISIBLE | SS_CENTER,
            rc.left, rc.top,
            rc.right - rc.left, rc.bottom - rc.top,
            hwnd,
            nullptr,
            hInstance,
            nullptr
        );
    }
}
