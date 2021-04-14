//API
#include <Windows.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <DirectXMath.h>
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

//API(IO)
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

//shader
#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")

//STL
#include <vector>
#include <string>

//namespace
using namespace DirectX;

//WindowProcedure
LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	if (msg == WM_DESTROY) {
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hwnd, msg, wparam, lparam);
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) 
{
#ifdef _DEBUG
	ID3D12Debug *debugController;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
		debugController->EnableDebugLayer();
	}
#endif // _DEBUG

#pragma region WinAPI Initialize
	OutputDebugStringA("Hello,DirectX!!\n");
	const int window_width = 1280;
	const int window_height = 720;

	WNDCLASSEX w{};
	w.cbSize = sizeof(WNDCLASSEX);
	w.lpfnWndProc = (WNDPROC)WindowProc;
	w.lpszClassName = L"DirectXGame";
	w.hInstance = GetModuleHandle(nullptr);
	w.hCursor = LoadCursor(NULL, IDC_ARROW);

	//Window class add
	RegisterClassEx(&w);

	//Window Size
	RECT wrc = { 0, 0, window_width, window_height };
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

	HWND hwnd = CreateWindow(
		w.lpszClassName,		//Class name
		L"DirectXGame",			//Title bar
		WS_OVERLAPPEDWINDOW,	//Window type
		CW_USEDEFAULT,			//Window position
		CW_USEDEFAULT,			//Window position
		wrc.right - wrc.left,	//Window width
		wrc.bottom - wrc.top,	//Window height
		nullptr,				//Window handle
		nullptr,				//Menu handle
		w.hInstance,			//Call application handke
		nullptr					//param
	);

	//View window
	ShowWindow(hwnd, SW_SHOW);

	//Msg
	MSG msg{};
#pragma endregion

#pragma region DirectX12 Initialize components
	HRESULT result;
	ID3D12Device *dev = nullptr;
	IDXGIFactory6 *dxgiFactory = nullptr;
	IDXGISwapChain4 *swapchain = nullptr;

	ID3D12CommandAllocator *cmdAllocator = nullptr;
	ID3D12GraphicsCommandList *cmdList = nullptr;
	ID3D12CommandQueue *cmdQueue = nullptr;
	ID3D12DescriptorHeap *rtvHeaps = nullptr;

	//Graphics adapter
	result = CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory));
	std::vector<IDXGIAdapter1 *> adapters;
	IDXGIAdapter1 *tmpAdapter = nullptr;

	//Listup GPU
	for (auto i = 0; dxgiFactory->EnumAdapters1(i, &tmpAdapter) != DXGI_ERROR_NOT_FOUND; ++i) {
		adapters.push_back(tmpAdapter);
	}

	//Select GPU
	for (auto i = 0; i < adapters.size(); ++i) {
		DXGI_ADAPTER_DESC1 adesc;
		adapters[i]->GetDesc1(&adesc);

		//throw software GPU
		if (adesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) {
			continue;
		}

		std::wstring strDesc = adesc.Description;

		//Negative keywords
		if (strDesc.find(L"Intel") == std::wstring::npos) {
			tmpAdapter = adapters[i];
			break;
		}
	}

	//Feature level
	D3D_FEATURE_LEVEL featurelevel;
	D3D_FEATURE_LEVEL levels[] = {
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0
	};

	for (auto i = 0; i < _countof(levels); ++i) {
		result = D3D12CreateDevice(tmpAdapter, levels[i], IID_PPV_ARGS(&dev));
		if (result == S_OK) {
			featurelevel = levels[i];
			break;
		}
	}

	//Genelate allocater
	result = dev->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(&cmdAllocator)
	);

	//Genelate commandlist
	result = dev->CreateCommandList(
		0,
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		cmdAllocator,
		nullptr,
		IID_PPV_ARGS(&cmdList)
	);

	//Command queue
	D3D12_COMMAND_QUEUE_DESC cmdQueueDesc{};
	dev->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(&cmdQueue));

	//Swap chain
	DXGI_SWAP_CHAIN_DESC1 swapchainDesc{};
	swapchainDesc.Width = 1280;
	swapchainDesc.Height = 720;
	swapchainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapchainDesc.SampleDesc.Count = 1;
	swapchainDesc.BufferUsage = DXGI_USAGE_BACK_BUFFER;
	swapchainDesc.BufferCount = 2;
	swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapchainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	dxgiFactory->CreateSwapChainForHwnd(
		cmdQueue,
		hwnd,
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
	dev->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&rtvHeaps));

	//target view
	std::vector<ID3D12Resource *> backBuffers(2);
	for (auto i = 0; i < 2; ++i) {
		result = swapchain->GetBuffer(i, IID_PPV_ARGS(&backBuffers[i]));
		D3D12_CPU_DESCRIPTOR_HANDLE handle = rtvHeaps->GetCPUDescriptorHandleForHeapStart();
		handle.ptr += i * dev->GetDescriptorHandleIncrementSize(heapDesc.Type);
		dev->CreateRenderTargetView(
			backBuffers[i],
			nullptr,
			handle
		);
	}

	//Generate Fence
	ID3D12Fence *fence = nullptr;
	UINT64 fenceVal = 0;
	result = dev->CreateFence(fenceVal, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
#pragma endregion

#pragma region DirectInput Initialize component
	//Initialize
	IDirectInput8 *dinput = nullptr;
	result = DirectInput8Create(
		w.hInstance,
		DIRECTINPUT_VERSION, 
		IID_IDirectInput8, 
		(void**)&dinput, 
		nullptr
	);

	//Create keybord device
	IDirectInputDevice8 *devkeybord = nullptr;
	result = dinput->CreateDevice(GUID_SysKeyboard, &devkeybord, NULL);

	//Set data format
	result = devkeybord->SetDataFormat(&c_dfDIKeyboard);

	//Set cooperativel level
	result = devkeybord->SetCooperativeLevel(
		hwnd,
		DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY
		//Active window IO only
		//No exclusive IO
		//Invalidation Windows
	);
#pragma endregion


#pragma region Initialize drawing command
	XMFLOAT3 vertices[] = {
		//Lower left
		{ -.5f, -.5f, .0f },
		{ -.5f, +.5f, .0f },
		{ +.5f, -.5f, .0f },

		//Upper right
		{ +.5f, -.5f, .0f },
		{ -.5f, +.5f, .0f },
		{ +.5f, +.5f, .0f },
	};
	UINT sizeVB = static_cast<UINT>(sizeof(XMFLOAT3) * _countof(vertices));

	//Top buffer setting
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

	//Genelate top buffer
	ID3D12Resource *verBuff = nullptr;
	result = dev->CreateCommittedResource(
		&heapprop,
		D3D12_HEAP_FLAG_NONE,
		&resdesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&verBuff)
	);

	//Get VirtualMemory
	XMFLOAT3 *vertMap = nullptr;
	result = verBuff->Map(0, nullptr, (void **)&vertMap);
	std::copy(std::begin(vertices), std::end(vertices), vertMap);
	for (auto i = 0; i < _countof(vertices); ++i ) {
		vertMap[i] = vertices[i];
	}
	verBuff->Unmap(0, nullptr);

	//Create top buffer view
	D3D12_VERTEX_BUFFER_VIEW vbView{};

	vbView.BufferLocation = verBuff->GetGPUVirtualAddress();
	/*vbView.SizeInBytes = sizeVB;*/
	vbView.SizeInBytes = sizeof(vertices);
	vbView.StrideInBytes = sizeof(XMFLOAT3);

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

	result = D3DCompileFromFile(
		L"BasicPS.hlsl",
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"main", "ps_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&psBlob, &errorBlob
	);

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
	gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	gpipeline.RasterizerState.MultisampleEnable = false;
	gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	gpipeline.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	gpipeline.RasterizerState.DepthClipEnable = true;
	gpipeline.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	//top layout setting
	gpipeline.InputLayout.pInputElementDescs = inputLayout;
	gpipeline.InputLayout.NumElements = _countof(inputLayout);
	gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	//etc
	gpipeline.NumRenderTargets = 1;
	gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	gpipeline.SampleDesc.Count = 1;

	//root signeture
	ID3D12RootSignature *rootsignature;
	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	ID3DBlob *rootSigBlob = nullptr;
	result = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob, &errorBlob);
	result = dev->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(), IID_PPV_ARGS(&rootsignature));
	rootSigBlob->Release();

	//set signature
	gpipeline.pRootSignature = rootsignature;

	ID3D12PipelineState *pipelinestate = nullptr;
	result = dev->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(&pipelinestate));

#pragma endregion

	while (true)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (msg.message == WM_QUIT) {
			break;
		}
#pragma region Get IO status
		//Get
		result = devkeybord->Acquire();

		//Get all key input status
		BYTE key[256] = {};
		result = devkeybord->GetDeviceState(sizeof(key), key);

#ifdef _DEBUG
		if (key[DIK_0]) {
			OutputDebugStringA("Hit 0\n");
		}
#endif // _DEBUG

#pragma endregion

#pragma region Frame process
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
		rtvH.ptr += bbIndex * dev->GetDescriptorHandleIncrementSize(heapDesc.Type);
		cmdList->OMSetRenderTargets(1, &rtvH, false, nullptr);

		//Display clear
		//Change clear color if space key is pressed
		if (key[DIK_SPACE]) {
			float clearColor[] = { .5f, 1.f, .5f, .0f };
			cmdList->ClearRenderTargetView(rtvH, clearColor, 0, nullptr);
		}
		else {
			float clearColor[] = { .3f, .3f, .3f, .0f };
			cmdList->ClearRenderTargetView(rtvH, clearColor, 0, nullptr);
		}

		//Move object if arrow right key pressed
		if (key[DIK_RIGHT]) {
			vertices[0].x += .1f;
			vertices[1].x += .1f;
			vertices[2].x += .1f;
		}

		//Get VirtualMemory
		XMFLOAT3 *vertMap = nullptr;
		result = verBuff->Map(0, nullptr, (void **)&vertMap);
		std::copy(std::begin(vertices), std::end(vertices), vertMap);
		verBuff->Unmap(0, nullptr);
#pragma endregion

#pragma region Draw command
		//pipeline
		cmdList->SetPipelineState(pipelinestate);
		cmdList->SetGraphicsRootSignature(rootsignature);

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
		scissorrect.left = 0.f;
		scissorrect.right = scissorrect.left + window_width;
		scissorrect.top = 0.f;
		scissorrect.bottom = scissorrect.top + window_height;

		cmdList->RSSetScissorRects(1, &scissorrect);
		cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		cmdList->IASetVertexBuffers(0, 1, &vbView);
		cmdList->DrawInstanced((int)_countof(vertices), 1, 0, 0);
#pragma endregion

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
			WaitForSingleObject(event, INFINITE);
			CloseHandle(event);
		}

		//Change display
		cmdAllocator->Reset();
		cmdList->Reset(cmdAllocator, nullptr);
		swapchain->Present(1, 0);
	}

	return 0;
}