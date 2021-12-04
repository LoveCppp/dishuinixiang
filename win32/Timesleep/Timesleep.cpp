// Timesleep.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "resource.h"
#include <windows.h>		

HWND hedit;

HANDLE hThread;
DWORD isEnd;


CRITICAL_SECTION cs;			

DWORD WINAPI ThreadProc(				
						LPVOID lpParameter   // thread data		
						){
	
	TCHAR sizebuff[10];

		
	DWORD dwIndex = 0;
	DWORD dwCount;
	memset(&sizebuff,0,10);



	while(dwIndex<10000){
		

		/*
		if (dwIndex == 10)
		{
			::ExitThread(1);
		}*/
		
		EnterCriticalSection(&cs);
		GetWindowText(hedit,sizebuff,10);
		sscanf(sizebuff,"%d",&dwCount);
		dwCount++;
		memset(&sizebuff,0,10);
		sprintf(sizebuff,"%d",dwCount);
		SetWindowText(hedit,sizebuff);
		dwIndex++;
		LeaveCriticalSection(&cs);
		
	}
	return 0;
}


DWORD WINAPI ThreadProc1(				
						LPVOID lpParameter   // thread data		
						){
	
	TCHAR sizebuff[10];

		
	DWORD dwIndex = 0;
	DWORD dwCount;
	memset(&sizebuff,0,10);



	while(dwIndex<10000){
		
		EnterCriticalSection(&cs);

		GetWindowText(hedit,sizebuff,10);
		sscanf(sizebuff,"%d",&dwCount);
		
		dwCount++;

		memset(&sizebuff,0,10);
		sprintf(sizebuff,"%d",dwCount);
		SetWindowText(hedit,sizebuff);
		dwIndex++;
		LeaveCriticalSection(&cs);
	}
	return 0;
}






BOOL CALLBACK DialogProc(									
						 HWND hwndDlg,  // handle to dialog box			
						 UINT uMsg,     // message			
						 WPARAM wParam, // first message parameter			
						 LPARAM lParam  // second message parameter			
						 )			
{									
	
	switch(uMsg)								
	{			
		
	case WM_CLOSE:
		{
	
			EndDialog(hwndDlg,0);
			break;
		}
	case  WM_INITDIALOG :
		{
		
			hedit=GetDlgItem(hwndDlg,IDC_EDIT);
			SetWindowText(hedit,"0");

					InitializeCriticalSection(&cs);
			return TRUE ;

		}
	
							
		
	case  WM_COMMAND :								
		
		switch (LOWORD (wParam))							
		{							
			case IDC_BUTTON:
			{
				 hThread = ::CreateThread(NULL, 0, ThreadProc, 				
					NULL, 0, NULL);			
				
				//如果不在其他的地方引用它 关闭句柄				
				//::CloseHandle(hThread);				

				return TRUE;
			}


			
			case IDC_BUTTON4:
				{
					
					hThread = ::CreateThread(NULL, 0, ThreadProc1, 				
						NULL, 0, NULL);			
					
					//如果不在其他的地方引用它 关闭句柄				
					//::CloseHandle(hThread);				
					
					return TRUE;
			}	


			case IDC_BUTTON1:
				{
					
					::SuspendThread(hThread);
					CONTEXT context;
					context.ContextFlags = CONTEXT_CONTROL;				
					BOOL ok = ::GetThreadContext(hThread,&context);					
					context.Eip = 0x401000;			//挂起线程  跳转到自己的代码 然后恢复现场
					SetThreadContext(hThread,&context);			
					::ResumeThread(hThread);
					return TRUE;
			}	

			case IDC_BUTTON2:
				{
					::ResumeThread(hThread);
					
					return TRUE;
				}
				
			case IDC_BUTTON3:
				{
				
				//	::ExitThread(DWORD dwExitCode); //清理线程的堆栈
				//	::ExitThread(1);

					::TerminateThread(hThread,2);  //异步  exitthread 同步
					::WaitForSingleObject(hThread,INFINITE);
					return TRUE;
			}


			return TRUE;						
		}							
		break ;							
    }									
	
	return FALSE ;								
}									


int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
 	// TODO: Place code here.
	DialogBox(hInstance,MAKEINTRESOURCE(IDD_DIALOG_MAIN),NULL,DialogProc);
	return 0;
}



