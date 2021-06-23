#include "includes.h"

int WINAPI WinMain(_In_ HINSTANCE hinstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd) 
{
	const int window_width = 1920;
	const int window_height = 1080;

	//Initialize
	Win32		win32(L"Test", window_width, window_height);
	Input		input(win32.GetWindowClass(), win32.GetHandleWindow());
	DirectX12	dx12(win32.GetHandleWindow(), window_width, window_height);

	dx12.Initialize_components();
	ID3D12Device *dev = dx12.GetDevice();
	ID3D12GraphicsCommandList *cmdList = dx12.GetCommandList();

	//DrawObject
	Draw2D Circle(100, 0.5, D3D12_FILL_MODE_SOLID, dev, cmdList, window_width, window_height);
	Draw3D hoge0(   3,	10, D3D12_FILL_MODE_SOLID, dev, cmdList, window_width, window_height);
	Draw3D hoge1(   3,	10, D3D12_FILL_MODE_SOLID, dev, cmdList, window_width, window_height);

	while (true)
	{
		input.Update();
		dx12.ClearDrawScreen(dx12.GetColor(100, 200, 255, 255));

		Circle.execute(dx12.GetColor(255, 255, 255, 255));
		hoge0.execute(dx12.GetColor(255, 255, 255, 255), DirectX::XMMatrixTranslation(0.0f, 0.0f, 0.0f));
		hoge1.execute(dx12.GetColor(0, 0, 0, 255), DirectX::XMMatrixTranslation(-20.0f, 0.0f, 0.0f));

		dx12.ScreenFlip();
		if (!win32.ProcessMessage()) { break; }
		if (input.GetKeyDown(DIK_ESCAPE)) { break; }
	}
	return 0;
}