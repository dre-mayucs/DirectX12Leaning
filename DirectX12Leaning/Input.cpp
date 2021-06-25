//API(Win32)
#include <Windows.h>

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

bool Input::GetKey(keycode KeyCode)
{
	return latest_Key[static_cast<int>(KeyCode)];
}

bool Input::GetKeyDown(keycode KeyCode)
{
	return !(old_Key[static_cast<int>(KeyCode)]) && latest_Key[static_cast<int>(KeyCode)];
}

bool Input::GetKeyUp(keycode KeyCode)
{
	return old_Key[static_cast<int>(KeyCode)] && !(latest_Key[static_cast<int>(KeyCode)]);
}
