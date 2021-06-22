#include "includes.h"
#include "Draw3D.h"

const int window_width = 1920;
const int window_height = 1080;

int WINAPI WinMain(_In_ HINSTANCE hinstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd) 
{
	Win32 win32(L"Test", window_width, window_height);
	DirectX12 dx12(win32.hwnd, window_width, window_height);

	dx12.Initialize_components();

	Input input(win32.w, win32.hwnd);
	Player player;
	player.SetSpeed(0.01f);

	//InitializeDrawCommands
	/*Draw Circle(100, 0.1, D3D12_FILL_MODE_SOLID, dx12.dev, dx12.cmdList, window_width, window_height);
	Draw Circle2(50, 1, D3D12_FILL_MODE_SOLID, dx12.dev, dx12.cmdList, window_width, window_height);*/
	Draw3D hoge0(3, 10, D3D12_FILL_MODE_SOLID, dx12.dev, dx12.cmdList, window_width, window_height);
	Draw3D hoge1(3, 10, D3D12_FILL_MODE_SOLID, dx12.dev, dx12.cmdList, window_width, window_height);

	//Clear color
	DirectX::XMFLOAT4 clearColor;

	//Circle color
	DirectX::XMFLOAT4 circleColor = dx12.GetColor(255, 0, 255, 255);

	while (true)
	{
		input.Update();
		dx12.ClearDrawScreen(dx12.GetColor(100, 200, 255, 255));

		hoge0.execute(dx12.GetColor(255, 255, 255, 255), DirectX::XMMatrixTranslation(0.0f, 0.0f, 0.0f));
		hoge1.execute(dx12.GetColor(0, 0, 0, 255), DirectX::XMMatrixTranslation(-20.0f, 0.0f, 0.0f));

		dx12.ScreenFlip();
		if (!win32.ProcessMessage()) { break; }
		if (input.GetKeyDown(DIK_ESCAPE)) { break; }
	}
	return 0;
}