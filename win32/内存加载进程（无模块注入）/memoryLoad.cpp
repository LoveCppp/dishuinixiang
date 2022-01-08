// memoryLoad.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "stdlib.h"
#include "PeTools.h"

DWORD WINAPI InjectEntry(LPVOID lpParam)
{
	//RepairIAT(lpParam);
	//����PEͷ����Ϣ
	PIMAGE_DOS_HEADER pDosHeader = NULL;
	PIMAGE_NT_HEADERS pNTHeader = NULL;
	PIMAGE_FILE_HEADER pPEHeader = NULL;
	PIMAGE_OPTIONAL_HEADER32 pOptionHeader = NULL;
	PIMAGE_SECTION_HEADER pSectionHeader = NULL;
	
	if(!lpParam)
	{
		printf("��ȡ���ڴ��pfilebuffer��Ч��\n");
		return 0;
	}
	//�ж��ǲ���exe�ļ�
	if(*((PWORD)lpParam) != IMAGE_DOS_SIGNATURE)
	{
		printf("����MZ��־������exe�ļ���\n");
		return 0;
	}
	pDosHeader = (PIMAGE_DOS_HEADER)lpParam;
	if(*((PDWORD)((BYTE *)lpParam + pDosHeader->e_lfanew)) != IMAGE_NT_SIGNATURE){
		printf("����Ч��PE��־\n");
		return 0;
	}
	
	//��ȡpFileBuffer ��ȡDOSͷ��PEͷ���ڱ����Ϣ
	pDosHeader =(PIMAGE_DOS_HEADER)lpParam;
	pNTHeader = (PIMAGE_NT_HEADERS)((DWORD)lpParam + pDosHeader->e_lfanew);
	//��ӡNTͷ	
	pPEHeader = (PIMAGE_FILE_HEADER)(((DWORD)pNTHeader) + 4);  //��4���ֽڵ��˱�׼PEͷ
	pOptionHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPEHeader + IMAGE_SIZEOF_FILE_HEADER); //��׼PEͷ+��׼PEͷ�Ĵ�С 20
	
	
	//1000
	
	
	PIMAGE_IMPORT_DESCRIPTOR ImportExtable = (PIMAGE_IMPORT_DESCRIPTOR)((char*)lpParam + pOptionHeader->DataDirectory[1].VirtualAddress);	
	
	DbgPrintf("%x",ImportExtable->FirstThunk);
	while(ImportExtable->OriginalFirstThunk !=0 && ImportExtable->FirstThunk !=0){
		
		char *DllName =(char*)lpParam + ImportExtable->Name;
		
		DbgPrintf("���ص�DLL����:%s\n",DllName);
		
		//���ʱ���INT���Ѿ��Ҳ���������,Ҫ�ȱ���INT���ҵ���ź����ƣ�Ȼ������LoadLibrary��ȡ�����ĵ�ַȻ���޸�
		
		DWORD* pThunkData_INT = (DWORD*)((char*)lpParam + ImportExtable->OriginalFirstThunk);
		DWORD*	pThunkData_IAT = (PDWORD)(((PBYTE)lpParam + ImportExtable->FirstThunk));
		while(*pThunkData_INT){
			
			if (*pThunkData_INT & 0x80000000)
			{
				DbgPrintf("����ŵ������%x\n",(*pThunkData_INT & 0xFFFF));
				(*pThunkData_IAT) = (DWORD)GetProcAddress(LoadLibrary(DllName),(char*)(*pThunkData_INT & 0xFFFF));
				printf("--%x\n",*pThunkData_IAT);
			}else{
				
				PIMAGE_IMPORT_BY_NAME FirstThunkNames = (_IMAGE_IMPORT_BY_NAME*)((char*)lpParam + *pThunkData_INT);
				DbgPrintf("�����Ƶ���HIT/NAME-%x-%s\n",FirstThunkNames->Hint,FirstThunkNames->Name);
				(*pThunkData_IAT) = (DWORD)GetProcAddress(LoadLibrary(DllName),(char*)FirstThunkNames->Name);
				DbgPrintf("--%x\n",*pThunkData_IAT);
			}	
			
			pThunkData_IAT++,pThunkData_INT++;
		}
		
		ImportExtable = (PIMAGE_IMPORT_DESCRIPTOR)((DWORD)ImportExtable+sizeof(IMAGE_IMPORT_DESCRIPTOR));
		
	}
	
	
	while (TRUE)
	{
		MessageBoxA(0,TEXT("ע��ɹ�"),0,0);
		Sleep(10000);
	}
	return 0;
}

BOOL EnableDebugPrivilege()
{
	HANDLE hToken;
	BOOL fOk=FALSE;
	if(OpenProcessToken(GetCurrentProcess(),TOKEN_ADJUST_PRIVILEGES,&hToken))
	{
		TOKEN_PRIVILEGES tp;
		tp.PrivilegeCount=1;
		LookupPrivilegeValue(NULL,SE_DEBUG_NAME,&tp.Privileges[0].Luid);
		
		tp.Privileges[0].Attributes=SE_PRIVILEGE_ENABLED;
		AdjustTokenPrivileges(hToken,FALSE,&tp,sizeof(tp),NULL,NULL);
		
		fOk=(GetLastError()==ERROR_SUCCESS);
		CloseHandle(hToken);
	}
    return fOk;
}

int main(int argc, char* argv[])
{
		//1����ȡ������					
		
	
		EnableDebugPrivilege(); 	


		HMODULE pImageBuffer = GetModuleHandle(NULL);
		PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)pImageBuffer;
		PIMAGE_NT_HEADERS pNTHeader = (PIMAGE_NT_HEADERS)((DWORD)pDosHeader + pDosHeader->e_lfanew);
		PIMAGE_FILE_HEADER pPEHeader = (PIMAGE_FILE_HEADER)((DWORD)pDosHeader + pDosHeader->e_lfanew + 4);
		PIMAGE_OPTIONAL_HEADER32 pOptionHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPEHeader + sizeof(IMAGE_FILE_HEADER));

		//��ȡ����sizeofimage
		DWORD sizeOfImage = pOptionHeader->SizeOfImage;

		//3���ڵ�ǰ�ռ�����ռ����������					

		LPVOID pNewImagebuffer = malloc(sizeOfImage);
		
		//4��������������					
		memset(pNewImagebuffer, 0, sizeOfImage);
		memcpy(pNewImagebuffer, pImageBuffer, sizeOfImage);
		
		//5����Ҫע��Ľ���					
		//�Ȼ�ȡ���̾��
		DWORD pid = 0;
		printf("PID:");
		scanf("%d", &pid);
		
		HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS,FALSE,pid);  //����һ,ѡ������Ȩ��,������,���̳и�false,������,�����������õ�pid��ֵ
		if(NULL == hProcess)
		{
			printf("�򿪽���ʧ��\n");
			return 0;
			}

		//6����Զ�̽�������ռ�					

		
		LPVOID pImageBase = VirtualAllocEx(hProcess, NULL, sizeOfImage, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
		if (!pImageBase)
		{
			printf("�����ڴ�ʧ�ܣ�\n");
			return false;
		}
	

		//7����ģ���еĴ�������ض�λ					
		RestoreRelocation(pNewImagebuffer,(DWORD)pImageBase);
		
		//8���õ�ģ����Ҫ���еĺ����ĵ�ַ					
	
			//9����ģ���ڽ����еĵ�ַ��Ϊ�������ݸ���ں���	
			
		//10�����������ģ�� ͨ��WriteProcessMemoryд��Զ�̽��̵��ڴ�ռ���							
		DWORD byteWritten = 0;
		BOOL Write = WriteProcessMemory(hProcess, pImageBase, pNewImagebuffer, sizeOfImage, &byteWritten);
		if (!Write)
		{
			printf("can not write process memory!");
			return -1;
		}
		
		DWORD dwProcOffset = (DWORD)InjectEntry - (DWORD)pImageBuffer + (DWORD)pImageBase;	
		//11��ͨ��CreateRemoteThread������д��Ĵ���							
		
		//�����г��߳�,ִ����Ӧ����.
		HANDLE pThread = CreateRemoteThread(hProcess, NULL, NULL, (LPTHREAD_START_ROUTINE)dwProcOffset, pImageBase, NULL, NULL);
		WaitForSingleObject(pThread, -1);

		//12���ͷ��ڴ�							

		return 0;

}

