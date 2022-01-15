// processmon.h : main header file for the PROCESSMON application
//

#if !defined(AFX_PROCESSMON_H__211209F7_B0FB_410A_A482_E9C3270FA8F1__INCLUDED_)
#define AFX_PROCESSMON_H__211209F7_B0FB_410A_A482_E9C3270FA8F1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CProcessmonApp:
// See processmon.cpp for the implementation of this class
//

class CProcessmonApp : public CWinApp
{
public:
	CProcessmonApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProcessmonApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CProcessmonApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROCESSMON_H__211209F7_B0FB_410A_A482_E9C3270FA8F1__INCLUDED_)
