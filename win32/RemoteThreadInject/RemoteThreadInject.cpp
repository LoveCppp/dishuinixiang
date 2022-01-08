// RemoteThreadInject.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <WINDOWS.H>



int main(int argc, char* argv[])
{

	//先获取进程句柄
	TCHAR szTitle[MAX_PATH] = {0};							
	HWND hwnd = ::FindWindow(NULL,TEXT("计算器"));							

	if(hwnd != NULL)								
	{								
		
		/*
		  创建远程线程时，需要指定进程ID/线程函数/线程函数的参数												
		(2) 线程函数的地址？
		*/
		//(1) PID的获取。。略	
		DWORD ProcessID;
		::GetWindowThreadProcessId(hwnd,&ProcessID);
	
		HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS,FALSE,ProcessID);  //参数一,选择所有权限,参数二,不继承给false,参数三,给我们上面获得的pid的值
		if(NULL == hProcess)
		{
			printf("打开进程失败\n");
			return 0;
		}
		char *DllName = "c:\\newdll.dll";
		int DllLength = strlen(DllName)+sizeof(char);    //获取DLL长度
		//在计算器申请内存
		LPVOID pBuf = VirtualAllocEx(hProcess, NULL, DllLength, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
		if (!pBuf)
		{
			printf("申请内存失败！\n");
			return false;
		}
		//3.在计算器写入内存
		SIZE_T dwWrite = 0;
		if (!WriteProcessMemory(hProcess, pBuf, DllName, DllLength, &dwWrite))
		{
			printf("写入内存失败！\n");
			return false;
		}

	

		//4.创建远程线程，让对方调用LoadLibrary
		HANDLE hRemoteThread = CreateRemoteThread(hProcess, NULL, NULL,
			(LPTHREAD_START_ROUTINE)LoadLibrary, pBuf, 0, 0);

		
		//5.等待线程结束返回,释放资源
		WaitForSingleObject(hRemoteThread, -1);
		//GetExitCodeThread()
		CloseHandle(hRemoteThread);
		//CloseHandle(hProcess);
		VirtualFreeEx(hProcess, pBuf, 0, MEM_FREE);
	
	}
	else								
	{								
		::MessageBox(NULL,TEXT("窗口没有找到"),TEXT("[ERROR]"),MB_OK);							
	}								

	
	getchar();
	return 0;
}

