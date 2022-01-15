// tools.h: interface for the tools class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TOOLS_H__9D7447AB_7593_448F_B92D_F6399179BAE5__INCLUDED_)
#define AFX_TOOLS_H__9D7447AB_7593_448F_B92D_F6399179BAE5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



BOOL EnableDebugPrivilege();

BOOL MemoryInject(DWORD Pid);
DWORD WINAPI InjectEntry(LPVOID lpParam);
#endif // !defined(AFX_TOOLS_H__9D7447AB_7593_448F_B92D_F6399179BAE5__INCLUDED_)
