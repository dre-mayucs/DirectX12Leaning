//DirectX
#include <DirectXMath.h>
#include <dinput.h>

//Utility
#include "Input.h"
#include "tempUtility.h"

//this
#include "Bullet.h"

Bullet::Bullet(float speed, float radius, Input *input) : speed(speed), radius(radius), input(input)
{
	position = { 0.0f, 0.0f, 0.0f };
}

void Bullet::Update(Position3D playerPos)
{
	if (input->GetKeyDown(DIK_SPACE) && flag == false) {
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
