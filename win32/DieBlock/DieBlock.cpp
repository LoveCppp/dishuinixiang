// DieBlock.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "stdafx.h"		
#include <windows.h>		
		
CRITICAL_SECTION g_csA; 
CRITICAL_SECTION g_csB; 



DWORD WINAPI ThreadProc1(LPVOID lpParameter)		
{		
	for(int x=0;x<1000;x++)	
	{	
		EnterCriticalSection(&g_csA);
		

		EnterCriticalSection(&g_csB);
		
	
		
		printf("22221112:\n");
		
		LeaveCriticalSection(&g_csB);
		
		printf("11111:\n");
		
		LeaveCriticalSection(&g_csA);
		
	}	
	return 0;	
}		

DWORD WINAPI ThreadProc2(LPVOID lpParameter)		
{		
	for(int x=0;x<1000;x++)	
	{	
		EnterCriticalSection(&g_csB);
		
	
		EnterCriticalSection(&g_csA);
		printf("22222:\n");
		LeaveCriticalSection(&g_csB);
		
		LeaveCriticalSection(&g_csB);
		
	}	
	
	return 0;	
}		




int main(int argc, char* argv[])
{


	InitializeCriticalSection(&g_csA);	

	InitializeCriticalSection(&g_csB);	
	
	HANDLE hThread1 = ::CreateThread(NULL, 0, ThreadProc1, 	
		NULL, 0, NULL);
	
	//创建一个新的线程	
	HANDLE hThread2 = ::CreateThread(NULL, 0, ThreadProc2, 	
		NULL, 0, NULL);

	


	printf("Hello World!\n");
	getchar();
	return 0;
}

