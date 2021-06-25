#pragma once
#include <DirectXTex.h>

enum class DrawShapeData {
	TriangularPyramid,
	Box
};

class Draw3D
{
private:
	int window_width;
	int window_height;

public:
	Draw3D(DrawShapeData shapeData, const float radius, const int fillMode, ID3D12Device *dev, ID3D12GraphicsCommandList *cmdList, const int window_width, const int window_height);
	void execute(const DirectX::XMFLOAT4 color, const DirectX::XMMATRIX Translation);
	void SetPos(const DirectX::XMFLOAT3 pos);

private:
	void SetShape(DrawShapeData shapeData);
	void ChangeShapeSize();
	void SetVertices();
	void SetHeapProperty();
	void SetResourceDescription();
	void CreateVertexBuffer();
	void GetVertexMapVirtualMemory();
	void SetVertexBufferView();
	void SetIndexBuffer();
	void GetIndexMapVirtualMemory();
	void SetIndexBufferView();
	void SetShader();
	void SetConstantBufferHeapProperty();
	void SetConstantBufferResourceDescription();
	void SetDescripterHeap();
	void CreateConstantBuffer();
	void CreateTextureData();

	void CreateWorldMatrix();
	void CreateViewMatrix();
	void MappingConstBuffer();
	void SetDepthCulling();
	void SetNormalVector();

	void SetGraphicsPipeLine(const int fillMode);
	void SetRenderTargetBlendDescription();
	void SetRootParameter();
	void SetRootSignature();
	void SetSignature();

private:
	float radius;

	std::vector<unsigned short>indices;
	std::vector<Vertex3D>vertices;

	UINT sizeVB;
	HRESULT result;

	ID3D12Device *dev;
	ID3D12GraphicsCommandList *cmdList;

private:
	D3D12_HEAP_PROPERTIES heapprop;
	D3D12_RESOURCE_DESC resdesc;
	ID3D12Resource *verBuff;
	Vertex3D *vertMap;
	D3D12_VERTEX_BUFFER_VIEW vbView;
	ID3D12Resource *indexBuff;
	unsigned short *indexMap;
	D3D12_INDEX_BUFFER_VIEW ibView;

	ID3DBlob *vsBlob;
	ID3DBlob *psBlob;
	ID3DBlob *errorBlob;

	D3D12_HEAP_PROPERTIES cbheapprop;
	D3D12_RESOURCE_DESC cbresdesc;
	ID3D12Resource *constBuff;
	ID3D12DescriptorHeap *basicDescHeap;
	D3D12_CPU_DESCRIPTOR_HANDLE basicHeapHandle;
	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc;
	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;

	DirectX::TexMetadata metadata;
	DirectX::ScratchImage scratchImg;
	D3D12_HEAP_PROPERTIES texHeapProp;
	D3D12_RESOURCE_DESC texresDesc;
	ID3D12Resource *texbuff;
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc;

	DirectX::XMMATRIX matWorld;
	DirectX::XMMATRIX matScale;
	DirectX::XMMATRIX matRot;
	DirectX::XMMATRIX matTrans;
	DirectX::XMMATRIX matView;
	DirectX::XMMATRIX matProjection;
	DirectX::XMFLOAT3 eye;
	DirectX::XMFLOAT3 target;
	DirectX::XMFLOAT3 up;
	float angle;

	ConstBufferData3D *constMap;
	D3D12_RESOURCE_DESC depthResDesc{};
	D3D12_HEAP_PROPERTIES depthHeapProp{};
	D3D12_CLEAR_VALUE depthClearValue{};
	ID3D12Resource *depthBuffer;
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc{};
	ID3D12DescriptorHeap *dsvHeap;
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};

	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline;
	D3D12_DESCRIPTOR_RANGE descTblrange;
	D3D12_ROOT_PARAMETER rootparam;
	ID3D12RootSignature *rootsignature;
	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc;
	ID3DBlob *rootSigBlob;
	ID3D12PipelineState *pipelinestate;
};