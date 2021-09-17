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



DWORD MoveExprotTable(LPVOID pFilebuff,LPVOID* pNewbuff){

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
	
	
	//��ȡʣ���С //SizeOfHeaders(�ܵĴ�С)4096 -   ( pSectionHeader(һ����ַ���ļ���ʼ�ĵط�ƫ����pSectionHeader�Ĵ�С���ֽ�)  -    (�ļ�ƫ��+pPEHeader->NumberOfSections * 40)  һ����ַ �ļ���ʼ�ĵط�ƫ���˽ڵĴ�С���ֽ� �����ǽ����ļ��е�ƫ�Ƶ�ַ�����Ի�� pSectionHeaderС)
	DWORD whiteSpaceSize = pOptionHeader->SizeOfHeaders - ((DWORD)pSectionHeader - (DWORD)pFilebuff + pPEHeader->NumberOfSections * 40);
	  
	if (whiteSpaceSize < 80)
	{
		printf("�ռ䲻��");
		return false;
    }
	//����һ����
	//1.�޸�SizeOfImage��С



	pOptionHeader->SizeOfImage +=  0x1000;

	//�޸Ľڱ�NumberOfSection����
	

	pTempBuffer=malloc(pOptionHeader->SizeOfImage);

	//����ռ��㣬���ǽ�����80���ֽڲ�����0 ����Ҫ��PEͷ�����ƶ�,������������� 
	memset(pTempBuffer, 0, pOptionHeader->SizeOfImage);
	
	//������ԭ����pImageBuffer ���Ƶ��µ�imagefile
	memcpy(pTempBuffer,pFilebuff,pOptionHeader->SizeOfImage);
	
	for (int i=0;i<80;i++)
	{
		if (*((char*)pTempBuffer +whiteSpaceSize + i) !=0 )
		{
			
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
	
	LPVOID NewBuffer = malloc(pOptionHeader->SizeOfImage);//�����ڴ�
	memset(NewBuffer, 0, pOptionHeader->SizeOfImage);//��ʼ���ڴ�
    memcpy(NewBuffer, pFilebuff,pOptionHeader->SizeOfImage);//�����ڴ� 
	
	
	


	//��λ������
	PIMAGE_EXPORT_DIRECTORY Export_Directory = (PIMAGE_EXPORT_DIRECTORY)((char*)NewBuffer + RvaToFileOffset(NewBuffer,pOptionHeader->DataDirectory[0].VirtualAddress));
	//2.���ƺ�����AddressOfFunction(size:NumberOfFunctions * 4) �������Ľڣ�
	
	DWORD* AddressOfFunctionsAddress = (DWORD*)((char*)NewBuffer + RvaToFileOffset(NewBuffer,Export_Directory->AddressOfFunctions));

	//��λ�½ڱ�ĵ�ַ
	LPVOID pNewSecAddr = (LPVOID)((DWORD)NewBuffer+pNewSec->PointerToRawData);
	

	memcpy(pNewSecAddr,AddressOfFunctionsAddress,Export_Directory->NumberOfFunctions * 4);

	//3.������ű�AddressOfNameOrdinals(size:NumberOfNames * 2) �������Ľ��У�
	//��ȡ���ƺ�����ַ֮���ƫ��
	pNewSecAddr = (LPVOID)((char*)pNewSecAddr +Export_Directory->NumberOfFunctions * 4);
	
	WORD* AddressOfNameOrdinalsAddress =(WORD*)((DWORD)NewBuffer + RvaToFileOffset(NewBuffer,Export_Directory->AddressOfNameOrdinals));
	memcpy(pNewSecAddr,AddressOfNameOrdinalsAddress,Export_Directory->NumberOfNames * 2);
	
	//4.�������ֱ�
	//��ȡ���е�ƫ��
	pNewSecAddr = (LPVOID)((char*)pNewSecAddr + Export_Directory->NumberOfNames * 2);
	//��ȡ�������ֱ�ĵ�ַ
	DWORD* AddressOfNamesFunctionsAddress = (DWORD*)((char*)NewBuffer + RvaToFileOffset(NewBuffer,Export_Directory->AddressOfNames));
	PDWORD NameAddr = (PDWORD)pNewSecAddr;//����洢һ�º�����ַ���Ʊ�ĵ�ַ���Ա����ƶ����Ƶ�ʱ���޸���Ӧ��ַ
	//�������ֵ�ַ��
	memcpy(pNewSecAddr,AddressOfNamesFunctionsAddress,Export_Directory->NumberOfNames * 4);
	
	//��������
	pNewSecAddr = (LPVOID)((char*)pNewSecAddr + Export_Directory->NumberOfNames * 4);
	
	//DWORD NameAddOffset = (DWORD)pNewSecAddr - (DWORD)NewBuffer;//��������ƫ��
	for(int x =0;x<Export_Directory->NumberOfNames;x++,AddressOfNamesFunctionsAddress++){
		
		//printf("��������%s\n",((char*)NewBuffer+RvaToFileOffset(NewBuffer,*AddressOfNamesFunctionsAddress));
		
		//�������ļ��е�ƫ��
		DWORD NameOffset = DWORD(*AddressOfNamesFunctionsAddress);
		//��ȡ���ֵ�ַ
		char* FuncName = (char*)((char*)NewBuffer + NameOffset);
			
		//���ƺ�������ƫ�Ƶ��µĺ������Ʊ� 
		*NameAddr = (DWORD)(FoaToImageOffset(NewBuffer,NameOffset));   //AddressOfName�еĵ�ַ���ڴ��ַ��������Ҫ���ļ���ַת��Ϊ�ڴ��
		
		//������ַ+4
		NameAddr = (PDWORD)((DWORD)NameAddr + 4);

		size_t l = 0;
		while(FuncName[l] !=0){
			l += 1;
		}
		//�������ֵ��ڴ� \0���� ������Ҫ+1
		memcpy(pNewSecAddr,FuncName,l+1);
		pNewSecAddr = (LPVOID)((DWORD)pNewSecAddr + l+1); //��ַƫ��

	}

	
	//���Ƶ�������
    memcpy(pNewSecAddr,Export_Directory,pOptionHeader->DataDirectory[0].Size);
	
	//�µĵ�����ĵ�ַ
	PIMAGE_EXPORT_DIRECTORY  NewExport_Directory = (PIMAGE_EXPORT_DIRECTORY)((char*)pNewSecAddr);

	//�޸��µĵ������AddressOfFunctions��AddressOfNameOrdinals��AddressOfNames��ַ���⼸����ַ�����ڴ��ַ �����ŵ�ֵ�����ļ�ƫ��   ��ҪFOA->RVA  
    NewExport_Directory->AddressOfFunctions = FoaToImageOffset(NewBuffer,pNewSec->PointerToRawData);			//AddressOfFunctions �ڴ��ַ ���ļ�ƫ�� ��Ҫת�����ڴ��ַ
	NewExport_Directory->AddressOfNameOrdinals = FoaToImageOffset(NewBuffer,pNewSec->PointerToRawData + NewExport_Directory->NumberOfFunctions * 4);  //AddressOfNameOrdinals �ڴ��ַ ���ļ�ƫ�� ��Ҫת�����ڴ��ַ���ļ�ƫ��
    NewExport_Directory->AddressOfNames = FoaToImageOffset(NewBuffer,pNewSec->PointerToRawData + NewExport_Directory->NumberOfFunctions * 4 + NewExport_Directory->NumberOfNames * 2);  //AddressOfNames ���ļ�ƫ�� ��Ҫת�����ڴ��ַ���ļ�ƫ��
	

	pDosHeader =(PIMAGE_DOS_HEADER)NewBuffer;
	pNTHeader = (PIMAGE_NT_HEADERS)((DWORD)NewBuffer + pDosHeader->e_lfanew);
	//��ӡNTͷ	
	pPEHeader = (PIMAGE_FILE_HEADER)(((DWORD)pNTHeader) + 4);  //��4���ֽڵ��˱�׼PEͷ
	pOptionHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPEHeader + IMAGE_SIZEOF_FILE_HEADER); //��׼PEͷ+��׼PEͷ�Ĵ�С 20
	pSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)pOptionHeader + pPEHeader->SizeOfOptionalHeader);
	
	//�µ�����ĵ�ַ �µ�����ĵ�ַ= NewExport_Directory �ڴ��ַ - �ļ���ʼ��λ�� = ƫ��
	pOptionHeader->DataDirectory[0].VirtualAddress = FoaToImageOffset(NewBuffer,(DWORD)NewExport_Directory - (DWORD)NewBuffer);


	FILE* fp = fopen("C://project/testNew.dll","wb+");
	size_t n = 	fwrite(NewBuffer,pOptionHeader->SizeOfImage,1,fp);
	if(!n){
		printf("fwrite����д��ʧ��...\n");
		fclose(fp);
		return ERROR;
	}
	

	printf("���̳ɹ�...\n");
	fclose(fp);



	return 0;
};






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


DWORD MoveRectTable(LPVOID pFilebuff,LPVOID* pNewbuff){
	
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
	
	
	//��ȡʣ���С //SizeOfHeaders(�ܵĴ�С)4096 -   ( pSectionHeader(һ����ַ���ļ���ʼ�ĵط�ƫ����pSectionHeader�Ĵ�С���ֽ�)  -    (�ļ�ƫ��+pPEHeader->NumberOfSections * 40)  һ����ַ �ļ���ʼ�ĵط�ƫ���˽ڵĴ�С���ֽ� �����ǽ����ļ��е�ƫ�Ƶ�ַ�����Ի�� pSectionHeaderС)
	DWORD whiteSpaceSize = pOptionHeader->SizeOfHeaders - ((DWORD)pSectionHeader - (DWORD)pFilebuff + pPEHeader->NumberOfSections * 40);
	  
	if (whiteSpaceSize < 80)
	{
		printf("�ռ䲻��");
		return false;
    }
	//����һ����
	//1.�޸�SizeOfImage��С



	pOptionHeader->SizeOfImage +=  0x1000;

	//�޸Ľڱ�NumberOfSection����
	

	pTempBuffer=malloc(pOptionHeader->SizeOfImage);

	//����ռ��㣬���ǽ�����80���ֽڲ�����0 ����Ҫ��PEͷ�����ƶ�,������������� 
	memset(pTempBuffer, 0, pOptionHeader->SizeOfImage);
	
	//������ԭ����pImageBuffer ���Ƶ��µ�imagefile
	memcpy(pTempBuffer,pFilebuff,pOptionHeader->SizeOfImage);
	
	for (int i=0;i<80;i++)
	{
		if (*((char*)pTempBuffer +whiteSpaceSize + i) !=0 )
		{
			
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
	
	LPVOID NewBuffer = malloc(pOptionHeader->SizeOfImage);//�����ڴ�
	memset(NewBuffer, 0, pOptionHeader->SizeOfImage);//��ʼ���ڴ�
    memcpy(NewBuffer, pFilebuff,pOptionHeader->SizeOfImage);//�����ڴ� 

	printf("===�ض�λ��====\n");
	printf("�ڴ��ַ%x\n",pOptionHeader->DataDirectory[5].VirtualAddress);
	printf("�ڴ��С%x\n",pOptionHeader->DataDirectory[5].Size);
	
	if(pOptionHeader->DataDirectory[5].VirtualAddress == 0){
		printf("%s","�������ض�λ��...");
		return 0;
	}
	
	printf("===��һ���ض�λ��ṹ====\n");
	PIMAGE_BASE_RELOCATION ReloCation = (_IMAGE_BASE_RELOCATION*)((char*)NewBuffer + RvaToFileOffset(NewBuffer,pOptionHeader->DataDirectory[5].VirtualAddress));
	
	int ReloSize = 0;
    while (true) {
          
		//���ж��ض�λ���Ƿ���� 0�����Ѿ�����
		if (ReloCation->VirtualAddress != 0 && ReloCation->SizeOfBlock !=0)
		{
			ReloSize += ReloCation->SizeOfBlock;
			//�ڵ�λ�����
			ReloCation = (_IMAGE_BASE_RELOCATION*)((char*)ReloCation + ReloCation->SizeOfBlock);
		}else{
			break;
		}
	
    }
	//��λ�½ڱ�ĵ�ַ
	LPVOID pNewSecAddr = (LPVOID)((DWORD)NewBuffer+pNewSec->PointerToRawData);
	memcpy(pNewSecAddr, ReloCation,ReloSize);

	pDosHeader =(PIMAGE_DOS_HEADER)NewBuffer;
	pNTHeader = (PIMAGE_NT_HEADERS)((DWORD)NewBuffer + pDosHeader->e_lfanew);
	//��ӡNTͷ	
	pPEHeader = (PIMAGE_FILE_HEADER)(((DWORD)pNTHeader) + 4);  //��4���ֽڵ��˱�׼PEͷ
	pOptionHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPEHeader + IMAGE_SIZEOF_FILE_HEADER); //��׼PEͷ+��׼PEͷ�Ĵ�С 20
	pSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)pOptionHeader + pPEHeader->SizeOfOptionalHeader);
	
	//�µ�����ĵ�ַ �µ�����ĵ�ַ= NewExport_Directory �ڴ��ַ - �ļ���ʼ��λ�� = ƫ��
	pOptionHeader->DataDirectory[5].VirtualAddress = FoaToImageOffset(NewBuffer,(DWORD)pNewSecAddr-(DWORD)NewBuffer);	

	FILE* fp = fopen("C://project/testNext.dll","wb+");
	size_t n = 	fwrite(NewBuffer,pOptionHeader->SizeOfImage,1,fp);
	if(!n){
		printf("fwrite����д��ʧ��...\n");
		fclose(fp);
		return ERROR;
	}

	printf("���̳ɹ�...\n");
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
	PIMAGE_BASE_RELOCATION ReloCation = (_IMAGE_BASE_RELOCATION*)((char*)pFilebuff + RvaToFileOffset(pFilebuff,pOptionHeader->DataDirectory[5].VirtualAddress));
	
    pOptionHeader ->ImageBase = pOptionHeader->ImageBase + 0x10000000;

	int cnt = 0;
    while (true) {
          
		//���ж��ض�λ���Ƿ���� 0�����Ѿ�����
		if (ReloCation->VirtualAddress != 0 && ReloCation->SizeOfBlock !=0)
		{
			printf("**********************************\n");
			printf("%x\n",ReloCation);
			//��ȡһ���ж��ٿ����� ���� = �ض�λ���С -  8 (VirtualAddress��С + SizeOfBlock һ��ռ��8�ֽ�) / 2 (��Ϊÿ����վ2���ֽڵĴ�С)
			int num = (ReloCation->SizeOfBlock - 8) / 2;
			for (int i =0;i<num-1;i++)  //ѭ����
			{
				
				//��ȡ��ĵ�ַ
			     WORD* offset = (WORD*)((char*)ReloCation+8+2*i);
				//�жϸ�λ�Ƿ� =3 ��λ�Ƿ����= 0011
				if (*offset >= 0x3000)
				{
					   //*offset-0x3000 *offset ��2���ֽ� ռ��16λ ����ֻ��Ҫ12λ ���Լ�ȥ��λ0011 �ͱ����12λ
					   printf("��%x��\t��ַ:%X\tƫ��:%X\n", ReloCation->VirtualAddress, *offset-0x3000);
					   //ReloCation->VirtualAddress
					   DWORD rva = ReloCation->VirtualAddress + (*offset-0x3000);        //�������ڴ���ƫ��
					   DWORD foa = RvaToFileOffset(pFilebuff, rva);    //ת��foa
					   PDWORD changeAddr = (PDWORD)((DWORD)pFilebuff + foa);    //��ȡ�������ļ��е��ļ�ƫ��
					   *changeAddr = *changeAddr + 0x10000000; //ԭֵ����imagebase�ӵĴ�С
		

				}
			}
			ReloCation = (_IMAGE_BASE_RELOCATION*)((char*)ReloCation + ReloCation->SizeOfBlock);
			  cnt++;
		}else{
			break;
		}
	
    }


    LPVOID NewBuffer = malloc(pOptionHeader->SizeOfImage);//�����ڴ�
	memset(NewBuffer, 0, pOptionHeader->SizeOfImage);//��ʼ���ڴ�
    memcpy(NewBuffer, pFilebuff,pOptionHeader->SizeOfImage);//�����ڴ� 
	


	FILE* fp = fopen("C://project/testNext.dll","wb+");
	size_t n = 	fwrite(NewBuffer,pOptionHeader->SizeOfImage,1,fp);
	if(!n){
		printf("fwrite����д��ʧ��...\n");
		fclose(fp);
		return ERROR;
	}
	
	printf("���̳ɹ�...\n");
	fclose(fp);
	return 0;


}