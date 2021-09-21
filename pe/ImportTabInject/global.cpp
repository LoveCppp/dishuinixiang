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


//��ȡ�ļ�����

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




//�����ע��
DWORD InjectDll(LPVOID pFilebuff){
	//����PEͷ����Ϣ
	PIMAGE_DOS_HEADER pDosHeader = NULL;
	PIMAGE_NT_HEADERS pNTHeader = NULL;
	PIMAGE_FILE_HEADER pPEHeader = NULL;
	PIMAGE_OPTIONAL_HEADER32 pOptionHeader = NULL;
	PIMAGE_SECTION_HEADER pSectionHeader = NULL;
	LPVOID pTempBuffer = NULL;
	bool Ssize = false;
	if(!pFilebuff)
	{
		printf("��ȡ���ڴ��pfilebuffer��Ч��\n");
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

	//�ƶ������
	
	//��ȡʣ���С //SizeOfHeaders(�ܵĴ�С)4096 -   ( pSectionHeader(һ����ַ���ļ���ʼ�ĵط�ƫ����pSectionHeader�Ĵ�С���ֽ�)  -    (�ļ�ƫ��+pPEHeader->NumberOfSections * 40)  һ����ַ �ļ���ʼ�ĵط�ƫ���˽ڵĴ�С���ֽ� �����ǽ����ļ��е�ƫ�Ƶ�ַ�����Ի�� pSectionHeaderС)
	DWORD whiteSpaceSize = pOptionHeader->SizeOfHeaders - ((DWORD)pSectionHeader - (DWORD)pFilebuff + pPEHeader->NumberOfSections * 40);
	
	if (whiteSpaceSize < 80)
	{
		printf("�ռ䲻��");
		return false;
    }
	//����һ��������ŵ����
	//1.�޸�SizeOfImage��С
	pOptionHeader->SizeOfImage +=  0x1000;
	
	
	
	LPVOID NewBuffer = malloc(pOptionHeader->SizeOfImage);//�����ڴ�
	memset(NewBuffer, 0, pOptionHeader->SizeOfImage);//��ʼ���ڴ�
	//�޸Ľڱ�NumberOfSection����
	
	for (int i=0;i<80;i++)
	{
		if (*((char*)pFilebuff +whiteSpaceSize + i) !=0 )
		{
			
			pTempBuffer=malloc(pOptionHeader->SizeOfImage);
			
			//����ռ��㣬���ǽ�����80���ֽڲ�����0 ����Ҫ��PEͷ�����ƶ�,������������� 
			memset(pTempBuffer, 0, pOptionHeader->SizeOfImage);
			
			//������ԭ����pImageBuffer ���Ƶ��µ�imagefile
			memcpy(pTempBuffer,pFilebuff,pOptionHeader->SizeOfImage);

			DWORD e_lfanew = sizeof(IMAGE_DOS_HEADER);
			//����PE-���ڱ��ж��ٸ��ֽ�
			DWORD moveSize = pOptionHeader->SizeOfHeaders - whiteSpaceSize -  pDosHeader->e_lfanew;
			//printf("�������ݿ�ʼ�ĵط�%x\n",(char*)pTempImageBuffer+e_lfanew);
			//printf("PE��ʼ�ĵط�%x\n",(char*)pTempImageBuffer+pDosHeader->e_lfanew);
			//��PE���ڵ������ƶ���DOSSUB����  
			memcpy((char*)pTempBuffer+e_lfanew,(char*)pTempBuffer+pDosHeader->e_lfanew,moveSize);
			//Ȼ��PE-���ڱ�Ȼ��֮ǰ��������0 //�����С �ܴ�С = pOptionHeader->SizeOfHeaders  - whiteSpaceSize - DOSͷ�Ĵ�С - PE���ڱ�Ĵ�С
			DWORD setZeroSzie = pOptionHeader->SizeOfHeaders - whiteSpaceSize - e_lfanew - moveSize;
			//֮ǰ��������0
			memset((char*)pTempBuffer+e_lfanew+moveSize,0,setZeroSzie);
			//����e_lfanew
			memcpy((char*)pTempBuffer+sizeof(IMAGE_DOS_HEADER)-4,&e_lfanew,1);
			
			//���´�pTempImageBuffer ��ȡDOSͷ��PEͷ���ڱ����Ϣ
			pDosHeader =(PIMAGE_DOS_HEADER)pTempBuffer;
			pNTHeader = (PIMAGE_NT_HEADERS)((DWORD)pTempBuffer + pDosHeader->e_lfanew);
			//��ӡNTͷ	
			pPEHeader = (PIMAGE_FILE_HEADER)(((DWORD)pNTHeader) + 4);  //��4���ֽڵ��˱�׼PEͷ
			pOptionHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPEHeader + IMAGE_SIZEOF_FILE_HEADER); //��׼PEͷ+��׼PEͷ�Ĵ�С 20
			pSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)pOptionHeader + pPEHeader->SizeOfOptionalHeader);
			
			pFilebuff = pTempBuffer;

		}	
	}
	


	PIMAGE_SECTION_HEADER pNewSec = (PIMAGE_SECTION_HEADER)(pSectionHeader + pPEHeader->NumberOfSections);
	
    memcpy(pNewSec->Name,".addSec",8);//�޸Ľڱ���
	
	//��ȡ���һ����
	PIMAGE_SECTION_HEADER EndSection = (PIMAGE_SECTION_HEADER)(pSectionHeader + pPEHeader->NumberOfSections-1);
	
	//�ڴ����
	pNewSec->VirtualAddress = EndSection->VirtualAddress + (DWORD)(ceil(max(EndSection->Misc.VirtualSize, EndSection->SizeOfRawData)  / pOptionHeader->SectionAlignment) * pOptionHeader->SectionAlignment);
	//�ڴ��С
	pNewSec->Misc.VirtualSize = 0x1000;
	//�ļ���С
	pNewSec->SizeOfRawData = 0x1000;//�����Ľ����Ĵ�С
	//�ļ�ƫ��
	pNewSec->PointerToRawData = EndSection->PointerToRawData + EndSection->SizeOfRawData;
	pNewSec->Characteristics = 0x60000020;//�޸�����(��ִ��)
	pPEHeader->NumberOfSections += 1;
	//�������ڱ������40���ֽڵĿհ���
    memset(pNewSec+1, 0, 40);



    memcpy(NewBuffer, pFilebuff,pOptionHeader->SizeOfImage);//�����ڴ� 

	
	if (!pOptionHeader->DataDirectory[1].VirtualAddress)
	{
		printf("(DLLInject)This program has no import table.\n");
		return 0;
	}
	
	
	//��λ�½ڱ�ĵ�ַ
	LPVOID pNewSecAddr = (LPVOID)((DWORD)NewBuffer+pNewSec->PointerToRawData);
	
	pDosHeader =(PIMAGE_DOS_HEADER)NewBuffer;
	pNTHeader = (PIMAGE_NT_HEADERS)((DWORD)NewBuffer + pDosHeader->e_lfanew);
	//��ӡNTͷ	
	pPEHeader = (PIMAGE_FILE_HEADER)(((DWORD)pNTHeader) + 4);  //��4���ֽڵ��˱�׼PEͷ
	pOptionHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPEHeader + IMAGE_SIZEOF_FILE_HEADER); //��׼PEͷ+��׼PEͷ�Ĵ�С 20
	pSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)pOptionHeader + pPEHeader->SizeOfOptionalHeader);
	


	//��λ�����	
	PIMAGE_IMPORT_DESCRIPTOR ImportExtable = (PIMAGE_IMPORT_DESCRIPTOR)((char*)NewBuffer + RvaToFileOffset(NewBuffer,pOptionHeader->DataDirectory[1].VirtualAddress));

	//�����µĵ����
	memcpy(pNewSecAddr,ImportExtable,pOptionHeader->DataDirectory[1].Size);

	//��������ѭ����ȡ��С

	int j =0;
	while(ImportExtable->FirstThunk !=0 &&ImportExtable->OriginalFirstThunk !=0){
		ImportExtable++;
		j++;
	}	
	
	//�µĵ����ṹ�ĵ�ַ,��Ϊ�������ڵķ�ʽ�������ƶ��굼����֮������ط���Ϊ0������ֱ��ʹ��һ��PIMAGE_IMPORT_DESCRIPTOR�ṹ
	PIMAGE_IMPORT_DESCRIPTOR NewImportExtable= (PIMAGE_IMPORT_DESCRIPTOR)((DWORD)pNewSecAddr + j * (sizeof(IMAGE_IMPORT_DESCRIPTOR)));

	
	//����IAT�� IAT��ĵ�ַ = �Լ������ĵ����ĵ�ַ + 2���Լ��ṹ�Ĵ�С  ��һ���ṹ��������ĵ�����ڶ����ṹ��Ϊ�����Ľ������
	PIMAGE_THUNK_DATA32 IatTable = (PIMAGE_THUNK_DATA32)(NewImportExtable +2);

	//����InT��
	PIMAGE_THUNK_DATA32 IntTable = (PIMAGE_THUNK_DATA32)(IatTable+2);

	//����IMAGE_IMPORT_BY_NAME

	PIMAGE_IMPORT_BY_NAME ImageName = (PIMAGE_IMPORT_BY_NAME)(IntTable+2);
	
	memcpy(ImageName->Name,"ExportFunction",strlen("ExportFunction")+1);

	//��ȡ���DLL���Ƶĵ�ַ	�����ַ��֤��PIMAGE_IMPORT_BY_NAME�Ľṹ�������
	
	DWORD* DllName  = (DWORD*)(ImageName + 1);//
	
	memcpy(DllName,"InjectDll.dll",strlen("InjectDll.dll")+1);


	//����IAT���ַ
	DWORD InAtFoa = FoaToImageOffset(NewBuffer,(DWORD)ImageName -(DWORD)NewBuffer);
	memcpy(IntTable,&InAtFoa,4);
	memcpy(IntTable,&InAtFoa,4);
	
	//�޵������ַ
	NewImportExtable->FirstThunk = FoaToImageOffset(NewBuffer,(DWORD)IntTable- (DWORD)NewBuffer);
	NewImportExtable->OriginalFirstThunk = FoaToImageOffset(NewBuffer,(DWORD)IatTable- (DWORD)NewBuffer);
	//����dll���Ƶĵ�ַ
	NewImportExtable->Name =  FoaToImageOffset(NewBuffer,(DWORD)DllName - (DWORD)NewBuffer);
	//�µ�����ĵ�ַ �µ�����ĵ�ַ= NewExport_Directory �ڴ��ַ - �ļ���ʼ��λ�� = ƫ��
	pOptionHeader->DataDirectory[1].VirtualAddress = FoaToImageOffset(NewBuffer,(DWORD)pNewSecAddr-(DWORD)NewBuffer);
	pOptionHeader->DataDirectory[1].Size = pOptionHeader->DataDirectory[1].Size + 40;

	FILE* fp = fopen("C://project/ipmsg1.exe","wb+");
	size_t n = 	fwrite(NewBuffer,pOptionHeader->SizeOfImage,1,fp);
	if(!n){
		printf("fwrite����д��ʧ��...\n");
		fclose(fp);
		return ERROR;
	}
	printf("���̳ɹ�...\n");
	free(pFilebuff);
	free(NewBuffer);
	fclose(fp);

	
	

	return 1;
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