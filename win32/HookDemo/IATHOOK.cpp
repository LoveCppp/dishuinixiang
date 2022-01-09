// IATHOOK.cpp: implementation of the IATHOOK class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "IATHOOK.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

DWORD g_dwOldAddr;
DWORD g_dwNewAddr;
DWORD g_dwIATHookFlag;

int WINAPI MyMessageBox(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType){
	
	//����MESSBOX��ָ��
	typedef int (WINAPI *PMESSAGEBOX)(HWND,LPCSTR,LPCSTR,UINT);
	//��ӡ����
	printf("����: %x %s %s %x \n",hWnd,lpText,lpCaption,uType);
	
	//ִ��ԭ����messbox
	int ret = ((PMESSAGEBOX)g_dwOldAddr)(hWnd,lpText,lpCaption,uType);
	
	printf("����ֵ%d\n",ret);
	return ret;
}


BOOL SetIATHOOK(DWORD pOldFuncAddr,DWORD pNewFuncAddr){

	//pOldFuncAddr = pOldFuncAddr;
	
	bool bFlag = FALSE;
	DWORD dwImageBase = 0;
	PDWORD pFuncAddr = NULL;  		//�ֲ��ĺ��� ������Ҫ��hook�ĺ�����ַ��Ӧ
	PIMAGE_NT_HEADERS pNtHeader = NULL;  //nt��ʶ
	PIMAGE_IMPORT_DESCRIPTOR pImportDescriptor = NULL; //�����������
	
	//�õ�������ģ���ַ �����̾���һ��module ���صĵ�ַ ���Ǳ������ImageBase
	dwImageBase = (DWORD)::GetModuleHandle(NULL); //NULL  --- >������ 
	pNtHeader = (PIMAGE_NT_HEADERS)(dwImageBase + ((PIMAGE_DOS_HEADER)dwImageBase)->e_lfanew);
    //�ҵ�IAT��

	pImportDescriptor = (PIMAGE_IMPORT_DESCRIPTOR)(dwImageBase + pNtHeader->OptionalHeader.DataDirectory[1].VirtualAddress); 
	//����IAT�� ��Ҫ��hook�ĺ�����ַ

	printf("Ҫ��hook�����ĵ�ַ��%x \n",pOldFuncAddr);
    
	while (pImportDescriptor->FirstThunk !=0 && bFlag == false)
	{
		//��ȡIAT�����ĵ�ַ
		pFuncAddr = PDWORD(dwImageBase + pImportDescriptor->FirstThunk);//ָ��IAT�ĵ�ַ��
		
		while (*pFuncAddr)
		{
			
			if (pOldFuncAddr == *pFuncAddr) //����ҵ�Ҫ��hook�ĺ����ĵ�ַ
			{
				//�ҵ��Ժ��滻��Ҫ�޸ĵĺ����ĵ�ַ
				*pFuncAddr = pNewFuncAddr;
				bFlag = true; //set hook Flag
				break;
			}
			pFuncAddr++; //һֱ��
		}
		
		pImportDescriptor++;
		
	}
	g_dwIATHookFlag = 1;
	g_dwOldAddr = pOldFuncAddr;
	g_dwNewAddr = pNewFuncAddr;
	
	return bFlag;

};



bool UnSetIATHook(){
	bool bFlag = true;
	DWORD dwImageBase = 0;
	PDWORD pFuncAddr = NULL;
	PIMAGE_NT_HEADERS pNtHeader = NULL;
	PIMAGE_IMPORT_DESCRIPTOR pImportDescriptor = NULL;
	
	//�õ�������ģ���ַ
	dwImageBase = (DWORD)::GetModuleHandle(NULL); //NULL  --- >������
	pNtHeader = (PIMAGE_NT_HEADERS)(dwImageBase + ((PIMAGE_DOS_HEADER)dwImageBase)->e_lfanew);
	pImportDescriptor = (PIMAGE_IMPORT_DESCRIPTOR)(dwImageBase + pNtHeader->OptionalHeader.DataDirectory[1].VirtualAddress); //�ڶ���Ŀ¼���ǵ����
	
	//����IAT�� �ҵ�ԭ����MessageBoxA
	//FirstThunk -> IAT
	//OrignalFirstThunk -> INT
	//printf("ԭ������ַ��%x \n",g_dwOldAddr);
	while (pImportDescriptor->FirstThunk !=0 && bFlag == false)
	{

		pFuncAddr = PDWORD(dwImageBase + pImportDescriptor->FirstThunk);//ָ��IAT�ĵ�ַ��
		
		while (*pFuncAddr)
		{
			
			if (g_dwNewAddr == *pFuncAddr) //����ҵ��滻�˵�hook�����ĵ�ַ
			{
				*pFuncAddr = g_dwOldAddr;  //�ָ���ԭ�������ĵ�ַ
				bFlag = FALSE;
				break;
			}
			pFuncAddr++;
		}
	
		pImportDescriptor++;
		
	}
	DWORD g_dwIATHookFlag = 0;
	
	
	return true;
}
