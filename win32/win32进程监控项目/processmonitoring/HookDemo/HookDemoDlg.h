// HookDemoDlg.h : header file
//

#if !defined(AFX_HOOKDEMODLG_H__C2E202AC_60A2_44C2_B80B_2F39688A9280__INCLUDED_)
#define AFX_HOOKDEMODLG_H__C2E202AC_60A2_44C2_B80B_2F39688A9280__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CHookDemoDlg dialog

class CHookDemoDlg : public CDialog
{
// Construction
public:
	CHookDemoDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CHookDemoDlg)
	enum { IDD = IDD_HOOKDEMO_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHookDemoDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CHookDemoDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnButton1();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HOOKDEMODLG_H__C2E202AC_60A2_44C2_B80B_2F39688A9280__INCLUDED_)
