// RemoteThreadInject.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <WINDOWS.H>



int main(int argc, char* argv[])
{

	//�Ȼ�ȡ���̾��
	TCHAR szTitle[MAX_PATH] = {0};							
	HWND hwnd = ::FindWindow(NULL,TEXT("������"));							

	if(hwnd != NULL)								
	{								
		
		/*
		  ����Զ���߳�ʱ����Ҫָ������ID/�̺߳���/�̺߳����Ĳ���												
		(2) �̺߳����ĵ�ַ��
		*/
		//(1) PID�Ļ�ȡ������	
		DWORD ProcessID;
		::GetWindowThreadProcessId(hwnd,&ProcessID);
	
		HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS,FALSE,ProcessID);  //����һ,ѡ������Ȩ��,������,���̳и�false,������,�����������õ�pid��ֵ
		if(NULL == hProcess)
		{
			printf("�򿪽���ʧ��\n");
			return 0;
		}
		char *DllName = "c:\\newdll.dll";
		int DllLength = strlen(DllName)+sizeof(char);    //��ȡDLL����
		//�ڼ����������ڴ�
		LPVOID pBuf = VirtualAllocEx(hProcess, NULL, DllLength, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
		if (!pBuf)
		{
			printf("�����ڴ�ʧ�ܣ�\n");
			return false;
		}
		//3.�ڼ�����д���ڴ�
		SIZE_T dwWrite = 0;
		if (!WriteProcessMemory(hProcess, pBuf, DllName, DllLength, &dwWrite))
		{
			printf("д���ڴ�ʧ�ܣ�\n");
			return false;
		}

	

		//4.����Զ���̣߳��öԷ�����LoadLibrary
		HANDLE hRemoteThread = CreateRemoteThread(hProcess, NULL, NULL,
			(LPTHREAD_START_ROUTINE)LoadLibrary, pBuf, 0, 0);

		
		//5.�ȴ��߳̽�������,�ͷ���Դ
		WaitForSingleObject(hRemoteThread, -1);
		//GetExitCodeThread()
		CloseHandle(hRemoteThread);
		//CloseHandle(hProcess);
		VirtualFreeEx(hProcess, pBuf, 0, MEM_FREE);
	
	}
	else								
	{								
		::MessageBox(NULL,TEXT("����û���ҵ�"),TEXT("[ERROR]"),MB_OK);							
	}								

	
	getchar();
	return 0;
}

