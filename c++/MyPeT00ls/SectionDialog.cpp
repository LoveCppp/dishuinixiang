// SectionDialog.cpp: implementation of the SectionDialog class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SectionDialog.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


PIMAGE_SECTION_HEADER PeSection;
DWORD NumberOfSections;



void initSectionListView(HWND hDlg){
	LV_COLUMN lv;								
	HWND hListProcess;								
	
	//初始化								
	memset(&lv,0,sizeof(LV_COLUMN));								
	//获取IDC_LIST_MODEL句柄								
	hListProcess = GetDlgItem(hDlg,IDC_LIST_SECTION);								
	//设置整行选中								
	SendMessage(hListProcess,LVM_SETEXTENDEDLISTVIEWSTYLE,LVS_EX_FULLROWSELECT,LVS_EX_FULLROWSELECT);								
	
	//第一列								
	lv.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;								
	lv.pszText = TEXT("名称");				//列标题				
	lv.cx = 80;								//列宽
	lv.iSubItem = 0;								
	//ListView_InsertColumn(hListProcess, 0, &lv);								
	SendMessage(hListProcess,LVM_INSERTCOLUMN,0,(DWORD)&lv);								
	//第二列								
	lv.pszText = TEXT("内存偏移");								
	lv.cx = 80;								
	lv.iSubItem = 1;								
	//ListView_InsertColumn(hListProcess, 1, &lv);								
	SendMessage(hListProcess,LVM_INSERTCOLUMN,1,(DWORD)&lv);
	
	lv.pszText = TEXT("内存大小");								
	lv.cx = 80;								
	lv.iSubItem = 2;								
	//ListView_InsertColumn(hListProcess, 1, &lv);								
	SendMessage(hListProcess,LVM_INSERTCOLUMN,2,(DWORD)&lv);	

	
	lv.pszText = TEXT("文件偏移");								
	lv.cx = 80;								
	lv.iSubItem = 3;								
	//ListView_InsertColumn(hListProcess, 1, &lv);								
	SendMessage(hListProcess,LVM_INSERTCOLUMN,3,(DWORD)&lv);	
	

	
	lv.pszText = TEXT("文件大小");								
	lv.cx = 80;								
	lv.iSubItem = 4;								
	//ListView_InsertColumn(hListProcess, 1, &lv);								
	SendMessage(hListProcess,LVM_INSERTCOLUMN,4,(DWORD)&lv);	
	
	lv.pszText = TEXT("标志");								
	lv.cx = 80;								
	lv.iSubItem = 5;								
	//ListView_InsertColumn(hListProcess, 1, &lv);								
	SendMessage(hListProcess,LVM_INSERTCOLUMN,5,(DWORD)&lv);

	
}




void EnumSection(HWND hwndDlg, WPARAM wParam, LPARAM lParam ){
	

	LV_ITEM vitem;	
	DWORD z=0;							
	
	memset(&vitem, 0, sizeof(LV_ITEM));
	vitem.mask = LVIF_TEXT;
	
	DbgPrintf("%d",NumberOfSections);
	for (int i=0;i<NumberOfSections;i++,PeSection++)
	{

		vitem.pszText= (CHAR*)PeSection->Name;					
		vitem.iItem = i;						
		vitem.iSubItem = 0;						
		//ListView_InsertItem(hListProcess, &vitem);						
		SendMessage(hwndDlg, LVM_INSERTITEM,0,(DWORD)&vitem);
		
		//DbgPrintf("%x",PeSection->VirtualAddress);
		wsprintf(vitem.pszText, TEXT("%x%"), PeSection->VirtualAddress);												
		vitem.iItem = i;								
		vitem.iSubItem = 1;								
		ListView_SetItem(hwndDlg, &vitem);

		//SendMessage(hwndDlg, LVM_INSERTITEM,1,(DWORD)&vitem);

		wsprintf(vitem.pszText, TEXT("%x%"), PeSection->Misc.VirtualSize);												
		vitem.iItem = i;								
		vitem.iSubItem = 2;								
		ListView_SetItem(hwndDlg, &vitem);


		wsprintf(vitem.pszText, TEXT("%x%"), PeSection->PointerToRawData);												
		vitem.iItem = i;								
		vitem.iSubItem = 3;								
		ListView_SetItem(hwndDlg, &vitem);
		


		wsprintf(vitem.pszText, TEXT("%x%"), PeSection->SizeOfRawData);												
		vitem.iItem = i;								
		vitem.iSubItem = 4;								
		ListView_SetItem(hwndDlg, &vitem);

		wsprintf(vitem.pszText, TEXT("%x%"), PeSection->Characteristics);												
		vitem.iItem = i;								
		vitem.iSubItem = 5;								
		ListView_SetItem(hwndDlg, &vitem);
	}
		
	
	
	
	
}




BOOL CALLBACK SectionDialogProc(									
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


			initSectionListView(hwndDlg);
			EnumSection(GetDlgItem(hwndDlg,IDC_LIST_SECTION),wParam,lParam);
			return TRUE ;
		

		}
								
	case WM_NOTIFY:
		{	
		}
	case  WM_COMMAND :								
		return TRUE;							
    }									
	
	return FALSE ;								
}									




void SectionView(HINSTANCE hInstance ,HWND hDlg,PIMAGE_SECTION_HEADER Section,DWORD numbers)
{
	PeSection = Section;
	NumberOfSections = numbers;
	DialogBox(hInstance,MAKEINTRESOURCE(IDD_DIALOG_SECTION),hDlg, SectionDialogProc);
}
