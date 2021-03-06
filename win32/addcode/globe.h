// globe.h: interface for the globe class.
//
//////////////////////////////////////////////////////////////////////



#if !defined(AFX_GLOBE_H__A96CA7AD_596B_4CD8_B891_9D731DF7ADD5__INCLUDED_)
#define AFX_GLOBE_H__A96CA7AD_596B_4CD8_B891_9D731DF7ADD5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include <windows.h>

extern BYTE shellcode[];


DWORD ReadPeFile(IN LPSTR lpfzFile,OUT LPVOID* pFileBuffer);


DWORD CopyFileBufferToImageBuffer(IN LPVOID pFileBuffer,OUT LPVOID* pImageBuffer);


DWORD CopyImageBufferToNewFileBuffer(IN LPVOID pImageBuffer, OUT LPVOID* pNewFileBuffer);

DWORD Add_SectionCode(IN LPVOID pImageBuffer);


DWORD Add_SectionInNewSec(IN LPVOID pImageBuffer,OUT LPVOID* pNewImageBuffer);


DWORD Add_SectionInNewSecExt(IN LPVOID pImageBuffer,OUT LPVOID* pNewImageBuffer);


BOOL NewFileBufferToFile(IN LPVOID pNewFileBuffer, size_t size,OUT LPSTR lpszFile);

DWORD Merge_Sec(IN LPVOID pImageBuffer,OUT LPVOID* pNewImageBuffer);

DWORD Alignment(DWORD alignment_value, DWORD addend, DWORD address);
int MergeLastSection( LPVOID pImageBuffer );
DWORD PrintDriectory(LPVOID pImageBuffer);
DWORD PrintExport(LPVOID pFileBuffer);
DWORD PrintRelocation(LPVOID pFileBuffer);

DWORD PrintImportTable(LPVOID pFileBuffer);
DWORD printBoundImportTable(LPVOID pFileBuffer);

DWORD RvaToFileOffset(PVOID pFileBuffer, DWORD dwFoa);
DWORD FoaToImageOffset(PVOID pBuffer, DWORD dwFoa);
DWORD GetFunctionAddrByName(PVOID pFileBuffer,char* FuncName);
DWORD GetFunctionAddrByOrdinals(LPVOID pFileBuffer,DWORD FunctionOrdinals);
DWORD MoveRelocation(LPVOID pFilebuff);
class globe  
{
public:
	globe();
	virtual ~globe();

};

#endif // !defined(AFX_GLOBE_H__A96CA7AD_596B_4CD8_B891_9D731DF7ADD5__INCLUDED_)
