// PeFileOp.cpp: implementation of the PeFileOp class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PeFileOp.h"
#include <math.h>
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


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



BOOL SAddNewSection(PVOID pFileBuffer,PVOID SrcFileBuffer,DWORD SrcBuffSize){
	
	//定义PE头的信息
	PIMAGE_DOS_HEADER pDosHeader = NULL;
	PIMAGE_NT_HEADERS pNTHeader = NULL;
	PIMAGE_FILE_HEADER pPEHeader = NULL;
	PIMAGE_OPTIONAL_HEADER32 pOptionHeader = NULL;
	PIMAGE_SECTION_HEADER pSectionHeader = NULL;
	LPVOID pTempBuffer = NULL;
	bool Ssize = false;
	if(!pFileBuffer)
	{
		MessageBoxA(0,TEXT("读取到内存的pfilebuffer无效"),0,0);
		return 0;
	}
	//判断是不是exe文件
	if(*((PWORD)pFileBuffer) != IMAGE_DOS_SIGNATURE)
	{
		MessageBoxA(0,TEXT("不含MZ标志，不是exe文件！"),0,0);
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

	//移动导入表
	
	//获取剩余大小 //SizeOfHeaders(总的大小)4096 -   ( pSectionHeader(一个地址，文件开始的地方偏移了pSectionHeader的大小的字节)  -    (文件偏移+pPEHeader->NumberOfSections * 40)  一个地址 文件开始的地方偏移了节的大小的字节 并不是节在文件中的偏移地址，所以会比 pSectionHeader小)
	DWORD whiteSpaceSize = pOptionHeader->SizeOfHeaders - ((DWORD)pSectionHeader - (DWORD)pFileBuffer + pPEHeader->NumberOfSections * 40);
	
	if (whiteSpaceSize < 80)
	{
		printf("空间不足");
		return false;
    }

	//1.修改SizeOfImage大小 = 壳文件大小 + 要加壳文件的大小
	pOptionHeader->SizeOfImage +=  SrcBuffSize; 
	


	LPVOID NewBuffer = malloc(pOptionHeader->SizeOfImage);//申请内存
	memset(NewBuffer, 0, pOptionHeader->SizeOfImage);//初始化内存
	//修改节表NumberOfSection数量
	
	for (int i=0;i<80;i++)
	{
		if (*((char*)pFileBuffer +whiteSpaceSize + i) !=0 )
		{
			
			pTempBuffer=malloc(pOptionHeader->SizeOfImage);
			
			//如果空间足，但是节最后的80个字节不等于0 就需要将PE头往上移动,把垃圾数据清空 
			memset(pTempBuffer, 0, pOptionHeader->SizeOfImage);
			
			//把整个原来的pImageBuffer 复制到新的imagefile
			memcpy(pTempBuffer,pFileBuffer,pOptionHeader->SizeOfImage);

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
			pFileBuffer = pTempBuffer;

		}	
	}


	PIMAGE_SECTION_HEADER pNewSec = (PIMAGE_SECTION_HEADER)(pSectionHeader + pPEHeader->NumberOfSections);
	
    memcpy(pNewSec->Name,".addSec",8);//修改节表名
	
	//获取最后一个节
	PIMAGE_SECTION_HEADER EndSection = (PIMAGE_SECTION_HEADER)(pSectionHeader + pPEHeader->NumberOfSections-1);
	
	//内存对齐
	pNewSec->VirtualAddress = EndSection->VirtualAddress + (DWORD)(ceil(max(EndSection->Misc.VirtualSize, EndSection->SizeOfRawData)  / pOptionHeader->SectionAlignment) * pOptionHeader->SectionAlignment);
	//内存大小
	pNewSec->Misc.VirtualSize = SrcBuffSize; //新节的大小 == 要加壳文件的大小
	//文件大小
	pNewSec->SizeOfRawData = SrcBuffSize;//新增的节区的大小
	//文件偏移
	pNewSec->PointerToRawData = EndSection->PointerToRawData + EndSection->SizeOfRawData;
	//pNewSec->Characteristics = 0xC0000040;//修改属性(可执行)
	pNewSec->Characteristics |= IMAGE_SCN_MEM_EXECUTE |IMAGE_SCN_MEM_WRITE|IMAGE_SCN_MEM_READ;
	pPEHeader->NumberOfSections += 1;
	
	//在新增节表后增加40个字节的空白区
    memset(pNewSec+1, 0, 40);

	memcpy(NewBuffer,pFileBuffer,pOptionHeader->SizeOfImage);

	LPVOID pNewSecAddr = (LPVOID)((DWORD)NewBuffer+pNewSec->PointerToRawData);
	memcpy(pNewSecAddr,SrcFileBuffer,SrcBuffSize);



	FILE* fp = fopen("C://project/ipmsgnew.exe","wb+");
	size_t n = 	fwrite(NewBuffer,pOptionHeader->SizeOfImage,1,fp);
	if(!n){
		printf("fwrite数据写入失败...\n");
		fclose(fp);
		return ERROR;
	}
	
	free(pFileBuffer);
	free(NewBuffer);
	
	printf("存盘成功...\n");
	
	fclose(fp);
	
	

	return TRUE;
}