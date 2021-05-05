#pragma once
class Player
{
private:
	float speed;

public:
	Player();
	void SetSpeed(float speed);
	void Operation(std::vector<DirectX::XMFLOAT3> *vertices, Input *input);
};

