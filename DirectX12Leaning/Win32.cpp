#include <Windows.h>
#include "Win32.h"

LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	if (msg == WM_DESTROY) {
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hwnd, msg, wparam, lparam);
}

Win32::Win32(const wchar_t *DisplayName, const unsigned int DisplaySize_X, const unsigned int DisplaySize_Y) : DisplayName(DisplayName)
{
	w.cbSize = sizeof(WNDCLASSEX);
	w.lpfnWndProc = (WNDPROC)WindowProc;
	w.lpszClassName = DisplayName;
	w.hInstance = GetModuleHandle(nullptr);
	w.hCursor = LoadCursor(NULL, IDC_ARROW);

	//Window class add
	RegisterClassEx(&w);

	//Window Size
	RECT wrc = { 0, 0, DisplaySize_X, DisplaySize_Y };
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

	hwnd = CreateWindow(
		w.lpszClassName,		//Class name
		DisplayName,			//Title bar
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

bool Win32::ProcessMessage()
{
	if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return !(msg.message == WM_QUIT);
}

const wchar_t *Win32::GetDisplayname() const
{
	return DisplayName;
}

WNDCLASSEX Win32::GetWindowClass() const
{
	return w;
}

HWND Win32::GetHandleWindow() const
{
	return hwnd;
}
