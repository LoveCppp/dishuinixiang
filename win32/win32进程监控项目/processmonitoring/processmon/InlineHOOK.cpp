// InlineHOOK.cpp: implementation of the InlineHOOK class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "processmon.h"
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

PBYTE pCode;
DWORD dwHookFlag;
DWORD length;
DWORD g_Error;
HANDLE g_hImportFile;
TCHAR szBuffer[1000] = {0};
DWORD dwWritten = 0;

void SetMESSAGEBOXInlineHook(DWORD HookAddr,DWORD HookProc,DWORD dwlenth,DWORD* WriteHOOKAddr){
	DWORD ReAdd;
	
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
    
	//把内存改为可写

	ReAdd = VirtualProtect((LPBYTE)HookAddr, dwlenth, PAGE_EXECUTE_READWRITE, &OldProtect);
	
    if (!ReAdd)
    {
        printf("内存无法修改，申请失败 - Error!\n\n");
        return;
    }
	
	//申请空间保存原来的字节
	
	pCode = new BYTE[dwlenth];
	memcpy(pCode,(PWORD)HookAddr,dwlenth);
		
	//计算E9的地址 要跳转的地址-E9的地址
	DWORD dwJmpcode =  HookProc - HookAddr - 5;
	
	memset((LPBYTE)HookAddr, 0x90, dwlenth);   //将Hook的内存全部初始化为nop(dwLength > 5)
	
	*(LPBYTE)HookAddr = 0xE9;
    *(PDWORD)((LPBYTE)HookAddr + 1) = dwJmpcode;
	
    *WriteHOOKAddr = (DWORD)HookAddr;
    RetWriteHookAddr = (DWORD)HookAddr + dwlenth;
    dwHookFlag = 1;

}


BOOL UnInlineHook(DWORD nweMessageBoxAddr,DWORD length)
{
	
	if (nweMessageBoxAddr != NULL)
	{
		memcpy((LPVOID)nweMessageBoxAddr,pCode, length);
		delete[] pCode;
		dwHookFlag = 0;
		DbgPrintf("unhookflag: %d\n",dwHookFlag);
		DbgPrintf("卸载成功");
		return TRUE;
	}else{
		return FALSE;
	}


  


}


_declspec(naked) void MessageBoxHOOK()
{
    _asm
    {
        pushad;     //保留寄存器
        pushfd;     //保留标志寄存器
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
		mov eax, DWORD PTR ss : [esp + 0x28]   //获取参数值
			mov x, eax
			mov eax, DWORD PTR ss : [esp + 0x2c]
			mov y, eax
			mov eax, DWORD PTR ss : [esp + 0x30]
			mov z, eax
    }
	
	g_hImportFile = CreateFile(TEXT("C:\\Export.txt"),GENERIC_READ|GENERIC_WRITE,0,NULL,CREATE_NEW,FILE_ATTRIBUTE_NORMAL,NULL);
	g_Error=GetLastError();
	//存在就只打开
	if(g_Error!=0)
	{
		g_hImportFile = CreateFile(TEXT("C:\\Export.txt"),GENERIC_READ|GENERIC_WRITE,0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
		g_Error = 0;
	}

	
	sprintf(szBuffer,"HWND:%x ,标题:%s ,参数:%s ,UINT:%d\r\n",x,y,z);
	SetFilePointer(g_hImportFile, 0, NULL, FILE_END);
    WriteFile(g_hImportFile, szBuffer, strlen(szBuffer), &dwWritten, NULL);
    CloseHandle(g_hImportFile);


    _asm
    {
        popfd;      //还原标志寄存器
        popad;      //还原寄存器
    }
	
    //执行之前覆盖的代码
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
	
    //执行完后跳转回hook地址
    _asm
    {
        jmp RetWriteHookAddr;
    }
	
}



