// ExportTable.cpp: implementation of the ExportTable class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ExportTable.h"
#include "T00ls.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#include "resource.h"


PIMAGE_OPTIONAL_HEADER Export_Directory;
LPVOID PeFileBuff;

void EnumExprotTable(HWND hDlg,PIMAGE_EXPORT_DIRECTORY Export_DirectoryTable){
	LV_ITEM vitem;	
	HWND hListProcess;
	memset(&vitem, 0, sizeof(LV_ITEM));
	vitem.mask = LVIF_TEXT;

	hListProcess = GetDlgItem(hDlg,IDC_LIST_ExportTable);


	WORD* AddressOfNameOrdinalsAddress =(WORD*)((DWORD)PeFileBuff + RvaToFileOffset(PeFileBuff,Export_DirectoryTable->AddressOfNameOrdinals));
	DWORD* AddressOfNamesFunctionsAddress = (DWORD*)((char*)PeFileBuff + RvaToFileOffset(PeFileBuff,Export_DirectoryTable->AddressOfNames));
	DWORD* AddressOfFunctionsAddress = (DWORD*)((char*)PeFileBuff + RvaToFileOffset(PeFileBuff,Export_DirectoryTable->AddressOfFunctions));
	char szBuffer[0x20];

	for(int j =0;j<Export_DirectoryTable->NumberOfNames;j++,AddressOfNameOrdinalsAddress++,AddressOfFunctionsAddress++,AddressOfNamesFunctionsAddress++)
	{
		
			//0 ������
		memset(szBuffer, 0, sizeof(szBuffer));
		sprintf(szBuffer, "%x",*AddressOfNameOrdinalsAddress + Export_DirectoryTable->Base);	
		vitem.pszText = szBuffer;						
		vitem.iItem = j;						
		vitem.iSubItem = 0;						
		//ListView_InsertItem(hListProcess, &vitem);						
		SendMessage(hListProcess, LVM_INSERTITEM,0,(DWORD)&vitem);	
		
		memset(szBuffer, 0, sizeof(szBuffer));
		sprintf(szBuffer, "%x",*AddressOfFunctionsAddress);	
		vitem.pszText = szBuffer;											
		vitem.iItem = j;						
		vitem.iSubItem = 1;						
		ListView_SetItem(hListProcess, &vitem);			
		
		
		memset(szBuffer, 0, sizeof(szBuffer));
		sprintf(szBuffer, "%x",*AddressOfNamesFunctionsAddress);	
		vitem.pszText = szBuffer;											
		vitem.iItem = j;						
		vitem.iSubItem = 2;						
		ListView_SetItem(hListProcess, &vitem);	
		
		memset(szBuffer, 0, sizeof(szBuffer));
		sprintf(szBuffer, "%s",(char*)PeFileBuff+RvaToFileOffset(PeFileBuff,*AddressOfNamesFunctionsAddress));	
		vitem.pszText = szBuffer;											
		vitem.iItem = j;						
		vitem.iSubItem = 3;						
		ListView_SetItem(hListProcess, &vitem);	



	}
	return;
	

}

void initExportTableListView(HWND hDlg){
	LV_COLUMN lv;								
	HWND hListProcess;								
	
	//��ʼ��								
	memset(&lv,0,sizeof(LV_COLUMN));								
	//��ȡIDC_LIST_PROCESS���								
	hListProcess = GetDlgItem(hDlg,IDC_LIST_ExportTable);								
	//��������ѡ��								
	SendMessage(hListProcess,LVM_SETEXTENDEDLISTVIEWSTYLE,LVS_EX_FULLROWSELECT,LVS_EX_FULLROWSELECT);								
	
	//��һ��								
	lv.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;								
	lv.pszText = TEXT("Ordinal");				//�б���				
	lv.cx = 150;								//�п�
	lv.iSubItem = 0;								
	//ListView_InsertColumn(hListProcess, 0, &lv);								
	SendMessage(hListProcess,LVM_INSERTCOLUMN,0,(DWORD)&lv);								
	//�ڶ���								
	lv.pszText = TEXT("EVA");								
	lv.cx = 80;								
	lv.iSubItem = 1;								
	//ListView_InsertColumn(hListProcess, 1, &lv);								
	SendMessage(hListProcess,LVM_INSERTCOLUMN,1,(DWORD)&lv);								
	//������								
	lv.pszText = TEXT("Offset");								
	lv.cx = 80;								
	lv.iSubItem = 2;								
	ListView_InsertColumn(hListProcess, 2, &lv);								
	//������								
	lv.pszText = TEXT("FunctionName");								
	lv.cx = 80;								
	lv.iSubItem = 3;								
	ListView_InsertColumn(hListProcess, 3, &lv);								

}




BOOL CALLBACK ExportDirectoryDialogProc(									
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
			if (Export_Directory->DataDirectory[0].VirtualAddress == 0)
			{
				MessageBox(0,TEXT("û�е����"),0,0);
				EndDialog(hwndDlg,0);
				return TRUE;
			}
			
			PIMAGE_EXPORT_DIRECTORY Export_DirectoryTable = (PIMAGE_EXPORT_DIRECTORY)((char*)PeFileBuff + RvaToFileOffset(PeFileBuff,Export_Directory->DataDirectory[0].VirtualAddress));

			char szBuffer[0x20];


			
			
		
			memset(szBuffer, 0, sizeof(szBuffer));
			sprintf(szBuffer, "%x",Export_DirectoryTable->NumberOfFunctions);	
			SendDlgItemMessage(hwndDlg,IDC_EDIT_NumberOfFunctions,WM_SETTEXT,0,(DWORD)szBuffer);


			memset(szBuffer, 0, sizeof(szBuffer));
			sprintf(szBuffer, "%x",Export_DirectoryTable->NumberOfNames);	
			SendDlgItemMessage(hwndDlg,IDC_EDIT_NumberOfNames,WM_SETTEXT,0,(DWORD)szBuffer);
			

			memset(szBuffer, 0, sizeof(szBuffer));
			sprintf(szBuffer, "%x",Export_DirectoryTable->AddressOfFunctions);	
			SendDlgItemMessage(hwndDlg,IDC_EDIT_AddressOfFunctions,WM_SETTEXT,0,(DWORD)szBuffer);
			

			initExportTableListView(hwndDlg);
			
			EnumExprotTable(hwndDlg,Export_DirectoryTable);
		
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
					
					return TRUE;
				}
		}
		return TRUE;							
    }									
	
	return FALSE ;								
}									






void ExportDirectoryView(HINSTANCE hInstance ,HWND hDlg,PIMAGE_OPTIONAL_HEADER Directory,LPVOID FileBuff)
{
	Export_Directory= Directory;
	PeFileBuff = FileBuff;
	
	DialogBox(hInstance,MAKEINTRESOURCE(IDD_DIALOG_EXPORTR),hDlg, ExportDirectoryDialogProc);
}
