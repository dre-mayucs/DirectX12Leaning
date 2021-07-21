#pragma once
#include <DirectXTex.h>
class Draw2DGraph
{
private:
	int window_width;
	int window_height;

public:
	Draw2DGraph();
	Draw2DGraph(const wchar_t *fileName, const int fillMode, ID3D12Device *dev, ID3D12GraphicsCommandList *cmdList, const int window_width, const int window_height);
	void Update(float x, float y, float rotate);
	void execute(const DirectX::XMFLOAT4 color);
	void execute(const DirectX::XMFLOAT4 color, const float adjustXPos = 0, const float adjustYPos = 0);

private:
	void SetVertices();
	void SetHeapProperty();
	void SetResourceDescription();
	void CreateVertexBuffer();
	void GetVertexMapVirtualMemory();
	void SetVertexBufferView();
	void SetIndices();
	void SetIndexBuffer();
	void GetIndexMapVirtualMemory();
	void SetIndexBufferView();
	void SetShader();
	void SetConstantBufferHeapProperty();
	void SetConstantBufferResourceDescription();
	void SetDescripterHeap();
	void CreateConstantBuffer();
	void CreateTextureData(const wchar_t *fileName);
	void SetGraphicsPipeLine(const int fillMode);
	void SetRenderTargetBlendDescription();
	void SetRootParameter();
	void SetRootSignature();
	void SetSignature();

private:
	std::vector<unsigned short>indices;
	std::vector<Graph2DVertex>vertices;

	UINT sizeVB;
	HRESULT result;

	ID3D12Device *dev;
	ID3D12GraphicsCommandList *cmdList;

private:
	DirectX::XMMATRIX matProjection;
	DirectX::XMMATRIX matrix;

	D3D12_HEAP_PROPERTIES heapprop;
	D3D12_RESOURCE_DESC resdesc;
	ID3D12Resource *verBuff;
	Graph2DVertex *vertMap;
	D3D12_VERTEX_BUFFER_VIEW vbView;
	ID3D12Resource *indexBuff;
	unsigned short *indexMap;
	D3D12_INDEX_BUFFER_VIEW ibView;

	ID3DBlob *vsBlob;
	ID3DBlob *psBlob;
	ID3DBlob *errorBlob;

	DirectX::TexMetadata metadata;
	DirectX::ScratchImage scratchImg;
	D3D12_HEAP_PROPERTIES texHeapProp;
	D3D12_RESOURCE_DESC texresDesc;
	ID3D12Resource *texbuff;
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc;

	D3D12_HEAP_PROPERTIES cbheapprop;
	D3D12_RESOURCE_DESC cbresdesc;
	ID3D12Resource *constBuff;
	ID3D12DescriptorHeap *basicDescHeap;
	D3D12_CPU_DESCRIPTOR_HANDLE basicHeapHandle;
	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc;
	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline;
	D3D12_DESCRIPTOR_RANGE descRangeCBV;
	D3D12_ROOT_PARAMETER rootparam;
	ID3D12RootSignature *rootsignature;
	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc;
	ID3DBlob *rootSigBlob;
	ID3D12PipelineState *pipelinestate;
};

