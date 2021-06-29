#pragma once
class PlayerOP
{
public:
	PlayerOP();
	PlayerOP(float x, float y, float z, float r, Input *input);
	void Update();
	const DirectX::XMMATRIX GetPlayerPositionMatrix() const;
	DirectX::XMFLOAT2 GetPosition() const;
	Position3D Get3DPoint() const;

private:
	bool GetCollition(Position3D targetPos, float targetRadius) const;

private:
	Input *input;

	float radius;
	Position3D position;
	DirectX::XMMATRIX matrixTranslation;
};