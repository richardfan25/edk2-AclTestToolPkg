#include <Windows.h>

#include "ioport.h"

typedef BOOL (WINAPI * DLL_InitializeWinIo)();
DLL_InitializeWinIo InitializeWinIo;
typedef BOOL (WINAPI * DLL_RemoveWinIoDriver)();
DLL_RemoveWinIoDriver RemoveWinIoDriver;
typedef void (WINAPI * DLL_ShutdownWinIo)();
DLL_ShutdownWinIo ShutdownWinIo;
typedef BOOL (WINAPI * DLL_GetPortVal)(WORD wPortAddr, PDWORD pdwPortVal, BYTE bSize);
DLL_GetPortVal GetPortVal;
typedef BOOL (WINAPI * DLL_SetPortVal)(WORD wPortAddr, DWORD dwPortVal, BYTE bSize);
DLL_SetPortVal SetPortVal;

static HINSTANCE hDLL;

int32_t init_winio()
{
	int ret = 0;

#if defined(_WIN64)
	hDLL = GetModuleHandle(L"WinIo64.dll");
	if(hDLL == NULL)
		hDLL = LoadLibrary(L"WinIo64.dll");
#elif defined(_WIN32)
	hDLL = GetModuleHandle(L"WinIo32.dll");
	if(hDLL == NULL)
		hDLL = LoadLibrary(L"WinIo32.dll");
#endif

	if (hDLL == NULL)
	{
		printf("Error load winio.dll fail. %08X\n", GetLastError());
		return -1;
	}

	InitializeWinIo = (DLL_InitializeWinIo)GetProcAddress(hDLL, "InitializeWinIo");
	if (!InitializeWinIo)
	{
		printf("Error InitializeWinIo fail. %08X\n", GetLastError());
		ret = -2;
		goto exit;
	}

	RemoveWinIoDriver = (DLL_RemoveWinIoDriver)GetProcAddress(hDLL, "RemoveWinIoDriver");
	if (!RemoveWinIoDriver)
	{
		printf("Error RemoveWinIoDriver fail. %08X\n", GetLastError());
		ret = -3;
		goto exit;
	}

	ShutdownWinIo = (DLL_ShutdownWinIo)GetProcAddress(hDLL, "ShutdownWinIo");
	if (!ShutdownWinIo)
	{
		printf("Error ShutdownWinIo fail. %08X\n", GetLastError());
		ret = -4;
		goto exit;
	}

	GetPortVal = (DLL_GetPortVal)GetProcAddress(hDLL, "GetPortVal");
	if (!GetPortVal)
	{
		printf("Error GetPortVal fail. %08X\n", GetLastError());
		ret = -5;
		goto exit;
	}

	SetPortVal = (DLL_SetPortVal)GetProcAddress(hDLL, "SetPortVal");
	if (!SetPortVal)
	{
		printf("Error SetPortVal fail. %08X\n", GetLastError());
		ret = -6;
		goto exit;
	}

	if (!InitializeWinIo()) 
	{
		printf("Error during initialization of WinIo. %08X\n", GetLastError());
		ret = -7;
		goto exit;
	}

	return ret;

exit:
	FreeLibrary(hDLL);
	return ret;
}

void deinit_winio()
{
	RemoveWinIoDriver();
	ShutdownWinIo();
	FreeLibrary(hDLL); 
}

uint8_t inp_winio(uint16_t port)
{
	DWORD dwTemp = 0;
	GetPortVal(port, &dwTemp, 1);

	return (uint8_t)dwTemp;
}

uint16_t inpw_winio(uint16_t port)
{
	DWORD dwTemp = 0;
	GetPortVal(port, &dwTemp, 2);

	return (uint16_t)dwTemp;
}

uint32_t inpd_winio(uint16_t port)
{
	DWORD dwTemp = 0;
	GetPortVal(port, &dwTemp, 4);

	return (uint32_t)dwTemp;
}

void outp_winio(uint16_t port, uint8_t val)
{
	SetPortVal(port, val, 1);
}

void outpw_winio(uint16_t port, uint16_t val)
{
	SetPortVal(port, val, 2);
}

void outpd_winio(uint16_t port, uint32_t val)
{
	SetPortVal(port, val, 4);
}
