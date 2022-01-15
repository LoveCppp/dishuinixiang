// Server.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <WINDOWS.H>


int main(int argc, char* argv[])
{
	HANDLE hRead;										
	HANDLE hWrite;										
	
	SECURITY_ATTRIBUTES sa;										
	
	sa.bInheritHandle = TRUE;										
	sa.lpSecurityDescriptor = NULL;										
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);										
	
	if(!CreatePipe(&hRead,&hWrite,&sa,0))										
	{										
		MessageBox(0,TEXT("创建匿名管道失败!"),TEXT("Error"),MB_OK);									
	}										
	
	STARTUPINFO si;										
	PROCESS_INFORMATION pi;										
	
	ZeroMemory(&si,sizeof(STARTUPINFO));										
	
	si.cb = sizeof(STARTUPINFO);										
	si.dwFlags = STARTF_USESTDHANDLES;										
	si.hStdInput = hRead;										
	si.hStdOutput = hWrite;										
	si.hStdError = GetStdHandle(STD_ERROR_HANDLE);										
	
	if(!CreateProcess("C://project//进程通信//管道//匿名管道//Server//Debug//Client.exe",NULL,NULL,NULL,TRUE,0,NULL,NULL,&si,&pi))										
	{										
		CloseHandle(hRead);									
		CloseHandle(hWrite);									
		hRead = NULL;									
		hWrite = NULL;									
		MessageBox(0,TEXT("创建子进程失败!"),TEXT("Error"),MB_OK);									
	}										
	else										
	{										
		CloseHandle(pi.hProcess);									
		CloseHandle(pi.hThread);									
	}										
	
	
	
	//写数据										
	TCHAR szBuffer[] = "http:\\www.dtdebug.com";										
	DWORD dwWrite;										
	if(!WriteFile(hWrite,szBuffer,strlen(szBuffer)+1,&dwWrite,NULL))										
	{										
		MessageBox(0,TEXT("写数据失败!"),TEXT("Error"),MB_OK);									
	}										
	
											
	DWORD dwRead;										
	if(!ReadFile(hRead,szBuffer,100,&dwRead,NULL))										
	{										
		MessageBox(NULL,TEXT("读取数据失败!"),TEXT("Error"),MB_OK);									
	}										
	else										
	{										
		MessageBox(NULL,szBuffer,TEXT("[读取数据]"),MB_OK);									
	}										
	
	return 0;
}

