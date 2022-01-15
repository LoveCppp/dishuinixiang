// CLIENT.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <WINDOWS.H>

int main(int argc, char* argv[])
{


	HANDLE hMapObject;					
	HANDLE hMapView;					
	
	//创建FileMapping对象					
	hMapObject = CreateFileMapping((HANDLE)0xFFFFFFFF,NULL,PAGE_READWRITE,0,0x1000,TEXT("shared"));					
	if(!hMapObject)					
	{					
		MessageBox(NULL,TEXT("共享内存失败"),TEXT("Error"),MB_OK);				
		return FALSE;				
	}					
	//将FileMapping对象映射到自己的进程					
	hMapView = MapViewOfFile(hMapObject,FILE_MAP_WRITE,0,0,0);					
	if(!hMapView)					
	{					
		MessageBox(NULL,TEXT("内存映射失败"),TEXT("Error"),MB_OK);				
		return FALSE;				
	}					
	//向共享内存写入数据					
	strcpy((char*)hMapView,"Test Shared Memeryaaaa");					
	
	getchar();

	return 0;
}

