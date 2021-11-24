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
	//���ļ�
	pFile = fopen(lpszFile, "rb");
	if (!pFile)
	{
		printf(" �޷��� EXE �ļ�! ");
		return 0;
	}
	//��ȡ�ļ���С
	
	fseek(pFile, 0, SEEK_END);
	fileSize = ftell(pFile);
	fseek(pFile, 0, SEEK_SET);
	//���仺����	
	pFileTempBuffer = malloc(fileSize);
	
	
	if (!pFileTempBuffer)
	{
		printf(" ����ռ�ʧ��! ");
		fclose(pFile); //Ȼ����Ҫ�ر��ļ�
		return NULL;
	}
	
	size_t n = fread(pFileTempBuffer, fileSize, 1, pFile);
	if (!n)
	{
		printf(" ��ȡ����ʧ��! ");
		free(pFileTempBuffer);
		fclose(pFile);
		return NULL;
	}
	
	*pFileBuffer = pFileTempBuffer;
	pFileTempBuffer = NULL;
	//�ر��ļ�	
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
		printf("��ȡ���ڴ��pImageBuffer��Ч��\n");
		return 0;
	}
	//�ж��ǲ���exe�ļ�
	if(*((PWORD)pFilebuff) != IMAGE_DOS_SIGNATURE)
	{
		printf("����MZ��־������exe�ļ���\n");
		return 0;
	}
	pDosHeader = (PIMAGE_DOS_HEADER)pFilebuff;
	if(*((PDWORD)((BYTE *)pFilebuff + pDosHeader->e_lfanew)) != IMAGE_NT_SIGNATURE){
		printf("����Ч��PE��־\n");
		return 0;
	}
	
	
	
	
	//��ȡpFileBuffer ��ȡDOSͷ��PEͷ���ڱ����Ϣ
	pDosHeader =(PIMAGE_DOS_HEADER)pFilebuff;
	pNTHeader = (PIMAGE_NT_HEADERS)((DWORD)pFilebuff + pDosHeader->e_lfanew);
	//��ӡNTͷ	
	pPEHeader = (PIMAGE_FILE_HEADER)(((DWORD)pNTHeader) + 4);  //��4���ֽڵ��˱�׼PEͷ
	pOptionHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPEHeader + IMAGE_SIZEOF_FILE_HEADER); //��׼PEͷ+��׼PEͷ�Ĵ�С 20
	pSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)pOptionHeader + pPEHeader->SizeOfOptionalHeader);		
	

	printf("===��Դ��====\n");
	printf("��Դ���ڴ��ַ:%x\n",pOptionHeader->DataDirectory[2].VirtualAddress);
	printf("��Դ���ʴ��С:%x\n",pOptionHeader->DataDirectory[2].Size);
	



	//��λ��Դ��	
	ImageResourceTable = (PIMAGE_RESOURCE_DIRECTORY)((char*)pFilebuff + RvaToFileOffset(pFilebuff,pOptionHeader->DataDirectory[2].VirtualAddress));


	GetResourceEntry(ImageResourceTable,0);

	getchar();
	return 1;


	//�����ж��ٸ���Դ��
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
		case 1: Tempstr = "���"; break;
		case 2: Tempstr = "λͼ"; break;
		case 3: Tempstr = "ͼ��"; break;
		case 4: Tempstr = "�˵�"; break;
		case 5: Tempstr = "�Ի���"; break;
		case 6: Tempstr = "�ַ���"; break;
		case 7: Tempstr = "����Ŀ¼"; break;
		case 8: Tempstr = "����"; break;
		case 9: Tempstr = "���ټ�"; break;
		case 10: Tempstr = "RC����"; break;
		case 11: Tempstr = "��Ϣ��"; break;
		case 12: Tempstr = "�����"; break;
		case 14: Tempstr = "ͼ����"; break;
		case 16: Tempstr = "�汾��Ϣ"; break;
		case 23: Tempstr = "HTML"; break;
		case 24: Tempstr = "�嵥"; break;
		default: Tempstr="δ֪"; break;
                               
		}
	    printf("��Դ����ID:%p %s ��ţ� %d\n", ImgResDirEntry[i].Id, Tempstr,ImgResDirEntry[i].NameOffset);
		
		if(ImgResDirEntry[i].OffsetToData & 0x80000000){
			//��һ����Դ����ʼ��ַ��
			PIMAGE_RESOURCE_DIRECTORY TwoResourceTable = (PIMAGE_RESOURCE_DIRECTORY)((char*)ImageResourceTable+ImgResDirEntry[i].OffsetToDirectory);
			//�����ж��ٸ���Դ��
			DWORD TwoNum = TwoResourceTable->NumberOfIdEntries + TwoResourceTable->NumberOfNamedEntries;
		
			printf("      ����Ŀ¼��Դ�����%d\n",TwoNum);
			PIMAGE_RESOURCE_DIRECTORY_ENTRY TwoDirEntry= (PIMAGE_RESOURCE_DIRECTORY_ENTRY)(TwoResourceTable+1);
			for (DWORD j =0 ; j<TwoNum;j++)
			{
				if (~TwoDirEntry[j].NameIsString)
				{
					printf("     ������ԴId: %d\n",TwoDirEntry[j].Id);
					//������Դ����ʼ��ַ
					if(TwoDirEntry[j].OffsetToData & 0x80000000){
						PIMAGE_RESOURCE_DIRECTORY ThreeResourceTable = (PIMAGE_RESOURCE_DIRECTORY)((char*)ImageResourceTable+TwoDirEntry[j].OffsetToDirectory);
						DWORD ThreeNum = ThreeResourceTable->NumberOfIdEntries + ThreeResourceTable->NumberOfNamedEntries;
						PIMAGE_RESOURCE_DIRECTORY_ENTRY ThreeDirEntry= (PIMAGE_RESOURCE_DIRECTORY_ENTRY)(ThreeResourceTable+1);
						printf("         ����Ŀ¼��Դ�����%d\n",ThreeNum);
						for (DWORD k=0;k<ThreeNum;k++)
						{
							if(~ThreeDirEntry[k].NameIsString){
								printf("             ����Ŀ¼ID:%d\n",ThreeDirEntry[k].Id);
								
								if (~(ThreeDirEntry[k].DataIsDirectory))
								{
									//PIMAGE_RESOURCE_DATA_ENTRY 
									
									PIMAGE_DATA_DIRECTORY imgResData = (PIMAGE_DATA_DIRECTORY)((char*)ImageResourceTable + ThreeDirEntry[k].OffsetToData);
									printf("	        IMAGE_RESOURCE_DATA_ENTRY��ַ:%x\n",imgResData);
									printf("	        IMAGE_RESOURCE_DATA_ENTRY->VirtualAddress��ַ:%x\n",imgResData->VirtualAddress);
									printf("	        IMAGE_RESOURCE_DATA_ENTRY->��С:%x\n",imgResData->Size);
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
	

	
	//�����ж��ٸ���Դ��
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
				printf("	        IMAGE_RESOURCE_DATA_ENTRY��ַ:%x\n",imgResData);
				printf("	        IMAGE_RESOURCE_DATA_ENTRY->VirtualAddress��ַ:%x\n",imgResData->VirtualAddress);
						printf("	        IMAGE_RESOURCE_DATA_ENTRY->��С:%x\n",imgResData->Size);
			}
		}

		
		
	}


};




void GetResourceName(DWORD depth,PIMAGE_RESOURCE_DIRECTORY_ENTRY ImgResDirEntry){
	
	char* Tempstr;
	if (ImgResDirEntry->NameIsString)
	{
		printf("���λֵΪ1 name%x\n",ImgResDirEntry->Id);
	}else{
		switch (depth)
		{
		case 0:
			switch (ImgResDirEntry->Id)
			{
			case 1: Tempstr = "���"; break;
			case 2: Tempstr = "λͼ"; break;
			case 3: Tempstr = "ͼ��"; break;
			case 4: Tempstr = "�˵�"; break;
			case 5: Tempstr = "�Ի���"; break;
			case 6: Tempstr = "�ַ���"; break;
			case 7: Tempstr = "����Ŀ¼"; break;
			case 8: Tempstr = "����"; break;
			case 9: Tempstr = "���ټ�"; break;
			case 10: Tempstr = "RC����"; break;
			case 11: Tempstr = "��Ϣ��"; break;
			case 12: Tempstr = "�����"; break;
			case 14: Tempstr = "ͼ����"; break;
			case 16: Tempstr = "�汾��Ϣ"; break;
			case 23: Tempstr = "HTML"; break;
			case 24: Tempstr = "�嵥"; break;
			default: Tempstr= (char*)ImgResDirEntry->Id; break;
			}
			break;
			
			case 1: Tempstr = "δ֪"; break;
				
			case 2:
				
				printf("�������Ա�ʶ: 0x%04X", ImgResDirEntry->Id);
				break;
			default:
				Tempstr = "δ֪";
				break;
		}
		
		printf("��%d��Ŀ¼������:%x\n ",depth,depth);

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
	pPEHeader = (PIMAGE_FILE_HEADER)((DWORD)pNTHeader + 4); // �������ǿ������ת��
	pOptionHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPEHeader + IMAGE_SIZEOF_FILE_HEADER);
	pSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)pOptionHeader + pPEHeader->SizeOfOptionalHeader);
	
	
	if (dwFoa <= pOptionHeader->SizeOfHeaders)
		return (DWORD)dwFoa;
	else
	{
		for (int n = 0; n < pPEHeader->NumberOfSections; n++, pSectionHeader++)
		{	//�ж� :   �ļ�����+�ļ�ƫ��>file_panyi>�ļ�ƫ��  (�������ļ����ĸ�����)
			if ((dwFoa >= pSectionHeader->PointerToRawData) && (dwFoa < pSectionHeader->PointerToRawData + pSectionHeader->SizeOfRawData))
			{
				return dwFoa - pSectionHeader->PointerToRawData + pSectionHeader->VirtualAddress;
			}
		}
	}
	printf("FoaToRva failed��\n");
	return 0;
}








DWORD RvaToFileOffset(PVOID pFileBuffer, DWORD dwRva){
	
	//����PEͷ����Ϣ
	PIMAGE_DOS_HEADER pDosHeader = NULL;
	PIMAGE_NT_HEADERS pNTHeader = NULL;
	PIMAGE_FILE_HEADER pPEHeader = NULL;
	PIMAGE_OPTIONAL_HEADER32 pOptionHeader = NULL;
	PIMAGE_SECTION_HEADER pSectionHeader = NULL;
	
	if(!pFileBuffer)
	{
		printf("��ȡ���ڴ��pfilebuffer��Ч��\n");
		return 0;
	}
	//�ж��ǲ���exe�ļ�
	if(*((PWORD)pFileBuffer) != IMAGE_DOS_SIGNATURE)
	{
		printf("����MZ��־������exe�ļ���\n");
		return 0;
	}
	pDosHeader = (PIMAGE_DOS_HEADER)pFileBuffer;
	if(*((PDWORD)((BYTE *)pFileBuffer + pDosHeader->e_lfanew)) != IMAGE_NT_SIGNATURE){
		printf("����Ч��PE��־\n");
		return 0;
	}
	
	//��ȡpFileBuffer ��ȡDOSͷ��PEͷ���ڱ����Ϣ
	pDosHeader =(PIMAGE_DOS_HEADER)pFileBuffer;
	pNTHeader = (PIMAGE_NT_HEADERS)((DWORD)pFileBuffer + pDosHeader->e_lfanew);
	//��ӡNTͷ	
	pPEHeader = (PIMAGE_FILE_HEADER)(((DWORD)pNTHeader) + 4);  //��4���ֽڵ��˱�׼PEͷ
	pOptionHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPEHeader + IMAGE_SIZEOF_FILE_HEADER); //��׼PEͷ+��׼PEͷ�Ĵ�С 20
	
	
	pSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)pOptionHeader + pPEHeader->SizeOfOptionalHeader);
	
	
	//PIMAGE_SECTION_HEADER pSectionTemp = pSectionHeader;
	
	if (dwRva <= pOptionHeader->SizeOfHeaders)
		return (DWORD)dwRva;
	else
	{
		for (int n = 0; n < pPEHeader->NumberOfSections; n++, pSectionHeader++)
		{	
			//�ж� :   �ļ�����+�ļ�ƫ��>file_panyi>�ļ�ƫ��  (�������ļ����ĸ�����)
			//  ����Ҫ�ж����Ǹ����У�����С�ڵ�ǰ�ڵĴ�С
			if ((dwRva >= pSectionHeader->VirtualAddress) && (dwRva < pSectionHeader->VirtualAddress + pSectionHeader->Misc.VirtualSize))
			{
				//�ļ�ƫ�� =  ��ǰ�ڴ�ƫ��-��ǰ�ڴ��ַ+ ��ǰ�ڴ��е��ļ�ƫ��
				return dwRva - pSectionHeader->VirtualAddress + pSectionHeader->PointerToRawData;
			}
		}
	}
	printf("RvaToFoa failed��\n");
	return 0;
}

