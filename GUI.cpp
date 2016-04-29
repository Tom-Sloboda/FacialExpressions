#include "GUI.h"



GUI::GUI()
{
}

GUI::~GUI()
{
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}


void GUI::console()
{
	AllocConsole();
	freopen("conin$", "r", stdin);
	freopen("conout$", "w", stdout);
	freopen("conout$", "w", stderr);
}

HWND GUI::createScrnCapWnd(HINSTANCE hInstance)// LPSTR lpCmdLine, int nCmdShow)
{
	int nCmdShow = 1;
	const char g_szClassName[] = "myWindowClass";
	WNDCLASSEX wc;
	HWND hwnd;
	//Step 1: Registering the Window Class
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = 0;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = g_szClassName;
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	if (!RegisterClassEx(&wc))
	{
		MessageBox(NULL, "Window Registration Failed!", "Error!",
			MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}
	// Step 2: Creating the Window
	hwnd = CreateWindowEx(
		WS_EX_CLIENTEDGE|WS_EX_LAYERED,//|WS_EX_TOPMOST|WS_EX_TOOLWINDOW,
		g_szClassName,
		"Capture Window",
		WS_OVERLAPPEDWINDOW| WS_POPUP | WS_VISIBLE,
		CW_USEDEFAULT, CW_USEDEFAULT, 240, 120,
		NULL, NULL, hInstance, NULL);
	if (hwnd == NULL)
	{
		MessageBox(NULL, "Window Creation Failed!", "Error!",
			MB_ICONEXCLAMATION | MB_OK);
		return NULL;
	}
	HBRUSH brush = CreateSolidBrush(RGB(0, 0, 255));
	SetClassLongPtr(hwnd, GCLP_HBRBACKGROUND, (LONG)brush);
	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);
	BLENDFUNCTION blend;
	blend.BlendOp = AC_SRC_OVER;
	blend.BlendFlags = 0;
	blend.AlphaFormat = 0;
	blend.SourceConstantAlpha = 0.5;
	SetLayeredWindowAttributes(hwnd, RGB(0, 0, 255), 50, LWA_COLORKEY);
	HRGN winRgn = CreateRectRgn(0, 0, 100, 100);
	SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 200, 200, SWP_SHOWWINDOW);
	return hwnd;
}
