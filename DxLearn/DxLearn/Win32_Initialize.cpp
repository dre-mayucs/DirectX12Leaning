#include <Windows.h>

#include "Win32_Initialize.h"

LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	if (msg == WM_DESTROY) {
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hwnd, msg, wparam, lparam);
}

void Win32_Initialize::Win32_Initialize_Conponents(WNDCLASSEX &w, HWND &hwnd)
{
	OutputDebugStringA("Hello,DirectX!!\n");

	w.cbSize = sizeof(WNDCLASSEX);
	w.lpfnWndProc = (WNDPROC)WindowProc;
	w.lpszClassName = L"DirectXGame";
	w.hInstance = GetModuleHandle(nullptr);
	w.hCursor = LoadCursor(NULL, IDC_ARROW);

	//Window class add
	RegisterClassEx(&w);

	//Window Size
	RECT wrc = { 0, 0, window_width, window_height };
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

	hwnd = CreateWindow(
		w.lpszClassName,		//Class name
		L"DirectXGame",			//Title bar
		WS_OVERLAPPEDWINDOW,	//Window type
		CW_USEDEFAULT,			//Window position
		CW_USEDEFAULT,			//Window position
		wrc.right - wrc.left,	//Window width
		wrc.bottom - wrc.top,	//Window height
		nullptr,				//Window handle
		nullptr,				//Menu handle
		w.hInstance,			//Call application handke
		nullptr					//param
	);

	//View window
	ShowWindow(hwnd, SW_SHOW);
}
