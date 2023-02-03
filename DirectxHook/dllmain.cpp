// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "Render.h"
#include <Windows.h>
void cc() {
	ImGui::Begin("Hello");
	float samples[100];
	for (int n = 0; n < 100; n++)
		samples[n] = sinf(n * 0.2f + ImGui::GetTime() * 1.5f);
	ImGui::PlotLines("Samples", samples, 100);

	ImGui::End();
	ImGui::GetForegroundDrawList()->AddLine(ImVec2(100.0f, 100.0f), ImVec2(500.0f, 500.0f), IM_COL32(0, 255, 255, 255), 1.0f);
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

