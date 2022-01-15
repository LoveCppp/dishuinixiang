// PeTools.h: interface for the PeTools class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PETOOLS_H__77F115D7_72B6_4792_A6B1_0C322A5F59C9__INCLUDED_)
#define AFX_PETOOLS_H__77F115D7_72B6_4792_A6B1_0C322A5F59C9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

DWORD ReadPeFile(IN LPSTR lpfzFile,OUT LPVOID* pFileBuffer);
DWORD GetSection(IN LPVOID pFileBuffer,OUT LPVOID* EncyBuffer);
void XorDecodeAAA(char* p_data,DWORD DecodeSize);
DWORD CopyFileBufferToImageBuffer(IN LPVOID pFileBuffer,OUT LPVOID* pImageBuffer);
DWORD GetImageBase(LPVOID pImageBuffer, DWORD* srcImageBase,DWORD* srcSizeOfImage,DWORD* srcOEP);
DWORD isRelocation(LPVOID pFileBuffer);
DWORD GetRelocatio(LPVOID pFileBuffer);
void ChangesImageBase(PVOID pFileBuffer, DWORD TempImageBase);
DWORD RvaToFileOffset(PVOID pFileBuffer, DWORD dwFoa);
DWORD FoaToImageOffset(PVOID pBuffer, DWORD dwFoa);
void RestoreRelocation(IN LPVOID pImageBuffer, IN DWORD newImageBase);

void RepairIAT(LPVOID pImageBuffer);

#endif // !defined(AFX_PETOOLS_H__77F115D7_72B6_4792_A6B1_0C322A5F59C9__INCLUDED_)
