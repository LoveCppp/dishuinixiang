// LodePeExecute.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "PeTools.h"
int main(int argc, char* argv[])
{
	//要加载的文件地址
	char FilePath[] = "c://project/notepad.exe";

	PVOID pFileBuffer = NULL;
	LPSTR FILEPATH = FilePath;
	//获取当前程序
	ReadPeFile(FILEPATH,&pFileBuffer);
	//拉伸
	PVOID pImageBuffer = NULL;
	int ImageSize=	CopyFileBufferToImageBuffer(pFileBuffer,&pImageBuffer);
	
	//挂起线程
	STARTUPINFO si = {0};
	PROCESS_INFORMATION pi;
	si.cb = sizeof(si);
	::CreateProcess(FilePath,NULL,NULL,NULL,NULL,CREATE_SUSPENDED, NULL,NULL,&si,&pi);
	printf("error is %d\n", GetLastError());
	
	//获取刚刚创建线程Context结构 并获取它的入口点 和 imgebase
	CONTEXT contx;
    contx.ContextFlags = CONTEXT_FULL;
    GetThreadContext(pi.hThread, &contx);
	
    //获取入口点                                
    DWORD dwEntryPoint = contx.Eax;  //当前程序的入口点
    //获取ImageBase                                  
    char* baseAddress = (CHAR *)contx.Ebx + 8; 
    DWORD imageBase = 0;
    SIZE_T byteSize = 0;
	//获取helloword进程的imagebase
    ReadProcessMemory(pi.hProcess, baseAddress, &imageBase, 4, &byteSize); 
	

	//卸载进程
	typedef long NTSTATUS;
	typedef NTSTATUS(__stdcall *pfnZwUnmapViewOfSection)(
		IN HANDLE ProcessHandle,
		IN LPVOID BaseAddress
		);
	
	pfnZwUnmapViewOfSection ZwUnmapViewOfSection;
	ZwUnmapViewOfSection = (pfnZwUnmapViewOfSection)GetProcAddress(
        GetModuleHandleA("ntdll.dll"), "ZwUnmapViewOfSection");
    if (!ZwUnmapViewOfSection)
    {
        ::TerminateThread(pi.hThread, 2);
        ::WaitForSingleObject(pi.hThread, INFINITE);
        return 0;
    }
    DWORD a = 0;
    a = ZwUnmapViewOfSection(pi.hProcess, (PVOID)imageBase);
	//2、读取A.exe 拉伸 复制到0x400000	
	
	//获取要加载程序的imagebase地址

	//获取src imagebase  sizeofimage
    DWORD srcImageBase = 0;
    DWORD srcSizeOfImage = 0;
    DWORD srcOEP = 0;
    GetImageBase(pImageBuffer, &srcImageBase,&srcSizeOfImage,&srcOEP);
	


	//把a.exe复制到现在的空间

	LPVOID status = NULL;
    //当前进程空间中申请内存
    status = VirtualAllocEx(pi.hProcess,(LPVOID)srcImageBase, srcSizeOfImage,MEM_RESERVE | MEM_COMMIT,PAGE_EXECUTE_READWRITE);
	
	printf("VirtualAllocEx: %x\n",status);
	printf("error is %d\n", GetLastError());

	
	//判断是否申请成功,如果上一步内存没有申请到需要 判断这个src有没有重定位表,  有的话就在别的地方申请, 没有的话就解密失败					
	if(status != NULL){
	
		//把a.exe复制到内存
		WriteProcessMemory(pi.hProcess, (LPVOID)srcImageBase, pImageBuffer, srcSizeOfImage, NULL);
		
		//修复IAT表
		
		PrintImportTable(pImageBuffer);

	}


	
	// 10、修改外壳程序的Context:
	CONTEXT cont;
	cont.ContextFlags = CONTEXT_FULL; 
	::GetThreadContext(pi.hThread, &cont);
	cont.Eax = srcOEP + srcImageBase; // imagebase + op
	
	DWORD theOep = cont.Ebx + 8;
	DWORD dwBytes=0;
	WriteProcessMemory(pi.hProcess, &theOep, &srcImageBase,4, &dwBytes);
	
    SetThreadContext(pi.hThread, &cont);
	//记得恢复线程
    ResumeThread(pi.hThread);

	getchar();
	ExitProcess(0);
	//记得恢复线程

	return 0;
	
}

