#pragma once
class Bullet
{
public:
	Bullet();
	Bullet(float speed, float radius, Input *input);
	void Update(Position3D playerPos);
	bool GetCollision(Position3D targetPos, float radius);
	bool GetActiveFlag() const;
	void SetActiveFlag(bool setFlag);
	DirectX::XMMATRIX GetBulletPositionMatrix() const;

private:
	bool flag;
	float speed;
	Input *input;

	float radius;
	Position3D position;
	DirectX::XMMATRIX matrixTranslation;
};