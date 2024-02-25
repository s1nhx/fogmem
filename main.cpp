#include "main.h"
bool vlods = false; // not necessary

void _cdecl fogdist(char* params) {
	int fd = atoi(params);
	DWORD nold;
	VirtualProtect((LPVOID)0xB7C4F0, 4, PAGE_EXECUTE_READWRITE, &nold); // saving memory protection & changing it to READWRITE
	*(float*)0xB7C4F0 = (float)fd;
	VirtualProtect((LPVOID)0xB7C4F0, 4, nold, NULL); // returning old protection
}

void _cdecl tideup(char* params) {
	DWORD old;
	VirtualProtect((LPVOID)0x8E4CB4, 4, PAGE_EXECUTE_READWRITE, &old); // amount of memory that being used. also protected
	int oldmem = *reinterpret_cast<int*>(0x8E4CB4);
	VirtualProtect((LPVOID)0x8E4CB4, 4, old, NULL);
	if (oldmem > 419430400) { // 400 mb

		CGame::TidyUpMemory(true, true); // 0x53C500
		CGame::DrasticTidyUpMemory(true); // 0x53C810
		CStreaming::RemoveAllUnusedModels(); // 0x40CF80
		CStreaming::DeleteAllRwObjects(); // 0x4090A0
		CObject::DeleteAllTempObjects(); // 0x5A18B0 | 0x5A1980 to clean in area (requires point and radius)
		CShadows::TidyUpShadows(); // 0x707770

		CVector pos = FindPlayerCoors(-1);
		CStreaming::LoadScene(&pos); // reloading scenes which may got corrupted
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

void _cdecl vehlods(char* params) { // could be done in single line :/
	vlods ^= true;
	DWORD nold;
	VirtualProtect((LPVOID)0x52C9EE, 4, PAGE_EXECUTE_READWRITE, &nold); // it is off by default
	*(DWORD*)0x52C9EE = vlods ? 0x1 : 0x0;
	VirtualProtect((LPVOID)0x52C9EE, 4, nold, NULL);
}

void _cdecl all(char* params) {
	fogdist("60");
	tideup("a"); // doesn't really matters with which params u calling this, it will never even use them
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
