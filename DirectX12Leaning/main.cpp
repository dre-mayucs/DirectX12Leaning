#include "includes.h"

int WINAPI WinMain(_In_ HINSTANCE hinstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd) 
{
	srand(time(nullptr));
	const int window_width = 1920;
	const int window_height = 1080;

	//Initialize
	Win32		win32(L"Test", window_width, window_height);
	Input		*input = new Input(win32.GetWindowClass(), win32.GetHandleWindow());
	DirectX12	dx12(win32.GetHandleWindow(), window_width, window_height, SelectVSYNC::EnableVSYNC);

	dx12.Initialize_components();
	ID3D12Device *dev = dx12.GetDevice();
	ID3D12GraphicsCommandList *cmdList = dx12.GetCommandList();

	//PLayer
	Draw3D drawPlayer(DrawShapeData::TriangularPyramid, 5, D3D12_FILL_MODE_SOLID, dev, cmdList, window_width, window_height);
	PlayerOP player(0, 0, 0, 5, input);

	//DrawObject
	Draw3D enemyObject(DrawShapeData::TriangularPyramid, 5, D3D12_FILL_MODE_SOLID, dev, cmdList, window_width, window_height);
	DirectX::XMFLOAT4 enemyColor = dx12.GetColor(255, 0, 0, 255);
	Position3D enemyPos = {-20, 0, 0};

	while (true)
	{
		input->Update();
		dx12.ClearDrawScreen(dx12.GetColor(100, 200, 255, 255));

		player.Update();

		if (player.GetCollition(enemyPos, 5)) {
			enemyPos.x = rand() % 100 - 50;
			enemyPos.y = rand() % 50 - 25;
		}

		drawPlayer.execute(dx12.GetColor(255, 255, 255, 255), player.GetPlayerPositionMatrix());
		enemyObject.execute(enemyColor, DirectX::XMMatrixTranslation(enemyPos.x, enemyPos.y, enemyPos.z));

		dx12.ScreenFlip();
		if (!win32.ProcessMessage()) { break; }
		if (input->GetKeyDown(DIK_ESCAPE)) { break; }
	}
	return 0;
}