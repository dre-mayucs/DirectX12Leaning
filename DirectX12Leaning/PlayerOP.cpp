//DirectX12
#include <DirectXMath.h>

//Utility
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
	if (position.y <  25 && input->GetKey(keycode::W) || input->GetKey(keycode::UpArrow))		{ position.y += 1.f; }
	if (position.y > -25 && input->GetKey(keycode::S) || input->GetKey(keycode::DownAllow))		{ position.y -= 1.f; }
	if (position.x > -50 && input->GetKey(keycode::A) || input->GetKey(keycode::LeftArrow))		{ position.x -= 1.f; }
	if (position.x <  50 && input->GetKey(keycode::D) || input->GetKey(keycode::RightArrow))	{ position.x += 1.f; }

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

DirectX::XMFLOAT2 PlayerOP::GetPosition() const
{
	return DirectX::XMFLOAT2(position.x, position.y);
}

Position3D PlayerOP::Get3DPoint() const
{
	return position;
}
