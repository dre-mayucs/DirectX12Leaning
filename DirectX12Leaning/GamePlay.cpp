#include "includes.h"
#include "GamePlay.h"

GamePlay::GamePlay(Win32 *win32,DirectX12 *dx12, Input *input, const int window_width, const int window_height) : 
	win32(win32),
	dx12(dx12),
	input(input),
	window_height(window_height),
	window_width(window_width)
{
	dev = dx12->GetDevice();
	cmdList = dx12->GetCommandList();

	player = PlayerOP(0, 0, 0, 5, input);
	drawPlayer = new Draw3D(L"Resources/AI.png", DrawShapeData::TriangularPyramid, 5, D3D12_FILL_MODE_SOLID, dev, cmdList, window_width, window_height);
	drawPlayer->SetRotation(DirectX::XMMatrixRotationZ(DirectX::XMConvertToRadians(-90.0f)));

	bullet = Bullet(1.0f, 3, input, dx12, window_width, window_height);

	enemyObject = std::vector<Draw3D *>(2);
	for (auto i = 0; i < enemyObject.size(); i++) {
		enemyObject[i] = new Draw3D(L"Resources/seven.png", DrawShapeData::Box, 2, D3D12_FILL_MODE_SOLID, dev, cmdList, window_width, window_height);
	}

	Background = std::vector<Draw2DGraph*>(2);
	for (auto i = 0; i < Background.size(); i++) {
		Background[i] = new Draw2DGraph(L"Resources/data.png", D3D12_FILL_MODE_SOLID, dev, cmdList, window_width, window_height);
	}
}

GamePlay::~GamePlay()
{
	delete drawPlayer;

	for (auto v : enemyObject) {
		delete v;
	}

	for (auto v : Background) {
		delete v;
	}
}

void GamePlay::Update()
{
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
			else if (enemyPos[i].y >= 25) { enemyTurnFlag[i] = false; }

			if (enemyTurnFlag[i]) { enemyPos[i].y += enemySpeed[i]; }
			else { enemyPos[i].y -= enemySpeed[i]; }
		}
#pragma endregion

#pragma region DrawProcess
		//Clear
		dx12->ClearDrawScreen(dx12->GetColor(100, 200, 255, 255));
		player.Update();
		bullet.Update(player.Get3DPoint());

		//Background image
		for (auto i = 0; i < Background.size(); i++) {
			Background[i]->execute({ 0, 0, 0, 0 }, xAdjust[i]);
		}

		//player
		drawPlayer->execute(dx12->GetColor(255, 255, 255, 255), player.GetPlayerPositionMatrix());

		//enemy
		for (auto i = 0; i < enemyObject.size(); i++) {
			if (enemyFlag[i]) {
				enemyObject[i]->execute(dx12->GetColor(255, 255, 255, 255), DirectX::XMMatrixTranslation(enemyPos[i].x, enemyPos[i].y, enemyPos[i].z));
			}
		}
#pragma endregion

		dx12->ScreenFlip();
		if (!win32->ProcessMessage()) { break; }
		if (input->GetKeyDown(keycode::Escape)) { break; }
	}
}
