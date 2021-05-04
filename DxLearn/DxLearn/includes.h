#pragma once

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
#include "D3D12Commands.h"