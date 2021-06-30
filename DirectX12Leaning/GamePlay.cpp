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

	SceneNum = 0;

	player = PlayerOP(0, 0, 0, 5, input);
	drawPlayer = new Draw3D(L"Resources/AI.png", DrawShapeData::TriangularPyramid, 5, D3D12_FILL_MODE_SOLID, dev, cmdList, window_width, window_height);
	drawPlayer->SetRotation(DirectX::XMMatrixRotationZ(DirectX::XMConvertToRadians(-90.0f)));

	bullet = Bullet(1.0f, 3, input);
	DrawBullet = new Draw3D(L"Resources/senju.png", DrawShapeData::TriangularPyramid, 2, D3D12_FILL_MODE_SOLID, dev, cmdList, window_width, window_height);
	DrawBullet->SetRotation(DirectX::XMMatrixRotationZ(DirectX::XMConvertToRadians(-90.0f)));

	enemyObject = std::vector<Draw3D *>(2);
	for (auto i = 0; i < enemyObject.size(); i++) {
		enemyObject[i] = new Draw3D(L"Resources/seven.png", DrawShapeData::Box, 2, D3D12_FILL_MODE_SOLID, dev, cmdList, window_width, window_height);
	}

	Background = std::vector<Draw2DGraph*>(2);
	for (auto i = 0; i < Background.size(); i++) {
		Background[i] = new Draw2DGraph(L"Resources/data.png", D3D12_FILL_MODE_SOLID, dev, cmdList, window_width, window_height);
	}

	titleFlag = false;
	alpha = 255;
	Timer = 0;
	TitleBG = new Draw2DGraph(L"Resources/TitleBG.png", D3D12_FILL_MODE_SOLID, dev, cmdList, window_width, window_height);
	TitleMessage = new Draw2DGraph(L"Resources/PressMessage.png", D3D12_FILL_MODE_SOLID, dev, cmdList, window_width, window_height);
	BackHome = new Draw2DGraph(L"Resources/BackHome.png", D3D12_FILL_MODE_SOLID, dev, cmdList, window_width, window_height);
}

GamePlay::~GamePlay()
{
	delete drawPlayer;
	delete DrawBullet;
	delete TitleBG, TitleMessage, BackHome;

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
		//Clear
		dx12->ClearDrawScreen(dx12->GetColor(30, 30, 30, 255));
		input->Update();

		switch (SceneNum)
		{
			case 0: {
				Title();
				break;
			}
			case 1: {
				GameScene();
				break;
			}

			default: break;
		}

		dx12->ScreenFlip();
		if (!win32->ProcessMessage()) { break; }
		if (input->GetKeyDown(keycode::Escape)) { break; }
	}
}

void GamePlay::Title()
{
#pragma region UpdatePorocess
	Timer++;
	if (Timer > 60) {
		Timer = 0;
	}
	if (input->GetKeyDown(keycode::Space)) {
		titleFlag = true;
	}

	if (titleFlag == true) {
		alpha -= 3.0f;
	}

	if (alpha <= 0.1f) {
		SceneNum = 1;
		titleFlag = false;
	}
#pragma endregion

#pragma region DrawProcess
	TitleBG->execute(dx12->GetColor(255, 255, 255, alpha), 0.0f);

	if (Timer <= 30) {
		TitleMessage->execute(dx12->GetColor(255, 255, 255, alpha), 0.0f);
	}
#pragma endregion
}

void GamePlay::GameScene()
{
#pragma region UpdateProcess
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

		if (enemyFlag[i] == true && enemyPos[i].y <= -25) { enemyTurnFlag[i] = true; }
		if (enemyFlag[i] == true && enemyPos[i].y >= 25) { enemyTurnFlag[i] = false; }

		if (enemyTurnFlag[i]) { enemyPos[i].y += enemySpeed[i]; }
		else { enemyPos[i].y -= enemySpeed[i]; }

		enemyObject[i]->SetRotation(DirectX::XMMatrixRotationY(DirectX::XMConvertToRadians(-10.0f)));
	}

	player.Update();
	bullet.Update(player.Get3DPoint());
#pragma endregion

#pragma region DrawProcess

	//Background image
	for (auto i = 0; i < Background.size(); i++) {
		Background[i]->execute(dx12->GetColor(255, 255, 255, alpha), xAdjust[i]);
	}

	//player
	drawPlayer->execute(dx12->GetColor(255, 255, 255, alpha), player.GetPlayerPositionMatrix());

	//enemy
	for (auto i = 0; i < enemyObject.size(); i++) {
		if (enemyFlag[i]) {
			enemyObject[i]->execute(dx12->GetColor(255, 255, 255, alpha), DirectX::XMMatrixTranslation(enemyPos[i].x, enemyPos[i].y, enemyPos[i].z));
		}
	}

	//projectile
	if (bullet.GetActiveFlag()) {
		DrawBullet->execute(dx12->GetColor(138, 119, 183, alpha), bullet.GetBulletPositionMatrix());
	}

	BackHome->execute(dx12->GetColor(255, 255, 255, alpha), 0);
#pragma endregion

#pragma region SceneSetting
	if (input->GetKeyDown(keycode::H)) {
		titleFlag = true;
	}

	if (titleFlag == true) {
		alpha -= 3.0f;

		if (alpha <= 0.1f) {
			SceneNum = 0;
			titleFlag = false;
			alpha = 255;
		}
	}
	else {
		if (alpha <= 255) {
			alpha += 3.0f;
		}
		if (alpha > 255) {
			alpha = 255;
		}
	}
#pragma endregion

}