// PeFileOp.h: interface for the PeFileOp class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PEFILEOP_H__C85CABCC_9241_45B7_A94C_DD70C2DF9AD3__INCLUDED_)
#define AFX_PEFILEOP_H__C85CABCC_9241_45B7_A94C_DD70C2DF9AD3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//╤ах║нд╪Ч
DWORD ReadPeFile(IN LPSTR lpfzFile,OUT LPVOID* pFileBuffer);
BOOL SAddNewSection(PVOID pFileBuffer,PVOID SrcFileBuffer,DWORD SrcBuffSize);
#endif // !defined(AFX_PEFILEOP_H__C85CABCC_9241_45B7_A94C_DD70C2DF9AD3__INCLUDED_)
