// Client.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <windows.h>
int main(int argc, char* argv[])
{

	HANDLE hRead = GetStdHandle(STD_INPUT_HANDLE);								
	HANDLE hWrite = GetStdHandle(STD_OUTPUT_HANDLE);  								
	
	//������								
	TCHAR szBuffer[100];								
	DWORD dwRead;								
	if(!ReadFile(hRead,szBuffer,100,&dwRead,NULL))								
	{								
		MessageBox(NULL,TEXT("��ȡ����ʧ��!"),TEXT("Error"),MB_OK);							
	}								
	else								
	{								
		MessageBox(NULL,szBuffer,TEXT("[��ȡ����]"),MB_OK);							
	}								
	
	//д����								
	TCHAR szsBuffer[100] = "�����ܵ�";								
	DWORD dwWrite;								
	if(!WriteFile(hWrite,szsBuffer,strlen(szsBuffer)+1,&dwWrite,NULL))								
	{								
		MessageBox(NULL,TEXT("д������ʧ��!"),TEXT("Error"),MB_OK);							
	}								


	return 0;
}

