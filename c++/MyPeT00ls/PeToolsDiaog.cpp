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
	//���ļ�
	pFile = fopen(FileName, "rb");
	if (!pFile)
	{
		MessageBox(0,TEXT(" �޷��� EXE �ļ�!ʧ��!!"),0,0);
		
		return;
	}
	//��ȡ�ļ���С
	
	fseek(pFile, 0, SEEK_END);
	fileSize = ftell(pFile);
	fseek(pFile, 0, SEEK_SET);
	//���仺����	
	pFilebuff = malloc(fileSize);
	
	
	if (!pFilebuff)
	{
	
		MessageBox(0,TEXT(" ����ռ�ʧ��!!"),0,0);
		fclose(pFile); //Ȼ����Ҫ�ر��ļ�
		return;
	}
	
	size_t n = fread(pFilebuff, fileSize, 1, pFile);
	if (!n)
	{
	
		MessageBox(0,TEXT(" ��ȡ����ʧ��!!"),0,0);
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
	
		MessageBox(0,TEXT("��ȡ���ڴ��pfilebuffer��Ч!"),0,0);
		return;
	}
	//�ж��ǲ���exe�ļ�
	if(*((PWORD)pFilebuff) != IMAGE_DOS_SIGNATURE)
	{
	
		MessageBox(0,TEXT("����MZ��־������exe�ļ�!"),0,0);
		return;
	}
	pDosHeader = (PIMAGE_DOS_HEADER)pFilebuff;
	if(*((PDWORD)((BYTE *)pFilebuff + pDosHeader->e_lfanew)) != IMAGE_NT_SIGNATURE){
		MessageBox(0,TEXT("����Ч��PE��־!"),0,0);
		return;
	}

	//��ȡpFileBuffer ��ȡDOSͷ��PEͷ���ڱ����Ϣ
	pDosHeader =(PIMAGE_DOS_HEADER)pFilebuff;
	pNTHeader = (PIMAGE_NT_HEADERS)((DWORD)pFilebuff + pDosHeader->e_lfanew);
	//��ӡNTͷ	
	pPEHeader = (PIMAGE_FILE_HEADER)(((DWORD)pNTHeader) + 4);  //��4���ֽڵ��˱�׼PEͷ
	pOptionHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPEHeader + IMAGE_SIZEOF_FILE_HEADER); //��׼PEͷ+��׼PEͷ�Ĵ�С 20
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
			
			//��ڵ�
			memset(szBuffer, 0, sizeof(szBuffer));
			sprintf(szBuffer, "%x", peDesc.opheader->AddressOfEntryPoint);	
			SendDlgItemMessage(hwndDlg,IDC_EDIT_OEP,WM_SETTEXT,0,(DWORD)szBuffer);
		

			//��ַ
			memset(szBuffer, 0, sizeof(szBuffer));
			sprintf(szBuffer, "%x", peDesc.opheader->ImageBase);	
			SendDlgItemMessage(hwndDlg,IDC_EDIT_IMAGEBASE,WM_SETTEXT,0,(DWORD)szBuffer);

			//������Ŀ
			memset(szBuffer, 0, sizeof(szBuffer));
			sprintf(szBuffer, "%x", peDesc.HEADER->NumberOfSections);	
			SendDlgItemMessage(hwndDlg,IDC_EDIT_NUM,WM_SETTEXT,0,(DWORD)szBuffer);

			//��ϵͳ
			memset(szBuffer, 0, sizeof(szBuffer));
			sprintf(szBuffer, "%x", peDesc.opheader->Subsystem);	
			SendDlgItemMessage(hwndDlg,IDC_EDIT_CHIODOS,WM_SETTEXT,0,(DWORD)szBuffer);
			
			//�����С��
			memset(szBuffer, 0, sizeof(szBuffer));
			sprintf(szBuffer, "%x", peDesc.opheader->SizeOfImage);	
			SendDlgItemMessage(hwndDlg,IDC_EDIT_SizeOfImage,WM_SETTEXT,0,(DWORD)szBuffer);
			
			//ʱ���

			memset(szBuffer, 0, sizeof(szBuffer));
			sprintf(szBuffer, "%x", peDesc.HEADER->TimeDateStamp);	
			SendDlgItemMessage(hwndDlg,IDC_EDIT_TimeDateStamp,WM_SETTEXT,0,(DWORD)szBuffer);
			
			//�����ַ
			
			memset(szBuffer, 0, sizeof(szBuffer));
			sprintf(szBuffer, "%x", peDesc.opheader->BaseOfCode);	
			SendDlgItemMessage(hwndDlg,IDC_EDIT_BaseOfCode,WM_SETTEXT,0,(DWORD)szBuffer);

			//peͷ��С
			memset(szBuffer, 0, sizeof(szBuffer));
			sprintf(szBuffer, "%x", peDesc.opheader->SizeOfHeaders);	
			SendDlgItemMessage(hwndDlg,IDC_EDIT_SizeOfHeaders,WM_SETTEXT,0,(DWORD)szBuffer);

			//���ݻ�ַ IDC_EDIT_BaseOfData
			memset(szBuffer, 0, sizeof(szBuffer));
			sprintf(szBuffer, "%x", peDesc.opheader->BaseOfData);	
			SendDlgItemMessage(hwndDlg,IDC_EDIT_BaseOfData,WM_SETTEXT,0,(DWORD)szBuffer);

			//����ֵ
	
			//�ڴ����
			memset(szBuffer, 0, sizeof(szBuffer));
			sprintf(szBuffer, "%x", peDesc.opheader->SectionAlignment);	
			SendDlgItemMessage(hwndDlg,IDC_EDIT_SectionAlignment,WM_SETTEXT,0,(DWORD)szBuffer);

			//У���IDC_EDIT_CHECKSUM
			memset(szBuffer, 0, sizeof(szBuffer));
			sprintf(szBuffer, "%x", peDesc.opheader->CheckSum);	
			SendDlgItemMessage(hwndDlg,IDC_EDIT_CHECKSUM,WM_SETTEXT,0,(DWORD)szBuffer);

			//�ļ�����
			memset(szBuffer, 0, sizeof(szBuffer));
			sprintf(szBuffer, "%x", peDesc.opheader->FileAlignment);
			SendDlgItemMessage(hwndDlg,IDC_EDIT_FileAlignment,WM_SETTEXT,0,(DWORD)szBuffer);

			//��ѡPE SizeOfOptionalHeader
			memset(szBuffer, 0, sizeof(szBuffer));
			sprintf(szBuffer, "%x", peDesc.HEADER->SizeOfOptionalHeader);
			SendDlgItemMessage(hwndDlg,IDC_EDIT_SizeOfOptionalHeader,WM_SETTEXT,0,(DWORD)szBuffer);

			//��־
			memset(szBuffer, 0, sizeof(szBuffer));
			sprintf(szBuffer, "%x", peDesc.opheader->Magic);
			SendDlgItemMessage(hwndDlg,IDC_EDIT_Magic,WM_SETTEXT,0,(DWORD)szBuffer);

			//Ŀ¼����

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
