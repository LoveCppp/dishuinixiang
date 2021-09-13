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




DWORD CopyImageBufferToNewFileBuffer(IN LPVOID pImageBuffer, OUT LPVOID *pNewFileBuffer)
{
	PIMAGE_DOS_HEADER pDosHeader = NULL;
	PIMAGE_NT_HEADERS pNTHeader = NULL;
	PIMAGE_FILE_HEADER pPEHeader = NULL;
	PIMAGE_OPTIONAL_HEADER32 pOptionHeader = NULL;
	PIMAGE_SECTION_HEADER pSectionHeader = NULL;
	
	if(pImageBuffer ==NULL){
		printf("pImageBuffer������ָ�����...\n");
		return ERROR;
	}
	
	//�ж��Ƿ�����ЧMZ��PE��־
	if(*((PWORD)pImageBuffer) != IMAGE_DOS_SIGNATURE){
		printf("����Ч��MZ��־\n");
		return ERROR;
	}
	pDosHeader = (PIMAGE_DOS_HEADER)pImageBuffer;
	if(*((PDWORD)((BYTE *)pImageBuffer + pDosHeader->e_lfanew)) != IMAGE_NT_SIGNATURE){
		printf("����Ч��PE��־\n");
		return ERROR;
	}
	
	//��ȡpFileBuffer ��ȡDOSͷ��PEͷ���ڱ����Ϣ
	pDosHeader =(PIMAGE_DOS_HEADER)pImageBuffer;
	pNTHeader = (PIMAGE_NT_HEADERS)((DWORD)pImageBuffer + pDosHeader->e_lfanew);
	//��ӡNTͷ	
	pPEHeader = (PIMAGE_FILE_HEADER)(((DWORD)pNTHeader) + 4);  //��4���ֽڵ��˱�׼PEͷ
	pOptionHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPEHeader + IMAGE_SIZEOF_FILE_HEADER); //��׼PEͷ+��׼PEͷ�Ĵ�С 20
	pSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)pOptionHeader + pPEHeader->SizeOfOptionalHeader);
	
	for (int i=0;i<pPEHeader->NumberOfSections;i++,pSectionHeader++)
	{
		//��pImageBuffer��PointerToRawData Ҳ�����ļ��Ľڿ�ʼ��λ�ã�Ȼ����ڴ��нڵĿ�ʼ�ĵ�ַд��SizeOfRawData���ļ������Ĵ�С���ֽڣ�
		//printf("%d\n",pSectionHeader->SizeOfRawData);

		memcpy((PVOID)((DWORD)pImageBuffer + pSectionHeader->PointerToRawData), (PVOID)((DWORD)pImageBuffer + pSectionHeader->VirtualAddress), pSectionHeader->SizeOfRawData);		
	};

	
	*pNewFileBuffer = pImageBuffer;

	return pOptionHeader->SizeOfImage;
}



BOOL NewFileBufferToFile(IN LPVOID pNewFileBuffer, size_t size,OUT LPSTR lpszFile)
{
	if(pNewFileBuffer == NULL){
		printf("pNewFileBuffer����������...\n");
		return ERROR;
	}
	//����PEͷ����Ϣ
	PIMAGE_DOS_HEADER pDosHeader = NULL;
	PIMAGE_NT_HEADERS pNTHeader = NULL;
	PIMAGE_FILE_HEADER pPEHeader = NULL;
	PIMAGE_OPTIONAL_HEADER32 pOptionHeader = NULL;
	PIMAGE_SECTION_HEADER pSectionHeader = NULL;
	if(pNewFileBuffer ==NULL){
		printf("pImageBuffer������ָ�����...\n");
		return ERROR;
	}
	
	//�ж��Ƿ�����ЧMZ��PE��־
	if(*((PWORD)pNewFileBuffer) != IMAGE_DOS_SIGNATURE){
		printf("����Ч��MZ��־\n");
		return ERROR;
	}
	FILE *pFile = NULL;
	size_t fileSize = size;
	
	pFile = fopen(lpszFile, "wb");
	if(!pFile){
		printf("fopen����EXE�ļ�ʧ��...\n");
		return ERROR;
	}

	size_t n = fwrite(pNewFileBuffer, fileSize, 1, pFile);
	
	if(!n){
		printf("fwrite����д��ʧ��...\n");
		fclose(pFile);
		return ERROR;
	}
	printf("���̳ɹ�...\n");
	fclose(pFile);
	
	return TRUE;
};




DWORD Add_SectionCode(LPVOID pImageBuffer)
{
	//����PEͷ����Ϣ
	PIMAGE_DOS_HEADER pDosHeader = NULL;
	PIMAGE_NT_HEADERS pNTHeader = NULL;
	PIMAGE_FILE_HEADER pPEHeader = NULL;
	PIMAGE_OPTIONAL_HEADER32 pOptionHeader = NULL;
	PIMAGE_SECTION_HEADER pSectionHeader = NULL;
	
	if(!pImageBuffer)
	{
		printf("��ȡ���ڴ��pfilebuffer��Ч��\n");
		return 0;
	}
	//�ж��ǲ���exe�ļ�
	if(*((PWORD)pImageBuffer) != IMAGE_DOS_SIGNATURE)
	{
		printf("����MZ��־������exe�ļ���\n");
		return 0;
	}
	pDosHeader = (PIMAGE_DOS_HEADER)pImageBuffer;
	if(*((PDWORD)((BYTE *)pImageBuffer + pDosHeader->e_lfanew)) != IMAGE_NT_SIGNATURE){
		printf("����Ч��PE��־\n");
		return 0;
	}
	
	//��ȡpFileBuffer ��ȡDOSͷ��PEͷ���ڱ����Ϣ
	pDosHeader =(PIMAGE_DOS_HEADER)pImageBuffer;
	pNTHeader = (PIMAGE_NT_HEADERS)((DWORD)pImageBuffer + pDosHeader->e_lfanew);
	//��ӡNTͷ	
	pPEHeader = (PIMAGE_FILE_HEADER)(((DWORD)pNTHeader) + 4);  //��4���ֽڵ��˱�׼PEͷ
	pOptionHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPEHeader + IMAGE_SIZEOF_FILE_HEADER); //��׼PEͷ+��׼PEͷ�Ĵ�С 20
	pSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)pOptionHeader + pPEHeader->SizeOfOptionalHeader);
	
	//����

	

	//�����������С
	//SizeOfRawData �ļ��ж���Ĵ�С  ��ȥ VirtualSize û�ж���Ĵ�С  �����shellcode�Ĵ�С����ô�������޷�д��shellcode
	if ( ((pSectionHeader->SizeOfRawData) - (pSectionHeader->Misc.VirtualSize)) < size_shellcode  )
	{
		printf("�������ռ䲻�㣡\n"); 
		free(pImageBuffer);
		return 0;
	}

	//Ȼ����shellcode��
	PBYTE code_begin = (PBYTE)((DWORD)pImageBuffer + pSectionHeader->VirtualAddress + pSectionHeader->Misc.VirtualSize );

	//��shellcode�������ڴ���
	memcpy(code_begin,shellcode,size_shellcode);
	
	//����E8 E9 ���е�ʱ����Ҫ��imagebas��ʼ
	//����Ҫ��ת�ĵ�ַ = E8����һ���ĵ�ַ  + x
	//x = messbox -  E8����һ���ĵ�ַ
	
	/*
		pOptionHeader->ImageBase  �ڴ��еĵ�ַ
		code_begin + 0XD E8����һ���ĵ�ַ  - pImageBuffer �õ�ƫ�Ƶ�ַ 
		pOptionHeader->ImageBas + ƫ�Ƶ�ַ ����E9�ĵ�ַ ��������ʱ��ĵ�ַ
	*/

	DWORD callAddr = messagebox_add - ((DWORD)pOptionHeader->ImageBase + ((DWORD)(code_begin + 0X14) - (DWORD)pImageBuffer));
	
	//code_begin + 9 ����E8�����һ���ֽڵĵ�ַ
	*(PDWORD)(code_begin + 0xf) = callAddr;

	//��ʼ����E9
	/*
		 X=����Ҫ��ת�ĵ�ַ - E9����ִ�е���һ�еĵ�ַ 
		����Ҫ��ת�ĵ�ַ = pOptionHeader->ImageBase + pOptionHeader->AddressOfEntryPoint   ImageBase + AddressOfEntryPoint
		E9��һ�еĵ�ַ    pOptionHeader->ImageBase + code_begin + size_shellcode �Ĵ�С
	*/

	DWORD jmpAddr = ((pOptionHeader->ImageBase + pOptionHeader->AddressOfEntryPoint)-(pOptionHeader->ImageBase+((DWORD)code_begin+size_shellcode-(DWORD)pImageBuffer)));
	//��ȡE9����ĵ�һ���ֽڵ�ַ 
	*(PDWORD)(code_begin + 0x14) = jmpAddr;
	
	pOptionHeader->AddressOfEntryPoint = (DWORD)code_begin - (DWORD)pImageBuffer;
	
	
	return 1;

};




//����һ����

DWORD Add_SectionInNewSecExt(IN LPVOID pImageBuffer,OUT LPVOID* pNewImageBuffer){
	
	PIMAGE_DOS_HEADER pDosHeader = NULL;
	PIMAGE_NT_HEADERS pNTHeader = NULL;
	PIMAGE_FILE_HEADER pPEHeader = NULL;
	PIMAGE_OPTIONAL_HEADER32 pOptionHeader = NULL;
	PIMAGE_SECTION_HEADER pSectionHeader = NULL;
	LPVOID pTempImageBuffer = NULL;
	if(!pImageBuffer)
	{
		printf("��ȡ���ڴ��pImageBuffer��Ч��\n");
		return 0;
	}
	//�ж��ǲ���exe�ļ�
	if(*((PWORD)pImageBuffer) != IMAGE_DOS_SIGNATURE)
	{
		printf("����MZ��־������exe�ļ���\n");
		return 0;
	}
	pDosHeader = (PIMAGE_DOS_HEADER)pImageBuffer;
	if(*((PDWORD)((BYTE *)pImageBuffer + pDosHeader->e_lfanew)) != IMAGE_NT_SIGNATURE){
		printf("����Ч��PE��־\n");
		return 0;
	}
	
	//��ȡpFileBuffer ��ȡDOSͷ��PEͷ���ڱ����Ϣ
	pDosHeader =(PIMAGE_DOS_HEADER)pImageBuffer;
	pNTHeader = (PIMAGE_NT_HEADERS)((DWORD)pImageBuffer + pDosHeader->e_lfanew);
	//��ӡNTͷ	
	pPEHeader = (PIMAGE_FILE_HEADER)(((DWORD)pNTHeader) + 4);  //��4���ֽڵ��˱�׼PEͷ
	pOptionHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPEHeader + IMAGE_SIZEOF_FILE_HEADER); //��׼PEͷ+��׼PEͷ�Ĵ�С 20
	pSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)pOptionHeader + pPEHeader->SizeOfOptionalHeader);

	//����ڵĻ���Ҫ�ҵ��ڵ������ַ�ʹ�С Ȼ����� Ȼ��ı�imagebase
	DWORD SizeOfImageAddSize = pOptionHeader->SizeOfImage + 0x1000;
	//�����ڴ�
	pTempImageBuffer=malloc(SizeOfImageAddSize);
	
	if (!pTempImageBuffer)
	{
		printf("pTempImageBuffer�ռ�����ʧ��...");
		free(pTempImageBuffer);
		
		return 0;
	}
	//Ȼ��ռ�����Ϊ0
	memset(pTempImageBuffer, 0, SizeOfImageAddSize);
	//������ԭ����pImageBuffer ���Ƶ��µ�imagefile
	memcpy(pTempImageBuffer,pImageBuffer,SizeOfImageAddSize);

	//�ҵ����һ����

	pSectionHeader = pSectionHeader+pPEHeader->NumberOfSections -1;
	//�޸ĽڵĴ�С��
	DWORD sizeTotal = (DWORD)(ceil(max(pSectionHeader->Misc.VirtualSize+0x1000, pSectionHeader->SizeOfRawData+0x1000)  / pOptionHeader->SectionAlignment) * pOptionHeader->SectionAlignment);
	//�ļ������С
	pSectionHeader->SizeOfRawData = sizeTotal;
	//�ڴ�����С
	pSectionHeader->Misc.VirtualSize = sizeTotal;
	//�ҵ����һ���ڵĿ�ʼλ��ƫ��
	//���һ���ڵĵ�ַ 
	DWORD endSectionsSize =(pDosHeader->e_lfanew + sizeof(pNTHeader->Signature) + sizeof(*pPEHeader) + sizeof(*pOptionHeader) + ( (pPEHeader->NumberOfSections-1) * sizeof(IMAGE_SECTION_HEADER)));
	//printf("%x",(char*)pTempImageBuffer+endSectionsSize);
	memcpy((char*)pTempImageBuffer+endSectionsSize,pSectionHeader,sizeof(IMAGE_SECTION_HEADER));
	
	//�޸�sizeofimage�Ĵ�С
	DWORD SizeofImageSzie  =  pDosHeader->e_lfanew + sizeof(pNTHeader->Signature) + IMAGE_SIZEOF_FILE_HEADER + 56;
	
	memcpy((char*)pTempImageBuffer+SizeofImageSzie,&SizeOfImageAddSize,4);
	

	*pNewImageBuffer = pTempImageBuffer;


	free(pTempImageBuffer);
	free(pImageBuffer);
	return 1;
}



//������


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
		printf("��ȡ���ڴ��pImageBuffer��Ч��\n");
		return 0;
	}
	//�ж��ǲ���exe�ļ�
	if(*((PWORD)pImageBuffer) != IMAGE_DOS_SIGNATURE)
	{
		printf("����MZ��־������exe�ļ���\n");
		return 0;
	}
	pDosHeader = (PIMAGE_DOS_HEADER)pImageBuffer;
	if(*((PDWORD)((BYTE *)pImageBuffer + pDosHeader->e_lfanew)) != IMAGE_NT_SIGNATURE){
		printf("����Ч��PE��־\n");
		return 0;
	}
	
	//��ȡpFileBuffer ��ȡDOSͷ��PEͷ���ڱ����Ϣ
	pDosHeader =(PIMAGE_DOS_HEADER)pImageBuffer;
	pNTHeader = (PIMAGE_NT_HEADERS)((DWORD)pImageBuffer + pDosHeader->e_lfanew);
	//��ӡNTͷ	
	pPEHeader = (PIMAGE_FILE_HEADER)(((DWORD)pNTHeader) + 4);  //��4���ֽڵ��˱�׼PEͷ
	pOptionHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPEHeader + IMAGE_SIZEOF_FILE_HEADER); //��׼PEͷ+��׼PEͷ�Ĵ�С 20
	pSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)pOptionHeader + pPEHeader->SizeOfOptionalHeader);
	
	//1���ж��Ƿ����㹻�Ŀռ䣬�������һ���ڱ�.
	//	SizeOfHeader - (DOS + �������� + PE��� + ��׼PEͷ + ��ѡPEͷ + �Ѵ��ڽڱ�)
	//DOSͷ��С  +�������ݵĴ�С���� = pDosHeader->e_lfanew �Ĵ�С
	// PE��� sizeof(pNTHeader->Signature)
	//��׼PE��С  sizeof(*pPEHeader) 
	//��ѡPEͷ sizeof(*pOptionHeader) 
	//�Ѵ��ڵĽڱ��С  �ڵ����� * �ڱ�Ĵ�С pPEHeader->NumberOfSections *��sizeof

	DWORD whiteSpaceSize =pOptionHeader->SizeOfHeaders - (pDosHeader->e_lfanew + sizeof(pNTHeader->Signature) + sizeof(*pPEHeader) + sizeof(*pOptionHeader) + ( pPEHeader->NumberOfSections * sizeof(IMAGE_SECTION_HEADER)));
	//�ж��Ƿ���2���ֽڵĴ�С
	
	printf("%d\n",whiteSpaceSize);

	if (whiteSpaceSize < (2 * sizeof(IMAGE_SECTION_HEADER)))
	{
		printf("---�ռ䲻���޷������ڱ�----");
		return false;
		//����ռ䲻�㣬���ж��Ƿ���Խ�PE��������

		
	}
	
	//2�޸�����
	//2.1��Ҫ����һ���µĿռ�������޸ĺ�filebuff

	//Ȼ���ȡ��Ҫ�������ڴ棬��Ҫ������ڴ����ԭ�ȵĴ�С������Ҫ���ĽڵĴ�С����1000Ϊ��

	DWORD SizeOfImageTotalSize = pOptionHeader->SizeOfImage + 0x1000;
	
	pTempImageBuffer=malloc(SizeOfImageTotalSize);
	
	if (!pTempImageBuffer)
	{
		printf("pTempImageBuffer�ռ�����ʧ��...");
		free(pTempImageBuffer);
		
		return 0;
	}
	//Ȼ��ռ�����Ϊ0
	memset(pTempImageBuffer, 0, SizeOfImageTotalSize);
	
	//������ԭ����pImageBuffer ���Ƶ��µ�imagefile
	memcpy(pTempImageBuffer,pImageBuffer,SizeOfImageTotalSize);


	//�жϺ���80���ֽ��Ƿ���0
	for (int i=0;i<80;i++)
	{
		if (*((char*)pTempImageBuffer +whiteSpaceSize + i) !=0 )
		{

			DWORD e_lfanew = sizeof(IMAGE_DOS_HEADER);
			//����PE-���ڱ��ж��ٸ��ֽ�
			DWORD moveSize = pOptionHeader->SizeOfHeaders - whiteSpaceSize -  pDosHeader->e_lfanew;
			//printf("�������ݿ�ʼ�ĵط�%x\n",(char*)pTempImageBuffer+e_lfanew);
			//printf("PE��ʼ�ĵط�%x\n",(char*)pTempImageBuffer+pDosHeader->e_lfanew);
			//��PE���ڵ������ƶ���DOSSUB����  
			memcpy((char*)pTempImageBuffer+e_lfanew,(char*)pTempImageBuffer+pDosHeader->e_lfanew,moveSize);
			//Ȼ��PE-���ڱ�Ȼ��֮ǰ��������0 //�����С �ܴ�С = pOptionHeader->SizeOfHeaders  - whiteSpaceSize - DOSͷ�Ĵ�С - PE���ڱ�Ĵ�С
			DWORD setZeroSzie = pOptionHeader->SizeOfHeaders - whiteSpaceSize - e_lfanew - moveSize;
			//֮ǰ��������0
			memset((char*)pTempImageBuffer+e_lfanew+moveSize,0,setZeroSzie);
			//����e_lfanew
			memcpy((char*)pTempImageBuffer+sizeof(IMAGE_DOS_HEADER)-4,&e_lfanew,1);
			
			
			//���´�pTempImageBuffer ��ȡDOSͷ��PEͷ���ڱ����Ϣ
			pDosHeader =(PIMAGE_DOS_HEADER)pTempImageBuffer;
			
			pNTHeader = (PIMAGE_NT_HEADERS)((DWORD)pTempImageBuffer + pDosHeader->e_lfanew);

			//��ӡNTͷ	
			pPEHeader = (PIMAGE_FILE_HEADER)(((DWORD)pNTHeader) + 4);  //��4���ֽڵ��˱�׼PEͷ
			pOptionHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPEHeader + IMAGE_SIZEOF_FILE_HEADER); //��׼PEͷ+��׼PEͷ�Ĵ�С 20
		
			DWORD whiteSpaceSize = pDosHeader->e_lfanew + sizeof(pNTHeader->Signature) + sizeof(*pPEHeader) + sizeof(*pOptionHeader) + ( pPEHeader->NumberOfSections * sizeof(IMAGE_SECTION_HEADER));
			
			x= true;
			


		}	
	}
	
    //�����ڿ�ʼ
    PIMAGE_SECTION_HEADER AddSectionHeader;
    AddSectionHeader = pSectionHeader;  //���Ƶ�һ���ڱ��µĽڱ�
	//�޸Ľڵ�����
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

	//�޸Ľڵ��ڴ��С
	AddSectionHeader->Misc.VirtualSize = 0x1000;
	//��ȡ���һ����
	PIMAGE_SECTION_HEADER EndSection = pSectionHeader+(pPEHeader->NumberOfSections-1);
	//�޸Ľڵ��ڴ�ƫ�Ƶ�ַ
    //�ڴ���� = ��һ���ڵ��ڴ��С���ļ�����ȡ���ֵ   /  �ڴ�Ķ����С Ȼ��* �ڴ����Ĵ�С 
	//(DWORD)(ceil(max(EndSection->Misc.VirtualSize, EndSection->SizeOfRawData)  / pOptionHeader->SectionAlignment) * pOptionHeader->SectionAlignment)
	AddSectionHeader->VirtualAddress = EndSection->VirtualAddress + (DWORD)(ceil(max(EndSection->Misc.VirtualSize, EndSection->SizeOfRawData)  / pOptionHeader->SectionAlignment) * pOptionHeader->SectionAlignment);

	//�޸��ļ���С
	AddSectionHeader->SizeOfRawData  = 0x1000;
	//�޸Ľڵ��ļ�ƫ�Ƶ�ַ
	//�ļ����� = 
	AddSectionHeader->PointerToRawData = EndSection->PointerToRawData  + EndSection->SizeOfRawData;
	//�������Ľڸ��Ƶ��ڴ��У������ڸ��ƽ���
	memcpy((char*)pTempImageBuffer+Numsize,AddSectionHeader,sizeof(IMAGE_SECTION_HEADER));
	//�޸Ľڵ�����
    DWORD SectionNumSzie  =  pDosHeader->e_lfanew + sizeof(pNTHeader->Signature) + 2;
	//��Ҫ����һ����ַ����ŽڵĴ�С

	DWORD NewNumTotal = pPEHeader->NumberOfSections+1;
	memcpy((char*)pTempImageBuffer+SectionNumSzie, &NewNumTotal, 0x1);
	//����SIZEofimage ��ƫ��
		
	DWORD SizeofImageSzie  =  pDosHeader->e_lfanew + sizeof(pNTHeader->Signature) + IMAGE_SIZEOF_FILE_HEADER + 56;

	memcpy((char*)pTempImageBuffer+SizeofImageSzie,&SizeOfImageTotalSize,4);
	
	*pNewImageBuffer = pTempImageBuffer;
	
	return 1;
}




//�ϲ���

DWORD Merge_Sec(IN LPVOID pImageBuffer,OUT LPVOID* pNewImageBuffer){\

	PIMAGE_DOS_HEADER pDosHeader = NULL;
	PIMAGE_NT_HEADERS pNTHeader = NULL;
	PIMAGE_FILE_HEADER pPEHeader = NULL;
	PIMAGE_OPTIONAL_HEADER32 pOptionHeader = NULL;
	PIMAGE_SECTION_HEADER pSectionHeader = NULL;
	LPVOID pTempImageBuffer = NULL;
	if(!pImageBuffer)
	{
		printf("��ȡ���ڴ��pImageBuffer��Ч��\n");
		return 0;
	}
	//�ж��ǲ���exe�ļ�
	if(*((PWORD)pImageBuffer) != IMAGE_DOS_SIGNATURE)
	{
		printf("����MZ��־������exe�ļ���\n");
		return 0;
	}
	pDosHeader = (PIMAGE_DOS_HEADER)pImageBuffer;
	if(*((PDWORD)((BYTE *)pImageBuffer + pDosHeader->e_lfanew)) != IMAGE_NT_SIGNATURE){
		printf("����Ч��PE��־\n");
		return 0;
	}
	
	//��ȡpFileBuffer ��ȡDOSͷ��PEͷ���ڱ����Ϣ
	pDosHeader =(PIMAGE_DOS_HEADER)pImageBuffer;
	pNTHeader = (PIMAGE_NT_HEADERS)((DWORD)pImageBuffer + pDosHeader->e_lfanew);
	//��ӡNTͷ	
	pPEHeader = (PIMAGE_FILE_HEADER)(((DWORD)pNTHeader) + 4);  //��4���ֽڵ��˱�׼PEͷ
	pOptionHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPEHeader + IMAGE_SIZEOF_FILE_HEADER); //��׼PEͷ+��׼PEͷ�Ĵ�С 20
	pSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)pOptionHeader + pPEHeader->SizeOfOptionalHeader);
	
	//��һ���㷨
	DWORD Max =(char*)pOptionHeader->SizeOfImage - (char*)pSectionHeader->VirtualAddress;

	//�ڶ����㷨
	//���һ���ڵĵ�ַ VirtualSize ���� VirtualAddress + SizeOfRawData - ͷ�Ĵ�С
	//��ȡ���һ����
	//PIMAGE_SECTION_HEADER EndSection = pSectionHeader+(pPEHeader->NumberOfSections-1);
	//DWORD Max1 = EndSection->VirtualAddress +Alignment(pOptionHeader->SectionAlignment,max(EndSection->Misc.VirtualSize, EndSection->SizeOfRawData),0)   - Alignment(pOptionHeader->SectionAlignment, pOptionHeader->SizeOfHeaders, 0);
	//VirtualSize = SizeOfRawData  = Max	
	//1.�޸Ľڱ������
	pPEHeader->NumberOfSections = 1;


     pSectionHeader->Characteristics |= IMAGE_SCN_MEM_EXECUTE |IMAGE_SCN_MEM_WRITE|IMAGE_SCN_MEM_READ;
	//pSectionHeader->Characteristics |= ( (PIMAGE_SECTION_HEADER)( (DWORD)pSectionHeader + IMAGE_SIZEOF_SECTION_HEADER ) )->Characteristics ;
	
	//pSectionHeader->Characteristics = 0xE0000060;

	
	//VS SR   VirtualAddress PD���ø�

	pSectionHeader->Misc.VirtualSize = Max;

	pSectionHeader->SizeOfRawData = Max;

	//�ȸ��Ƶ��ڴ�
	pTempImageBuffer  =malloc(pOptionHeader->SizeOfImage);
	if (!pTempImageBuffer)
	{
		printf("pTempFileBuffer�ռ�����ʧ��...");
		return 0;
	}
	//Ȼ��ռ�����Ϊ0
	memset(pTempImageBuffer, 0, pOptionHeader->SizeOfImage);
	//���������ļ�
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
	
	//����PEͷ����Ϣ
	PIMAGE_DOS_HEADER pDosHeader = NULL;
	PIMAGE_NT_HEADERS pNTHeader = NULL;
	PIMAGE_FILE_HEADER pPEHeader = NULL;
	PIMAGE_OPTIONAL_HEADER32 pOptionHeader = NULL;
	PIMAGE_SECTION_HEADER pSectionHeader = NULL;
	
	if(!pImageBuffer)
	{
		printf("��ȡ���ڴ��pfilebuffer��Ч��\n");
		return 0;
	}
	//�ж��ǲ���exe�ļ�
	if(*((PWORD)pImageBuffer) != IMAGE_DOS_SIGNATURE)
	{
		printf("����MZ��־������exe�ļ���\n");
		return 0;
	}
	pDosHeader = (PIMAGE_DOS_HEADER)pImageBuffer;
	if(*((PDWORD)((BYTE *)pImageBuffer + pDosHeader->e_lfanew)) != IMAGE_NT_SIGNATURE){
		printf("����Ч��PE��־\n");
		return 0;
	}
	
	//��ȡpFileBuffer ��ȡDOSͷ��PEͷ���ڱ����Ϣ
	pDosHeader =(PIMAGE_DOS_HEADER)pImageBuffer;
	pNTHeader = (PIMAGE_NT_HEADERS)((DWORD)pImageBuffer + pDosHeader->e_lfanew);
	//��ӡNTͷ	
	pPEHeader = (PIMAGE_FILE_HEADER)(((DWORD)pNTHeader) + 4);  //��4���ֽڵ��˱�׼PEͷ
	pOptionHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPEHeader + IMAGE_SIZEOF_FILE_HEADER); //��׼PEͷ+��׼PEͷ�Ĵ�С 20
	

	printf("===������====\n");
	
	printf("�ڴ��ַ%x\n",pOptionHeader->DataDirectory[0].VirtualAddress);
	
	printf("�ڴ��С%x\n",pOptionHeader->DataDirectory[0].Size);

	printf("===�����====\n");

	printf("�ڴ��ַ%x\n",pOptionHeader->DataDirectory[1].VirtualAddress);
	printf("�ڴ��С%x\n",pOptionHeader->DataDirectory[1].Size);
	

	printf("===��Դ��====\n");
	
	printf("�ڴ��ַ%x\n",pOptionHeader->DataDirectory[2].VirtualAddress);
	printf("�ڴ��С%x\n",pOptionHeader->DataDirectory[2].Size);
	
	
	printf("===�쳣��Ϣ��====\n");
	
	printf("�ڴ��ַ%x\n",pOptionHeader->DataDirectory[3].VirtualAddress);
	printf("�ڴ��С%x\n",pOptionHeader->DataDirectory[3].Size);
	
	printf("===��ȫ֤���====\n");
	printf("�ڴ��ַ%x\n",pOptionHeader->DataDirectory[4].VirtualAddress);
	printf("�ڴ��С%x\n",pOptionHeader->DataDirectory[4].Size);
	
	
	printf("===�ض�λ��====\n");
	printf("�ڴ��ַ%x\n",pOptionHeader->DataDirectory[5].VirtualAddress);
	printf("�ڴ��С%x\n",pOptionHeader->DataDirectory[5].Size);	

	printf("===������Ϣ��֤���====\n");
	printf("�ڴ��ַ%x\n",pOptionHeader->DataDirectory[6].VirtualAddress);
	printf("�ڴ��С%x\n",pOptionHeader->DataDirectory[6].Size);
	

	printf("===��Ȩ���б�====\n");
	printf("�ڴ��ַ%x\n",pOptionHeader->DataDirectory[7].VirtualAddress);
	printf("�ڴ��С%x\n",pOptionHeader->DataDirectory[7].Size);

	printf("===ȫ��ָ���====\n");
	printf("�ڴ��ַ%x\n",pOptionHeader->DataDirectory[8].VirtualAddress);
	printf("�ڴ��С%x\n",pOptionHeader->DataDirectory[8].Size);

	printf("===TLS��====\n");
	printf("�ڴ��ַ%x\n",pOptionHeader->DataDirectory[9].VirtualAddress);
	printf("�ڴ��С%x\n",pOptionHeader->DataDirectory[9].Size);

	printf("===�������ñ�====\n");
	printf("�ڴ��ַ%x\n",pOptionHeader->DataDirectory[10].VirtualAddress);
	printf("�ڴ��С%x\n",pOptionHeader->DataDirectory[10].Size);

	printf("===�󶨵����====\n");
	printf("�ڴ��ַ%x\n",pOptionHeader->DataDirectory[11].VirtualAddress);
	printf("�ڴ��С%x\n",pOptionHeader->DataDirectory[11].Size);

	printf("====IAT��===\n");
	printf("�ڴ��ַ%x\n",pOptionHeader->DataDirectory[12].VirtualAddress);
	printf("�ڴ��С%x\n",pOptionHeader->DataDirectory[12].Size);

	printf("====�ӳٵ���===\n");
	printf("�ڴ��ַ%x\n",pOptionHeader->DataDirectory[13].VirtualAddress);
	printf("�ڴ��С%x\n",pOptionHeader->DataDirectory[13].Size);

	
	printf("====COM===\n");
	printf("�ڴ��ַ%x\n",pOptionHeader->DataDirectory[14].VirtualAddress);
	printf("�ڴ��С%x\n",pOptionHeader->DataDirectory[14].Size);
	

	
	printf("====����===\n");
	printf("�ڴ��ַ%x\n",pOptionHeader->DataDirectory[15].VirtualAddress);
	printf("�ڴ��С%x\n",pOptionHeader->DataDirectory[15].Size);

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

DWORD PrintExport(LPVOID pFileBuffer){
	
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
	

	
	printf("===������====\n");
	
	printf("�������ڴ��ַ%x\n",pOptionHeader->DataDirectory[0].VirtualAddress);
	
	printf("�������ڴ��С%x\n",pOptionHeader->DataDirectory[0].Size);
	
	printf("===������ṹ====\n");
	
	PIMAGE_EXPORT_DIRECTORY Export_Directory = (PIMAGE_EXPORT_DIRECTORY)((char*)pFileBuffer + RvaToFileOffset(pFileBuffer,pOptionHeader->DataDirectory[0].VirtualAddress));
	
	//cout << hex << "-exportName                  = " << Export_Directory->Name                   << endl;
	printf("Name:%sָ��õ�������ļ���\n",(char*)pFileBuffer + RvaToFileOffset(pFileBuffer,Export_Directory->Name));
	printf("Base:%x���������ʼ���\n",Export_Directory->Base);
	printf("NumberOfFunctions:%x ���������ĺ�������\n",Export_Directory->NumberOfFunctions);
	printf("NumberOfNames:%x �Ժ������ֵ����ĺ�������\n",Export_Directory->NumberOfNames);
	printf("AddressOfFunctions: %x����������ַ��RVA\n", Export_Directory-> AddressOfFunctions);
	printf("AddressOfNames;: %x�����������Ʊ�RVA\n", Export_Directory-> AddressOfNames);
	printf("AddressOfNameOrdinals;: %x����������ű�RVA\n\n", Export_Directory-> AddressOfNameOrdinals);


	
	printf("===������ַ��====\n");
	
	DWORD* AddressOfFunctionsAddress = (DWORD*)((char*)pFileBuffer + RvaToFileOffset(pFileBuffer,Export_Directory->AddressOfFunctions));

	//printf("====%d\n",AddressOfFunctionsAddress);
	
	for(int i =0;i<Export_Directory->NumberOfFunctions;i++,AddressOfFunctionsAddress++)
	{
		printf("������ַ%x\n",*AddressOfFunctionsAddress);
	}
	
	printf("\n");
	printf("===�������Ʊ�====\n");
	DWORD* AddressOfNamesFunctionsAddress = (DWORD*)((char*)pFileBuffer + RvaToFileOffset(pFileBuffer,Export_Directory->AddressOfNames));


	for(int x =0;x<Export_Directory->NumberOfNames;x++,AddressOfNamesFunctionsAddress++)
	{
		printf("������ַ%x\n",*AddressOfNamesFunctionsAddress);
		printf("��������%s\n",(char*)pFileBuffer+RvaToFileOffset(pFileBuffer,*AddressOfNamesFunctionsAddress));
	}

	printf("\n");
	printf("===������ű�====\n");
	
	//��ȡ�������Ƶ�ַ

	WORD* AddressOfNameOrdinalsAddress =(WORD*)((DWORD)pFileBuffer + RvaToFileOffset(pFileBuffer,Export_Directory->AddressOfNameOrdinals));
	

	for(int j =0;j<Export_Directory->NumberOfNames;j++,AddressOfNameOrdinalsAddress++)
	{

		printf("��ű�%x\n",*AddressOfNameOrdinalsAddress + Export_Directory->Base);

	}
	
	
	return 1;
}


DWORD GetFunctionAddrByName(PVOID pFileBuffer,char* FuncName){
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
	//
	PIMAGE_EXPORT_DIRECTORY Export_Directory = (PIMAGE_EXPORT_DIRECTORY)((char*)pFileBuffer + RvaToFileOffset(pFileBuffer,pOptionHeader->DataDirectory[0].VirtualAddress));
	

	DWORD* AddressOfNamesFunctionsAddress = (DWORD*)((char*)pFileBuffer + RvaToFileOffset(pFileBuffer,Export_Directory->AddressOfNames));
	

	WORD* AddressOfNameOrdinalsAddress =(WORD*)((DWORD)pFileBuffer + RvaToFileOffset(pFileBuffer,Export_Directory->AddressOfNameOrdinals));
	
	DWORD* AddressOfFunctionsAddress = (DWORD*)((char*)pFileBuffer + RvaToFileOffset(pFileBuffer,Export_Directory->AddressOfFunctions));
	
	for(int x =0;x<Export_Directory->NumberOfNames;x++,AddressOfNamesFunctionsAddress++)
	{
	
		//printf("��������%s\n",(char*)pFileBuffer+RvaToFileOffset(pFileBuffer,*AddressOfNamesFunctionsAddress));
		if (*FuncName == *((char*)pFileBuffer+RvaToFileOffset(pFileBuffer,*AddressOfNamesFunctionsAddress)))
		{
			printf("������ַΪ:%x\n",AddressOfFunctionsAddress[AddressOfNameOrdinalsAddress[x]]);
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
	//
	PIMAGE_EXPORT_DIRECTORY Export_Directory = (PIMAGE_EXPORT_DIRECTORY)((char*)pFileBuffer + RvaToFileOffset(pFileBuffer,pOptionHeader->DataDirectory[0].VirtualAddress));
	
	
	DWORD* AddressOfNamesFunctionsAddress = (DWORD*)((char*)pFileBuffer + RvaToFileOffset(pFileBuffer,Export_Directory->AddressOfNames));

	

	printf("������ַΪ��%x\n",AddressOfNamesFunctionsAddress[FunctionOrdinals-Export_Directory->Base]);

	

	
}

DWORD PrintRelocation(LPVOID pFileBuffer){
	
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
	printf("===�ض�λ��====\n");
	printf("�ڴ��ַ%x\n",pOptionHeader->DataDirectory[5].VirtualAddress);
	printf("�ڴ��С%x\n",pOptionHeader->DataDirectory[5].Size);

	
	if(pOptionHeader->DataDirectory[5].VirtualAddress == 0){
		printf("%s","�������ض�λ��...");
		return 0;
	}
 

	printf("===��һ���ض�λ��ṹ====\n");
	PIMAGE_BASE_RELOCATION ReloCation = (_IMAGE_BASE_RELOCATION*)((char*)pFileBuffer + RvaToFileOffset(pFileBuffer,pOptionHeader->DataDirectory[5].VirtualAddress));

	
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