// threadwork.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "resource.h"
#include <windows.h>
#include <stdio.h> 



HWND g_hwnd;

HWND hEditC1,hEditC2,hEditC3,hEditC4;
HWND hEditB1,hEditB2;
HWND hEditRc;


//存储所有线程局句柄
HANDLE hAllThread[5];


//存储消费者文本框
HWND heditCustomer[4];

//缓冲区文本
HWND heditStrbuff[2];

//信号量

HANDLE hSemphoreEmpty,hSemphoreFull;

//临界区
CRITICAL_SECTION  g_Buffer_CS;



//生产者线程

DWORD WINAPI ThreadProduct(				
							LPVOID lpParameter   // thread data		
						){

	//判断资源长度



	TCHAR sizeBuff[256];
	TCHAR sizeTmpBuff[2];
	DWORD dwlength =0;

	TCHAR sizetmp[2]; //拷贝存放的数据

	GetWindowText(hEditRc,sizeBuff,256);
	//获取资源长度
	dwlength = strlen(sizeBuff); 
	
	//循环读取数据
	for (DWORD i =0;i<dwlength;i++)
	{
		memset(sizetmp,0,2);
		//取字母
		memcpy(sizetmp,&sizeBuff[i],1);
		WaitForSingleObject(hSemphoreEmpty,INFINITE);
		//将字母存储到缓冲区
		for(DWORD k=0;k<2;k++){
			//读取缓冲区中的值是否为0
			EnterCriticalSection(&g_Buffer_CS);
			GetWindowText(heditStrbuff[k],sizeTmpBuff,256);
			if (strcmp(sizeTmpBuff,"0") == 0)
			{
				//写入缓冲区
				SetWindowText(heditStrbuff[k],sizetmp);
				LeaveCriticalSection(&g_Buffer_CS);
				break;
			}
			LeaveCriticalSection(&g_Buffer_CS);
			
		}
		//加消费者信号量

		ReleaseSemaphore(hSemphoreFull,1,NULL);

	}


	return 0;
}

DWORD WINAPI ThreadCustomer(				
							LPVOID lpParameter   // thread data		
							){

	TCHAR sizeSubffer[2];
	DWORD dwIndex;
	dwIndex = (DWORD)lpParameter;
	while(TRUE){
		WaitForSingleObject(hSemphoreFull,INFINITE);
		//缓冲区读取字母，写到文本中，将文本框的值置0
		for (DWORD i=0;i<2;i++)
		{
			//获取缓冲区
			EnterCriticalSection(&g_Buffer_CS);
			GetWindowText(heditStrbuff[i],sizeSubffer,2);
			if (strcmp(sizeSubffer,"0") != 0)
			{
				//写到自己的文本框中
				SetWindowText(heditCustomer[dwIndex],sizeSubffer);
				//缓冲区设置为0
				SetWindowText(heditStrbuff[i],"0");
				LeaveCriticalSection(&g_Buffer_CS);
				break;
			}
			LeaveCriticalSection(&g_Buffer_CS);
		}
		ReleaseSemaphore(hSemphoreEmpty,1,NULL);
		
	}
	

	return 0;

}






DWORD WINAPI ThreadProcMain(				
						LPVOID lpParameter   // thread data		
						){

	//创建信号量
	//生产者线程使用
	hSemphoreEmpty = CreateSemaphore(NULL,2,2,NULL); 
	//消费者线程用
	hSemphoreFull =  CreateSemaphore(NULL,0,2,NULL);
	

	InitializeCriticalSection(&g_Buffer_CS);						

	//创建1个生产者线程
	hAllThread[0] = ::CreateThread(NULL,0,ThreadProduct,NULL,0,NULL);
	
	//创建4个消费者线程
	hAllThread[1] = ::CreateThread(NULL,0,ThreadCustomer,(VOID*)0,0,NULL);
	hAllThread[2] = ::CreateThread(NULL,0,ThreadCustomer,(VOID*)1,0,NULL);
	hAllThread[3] = ::CreateThread(NULL,0,ThreadCustomer,(VOID*)2,0,NULL);
	hAllThread[4] = ::CreateThread(NULL,0,ThreadCustomer,(VOID*)3,0,NULL);

	//等待所有线程结束 
	::WaitForMultipleObjects(5,hAllThread,TRUE,INFINITE);

	//关闭句柄
	::CloseHandle(hAllThread[0]);
	::CloseHandle(hAllThread[1]);
	::CloseHandle(hAllThread[2]);
	::CloseHandle(hAllThread[3]);
	::CloseHandle(hAllThread[4]);
	
	::CloseHandle(hSemphoreEmpty);
	::CloseHandle(hSemphoreFull);

	//释放临界区

	::DeleteCriticalSection(&g_Buffer_CS);
	
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
			//资源
			hEditRc = GetDlgItem(hwndDlg,IDC_EDITRC);
			
			//生产者
			hEditB1 = GetDlgItem(hwndDlg,IDC_EDITB1);
			hEditB2 = GetDlgItem(hwndDlg,IDC_EDITB2);
			
			heditStrbuff[0] = hEditB1;
			heditStrbuff[1] = hEditB2;
			//消费者

			hEditC1 = GetDlgItem(hwndDlg,IDC_EDITC1);
			hEditC2 = GetDlgItem(hwndDlg,IDC_EDITC2);
			hEditC3 = GetDlgItem(hwndDlg,IDC_EDITC3);
			hEditC4 = GetDlgItem(hwndDlg,IDC_EDITC4);

			heditCustomer[0] = hEditC1;
			heditCustomer[1] = hEditC2;
			heditCustomer[2] = hEditC3;
			heditCustomer[3] = hEditC4;

			//设置初始值
	
			SetWindowText(hEditRc,"0");
			SetWindowText(hEditB1,"0");
			SetWindowText(hEditB2,"0");
			SetWindowText(hEditC1,"");
			SetWindowText(hEditC2,"");
			SetWindowText(hEditC3,"");
			SetWindowText(hEditC4,"");

			
			return TRUE ;
			
		}
		
		
		
	case  WM_COMMAND :								
		
		switch (LOWORD (wParam))							
		{							
			case IDC_BUTTON1:
				{
				

					HANDLE 	hThread = ::CreateThread(NULL, 0, ThreadProcMain, 				
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



