#include <vector>

#include <DirectXMath.h>
#include <dinput.h>

#include "Input.h"
#include "Player.h"

Player::Player()
{
	speed = 0.f;
}

void Player::SetSpeed(float speed)
{
	this->speed = speed;
	return;
}

void Player::Operation(std::vector<DirectX::XMFLOAT3> *vertices, Input *input)
{
	if (input->GetKey(DIK_W)) {
		for (auto &verData : *vertices) {
			verData.y += speed;
		}
	}
	if (input->GetKey(DIK_S)) {
		for (auto &verData : *vertices) {
			verData.y -= speed;
		}
	}
	if (input->GetKey(DIK_A)) {
		for (auto &verData : *vertices) {
			verData.x -= speed;
		}
	}
	if (input->GetKey(DIK_D)) {
		for (auto &verData : *vertices) {
			verData.x += speed;
		}
	}
}
