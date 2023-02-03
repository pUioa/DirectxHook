#include "Render.h"
Render* g_pRender = nullptr;									//供类Render下静态函数访问动态成员
BOOL Render::GetSwapChain() {
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

	if (D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, featureLevels, 2, D3D11_SDK_VERSION, &swapChainDesc, &this->g_pSwapChain, nullptr, &featureLevel, nullptr) != S_OK)
	{
		return FALSE;
	}
	return this->g_pSwapChain != nullptr;
}
BOOL Render::GetVirtualMethodTable(uintptr_t* Table, int index, uintptr_t* Method) {
	try {

		*Method = Table[index];;					//获取虚表方法
		return TRUE;
	}
	catch (...) {};

	return FALSE;
}

BOOL Render::ChangeVirtualMethodTable(uintptr_t* Table, int index, void* NewMethod) {
	if (NewMethod) {
		DWORD OldProtect = NULL;
		try {
			VirtualProtect((LPVOID)&Table[index], sizeof(uintptr_t), PAGE_EXECUTE_READWRITE, &OldProtect); //修改内存属性 虚表可读可执行不可写
			Table[index] = (uintptr_t)NewMethod;//替换我们的方法
			VirtualProtect((LPVOID)&Table[index], sizeof(uintptr_t), OldProtect, &OldProtect);						//改回内存属性
			return TRUE;
		}
		catch (...) {};
	}
	return FALSE;
}
BOOL Render::ChangeWndProc(WNDPROC NewWndProc, WNDPROC* OldWndProc) {
	*OldWndProc = (WNDPROC)SetWindowLongPtrA(this->g_Hwnd, GWLP_WNDPROC, (LONG_PTR)NewWndProc);
	return *OldWndProc != NULL;
}

HRESULT STDMETHODCALLTYPE Render::HookFn_Present(IDXGISwapChain* This, UINT SyncInterval, UINT Flags) {


	static bool HaveInitImGuiFn = false;
	if (!HaveInitImGuiFn) {
		if (SUCCEEDED(This->GetDevice(__uuidof(ID3D11Device), (void**)&g_pRender->g_pDevice)))
		{
			g_pRender->g_pDevice->GetImmediateContext(&g_pRender->g_pContext);


			DXGI_SWAP_CHAIN_DESC Desc;
			This->GetDesc(&Desc);
			g_pRender->g_Hwnd = Desc.OutputWindow;
			ID3D11Texture2D* pBackBuffer;
			This->GetBuffer(NULL, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
			g_pRender->g_pDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_pRender->g_pmainRenderTargetView);
			pBackBuffer->Release();


			ImGui::CreateContext();
			ImGuiIO& io = ImGui::GetIO();
			io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;
			io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\simkai.ttf", 15.0f, 0, io.Fonts->GetGlyphRangesChineseFull());
			io.ChangeImGuiFilePath("C:\\XinYu\\ImGui.ini", "C:\\XinYu\\ImGui.log");

			ImGui_ImplWin32_Init(g_pRender->g_Hwnd);
			ImGui_ImplDX11_Init(g_pRender->g_pDevice, g_pRender->g_pContext);
			HaveInitImGuiFn = true;
		}
		else
			return g_pRender->Original_Present(This, SyncInterval, Flags);
		
	}
	if (g_pRender->NeedReinitialization) {
		if (SUCCEEDED(This->GetDevice(__uuidof(ID3D11Device), (void**)&g_pRender->g_pDevice)))
		{
			g_pRender->g_pDevice->GetImmediateContext(&g_pRender->g_pContext);


			DXGI_SWAP_CHAIN_DESC Desc;
			This->GetDesc(&Desc);
			g_pRender->g_Hwnd = Desc.OutputWindow;
			ID3D11Texture2D* pBackBuffer;
			This->GetBuffer(NULL, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
			g_pRender->g_pDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_pRender->g_pmainRenderTargetView);
			pBackBuffer->Release();

			ImGui_ImplDX11_Init(g_pRender->g_pDevice, g_pRender->g_pContext);
			g_pRender->NeedReinitialization = false;
		}
	}

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	g_pRender->CallBack();

	ImGui::Render();
	g_pRender->g_pContext->OMSetRenderTargets(1, &g_pRender->g_pmainRenderTargetView, NULL);
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	return  g_pRender->Original_Present(This, SyncInterval, Flags);

}
HRESULT STDMETHODCALLTYPE Render::HookFn_ResizeBuffers(IDXGISwapChain* This, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags) {
	
	if (g_pRender->g_pDevice) {
		g_pRender->g_pDevice->Release();
		g_pRender->g_pmainRenderTargetView->Release();
		ImGui_ImplDX11_Shutdown();
		g_pRender->g_pDevice = nullptr;
		g_pRender->NeedReinitialization = true;
	}

	return g_pRender->Original_ResizeBuffers(This,BufferCount,Width,Height,NewFormat,SwapChainFlags);
}
LRESULT WINAPI Render::HookFn_WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
		return true;
	return CallWindowProcA(g_pRender->Original_WndProc, hWnd, uMsg, wParam, lParam);
}


BOOL Render::Init(std::function<void(void)> Fn) {
	this->g_Hwnd = ::FindWindowA("UnrealWindow", NULL);

	if (this->GetSwapChain()) {
		g_pRender = this;																												//立即填充指针 挂钩后会立即调用
		this->CallBack = Fn;																											//立即填充方法 挂钩后会立即调用

		this->DirectxVirtualMethodTable = *(uintptr_t**)this->g_pSwapChain;																//取得Dx虚表


		this->GetVirtualMethodTable(this->DirectxVirtualMethodTable, Index_Present, (uintptr_t*)&this->Original_Present);				//获取原Present
		this->GetVirtualMethodTable(this->DirectxVirtualMethodTable, Index_ResizeBuffers, (uintptr_t*)&this->Original_ResizeBuffers);   //获取原ResizeBuffers

		this->ChangeWndProc((WNDPROC)&HookFn_WndProc, &this->Original_WndProc);														    //替换窗口消息	

		this->ChangeVirtualMethodTable(this->DirectxVirtualMethodTable, Index_Present, &HookFn_Present);								//替换虚表指针

		this->ChangeVirtualMethodTable(this->DirectxVirtualMethodTable, Index_ResizeBuffers, &HookFn_ResizeBuffers);					//替换虚表指针
	
	
	
		this->g_pSwapChain->Release();																									//自己创建的交换链就可以释放了
		return TRUE;
	}
	return FALSE;
}
