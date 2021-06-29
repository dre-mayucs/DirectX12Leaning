#include "includes.h"
#include "GamePlay.h"

int WINAPI WinMain(_In_ HINSTANCE hinstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd) 
{
	srand(time(nullptr));
	const int window_width = 1920;
	const int window_height = 1080;

	//Initialize
	Win32		*win32 = new Win32(L"Test", window_width, window_height);
	Input		*input = new Input(win32->GetWindowClass(), win32->GetHandleWindow());
	DirectX12	*dx12 = new DirectX12(win32->GetHandleWindow(), window_width, window_height, SelectVSYNC::EnableVSYNC);

	dx12->Initialize_components();

	GamePlay play(win32, dx12, input, window_width, window_height);
	play.Update();

	delete win32, dx12, input;
	return 0;
}