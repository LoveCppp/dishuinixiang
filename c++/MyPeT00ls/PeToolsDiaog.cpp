// PeToolsDiaog.cpp: implementation of the PeToolsDiaog class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "PeToolsDiaog.h"
#include "SectionDialog.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


TCHAR* FileName;
LPVOID pFilebuff = NULL;
HINSTANCE peHInstance;
struct  PeDesc
{
	PIMAGE_FILE_HEADER HEADER;
	PIMAGE_OPTIONAL_HEADER32 opheader;
	PIMAGE_SECTION_HEADER section;
};

PeDesc peDesc;
void ReadPE(TCHAR* FileName){
	FILE* pFile = NULL;
	DWORD fileSize = 0;
	//打开文件
	pFile = fopen(FileName, "rb");
	if (!pFile)
	{
		MessageBox(0,TEXT(" 无法打开 EXE 文件!失败!!"),0,0);
		
		return;
	}
	//读取文件大小
	
	fseek(pFile, 0, SEEK_END);
	fileSize = ftell(pFile);
	fseek(pFile, 0, SEEK_SET);
	//分配缓冲区	
	pFilebuff = malloc(fileSize);
	
	
	if (!pFilebuff)
	{
	
		MessageBox(0,TEXT(" 分配空间失败!!"),0,0);
		fclose(pFile); //然后需要关闭文件
		return;
	}
	
	size_t n = fread(pFilebuff, fileSize, 1, pFile);
	if (!n)
	{
	
		MessageBox(0,TEXT(" 读取数据失败!!"),0,0);
		free(pFilebuff);
		fclose(pFile);
		return;
	}
	


	PIMAGE_DOS_HEADER pDosHeader = NULL;
	PIMAGE_NT_HEADERS pNTHeader = NULL;
	PIMAGE_FILE_HEADER pPEHeader = NULL;
	PIMAGE_OPTIONAL_HEADER32 pOptionHeader = NULL;
	PIMAGE_SECTION_HEADER pSectionHeader = NULL;
	LPVOID pTempBuffer = NULL;
	bool Ssize = false;
	if(!pFilebuff)
	{
	
		MessageBox(0,TEXT("读取到内存的pfilebuffer无效!"),0,0);
		return;
	}
	//判断是不是exe文件
	if(*((PWORD)pFilebuff) != IMAGE_DOS_SIGNATURE)
	{
	
		MessageBox(0,TEXT("不含MZ标志，不是exe文件!"),0,0);
		return;
	}
	pDosHeader = (PIMAGE_DOS_HEADER)pFilebuff;
	if(*((PDWORD)((BYTE *)pFilebuff + pDosHeader->e_lfanew)) != IMAGE_NT_SIGNATURE){
		MessageBox(0,TEXT("无有效的PE标志!"),0,0);
		return;
	}

	//读取pFileBuffer 获取DOS头，PE头，节表等信息
	pDosHeader =(PIMAGE_DOS_HEADER)pFilebuff;
	pNTHeader = (PIMAGE_NT_HEADERS)((DWORD)pFilebuff + pDosHeader->e_lfanew);
	//打印NT头	
	pPEHeader = (PIMAGE_FILE_HEADER)(((DWORD)pNTHeader) + 4);  //加4个字节到了标准PE头
	pOptionHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPEHeader + IMAGE_SIZEOF_FILE_HEADER); //标准PE头+标准PE头的大小 20
	pSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)pOptionHeader + pPEHeader->SizeOfOptionalHeader);
	

	peDesc.HEADER = pPEHeader;
	peDesc.opheader = pOptionHeader;
	peDesc.section = pSectionHeader;
//	DbgPrintf("%d",peDesc.opheader->AddressOfEntryPoint);
//	free(pFilebuff);

}


BOOL CALLBACK PeDialogProc(									
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
			//MessageBox(NULL,TEXT("WM_INITDIALOG"),TEXT("INIT"),MB_OK);							
			ReadPE(FileName);
			char szBuffer[0x20];
			
			//入口点
			memset(szBuffer, 0, sizeof(szBuffer));
			sprintf(szBuffer, "%x", peDesc.opheader->AddressOfEntryPoint);	
			SendDlgItemMessage(hwndDlg,IDC_EDIT_OEP,WM_SETTEXT,0,(DWORD)szBuffer);
		

			//基址
			memset(szBuffer, 0, sizeof(szBuffer));
			sprintf(szBuffer, "%x", peDesc.opheader->ImageBase);	
			SendDlgItemMessage(hwndDlg,IDC_EDIT_IMAGEBASE,WM_SETTEXT,0,(DWORD)szBuffer);

			//区段数目
			memset(szBuffer, 0, sizeof(szBuffer));
			sprintf(szBuffer, "%x", peDesc.HEADER->NumberOfSections);	
			SendDlgItemMessage(hwndDlg,IDC_EDIT_NUM,WM_SETTEXT,0,(DWORD)szBuffer);

			//子系统
			memset(szBuffer, 0, sizeof(szBuffer));
			sprintf(szBuffer, "%x", peDesc.opheader->Subsystem);	
			SendDlgItemMessage(hwndDlg,IDC_EDIT_CHIODOS,WM_SETTEXT,0,(DWORD)szBuffer);
			
			//镜像大小、
			memset(szBuffer, 0, sizeof(szBuffer));
			sprintf(szBuffer, "%x", peDesc.opheader->SizeOfImage);	
			SendDlgItemMessage(hwndDlg,IDC_EDIT_SizeOfImage,WM_SETTEXT,0,(DWORD)szBuffer);
			
			//时间戳

			memset(szBuffer, 0, sizeof(szBuffer));
			sprintf(szBuffer, "%x", peDesc.HEADER->TimeDateStamp);	
			SendDlgItemMessage(hwndDlg,IDC_EDIT_TimeDateStamp,WM_SETTEXT,0,(DWORD)szBuffer);
			
			//代码基址
			
			memset(szBuffer, 0, sizeof(szBuffer));
			sprintf(szBuffer, "%x", peDesc.opheader->BaseOfCode);	
			SendDlgItemMessage(hwndDlg,IDC_EDIT_BaseOfCode,WM_SETTEXT,0,(DWORD)szBuffer);

			//pe头大小
			memset(szBuffer, 0, sizeof(szBuffer));
			sprintf(szBuffer, "%x", peDesc.opheader->SizeOfHeaders);	
			SendDlgItemMessage(hwndDlg,IDC_EDIT_SizeOfHeaders,WM_SETTEXT,0,(DWORD)szBuffer);

			//数据基址 IDC_EDIT_BaseOfData
			memset(szBuffer, 0, sizeof(szBuffer));
			sprintf(szBuffer, "%x", peDesc.opheader->BaseOfData);	
			SendDlgItemMessage(hwndDlg,IDC_EDIT_BaseOfData,WM_SETTEXT,0,(DWORD)szBuffer);

			//特征值
	
			//内存对齐
			memset(szBuffer, 0, sizeof(szBuffer));
			sprintf(szBuffer, "%x", peDesc.opheader->SectionAlignment);	
			SendDlgItemMessage(hwndDlg,IDC_EDIT_SectionAlignment,WM_SETTEXT,0,(DWORD)szBuffer);

			//校验和IDC_EDIT_CHECKSUM
			memset(szBuffer, 0, sizeof(szBuffer));
			sprintf(szBuffer, "%x", peDesc.opheader->CheckSum);	
			SendDlgItemMessage(hwndDlg,IDC_EDIT_CHECKSUM,WM_SETTEXT,0,(DWORD)szBuffer);

			//文件对齐
			memset(szBuffer, 0, sizeof(szBuffer));
			sprintf(szBuffer, "%x", peDesc.opheader->FileAlignment);
			SendDlgItemMessage(hwndDlg,IDC_EDIT_FileAlignment,WM_SETTEXT,0,(DWORD)szBuffer);

			//可选PE SizeOfOptionalHeader
			memset(szBuffer, 0, sizeof(szBuffer));
			sprintf(szBuffer, "%x", peDesc.HEADER->SizeOfOptionalHeader);
			SendDlgItemMessage(hwndDlg,IDC_EDIT_SizeOfOptionalHeader,WM_SETTEXT,0,(DWORD)szBuffer);

			//标志
			memset(szBuffer, 0, sizeof(szBuffer));
			sprintf(szBuffer, "%x", peDesc.opheader->Magic);
			SendDlgItemMessage(hwndDlg,IDC_EDIT_Magic,WM_SETTEXT,0,(DWORD)szBuffer);

			//目录项数

			memset(szBuffer, 0, sizeof(szBuffer));
			sprintf(szBuffer, "%x", peDesc.opheader->NumberOfRvaAndSizes);
			SendDlgItemMessage(hwndDlg,IDC_EDIT_NumberOfRvaAndSizes,WM_SETTEXT,0,(DWORD)szBuffer);


			
			return TRUE ;
		

		}
								
	case WM_NOTIFY:
		{	
		}
	case  WM_COMMAND :								
		
		switch (LOWORD (wParam))							
		{		
		case  IDC_BUTTON_PENumberOfSections:
			{
				SectionView(peHInstance,hwndDlg,peDesc.section,peDesc.HEADER->NumberOfSections);						
				return TRUE;
			}
	
		case   IDC_BUTTON_PECLOSE:												
			{
				EndDialog(hwndDlg, 0);						
				return TRUE;
			}
			
		}						
		break ;							
    }									
	
	return FALSE ;								
}									





void PeToolView(HINSTANCE hInstance ,HWND hDlg,TCHAR* szFileName)
{
	FileName = szFileName;
	peHInstance = hInstance;
	DialogBox(hInstance,MAKEINTRESOURCE(IDD_DIALOG_PE),hDlg, PeDialogProc);
}
