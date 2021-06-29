//API
#pragma comment(lib, "d3d12.lib")
#include <DirectXMath.h>

//shader(HLSL)
#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")

//STL
#include <iostream>
#include <vector>

//Utility
#include "tempUtility.h"
#include "DrawUtility.h"
#include "Draw3D.h"

Draw3D::Draw3D() {}
Draw3D::Draw3D(const wchar_t *fileName, DrawShapeData shapeData, const float radius, const int fillMode, ID3D12Device *dev, ID3D12GraphicsCommandList *cmdList, const int window_width, const int window_height) :
	radius(radius),
	dev(dev),
	cmdList(cmdList),
	dsvHeap(dsvHeap),
	window_width(window_width),
	window_height(window_height)
{
	SetShape(shapeData);
	SetHeapProperty();
	SetResourceDescription();
	CreateVertexBuffer();
	GetVertexMapVirtualMemory();
	SetVertexBufferView();
	SetIndexBuffer();
	GetIndexMapVirtualMemory();
	SetIndexBufferView();
	SetShader();
	SetConstantBufferHeapProperty();
	SetConstantBufferResourceDescription();
	SetDescripterHeap();
	CreateConstantBuffer();
	CreateTextureData(fileName);

	//Top layout
	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
		{ "POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL",		0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT,	0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};

	CreateWorldMatrix();
	CreateViewMatrix();
	MappingConstBuffer();
	SetDepthCulling();
	SetNormalVector();

	SetGraphicsPipeLine(fillMode);

	//top layout setting
	gpipeline.InputLayout.pInputElementDescs = inputLayout;
	gpipeline.InputLayout.NumElements = std::extent<decltype(inputLayout), 0>::value;
	gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	gpipeline.NumRenderTargets = 1;
	gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	gpipeline.SampleDesc.Count = 1;

	SetRenderTargetBlendDescription();
	SetRootParameter();
	SetRootSignature();
	SetSignature();
}

void Draw3D::execute(const DirectX::XMFLOAT4 color, const DirectX::XMMATRIX Translation)
{
	//Get VirtualMemory
	vertMap = nullptr;
	result = verBuff->Map(0, nullptr, (void **)&vertMap);
	assert(result == S_OK);

	//Update point
	std::copy(vertices.begin(), vertices.end(), vertMap);
	verBuff->Unmap(0, nullptr);

	matTrans = Translation;

	matWorld = DirectX::XMMatrixIdentity();
	matWorld *= matScale;
	matWorld *= matRot;
	matWorld *= matTrans;

	matView = DirectX::XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&target), XMLoadFloat3(&up));

	constMap = nullptr;
	result = constBuff->Map(0, nullptr, (void **)&constMap);
	assert(result == S_OK);

	constMap->color = color;
	constMap->mat = matWorld * matView * matProjection;
	constBuff->Unmap(0, nullptr);

	//pipeline
	cmdList->SetPipelineState(pipelinestate);
	cmdList->SetGraphicsRootSignature(rootsignature);

	//Set Descriptor heap
	ID3D12DescriptorHeap *ppHeaps[] = { basicDescHeap };
	cmdList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

	//Set constant buffer view
	D3D12_GPU_DESCRIPTOR_HANDLE gpuDescHandle = basicDescHeap->GetGPUDescriptorHandleForHeapStart();
	cmdList->SetGraphicsRootDescriptorTable(0, gpuDescHandle);
	gpuDescHandle.ptr += this->dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	cmdList->SetGraphicsRootDescriptorTable(1, gpuDescHandle);

	//Set constant buffer view
	/*cmdList->SetGraphicsRootDescriptorTable(0, basicDescHeap->GetGPUDescriptorHandleForHeapStart());*/

	/*cmdList->IASetVertexBuffers(0, 1, &vbView);
	cmdList->IASetIndexBuffer(&ibView);
	cmdList->DrawIndexedInstanced((int)indices.size(), 1, 0, 0, 0);*/

	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	cmdList->IASetVertexBuffers(0, 1, &vbView);

	//Index buffer set command
	cmdList->IASetIndexBuffer(&ibView);
	cmdList->DrawIndexedInstanced((int)indices.size(), 1, 0, 0, 0);
}

void Draw3D::SetRotation(DirectX::XMMATRIX Rotation)
{
	matRot *= Rotation;
}

void Draw3D::SetShape(DrawShapeData shapeData)
{
	switch (shapeData)
	{
		case DrawShapeData::TriangularPyramid:
		{
			DrawTriangularPyramidObjData3D spriteData;

			//SetVertex
			vertices = std::vector<Vertex3D>(spriteData.VertexSize);

			for (auto i = 0; i < spriteData.VertexSize; i++) {
				vertices[i] = spriteData.vertices[i];

				vertices[i].pos.x *= radius;
				vertices[i].pos.y *= radius;
			}
			sizeVB = static_cast<UINT>(sizeof(Vertex3D) * vertices.size());

			//SetIndex
			indices = std::vector<unsigned short>(spriteData.IndexSize);
			for (auto i = 0; i < spriteData.IndexSize; i++) {
				indices[i] = spriteData.indices[i];
			}
			break;
		}

		case DrawShapeData::Box:
		{
			DrawBoxObjData3D spriteData;

			//SetVertex
			vertices = std::vector<Vertex3D>(spriteData.VertexSize);

			for (auto i = 0; i < spriteData.VertexSize; i++) {
				vertices[i] = spriteData.vertices[i];

				vertices[i].pos.x *= radius;
				vertices[i].pos.y *= radius;
			}
			sizeVB = static_cast<UINT>(sizeof(Vertex3D) * vertices.size());

			//SetIndex
			indices = std::vector<unsigned short>(spriteData.IndexSize);
			for (auto i = 0; i < spriteData.IndexSize; i++) {
				indices[i] = spriteData.indices[i];
			}
			break;
		}
	}
}

void Draw3D::SetHeapProperty()
{
	//Set vertex buffer
	heapprop = {};
	heapprop.Type = D3D12_HEAP_TYPE_UPLOAD;	//Transfer to GPU
	heapprop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapprop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
}

void Draw3D::SetResourceDescription()
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

void Draw3D::CreateVertexBuffer()
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

void Draw3D::SetVertexBufferView()
{
	//Create vertex buffer view
	vbView = {};
	vbView.BufferLocation = verBuff->GetGPUVirtualAddress();
	vbView.SizeInBytes = sizeof(Vertex3D) * vertices.size();
	vbView.StrideInBytes = sizeof(Vertex3D);
}

void Draw3D::SetIndexBuffer()
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

void Draw3D::GetIndexMapVirtualMemory()
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

void Draw3D::GetVertexMapVirtualMemory()
{
	//Get VirtualMemory
	vertMap = nullptr;
	result = verBuff->Map(0, nullptr, (void **)&vertMap);
	assert(result == S_OK);

	std::copy(vertices.begin(), vertices.end(), vertMap);
	verBuff->Unmap(0, nullptr);
}

void Draw3D::SetIndexBufferView()
{
	//Create index buffer view
	ibView = {};
	ibView.BufferLocation = indexBuff->GetGPUVirtualAddress();
	ibView.Format = DXGI_FORMAT_R16_UINT;
	ibView.SizeInBytes = sizeof(unsigned short) * indices.size();
}

void Draw3D::SetShader()
{
	//shader
	vsBlob = nullptr;
	psBlob = nullptr;
	errorBlob = nullptr;

	result = D3DCompileFromFile(
		L"BasicVS3D.hlsl",
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"main", "vs_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&vsBlob, &errorBlob
	);
	assert(result == S_OK);

	result = D3DCompileFromFile(
		L"BasicPS3D.hlsl",
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"main", "ps_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&psBlob, &errorBlob
	);
	assert(result == S_OK);
}

void Draw3D::SetConstantBufferHeapProperty()
{
	//Heap
	cbheapprop.Type = D3D12_HEAP_TYPE_UPLOAD;
}

void Draw3D::SetConstantBufferResourceDescription()
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

void Draw3D::SetDescripterHeap()
{
	//Descripter heap
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	descHeapDesc.NumDescriptors = 2;

	result = this->dev->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&basicDescHeap));
	assert(result == S_OK);

	basicHeapHandle = basicDescHeap->GetCPUDescriptorHandleForHeapStart();
}

void Draw3D::CreateConstantBuffer()
{
	//Create const buffer
	cbvDesc.BufferLocation = constBuff->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = (UINT)constBuff->GetDesc().Width;
	this->dev->CreateConstantBufferView(&cbvDesc, basicHeapHandle);
}

void Draw3D::CreateTextureData(const wchar_t *fileName)
{
	if (fileName == nullptr) {
		result = LoadFromWICFile(
			L"Resources/Default.png",
			DirectX::WIC_FLAGS_NONE,
			&metadata, scratchImg
		);
		assert(result == S_OK);
	}
	else {
		result = LoadFromWICFile(
			fileName,
			DirectX::WIC_FLAGS_NONE,
			&metadata, scratchImg
		);
		assert(result == S_OK);
	}

	const DirectX::Image *img = scratchImg.GetImage(0, 0, 0);

	texHeapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
	texHeapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	texHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;

	texresDesc.Dimension = static_cast<D3D12_RESOURCE_DIMENSION>(metadata.dimension);
	texresDesc.Format = metadata.format;
	texresDesc.Width = metadata.width;
	texresDesc.Height = (UINT)metadata.height;
	texresDesc.DepthOrArraySize = (UINT16)metadata.arraySize;
	texresDesc.MipLevels = (UINT16)metadata.mipLevels;
	texresDesc.SampleDesc.Count = 1;

	texbuff = nullptr;
	result = this->dev->CreateCommittedResource(
		&texHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&texresDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&texbuff)
	);

	result = texbuff->WriteToSubresource(
		0,
		nullptr,
		img->pixels,
		(UINT)img->rowPitch,
		(UINT)img->slicePitch
	);

	basicHeapHandle.ptr += this->dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	srvDesc.Format = metadata.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;

	this->dev->CreateShaderResourceView(texbuff, &srvDesc, basicHeapHandle);
}

void Draw3D::CreateWorldMatrix()
{
	matWorld = DirectX::XMMatrixIdentity();

	//scalling
	matScale = DirectX::XMMatrixScaling(1.0f, 1.0f, 1.0f);
	matWorld *= matScale;

	//rotation
	matRot = DirectX::XMMatrixIdentity();
	matRot *= DirectX::XMMatrixRotationZ(DirectX::XMConvertToRadians(0.0f));
	matWorld *= matRot;

	//Translation
	matTrans = DirectX::XMMatrixTranslation(0.0f, 0.0f, 0.0f);
	matWorld *= matTrans;
}

void Draw3D::CreateViewMatrix()
{
	eye = { 0, 0, -50 };
	target = { 0, 0, 0 };
	up = { 0, 1, 0 };
	angle = 0;
	matView = DirectX::XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&target), XMLoadFloat3(&up));
}

void Draw3D::MappingConstBuffer()
{
	constMap = nullptr;
	result = constBuff->Map(0, nullptr, (void **)&constMap);

	matProjection = DirectX::XMMatrixPerspectiveFovLH(
		DirectX::XMConvertToRadians(60.0f),
		(float)window_width / window_height,
		0.1f, 1000.0f
	);

	constMap->mat = matWorld * matView * matProjection;

	constBuff->Unmap(0, nullptr);
	assert(result == S_OK);
}

void Draw3D::SetDepthCulling()
{
	depthResDesc = {};
	depthResDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depthResDesc.Width = window_width;
	depthResDesc.Height = window_height;
	depthResDesc.DepthOrArraySize = 1;
	depthResDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthResDesc.SampleDesc.Count = 1;
	depthResDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	depthHeapProp = {};
	depthHeapProp.Type = D3D12_HEAP_TYPE_DEFAULT;

	depthClearValue = {};
	depthClearValue.DepthStencil.Depth = 1.0f;
	depthClearValue.Format = DXGI_FORMAT_D32_FLOAT;

	depthBuffer = nullptr;
	result = this->dev->CreateCommittedResource(
		&depthHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&depthResDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&depthClearValue,
		IID_PPV_ARGS(&depthBuffer)
	);

	dsvHeapDesc = {};
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;	//DepthStencilView

	dsvHeap = nullptr;
	result = this->dev->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&dsvHeap));

	dsvDesc = {};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	this->dev->CreateDepthStencilView(
		depthBuffer,
		&dsvDesc,
		dsvHeap->GetCPUDescriptorHandleForHeapStart()
	);
}

void Draw3D::SetNormalVector()
{
	for (auto i = 0; i < indices.size() / 3; ++i) {
		unsigned short index[3];
		index[0] = indices[i * 3 + 0];
		index[1] = indices[i * 3 + 1];
		index[2] = indices[i * 3 + 2];

		DirectX::XMVECTOR p0 = DirectX::XMLoadFloat3(&vertices[index[0]].pos);
		DirectX::XMVECTOR p1 = DirectX::XMLoadFloat3(&vertices[index[1]].pos);
		DirectX::XMVECTOR p2 = DirectX::XMLoadFloat3(&vertices[index[2]].pos);
		
		DirectX::XMVECTOR v1 = DirectX::XMVectorSubtract(p1, p0);
		DirectX::XMVECTOR v2 = DirectX::XMVectorSubtract(p2, p0);
		
		DirectX::XMVECTOR normal = DirectX::XMVector3Cross(v1, v2);
		normal = DirectX::XMVector3Normalize(normal);

		XMStoreFloat3(&vertices[index[0]].normal, normal);
		XMStoreFloat3(&vertices[index[1]].normal, normal);
		XMStoreFloat3(&vertices[index[2]].normal, normal);
	}
}

void Draw3D::SetGraphicsPipeLine(const int fillMode)
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
	gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
	gpipeline.RasterizerState.FillMode = (D3D12_FILL_MODE)fillMode;
	gpipeline.RasterizerState.DepthClipEnable = true;
}

void Draw3D::SetRenderTargetBlendDescription()
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

void Draw3D::SetRootParameter()
{
	////root parameter
	//descTblrange.NumDescriptors = 1;
	//descTblrange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	//descTblrange.BaseShaderRegister = 0;
	//descTblrange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	////setting root parameter
	//rootparam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	//rootparam.DescriptorTable.pDescriptorRanges = &descTblrange;
	//rootparam.DescriptorTable.NumDescriptorRanges = 1;
	//rootparam.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	//root parameter
	D3D12_DESCRIPTOR_RANGE descRangeCBV{};
	descRangeCBV.NumDescriptors = 1;
	descRangeCBV.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	descRangeCBV.BaseShaderRegister = 0;
	descRangeCBV.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	D3D12_DESCRIPTOR_RANGE descRangeSRV{};
	descRangeSRV.NumDescriptors = 1;
	descRangeSRV.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descRangeSRV.BaseShaderRegister = 0;
	descRangeSRV.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	//setting root parameter
	D3D12_ROOT_PARAMETER rootparam[2] = {};
	rootparam[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootparam[0].DescriptorTable.pDescriptorRanges = &descRangeCBV;
	rootparam[0].DescriptorTable.NumDescriptorRanges = 1;
	rootparam[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	rootparam[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootparam[1].DescriptorTable.pDescriptorRanges = &descRangeSRV;
	rootparam[1].DescriptorTable.NumDescriptorRanges = 1;
	rootparam[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	D3D12_STATIC_SAMPLER_DESC sampleDesc{};
	sampleDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	sampleDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	sampleDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	sampleDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
	sampleDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
	sampleDesc.MaxLOD = D3D12_FLOAT32_MAX;
	sampleDesc.MinLOD = 0.0f;
	sampleDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	sampleDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	//root signeture
	rootsignature = nullptr;
	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	rootSignatureDesc.pParameters = rootparam;
	rootSignatureDesc.NumParameters = _countof(rootparam);
	rootSignatureDesc.pStaticSamplers = &sampleDesc;
	rootSignatureDesc.NumStaticSamplers = 1;

	rootSigBlob = nullptr;
	result = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob, &errorBlob);
	assert(result == S_OK);

	result = this->dev->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(), IID_PPV_ARGS(&rootsignature));
	assert(result == S_OK);

	rootSigBlob->Release();
}

void Draw3D::SetRootSignature()
{
	////root signeture
	//rootsignature = nullptr;
	//rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	//rootSignatureDesc.pParameters = rootparam;
	//rootSignatureDesc.NumParameters = 1;

	//rootSigBlob = nullptr;
	//result = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob, &errorBlob);
	//assert(result == S_OK);

	//result = this->dev->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(), IID_PPV_ARGS(&rootsignature));
	//assert(result == S_OK);

	//rootSigBlob->Release();
}

void Draw3D::SetSignature()
{
	//set signature
	gpipeline.pRootSignature = rootsignature;

	//SetZBuffer
	gpipeline.DepthStencilState.DepthEnable = true;
	gpipeline.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	gpipeline.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	gpipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;

	pipelinestate = nullptr;
	result = this->dev->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(&pipelinestate));
	assert(result == S_OK);
}