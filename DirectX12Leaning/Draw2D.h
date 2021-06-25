#pragma once

class Draw2D
{
private:
	int window_width;
	int window_height;

public:
	Draw2D(const unsigned int shapeSize, const float radius, const int fillMode, ID3D12Device *dev, ID3D12GraphicsCommandList *cmdList, const int window_width, const int window_height);
	void execute(const DirectX::XMFLOAT4 color);
	void SetPos(const DirectX::XMFLOAT3 pos);

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
	void SetGraphicsPipeLine(const int fillMode);
	void SetRenderTargetBlendDescription();
	void SetRootParameter();
	void SetRootSignature();
	void SetSignature();

private:
	float radius;
	unsigned int shapeSize;

	std::vector<unsigned short>indices;
	std::vector<DirectX::XMFLOAT3>vertices;

	UINT sizeVB;
	HRESULT result;

	ID3D12Device *dev;
	ID3D12GraphicsCommandList *cmdList;

private:
	D3D12_HEAP_PROPERTIES heapprop;
	D3D12_RESOURCE_DESC resdesc;
	ID3D12Resource *verBuff;
	DirectX::XMFLOAT3 *vertMap;
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
	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc;
	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline;
	D3D12_DESCRIPTOR_RANGE descTblrange;
	D3D12_ROOT_PARAMETER rootparam;
	ID3D12RootSignature *rootsignature;
	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc;
	ID3DBlob *rootSigBlob;
	ID3D12PipelineState *pipelinestate;
};

