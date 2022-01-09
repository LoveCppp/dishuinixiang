// HookDemo.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "IATHOOK.h"
#include "InlineHOOK.h"



void TestIATHOOK(){
	
	DWORD pOldFuncAddr = (DWORD)GetProcAddress(LoadLibrary("user32.dll"),"MessageBoxA");
	
	SetIATHOOK(pOldFuncAddr,(DWORD)MyMessageBox);
	MessageBoxA(NULL,"IAT HOOK","IAT HOOK",MB_OK);
	UnSetIATHook();
};

DWORD Plus(DWORD x,DWORD y){
	
	return x+y;
}

void TestInlineHOOK(){
	

	SetInlineHook(0x77D507EA,(DWORD)HOOK,5);
	MessageBoxA(NULL,"inline HOOK","inline HOOK",MB_OK);
	Plus(1,2);
	UnInlineHook();
//	MessageBoxA(NULL,"inline HOOK","inline HOOK",MB_OK);
}


int main(int argc, char* argv[])
{
	
	//77D507EA 8B FF                mov         edi,edi
	//77D507EC 55                   push        ebp
	//77D507ED 8B EC                mov         ebp,esp

	

	//TestIATHOOK();
	TestInlineHOOK();

	getchar();
	return 0;
}

