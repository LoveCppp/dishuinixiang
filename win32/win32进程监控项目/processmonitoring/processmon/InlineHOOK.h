// InlineHOOK.h: interface for the InlineHOOK class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INLINEHOOK_H__15A22A71_ED0A_421B_9EC4_4F2339C6EFA4__INCLUDED_)
#define AFX_INLINEHOOK_H__15A22A71_ED0A_421B_9EC4_4F2339C6EFA4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

void SetMESSAGEBOXInlineHook(DWORD HookAddr,DWORD HookProc,DWORD dwlength,DWORD* WriteHOOKAddr);
BOOL UnInlineHook(DWORD nweMessageBoxAddr,DWORD length);
extern "C"  void MessageBoxHOOK();


#endif // !defined(AFX_INLINEHOOK_H__15A22A71_ED0A_421B_9EC4_4F2339C6EFA4__INCLUDED_)
