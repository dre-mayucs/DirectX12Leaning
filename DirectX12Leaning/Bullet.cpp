//DirectX
#include <DirectXMath.h>
#include <d3d12.h>
#include "DirectX12.h"

//Utility
#include "Input.h"
#include "tempUtility.h"
#include <iostream>
#include "Draw3D.h"

//this
#include "Bullet.h"

Bullet::Bullet() {}

Bullet::Bullet(float speed, float radius, Input *input, DirectX12 *dx12, const int window_width, const int window_height) :
	speed(speed), 
	radius(radius), 
	input(input),
	dx12(dx12)
{
	flag = false;
	position = { 0.0f, 0.0f, 0.0f };
	bullet = new Draw3D(L"Resources/senju.png", DrawShapeData::TriangularPyramid, 2, D3D12_FILL_MODE_SOLID, dx12->GetDevice(), dx12->GetCommandList(), window_width, window_height);
	bullet->SetRotation(DirectX::XMMatrixRotationZ(DirectX::XMConvertToRadians(-90.0f)));
}

Bullet::~Bullet()
{
	bullet = nullptr;
}

void Bullet::Update(Position3D playerPos)
{
	if (input->GetKeyDown(keycode::Space) && flag == false) {
		position = playerPos;
		flag = true;
	}

	if (flag == true) {
		position.x += speed;

		if (position.x >= 60) {
			flag = false;
		}
	}

	matrixTranslation = DirectX::XMMatrixTranslation(position.x, position.y, position.z);

	if (flag == true) {
		bullet->execute(dx12->GetColor(138, 119, 183, 255), GetBulletPositionMatrix());
	}
}

bool Bullet::GetCollision(Position3D targetPos, float targetRadius)
{
	float a = position.x - targetPos.x;
	float b = position.y - targetPos.y;
	float c = sqrtf(a * a + b * b);

	if (c <= radius + targetRadius) { return true; }

	return false;
}

bool Bullet::GetActiveFlag() const
{
	return flag;
}

void Bullet::SetActiveFlag(bool setFlag)
{
	flag = setFlag;
}

DirectX::XMMATRIX Bullet::GetBulletPositionMatrix() const
{
	return matrixTranslation;
}
