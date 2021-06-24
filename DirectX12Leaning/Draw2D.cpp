//API
#include <d3d12.h>
#include <DirectXMath.h>

//shader(HLSL)
#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")

//STL
#include <iostream>
#include <vector>

//Utility
#include "tempUtility.h"

//this
#include "Draw2D.h"

Draw2D::Draw2D(const unsigned int shapeSize, const float radius, const int fillMode, ID3D12Device *dev, ID3D12GraphicsCommandList *cmdList, const int window_width, const int window_height) :
	shapeSize(shapeSize),
	radius(radius),
	dev(dev),
	cmdList(cmdList),
	window_width(window_width),
	window_height(window_height)
{
	SetVertices();
	SetHeapProperty();
	SetResourceDescription();
	CreateVertexBuffer();
	GetVertexMapVirtualMemory();
	SetVertexBufferView();
	SetIndices();
	SetIndexBuffer();
	GetIndexMapVirtualMemory();
	SetIndexBufferView();
	SetShader();
	SetConstantBufferHeapProperty();
	SetConstantBufferResourceDescription();
	SetDescripterHeap();
	CreateConstantBuffer();
	
	//Top layout
	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
		{
			"POSITION",
			0,
			DXGI_FORMAT_R32G32B32_FLOAT,
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0
		},
	};

	SetGraphicsPipeLine(fillMode);
	SetRenderTargetBlendDescription();

	//top layout setting
	gpipeline.InputLayout.pInputElementDescs = inputLayout;
	gpipeline.InputLayout.NumElements = std::extent<decltype(inputLayout), 0>::value;
	gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	gpipeline.NumRenderTargets = 1;
	gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	gpipeline.SampleDesc.Count = 1;

	SetRootParameter();
	SetRootSignature();
	SetSignature();
}

void Draw2D::execute(const DirectX::XMFLOAT4 color)
{
	//Get VirtualMemory
	DirectX::XMFLOAT3 *vertMap = nullptr;
	result = verBuff->Map(0, nullptr, (void **)&vertMap);
	assert(result == S_OK);

	//Update point
	std::copy(vertices.begin(), vertices.end(), vertMap);
	verBuff->Unmap(0, nullptr);

	//Initialize const buffer
	ConstBufferData *constMap = nullptr;
	result = constBuff->Map(0, nullptr, (void **)&constMap);
	constMap->color = color;
	constBuff->Unmap(0, nullptr);
	assert(result == S_OK);

	//pipeline
	cmdList->SetPipelineState(pipelinestate);
	cmdList->SetGraphicsRootSignature(rootsignature);

	//Set Descriptor heap
	ID3D12DescriptorHeap *ppHeaps[] = { basicDescHeap };
	cmdList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

	//Set constant buffer view
	cmdList->SetGraphicsRootDescriptorTable(0, basicDescHeap->GetGPUDescriptorHandleForHeapStart());

	cmdList->IASetVertexBuffers(0, 1, &vbView);
	cmdList->IASetIndexBuffer(&ibView);
	cmdList->DrawIndexedInstanced((int)indices.size(), 1, 0, 0, 0);
}

void Draw2D::SetVertices()
{
	vertices = std::vector<DirectX::XMFLOAT3>(shapeSize + 1);
	for (auto i = 0; i < shapeSize; ++i) {
		vertices[i] = {
			(float)radius * sinf(DirectX::XM_2PI / shapeSize * i) * window_height / window_width,
			(float)radius * cosf(2 * DirectX::XM_PI / shapeSize * i),
			0.f,
		};
	}
	vertices[shapeSize] = { 0, 0, 0 };
	sizeVB = static_cast<UINT>(sizeof(DirectX::XMFLOAT3) * vertices.size());
}

void Draw2D::SetHeapProperty()
{
	//Set vertex buffer
	heapprop = {};
	heapprop.Type = D3D12_HEAP_TYPE_UPLOAD;	//Transfer to GPU
	heapprop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapprop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
}

void Draw2D::SetResourceDescription()
{
	//Resource setting
	resdesc = {};
	resdesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resdesc.Width = sizeVB;
	resdesc.Height = 1;
	resdesc.DepthOrArraySize = 1;
	resdesc.MipLevels = 1;
	resdesc.Format = DXGI_FORMAT_UNKNOWN;
	resdesc.SampleDesc.Count = 1;
	resdesc.Flags = D3D12_RESOURCE_FLAG_NONE;
	resdesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
}

void Draw2D::CreateVertexBuffer()
{
	//Create top buffer
	verBuff = nullptr;
	result = this->dev->CreateCommittedResource(
		&heapprop,
		D3D12_HEAP_FLAG_NONE,
		&resdesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&verBuff)
	);
	assert(result == S_OK);
}

void Draw2D::SetVertexBufferView()
{
	//Create vertex buffer view
	vbView = {};
	vbView.BufferLocation = verBuff->GetGPUVirtualAddress();
	vbView.SizeInBytes = sizeof(DirectX::XMFLOAT3) * vertices.size();
	vbView.StrideInBytes = sizeof(DirectX::XMFLOAT3);
}

void Draw2D::SetIndices()
{
	indices = std::vector<unsigned short>(shapeSize * 3);
	for (int i = 0; i < shapeSize; ++i) {
		indices[i * 3] = i;
		indices[(i * 3) + 1] = i + 1;
		indices[(i * 3) + 2] = shapeSize;
	}
	indices[(shapeSize * 3) - 3] = shapeSize - 1;
	indices[(shapeSize * 3) - 2] = 0;
	indices[(shapeSize * 3) - 1] = shapeSize;
}

void Draw2D::SetIndexBuffer()
{
	//Add index buffer
	indexBuff = nullptr;
	resdesc.Width = sizeof(unsigned short) * indices.size();
	result = this->dev->CreateCommittedResource(
		&heapprop,
		D3D12_HEAP_FLAG_NONE,
		&resdesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&indexBuff)
	);
	assert(result == S_OK);
}

void Draw2D::GetIndexMapVirtualMemory()
{
	//Get virtual memory
	indexMap = nullptr;
	result = indexBuff->Map(0, nullptr, (void **)&indexMap);
	assert(result == S_OK);
	for (int i = 0; i < indices.size(); ++i) {
		indexMap[i] = indices[i];
	}
	indexBuff->Unmap(0, nullptr);
}

void Draw2D::GetVertexMapVirtualMemory()
{
	//Get VirtualMemory
	vertMap = nullptr;
	result = verBuff->Map(0, nullptr, (void **)&vertMap);
	assert(result == S_OK);

	std::copy(vertices.begin(), vertices.end(), vertMap);
	for (auto i = 0; i < vertices.size(); ++i) {
		vertMap[i] = vertices[i];
	}
	verBuff->Unmap(0, nullptr);
}

void Draw2D::SetIndexBufferView()
{
	//Create index buffer view
	ibView = {};
	ibView.BufferLocation = indexBuff->GetGPUVirtualAddress();
	ibView.Format = DXGI_FORMAT_R16_UINT;
	ibView.SizeInBytes = sizeof(unsigned short) * indices.size();
}

void Draw2D::SetShader()
{
	//shader
	vsBlob = nullptr;
	psBlob = nullptr;
	errorBlob = nullptr;

	result = D3DCompileFromFile(
		L"BasicVS.hlsl",
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"main", "vs_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&vsBlob, &errorBlob
	);
	assert(result == S_OK);

	result = D3DCompileFromFile(
		L"BasicPS.hlsl",
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"main", "ps_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&psBlob, &errorBlob
	);
	assert(result == S_OK);
}

void Draw2D::SetConstantBufferHeapProperty()
{
	//Heap
	cbheapprop.Type = D3D12_HEAP_TYPE_UPLOAD;
}

void Draw2D::SetConstantBufferResourceDescription()
{
	//Resources
	cbresdesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	cbresdesc.Width = (sizeof(ConstBufferData) + 0xff) & ~0xff;
	cbresdesc.Height = 1;
	cbresdesc.DepthOrArraySize = 1;
	cbresdesc.MipLevels = 1;
	cbresdesc.SampleDesc.Count = 1;
	cbresdesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	//Create
	constBuff = nullptr;
	result = this->dev->CreateCommittedResource(
		&cbheapprop,
		D3D12_HEAP_FLAG_NONE,
		&cbresdesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuff)
	);
	assert(result == S_OK);
}

void Draw2D::SetDescripterHeap()
{
	//Descripter heap
	basicDescHeap = nullptr;
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	descHeapDesc.NumDescriptors = 1;

	result = this->dev->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&basicDescHeap));
	assert(result == S_OK);
}

void Draw2D::CreateConstantBuffer()
{
	//Create const buffer
	cbvDesc.BufferLocation = constBuff->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = (UINT)constBuff->GetDesc().Width;
	this->dev->CreateConstantBufferView(&cbvDesc, basicDescHeap->GetCPUDescriptorHandleForHeapStart());
}

void Draw2D::SetGraphicsPipeLine(const int fillMode)
{
	//Graphics pipeline
	gpipeline.pRootSignature = nullptr;
	gpipeline.VS.pShaderBytecode = vsBlob->GetBufferPointer();
	gpipeline.VS.BytecodeLength = vsBlob->GetBufferSize();
	gpipeline.PS.pShaderBytecode = psBlob->GetBufferPointer();
	gpipeline.PS.BytecodeLength = psBlob->GetBufferSize();

	//SampleMask
	//RasterizerState
	//Fill mode
	gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	gpipeline.RasterizerState.MultisampleEnable = false;
	gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	gpipeline.RasterizerState.FillMode = (D3D12_FILL_MODE)fillMode;
	gpipeline.RasterizerState.DepthClipEnable = true;
}

void Draw2D::SetRenderTargetBlendDescription()
{
	//Blend
	D3D12_RENDER_TARGET_BLEND_DESC &blenddesc = gpipeline.BlendState.RenderTarget[0];
	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	blenddesc.BlendEnable = true;
	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;
	blenddesc.DestBlendAlpha = D3D12_BLEND_ONE;

	//Alpha blend
	blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
}

void Draw2D::SetRootParameter()
{
	//root parameter
	descTblrange.NumDescriptors = 1;
	descTblrange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	descTblrange.BaseShaderRegister = 0;
	descTblrange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	//setting root parameter
	rootparam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootparam.DescriptorTable.pDescriptorRanges = &descTblrange;
	rootparam.DescriptorTable.NumDescriptorRanges = 1;
	rootparam.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
}

void Draw2D::SetRootSignature()
{
	//root signeture
	rootsignature = nullptr;
	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	rootSignatureDesc.pParameters = &rootparam;
	rootSignatureDesc.NumParameters = 1;

	rootSigBlob = nullptr;
	result = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob, &errorBlob);
	assert(result == S_OK);

	result = this->dev->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(), IID_PPV_ARGS(&rootsignature));
	assert(result == S_OK);

	rootSigBlob->Release();
}

void Draw2D::SetSignature()
{
	//set signature
	gpipeline.pRootSignature = rootsignature;

	pipelinestate = nullptr;
	result = this->dev->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(&pipelinestate));
	assert(result == S_OK);
}
