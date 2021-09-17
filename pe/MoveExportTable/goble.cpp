// goble.cpp: implementation of the goble class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "goble.h"
#include "stdafx.h"
#include <malloc.h>
#include <math.h>
#include <string>
#include <stdlib.h>
#include <stdio.h>
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



DWORD MoveExprotTable(LPVOID pFilebuff,LPVOID* pNewbuff){

	PIMAGE_DOS_HEADER pDosHeader = NULL;
	PIMAGE_NT_HEADERS pNTHeader = NULL;
	PIMAGE_FILE_HEADER pPEHeader = NULL;
	PIMAGE_OPTIONAL_HEADER32 pOptionHeader = NULL;
	PIMAGE_SECTION_HEADER pSectionHeader = NULL;
	LPVOID pTempBuffer = NULL;



	if(!pFilebuff)
	{
		printf("读取到内存的pImageBuffer无效！\n");
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
	
	
	//获取剩余大小 //SizeOfHeaders(总的大小)4096 -   ( pSectionHeader(一个地址，文件开始的地方偏移了pSectionHeader的大小的字节)  -    (文件偏移+pPEHeader->NumberOfSections * 40)  一个地址 文件开始的地方偏移了节的大小的字节 并不是节在文件中的偏移地址，所以会比 pSectionHeader小)
	DWORD whiteSpaceSize = pOptionHeader->SizeOfHeaders - ((DWORD)pSectionHeader - (DWORD)pFilebuff + pPEHeader->NumberOfSections * 40);
	  
	if (whiteSpaceSize < 80)
	{
		printf("空间不足");
		return false;
    }
	//新增一个节
	//1.修改SizeOfImage大小



	pOptionHeader->SizeOfImage +=  0x1000;

	//修改节表NumberOfSection数量
	

	pTempBuffer=malloc(pOptionHeader->SizeOfImage);

	//如果空间足，但是节最后的80个字节不等于0 就需要将PE头往上移动,把垃圾数据清空 
	memset(pTempBuffer, 0, pOptionHeader->SizeOfImage);
	
	//把整个原来的pImageBuffer 复制到新的imagefile
	memcpy(pTempBuffer,pFilebuff,pOptionHeader->SizeOfImage);
	
	for (int i=0;i<80;i++)
	{
		if (*((char*)pTempBuffer +whiteSpaceSize + i) !=0 )
		{
			
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
	
	LPVOID NewBuffer = malloc(pOptionHeader->SizeOfImage);//申请内存
	memset(NewBuffer, 0, pOptionHeader->SizeOfImage);//初始化内存
    memcpy(NewBuffer, pFilebuff,pOptionHeader->SizeOfImage);//复制内存 
	
	
	


	//定位导出表
	PIMAGE_EXPORT_DIRECTORY Export_Directory = (PIMAGE_EXPORT_DIRECTORY)((char*)NewBuffer + RvaToFileOffset(NewBuffer,pOptionHeader->DataDirectory[0].VirtualAddress));
	//2.复制函数表：AddressOfFunction(size:NumberOfFunctions * 4) 到新增的节；
	
	DWORD* AddressOfFunctionsAddress = (DWORD*)((char*)NewBuffer + RvaToFileOffset(NewBuffer,Export_Directory->AddressOfFunctions));

	//定位新节表的地址
	LPVOID pNewSecAddr = (LPVOID)((DWORD)NewBuffer+pNewSec->PointerToRawData);
	

	memcpy(pNewSecAddr,AddressOfFunctionsAddress,Export_Directory->NumberOfFunctions * 4);

	//3.复制序号表：AddressOfNameOrdinals(size:NumberOfNames * 2) 到新增的节中；
	//获取复制函数地址之后的偏移
	pNewSecAddr = (LPVOID)((char*)pNewSecAddr +Export_Directory->NumberOfFunctions * 4);
	
	WORD* AddressOfNameOrdinalsAddress =(WORD*)((DWORD)NewBuffer + RvaToFileOffset(NewBuffer,Export_Directory->AddressOfNameOrdinals));
	memcpy(pNewSecAddr,AddressOfNameOrdinalsAddress,Export_Directory->NumberOfNames * 2);
	
	//4.复制名字表
	//获取节中的偏移
	pNewSecAddr = (LPVOID)((char*)pNewSecAddr + Export_Directory->NumberOfNames * 2);
	//获取函数名字标的地址
	DWORD* AddressOfNamesFunctionsAddress = (DWORD*)((char*)NewBuffer + RvaToFileOffset(NewBuffer,Export_Directory->AddressOfNames));
	PDWORD NameAddr = (PDWORD)pNewSecAddr;//这里存储一下函数地址名称表的地址，以便后边移动名称的时候修改相应地址
	//复制名字地址表
	memcpy(pNewSecAddr,AddressOfNamesFunctionsAddress,Export_Directory->NumberOfNames * 4);
	
	//复制名字
	pNewSecAddr = (LPVOID)((char*)pNewSecAddr + Export_Directory->NumberOfNames * 4);
	
	//DWORD NameAddOffset = (DWORD)pNewSecAddr - (DWORD)NewBuffer;//函数名称偏移
	for(int x =0;x<Export_Directory->NumberOfNames;x++,AddressOfNamesFunctionsAddress++){
		
		//printf("函数名称%s\n",((char*)NewBuffer+RvaToFileOffset(NewBuffer,*AddressOfNamesFunctionsAddress));
		
		//名字在文件中的偏移
		DWORD NameOffset = DWORD(*AddressOfNamesFunctionsAddress);
		//获取名字地址
		char* FuncName = (char*)((char*)NewBuffer + NameOffset);
			
		//复制函数名称偏移到新的函数名称表 
		*NameAddr = (DWORD)(FoaToImageOffset(NewBuffer,NameOffset));   //AddressOfName中的地址是内存地址，所以需要把文件地址转换为内存地
		
		//函数地址+4
		NameAddr = (PDWORD)((DWORD)NameAddr + 4);

		size_t l = 0;
		while(FuncName[l] !=0){
			l += 1;
		}
		//复制名字到内存 \0结束 所以需要+1
		memcpy(pNewSecAddr,FuncName,l+1);
		pNewSecAddr = (LPVOID)((DWORD)pNewSecAddr + l+1); //地址偏移

	}

	
	//复制导出表到节
    memcpy(pNewSecAddr,Export_Directory,pOptionHeader->DataDirectory[0].Size);
	
	//新的导出表的地址
	PIMAGE_EXPORT_DIRECTORY  NewExport_Directory = (PIMAGE_EXPORT_DIRECTORY)((char*)pNewSecAddr);

	//修改新的导出表的AddressOfFunctions、AddressOfNameOrdinals、AddressOfNames地址，这几个地址都是内存地址 里面存放的值才是文件偏移   需要FOA->RVA  
    NewExport_Directory->AddressOfFunctions = FoaToImageOffset(NewBuffer,pNewSec->PointerToRawData);			//AddressOfFunctions 内存地址 非文件偏移 需要转换成内存地址
	NewExport_Directory->AddressOfNameOrdinals = FoaToImageOffset(NewBuffer,pNewSec->PointerToRawData + NewExport_Directory->NumberOfFunctions * 4);  //AddressOfNameOrdinals 内存地址 非文件偏移 需要转换成内存地址非文件偏移
    NewExport_Directory->AddressOfNames = FoaToImageOffset(NewBuffer,pNewSec->PointerToRawData + NewExport_Directory->NumberOfFunctions * 4 + NewExport_Directory->NumberOfNames * 2);  //AddressOfNames 非文件偏移 需要转换成内存地址非文件偏移
	

	pDosHeader =(PIMAGE_DOS_HEADER)NewBuffer;
	pNTHeader = (PIMAGE_NT_HEADERS)((DWORD)NewBuffer + pDosHeader->e_lfanew);
	//打印NT头	
	pPEHeader = (PIMAGE_FILE_HEADER)(((DWORD)pNTHeader) + 4);  //加4个字节到了标准PE头
	pOptionHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPEHeader + IMAGE_SIZEOF_FILE_HEADER); //标准PE头+标准PE头的大小 20
	pSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)pOptionHeader + pPEHeader->SizeOfOptionalHeader);
	
	//新导出表的地址 新导出表的地址= NewExport_Directory 内存地址 - 文件开始的位置 = 偏移
	pOptionHeader->DataDirectory[0].VirtualAddress = FoaToImageOffset(NewBuffer,(DWORD)NewExport_Directory - (DWORD)NewBuffer);


	FILE* fp = fopen("C://project/testNew.dll","wb+");
	size_t n = 	fwrite(NewBuffer,pOptionHeader->SizeOfImage,1,fp);
	if(!n){
		printf("fwrite数据写入失败...\n");
		fclose(fp);
		return ERROR;
	}
	

	printf("存盘成功...\n");
	fclose(fp);



	return 0;
};






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


DWORD MoveRectTable(LPVOID pFilebuff,LPVOID* pNewbuff){
	
	PIMAGE_DOS_HEADER pDosHeader = NULL;
	PIMAGE_NT_HEADERS pNTHeader = NULL;
	PIMAGE_FILE_HEADER pPEHeader = NULL;
	PIMAGE_OPTIONAL_HEADER32 pOptionHeader = NULL;
	PIMAGE_SECTION_HEADER pSectionHeader = NULL;
	LPVOID pTempBuffer = NULL;



	if(!pFilebuff)
	{
		printf("读取到内存的pImageBuffer无效！\n");
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
	
	
	//获取剩余大小 //SizeOfHeaders(总的大小)4096 -   ( pSectionHeader(一个地址，文件开始的地方偏移了pSectionHeader的大小的字节)  -    (文件偏移+pPEHeader->NumberOfSections * 40)  一个地址 文件开始的地方偏移了节的大小的字节 并不是节在文件中的偏移地址，所以会比 pSectionHeader小)
	DWORD whiteSpaceSize = pOptionHeader->SizeOfHeaders - ((DWORD)pSectionHeader - (DWORD)pFilebuff + pPEHeader->NumberOfSections * 40);
	  
	if (whiteSpaceSize < 80)
	{
		printf("空间不足");
		return false;
    }
	//新增一个节
	//1.修改SizeOfImage大小



	pOptionHeader->SizeOfImage +=  0x1000;

	//修改节表NumberOfSection数量
	

	pTempBuffer=malloc(pOptionHeader->SizeOfImage);

	//如果空间足，但是节最后的80个字节不等于0 就需要将PE头往上移动,把垃圾数据清空 
	memset(pTempBuffer, 0, pOptionHeader->SizeOfImage);
	
	//把整个原来的pImageBuffer 复制到新的imagefile
	memcpy(pTempBuffer,pFilebuff,pOptionHeader->SizeOfImage);
	
	for (int i=0;i<80;i++)
	{
		if (*((char*)pTempBuffer +whiteSpaceSize + i) !=0 )
		{
			
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
	
	LPVOID NewBuffer = malloc(pOptionHeader->SizeOfImage);//申请内存
	memset(NewBuffer, 0, pOptionHeader->SizeOfImage);//初始化内存
    memcpy(NewBuffer, pFilebuff,pOptionHeader->SizeOfImage);//复制内存 

	printf("===重定位表====\n");
	printf("内存地址%x\n",pOptionHeader->DataDirectory[5].VirtualAddress);
	printf("内存大小%x\n",pOptionHeader->DataDirectory[5].Size);
	
	if(pOptionHeader->DataDirectory[5].VirtualAddress == 0){
		printf("%s","不存在重定位表...");
		return 0;
	}
	
	printf("===第一个重定位表结构====\n");
	PIMAGE_BASE_RELOCATION ReloCation = (_IMAGE_BASE_RELOCATION*)((char*)NewBuffer + RvaToFileOffset(NewBuffer,pOptionHeader->DataDirectory[5].VirtualAddress));
	
	int ReloSize = 0;
    while (true) {
          
		//先判断重定位表是否结束 0代表已经结束
		if (ReloCation->VirtualAddress != 0 && ReloCation->SizeOfBlock !=0)
		{
			ReloSize += ReloCation->SizeOfBlock;
			//节的位置相加
			ReloCation = (_IMAGE_BASE_RELOCATION*)((char*)ReloCation + ReloCation->SizeOfBlock);
		}else{
			break;
		}
	
    }
	//定位新节表的地址
	LPVOID pNewSecAddr = (LPVOID)((DWORD)NewBuffer+pNewSec->PointerToRawData);
	memcpy(pNewSecAddr, ReloCation,ReloSize);

	pDosHeader =(PIMAGE_DOS_HEADER)NewBuffer;
	pNTHeader = (PIMAGE_NT_HEADERS)((DWORD)NewBuffer + pDosHeader->e_lfanew);
	//打印NT头	
	pPEHeader = (PIMAGE_FILE_HEADER)(((DWORD)pNTHeader) + 4);  //加4个字节到了标准PE头
	pOptionHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPEHeader + IMAGE_SIZEOF_FILE_HEADER); //标准PE头+标准PE头的大小 20
	pSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)pOptionHeader + pPEHeader->SizeOfOptionalHeader);
	
	//新导出表的地址 新导出表的地址= NewExport_Directory 内存地址 - 文件开始的位置 = 偏移
	pOptionHeader->DataDirectory[5].VirtualAddress = FoaToImageOffset(NewBuffer,(DWORD)pNewSecAddr-(DWORD)NewBuffer);	

	FILE* fp = fopen("C://project/testNext.dll","wb+");
	size_t n = 	fwrite(NewBuffer,pOptionHeader->SizeOfImage,1,fp);
	if(!n){
		printf("fwrite数据写入失败...\n");
		fclose(fp);
		return ERROR;
	}

	printf("存盘成功...\n");
	fclose(fp);
	return 0;
}

DWORD repairReloCation(LPVOID pFilebuff,LPVOID* pNewbuff){
	
	PIMAGE_DOS_HEADER pDosHeader = NULL;
	PIMAGE_NT_HEADERS pNTHeader = NULL;
	PIMAGE_FILE_HEADER pPEHeader = NULL;
	PIMAGE_OPTIONAL_HEADER32 pOptionHeader = NULL;
	PIMAGE_SECTION_HEADER pSectionHeader = NULL;
	LPVOID pTempBuffer = NULL;

	if(!pFilebuff)
	{
		printf("读取到内存的pImageBuffer无效！\n");
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
	PIMAGE_BASE_RELOCATION ReloCation = (_IMAGE_BASE_RELOCATION*)((char*)pFilebuff + RvaToFileOffset(pFilebuff,pOptionHeader->DataDirectory[5].VirtualAddress));
	
    pOptionHeader ->ImageBase = pOptionHeader->ImageBase + 0x10000000;

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
					   //ReloCation->VirtualAddress
					   DWORD rva = ReloCation->VirtualAddress + (*offset-0x3000);        //计算在内存中偏移
					   DWORD foa = RvaToFileOffset(pFilebuff, rva);    //转成foa
					   PDWORD changeAddr = (PDWORD)((DWORD)pFilebuff + foa);    //获取真正在文件中的文件偏移
					   *changeAddr = *changeAddr + 0x10000000; //原值加上imagebase加的大小
		

				}
			}
			ReloCation = (_IMAGE_BASE_RELOCATION*)((char*)ReloCation + ReloCation->SizeOfBlock);
			  cnt++;
		}else{
			break;
		}
	
    }


    LPVOID NewBuffer = malloc(pOptionHeader->SizeOfImage);//申请内存
	memset(NewBuffer, 0, pOptionHeader->SizeOfImage);//初始化内存
    memcpy(NewBuffer, pFilebuff,pOptionHeader->SizeOfImage);//复制内存 
	


	FILE* fp = fopen("C://project/testNext.dll","wb+");
	size_t n = 	fwrite(NewBuffer,pOptionHeader->SizeOfImage,1,fp);
	if(!n){
		printf("fwrite数据写入失败...\n");
		fclose(fp);
		return ERROR;
	}
	
	printf("存盘成功...\n");
	fclose(fp);
	return 0;


}