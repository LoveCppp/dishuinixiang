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
		printf("ע��DLLʧ��\n");
		return -1;
	}
	else
	{
		printf("ע��DLL�ɹ�\n");
	}

	HMODULE hModule = LoadLibraryA("mydll.dll");
	if (hModule == NULL)
	{
		printf("��ȡDLL���ʧ��\n");
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
		printf("����Ҫ���͵�����: ");
		ZeroMemory(szBuffer, 0x1000);
		scanf("%s", szBuffer);
		pFnSetData(szBuffer, strlen(szBuffer));
		//pFnGetData(szBuffer);
		printf("�޸����ݳɹ�����ǰ����: %s\n", szBuffer);
		if (strcmp(szBuffer, "quit") == 0) break;
	}
	
	return 0;
}
				
							

BOOL InjectDLL()
	{
		// ��Ȩ(win10)
		EnableDebugPrivilege();
		// ���ݴ�������ȡ���̾��
		HWND hWnd = ::FindWindow(NULL,TEXT("�ɸ봫��  IP Messenger  (VV �����)"));	
		if (hWnd == NULL)
		{
			printf("��ȡ���ھ��ʧ��\n");
			return FALSE;
		}
		DWORD dwPid = -1;
		GetWindowThreadProcessId(hWnd, &dwPid);
		HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPid);
		if (hProcess == INVALID_HANDLE_VALUE)
		{
			printf("�򿪽���ʧ��\n");
			return FALSE;
		}
		// ��Ҫע��Ľ���������һ���ڴ棬��ΪLoadLibrary�Ĳ���
		char szDllName[MAX_PATH] = "mydll.dll";
		LPVOID pAddress = VirtualAllocEx(hProcess, NULL, MAX_PATH, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
		WriteProcessMemory(hProcess, pAddress, szDllName, strlen(szDllName), NULL);
		// ����Զ���̣߳��߳��������ΪLoadLibrary�������Ϳ����Զ�����dll
		HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)LoadLibraryA, pAddress, 0, NULL);	
		VirtualFreeEx(hProcess, pAddress, 0, MEM_RELEASE);
		CloseHandle(hProcess);
		return TRUE;
	}


// ��Ȩ����������ΪDEBUGȨ��
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
