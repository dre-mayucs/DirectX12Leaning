#pragma once
class DirectX12
{
public: //Public function
	//Constructor / Destructor
	DirectX12(HWND hwnd, const int window_width, const int window_height);
	void Initialize_components();

public: //Private function
	//GPU
	void D3D12ListUpGPU();
	void D3D12SelectGPU();
	HRESULT D3D12FeatureLv();

	//Commands
	HRESULT D3D12CreateCommandAllocator();
	HRESULT D3D12CreateCommandList();
	HRESULT D3D12CreateCommandQueueDescription();

	//Swapchain
	void D3D12CreateSwapchainDescription();

private: //Win32 etc...
	HRESULT result;
	HWND hwnd;
	int window_width;
	int window_height;

public: //DirectX12
	//GPU
	ID3D12Device *dev;
	std::vector<IDXGIAdapter1 *> adapters;
	IDXGIAdapter1 *tmpAdapter;
	IDXGIFactory6 *dxgiFactory;
	D3D_FEATURE_LEVEL featurelevel;
	D3D_FEATURE_LEVEL levels[4] = {
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0
	};

	//Commands
	ID3D12CommandAllocator *cmdAllocator;
	ID3D12GraphicsCommandList *cmdList;
	ID3D12CommandQueue *cmdQueue;
	D3D12_COMMAND_QUEUE_DESC cmdQueueDesc;

	//Swapchain
	DXGI_SWAP_CHAIN_DESC1 swapchainDesc;
	IDXGISwapChain4 *swapchain;
};

