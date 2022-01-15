// mydll.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"

 
#pragma data_seg("Shared")
char g_buffer[0x1000] = {0};
BOOL Flag=TRUE;
#pragma data_seg()
#pragma comment(linker,"/section:Shared,rws")
 
extern "C"  __declspec(dllexport) void SetData(char *buf, DWORD dwDataLen) //导出函数 设置数据
{
    ZeroMemory(g_buffer, 0x1000);
    memcpy(g_buffer, buf, dwDataLen);
}

extern "C"  __declspec(dllexport) void GetData(char *buf) //导出函数 获取数据
{    
    memcpy(buf, g_buffer, 0x1000);
}

BOOL APIENTRY DllMain( HMODULE hModule,
					  DWORD  ul_reason_for_call,
					  LPVOID lpReserved
					  )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
		{
				MessageBoxA(NULL, "飞鸽程序注入DLL成功", "", MB_OK);
		
		}
    case DLL_THREAD_ATTACH:
		{

			char szModule[MAX_PATH] = { 0 };
			GetModuleFileNameA(NULL, szModule, MAX_PATH);
			if (strstr(szModule, "ipmsg") != NULL)
			{            
				MessageBoxA(NULL, "飞鸽程序注入DLL成功", "", MB_OK);
				while (Flag)
				{
					if (strcmp(g_buffer, "quit") == 0) break; // 控制程序给的退出信号
					
					if (strlen(g_buffer) > 0)
					{
						MessageBoxA(NULL, g_buffer, szModule, MB_OK);
						Flag=FALSE;
					}
					
				}
			}
			break;

		}
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
 