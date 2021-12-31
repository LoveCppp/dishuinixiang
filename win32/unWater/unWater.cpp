// unWater.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include <stdio.h>
#include "PeTools.h"
//修复重定位
VOID ModificationBaseRel(IN LPVOID ImageBuffer, DWORD newImageBase) {
	PIMAGE_DOS_HEADER pDosHeader = NULL; //DOs 头
	PIMAGE_NT_HEADERS pNTHeader = NULL; //NT头
	PIMAGE_FILE_HEADER pFileHeader = NULL; // 标准PE头
	PIMAGE_OPTIONAL_HEADER pOptionHerader = NULL; // 可选PE头
	PIMAGE_SECTION_HEADER pSectionHeader = NULL; // 节表
	PIMAGE_BASE_RELOCATION pBaseRelocation = NULL; //重定位表
	
	pDosHeader = (PIMAGE_DOS_HEADER)ImageBuffer;
	pNTHeader = (PIMAGE_NT_HEADERS)((DWORD)pDosHeader + pDosHeader->e_lfanew);
	pFileHeader = (PIMAGE_FILE_HEADER)((DWORD)pNTHeader + sizeof(DWORD));
	pOptionHerader = (PIMAGE_OPTIONAL_HEADER)((DWORD)pFileHeader + IMAGE_SIZEOF_FILE_HEADER);
	pSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)pOptionHerader + pFileHeader->SizeOfOptionalHeader);
	pBaseRelocation = (PIMAGE_BASE_RELOCATION)((DWORD)pDosHeader + pOptionHerader->DataDirectory[5].VirtualAddress);
	
	pOptionHerader->ImageBase = newImageBase;
	
	int index = 0;
	while (pBaseRelocation->VirtualAddress != 0) {
		
		int count = (pBaseRelocation->SizeOfBlock - 8) / 2;
		PWORD addr = (PWORD)((DWORD)pBaseRelocation + 8);
		
		for (int i = 0; i < count; i++) {
			DWORD height4 = addr[i] >> 12;
			if (height4 == 3) {
				DWORD low12 = addr[i] & 0x0fff;
				DWORD rva = pBaseRelocation->VirtualAddress + low12;
				PDWORD addr = (PDWORD)((DWORD)ImageBuffer + rva);
				*addr = *addr - pOptionHerader->ImageBase + newImageBase;
			}
		}
		index++;
		pBaseRelocation = (PIMAGE_BASE_RELOCATION)((DWORD)pBaseRelocation + pBaseRelocation->SizeOfBlock);
	}
        }


int main(int argc, char* argv[])
{	
	/*

	//获取当前文件路径
	char FilePath[255] = {0};
	GetModuleFileName(NULL, FilePath, 255);
	*/

	char FilePath[] = "c://project/ipmsgnew.exe";
	// 1、读取当前程序数据
	PVOID pFileBuffer = NULL;
	PVOID EnyBuffer = NULL;
	DWORD szizFilebuff = 0;
	
	LPSTR FILEPATH = FilePath;
	//获取当前程序
	ReadPeFile(FILEPATH,&pFileBuffer);
	
	//获取到加密程序节的数据
	DWORD EncyFileSize=0;
	EncyFileSize = GetSection(pFileBuffer,&EnyBuffer);
	//解密
	if (EncyFileSize == 0)
	{
		MessageBoxA(0,0,0,0);
		return 0;
	}
	//2、解密得到原来的PE文件
	XorDecodeAAA((char*)EnyBuffer,EncyFileSize);
	
	//解密完 filebuff -> imagebuff
	PVOID pImageBuffer = NULL;

	int ImageSize=	CopyFileBufferToImageBuffer(EnyBuffer,&pImageBuffer);
	
	//3、已挂起的方式创建进程(傀儡进程) 创建的进程就是壳子本身
	
	STARTUPINFO si = {0};
	PROCESS_INFORMATION pi;
	si.cb = sizeof(si);
	::CreateProcess(FilePath,NULL,NULL,NULL,NULL,CREATE_SUSPENDED, NULL,NULL,&si,&pi);
	printf("error is %d\n", GetLastError());
	

	//4、获取刚刚创建线程Context结构 并获取它的入口点 和 imgebase
	CONTEXT contx;
    contx.ContextFlags = CONTEXT_FULL;
    GetThreadContext(pi.hThread, &contx);
	
    //获取入口点                                
    DWORD dwEntryPoint = contx.Eax;  //当前程序的入口点
    //获取ImageBase                                  
    char* baseAddress = (CHAR *)contx.Ebx + 8; 
    DWORD imageBase = 0;
    SIZE_T byteSize = 0;
	//获取到壳进程的imagebase
    ReadProcessMemory(pi.hProcess, baseAddress, &imageBase, 4, &byteSize); 


	//5、 卸载外壳镜像代码 


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
	
	

	//6、在指定的位置分配空间：位置就是src的ImageBase  大小就是Src的SizeOfImage	
	
	//获取src imagebase  sizeofimage
    DWORD srcImageBase = 0;
    DWORD srcSizeOfImage = 0;
    DWORD srcOEP = 0;
    GetImageBase(pImageBuffer, &srcImageBase,&srcSizeOfImage,&srcOEP);
	
	LPVOID status = NULL;
    //在壳子进程空间中申请内存
    status = VirtualAllocEx(pi.hProcess,(LPVOID)srcImageBase, srcSizeOfImage,MEM_RESERVE | MEM_COMMIT,PAGE_EXECUTE_READWRITE);

	printf("VirtualAllocEx: %x\n",status);
	printf("error is %d\n", GetLastError());
	


	//判断是否申请成功,如果上一步内存没有申请到需要 判断这个src有没有重定位表,  有的话就在别的地方申请, 没有的话就解密失败					
	if(status != NULL){
		printf("7777777\n");
		//7、如果成功，将Src的PE文件拉伸 复制到该空间中
		
		//dwBufferImageBaseSrc 解密后imgbuff的image base 
		//pImageBufferSrc  拉伸后的imagebuffer
		//dwBufferLengthSrc 解密后的size
		printf("%x,%x,%x",srcImageBase,pImageBuffer);
		WriteProcessMemory(pi.hProcess, (LPVOID)srcImageBase, pImageBuffer, srcSizeOfImage, NULL);
		
	}else{
		if(isRelocation(pImageBuffer)){
			printf("有重定位表");
			//修复重定位表
			PIMAGE_BASE_RELOCATION pRelocationDirectory = NULL;
			DWORD pRelocationDirectoryVirtual = 0;
			
			DWORD NumberOfRelocation;
			PWORD Location;
			DWORD RVA_Data;
			WORD reloData;
			DWORD FOA;
			DWORD dwTempImageBaseSrc = srcImageBase + 0x50000;  //新的imagebase
			
			pRelocationDirectoryVirtual = GetRelocatio(EnyBuffer); //当前重定位表的虚拟地址
			printf("%x",pRelocationDirectoryVirtual);
			if(pRelocationDirectoryVirtual){
				FOA = RvaToFileOffset(EnyBuffer, pRelocationDirectoryVirtual);
				pRelocationDirectory = (PIMAGE_BASE_RELOCATION)((DWORD)EnyBuffer + FOA);
				//申请空间
				status = VirtualAllocEx(pi.hProcess, (LPVOID)dwTempImageBaseSrc,srcSizeOfImage,MEM_RESERVE | MEM_COMMIT,PAGE_EXECUTE_READWRITE);
				ChangesImageBase(EnyBuffer, dwTempImageBaseSrc);
				WriteProcessMemory(pi.hProcess, (LPVOID)dwTempImageBaseSrc, EnyBuffer, srcSizeOfImage, NULL);
				while(pRelocationDirectory->SizeOfBlock && pRelocationDirectory->VirtualAddress){				
					NumberOfRelocation = (pRelocationDirectory->SizeOfBlock - 8)/2;// 每个重定位块中的数据项的数量
					Location = (PWORD)((DWORD)pRelocationDirectory + 8); // 加上8个字节
					for(DWORD i=0;i<NumberOfRelocation;i++){
						if(Location[i] >> 12 != 0){ //判断是否是垃圾数据
							// WORD类型的变量进行接收
							reloData = (Location[i] & 0xFFF); //这里进行与操作 只取4字节 二进制的后12位
							RVA_Data = pRelocationDirectory->VirtualAddress + reloData; //这个是RVA的地址
							FOA = RvaToFileOffset(EnyBuffer,RVA_Data);
							//这里是自增的 进行修复重定位，上面的Imagebase我们改成了TempImageBase,那么改变的值就是 TempImageBase-dwBufferImageBaseSrc
							*(PDWORD)((DWORD)EnyBuffer+(DWORD)FOA) = *(PDWORD)((DWORD)EnyBuffer+(DWORD)FOA) + dwTempImageBaseSrc - srcSizeOfImage;	 // 任意位置 - Origin ImageBase			
						}
					}
					pRelocationDirectory = (PIMAGE_BASE_RELOCATION)((DWORD)pRelocationDirectory + (DWORD)pRelocationDirectory->SizeOfBlock); //上面的for循环完成之后，跳转到下个重定位块 继续如上的操作
				}
				
			srcImageBase = dwTempImageBaseSrc;
	

		}else{
		   printf("解壳申请内存失败");
		   return 0;
		}
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
	ExitProcess(0);
	//记得恢复线程
	return 0;



	}
}