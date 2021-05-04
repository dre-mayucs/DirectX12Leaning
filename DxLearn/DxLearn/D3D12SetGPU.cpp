//Win32API
#include <Windows.h>

//DirectX
#include <d3d12.h>
#include <dxgi1_6.h>

//STL
#include <vector>
#include <string>
#include <assert.h>

//Utility
#include "D3D12SetGPU.h"

void D3D12SetGPU::D3D12ListUpGPU()
{
	result = CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory));
	assert(result == S_OK);

	for (auto i = 0; dxgiFactory->EnumAdapters1(i, &tmpAdapter) != DXGI_ERROR_NOT_FOUND; ++i) {
		adapters.push_back(tmpAdapter);
	}
}

void D3D12SetGPU::D3D12SelectGPU()
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

HRESULT D3D12SetGPU::D3D12FeatureLv()
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


