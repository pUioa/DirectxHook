#pragma once
#ifndef _H_Render_
#define _H_Render_

# include <dxgi.h>
# include <d3d11.h>
#include <functional>
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_dx11.h"
#include "ImGui/imgui_impl_win32.h"
#pragma comment(lib, "d3d11.lib")
LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);		//声明ImGui消息处理函数


class Render {
private:

	#define Index_Present 8										//虚表中第9个函数
	#define Index_ResizeBuffers 13								//虚表中第13个函数

	typedef HRESULT(__stdcall* Present) (IDXGISwapChain* This, UINT SyncInterval, UINT Flags);
	typedef HRESULT(__stdcall* ResizeBuffers)(IDXGISwapChain* This, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags);
public:
	HWND g_Hwnd = NULL;
	IDXGISwapChain* g_pSwapChain = nullptr;					    //D3D交换链
	ID3D11Device* g_pDevice = nullptr;							//D3D设备
	ID3D11DeviceContext* g_pContext = nullptr;                  //D3D环境
	ID3D11RenderTargetView* g_pmainRenderTargetView = nullptr;  //D3D渲染目标
public:
	uintptr_t* DirectxVirtualMethodTable = nullptr;				//指向  g_pSwapChain虚表

	Present Original_Present = nullptr;
	ResizeBuffers  Original_ResizeBuffers = nullptr;
	WNDPROC Original_WndProc;									//原窗口消息函数

	std::function<void(void)> CallBack;							//自渲染函数

	bool NeedReinitialization = false;							//ResizeBuffers后需要重新创建一些组件

private:
	BOOL GetSwapChain();
	BOOL GetVirtualMethodTable(uintptr_t* Table, int index, uintptr_t* Method);

	BOOL ChangeVirtualMethodTable(uintptr_t* Table, int index, void* NewMethod);
	BOOL ChangeWndProc(WNDPROC NewWndProc, WNDPROC* OldWndProc);
private:
	static  HRESULT STDMETHODCALLTYPE HookFn_Present(IDXGISwapChain* This, UINT SyncInterval, UINT Flags);
	static  HRESULT STDMETHODCALLTYPE HookFn_ResizeBuffers(IDXGISwapChain* This, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags);
	static	LRESULT WINAPI HookFn_WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

public:
	BOOL Init(std::function<void(void)>);
};
#endif // !_H_Render_
