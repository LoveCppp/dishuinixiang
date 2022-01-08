// PeTools.h: interface for the PeTools class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PETOOLS_H__C8AD7CD0_EA67_46D4_8CA7_9277B3D23DC4__INCLUDED_)
#define AFX_PETOOLS_H__C8AD7CD0_EA67_46D4_8CA7_9277B3D23DC4__INCLUDED_

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
DWORD PrintImportTable(LPVOID pImageBuffer);
#endif // !defined(AFX_PETOOLS_H__C8AD7CD0_EA67_46D4_8CA7_9277B3D23DC4__INCLUDED_)
