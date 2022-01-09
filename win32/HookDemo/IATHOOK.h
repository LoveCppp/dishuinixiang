// IATHOOK.h: interface for the IATHOOK class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IATHOOK_H__AA65BEC0_3097_44EB_A0C6_FC4EC9E90806__INCLUDED_)
#define AFX_IATHOOK_H__AA65BEC0_3097_44EB_A0C6_FC4EC9E90806__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

BOOL SetIATHOOK(DWORD pOldFuncAddr,DWORD pNewFuncAddr);
int WINAPI MyMessageBox(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType);
bool UnSetIATHook();
#endif // !defined(AFX_IATHOOK_H__AA65BEC0_3097_44EB_A0C6_FC4EC9E90806__INCLUDED_)
