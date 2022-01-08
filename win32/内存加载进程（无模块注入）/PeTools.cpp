// PeTools.cpp: implementation of the PeTools class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <stdio.h>
#include "PeTools.h"
#define KEY 0x86
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

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



DWORD GetSection(IN LPVOID pFileBuffer,OUT LPVOID* EncyBuffer){
	//����PEͷ����Ϣ
	PIMAGE_DOS_HEADER pDosHeader = NULL;
	PIMAGE_NT_HEADERS pNTHeader = NULL;
	PIMAGE_FILE_HEADER pPEHeader = NULL;
	PIMAGE_OPTIONAL_HEADER32 pOptionHeader = NULL;
	PIMAGE_SECTION_HEADER pSectionHeader = NULL;
	LPVOID pTempImageBuffer = NULL;
	DWORD fileSize = 0;

	bool Ssize = false;
	if(!pFileBuffer)
	{
		MessageBoxA(0,TEXT("��ȡ���ڴ��pfilebuffer��Ч"),0,0);
		return 0;
	}
	//�ж��ǲ���exe�ļ�
	if(*((PWORD)pFileBuffer) != IMAGE_DOS_SIGNATURE)
	{
		MessageBoxA(0,TEXT("����MZ��־������exe�ļ���"),0,0);
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
	
	DWORD secNum = pPEHeader->NumberOfSections;

	for(int i=0;i<secNum;i++,pSectionHeader++){
		if (strcmp((char*)pSectionHeader->Name,".addSec") == 0)
		{
			//��ȡ�ļ���С
			fileSize = pSectionHeader->SizeOfRawData;
			//�����ڴ�
			pTempImageBuffer = malloc(fileSize);
			memset(pTempImageBuffer,0,fileSize);
	//		pTempBuffer = ;
			memcpy(pTempImageBuffer,(CHAR*)((DWORD)pFileBuffer+pSectionHeader->PointerToRawData),fileSize);
			*EncyBuffer = pTempImageBuffer;
			pTempImageBuffer =NULL;
			return fileSize;
		}else{
			continue;
		}	
	}
	return 0;
}


void XorDecodeAAA(char* p_data,DWORD DecodeSize)
{   
    for(DWORD i = 0; i < DecodeSize; i++)
    {
		p_data[i] = p_data[i] ^ KEY;
    }	
}



DWORD CopyFileBufferToImageBuffer(IN LPVOID pFileBuffer,OUT LPVOID* pImageBuffer)
{
	PIMAGE_DOS_HEADER pDosHeader = NULL;
	PIMAGE_NT_HEADERS pNTHeader = NULL;
	PIMAGE_FILE_HEADER pPEHeader = NULL;
	PIMAGE_OPTIONAL_HEADER32 pOptionHeader = NULL;
	PIMAGE_SECTION_HEADER pSectionHeader = NULL;
	LPVOID pTempFileBuffer = NULL;
	
	if(pFileBuffer ==NULL){
		printf("pFileBuffer������ָ�����...\n");
		return ERROR;
	}
	
	
	//�ж��Ƿ�����ЧMZ��PE��־
	if(*((PWORD)pFileBuffer) != IMAGE_DOS_SIGNATURE){
		printf("����Ч��MZ��־\n");
		return ERROR;
	}
	pDosHeader = (PIMAGE_DOS_HEADER)pFileBuffer;
	//IMAGE_SIZEOF_FILE_HEADER = 232   ���ת�����͵Ļ��͵��ڵ�ǰ�����Ϳ�ȥһ��* ���ϵ�ǰ�ĳ���
	if(*((PDWORD)((BYTE *)pFileBuffer + pDosHeader->e_lfanew)) != IMAGE_NT_SIGNATURE){
		printf("����Ч��PE��־\n");
		return ERROR;
	}
	
	pDosHeader =(PIMAGE_DOS_HEADER)pFileBuffer;
	pNTHeader = (PIMAGE_NT_HEADERS)((DWORD)pFileBuffer + pDosHeader->e_lfanew);
	//��ӡNTͷ
	pPEHeader = (PIMAGE_FILE_HEADER)(((DWORD)pNTHeader) + 4);  //��4���ֽڵ��˱�׼PEͷ
	
	pOptionHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPEHeader + IMAGE_SIZEOF_FILE_HEADER); //��׼PEͷ+��׼PEͷ�Ĵ�С 20
	
	//��ѡPEͷ����SizeOfOptionalHeader��
	pSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)pOptionHeader + pPEHeader->SizeOfOptionalHeader);
	
	//����SizeOfImage����ռ�
	pTempFileBuffer  =malloc(pOptionHeader->SizeOfImage);
	if (!pTempFileBuffer)
	{
		printf("pTempFileBuffer�ռ�����ʧ��...");
		return 0;
	}
	//Ȼ��ռ�����Ϊ0
	memset(pTempFileBuffer, 0, pOptionHeader->SizeOfImage);
	//�����ļ�ͷ��������ĵط�
	memcpy(pTempFileBuffer,pFileBuffer,pOptionHeader->SizeOfHeaders);
	
	//ѭ���ڱ��Ƶ��ڴ�
	
	for (int i=0;i<pPEHeader->NumberOfSections;i++,pSectionHeader++)
	{
		
		//��pImageBuffer�ĵ�ַ����ƫ��+virtualAddress �����ڴ��е�һ���ڵ�λ�ã�Ȼ����ļ���PointerToRawData Ҳ�����ļ��е�һ���ڵ�λ�ã�д��pSectionHeader->SizeOfRawData �ļ��ж���Ĵ�С���ֽ�
		memcpy((PVOID)((char *)pTempFileBuffer + pSectionHeader->VirtualAddress), (PVOID)((char *)pFileBuffer + pSectionHeader->PointerToRawData), pSectionHeader->SizeOfRawData);
		//����������
		
	};
	
	*pImageBuffer = pTempFileBuffer;
	pTempFileBuffer = NULL;
	return pOptionHeader->SizeOfImage;
}


DWORD GetImageBase(LPVOID pImageBuffer, DWORD* srcImageBase,DWORD* srcSizeOfImage,DWORD* srcOEP){

	
	PIMAGE_DOS_HEADER pImageDosHeader = NULL;
	PIMAGE_FILE_HEADER pImageFileHeader = NULL;
	PIMAGE_OPTIONAL_HEADER32 pImageOptionalHeader = NULL;
	PIMAGE_SECTION_HEADER pImageSectionHeaderGroup = NULL;
	PIMAGE_SECTION_HEADER NewSec = NULL;
	
	DWORD NewLength=0;
	PVOID LastSection = NULL;
	PVOID CodeSection = NULL;
	PVOID AddressOfSectionTable = NULL;
	
	
	pImageDosHeader = (PIMAGE_DOS_HEADER)pImageBuffer;
	pImageFileHeader = (PIMAGE_FILE_HEADER)((DWORD)pImageDosHeader + pImageDosHeader->e_lfanew + 4);
	pImageOptionalHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pImageFileHeader + sizeof(IMAGE_FILE_HEADER));
	pImageSectionHeaderGroup = (PIMAGE_SECTION_HEADER)((DWORD)pImageOptionalHeader + pImageFileHeader->SizeOfOptionalHeader);
	

	*srcImageBase = pImageOptionalHeader->ImageBase;
	*srcSizeOfImage = pImageOptionalHeader->SizeOfImage;
	*srcOEP = pImageOptionalHeader->AddressOfEntryPoint;


	return pImageOptionalHeader->ImageBase;

}

DWORD GetRelocationTable(PVOID pFileBuffer){
	PIMAGE_DOS_HEADER pImageDosHeader = NULL;
	PIMAGE_FILE_HEADER pImageFileHeader = NULL;
	PIMAGE_OPTIONAL_HEADER32 pImageOptionalHeader = NULL;
	PIMAGE_SECTION_HEADER pImageSectionHeaderGroup = NULL;

	
	pImageDosHeader = (PIMAGE_DOS_HEADER)pFileBuffer;
	pImageFileHeader = (PIMAGE_FILE_HEADER)((DWORD)pImageDosHeader + pImageDosHeader->e_lfanew + 4);
	pImageOptionalHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pImageFileHeader + sizeof(IMAGE_FILE_HEADER));
	pImageSectionHeaderGroup = (PIMAGE_SECTION_HEADER)((DWORD)pImageOptionalHeader + pImageFileHeader->SizeOfOptionalHeader);
	return pImageOptionalHeader->DataDirectory[5].VirtualAddress;
}



DWORD isRelocation(LPVOID pFileBuffer){
	
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
	//�ض�λ��ʼ
	
	if(pOptionHeader->DataDirectory[5].VirtualAddress == 0 || pOptionHeader->DataDirectory[5].Size == 0 ){
		printf("%s","�������ض�λ��...");
		return 0;
	}
	
	
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
	printf("FileOffset: %#x\n", dwFoa);
	
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




void ChangesImageBase(PVOID pFileBuffer, DWORD TempImageBase){
	PIMAGE_DOS_HEADER pImageDosHeader = NULL;
	PIMAGE_FILE_HEADER pImageFileHeader = NULL;
	PIMAGE_OPTIONAL_HEADER32 pImageOptionalHeader = NULL;
	PIMAGE_SECTION_HEADER pImageSectionHeaderGroup = NULL;
	pImageDosHeader = (PIMAGE_DOS_HEADER)pFileBuffer;
	pImageFileHeader = (PIMAGE_FILE_HEADER)((DWORD)pImageDosHeader + pImageDosHeader->e_lfanew + 4);
	pImageOptionalHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pImageFileHeader + sizeof(IMAGE_FILE_HEADER));
	pImageSectionHeaderGroup = (PIMAGE_SECTION_HEADER)((DWORD)pImageOptionalHeader + pImageFileHeader->SizeOfOptionalHeader);
	
	pImageOptionalHeader->ImageBase = TempImageBase;
}


DWORD GetRelocatio(LPVOID pFileBuffer){
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
	//�ض�λ��ʼ
	
	if(pOptionHeader->DataDirectory[5].VirtualAddress == 0 || pOptionHeader->DataDirectory[5].Size == 0 ){
		printf("%s","�������ض�λ��...");
		return 0;
	}
	
	
	return pOptionHeader->DataDirectory[5].VirtualAddress;


}


void RestoreRelocation(IN LPVOID pImageBuffer, IN DWORD newImageBase){
	
	//����PEͷ����Ϣ
	PIMAGE_DOS_HEADER pDosHeader = NULL;
	PIMAGE_NT_HEADERS pNTHeader = NULL;
	PIMAGE_FILE_HEADER pPEHeader = NULL;
	PIMAGE_OPTIONAL_HEADER32 pOptionHeader = NULL;
	PIMAGE_SECTION_HEADER pSectionHeader = NULL;
	LPVOID pTempBuffer = NULL;
	bool Ssize = false;
	if(!pImageBuffer)
	{
		printf("��ȡ���ڴ��pfilebuffer��Ч��\n");
		return;
	}
	//�ж��ǲ���exe�ļ�
	if(*((PWORD)pImageBuffer) != IMAGE_DOS_SIGNATURE)
	{
		printf("����MZ��־������exe�ļ���\n");
		return;
	}
	pDosHeader = (PIMAGE_DOS_HEADER)pImageBuffer;
	if(*((PDWORD)((BYTE *)pImageBuffer + pDosHeader->e_lfanew)) != IMAGE_NT_SIGNATURE){
		printf("����Ч��PE��־\n");
		return;
	}
	
	//��ȡpFileBuffer ��ȡDOSͷ��PEͷ���ڱ����Ϣ
	pDosHeader =(PIMAGE_DOS_HEADER)pImageBuffer;
	pNTHeader = (PIMAGE_NT_HEADERS)((DWORD)pImageBuffer + pDosHeader->e_lfanew);
	//��ӡNTͷ	
	pPEHeader = (PIMAGE_FILE_HEADER)(((DWORD)pNTHeader) + 4);  //��4���ֽڵ��˱�׼PEͷ
	pOptionHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPEHeader + IMAGE_SIZEOF_FILE_HEADER); //��׼PEͷ+��׼PEͷ�Ĵ�С 20
	pSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)pOptionHeader + pPEHeader->SizeOfOptionalHeader);
	
	DWORD oldImagebase = pOptionHeader->ImageBase;

    pOptionHeader->ImageBase = newImageBase;


	//��λ�ض�λ��
	PIMAGE_BASE_RELOCATION ReloCation = (_IMAGE_BASE_RELOCATION*)((char*)pImageBuffer + pOptionHeader->DataDirectory[5].VirtualAddress);
		

	
	PWORD pItem;		//�ض������е���ָ�룬2�ֽڲ����ƶ�
	int NumberOfItems;	//�ض����һ���е�����
	
	//����pRelocationTable�Ѿ��ǵ�ǰ���Ե�ַ��
	
	while (ReloCation->VirtualAddress && ReloCation->SizeOfBlock)
	{
		
		NumberOfItems = (ReloCation->SizeOfBlock - 8) / 2;
		pItem = (PWORD)((DWORD)ReloCation + 8);	//����ȡ�������ǻ�û�����ض�����е�VirtualAddress��Rva
	
		for (int j = 0; j < NumberOfItems; j++)
		{
			WORD* offset = (WORD*)((char*)ReloCation+8+2*j); //ÿ��������ĵ�ַ
			if (*offset >= 0x3000)
			{
				//��12λ����x����Ҫ�޸ĵĵط�
				
				PDWORD RVA = (PDWORD)((DWORD)pImageBuffer + (ReloCation->VirtualAddress + (*offset-0x3000)));	//��ΪFOA�ٱ�Ϊ���Ե�ַ�����ҵ�������ֵ
				*RVA =  *RVA - oldImagebase + pOptionHeader->ImageBase;
			}
			
		}
		ReloCation = (PIMAGE_BASE_RELOCATION)(ReloCation->SizeOfBlock + (DWORD)ReloCation);
	}

	
}


void RepairIAT(LPVOID pImageBuffer)
{
	PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)pImageBuffer;
	PIMAGE_FILE_HEADER pPEHeader = (PIMAGE_FILE_HEADER)(pDosHeader->e_lfanew + (DWORD)pDosHeader + 4);
	PIMAGE_OPTIONAL_HEADER32 pOptionHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPEHeader + sizeof(IMAGE_FILE_HEADER));
	PIMAGE_SECTION_HEADER pSectionHeader = \
		(PIMAGE_SECTION_HEADER)((DWORD)pOptionHeader + pPEHeader->SizeOfOptionalHeader);
	PIMAGE_IMPORT_DESCRIPTOR pImportTable = (PIMAGE_IMPORT_DESCRIPTOR)((DWORD)pImageBuffer + \
		pOptionHeader->DataDirectory[1].VirtualAddress);	
	// �ϸ���˵Ӧ���� sizeof(IMAGE_IMPORT_DESCRIPTOR) ���ֽ�Ϊ0��ʾ����
	while (pImportTable->OriginalFirstThunk || pImportTable->FirstThunk)
	{
		// ��ӡģ����
		//printf("%s\n", (LPCSTR)(pImportTable->Name + (DWORD)pImageBuffer));
		// ��ȡģ����
		HMODULE hModule = LoadLibraryA((LPCSTR)(pImportTable->Name + (DWORD)pImageBuffer));
		if (NULL == hModule)
		{
			printf("��ȡģ����ʧ�ܣ�ģ����: %s\n",(LPCSTR)(pImportTable->Name + (DWORD)pImageBuffer));
		}
		// �޸�IAT��
		//printf("--------------FirstThunkRVA:%x--------------\n", pImportTable->FirstThunk);		
		PIMAGE_THUNK_DATA32 pThunkData = (PIMAGE_THUNK_DATA32)((DWORD)pImageBuffer + \
			pImportTable->FirstThunk);
		while (*((PDWORD)pThunkData) != 0)
		{
			// IMAGE_THUNK_DATA32 ��һ��4�ֽ�����
			// ������λ��1����ô��ȥ���λ���ǵ������
			// ������λ��0����ô���ֵ��RVA ָ�� IMAGE_IMPORT_BY_NAME
			if ((*((PDWORD)pThunkData) & 0x80000000) == 0x80000000)
			{
				//printf("����ŵ��� Ordinal:%04x\n", (*((PDWORD)pThunkData) & 0x7FFFFFFF));
				DWORD dwProcAddress = (DWORD)GetProcAddress(hModule,MAKEINTRESOURCE((*((PDWORD)pThunkData) & 0x7FFFFFFF)));				
				*((PDWORD)pThunkData) = dwProcAddress;
			}
			else
			{
				PIMAGE_IMPORT_BY_NAME pIBN = (PIMAGE_IMPORT_BY_NAME)(*((PDWORD)pThunkData) + \
					(DWORD)pImageBuffer);
				
				//printf("�����ֵ��� Hint:%04x Name:%s\n", pIBN->Hint, pIBN->Name);
				DWORD dwProcAddress = (DWORD)GetProcAddress(hModule,(LPCSTR)pIBN->Name);
				*((PDWORD)pThunkData) = dwProcAddress;
			}
			pThunkData++;
		}
		pImportTable = (PIMAGE_IMPORT_DESCRIPTOR)((DWORD)pImportTable + sizeof(IMAGE_IMPORT_DESCRIPTOR));		
	}	
}
