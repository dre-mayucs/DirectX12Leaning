#pragma once
#include <DirectXMath.h>

struct ConstBufferData
{
	DirectX::XMFLOAT4 color;
};

struct ConstBufferData3D
{
	DirectX::XMFLOAT4 color;
	DirectX::XMMATRIX mat;
};

struct Vertex3D 
{
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT3 normal;
	DirectX::XMFLOAT2 uv;
};

struct Vertex
{
	DirectX::XMFLOAT3 pos;
};

struct Position3D
{
	float x = 0;
	float y = 0;
	float z = 0;
};

class tempUtility
{
public:
	tempUtility();
	~tempUtility();
};