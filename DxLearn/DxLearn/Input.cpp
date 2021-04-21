//API(Win32)
#include <Windows.h>

//API(IO)
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

#include "Input.h"

Input::Input(const WNDCLASSEX &w, const HWND &hwnd)
{
	//Initialize array
	memset(latest_Key, 0, size_t(key_size));
	memset(old_Key, 0, size_t(key_size));

	//Initialize DirectInput
	dinput = nullptr;
	result = DirectInput8Create(
		w.hInstance,
		DIRECTINPUT_VERSION,
		IID_IDirectInput8,
		(void **)&dinput,
		nullptr
	);

	//Create keybord device
	devkeybord = nullptr;
	result = dinput->CreateDevice(GUID_SysKeyboard, &devkeybord, NULL);

	//Set data format
	result = devkeybord->SetDataFormat(&c_dfDIKeyboard);

	//Set cooperativel level
	//Active window IO only
	//No exclusive IO
	//Invalidation Windows
	result = devkeybord->SetCooperativeLevel( hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY );
}

HRESULT Input::Update()
{
	//Copy key data from the last frame
	memcpy(old_Key, latest_Key, size_t(key_size));

	//Update latest key data
	result = devkeybord->Acquire();
	result = devkeybord->GetDeviceState(sizeof(latest_Key), &latest_Key);

	return result;
}

bool Input::GetKey(const int KeyCode)
{
	return latest_Key[KeyCode] ? true : false;
}

bool Input::GetKeyDown(const int KeyCode)
{
	if (!(old_Key[KeyCode]) && latest_Key[KeyCode]) {
		return true;
	}
	return false;
}

bool Input::GetKeyUp(const int KeyCode)
{
	if (old_Key[KeyCode] && !(latest_Key[KeyCode])) {
		return true;
	}
	return false;
}
