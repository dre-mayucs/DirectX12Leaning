#pragma once

//API(IO)
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

enum class keycode : int {
	Escape = 0x01,
	Alpha1 = 0x02,
	Alpha2 = 0x03,
	Alpha3 = 0x04,
	Alpha4 = 0x05,
	Alpha5 = 0x06,
	Alpha6 = 0x07,
	Alpha7 = 0x08,
	Alpha8 = 0x09,
	Alpha9 = 0x0A,
	Alpha0 = 0x0B,
	minus = 0x0C,
	equal = 0x0D,
	back = 0x0E,
	tab = 0x0F,
	Q = 0x10,
	W = 0x11,
	E = 0x12,
	R = 0x13,
	T = 0x14,
	Y = 0x15,
	U = 0x16,
	I = 0x17,
	O = 0x18,
	P = 0x19,
	LeftSquareBracket = 0x1A,
	RightSquareBracket = 0x1B,
	Return = 0x1C,
	LeftControll = 0x1D,
	A = 0x1E,
	S = 0x1F,
	D = 0x20,
	F = 0x21,
	G = 0x22,
	H = 0x23,
	J = 0x24,
	K = 0x25,
	L = 0x26,
	Semicoron = 0x27,
	Apostrophe = 0x28,
	GraveAccent = 0x29,
	LeftShift = 0x2A,
	BackSlash = 0x2B,
	Z = 0x2C,
	X = 0x2D,
	C = 0x2E,
	V = 0x2F,
	B = 0x30,
	N = 0x31,
	M = 0x32,
	Comma = 0x33,
	//Period				= 0x34,
	Slash = 0x35,
	RightShift = 0x36,
	KeypadAsterisk = 0x37,
	LeftAlt = 0x38,
	Space = 0x39,
	CapsLock = 0x3A,
	F1 = 0x3B,
	F2 = 0x3C,
	F3 = 0x3D,
	F4 = 0x3E,
	F5 = 0x3F,
	F6 = 0x40,
	F7 = 0x41,
	F8 = 0x42,
	F9 = 0x43,
	F10 = 0x44,
	Numlock = 0x45,
	Scroll = 0x46,
	Keypad7 = 0x47,
	Keypad8 = 0x48,
	Keypad9 = 0x49,
	KeypadMinus = 0x4A,
	Keypad4 = 0x4B,
	Keypad5 = 0x4C,
	Keypad6 = 0x4D,
	KeypadPlus = 0x4E,
	Keypad1 = 0x4F,
	Keypad2 = 0x50,
	Keypad3 = 0x51,
	Keypad0 = 0x52,
	//Period				= 0x53,
	F11 = 0x57,
	F12 = 0x58,
	F13 = 0x64,
	F14 = 0x65,
	F15 = 0x66,
	Kana = 0x70,
	Convert = 0x79,
	Noconvert = 0x7B,
	KeypadEqual = 0x8D,
	Pevtrack = 0x90,
	At = 0x91,
	Coron = 0x92,
	Stop = 0x95,
	Ax = 0x96,
	Unlabeled = 0x97,
	Nexttrack = 0x99,
	KeypadEnter = 0x9C,
	RightControll = 0x9D,
	Mute = 0xA0,
	VolumeMinus = 0xAE,
	VolumePlus = 0xB0,
	KeypadComma = 0xB3,
	Pause = 0xC5,
	Home = 0xC7,
	UpArrow = 0xC8,
	PageUp = 0xC9,
	LeftArrow = 0xCB,
	RightArrow = 0xCD,
	End = 0xCF,
	DownAllow = 0xD0,
	PageDown = 0xD1,
	Insert = 0xD2,
	Delete = 0xD3,
	LeftWindows = 0xDB,
	RightWindows = 0xDC,
};

//key data size
const int key_size = 256;

class Input
{
private:
	IDirectInput8 *dinput;
	IDirectInputDevice8 *devkeybord;

	HRESULT result;
	BYTE latest_Key[key_size];
	BYTE old_Key[key_size];


public:
	Input(const WNDCLASSEX &w, const HWND &hwnd);

	/// <summary>
	/// Update input key data
	/// </summary>
	/// <returns>HRESULT</returns>
	HRESULT Update();

	/// <summary>
	/// Get key press
	/// </summary>
	/// <param name="KeyCode">DIK</param>
	/// <returns>bool</returns>
	bool GetKey(keycode KeyCode);

	/// <summary>
	/// Get key down
	/// </summary>
	/// <param name="KeyCode">DIK</param>
	/// <returns>bool</returns>
	bool GetKeyDown(keycode KeyCode);

	/// <summary>
	/// Get key down
	/// </summary>
	/// <param name="KeyCode">DIK</param>
	/// <returns>bool</returns>
	bool GetKeyUp(keycode KeyCode);
};