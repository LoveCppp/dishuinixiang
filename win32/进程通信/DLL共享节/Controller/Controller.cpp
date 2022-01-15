// Controller.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <stdio.h>
 
BOOL InjectDLL();
BOOL EnableDebugPrivilege();

int main()
{
	if (FALSE == InjectDLL())
	{
		printf("注入DLL失败\n");
		return -1;
	}
	else
	{
		printf("注入DLL成功\n");
	}

	HMODULE hModule = LoadLibraryA("mydll.dll");
	if (hModule == NULL)
	{
		printf("获取DLL句柄失败\n");
		return -1; `
	}
	typedef void (*PFNSETDATA)(char *, DWORD);
	typedef void (*PFNGETDATA)(char *);
	PFNSETDATA pFnSetData = (PFNSETDATA)GetProcAddress(hModule, "SetData");
	PFNGETDATA pFnGetData = (PFNGETDATA)GetProcAddress(hModule, "GetData");
	char szBuffer[0x1000];	

	while (1)
	{
		BOOL Flag=TRUE;
		printf("输入要发送的数据: ");
		ZeroMemory(szBuffer, 0x1000);
		scanf("%s", szBuffer);
		pFnSetData(szBuffer, strlen(szBuffer));
		//pFnGetData(szBuffer);
		printf("修改数据成功，当前数据: %s\n", szBuffer);
		if (strcmp(szBuffer, "quit") == 0) break;
	}
	
	return 0;
}
				
							

BOOL InjectDLL()
	{
		// 提权(win10)
		EnableDebugPrivilege();
		// 根据窗口名获取进程句柄
		HWND hWnd = ::FindWindow(NULL,TEXT("飞鸽传书  IP Messenger  (VV 纪念版)"));	
		if (hWnd == NULL)
		{
			printf("获取窗口句柄失败\n");
			return FALSE;
		}
		DWORD dwPid = -1;
		GetWindowThreadProcessId(hWnd, &dwPid);
		HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPid);
		if (hProcess == INVALID_HANDLE_VALUE)
		{
			printf("打开进程失败\n");
			return FALSE;
		}
		// 在要注入的进程中申请一块内存，作为LoadLibrary的参数
		char szDllName[MAX_PATH] = "mydll.dll";
		LPVOID pAddress = VirtualAllocEx(hProcess, NULL, MAX_PATH, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
		WriteProcessMemory(hProcess, pAddress, szDllName, strlen(szDllName), NULL);
		// 创建远程线程，线程入口设置为LoadLibrary，这样就可以自动加载dll
		HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)LoadLibraryA, pAddress, 0, NULL);	
		VirtualFreeEx(hProcess, pAddress, 0, MEM_RELEASE);
		CloseHandle(hProcess);
		return TRUE;
	}


// 提权函数：提升为DEBUG权限
BOOL EnableDebugPrivilege()
{
	HANDLE hToken;
	BOOL fOk = FALSE;
	if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken))
	{
		TOKEN_PRIVILEGES tp;
		tp.PrivilegeCount = 1;
		LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &tp.Privileges[0].Luid);
		
		tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
		AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(tp), NULL, NULL);
		
		fOk = (GetLastError() == ERROR_SUCCESS);
		CloseHandle(hToken);
	}
	return fOk;
}
