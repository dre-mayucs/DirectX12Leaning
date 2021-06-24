#pragma once
class PlayerOP
{
public:
	PlayerOP(float x, float y, float z, float r, Input *input);
	void Update();
	bool GetCollition(Position3D targetPos, float targetRadius) const;
	const DirectX::XMMATRIX GetPlayerPositionMatrix() const;

private:
	Input *input;

	float radius;
	Position3D position;
	DirectX::XMMATRIX matrixTranslation;
};