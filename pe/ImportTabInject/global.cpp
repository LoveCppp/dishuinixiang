// global.cpp: implementation of the global class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "global.h"
#include "stdafx.h"
#include <malloc.h>
#include <math.h>
#include <string>
#include <stdlib.h>
#include <stdio.h>
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


//读取文件函数

DWORD ReadPeFile(IN LPSTR lpszFile,OUT LPVOID* pFileBuffer)
{
	
	FILE* pFile = NULL;
	DWORD fileSize = 0;
	LPVOID pFileTempBuffer = NULL;
	//打开文件
	pFile = fopen(lpszFile, "rb");
	if (!pFile)
	{
		printf(" 无法打开 EXE 文件! ");
		return 0;
	}
	//读取文件大小
	
	fseek(pFile, 0, SEEK_END);
	fileSize = ftell(pFile);
	fseek(pFile, 0, SEEK_SET);
	//分配缓冲区	
	pFileTempBuffer = malloc(fileSize);
	
	
	if (!pFileTempBuffer)
	{
		printf(" 分配空间失败! ");
		fclose(pFile); //然后需要关闭文件
		return NULL;
	}
	
	size_t n = fread(pFileTempBuffer, fileSize, 1, pFile);
	if (!n)
	{
		printf(" 读取数据失败! ");
		free(pFileTempBuffer);
		fclose(pFile);
		return NULL;
	}
	
	*pFileBuffer = pFileTempBuffer;
	pFileTempBuffer = NULL;
	//关闭文件	
	fclose(pFile);
	
	return fileSize;
};




//导入表注入
DWORD InjectDll(LPVOID pFilebuff){
	//定义PE头的信息
	PIMAGE_DOS_HEADER pDosHeader = NULL;
	PIMAGE_NT_HEADERS pNTHeader = NULL;
	PIMAGE_FILE_HEADER pPEHeader = NULL;
	PIMAGE_OPTIONAL_HEADER32 pOptionHeader = NULL;
	PIMAGE_SECTION_HEADER pSectionHeader = NULL;
	LPVOID pTempBuffer = NULL;
	bool Ssize = false;
	if(!pFilebuff)
	{
		printf("读取到内存的pfilebuffer无效！\n");
		return 0;
	}
	//判断是不是exe文件
	if(*((PWORD)pFilebuff) != IMAGE_DOS_SIGNATURE)
	{
		printf("不含MZ标志，不是exe文件！\n");
		return 0;
	}
	pDosHeader = (PIMAGE_DOS_HEADER)pFilebuff;
	if(*((PDWORD)((BYTE *)pFilebuff + pDosHeader->e_lfanew)) != IMAGE_NT_SIGNATURE){
		printf("无有效的PE标志\n");
		return 0;
	}
	
	//读取pFileBuffer 获取DOS头，PE头，节表等信息
	pDosHeader =(PIMAGE_DOS_HEADER)pFilebuff;
	pNTHeader = (PIMAGE_NT_HEADERS)((DWORD)pFilebuff + pDosHeader->e_lfanew);
	//打印NT头	
	pPEHeader = (PIMAGE_FILE_HEADER)(((DWORD)pNTHeader) + 4);  //加4个字节到了标准PE头
	pOptionHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPEHeader + IMAGE_SIZEOF_FILE_HEADER); //标准PE头+标准PE头的大小 20
	pSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)pOptionHeader + pPEHeader->SizeOfOptionalHeader);

	//移动导入表
	
	//获取剩余大小 //SizeOfHeaders(总的大小)4096 -   ( pSectionHeader(一个地址，文件开始的地方偏移了pSectionHeader的大小的字节)  -    (文件偏移+pPEHeader->NumberOfSections * 40)  一个地址 文件开始的地方偏移了节的大小的字节 并不是节在文件中的偏移地址，所以会比 pSectionHeader小)
	DWORD whiteSpaceSize = pOptionHeader->SizeOfHeaders - ((DWORD)pSectionHeader - (DWORD)pFilebuff + pPEHeader->NumberOfSections * 40);
	
	if (whiteSpaceSize < 80)
	{
		printf("空间不足");
		return false;
    }
	//新增一个节来存放导入表
	//1.修改SizeOfImage大小
	pOptionHeader->SizeOfImage +=  0x1000;
	
	
	
	LPVOID NewBuffer = malloc(pOptionHeader->SizeOfImage);//申请内存
	memset(NewBuffer, 0, pOptionHeader->SizeOfImage);//初始化内存
	//修改节表NumberOfSection数量
	
	for (int i=0;i<80;i++)
	{
		if (*((char*)pFilebuff +whiteSpaceSize + i) !=0 )
		{
			
			pTempBuffer=malloc(pOptionHeader->SizeOfImage);
			
			//如果空间足，但是节最后的80个字节不等于0 就需要将PE头往上移动,把垃圾数据清空 
			memset(pTempBuffer, 0, pOptionHeader->SizeOfImage);
			
			//把整个原来的pImageBuffer 复制到新的imagefile
			memcpy(pTempBuffer,pFilebuff,pOptionHeader->SizeOfImage);

			DWORD e_lfanew = sizeof(IMAGE_DOS_HEADER);
			//计算PE-到节表有多少个字节
			DWORD moveSize = pOptionHeader->SizeOfHeaders - whiteSpaceSize -  pDosHeader->e_lfanew;
			//printf("垃圾数据开始的地方%x\n",(char*)pTempImageBuffer+e_lfanew);
			//printf("PE开始的地方%x\n",(char*)pTempImageBuffer+pDosHeader->e_lfanew);
			//把PE到节的数据移动到DOSSUB区域  
			memcpy((char*)pTempBuffer+e_lfanew,(char*)pTempBuffer+pDosHeader->e_lfanew,moveSize);
			//然后将PE-到节表然后到之前的数据清0 //计算大小 总大小 = pOptionHeader->SizeOfHeaders  - whiteSpaceSize - DOS头的大小 - PE到节表的大小
			DWORD setZeroSzie = pOptionHeader->SizeOfHeaders - whiteSpaceSize - e_lfanew - moveSize;
			//之前的数据清0
			memset((char*)pTempBuffer+e_lfanew+moveSize,0,setZeroSzie);
			//修正e_lfanew
			memcpy((char*)pTempBuffer+sizeof(IMAGE_DOS_HEADER)-4,&e_lfanew,1);
			
			//重新从pTempImageBuffer 获取DOS头，PE头，节表等信息
			pDosHeader =(PIMAGE_DOS_HEADER)pTempBuffer;
			pNTHeader = (PIMAGE_NT_HEADERS)((DWORD)pTempBuffer + pDosHeader->e_lfanew);
			//打印NT头	
			pPEHeader = (PIMAGE_FILE_HEADER)(((DWORD)pNTHeader) + 4);  //加4个字节到了标准PE头
			pOptionHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPEHeader + IMAGE_SIZEOF_FILE_HEADER); //标准PE头+标准PE头的大小 20
			pSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)pOptionHeader + pPEHeader->SizeOfOptionalHeader);
			
			pFilebuff = pTempBuffer;

		}	
	}
	


	PIMAGE_SECTION_HEADER pNewSec = (PIMAGE_SECTION_HEADER)(pSectionHeader + pPEHeader->NumberOfSections);
	
    memcpy(pNewSec->Name,".addSec",8);//修改节表名
	
	//获取最后一个节
	PIMAGE_SECTION_HEADER EndSection = (PIMAGE_SECTION_HEADER)(pSectionHeader + pPEHeader->NumberOfSections-1);
	
	//内存对齐
	pNewSec->VirtualAddress = EndSection->VirtualAddress + (DWORD)(ceil(max(EndSection->Misc.VirtualSize, EndSection->SizeOfRawData)  / pOptionHeader->SectionAlignment) * pOptionHeader->SectionAlignment);
	//内存大小
	pNewSec->Misc.VirtualSize = 0x1000;
	//文件大小
	pNewSec->SizeOfRawData = 0x1000;//新增的节区的大小
	//文件偏移
	pNewSec->PointerToRawData = EndSection->PointerToRawData + EndSection->SizeOfRawData;
	pNewSec->Characteristics = 0x60000020;//修改属性(可执行)
	pPEHeader->NumberOfSections += 1;
	//在新增节表后增加40个字节的空白区
    memset(pNewSec+1, 0, 40);



    memcpy(NewBuffer, pFilebuff,pOptionHeader->SizeOfImage);//复制内存 

	
	if (!pOptionHeader->DataDirectory[1].VirtualAddress)
	{
		printf("(DLLInject)This program has no import table.\n");
		return 0;
	}
	
	
	//定位新节表的地址
	LPVOID pNewSecAddr = (LPVOID)((DWORD)NewBuffer+pNewSec->PointerToRawData);
	
	pDosHeader =(PIMAGE_DOS_HEADER)NewBuffer;
	pNTHeader = (PIMAGE_NT_HEADERS)((DWORD)NewBuffer + pDosHeader->e_lfanew);
	//打印NT头	
	pPEHeader = (PIMAGE_FILE_HEADER)(((DWORD)pNTHeader) + 4);  //加4个字节到了标准PE头
	pOptionHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPEHeader + IMAGE_SIZEOF_FILE_HEADER); //标准PE头+标准PE头的大小 20
	pSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)pOptionHeader + pPEHeader->SizeOfOptionalHeader);
	


	//定位导入表	
	PIMAGE_IMPORT_DESCRIPTOR ImportExtable = (PIMAGE_IMPORT_DESCRIPTOR)((char*)NewBuffer + RvaToFileOffset(NewBuffer,pOptionHeader->DataDirectory[1].VirtualAddress));

	//复制新的导入表
	memcpy(pNewSecAddr,ImportExtable,pOptionHeader->DataDirectory[1].Size);

	//复制完在循环获取大小

	int j =0;
	while(ImportExtable->FirstThunk !=0 &&ImportExtable->OriginalFirstThunk !=0){
		ImportExtable++;
		j++;
	}	
	
	//新的导入表结构的地址,因为是新增节的方式，所以移动完导出表之后这个地方会为0，可以直接使用一个PIMAGE_IMPORT_DESCRIPTOR结构
	PIMAGE_IMPORT_DESCRIPTOR NewImportExtable= (PIMAGE_IMPORT_DESCRIPTOR)((DWORD)pNewSecAddr + j * (sizeof(IMAGE_IMPORT_DESCRIPTOR)));

	
	//设置IAT表 IAT表的地址 = 自己新增的导入表的地址 + 2个自己结构的大小  第一个结构存放新增的导入表，第二个结构做为导入表的结束标记
	PIMAGE_THUNK_DATA32 IatTable = (PIMAGE_THUNK_DATA32)(NewImportExtable +2);

	//设置InT表
	PIMAGE_THUNK_DATA32 IntTable = (PIMAGE_THUNK_DATA32)(IatTable+2);

	//设置IMAGE_IMPORT_BY_NAME

	PIMAGE_IMPORT_BY_NAME ImageName = (PIMAGE_IMPORT_BY_NAME)(IntTable+2);
	
	memcpy(ImageName->Name,"ExportFunction",strlen("ExportFunction")+1);

	//获取存放DLL名称的地址	这个地址保证在PIMAGE_IMPORT_BY_NAME的结构后面就行
	
	DWORD* DllName  = (DWORD*)(ImageName + 1);//
	
	memcpy(DllName,"InjectDll.dll",strlen("InjectDll.dll")+1);


	//修正IAT表地址
	DWORD InAtFoa = FoaToImageOffset(NewBuffer,(DWORD)ImageName -(DWORD)NewBuffer);
	memcpy(IntTable,&InAtFoa,4);
	memcpy(IntTable,&InAtFoa,4);
	
	//修导出表地址
	NewImportExtable->FirstThunk = FoaToImageOffset(NewBuffer,(DWORD)IntTable- (DWORD)NewBuffer);
	NewImportExtable->OriginalFirstThunk = FoaToImageOffset(NewBuffer,(DWORD)IatTable- (DWORD)NewBuffer);
	//修正dll名称的地址
	NewImportExtable->Name =  FoaToImageOffset(NewBuffer,(DWORD)DllName - (DWORD)NewBuffer);
	//新导出表的地址 新导出表的地址= NewExport_Directory 内存地址 - 文件开始的位置 = 偏移
	pOptionHeader->DataDirectory[1].VirtualAddress = FoaToImageOffset(NewBuffer,(DWORD)pNewSecAddr-(DWORD)NewBuffer);
	pOptionHeader->DataDirectory[1].Size = pOptionHeader->DataDirectory[1].Size + 40;

	FILE* fp = fopen("C://project/ipmsg1.exe","wb+");
	size_t n = 	fwrite(NewBuffer,pOptionHeader->SizeOfImage,1,fp);
	if(!n){
		printf("fwrite数据写入失败...\n");
		fclose(fp);
		return ERROR;
	}
	printf("存盘成功...\n");
	free(pFilebuff);
	free(NewBuffer);
	fclose(fp);

	
	

	return 1;
}







DWORD RvaToFileOffset(PVOID pFileBuffer, DWORD dwRva){
	
	//定义PE头的信息
	PIMAGE_DOS_HEADER pDosHeader = NULL;
	PIMAGE_NT_HEADERS pNTHeader = NULL;
	PIMAGE_FILE_HEADER pPEHeader = NULL;
	PIMAGE_OPTIONAL_HEADER32 pOptionHeader = NULL;
	PIMAGE_SECTION_HEADER pSectionHeader = NULL;
	
	if(!pFileBuffer)
	{
		printf("读取到内存的pfilebuffer无效！\n");
		return 0;
	}
	//判断是不是exe文件
	if(*((PWORD)pFileBuffer) != IMAGE_DOS_SIGNATURE)
	{
		printf("不含MZ标志，不是exe文件！\n");
		return 0;
	}
	pDosHeader = (PIMAGE_DOS_HEADER)pFileBuffer;
	if(*((PDWORD)((BYTE *)pFileBuffer + pDosHeader->e_lfanew)) != IMAGE_NT_SIGNATURE){
		printf("无有效的PE标志\n");
		return 0;
	}
	
	//读取pFileBuffer 获取DOS头，PE头，节表等信息
	pDosHeader =(PIMAGE_DOS_HEADER)pFileBuffer;
	pNTHeader = (PIMAGE_NT_HEADERS)((DWORD)pFileBuffer + pDosHeader->e_lfanew);
	//打印NT头	
	pPEHeader = (PIMAGE_FILE_HEADER)(((DWORD)pNTHeader) + 4);  //加4个字节到了标准PE头
	pOptionHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPEHeader + IMAGE_SIZEOF_FILE_HEADER); //标准PE头+标准PE头的大小 20

	
	pSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)pOptionHeader + pPEHeader->SizeOfOptionalHeader);
	

	//PIMAGE_SECTION_HEADER pSectionTemp = pSectionHeader;

	if (dwRva <= pOptionHeader->SizeOfHeaders)
		return (DWORD)dwRva;
	else
	{
		for (int n = 0; n < pPEHeader->NumberOfSections; n++, pSectionHeader++)
		{	
			//判断 :   文件对齐+文件偏移>file_panyi>文件偏移  (即是在文件的哪个节中)
			//  首先要判断在那个节中，并且小于当前节的大小
			if ((dwRva >= pSectionHeader->VirtualAddress) && (dwRva < pSectionHeader->VirtualAddress + pSectionHeader->Misc.VirtualSize))
			{
				//文件偏移 =  当前内存偏移-当前内存地址+ 当前内存中的文件偏移
				return dwRva - pSectionHeader->VirtualAddress + pSectionHeader->PointerToRawData;
			}
		}
	}
	printf("RvaToFoa failed！\n");
	return 0;
}



DWORD FoaToImageOffset(PVOID pBuffer, DWORD dwFoa)
{
	PIMAGE_DOS_HEADER pDosHeader = NULL;
	PIMAGE_NT_HEADERS pNTHeader = NULL;
	PIMAGE_FILE_HEADER pPEHeader = NULL;
	PIMAGE_OPTIONAL_HEADER32 pOptionHeader = NULL;
	PIMAGE_SECTION_HEADER pSectionHeader = NULL;
	
	if (!pBuffer)
	{
		printf("(FoaToImageOffset)Can't open file!\n");
		return 0;
	}
	
	if (*((PWORD)pBuffer) != IMAGE_DOS_SIGNATURE)
	{
		printf("(FoaToImageOffset)No MZ flag, not exe file!\n");
		return 0;
	}
	
	pDosHeader = (PIMAGE_DOS_HEADER)pBuffer;
	if (*((PDWORD)((DWORD)pBuffer + pDosHeader->e_lfanew)) != IMAGE_NT_SIGNATURE)
	{
		printf("(FoaToImageOffset)Not a valid PE flag!\n");
		return 0;
	}

	
	pNTHeader = (PIMAGE_NT_HEADERS)((DWORD)pBuffer + pDosHeader->e_lfanew);
	pPEHeader = (PIMAGE_FILE_HEADER)((DWORD)pNTHeader + 4); // 这里必须强制类型转换
	pOptionHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPEHeader + IMAGE_SIZEOF_FILE_HEADER);
	pSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)pOptionHeader + pPEHeader->SizeOfOptionalHeader);
	

	
	if (dwFoa <= pOptionHeader->SizeOfHeaders)
		return (DWORD)dwFoa;
	else
	{
		for (int n = 0; n < pPEHeader->NumberOfSections; n++, pSectionHeader++)
		{	//判断 :   文件对齐+文件偏移>file_panyi>文件偏移  (即是在文件的哪个节中)
			if ((dwFoa >= pSectionHeader->PointerToRawData) && (dwFoa < pSectionHeader->PointerToRawData + pSectionHeader->SizeOfRawData))
			{
				return dwFoa - pSectionHeader->PointerToRawData + pSectionHeader->VirtualAddress;
			}
		}
	}
	printf("FoaToRva failed！\n");
	return 0;
}