#include <iostream>
#include <Windows.h>
#include "dllmain.h"
#include "engine.h"
#include "VMProtectSDK.h"
#pragma comment(lib, "VMProtectSDK64.lib")
using namespace std;
HMODULE m_hModule = NULL;
BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	VMProtectBeginMutation("Crepe::DllMain");
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	{
		m_hModule = hModule;
		InitializeEngine();
		break;
	}
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
	VMProtectEnd();
}
