// global.h: interface for the global class.
//
//////////////////////////////////////////////////////////////////////
#include "RESAPI.H"
#include "WINNT.H"
#if !defined(AFX_GLOBAL_H__9C189CD5_5D09_465A_8535_32C0DD87D537__INCLUDED_)
#define AFX_GLOBAL_H__9C189CD5_5D09_465A_8535_32C0DD87D537__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//��ȡ�ļ�
DWORD ReadPeFile(IN LPSTR lpfzFile,OUT LPVOID* pFileBuffer);

DWORD InjectDll(LPVOID pFilebuff);


//�����ڴ�ƫ��ת�ļ��ĺ���
DWORD RvaToFileOffset(PVOID pFileBuffer, DWORD dwFoa);
DWORD FoaToImageOffset(PVOID pBuffer, DWORD dwFoa);


#endif // !defined(AFX_GLOBAL_H__9C189CD5_5D09_465A_8535_32C0DD87D537__INCLUDED_)
