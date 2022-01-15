// processmonDlg.h : header file
//

#if !defined(AFX_PROCESSMONDLG_H__68F14918_7837_4FD4_AA39_0C9B864C9203__INCLUDED_)
#define AFX_PROCESSMONDLG_H__68F14918_7837_4FD4_AA39_0C9B864C9203__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CProcessmonDlg dialog

class CProcessmonDlg : public CDialog
{
// Construction
public:
	CProcessmonDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CProcessmonDlg)
	enum { IDD = IDD_PROCESSMON_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProcessmonDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CProcessmonDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnButton1Inject();
	afx_msg void OnButtonMessageboxopen();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROCESSMONDLG_H__68F14918_7837_4FD4_AA39_0C9B864C9203__INCLUDED_)
