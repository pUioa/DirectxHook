// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "Render.h"
#include <Windows.h>
void cc() {
	ImGui::Begin("Hellp");
	ImGui::End();
}
void Entry() {
	Render p;
	p.Init(cc);
	Sleep(INFINITE);
}
BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		AllocConsole();
		freopen("CONOUT$", "w", stdout);
		CloseHandle(CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)Entry, NULL, NULL, NULL));
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

