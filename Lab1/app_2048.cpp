#include "app_2048.h"
#include "resource.h"
#include <stdexcept>
#include <dwmapi.h> //do poprawnego nachodzenia sie okienek przy przezroczystosci
#include <chrono>

std::wstring const app_2048::s_class_name{ L"2048 Window" };

bool app_2048::register_class() {
	WNDCLASSEXW desc{};
	if (GetClassInfoExW(m_instance, s_class_name.c_str(),
		&desc) != 0) return true;
	//desc = { .cbSize = sizeof(WNDCLASSEXW),
	//.lpfnWndProc = window_proc_static,
	//.hInstance = m_instance,
	//.hCursor = LoadCursorW(nullptr, L"IDC_ARROW"),
	//.hbrBackground =
	//	CreateSolidBrush(RGB(250, 247, 238)),
	//.lpszClassName = s_class_name.c_str() };
	desc = {
		.cbSize = sizeof(WNDCLASSEXW),
		.lpfnWndProc = window_proc_static,
		.hInstance = m_instance,
		.hIcon = static_cast<HICON>(LoadImageW(
		m_instance,
		MAKEINTRESOURCEW(ID_APPICON),
		IMAGE_ICON,
		0, 0,
		LR_SHARED | LR_DEFAULTSIZE)),
		/*you can also use:
		LoadIconW(m_instance,
		MAKEINTRESOURCEW(ID_APPICON)) */
		.hCursor = LoadCursorW(nullptr, L"IDC_ARROW"),
		.hbrBackground =
		CreateSolidBrush(RGB(250, 247, 238)),
		.lpszMenuName =
		MAKEINTRESOURCEW(ID_MAINMENU), //add this
		.lpszClassName = s_class_name.c_str()
	};
	return RegisterClassExW(&desc) != 0;
}
HWND app_2048::create_window(DWORD style, HWND parent,
	DWORD ex_style)
{
	RECT size{ 0, 0, board::width, board::height };
	AdjustWindowRectEx(&size, style, true, 0);
	HWND window = CreateWindowExW(ex_style,
		s_class_name.c_str(), L"2048", style,
		CW_USEDEFAULT, 0,
		size.right - size.left, size.bottom - size.top,
		parent, nullptr, m_instance, this);
	/*for (auto& f : m_board.fields())
		CreateWindowExW(
			0,
			L"STATIC",
			nullptr,
			WS_CHILD | WS_VISIBLE | SS_CENTER,
			f.position.left, f.position.top,
			f.position.right - f.position.left,
			f.position.bottom - f.position.top,
			window,
			nullptr,
			m_instance,
			nullptr);*/
	return window;
}
LRESULT app_2048::window_proc_static(
	HWND window,
	UINT message,
	WPARAM wparam,
	LPARAM lparam)
{
	app_2048* app = nullptr;
	if (message == WM_NCCREATE)
	{
		auto p = reinterpret_cast<LPCREATESTRUCTW>(lparam);
		app = static_cast<app_2048*>(p-> lpCreateParams);
		SetWindowLongPtrW(window, GWLP_USERDATA,
			reinterpret_cast<LONG_PTR>(app));
	}
	else
	{
		app = reinterpret_cast<app_2048*>(
			GetWindowLongPtrW(window, GWLP_USERDATA));
	}
	if (app != nullptr)
	{
		return app->window_proc(window, message,
			wparam, lparam);
	}
	return DefWindowProcW(window, message, wparam, lparam);
}
LRESULT app_2048::window_proc(
	HWND window, UINT message,
	WPARAM wparam, LPARAM lparam)
{
	switch (message) {
	case WM_TIMER:
		on_timer();
		return 0;
	case WM_CLOSE:
		DestroyWindow(window);
		return 0;
	case WM_DESTROY:
		if (window == m_main)
			PostQuitMessage(EXIT_SUCCESS);
		return 0;
	//case WM_CTLCOLORSTATIC:
	//	return reinterpret_cast<INT_PTR>(m_field_brush);
	case WM_WINDOWPOSCHANGED:
		on_window_move(window,
			reinterpret_cast<LPWINDOWPOS>(lparam));
		return 0;
	case WM_COMMAND:
		on_command(LOWORD(wparam));
		return 0;
	case WM_PAINT:
		on_paint(window);
		return 0;
	case WM_LBUTTONDOWN:
	{
			int x = GET_X_LPARAM(lparam);
			int y = GET_Y_LPARAM(lparam);
			on_mouse_click(window, x, y);
			}
			return 0;
	}
	return DefWindowProcW(window, message, wparam, lparam);
}

app_2048::app_2048(HINSTANCE instance)
	: m_instance{ instance }, m_main{}, m_popup{},
	m_field_brush{ CreateSolidBrush(RGB(204, 192, 174)) },
	m_screen_size{ GetSystemMetrics(SM_CXSCREEN),
		GetSystemMetrics(SM_CYSCREEN) }
{
	register_class();
	DWORD main_style = WS_OVERLAPPED | WS_SYSMENU |
		WS_CAPTION | WS_MINIMIZEBOX;
	DWORD popup_style = WS_OVERLAPPED | WS_CAPTION;
	m_main = create_window(main_style);
	m_popup = create_window(
		popup_style, m_main, WS_EX_LAYERED);
	SetLayeredWindowAttributes(m_popup, 0, 255, LWA_ALPHA);
}

int app_2048::run(int show_command)
{
	HACCEL shortcuts = LoadAcceleratorsW(m_instance,
		MAKEINTRESOURCEW(ID_SHORTCUTS));
	ShowWindow(m_main, show_command);
	ShowWindow(m_popup, SW_SHOWNA);
	SetTimer(m_main, s_timer, 1000, nullptr);
	m_startTime = std::chrono::system_clock::now();
	MSG msg{};
	BOOL result = TRUE;
	while ((result = GetMessageW(&msg, nullptr, 0, 0)) != 0)
	{
		if (result == -1)
			return EXIT_FAILURE;
		if (!TranslateAcceleratorW(
			msg.hwnd, shortcuts, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}
	}
	return EXIT_SUCCESS;
}

void app_2048::on_window_move(
	HWND window,
	LPWINDOWPOS params)
{
	HWND other = (window == m_main) ? m_popup : m_main;
	RECT other_rc;
	GetWindowRect(other, &other_rc);
	SIZE other_size{
	.cx = other_rc.right - other_rc.left,
	.cy = other_rc.bottom - other_rc.top };
	POINT Cw{
		.x = params->x + params->cx / 2,
		.y = params->y + params->cy / 2
	};

	// Calculate the center of the other window
	POINT Co{
		.x = m_screen_size.x - Cw.x,
		.y = m_screen_size.y - Cw.y
	};

	// Calculate the top-left position of the other window
	POINT new_pos{
		.x = Co.x - other_size.cx / 2,
		.y = Co.y - other_size.cy / 2
	};
	if (new_pos.x == other_rc.left &&
		new_pos.y == other_rc.top)
		return;
	SetWindowPos(other, nullptr, new_pos.x, new_pos.y,
		0, 0, SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOZORDER);
	update_transparency();
}

void app_2048::update_transparency()
{
	RECT main_rc, popup_rc, inter;
	DwmGetWindowAttribute(m_main,DWMWA_EXTENDED_FRAME_BOUNDS,&main_rc, sizeof(RECT));
	DwmGetWindowAttribute(m_popup,DWMWA_EXTENDED_FRAME_BOUNDS,
		&popup_rc, sizeof(RECT));
	IntersectRect(&inter, &main_rc, &popup_rc);
	BYTE a = IsRectEmpty(&inter) ? 255 : 255 * 50 / 100;
	SetLayeredWindowAttributes(m_popup, 0, a, LWA_ALPHA);
}

void app_2048::on_timer()
{
	using namespace std::chrono;
	auto title = std::format(L"2048 {:%M:%S}",
		duration_cast<duration<int>>(
			system_clock::now() - m_startTime));
	SetWindowTextW(m_main, title.c_str());
	SetWindowTextW(m_popup, title.c_str());
}
void app_2048::on_command(WORD cmdID)
{
	switch (cmdID)
	{
	case ID_NEWGAME:
		m_startTime = std::chrono::system_clock::now();
		on_timer();
		break;
	}
}
void app_2048::on_paint(HWND window)
{
	PAINTSTRUCT ps;
	HDC dc = BeginPaint(window, &ps);
	/*auto oldBrush = SelectObject(dc, m_field_brush);
	auto oldPen = SelectObject(dc,
		GetStockObject(NULL_PEN));
	for (auto& f : m_board.fields())
		RoundRect(dc, f.position.left, f.position.top,
			f.position.right, f.position.bottom, 11, 11);
	SelectObject(dc, oldBrush);
	SelectObject(dc, oldPen);
	EndPaint(window, &ps);
	DeleteObject(SelectObject(dc, oldPen));*/
	for (auto& f : m_board.fields())
		{
		HBRUSH brush = CreateSolidBrush(f.color);
		HBRUSH oldBrush = (HBRUSH)SelectObject(dc, brush);
		HPEN oldPen = (HPEN)SelectObject(dc, GetStockObject(NULL_PEN));
		RoundRect(dc, f.position.left, f.position.top,
			f.position.right, f.position.bottom, 11, 11);
		SelectObject(dc, oldBrush);
		SelectObject(dc, oldPen);
		DeleteObject(brush);
		}
	EndPaint(window, &ps);
}
void app_2048::on_mouse_click(HWND window, int x, int y)
{
	POINT pt{ x, y };
	bool updated = false;
	// Sprawdzenie, czy klikni�cie nast�pi�o na kt�rym� z p�l
	for (auto& f : m_board.fields()) {
		if (PtInRect(&f.position, pt)) {
			// Prze��czenie koloru � je�li domy�lny, ustaw nowy (np. CornflowerBlue), inaczej przywr�� domy�lny
			if (f.color == RGB(204, 192, 174))
				f.color = RGB(100, 149, 237); // nowy kolor
			else
				f.color = RGB(204, 192, 174); // przywr�cenie domy�lnego koloru
			updated = true;
			break; // zmieniono tylko jedno pole
		}
	}
	if (updated) {
		InvalidateRect(window, nullptr, TRUE); // od�wie�enie okna, by zmiana by�a widoczna
	}
}