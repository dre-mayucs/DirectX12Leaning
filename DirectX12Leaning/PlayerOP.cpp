//DirectX12
#include <DirectXMath.h>
#include <dinput.h>

//Utility
#include <cmath>
#include "Input.h"
#include "TempUtility.h"

//this
#include "PlayerOP.h"

PlayerOP::PlayerOP(float x, float y, float z, float r, Input *input) : input(input)
{
	position.x = x;
	position.y = y;
	position.z = z;
	radius = r;

	matrixTranslation = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
	return;
}

void PlayerOP::Update()
{
	if (position.y <  25 && input->GetKey(DIK_W) || input->GetKey(DIK_UP))		{ position.y += 1.f; }
	if (position.y > -25 && input->GetKey(DIK_S) || input->GetKey(DIK_DOWN))	{ position.y -= 1.f; }
	if (position.x > -50 && input->GetKey(DIK_A) || input->GetKey(DIK_LEFT))	{ position.x -= 1.f; }
	if (position.x <  50 && input->GetKey(DIK_D) || input->GetKey(DIK_RIGHT))	{ position.x += 1.f; }

	matrixTranslation = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
}

bool PlayerOP::GetCollition(Position3D targetPos, float targetRadius) const
{
	float a = position.x - targetPos.x;
	float b = position.y - targetPos.y;
	float c = sqrtf(a * a + b * b);

	if (c <= radius + targetRadius) { return true; }
	
	return false;
}

const DirectX::XMMATRIX PlayerOP::GetPlayerPositionMatrix() const
{
	return matrixTranslation;
}
