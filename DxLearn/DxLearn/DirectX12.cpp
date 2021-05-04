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
#include "tempUtility.h"

//this me
#include "DirectX12.h"

DirectX12::DirectX12(HWND hwnd, const int window_width, const int window_height) : hwnd(hwnd), window_width(window_width), window_height(window_height)
{
	//GPU
	result			= S_FALSE;
	dev				= nullptr;
	dxgiFactory		= nullptr;
	tmpAdapter		= nullptr;
	adapters		= {};
	featurelevel	= {};

	//Commands
	cmdAllocator	= nullptr;
	cmdList			= nullptr;
	cmdQueue		= nullptr;
	cmdQueueDesc	= {};

	//Swapchain
	swapchain		= nullptr;
	swapchainDesc	= {};

	//Heap
	heapDesc		= {};

	//Fence
	fence			= nullptr;
	fenceVal		= 0;

	//Draw
	barrierDesc		= {};
}

void DirectX12::Initialize_components()
{
	//Initiaize GPU
	D3D12ListUpGPU();
	D3D12SelectGPU();
	D3D12FeatureLv();

	//Create allocater
	D3D12CreateCommandAllocator();
	D3D12CreateCommandList();
	D3D12CreateCommandQueueDescription();

	//Swap chain
	D3D12SetSwapchainDescription();

	//Heap
	D3D12SetDescripterHeap();

	//Target veiw
	D3D12SetTargetView();

	//Fence
	D3D12CreateFence();
}

#pragma region Draw
DirectX::XMFLOAT4 DirectX12::GetColor(const float R, const float G, const float B, const float A)
{
	return DirectX::XMFLOAT4(R / 255, G / 255, B / 255, A / 255);
}

void DirectX12::ClearDrawScreen(const DirectX::XMFLOAT4 color)
{
	//Get buck buffer number
	UINT bbIndex = swapchain->GetCurrentBackBufferIndex();

	//Resources barrier(change OP)
	barrierDesc.Transition.pResource = backBuffers[bbIndex];
	barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;		//view
	barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;	//draw
	cmdList->ResourceBarrier(1, &barrierDesc);

	//Get Render target view discriper heap handle
	D3D12_CPU_DESCRIPTOR_HANDLE rtvH = rtvHeaps->GetCPUDescriptorHandleForHeapStart();
	rtvH.ptr += bbIndex * dev->GetDescriptorHandleIncrementSize(heapDesc.Type);
	cmdList->OMSetRenderTargets(1, &rtvH, false, nullptr);

	//Display clear
	float clearColor[] = { color.x, color.y, color.z, color.w };
	cmdList->ClearRenderTargetView(rtvH, clearColor, 0, nullptr);
}

void DirectX12::ScreenFlip()
{
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

	cmdAllocator->Reset();
	cmdList->Reset(cmdAllocator, nullptr);
	swapchain->Present(1, 0);
}
#pragma endregion

#pragma region GPU
void DirectX12::D3D12ListUpGPU()
{
	result = CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory));
	assert(result == S_OK);

	for (auto i = 0; dxgiFactory->EnumAdapters1(i, &tmpAdapter) != DXGI_ERROR_NOT_FOUND; ++i) {
		adapters.push_back(tmpAdapter);
	}
}

void DirectX12::D3D12SelectGPU()
{
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
}

HRESULT DirectX12::D3D12FeatureLv()
{
	for (auto i = 0; i < _countof(levels); ++i) {
		result = D3D12CreateDevice(tmpAdapter, levels[i], IID_PPV_ARGS(&dev));
		assert(result == S_OK);

		if (result == S_OK) {
			featurelevel = levels[i];
			break;
		}
	}
	assert(result == S_OK);

	return result;
}
#pragma endregion

#pragma region Commands
HRESULT DirectX12::D3D12CreateCommandAllocator()
{
	result = dev->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(&cmdAllocator)
	);
	assert(result == S_OK);
	return result;
}

HRESULT DirectX12::D3D12CreateCommandList()
{
	result = dev->CreateCommandList(
		0,
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		cmdAllocator,
		nullptr,
		IID_PPV_ARGS(&cmdList)
	);
	assert(result == S_OK);
	return result;
}

HRESULT DirectX12::D3D12CreateCommandQueueDescription()
{
	result = dev->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(&cmdQueue));
	assert(result == S_OK);
	return result;
}
#pragma endregion

#pragma region Swapchain
void DirectX12::D3D12SetSwapchainDescription()
{
	swapchainDesc.Width = window_width;
	swapchainDesc.Height = window_height;
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
}
#pragma endregion

#pragma region Heap
void DirectX12::D3D12SetDescripterHeap()
{
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;

	//Front and Back screen layer
	heapDesc.NumDescriptors = 2;
	dev->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&rtvHeaps));
}
#pragma endregion

#pragma region Target view
void DirectX12::D3D12SetTargetView()
{
	//target view
	for (auto i = 0; i < 2; ++i) {
		result = swapchain->GetBuffer(i, IID_PPV_ARGS(&backBuffers[i]));
		assert(result == S_OK);

		D3D12_CPU_DESCRIPTOR_HANDLE handle = rtvHeaps->GetCPUDescriptorHandleForHeapStart();
		handle.ptr += i * dev->GetDescriptorHandleIncrementSize(heapDesc.Type);
		dev->CreateRenderTargetView(
			backBuffers[i],
			nullptr,
			handle
		);
	}
}
#pragma endregion

#pragma region Fence
void DirectX12::D3D12CreateFence()
{
	//Generate Fence
	result = dev->CreateFence(fenceVal, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
	assert(result == S_OK);
}
#pragma endregion