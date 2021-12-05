// eventTread.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <windows.h>

//事件和临界区		
HANDLE g_hSet, g_hClear;
int g_Max = 10;
int g_Number = 0;

//生产者线程函数  		
DWORD WINAPI ThreadProduct(LPVOID pM)
{
    for (int i = 0; i < g_Max; i++)
    {
        WaitForSingleObject(g_hSet, INFINITE);
        g_Number = 1;
        DWORD id = GetCurrentThreadId();
        printf("生产者%d将数据%d放入缓冲区\n", id, g_Number);
        SetEvent(g_hClear); //生产数据之后改变消费者通知状态
    }
    return 0;
}
//消费者线程函数		
DWORD WINAPI ThreadConsumer(LPVOID pM)
{
    for (int i = 0; i < g_Max; i++)
    {
        WaitForSingleObject(g_hClear, INFINITE);
        g_Number = 0;
        DWORD id = GetCurrentThreadId();
        printf("----消费者%d将数据%d放入缓冲区\n", id, g_Number);
        SetEvent(g_hSet);  //消费完成之后改变生产者状态
    }
    return 0;
}

int main(int argc, char* argv[])
{

    HANDLE hThread[2];

    g_hSet = CreateEvent(NULL, FALSE, TRUE, NULL);   //线程创建生产者就可以收到通知
    g_hClear = CreateEvent(NULL, FALSE, FALSE, NULL);  //线程创建消费者等待 

    hThread[0] = ::CreateThread(NULL, 0, ThreadProduct, NULL, 0, NULL); 
    hThread[1] = ::CreateThread(NULL, 0, ThreadConsumer, NULL, 0, NULL);

    WaitForMultipleObjects(2, hThread, TRUE, INFINITE);
    CloseHandle(hThread[0]);
    CloseHandle(hThread[1]);

    //销毁 	
    CloseHandle(g_hSet);
    CloseHandle(g_hClear);
    getchar();
    return 0;
}
