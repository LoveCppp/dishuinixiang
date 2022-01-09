// InlineHOOK.cpp: implementation of the InlineHOOK class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "InlineHOOK.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////



typedef struct _regeist
{
    DWORD EAX;
    DWORD EBX;
    DWORD ECX;
    DWORD EDX;
    DWORD EBP;
    DWORD ESP;
    DWORD ESI;
    DWORD EDI;
}regeist;

regeist reg = { 0 };

DWORD x, y, z;
DWORD RetWriteHookAddr;
DWORD ReAdd;
PBYTE pCode;
DWORD dwHookFlag;
DWORD WriteHookAddr;
DWORD length;

void SetInlineHook(DWORD HookAddr,DWORD HookProc,DWORD dwlenth){

	
	if (HookAddr == NULL || HookProc == NULL)
    {
        printf("The address is error,please try again!\n\n");
        return;
    }
	
    if (dwlenth < 5)
    {
        printf("The alloc is too small,please try adgin!\n\n");
        return;
    }
	
    DWORD OldProtect;
    
	//���ڴ��Ϊ��д

	ReAdd = VirtualProtect((LPBYTE)HookAddr, dwlenth, PAGE_EXECUTE_READWRITE, &OldProtect);
	
    if (!ReAdd)
    {
        printf("�ڴ��޷��޸ģ�����ʧ�� - Error!\n\n");
        return;
    }
	
	//����ռ䱣��ԭ�����ֽ�
	
	pCode = new BYTE[dwlenth];
	memcpy(pCode,(PWORD)HookAddr,dwlenth);
		
	//����E9�ĵ�ַ Ҫ��ת�ĵ�ַ-E9�ĵ�ַ
	DWORD dwJmpcode =  HookProc - HookAddr - 5;
	
	memset((LPBYTE)HookAddr, 0x90, dwlenth);   //��Hook���ڴ�ȫ����ʼ��Ϊnop(dwLength > 5)
	
	*(LPBYTE)HookAddr = 0xE9;
    *(PDWORD)((LPBYTE)HookAddr + 1) = dwJmpcode;
	
    WriteHookAddr = (DWORD)HookAddr;
	length = dwlenth;
    RetWriteHookAddr = (DWORD)HookAddr + dwlenth;
    dwHookFlag = 1;
	
}


BOOL UnInlineHook()
{
   	bool bFlag = true;
	if (!dwHookFlag)
    {
        printf("UnInlineHook!\n\n");
        return FALSE;
    }
	
    memcpy((LPVOID)WriteHookAddr,pCode, length);
	delete[] pCode;
    dwHookFlag = 0;
	
    return 1;


}


_declspec(naked) void HOOK()
{
    _asm
    {
        pushad;     //�����Ĵ���
        pushfd;     //������־�Ĵ���
    }
	
    _asm
    {
        mov reg.EAX, eax
			mov reg.EBX, ebx
			mov reg.ECX, ecx
			mov reg.EDX, edx
			mov reg.EDI, edi
			mov reg.ESI, esi
			mov reg.ESP, esp
			mov reg.EBP, ebp
    }
	
    _asm
    {
		mov eax, DWORD PTR ss : [esp + 0x28]   //��ȡ����ֵ
			mov x, eax
			mov eax, DWORD PTR ss : [esp + 0x2c]
			mov y, eax
			mov eax, DWORD PTR ss : [esp + 0x30]
			mov z, eax
    }
	
    printf("EAX:%x EBX:%x ECX:%x EDX:%x EDI:%x ESI:%x ESP:%x EBP:%x\n\n", reg.EAX, reg.EBX, reg.ECX, reg.EDX, reg.EDI, reg.ESI, reg.ESP, reg.EBP);
	//printf(NULL,TEXT("���سɹ�"),NULL,NULL);
    printf("x:%d y:%s z:%d\n\n", x, y, z);
	
    _asm
    {
        popfd;      //��ԭ��־�Ĵ���
        popad;      //��ԭ�Ĵ���
    }
	
    //ִ��֮ǰ���ǵĴ���
    _asm
    {

			//77D507EA 8B FF                mov         edi,edi
			//77D507EC 55                   push        ebp
			//77D507ED 8B EC                mov         ebp,esp
			//77D507EF 83 3D BC 14 D7 77 00 cmp         dword ptr ds:[77D714BCh],0
			//77D507F6 74 24                je          77D5081C

     
		mov         edi,edi
		push        ebp
		mov         ebp,esp

			
			
    }
	
    //ִ�������ת��hook��ַ
    _asm
    {
        jmp RetWriteHookAddr;
    }
	
}



