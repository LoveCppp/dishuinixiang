// IATHOOK.cpp: implementation of the IATHOOK class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "IATHOOK.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

DWORD g_dwOldAddr;
DWORD g_dwNewAddr;
DWORD g_dwIATHookFlag;

int WINAPI MyMessageBox(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType){
	
	//定义MESSBOX的指针
	typedef int (WINAPI *PMESSAGEBOX)(HWND,LPCSTR,LPCSTR,UINT);
	//打印参数
	printf("参数: %x %s %s %x \n",hWnd,lpText,lpCaption,uType);
	
	//执行原来的messbox
	int ret = ((PMESSAGEBOX)g_dwOldAddr)(hWnd,lpText,lpCaption,uType);
	
	printf("返回值%d\n",ret);
	return ret;
}


BOOL SetIATHOOK(DWORD pOldFuncAddr,DWORD pNewFuncAddr){

	//pOldFuncAddr = pOldFuncAddr;
	
	bool bFlag = FALSE;
	DWORD dwImageBase = 0;
	PDWORD pFuncAddr = NULL;  		//局部的函数 用来与要被hook的函数地址对应
	PIMAGE_NT_HEADERS pNtHeader = NULL;  //nt标识
	PIMAGE_IMPORT_DESCRIPTOR pImportDescriptor = NULL; //导入表描述符
	
	//得到本进程模块地址 本进程就是一个module 返回的地址 就是本程序的ImageBase
	dwImageBase = (DWORD)::GetModuleHandle(NULL); //NULL  --- >本程序 
	pNtHeader = (PIMAGE_NT_HEADERS)(dwImageBase + ((PIMAGE_DOS_HEADER)dwImageBase)->e_lfanew);
    //找到IAT表

	pImportDescriptor = (PIMAGE_IMPORT_DESCRIPTOR)(dwImageBase + pNtHeader->OptionalHeader.DataDirectory[1].VirtualAddress); 
	//遍历IAT表 找要被hook的函数地址

	printf("要被hook函数的地址：%x \n",pOldFuncAddr);
    
	while (pImportDescriptor->FirstThunk !=0 && bFlag == false)
	{
		//获取IAT表函数的地址
		pFuncAddr = PDWORD(dwImageBase + pImportDescriptor->FirstThunk);//指向IAT的地址表
		
		while (*pFuncAddr)
		{
			
			if (pOldFuncAddr == *pFuncAddr) //如果找到要被hook的函数的地址
			{
				//找到以后替换成要修改的函数的地址
				*pFuncAddr = pNewFuncAddr;
				bFlag = true; //set hook Flag
				break;
			}
			pFuncAddr++; //一直找
		}
		
		pImportDescriptor++;
		
	}
	g_dwIATHookFlag = 1;
	g_dwOldAddr = pOldFuncAddr;
	g_dwNewAddr = pNewFuncAddr;
	
	return bFlag;

};



bool UnSetIATHook(){
	bool bFlag = true;
	DWORD dwImageBase = 0;
	PDWORD pFuncAddr = NULL;
	PIMAGE_NT_HEADERS pNtHeader = NULL;
	PIMAGE_IMPORT_DESCRIPTOR pImportDescriptor = NULL;
	
	//得到本进程模块地址
	dwImageBase = (DWORD)::GetModuleHandle(NULL); //NULL  --- >本程序
	pNtHeader = (PIMAGE_NT_HEADERS)(dwImageBase + ((PIMAGE_DOS_HEADER)dwImageBase)->e_lfanew);
	pImportDescriptor = (PIMAGE_IMPORT_DESCRIPTOR)(dwImageBase + pNtHeader->OptionalHeader.DataDirectory[1].VirtualAddress); //第二个目录项是导入表
	
	//遍历IAT表 找到原来的MessageBoxA
	//FirstThunk -> IAT
	//OrignalFirstThunk -> INT
	//printf("原函数地址：%x \n",g_dwOldAddr);
	while (pImportDescriptor->FirstThunk !=0 && bFlag == false)
	{

		pFuncAddr = PDWORD(dwImageBase + pImportDescriptor->FirstThunk);//指向IAT的地址表
		
		while (*pFuncAddr)
		{
			
			if (g_dwNewAddr == *pFuncAddr) //如果找到替换了的hook函数的地址
			{
				*pFuncAddr = g_dwOldAddr;  //恢复成原来函数的地址
				bFlag = FALSE;
				break;
			}
			pFuncAddr++;
		}
	
		pImportDescriptor++;
		
	}
	DWORD g_dwIATHookFlag = 0;
	
	
	return true;
}
