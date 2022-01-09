// InlineHOOK.h: interface for the InlineHOOK class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INLINEHOOK_H__B3019F62_85F5_45F9_9926_E7BE0152315F__INCLUDED_)
#define AFX_INLINEHOOK_H__B3019F62_85F5_45F9_9926_E7BE0152315F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

void SetInlineHook(DWORD HookAddr,DWORD HookProc,DWORD dwlength);
BOOL UnInlineHook();
extern "C"  void HOOK();

#endif // !defined(AFX_INLINEHOOK_H__B3019F62_85F5_45F9_9926_E7BE0152315F__INCLUDED_)
