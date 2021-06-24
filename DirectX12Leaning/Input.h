#pragma once

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
	bool GetKey(const int KeyCode);

	/// <summary>
	/// Get key down
	/// </summary>
	/// <param name="KeyCode">DIK</param>
	/// <returns>bool</returns>
	bool GetKeyDown(const int KeyCode);

	/// <summary>
	/// Get key down
	/// </summary>
	/// <param name="KeyCode">DIK</param>
	/// <returns>bool</returns>
	bool GetKeyUp(const int KeyCode);
};