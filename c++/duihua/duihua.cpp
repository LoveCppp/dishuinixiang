// duihua.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "resource.h"
HINSTANCE hAppInstance;

BOOL CALLBACK DialogProc(									
	HWND hwndDlg,  // handle to dialog box			
	UINT uMsg,     // message			
	WPARAM wParam, // first message parameter			
	LPARAM lParam  // second message parameter			
	)			
{						
	HICON hbigIcon;	
	HICON hsmailIcon;	
	HWND HeditUser = NULL;
	HWND HeditPass = NULL;
	
	switch(uMsg)								
	{								
	case  WM_INITDIALOG :	
		hbigIcon = LoadIcon (hAppInstance, MAKEINTRESOURCE (IDI_ICON2));
		hsmailIcon = LoadIcon (hAppInstance, MAKEINTRESOURCE (IDI_ICON2));		
		//…Ë÷√Õº±Í		
		SendMessage(hwndDlg,WM_SETICON,IDI_ICON1,(DWORD)hbigIcon);		
		SendMessage(hwndDlg,WM_SETICON,IDI_ICON2,(DWORD)hsmailIcon);		
		return TRUE;		

		
		MessageBox(NULL,TEXT("WM_INITDIALOG"),TEXT("INIT"),MB_OK);							
		
		return TRUE ;							
		
	case  WM_COMMAND :								
	
		switch (LOWORD (wParam))							
		{							
		case   IDC_BUTTON_OK :							
			
			HeditUser =  GetDlgItem(hwndDlg,IDC_EDIT_USERNAME);
			HeditPass =  GetDlgItem(hwndDlg,IDC_EDIT_PASSWD);

			TCHAR szUserBuff[0x50];	
			TCHAR szPassBuff[0x50];	
			
			GetWindowText(HeditUser,szUserBuff,0x50);	
			GetWindowText(HeditPass,szPassBuff,0x50);


			MessageBox(NULL,TEXT("IDC_BUTTON_OK"),TEXT("OK"),MB_OK);						
			
			return TRUE;						
			
		case   IDC_BUTTON_OUT:							
			
			MessageBox(NULL,TEXT("IDC_BUTTON_OUT"),TEXT("OUT"),MB_OK);						
			
			EndDialog(hwndDlg, 0);						
			
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
	hAppInstance = hInstance;

	DialogBox(hInstance,MAKEINTRESOURCE(IDD_DIALOG1),NULL,DialogProc);
	return 0;
}



