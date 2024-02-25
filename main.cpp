#include "main.h"
bool vlods = false;

void _cdecl fogdist(char* params) {
	int fd = atoi(params);
	DWORD nold;
	VirtualProtect((LPVOID)0xB7C4F0, 4, PAGE_EXECUTE_READWRITE, &nold);
	*(float*)0xB7C4F0 = (float)fd;
	VirtualProtect((LPVOID)0xB7C4F0, 4, nold, NULL);
}

void _cdecl tideup(char* params) {
	DWORD old;
	VirtualProtect((LPVOID)0x8E4CB4, 4, PAGE_EXECUTE_READWRITE, &old);
	int oldmem = *reinterpret_cast<int*>(0x8E4CB4);
	VirtualProtect((LPVOID)0x8E4CB4, 4, old, NULL);
	if (oldmem > 419430400) {

		CGame::TidyUpMemory(true, true);
		CGame::DrasticTidyUpMemory(true);
		CStreaming::RemoveAllUnusedModels();
		CStreaming::DeleteAllRwObjects();
		CObject::DeleteAllTempObjects();
		CShadows::TidyUpShadows();

		CVector pos = FindPlayerCoors(-1);
		CStreaming::LoadScene(&pos);
		CStreaming::LoadSceneCollision(&pos);
		DWORD old;
		VirtualProtect((LPVOID)0x8E4CB4, 4, PAGE_EXECUTE_READWRITE, &old);
		int newmem = *reinterpret_cast<int*>(0x8E4CB4);
		VirtualProtect((LPVOID)0x8E4CB4, 4, old, NULL);
		SAMP::pSAMP->addMessageToChat(-1, "before: %d mb, now: %d mb", oldmem / 1048576, newmem / 1048576);
	}
	else {
		SAMP::pSAMP->addMessageToChat(-1, "nothing to clean (used under 400 mb)");
	}
}

void _cdecl vehlods(char* params) {
	vlods ^= true;
	DWORD nold;
	VirtualProtect((LPVOID)0x52C9EE, 4, PAGE_EXECUTE_READWRITE, &nold);
	*(DWORD*)0x52C9EE = vlods ? 0x1 : 0x0;
	VirtualProtect((LPVOID)0x52C9EE, 4, nold, NULL);
}

void _cdecl all(char* params) {
	fogdist("60");
	tideup("a");
	vehlods("a");
}

void __stdcall GameLoop() {
	static bool initialized = false;
	if (!initialized) {
		if (SAMP::pSAMP->LoadAPI()) {
			initialized = true;
			SAMP::pSAMP->addClientCommand("fd", fogdist);
			SAMP::pSAMP->addClientCommand("cmem", tideup);
			SAMP::pSAMP->addClientCommand("vlods", vehlods);
			SAMP::pSAMP->addClientCommand("potatopc", all);
		}
	}
	if (initialized) {

	}
}

int __stdcall DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)
{
	switch (dwReason)
	{
		case DLL_PROCESS_ATTACH: {
			SAMP::Init();
			SAMP::CallBacks::pCallBackRegister->RegisterGameLoopCallback(GameLoop);
			break;
		}
		case DLL_PROCESS_DETACH: {
			SAMP::pSAMP->unregisterChatCommand(fogdist);
			SAMP::pSAMP->unregisterChatCommand(tideup);
			SAMP::ShutDown();
			break;
		}
	}
	return true;
}