// Client.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <windows.h>
int main(int argc, char* argv[])
{

	HANDLE hRead = GetStdHandle(STD_INPUT_HANDLE);								
	HANDLE hWrite = GetStdHandle(STD_OUTPUT_HANDLE);  								
	
	//读数据								
	TCHAR szBuffer[100];								
	DWORD dwRead;								
	if(!ReadFile(hRead,szBuffer,100,&dwRead,NULL))								
	{								
		MessageBox(NULL,TEXT("读取数据失败!"),TEXT("Error"),MB_OK);							
	}								
	else								
	{								
		MessageBox(NULL,szBuffer,TEXT("[读取数据]"),MB_OK);							
	}								
	
	//写数据								
	TCHAR szsBuffer[100] = "匿名管道";								
	DWORD dwWrite;								
	if(!WriteFile(hWrite,szsBuffer,strlen(szsBuffer)+1,&dwWrite,NULL))								
	{								
		MessageBox(NULL,TEXT("写入数据失败!"),TEXT("Error"),MB_OK);							
	}								


	return 0;
}

