// DIRECTORYView.cpp: implementation of the DIRECTORYView class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "DIRECTORYView.h"
#include "ExportTable.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


PIMAGE_OPTIONAL_HEADER DirectoryOption;
HINSTANCE ghInstance;
LPVOID PeFilebuff;

BOOL CALLBACK DirectoryDialogProc(									
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
			DirectoryOption = NULL;
			EndDialog(hwndDlg,0);
			break;
		}
		
	case  WM_INITDIALOG :								
		{
			char szBuffer[0x20];
			//IDC_EDIT_DIRECTORY_ENTRY_EXPORTRVA
			/*
			#define IMAGE_DIRECTORY_ENTRY_EXPORT		 
			#define IMAGE_DIRECTORY_ENTRY_IMPORT		 1 导入表 
			#define IMAGE_DIRECTORY_ENTRY_RESOURCE		 2 资源目录
			#define IMAGE_DIRECTORY_ENTRY_EXCEPTION		 3 异常目录
			#define IMAGE_DIRECTORY_ENTRY_SECURITY		 4 安全目录
			#define IMAGE_DIRECTORY_ENTRY_BASERELOC	         5 重定位基本表
			#define IMAGE_DIRECTORY_ENTRY_DEBUG		 6 调试目录
			#define IMAGE_DIRECTORY_ENTRY_COPYRIGHT		 7 描术字串
			#define IMAGE_DIRECTORY_ENTRY_GLOBALPTR		 8 机器值
			#define IMAGE_DIRECTORY_ENTRY_TLS		 9 TLS目录
			#define IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG	 10 载入配值目录
			#define IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT       11 绑定输入表
			#define IMAGE_DIRECTORY_ENTRY_IAT		 12 导入地址表
			#define IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT	 13 延迟载入描述
			#define IMAGE_DIRECTORY_ENTRY_COM_DESCRIPTOR     14 COM信息
		     */

	
			//0 导出表
			memset(szBuffer, 0, sizeof(szBuffer));
			sprintf(szBuffer, "%x",DirectoryOption->DataDirectory[0].VirtualAddress);	
			SendDlgItemMessage(hwndDlg,IDC_EDIT_DIRECTORY_EXPORTRVA,WM_SETTEXT,0,(DWORD)szBuffer);
		
		
			memset(szBuffer, 0, sizeof(szBuffer));
			sprintf(szBuffer, "%x",DirectoryOption->DataDirectory[0].Size);	
			SendDlgItemMessage(hwndDlg,IDC_EDIT_DIRECTORY_ENTRY_EXPORTSIZE,WM_SETTEXT,0,(DWORD)szBuffer);
		
			//DbgPrintf("%x",DirectoryOption->DataDirectory[1].VirtualAddress);
			// 1 导入
			memset(szBuffer, 0, sizeof(szBuffer));
			sprintf(szBuffer, "%x",DirectoryOption->DataDirectory[1].VirtualAddress);	
			SendDlgItemMessage(hwndDlg,IDC_EDIT_DIRECTORY_ENTRY_IMPORT,WM_SETTEXT,0,(DWORD)szBuffer);
	
			memset(szBuffer, 0, sizeof(szBuffer));
			sprintf(szBuffer, "%x",DirectoryOption->DataDirectory[1].Size);	
			SendDlgItemMessage(hwndDlg,IDC_EDIT_DIRECTORY_ENTRY_IMPORTSIZE,WM_SETTEXT,0,(DWORD)szBuffer);

			//资源表
			memset(szBuffer, 0, sizeof(szBuffer));
			sprintf(szBuffer, "%x",DirectoryOption->DataDirectory[2].VirtualAddress);	
			SendDlgItemMessage(hwndDlg,IDC_EDIT_DIRECTORY_ENTRY_RESOURCE,WM_SETTEXT,0,(DWORD)szBuffer);
			
			memset(szBuffer, 0, sizeof(szBuffer));
			sprintf(szBuffer, "%x",DirectoryOption->DataDirectory[2].Size);	
			SendDlgItemMessage(hwndDlg,IDC_EDIT_DIRECTORY_ENTRY_RESOURCESIZE,WM_SETTEXT,0,(DWORD)szBuffer);
				
			//printf("===异常信息表====\n");

			memset(szBuffer, 0, sizeof(szBuffer));
			sprintf(szBuffer, "%x",DirectoryOption->DataDirectory[3].VirtualAddress);	
			SendDlgItemMessage(hwndDlg,IDC_EDIT_DIRECTORY_ENTRY_EXCEPTION,WM_SETTEXT,0,(DWORD)szBuffer);
			
			memset(szBuffer, 0, sizeof(szBuffer));
			sprintf(szBuffer, "%x",DirectoryOption->DataDirectory[3].Size);	
			SendDlgItemMessage(hwndDlg,IDC_EDIT_DIRECTORY_ENTRY_EXCEPTIONSIZE,WM_SETTEXT,0,(DWORD)szBuffer);
	
			//printf("===安全证书表====\n");
			memset(szBuffer, 0, sizeof(szBuffer));
			sprintf(szBuffer, "%x",DirectoryOption->DataDirectory[4].VirtualAddress);	
			SendDlgItemMessage(hwndDlg,IDC_EDIT_DIRECTORY_ENTRY_SECURITY,WM_SETTEXT,0,(DWORD)szBuffer);
			
			memset(szBuffer, 0, sizeof(szBuffer));
			sprintf(szBuffer, "%x",DirectoryOption->DataDirectory[4].Size);	
			SendDlgItemMessage(hwndDlg,IDC_EDIT_DIRECTORY_ENTRY_SECURITYSIZE,WM_SETTEXT,0,(DWORD)szBuffer);
			
			
			//printf("===重定位表====\n");
			memset(szBuffer, 0, sizeof(szBuffer));
			sprintf(szBuffer, "%x",DirectoryOption->DataDirectory[5].VirtualAddress);	
			SendDlgItemMessage(hwndDlg,IDC_EDIT_DIRECTORY_ENTRY_BASERELOC,WM_SETTEXT,0,(DWORD)szBuffer);
			
			memset(szBuffer, 0, sizeof(szBuffer));
			sprintf(szBuffer, "%x",DirectoryOption->DataDirectory[5].Size);	
			SendDlgItemMessage(hwndDlg,IDC_EDIT_DIRECTORY_ENTRY_BASERELOCSIZE,WM_SETTEXT,0,(DWORD)szBuffer);	
			
			//printf("===调试信息表证书表====\n");
			memset(szBuffer, 0, sizeof(szBuffer));
			sprintf(szBuffer, "%x",DirectoryOption->DataDirectory[6].VirtualAddress);	
			SendDlgItemMessage(hwndDlg,IDC_EDIT_DIRECTORY_ENTRY_DEBUG,WM_SETTEXT,0,(DWORD)szBuffer);
			
			memset(szBuffer, 0, sizeof(szBuffer));
			sprintf(szBuffer, "%x",DirectoryOption->DataDirectory[6].Size);	
			SendDlgItemMessage(hwndDlg,IDC_EDIT_DIRECTORY_ENTRY_DEBUGSIZE,WM_SETTEXT,0,(DWORD)szBuffer);
			

			/*
			#define IMAGE_DIRECTORY_ENTRY_COPYRIGHT		 7 描术字串
			#define 
			#define IMAGE_DIRECTORY录
			#define IMAGE_DIRECTORY_ENTRY_
			#define IMAGE_DIRECTORY_ENTRY_
			#define IMAGE_DIRECTORY_ENTRY_IAT		 12 导入地址表
			#define IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT	 13 延迟载入描述
			#define IMAGE_DIRECTORY_ENTRY_COM_DESCRIPTOR     14 COM信息
			*/
			//printf("===版权所有表====\n");
			memset(szBuffer, 0, sizeof(szBuffer));
			sprintf(szBuffer, "%x",DirectoryOption->DataDirectory[7].VirtualAddress);	
			SendDlgItemMessage(hwndDlg,IDC_EDIT_DIRECTORY_ENTRY_COPYRIGHT,WM_SETTEXT,0,(DWORD)szBuffer);
			
			memset(szBuffer, 0, sizeof(szBuffer));
			sprintf(szBuffer, "%x",DirectoryOption->DataDirectory[7].Size);	
			SendDlgItemMessage(hwndDlg,IDC_EDIT_DIRECTORY_ENTRY_COPYRIGHTSIZE,WM_SETTEXT,0,(DWORD)szBuffer);
			

			//IMAGE_		 8 机器值

			memset(szBuffer, 0, sizeof(szBuffer));
			sprintf(szBuffer, "%x",DirectoryOption->DataDirectory[8].VirtualAddress);	
			SendDlgItemMessage(hwndDlg,IDC_EDIT_DIRECTORY_ENTRY_GLOBALPTR,WM_SETTEXT,0,(DWORD)szBuffer);
			
			memset(szBuffer, 0, sizeof(szBuffer));
			sprintf(szBuffer, "%x",DirectoryOption->DataDirectory[8].Size);	
			SendDlgItemMessage(hwndDlg,IDC_EDIT_DIRECTORY_ENTRY_GLOBALPTRsize,WM_SETTEXT,0,(DWORD)szBuffer);
			
			//_ENTRY_TLS		 9 TLS目
			
			memset(szBuffer, 0, sizeof(szBuffer));
			sprintf(szBuffer, "%x",DirectoryOption->DataDirectory[9].VirtualAddress);	
			SendDlgItemMessage(hwndDlg,IDC_EDIT_DIRECTORY_ENTRY_TLS,WM_SETTEXT,0,(DWORD)szBuffer);
			
			memset(szBuffer, 0, sizeof(szBuffer));
			sprintf(szBuffer, "%x",DirectoryOption->DataDirectory[9].Size);	
			SendDlgItemMessage(hwndDlg,IDC_EDIT_DIRECTORY_ENTRY_TLSSIZE,WM_SETTEXT,0,(DWORD)szBuffer);
			
			//LOAD_CONFIG	 10 载入配值目录
			memset(szBuffer, 0, sizeof(szBuffer));
			sprintf(szBuffer, "%x",DirectoryOption->DataDirectory[10].VirtualAddress);	
			SendDlgItemMessage(hwndDlg,IDC_EDIT_DIRECTORY_ENTRY_LOAD_CONFIG,WM_SETTEXT,0,(DWORD)szBuffer);
			
			memset(szBuffer, 0, sizeof(szBuffer));
			sprintf(szBuffer, "%x",DirectoryOption->DataDirectory[10].Size);	
			SendDlgItemMessage(hwndDlg,IDC_EDIT_DIRECTORY_ENTRY_LOAD_CONFIGSIZE,WM_SETTEXT,0,(DWORD)szBuffer);
			
			//BOUND_IMPORT       11 绑定输入表

			memset(szBuffer, 0, sizeof(szBuffer));
			sprintf(szBuffer, "%x",DirectoryOption->DataDirectory[11].VirtualAddress);	
			SendDlgItemMessage(hwndDlg,IDC_EDIT_DIRECTORY_ENTRY_BOUND_IMPORT,WM_SETTEXT,0,(DWORD)szBuffer);
			
			memset(szBuffer, 0, sizeof(szBuffer));
			sprintf(szBuffer, "%x",DirectoryOption->DataDirectory[11].Size);	
			SendDlgItemMessage(hwndDlg,IDC_EDIT_DIRECTORY_ENTRY_IBOUND_IMPORTSIZE,WM_SETTEXT,0,(DWORD)szBuffer);
			
			//_IAT		 12 导入

			memset(szBuffer, 0, sizeof(szBuffer));
			sprintf(szBuffer, "%x",DirectoryOption->DataDirectory[12].VirtualAddress);	
			SendDlgItemMessage(hwndDlg,IDC_EDIT_DIRECTORY_ENTRY_IAT,WM_SETTEXT,0,(DWORD)szBuffer);
			
			memset(szBuffer, 0, sizeof(szBuffer));
			sprintf(szBuffer, "%x",DirectoryOption->DataDirectory[12].Size);	
			SendDlgItemMessage(hwndDlg,IDC_EDIT_DIRECTORY_ENTRY_IATSIZE,WM_SETTEXT,0,(DWORD)szBuffer);
			
			//13 延迟载入描述

			memset(szBuffer, 0, sizeof(szBuffer));
			sprintf(szBuffer, "%x",DirectoryOption->DataDirectory[13].VirtualAddress);	
			SendDlgItemMessage(hwndDlg,IDC_EDIT_DIRECTORY_ENTRY_DELAY_IMPORT,WM_SETTEXT,0,(DWORD)szBuffer);
			
			memset(szBuffer, 0, sizeof(szBuffer));
			sprintf(szBuffer, "%x",DirectoryOption->DataDirectory[13].Size);	
			SendDlgItemMessage(hwndDlg,IDC_EDIT_DIRECTORY_ENTRY_DELAY_IMPORTSIZE,WM_SETTEXT,0,(DWORD)szBuffer);
			
			//IMAGE_DIRECTORY_ENTRY_COM_DESCRIPTOR     14 COM信
			memset(szBuffer, 0, sizeof(szBuffer));
			sprintf(szBuffer, "%x",DirectoryOption->DataDirectory[14].VirtualAddress);	
			SendDlgItemMessage(hwndDlg,IDC_EDIT_DIRECTORY_ENTRY_COM_DESCRIPTOR,WM_SETTEXT,0,(DWORD)szBuffer);
			
			memset(szBuffer, 0, sizeof(szBuffer));
			sprintf(szBuffer, "%x",DirectoryOption->DataDirectory[14].Size);	
			SendDlgItemMessage(hwndDlg,IDC_EDIT_DIRECTORY_ENTRY_COM_DESCRIPTORSIZE,WM_SETTEXT,0,(DWORD)szBuffer);
			


			memset(szBuffer, 0, sizeof(szBuffer));
			sprintf(szBuffer, "%x",DirectoryOption->DataDirectory[15].VirtualAddress);	
			SendDlgItemMessage(hwndDlg,IDC_EDIT_DIRECTORY_ENTRY_EXP,WM_SETTEXT,0,(DWORD)szBuffer);
			
			memset(szBuffer, 0, sizeof(szBuffer));
			sprintf(szBuffer, "%x",DirectoryOption->DataDirectory[15].Size);	
			SendDlgItemMessage(hwndDlg,IDC_EDIT_DIRECTORY_ENTRY_EXPSIZE,WM_SETTEXT,0,(DWORD)szBuffer);

			return TRUE ;
			
			
		}
								
	case WM_NOTIFY:
		{	
		}
	case  WM_COMMAND :
		switch (LOWORD (wParam))							
		{	
			case IDC_BUTTON_EXPORTRVA:
				{
					ExportDirectoryView(ghInstance,hwndDlg,DirectoryOption,PeFilebuff);
					return TRUE;
				}
		}
								
		return TRUE;							
    }									
	
	return FALSE ;								
}	



void DirectoryView(HINSTANCE hInstance ,HWND hDlg,PIMAGE_OPTIONAL_HEADER Option,LPVOID Filebuff)
{
	DirectoryOption= Option;
	ghInstance =hInstance;
	PeFilebuff = Filebuff;
	DialogBox(hInstance,MAKEINTRESOURCE(IDD_DIALOG_DIRECTORY),hDlg, DirectoryDialogProc);
}
