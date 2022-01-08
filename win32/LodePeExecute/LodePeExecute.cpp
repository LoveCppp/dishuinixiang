// LodePeExecute.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "PeTools.h"
int main(int argc, char* argv[])
{
	//Ҫ���ص��ļ���ַ
	char FilePath[] = "c://project/notepad.exe";

	PVOID pFileBuffer = NULL;
	LPSTR FILEPATH = FilePath;
	//��ȡ��ǰ����
	ReadPeFile(FILEPATH,&pFileBuffer);
	//����
	PVOID pImageBuffer = NULL;
	int ImageSize=	CopyFileBufferToImageBuffer(pFileBuffer,&pImageBuffer);
	
	//�����߳�
	STARTUPINFO si = {0};
	PROCESS_INFORMATION pi;
	si.cb = sizeof(si);
	::CreateProcess(FilePath,NULL,NULL,NULL,NULL,CREATE_SUSPENDED, NULL,NULL,&si,&pi);
	printf("error is %d\n", GetLastError());
	
	//��ȡ�ոմ����߳�Context�ṹ ����ȡ������ڵ� �� imgebase
	CONTEXT contx;
    contx.ContextFlags = CONTEXT_FULL;
    GetThreadContext(pi.hThread, &contx);
	
    //��ȡ��ڵ�                                
    DWORD dwEntryPoint = contx.Eax;  //��ǰ�������ڵ�
    //��ȡImageBase                                  
    char* baseAddress = (CHAR *)contx.Ebx + 8; 
    DWORD imageBase = 0;
    SIZE_T byteSize = 0;
	//��ȡhelloword���̵�imagebase
    ReadProcessMemory(pi.hProcess, baseAddress, &imageBase, 4, &byteSize); 
	

	//ж�ؽ���
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
	//2����ȡA.exe ���� ���Ƶ�0x400000	
	
	//��ȡҪ���س����imagebase��ַ

	//��ȡsrc imagebase  sizeofimage
    DWORD srcImageBase = 0;
    DWORD srcSizeOfImage = 0;
    DWORD srcOEP = 0;
    GetImageBase(pImageBuffer, &srcImageBase,&srcSizeOfImage,&srcOEP);
	


	//��a.exe���Ƶ����ڵĿռ�

	LPVOID status = NULL;
    //��ǰ���̿ռ��������ڴ�
    status = VirtualAllocEx(pi.hProcess,(LPVOID)srcImageBase, srcSizeOfImage,MEM_RESERVE | MEM_COMMIT,PAGE_EXECUTE_READWRITE);
	
	printf("VirtualAllocEx: %x\n",status);
	printf("error is %d\n", GetLastError());

	
	//�ж��Ƿ�����ɹ�,�����һ���ڴ�û�����뵽��Ҫ �ж����src��û���ض�λ��,  �еĻ����ڱ�ĵط�����, û�еĻ��ͽ���ʧ��					
	if(status != NULL){
	
		//��a.exe���Ƶ��ڴ�
		WriteProcessMemory(pi.hProcess, (LPVOID)srcImageBase, pImageBuffer, srcSizeOfImage, NULL);
		
		//�޸�IAT��
		
		PrintImportTable(pImageBuffer);

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

	getchar();
	ExitProcess(0);
	//�ǵûָ��߳�

	return 0;
	
}

