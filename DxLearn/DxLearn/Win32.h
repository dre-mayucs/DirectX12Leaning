#pragma once
class Win32
{
public:
	const wchar_t *DisplayName;
	const unsigned int DisplaySize_X;
	const unsigned int DisplaySize_Y;

	WNDCLASSEX w{};
	MSG msg{};
	HWND hwnd;

public:
	//Constructor
	Win32(const wchar_t *DisplayName, const unsigned int DisplaySize_X, const unsigned int DisplaySize_Y);
};