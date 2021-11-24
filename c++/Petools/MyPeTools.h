// MyPeTools.h: interface for the MyPeTools class.
//
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
#include "RESAPI.H"
#include "WINNT.H"
#if !defined(AFX_GOBLE_H__636E32BB_C3E0_4F81_A7DC_C571DEB1702D__INCLUDED_)
#define AFX_GOBLE_H__636E32BB_C3E0_4F81_A7DC_C571DEB1702D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000




//¶ÁÈ¡ÎÄ¼þ
DWORD ReadPeFile(IN LPSTR lpfzFile,OUT LPVOID* pFileBuffer);
DWORD FoaToImageOffset(PVOID pBuffer, DWORD dwFoa);
DWORD RvaToFileOffset(PVOID pFileBuffer, DWORD dwFoa);
DWORD readPeResoure(LPVOID pFilebuff);
void GetResourceEntry(PIMAGE_RESOURCE_DIRECTORY resTable,DWORD depth);
void GetResourceName(DWORD depth,PIMAGE_RESOURCE_DIRECTORY_ENTRY ImgResDirEntry );
#endif // !defined(AFX_GOBLE_H__636E32BB_C3E0_4F81_A7DC_C571DEB1702D__INCLUDED_)