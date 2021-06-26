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
	Draw3D drawPlayer(L"Resources/AI.png", DrawShapeData::TriangularPyramid, 5, D3D12_FILL_MODE_SOLID, dev, cmdList, window_width, window_height);
	drawPlayer.SetRotation(DirectX::XMMatrixRotationZ(DirectX::XMConvertToRadians(-90.0f)));
	PlayerOP player(0, 0, 0, 5, input);

	//Projectile
	Draw3D DrawBullet(nullptr, DrawShapeData::TriangularPyramid, 2, D3D12_FILL_MODE_SOLID, dev, cmdList, window_width, window_height);
	DrawBullet.SetRotation(DirectX::XMMatrixRotationZ(DirectX::XMConvertToRadians(-90.0f)));
	Bullet bullet(1.0f, 3, input);

	//DrawObject
	Position3D enemyPos[2] = {
		{20, rand() % 50 - 25, 0},
		{20, rand() % 50 - 25, 0} }
	;

	bool enemyFlag[2] = { true, true };
	bool enemyTurnFlag[2] = { false, false };
	int enemyWaitTime[2] = { 0, 0 };
	float enemySpeed[2] = { 0.5f, 1.0f };

	Draw3D enemyObject (nullptr, DrawShapeData::Box, 2, D3D12_FILL_MODE_SOLID, dev, cmdList, window_width, window_height);
	Draw3D enemyObject2(nullptr, DrawShapeData::Box, 2, D3D12_FILL_MODE_SOLID, dev, cmdList, window_width, window_height);

	Draw2DGraph BG0(L"Resources/data.png", D3D12_FILL_MODE_SOLID, dev, cmdList, window_width, window_height);
	Draw2DGraph BG1(L"Resources/data.png", D3D12_FILL_MODE_SOLID, dev, cmdList, window_width, window_height);
	float xAdjust[2] = {0, 2};

	while (true)
	{
#pragma region UpdateProcess
		input->Update();

		//Background position adjust
		for (int i = 0; i < 2; i++) {
			if (xAdjust[i] <= -1.99f) {
				xAdjust[i] = 2.0f;
			}
			xAdjust[i] -= .01f;
		}

		//Enemy
		for (int i = 0; i < 2; ++i) {
			if (bullet.GetActiveFlag() && bullet.GetCollision(enemyPos[i], 2)) {
				enemyFlag[i] = false;
				bullet.SetActiveFlag(false);
			}

			if (enemyFlag[i] == false) {
				enemyWaitTime[i] += 1;

				if (enemyWaitTime[i] > 60) {
					enemyPos[i].x = rand() % 51;
					enemyPos[i].y = rand() % 50 - 25;
					enemyWaitTime[i] = 0;
					enemyFlag[i] = true;
				}
			}
			else if (enemyPos[i].y <= -25) { enemyTurnFlag[i] = true; }
			else if (enemyPos[i].y >= 25)  { enemyTurnFlag[i] = false; }

			if (enemyTurnFlag[i])	{ enemyPos[i].y += enemySpeed[i]; }
			else					{ enemyPos[i].y -= enemySpeed[i]; }
		}

		player.Update();
		bullet.Update(player.Get3DPoint());
#pragma endregion


#pragma region DrawProcess
		//Clear
		dx12.ClearDrawScreen(dx12.GetColor(100, 200, 255, 255));

		//Background image
		BG0.execute({ 0, 0, 0, 0 }, xAdjust[0]);
		BG1.execute({ 0, 0, 0, 0 }, xAdjust[1]);

		//player
		drawPlayer.execute(dx12.GetColor(255, 255, 255, 255), player.GetPlayerPositionMatrix());

		//enemy
		if (enemyFlag[0]) {
			enemyObject.execute(dx12.GetColor(0, 200, 255, 255), DirectX::XMMatrixTranslation(enemyPos[0].x, enemyPos[0].y, enemyPos[0].z));
		}
		if (enemyFlag[1]) {
			enemyObject2.execute(dx12.GetColor(255, 153, 255, 255), DirectX::XMMatrixTranslation(enemyPos[1].x, enemyPos[1].y, enemyPos[1].z));
		}

		//projectile
		if (bullet.GetActiveFlag()) {
			DrawBullet.execute(dx12.GetColor(138, 119, 183, 255), bullet.GetBulletPositionMatrix());
		}
#pragma endregion

		dx12.ScreenFlip();
		if (!win32.ProcessMessage()) { break; }
		if (input->GetKeyDown(keycode::Escape)) { break; }
	}

	delete input;
	return 0;
}