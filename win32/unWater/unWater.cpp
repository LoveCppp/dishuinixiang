// unWater.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include <stdio.h>
#include "PeTools.h"
//�޸��ض�λ
VOID ModificationBaseRel(IN LPVOID ImageBuffer, DWORD newImageBase) {
	PIMAGE_DOS_HEADER pDosHeader = NULL; //DOs ͷ
	PIMAGE_NT_HEADERS pNTHeader = NULL; //NTͷ
	PIMAGE_FILE_HEADER pFileHeader = NULL; // ��׼PEͷ
	PIMAGE_OPTIONAL_HEADER pOptionHerader = NULL; // ��ѡPEͷ
	PIMAGE_SECTION_HEADER pSectionHeader = NULL; // �ڱ�
	PIMAGE_BASE_RELOCATION pBaseRelocation = NULL; //�ض�λ��
	
	pDosHeader = (PIMAGE_DOS_HEADER)ImageBuffer;
	pNTHeader = (PIMAGE_NT_HEADERS)((DWORD)pDosHeader + pDosHeader->e_lfanew);
	pFileHeader = (PIMAGE_FILE_HEADER)((DWORD)pNTHeader + sizeof(DWORD));
	pOptionHerader = (PIMAGE_OPTIONAL_HEADER)((DWORD)pFileHeader + IMAGE_SIZEOF_FILE_HEADER);
	pSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)pOptionHerader + pFileHeader->SizeOfOptionalHeader);
	pBaseRelocation = (PIMAGE_BASE_RELOCATION)((DWORD)pDosHeader + pOptionHerader->DataDirectory[5].VirtualAddress);
	
	pOptionHerader->ImageBase = newImageBase;
	
	int index = 0;
	while (pBaseRelocation->VirtualAddress != 0) {
		
		int count = (pBaseRelocation->SizeOfBlock - 8) / 2;
		PWORD addr = (PWORD)((DWORD)pBaseRelocation + 8);
		
		for (int i = 0; i < count; i++) {
			DWORD height4 = addr[i] >> 12;
			if (height4 == 3) {
				DWORD low12 = addr[i] & 0x0fff;
				DWORD rva = pBaseRelocation->VirtualAddress + low12;
				PDWORD addr = (PDWORD)((DWORD)ImageBuffer + rva);
				*addr = *addr - pOptionHerader->ImageBase + newImageBase;
			}
		}
		index++;
		pBaseRelocation = (PIMAGE_BASE_RELOCATION)((DWORD)pBaseRelocation + pBaseRelocation->SizeOfBlock);
	}
        }


int main(int argc, char* argv[])
{	
	/*

	//��ȡ��ǰ�ļ�·��
	char FilePath[255] = {0};
	GetModuleFileName(NULL, FilePath, 255);
	*/

	char FilePath[] = "c://project/ipmsgnew.exe";
	// 1����ȡ��ǰ��������
	PVOID pFileBuffer = NULL;
	PVOID EnyBuffer = NULL;
	DWORD szizFilebuff = 0;
	
	LPSTR FILEPATH = FilePath;
	//��ȡ��ǰ����
	ReadPeFile(FILEPATH,&pFileBuffer);
	
	//��ȡ�����ܳ���ڵ�����
	DWORD EncyFileSize=0;
	EncyFileSize = GetSection(pFileBuffer,&EnyBuffer);
	//����
	if (EncyFileSize == 0)
	{
		MessageBoxA(0,0,0,0);
		return 0;
	}
	//2�����ܵõ�ԭ����PE�ļ�
	XorDecodeAAA((char*)EnyBuffer,EncyFileSize);
	
	//������ filebuff -> imagebuff
	PVOID pImageBuffer = NULL;

	int ImageSize=	CopyFileBufferToImageBuffer(EnyBuffer,&pImageBuffer);
	
	//3���ѹ���ķ�ʽ��������(���ܽ���) �����Ľ��̾��ǿ��ӱ���
	
	STARTUPINFO si = {0};
	PROCESS_INFORMATION pi;
	si.cb = sizeof(si);
	::CreateProcess(FilePath,NULL,NULL,NULL,NULL,CREATE_SUSPENDED, NULL,NULL,&si,&pi);
	printf("error is %d\n", GetLastError());
	

	//4����ȡ�ոմ����߳�Context�ṹ ����ȡ������ڵ� �� imgebase
	CONTEXT contx;
    contx.ContextFlags = CONTEXT_FULL;
    GetThreadContext(pi.hThread, &contx);
	
    //��ȡ��ڵ�                                
    DWORD dwEntryPoint = contx.Eax;  //��ǰ�������ڵ�
    //��ȡImageBase                                  
    char* baseAddress = (CHAR *)contx.Ebx + 8; 
    DWORD imageBase = 0;
    SIZE_T byteSize = 0;
	//��ȡ���ǽ��̵�imagebase
    ReadProcessMemory(pi.hProcess, baseAddress, &imageBase, 4, &byteSize); 


	//5�� ж����Ǿ������ 


	typedef long NTSTATUS;
	typedef NTSTATUS(__stdcall *pfnZwUnmapViewOfSection)(
		IN HANDLE ProcessHandle,
		IN LPVOID BaseAddress
		);
 
	pfnZwUnmapViewOfSection ZwUnmapViewOfSection;
	ZwUnmapViewOfSection = (pfnZwUnmapViewOfSection)GetProcAddress(
        GetModuleHandleA("ntdll.dll"), "ZwUnmapViewOfSection");
    if (!ZwUnmapViewOfSection)
    {
        ::TerminateThread(pi.hThread, 2);
        ::WaitForSingleObject(pi.hThread, INFINITE);
        return 0;
    }
    DWORD a = 0;
    a = ZwUnmapViewOfSection(pi.hProcess, (PVOID)imageBase);
	
	

	//6����ָ����λ�÷���ռ䣺λ�þ���src��ImageBase  ��С����Src��SizeOfImage	
	
	//��ȡsrc imagebase  sizeofimage
    DWORD srcImageBase = 0;
    DWORD srcSizeOfImage = 0;
    DWORD srcOEP = 0;
    GetImageBase(pImageBuffer, &srcImageBase,&srcSizeOfImage,&srcOEP);
	
	LPVOID status = NULL;
    //�ڿ��ӽ��̿ռ��������ڴ�
    status = VirtualAllocEx(pi.hProcess,(LPVOID)srcImageBase, srcSizeOfImage,MEM_RESERVE | MEM_COMMIT,PAGE_EXECUTE_READWRITE);

	printf("VirtualAllocEx: %x\n",status);
	printf("error is %d\n", GetLastError());
	


	//�ж��Ƿ�����ɹ�,�����һ���ڴ�û�����뵽��Ҫ �ж����src��û���ض�λ��,  �еĻ����ڱ�ĵط�����, û�еĻ��ͽ���ʧ��					
	if(status != NULL){
		printf("7777777\n");
		//7������ɹ�����Src��PE�ļ����� ���Ƶ��ÿռ���
		
		//dwBufferImageBaseSrc ���ܺ�imgbuff��image base 
		//pImageBufferSrc  ������imagebuffer
		//dwBufferLengthSrc ���ܺ��size
		printf("%x,%x,%x",srcImageBase,pImageBuffer);
		WriteProcessMemory(pi.hProcess, (LPVOID)srcImageBase, pImageBuffer, srcSizeOfImage, NULL);
		
	}else{
		if(isRelocation(pImageBuffer)){
			printf("���ض�λ��");
			//�޸��ض�λ��
			PIMAGE_BASE_RELOCATION pRelocationDirectory = NULL;
			DWORD pRelocationDirectoryVirtual = 0;
			
			DWORD NumberOfRelocation;
			PWORD Location;
			DWORD RVA_Data;
			WORD reloData;
			DWORD FOA;
			DWORD dwTempImageBaseSrc = srcImageBase + 0x50000;  //�µ�imagebase
			
			pRelocationDirectoryVirtual = GetRelocatio(EnyBuffer); //��ǰ�ض�λ��������ַ
			printf("%x",pRelocationDirectoryVirtual);
			if(pRelocationDirectoryVirtual){
				FOA = RvaToFileOffset(EnyBuffer, pRelocationDirectoryVirtual);
				pRelocationDirectory = (PIMAGE_BASE_RELOCATION)((DWORD)EnyBuffer + FOA);
				//����ռ�
				status = VirtualAllocEx(pi.hProcess, (LPVOID)dwTempImageBaseSrc,srcSizeOfImage,MEM_RESERVE | MEM_COMMIT,PAGE_EXECUTE_READWRITE);
				ChangesImageBase(EnyBuffer, dwTempImageBaseSrc);
				WriteProcessMemory(pi.hProcess, (LPVOID)dwTempImageBaseSrc, EnyBuffer, srcSizeOfImage, NULL);
				while(pRelocationDirectory->SizeOfBlock && pRelocationDirectory->VirtualAddress){				
					NumberOfRelocation = (pRelocationDirectory->SizeOfBlock - 8)/2;// ÿ���ض�λ���е������������
					Location = (PWORD)((DWORD)pRelocationDirectory + 8); // ����8���ֽ�
					for(DWORD i=0;i<NumberOfRelocation;i++){
						if(Location[i] >> 12 != 0){ //�ж��Ƿ�����������
							// WORD���͵ı������н���
							reloData = (Location[i] & 0xFFF); //������������ ֻȡ4�ֽ� �����Ƶĺ�12λ
							RVA_Data = pRelocationDirectory->VirtualAddress + reloData; //�����RVA�ĵ�ַ
							FOA = RvaToFileOffset(EnyBuffer,RVA_Data);
							//������������ �����޸��ض�λ�������Imagebase���Ǹĳ���TempImageBase,��ô�ı��ֵ���� TempImageBase-dwBufferImageBaseSrc
							*(PDWORD)((DWORD)EnyBuffer+(DWORD)FOA) = *(PDWORD)((DWORD)EnyBuffer+(DWORD)FOA) + dwTempImageBaseSrc - srcSizeOfImage;	 // ����λ�� - Origin ImageBase			
						}
					}
					pRelocationDirectory = (PIMAGE_BASE_RELOCATION)((DWORD)pRelocationDirectory + (DWORD)pRelocationDirectory->SizeOfBlock); //�����forѭ�����֮����ת���¸��ض�λ�� �������ϵĲ���
				}
				
			srcImageBase = dwTempImageBaseSrc;
	

		}else{
		   printf("��������ڴ�ʧ��");
		   return 0;
		}
	}



	// 10���޸���ǳ����Context:
	CONTEXT cont;
	cont.ContextFlags = CONTEXT_FULL; 
	::GetThreadContext(pi.hThread, &cont);
	cont.Eax = srcOEP + srcImageBase; // imagebase + op
	
	DWORD theOep = cont.Ebx + 8;
	DWORD dwBytes=0;
	WriteProcessMemory(pi.hProcess, &theOep, &srcImageBase,4, &dwBytes);
	
    SetThreadContext(pi.hThread, &cont);
	//�ǵûָ��߳�
    ResumeThread(pi.hThread);
	ExitProcess(0);
	//�ǵûָ��߳�
	return 0;



	}
}