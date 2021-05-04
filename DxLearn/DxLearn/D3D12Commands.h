#pragma once
class D3D12Commands
{
private:
	HRESULT result;
	ID3D12Device *dev;

public:
	ID3D12CommandAllocator *cmdAllocator;
	ID3D12GraphicsCommandList *cmdList;
	ID3D12CommandQueue *cmdQueue;
	D3D12_COMMAND_QUEUE_DESC cmdQueueDesc;

public:
	D3D12Commands(ID3D12Device *dev);
	HRESULT D3D12CreateCommandAllocator();
	HRESULT D3D12CreateCommandList();
	HRESULT D3D12CreateCommandQueueDescription();
};

