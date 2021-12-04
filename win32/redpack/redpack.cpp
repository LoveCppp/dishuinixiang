// redpack.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "resource.h"
#include <windows.h>
#include <stdio.h> 


HANDLE hThread;
HWND g_hwnd;
HANDLE hHandleArr[5];
char string_buffer1[110];
char string_buffer2[110];

CRITICAL_SECTION cs; 
int g_num1 = 0;
int g_num2 = 0;
int g_num3 = 0;
int g_total = 0; // 用来存放红包总大小

DWORD WINAPI GetMoney1(				
						LPVOID lpParameter   // thread data		
						){
	while(1){
		EnterCriticalSection(&cs);
		if (g_total <= 0)
		{
			LeaveCriticalSection(&cs);
			break;
		}
		if (g_total >= 50)
		{
			g_total -= 50;
			g_num1 += 50;
		}
		else
		{
			g_num1 += g_total;
			g_total = 0;
		}
		sprintf(string_buffer1, "%d", g_total);
		sprintf(string_buffer2, "%d", g_num1);
		SetWindowText(GetDlgItem(g_hwnd, IDC_EDIT2), string_buffer2);
		SetWindowText(GetDlgItem(g_hwnd, IDC_EDIT1), string_buffer1);
		LeaveCriticalSection(&cs);
		Sleep(50);

	}


	return 0;
}



DWORD WINAPI GetMoney2(				
					  LPVOID lpParameter   // thread data		
					  ){


	
	while(1){
		EnterCriticalSection(&cs);
		if (g_total <= 0)
		{
			LeaveCriticalSection(&cs);
			break;
		}
		if (g_total >= 50)
		{
			g_total -= 50;
			g_num2 += 50;
		}
		else
		{
			g_num2 += g_total;
			g_total = 0;
		}
		sprintf(string_buffer1, "%d", g_total);
		sprintf(string_buffer2, "%d", g_num2);
		SetWindowText(GetDlgItem(g_hwnd, IDC_EDIT3), string_buffer2);
		SetWindowText(GetDlgItem(g_hwnd, IDC_EDIT1), string_buffer1);
		LeaveCriticalSection(&cs);
		Sleep(50);
	}

	
	return 0;
}


DWORD WINAPI GetMoney3(				
					  LPVOID lpParameter   // thread data		
					  ){

	while(g_total>50){


	
		EnterCriticalSection(&cs);
			if (g_total <= 0)
			{
				LeaveCriticalSection(&cs);
				break;
			}
			if (g_total >= 50)
			{
				g_total -= 50;
				g_num3 += 50;
			}
			else
			{
				g_num3 += g_total;
				g_total = 0;
		}
		sprintf(string_buffer1, "%d", g_total);
		sprintf(string_buffer2, "%d", g_num3);
		SetWindowText(GetDlgItem(g_hwnd, IDC_EDIT4), string_buffer2);
		SetWindowText(GetDlgItem(g_hwnd, IDC_EDIT1), string_buffer1);
		LeaveCriticalSection(&cs);
		Sleep(50);
	}

	return 0;
}


DWORD WINAPI ThreadProc(				
						LPVOID lpParameter   // thread data		
						){
	InitializeCriticalSection(&cs);
	GetWindowText(GetDlgItem(g_hwnd, IDC_EDIT1), string_buffer1,100);
	sscanf(string_buffer1,"%d",&g_total);

	if (g_total < 50)
	{
		MessageBox(0,"红包总数不能为0",0,0);
		return 0;
	}

	hHandleArr[0] = ::CreateThread(NULL, 0, GetMoney1,NULL, 0, NULL);	
	hHandleArr[1] = ::CreateThread(NULL, 0, GetMoney2,NULL, 0, NULL);	
	hHandleArr[2] = ::CreateThread(NULL, 0, GetMoney3,NULL, 0, NULL);
	::WaitForMultipleObjects(3, hHandleArr, TRUE, -1); // 有一个线程结束说明抢红包过程结束
	::CloseHandle(hHandleArr[0]);
	::CloseHandle(hHandleArr[1]);
	::CloseHandle(hHandleArr[2]); // 防止内核对象泄漏

	return 0;
}




BOOL CALLBACK DialogProc(									
						 HWND hwndDlg,  // handle to dialog box			
						 UINT uMsg,     // message			
						 WPARAM wParam, // first message parameter			
						 LPARAM lParam  // second message parameter			
						 )			
{									
	g_hwnd = hwndDlg;
	switch(uMsg)								
	{			
		
	case WM_CLOSE:
		{
			
			EndDialog(hwndDlg,0);
			break;
		}
	case  WM_INITDIALOG :
		{
			SetWindowText(GetDlgItem(g_hwnd, IDC_EDIT1), "0");
			SetWindowText(GetDlgItem(g_hwnd, IDC_EDIT2), "0");
			SetWindowText(GetDlgItem(g_hwnd, IDC_EDIT3), "0");
			SetWindowText(GetDlgItem(g_hwnd, IDC_EDIT4), "0");
			return TRUE ;
			
		}
		
		
		
	case  WM_COMMAND :								
		
		switch (LOWORD (wParam))							
		{							
		case IDC_BUTTON1:
			{
				hThread = ::CreateThread(NULL, 0, ThreadProc, 				
					NULL, 0, NULL);		
				::CloseHandle(hThread);
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
	DialogBox(hInstance,MAKEINTRESOURCE(IDD_DIALOG1),NULL,DialogProc);

	return 0;
}



