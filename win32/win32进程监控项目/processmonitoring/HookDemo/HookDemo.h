// HookDemo.h : main header file for the HOOKDEMO application
//

#if !defined(AFX_HOOKDEMO_H__62171ACC_9734_4A13_9D9B_69F076B42DC8__INCLUDED_)
#define AFX_HOOKDEMO_H__62171ACC_9734_4A13_9D9B_69F076B42DC8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CHookDemoApp:
// See HookDemo.cpp for the implementation of this class
//

class CHookDemoApp : public CWinApp
{
public:
	CHookDemoApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHookDemoApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CHookDemoApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HOOKDEMO_H__62171ACC_9734_4A13_9D9B_69F076B42DC8__INCLUDED_)
