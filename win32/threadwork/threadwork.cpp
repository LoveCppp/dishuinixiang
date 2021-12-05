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


//�洢�����߳̾־��
HANDLE hAllThread[5];


//�洢�������ı���
HWND heditCustomer[4];

//�������ı�
HWND heditStrbuff[2];

//�ź���

HANDLE hSemphoreEmpty,hSemphoreFull;

//�ٽ���
CRITICAL_SECTION  g_Buffer_CS;



//�������߳�

DWORD WINAPI ThreadProduct(				
							LPVOID lpParameter   // thread data		
						){

	//�ж���Դ����



	TCHAR sizeBuff[256];
	TCHAR sizeTmpBuff[2];
	DWORD dwlength =0;

	TCHAR sizetmp[2]; //������ŵ�����

	GetWindowText(hEditRc,sizeBuff,256);
	//��ȡ��Դ����
	dwlength = strlen(sizeBuff); 
	
	//ѭ����ȡ����
	for (DWORD i =0;i<dwlength;i++)
	{
		memset(sizetmp,0,2);
		//ȡ��ĸ
		memcpy(sizetmp,&sizeBuff[i],1);
		WaitForSingleObject(hSemphoreEmpty,INFINITE);
		//����ĸ�洢��������
		for(DWORD k=0;k<2;k++){
			//��ȡ�������е�ֵ�Ƿ�Ϊ0
			EnterCriticalSection(&g_Buffer_CS);
			GetWindowText(heditStrbuff[k],sizeTmpBuff,256);
			if (strcmp(sizeTmpBuff,"0") == 0)
			{
				//д�뻺����
				SetWindowText(heditStrbuff[k],sizetmp);
				LeaveCriticalSection(&g_Buffer_CS);
				break;
			}
			LeaveCriticalSection(&g_Buffer_CS);
			
		}
		//���������ź���

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
		//��������ȡ��ĸ��д���ı��У����ı����ֵ��0
		for (DWORD i=0;i<2;i++)
		{
			//��ȡ������
			EnterCriticalSection(&g_Buffer_CS);
			GetWindowText(heditStrbuff[i],sizeSubffer,2);
			if (strcmp(sizeSubffer,"0") != 0)
			{
				//д���Լ����ı�����
				SetWindowText(heditCustomer[dwIndex],sizeSubffer);
				//����������Ϊ0
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

	//�����ź���
	//�������߳�ʹ��
	hSemphoreEmpty = CreateSemaphore(NULL,2,2,NULL); 
	//�������߳���
	hSemphoreFull =  CreateSemaphore(NULL,0,2,NULL);
	

	InitializeCriticalSection(&g_Buffer_CS);						

	//����1���������߳�
	hAllThread[0] = ::CreateThread(NULL,0,ThreadProduct,NULL,0,NULL);
	
	//����4���������߳�
	hAllThread[1] = ::CreateThread(NULL,0,ThreadCustomer,(VOID*)0,0,NULL);
	hAllThread[2] = ::CreateThread(NULL,0,ThreadCustomer,(VOID*)1,0,NULL);
	hAllThread[3] = ::CreateThread(NULL,0,ThreadCustomer,(VOID*)2,0,NULL);
	hAllThread[4] = ::CreateThread(NULL,0,ThreadCustomer,(VOID*)3,0,NULL);

	//�ȴ������߳̽��� 
	::WaitForMultipleObjects(5,hAllThread,TRUE,INFINITE);

	//�رվ��
	::CloseHandle(hAllThread[0]);
	::CloseHandle(hAllThread[1]);
	::CloseHandle(hAllThread[2]);
	::CloseHandle(hAllThread[3]);
	::CloseHandle(hAllThread[4]);
	
	::CloseHandle(hSemphoreEmpty);
	::CloseHandle(hSemphoreFull);

	//�ͷ��ٽ���

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
			//��Դ
			hEditRc = GetDlgItem(hwndDlg,IDC_EDITRC);
			
			//������
			hEditB1 = GetDlgItem(hwndDlg,IDC_EDITB1);
			hEditB2 = GetDlgItem(hwndDlg,IDC_EDITB2);
			
			heditStrbuff[0] = hEditB1;
			heditStrbuff[1] = hEditB2;
			//������

			hEditC1 = GetDlgItem(hwndDlg,IDC_EDITC1);
			hEditC2 = GetDlgItem(hwndDlg,IDC_EDITC2);
			hEditC3 = GetDlgItem(hwndDlg,IDC_EDITC3);
			hEditC4 = GetDlgItem(hwndDlg,IDC_EDITC4);

			heditCustomer[0] = hEditC1;
			heditCustomer[1] = hEditC2;
			heditCustomer[2] = hEditC3;
			heditCustomer[3] = hEditC4;

			//���ó�ʼֵ
	
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



