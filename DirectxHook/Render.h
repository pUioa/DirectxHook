#pragma once
#ifndef _H_Render_
#define _H_Render_

# include <dxgi.h>
# include <d3d11.h>
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_dx11.h"
#include "ImGui/imgui_impl_win32.h"
Render* g_pRender = nullptr;									//供类Render下静态函数访问动态成员
class Render {
private:
	typedef HRESULT(STDMETHODCALLTYPE* Present) (IDXGISwapChain* This, UINT SyncInterval, UINT Flags);
	typedef HRESULT(STDMETHODCALLTYPE* ResizeBuffers)(IDXGISwapChain* This, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags);
	
private:
	HWND g_Hwnd = NULL;
	WNDPROC Original_WndProc;									//原窗口消息函数
	IDXGISwapChain* g_pSwapChain = nullptr;					    //D3D交换链
	ID3D11Device* g_pDevice = nullptr;							//D3D设备
	ID3D11DeviceContext* g_pContext = nullptr;                  //D3D环境
	ID3D11RenderTargetView* g_mainRenderTargetView = nullptr;   //D3D渲染目标
private:
	uintptr_t* DirectxVirtualMethodTable = nullptr;				//指向  g_pSwapChain虚表
	Present Original_Present = nullptr;
	ResizeBuffers  Original_ResizeBuffers = nullptr;
private:
	BOOL GetSwapChain() {
		if (!this->g_Hwnd)
			return FALSE;
		D3D_FEATURE_LEVEL featureLevel;
		const D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_11_0 };

		DXGI_RATIONAL refreshRate;
		refreshRate.Numerator = 60;
		refreshRate.Denominator = 1;

		DXGI_MODE_DESC bufferDesc;
		bufferDesc.Width = 100;
		bufferDesc.Height = 100;
		bufferDesc.RefreshRate = refreshRate;
		bufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		bufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		bufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

		DXGI_SAMPLE_DESC sampleDesc;
		sampleDesc.Count = 1;
		sampleDesc.Quality = 0;

		DXGI_SWAP_CHAIN_DESC swapChainDesc;
		swapChainDesc.BufferDesc = bufferDesc;
		swapChainDesc.SampleDesc = sampleDesc;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = 1;
		swapChainDesc.OutputWindow = this->g_Hwnd;
		swapChainDesc.Windowed = 1;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

		if (D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, featureLevels, 2, D3D11_SDK_VERSION, &swapChainDesc, &this->g_pSwapChain, nullptr , &featureLevel, nullptr) != S_OK)
		{
			return FALSE;
		}
		return this->g_pSwapChain != nullptr;
	}

private:
	static  HRESULT STDMETHODCALLTYPE HookFn_Present(IDXGISwapChain* This, UINT SyncInterval, UINT Flags) {
	
	}

	static  HRESULT STDMETHODCALLTYPE HookFn_Present(IDXGISwapChain* This, UINT SyncInterval, UINT Flags) {

	}

	static  HRESULT STDMETHODCALLTYPE HookFn_Present(IDXGISwapChain* This, UINT SyncInterval, UINT Flags) {

	}
public:


	BOOL Init() {
		this->g_Hwnd = ::FindWindowA("UnrealWindow", NULL);
		if (this->GetSwapChain()) {
			this->DirectxVirtualMethodTable = *(uintptr_t**)this->g_pSwapChain;
			auto a = &this->Render::HookFn_Present;
		}
	}
};
#endif // !_H_Render_
