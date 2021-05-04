#pragma once
class D3D12SetGPU
{
private:
	HRESULT result;
	D3D_FEATURE_LEVEL levels[4] = {
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0
	};

	std::vector<IDXGIAdapter1 *> adapters;
	IDXGIAdapter1 *tmpAdapter = nullptr;

public:
	IDXGIFactory6 *dxgiFactory;
	D3D_FEATURE_LEVEL featurelevel;
	ID3D12Device *dev;

public:
	void D3D12ListUpGPU();
	void D3D12SelectGPU();
	HRESULT D3D12FeatureLv();
};

