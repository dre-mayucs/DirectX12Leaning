#include "includes.h"

const int window_width = 1000;
const int window_height = 1000;

//namespace
using namespace DirectX;

HRESULT result;
Win32 win32(L"Test", window_width, window_height);
DirectX12 dx12(win32.hwnd, window_width, window_height);

int WINAPI WinMain(_In_ HINSTANCE hinstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd) 
{
	dx12.Initialize_components();

	//Initialize IO
	Input input(win32.w, win32.hwnd);

	//InitializeDrawCommands
	Draw draw(100, 0.5, D3D12_FILL_MODE_WIREFRAME, dx12.dev, dx12.cmdList, window_width, window_height);
	Draw draw2(50, 1, D3D12_FILL_MODE_WIREFRAME, dx12.dev, dx12.cmdList, window_width, window_height);

	float speed = .01f;
	while (true)
	{
		dx12.ClearDrawScreen(dx12.GetColor(100, 200, 255, 255));
		input.Update();

		if (input.GetKey(DIK_W)) {
			for (auto &verData : draw.vertices) {
				verData.y += speed;
			}
		}
		if (input.GetKey(DIK_S)) {
			for (auto &verData : draw.vertices) {
				verData.y -= speed;
			}
		}
		if (input.GetKey(DIK_A)) {
			for (auto &verData : draw.vertices) {
				verData.x -= speed;
			}
		}
		if (input.GetKey(DIK_D)) {
			for (auto &verData : draw.vertices) {
				verData.x += speed;
			}
		}

		//viewport setting
		D3D12_VIEWPORT viewport{};

		viewport.Width = window_width;
		viewport.Height = window_height;
		viewport.TopLeftX = .0f;
		viewport.TopLeftY = .0f;
		viewport.MinDepth = .0f;
		viewport.MaxDepth = 1.f;

		dx12.cmdList->RSSetViewports(1, &viewport);

		//Set scissorrect
		D3D12_RECT scissorrect{};
		scissorrect.left = 0L;
		scissorrect.right = scissorrect.left + window_width;
		scissorrect.top = 0L;
		scissorrect.bottom = scissorrect.top + window_height;

		dx12.cmdList->RSSetScissorRects(1, &scissorrect);
		dx12.cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		draw.execute(dx12.GetColor(0, 0, 0, 255));
		draw2.execute(dx12.GetColor(255, 255, 0, 255));

		//Restore Resource barrier setting(writing inhibition)
		dx12.barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		dx12.barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
		dx12.cmdList->ResourceBarrier(1, &dx12.barrierDesc);

		//Change display
		dx12.ScreenFlip();
		if (!win32.ProcessMessage()) { break; }
		if (input.GetKeyDown(DIK_ESCAPE)) { break; }
	}
	return 0;
}