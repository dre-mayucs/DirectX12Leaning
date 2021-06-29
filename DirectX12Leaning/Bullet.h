#pragma once
class Bullet
{
public:
	Bullet();
	Bullet(float speed, float radius, Input *input, DirectX12 *dx12, const int window_width, const int window_height);
	~Bullet();
	void Update(Position3D playerPos);
	bool GetCollision(Position3D targetPos, float radius);
	bool GetActiveFlag() const;
	void SetActiveFlag(bool setFlag);
	DirectX::XMMATRIX GetBulletPositionMatrix() const;

private:
	bool flag;
	float speed;
	Input *input;

	DirectX12 *dx12;

	float radius;
	Position3D position;
	DirectX::XMMATRIX matrixTranslation;

	Draw3D *bullet;
};