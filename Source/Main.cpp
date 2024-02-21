#include <windows.h>
#include <memory>
#include <assert.h>
#include <tchar.h>
#include "../resource.h"

#include "Framework.h"

#if 0
const LONG SCREEN_WIDTH = 1280;
const LONG SCREEN_HEIGHT = 720;
#else
const LONG SCREEN_WIDTH = 1920;
const LONG SCREEN_HEIGHT = 1080;
#endif

LRESULT CALLBACK fnWndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	Framework *f = reinterpret_cast<Framework*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
	return f ? f->HandleMessage(hwnd, msg, wparam, lparam) : DefWindowProc(hwnd, msg, wparam, lparam);
}

INT WINAPI wWinMain( _In_ HINSTANCE instance, _In_opt_ HINSTANCE prev_instance, _In_ LPWSTR cmd_line, _In_ INT cmd_show)
{
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtSetBreakAlloc(14000);
#endif
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = fnWndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = instance;
	wcex.hIcon = 0;
	wcex.hIcon = static_cast<HICON>(LoadImage(instance, MAKEINTRESOURCE(IDI_ICON1),IMAGE_ICON,64,64,0));
	wcex.hCursor = static_cast<HCURSOR>(LoadImage(NULL, MAKEINTRESOURCE(IDC_ARROW), IMAGE_CURSOR,0,0,LR_DEFAULTSIZE|LR_SHARED));
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = _T("Game");
	wcex.hIconSm = static_cast<HICON>(LoadImage(instance, MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 32, 32, 0));
	RegisterClassEx(&wcex);

	DWORD windowStyle = WS_BORDER;
	//RECT rc = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
	RECT rc = {0,0,GetSystemMetrics(SM_CXSCREEN),GetSystemMetrics(SM_CYSCREEN)};
	AdjustWindowRect(&rc, windowStyle, FALSE);
	HWND hWnd = CreateWindow(_T("Game"), _T("PIRATES OF LILY"), windowStyle, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, instance, NULL);
	SetWindowLong(hWnd,GWL_STYLE,WS_POPUP);
	SetWindowPos(hWnd,NULL,0,0,GetSystemMetrics(SM_CXSCREEN),GetSystemMetrics(SM_CYSCREEN),SWP_FRAMECHANGED);
	ShowWindow(hWnd, cmd_show);


	Framework f(hWnd);
	SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(&f));
	return f.Run();
}
