// processmonDlg.cpp : implementation file
//

#include "stdafx.h"
#include "processmon.h"
#include "processmonDlg.h"
#include "tools.h"
#include "PeTools.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


HANDLE g_hFileMap;
LPVOID lpHacker;


/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProcessmonDlg dialog

CProcessmonDlg::CProcessmonDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CProcessmonDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CProcessmonDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CProcessmonDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProcessmonDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CProcessmonDlg, CDialog)
	//{{AFX_MSG_MAP(CProcessmonDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, OnButton1Inject)
	ON_BN_CLICKED(IDC_BUTTON_MESSAGEBOXOPEN, OnButtonMessageboxopen)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProcessmonDlg message handlers

BOOL CProcessmonDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CProcessmonDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CProcessmonDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}


DWORD WINAPI MainThread(LPVOID lpParam)
{
  
	EnableDebugPrivilege();
	MemoryInject((DWORD)lpParam);
	return 0;
}


// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CProcessmonDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CProcessmonDlg::OnButton1Inject() 
{
	//创建共享内存
	g_hFileMap = CreateFileMapping(INVALID_HANDLE_VALUE,NULL,PAGE_READWRITE,0,0x1000,"Hacker1");
	if(g_hFileMap==NULL)
	{				
		MessageBox(NULL,"创建共享内存失败",0);
		return;
	}
	lpHacker = MapViewOfFile(g_hFileMap,FILE_MAP_ALL_ACCESS,0,0,0x1000);
	ZeroMemory(lpHacker,0x1000);
	

	//创建一个线程注入 不然会卡死

	HANDLE hThread1 = NULL;
	DWORD PID = GetDlgItemInt(IDC_EDIT_PID, NULL, 1);  //若看做有符号数，则bSigned为1，返回值直接以int类型去接收
	hThread1 = CreateThread(NULL,0,MainThread,(LPVOID)PID,0,NULL);
	// TODO: Add your control notification handler code here

	

}

void CProcessmonDlg::OnButtonMessageboxopen() 
{
	// TODO: Add your control notification handler code here
	PDWORD lpWriteFlag = (PDWORD)lpHacker;	
	lpWriteFlag[0] = 1;
	
	if (lpWriteFlag[1] == 0)
	{
		lpWriteFlag[1] = 1;
		SetDlgItemText(IDC_BUTTON_MESSAGEBOXOPEN,"关闭监控");
	}else{
		lpWriteFlag[1] = 0;
		SetDlgItemText(IDC_BUTTON_MESSAGEBOXOPEN,"开启监控");
	}


}

