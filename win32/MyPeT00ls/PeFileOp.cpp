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



BOOL SAddNewSection(PVOID pFileBuffer,PVOID SrcFileBuffer,DWORD SrcBuffSize){
	
	//����PEͷ����Ϣ
	PIMAGE_DOS_HEADER pDosHeader = NULL;
	PIMAGE_NT_HEADERS pNTHeader = NULL;
	PIMAGE_FILE_HEADER pPEHeader = NULL;
	PIMAGE_OPTIONAL_HEADER32 pOptionHeader = NULL;
	PIMAGE_SECTION_HEADER pSectionHeader = NULL;
	LPVOID pTempBuffer = NULL;
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

	//�ƶ������
	
	//��ȡʣ���С //SizeOfHeaders(�ܵĴ�С)4096 -   ( pSectionHeader(һ����ַ���ļ���ʼ�ĵط�ƫ����pSectionHeader�Ĵ�С���ֽ�)  -    (�ļ�ƫ��+pPEHeader->NumberOfSections * 40)  һ����ַ �ļ���ʼ�ĵط�ƫ���˽ڵĴ�С���ֽ� �����ǽ����ļ��е�ƫ�Ƶ�ַ�����Ի�� pSectionHeaderС)
	DWORD whiteSpaceSize = pOptionHeader->SizeOfHeaders - ((DWORD)pSectionHeader - (DWORD)pFileBuffer + pPEHeader->NumberOfSections * 40);
	
	if (whiteSpaceSize < 80)
	{
		printf("�ռ䲻��");
		return false;
    }

	//1.�޸�SizeOfImage��С = ���ļ���С + Ҫ�ӿ��ļ��Ĵ�С
	pOptionHeader->SizeOfImage +=  SrcBuffSize; 
	


	LPVOID NewBuffer = malloc(pOptionHeader->SizeOfImage);//�����ڴ�
	memset(NewBuffer, 0, pOptionHeader->SizeOfImage);//��ʼ���ڴ�
	//�޸Ľڱ�NumberOfSection����
	
	for (int i=0;i<80;i++)
	{
		if (*((char*)pFileBuffer +whiteSpaceSize + i) !=0 )
		{
			
			pTempBuffer=malloc(pOptionHeader->SizeOfImage);
			
			//����ռ��㣬���ǽ�����80���ֽڲ�����0 ����Ҫ��PEͷ�����ƶ�,������������� 
			memset(pTempBuffer, 0, pOptionHeader->SizeOfImage);
			
			//������ԭ����pImageBuffer ���Ƶ��µ�imagefile
			memcpy(pTempBuffer,pFileBuffer,pOptionHeader->SizeOfImage);

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
			pFileBuffer = pTempBuffer;

		}	
	}


	PIMAGE_SECTION_HEADER pNewSec = (PIMAGE_SECTION_HEADER)(pSectionHeader + pPEHeader->NumberOfSections);
	
    memcpy(pNewSec->Name,".addSec",8);//�޸Ľڱ���
	
	//��ȡ���һ����
	PIMAGE_SECTION_HEADER EndSection = (PIMAGE_SECTION_HEADER)(pSectionHeader + pPEHeader->NumberOfSections-1);
	
	//�ڴ����
	pNewSec->VirtualAddress = EndSection->VirtualAddress + (DWORD)(ceil(max(EndSection->Misc.VirtualSize, EndSection->SizeOfRawData)  / pOptionHeader->SectionAlignment) * pOptionHeader->SectionAlignment);
	//�ڴ��С
	pNewSec->Misc.VirtualSize = SrcBuffSize; //�½ڵĴ�С == Ҫ�ӿ��ļ��Ĵ�С
	//�ļ���С
	pNewSec->SizeOfRawData = SrcBuffSize;//�����Ľ����Ĵ�С
	//�ļ�ƫ��
	pNewSec->PointerToRawData = EndSection->PointerToRawData + EndSection->SizeOfRawData;
	//pNewSec->Characteristics = 0xC0000040;//�޸�����(��ִ��)
	pNewSec->Characteristics |= IMAGE_SCN_MEM_EXECUTE |IMAGE_SCN_MEM_WRITE|IMAGE_SCN_MEM_READ;
	pPEHeader->NumberOfSections += 1;
	
	//�������ڱ������40���ֽڵĿհ���
    memset(pNewSec+1, 0, 40);

	memcpy(NewBuffer,pFileBuffer,pOptionHeader->SizeOfImage);

	LPVOID pNewSecAddr = (LPVOID)((DWORD)NewBuffer+pNewSec->PointerToRawData);
	memcpy(pNewSecAddr,SrcFileBuffer,SrcBuffSize);



	FILE* fp = fopen("C://project/ipmsgnew.exe","wb+");
	size_t n = 	fwrite(NewBuffer,pOptionHeader->SizeOfImage,1,fp);
	if(!n){
		printf("fwrite����д��ʧ��...\n");
		fclose(fp);
		return ERROR;
	}
	
	free(pFileBuffer);
	free(NewBuffer);
	
	printf("���̳ɹ�...\n");
	
	fclose(fp);
	
	

	return TRUE;
}