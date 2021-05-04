//API(Win)
#include <Windows.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <DirectXMath.h>
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

//shader(HLSL)
#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")

//STL
#include <vector>
#include <string>
#include <assert.h>

//Utility
#include <dinput.h>
#include "Input.h"
#include "Win32.h"
#include "tempUtility.h"
#include "D3D12SetGPU.h"

const int window_width = 1000;
const int window_height = 1000;

//namespace
using namespace DirectX;

int WINAPI WinMain(_In_ HINSTANCE hinstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd) 
{
	//Win32initialize
	Win32 win32(L"Test", 1000, 1000);

#pragma region DirectX12 Initialize components
	HRESULT result;
	IDXGISwapChain4 *swapchain = nullptr;

	ID3D12CommandAllocator *cmdAllocator = nullptr;
	ID3D12GraphicsCommandList *cmdList = nullptr;
	ID3D12CommandQueue *cmdQueue = nullptr;
	ID3D12DescriptorHeap *rtvHeaps = nullptr;

	D3D12SetGPU GPU;
	GPU.D3D12ListUpGPU();
	GPU.D3D12SelectGPU();
	GPU.D3D12FeatureLv();

	//Create allocater
	result = GPU.dev->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(&cmdAllocator)
	);
	assert(result == S_OK);

	//Create commandlist
	result = GPU.dev->CreateCommandList(
		0,
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		cmdAllocator,
		nullptr,
		IID_PPV_ARGS(&cmdList)
	);
	assert(result == S_OK);

	//Command queue
	D3D12_COMMAND_QUEUE_DESC cmdQueueDesc{};
	result = GPU.dev->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(&cmdQueue));
	assert(result == S_OK);

	//Swap chain
	DXGI_SWAP_CHAIN_DESC1 swapchainDesc{};
	swapchainDesc.Width = window_width;
	swapchainDesc.Height = window_height;
	swapchainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapchainDesc.SampleDesc.Count = 1;
	swapchainDesc.BufferUsage = DXGI_USAGE_BACK_BUFFER;
	swapchainDesc.BufferCount = 2;
	swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapchainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	GPU.dxgiFactory->CreateSwapChainForHwnd(
		cmdQueue,
		win32.hwnd,
		&swapchainDesc,
		nullptr,
		nullptr,
		(IDXGISwapChain1 **)&swapchain
	);

	//Descriptor heap
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc{};
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	//Front and Back screen layer
	heapDesc.NumDescriptors = 2;
	GPU.dev->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&rtvHeaps));

	//target view
	std::vector<ID3D12Resource *> backBuffers(2);
	for (auto i = 0; i < 2; ++i) {
		result = swapchain->GetBuffer(i, IID_PPV_ARGS(&backBuffers[i]));
		assert(result == S_OK);

		D3D12_CPU_DESCRIPTOR_HANDLE handle = rtvHeaps->GetCPUDescriptorHandleForHeapStart();
		handle.ptr += i * GPU.dev->GetDescriptorHandleIncrementSize(heapDesc.Type);
		GPU.dev->CreateRenderTargetView(
			backBuffers[i],
			nullptr,
			handle
		);
	}

	//Generate Fence
	ID3D12Fence *fence = nullptr;
	UINT64 fenceVal = 0;
	result = GPU.dev->CreateFence(fenceVal, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
	assert(result == S_OK);
#pragma endregion

#pragma region Initialize drawing command

#pragma region Vertex buffer
	const unsigned int shape_size = 64;
	float radius = 0.2f;
	XMFLOAT3 vertices[shape_size + 1];
	for (auto i = 0; i < _countof(vertices) - 1; ++i) {
		vertices[i] = {
			radius * sinf(XM_2PI / shape_size * (i + 1)) * (window_width / window_height),
			radius * cosf(XM_2PI / shape_size * (i + 1)),
			0.f,
		};
	}
	vertices[shape_size] = {0, 0, 0};

	UINT sizeVB = static_cast<UINT>(sizeof(XMFLOAT3) * _countof(vertices));

	//Set vertex buffer
	D3D12_HEAP_PROPERTIES heapprop{};
	heapprop.Type = D3D12_HEAP_TYPE_UPLOAD;	//Transfer to GPU
	heapprop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapprop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

	//Resource setting
	D3D12_RESOURCE_DESC resdesc{};
	resdesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resdesc.Width = sizeVB;
	resdesc.Height = 1;
	resdesc.DepthOrArraySize = 1;
	resdesc.MipLevels = 1;
	resdesc.Format = DXGI_FORMAT_UNKNOWN;
	resdesc.SampleDesc.Count = 1;
	resdesc.Flags = D3D12_RESOURCE_FLAG_NONE;
	resdesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	//Create top buffer
	ID3D12Resource *verBuff = nullptr;
	result = GPU.dev->CreateCommittedResource(
		&heapprop,
		D3D12_HEAP_FLAG_NONE,
		&resdesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&verBuff)
	);
	assert(result == S_OK);

	//Get VirtualMemory
	XMFLOAT3 *vertMap = nullptr;
	result = verBuff->Map(0, nullptr, (void **)&vertMap);
	assert(result == S_OK);

	std::copy(std::begin(vertices), std::end(vertices), vertMap);
	for (auto i = 0; i < _countof(vertices); ++i) {
		vertMap[i] = vertices[i];
	}
	verBuff->Unmap(0, nullptr);

	//Create top buffer view
	D3D12_VERTEX_BUFFER_VIEW vbView{};

	vbView.BufferLocation = verBuff->GetGPUVirtualAddress();
	/*vbView.SizeInBytes = sizeVB;*/
	vbView.SizeInBytes = sizeof(vertices);
	vbView.StrideInBytes = sizeof(XMFLOAT3);
#pragma endregion

#pragma region Index buffer
	unsigned short indices[shape_size * 3];

	for (int i = 0; i < shape_size; ++i) {
		indices[i * 3]			= i;
		indices[(i * 3) + 1]	= i + 1;
		indices[(i * 3) + 2]	= shape_size;
	}
	indices[(shape_size * 3) - 3]	= shape_size - 1;
	indices[(shape_size * 3) - 2]	= 0;
	indices[(shape_size * 3) - 1]	= shape_size;

	//Add index buffer
	ID3D12Resource *indexBuff = nullptr;
	resdesc.Width = sizeof(indices);
	result = GPU.dev->CreateCommittedResource(
		&heapprop,
		D3D12_HEAP_FLAG_NONE,
		&resdesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&indexBuff)
	);
	assert(result == S_OK);

	//Get virtual memory
	unsigned short *indexMap = nullptr;
	result = indexBuff->Map(0, nullptr, (void **)&indexMap);
	assert(result == S_OK);
	for (int i = 0; i < _countof(indices); ++i) {
		indexMap[i] = indices[i];
	}
	indexBuff->Unmap(0, nullptr);

	//Create index buffer view
	D3D12_INDEX_BUFFER_VIEW ibView{};
	ibView.BufferLocation = indexBuff->GetGPUVirtualAddress();
	ibView.Format = DXGI_FORMAT_R16_UINT;
	ibView.SizeInBytes = sizeof(indices);
#pragma endregion

#pragma region Shader
	ID3DBlob *vsBlob = nullptr;
	ID3DBlob *psBlob = nullptr;
	ID3DBlob *errorBlob = nullptr;

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
#pragma endregion

#pragma region const buffer
	//Heap
	D3D12_HEAP_PROPERTIES cbheapprop{};
	cbheapprop.Type = D3D12_HEAP_TYPE_UPLOAD;

	//Resources
	D3D12_RESOURCE_DESC cbresdesc{};
	cbresdesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	cbresdesc.Width = (sizeof(ConstBufferData) + 0xff) & ~0xff;
	cbresdesc.Height = 1;
	cbresdesc.DepthOrArraySize = 1;
	cbresdesc.MipLevels = 1;
	cbresdesc.SampleDesc.Count = 1;
	cbresdesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	//Create
	ID3D12Resource *constBuff = nullptr;
	result = GPU.dev->CreateCommittedResource(
		&cbheapprop,
		D3D12_HEAP_FLAG_NONE,
		&cbresdesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuff)
	);
	assert(result == S_OK);

	//Descripter heap
	ID3D12DescriptorHeap *basicDescHeap = nullptr;
	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc{};
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	descHeapDesc.NumDescriptors = 1;

	result = GPU.dev->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&basicDescHeap));
	assert(result == S_OK);

	//Create const buffer
	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc{};
	cbvDesc.BufferLocation = constBuff->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = (UINT)constBuff->GetDesc().Width;
	GPU.dev->CreateConstantBufferView(&cbvDesc, basicDescHeap->GetCPUDescriptorHandleForHeapStart());
#pragma endregion

	//ErrorMsg
	if (FAILED(result)) {
		std::string errstr;
		errstr.resize(errorBlob->GetBufferSize());
		std::copy_n(
			(char *)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			errstr.begin()
		);
		errstr += "\n";
		OutputDebugStringA(errstr.c_str());
		exit(1);
	}

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

	//Graphics pipeline
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline{};
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
	gpipeline.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	gpipeline.RasterizerState.DepthClipEnable = true;

	//Blend
	//gpipeline.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
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

	//top layout setting
	gpipeline.InputLayout.pInputElementDescs = inputLayout;
	gpipeline.InputLayout.NumElements = _countof(inputLayout);
	gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	//etc
	gpipeline.NumRenderTargets = 1;
	gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	gpipeline.SampleDesc.Count = 1;

	//root parameter
	D3D12_DESCRIPTOR_RANGE descTblrange{};
	descTblrange.NumDescriptors = 1;
	descTblrange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	descTblrange.BaseShaderRegister = 0;
	descTblrange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	//setting root parameter
	D3D12_ROOT_PARAMETER rootparam = {};
	rootparam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootparam.DescriptorTable.pDescriptorRanges = &descTblrange;
	rootparam.DescriptorTable.NumDescriptorRanges = 1;
	rootparam.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	//root signeture
	ID3D12RootSignature *rootsignature;
	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	rootSignatureDesc.pParameters = &rootparam;
	rootSignatureDesc.NumParameters = 1;

	ID3DBlob *rootSigBlob = nullptr;
	result = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob, &errorBlob);
	assert(result == S_OK);

	result = GPU.dev->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(), IID_PPV_ARGS(&rootsignature));
	assert(result == S_OK);

	rootSigBlob->Release();

	//set signature
	gpipeline.pRootSignature = rootsignature;

	ID3D12PipelineState *pipelinestate = nullptr;
	result = GPU.dev->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(&pipelinestate));
	assert(result == S_OK);

#pragma endregion

	//Input Initialize
	Input input(win32.w, win32.hwnd);

	while (true)
	{
#pragma region Frame process
		if (PeekMessage(&win32.msg, nullptr, 0, 0, PM_REMOVE)) {
			TranslateMessage(&win32.msg);
			DispatchMessage(&win32.msg);
		}

		if (win32.msg.message == WM_QUIT) {
			break;
		}

		//Update
		input.Update();

		//trans
		float speed = .01f;

		if (input.GetKey(DIK_W)) {
			gpipeline.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
			for (auto &verData : vertices) {
				verData.y += speed;
			}
		}
		if (input.GetKey(DIK_S)) {
			for (auto &verData : vertices) {
				verData.y -= speed;
			}
		}
		if (input.GetKey(DIK_A)) {
			for (auto &verData : vertices) {
				verData.x -= speed;
			}
		}
		if (input.GetKey(DIK_D)) {
			for (auto &verData : vertices) {
				verData.x += speed;
			}
		}

		//Get buck buffer number
		UINT bbIndex = swapchain->GetCurrentBackBufferIndex();

		//Resources barrier(change OP)
		D3D12_RESOURCE_BARRIER barrierDesc{};
		barrierDesc.Transition.pResource = backBuffers[bbIndex];
		barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;		//view
		barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;	//draw
		cmdList->ResourceBarrier(1, &barrierDesc);

		//Get Render target view discriper heap handle
		D3D12_CPU_DESCRIPTOR_HANDLE rtvH = rtvHeaps->GetCPUDescriptorHandleForHeapStart();
		rtvH.ptr += bbIndex * GPU.dev->GetDescriptorHandleIncrementSize(heapDesc.Type);
		cmdList->OMSetRenderTargets(1, &rtvH, false, nullptr);

		//Display clear
		float clearColor[] = { .5f, .7f, .3f, 1.f };
		cmdList->ClearRenderTargetView(rtvH, clearColor, 0, nullptr);

		//Get VirtualMemory
		XMFLOAT3 *vertMap = nullptr;
		result = verBuff->Map(0, nullptr, (void **)&vertMap);
		assert(result == S_OK);

		//Update point
		std::copy(std::begin(vertices), std::end(vertices), vertMap);
		verBuff->Unmap(0, nullptr);

		//Initialize const buffer
		XMFLOAT4 colorCache;
		if (input.GetKey(DIK_SPACE)) {
			colorCache = { 0.f, 0.f, 0.f, .5f };
		}
		else {
			colorCache = { 0.f, 0.f, 0.f, 1.f };
		}

		ConstBufferData *constMap = nullptr;
		result = constBuff->Map(0, nullptr, (void **)&constMap);
		constMap->color = colorCache;
		constBuff->Unmap(0, nullptr);
		assert(result == S_OK);
#pragma endregion

#pragma region Draw command
		//pipeline
		cmdList->SetPipelineState(pipelinestate);
		cmdList->SetGraphicsRootSignature(rootsignature);

		//Set Descriptor heap
		ID3D12DescriptorHeap *ppHeaps[] = { basicDescHeap };
		cmdList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

		//Set constant buffer view
		cmdList->SetGraphicsRootDescriptorTable(0, basicDescHeap->GetGPUDescriptorHandleForHeapStart());

		//viewport setting
		D3D12_VIEWPORT viewport{};

		viewport.Width = window_width;
		viewport.Height = window_height;
		viewport.TopLeftX = .0f;
		viewport.TopLeftY = .0f;
		viewport.MinDepth = .0f;
		viewport.MaxDepth = 1.f;

		cmdList->RSSetViewports(1, &viewport);

		//Set scissorrect
		D3D12_RECT scissorrect{};
		scissorrect.left = 0L;
		scissorrect.right = scissorrect.left + window_width;
		scissorrect.top = 0L;
		scissorrect.bottom = scissorrect.top + window_height;

		cmdList->RSSetScissorRects(1, &scissorrect);
		cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		cmdList->IASetVertexBuffers(0, 1, &vbView);

		//Index buffer set command
		cmdList->IASetIndexBuffer(&ibView);
		cmdList->DrawIndexedInstanced((int)_countof(indices), 1, 0, 0, 0);

		//Restore Resource barrier setting(writing inhibition)
		barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
		cmdList->ResourceBarrier(1, &barrierDesc);

		//Close command
		cmdList->Close();

		//Run commandlist
		ID3D12CommandList *cmdLists[] = { cmdList };
		cmdQueue->ExecuteCommandLists(1, cmdLists);

		//awaiting run commandlist
		cmdQueue->Signal(fence, ++fenceVal);
		if (fence->GetCompletedValue() != fenceVal) {
			HANDLE event = CreateEvent(nullptr, false, false, nullptr);
			fence->SetEventOnCompletion(fenceVal, event);

			if (event != 0) {
				WaitForSingleObject(event, INFINITE);
				CloseHandle(event);
			}
		}

		//Change display
		cmdAllocator->Reset();
		cmdList->Reset(cmdAllocator, nullptr);
		swapchain->Present(1, 0);
#pragma endregion
	}

	return 0;
}