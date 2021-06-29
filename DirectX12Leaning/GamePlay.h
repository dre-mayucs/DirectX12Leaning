#pragma once
class GamePlay
{
public:
	GamePlay(Win32 *win32, DirectX12 *dx12, Input *input, const int window_width, const int window_height);
	~GamePlay();
	void Update();

private:
	Win32 *win32;
	DirectX12 *dx12;
	ID3D12Device *dev;
	ID3D12GraphicsCommandList *cmdList;
	Input *input;
	const int window_width;
	const int window_height;

private:
	PlayerOP player;
	Draw3D *drawPlayer;

	Bullet bullet;
	Draw3D *DrawBullet;

	std::vector<Draw3D *> enemyObject;
	std::vector<Draw2DGraph*> Background;

	Position3D enemyPos[2] = {
		{20, rand() % 50 - 25.0f, 0},
		{20, rand() % 50 - 25.0f, 0}
	};

	bool enemyFlag[2]		= { true,  true };
	bool enemyTurnFlag[2]	= { false, false };
	int enemyWaitTime[2]	= { 0,		0 };
	float enemySpeed[2]		= { 0.5f,	1.0f };
	float xAdjust[2]		= { 0, 2 };
};

