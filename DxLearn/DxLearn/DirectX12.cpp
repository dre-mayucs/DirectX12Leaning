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
	D3D12CreateSwapchainDescription();
}

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
void DirectX12::D3D12CreateSwapchainDescription()
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