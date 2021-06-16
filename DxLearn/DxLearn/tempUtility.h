#pragma once

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

class tempUtility
{
public:
	tempUtility();
	~tempUtility();
};