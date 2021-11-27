// MyPeT00ls.cpp : Defines the entry point for the application.
//

#include "stdafx.h"	
#include "PeToolsDiaog.h"
#include "resource.h"



HWND gHwndDlg;
HINSTANCE gHInstance;
void EnumProcess(HWND hDlg){
	LV_ITEM vitem;								
	HWND hListProcess;	
	PROCESSENTRY32 pe32;
	MODULEENTRY32 me32;
	HANDLE hProcess,hSnapshot_mod;
	DWORD n =0;
	
	//获取IDC_LIST_PROCESS句柄								
	hListProcess = GetDlgItem(hDlg,IDC_LIST_PROCESS);
	//初始化								
	memset(&vitem,0,sizeof(LV_ITEM));								
	vitem.mask = LVIF_TEXT;	
	
	HANDLE hProceessnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);  

	 if (hProceessnap == INVALID_HANDLE_VALUE)  
	 {  

		 return;  
	 }  
	 else  
	 {  
	
		 pe32.dwSize = sizeof(pe32);
		 HANDLE hSnapshot_proc = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

		 if (INVALID_HANDLE_VALUE == hSnapshot_proc)
		 {
			 MessageBox(NULL, TEXT("没有权限获取进程模块"), ("出错啦"), MB_OK);
			 return;
		 }
		
		
	
		 if (hSnapshot_proc != INVALID_HANDLE_VALUE)
		 {
	 
	
			 BOOL check = Process32First(hSnapshot_proc, &pe32);
			
			 while (check)
			 {
				 hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pe32.th32ProcessID);
				 hSnapshot_mod = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pe32.th32ProcessID);
				 me32.dwSize = sizeof(me32);
				 Module32First(hSnapshot_mod, &me32);
				 //printf("进程PID = %d 进程名 = %s\n", pe32.th32ProcessID, pe32.szExeFile);
				 vitem.pszText = pe32.szExeFile;						
				 vitem.iItem = n;						
				 vitem.iSubItem = 0;						
				 //ListView_InsertItem(hListProcess, &vitem);						
				 SendMessage(hListProcess, LVM_INSERTITEM,0,(DWORD)&vitem);						

				wsprintf(vitem.pszText, TEXT("%d%"), pe32.th32ProcessID);								
				vitem.iItem = n;								
				vitem.iSubItem = 1;								
				ListView_SetItem(hListProcess, &vitem);					
				
		
				wsprintf(vitem.pszText, TEXT("%x%"), me32.modBaseAddr);								
				vitem.iItem = n;								
				vitem.iSubItem = 2;								
				ListView_SetItem(hListProcess, &vitem);	


				wsprintf(vitem.pszText, TEXT("%d%"), me32.modBaseSize);								
				vitem.iItem = n;								
				vitem.iSubItem = 3;								
				ListView_SetItem(hListProcess, &vitem);	
				n++;
				check = Process32Next(hSnapshot_proc, &pe32);
				 CloseHandle(hProcess);
			 }
		 }
		 
		 CloseHandle(hSnapshot_proc);	
	 }  
	
    return;  
							
}



void initProcessListView(HWND hDlg){
	LV_COLUMN lv;								
	HWND hListProcess;								
	
	//初始化								
	memset(&lv,0,sizeof(LV_COLUMN));								
	//获取IDC_LIST_PROCESS句柄								
	hListProcess = GetDlgItem(hDlg,IDC_LIST_PROCESS);								
	//设置整行选中								
	SendMessage(hListProcess,LVM_SETEXTENDEDLISTVIEWSTYLE,LVS_EX_FULLROWSELECT,LVS_EX_FULLROWSELECT);								
	
	//第一列								
	lv.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;								
	lv.pszText = TEXT("进程");				//列标题				
	lv.cx = 150;								//列宽
	lv.iSubItem = 0;								
	//ListView_InsertColumn(hListProcess, 0, &lv);								
	SendMessage(hListProcess,LVM_INSERTCOLUMN,0,(DWORD)&lv);								
	//第二列								
	lv.pszText = TEXT("PID");								
	lv.cx = 80;								
	lv.iSubItem = 1;								
	//ListView_InsertColumn(hListProcess, 1, &lv);								
	SendMessage(hListProcess,LVM_INSERTCOLUMN,1,(DWORD)&lv);								
	//第三列								
	lv.pszText = TEXT("镜像基址");								
	lv.cx = 80;								
	lv.iSubItem = 2;								
	ListView_InsertColumn(hListProcess, 2, &lv);								
	//第四列								
	lv.pszText = TEXT("镜像大小");								
	lv.cx = 80;								
	lv.iSubItem = 3;								
	ListView_InsertColumn(hListProcess, 3, &lv);								
	EnumProcess(hDlg);
}




void initModelListView(HWND hDlg){
	LV_COLUMN lv;								
	HWND hListProcess;								
	
	//初始化								
	memset(&lv,0,sizeof(LV_COLUMN));								
	//获取IDC_LIST_MODEL句柄								
	hListProcess = GetDlgItem(hDlg,IDC_LIST_MODEL);								
	//设置整行选中								
	SendMessage(hListProcess,LVM_SETEXTENDEDLISTVIEWSTYLE,LVS_EX_FULLROWSELECT,LVS_EX_FULLROWSELECT);								
	
	//第一列								
	lv.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;								
	lv.pszText = TEXT("模块名称");				//列标题				
	lv.cx = 200;								//列宽
	lv.iSubItem = 0;								
	//ListView_InsertColumn(hListProcess, 0, &lv);								
	SendMessage(hListProcess,LVM_INSERTCOLUMN,0,(DWORD)&lv);								
	//第二列								
	lv.pszText = TEXT("模块句柄");								
	lv.cx = 200;								
	lv.iSubItem = 1;								
	//ListView_InsertColumn(hListProcess, 1, &lv);								
	SendMessage(hListProcess,LVM_INSERTCOLUMN,1,(DWORD)&lv);								

}






void EnumModels(HWND hwndDlg, WPARAM wParam, LPARAM lParam ){
	
    MODULEENTRY32 me32;
	HWND hListProcess;
    HANDLE hProcess, hSnapshot_mod;
	DWORD dwRowId;
	LV_ITEM lv,vitem;	
	TCHAR szPid[0x20];
	DWORD z=0;

	memset(&lv,0,sizeof(LV_ITEM));
	memset(&szPid,0,0x20);
	
	dwRowId = SendMessage(hwndDlg,LVM_GETNEXTITEM,-1,LVNI_SELECTED);

	if(dwRowId == -1){
		MessageBox(NULL,TEXT("请选择进程"),TEXT("出错了"),MB_OK);
		return;
	}
	
	lv.iSubItem = 1;
	lv.pszText=szPid;
	lv.cchTextMax = 0x20;
	SendMessage(hwndDlg,LVM_GETITEMTEXT,dwRowId,(DWORD)&lv);
	
	//MessageBox(NULL,szPid,TEXT("PID"),MB_OK);
	//初始化								
	
	memset(&vitem, 0, sizeof(LV_ITEM));
	vitem.mask = LVIF_TEXT;

	hListProcess = GetDlgItem(gHwndDlg,IDC_LIST_MODEL);
					
	//ListView_InsertItem(hListProcess, &vitem);						
	SendMessage(hListProcess, LVM_INSERTITEM,0,(DWORD)&vitem);

	hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, atoi(szPid));
	if (atoi(szPid) && atoi(szPid) != 4 && atoi(szPid) != 8)
	{
		me32.dwSize = sizeof(me32);
		hSnapshot_mod = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, atoi(szPid));
		Module32First(hSnapshot_mod, &me32);
		do
		{
			vitem.pszText = me32.szModule;						
			vitem.iItem = z;						
			vitem.iSubItem = 0;						
			//ListView_InsertItem(hListProcess, &vitem);						
		    SendMessage(hListProcess, LVM_INSERTITEM,0,(DWORD)&vitem);	

			wsprintf(vitem.pszText, TEXT("%x%"), me32.hModule);								
			vitem.iItem = z;								
			vitem.iSubItem = 1;								
			ListView_SetItem(hListProcess, &vitem);
	

			z++;
			

		} while (Module32Next(hSnapshot_mod, &me32));
	
		CloseHandle(hSnapshot_mod);
	}
    CloseHandle(hProcess);



}




BOOL CALLBACK DialogProc(									
						 HWND hwndDlg,  // handle to dialog box			
						 UINT uMsg,     // message			
						 WPARAM wParam, // first message parameter			
						 LPARAM lParam  // second message parameter			
						 )			
{									
	gHwndDlg = hwndDlg;
	switch(uMsg)								
	{			
	case WM_CLOSE:
		{
			gHwndDlg = NULL;
			gHInstance = NULL;
			EndDialog(hwndDlg,0);
			break;
		}

	case  WM_INITDIALOG :								
		{
			//MessageBox(NULL,TEXT("WM_INITDIALOG"),TEXT("INIT"),MB_OK);							
			initProcessListView(hwndDlg);
			initModelListView(hwndDlg);
			return TRUE ;
		}
								
	case WM_NOTIFY:
		{
			NMHDR* pMHDR = (NMHDR*)lParam;

			if (wParam == IDC_LIST_PROCESS && pMHDR->code == NM_CLICK )
			{
				EnumModels(GetDlgItem(hwndDlg,IDC_LIST_PROCESS),wParam,lParam);
			}

		}
	case  WM_COMMAND :								
	
		switch (LOWORD (wParam))							
		{		
		
		case  IDC_BUTTON_PE:
			{
				OPENFILENAME setOenFile;
				TCHAR szPeFileExt[100] =TEXT("PE File\0*.EXE;*.DLL;*.SCR;*.DRV;*.SYS\0");
				TCHAR szFileName[256];
				memset(&szFileName,0,256);
				memset(&setOenFile,0,sizeof(OPENFILENAME));			
				setOenFile.lStructSize = sizeof(OPENFILENAME);
				setOenFile.Flags = OFN_FILEMUSTEXIST|OFN_PATHMUSTEXIST; //OFN_FILEMUSTEXIST 指定用户仅可以在文件名登录字段中输入已存在的文件的名字。如果这个标记被指定的并且用户输入了一个无效的名字，对话框程序显示一个等待消息框。如果这个标记被指定，OFN_PATHMUSTEXIST标记也被使用。 
				setOenFile.hwndOwner = hwndDlg;
				setOenFile.lpstrFilter = szPeFileExt;
				setOenFile.lpstrFile = szFileName;
				setOenFile.nMaxFile = MAX_PATH;
				GetOpenFileName(&setOenFile);
				
				PeToolView(gHInstance,hwndDlg,szFileName);
				

				return TRUE;
			}
		case  IDC_BUTTON_ABOUT :
			{					
				DialogBox(gHInstance,MAKEINTRESOURCE(IDD_DIALOG_ABLOUT),hwndDlg, DialogProc );
				return TRUE;
			}
		case   IDC_BUTTON_EXIT:												
			{
				EndDialog(hwndDlg, 0);						
				return TRUE;
			}
								
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
	gHInstance = hInstance;
	INITCOMMONCONTROLSEX icex;				
	icex.dwSize = sizeof(INITCOMMONCONTROLSEX);				
	icex.dwICC = ICC_WIN95_CLASSES;				
	InitCommonControlsEx(&icex);				


 	// TODO: Place code here.
	 DialogBox(	hInstance, MAKEINTRESOURCE(IDD_DIALOG_MAIN),  NULL,  DialogProc  );					

	return 0;
}



