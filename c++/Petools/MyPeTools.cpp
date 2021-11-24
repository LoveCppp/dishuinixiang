// MyPeTools.cpp: implementation of the MyPeTools class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MyPeTools.h"
#include <malloc.h>
 
PIMAGE_RESOURCE_DIRECTORY ImageResourceTable;


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



DWORD readPeResoure(LPVOID pFilebuff)
{

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
	

	printf("===资源表====\n");
	printf("资源表内存地址:%x\n",pOptionHeader->DataDirectory[2].VirtualAddress);
	printf("资源表资存大小:%x\n",pOptionHeader->DataDirectory[2].Size);
	



	//定位资源表	
	ImageResourceTable = (PIMAGE_RESOURCE_DIRECTORY)((char*)pFilebuff + RvaToFileOffset(pFilebuff,pOptionHeader->DataDirectory[2].VirtualAddress));


	GetResourceEntry(ImageResourceTable,0);

	getchar();
	return 1;


	//计算有多少个资源表
	DWORD resTableNum = ImageResourceTable->NumberOfIdEntries + ImageResourceTable->NumberOfNamedEntries;
	
	  //IMAGE_RESOURCE_DIRECTORY_ENTRY
	PIMAGE_RESOURCE_DIRECTORY_ENTRY ImgResDirEntry= (PIMAGE_RESOURCE_DIRECTORY_ENTRY)((DWORD)ImageResourceTable+sizeof(IMAGE_RESOURCE_DIRECTORY));
	for (DWORD i =0;i<resTableNum;i++)
	{
		char* Tempstr;
		if (ImgResDirEntry[i].NameIsString)
		{
			printf("===%d",ImgResDirEntry[i].NameIsString);
		}
		switch (ImgResDirEntry[i].Id)
		{
		case 1: Tempstr = "光标"; break;
		case 2: Tempstr = "位图"; break;
		case 3: Tempstr = "图标"; break;
		case 4: Tempstr = "菜单"; break;
		case 5: Tempstr = "对话框"; break;
		case 6: Tempstr = "字符串"; break;
		case 7: Tempstr = "字体目录"; break;
		case 8: Tempstr = "字体"; break;
		case 9: Tempstr = "加速键"; break;
		case 10: Tempstr = "RC数据"; break;
		case 11: Tempstr = "消息表"; break;
		case 12: Tempstr = "光标组"; break;
		case 14: Tempstr = "图标组"; break;
		case 16: Tempstr = "版本信息"; break;
		case 23: Tempstr = "HTML"; break;
		case 24: Tempstr = "清单"; break;
		default: Tempstr="未知"; break;
                               
		}
	    printf("资源类型ID:%p %s 编号： %d\n", ImgResDirEntry[i].Id, Tempstr,ImgResDirEntry[i].NameOffset);
		
		if(ImgResDirEntry[i].OffsetToData & 0x80000000){
			//下一层资源的起始地址：
			PIMAGE_RESOURCE_DIRECTORY TwoResourceTable = (PIMAGE_RESOURCE_DIRECTORY)((char*)ImageResourceTable+ImgResDirEntry[i].OffsetToDirectory);
			//计算有多少个资源表
			DWORD TwoNum = TwoResourceTable->NumberOfIdEntries + TwoResourceTable->NumberOfNamedEntries;
		
			printf("      二级目录资源表个数%d\n",TwoNum);
			PIMAGE_RESOURCE_DIRECTORY_ENTRY TwoDirEntry= (PIMAGE_RESOURCE_DIRECTORY_ENTRY)(TwoResourceTable+1);
			for (DWORD j =0 ; j<TwoNum;j++)
			{
				if (~TwoDirEntry[j].NameIsString)
				{
					printf("     二级资源Id: %d\n",TwoDirEntry[j].Id);
					//三层资源的起始地址
					if(TwoDirEntry[j].OffsetToData & 0x80000000){
						PIMAGE_RESOURCE_DIRECTORY ThreeResourceTable = (PIMAGE_RESOURCE_DIRECTORY)((char*)ImageResourceTable+TwoDirEntry[j].OffsetToDirectory);
						DWORD ThreeNum = ThreeResourceTable->NumberOfIdEntries + ThreeResourceTable->NumberOfNamedEntries;
						PIMAGE_RESOURCE_DIRECTORY_ENTRY ThreeDirEntry= (PIMAGE_RESOURCE_DIRECTORY_ENTRY)(ThreeResourceTable+1);
						printf("         三级目录资源表个数%d\n",ThreeNum);
						for (DWORD k=0;k<ThreeNum;k++)
						{
							if(~ThreeDirEntry[k].NameIsString){
								printf("             三级目录ID:%d\n",ThreeDirEntry[k].Id);
								
								if (~(ThreeDirEntry[k].DataIsDirectory))
								{
									//PIMAGE_RESOURCE_DATA_ENTRY 
									
									PIMAGE_DATA_DIRECTORY imgResData = (PIMAGE_DATA_DIRECTORY)((char*)ImageResourceTable + ThreeDirEntry[k].OffsetToData);
									printf("	        IMAGE_RESOURCE_DATA_ENTRY地址:%x\n",imgResData);
									printf("	        IMAGE_RESOURCE_DATA_ENTRY->VirtualAddress地址:%x\n",imgResData->VirtualAddress);
									printf("	        IMAGE_RESOURCE_DATA_ENTRY->大小:%x\n",imgResData->Size);
								}
							}
							
						}
					}
				}

			
			}
			
		}
		

	}
	
	return 1;
}
		
	
         
void GetResourceEntry(PIMAGE_RESOURCE_DIRECTORY resTable,DWORD depth){
	

	
	//计算有多少个资源表
	DWORD resTableNum = resTable->NumberOfIdEntries + resTable->NumberOfNamedEntries;
	//IMAGE_RESOURCE_DIRECTORY_ENTRY
	PIMAGE_RESOURCE_DIRECTORY_ENTRY ImgResDirEntry= (PIMAGE_RESOURCE_DIRECTORY_ENTRY)(resTable+1);
	
	for (DWORD i =0;i<resTableNum;i++,ImgResDirEntry++){
	//	
		if (~ImgResDirEntry->NameIsString)
		{
			
			if(ImgResDirEntry->DataIsDirectory){
				//GetResourceName(depth,ImgResDirEntry);
				PIMAGE_RESOURCE_DIRECTORY TwoResourceTable = (PIMAGE_RESOURCE_DIRECTORY)((char*)ImageResourceTable+ImgResDirEntry->OffsetToDirectory);
				depth++;
				GetResourceEntry(TwoResourceTable,depth);
			}else{
				printf("ID:%x\n",ImgResDirEntry->Id);
				PIMAGE_DATA_DIRECTORY imgResData = (PIMAGE_DATA_DIRECTORY)((char*)ImageResourceTable + ImgResDirEntry->OffsetToData);
				printf("	        IMAGE_RESOURCE_DATA_ENTRY地址:%x\n",imgResData);
				printf("	        IMAGE_RESOURCE_DATA_ENTRY->VirtualAddress地址:%x\n",imgResData->VirtualAddress);
						printf("	        IMAGE_RESOURCE_DATA_ENTRY->大小:%x\n",imgResData->Size);
			}
		}

		
		
	}


};




void GetResourceName(DWORD depth,PIMAGE_RESOURCE_DIRECTORY_ENTRY ImgResDirEntry){
	
	char* Tempstr;
	if (ImgResDirEntry->NameIsString)
	{
		printf("最高位值为1 name%x\n",ImgResDirEntry->Id);
	}else{
		switch (depth)
		{
		case 0:
			switch (ImgResDirEntry->Id)
			{
			case 1: Tempstr = "光标"; break;
			case 2: Tempstr = "位图"; break;
			case 3: Tempstr = "图标"; break;
			case 4: Tempstr = "菜单"; break;
			case 5: Tempstr = "对话框"; break;
			case 6: Tempstr = "字符串"; break;
			case 7: Tempstr = "字体目录"; break;
			case 8: Tempstr = "字体"; break;
			case 9: Tempstr = "加速键"; break;
			case 10: Tempstr = "RC数据"; break;
			case 11: Tempstr = "消息表"; break;
			case 12: Tempstr = "光标组"; break;
			case 14: Tempstr = "图标组"; break;
			case 16: Tempstr = "版本信息"; break;
			case 23: Tempstr = "HTML"; break;
			case 24: Tempstr = "清单"; break;
			default: Tempstr= (char*)ImgResDirEntry->Id; break;
			}
			break;
			
			case 1: Tempstr = "未知"; break;
				
			case 2:
				
				printf("区域语言标识: 0x%04X", ImgResDirEntry->Id);
				break;
			default:
				Tempstr = "未知";
				break;
		}
		
		printf("第%d级目录，类型:%x\n ",depth,depth);

	}
	




                 
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

