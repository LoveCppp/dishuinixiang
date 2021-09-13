// globe.cpp: implementation of the globe class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "globe.h"
#include <malloc.h>
#include <math.h>
#include <string>
#include <stdlib.h>
#include <stdio.h>

/*
              
	0x6A,00,0x6A,00,0x6A,00,0x6A,00,
	0XE8,00,00,00,00,
	0XE9,00,00,00,00

*/


#define size_shellcode 0x18
#define messagebox_add 0x77D507EA 
//shellcode
BYTE shellcode[] = {
	0x6A,00,          
	0x68,0x1C,0x50, 0x42,00, 
	0x68,0x1C,0x50, 0x42,00,     
	0x6A,00, 
	0XE8,00,00,00,00,
	0XE9,00,00,00,00
};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

globe::globe()
{

}

globe::~globe()
{

}



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


DWORD CopyFileBufferToImageBuffer(IN LPVOID pFileBuffer,OUT LPVOID* pImageBuffer)
{
	PIMAGE_DOS_HEADER pDosHeader = NULL;
	PIMAGE_NT_HEADERS pNTHeader = NULL;
	PIMAGE_FILE_HEADER pPEHeader = NULL;
	PIMAGE_OPTIONAL_HEADER32 pOptionHeader = NULL;
	PIMAGE_SECTION_HEADER pSectionHeader = NULL;
	LPVOID pTempFileBuffer = NULL;

	if(pFileBuffer ==NULL){
		printf("pFileBuffer缓冲区指针出错...\n");
		return ERROR;
	}
	

	//判断是否含有有效MZ和PE标志
	if(*((PWORD)pFileBuffer) != IMAGE_DOS_SIGNATURE){
		printf("无有效的MZ标志\n");
		return ERROR;
	}
	pDosHeader = (PIMAGE_DOS_HEADER)pFileBuffer;
	//IMAGE_SIZEOF_FILE_HEADER = 232   如果转换类型的话就等于当前的类型砍去一个* 加上当前的长度
	if(*((PDWORD)((BYTE *)pFileBuffer + pDosHeader->e_lfanew)) != IMAGE_NT_SIGNATURE){
		printf("无有效的PE标志\n");
		return ERROR;
	}
	
	pDosHeader =(PIMAGE_DOS_HEADER)pFileBuffer;
	pNTHeader = (PIMAGE_NT_HEADERS)((DWORD)pFileBuffer + pDosHeader->e_lfanew);
	//打印NT头
	pPEHeader = (PIMAGE_FILE_HEADER)(((DWORD)pNTHeader) + 4);  //加4个字节到了标准PE头
	
	pOptionHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPEHeader + IMAGE_SIZEOF_FILE_HEADER); //标准PE头+标准PE头的大小 20
	
	//可选PE头加上SizeOfOptionalHeader的
	pSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)pOptionHeader + pPEHeader->SizeOfOptionalHeader);
	
	//根据SizeOfImage申请空间
	pTempFileBuffer  =malloc(pOptionHeader->SizeOfImage);
	if (!pTempFileBuffer)
	{
		printf("pTempFileBuffer空间申请失败...");
		return 0;
	}
	//然后空间设置为0
	memset(pTempFileBuffer, 0, pOptionHeader->SizeOfImage);
	//复制文件头到表结束的地方
	memcpy(pTempFileBuffer,pFileBuffer,pOptionHeader->SizeOfHeaders);
	
	//循环节表复制到内存
	
	for (int i=0;i<pPEHeader->NumberOfSections;i++,pSectionHeader++)
	{
		
		//从pImageBuffer的地址加上偏移+virtualAddress 就是内存中第一个节的位置，然后从文件的PointerToRawData 也就是文件中第一个节的位置，写入pSectionHeader->SizeOfRawData 文件中对其的大小各字节
		memcpy((PVOID)((char *)pTempFileBuffer + pSectionHeader->VirtualAddress), (PVOID)((char *)pFileBuffer + pSectionHeader->PointerToRawData), pSectionHeader->SizeOfRawData);
		//输出表的内容
		
	};

	*pImageBuffer = pTempFileBuffer;
	pTempFileBuffer = NULL;
	return pOptionHeader->SizeOfImage;
}




DWORD CopyImageBufferToNewFileBuffer(IN LPVOID pImageBuffer, OUT LPVOID *pNewFileBuffer)
{
	PIMAGE_DOS_HEADER pDosHeader = NULL;
	PIMAGE_NT_HEADERS pNTHeader = NULL;
	PIMAGE_FILE_HEADER pPEHeader = NULL;
	PIMAGE_OPTIONAL_HEADER32 pOptionHeader = NULL;
	PIMAGE_SECTION_HEADER pSectionHeader = NULL;
	
	if(pImageBuffer ==NULL){
		printf("pImageBuffer缓冲区指针出错...\n");
		return ERROR;
	}
	
	//判断是否含有有效MZ和PE标志
	if(*((PWORD)pImageBuffer) != IMAGE_DOS_SIGNATURE){
		printf("无有效的MZ标志\n");
		return ERROR;
	}
	pDosHeader = (PIMAGE_DOS_HEADER)pImageBuffer;
	if(*((PDWORD)((BYTE *)pImageBuffer + pDosHeader->e_lfanew)) != IMAGE_NT_SIGNATURE){
		printf("无有效的PE标志\n");
		return ERROR;
	}
	
	//读取pFileBuffer 获取DOS头，PE头，节表等信息
	pDosHeader =(PIMAGE_DOS_HEADER)pImageBuffer;
	pNTHeader = (PIMAGE_NT_HEADERS)((DWORD)pImageBuffer + pDosHeader->e_lfanew);
	//打印NT头	
	pPEHeader = (PIMAGE_FILE_HEADER)(((DWORD)pNTHeader) + 4);  //加4个字节到了标准PE头
	pOptionHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPEHeader + IMAGE_SIZEOF_FILE_HEADER); //标准PE头+标准PE头的大小 20
	pSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)pOptionHeader + pPEHeader->SizeOfOptionalHeader);
	
	for (int i=0;i<pPEHeader->NumberOfSections;i++,pSectionHeader++)
	{
		//从pImageBuffer的PointerToRawData 也就是文件的节开始的位置，然后从内存中节的开始的地址写入SizeOfRawData（文件对其后的大小个字节）
		//printf("%d\n",pSectionHeader->SizeOfRawData);

		memcpy((PVOID)((DWORD)pImageBuffer + pSectionHeader->PointerToRawData), (PVOID)((DWORD)pImageBuffer + pSectionHeader->VirtualAddress), pSectionHeader->SizeOfRawData);		
	};

	
	*pNewFileBuffer = pImageBuffer;

	return pOptionHeader->SizeOfImage;
}



BOOL NewFileBufferToFile(IN LPVOID pNewFileBuffer, size_t size,OUT LPSTR lpszFile)
{
	if(pNewFileBuffer == NULL){
		printf("pNewFileBuffer缓冲区出差...\n");
		return ERROR;
	}
	//定义PE头的信息
	PIMAGE_DOS_HEADER pDosHeader = NULL;
	PIMAGE_NT_HEADERS pNTHeader = NULL;
	PIMAGE_FILE_HEADER pPEHeader = NULL;
	PIMAGE_OPTIONAL_HEADER32 pOptionHeader = NULL;
	PIMAGE_SECTION_HEADER pSectionHeader = NULL;
	if(pNewFileBuffer ==NULL){
		printf("pImageBuffer缓冲区指针出错...\n");
		return ERROR;
	}
	
	//判断是否含有有效MZ和PE标志
	if(*((PWORD)pNewFileBuffer) != IMAGE_DOS_SIGNATURE){
		printf("无有效的MZ标志\n");
		return ERROR;
	}
	FILE *pFile = NULL;
	size_t fileSize = size;
	
	pFile = fopen(lpszFile, "wb");
	if(!pFile){
		printf("fopen保存EXE文件失败...\n");
		return ERROR;
	}

	size_t n = fwrite(pNewFileBuffer, fileSize, 1, pFile);
	
	if(!n){
		printf("fwrite数据写入失败...\n");
		fclose(pFile);
		return ERROR;
	}
	printf("存盘成功...\n");
	fclose(pFile);
	
	return TRUE;
};




DWORD Add_SectionCode(LPVOID pImageBuffer)
{
	//定义PE头的信息
	PIMAGE_DOS_HEADER pDosHeader = NULL;
	PIMAGE_NT_HEADERS pNTHeader = NULL;
	PIMAGE_FILE_HEADER pPEHeader = NULL;
	PIMAGE_OPTIONAL_HEADER32 pOptionHeader = NULL;
	PIMAGE_SECTION_HEADER pSectionHeader = NULL;
	
	if(!pImageBuffer)
	{
		printf("读取到内存的pfilebuffer无效！\n");
		return 0;
	}
	//判断是不是exe文件
	if(*((PWORD)pImageBuffer) != IMAGE_DOS_SIGNATURE)
	{
		printf("不含MZ标志，不是exe文件！\n");
		return 0;
	}
	pDosHeader = (PIMAGE_DOS_HEADER)pImageBuffer;
	if(*((PDWORD)((BYTE *)pImageBuffer + pDosHeader->e_lfanew)) != IMAGE_NT_SIGNATURE){
		printf("无有效的PE标志\n");
		return 0;
	}
	
	//读取pFileBuffer 获取DOS头，PE头，节表等信息
	pDosHeader =(PIMAGE_DOS_HEADER)pImageBuffer;
	pNTHeader = (PIMAGE_NT_HEADERS)((DWORD)pImageBuffer + pDosHeader->e_lfanew);
	//打印NT头	
	pPEHeader = (PIMAGE_FILE_HEADER)(((DWORD)pNTHeader) + 4);  //加4个字节到了标准PE头
	pOptionHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPEHeader + IMAGE_SIZEOF_FILE_HEADER); //标准PE头+标准PE头的大小 20
	pSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)pOptionHeader + pPEHeader->SizeOfOptionalHeader);
	
	//计算

	

	//计算空闲区大小
	//SizeOfRawData 文件中对齐的大小  减去 VirtualSize 没有对齐的大小  如果比shellcode的大小，那么代码区无法写入shellcode
	if ( ((pSectionHeader->SizeOfRawData) - (pSectionHeader->Misc.VirtualSize)) < size_shellcode  )
	{
		printf("代码区空间不足！\n"); 
		free(pImageBuffer);
		return 0;
	}

	//然后复制shellcode到
	PBYTE code_begin = (PBYTE)((DWORD)pImageBuffer + pSectionHeader->VirtualAddress + pSectionHeader->Misc.VirtualSize );

	//将shellcode拷贝到内存中
	memcpy(code_begin,shellcode,size_shellcode);
	
	//修正E8 E9 运行的时候需要从imagebas开始
	//真正要跳转的地址 = E8的下一跳的地址  + x
	//x = messbox -  E8的下一跳的地址
	
	/*
		pOptionHeader->ImageBase  内存中的地址
		code_begin + 0XD E8的下一跳的地址  - pImageBuffer 得到偏移地址 
		pOptionHeader->ImageBas + 偏移地址 就是E9的地址 就是运行时候的地址
	*/

	DWORD callAddr = messagebox_add - ((DWORD)pOptionHeader->ImageBase + ((DWORD)(code_begin + 0X14) - (DWORD)pImageBuffer));
	
	//code_begin + 9 就是E8后面第一个字节的地址
	*(PDWORD)(code_begin + 0xf) = callAddr;

	//开始修正E9
	/*
		 X=真正要跳转的地址 - E9这条执行的下一行的地址 
		真正要跳转的地址 = pOptionHeader->ImageBase + pOptionHeader->AddressOfEntryPoint   ImageBase + AddressOfEntryPoint
		E9下一行的地址    pOptionHeader->ImageBase + code_begin + size_shellcode 的大小
	*/

	DWORD jmpAddr = ((pOptionHeader->ImageBase + pOptionHeader->AddressOfEntryPoint)-(pOptionHeader->ImageBase+((DWORD)code_begin+size_shellcode-(DWORD)pImageBuffer)));
	//获取E9后面的第一个字节地址 
	*(PDWORD)(code_begin + 0x14) = jmpAddr;
	
	pOptionHeader->AddressOfEntryPoint = (DWORD)code_begin - (DWORD)pImageBuffer;
	
	
	return 1;

};




//扩大一个节

DWORD Add_SectionInNewSecExt(IN LPVOID pImageBuffer,OUT LPVOID* pNewImageBuffer){
	
	PIMAGE_DOS_HEADER pDosHeader = NULL;
	PIMAGE_NT_HEADERS pNTHeader = NULL;
	PIMAGE_FILE_HEADER pPEHeader = NULL;
	PIMAGE_OPTIONAL_HEADER32 pOptionHeader = NULL;
	PIMAGE_SECTION_HEADER pSectionHeader = NULL;
	LPVOID pTempImageBuffer = NULL;
	if(!pImageBuffer)
	{
		printf("读取到内存的pImageBuffer无效！\n");
		return 0;
	}
	//判断是不是exe文件
	if(*((PWORD)pImageBuffer) != IMAGE_DOS_SIGNATURE)
	{
		printf("不含MZ标志，不是exe文件！\n");
		return 0;
	}
	pDosHeader = (PIMAGE_DOS_HEADER)pImageBuffer;
	if(*((PDWORD)((BYTE *)pImageBuffer + pDosHeader->e_lfanew)) != IMAGE_NT_SIGNATURE){
		printf("无有效的PE标志\n");
		return 0;
	}
	
	//读取pFileBuffer 获取DOS头，PE头，节表等信息
	pDosHeader =(PIMAGE_DOS_HEADER)pImageBuffer;
	pNTHeader = (PIMAGE_NT_HEADERS)((DWORD)pImageBuffer + pDosHeader->e_lfanew);
	//打印NT头	
	pPEHeader = (PIMAGE_FILE_HEADER)(((DWORD)pNTHeader) + 4);  //加4个字节到了标准PE头
	pOptionHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPEHeader + IMAGE_SIZEOF_FILE_HEADER); //标准PE头+标准PE头的大小 20
	pSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)pOptionHeader + pPEHeader->SizeOfOptionalHeader);

	//扩大节的话需要找到节的虚拟地址和大小 然后相加 然后改变imagebase
	DWORD SizeOfImageAddSize = pOptionHeader->SizeOfImage + 0x1000;
	//申请内存
	pTempImageBuffer=malloc(SizeOfImageAddSize);
	
	if (!pTempImageBuffer)
	{
		printf("pTempImageBuffer空间申请失败...");
		free(pTempImageBuffer);
		
		return 0;
	}
	//然后空间设置为0
	memset(pTempImageBuffer, 0, SizeOfImageAddSize);
	//把整个原来的pImageBuffer 复制到新的imagefile
	memcpy(pTempImageBuffer,pImageBuffer,SizeOfImageAddSize);

	//找到最后一个节

	pSectionHeader = pSectionHeader+pPEHeader->NumberOfSections -1;
	//修改节的大小：
	DWORD sizeTotal = (DWORD)(ceil(max(pSectionHeader->Misc.VirtualSize+0x1000, pSectionHeader->SizeOfRawData+0x1000)  / pOptionHeader->SectionAlignment) * pOptionHeader->SectionAlignment);
	//文件对齐大小
	pSectionHeader->SizeOfRawData = sizeTotal;
	//内存对齐大小
	pSectionHeader->Misc.VirtualSize = sizeTotal;
	//找到最后一个节的开始位置偏移
	//最后一个节的地址 
	DWORD endSectionsSize =(pDosHeader->e_lfanew + sizeof(pNTHeader->Signature) + sizeof(*pPEHeader) + sizeof(*pOptionHeader) + ( (pPEHeader->NumberOfSections-1) * sizeof(IMAGE_SECTION_HEADER)));
	//printf("%x",(char*)pTempImageBuffer+endSectionsSize);
	memcpy((char*)pTempImageBuffer+endSectionsSize,pSectionHeader,sizeof(IMAGE_SECTION_HEADER));
	
	//修改sizeofimage的大小
	DWORD SizeofImageSzie  =  pDosHeader->e_lfanew + sizeof(pNTHeader->Signature) + IMAGE_SIZEOF_FILE_HEADER + 56;
	
	memcpy((char*)pTempImageBuffer+SizeofImageSzie,&SizeOfImageAddSize,4);
	

	*pNewImageBuffer = pTempImageBuffer;


	free(pTempImageBuffer);
	free(pImageBuffer);
	return 1;
}



//新增节


DWORD Add_SectionInNewSec(IN LPVOID pImageBuffer,OUT LPVOID* pNewImageBuffer){
	
	
	PIMAGE_DOS_HEADER pDosHeader = NULL;
	PIMAGE_NT_HEADERS pNTHeader = NULL;
	PIMAGE_FILE_HEADER pPEHeader = NULL;
	PIMAGE_OPTIONAL_HEADER32 pOptionHeader = NULL;
	PIMAGE_SECTION_HEADER pSectionHeader = NULL;
	LPVOID pTempImageBuffer = NULL;
	bool x = false;
	DWORD Numsize=0;
	if(!pImageBuffer)
	{
		printf("读取到内存的pImageBuffer无效！\n");
		return 0;
	}
	//判断是不是exe文件
	if(*((PWORD)pImageBuffer) != IMAGE_DOS_SIGNATURE)
	{
		printf("不含MZ标志，不是exe文件！\n");
		return 0;
	}
	pDosHeader = (PIMAGE_DOS_HEADER)pImageBuffer;
	if(*((PDWORD)((BYTE *)pImageBuffer + pDosHeader->e_lfanew)) != IMAGE_NT_SIGNATURE){
		printf("无有效的PE标志\n");
		return 0;
	}
	
	//读取pFileBuffer 获取DOS头，PE头，节表等信息
	pDosHeader =(PIMAGE_DOS_HEADER)pImageBuffer;
	pNTHeader = (PIMAGE_NT_HEADERS)((DWORD)pImageBuffer + pDosHeader->e_lfanew);
	//打印NT头	
	pPEHeader = (PIMAGE_FILE_HEADER)(((DWORD)pNTHeader) + 4);  //加4个字节到了标准PE头
	pOptionHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPEHeader + IMAGE_SIZEOF_FILE_HEADER); //标准PE头+标准PE头的大小 20
	pSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)pOptionHeader + pPEHeader->SizeOfOptionalHeader);
	
	//1、判断是否有足够的空间，可以添加一个节表.
	//	SizeOfHeader - (DOS + 垃圾数据 + PE标记 + 标准PE头 + 可选PE头 + 已存在节表)
	//DOS头大小  +垃圾数据的大小等于 = pDosHeader->e_lfanew 的大小
	// PE标记 sizeof(pNTHeader->Signature)
	//标准PE大小  sizeof(*pPEHeader) 
	//可选PE头 sizeof(*pOptionHeader) 
	//已存在的节表大小  节的总数 * 节表的大小 pPEHeader->NumberOfSections *　sizeof

	DWORD whiteSpaceSize =pOptionHeader->SizeOfHeaders - (pDosHeader->e_lfanew + sizeof(pNTHeader->Signature) + sizeof(*pPEHeader) + sizeof(*pOptionHeader) + ( pPEHeader->NumberOfSections * sizeof(IMAGE_SECTION_HEADER)));
	//判断是否还有2个字节的大小
	
	printf("%d\n",whiteSpaceSize);

	if (whiteSpaceSize < (2 * sizeof(IMAGE_SECTION_HEADER)))
	{
		printf("---空间不足无法新增节表----");
		return false;
		//如果空间不足，则判断是否可以将PE往上拉伸

		
	}
	
	//2修改数据
	//2.1需要开辟一个新的空间来存放修改后filebuff

	//然后获取需要申请多大内存，需要申请的内存等于原先的大小加上需要填充的节的大小，以1000为例

	DWORD SizeOfImageTotalSize = pOptionHeader->SizeOfImage + 0x1000;
	
	pTempImageBuffer=malloc(SizeOfImageTotalSize);
	
	if (!pTempImageBuffer)
	{
		printf("pTempImageBuffer空间申请失败...");
		free(pTempImageBuffer);
		
		return 0;
	}
	//然后空间设置为0
	memset(pTempImageBuffer, 0, SizeOfImageTotalSize);
	
	//把整个原来的pImageBuffer 复制到新的imagefile
	memcpy(pTempImageBuffer,pImageBuffer,SizeOfImageTotalSize);


	//判断后面80个字节是否都是0
	for (int i=0;i<80;i++)
	{
		if (*((char*)pTempImageBuffer +whiteSpaceSize + i) !=0 )
		{

			DWORD e_lfanew = sizeof(IMAGE_DOS_HEADER);
			//计算PE-到节表有多少个字节
			DWORD moveSize = pOptionHeader->SizeOfHeaders - whiteSpaceSize -  pDosHeader->e_lfanew;
			//printf("垃圾数据开始的地方%x\n",(char*)pTempImageBuffer+e_lfanew);
			//printf("PE开始的地方%x\n",(char*)pTempImageBuffer+pDosHeader->e_lfanew);
			//把PE到节的数据移动到DOSSUB区域  
			memcpy((char*)pTempImageBuffer+e_lfanew,(char*)pTempImageBuffer+pDosHeader->e_lfanew,moveSize);
			//然后将PE-到节表然后到之前的数据清0 //计算大小 总大小 = pOptionHeader->SizeOfHeaders  - whiteSpaceSize - DOS头的大小 - PE到节表的大小
			DWORD setZeroSzie = pOptionHeader->SizeOfHeaders - whiteSpaceSize - e_lfanew - moveSize;
			//之前的数据清0
			memset((char*)pTempImageBuffer+e_lfanew+moveSize,0,setZeroSzie);
			//修正e_lfanew
			memcpy((char*)pTempImageBuffer+sizeof(IMAGE_DOS_HEADER)-4,&e_lfanew,1);
			
			
			//重新从pTempImageBuffer 获取DOS头，PE头，节表等信息
			pDosHeader =(PIMAGE_DOS_HEADER)pTempImageBuffer;
			
			pNTHeader = (PIMAGE_NT_HEADERS)((DWORD)pTempImageBuffer + pDosHeader->e_lfanew);

			//打印NT头	
			pPEHeader = (PIMAGE_FILE_HEADER)(((DWORD)pNTHeader) + 4);  //加4个字节到了标准PE头
			pOptionHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPEHeader + IMAGE_SIZEOF_FILE_HEADER); //标准PE头+标准PE头的大小 20
		
			DWORD whiteSpaceSize = pDosHeader->e_lfanew + sizeof(pNTHeader->Signature) + sizeof(*pPEHeader) + sizeof(*pOptionHeader) + ( pPEHeader->NumberOfSections * sizeof(IMAGE_SECTION_HEADER));
			
			x= true;
			


		}	
	}
	
    //新增节开始
    PIMAGE_SECTION_HEADER AddSectionHeader;
    AddSectionHeader = pSectionHeader;  //复制第一个节表到新的节表
	//修改节的名字
	AddSectionHeader->Name[0] = '.'; 
	AddSectionHeader->Name[1] = 'a'; 
	AddSectionHeader->Name[2] = 'd'; 
	AddSectionHeader->Name[3] = 'd';
	AddSectionHeader->Name[4] = 'x';
	
	

	if (x)
	{
		 Numsize =   whiteSpaceSize;
	}else{
		 Numsize =  pOptionHeader->SizeOfHeaders - whiteSpaceSize;
	}

	//修改节的内存大小
	AddSectionHeader->Misc.VirtualSize = 0x1000;
	//获取最后一个节
	PIMAGE_SECTION_HEADER EndSection = pSectionHeader+(pPEHeader->NumberOfSections-1);
	//修改节的内存偏移地址
    //内存对齐 = 上一个节的内存大小和文件对齐取最大值   /  内存的对齐大小 然后* 内存对齐的大小 
	//(DWORD)(ceil(max(EndSection->Misc.VirtualSize, EndSection->SizeOfRawData)  / pOptionHeader->SectionAlignment) * pOptionHeader->SectionAlignment)
	AddSectionHeader->VirtualAddress = EndSection->VirtualAddress + (DWORD)(ceil(max(EndSection->Misc.VirtualSize, EndSection->SizeOfRawData)  / pOptionHeader->SectionAlignment) * pOptionHeader->SectionAlignment);

	//修改文件大小
	AddSectionHeader->SizeOfRawData  = 0x1000;
	//修改节的文件偏移地址
	//文件对齐 = 
	AddSectionHeader->PointerToRawData = EndSection->PointerToRawData  + EndSection->SizeOfRawData;
	//把新增的节复制到内存中，新增节复制结束
	memcpy((char*)pTempImageBuffer+Numsize,AddSectionHeader,sizeof(IMAGE_SECTION_HEADER));
	//修改节的数量
    DWORD SectionNumSzie  =  pDosHeader->e_lfanew + sizeof(pNTHeader->Signature) + 2;
	//需要申请一个地址来存放节的大小

	DWORD NewNumTotal = pPEHeader->NumberOfSections+1;
	memcpy((char*)pTempImageBuffer+SectionNumSzie, &NewNumTotal, 0x1);
	//计算SIZEofimage 的偏移
		
	DWORD SizeofImageSzie  =  pDosHeader->e_lfanew + sizeof(pNTHeader->Signature) + IMAGE_SIZEOF_FILE_HEADER + 56;

	memcpy((char*)pTempImageBuffer+SizeofImageSzie,&SizeOfImageTotalSize,4);
	
	*pNewImageBuffer = pTempImageBuffer;
	
	return 1;
}




//合并节

DWORD Merge_Sec(IN LPVOID pImageBuffer,OUT LPVOID* pNewImageBuffer){\

	PIMAGE_DOS_HEADER pDosHeader = NULL;
	PIMAGE_NT_HEADERS pNTHeader = NULL;
	PIMAGE_FILE_HEADER pPEHeader = NULL;
	PIMAGE_OPTIONAL_HEADER32 pOptionHeader = NULL;
	PIMAGE_SECTION_HEADER pSectionHeader = NULL;
	LPVOID pTempImageBuffer = NULL;
	if(!pImageBuffer)
	{
		printf("读取到内存的pImageBuffer无效！\n");
		return 0;
	}
	//判断是不是exe文件
	if(*((PWORD)pImageBuffer) != IMAGE_DOS_SIGNATURE)
	{
		printf("不含MZ标志，不是exe文件！\n");
		return 0;
	}
	pDosHeader = (PIMAGE_DOS_HEADER)pImageBuffer;
	if(*((PDWORD)((BYTE *)pImageBuffer + pDosHeader->e_lfanew)) != IMAGE_NT_SIGNATURE){
		printf("无有效的PE标志\n");
		return 0;
	}
	
	//读取pFileBuffer 获取DOS头，PE头，节表等信息
	pDosHeader =(PIMAGE_DOS_HEADER)pImageBuffer;
	pNTHeader = (PIMAGE_NT_HEADERS)((DWORD)pImageBuffer + pDosHeader->e_lfanew);
	//打印NT头	
	pPEHeader = (PIMAGE_FILE_HEADER)(((DWORD)pNTHeader) + 4);  //加4个字节到了标准PE头
	pOptionHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPEHeader + IMAGE_SIZEOF_FILE_HEADER); //标准PE头+标准PE头的大小 20
	pSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)pOptionHeader + pPEHeader->SizeOfOptionalHeader);
	
	//第一种算法
	DWORD Max =(char*)pOptionHeader->SizeOfImage - (char*)pSectionHeader->VirtualAddress;

	//第二种算法
	//最后一个节的地址 VirtualSize 或者 VirtualAddress + SizeOfRawData - 头的大小
	//获取最后一个节
	//PIMAGE_SECTION_HEADER EndSection = pSectionHeader+(pPEHeader->NumberOfSections-1);
	//DWORD Max1 = EndSection->VirtualAddress +Alignment(pOptionHeader->SectionAlignment,max(EndSection->Misc.VirtualSize, EndSection->SizeOfRawData),0)   - Alignment(pOptionHeader->SectionAlignment, pOptionHeader->SizeOfHeaders, 0);
	//VirtualSize = SizeOfRawData  = Max	
	//1.修改节表的数量
	pPEHeader->NumberOfSections = 1;


     pSectionHeader->Characteristics |= IMAGE_SCN_MEM_EXECUTE |IMAGE_SCN_MEM_WRITE|IMAGE_SCN_MEM_READ;
	//pSectionHeader->Characteristics |= ( (PIMAGE_SECTION_HEADER)( (DWORD)pSectionHeader + IMAGE_SIZEOF_SECTION_HEADER ) )->Characteristics ;
	
	//pSectionHeader->Characteristics = 0xE0000060;

	
	//VS SR   VirtualAddress PD不用改

	pSectionHeader->Misc.VirtualSize = Max;

	pSectionHeader->SizeOfRawData = Max;

	//先复制到内存
	pTempImageBuffer  =malloc(pOptionHeader->SizeOfImage);
	if (!pTempImageBuffer)
	{
		printf("pTempFileBuffer空间申请失败...");
		return 0;
	}
	//然后空间设置为0
	memset(pTempImageBuffer, 0, pOptionHeader->SizeOfImage);
	//复制整个文件
	memcpy(pTempImageBuffer,pImageBuffer,pOptionHeader->SizeOfImage);


	*pNewImageBuffer = pTempImageBuffer;	
	return 1;

}
	

DWORD Alignment(DWORD alignment_value, DWORD addend, DWORD address)
{
	int n = 0;
	if (addend / alignment_value)
	{
		if (addend%alignment_value)
		{
			n = addend / alignment_value + 1;
		}
		else
		{
			n = addend / alignment_value;
		}
	}
	else
	{
		if (addend)
			n = 1;
		else
			n = 0;
	}
	address += n * alignment_value;
	return address;
}


DWORD PrintDriectory(LPVOID pImageBuffer){
	
	//定义PE头的信息
	PIMAGE_DOS_HEADER pDosHeader = NULL;
	PIMAGE_NT_HEADERS pNTHeader = NULL;
	PIMAGE_FILE_HEADER pPEHeader = NULL;
	PIMAGE_OPTIONAL_HEADER32 pOptionHeader = NULL;
	PIMAGE_SECTION_HEADER pSectionHeader = NULL;
	
	if(!pImageBuffer)
	{
		printf("读取到内存的pfilebuffer无效！\n");
		return 0;
	}
	//判断是不是exe文件
	if(*((PWORD)pImageBuffer) != IMAGE_DOS_SIGNATURE)
	{
		printf("不含MZ标志，不是exe文件！\n");
		return 0;
	}
	pDosHeader = (PIMAGE_DOS_HEADER)pImageBuffer;
	if(*((PDWORD)((BYTE *)pImageBuffer + pDosHeader->e_lfanew)) != IMAGE_NT_SIGNATURE){
		printf("无有效的PE标志\n");
		return 0;
	}
	
	//读取pFileBuffer 获取DOS头，PE头，节表等信息
	pDosHeader =(PIMAGE_DOS_HEADER)pImageBuffer;
	pNTHeader = (PIMAGE_NT_HEADERS)((DWORD)pImageBuffer + pDosHeader->e_lfanew);
	//打印NT头	
	pPEHeader = (PIMAGE_FILE_HEADER)(((DWORD)pNTHeader) + 4);  //加4个字节到了标准PE头
	pOptionHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPEHeader + IMAGE_SIZEOF_FILE_HEADER); //标准PE头+标准PE头的大小 20
	

	printf("===导出表====\n");
	
	printf("内存地址%x\n",pOptionHeader->DataDirectory[0].VirtualAddress);
	
	printf("内存大小%x\n",pOptionHeader->DataDirectory[0].Size);

	printf("===导入表====\n");

	printf("内存地址%x\n",pOptionHeader->DataDirectory[1].VirtualAddress);
	printf("内存大小%x\n",pOptionHeader->DataDirectory[1].Size);
	

	printf("===资源表====\n");
	
	printf("内存地址%x\n",pOptionHeader->DataDirectory[2].VirtualAddress);
	printf("内存大小%x\n",pOptionHeader->DataDirectory[2].Size);
	
	
	printf("===异常信息表====\n");
	
	printf("内存地址%x\n",pOptionHeader->DataDirectory[3].VirtualAddress);
	printf("内存大小%x\n",pOptionHeader->DataDirectory[3].Size);
	
	printf("===安全证书表====\n");
	printf("内存地址%x\n",pOptionHeader->DataDirectory[4].VirtualAddress);
	printf("内存大小%x\n",pOptionHeader->DataDirectory[4].Size);
	
	
	printf("===重定位表====\n");
	printf("内存地址%x\n",pOptionHeader->DataDirectory[5].VirtualAddress);
	printf("内存大小%x\n",pOptionHeader->DataDirectory[5].Size);	

	printf("===调试信息表证书表====\n");
	printf("内存地址%x\n",pOptionHeader->DataDirectory[6].VirtualAddress);
	printf("内存大小%x\n",pOptionHeader->DataDirectory[6].Size);
	

	printf("===版权所有表====\n");
	printf("内存地址%x\n",pOptionHeader->DataDirectory[7].VirtualAddress);
	printf("内存大小%x\n",pOptionHeader->DataDirectory[7].Size);

	printf("===全局指针表====\n");
	printf("内存地址%x\n",pOptionHeader->DataDirectory[8].VirtualAddress);
	printf("内存大小%x\n",pOptionHeader->DataDirectory[8].Size);

	printf("===TLS表====\n");
	printf("内存地址%x\n",pOptionHeader->DataDirectory[9].VirtualAddress);
	printf("内存大小%x\n",pOptionHeader->DataDirectory[9].Size);

	printf("===加载配置表====\n");
	printf("内存地址%x\n",pOptionHeader->DataDirectory[10].VirtualAddress);
	printf("内存大小%x\n",pOptionHeader->DataDirectory[10].Size);

	printf("===绑定导入表====\n");
	printf("内存地址%x\n",pOptionHeader->DataDirectory[11].VirtualAddress);
	printf("内存大小%x\n",pOptionHeader->DataDirectory[11].Size);

	printf("====IAT表===\n");
	printf("内存地址%x\n",pOptionHeader->DataDirectory[12].VirtualAddress);
	printf("内存大小%x\n",pOptionHeader->DataDirectory[12].Size);

	printf("====延迟导入===\n");
	printf("内存地址%x\n",pOptionHeader->DataDirectory[13].VirtualAddress);
	printf("内存大小%x\n",pOptionHeader->DataDirectory[13].Size);

	
	printf("====COM===\n");
	printf("内存地址%x\n",pOptionHeader->DataDirectory[14].VirtualAddress);
	printf("内存大小%x\n",pOptionHeader->DataDirectory[14].Size);
	

	
	printf("====保留===\n");
	printf("内存地址%x\n",pOptionHeader->DataDirectory[15].VirtualAddress);
	printf("内存大小%x\n",pOptionHeader->DataDirectory[15].Size);

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
	printf("FileOffset: %#x\n", dwFoa);
	
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

DWORD PrintExport(LPVOID pFileBuffer){
	
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
	

	
	printf("===导出表====\n");
	
	printf("导出表内存地址%x\n",pOptionHeader->DataDirectory[0].VirtualAddress);
	
	printf("导出表内存大小%x\n",pOptionHeader->DataDirectory[0].Size);
	
	printf("===导出表结构====\n");
	
	PIMAGE_EXPORT_DIRECTORY Export_Directory = (PIMAGE_EXPORT_DIRECTORY)((char*)pFileBuffer + RvaToFileOffset(pFileBuffer,pOptionHeader->DataDirectory[0].VirtualAddress));
	
	//cout << hex << "-exportName                  = " << Export_Directory->Name                   << endl;
	printf("Name:%s指向该导出表的文件名\n",(char*)pFileBuffer + RvaToFileOffset(pFileBuffer,Export_Directory->Name));
	printf("Base:%x导出表的起始序号\n",Export_Directory->Base);
	printf("NumberOfFunctions:%x 导出函数的函数个数\n",Export_Directory->NumberOfFunctions);
	printf("NumberOfNames:%x 以函数名字导出的函数个数\n",Export_Directory->NumberOfNames);
	printf("AddressOfFunctions: %x导出函数地址表RVA\n", Export_Directory-> AddressOfFunctions);
	printf("AddressOfNames;: %x导出函数名称表RVA\n", Export_Directory-> AddressOfNames);
	printf("AddressOfNameOrdinals;: %x导出函数序号表RVA\n\n", Export_Directory-> AddressOfNameOrdinals);


	
	printf("===函数地址表====\n");
	
	DWORD* AddressOfFunctionsAddress = (DWORD*)((char*)pFileBuffer + RvaToFileOffset(pFileBuffer,Export_Directory->AddressOfFunctions));

	//printf("====%d\n",AddressOfFunctionsAddress);
	
	for(int i =0;i<Export_Directory->NumberOfFunctions;i++,AddressOfFunctionsAddress++)
	{
		printf("函数地址%x\n",*AddressOfFunctionsAddress);
	}
	
	printf("\n");
	printf("===函数名称表====\n");
	DWORD* AddressOfNamesFunctionsAddress = (DWORD*)((char*)pFileBuffer + RvaToFileOffset(pFileBuffer,Export_Directory->AddressOfNames));


	for(int x =0;x<Export_Directory->NumberOfNames;x++,AddressOfNamesFunctionsAddress++)
	{
		printf("函数地址%x\n",*AddressOfNamesFunctionsAddress);
		printf("函数名称%s\n",(char*)pFileBuffer+RvaToFileOffset(pFileBuffer,*AddressOfNamesFunctionsAddress));
	}

	printf("\n");
	printf("===函数序号表====\n");
	
	//获取函数名称地址

	WORD* AddressOfNameOrdinalsAddress =(WORD*)((DWORD)pFileBuffer + RvaToFileOffset(pFileBuffer,Export_Directory->AddressOfNameOrdinals));
	

	for(int j =0;j<Export_Directory->NumberOfNames;j++,AddressOfNameOrdinalsAddress++)
	{

		printf("序号表%x\n",*AddressOfNameOrdinalsAddress + Export_Directory->Base);

	}
	
	
	return 1;
}


DWORD GetFunctionAddrByName(PVOID pFileBuffer,char* FuncName){
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
	//
	PIMAGE_EXPORT_DIRECTORY Export_Directory = (PIMAGE_EXPORT_DIRECTORY)((char*)pFileBuffer + RvaToFileOffset(pFileBuffer,pOptionHeader->DataDirectory[0].VirtualAddress));
	

	DWORD* AddressOfNamesFunctionsAddress = (DWORD*)((char*)pFileBuffer + RvaToFileOffset(pFileBuffer,Export_Directory->AddressOfNames));
	

	WORD* AddressOfNameOrdinalsAddress =(WORD*)((DWORD)pFileBuffer + RvaToFileOffset(pFileBuffer,Export_Directory->AddressOfNameOrdinals));
	
	DWORD* AddressOfFunctionsAddress = (DWORD*)((char*)pFileBuffer + RvaToFileOffset(pFileBuffer,Export_Directory->AddressOfFunctions));
	
	for(int x =0;x<Export_Directory->NumberOfNames;x++,AddressOfNamesFunctionsAddress++)
	{
	
		//printf("函数名称%s\n",(char*)pFileBuffer+RvaToFileOffset(pFileBuffer,*AddressOfNamesFunctionsAddress));
		if (*FuncName == *((char*)pFileBuffer+RvaToFileOffset(pFileBuffer,*AddressOfNamesFunctionsAddress)))
		{
			printf("函数地址为:%x\n",AddressOfFunctionsAddress[AddressOfNameOrdinalsAddress[x]]);
		}
	
	}



}



DWORD GetFunctionAddrByOrdinals(LPVOID pFileBuffer,DWORD FunctionOrdinals){

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
	//
	PIMAGE_EXPORT_DIRECTORY Export_Directory = (PIMAGE_EXPORT_DIRECTORY)((char*)pFileBuffer + RvaToFileOffset(pFileBuffer,pOptionHeader->DataDirectory[0].VirtualAddress));
	
	
	DWORD* AddressOfNamesFunctionsAddress = (DWORD*)((char*)pFileBuffer + RvaToFileOffset(pFileBuffer,Export_Directory->AddressOfNames));

	

	printf("函数地址为：%x\n",AddressOfNamesFunctionsAddress[FunctionOrdinals-Export_Directory->Base]);

	

	
}

DWORD PrintRelocation(LPVOID pFileBuffer){
	
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
	//重定位表开始
	printf("===重定位表====\n");
	printf("内存地址%x\n",pOptionHeader->DataDirectory[5].VirtualAddress);
	printf("内存大小%x\n",pOptionHeader->DataDirectory[5].Size);

	
	if(pOptionHeader->DataDirectory[5].VirtualAddress == 0){
		printf("%s","不存在重定位表...");
		return 0;
	}
 

	printf("===第一个重定位表结构====\n");
	PIMAGE_BASE_RELOCATION ReloCation = (_IMAGE_BASE_RELOCATION*)((char*)pFileBuffer + RvaToFileOffset(pFileBuffer,pOptionHeader->DataDirectory[5].VirtualAddress));

	
   int cnt = 0;
    while (true) {
          
		//先判断重定位表是否结束 0代表已经结束
		if (ReloCation->VirtualAddress != 0 && ReloCation->SizeOfBlock !=0)
		{
			printf("**********************************\n");
			printf("%x\n",ReloCation);
			//获取一共有多少块数据 数量 = 重定位表大小 -  8 (VirtualAddress大小 + SizeOfBlock 一共占了8字节) / 2 (因为每个块站2个字节的大小)
			int num = (ReloCation->SizeOfBlock - 8) / 2;
			for (int i =0;i<num-1;i++)  //循环表
			{
				
				//获取表的地址
			     WORD* offset = (WORD*)((char*)ReloCation+8+2*i);
				//判断高位是否 =3 高位是否大于= 0011
				if (*offset >= 0x3000)
				{
					   //*offset-0x3000 *offset 是2个字节 占了16位 我们只需要12位 所以减去高位0011 就变成了12位
					   printf("第%x项\t地址:%X\t偏移:%X\n", ReloCation->VirtualAddress, *offset-0x3000);
				}
			}
			ReloCation = (_IMAGE_BASE_RELOCATION*)((char*)ReloCation + ReloCation->SizeOfBlock);
			  cnt++;
		}else{
			break;
		}



    }
    printf("%d\n", cnt);

}