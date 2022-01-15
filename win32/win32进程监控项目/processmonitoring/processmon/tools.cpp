// tools.cpp: implementation of the tools class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "processmon.h"
#include "tools.h"
#include "PeTools.h"
#include "InlineHOOK.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


DWORD pOldMessageBoxAddr;
DWORD nweMessageBoxAddr;

DWORD WINAPI InjectEntry(LPVOID lpParam)
{
	//RepairIAT(lpParam);
	//定义PE头的信息
	PIMAGE_DOS_HEADER pDosHeader = NULL;
	PIMAGE_NT_HEADERS pNTHeader = NULL;
	PIMAGE_FILE_HEADER pPEHeader = NULL;
	PIMAGE_OPTIONAL_HEADER32 pOptionHeader = NULL;
	PIMAGE_SECTION_HEADER pSectionHeader = NULL;
	
	if(!lpParam)
	{
		printf("读取到内存的pfilebuffer无效！\n");
		return 0;
	}
	//判断是不是exe文件
	if(*((PWORD)lpParam) != IMAGE_DOS_SIGNATURE)
	{
		printf("不含MZ标志，不是exe文件！\n");
		return 0;
	}
	pDosHeader = (PIMAGE_DOS_HEADER)lpParam;
	if(*((PDWORD)((BYTE *)lpParam + pDosHeader->e_lfanew)) != IMAGE_NT_SIGNATURE){
		printf("无有效的PE标志\n");
		return 0;
	}
	
	//读取pFileBuffer 获取DOS头，PE头，节表等信息
	pDosHeader =(PIMAGE_DOS_HEADER)lpParam;
	pNTHeader = (PIMAGE_NT_HEADERS)((DWORD)lpParam + pDosHeader->e_lfanew);
	//打印NT头	
	pPEHeader = (PIMAGE_FILE_HEADER)(((DWORD)pNTHeader) + 4);  //加4个字节到了标准PE头
	pOptionHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPEHeader + IMAGE_SIZEOF_FILE_HEADER); //标准PE头+标准PE头的大小 20
	
	
	//1000
	
	
	PIMAGE_IMPORT_DESCRIPTOR ImportExtable = (PIMAGE_IMPORT_DESCRIPTOR)((char*)lpParam + pOptionHeader->DataDirectory[1].VirtualAddress);	
	

	while(ImportExtable->OriginalFirstThunk !=0 && ImportExtable->FirstThunk !=0){
		
		char *DllName =(char*)lpParam + ImportExtable->Name;
		
	
		
		//这个时候的INT表已经找不到函数了,要先遍历INT表找到序号和名称，然后利用LoadLibrary获取函数的地址然后修复
		
		DWORD* pThunkData_INT = (DWORD*)((char*)lpParam + ImportExtable->OriginalFirstThunk);
		DWORD*	pThunkData_IAT = (PDWORD)(((PBYTE)lpParam + ImportExtable->FirstThunk));
		while(*pThunkData_INT){
			
			if (*pThunkData_INT & 0x80000000)
			{
				//DbgPrintf("按序号导入序号%x\n",(*pThunkData_INT & 0xFFFF));
				(*pThunkData_IAT) = (DWORD)GetProcAddress(LoadLibrary(DllName),(char*)(*pThunkData_INT & 0xFFFF));
				printf("--%x\n",*pThunkData_IAT);
			}else{
				
				PIMAGE_IMPORT_BY_NAME FirstThunkNames = (_IMAGE_IMPORT_BY_NAME*)((char*)lpParam + *pThunkData_INT);
			//	DbgPrintf("按名称导入HIT/NAME-%x-%s\n",FirstThunkNames->Hint,FirstThunkNames->Name);
				(*pThunkData_IAT) = (DWORD)GetProcAddress(LoadLibrary(DllName),(char*)FirstThunkNames->Name);
			//	DbgPrintf("--%x\n",*pThunkData_IAT);
			}	
			
			pThunkData_IAT++,pThunkData_INT++;
		}
		
		ImportExtable = (PIMAGE_IMPORT_DESCRIPTOR)((DWORD)ImportExtable+sizeof(IMAGE_IMPORT_DESCRIPTOR));
		
	}

	//创建共享内存
	HANDLE tohFileMap = NULL;
	LPVOID lpHackerBuffer;
	
	tohFileMap = CreateFileMapping(INVALID_HANDLE_VALUE,NULL,PAGE_READWRITE,0,0x1000,"Hacker1");
	lpHackerBuffer = MapViewOfFile(tohFileMap,FILE_MAP_ALL_ACCESS,0,0,0x1000);
	PDWORD lpReadBuffer = (PDWORD)lpHackerBuffer;
	
	MessageBox(NULL,"内存注入成功","内存注入成功",0);

	
	//替换的值
	DWORD nweMessageBoxAddr=NULL;
	DWORD pOldMessageBoxAddr=NULL;
	while(TRUE){
		
		switch (lpReadBuffer[0])
		{
			case 1:
				{
					//获取当前mesbox的值
					pOldMessageBoxAddr = (DWORD)GetProcAddress(LoadLibrary("user32.dll"),"MessageBoxA");
				
					if(lpReadBuffer[1]==1)
					{
						MessageBox(NULL,"Message监控开启","MessageBOX监控开启",0);
						SetMESSAGEBOXInlineHook(pOldMessageBoxAddr,(DWORD)MessageBoxHOOK,5,&nweMessageBoxAddr);
						lpReadBuffer[0] =0;
					}else{
					
						if(UnInlineHook(nweMessageBoxAddr,5)){
							lpReadBuffer[0] =0;
							MessageBox(NULL,"Message监控关闭成功","Message监控关闭成功",0);
						}else{
							lpReadBuffer[0] =0;
							MessageBox(NULL,"Message监控关闭失败","Message监控关闭失败",0);
						}
						
					}			
				
					break;

				}
		}
		Sleep(1000);
	}


	return 0;
}



BOOL EnableDebugPrivilege()
{
	HANDLE hToken;
	BOOL fOk=FALSE;
	if(OpenProcessToken(GetCurrentProcess(),TOKEN_ADJUST_PRIVILEGES,&hToken))
	{
		TOKEN_PRIVILEGES tp;
		tp.PrivilegeCount=1;
		LookupPrivilegeValue(NULL,SE_DEBUG_NAME,&tp.Privileges[0].Luid);
		
		tp.Privileges[0].Attributes=SE_PRIVILEGE_ENABLED;
		AdjustTokenPrivileges(hToken,FALSE,&tp,sizeof(tp),NULL,NULL);
		
		fOk=(GetLastError()==ERROR_SUCCESS);
		CloseHandle(hToken);
	}
    return fOk;
}

BOOL MemoryInject(DWORD Pid)
{
	HMODULE pImageBuffer = GetModuleHandle(NULL);
	PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)pImageBuffer;
	PIMAGE_NT_HEADERS pNTHeader = (PIMAGE_NT_HEADERS)((DWORD)pDosHeader + pDosHeader->e_lfanew);
	PIMAGE_FILE_HEADER pPEHeader = (PIMAGE_FILE_HEADER)((DWORD)pDosHeader + pDosHeader->e_lfanew + 4);
	PIMAGE_OPTIONAL_HEADER32 pOptionHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPEHeader + sizeof(IMAGE_FILE_HEADER));
	
	//获取自身sizeofimage
	DWORD sizeOfImage = pOptionHeader->SizeOfImage;
	
	//3、在当前空间申请空间存放自身代码					
	
	LPVOID pNewImagebuffer = malloc(sizeOfImage);
	
	//4、拷贝自身到缓存					
	memset(pNewImagebuffer, 0, sizeOfImage);
	memcpy(pNewImagebuffer, pImageBuffer, sizeOfImage);

	
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS,FALSE,Pid);  //参数一,选择所有权限,参数二,不继承给false,参数三,给我们上面获得的pid的值
	if(NULL == hProcess)
	{
		printf("打开进程失败\n");
		return 0;
	}
	
	//6、在远程进程申请空间					
	
	
	LPVOID pImageBase = VirtualAllocEx(hProcess, NULL, sizeOfImage, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	if (!pImageBase)
	{
		printf("申请内存失败！\n");
		return false;
	}
	
	
	//7、对模块中的代码进行重定位					
	RestoreRelocation(pNewImagebuffer,(DWORD)pImageBase);
	
	//8、得到模块中要运行的函数的地址					
	
	//9、将模块在进程中的地址作为参数传递给入口函数	
	
	//10、将修正后的模块 通过WriteProcessMemory写入远程进程的内存空间中							
	DWORD byteWritten = 0;
	BOOL Write = WriteProcessMemory(hProcess, pImageBase, pNewImagebuffer, sizeOfImage, &byteWritten);
	if (!Write)
	{
		printf("can not write process memory!");
		return FALSE;
	}
	
	DWORD dwProcOffset = (DWORD)InjectEntry - (DWORD)pImageBuffer + (DWORD)pImageBase;	
	//11、通过CreateRemoteThread启动刚写入的代码							
	
	//创建行程线程,执行相应函数.
	HANDLE pThread = CreateRemoteThread(hProcess, NULL, NULL, (LPTHREAD_START_ROUTINE)dwProcOffset, pImageBase, NULL, NULL);
	WaitForSingleObject(pThread, -1);
	
	//12、释放内存							
	
	return TRUE;

}