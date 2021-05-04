#include <Windows.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <DirectXMath.h>

#include "D3D12Commands.h"

D3D12Commands::D3D12Commands(ID3D12Device *dev) : dev(dev) 
{
	cmdAllocator	= nullptr;
	cmdList			= nullptr;
	cmdQueue		= nullptr;
	cmdQueueDesc	= {};
}

HRESULT D3D12Commands::D3D12CreateCommandAllocator()
{
	result = dev->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(&cmdAllocator)
	);
	assert(result == S_OK);
	return result;
}

HRESULT D3D12Commands::D3D12CreateCommandList()
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

HRESULT D3D12Commands::D3D12CreateCommandQueueDescription()
{
	result = dev->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(&cmdQueue));
	assert(result == S_OK);
	return result;
}