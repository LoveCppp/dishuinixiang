// T00ls.h: interface for the T00ls class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_T00LS_H__7A1E4B42_35C4_4996_A260_A1C303E80BA9__INCLUDED_)
#define AFX_T00LS_H__7A1E4B42_35C4_4996_A260_A1C303E80BA9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

DWORD RvaToFileOffset(PVOID pBuffer, DWORD dwFoa);
DWORD FoaToImageOffset(PVOID pBuffer, DWORD dwFoa);
void XorEncryptAAA(char* p_data,DWORD EncryptSize);

#endif // !defined(AFX_T00LS_H__7A1E4B42_35C4_4996_A260_A1C303E80BA9__INCLUDED_)
