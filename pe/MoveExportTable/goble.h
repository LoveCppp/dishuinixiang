// goble.h: interface for the goble class.
//
//////////////////////////////////////////////////////////////////////
#include "RESAPI.H"
#include "WINNT.H"
#if !defined(AFX_GOBLE_H__636E32BB_C3E0_4F81_A7DC_C571DEB1702D__INCLUDED_)
#define AFX_GOBLE_H__636E32BB_C3E0_4F81_A7DC_C571DEB1702D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//读取文件
DWORD ReadPeFile(IN LPSTR lpfzFile,OUT LPVOID* pFileBuffer);


//新增节
DWORD MoveExprotTable(LPVOID pFilebuff,LPVOID* pNewbuff);



//计算内纯转文件的函数
DWORD RvaToFileOffset(PVOID pFileBuffer, DWORD dwFoa);
DWORD FoaToImageOffset(PVOID pBuffer, DWORD dwFoa);
#endif // !defined(AFX_GOBLE_H__636E32BB_C3E0_4F81_A7DC_C571DEB1702D__INCLUDED_)
